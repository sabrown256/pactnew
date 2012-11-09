#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include cpyright.h
#

"""
Print Python's version number as major.minor
ignoring any patch version number
"""
import sys
ver = sys.version_info
print("%d.%d" % (ver[0], ver[1]))
