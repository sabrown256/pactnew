#!/bin/csh -f
#
# ENVF - write out environment information
#      - write versions for CSH, SH, and dotkit families
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#

unalias *

if (-f ../scripts/functions-pact.csh) then
   set SrcDir = $cwd
else if (-f ../../scripts/functions-pact.csh) then
   set SrcDir = $cwd:h
endif
set ldir = $SrcDir:h/scripts
set path = ( $ldir $path )
source $ldir/functions-pact.csh

dbmget $WLog MDG_Pck^        \
             HAVE_OPENMP^    \
             HAVE_READLINE^  \
             HAVE_PYTHON^    \
             PY_Vers^        \
             PSY_Root^       \
             PSY_SITE^       \
             PSY_TYPE^       \
             PSY_ASRoot^     \
             PSY_AIRoot

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

    set lnotice = $1

    Separator $WLog
    if ($lnotice == TRUE) then
       NoteD $WLog "   Environment setup files - env-pact"
    else
       Note $WLog "   Environment setup files - env-pact"
    endif
    Note $WLog " "

    set tmpF = $SEFile.tmp
    $RM $tmpF
    touch $tmpF
    Note $tmpF "setenv PSY_TYPE $PSY_TYPE"
    Note $tmpF "setenv PSY_SITE $PSY_SITE"
    Note $tmpF "setenv PSY_ASRoot $PSY_ASRoot"
    Note $tmpF "setenv PSY_AIRoot $PSY_AIRoot"

# make a temporary approximation to EnvCsh
    cat $SEFile | \
    awk '$1 != "PATH" {print "setenv " $0}' >> $tmpF

# use the C Shell to expand and print unique environment variable settings
# in Bourne Shell syntax
    /bin/csh -fc '(source clearenv ; source '$tmpF' ; env)' |& \
    awk '{printf("export %s\"\n", $0);}'                    |  \
    tr "\t" " "                                             |  \
    sed 's/= */=/'                                          |  \
    sed 's/=/="/' >&! $EnvSh

# reprocess to get C Shell settings
    cat $EnvSh | sed 's/export/setenv/' | sed 's/=/ /' >&! $EnvCsh

# the dotkit representation is the same as the C Shell at this point
    cp $EnvCsh $EnvDk
    $RM $tmpF
    unset tmpF

# do special handling for PATH and RPATH
    flog $WLog set EPath = ""
    set length = `wc -l $SEFile`
    @ nl = $length[1]
    @ il = 1
    while ($il <= $nl)
       get_line ln $SEFile $il
#       set ln = `head -n $il $SEFile | tail -n 1`
       set var = $ln[1]
       set val = "$ln[2-]"

       if ("$var" == PATH) then
          set tPath = `echo "$val" | sed 's|\${PATH}|'$EPath'|g'`
          set EPath = `echo "$tPath" | sed 's|\$PATH|'$EPath'|g'`
       endif
       @ il = $il + 1
    end

    Note $EnvCsh ""
    Note $EnvSh ""
    Note $EnvDk ""

# emit MANPATH settings
    Note $EnvCsh 'if ($?MANPATH == 1) then'
    Note $EnvCsh '   setenv MANPATH '$PSY_Root'/man:$MANPATH'
    Note $EnvCsh 'else'
    Note $EnvCsh '   setenv MANPATH '$PSY_Root'/man:'
    Note $EnvCsh 'endif'
    Note $EnvCsh ""

    Note $EnvSh 'if [ "$MANPATH" != "" ]; then'
    Note $EnvSh "   export MANPATH=$PSY_Root/man"':$MANPATH'
    Note $EnvSh "else"
    Note $EnvSh "   export MANPATH=$PSY_Root/man"
    Note $EnvSh "fi"
    Note $EnvSh ""

    Note $EnvDk 'dk_alter MANPATH '$PSY_Root'/man'
    Note $EnvDk ""

    if ($?HAVE_PYTHON == 1) then
       if ("$HAVE_PYTHON" =~ *TRUE*) then
          set lPython = $LibDir/python$PY_Vers
          if ($?PYTHONPATH) then
             if (`expr "$PYTHONPATH" : '.*'$lPython'.*'` == 0) then
                setenv PYTHONPATH ${lPython}:$PYTHONPATH
             endif
          else
             setenv PYTHONPATH ${lPython}
          endif
          unset lPython
          Note $EnvCsh "setenv PYTHONPATH $PYTHONPATH"
          Note $EnvDk  "dk_setenv PYTHONPATH $PYTHONPATH"
          Note $EnvSh  "export PYTHONPATH=$PYTHONPATH"
       endif
    endif

# emit PATH settings
    if ("$EPath" != "") then
       foreach dir (`echo $EPath | sed 's/:/ /g'`)
          Note $EnvDk  "dk_alter PATH $dir"
       end
       set EPath = `echo "$EPath"':$PATH' | sed 's/::/:/g'`
       Note $EnvCsh "setenv PATH    $PSY_Root/bin:${EPath}"
       Note $EnvSh  "export PATH=$PSY_Root/bin:${EPath}"
    else
       Note $EnvCsh 'setenv PATH    '$PSY_Root'/bin:$PATH'
       Note $EnvSh 'export PATH='$PSY_Root'/bin:$PATH'
       Note $EnvDk 'dk_alter  PATH    '$PSY_Root'/bin'
    endif

# write the CSH version like this because PCSH scripts like
# PDBView need to get the users SCHEME variable if defined
    Note $EnvCsh 'if ($?SCHEME == 1) then'
    Note $EnvCsh '   setenv SCHEME  ${SCHEME}:'"$PSY_Root/scheme"
    Note $EnvCsh 'else'
    Note $EnvCsh "   setenv SCHEME  $PSY_Root/scheme"
    Note $EnvCsh 'endif'
    Note $EnvCsh "setenv ULTRA   $PSY_Root/scheme"

    Note $EnvSh "export SCHEME=$PSY_Root/scheme"
    Note $EnvSh "export ULTRA=$PSY_Root/scheme"
    Note $EnvDk "dk_setenv SCHEME  $PSY_Root/scheme"
    Note $EnvDk "dk_setenv ULTRA   $PSY_Root/scheme"

    flog $WLog cat $EnvCsh
    flog $WLog cat $EnvSh
    flog $WLog cat $EnvDk

# source this now before trying to compile smake in WriteMAKEM
    Note $WLog "Sourcing $EnvCsh"
    source $EnvCsh

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

exit(0)

