#!/usr/bin/env python
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

""" This module test functions

Test defstr

"""

import pact_2.pdb as pdb
import unittest
import struct
import test_leak


class DefstrMixin:
    """A collection of classes to operate on defstrs"""
      
    def testa(self):
        """define a defstr with no arguments"""
        self.failUnlessRaises(TypeError, pdb.defstr)

    def testb(self):
        """define a defstr with too few arguments"""
        self.failUnlessRaises(TypeError, pdb.defstr, 'testb')

    def testb2(self):
        """define a defstr with wrong members"""
        self.failUnlessRaises(pdb.error, pdb.defstr, 'testb2', 1)

    def testc(self):
        """create a defstr"""
        d = pdb.defstr('testc', ('int i', 'float j'), self.fp)
        self.failUnlessEqual(type(d), pdb.defstr)

    def testc1(self):
        """try to create two defstr with the same name"""
        name = 'testc1'
        d = pdb.defstr(name, ('int i', 'float j'), self.fp)
        self.failUnlessRaises(pdb.error, pdb.defstr, 
                              name, ('int i', 'float j'), self.fp)

    def testd(self):
        """defstr attributes"""
        d = pdb.pdbdata('double', 4.0, self.fp)
        
        dp = pdb.getdefstr(d)
        self.failUnlessEqual(type(dp), pdb.defstr)
        self.failUnlessEqual(dp.type, 'double')
        # These fields are machine dependent
        # for now just make sure they exist
        size_bites = dp.size_bits
        size = dp.size
        alignment = dp.alignment
        n_indirects = dp.n_indirects
        is_indirect = dp.is_indirect
        convert = dp.convert
        onescmp = dp.onescmp
        unsgned = dp.unsgned
        order_flag = dp.order_flag

    def teste(self):
        """defstr - keys"""
        name = 'teste'
        d = pdb.defstr(name, ('int i', 'float j'), self.fp)
        k = d.keys()
        self.failUnlessEqual(k, ('i', 'j'))

    def testf(self):
        """create a C double from a python float"""
        d2 = struct.pack('d', 4.0)
        double = self.fp.defstr('double')
        d = double(4.0)
        self.failUnlessEqual(str(d2), str(d))
        o = pdb.unpack(d)
        self.failUnlessEqual(o, 4.0)

    def testf2(self):
        """create a C double, then delete"""
        double = self.fp.defstr('double')
        d = double(4.0)
        self.failUnlessEqual(pdb.unpack(d), 4.0)
        del d

    def testg(self):
        """create a double array"""
        ref = [1.0, 2.0, 4.0, 5.0]
        d2 = struct.pack('dddd', 1.0, 2.0, 4.0, 5.0)
        double = self.fp.defstr('double')
        # test ind as scalar
        d = double(ref, 4)
        self.failUnlessEqual(str(d2), str(d))
        o = pdb.unpack(d)
        self.failUnlessEqual(o, ref)

    def testg1(self):
        """create an array with a shape"""
        ref = [1.0, 2.0, 3.0, 4.0]
        d2 = struct.pack('dddd', 1.0, 2.0, 3.0, 4.0)
        double = self.fp.defstr('double')
        d = double(ref, 4)
        self.failUnlessEqual(str(d2), str(d))
        o = pdb.unpack(d)
        self.failUnlessEqual(o, ref)

    def testg2(self):
        """create a double array, then index"""
        ref = [1.0, 2.0, 3.0, 4.0]
        double = self.fp.defstr('double')
        d = double(ref, 4)

        d0 = d[0]
        self.failUnlessEqual(pdb.unpack(d0), 1.0)
        d0a = pdb.unpack(d0)
        self.failUnlessEqual(d0a, 1.0)

        d[0] = 5.0
        self.failUnlessEqual(pdb.unpack(d0), 5.0)
        self.failUnlessEqual(pdb.unpack(d), [5.0, 2.0, 3.0, 4.0])
        # Note: d0a does not change
        self.failUnlessEqual(d0a, 1.0)

        # reassign d
        del d
        self.failUnlessEqual(pdb.unpack(d0), 5.0)
        

    def testh(self):
        """Access defstr members as attributes"""
        name = 'testh'
        d = pdb.defstr(name, ('int i', 'float j'), self.fp)
        d2 = d((1, 2))

        a = dir(d2)
        self.failUnless('i' in a)
        self.failUnless('j' in a)
        
        d3 = d2.i
        self.failUnlessEqual(d3, 1)
        d4 = d2.j
        self.failUnlessEqual(d4, 2.0)

        d2.i = 3
        d2.j = 4
        u = pdb.unpack(d2)
        self.failUnlessEqual(u, (3, 4.0))

    def testi(self):
        """create a defstr then array of data"""
        name = 'testi'
        d = pdb.defstr(name, ('int i', 'float j[3]'), self.fp)
        ref = [(11,[12, 13, 14]), (21, [22, 23, 24]),
               (31,[32, 33, 34])]
        # test ind as tuple
        d2 = d(ref, (3,))
        d3 = pdb.unpack(d2)
        self.failUnlessEqual(ref, d3)

        t0 = d2[0]
        self.failUnlessEqual(type(d2), type(t0))
        self.failUnlessEqual(pdb.unpack(t0), ref[0])
        self.failUnlessEqual(t0.i, 11)
        self.failUnlessEqual(t0.j, [12., 13., 14])
        del d2
        self.failUnlessEqual(pdb.unpack(t0), ref[0])



class DefstrVif(test_leak.LeakVif, DefstrMixin):
    pass


