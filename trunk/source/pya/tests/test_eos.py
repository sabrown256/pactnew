#!/usr/bin/env python
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

import pact.pdb as pdb

fp = pdb.PDBfile("leos", "r")
fp.cd('L10/material_info')
bulkmod = fp.read('bulkmod')

print bulkmod

fp.cd('../Cs')

comments = fp.read('comments')

print comments

pdb.setform(array=pdb.AS_LIST)
#fp.as_list = 1
temp = fp.read('temp')
print temp

pdb.setform(array=pdb.AS_ARRAY)
#fp.as_array = 1
temp = fp.read('temp')
print temp

