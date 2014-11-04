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

class DoIO(test_leak.LeakFile):
    def tearDown(self):
        pdb.vif.close()
        test_leak.LeakFile.tearDown(self)
        

# without leak detection
class XXDoIO(unittest.TestCase):
    def setUp(self):
        self.fp = pdb.open("testfile", "w")
        
    def tearDown(self):
        self.fp.close()


class DoubleIO(DoIO):
        
    def testa(self):
        """write a C double from a python float"""
        ref = 4.0
#        d2 = struct.pack('d', ref)
        d = pdb.pdbdata(ref, 'double')
#        self.failUnlessEqual(str(d2), str(d))
#        o = d.unpack()
#        self.failUnlessEqual(o, ref)
        self.fp.write_raw('testa_raw', d, 'double')
        self.fp.write('testa', d)
        self.fp.write('testa_ref', ref)

        r = self.fp.read('testa')
        self.failUnlessEqual(r, ref)

#    def xtestb(self):
#        """write a C double from an python int"""
#        d2 = struct.pack('d', 4)
#        d = pdb.pdbdata(4)
#        self.failUnlessEqual(str(d2), str(d))
#        o = d.unpack()
#        self.failUnlessEqual(o, 4.0)

#    def xtestb1(self):
#        """write a C double from an python long"""
#        d2 = struct.pack('d', 4.0)
#        d = pdb.pdbdata(4L, 'double')
#        self.failUnlessEqual(str(d2), str(d))
#        o = d.unpack()
#        self.failUnlessEqual(o, 4.0)

#    def xtestb2(self):
#        """try to write a C double from a string"""
#        self.failUnlessRaises(pdb.error, pdb.pdbdata,
#                              'four', 'double')

    def testc(self):
        """write/read C double[2] from list of two floats"""
#        d2 = struct.pack('dd', 4.0, 5.0)
        ref = [4.0, 5.0]
        d = pdb.pdbdata(ref)
        self.fp.write_raw('testc_raw[2]', d, 'double')
        self.fp.write('testc', d)
        self.fp.write('testc_ref', ref)
#        self.failUnlessEqual(str(d2), str(d))
#        o = d.unpack()
#        self.failUnlessEqual(o, [4.0, 5.0])

        r = self.fp.read('testc')
        self.failUnlessEqual(r, ref)


    def testc1(self):
        """write/read C double[2] from tuple of two floats, implicit"""
        ref = [4.0, 5.0]
#        d2 = struct.pack('dd', 4.0, 5.0)
        self.fp.write('testc1', ref)
#        self.failUnlessEqual(str(d2), str(d))
#        o = pdb.unpack(d)
#        self.failUnlessEqual(o, [4.0, 5.0])

        r = self.fp.read('testc1')
        self.failUnlessEqual(r, ref)

    def testc2(self):
        """write/read C double[2,3], implicit"""
#        d2 = struct.pack('dd', 4.0, 5.0)
        ref = [[1.0, 2.0, 3.0], [4.0, 5.0, 6.0]]
        self.fp.write('testc2', ref)
#        self.failUnlessEqual(str(d2), str(d))
#        o = pdb.unpack(d)
#        self.failUnlessEqual(o, [4.0, 5.0])

        r = self.fp.read('testc2')
        self.failUnlessEqual(r, ref)

    def testc3(self):
        """write/read C double[2,3,4], implicit"""
#        d2 = struct.pack('dd', 4.0, 5.0)
        ref = [
            [[111., 211.], [121., 221.], [131., 231.]],
            [[112., 212.], [122., 222.], [132., 232.]],
            [[113., 213.], [123., 223.], [133., 233.]]
            ]

        self.fp.write('testc3', ref)
#        self.failUnlessEqual(str(d2), str(d))
#        o = pdb.unpack(d)
#        self.failUnlessEqual(o, [4.0, 5.0])

        r = self.fp.read('testc3')
        self.failUnlessEqual(r, ref)

    def xtestd(self):
        """write C double[2] from tuple of float and int"""
        d2 = struct.pack('dd', 4.0, 5.0)
        d = pdb.pdbdata((4.0, 5))
        self.failUnlessEqual(str(d2), str(d))
        o = d.unpack()
        self.failUnlessEqual(o, [4.0, 5.0])

    def xteste(self):
        """write C double[2] from too few items"""
