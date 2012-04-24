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

#--------------------------------------------------------------------------

    def testc1(self):
        """try to create two defstr with the same name"""
        name = 'testc1'
        d = pdb.defstr(name, ('int i', 'float j'), self.fp)
        self.failUnlessRaises(pdb.error, pdb.defstr, 
                              name, ('int i', 'float j'), self.fp)

    def testc2(self):
        """Create a defstr via the factory function"""
        name = 'testc2'
        d = self.fp.defstr(name, ('int i', 'float j'))
        self.failUnlessEqual(type(d), pdb.defstr)
        
    def testc3(self):
        """Create a defstr, install again under a new name"""
        d1 = self.fp.defstr('testc3', ('int i', 'float j'))
        d2 = self.fp.defstr('testc3a', d1)
        v2 = d2((1, 2))
        
    def testd(self):
        """defstr attributes"""
        d = pdb.pdbdata(4.0, 'double', self.fp)
        
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

    def driverf(self):
        self.ref = 4.0
        double = self.fp.defstr('double')
        d = double(self.ref)
        return d

    def testf1a(self):
        """create a C double from a python float, compare str"""
        d = self.driverf()
        d2 = struct.pack('d', self.ref)
        self.failUnlessEqual(str(d2), str(d))

#--------------------------------------------------------------------------

    def testf1b(self):
        """create a C double from a python float, unpack"""
        d = self.driverf()
        o = pdb.unpack(d)
        self.failUnlessEqual(o, self.ref)

    def testf1c(self):
        """create a C double from a python float, unpack AS_OBJECT"""
        d = self.driverf()
        o = pdb.unpack(d, scalar=pdb.AS_OBJECT)
        self.failUnlessEqual(o, self.ref)

    def testf1d(self):
        """create a C double from a python float, unpack AS_PDBDATA"""
        d = self.driverf()
        o = pdb.unpack(d, scalar=pdb.AS_PDBDATA)
        self.failUnlessEqual(o, self.ref)

    def testf1e(self):
        """create a C double from a python float, unpack AS_LIST"""
        d = self.driverf()
        self.failUnlessRaises(pdb.error, pdb.unpack, d,
                              pdb.AS_NONE, pdb.AS_NONE, pdb.AS_LIST)

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

#--------------------------------------------------------------------------

    def driverg(self):
        """create a double array"""
        self.ref = [1.0, 2.0, 3.0, 4.0]
        double = self.fp.defstr('double')
        d = double(self.ref, 4)
        return d

    def testg1(self):
        """create a double array, compare with str"""
        d2 = struct.pack('dddd', 1.0, 2.0, 3.0, 4.0)
        d = self.driverg()
        self.failUnlessEqual(str(d2), str(d))

    def testg1a(self):
        """double array, unpack with default"""
        d = self.driverg()
        o = pdb.unpack(d)
        self.failUnlessEqual(type(o), list)
        self.failUnlessEqual(o, self.ref)

#--------------------------------------------------------------------------

    def testg1b(self):
        """double array, unpack AS_PDBDATA"""
        d = self.driverg()
        o = pdb.unpack(d, array=pdb.AS_PDBDATA)
        self.failUnlessEqual(type(o), list)
        self.failUnlessEqual(o, self.ref)

    def testg1c(self):
        """double array, unpack AS_TUPLE"""
        d = self.driverg()
        o = pdb.unpack(d, array=pdb.AS_TUPLE)
        self.failUnlessEqual(type(o), tuple)
        self.failUnlessEqual(o, tuple(self.ref))

    def testg1d(self):
        """double array, unpack AS_LIST"""
        d = self.driverg()
        o = pdb.unpack(d, array=pdb.AS_LIST)
        self.failUnlessEqual(type(o), list)
        self.failUnlessEqual(o, list(self.ref))

    def testg1e(self):
        """double array, unpack AS_OBJECT"""
        d = self.driverg()
        self.failUnlessRaises(pdb.error, pdb.unpack, d,
                              pdb.AS_OBJECT, pdb.AS_NONE, pdb.AS_NONE)

#--------------------------------------------------------------------------

    def testg1f(self):
        """create array, index as pdbdata"""
        d = self.driverg()
        pdb.setform(scalar=pdb.AS_PDBDATA)
        d0 = d[0]
        self.assert_(isinstance(d0, pdb.pdbdata))
        self.failUnlessEqual(pdb.unpack(d0), self.ref[0])

    def testg1g(self):
        """create array, index as object"""
        d = self.driverg()
        pdb.setform(scalar=pdb.AS_OBJECT)
        d0 = d[0]
        self.assert_(type(d0), float)
        self.failUnlessEqual(d0, self.ref[0])

