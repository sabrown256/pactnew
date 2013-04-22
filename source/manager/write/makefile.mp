#!/bin/csh -f
#
# MAKEFILE - write the distribution multi-platform MAKEFILE
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
source $ScrDir/functions-pact.csh

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

Separator $Log

NoteD $Log "   Distribution Makefile - Makefile"
Note $Log ""

dbmget $Log SysDir^     \
            Platforms

echo "mf-> a $Log"
echo "mf-> b $SysDir"
echo "mf-> c $Platforms"

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

set inf  = ( `echo "$Platforms" | sed 's|[:()]| |g'` )
@ ni = $#inf

set base = $SysDir:h
set etcb = $SysDir/etc
set binb = $SysDir/bin

set STDOUT = $etcb/Makefile
flog $Log $RM $STDOUT
flog $Log touch $STDOUT

   Note $STDOUT "#"
   Note $STDOUT "# MAKEFILE - operate on this distribution"
   Note $STDOUT "#"
   Note $STDOUT ""

   Note $STDOUT "PACT = $bind/pact"
   Note $STDOUT ""

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

   Note $STDOUT 'build:'
   Note $STDOUT '	@(echo "" ; echo "Generate sources")'
   Note $STDOUT '	${BARRIER}'

   @ i = 0
   while ($i < $ni)
      @ i = $i + 1
      set cfg = $inf[$i]
      @ i = $i + 1
      set sid = $inf[$i]

      set root = $base/$sid 
      set etcd = $root/etc
      set bind = $root/bin
   
      set Pck = ( `cat $etcd/package` )

      foreach i ($Pck)
         Note $STDOUT '	@(cd '$i' ; echo "" ; echo "pact generate in '$i'" ; ${PACT} -rcr generate)'
         if ($i == score) then
            Note $STDOUT '	${BARRIER}'
         endif
      end
   end
   Note $STDOUT '	${BARRIER}'

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

   Note $STDOUT '	@(echo "" ; echo "Install headers")'
   Note $STDOUT '	${BARRIER}'

   @ i = 0
   while ($i < $ni)
      @ i = $i + 1
      set cfg = $inf[$i]
      @ i = $i + 1
      set sid = $inf[$i]

      set root = $base/$sid 
      set etcd = $root/etc
      set bind = $root/bin
   
      set Pck = ( `cat $etcd/package` )

      foreach i ($Pck)
         Note $STDOUT '	@(cd '$i' ; echo "" ; echo "pact incinstall in '$i'" ; ${PACT} -rcr incinstall)'
      end
   end
   Note $STDOUT '	${BARRIER}'

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

   Note $STDOUT '	@(echo "" ; echo "Compile sources")'
   Note $STDOUT '	${BARRIER}'

   @ i = 0
   while ($i < $ni)
      @ i = $i + 1
      set cfg = $inf[$i]
      @ i = $i + 1
      set sid = $inf[$i]

      set root = $base/$sid 
      set etcd = $root/etc
      set bind = $root/bin
   
      set Pck = ( `cat $etcd/package` )

      foreach i ($Pck)
         Note $STDOUT '	@(cd '$i' ; echo "" ; echo "pact devlib in '$i'" ; ${PACT} -rcr devlib)'
      end
   end
   Note $STDOUT '	${BARRIER}'

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

   Note $STDOUT '	@(echo "" ; echo "Link Executables")'
   Note $STDOUT '	${BARRIER}'

   @ i = 0
   while ($i < $ni)
      @ i = $i + 1
      set cfg = $inf[$i]
      @ i = $i + 1
      set sid = $inf[$i]

      set root = $base/$sid 
      set etcd = $root/etc
      set bind = $root/bin
   
      set Pck = ( `cat $etcd/package` )

      foreach i ($Pck)
         Note $STDOUT '	@(cd '$i' ; echo "" ; echo "pact link in '$i'" ; ${PACT} -rcr link)'
      end
   end
   Note $STDOUT ''

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

   Note $STDOUT 'link:'
   Note $STDOUT '	@(echo "" ; echo "Link Executables")'
   Note $STDOUT '	${BARRIER}'

   @ i = 0
   while ($i < $ni)
      @ i = $i + 1
      set cfg = $inf[$i]
      @ i = $i + 1
      set sid = $inf[$i]

      set root = $base/$sid 
      set etcd = $root/etc
      set bind = $root/bin
   
      set Pck = ( `cat $etcd/package` )

      foreach i ($Pck)
         Note $STDOUT '	@(cd '$i' ; echo "" ; echo "pact link in '$i'" ; ${PACT} -rcr link)'
      end
   end
   Note $STDOUT ''

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

echo "mf-> z"

exit(0)

