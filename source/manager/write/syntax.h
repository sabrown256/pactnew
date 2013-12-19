#!/bin/csh -f
#
# SYNTAX.H - write the SYNTAX.H file
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#

set Me = $0
source $Me:h/pre-common

Note $WLog "----- write/syntax.h -----"
Note $WLog "Write: syntax.h"
Note $WLog ""

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

Separator $WLog

NoteD $WLog "   Interpeter Syntax Mode Configuration - syntax.h"
Note $WLog ""

dbmget $WLog IncDir^     \
             SHELL_Cont^ \
             Yacc_Exe^   \
             PSY_Base^   \
             PSY_Cfg

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

    set STDOUT = $IncDir/syntax.h

    flog $WLog $RM $STDOUT
    flog $WLog touch $STDOUT

    Note $STDOUT "/*"
    Note $STDOUT " * SYNTAX.H - Configure SCHEME Syntax Modes for $PSY_Cfg"
    Note $STDOUT " *"
    Note $STDOUT " * Source Version: 5.0"
    Note $STDOUT " * Software Release #: LLNL-CODE-422942"
    Note $STDOUT " *"
    Note $STDOUT " */"

    Note $STDOUT ""

# setup the Scheme syntax modes
    set lsm   = ""
    @ nsyntxs = 0
    if ("$Yacc_Exe" != "") then
       flog $WLog pushd $PSY_Base/scheme
       if (!(-d syntax)) then
          flog $WLog mkdir syntax
       endif
       flog $WLog cd syntax
       set scsd = `scs-meta -n`
       set Dirs = `ls`
       foreach dir ($Dirs)
          if (-d $dir) then
             if ($dir != "$scsd") then
                set lsm   = ( $lsm $dir )
                @ nsyntxs = $nsyntxs + 1
             endif
          endif
       end
       flog $WLog popd
    endif

    if ($nsyntxs != 0) then

       set isyntx  = 0
       while (`expr $isyntx \< $nsyntxs`)
          set isyntx = `expr $isyntx + 1`
          set Syntx  = $lsm[$isyntx]
          set Syntx  = `echo $Syntx | tr "[a-z]" "[A-Z]"`
          Note $STDOUT "#define HAVE_"$Syntx"_SYNTAX"
       end

       Note $STDOUT ""

       Note $STDOUT "#define DEF_SYNTAX_MODES(_si) "$SHELL_Cont

       set isyntx = $nsyntxs
       while (`expr $isyntx \> 0`)
          set Syntx  = $lsm[$isyntx]
          if ($isyntx == 1) then
             Note $STDOUT "    SS_init_"$Syntx"_syntax_mode(_si)"
          else
             Note $STDOUT "    SS_init_"$Syntx"_syntax_mode(_si); "$SHELL_Cont
          endif
          set isyntx = `expr $isyntx - 1`
       end

       Note $STDOUT ""

       set isyntx = $nsyntxs
       Note $STDOUT "extern void"
       while (`expr $isyntx \> 0`)
          set Syntx  = $lsm[$isyntx]
          if ($isyntx == 1) then
             Note $STDOUT " SS_init_"$Syntx"_syntax_mode(SS_psides *si);"
          else
             Note $STDOUT " SS_init_"$Syntx"_syntax_mode(SS_psides *si),"
          endif
          set isyntx = `expr $isyntx - 1`
       end

# if we have no syntax modes we still have work to do
    else

       Note $STDOUT "#define DEF_SYNTAX_MODES(_si)"

    endif
   
    Note $STDOUT ""

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

source $Me:h/post-common

exit(0)

