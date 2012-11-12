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
        self.failUnlessRaises(TypeError, pdb.getdefstr, 1)
        self.failUnlessRaises(TypeError, pdb.gettype, 1)
        self.failUnlessRaises(TypeError, pdb.getfile, 1)
        self.failUnlessRaises(TypeError, pdb.getdata, 1)
        self.failUnlessRaises(TypeError, pdb.unpack, 1)
        
    def testb(self):
        """pdbdata get attributes functions"""
        d = pdb.pdbdata(4.0)
        data = pdb.getdata(d)
        
        dp = pdb.getdefstr(d)
        self.failUnlessEqual(type(dp), pdb.defstr)

        # check singleton nature of defstr's
        dpvif = pdb.vif.defstr("double")
        self.assert_(dp is dpvif)

        t = pdb.gettype(d)
        self.failUnlessEqual(t, "double")

        t = pdb.getfile(d)
        self.assert_(t is pdb.vif)

        v = pdb.unpack(d)
        self.failUnlessEqual(v, 4.0)

class DataMixin:
    def work_testc2(self, input,
                    descr_type=None, descr_shape=None,
                    outtype=None, gettype=None,
                    ref_tuple=None, ref_list=None, s=None):

        if descr_type is not None:
            desc = self.fp.get_obj_descr(input)
            self.failUnlessEqual(desc[2], descr_type)
            self.failUnlessEqual(desc[3], descr_shape)

        if gettype is None:
            gettype = outtype or descr_type

        if outtype is None:
            x = pdb.pdbdata(input)
        else:
            x = pdb.pdbdata(input, outtype)
#        print(x)

        self.failUnlessEqual(pdb.gettype(x), gettype)

        if ref_tuple is None:
            if isinstance(input, tuple):
                ref_tuple = input
            elif isinstance(input, list):
                ref_tuple = tuple(input)
            else:
                ref_tuple = input
        if ref_list is None:
            if isinstance(input, tuple):
                ref_list = list(input)
            elif isinstance(input, list):
                ref_list = input
            else:
                ref_list = input

        obj = pdb.unpack(x, array=pdb.AS_LIST)
#        print("XXX", obj)
        self.failUnlessEqual(obj, ref_list)
        obj = pdb.unpack(x, array=pdb.AS_TUPLE)
#        print("YYY", obj)
        self.failUnlessEqual(obj, ref_tuple)

        if s is not None:
            self.failUnlessEqual(str(x), str(s))

##################################################


class Double(test_leak.LeakVif, DataMixin):
#class Double(test_leak.Leak, DataMixin):
    def testa(self):
        """pack/unpack double scalar, implicit type"""
        self.work_testc2(
            input = 4.,
            descr_type = 'double',
            s = struct.pack('d', 4.0),
            )

    def testb(self):
        """pack/unpack double scalar, explicit type"""
        self.work_testc2(
            input = 4.,
            descr_type = 'double',
            outtype = 'double',
            s = struct.pack('d', 4.0),
            )

    def testb1(self):
        """pack/unpack double scalar from int, explicit type"""
        self.work_testc2(
            input = 4,
            descr_type = 'long',
            outtype = 'double',
            s = struct.pack('d', 4.0),
            )

    def testb2(self):
        """pack/unpack double scalar from long integer, explicit type"""
        self.work_testc2(
            input = 4L,
            outtype = 'double',
            s = struct.pack('d', 4.0),
            )

    def testc(self):
        """pack/unpack C double[2] from tuple of two floats"""
        self.work_testc2(
            input = (4.0, 5.0),
            descr_type = 'double',
            descr_shape = ((0,1),),
            s = struct.pack('dd', 4.0, 5.0),
            )

    def testc1(self):
        """pack/unpack C double[2] from tuple of float and int"""
        self.work_testc2(
            input = (4.0, 5),
            descr_type = 'double',
            descr_shape = ((0,1),),
            s = struct.pack('dd', 4.0, 5.0),
            )

    def testc2(self):
        """pack/unpack C double[2] from tuple of two ints"""
        self.work_testc2(
            input = (4, 5),
            descr_type = 'long',
            descr_shape = ((0, 1),),
            outtype = 'double[2]',
            gettype = 'double',
            ref_tuple = (4., 5.),
            ref_list = [4., 5.],
            s = struct.pack('dd', 4.0, 5.0)
            )

    def testd(self):
        """pack/unpack C double[2,2] from tuple of floats"""
        self.work_testc2(
            input = ((1., 2.), (3., 4.), (5., 6.)),
            descr_type = 'double',
            descr_shape = ((0,2), (0,1)),
            ref_list = [[1., 2.], [3., 4.], [5., 6.]],
            s = struct.pack('dddddd', 1., 2., 3., 4., 5., 6.),
            )

