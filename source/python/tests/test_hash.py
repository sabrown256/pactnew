#!/usr/bin/env python
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

""" This module test hasharrays

"""

import pact_test.pdb as pdb
import unittest
import test_leak


class Hasharr(test_leak.LeakVif):
    sampledict = {'foo':1, 'bar':2, 'baz':3}
      
    def samplehash(self):
        h = pdb.hasharr()
        for key, value in self.sampledict.items():
            h[key] = value
        return h
        
    def testa(self):
        '''create a hash table'''
        h = pdb.hasharr()
        self.failUnlessEqual(type(h), pdb.hasharr)
        self.failUnless(isinstance(h, pdb.hasharr))
        self.failUnless(isinstance(h, pdb.pdbdata))

    def testa1(self):
        '''create a hash table, then del'''
        h = pdb.hasharr()
        del h

    def testb(self):
        '''hasharr as mapping, scalar int'''
        h = pdb.hasharr()
        h['foo'] = 5

        k = h['foo']
        self.failUnlessEqual(type(k), int)
        self.failUnlessEqual(k, 5)

    def testb1(self):
        '''hasharr as mapping, string'''
        h = pdb.hasharr()
        h['foo'] = "I am string"

        k = h['foo']
        self.failUnlessEqual(type(k), str)
        self.failUnlessEqual(k, "I am string")

    def testc(self):
        '''hasharr as mapping, single item array'''
        h = pdb.hasharr()
        h['foo'] = [5]

        k = h['foo']
        self.failUnlessEqual(type(k), int)
        self.failUnlessEqual(k, 5)

    def testd(self):
        '''hasharr as mapping, array'''
        ref = [5, 6, 7]
        h = pdb.hasharr()
        h['foo'] = ref

        k = h['foo']
        self.failUnlessEqual(type(k), list)
        self.failUnlessEqual(k, ref)

#--------------------------------------------------------------------------

    def testd1(self):
        '''hasharr as mapping, pdbdata'''
        ref = [1., 2.]
        d = pdb.pdbdata(ref, 'double *')
        h = pdb.hasharr()
        h['foo'] = d

        k = h['foo']
        self.failUnlessEqual(type(k), list)
        self.failUnlessEqual(k, ref)

    def teste(self):
        '''hasharr - len'''
        h = self.samplehash()
        self.failUnlessEqual(len(h), len(self.sampledict))

    def testf(self):
        '''hasharr - keys'''
        h = self.samplehash()
        k = h.keys()
        self.failUnlessEqual(len(k), len(self.sampledict))
        for key in self.sampledict.keys():
            self.failUnless(key in k)

    def testg(self):
        '''hasharr - has_key'''
        h = self.samplehash()
        self.failUnless(h.has_key('foo'))
        self.failUnless(not h.has_key('FOO'))

    def testg1(self):
        '''hasharr - update from non-mapping'''
        h = pdb.hasharr()
        self.failUnlessRaises(AttributeError, h.update, 1)

    def testg2(self):
        '''hasharr - update from dict'''
        h = pdb.hasharr()
        h.update({"a":1, "b":2, "c":3})
        self.failUnlessEqual(len(h), 3)
        self.failUnlessEqual(h["a"], 1)
        self.failUnlessEqual(h["b"], 2)
        self.failUnlessEqual(h["c"], 3)

#--------------------------------------------------------------------------

    def testg3(self):
        '''hasharr - repr, empty hasharr'''
        h = pdb.hasharr()
        r = repr(h);
        self.failUnlessEqual(type(r), str);
        a = eval(r);
        self.failUnlessEqual(type(a), dict);

    def testg4(self):
        '''hasharr - repr'''
        h = pdb.hasharr()
        h.update(self.sampledict);
        r = repr(h);
        self.failUnlessEqual(type(r), str);
        a = eval(r);
        self.failUnlessEqual(type(a), dict);
        self.failUnlessEqual(len(a), len(self.sampledict))
        self.failUnlessEqual(a, self.sampledict)

#--------------------------------------------------------------------------

    def xtesth(self):
        # see HashIO
        '''hasharr - write to a file'''
        h = self.samplehash()
        fp = pdb.open("testdata", "w")
        fp.defstr("hasharr", pdb.vif.defstr("hasharr"))
        fp.defstr("haelem", pdb.vif.defstr("haelem"))
        fp.write("testh", h)
        fp.close()


class HashIO(test_leak.LeakFile):
    def samplehash(self):
        h = pdb.hasharr()
        h['foo'] = 1
        h['bar'] = 2
        h['baz'] = 3
        return h
        
    def setUp(self):
        test_leak.LeakFile.setUp(self)
        self.fp.defstr('hasharr', pdb.vif.defstr('hasharr'))
        self.fp.defstr('haelem', pdb.vif.defstr('haelem'))

    def tearDown(self):
        pdb.vif.close()
        test_leak.LeakFile.tearDown(self)
        
    def testa(self):
        pass
#        h = self.samplehash()
#        self.fp.write("testa", h)


if __name__ == '__main__':
    unittest.main()
    
