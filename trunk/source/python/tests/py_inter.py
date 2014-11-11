#
# PY_INTER.PY
#

import sys

def to_bytes(s):
   if sys.version_info[0] >= 3:
      d = str(s)
   else:
      d = s
   return(d)

