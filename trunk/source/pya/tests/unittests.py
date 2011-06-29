#!/usr/bin/env python
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

import unittest

import test_mod
import test_mem
import test_pdbdata
import test_pdbdata_write
import test_defstr
import test_file
import test_hash
import test_assoc
import test_numpy

suite = unittest.TestSuite()
for cls in (test_mod.Mod,
            test_mem.SCmem,
            test_mem.SCmemLeak,
            test_pdbdata.Pdbdata,
            test_pdbdata.Double,
            test_pdbdata.DoubleIndex,
            test_pdbdata.String,
            test_pdbdata.ClassObject,
#            test_pdbdata_unpack.Pdbdata,
#            test_pdbdata_unpack.Double,
#            test_pdbdata_unpack.String,
            test_pdbdata_write.DoubleIO,
            test_pdbdata_write.StringIO,
            test_pdbdata_write.ClassObjectIO,
            test_defstr.DefstrVif,
            test_defstr.DefstrFile,
            test_defstr.DefstrPdbdata,
            test_defstr.Memdes,
            test_file.File,
            test_hash.Hashtab,
            test_hash.HashIO,
            test_assoc.Assoc,
#            test_assoc.AssocIO,
            test_numpy.ArrayVif,
            test_numpy.ArrayFile,
            ):
    suite.addTest(unittest.makeSuite(cls))
unittest.TextTestRunner(verbosity=2).run(suite)
