#!/bin/csh -f
#
# FNC.CSH - test functions-pact.csh
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#

unalias *
source ../../functions-pact.csh

@ err = 0

setenv PERDB_PATH $cwd/fnc-csh

InitLog log $cwd/log.fnc.csh

Separator $log

set tmp = $cwd/tmp-$$

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

#alias Intervene

Separator $log

dbset foo "a b c"
flog $log printenv foo
flog $log unsetenv foo

dbsets bar "x y z"
flog $log echo $bar
flog $log unset bar

dbget foo
flog $log printenv foo
flog $log unsetenv foo

dbgets bar
flog $log echo $bar
flog $log unset bar

dbdef foo
if ($status == 0) then
   Note $log "foo defined"
endif

dbini foo "1 2 3"
flog $log printenv foo
flog $log unsetenv foo

dbini baz "4 5 6"
flog $log printenv baz
flog $log unsetenv baz

envini foo "1 2 3"
flog $log printenv foo
flog $log unsetenv foo

envini baz "4 5 6"
flog $log printenv baz
flog $log unsetenv baz

#alias SafeSet

dbexp HOME

setenv Foo "the quick brown fox"
envexp Foo

fexec $log echo "foo" @b cat -n
Note $log gstatus = $gstatus

flog $log perdb quit:
flog $log rm -f fnc-csh.*

exit($err)
