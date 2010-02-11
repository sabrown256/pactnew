#!/usr/bin/env python
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

""" This module test functions

Use the struct module to test the output for simple types
(no derived types).

"""

import pact.pdb as pdb
import unittest
from Numeric import *
import test_leak

class ArrayMixin:
    def xtesta(self):
        # commented out 4-21-08 pending numpy conversion (from Numeric)
        '''make a double array from an numpy array'''
        ref = [1., 2., 3., 4., 5.]
        a = array(ref, Float)
        d = pdb.pdbdata(a, 'double *', self.fp)
        x = pdb.unpack(d)
        self.failUnlessEqual(ref, x)
        self.failUnlessEqual(type(ref), type(x))
#        x = d.array()
#        self.failUnlessEqual(a, x)
#        self.failUnlessEqual(type(a), type(x))

    def xtestb(self):
        """create an double array from defstr"""
        ref = [1.0, 2.0, 4.0, 5.0]
        d2 = struct.pack('dddd', 1.0, 2.0, 4.0, 5.0)
        double = self.fp.defstr('double')
        d = double(ref, 4)
        self.failUnlessEqual(str(d2), str(d))
        o = pdb.unpack(d)
        self.failUnlessEqual(o, ref)

class ArrayVif(test_leak.LeakVif, ArrayMixin):
    pass

class ArrayFile(test_leak.LeakFile, ArrayMixin):
    pass

if __name__ == '__main__':
    unittest.main()
