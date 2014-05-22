#!/bin/csh -f
#
# MAKEFILE - write the distribution MAKEFILE
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#

set Me = $0
source $Me:h/pre-common

Note $WLog "----- write/makefile -----"
Note $WLog "Write: makefile"
Note $WLog ""

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

Separator $WLog

NoteD $WLog "   Distribution Makefile - Makefile"
Note $WLog ""

dbmget $WLog Bits^           \
             PSY_Root^       \
             PSY_ID^         \
             PSY_Platforms^  \
             Pck

setenv EtcDir  $PSY_Root/etc

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

   set inf = ( base $PSY_ID )
   set inf = ( $inf `echo "$PSY_Platforms" | sed 's|[:()]| |g'` )
   @ ni = $#inf
   Note $WLog "Added platform components ($ni):"
   Note $WLog "   $inf"

   set base = $PSY_Root:h

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

    set STDOUT = $EtcDir/Makefile

    flog $WLog $RM $STDOUT
    flog $WLog touch $STDOUT

    set Pck = `cat $EtcDir/package`

    Note $WLog "Packages: $Pck"

    Note $STDOUT "#"
    Note $STDOUT "# MAKEFILE - operate on this distribution"
    Note $STDOUT "#"
    Note $STDOUT ""

    @ i = 0
    @ n = 0
    while ($i < $ni)
       @ n = $n + 1
       @ i = $i + 1
       set cfg = $inf[$i]
       @ i = $i + 1
       set sid = $inf[$i]

       set root = $base/$sid 
       set etcd = $root/etc
       set bind = $root/bin

       Note $STDOUT "PACT$n   = $bind/pact"
       Note $STDOUT "MkFile$n = $etcd/Makefile"
       Note $STDOUT "Env$n    = $etcd/env-pact.csh"
    end

    Note $STDOUT ""

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

# compilers target

   Note $STDOUT 'compilers:'
   @ i = 0
   @ n = 0
   while ($i < $ni)
      @ n = $n + 1
      @ i = $i + 1
      set cfg = $inf[$i]
      @ i = $i + 1
      set sid = $inf[$i]

      Note $STDOUT '	@(cd score ; printf "\nreport cc for %s\n" '$sid' ; ${PACT'$n'} -a '$sid' compilers)'
   end

    Note $STDOUT ''

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

# build target

    Note $STDOUT 'build:'
    Note $STDOUT '	@(echo "" ; echo "Generate sources for '$PSY_ID'")'
    Note $STDOUT '	${BARRIER}'

    foreach p ($Pck)
       @ i = 0
       @ n = 0
       while ($i < $ni)
          @ n = $n + 1
          @ i = $i + 1
          set cfg = $inf[$i]
          @ i = $i + 1
          set sid = $inf[$i]

          set root = $base/$sid 
          set etcd = $root/etc
   
#          Note $STDOUT '	@/bin/csh -c "(cd '$p' ; echo '"''"' ; echo '"'"'pact generate in '$p' for '$sid"'"' ; source ${Env'$n'} ; ${PACT'$n'} -rcr generate)"'
          Note $STDOUT '	@(cd '$p' ; printf "\npact generate in %s for %s\n" '$p $sid' ; ${PACT'$n'} -a '$sid' -rcr generate)'
       end
       if ($p == score) then
          Note $STDOUT '	${BARRIER}'
       endif
    end

    Note $STDOUT '	${BARRIER}'

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

    Note $STDOUT '	@(echo "" ; echo "Install headers for '$PSY_ID'")'
    Note $STDOUT '	${BARRIER}'

    foreach p ($Pck)
       @ i = 0
       @ n = 0
       while ($i < $ni)
          @ n = $n + 1
          @ i = $i + 1
          set cfg = $inf[$i]
          @ i = $i + 1
          set sid = $inf[$i]

          set root = $base/$sid 
          set etcd = $root/etc
   
#          Note $STDOUT '	@/bin/csh -c "(cd '$p' ; echo '"''"' ; echo '"'"'pact incinstall in '$p' for '$sid"'"' ; source ${Env'$n'} ; ${PACT'$n'} -rcr incinstall)"'
          Note $STDOUT '	@(cd '$p' ; printf "\npact incinstall in %s for %s\n" '$p $sid' ; ${PACT'$n'} -a '$sid' -rcr incinstall)'
       end
    end

    Note $STDOUT '	${BARRIER}'

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

    Note $STDOUT '	@(echo "" ; echo "Compile sources for '$PSY_ID'")'
    Note $STDOUT '	${BARRIER}'

    foreach p ($Pck)
       @ i = 0
       @ n = 0
       while ($i < $ni)
          @ n = $n + 1
          @ i = $i + 1
          set cfg = $inf[$i]
          @ i = $i + 1
          set sid = $inf[$i]

          set root = $base/$sid 
          set etcd = $root/etc
   