# --- test pointers ---

    def test_ptra(self):
        """pack/unpack C double * from None"""
        self.work_testc2(
            input = None,
            outtype = 'double *',
            )

    def test_ptrb(self):
        """pack/unpack C double * from [4.0]"""
        self.work_testc2(
            input = [4.0],
            outtype = 'double *',
            )

    def test_ptre(self):
        """pack/unpack C double * from (4.0, 5.0)"""
        self.work_testc2(
            input = (4.0, 5.0),
            outtype = 'double *',
            )

    def test_ptrf(self):
        """pack/unpack C double ** from nested tuples of floats"""
        self.work_testc2(
            input = ((1., 2.), (3., 4.), (5., 6., 7.)),
            outtype = 'double **',
            ref_list = [[1., 2.], [3., 4.], [5., 6., 7.]],
            )

    def test_ptrg(self):
        """pack/unpack C double ** from None"""
        self.work_testc2(
            input = None,
            outtype = 'double **',
            )

    def test_ptrh(self):
        """pack/unpack "double **" from [None, [4., 5., 6.]]"""
        self.work_testc2(
            input = [None, [4., 5., 6.]],
            outtype = 'double **',
            ref_tuple = (None, (4., 5., 6.)),
            )

    def test_ptri(self):
        """pack/unpack "double **" from [None, None, None]"""
        self.work_testc2(
            input = [None, None, None],
            outtype = 'double **',
            )

    def test_ptrj(self):
        """pack/unpack "double *[2]" from [[2.0], [4., 5., 6.]]"""
        self.work_testc2(
            input = [[2.0], [4., 5., 6.]],
            outtype = 'double *[2]',
            gettype = 'double *',
            ref_tuple = ((2.0,), (4., 5., 6.)),
            )

    def test_ptrk(self):
        """pack/unpack "double **[3]" """
        self.work_testc2(
            input = [
                     [                      # double **
                      [1.0],                # double *
                      [2.0, 3.0, 4.0]       # double *
                     ],[                    # [1]
                      [1.1],                # double *
                      [2.1, 3.1, 4.1],      # double *
                      None
                     ], None
                    ],
            outtype = 'double **[3]',
            gettype = 'double **',
            ref_tuple = (
                     (                      # double **
                      (1.0,),               # double *
                      (2.0, 3.0, 4.0)       # double *
                     ),(                    # (1)
                      (1.1,),               # double *
                      (2.1, 3.1, 4.1),      # double *
                      None
                     ), None
                    ),
            )

# --- test error ---

    def test_erra(self):
        """try to pack using an illegal type"""
        self.failUnlessRaises(pdb.error, pdb.pdbdata,
                              4.0, 'special_double')
                              
    def XX_test_errb(self):
        """try to pack a C double from a string"""
        self.failUnlessRaises(pdb.error, pdb.pdbdata,
                              'four', 'double')
        
    def test_errc(self):
        """try to pack C double[2] from too few items"""
        input = [4.0, 5.0]
        self.failUnlessRaises(pdb.error, pdb.pdbdata,
                              input, 'double[3]')

    def test_errd(self):
        """try to pack C double[2] from too many items"""
        input = [4.0, 5.0, 6.0, 7.0]
        self.failUnlessRaises(pdb.error, pdb.pdbdata,
                              input, 'double[3]')

        
##################################################

class DoubleIndex(test_leak.LeakVif, DataMixin):
#class DoubleIndex(test_leak.Leak, DataMixin):
    def testp1(self):
        """Index a scalar double (read)"""
        d = pdb.pdbdata(4.0)
        l = len(d)
        self.failUnlessEqual(l, 1)
        self.failUnlessRaises(IndexError, lambda : d[1])
        p = d[0]
        self.failUnlessEqual(pdb.unpack(d), 4.0)

    def testp2(self):
        """Index a single subscripted double array (read)"""
        d = pdb.pdbdata((4.0, 5.0))
        l = len(d)
        self.failUnlessEqual(l, 2)
        p = d[0]
        self.failUnlessEqual(p, 4.0)
        p = d[1]
        self.failUnlessEqual(p, 5.0)

