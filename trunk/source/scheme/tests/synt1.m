#
# SYNT1 - BASIS syntax test #1
#       - test variable declaration, function definition,
#       - and function call syntax
#
#
####################
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#

# test declaration of single variable
      logical i

# test declaration of two variables
      real j, k

# test declaration of two variables with some initializations
      integer l, m = 2
      integer n = 3, o
      integer p = -1, q = 1

      << "m " << m
      << "n " << n
      << "p " << p
      << "q " << q

# test declarations of five variables
#      character*8 a, b, c, d, e

# test array declarations
      integer x(10)
      integer y(0:9)
      integer u(2), v(0:1), w(-1:0)

      << "u " << u
      << "v " << v
      << "w " << w

#       forget

# test no arg function declaration
      function fooa
         << "This is fooa"
      endf
      
#      call fooa
#      fooa
#      fooa;
      
# test 1 arg function declaration
      function foo1(x)
         << "This is foo1 " << x
      endf

       call diagnose_m_grammar(1)
#       call diagnose_m_parse(1)

      foo1(1)
      call foo1(2)
      foo1(3);
      foo1 command_e 4

# test 2 arg function declaration
      function foo2(x, y)
         << "This is foo2 " << x << " " << y
      endf

      foo2(1, 2)
      call foo2(2, 3)
      foo2(3, 4);
      foo2 command_ee 4 5

# test 3 arg function declaration
      function foo3(x, y, z)
         << "This is foo3 " << x << " " << y << " " << z
      endf

      foo3(1.1, 2.2, 3.3)
      call foo3(2.1, 3.2, 4.3)
      foo3(3.1, 4.2, 5.3);
      foo3 command_(e) 4.1 5.2 6.3

      quit
