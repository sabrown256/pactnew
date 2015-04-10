c
c SHFTST.F - test Fortran syntax mode
c
c
cccccccccccccccccccc
c Source Version: 3.0
c Software Release #: LLNL-CODE-422942
c
c include "cpyright.h"
c

c      call diagnose_f_grammar(1)

c--------------------------------------------------------------------------
c--------------------------------------------------------------------------

c ... test IF statements

      function test_if(x)
      integer x

      if ((0 .lt. x) .and. (x .le. 1)) then
         call printf(nil, "If test c1 x")
         call printf(nil, ", y\n")
      endif

      if ((1 .lt. x) .and. (x .le. 2)) then
         call printf(nil, "If test c2 x\n")
      else
         call printf(nil, "If test c2 y\n")
      endif

      if (2 .lt. x) then
         call printf(nil, "If test c3 x\n")
      elseif (3 .lt. x) then
         call printf(nil, "If test c3 y\n")
      endif

      if (4 .lt. x) then
         call printf(nil, "If test c4 x\n")
      elseif (5 .lt. x) then
         call printf(nil, "If test c4 y\n")
      else
         call printf(nil, "If test c4 z\n")
      endif

      return(x)
      end

c--------------------------------------------------------------------------
c--------------------------------------------------------------------------

c ... test DO statements

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
      end

c--------------------------------------------------------------------------
c--------------------------------------------------------------------------

c ... test WHILE statements

      function test_while(x)
      integer x
      integer i, j

      i = 1
      while (i .le. x)
         call printf(nil, "%d\n", i)
         i = i+1
      endwhile

      return(x)
      end

c--------------------------------------------------------------------------
c--------------------------------------------------------------------------

      call test_if(-1)
      call test_if(0)
      call test_if(1)
      call test_if(2)
      call test_if(3)
      call test_if(4)
      call test_if(5)
      call test_if(6)

      call quit