#    def testp0(self):
#        """Index a NULL pointer (read)"""
#        d = pdb.pdbdata(None, 'double *')
#        l = len(d)
#        self.failUnlessEqual(l, 0)

    def testq1(self):
        """Index a scalar double (write)"""
        d = pdb.pdbdata(4.0)
        def foo():
            d[1] = 5.0
        self.failUnlessRaises(IndexError, foo)
        d[0] = 5.0
        self.failUnlessEqual(pdb.unpack(d), 5.0)

    def testq2(self):
        """Index a single subscripted double array (write)"""
        d = pdb.pdbdata((4.0, 5.0))
        d[0] = 6.0
        self.failUnlessEqual(pdb.unpack(d), [6.0, 5.0])
        d[1] = 7.0
        self.failUnlessEqual(pdb.unpack(d), [6.0, 7.0])

##################################################

class String(test_leak.LeakVif, DataMixin):
#class String(test_leak.Leak, DataMixin):
    def testa(self):
        """pack/unpack a C char from a python string"""
        self.work_testc2(
            input = 'a',
            descr_type = 'char',
            descr_shape = ((0,0),),
            )

    def testb(self):
        """pack/unpack a C char from a python string"""
        self.work_testc2(
            input = 'abcdefg',
            descr_type = 'char',
            descr_shape = ((0,6),),
            )

    def testc(self):
        """pack/unpack a C char[5] from a python string"""
        self.work_testc2(
            input = 'abcde',
            outtype = 'char[5]',
            gettype = 'char',
            )

    def testd(self):
        """pack/unpack a C char * from a python string"""
        self.work_testc2(
            input = 'abcde',
            outtype = 'char *',
            )


    def teste(self):
        """pack/unpack a C char** from a tuple of strings"""
        self.work_testc2(
            input = ['hello', 'world!'],
            outtype = 'char **',
            )

# A class to convert into a pdbdata
class UserClass:
    def __init__(self, a, b, c):
        self.a = a
        self.b = b
        self.c = c
    def __repr__(self):
        return 'User(%(a)d, %(b)d, %(c)f)' % self.__dict__
    def __eq__(self, other):
        return self.a == other.a and self.b == other.b and self.c == other.c
    

def makeUserClass(dict):
    return UserClass(dict['a'], dict['b'], dict['c'])


# Turn off leak detection because the VIF will accumulate information
# with register_class that does not go away
class ClassObject(test_leak.LeakVif, DataMixin):
    def setUp(self):
        test_leak.Leak.setUp(self)
        self.fp = pdb.open_vif("namevif")

    def tearDown(self):
        self.fp.close()
        test_leak.Leak.tearDown(self)

    def testa(self):
        # reports leaks
        "Call get_obj_descr with unknown Class"
        input = UserClass(1, 2, 3)
        self.failUnlessRaises(pdb.error, self.fp.get_obj_descr, input)

    def testb1(self):
        "Create pdbdata from class instance"
        input = UserClass(1, 2, 3)
        self.fp.defstr( "user", ("int a", "int b", "float c"))
        self.fp.register_class(UserClass, 'user')

        desc = self.fp.get_obj_descr(input)
        self.failUnlessEqual(desc[2], 'user')

        x = pdb.pdbdata(input, file=self.fp)
        self.failUnlessEqual(type(x), pdb.pdbdata)
        self.failUnlessEqual(pdb.gettype(x), 'user')
        
#        self.failUnlessEqual(desc[3], descr_shape)
#        self.work_testc2(uvar,
#                         descr_type = 'user'
#                         )

    def testb2(self):
        "Create pdbdata array from class instance"
        input1 = UserClass(1, 2, 3.5)
        input2 = UserClass(4, 5, 6.5)
        input3 = UserClass(7, 8, 9.5)
        input = [ input1, input2, input3 ]
        self.fp.defstr( "user", ("int a", "int b", "float c"))
        self.fp.register_class(UserClass, 'user')

        desc = self.fp.get_obj_descr(input)
        print(desc)
        self.failUnlessEqual(desc[2], 'user')
        self.failUnlessEqual(desc[3], ((0, 2),))

        x = pdb.pdbdata(input, file=self.fp)
        self.failUnlessEqual(type(x), pdb.pdbdata)
        self.failUnlessEqual(pdb.gettype(x), 'user')



if __name__ == '__main__':
    unittest.main()
