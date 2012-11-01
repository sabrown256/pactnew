#!/usr/bin/env python
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

""" This module test memory functions

"""

import pact_2.pdb as pdb
import unittest

class SCmem(unittest.TestCase):
      
    def tearDown(self):
        pdb.mem_chk(3)

    def testa(self):
        """memory - test leak detection"""
        # This will leak
        name = 'testa'
        glnnmb, msg = pdb.mem_monitor(-1, -1, "glob");
        a = pdb.alloc(2, 8, name)
        b1, msg = pdb.mem_monitor(glnnmb, -1, "glob");
        self.assert_(msg is not None)
        self.assert_(msg.startswith("LEAKED"))

    def testb(self):
        """memory - automatic deallocation of memory"""
        name = 'testb'
        glnnmb, msg = pdb.mem_monitor(-1, -1, "glob");
        def foo():
            a = pdb.alloc(2, 8, name)
        foo()
        b1, msg = pdb.mem_monitor(glnnmb, -1, "glob");
        self.assert_(msg is None)

    def testc(self):
        """memory - mark"""
        name = 'testc'
        def foo():
            a = pdb.alloc(2, 8, name)
            pdb.mark(a, 1)
            glnnmb, msg = pdb.mem_monitor(-1, -1, "glob");
            return glnnmb
        glnnmb = foo()
        # We will have the same amout of memory now as after
        # allocating the block.  The free called with python
        # garbage collects a will not release the memory
        # since Score's reference count was incremented
        b1, msg = pdb.mem_monitor(glnnmb, -1, "glob");
        self.assert_(msg is None)

    def testd(self):
        """memory - test allocation"""
        name = 'testd'
        a = pdb.alloc(2, 8, name)
        #  pdb.mem_print(a)
        self.failUnlessEqual(pdb.mem_lookup(a), name)
        self.assert_(pdb.is_score_ptr(a))
        self.failUnlessEqual(pdb.arrlen(a), 16)

    def teste(self):
        """memory - test allocation non-pointer"""
        self.failUnlessRaises(TypeError, pdb.cfree, 1)

#--------------------------------------------------------------------------


class SCmemLeak(unittest.TestCase):
    def setUp(self):
        self.glnnmb, msg = pdb.mem_monitor(-1, -2, "glob");

    def tearDown(self):
        b1, msg = pdb.mem_monitor(self.glnnmb, -2, "glob");

    def testa(self):
        """memory - test leak detection"""
        name = 'testa'
        a = pdb.alloc(2, 8, name)



if __name__ == '__main__':
    unittest.main()
