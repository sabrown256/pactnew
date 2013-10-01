#!/bin/csh -f
#
# MAKE-DEF.MP - write the multi-platform MAKE-DEF file
#
# include "cpyright.h"
#

set Me = $0
source $Me:h/common

Note $Log "----- write/make-def.mp -----"
Note $Log "Write: make-def.mp"
Note $Log ""

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

Separator $Log

NoteD $Log "   Site Dependent MAKE Configuration - make-def.mp"
Note $Log ""

dbmget $Log BaseDir^                \
            EtcDir^                 \
            IncDir^                 \
            MngDir^                 \
            OS_Name^                \
            OS_Type^                \
            PubLib^                 \
            PubInc^                 \
            SysDir^                 \
            System

set vl = ""

# compiler tools
foreach t (CC FC CXX)
   foreach f (Debug Exe Flags Inc Linker Optimize)
      set vl = ( $vl ${t}_${f} )
   end
end

# linker tool
foreach f (Exe Flags Lib RPath)
   set vl = ( $vl LD_${f} )
end

# other tools
foreach t (AR Lex Ranlib Yacc)
   foreach f (Exe Flags)
      set vl = ( $vl ${t}_${f} )
   end
end

# groups
foreach g (Cfg Cfe Ser Std Shared)

# compilers
   foreach t (CC FC CXX)
      foreach f (Exe Flags Inc Linker)
         set vl = ( $vl ${g}_${t}_${f} )
      end
   end

# linker
   foreach f (Exe Lib RPath)
      set vl = ( $vl ${g}_LD_${f} )
   end
end

# others
foreach t (MD MDE MDI MDG DP LibM)
   foreach f (Inc Lib RPath Path)
      set vl = ( $vl ${t}_${f} )
   end
end

source $MngDir/write/import-db

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

if ("$OS_Type" == USE_MSW) then
   set lostd = WIN32
else
   set lostd = UNIX
endif

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

    set STDOUT = $EtcDir/make-def.mp

    flog $Log $RM $STDOUT
    flog $Log touch $STDOUT

    Note $STDOUT "#"
    Note $STDOUT "# MAKE-DEF.MP - installation dependent definitions for $Sys"
    Note $STDOUT "#"
    Note $STDOUT ""
    Note $STDOUT "System  = $System"
    Note $STDOUT 'archo   = z-${System}/obj'
    set QUOTE = \"
    set THE_DATE = `cat .pact-version`
    Note $STDOUT "Version = $THE_DATE"

    set mk = ( `$MngDir/pwhich gmake` )
    if (!(-x "$mk")) then
       set mk = `$MngDir/pwhich make`
    endif
    Note $Log "Using '$mk' for system make"
    Note $STDOUT "UMake   = $mk"
    unset mk

    Note $STDOUT "BaseDir = $BaseDir"
    Note $STDOUT "ScrDir  = $BaseDir/scripts"
    Note $STDOUT "SysDir  = $SysDir"
    Note $STDOUT 'ScmDir  = ${SysDir}/scheme'
    Note $STDOUT 'BinDir  = ${SysDir}/bin'
    Note $STDOUT 'LibDir  = ${SysDir}/lib'
    Note $STDOUT 'IncDir  = ${SysDir}/include'
    Note $STDOUT 'EtcDir  = ${SysDir}/etc'
    Note $STDOUT 'Man1Dir = ${SysDir}/man/man1'
    Note $STDOUT 'Man3Dir = ${SysDir}/man/man3'
    Note $STDOUT ""

    if ("$OS_Name" == "HP-UX") then
       Note $STDOUT 'TGTLib  = nolib'
    endif
    Note $STDOUT 'TGTBin  = nobin'
    Note $STDOUT 'TGTEtc  = noetc'
    Note $STDOUT 'TGTInc  = noinc'
    Note $STDOUT 'TGTScm  = noscm'
    Note $STDOUT 'TGTScr  = noscr'
    Note $STDOUT 'TGTScmX = noscmx'
    Note $STDOUT 'TGTScrX = noscrx'
    Note $STDOUT 'TGTMan3 = noman'
    Note $STDOUT 'LibName = noname'
    Note $STDOUT ""

    exit(0)

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

