#!/usr/bin/env perl
#
# FNC.PL - test functions-pact.pl
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#

require "../../functions-pact.pl";

my $err = 0;
my $cwd = `pwd`;
chop($cwd);

$ENV{PERDB_PATH} = "$cwd/fnc-pl";

$log = InitLog("$cwd/log.fnc.pl");

Separator($log);

my $tmp = "$cwd/tmp-$$";

Note($log, "Test MV");
flog($log, "touch $tmp");
flog($log, "$MV $tmp $tmp.1");

Note($log, "Test MV");
flog($log, "$RM $tmp.1");

Note($log, "Test RMDir");
flog($log, "mkdir $tmp");
flog($log, "$RMDir $tmp");

Separator($log);

Note($log, "Test Note");
NoteD($log, "Test NoteD");

Separator($log);

flog($log, 'echo "Test flog"');
ftee($log, 'echo "Test ftee"');
ftty($log, 'echo "Test ftty"');

Separator($log);

dbset(foo, "a b c");
flog($log, "printenv foo");
flog($log, "unsetenv foo");

dbsets(bar, "x y z");
flog($log, "echo $bar");
flog($log, "unset bar");

dbget(foo);
flog($log, "printenv foo");
flog($log, "unsetenv foo");

dbgets(bar);
flog($log, "echo $bar");
flog($log, "unset bar");

dbdef(foo);
if ($status == 0) {
   Note($log, "foo defined");
};

dbini(foo, "1 2 3");
flog($log, "printenv foo");
flog($log, "unsetenv foo");

dbini(baz, "4 5 6");
flog($log, "printenv baz");
flog($log, "unsetenv baz");

envini(foo, "1 2 3");
flog($log, "printenv foo");
flog($log, "unsetenv foo");

envini(baz, "4 5 6");
flog($log, "printenv baz");
flog($log, "unsetenv baz");

#alias SafeSet

dbexp(HOME);

fexec($log, 'echo "foo" @b cat -n');
Note($log, "gstatus = $gstatus");

flog($log, "perdb quit:");

exit($err)