class DefstrFile(test_leak.LeakFile, DefstrMixin):
    pass


class DefstrPdbdata:#(unittest.TestCase):

    def testd(self):
        """create a defstr then pdbdata"""
        name = 'ptestd'
        d1 = struct.pack('if', 1, 2)
        d = pdb.defstr(name, ('int i', 'float j'))
        d2 = pdb.pdbdata(name, (1, 2))
#        self.failUnlessEqual(str(d1), str(d2))
#        d3 = pdb.unpack(d2)

    def testd1(self):
        """create a defstr then use dir"""
        name = 'ptestd1'
        d1 = struct.pack('if', 1, 2)
        d = pdb.defstr(name, ('int i', 'float j'))
        d2 = pdb.pdbdata(name, (1, 2))
        d3 = pdb.getdefstr(d2)
        self.failUnlessEqual(d, d3)
        self.failUnlessEqual(d3.keys(), ('i', 'j'))
        
#        a = dir(d2)
#        self.failUnless('i' in a)
#        self.failUnless('j' in a)

    def testd2(self):
        """Access defstr members as attributes"""
        name = 'ptestd2'
        d = pdb.defstr(name, ('int i', 'float j'))
#        d2 = pdb.pdbdata(name, (1, 2))
        d2 = d((1, 2))
        d3 = d2.i
        self.failUnlessEqual(d3, 1)
        d4 = d2.j
        self.failUnlessEqual(d4, 2.0)

        d1 = struct.pack('if', 3, 4)
        d2.i = 3
        d2.j = 4
        self.failUnlessEqual(str(d1), str(d2))


    def teste(self):
        """create a defstr then pdbdata array"""
        name = 'pteste'
        d1 = struct.pack('ififif', 1, 2, 3, 4, 5, 6)
        d = pdb.defstr(name, ('int i', 'float j'))
        ref = [(1,2), (3, 4), (5, 6)]
        d2 = pdb.pdbdata(name + '[3]', ref)
        self.failUnlessEqual(str(d1), str(d2))
        d3 = pdb.unpack(d2)

    def testf(self):
        """create a defstr then pdbdata pointer"""
        name = 'ptestf'
        d1 = struct.pack('ififif', 1, 2, 3, 4, 5, 6)
        d = pdb.defstr(name, ('int i', 'float j'))
        ref = [(1,2), (3, 4), (5, 6)]
        d2 = pdb.pdbdata(name + ' *', ref)
        self.failUnlessEqual(str(d1), str(d2))
        d3 = pdb.unpack(d2)
        self.failUnlessEqual(ref, d3)

    def testg(self):
        """create a defstr with pointer then pdbdata"""
        name = 'ptestg'
        d = pdb.defstr(name, ('int i', 'float *j'))
        ref = [(1,[2]), (3, [4, 5, 6, 7]), (8, None)]
        d2 = pdb.pdbdata(name + ' *', ref)
        d3 = pdb.unpack(d2)
        self.failUnlessEqual(ref, d3)

    def testh(self):
        """create a defstr then pdbdata with too few members"""
        name = 'ptesth'
        d = pdb.defstr(name, ('int i', 'float j'))
        self.failUnlessRaises(pdb.error, pdb.pdbdata,
                              name, (1,))

    def testi(self):
        """create a defstr then pdbdata with too many members"""
        name = 'ptesti'
        d = pdb.defstr(name, ('int i', 'float j'))
        self.failUnlessRaises(pdb.error, pdb.pdbdata,
                              name, (1, 2, 3))

class Memdes:#(unittest.TestCase):    

    def testc(self):
        """memdes from defstr, defstr as mapping"""
        name = 'mtestc'
        d = pdb.defstr(name, ('int i',
                              'float j[4][5]',
                              'double *k'))
        m1 = d['i']
        self.failUnlessEqual(type(m1), pdb.memdes)

        self.failUnlessEqual(m1.member, 'int i')
        self.failUnlessEqual(m1.cast_memb, None)
        self.failUnlessEqual(m1.cast_offs, -1)
        self.assert_(not m1.is_indirect)
        self.failUnlessEqual(m1.type, 'int')
        self.failUnlessEqual(m1.base_type, 'int')
        self.failUnlessEqual(m1.name, 'i')
        self.failUnlessEqual(m1.number, 1)

        m2 = d['j']
        self.failUnlessEqual(m2.member, 'float j[4][5]')
        self.failUnlessEqual(m2.cast_memb, None)
        self.failUnlessEqual(m2.cast_offs, -1)
        self.assert_(not m2.is_indirect)
        self.failUnlessEqual(m2.type, 'float')
        self.failUnlessEqual(m2.base_type, 'float')
        self.failUnlessEqual(m2.name, 'j')
        self.failUnlessEqual(m2.number, 20)

        m3 = d['k']
        self.failUnlessEqual(m3.member, 'double *k')
        self.failUnlessEqual(m3.cast_memb, None)
        self.failUnlessEqual(m3.cast_offs, -1)
        self.assert_(m3.is_indirect)
        self.failUnlessEqual(m3.type, 'double *')
        self.failUnlessEqual(m3.base_type, 'double')
        self.failUnlessEqual(m3.name, 'k')
        self.failUnlessEqual(m3.number, 1)

    def testd(self):
        """memdes from defstr, assign to mapping"""
        name = 'mtestd'
        d = pdb.defstr(name, ('int i',
                              'float j[4][5]',
                              'double *k'))
        def assign():
            d['i'] = 5
        self.failUnlessRaises(TypeError, assign)


if __name__ == '__main__':
    unittest.main()
    
