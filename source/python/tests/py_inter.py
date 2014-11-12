#
# PY_INTER.PY - stuff to help get between python versions
#
# include <cpyright.h>
#

import sys

def to_bytes(s):
   if sys.version_info[0] == 3:
      d = str(s)
   else:
      d = s
#   d = s
   return(d)

