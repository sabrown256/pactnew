#!/bin/csh -f
#
# SCCONFIG.H - write the SCCONFIG.H file
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

eval `$ScrDir/prune-env -e MngDir pact`
set path = ( $ScrDir $path )
source $ScrDir/env-csh

dbget Arch
dbget BFD_Version
dbget CC_Exe
dbget CEFile
dbget CPU
dbget DEFAULT_SHELL
dbget DP_Lib
dbget DP_Inc
dbget FC_ID_CASE
dbget FC_ID_UNDERSCORE
dbget FFIntPtrDiffer
dbget FilterDir
dbget FPU
dbget GETSOCKOPT_TYPE
dbget HAVE_BFD
dbget HaveINLINE
dbget HAVE_OPENMP
dbget HAVE_OPENMPI
dbget HAVE_SOCKETS_P
dbget HaveTRACKER
dbget HaveVACOPY
dbget HaveVALIST
dbget HostOS
dbget HostOSRel
dbget IncDir
dbget IPCSupport
dbget Linking
dbget LONG64
dbget MACRev
dbget MPIAllStdin
dbget MPIGoodIO
dbget NO_LONG_LONG
dbget NEED_ALT_LARGE_FILE
dbget PACT_CC_FAMILY
dbget PACT_CC_VERSION
dbget PACT_SO_CACHE
dbget PTHREAD_POSIX
dbget OSType
dbget Sys
dbget TRACKERExe

dbget MYSQL_SO
dbget SQLITE3_SO
dbget HDF5_SO

Separator $Log

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

    NoteD $Log "   C Environment Configuration - scconfig.h"

    set STDOUT = $IncDir/scconfig.h

    flog $Log $RM $STDOUT
    flog $Log touch $STDOUT

    Note $STDOUT "/*"
    Note $STDOUT " * SCCONFIG.H - configuration defined by $Sys"
    Note $STDOUT " *            - defines the following configuration parameters for PACT"
    Note $STDOUT " *"
    Note $STDOUT " * Source Version: 3.0"
    Note $STDOUT " * Software Release #: LLNL-CODE-422942"
    Note $STDOUT " *"
    Note $STDOUT " */"
    Note $STDOUT ""

    Note $STDOUT "#ifndef PCK_CONFIGURATION"
    Note $STDOUT ""
    Note $STDOUT "#define PCK_CONFIGURATION"
    Note $STDOUT ""

    set QUOTE = \"
    set THE_DATE = `cat .pact-version`
    Note $STDOUT '#define PACT_VERSION        '$QUOTE$THE_DATE$QUOTE

    Note $STDOUT "#define CPU_TYPE            $CPU"
    Note $STDOUT "#define FPU_TYPE            $FPU"
    Note $STDOUT '#define SYSTEM_ID           '$QUOTE$Arch$QUOTE
    if ("$MACRev" != "") then
       Note $STDOUT "#define MACOSX_VERSION      $MACRev"
    endif

    Note $STDOUT '#define USE_COMPILER        '$QUOTE$CC_Exe$QUOTE

    Note $STDOUT '#define COMPILER_'$PACT_CC_FAMILY
    Note $STDOUT '#define COMPILER_VERSION    '${QUOTE}$PACT_CC_VERSION${QUOTE}

    Note $STDOUT '#define DEFAULT_SHELL       '$QUOTE$DEFAULT_SHELL$QUOTE

    if ("$FilterDir" != "") then
       set FiltFile = $FilterDir/$Arch
       Note $STDOUT '#define MAKE_FILTER_FILE    '$QUOTE$FiltFile$QUOTE
    endif

    if ("$PACT_SO_CACHE" != "") then
       Note $STDOUT '#define PACT_SO_CACHE       '$QUOTE$PACT_SO_CACHE$QUOTE
    endif

    Note $STDOUT ""

    Note $STDOUT "#define ANSI"
    Note $STDOUT "#define byte void"
    Note $STDOUT "#undef CONST"
    Note $STDOUT "#define CONST const"
    Note $STDOUT "#undef SIGNED"
    Note $STDOUT "#define SIGNED signed"
    Note $STDOUT "#undef INLINE"
    if ($HaveINLINE == TRUE) then
       Note $STDOUT "#define INLINE inline"
    else
       Note $STDOUT "#define INLINE"
    endif

    if ($NO_LONG_LONG == TRUE) then
       Note $STDOUT "#define NO_LONG_LONG"
    endif

    if ("$LONG64" != "") then
       Note $STDOUT "#define LONG64"
    endif

    if ("$FFIntPtrDiffer" != "") then
       Note $STDOUT "#define $FFIntPtrDiffer"
    endif

    if (-e $IncDir/Finteger.bytes) then
       Note $STDOUT `cat $IncDir/Finteger.bytes`
       $RM $IncDir/Finteger.bytes
    endif

    Note $STDOUT ""

