
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

def build_libpcsc_lite(compiler, lib_pcsc_dir):
  cmd('sh',
    '-c',
    '''
cd {lib_pcsc_dir}
./bootstrap && ./configure --enable-debugatr --enable-scf --enable-static && make
'''.format(
  lib_pcsc_dir=lib_pcsc_dir
).strip()
  )

def build_libpcsc(compiler, libpcsc_cpp_dir):
  cmd('sh',
    '-c',
    '''
cd {libpcsc_cpp_dir}
cmake -DCMAKE_BUILD_TYPE=release -B build -S .
cmake --build build --config release
'''.format(
  libpcsc_cpp_dir=libpcsc_cpp_dir
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

  if not e('build', 'libpcsc-cpp'):
    cmd('git', 'clone', '--recurse-submodules', '--depth', '1', 'https://github.com/web-eid/libpcsc-cpp.git', j('build', 'libpcsc-cpp'))

  if not e('build', 'scsh3.17.584-noinstall.zip'):
    cmd('wget', '-O', j('build', 'scsh3.17.584-noinstall.zip'), 'https://www.openscdp.org/download/scsh3/scsh3.17.584-noinstall.zip')

  if not e('build', 'scsh3.17.584-noinstall'):
    cmd('unzip', j('build', 'scsh3.17.584-noinstall.zip'), '-d', j('build', 'scsh3.17.584-noinstall'))


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
  if not e('build', 'PCSC', 'src', '.libs', 'libpcsclite.a'):
    build_libpcsc_lite(compiler, j('build', 'PCSC'))
  if not e('build', 'libpcsc-cpp', 'build', 'libpcsc-cpp.a'):
    build_libpcsc(compiler, j('build', 'libpcsc-cpp'))

  # Then build card-id
  cmd(compiler,
    '-o', j('build', 'card-id'),
    
    '-Wall', '-Werror', '-std=c++20', '-static',

    # Turn off when building for small releases
    '-g',
    
    '-I', j('build', 'PCSC', 'src'),
    '-L', j('build', 'PCSC', 'src', '.libs'),
    
    '-I', j('build', 'libpcsc-cpp', 'include'),
    '-L', j('build', 'libpcsc-cpp', 'build'),

    '-lpcsc-cpp',
    '-lpcsclite',
    
    j('src', 'main.cpp'),
    
    '-lpcsc-cpp',
    '-lpcsclite',
  )

  try:
    if shutil.which('gdbbin'):
      cmd('gdbbin', j('build', 'card-id'))
    else:
      cmd(j('build', 'card-id'))
  except:
    pass


if __name__ == '__main__':
  main()

