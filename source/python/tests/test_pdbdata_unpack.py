#!/usr/bin/env python
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

""" This module test functions

Use the struct module to test the output for simple types
(no derived types).

"""

import pact_test.pdb as pdb
import unittest
import struct
import test_leak


class Pdbdata(test_leak.LeakVif):
    def testa(self):
        """Check argument type of pdbdata query functions"""
        self.assertRaises(TypeError, pdb.getdefstr, 1)
        self.assertRaises(TypeError, pdb.gettype, 1)
        self.assertRaises(TypeError, pdb.getfile, 1)
        self.assertRaises(TypeError, pdb.getdata, 1)
        self.assertRaises(TypeError, pdb.unpack, 1)
        
    def testb(self):
        """pdbdata get attributes functions"""
        d = pdb.pdbdata('double', 4.0)
        data = pdb.getdata(d)
        
        dp = pdb.getdefstr(d)
        self.assertEqual(type(dp), pdb.defstr)

        # check singleton nature of defstr's
        dpvif = pdb.vif.defstr("double")
        self.assert_(dp is dpvif)

        t = pdb.gettype(d)
        self.assertEqual(t, "double")

        t = pdb.getfile(d)
        self.assert_(t is pdb.vif)

        v = pdb.unpack(d)
        self.assertEqual(v, 4.0)

#--------------------------------------------------------------------------

class Double(test_leak.LeakVif):
    def testa(self):
        """create a C double from a python float"""
        d2 = struct.pack('d', 4.0)
        d = pdb.pdbdata('double', 4.0)
        self.assertEqual(str(d2), str(d))
        o = pdb.unpack(d)
        self.assertEqual(o, 4.0)

    def testa1(self):
        """create a C double from a python float, delete"""
        d = pdb.pdbdata('double', 4.0)
        del d

    def testb(self):
        """create a C double from an python int"""
        d2 = struct.pack('d', 4)
        d = pdb.pdbdata('double', 4)
        self.assertEqual(str(d2), str(d))
        o = pdb.unpack(d)
        self.assertEqual(o, 4.0)

    def testb1(self):
        """create a C double from an python long"""
        d2 = struct.pack('d', 4.0)
#        d = pdb.pdbdata('double', 4L)
        d = pdb.pdbdata('double', 4)
        self.assertEqual(str(d2), str(d))
        o = pdb.unpack(d)
        self.assertEqual(o, 4.0)

    def testb2(self):
        """try to create a C double from a string"""
        self.assertRaises(pdb.error, pdb.pdbdata,
                              'double', 'four')

    def testc(self):
        """create C double[2] from tuple of two floats"""
        d2 = struct.pack('dd', 4.0, 5.0)
        d = pdb.pdbdata('double[2]', (4.0, 5.0))
        self.assertEqual(str(d2), str(d))
        o = pdb.unpack(d)
        self.assertEqual(o, [4.0, 5.0])

    def xtestc1(self):
        """create C double[2] from tuple of two floats, implicit"""
        d2 = struct.pack('dd', 4.0, 5.0)
        d = pdb.pdbdata('double', (4.0, 5.0))
        self.assertEqual(str(d2), str(d))
        o = pdb.unpack(d)
        self.assertEqual(o, [4.0, 5.0])

    def xtestc2(self):
        pass

#--------------------------------------------------------------------------

    def testd(self):
        """create C double[2] from tuple of float and int"""
        d2 = struct.pack('dd', 4.0, 5.0)
        d = pdb.pdbdata('double[2]', (4.0, 5))
        self.assertEqual(str(d2), str(d))
        o = pdb.unpack(d)
        self.assertEqual(o, [4.0, 5.0])

    def teste(self):
        """create C double[2] from too few items"""
#        d2 = struct.pack('dd', 4.0, 5.0)
        ref = [4.0, 5.0]
        self.assertRaises(pdb.error, pdb.pdbdata,
                              'double[3]', ref)
#        self.assertEqual(str(d2), str(d))
#        o = pdb.unpack(d)
#        self.assertEqual(o, ref)

    def testf(self):
        """create C double[2] from too many items"""
#        d2 = struct.pack('dd', 4.0, 5.0)
        ref = [4.0, 5.0, 6.0, 7.0]
        self.assertRaises(pdb.error, pdb.pdbdata,
                              'double[3]', ref)