#--------------------------------------------------------------------------

    def testg1h(self):
        """create array, negative index as pdbdata"""
        d = self.driverg()
        pdb.setform(scalar=pdb.AS_PDBDATA)
        d0 = d[-1]
        self.assert_(isinstance(d0, pdb.pdbdata))
        self.failUnlessEqual(pdb.unpack(d0), self.ref[-1])

    def testg1i(self):
        """create array, negative index as object"""
        d = self.driverg()
        pdb.setform(scalar=pdb.AS_OBJECT)
        d0 = d[-1]
        self.assert_(type(d0), float)
        self.failUnlessEqual(d0, self.ref[-1])

    def testg3a(self):
        """create array, slice of 2 as pdbdata"""
        d = self.driverg()
        pdb.setform(array=pdb.AS_PDBDATA)
        d0 = d[0:2]
        self.assert_(isinstance(d0, pdb.pdbdata))
        self.failUnlessEqual(pdb.unpack(d0), self.ref[0:2])

    def testg3b(self):
        """create array, slice of 2 as list"""
        d = self.driverg()
        pdb.setform(array=pdb.AS_LIST)
        d0 = d[0:2]
        self.assert_(type(d0), list)
        self.failUnlessEqual(d0, self.ref[0:2])

#--------------------------------------------------------------------------

    def testg3c(self):
        """create array, slice of 2 as pdbdata"""
        d = self.driverg()
        pdb.setform(array=pdb.AS_TUPLE)
        d0 = d[0:2]
        self.assert_(type(d0), tuple)
        self.failUnlessEqual(d0, tuple(self.ref[0:2]))

    def xtestg3d(self):
        """create array, slice of 1 as pdbdata"""
        d = self.driverg()
        pdb.setform(array=pdb.AS_PDBDATA)
        d0 = d[1:1]
        self.assert_(isinstance(d0, pdb.pdbdata))
        self.failUnlessEqual(pdb.unpack(d0), self.ref[1:1])

    def testg3e(self):
        """create array, slice of 1 as list"""
        d = self.driverg()
        pdb.setform(array=pdb.AS_LIST)
        d0 = d[1:1]
        self.assert_(type(d0), list)
        self.failUnlessEqual(d0, self.ref[1:1])

#--------------------------------------------------------------------------

    def testg3f(self):
        """create array, slice of 2 as pdbdata"""
        d = self.driverg()
        pdb.setform(array=pdb.AS_TUPLE)
        d0 = d[1:1]
        self.assert_(type(d0), tuple)
        self.failUnlessEqual(d0, tuple(self.ref[1:1]))

    def testg4(self):
        """create a double array, reference 2nd item, del array"""
        # This tests that d0 keeps a reference to d
        ref = [1.0, 2.0, 3.0, 4.0]
        double = self.fp.defstr('double')
        d = double(ref, 4)

        pdb.setform(scalar=pdb.AS_PDBDATA)
        d0 = d[1]
        del d
        self.failUnlessEqual(pdb.unpack(d0), 2.0)
        
    def testh(self):
        """Access defstr members with dir, assign from tuple"""
        name = 'testh'
        d = pdb.defstr(name, ('int i', 'float j'), self.fp)
        d2 = d((1, 2))

        a = dir(d2)
        self.failUnless('i' in a)
        self.failUnless('j' in a)
        
        d2.i = 3
        d2.j = 4
        u = pdb.unpack(d2)
        self.failUnlessEqual(u, (3, 4.0))

    def testh1(self):
        """Access struct defstr, assign from dict"""
        name = 'testh'
        ref = struct.pack('if', 1, 2)
        d = pdb.defstr(name, ('int i', 'float j'), self.fp)
        d2 = d({'i':1, 'j':2.0, 'k':'unused'})
        self.failUnlessEqual(str(d2), str(ref))

