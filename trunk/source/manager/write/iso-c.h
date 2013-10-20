#!/bin/csh -f
#
# ISO-C.H - write the ISO-C.H file
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#

set Me = $0
source $Me:h/common

Note $Log "----- write/iso-c.h -----"
Note $Log "Write: iso-c.h"
Note $Log ""

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

Separator $Log

NoteD $Log "   C Standards Configuration - iso-c.h"
Note $Log ""

dbmget $Log AF_VERSION_GLIBC^  \
            HSY_OS_Name^       \
            IncDir^            \
            PSY_Cfg^           \
            STD_C^             \
            STD_POSIX^         \
            STD_XOPEN

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

    set STDOUT = $IncDir/iso-c.h

    flog $Log $RM $STDOUT
    flog $Log touch $STDOUT

    Note $STDOUT '/*'
    Note $STDOUT " * ISO-C.H - configuration defined by $PSY_Cfg"
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

    if ($?AF_VERSION_GLIBC == 1) then
       set inf = ( `echo $AF_VERSION_GLIBC | sed 's/\./ /g'` )
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
# default is STD_POSIX   200112
#
    if ($#inf >= 3) then
       setenv STD_POSIX   199506

# versions 3.0.0 or greater
       if ($inf[1] >= 3) then
          setenv STD_POSIX   200809

# versions 2.0.0 or greater
       else if ($inf[1] >= 2) then

# versions 2.10.0 or greater
          if ($inf[2] >= 10) then
             setenv STD_POSIX   200809

# versions 2.4.0 or greater
          else if ($inf[2] >= 4) then
             setenv STD_POSIX   200112

# versions 2.3.3 or greater
          else if (($inf[2] == 3) && ($inf[3] >= 3)) then
                setenv STD_POSIX   200112
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
# default is STD_XOPEN   600
#
    if ($#inf >= 2) then
       setenv STD_XOPEN   500

# versions 3.0.0 or greater
       if ($inf[1] >= 3) then
          setenv STD_XOPEN   700

# versions 2.0.0 or greater
       else if ($inf[1] >= 2) then

# versions 2.10.0 or greater
          if ($inf[2] >= 10) then
             setenv STD_XOPEN   700

# versions 2.2.0 or greater
          else if ($inf[2] >= 2) then
             setenv STD_XOPEN   600
          endif
       endif
    endif

    switch ($STD_C)
       case C11:
            Note $STDOUT '#define ISO_C11'
            Note $STDOUT ''
            breaksw
       case X11:
            Note $STDOUT '#define ISO_C11'
            Note $STDOUT ''
            Note $STDOUT '#ifdef __GNUC__'
            Note $STDOUT ''
#            Note $STDOUT '# ifndef _POSIX_C_SOURCE'
#            Note $STDOUT "#  define _POSIX_C_SOURCE   ${STD_POSIX}L"
#            Note $STDOUT '# endif'
#            Note $STDOUT ''

            if (($HSY_OS_Name != FreeBSD) && ($HSY_OS_Name != SunOS)) then
               Note $STDOUT '# ifndef _XOPEN_SOURCE'
               Note $STDOUT "#  define _XOPEN_SOURCE     ${STD_XOPEN}"
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
#            Note $STDOUT "#  define _POSIX_C_SOURCE   ${STD_POSIX}L"
#            Note $STDOUT '# endif'
#            Note $STDOUT ''

            if (($HSY_OS_Name != FreeBSD) && ($HSY_OS_Name != SunOS)) then
               Note $STDOUT '# ifndef _XOPEN_SOURCE'
               Note $STDOUT "#  define _XOPEN_SOURCE     ${STD_XOPEN}"
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
#            Note $STDOUT "#  define _POSIX_C_SOURCE   ${STD_POSIX}L"
#            Note $STDOUT '# endif'
#            Note $STDOUT ''
            Note $STDOUT '# ifndef _XOPEN_SOURCE'
            Note $STDOUT "#  define _XOPEN_SOURCE     ${STD_XOPEN}"
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

