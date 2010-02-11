#
# SUBR4.INC - part of include test SUBR4
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#

source $IncDir/csh-subroutines
HAVE_SUBROUTINES

call bar()

endinclude

bar:
   echo "here"
   return

