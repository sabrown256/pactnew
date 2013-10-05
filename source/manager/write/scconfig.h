#!/bin/csh -f
#
# SCCONFIG.H - write the SCCONFIG.H file
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#

set Me = $0
source $Me:h/common

Note $Log "----- write/scconfig.h -----"
Note $Log "Write: scconfig.h"
Note $Log ""

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

Separator $Log

NoteD $Log "   C Environment Configuration - scconfig.h"
Note $Log ""

dbmget $Log Arch^                     \
            BFD_Version^              \
            BUILD_DEBUG^              \
            BUILD_OPTIMIZE^           \
            BUILD_PROFILE^            \
            CC_Exe^                   \
            CEFile^                   \
            CPU^                      \
            DEFAULT_SHELL^            \
            DP_Lib^                   \
            DP_Inc^                   \
            FC_ID_CASE^               \
            FC_ID_UNDERSCORE^         \
            FC_INT_PTR_DIFFER^        \
            FC_INTEGER_C^             \
            FilterDir^                \
            FPU^                      \
            GETSOCKOPT_TYPE^          \
            OS_Name^                  \
            OS_Release^               \
            IncDir^                   \
            STD_IPC^                  \
            Linking^                  \
            LONG64^                   \
            HAVE_MPI_STDIN_ALL^       \
            HAVE_MPI_GOOD_IO^         \
            NO_LONG_LONG^             \
            NEED_ALT_LARGE_FILE^      \
            PACT_CC_FAMILY^           \
            PACT_CC_VERSION^          \
            PACT_SO_CACHE^            \
            PTHREAD_POSIX^            \
            OS_Type^                  \
            OSX_Version^              \
            Sys^                      \
            TRACKER_Exe^              \
            MYSQL_SO^                 \
            SQLITE3_SO^               \
            HDF5_SO^                  \
            HAVE_BAD_LINE_DIRECTIVES^ \
            BUILD_DEBUG^              \
            BUILD_OPTIMIZE^           \
            BUILD_PROFILE^            \
            HAVE_INLINE^              \
            HAVE_NORETURN^            \
            HAVE_OPENMP^              \
            HAVE_OPENMPI^             \
            HAVE_SOCKETS^             \
            HAVE_TRACKER^             \
            HAVE_ANSI_FLOAT16^        \
            HAVE_ANSI_C9X_COMPLEX^    \
            HAVE_ANSI_C9X_FENV^       \
            HAVE_GNU_FENV^            \
            HAVE_VA_COPY^             \
            HAVE_VA_LIST^             \
            HAVE_PYTHON^              \
            HAVE_PY_NUMERIC^          \
            HAVE_PY_NUMPY^            \
            HAVE_FLEX_SCANNER^        \
            HAVE_READLINE^            \
            HAVE_HDF5^                \
            HAVE_SQL^                 \
            HAVE_MYSQL^               \
            HAVE_SQLITE^              \
            HAVE_SQLITE_V2^           \
            HAVE_JPEGLIB^             \
            HAVE_PNGLIB^              \
            HAVE_FORK_EXEC^           \
            HAVE_GETHOSTBYNAME^       \
            HAVE_RESOURCE_USAGE^      \
            HAVE_GETPWUID^            \
            HAVE_MMAP^                \
            HAVE_VSNPRINTF^           \
            HAVE_ASYNC_STREAMS^       \
            HAVE_POSIX_SIGNALS^       \
            HAVE_STREAMS^             \
            HAVE_SELECT^              \
            HAVE_UNIX98_PTY^          \
            HAVE_POSIX_STRERROR^      \
            HAVE_GNU_STRERROR^        \
            HAVE_GNU_LIBC_6^          \
            HAVE_DYNAMIC_LINKER^      \
            HAVE_BFD^                 \
            HAVE_DEMANGLE^            \
            USE_FULL_MM^              \
            LEH_TTY_CRNL^             \
            LEH_TTY_ECHO^             \
            LEH_TTY_VTIME^            \
            Std_UseX^                 \
            Std_UseOGL^               \
            Std_UseQD^                \
            SYS_ID^                   \
            SYS_TYPE^                 \
            SYS_Prefix^               \
            SYS_TmpDir

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

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
    Note $STDOUT '#define PACT_VERSION        '$QUOTE${THE_DATE}$QUOTE

    Note $STDOUT "#define CPU_TYPE            $CPU"
    Note $STDOUT "#define FPU_TYPE            $FPU"
    Note $STDOUT '#define SYSTEM_ID           '$QUOTE${Arch}$QUOTE
    Note $STDOUT '#define SYS_TYPE            '$QUOTE${SYS_TYPE}$QUOTE
    Note $STDOUT '#define SYS_ID              '$QUOTE${SYS_ID}$QUOTE
    Note $STDOUT '#define SYS_Prefix          '$QUOTE${SYS_Prefix}$QUOTE
    Note $STDOUT '#define SYS_TmpDir          '$QUOTE${SYS_TmpDir}$QUOTE
    if ("$OSX_Version" != "") then
       Note $STDOUT "#define MACOSX_VERSION      $OSX_Version"
    endif

    Note $STDOUT '#define USE_COMPILER        '$QUOTE${CC_Exe}$QUOTE

    Note $STDOUT '#define COMPILER_'$PACT_CC_FAMILY
    Note $STDOUT '#define COMPILER_VERSION    '${QUOTE}${PACT_CC_VERSION}${QUOTE}

    Note $STDOUT '#define DEFAULT_SHELL       '$QUOTE${DEFAULT_SHELL}$QUOTE

    if ("$FilterDir" != "") then
       set FiltFile = $FilterDir/$Arch
       Note $STDOUT '#define MAKE_FILTER_FILE    '$QUOTE${FiltFile}$QUOTE
    endif

    if ("$PACT_SO_CACHE" != "") then
       Note $STDOUT '#define PACT_SO_CACHE       '$QUOTE${PACT_SO_CACHE}$QUOTE
    endif

    Note $STDOUT ""

    Note $STDOUT "#define ANSI"
    Note $STDOUT "#define byte void"
    Note $STDOUT "#undef CONST"
    Note $STDOUT "#define CONST const"
    Note $STDOUT "#undef SIGNED"
    Note $STDOUT "#define SIGNED signed"
    Note $STDOUT "#undef INLINE"
    if ($HAVE_INLINE == TRUE) then
       Note $STDOUT "#define INLINE inline"
    else
       Note $STDOUT "#define INLINE"
    endif

    Note $STDOUT "#undef NORETURN"
    if ($HAVE_NORETURN == TRUE) then
       Note $STDOUT "#define NORETURN _Noreturn"
    else
       Note $STDOUT "#define NORETURN"
    endif

    if ($NO_LONG_LONG == TRUE) then
       Note $STDOUT "#define NO_LONG_LONG"
    endif

    if ("$LONG64" != "") then
       Note $STDOUT "#define LONG64"
    endif

    if ("$FC_INTEGER_C" != "unknown") then
       Note $STDOUT "#define FIXNUM $FC_INTEGER_C"
    endif

    Note $STDOUT ""

