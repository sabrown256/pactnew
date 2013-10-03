#!/bin/csh -f
#
# MAKEFILE.MP - write the multi-platform distribution MAKEFILE
#
# include "cpyright.h"
#

set Me = $0
source $Me:h/common

Note $Log "----- write/makefile.mp -----"
Note $Log "Write: makefile.mp"
Note $Log ""

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

Separator $Log

NoteD $Log "   Distribution Makefile - Makefile"
Note $Log ""

dbmget $Log SysDir^     \
            Platforms

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

   set base = $SysDir:h
   set etcb = $SysDir/etc
   set binb = $SysDir/bin

   set inf  = ( `echo "$Platforms" | sed 's|:| |g'` )
   @ np = $#inf
   Note $Log "$np platforms"

   set inf  = ( `echo "$Platforms" | sed 's|[:()]| |g'` )
   @ ni = $#inf
   Note $Log "$ni platforms components"

# get the package list
   set sid  = $inf[2]
   set root = $base/$sid 
   set etcd = $root/etc
   set bind = $root/bin
   set Pck  = ( `cat $etcd/package` )

# set the pact links
   set pact = $bind/pact
   @ i = 2
   while ($i < $ni)
      @ i = $i + 1
      set cfg = $inf[$i]
      @ i = $i + 1
      set sid = $inf[$i]

      set root = $base/$sid 
      set bind = $root/bin
      flog $Log pushd $bind
      flog $Log mv pact pact.cfg
      flog $Log ln -s $pact pact
      flog $Log popd
   end

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

   set STDOUT = $etcb/Makefile
   flog $Log $RM $STDOUT
   flog $Log touch $STDOUT

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
   end

   Note $STDOUT "PACT    = $binb/pact"
   Note $STDOUT "MkFile  = $etcb/Makefile"
   Note $STDOUT ""

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

# build target

   Note $STDOUT 'build:'
   Note $STDOUT '	@(echo "" ; echo "Generate sources")'
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
   
         Note $STDOUT '	@(cd '$p' ; echo "" ; echo "pact generate in '$p' for '$sid'" ; ${PACT'$n'} -rcr generate)'
      end
      if ($p == score) then
         Note $STDOUT '	${BARRIER}'
      endif
   end
   Note $STDOUT '	${BARRIER}'

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

   Note $STDOUT '	@(echo "" ; echo "Install headers")'
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
   
         Note $STDOUT '	@(cd '$p' ; echo "" ; echo "pact incinstall in '$p' for '$sid'" ; ${PACT'$n'} -rcr incinstall)'
      end
   end
   Note $STDOUT '	${BARRIER}'

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

   Note $STDOUT '	@(echo "" ; echo "Compile sources")'
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
   
         Note $STDOUT '	@(cd '$p' ; echo "" ; echo "pact devlib in '$p' for '$sid'" ; ${PACT'$n'} -rcr devlib)'
      end
   end
   Note $STDOUT '	${BARRIER}'
   Note $STDOUT '	${PACT} -f ${MkFile} link'

   Note $STDOUT ''

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

# link target

   Note $STDOUT 'link:'
   Note $STDOUT '	@(echo "" ; echo "Link Executables")'
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
   
         Note $STDOUT '	@(cd '$p' ; echo "" ; echo "pact link in '$p' for '$sid'" ; ${PACT'$n'} -rcr link)'
      end
   end

   Note $STDOUT '	${BARRIER}'
   Note $STDOUT '	${PACT} -f ${MkFile} unilib'

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
   
      Note $STDOUT '	${PACT'$n'} -f ${MkFile'$n'} unilib'
   end
   Note $STDOUT ''

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

# install target

   Note $STDOUT 'install:'

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
   
      Note $STDOUT '	@(echo "Installing PACT on '$sid' ; cd manager ; dsys install -a '$sid')'
   end
   Note $STDOUT ''

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

# remove target

   Note $STDOUT 'remove:'

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
   
         Note $STDOUT '	@(cd '$p' ; echo "" ; echo "pact test in '$p' for '$sid'" ; ${PACT'$n'} -a '$sid' -rcr test)'
      end
   end
   Note $STDOUT ''

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

exit(0)

