#!/usr/bin/env python
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

""" This module tests

  defstr ctor

"""

import pact_2.pdb as pdb
import unittest
import struct


class Meta(unittest.TestCase):
      
#    def testa(self):
#        """  """
#        ref = 4.0
#        d2 = struct.pack('d', ref)
#        ctor = pdb.vif.defstr("double")
#        d = ctor(ref)
#        self.failUnlessEqual(str(d2), str(d))
#        o = d.unpack()
#        o = repr(d)
#        self.failUnlessEqual(o, ref)

    def testb(self):
        """  """
#        d2 = struct.pack('id', 3, 4.0)
        
        ctor = pdb.vif.defstr("two", ('int i', 'float j'))
        print ctor
        print type(ctor)
        print dir(ctor)
        print ctor.__class__
        d = ctor((3, 4.0))
        print d
        self.failUnlessEqual(d.i, 3)
        self.failUnlessEqual(d.j, 4.0)


if __name__ == '__main__':
    unittest.main()
