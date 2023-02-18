
import os
import sys
import subprocess

def cmd(*parts):
  print('> {}'.format(' '.join(list(parts))))
  subprocess.run(list(parts))

def j(*parts):
  return os.path.join(*parts)

def e(*parts):
  return os.path.exists(j(*parts))

def main(args=sys.argv):
  if not e('build'):
    os.makedirs('build')

  if not e('build', 'libpcsc-cpp'):
    cmd('git', 'clone', 'https://github.com/web-eid/libpcsc-cpp.git', j('build', 'libpcsc-cpp'))

  cmd('g++',
    '-o', j('build', 'card-id'),
    '-I', j('build', 'libpcsc-cpp', 'include'),
    j('src', 'main.cpp')
  )

  cmd(j('build', 'card-id'))


if __name__ == '__main__':
  main()

