#!/usr/bin/env python
#
# Setup script for building the PACT extension to Python.
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
#
# To use:
#     run this to build PACT Python:
#       python setup.py build
#     or run this to build and install it
#       python setup.py install

import sys
import os
from re import search
from getopt import getopt 
from string import split
from distutils.core import setup, Extension

debug = False

def log(string):
  global debug

  if debug:
    print(str(string))
    
# Default assumption: dsys built PACT    
autotool = False 
dsys     = True  

# Parse any argv flags
# getopt does not work so well in setup.py scripts; hence, this hack.
for arg in sys.argv[1:]:
  log(arg)
  
  if search('-a', arg) or search('--autotool', arg):
    autotool = True
    dsys     = False
    sys.argv.remove(arg)
    
  if search('-d', arg) or search('--dsys', arg):
    dsys     = True
    autotool = False
    sys.argv.remove(arg)
    
  if search('-D', arg) or search('--debug', arg):
    debug = True
    sys.argv.remove(arg)

    
# Environment setup
    
# PACTHOME
if dsys:
  # In dsys, SCHEME is set by the $EtcDir/env-pact.csh script created by pact-config.
  SCHEME = os.environ.get('SCHEME')

  if SCHEME is not None:
    PACTHOME = os.path.split(SCHEME)[0] 
  else:
    PACTHOME = os.environ.get('PACT') # , '/usr/gapps/pactnew')
  
  if not os.path.exists(PACTHOME):
    print("%s does not exist" % PACTHOME)
    print("Set environmental variable PACT")
    sys.exit(1)
  
# GTK_HOME 
GTK_HOME = None
#GTK_HOME = '/g/g17/taylor/local-dec'

if autotool:
  fd = os.popen('grep ROOTDIR Makefile | sed -e \'s|ROOTDIR = ||\'')
  rootdir = split(fd.read(), '\n')[0] 
  fd.close()

  fd = os.popen('grep package Makefile | sed -e \'s|package = ||\'')
  package = split(fd.read(), '\n')[0] 
  fd.close()

  fd = os.popen('grep top_srcdir Makefile | sed -e \'s|top_srcdir = ||\'')
  top_srcdir = split(fd.read(), '\n')[0] 
  fd.close()

  fd = os.popen('grep top_builddir Makefile | sed -e \'s|top_builddir = ||\'')
  top_builddir = split(fd.read(), '\n')[0] 
  fd.close()

  sourcedir = os.path.join(rootdir, package, top_srcdir)
  builddir = os.path.join(rootdir, package, top_builddir)

  log('sourcedir = ' + str(sourcedir))
  log('builddir = ' + str(builddir))

# GRAPHICS LIBS
libdirs = []
MDGLib = []
  
if dsys:
  libdirs.append(os.path.join(PACTHOME, 'lib'))
  
  fd = os.popen(PACTHOME + '/bin/pact -info MDGLib')
  
  # must strip off the lib prefix and the suffix.
  # /usr/local/lib/libfoo.a becomes /usr/local/lib/foo
  for name in split(split(fd.read(), '\n')[0], ' '):
    if name.startswith('-L'):
      libdirs.append(name[2:])
    elif name.startswith('-l'):
      MDGLib.append(name[2:])
    else:
      (dir, file) = os.path.split(name)
      MDGLib.append(os.path.join(dir,split(file[3:], '.')[0]))

  fd.close()

if autotool:
  fd = os.popen('uname -a')
  uname = split(fd.read(), '\n')[0]
  fd.close()
  
  # HACK: AIX needs this -- need a better way
  if uname.find('AIX') != -1:
    MDGLib.append('X11')
    
  # HACK: Darwin needs this -- need a better way
  if uname.find('Darwin') != -1:
    MDGLib.append('X11')  
    libdirs.append('/usr/X11R6/lib')
    
if autotool:
  libdirs.append(os.path.join(builddir, 'score/.libs'))
  libdirs.append(os.path.join(builddir, 'pml/.libs'))
  libdirs.append(os.path.join(builddir, 'pdb/.libs'))
  libdirs.append(os.path.join(builddir, 'pgs/.libs'))
  
# INCLUDES
if dsys:  
  incdirs = [os.path.join(PACTHOME, 'include')]

if autotool:
  incdirs = []
  incdirs.append(os.path.join(builddir,  'config'))
  incdirs.append(os.path.join(sourcedir, 'score'))
  incdirs.append(os.path.join(sourcedir, 'pml'))
  incdirs.append(os.path.join(sourcedir, 'pdb'))
  incdirs.append(os.path.join(sourcedir, 'pgs'))
   
# MACROS
define_macros = []

