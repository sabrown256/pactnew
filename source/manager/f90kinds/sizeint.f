!
! Source Version: 3.0
! Software Release #: LLNL-CODE-422942
!
! include "cpyright.h"
!

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

! SIZEINT - determine the byte size of type integer

      program sizeint
      implicit none
      integer a, i, n
      character*32 c

      equivalence(a, c)

! load the char array with a non-zero value - here it is the character A
      do i = 1, 32
         c(i:i) = char(65)
      enddo

! zero out the integer
      a = 0

! count the zeroed bytes - that will be the byte size
      n = 0
      do i = 1, 32
         if (c(i:i) .ne. char(65)) then
            n = n + 1
         endif
      enddo

! write out the integer byte size
      write(6, *) n

      end program sizeint

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

