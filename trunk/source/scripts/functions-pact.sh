#
# FUNCTIONS-PACT.SH - a standard set of functions for PACT BASH scripts
#                   - defines useful functions for command logging, etc
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#

export PERDB=perdb
export MV="mv -f"
export RM="rm -f"
export RMDir="rm -rf"

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
   log=$1
   shift
   cmd=$*
   echo "Command: $cmd" >> $log 2>&1
   $cmd >> $log 2>&1
}

ftee () {
   log=$1
   shift
   cmd=$*
   echo "Command: $cmd" >> $log 2>&1
   $cmd 2>&1 | tee -ai $log
}

ftty () {
   log=$1
   shift
   cmd=$*
   echo "Command: $cmd" >> $log 2>&1
   echo "$cmd"
   $cmd >> $log 2>&1
}

Note () {
   log=$1
   shift
   msg=$*
   echo $msg >> $log 2>&1
}

NoteD () {
   log=$1
   shift
   msg=$*
   echo $msg >> $log 2>&1
   echo $msg
}

Separator () {
   log=$1
   echo "--------------------------------------------------------------------------" >> $log 2>&1
   echo "" >> $log 2>&1
}

InitLog () {
   log=$1
   file=$2
   rm -f $file
   echo $USER >& $file
   date >> $file
   pwd >> $file
   echo "" >> $file
   export $log=$file
}

# dbset sets a variable in the database and the current environment
# use dbset instead of setenv
# matches C call dbset
# usage: dbset <var> <value>

dbset () {
   var=$1
   shift
   val=$*
   export $var="`$PERDB $var = $val`"
}

dbsets () {
   var=$1
   shift
   val=$*
   export -n $var="`$PERDB -e $var = $val`"
}

# dbget imports a database variable into the current environment
# use dbget before referencing the variable
# matches C call dbget
# usage: dbget <var>
#        echo $<var>

dbget () {
   var=$1
   export $var="`$PERDB -e $var`"
}

dbgets () {
   var=$1
   export -n $var="`$PERDB -e $var`"
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
   export $var="`$PERDB $var =\? $val`"
}

envini () {
   var=$1
   val=$2
   if [ $var ] ; then
      export $var="$val"
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

envexp () {
   var=$1
   val=`printenv $var`
   echo "parent $var($val)"
}

# fexec logs the gexec command, runs it, and
# gets gstatus out as a shell variable
# usage: fexec $log <gexec-specs>

fexec () {
   log=$1
   shift
   cmd=$*
   echo "Command: gexec $cmd" >> $log 2>&1
   gexec $cmd >> $log 2>&1
   gstatus=`$PERDB -e gstatus`
}

