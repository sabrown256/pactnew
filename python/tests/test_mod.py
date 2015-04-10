#!/usr/bin/env python
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

""" This module test functions

Test module level functions

"""

import pact_test.pdb as pdb
import unittest

class Mod(unittest.TestCase):
    def setUp(self):
      pass
      
    def tearDown(self):
      pass
    
    def testa(self):
        """Test flags for setform"""

        as_none = pdb.AS_NONE
        as_pdbdata = pdb.AS_PDBDATA
        as_object = pdb.AS_OBJECT
        as_tuple = pdb.AS_TUPLE
        as_list = pdb.AS_LIST
        as_dict = pdb.AS_DICT
        as_array = pdb.AS_ARRAY

        array, struct, scalar = pdb.setform()
        self.assertEqual(array, as_list)
        self.assertEqual(struct, as_tuple)
        self.assertEqual(scalar, as_object)

        # test array
        form = pdb.setform(array, struct, scalar)
        form1 = pdb.setform(as_pdbdata, as_none, as_none)
        self.assertEqual(form1, (as_pdbdata, struct, scalar))
        self.assertRaises(pdb.error, pdb.setform,
                              as_object, struct, scalar)
        form1 = pdb.setform(as_tuple, as_none, as_none)
        self.assertEqual(form1, (as_tuple, struct, scalar))
        form1 = pdb.setform(as_list, as_none, as_none)
        self.assertEqual(form1, (as_list, struct, scalar))
        self.assertRaises(pdb.error, pdb.setform,
                              as_dict, as_none, as_none)
        form1 = pdb.setform(as_array, as_none, as_none)
        self.assertEqual(form1, (as_array, struct, scalar))

        # test struct
        form = pdb.setform(array, struct, scalar)
        form1 = pdb.setform(as_none, as_pdbdata, as_none)
        self.assertEqual(form1, (array, as_pdbdata, scalar))
        self.assertRaises(pdb.error, pdb.setform,
                              as_none, as_object, as_none)
        form1 = pdb.setform(as_none, as_tuple, as_none)
        self.assertEqual(form1, (array, as_tuple, scalar))
        self.assertRaises(pdb.error, pdb.setform,
                              as_none, as_list, as_none)
        form1 = pdb.setform(as_none, as_dict, as_none)
        self.assertEqual(form1, (array, as_dict, scalar))
        self.assertRaises(pdb.error, pdb.setform,
                              as_none, as_array, as_none)

        # test scalar
        form = pdb.setform(array, struct, scalar)
        form1 = pdb.setform(as_none, as_none, as_pdbdata)
        self.assertEqual(form1, (array, struct, as_pdbdata))
        form1 = pdb.setform(as_none, as_none, as_object)
        self.assertEqual(form1, (array, struct, as_object))
        self.assertRaises(pdb.error, pdb.setform,
                              as_none, as_none, as_tuple)
        self.assertRaises(pdb.error, pdb.setform,
                              as_none, as_none, as_list)
        self.assertRaises(pdb.error, pdb.setform,
                              as_none, as_none, as_dict)
        self.assertRaises(pdb.error, pdb.setform,
                              as_none, as_none, as_array)


if __name__ == '__main__':
    unittest.main()