#--------------------------------------------------------------------------

    def testh1b(self):
        """Create defstr, test exceptions"""
        name = 'testh'
        d = pdb.defstr(name, ('int i', 'float j'), self.fp)
        self.failUnlessRaises(pdb.error, d, 1)
        self.failUnlessRaises(pdb.error, d, (1,))
        self.failUnlessRaises(pdb.error, d, {})

    def driverh2(self):
        name = 'testh2'
        self.ref = {'i':1, 'j':2.0}
        d = pdb.defstr(name, ('int i', 'float j'), self.fp)
        d2 = d(self.ref)
        return d2
        
    def testh2a(self):
        """structure, unpack AS_PDBDATA"""
        d = self.driverh2()
        o = pdb.unpack(d, struct=pdb.AS_PDBDATA)
        self.failUnlessEqual(type(o), tuple)
        self.failUnlessEqual(o, (1, 2.0))

#--------------------------------------------------------------------------

    def testh2b(self):
        """structure, unpack AS_TUPLE"""
        d = self.driverh2()
        o = pdb.unpack(d, struct=pdb.AS_TUPLE)
        self.failUnlessEqual(type(o), tuple)
        self.failUnlessEqual(o, (1, 2.0))

    def testh2c(self):
        """structure, unpack AS_LIST"""
        d = self.driverh2()
        self.failUnlessRaises(pdb.error, pdb.unpack, d,
                              pdb.AS_NONE, pdb.AS_LIST, pdb.AS_NONE)

    def testh2d(self):
        """structure, unpack AS_DICT"""
        d = self.driverh2()
        o = pdb.unpack(d, struct=pdb.AS_DICT)
        self.failUnlessEqual(type(o), dict)
        self.failUnlessEqual(o, self.ref)

    def testh2e(self):
        """structure, unpack AS_OBJECT"""
        d = self.driverh2()
        self.failUnlessRaises(pdb.error, pdb.unpack, d,
                              pdb.AS_NONE, pdb.AS_OBJECT, pdb.AS_NONE)

#--------------------------------------------------------------------------

    def testh2f(self):
        """Access defstr members as attributes form=AS_PDBDATA"""
        d2 = self.driverh2()
        pdb.setform(scalar=pdb.AS_PDBDATA)
        d2j = d2.j
        self.assert_(isinstance(d2j, pdb.pdbdata))
        self.failUnlessEqual(pdb.unpack(d2j), self.ref['j'])

    def testh2g(self):
        """Access defstr members as attributes form=AS_OBJECT"""
        d2 = self.driverh2()
        pdb.setform(scalar=pdb.AS_OBJECT)
        d2j = d2.j
        self.assert_(type(d2j), float)
        self.failUnlessEqual(d2j, self.ref['j'])


#--------------------------------------------------------------------------

    def driverh3(self):
        name = 'testh'
        self.ref = [(11,[12., 13., 14.]), (21, [22., 23., 24.]),
                    (31,[32., 33., 34.])]
        d = pdb.defstr(name, ('int i', 'float j[3]'), self.fp)
        # test ind as tuple
        d2 = d(self.ref, (3,))
        return d2

    def testh3a(self):
        """create a struct defstr then array of data"""
        d2 = self.driverh3()
        d3 = pdb.unpack(d2)
        self.failUnlessEqual(self.ref, d3)

    def testh3b(self):
        """create array of structs, index return tuple"""
        d2 = self.driverh3()
        pdb.setform(struct=pdb.AS_TUPLE)
        t0 = d2[1]
        self.failUnlessEqual(type(t0), tuple)
        self.failUnlessEqual(t0, self.ref[1])

    def testh3d(self):
        """create array of structs, index return dict"""
        d2 = self.driverh3()
        pdb.setform(struct=pdb.AS_DICT)
        t0 = d2[1]
        self.failUnlessEqual(type(t0), dict)
        ref = {'i':self.ref[1][0], 'j':self.ref[1][1]}
        self.failUnlessEqual(t0, ref)

    def testh3db(self):
        """create array of structs, index return pdbdata"""
        d2 = self.driverh3()
        pdb.setform(struct=pdb.AS_PDBDATA)
        t0 = d2[1]
        self.failUnlessEqual(type(t0), type(d2))
        self.failUnlessEqual(pdb.unpack(t0), self.ref[1])

#--------------------------------------------------------------------------

    def testh3e(self):
        """access member of array of struct"""
        d2 = self.driverh3()
        pdb.setform(struct=pdb.AS_PDBDATA)
        ref = self.ref
        t0 = d2[0]
        self.failUnlessEqual(t0.i, ref[0][0])
        self.failUnlessEqual(t0.j, ref[0][1])
