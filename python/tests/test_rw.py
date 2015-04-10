#!/usr/bin/env python
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

import pact_test.pdb as pdb
from copy import deepcopy
import unittest
import os

# Test Cases already implemented 
#
# Scalars:    int, float, string, dict, struct, nested struct 
# 1D Arrays:  int, float, string, dict, struct, nested struct
# 2D Arrays:  int, float, string, dict, struct, nested struct
# Hier. Org.: mkdir, cd, cwd, ls
# Miscellaneous: read->mutate->overwrite
#
# These simply test the interface of HDB to make sure the results
# you receive back are exactly what you would expect them to be.
# A few combinations are tried so these aren't all "unit" tests.

debug = 0

class PPUseCases(unittest.TestCase):
  # Fixtures for initializing and terminating test cases

  def setUp(self):
    self.fp = pdb.open('test_rw.pdb', 'w')

  def tearDown(self):
    self.fp.flush()
    self.fp.close()
    os.system('rm -f ' + os.getcwd() + os.sep + 'test_rw.pdb')

    if debug: 
      os.system('sleep 3') 
      print('')

  # Miscellaneous tests (found by experimentation)

  def testEMutate(self):
    outputPow = [2.0, 4.0, 8.0, 16.0, 32.0, 64.0, 128.0, 256.0, 512.0, 1024.0, 2048.0, 4096.0]
    outputPowMut = [2.0, 4.0, 8.0, 16.0, 42.0, 64.0, 128.0, 256.0, 512.0, 1024.0, 2048.0, 4096.0]
      
    self.fp.write('pow', outputPow)
    self.fp.flush()
    inputPow = self.fp.read('pow')
    inputPow[4] = 42.0
    self.fp.write('pow', inputPow)
    inputPow = self.fp.read('pow')
    self.assertEqual(inputPow, outputPowMut)
 
  # Scalar tests:  int, float, string, dict, struct, nested struct

  def testFScalarInt(self): 
    outputAnswer = 42

    self.fp.write('answer', outputAnswer)
    inputAnswer = self.fp.read('answer')
    self.assertEqual(outputAnswer, inputAnswer)

  def testGScalarFloat(self): 
    outputPi     = 3.1415926

    self.fp.write('pi',     outputPi)
    inputPi = self.fp.read('pi')
    self.assertEqual(outputPi, inputPi)

  def testHScalarString(self): 
    outputString = 'This is a test'

    self.fp.write('string', outputString)
    inputString = self.fp.read('string')
    self.assertEqual(outputString, inputString)

  def testIScalarStruct(self): 
    dp = pdb.defstr('thermo', ('float T', 'float p', 'float N'), self.fp)
    outputThermo = dp((1.0, 2.0, 3.0)) 

    self.fp.write('struct', outputThermo)
    inputThermo = self.fp.read('struct')
    inputThermo = dp(inputThermo)

    self.assertEqual(outputThermo.T, inputThermo.T)
    self.assertEqual(outputThermo.p, inputThermo.p)
    self.assertEqual(outputThermo.N, inputThermo.N)
 
  def testJScalarNestedStruct(self):
    dp = pdb.defstr('thermo', ('float T', 'float p', 'float N'), self.fp)
    outputThermo = dp((1.0, 2.0, 3.0)) 

    dp = pdb.defstr('nested', ('int t', 'int c[3]', 'int D[2][2]', 'thermo s'), self.fp)
    outputNestedStruct = dp((0, [0, 1, 2], [0, 1, 2, 3], (1.0, 2.0, 3.0))) #outputThermo))
     
    self.fp.write('nestedStruct', outputNestedStruct)
    inputNestedStruct = self.fp.read('nestedStruct')
    inputNestedStruct = dp(inputNestedStruct)
   
    # FIXME 
    #self.assertEqual(outputNestedStruct, inputNestedStruct)
    
  # 1D Array tests: int, float, string, dict, struct, nested struct

  def testKOneDimArrayInt(self):
    outputPowers = [2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096]

    self.fp.write('powers', outputPowers)
    inputPowers = self.fp.read('powers')
    self.assertEqual(outputPowers, inputPowers)

  def testLOneDimArrayFloat(self):
    outputPow = [2.0, 4.0, 8.0, 16.0, 32.0, 64.0, 128.0, 256.0, 512.0, 1024.0, 2048.0, 4096.0]
      
    self.fp.write('pow', outputPow)
    inputPow = self.fp.read('pow')
    self.assertEqual(outputPow, inputPow)
      
  def xtestMOneDimArrayString(self):
    outputString = ['This', 'test', 'does', 'suck']
    
    # FIXME 
    self.fp.write('arrayString', outputString)
    inputString = self.fp.read('arrayString')
    self.assertEqual(outputString, inputString)

  def testNOneDimArrayStruct(self):
    outputThermoArray = []

    dp = pdb.defstr('thermoArray', ('float T', 'float p', 'float N'), self.fp)

    for i in xrange(10):
      outputThermo = dp((1.0, 2.0, 3.0)) 
      outputThermoArray.append(outputThermo)
      
    # FIXME 
    #self.fp.write('oneDimArrayStruct', outputThermoArray)
    #inputThermoArray = self.fp.read('oneDimArrayStruct')

    #for i in xrange(10): 
    #  self.assertEqual(outputThermoArray[i].T, inputThermoArray[i].T)
    #  self.assertEqual(outputThermoArray[i].p, inputThermoArray[i].p)
    #  self.assertEqual(outputThermoArray[i].N, inputThermoArray[i].N)

  def xtestOOneDimArrayNestedStruct(self):
    # FIXME 
    outputNestedStructArray = []

    for i in xrange(10):
      outputNestedStruct = {}
      outputNestedStruct['tag'] = 'triangle' + str(i)
      outputNestedStruct['time'] = i
      outputNestedStruct['xcoords'] = [i, i+1, i+2] 
      outputNestedStruct['ycoords'] = [i+3, i+4, i+5] 
      outputNestedStruct['zcoords'] = [i+6, i+7, i+8] 
      outputNestedStruct['twoD'] = [[i+9, i+10], [i+11, i+12]] 
      outputNestedStruct['state'] = {}
      outputNestedStruct['state']['T'] = 1.11222333444 + i
      outputNestedStruct['state']['p'] = 5.55666777888 + i
      outputNestedStruct['state']['N'] = 1.2345678910 + i

      outputNestedStructArray.append(outputNestedStruct)

    for format in self.formats:
      self.fp.write('oneDimArrayNestedStruct', outputNestedStructArray)
      inputNestedStructArray = self.fp.read('oneDimArrayNestedStruct')

    for i in xrange(10):
      for key in outputNestedStructArray[i].keys():
        if type(outputNestedStructArray[i][key]) is DictType:
          for k in outputNestedStructArray[i][key]:
            self.assertEqual(outputNestedStructArray[i][key][k], 
                              inputNestedStructArray[i][key][k])
        else:
          self.assertEqual(outputNestedStructArray[i][key], inputNestedStructArray[i][key])


  
  # FIXME: ALL 2D tests
  # 2D Array tests: int, float, string, dict, struct, nested struct

  def xtestPTwoDimArrayInt(self):
    outputPowers = [[2, 4, 8, 16], 
                    [32, 64, 128, 256], 
                    [512, 1024, 2048, 4096]]

    self.fp.write('twoDimArrayInt', outputPowers)
    inputPowers = self.fp.read('twoDimArrayInt')
    self.assertEqual(outputPowers, inputPowers)

  def xtestQTwoDimArrayFloat(self):
    outputPow = [[2.0, 4.0, 8.0, 16.0], 
                 [32.0, 64.0, 128.0, 256.0], 
                 [512.0, 1024.0, 2048.0, 4096.0]]
      
    for format in self.formats:
      self.fp.write('twoDimArrayFloat', outputPow)
      inputPow = self.fp.read('twoDimArrayFloat')
      self.assertEqual(outputPow, inputPow)

  def xtestRTwoDimArrayString(self):
    outputString = [['This', 'test'], ['does', 'suck']]
    
    for format in self.formats:
      self.fp.write('twoDimArrayString', outputString)
      inputString = self.fp.read('twoDimArrayString')
      self.assertEqual(outputString, inputString)

  def xtestSTwoDimArrayStruct(self):
    outputThermoTwoDimArray = []

    for j in xrange(2):
      outputThermoOneDimArray = []

      for i in xrange(5):
        outputThermo = {}
        outputThermo['T'] = 1.0 + i + j
        outputThermo['p'] = 2.0 + i + j
        outputThermo['N'] = 3.0 + i + j
  
        outputThermoOneDimArray.append(outputThermo)

      outputThermoTwoDimArray.append(outputThermoOneDimArray)
      
    for format in self.formats:
      self.fp.write('twoDimArrayStruct', outputThermoTwoDimArray)
      inputThermoTwoDimArray = self.fp.read('twoDimArrayStruct')

      for j in xrange(2):
        for i in xrange(5):
          for key in outputThermoTwoDimArray[j][i].keys():
            self.assertEqual(outputThermoTwoDimArray[j][i][key], inputThermoTwoDimArray[j][i][key])

  def xtestTTwoDimArrayNestedStruct(self):
    outputNestedStructTwoDimArray = []
  
    for j in xrange(2):
      outputNestedStructOneDimArray = []
  
      for i in xrange(5):
        outputNestedStruct = {}
        outputNestedStruct['tag'] = 'triangle' + str(i+j)
        outputNestedStruct['time'] = i + j
        outputNestedStruct['xcoords'] = [i+j, i+j+1, i+j+2] 
        outputNestedStruct['ycoords'] = [i+j+3, i+j+4, i+j+5] 
        outputNestedStruct['zcoords'] = [i+j+6, i+j+7, i+j+8] 
        outputNestedStruct['twoD'] = [[i+j+9, i+j+10], [i+j+11, i+j+12]] 
        outputNestedStruct['state'] = {}
        outputNestedStruct['state']['T'] = 1.11222333444 + i + j
        outputNestedStruct['state']['p'] = 5.55666777888 + i + j
        outputNestedStruct['state']['N'] = 1.2345678910 + i + j
  
        outputNestedStructOneDimArray.append(outputNestedStruct)

      outputNestedStructTwoDimArray.append(outputNestedStructOneDimArray)
  
    for format in self.formats:
      self.fp.write('twoDimArrayNestedStruct', outputNestedStructTwoDimArray)
      inputNestedStructTwoDimArray = self.fp.read('twoDimArrayNestedStruct')

    for j in xrange(2):
      for i in xrange(5):
        for key in outputNestedStructTwoDimArray[j][i].keys():
          if type(outputNestedStructTwoDimArray[j][i][key]) is DictType:
            for k in outputNestedStructTwoDimArray[j][i][key]:
              self.assertEqual(outputNestedStructTwoDimArray[j][i][key][k], 
                                inputNestedStructTwoDimArray[j][i][key][k])
          else:
            self.assertEqual(outputNestedStructTwoDimArray[j][i][key], 
                              inputNestedStructTwoDimArray[j][i][key])
  


# Run the test cases 
if __name__ == '__main__':
  unittest.main()
  os.system('rm -f test_rw.pdb')