#        d2 = struct.pack('dd', 4.0, 5.0)
        ref = [4.0, 5.0]
        self.failUnlessRaises(pdb.error, pdb.pdbdata,
                              ref, 'double[3]')
#        self.failUnlessEqual(str(d2), str(d))
#        o = d.unpack()
#        self.failUnlessEqual(o, ref)

    def xtestf(self):
        """write C double[2] from too many items"""
#        d2 = struct.pack('dd', 4.0, 5.0)
        ref = [4.0, 5.0, 6.0, 7.0]
        self.failUnlessRaises(pdb.error, pdb.pdbdata,
                              ref, 'double[3]')
#        self.failUnlessEqual(str(d2), str(d))
#        o = d.unpack()
#        self.failUnlessEqual(o, ref)

    def xtestg(self):
        """write "double *" from None"""
        d = pdb.pdbdata(None, 'double *')
        o = d.unpack()
        self.failUnlessEqual(o, None)

    def testh(self):
        """write "double *" from [1.0]"""
#        d2 = struct.pack('d', 1.)
        ref = [1.]
        d = pdb.pdbdata(ref, 'double *')
        self.fp.write_raw('testh_raw', d, 'double *')
        self.fp.write('testh', d)
        self.fp.write('testh_ref', ref)
#        self.failUnlessEqual(str(d2), str(d))
#        o = d.unpack()
#        self.failUnlessEqual(o, ref)

        r = self.fp.read('testh')
        self.failUnlessEqual(r, ref)

    def testi(self):
        """write "double *" from [1.0, 2.0]"""
#        d2 = struct.pack('dd', 1., 2.)
        ref = [1., 2.]
        d = pdb.pdbdata(ref, 'double *')
        self.fp.write_raw('testi_raw', d, 'double *')
        self.fp.write('testi', d)
        self.fp.write('testi_ref', ref)
#        self.failUnlessEqual(str(d2), str(d))
#        o = d.unpack()
#        self.failUnlessEqual(o, ref)

        r = self.fp.read('testi')
        self.failUnlessEqual(r, ref)

    def xtestj(self):
        """write "double **" from None"""
        d = pdb.pdbdata(None, 'double **')
        o = d.unpack()
        self.failUnlessEqual(o, None)

    def testk(self):
        """write "double **" from [None, [4., 5.]]"""
        ref = [None, [4., 5.]]
        d = pdb.pdbdata(ref, 'double **')
        self.fp.write_raw('testk_raw', d, 'double **')
        self.fp.write('testk', d)
#        self.fp.write('testk_ref', ref)
#        self.fp.write('testk_a', ref, 'double *[2]')

        r = self.fp.read('testk')
        self.failUnlessEqual(r, ref)

    def testl(self):
        """write "double **" from [[1.0, 2.0, 3.0], [4., 5.]]"""
        ref = [[1.0, 2.0, 3.0], [4., 5.]]
        d = pdb.pdbdata(ref, 'double **')
        self.fp.write_raw('testl_raw', d, 'double **')
        self.fp.write('testl', d)
        self.fp.write('testl_ref', ref)
##        self.fp.write('testl_a', ref, 'double *[2]')

##        r = self.fp.read('testl_ref')

        r = self.fp.read('testl')
        self.failUnlessEqual(r, ref)

#--------------------------------------------------------------------------

    def testm(self):
        """write "double **" from [None, None, None]"""
        ref = [None, None, None]
        d = pdb.pdbdata(ref, 'double **')
        self.fp.write_raw('testm_raw', d, 'double **')
        self.fp.write('testm', d)
#        self.fp.write('testm_ref', ref)

        r = self.fp.read('testm')
        self.failUnlessEqual(r, ref)

    def testn(self):
        """write "double *[2]" from [[2.0], [4., 5., 6.]]"""
        ref = [[2.0], [4., 5., 6.]]
        d = pdb.pdbdata(ref, 'double *[2]')
        self.fp.write_raw('testn_raw[2]', d, 'double *')
        self.fp.write('testn[2]', d)