# emit the HAVE/USE flags - no value
    set lhave = ""
    set lhave = ( $lhave HaveFLOAT16         HAVE_ANSI_FLOAT16 )
    set lhave = ( $lhave HaveCOMPLEX         HAVE_ANSI_C9X_COMPLEX )
    set lhave = ( $lhave HaveANSIFenv        HAVE_ANSI_C9X_FENV )
    set lhave = ( $lhave HaveGNUFenv         HAVE_GNU_FENV )
    set lhave = ( $lhave HaveVACOPY          HAVE_VA_COPY )
    set lhave = ( $lhave HavePython          HAVE_PYTHON )
    set lhave = ( $lhave HavePythonNumeric   HAVE_PYTHON_NUMERIC )
    set lhave = ( $lhave HavePythonNumpy     HAVE_PYTHON_NUMPY )
    set lhave = ( $lhave HAVE_FLEX_SCANNER   USE_FLEX )
    set lhave = ( $lhave HAVE_READLINE       none )
    set lhave = ( $lhave HAVE_HDF5           none )
    set lhave = ( $lhave HAVE_SQL            none )
    set lhave = ( $lhave HAVE_MYSQL          none )
    set lhave = ( $lhave HAVE_SQLITE         none )
    set lhave = ( $lhave HAVE_SQLITE_V2      none )
    set lhave = ( $lhave Std_UseX            HAVE_X11 )
    set lhave = ( $lhave Std_UseOGL          HAVE_OGL )
    set lhave = ( $lhave Std_UseQD           HAVE_QUICKDRAW )
    set lhave = ( $lhave HAVE_JPEGLIB        none )
    set lhave = ( $lhave HAVE_PNGLIB         none )
    set lhave = ( $lhave HAVE_FORK_EXEC      none )
    set lhave = ( $lhave HAVE_GETHOSTBYNAME  none )
    set lhave = ( $lhave HAVE_RESOURCE_USAGE none )
    set lhave = ( $lhave HAVE_GETPWUID       none )
    set lhave = ( $lhave HAVE_ASYNC_STREAMS  none )
    set lhave = ( $lhave HAVE_POSIX_SIGNALS  USE_POSIX_SIGNALS )
    set lhave = ( $lhave HAVE_STREAMS_P      none )
    set lhave = ( $lhave HAVE_SELECT_P       none )
    set lhave = ( $lhave HAVE_UNIX98_PTY     none )
    set lhave = ( $lhave HAVE_MMAP           none )
    set lhave = ( $lhave BadLineDirectives   HAVE_BAD_LINE_DIRECTIVES )
    set lhave = ( $lhave HAVE_POSIX_STRERROR HAVE_POSIX_STRERROR_R )
    set lhave = ( $lhave HAVE_GNU_STRERROR   HAVE_GNU_STRERROR_R )
    set lhave = ( $lhave HAVE_VSNPRINTF      HAVE_VSNPRINTF )
    set lhave = ( $lhave HAVE_GNU_LIBC_6     none )
    set lhave = ( $lhave HAVE_DYNAMIC_LINKER none )
    set lhave = ( $lhave HAVE_BFD            none )
    set lhave = ( $lhave HAVE_DEMANGLE       none )
    set lhave = ( $lhave USE_FULL_MM         none )

    while ($#lhave > 0)
       set lvr = $lhave[1]
       set lvl = $lhave[2]
       if ($lvl == none) then
          set lvl = $lvr
       endif
       shift lhave
       shift lhave
       dbget $lvr
       set res = ( `printenv $lvr` )
       if ("$res" == "TRUE") then
          Note $STDOUT "#define $lvl"
       endif
    end
    unset lhave
    Note $STDOUT ""

# emit the SO flags
    if ($Linking == dynamic) then
       set lso = ""
       set lso = ( $lso HDF5_SO )
       set lso = ( $lso SQLITE3_SO )
       set lso = ( $lso MYSQL_SO )
       set lso = ( $lso BFD_SO )
       while ($#lso > 0)
          set lvr = $lso[1]
          shift lso
          dbget $lvr
          set res = ( `printenv $lvr` )
          if ("$res" != "") then
             Note $STDOUT "#define $lvr "'"'$res'"'
          endif
       end
       unset lso
       Note $STDOUT ""
    endif
   
# emit non-systematic sets of flags
    if ($HAVE_BFD == TRUE) then
       Note $STDOUT '#define BFD_VERSION "'$BFD_Version'"'
       set inf = ( `echo $BFD_Version | sed 's/\./ /g'` )
       if ($#inf > 2) then
          Note $STDOUT "#define BFD_MAJOR_VERSION $inf[1]"
          Note $STDOUT "#define BFD_MINOR_VERSION $inf[2]"
       endif
       unset inf
    endif

    if (($HaveVACOPY != TRUE) && ($HaveVALIST == FALSE)) then
       Note $STDOUT "#define NO_VA_LIST"
    endif

    if ($HaveTRACKER == TRUE) then
       Note $STDOUT "#define HAVE_TRACKER"
       Note $STDOUT "#define TRACKER_EXE "$QUOTE$TRACKERExe$QUOTE
    endif

    if (-e /dev/zero) then
       if (("$HostOS" != "HP-UX") && ("$HostOS" != "Darwin")) then
          Note $STDOUT "#define HAVE_DEV_ZERO"
       endif
    endif

    if ($IPCSupport != "NOIPC") then
       flog $Log $RM $IncDir/noipc
       Note $STDOUT "#define HAVE_PROCESS_CONTROL"
       if ($HAVE_SOCKETS_P != "FALSE") then
          set sty = ( `echo $GETSOCKOPT_TYPE | sed 's|\"||g'` )
          Note $STDOUT "#define HAVE_SOCKETS_P"
          Note $STDOUT "typedef $sty SOCKOPT_T;"
       endif
    else
       flog $Log touch $IncDir/noipc
    endif

    if (("$NEED_ALT_LARGE_FILE" == "TRUE") && ($HostOS != FreeBSD)) then
       Note $STDOUT "#define NEED_ALT_LARGE_FILE"
       Note $STDOUT "#define HAVE_ALT_LARGE_FILE"
    endif

    Note $STDOUT ""

# parallel defines
    if ($HAVE_OPENMP == TRUE) then
       Note $STDOUT "#define PTHREAD_OMP"
       Note $STDOUT "#define THREADING openmp"

    else if ($PTHREAD_POSIX == TRUE) then
       Note $STDOUT "#define PTHREAD_POSIX"
       Note $STDOUT "#define THREADING pthread"
    endif

    if ($HAVE_OPENMPI == TRUE) then
       Note $STDOUT "#define HAVE_OPENMPI"
    endif

    if (("$DP_Lib" != "") || ("$DP_Inc" != "")) then
       Note $STDOUT "#define HAVE_MPI"
       if ($MPIGoodIO == FALSE) then
          Note $STDOUT "#define HAVE_BAD_MPI_IO"
       endif
       if ($MPIAllStdin == TRUE) then
          Note $STDOUT "#define HAVE_MPI_STDIN_ALL"
       else
          Note $STDOUT "#define HAVE_MPI_STDIN_ONE"
       endif
    endif

    Note $STDOUT ""

    if (-f $CEFile) then
       cat $CEFile >>& $STDOUT
       flog $Log $RM $CEFile
       Note $STDOUT ""
    endif

    if ("$FC_ID_CASE" == "upper") then
       if ("$FC_ID_UNDERSCORE" == "") then
          Note $STDOUT "#define FF_ID(x, X)      X"
       else
          Note $STDOUT "#define FF_ID(x, X)      X ## $FC_ID_UNDERSCORE"
       endif
       Note $STDOUT ""
    else if ("$FC_ID_CASE" == "lower") then
       if ("$FC_ID_UNDERSCORE" == "") then
          Note $STDOUT "#define FF_ID(x, X)      x"
       else
          Note $STDOUT "#define FF_ID(x, X)      x ## $FC_ID_UNDERSCORE"
       endif
       Note $STDOUT ""
    endif

# OS defines
    SafeEnv OSType "HAVE_SYSV"
    Note $STDOUT "#undef UNIX"
    Note $STDOUT "#define UNIX"

    Note $STDOUT "#undef $OSType"
    Note $STDOUT "#define $OSType"

    set OSL = ( `cat $MngDir/std/os | grep -v '#'` )
    while ($#OSL > 2)
       set sysn = $OSL[1]
       set pctn = $OSL[2]
       set incn = $OSL[3]
       shift OSL
       shift OSL
       shift OSL
       if ("$HostOS" == "$sysn") then
          Note $STDOUT "#undef $pctn"
          Note $STDOUT "#define $pctn"
          Note $STDOUT '#include <'$incn'>'
       endif
    end

    Note $STDOUT ""

    SafeSet HostArch $HostOSRel

    Note $STDOUT "#endif"
    Note $STDOUT ""

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

exit(0)