#        self.assertEqual(str(d2), str(d))
#        o = pdb.unpack(d)
#        self.assertEqual(o, ref)

    def testg(self):
        """create "double *" from None"""
        d = pdb.pdbdata('double *', None)
        o = pdb.unpack(d)
        self.assertEqual(o, None)

    def testh(self):
        """create "double *" from [1.0]"""
        d2 = struct.pack('d', 1.)
        ref = [1.]
        d = pdb.pdbdata('double *', ref)
        self.assertEqual(str(d2), str(d))
        o = pdb.unpack(d)
        self.assertEqual(o, ref)

    def testi(self):
        """create "double *" from [1.0, 2.0]"""
        d2 = struct.pack('dd', 1., 2.)
        ref = [1., 2.]
        d = pdb.pdbdata('double *', ref)
        self.assertEqual(str(d2), str(d))
        o = pdb.unpack(d)
        self.assertEqual(o, ref)

#--------------------------------------------------------------------------

    def testj(self):
        """create "double **" from None"""
        d = pdb.pdbdata('double **', None)
        o = pdb.unpack(d)
        self.assertEqual(o, None)

    def testk(self):
        """create "double **" from [None, [4., 5.]]"""
        ref = [None, [4., 5.]]
        d = pdb.pdbdata('double **', ref)
        o = pdb.unpack(d)
        self.assertEqual(o, ref)

    def testl(self):
        """create "double **" from [[1.0, 2.0, 3.0], [4., 5.]]"""
        ref = [[1.0, 2.0, 3.0], [4., 5.]]
        d = pdb.pdbdata('double **', ref)
        o = pdb.unpack(d)
        self.assertEqual(o, ref)

    def testm(self):
        """create "double **" from [None, None, None]"""
        ref = [None, None, None]
        d = pdb.pdbdata('double **', ref)
        o = pdb.unpack(d)
        self.assertEqual(o, ref)

    def testn(self):
        """create "double *[2]" from [[2.0], [4., 5., 6.]]"""
        ref = [[2.0], [4., 5., 6.]]
        d = pdb.pdbdata('double *[2]', ref)
        o = pdb.unpack(d)
        self.assertEqual(o, ref)

    def testo(self):
        """create "double **[3]" """
        ref = [
               [                      # double **
                [1.0],                # double *
                [2.0, 3.0, 4.0]       # double *
               ],[                    # [1]
                [1.1],                # double *
                [2.1, 3.1, 4.1],      # double *
                None
               ], None
              ]
        d = pdb.pdbdata('double **[3]', ref)
        o = pdb.unpack(d)
        self.assertEqual(o, ref)

#--------------------------------------------------------------------------

# indexing tests

    def testp1(self):
        """Index a scalar double (read)"""
        d = pdb.pdbdata('double', 4.0)
        l = len(d)
        self.assertEqual(l, 1)
        self.assertRaises(IndexError, lambda : d[1])
        p = d[0]
        self.assertEqual(pdb.unpack(d), 4.0)

    def testp2(self):
        """Index a single subscripted double array (read)"""
        d = pdb.pdbdata('double[2]', (4.0, 5.0))
        l = len(d)
        self.assertEqual(l, 2)
        p = d[0]
        self.assertEqual(p, 4.0)
        p = d[1]
        self.assertEqual(p, 5.0)

#    def testp0(self):
#        """Index a NULL pointer (read)"""
#        d = pdb.pdbdata('double *', None)
#        l = len(d)
#        self.assertEqual(l, 0)

    def testq1(self):
        """Index a scalar double (write)"""
        d = pdb.pdbdata('double', 4.0)
        def foo():
            d[1] = 5.0
        self.assertRaises(IndexError, foo)
        d[0] = 5.0
        self.assertEqual(pdb.unpack(d), 5.0)

    def testq2(self):
        """Index a single subscripted double array (write)"""
        d = pdb.pdbdata('double[2]', (4.0, 5.0))
        d[0] = 6.0
        self.assertEqual(pdb.unpack(d), [6.0, 5.0])
        d[1] = 7.0
        self.assertEqual(pdb.unpack(d), [6.0, 7.0])



class String(test_leak.LeakVif):
    def testStringa(self):
        """create a C char from a python string"""
        d = pdb.pdbdata('char', 'a')
        self.assertEqual('a', str(d))
        o = pdb.unpack(d)
        self.assertEqual(o, 'a')

    def testStringb(self):
        """create a C char[5] from a python string"""
        d = pdb.pdbdata('char[5]', 'abcde')
        self.assertEqual('abcde', str(d))
        o = pdb.unpack(d)
        self.assertEqual(o, 'abcde')


if __name__ == '__main__':
    unittest.main()
