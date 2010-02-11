#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include cpyright.h
#

"""
Print path to Python's config directory
"""
from distutils.sysconfig import *
import os.path as path
dir = get_python_lib()
print path.join(path.split(dir)[0], 'config')
