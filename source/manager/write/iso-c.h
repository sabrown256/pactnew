#!/bin/csh -f
#
# ISO-C.H - write the ISO-C.H file
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#

unalias *

# put these in shell variables since
# prune-env will remove them as environment variables
set Log    = $1
set ScrDir = $2

eval `$ScrDir/prune-env pact`
set path = ( $ScrDir $path )
source $ScrDir/env-csh

dbget C_STD
dbget GLIBC_VERSION
dbget HostOS
dbget IncDir
dbget Sys

Separator $Log
Note $Log "   C_STD         = $C_STD"
Note $Log "   GLIBC_VERSION = $GLIBC_VERSION"
Note $Log "   HostOS        = $HostOS"
Note $Log "   IncDir        = $IncDir"
Note $Log "   Sys           = $Sys"

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

    NoteD $Log "   C Environment Configuration - iso-c.h"

    set STDOUT = $IncDir/iso-c.h

    flog $Log $RM $STDOUT
    flog $Log touch $STDOUT

    Note $STDOUT '/*'
    Note $STDOUT " * ISO-C.H - configuration defined by $Sys"
    Note $STDOUT ' *         - defines the ISO C, POSIX and XOPEN compliance expected'
    Note $STDOUT ' *'
    Note $STDOUT ' * Source Version: 3.0'
    Note $STDOUT ' * Software Release #: LLNL-CODE-422942'
    Note $STDOUT ' *'
    Note $STDOUT ' */'
    Note $STDOUT ''

    Note $STDOUT '#ifndef PCK_ISO_C'
    Note $STDOUT ''
    Note $STDOUT '#define PCK_ISO_C'
    Note $STDOUT ''

    if ($?GLIBC_VERSION == 1) then
       set inf = ( `echo $GLIBC_VERSION | sed 's/\./ /g'` )
    else
       set inf = ( 0 )
    endif

# some of the following info can be found in
#   /usr/include/features.h  on a Linux system
#   http://man.he.net/man7/feature_test_macros

# _POSIX_C_SOURCE
#     Value       Comment
#         1       defines conformance to POSIX.1-1990 and ISO C (1990)
#       2,>       adds conformance to POSIX.2-1992
# 199309L,>       adds conformance to POSIX.1b (real-time extensions)
# 199506L,>       adds conformance to POSIX.1c (threads)
# 200112L,>       adds conformance to POSIX.1-2001 base specification
#                 excluding the XSI extension
#                 glibc 2.3.3 and >
# 200809L,>       adds conformance to POSIX.1-2008 base specification
#                 excluding the XSI extension
#                 glibc 2.10 and >
#
    setenv POSIX_STD   200112
    if ($#inf >= 3) then
       setenv POSIX_STD   199506

# versions 3.0.0 or greater
       if ($inf[1] >= 3) then
          setenv POSIX_STD   200809

# versions 2.0.0 or greater
       else if ($inf[1] >= 2) then

# versions 2.10.0 or greater
          if ($inf[2] >= 10) then
             setenv POSIX_STD   200809

# versions 2.4.0 or greater
          else if ($inf[2] >= 4) then
             setenv POSIX_STD   200112

# versions 2.3.3 or greater
          else if (($inf[2] == 3) && ($inf[3] >= 3)) then
                setenv POSIX_STD   200112
             endif
          endif
       endif
    endif

# _XOPEN_SOURCE
#     Value       Comment
#       any       adds conformance to POSIX.1, POSIX.2, and XPG4
#     500,>       adds conformance to SUSv2 (UNIX 98)
#     600,>       adds conformance to SUSv3 (UNIX 03 - the
#                 POSIX.1-2001 base specification, the XSI extension, and C99)
#                 glibc 2.2 and >
#     700,>       adds conformance to SUSv4 (the POSIX.1-2008 base
#                 specification, the XSI extension)
#                 glibc 2.10 and >
#
    setenv XOPEN_STD   600
    if ($#inf >= 2) then
       setenv XOPEN_STD   500

# versions 3.0.0 or greater
       if ($inf[1] >= 3) then
          setenv XOPEN_STD   700

# versions 2.0.0 or greater
       else if ($inf[1] >= 2) then

# versions 2.10.0 or greater
          if ($inf[2] >= 10) then
             setenv XOPEN_STD   700

# versions 2.2.0 or greater
          else if ($inf[2] >= 2) then
             setenv XOPEN_STD   600
          endif
       endif
    endif

    switch ($C_STD)
       case C99:
            Note $STDOUT '#define ISO_C99'
            Note $STDOUT ''
            breaksw
       case X99:
            Note $STDOUT '#define ISO_C99'
            Note $STDOUT ''
            Note $STDOUT '#ifdef __GNUC__'
            Note $STDOUT ''
#            Note $STDOUT '# ifndef _POSIX_C_SOURCE'
#            Note $STDOUT "#  define _POSIX_C_SOURCE   ${POSIX_STD}L"
#            Note $STDOUT '# endif'
#            Note $STDOUT ''

            if (($HostOS != FreeBSD) && ($HostOS != SunOS)) then
               Note $STDOUT '# ifndef _XOPEN_SOURCE'
               Note $STDOUT "#  define _XOPEN_SOURCE     ${XOPEN_STD}"
               Note $STDOUT '# endif'
               Note $STDOUT ''
            endif

            Note $STDOUT '# ifndef _DARWIN_C_SOURCE'
            Note $STDOUT '#  define _DARWIN_C_SOURCE'
            Note $STDOUT '# endif'
            Note $STDOUT ''
            Note $STDOUT '#endif'
            Note $STDOUT ''
            breaksw
       case C89:
            Note $STDOUT '#define ISO_C89'
            Note $STDOUT ''
            breaksw
       case X89:
            Note $STDOUT '#define ISO_C89'
            Note $STDOUT ''
            Note $STDOUT '#ifdef __GNUC__'
            Note $STDOUT ''
#            Note $STDOUT '# ifndef _POSIX_C_SOURCE'
#            Note $STDOUT "#  define _POSIX_C_SOURCE   ${POSIX_STD}L"
#            Note $STDOUT '# endif'
#            Note $STDOUT ''
            Note $STDOUT '# ifndef _XOPEN_SOURCE'
            Note $STDOUT "#  define _XOPEN_SOURCE     ${XOPEN_STD}"
            Note $STDOUT '# endif'
            Note $STDOUT ''
            Note $STDOUT '# ifndef _DARWIN_C_SOURCE'
            Note $STDOUT '#  define _DARWIN_C_SOURCE'
            Note $STDOUT '# endif'
            Note $STDOUT ''
            Note $STDOUT '#endif'
            Note $STDOUT ''
            breaksw
        default:
            Note $STDOUT '/* system default ISO, POSIX, and XOPEN conformance */'
            Note $STDOUT ''
            breaksw
    endsw

    Note $STDOUT '#endif'
    Note $STDOUT ''

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

exit(0)

