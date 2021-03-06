#
# FUNCTIONS-PACT.SH - a standard set of functions for PACT BASH scripts
#                   - defines useful functions for command logging, etc
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#

PERDB=perdb
MV="mv -f"
RM="rm -f"
RMDir="rm -rf"

export PERDB
export MV
export RM
export RMDir

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

flog () {
   log_=$1
   shift
   cmd=$*
   echo "Command: $cmd" >> $log_ 2>&1
   $cmd >> $log_ 2>&1
}

ftee () {
   log_=$1
   shift
   cmd=$*
   echo "Command: $cmd" >> $log_ 2>&1
   $cmd 2>&1 | tee -ai $log_
}

ftty () {
   log_=$1
   shift
   cmd=$*
   echo "Command: $cmd" >> $log_ 2>&1
   echo "$cmd"
   $cmd >> $log_ 2>&1
}

Note () {
   log_=$1
   shift
   msg=$*
   echo $msg >> $log_ 2>&1
}

PrintF () {
   log_=$1
   shift
   msg=$*
   printf $msg >> $log_ 2>&1
}

NoteD () {
   log_=$1
   shift
   msg=$*
   echo $msg >> $log_ 2>&1
   echo $msg
}

Separator () {
   log_=$1
   echo "--------------------------------------------------------------------------" >> $log_ 2>&1
   echo "" >> $log_ 2>&1
}

InitLog () {
   log_=$1
   file=$2
   rm -f $file
   echo $USER > $file 2>&1
   date >> $file
   pwd >> $file
   echo "" >> $file
   eval "$log_=$file"
   export $log_
}

# dbset sets a variable in the database and the current environment
# use dbset instead of setenv
# matches C call dbset
# usage: dbset <var> <value>

dbset () {
   var=$1
   shift
   val=$*
   eval "$var='`$PERDB $var = $val`'"
   export $var
}

dbsets () {
   var=$1
   shift
   val=$*
   eval "$var='`$PERDB -e $var = $val`'"
}

# dbget imports a database variable into the current environment
# use dbget before referencing the variable
# matches C call dbget
# usage: dbget <var>
#        echo $<var>

dbget () {
   var=$1
   eval "$var='`$PERDB -e $var`'"
   export $var
}

dbgets () {
   var=$1
   eval "$var='`$PERDB -e $var`'"
}

# dbunset removes a variable from the database and the current environment
# use dbunset instead of unsetenv
# matches C call dbunset
# usage: dbunset <var>

dbunset () {
   var=$1
   $PERDB del:$var > /dev/null 
   unset $var
}

dbunsets () {
   var=$1
   $PERDB del:$var > /dev/null 
   unset -f $var
}

# dbdef queries database for existence of a variable
# matches C call dbdef
# usage: dbdef <var>
#        if [ $? == 0 ] ; then
#           echo "defined"
#        else
#           echo "undefined"
#        fi

dbdef () {
   var=$1
   $PERDB $var \?
   return $?
}

# dbini initializes <var> to <val> in the database iff <var>
# does not already have a value
# matches C call dbinitv
# usage: dbini <var> <val>

dbini () {
   var=$1
   shift
   val=$*
   eval "$var='`$PERDB $var =\? $val`'"
   export $var
}

envini () {
   var=$1
   val=$2
   if [ $var ] ; then
      eval "$var='$val'"
      export $var
   fi
}

# dbexp export the current value of environment variable <var>
# back to the database
# matches C call dbexp
# usage: dbexp <var>
dbexp () {
   var=$1
   val=`printenv $var`
   t_="`$PERDB -l $var = $val`"
   unset t_
}

# fexec logs the gexec command, runs it, and
# gets gstatus out as a shell variable
# usage: fexec $log <gexec-specs>

fexec () {
   log_=$1
   shift
   cmd=$*
   echo "Command: gexec $cmd" >> $log_ 2>&1
   gexec $cmd >> $log_ 2>&1
   gstatus=`$PERDB -e gstatus`
}

# fexvar logs the gexec command, runs it,
# sets <var> to the result, and
# gets gstatus out as a shell variable
# usage: fexvar $log <var> <gexec-specs>

fexvar () {
   log_=$1
   shift
   var=$1
   shift
   cmd=$*
   echo "Command: gexec $cmd @b vw:$var" >> $log_ 2>&1
   gexec $cmd @b vw:$var >> $log_ 2>&1
   gstatus=`$PERDB -e gstatus`
   eval "$var='`$PERDB -e $var`'"
   export $var
}

fexvars () {
   log_=$1
   shift
   var=$1
   shift
   cmd=$*
   echo "Command: gexec $cmd @b vw:$var" >> $log_ 2>&1
   gexec $cmd @b vw:$var >> $log_ 2>&1
   gstatus=`$PERDB -e gstatus`
   eval "$var='`$PERDB -e $var`'"
}

