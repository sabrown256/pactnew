!
! PCFTST.F - test connections child side in FORTRAN
!
! Source Version: 3.0
! Software Release #: LLNL-CODE-422942
!
! #include "cpyright.h"

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

      program pcftst

      character*80 msg

   10 write(6, 1000)
      read(5, 2000) msg
      write(6, 2000) msg
      if (msg .ne. 'end') goto 10

 1000 format('-> ')
 2000 format(a80)

      end

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

