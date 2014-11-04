#!/usr/bin/env python
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

""" This module test functions

"""

import pact_test.pdb as pdb
import unittest
import os
import struct
import test_leak

class File(test_leak.Leak):      
    def test0a(self):
        '''PDBfile - existence of pdb.files'''
        self.failUnlessEqual(type(pdb.files), dict)
        self.failUnlessEqual(len(pdb.files), 0)
    
    def test0(self):
        '''PDBfile - open with no arguments'''
        self.failUnlessRaises(TypeError, pdb.PDBfile);

    def test1(self):
        '''PDBfile - make sure output does not exist, then try to open'''
        try:
            os.unlink("testfile")
        except OSError:
            pass # file does not exits
        self.failUnlessRaises(pdb.error, pdb.PDBfile, "testfile", "r");
#        fp.close()

    def test1a(self):
        '''PDBfile - open and close file'''
        fp = pdb.PDBfile("testfile", "w");
        self.failUnlessEqual(type(fp), pdb.PDBfile)
        self.failUnlessEqual(len(pdb.files), 1)
        self.assert_(pdb.files.has_key('testfile'))
        # cause defstr object to be created so it can be deleted
        # during the close
        fp.defstr("double")
        fp.close()
        self.failUnlessEqual(len(pdb.files), 0)

    def test1b(self):
        '''PDBfile - open and close file using "open"'''
        fp = pdb.open("testfile", "w");
        self.failUnlessEqual(type(fp), pdb.PDBfile)
        fp.close()

    def test1c(self):
        '''PDBfile - open mode defaults to "r"'''
        fp = pdb.open("testfile", "w");
        fp.close()
        fp = pdb.open("testfile");
        self.failUnlessEqual(fp.mode, pdb.PD_OPEN)
        fp.close()

#--------------------------------------------------------------------------

    def test1d(self):
        '''PDBfile - get attributes'''
        fp = pdb.open("testfile", "w");
        name = fp.name
        self.failUnlessEqual(name, "testfile")
        self.failUnlessEqual(fp.type, None)
#        symtab = fp.symtab
#        chart = fp.chart
#        host_chart = fp.host_chart
#        chart_hashed = fp.chart_hashed
#        host_chart_hashed = fp.host_chart_hashed
#        attrtab = fp.attrtab

        previous_file = fp.previous_file
        self.failUnlessEqual(previous_file, None)

        date = fp.date
        self.failUnlessEqual(type(date), str)

        self.failUnlessEqual(fp.mode, pdb.PD_CREATE)
        self.failUnlessEqual(fp.default_offset, 0)
        self.failUnlessEqual(fp.virtual_internal, 0)
        system_version = fp.system_version
        self.failUnlessEqual(fp.major_order, pdb.ROW_MAJOR_ORDER)
        fp.close()

    def xtest1e(self):
        '''PDBfile - set attributes'''
        # These will all raise TypeError
        fp = pdb.open("testfile", "w");
        fp.name = "foo"
        fp.type = None

#        symtab = fp.symtab
#        chart = fp.chart
#        host_chart = fp.host_chart
#        chart_hashed = fp.chart_hashed
#        host_chart_hashed = fp.host_chart_hashed
#        attrtab = fp.attrtab

        previous_file = fp.previous_file
        self.failUnlessEqual(previous_file, None)

        date = fp.date
        self.failUnlessEqual(type(date), str)

        mode = fp.mode
        self.failUnlessEqual(type(mode), int)

        default_offset = fp.default_offset
        self.failUnlessEqual(default_offset, 0)

        virtual_internal = fp.virtual_internal
        self.failUnlessEqual(virtual_internal, 0)

        system_version = fp.system_version

        major_order = fp.major_order
        self.failUnlessEqual(major_order, pdb.ROW_MAJOR_ORDER)
        fp.close()

    def test2a(self):
        '''PDBfile - write two doubles to file'''
        fp = pdb.PDBfile("testfile", "w");
        ref = [2.0, 3.0]
        fp.write("d2", ref)
        fp.close()

    def test2b(self):
        '''PDBfile - write two doubles to file with outtype'''
        fp = pdb.PDBfile("testfile", "w");
        ref = [2.0, 3.0]
        fp.write("d2", ref, outtype="float")
        fp.close()

    def test2c(self):
        '''PDBfile - write four doubles to file as a 2-d array'''
        fp = pdb.PDBfile("testfile", "w");
        ref = [2.0, 3.0, 4.0, 5.0]
        fp.write("d2", ref, ind=[2,2])
        fp.close()

    def test2d(self):
        '''PDBfile - write part of a list of doubles'''
        fp = pdb.PDBfile("testfile", "w");
        ref = [2.0, 3.0, 4.0, 5.0]
        fp.write("d2", ref, ind=[(1,),(2,3)])
        fp.close()

    def test3a(self):
        '''PDBfile - write two doubles to file (write_raw)'''
        fp = pdb.PDBfile("testfile", "w");
        d2 = struct.pack('dd', 2.0, 3.0)
        fp.write_raw("d2(2)", d2, "double")
        fp.close()

