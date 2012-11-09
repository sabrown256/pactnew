#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include cpyright.h
#

"""
Print path to Python's include directory
"""
from distutils.sysconfig import *
print(get_python_inc())