# emit the HAVE/USE flags - no value
    set lhave = ""
    set lhave = ( $lhave BUILD_DEBUG              none )
    set lhave = ( $lhave BUILD_OPTIMIZE           none )
    set lhave = ( $lhave BUILD_PROFILE            none )
    set lhave = ( $lhave FC_INT_PTR_DIFFER        none )
    set lhave = ( $lhave HAVE_ANSI_FLOAT16        none )
    set lhave = ( $lhave HAVE_ANSI_C9X_COMPLEX    none )
    set lhave = ( $lhave HAVE_ANSI_C9X_FENV       none )
    set lhave = ( $lhave HAVE_GNU_FENV            none )
    set lhave = ( $lhave HAVE_VA_COPY             none )
    set lhave = ( $lhave HAVE_PYTHON              none )
    set lhave = ( $lhave HAVE_PY_NUMERIC          none )
    set lhave = ( $lhave HAVE_PY_NUMPY            none )
    set lhave = ( $lhave HAVE_HDF5                none )
    set lhave = ( $lhave HAVE_SQL                 none )
    set lhave = ( $lhave HAVE_MYSQL               none )
    set lhave = ( $lhave HAVE_SQLITE              none )
    set lhave = ( $lhave HAVE_SQLITE_V2           none )
    set lhave = ( $lhave HAVE_JPEGLIB             none )
    set lhave = ( $lhave HAVE_PNGLIB              none )
    set lhave = ( $lhave HAVE_FORK_EXEC           none )
    set lhave = ( $lhave HAVE_GETHOSTBYNAME       none )
    set lhave = ( $lhave HAVE_RESOURCE_USAGE      none )
    set lhave = ( $lhave HAVE_GETPWUID            none )
    set lhave = ( $lhave HAVE_ASYNC_STREAMS       none )
    set lhave = ( $lhave HAVE_POSIX_SIGNALS       none )
    set lhave = ( $lhave HAVE_STREAMS             none )
    set lhave = ( $lhave HAVE_SELECT              none )
    set lhave = ( $lhave HAVE_UNIX98_PTY          none )
    set lhave = ( $lhave HAVE_MMAP                none )
    set lhave = ( $lhave HAVE_BAD_LINE_DIRECTIVES none )
    set lhave = ( $lhave HAVE_POSIX_STRERROR      none )
    set lhave = ( $lhave HAVE_GNU_STRERROR        none )
    set lhave = ( $lhave HAVE_VSNPRINTF           none )
    set lhave = ( $lhave HAVE_GNU_LIBC_6          none )
    set lhave = ( $lhave HAVE_DYNAMIC_LINKER      none )
    set lhave = ( $lhave HAVE_BFD                 none )
    set lhave = ( $lhave HAVE_DEMANGLE            none )
    set lhave = ( $lhave USE_FULL_MM              none )
