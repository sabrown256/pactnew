#!/usr/bin/env python
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

import unittest

import test_numpy

suite = unittest.TestSuite()
for cls in (test_numpy.ArrayVif,
            test_numpy.ArrayFile,
            ):
    suite.addTest(unittest.makeSuite(cls))
unittest.TextTestRunner(verbosity=2).run(suite)
