#!/usr/bin/env python
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

import pact_2.pdb as pdb
import os
import sys

# What should be tested:
#  - scalars and arrays (1D, 2D, at least)
#  - mkdir, cd, cwd, ls
#
# Scalars:    int, float, string, struct, nested struct 
# 1D Arrays:  int, float, STRING, struct, nested struct
# 2D Arrays:  INT, FLOAT, STRING, STRUCT, NESTED STRUCT
#
# TODO: in CAPS

# DEBUG
debug = 1

# OPEN
fp = pdb.open('foobar.pdb', 'w')

# MKDIR
fp.mkdir('/mesh')  # FIXME: mkdir -p as default?
fp.mkdir('/mesh/face')

# Scalar tests:  int, float, string, dict, struct, nested struct
outputAnswer = 42
outputPi     = 3.1415926
outputString = 'This is a test'

fp.write('answer', outputAnswer)
fp.write('pi',     outputPi)
fp.write('string', outputString)
 
# CD 
fp.cd('mesh')
 
# Array tests: int, float, STRING, DICT, struct, nested struct
outputPowers = [2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096]
outputPow    = [2.0, 4.0, 8.0, 16.0, 32.0, 64.0, 128.0, 256.0, 512.0, 1024.0, 2048.0, 4096.0]

fp.write('powers', outputPowers)
fp.write('pows', outputPow)  

fp.cd('..')

# These also test relative and absolute names used for reads 
inputAnswer  = fp.read('answer')
inputPi      = fp.read('/pi')
inputString  = fp.read('string')
inputPowers  = fp.read('mesh/powers')
inputPow     = fp.read('mesh/pows')

# INT
if inputAnswer != outputAnswer:
  print 'Integer test failed'
  print inputAnswer, '!=', outputAnswer
  fp.close()
  sys.exit(1)

if debug:
  print str(inputAnswer)

# FLOAT
if inputPi != outputPi:
  print 'Floating pt test failed'
  print inputPi, '!=', outputPi
  fp.close()
  sys.exit(1)

if debug:
  print str(inputPi)

# STRING
if inputString != outputString:
  print 'String test failed'
  print inputString, '!=', outputString
  fp.close()
  sys.exit(1)

if debug:
  print str(inputString)

if inputPowers != outputPowers:
  print 'Integer array test failed'
  print inputPowers
  fp.close()
  sys.exit(1)

if debug:
  print str(inputPowers)

if inputPow != outputPow:
  print 'Floating pt array test failed'
  print inputPow
  fp.close()
  sys.exit(1)

if debug:
  print str(inputPow)
 
# Go there
fp.cd('/mesh/face')

# Create some data structure types

dp = fp.defstr('arrayStruct', ('char *tag', 
                               'int time', 
                               'int coords[3]',
                               'int twoD[2][2]'))
 
outputArrayStruct = [0]

# for i in xrange(2):
#   myFace = dp(('triangle' + str(i), i, [i+0, i+1, i+2], [i, i+1, i+2, i+3]))
#   outputArrayStruct.append(myFace)
 
fp.write('outputArrayStruct', outputArrayStruct)
 
# # Read that array of structs back
# inputArrayStruct = fp.read('outputArrayStruct')
# 
# for i in xrange(len(inputArrayStruct)):
#   if inputArrayStruct[i].tag != outputArrayStruct[i].tag or \
#      inputArrayStruct[i].time != outputArrayStruct[i].time or \
#      inputArrayStruct[i].coords != outputArrayStruct[i].coords or \
#      inputArrayStruct[i].twoD != outputArrayStruct[i].twoD:
#     print 'Array of structs test failed'
#     print str(inputArrayStruct[i])
#     print str(outputArrayStruct[i])
#     fp.close()
#     sys.exit(1)
# 
#   if debug:
#     print str(inputArrayStruct[i])
# 
fp.cd('../..')

if fp.ls() != ('answer', 'mesh/', 'pi', 'string'):
  print 'ls / test failed'
  print str(fp.ls())
  fp.close()
  sys.exit(1)

if debug:
  print str(fp.ls())

fp.cd('mesh')

if fp.ls() != ('face/', 'powers', 'pows'):
  print 'ls /mesh test failed'
  print str(fp.ls())
  fp.close()
  sys.exit(1)

if debug:
  print str(fp.ls())

fp.cd('face')

if fp.ls() != ('outputArrayStruct',):
  print 'ls /mesh/face test failed'
  print str(fp.ls())
  fp.close()
  sys.exit(1)

if debug:
  print str(fp.ls())

fp.close()

print 'tests passed'
 
if not(debug):
  os.system('rm foobar.pdb')