#        self.failUnlessEqual(pdb.unpack(t0.i), ref[0][0])
#        self.failUnlessEqual(pdb.unpack(t0.j), ref[0][1])

        del d2
        self.failUnlessEqual(pdb.unpack(t0), ref[0])

    def xtesth4(self):
        """create struct, reference member, del struct"""
        name = 'testh2'
        d = pdb.defstr(name, ('int i', 'float j'), self.fp)
        d2 = d((1, 2.0))
        dj = d2.j
        del d2
        self.failUnlessEqual(pdb.unpack(dj), 2.0)

    def testi(self):
        """create nested structures from tuple and dict"""
        ref = struct.pack('ifi', 1, 2.0, 3)
        self.fp.defstr('member', ('int i', 'float j'))
        d = self.fp.defstr('top', ('member m', 'int k'))
        v = d(({'i':1, 'j':2.0}, 3))
        self.failUnlessEqual(str(v), str(ref))

class DefstrVif(test_leak.LeakVif, DefstrMixin):
    pass


class DefstrFile(test_leak.LeakFile, DefstrMixin):
    pass


class DefstrPdbdata(test_leak.LeakVif):

#--------------------------------------------------------------------------

    def testd(self):
        """create a defstr then pdbdata"""
        name = 'ptestd'
        d1 = struct.pack('if', 1, 2)
        d = pdb.defstr(name, ('int i', 'float j'))
        d2 = pdb.pdbdata((1, 2), name)
#        self.failUnlessEqual(str(d1), str(d2))
#        d3 = pdb.unpack(d2)

    def testd1(self):
        """create a defstr then use dir"""
        name = 'ptestd1'
        d1 = struct.pack('if', 1, 2)
        d = pdb.defstr(name, ('int i', 'float j'))
        d2 = pdb.pdbdata((1, 2), name)
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
#        d2 = pdb.pdbdata((1, 2), name)
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
        # shape in type  - pteste[3]
        name = 'pteste'
        d1 = struct.pack('ififif', 1, 2, 3, 4, 5, 6)
        d = pdb.defstr(name, ('int i', 'float j'))
        ref = [(1,2), (3, 4), (5, 6)]
        d2 = pdb.pdbdata(ref, name + '[3]')
        self.failUnlessEqual(str(d1), str(d2))
        d3 = pdb.unpack(d2)

#    def testf(self):
#        """create a defstr then pdbdata pointer"""
#        name = 'ptestf'
#        d1 = struct.pack('ififif', 1, 2, 3, 4, 5, 6)
#        d = pdb.defstr(name, ('int i', 'float j'))
#        ref = [(1,2.), (3, 4.), (5, 6.)]
#        d2 = pdb.pdbdata(ref, name + ' *')
#        self.failUnlessEqual(str(d1), str(d2))
#        d3 = pdb.unpack(d2)
#        self.failUnlessEqual(ref, d3)

    def testg(self):
        """create a defstr with pointer then pdbdata"""
        name = 'ptestg'
        d = pdb.defstr(name, ('int i', 'float *j'))
        ref = [(1,[2.]), (3, [4., 5., 6., 7.]), (8, None)]
        d2 = pdb.pdbdata(ref, name + ' *')
        d3 = pdb.unpack(d2)
        self.failUnlessEqual(ref, d3)

    def teste2(self):
        """create a defstr then pdbdata array, compute shape"""
        # ref is [3,2], convert to [3] since [2] are members.
        name = 'pteste'
        d1 = struct.pack('ififif', 1, 2, 3, 4, 5, 6)
        d = pdb.defstr(name, ('int i', 'float j'))
        ref = [(1,2), (3, 4), (5, 6)]
        d2 = pdb.pdbdata(ref, name)
        self.failUnlessEqual(str(d1), str(d2))
        d3 = pdb.unpack(d2)

#--------------------------------------------------------------------------

    def testh(self):
        """create a defstr then pdbdata with too few members"""
        name = 'ptesth'
        d = pdb.defstr(name, ('int i', 'float j'))
        self.failUnlessRaises(pdb.error, pdb.pdbdata,
                              (1,), name)

    def testi(self):
        """create a defstr then pdbdata with too many members"""
        name = 'ptesti'
        d = pdb.defstr(name, ('int i', 'float j'))
        self.failUnlessRaises(pdb.error, pdb.pdbdata,
                              (1, 2, 3), name)

class Memdes(test_leak.LeakVif):
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
    
