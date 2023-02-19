
import os
import sys
import subprocess
import shutil
import random

def cmd(*parts):
  print('> {}'.format(' '.join(list(parts))))
  subprocess.run(list(parts), check=True)

def j(*parts):
  return os.path.join(*parts)

def e(*parts):
  return os.path.exists(j(*parts))

def build_libpcsc(compiler, lib_pcsc_dir):
  cmd('sh',
    '-c',
    '''
cd {lib_pcsc_dir}
./bootstrap && ./configure --enable-debugatr --enable-scf --enable-static && make
'''.format(
  lib_pcsc_dir=lib_pcsc_dir
).strip()
  )


def main(args=sys.argv):
  if 'clean' in args:
    if e('build'):
      shutil.rmtree('build')

  if not e('build'):
    os.makedirs('build')

  # Download dependencies
  if not e('build', 'PCSC'):
    cmd('git', 'clone', '--recurse-submodules', '--depth', '1', 'https://salsa.debian.org/rousseau/PCSC.git', j('build', 'PCSC'))

  supported_compilers = [
    'g++', 'clang++'
  ]
  existing_compilers = []
  for c in supported_compilers:
    if shutil.which(c):
      existing_compilers.append( shutil.which(c) )
  random.shuffle(existing_compilers)

  compiler = existing_compilers[0] if len(existing_compilers) > 0 else None
  if compiler is None:
    raise Exception('Cannot find any supported compilers on this system! supported copilers = {}'.format(supported_compilers))

  # First build dependencies (we don't allow CMAKE in this house)
  if not e('build', 'PCSC', 'build', 'libpcsc-cpp.a'):
    build_libpcsc(compiler, j('build', 'PCSC'))

  # Then build card-id
  cmd(compiler,
    '-o', j('build', 'card-id'),
    '-Wall', '-Werror',
    '-I', j('build', 'PCSC', 'src'),
    '-L', j('build', 'PCSC', 'src', '.libs'),
    '-lpcsclite',
    j('src', 'main.cpp'),
    '-lpcsc-cpp',
  )

  cmd(j('build', 'card-id'))


if __name__ == '__main__':
  main()

