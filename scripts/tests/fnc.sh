#!/bin/sh -f
#
# FNC.SH - test functions-pact.sh
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#

. ../../functions-pact.sh

err=0
cwd=`pwd`

PERDB_PATH=$cwd/fnc-sh
export PERDB_PATH

InitLog log $cwd/log.fnc.sh

Separator $log

tmp=$cwd/tmp-$$

Note $log "Test MV"
flog $log touch $tmp
flog $log $MV $tmp $tmp.1

Note $log "Test MV"
flog $log $RM $tmp.1

Note $log "Test RMDir"
flog $log mkdir $tmp
flog $log $RMDir $tmp

Separator $log

Note $log "Test Note"
NoteD $log "Test NoteD"

Separator $log

flog $log echo "Test flog"
ftee $log echo "Test ftee"
ftty $log echo "Test ftty"

Separator $log

dbset foo "a b c"
flog $log printenv foo
flog $log unset foo

dbsets bar "x y z"
flog $log echo $bar
flog $log unset bar

dbget foo
flog $log printenv foo
flog $log unset foo

dbgets bar
flog $log echo $bar
flog $log unset bar

dbdef foo
if [ $? = 0 ] ; then
   Note $log "foo defined"
fi

dbini foo "1 2 3"
flog $log printenv foo
flog $log unset foo

dbini baz "4 5 6"
flog $log printenv baz
flog $log unset baz

envini foo "1 2 3"
flog $log printenv foo
flog $log unset foo

envini baz "4 5 6"
flog $log printenv baz
flog $log unset baz

#alias SafeSet

dbexp HOME

fexec $log echo "foo" @b cat -n
Note $log gstatus = $gstatus

flog $log perdb quit:

exit $err
