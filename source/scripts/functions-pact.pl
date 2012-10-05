#
# FUNCTIONS-PACT.PL - a standard set of functions for PACT PERL scripts
#                   - defines useful functions for command logging, etc
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#

$PERDB = perdb;
$MV    = "mv -f";
$RM    = "rm -f";
$RMDir = "rm -rf";

# Function Usage:
#
#  flog <log> <command>
#       Text "Command: <command>"  goes to <log>
#       Command output goes to <log>
#
#  ftee <log> <command>
#       Text "Command: <command>"  goes to <log>
#       Command output goes to <log> and to TTY
#
#  ftty <log> <command>
#       Text "Command: <command>"  goes to <log>
#       Text "<command>" goes to TTY
#       Command output goes to <log>
#
#  Note <log> <text>
#       Text "<text>" goes to <log>
#
#  NoteD <log> <text>
#       Text "<text>" goes to <log> and to TTY
#
#  Separator <log>
#       Line of '-'s  goes to <log>
#
#  InitLog <log> <path>
#       Initialize a log file with variable name <log> and file name <path>
#
#  envini <var> <default>
#       If <var> is not defined, define it with value <default>
#

sub flog {
    my $log = shift;
    my $cmd = shift;
    print($log, "Command: $cmd\n");
    `$cmd >>& $log`;
}

sub ftee {
    my $log = shift;
    my $cmd = shift;
    print($log, "Command: $cmd\n");
    `$cmd |& tee -ai $log`;
}

sub ftty {
    my $log = shift;
    my $cmd = shift;
    print($log, "Command: $cmd\n");
    print($log, "$cmd\n");
    `$cmd >>& $log`;
}

sub Note {
    my $log = shift;
    my $msg = shift;
    print($log, $msg);
}

sub NoteD {
    my $log = shift;
    my $msg = shift;
    print($log, $msg);
    print($msg);
}

sub Separator {
    my $log = shift;
    print($log, "--------------------------------------------------------------------------\n");
    print($log, "");
}

sub InitLog {
    my $log = shift;
    my $file = shift;
    rm -f $file;
    echo $USER >&! $file;
    date >> $file;
    pwd >> $file;
    echo "" >> $file;
    $ENV{$log} = $file;
}

# dbset sets a variable in the database and the current environment
# use dbset instead of setenv
# matches C call dbset
# usage: dbset <var> <value>

sub dbset {
    my $var = shift;
    my $val = shift;
    $ENV{$var} = `$PERDB $var = $val`;
}

sub dbsets {
    my $var = shift;
    my $val = shift;
    ${$var} = `$PERDB $var = $val`;
}

# dbget imports a database variable into the current environment
# use dbget before referencing the variable
# matches C call dbget
# usage: dbget <var>
#        echo $<var>

sub dbget {
    my $var = shift;
    $ENV{$var} = `$PERDB -e $var`;
}

sub dbgets {
    my $var = shift;
    ${$var} = `$PERDB -e $var`;
}

# dbdef queries database for existence of a variable
# matches C call dbdef
# usage: dbdef <var>
#        if ($status == 0) then
#           echo "defined"
#        else
#           echo "undefined"

sub dbdef {
    my $var = shift;
    $PERDB $var \?
}

# dbini initializes <var> to <val> in the database iff <var>
# does not already have a value
# matches C call dbinitv
# usage: dbini <var> <val>

sub dbini {
    my $var = shift;
    my $val = shift;
    my $t = `$PERDB $var =\? $val`;
    setenv $var "$t"
}

sub envini {
    my $var = shift;
    my $val = shift;
    if ($?$var == 0)
       {setenv $var $val;};
}

# dbexp export the current value of environment variable <var>
# back to the database
# matches C call dbexp
# usage: dbexp <var>

sub dbexp {
    my $var = shift;
    my $val = ${$var};
    `$PERDB -l $var = $val`;
}

sub envexp {
    my $var = shift;
    my $val = ${$var};
    print "parent $var($val)"'
}

# fexec logs the gexec command, runs it, and
# gets gstatus out as a shell variable
# usage: fexec $log <gexec-specs>

sub fexec {
    my $log = shift;
    my $cmd = shift;
    print($log, "Command: gexec $cmd\n");
    gexec $cmd >>& $log;
    $gstatus = `$PERDB -e gstatus`;
}

