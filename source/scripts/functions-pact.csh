#
# FUNCTIONS-PACT.CSH - a standard set of functions for PACT CSH scripts
#                    - defines useful functions for command logging, etc
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#

unalias cd
unalias ls
unalias rm
unalias mv

# nasty bootstrapping issue here when building PACT for the first time
# we are not supposed to be doing a lot of work in this script
#setenv PERDB `where perdb | head -n 1`
setenv PERDB perdb

# these two go together
alias GOSUB     'set tJump = \!\!:1 ; set Return = ( \!\!:2 $Return ) ; set CallArgs = \!\!:3-$ ; if ($Trace == TRUE) echo "Calling $tJump (from $Return[2])" ; goto $tJump'
alias RETURN    'set tReturn = $Return[1] ; shift Return ; if ($Trace == TRUE) echo "Returning to $tReturn (stack $Return)" ; goto $tReturn'

# and these two go together
alias GOCONT    'set tJump = \!\!:1 ; set CallArgs = \!\!:2-$ ; if ($Trace == TRUE) echo "Going to $tJump (from $Return[1])" ; goto $tJump'
alias CONTINUE  'set tReturn = $Return[1] ; if ($Trace == TRUE) echo "Continuing to $tReturn (stack $Return)" ; goto $tReturn'

set tReturn = ""
if (!($?Return)) then
   set Trace   = FALSE
   set Return  = "none"
endif

# Alias Usage:
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

alias flog      'echo "Command: \!\!:2-$" >>& \!\!:1 ; \!\!:2-$ >>& \!\!:1'
alias ftee      'echo "Command: \!\!:2-$" >>& \!\!:1 ; \!\!:2-$ |& tee -ai \!\!:1'
alias ftty      'echo "Command: \!\!:2-$" >>& \!\!:1 ; echo "\!\!:2-$" ; \!\!:2-$ >>& \!\!:1'
alias Note      'echo \!\!:2-$ >>& \!\!:1'
alias NoteD     'echo \!\!:2-$ >>& \!\!:1 ; echo \!\!:2-$'
alias Separator 'echo "--------------------------------------------------------------------------" >>& \!\!:1 ; echo "" >>& \!\!:1'
alias InitLog   'set t = \!\!:2 ; rm -f $t ; echo $USER >&! $t ; date >> $t ; pwd >> $t ; echo "" >> $t ; setenv \!\!:1 $t ; unset t'
alias Intervene 'test -f ~/\!\!:1 && source ~/\!\!:1'
alias SafeSet   'if ($?\!\!:1 == 0) set \!\!:1 = \!\!:2-$'

# dbset sets a variable in the database and the current environment
# use dbset instead of setenv
# matches C call dbset
# usage: dbset <var> <value>
#alias dbset     'setenv \!\!:1 "`$PERDB \!\!:1 = \!\!:2-$`"'
alias dbset     'set t_ = ( `$PERDB \!\!:1 = \!\!:2-$` ) ; setenv \!\!:1 "$t_" ; unset t_'
alias dbsets    'set \!\!:1 = ( `$PERDB -e \!\!:1 = \!\!:2-$` )'

# dbget imports a database variable into the current environment
# use dbget before referencing the variable
# matches C call dbget
# usage: dbget <var>
#        echo $<var>
alias dbget     'setenv \!\!:1 `$PERDB -e \!\!:1`'
alias dbgets    'set \!\!:1 = ( `$PERDB -e \!\!:1` )'

# dbdef queries database for existence of a variable
# matches C call dbdef
# usage: dbdef <var>
#        if ($status == 0) then
#           echo "defined"
#        else
#           echo "undefined"
alias dbdef     '$PERDB \!\!:1 \?'

# dbini initializes <var> to <val> in the database iff <var>
# does not already have a value
# matches C call dbinitv
# usage: dbini <var> <val>
alias dbini     'set t_ = ( `$PERDB \!\!:1 =\? \!\!:2-$` ) ; setenv \!\!:1 "$t_" ; unset t_'
alias envini    'if ($?\!\!:1 == 0) setenv \!\!:1 \!\!:2'

# dbexp export the current value of environment variable <var>
# back to the database
# matches C call dbexp
# usage: dbexp <var>
alias dbexp     'set t_ = ( `$PERDB -l \!\!:1 = $\!\!:1` )'
alias envexp    'echo "parent \!\!:1($\!\!:1)"'

# fexec logs the gexec command, runs it, and
# gets gstatus out as a shell variable
# usage: fexec $log <gexec-specs>
alias fexec     'echo "Command: gexec \!\!:2-$" >>& \!\!:1 ; gexec \!\!:2-$ >>& \!\!:1 ; set gstatus = `$PERDB -e gstatus`'

setenv MV     "mv -f"
setenv RM     "rm -f"
setenv RMDir  "rm -rf"