if dsys:
  fd = os.popen(PACTHOME + '/bin/pact -info CFLAGS')

  for name in split(split(fd.read(), '\n')[0], ' '):
    if name.startswith('-D'):
      nn = name[2:]
      
      if nn == '_LARGE_FILES':
        define_macros.append((name[2:], None))
        
  fd.close()

if autotool:
  fd = os.popen('grep \'_LARGE_FILES\' Makefile')
  macro = split(fd.read(), '\n')[0]
  
  if macro == '_LARGE_FILES':
    define_macros.append((macro, None))
  
  fd.close()
   
# GTK  
if GTK_HOME:
  fd = os.popen(GTK_HOME + '/bin/gtk-config --cflags --libs')

  # must strip off -I, -L and -l
  args = fd.readline()[:-1].split(' ')
  gtk_include_dirs = [os.path.join(GTK_HOME,'include')] + \
                     [s[2:] for s in args]

  args = fd.readline()[:-1].split(' ')
  gtk_library_dirs = [s[2:] for s in args if s[:2] == '-L']
  gtk_libraries    = [s[2:] for s in args if s[:2] == '-l']

  fd.close()
else:
  gtk_include_dirs = []
  gtk_library_dirs = []
  gtk_libraries    = []


log('define_macros      = ' + str(define_macros))
log('incdirs            = ' + str(incdirs))
log('libdirs            = ' + str(libdirs))
log('MDGLib = ' + str(MDGLib))
log('gtk_include_dirs   = ' + str(gtk_include_dirs))
log('gtk_library_dirs   = ' + str(gtk_library_dirs))
log('gtk_libraries      = ' + str(gtk_libraries))


PDB_MOD_NAME = '_pdb'

setup(name = 'pypact',
       version = '2.0a6',
       description = 'PACT Extension to Python',
       maintainer = 'Lee Taylor',
       maintainer_email = 'ltaylor@llnl.gov',
       url = 'http://pact.llnl.gov',

       packages = ['pact'],

       include_dirs = incdirs,
       
       options = {'build_ext':
                  {'library_dirs': libdirs,
                   },
                  },

       ext_modules = [
                      Extension(PDB_MOD_NAME,
                                ['pdbmodule.c',
                                 'pdbPDBfile.c',
                                 'pdbmemdes.c',
                                 'pdbdefstr.c',
                                 'pdbpdbdata.c',
                                 'pdbhashtab.c',
                                 'pdbassoc.c',
                                 'pmlfield.c',
                                 'pmlset.c',
                                 'pmltopology.c',
                                 'pmlmapping.c',
                                 'pputil.c',
                                 'pptypes.c',
                                 'pparrays.c',
                                 'ppdefstr.c',
                                 'pphashtab.c',
                                 'ppassoc.c',
                                 'pppdbr.c',
                                 'pypdbr.c',
                                 'pypdbl.c',
                                 'pypdbd.c',
                                 'struct.c',
                                 'pppml.c',
                                 ],
                                define_macros=define_macros,
                                include_dirs=['.'],
                                libraries=['pdb', 'pml', 'score'],
                                ),

                      Extension('_pgs',
                                ['pgsmodule.c',
                                 'pgsdevice.c',
##                                 'pgsevent.c',
                                 'pgsgraph.c',
                                 'pgsimage.c',
                                 'pgspalette.c',
##                                 'pgsinfo.c',
##                                 'pgsiob.c',
##                                 'pgscallback.c',
##                                 'pgsutil.c'
                                 'utilpgs.c',
                                 
                                 # list from pdb
                                 'pdbPDBfile.c',
                                 'pdbmemdes.c',
                                 'pdbdefstr.c',
                                 'pdbpdbdata.c',
                                 'pdbhashtab.c',
                                 'pdbassoc.c',
                                 'pmlfield.c',
                                 'pmltopology.c',
                                 'pmlset.c',
                                 'pmlmapping.c',
                                 'pputil.c',
                                 'pptypes.c',
                                 'pparrays.c',
                                 'ppdefstr.c',
                                 'pphashtab.c',
                                 'ppassoc.c',
                                 'pppdbr.c',
                                 'pypdbr.c',
                                 'pypdbl.c',
                                 'pypdbd.c',
                                 'struct.c',
                                 'pppml.c',
                                 
                                 ],
                                include_dirs=['.'],
                                libraries=['pgs', 'pdb', 'pml', 'score'] + MDGLib
###                                extra_objects=[MDGLib]
###                                extra_link_args=[MDGLib]
                                ),


#                      Extension('_gtkpgs',
#                                ['src/gtk/gtkpgsmodule.c',
#                                 'src/gtk/gtkpgs.c'],
#                                include_dirs=['src/pgs', 'src/score', 'src/gtk'] + \
#                                             gtk_include_dirs
##                                library_dirs=gtk_library_dirs,
##                                libraries=gtk_libraries
#                        ),

                      ]
       )
