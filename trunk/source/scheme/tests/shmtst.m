#
# SHMTST.M - test BASIS syntax mode
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#

#      call diagnose_m_grammar(1)

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

# ... test IF statements

      function test_if(x)
      integer x

      if ((0 .lt. x) .and. (x .le. 1)) then
         call printf(nil, "If test #1 x")
         call printf(nil, ", y\n")
      endif

      if ((1 .lt. x) .and. (x .le. 2)) then
         call printf(nil, "If test #2 x\n")
      else
         call printf(nil, "If test #2 y\n")
      endif

      if (2 .lt. x) then
         call printf(nil, "If test #3 x\n")
      elseif (3 .lt. x) then
         call printf(nil, "If test #3 y\n")
      endif

      if (4 .lt. x) then
         call printf(nil, "If test #4 x\n")
      elseif (5 .lt. x) then
         call printf(nil, "If test #4 y\n")
      else
         call printf(nil, "If test #4 z\n")
      endif

      return(x)
      endf

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

# ... test DO statements

      function test_do(x)
      integer x
      integer i

      do i = 1, x
         call printf(nil, "%s\n", i)
      enddo

      do i = x, 1, -1
         call printf(nil, "%s\n", i)
      enddo

      do
         call printf(nil, "%s\n", i)
         i = i + 1
         if (i .gt. x) break
      enddo

      return(x)
      endf

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

# ... test FOR statements

      function test_for(x)
      integer x
      integer i, j

      for (i = 1, i .le. x, i = i+1)
          call printf(nil, "%d\n", i)
      endfor

      for (i = x, 1 .le. i, i = i-1)
          call printf(nil, "%d\n", i)
      endfor

      for (i = 1
           j = x,
           i .le. x,
           i = i+1
           j = j-1)
         call printf(nil, "i = %d\n", i)
         call printf(nil, "j = %d\n", j)
      endfor

      return(x)
      endf

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

# ... test WHILE statements

      function test_while(x)
      integer x
      integer i, j

      i = 1
      while (i .le. x)
         call printf(nil, "%d\n", i)
         i = i+1
      endwhile

      return(x)
      endf

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

# ... test output operator << statements

      function test_output(x)
      integer x

      << "hi "
      << "there " << 3
      << x+2
#      << "there " << x+4

      endf

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

      call test_if(-1)
      call test_if(0)
      call test_if(1)
      call test_if(2)
      call test_if(3)
      call test_if(4)
      call test_if(5)
      call test_if(6)

      call quit
