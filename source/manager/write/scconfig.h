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
source $Me:h/pre-common

Note $WLog "----- write/scconfig.h -----"
Note $WLog "Write: scconfig.h"
Note $WLog ""

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

Separator $WLog

NoteD $WLog "   C Environment Configuration - scconfig.h"
Note $WLog ""

dbmget $WLog AF_ALT_LARGE_FILE^        \
             AF_LONG8^                 \
             AF_LONG_DOUBLE_IO^        \
             AF_SOCKOPT_TYPE^          \
             AF_VERSION_OSX^           \
             BFD_Version^              \
             CC_Exe^                   \
             'Cdefine*'^               \
             DP_Lib^                   \
             DP_Inc^                   \
             FC_ID_CASE^               \
             FC_ID_UNDERSCORE^         \
             FC_INT_PTR_DIFFER^        \
             FC_INTEGER_C^             \
             GRAPHICS_UseX^            \
             GRAPHICS_UseOGL^          \
             GRAPHICS_UseQD^           \
             HAVE_MPI_STDIN_ALL^       \
             HAVE_MPI_GOOD_IO^         \
             HAVE_LONG_LONG^           \
             HAVE_BAD_LINE_DIRECTIVES^ \
             HAVE_INLINE^              \
             HAVE_NORETURN^            \
             HAVE_OPENMP^              \
             HAVE_OPENMPI^             \
             HAVE_SOCKETS^             \
             HAVE_TRACKER^             \
             HAVE_ANSI_FLOAT16^        \
             HAVE_ANSI_C9X_COMPLEX^    \
             HAVE_ANSI_C9X_FENV^       \
             HAVE_COMPLEX_VA_ARG^      \
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
             HDF5_SO^                  \
             HSY_OS_Name^              \
             HSY_OS_Release^           \
             HSY_OS_Type^              \
             HSY_CPU^                  \
             HSY_FPU^                  \
             IBM_HW^                   \
             LEH_TTY_CRNL^             \
             LEH_TTY_ECHO^             \
             LEH_TTY_VTIME^            \
             LibM_Functions^           \
             MYSQL_SO^                 \
             OpenMP_OVER_SCHED^        \
             PACT_CC_FAMILY^           \
             PACT_CC_VERSION^          \
             PACT_SO_CACHE^            \
             PSY_Arch^                 \
             PSY_Cfg^                  \
             PSY_FltDir^               \
             PSY_MngDir^               \
             PSY_ID^                   \
             PSY_Root^                 \
             PSY_TYPE^                 \
             PSY_Prefix^               \
             PSY_TmpDir^               \
             RF_DEBUG^                 \
             RF_OPTIMIZE^              \
             RF_PROFILE^               \
             RF_Linking^               \
             SHELL_Default^            \
             SMP_Pthread^              \
             SQLITE3_SO^               \
             STD_IPC^                  \
             TRACKER_Exe^              \
             USE_FULL_MM

setenv IncDir  $PSY_Root/include

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

    set STDOUT  = $IncDir/scconfig.h
    set psy_cfg = $PSY_Cfg:t

    flog $WLog $RM $STDOUT
    flog $WLog touch $STDOUT

    Note $STDOUT "/*"
    Note $STDOUT " * SCCONFIG.H - configuration defined by $psy_cfg"
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

# define Q to be a double quote
    set q = \"

    set THE_DATE = `cat .pact-version`
    Note $STDOUT '#define PACT_VERSION        '$q${THE_DATE}$q

    Note $STDOUT "#define CPU_TYPE            $HSY_CPU"
    Note $STDOUT "#define FPU_TYPE            $HSY_FPU"
    Note $STDOUT '#define PSY_Arch            '$q${PSY_Arch}$q
    Note $STDOUT '#define PSY_TYPE            '$q${PSY_TYPE}$q
    Note $STDOUT '#define PSY_ID              '$q${PSY_ID}$q
    Note $STDOUT '#define PSY_Prefix          '$q${PSY_Prefix}$q
    Note $STDOUT '#define PSY_TmpDir          '$q${PSY_TmpDir}$q
    if ("$AF_VERSION_OSX" != "") then
       Note $STDOUT "#define MACOSX_VERSION      $AF_VERSION_OSX"
    endif

    Note $STDOUT '#define USE_COMPILER        '$q${CC_Exe}$q

    Note $STDOUT '#define COMPILER_'$PACT_CC_FAMILY
    Note $STDOUT '#define COMPILER_VERSION    '${q}${PACT_CC_VERSION}${q}

    Note $STDOUT '#define SHELL_Default       '$q${SHELL_Default}$q

    if ("$PSY_FltDir" != "") then
       set FiltFile = $PSY_FltDir/$PSY_Arch
       Note $STDOUT '#define MAKE_FILTER_FILE    '$q${FiltFile}$q
    endif

    if ("$PACT_SO_CACHE" != "") then
       Note $STDOUT '#define PACT_SO_CACHE       '$q${PACT_SO_CACHE}$q
    endif

    Note $STDOUT ""

    Note $STDOUT "#define ANSI"
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

    if ($HAVE_LONG_LONG == FALSE) then
       Note $STDOUT "#define NO_LONG_LONG"
    endif

    if ("$AF_LONG8" != "") then
       Note $STDOUT "#define AF_LONG8"
    endif

    if ("$AF_LONG_DOUBLE_IO" != "") then
       Note $STDOUT "#define AF_LONG_DOUBLE_IO $AF_LONG_DOUBLE_IO"
    endif

    if ("$FC_INTEGER_C" != "unknown") then
       Note $STDOUT "#define FIXNUM $FC_INTEGER_C"
    endif

    Note $STDOUT ""

