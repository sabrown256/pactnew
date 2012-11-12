#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include cpyright.h
#

from distutils.sysconfig import *
from re import search, sub
import sys 

# version number

"""
Print Python's version number as major.minor
ignoring any patch version number
"""
ver = sys.version_info
print("Version: %d.%d" % (ver[0], ver[1]))

# include

"""
Print path to Python's include directory
"""
print("IncDir: %s" % get_python_inc())

# lib

"""
Print path to Python's lib directory
"""
print("LibDir: %s" % get_python_lib())

# config

"""
Print path to Python's config directory
"""
import os.path as path
dir = get_python_lib()
print("CfgDir: %s" % path.join(path.split(dir)[0], 'config'))

# compiler

compiler = None

fd = open(get_makefile_filename(), 'r')

line = fd.readline()

while (line):
  if search('^CC\s*=', line):
      line = sub('^CC\s*=',  '', line)   # Remove macro name
      line = sub('#.*',      '', line)   # Remove comments
      line = sub('^\s+',     '', line)   # Remove leading white-space
      line = sub('\s+$',     '', line)   # Remove trailing white-space
      line = sub('\s+',     ' ', line)   # Remove multiple white-space
      line = sub('\"',       '', line)   # Remove quotes

      if line == '':
        continue                         # Anything left?
      else:
        compiler = line

  line = fd.readline()

if compiler != None:
  print("Compiler: %s" % compiler)
else:
  print("Compiler: unknown")

