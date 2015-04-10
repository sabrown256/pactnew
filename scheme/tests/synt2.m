#
# SYNT2 - BASIS syntax test #2
#       - test if statements and logical expressions
#
#
####################
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#

function foo(x)

   if (x .gt. 1) then
      << "A " << x  << " is greater than 1"
   endif

   if (x .gt. 2) then
      << "B " << x  << " is greater than 2"
   else
      << "B " << x  << " is less than or equal 2"
   endif

   if (x .gt. 3) then
      << "C " << x  << " is greater than 3"
   elseif (x .gt. 4) then
      << "C " << x  << " is greater than 4"
   endif

   if (x .gt. 6) then
      << "D " << x  << " is greater than 6"
   elseif (x .gt. 5) then
      << "D " << x  << " is greater than 5"
   else
      << "D " << x  << " is less than or equal 5"
   endif

endf

foo(1)
foo(2)
foo(3)
foo(4)
foo(5)
foo(6)
foo(7)

# NOTE: this doesn't test precedence

function bar(x)

   if ((x .lt. 0) .or. (x .gt. 1)) then
      << "E " << x  << " is greater than 1 or less than 0"
   endif

   if ((x .gt. 2) .and. (x .lt. 4)) then
      << "F " << x  << " is greater than 2 and less than 4"
   endif

   if (.not. ((x .lt. 0) .or. (x .gt. 1))) then
      << "G " << x  << " is not greater than 1 or less than 0"
   endif

endf

bar(-1)
bar(0)
bar(1)
bar(2)
bar(3)
bar(4)
bar(5)

quit