#--------------------------------------------------------------------------

    def test3b(self):
        '''PDBfile - write two doubles to file with explicit index (write_raw)'''
        fp = pdb.PDBfile("testfile", "w");
        d2 = struct.pack('dd', 2.0, 3.0)
        fp.write_raw("d2", d2, "double", [2])
        fp.close()

    def test3c(self):
        '''PDBfile - write two doubles to file with explicit lower,upper (write_raw)'''
        fp = pdb.PDBfile("testfile", "w");
        d2 = struct.pack('dd', 2.0, 3.0)
        fp.write_raw("d2", d2, "double", [(2,3)])
        fp.close()

    def test3d(self):
        '''PDBfile - write to a closed file'''
        fp = pdb.PDBfile("testfile", "w");
        fp.close()
        ref = (1.0, 2.0, 3.0, 4.0)
        self.failUnlessRaises(pdb.error, fp.write, "d2", ref)

    def test3e(self):
        '''PDBfile - write_raw to a closed file'''
        fp = pdb.PDBfile("testfile", "w");
        fp.close()
        d2 = struct.pack('dd', 2.0, 3.0)
        self.failUnlessRaises(pdb.error, fp.write_raw,
                              "d2(2)", d2, "double")

    def test4(self):
        '''PDBfile - read a non existent variable'''
        fp = pdb.PDBfile("testfile", "w");
        self.failUnlessRaises(pdb.error, fp.read, "d1")
        fp.close()

    def test4b(self):
        '''PDBfile - read from a closed file'''
        fp = pdb.PDBfile("testfile", "w");
        ref = (1.0, 2.0, 3.0, 4.0)
        fp.write("d2", ref)
        fp.close()
        self.failUnlessRaises(pdb.error, fp.read, "d2")

#--------------------------------------------------------------------------

    def test5a(self):
        '''PDBfile - read an array as pdbdata'''
        pdb.setform(array=pdb.AS_PDBDATA)
        fp = pdb.PDBfile("testfile", "w");
        ref = (1.0, 2.0, 3.0, 4.0)
        fp.write("d2", ref)
        d2 = fp.read("d2")
#        t = fp.defstr("double")
#        self.failUnlessEqual(type(d2), pdb.pdbdata)
        self.assert_(isinstance(d2, pdb.pdbdata))
        d3 = struct.unpack('dddd', str(d2))
        self.failUnlessEqual(d3, ref)
        fp.close()

    def test5b(self):
        '''PDBfile - read an array as a tuple'''
        pdb.setform(array=pdb.AS_TUPLE)
        fp = pdb.PDBfile("testfile", "w");
        ref = (1.0, 2.0, 3.0, 4.0)
        fp.write("d2", ref)
        d2 = fp.read("d2")
        self.failUnlessEqual(type(d2), tuple)
        self.failUnlessEqual(d2, ref)
        fp.close()

    def test5c(self):
        '''PDBfile - read an array as a list'''
        pdb.setform(array=pdb.AS_LIST)
        fp = pdb.PDBfile("testfile", "w");
        ref = [1.0, 2.0, 3.0, 4.0]
        fp.write("d2", ref)
        d2 = fp.read("d2")
        self.failUnlessEqual(type(d2), list)
        self.failUnlessEqual(d2, ref)
        fp.close()

    def test6a(self):
        '''PDBfile - partial read of variable'''
        fp = pdb.PDBfile("testfile", "w");
        ref = [1.0, 2.0, 3.0, 4.0]
        fp.write("d2", ref)
        d2 = fp.read("d2", ind=[(1,2)])
        self.failUnlessEqual(d2, ref[1:3])
        fp.close()

    def test6b(self):
        '''PDBfile - partial read of variable with stride'''
        fp = pdb.PDBfile("testfile", "w");
        ref = [1.0, 2.0, 3.0, 4.0]
        fp.write("d2", ref)
        d2 = fp.read("d2", ind=[(0,3,2)])
        self.failUnlessEqual(d2, [1.0, 3.0])
        fp.close()

    def test7a(self):
        '''PDBfile - write struct'''
        pass

    def test8(self):
        '''PDBfile - directories'''
        fp = pdb.PDBfile("testfile", "w")
        ls = fp.ls()
        self.failUnlessEqual(ls, ())

        pwd = fp.pwd()
        self.failUnlessEqual(pwd, '/')

        fp.mkdir('foo')
        ls = fp.ls()
        self.failUnlessEqual(ls, ('foo/',))

        fp.cd('foo')

        pwd = fp.pwd()
        self.failUnlessEqual(pwd, '/foo')

        fp.close()

    def test8b(self):
        '''PDBfile - directories, on closed file'''
        fp = pdb.PDBfile("testfile", "w")
        fp.close()
        self.failUnlessEqual(fp.cd('/'), 0)
        self.failUnlessEqual(fp.pwd(), None)
        self.failUnlessEqual(fp.mkdir('foo'), 0)
#        self.failUnlessRaises(pdb.error, fp.ls)

    def test9(self):
        '''PDBfile - links'''
        fp = pdb.PDBfile("testfile", "w");

        # link to non-existent variable
        self.failUnlessEqual(fp.ln('d2', 'd2link'), 0)

        # write a variable
        d2 = struct.pack('dd', 2.0, 3.0)
        fp.write_raw("d2", d2, "double", [2])

        # link to it
        fp.ln('d2', 'd2link')

        ls = fp.ls()

        # not sure order names are returned in,
        # so check each name
        self.failUnlessEqual(len(ls), 2)
        self.failUnless('d2' in ls)
        self.failUnless('d2link' in ls)

        # read by link
        d2 = fp.read("d2link")
        self.failUnlessEqual(d2, [2.0, 3.0])
        
        fp.close()
        

if __name__ == '__main__':
    unittest.main()
