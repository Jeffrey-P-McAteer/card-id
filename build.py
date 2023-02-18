
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
  if not e('build'):
    os.makedirs('build')

  if not e('build', 'libpcsc-cpp'):
    cmd('git', 'clone', '--recurse-submodules', 'https://github.com/web-eid/libpcsc-cpp.git', j('build', 'libpcsc-cpp'))

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
  if not e('build', 'libpcsc-cpp', 'build', 'libpcsc-cpp.a'):
    build_libpcsc(compiler, j('build', 'libpcsc-cpp'))

  # Then build card-id
  cmd(compiler,
    '-o', j('build', 'card-id'),
    '-Wall', '-Werror',
    '-I', j('build', 'libpcsc-cpp', 'include'),
    '-L', j('build', 'libpcsc-cpp', 'build'),
    '-lpcsc-cpp',
    j('src', 'main.cpp'),
    '-lpcsc-cpp',
  )

  cmd(j('build', 'card-id'))


if __name__ == '__main__':
  main()