#        self.fp.write('testn_ref[2]', ref)

        r = self.fp.read('testn')
        self.failUnlessEqual(r, ref)

    def testo(self):
        """write "double **[3]" """
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
        d = pdb.pdbdata(ref, 'double **[3]')
        self.fp.write_raw('testo_raw[3]', d, 'double **')
        self.fp.write('testo', d)
#        self.fp.write('testo_raw', ref)

        r = self.fp.read('testo')
        self.failUnlessEqual(r, ref)

class StringIO(DoIO):
        
    def testStringa(self):
        """write a C char from a python string"""
        ref = 'a'
        d = pdb.pdbdata(ref)
        self.fp.write_raw('testa_raw', d, 'char')
        self.fp.write('testa', d)

        r = self.fp.read('testa')
        self.failUnlessEqual(r, ref)

    def testStringb(self):
        """write a C char[5] from a python string"""
        ref = 'abcde'
        d = pdb.pdbdata('abcde', 'char[5]')
        self.fp.write_raw('testb_raw[5]', d, 'char')
        self.fp.write('testb', d)

        r = self.fp.read('testb')
        self.failUnlessEqual(r, ref)

    def testStringc(self):
        """write a C char** from a python string"""
        ref = ["hello", "world!"]
        d = pdb.pdbdata(ref, 'char**')
        self.fp.write_raw('testc_raw', d, 'char **')
        self.fp.write('testc', d)

        r = self.fp.read('testc')
        self.failUnlessEqual(r, ref)

    def xtestStringd(self):
        """write a list of strings"""
#        d = pdb.pdbdata('abcde', 'char[5]')
#        self.fp.write_raw('testd_raw[5]', d, 'char')
        self.fp.write('testd', ["hello", "world"])


class DefstrIO(DoIO):

#--------------------------------------------------------------------------

    def testd(self):
        """create a defstr then pdbdata"""
        name = 'ptestd'
        typ  = name + '_s'
        ref = (1, 2)
        fp = self.fp
#        d1 = struct.pack('if', 1, 2)
        d = fp.defstr(typ, ('int i', 'float j'))
#        d2 = pdb.pdbdata(ref, typ)
#        self.failUnlessEqual(str(d1), str(d2))
#        d3 = pdb.unpack(d2)

        fp.write('ptestd', ref, typ)
        fp.write('ptestd0', ref)


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
class ClassObjectIO(test_leak.LeakFile):
    def testa1(self):
        "Write class instance to file"
        input = UserClass(1, 2, 3.5)
        self.fp.defstr( "user", ("int a", "int b", "float c"))
        self.fp.register_class(UserClass, 'user', makeUserClass)

        self.fp.write("var1", input)
        r = self.fp.read("var1")
        self.failUnlessEqual(r.__class__, UserClass)
        self.failUnlessEqual(r, input)

    def testa2(self):
        "Write class instance to file using type"
        input = UserClass(1, 2, 3.5)
        self.fp.defstr( "user", ("int a", "int b", "float c"))
        self.fp.register_class(UserClass, 'user', makeUserClass)

        self.fp.write("var1", input, 'user *')
        r = self.fp.read("var1")
        self.failUnless(isinstance(r, list))
        self.failUnlessEqual(len(r), 1)
        self.failUnlessEqual(r[0].__class__, UserClass)
        self.failUnlessEqual(r[0], input)

    def testb1(self):
        "Write class instance array to file"
        input1 = UserClass(1, 2, 3.5)
        input2 = UserClass(4, 5, 6.5)
        input3 = UserClass(7, 8, 9.5)
        input = [ input1, input2, input3 ]
        self.fp.defstr( "user", ("int a", "int b", "float c"))
        self.fp.register_class(UserClass, 'user', makeUserClass)

        self.fp.write("var1", input)
        r = self.fp.read("var1")
        self.failUnlessEqual(r, input)

    def testb2(self):
        "Write class instance array to file using type"
        input1 = UserClass(1, 2, 3.5)
        input2 = UserClass(4, 5, 6.5)
        input3 = UserClass(7, 8, 9.5)
        input = [ input1, input2, input3 ]
        self.fp.defstr( "user", ("int a", "int b", "float c"))
        self.fp.register_class(UserClass, 'user', makeUserClass)

        self.fp.write("var1", input, 'user *')
        r = self.fp.read("var1")
        self.failUnlessEqual(r, input)




if __name__ == '__main__':
    unittest.main()