# emit the HAVE/USE flags - no value
    set lhave = ""
    set lhave = ( $lhave FC_INT_PTR_DIFFER        none )
    set lhave = ( $lhave HAVE_ANSI_FLOAT16        none )
    set lhave = ( $lhave HAVE_ANSI_C9X_COMPLEX    none )
    set lhave = ( $lhave HAVE_ANSI_C9X_FENV       none )
    set lhave = ( $lhave HAVE_COMPLEX_VA_ARG      none )
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
    set lhave = ( $lhave OpenMP_OVER_SCHED        none )
    set lhave = ( $lhave RF_DEBUG                 none )
    set lhave = ( $lhave RF_OPTIMIZE              none )
    set lhave = ( $lhave RF_PROFILE               none )
    set lhave = ( $lhave USE_FULL_MM              none )
#    set lhave = ( $lhave HAVE_READLINE            none )

# these involve the HAVE/USE dichotomy
    set lhave = ( $lhave HAVE_FLEX_SCANNER        USE_FLEX )
    set lhave = ( $lhave GRAPHICS_UseX            HAVE_X11 )
    set lhave = ( $lhave GRAPHICS_UseOGL          HAVE_OGL )
    set lhave = ( $lhave GRAPHICS_UseQD           HAVE_QUICKDRAW )

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
    if ($RF_Linking == dynamic) then
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
       Note $STDOUT "#define TRACKER_EXE "$q${TRACKER_Exe}$q
    endif

    if (-e /dev/zero) then
       if (("$HSY_OS_Name" != "HP-UX") && ("$HSY_OS_Name" != "Darwin")) then
          Note $STDOUT "#define HAVE_DEV_ZERO"
       endif
    endif

    if ($STD_IPC != "NOIPC") then
       flog $WLog $RM $IncDir/noipc
       Note $STDOUT "#define HAVE_PROCESS_CONTROL"
       if ($HAVE_SOCKETS != "FALSE") then
          set sty = ( `echo $AF_SOCKOPT_TYPE | sed 's|\"||g'` )
          Note $STDOUT "#define HAVE_SOCKETS"
          Note $STDOUT "typedef $sty SOCKOPT_T;"
       endif
    else
       flog $WLog touch $IncDir/noipc
    endif

    if (("$AF_ALT_LARGE_FILE" == "TRUE") && ($HSY_OS_Name != FreeBSD)) then
       Note $STDOUT "#define AF_ALT_LARGE_FILE"
       Note $STDOUT "#define HAVE_ALT_LARGE_FILE"
    endif

    Note $STDOUT ""

# parallel defines
    if ($HAVE_OPENMP == TRUE) then
       Note $STDOUT "#define SMP_OpenMP"
       Note $STDOUT "#define THREADING openmp"

    else if ($SMP_Pthread == TRUE) then
       Note $STDOUT "#define SMP_Pthread"
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

# function defines
    set hlst = ( $LibM_Functions )
    while ($#hlst > 0)
       set m = $hlst[1]
       set f = $hlst[2]
       shift hlst
       shift hlst
       Note $STDOUT "#define $m  $f"
    end
    Note $STDOUT ""

# do some defines for IBM BG hardware
    if ($IBM_HW =~ BG*) then
       Note $STDOUT "#define IBM_$IBM_HW"
    endif
            
# find and write out the C #defines
flog $WLog ( env | grep Cdefine_ )
    foreach v (`env | grep Cdefine_`)
       set inf = ( `echo "$v" | sed 's|^Cdefine_||' | sed 's|=| |'` )
       flog $WLog set var = $inf[1]
       flog $WLog set val = ( $inf[2-] )
       Note $WLog "#define $var $val"
       Note $STDOUT "#define $var $val"
    end

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
    if ("$HSY_OS_Type" == USE_MSW) then
       Note $STDOUT "#undef $HSY_OS_Name"
       Note $STDOUT "#define $HSY_OS_Name"
    else
       Note $STDOUT "#undef UNIX"
       Note $STDOUT "#define UNIX"
    endif

    Note $STDOUT "#undef $HSY_OS_Type"
    Note $STDOUT "#define $HSY_OS_Type"

    set OSL = ( `cat $PSY_MngDir/std/os | grep -v '#'` )
    while ($#OSL > 2)
       set sysn = $OSL[1]
       set pctn = $OSL[2]
       set incn = $OSL[3]
       shift OSL
       shift OSL
       shift OSL
       if ("$HSY_OS_Name" == "$sysn") then
          Note $STDOUT "#undef $pctn"
          Note $STDOUT "#define $pctn"
          Note $STDOUT '#include <'$incn'>'
       endif
    end

    Note $STDOUT ""

    Note $STDOUT "#endif"
    Note $STDOUT ""

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

source $Me:h/post-common

exit(0)

