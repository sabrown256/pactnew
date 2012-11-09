#!/usr/bin/env python2.2
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

from distutils.sysconfig import *
from re import search, sub
import sys 

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
  print(compiler)
else:
  print('')
  sys.exit(1)