#          Note $STDOUT '	@/bin/csh -c "(cd '$p' ; echo '"''"' ; echo '"'"'pact devlib in '$p' for '$sid"'"' ; source ${Env'$n'} ; ${PACT'$n'} -rcr devlib)"'
          Note $STDOUT '	@(cd '$p' ; printf "\npact devlib in %s for %s\n" '$p $sid' ; ${PACT'$n'} -a '$sid' -rcr devlib)'
       end
    end

    Note $STDOUT '	${BARRIER}'
    Note $STDOUT '	${PACT1} -f ${MkFile1} link'
    Note $STDOUT ''

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

# link target

    Note $STDOUT 'link:'
    Note $STDOUT '	@(echo "" ; echo "Link Executables for '$PSY_ID'")'
    Note $STDOUT '	${BARRIER}'

    foreach p ($Pck)
       @ i = 0
       @ n = 0
       while ($i < $ni)
          @ n = $n + 1
          @ i = $i + 1
          set cfg = $inf[$i]
          @ i = $i + 1
          set sid = $inf[$i]

          set root = $base/$sid 
          set etcd = $root/etc
   
#          Note $STDOUT '	@/bin/csh -c "(cd '$p' ; echo '"''"' ; echo '"'"'pact link in '$p' for '$sid"'"' ; source ${Env'$n'} ; ${PACT'$n'} -rcr link)"'
          Note $STDOUT '	@(cd '$p' ; printf "\npact link in %s for %s\n" '$p $sid' ; ${PACT'$n'} -a '$sid' -rcr link)'
       end
    end

    Note $STDOUT '	${BARRIER}'
    Note $STDOUT '	${PACT1} -f ${MkFile1} unilib'
    Note $STDOUT ''

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

# unilib target

    Note $STDOUT 'unilib:'

    @ i = 0
    @ n = 0
    while ($i < $ni)
       @ n = $n + 1
       @ i = $i + 1
       set cfg = $inf[$i]
       @ i = $i + 1
       set sid = $inf[$i]

       set root = $base/$sid 
       set etcd = $root/etc
   
       Note $STDOUT '	@(cd manager ; ./pact-lib -a '$sid')'
    end

    Note $STDOUT ''

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

# install added platforms

   Note $STDOUT 'install:'

   @ i = 0
   while ($i < $ni)
      @ i = $i + 1
      set cfg = $inf[$i]
      @ i = $i + 1
      set sid = $inf[$i]

      set root = $base/$sid 
      set etcd = $root/etc
   
      Note $STDOUT '	@(echo "Installing PACT on '$sid'" ; cd manager ; dsys install -a '$sid')'
   end

   Note $STDOUT ''

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

# remove added platforms

   Note $STDOUT 'remove:'

   @ i = 2
   while ($i < $ni)
      @ i = $i + 1
      set cfg = $inf[$i]
      @ i = $i + 1
      set sid = $inf[$i]

      set root = $base/$sid 
      set etcd = $root/etc
   
      Note $STDOUT '	@(cd manager ; dsys remove -a '$sid')'
   end

   Note $STDOUT ''

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

# test target

    Note $STDOUT 'test:'

    foreach p ($Pck)
       @ i = 0
       @ n = 0
       while ($i < $ni)
          @ n = $n + 1
          @ i = $i + 1
          set cfg = $inf[$i]
          @ i = $i + 1
          set sid = $inf[$i]

          set root = $base/$sid 
          set etcd = $root/etc
   
          Note $STDOUT '	@/bin/csh -c "(cd '$p' ; printf '"'"'\npact test in %s for %s\n'"'" $p $sid' ; source ${Env'$n'} ; ${PACT'$n'} -a '$sid' -rcr test)"'
#          Note $STDOUT '	@/bin/csh -c "(cd '$p' ; source ${Env'$n'} ; ${PACT'$n'} -a '$sid' -rcr test)"'
#          Note $STDOUT '	@(cd '$p' ; printf "\npact test in %s for %s\n" '$p $sid' ; ${PACT'$n'} -a '$sid' -rcr test)'
#          Note $STDOUT '	@(cd '$p' ; echo "" ; echo "pact test in '$p' for '$sid'" ; ${PACT'$n'} -a '$sid' -rcr test)'
       end
    end

    Note $STDOUT ''

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

source $Me:h/post-common

exit(0)

