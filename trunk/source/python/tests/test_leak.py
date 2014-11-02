#!/usr/bin/env python
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

""" Add SCORE memory checking to tests
"""

# if Py_DEBUG is set, then sys.gettotalrefcount exists

import pact_test.pdb as pdb
#import gc
import unittest

as_array, as_struct, as_scalar = pdb.setform()


class Leak(unittest.TestCase):
    lev  = 2
    leak = 0
#    leak = 1
    
    def setUp(self):
        pdb.zero_space(3)  # zero on free only
        if self.leak:
            self.glnnmb, msg = pdb.mem_monitor(-1, self.lev, "glob")
            del msg
        # reset to defaults
        pdb.setform(as_array, as_struct, as_scalar)
#        gc.set_debug(gc.DEBUG_LEAK)

        
    def tearDown(self):
#        gc.collect()
#        gc.set_debug(0)
        pdb.mem_chk(3)
        if self.leak:
            self.glnnmb, msg = pdb.mem_monitor(self.glnnmb, self.lev, "glob")
            self.failUnlessEqual(msg, None)


class LeakVif(Leak):
    def setUp(self):
        Leak.setUp(self)
        self.fp = pdb.vif
        
    def tearDown(self):
        self.fp.close()
        Leak.tearDown(self)


class LeakFile(Leak):
    def setUp(self):
        Leak.setUp(self)
        self.fp = pdb.open("testfile", "w")
        
    def tearDown(self):
        self.fp.close()
        Leak.tearDown(self)

