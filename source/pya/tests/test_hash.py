#!/usr/bin/env python
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

""" This module test hashtables

"""

import pact.pdb as pdb
import unittest
import test_leak


class Hashtab(test_leak.LeakVif):
    sampledict = {'foo':1, 'bar':2, 'baz':3}
      
    def samplehash(self):
        h = pdb.hashtab()
        for key, value in self.sampledict.items():
            h[key] = value
        return h
        
    def testa(self):
        '''create a hash table'''
        h = pdb.hashtab()
        self.failUnlessEqual(type(h), pdb.hashtab)
        self.failUnless(isinstance(h, pdb.hashtab))
        self.failUnless(isinstance(h, pdb.pdbdata))

    def testa1(self):
        '''create a hash table, then del'''
        h = pdb.hashtab()
        del h

    def testb(self):
        '''hashtab as mapping, scalar int'''
        h = pdb.hashtab()
        h['foo'] = 5

        k = h['foo']
        self.failUnlessEqual(type(k), int)
        self.failUnlessEqual(k, 5)

    def testb1(self):
        '''hashtab as mapping, string'''
        h = pdb.hashtab()
        h['foo'] = "I am string"

        k = h['foo']
        self.failUnlessEqual(type(k), str)
        self.failUnlessEqual(k, "I am string")

    def testc(self):
        '''hashtab as mapping, single item array'''
        h = pdb.hashtab()
        h['foo'] = [5]

        k = h['foo']
        self.failUnlessEqual(type(k), int)
        self.failUnlessEqual(k, 5)

    def testd(self):
        '''hashtab as mapping, array'''
        ref = [5, 6, 7]
        h = pdb.hashtab()
        h['foo'] = ref

        k = h['foo']
        self.failUnlessEqual(type(k), list)
        self.failUnlessEqual(k, ref)

#--------------------------------------------------------------------------

    def testd1(self):
        '''hashtab as mapping, pdbdata'''
        ref = [1., 2.]
        d = pdb.pdbdata(ref, 'double *')
        h = pdb.hashtab()
        h['foo'] = d

        k = h['foo']
        self.failUnlessEqual(type(k), list)
        self.failUnlessEqual(k, ref)

    def teste(self):
        '''hashtab - len'''
        h = self.samplehash()
        self.failUnlessEqual(len(h), len(self.sampledict))

    def testf(self):
        '''hashtab - keys'''
        h = self.samplehash()
        k = h.keys()
        self.failUnlessEqual(len(k), len(self.sampledict))
        for key in self.sampledict.keys():
            self.failUnless(key in k)

    def testg(self):
        '''hashtab - has_key'''
        h = self.samplehash()
        self.failUnless(h.has_key('foo'))
        self.failUnless(not h.has_key('FOO'))

    def testg1(self):
        '''hashtab - update from non-mapping'''
        h = pdb.hashtab()
        self.failUnlessRaises(AttributeError, h.update, 1)

    def testg2(self):
        '''hashtab - update from dict'''
        h = pdb.hashtab()
        h.update({"a":1, "b":2, "c":3})
        self.failUnlessEqual(len(h), 3)
        self.failUnlessEqual(h["a"], 1)
        self.failUnlessEqual(h["b"], 2)
        self.failUnlessEqual(h["c"], 3)

#--------------------------------------------------------------------------

    def testg3(self):
        '''hashtab - repr, empty hashtab'''
        h = pdb.hashtab()
        r = repr(h);
        self.failUnlessEqual(type(r), str);
        a = eval(r);
        self.failUnlessEqual(type(a), dict);

    def testg4(self):
        '''hashtab - repr'''
        h = pdb.hashtab()
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
        '''hashtab - write to a file'''
        h = self.samplehash()
        fp = pdb.open("testdata", "w")
        fp.defstr("HASHTAB", pdb.vif.defstr("HASHTAB"))
        fp.defstr("hashel", pdb.vif.defstr("hashel"))
        fp.write("testh", h)
        fp.close()


class HashIO(test_leak.LeakFile):
    def samplehash(self):
        h = pdb.hashtab()
        h['foo'] = 1
        h['bar'] = 2
        h['baz'] = 3
        return h
        
    def setUp(self):
        test_leak.LeakFile.setUp(self)
        self.fp.defstr('HASHTAB', pdb.vif.defstr('HASHTAB'))
        self.fp.defstr('hashel', pdb.vif.defstr('hashel'))

    def tearDown(self):
        pdb.vif.close()
        test_leak.LeakFile.tearDown(self)
        
    def testa(self):
        pass
#        h = self.samplehash()
#        self.fp.write("testa", h)


if __name__ == '__main__':
    unittest.main()
    