#    set lhave = ( $lhave HAVE_READLINE            none )

# these involve the HAVE/USE dichotomy
    set lhave = ( $lhave HAVE_FLEX_SCANNER        USE_FLEX )
    set lhave = ( $lhave Std_UseX                 HAVE_X11 )
    set lhave = ( $lhave Std_UseOGL               HAVE_OGL )
    set lhave = ( $lhave Std_UseQD                HAVE_QUICKDRAW )

# termios TTY controls
    set lhave = ( $lhave LEH_TTY_CRNL             none )
    set lhave = ( $lhave LEH_TTY_ECHO             none )
    set lhave = ( $lhave LEH_TTY_VTIME            none )

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

    if (($HAVE_VA_COPY != TRUE) && ($HAVE_VA_LIST == FALSE)) then
       Note $STDOUT "#define NO_VA_LIST"
    endif

    if ($HAVE_TRACKER == TRUE) then
       Note $STDOUT "#define HAVE_TRACKER"
       Note $STDOUT "#define TRACKER_EXE "$QUOTE${TRACKER_Exe}$QUOTE
    endif

    if (-e /dev/zero) then
       if (("$OS_Name" != "HP-UX") && ("$OS_Name" != "Darwin")) then
          Note $STDOUT "#define HAVE_DEV_ZERO"
       endif
    endif

    if ($STD_IPC != "NOIPC") then
       flog $Log $RM $IncDir/noipc
       Note $STDOUT "#define HAVE_PROCESS_CONTROL"
       if ($HAVE_SOCKETS != "FALSE") then
          set sty = ( `echo $GETSOCKOPT_TYPE | sed 's|\"||g'` )
          Note $STDOUT "#define HAVE_SOCKETS"
          Note $STDOUT "typedef $sty SOCKOPT_T;"
       endif
    else
       flog $Log touch $IncDir/noipc
    endif

    if (("$NEED_ALT_LARGE_FILE" == "TRUE") && ($OS_Name != FreeBSD)) then
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
       Note $STDOUT "#define HAVE_MPI   TRUE"
       if ($HAVE_MPI_GOOD_IO == FALSE) then
          Note $STDOUT "#define HAVE_BAD_MPI_IO"
       endif
       if ($HAVE_MPI_STDIN_ALL == TRUE) then
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
    if ("$OS_Type" == USE_MSW) then
       Note $STDOUT "#undef $OS_Name"
       Note $STDOUT "#define $OS_Name"
    else
       Note $STDOUT "#undef UNIX"
       Note $STDOUT "#define UNIX"
    endif

    Note $STDOUT "#undef $OS_Type"
    Note $STDOUT "#define $OS_Type"

    set OSL = ( `cat $MngDir/std/os | grep -v '#'` )
    while ($#OSL > 2)
       set sysn = $OSL[1]
       set pctn = $OSL[2]
       set incn = $OSL[3]
       shift OSL
       shift OSL
       shift OSL
       if ("$OS_Name" == "$sysn") then
          Note $STDOUT "#undef $pctn"
          Note $STDOUT "#define $pctn"
          Note $STDOUT '#include <'$incn'>'
       endif
    end

    Note $STDOUT ""

    SafeSet HostArch $OS_Release

    Note $STDOUT "#endif"
    Note $STDOUT ""

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

exit(0)

