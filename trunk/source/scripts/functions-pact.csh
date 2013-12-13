#
# FUNCTIONS-PACT.CSH - a standard set of functions for PACT CSH scripts
#                    - defines useful functions for command logging, etc
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#

# turn off echo and/or verbose mode

if ($?echo == 1) then
   unset echo
   set re_echo = TRUE
else
   set re_echo = FALSE
endif

if ($?verbose == 1) then
   unset verbose
   set re_verbose = TRUE
else
   set re_verbose = FALSE
endif

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

unalias cd
unalias ls
unalias rm
unalias mv

# nasty bootstrapping issue here when building PACT for the first time
# we are not supposed to be doing a lot of work in this script
#setenv PERDB `where perdb | head -n 1`
if ($?PERDB == 0) then
   setenv PERDB perdb
endif
if ($?GEXEC == 0) then
   setenv GEXEC gexec
endif
setenv MV     "mv -f"
setenv RM     "rm -f"
setenv RMDir  "rm -rf"

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

alias get_line  'set \!\!:1 = `awk "NR == \!\!:3 { print ; exit }" \!\!:2`'
alias flog      'echo "Command: \!\!:2-$" >>& \!\!:1 ; \!\!:2-$ >>& \!\!:1'
alias ftrace    'echo "Command: strace -f \!\!:2-$" >>& \!\!:1 ; strace -f \!\!:2-$ >>& \!\!:1'
alias ftty      'echo "Command: \!\!:2-$" >>& \!\!:1 ; echo "\!\!:2-$" ; \!\!:2-$ >>& \!\!:1'
alias Note      'echo \!\!:2-$ >>& \!\!:1'
alias NoteD     'echo \!\!:2-$ >>& \!\!:1 ; echo \!\!:2-$'
alias PrintF    'printf \!\!:2-$ >>& \!\!:1'
alias Separator 'echo "--------------------------------------------------------------------------" >>& \!\!:1 ; echo "" >>& \!\!:1'
alias InitLog   'set t = \!\!:2 ; rm -f $t ; echo $USER >&! $t ; date >> $t ; pwd >> $t ; echo "" >> $t ; setenv \!\!:1 $t ; unset t'
alias Intervene 'test -f ~/\!\!:1 && source ~/\!\!:1'
alias SafeSet   'if ($?\!\!:1 == 0) set \!\!:1 = \!\!:2-$'

alias gtee      'echo "Command: $GEXEC \!\!:2-$" >>& \!\!:1 ; $GEXEC \!\!:2-$ @b tee -ai \!\!:1 ; set gstatus = $status'
alias ftee      'echo "Command: $GEXEC \!\!:2-$" >>& \!\!:1 ; $GEXEC -nv \!\!:2-$ @b tee -ai \!\!:1 ; set gstatus = $status'
#alias ftee      'echo "Command: \!\!:2-$" >>& \!\!:1 ; \!\!:2-$ |& tee -ai \!\!:1'

# dbset sets a variable in the database and the current environment
# use dbset instead of setenv
# matches C call dbset
# usage: dbset <var> <value>

alias dbset     'set t_ = ( `$PERDB \!\!:1 = \!\!:2-$` ) ; setenv \!\!:1 "$t_" ; unset t_'
alias dbsets    'set \!\!:1 = ( `$PERDB -e \!\!:1 = \!\!:2-$` )'

# dbget imports a database variable into the current environment
# use dbget before referencing the variable
# matches C call dbget
# usage: dbget <var>
#        echo $<var>

alias dbget     'setenv \!\!:1 `$PERDB -e \!\!:1`'
alias dbgets    'set \!\!:1 = ( `$PERDB -e \!\!:1` )'

# dbunset remove a database variable from the database
# and the current environment
# use dbunset instead of unsetenv
# matches C call dbunset
# usage: dbunset <var>
#        echo $<var>

alias dbunset     '$PERDB del:\!\!:1 >& /dev/null ; unsetenv \!\!:1'
alias dbunsets    '$PERDB del:\!\!:1 >& /dev/null  ; unset \!\!:1'

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

alias dbini     'set u_ = \!\!:2-$ ; set t_ = ( `$PERDB \!\!:1 =\? "$u_"` ) ; setenv \!\!:1 "$t_" ; unset t_ ; unset u_'
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

alias fexec     'echo "Command: $GEXEC \!\!:2-$" >>& \!\!:1 ; $GEXEC \!\!:2-$ >>& \!\!:1 ; set gstatus = (`$PERDB -e gstatus`)'

# fexvar logs the gexec command, runs it,
# sets <var> to the result, and
# gets gstatus out as a shell variable
# usage: fexvar $log <var> <gexec-specs>

alias fexvar    'echo "Command: $GEXEC \!\!:3-$ @b vw:\!\!:2" >>& \!\!:1 ; $GEXEC \!\!:3-$ @b vw:\!\!:2 >>& \!\!:1 ; setenv \!\!:2 "`$PERDB -e \!\!:2`" ; set gstatus = (`$PERDB -e gstatus`)'

alias fexvars    'echo "Command: $GEXEC \!\!:3-$ @b vw:\!\!:2" >>& \!\!:1 ; $GEXEC \!\!:3-$ @b vw:\!\!:2 >>& \!\!:1 ; set \!\!:2 = (`$PERDB -e \!\!:2`) ; set gstatus = (`$PERDB -e gstatus`)'

alias dbmget 'echo "Command: dbmget $PERDB ^fmt:csh^ \!\!:2-$" >>& \!\!:1 ; set t_ = ( `$PERDB ^fmt:csh^ \!\!:2-$` ) ; echo "$t_" >>& \!\!:1 ; eval $t_ ; echo "dbmget status = $status" >>& \!\!:1 ; unset t_'

alias dbmset 'echo "Command: dbmset $PERDB ^fmt:db^ \!\!:2-$" >>& \!\!:1 ; $PERDB ^fmt:db^ \!\!:2-$ >>& \!\!:1 ; echo "dbmset status = $status" >>& \!\!:1'


#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

# restore echo and/or verbose mode

if ($re_echo == TRUE) then
   set echo
endif
unset re_echo

if ($re_verbose == TRUE) then
   set verbose
endif
unset re_verbose

