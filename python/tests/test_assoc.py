#!/usr/bin/env python
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

""" This module tests
 
 association lists / dictionaries

"""

import pact_test.pdb as pdb
import unittest
import test_leak


class Assoc(test_leak.LeakVif):
    sampledict = {'foo':1, 'bar':2, 'baz':3}
    
    def sampleassoc(self):
        # add in fixed, known order
        h = pdb.assoc()
        for key in ('foo', 'bar', 'baz'):
            h[key] = self.sampledict[key]
#        for key, value in self.sampledict.items():
#            h[key] = value
        return h
    
            
    def testa(self):
        '''create an assoc'''
        h = pdb.assoc()
        self.assertEqual(type(h), pdb.assoc)
        self.assertTrue(isinstance(h, pdb.assoc))
#        self.assertTrue(isinstance(h, pdb.pdbdata))

    def testa1(self):
        '''create a assoc, then del'''
        h = pdb.assoc()
        del h

    def testb(self):
        '''assoc as mapping, scalar int'''
        h = pdb.assoc()
        h['foo'] = 5

        k = h['foo']
        self.assertEqual(type(k), int)
        self.assertEqual(k, 5)

    def testb11(self):
        '''assoc as mapping, scalar int, replace index'''
        h = pdb.assoc()
        h['foo'] = 5
        self.assertEqual(len(h), 1)
        h['foo'] = 6
        self.assertEqual(len(h), 1)

        k = h['foo']
        self.assertEqual(type(k), int)
        self.assertEqual(k, 6)

    def testb1(self):
        '''assoc as mapping, string'''
        h = pdb.assoc()
        h['foo'] = "I am string"

        k = h['foo']
        self.assertEqual(type(k), str)
        self.assertEqual(k, "I am string")

    def testc(self):
        '''assoc as mapping, single item array'''
        h = pdb.assoc()
        h['foo'] = [5]

        k = h['foo']
        self.assertEqual(type(k), int)
        self.assertEqual(k, 5)

#--------------------------------------------------------------------------

    def testd(self):
        '''assoc as mapping, array'''
        ref = [5, 6, 7]
        h = pdb.assoc()
        h['foo'] = ref

        k = h['foo']
        self.assertEqual(type(k), list)
        self.assertEqual(k, ref)

    def testd1(self):
        '''assoc as mapping, pdbdata'''
        ref = [1., 2.]
        d = pdb.pdbdata(ref, 'double *')
        h = pdb.assoc()
        h['foo'] = d

        k = h['foo']
        self.assertEqual(type(k), list)
        self.assertEqual(k, ref)


    def teste(self):
        '''assoc - len'''
        h = self.sampleassoc()
        self.assertEqual(len(h), len(self.sampledict))

    def testf(self):
        '''assoc - keys'''
        h = self.sampleassoc()
        k = h.keys()
        self.assertEqual(len(k), len(self.sampledict))
        for key in self.sampledict.keys():
            self.assertTrue(key in k)

    def testf1(self):
        '''assoc - items'''
        h = self.sampleassoc()
        k = h.items()
        self.assertTrue(isinstance(k, list))
        self.assertEqual(len(k), len(self.sampledict))
        self.assertTrue(isinstance(k[0], tuple))
        self.assertTrue(isinstance(k[1], tuple))
        self.assertTrue(isinstance(k[1], tuple))
        self.assertEqual(k[0][0], 'baz')
        self.assertEqual(k[0][1], 3)
        self.assertEqual(k[1][0], 'bar')
        self.assertEqual(k[1][1], 2)
        self.assertEqual(k[2][0], 'foo')
        self.assertEqual(k[2][1], 1)

    def testg(self):
        '''assoc - has_key'''
        h = self.sampleassoc()
        self.assertTrue(h.has_key('foo'))
        self.assertTrue(not h.has_key('FOO'))

#--------------------------------------------------------------------------

    def testg1(self):
        '''assoc - update from non-mapping'''
        h = pdb.assoc()
        self.assertRaises(AttributeError, h.update, 1)

    def testg2(self):
        '''assoc - update from dict'''
        h = pdb.assoc()
        h.update({"a":1, "b":2, "c":3})
        self.assertEqual(len(h), 3)
        self.assertEqual(h["a"], 1)
        self.assertEqual(h["b"], 2)
        self.assertEqual(h["c"], 3)

    def testg3(self):
        '''assoc - repr, empty assoc'''
        h = pdb.assoc()
        r = repr(h);
        self.assertEqual(type(r), str);
        a = eval(r);
        self.assertEqual(type(a), dict);

    def testg4(self):
        '''assoc - repr'''
        h = pdb.assoc()
        h.update(self.sampledict);
        r = repr(h);
        self.assertEqual(type(r), str);
        a = eval(r);
        self.assertEqual(type(a), dict);
        self.assertEqual(len(a), len(self.sampledict))
        self.assertEqual(a, self.sampledict)

    def xtesth(self):
        # see HashIO
        '''assoc - write to a file'''
        h = self.sampleassoc()
        fp = pdb.open("testdata", "w")
        fp.defstr("ASSOC", pdb.vif.defstr("ASSOC"))
        fp.defstr("hashel", pdb.vif.defstr("hashel"))
        fp.write("testh", h)
        fp.close()


#class AssocIO(test_leak.LeakFile):
class AssocIO:
    def sampleassoc(self):
        h = pdb.assoc()
        h['foo'] = 1
        h['bar'] = 2
        h['baz'] = 3
        return h
        
    def setUp(self):
        test_leak.LeakFile.setUp(self)
        self.fp.defstr('ASSOC', pdb.vif.defstr('ASSOC'))
        self.fp.defstr('hashel', pdb.vif.defstr('hashel'))

    def tearDown(self):
        pdb.vif.close()
        test_leak.LeakFile.tearDown(self)
        
    def testa(self):
        pass
#        h = self.sampleassoc()
#        self.fp.write("testa", h)


if __name__ == '__main__':
    unittest.main()

