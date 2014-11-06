#!/usr/bin/env python
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

""" This module tests

  defstr ctor

"""

import pact_test.pdb as pdb
import unittest
import struct


class Meta(unittest.TestCase):
      
#    def testa(self):
#        """  """
#        ref = 4.0
#        d2 = struct.pack('d', ref)
#        ctor = pdb.vif.defstr("double")
#        d = ctor(ref)
#        self.assertEqual(str(d2), str(d))
#        o = d.unpack()
#        o = repr(d)
#        self.assertEqual(o, ref)

    def testb(self):
        """  """
#        d2 = struct.pack('id', 3, 4.0)
        
        ctor = pdb.vif.defstr("two", ('int i', 'float j'))
        print(ctor)
        print(type(ctor))
        print(dir(ctor))
        print(ctor.__class__)
        d = ctor((3, 4.0))
        print(d)
        self.assertEqual(d.i, 3)
        self.assertEqual(d.j, 4.0)


if __name__ == '__main__':
    unittest.main()
