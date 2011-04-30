#!/bin/csh -f
#
# SYNTAX.H - write the SYNTAX.H file
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#

unalias *

if (-f ../scripts/env-csh) then
   set SrcDir = $cwd
else if (-f ../../scripts/env-csh) then
   set SrcDir = $cwd:h
endif
set ldir = $SrcDir:h/scripts
set path = ( $ldir $path )
source $ldir/env-csh

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

    Separator $Log
    NoteD $Log "   Interpeter Syntax Mode Configuration - syntax.h"

    set STDOUT = $IncDir/syntax.h

    flog $Log $RM $STDOUT
    flog $Log touch $STDOUT

    Note $STDOUT "/*"
    Note $STDOUT " * SYNTAX.H - Configure SCHEME Syntax Modes for $Sys"
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
       flog $Log pushd $Base/scheme
       if (!(-d syntax)) then
          flog $Log mkdir syntax
       endif
       flog $Log cd syntax
       set Dirs = `ls`
       foreach dir ($Dirs)
          if (-d $dir) then
             if (($dir != CVS) && ($dir != .svn)) then
                set lsm   = ( $lsm $dir )
                @ nsyntxs = $nsyntxs + 1
             endif
          endif
       end
       flog $Log popd
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

       Note $STDOUT "#define DEF_SYNTAX_MODES(_si) "$BackSlash

       set isyntx = $nsyntxs
       while (`expr $isyntx \> 0`)
          set Syntx  = $lsm[$isyntx]
          if ($isyntx == 1) then
             Note $STDOUT "    SS_init_"$Syntx"_syntax_mode(_si)"
          else
             Note $STDOUT "    SS_init_"$Syntx"_syntax_mode(_si); "$BackSlash
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

       Note $STDOUT "#define DEF_SYNTAX_MODES()"

    endif
   
    Note $STDOUT ""

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

exit(0)

