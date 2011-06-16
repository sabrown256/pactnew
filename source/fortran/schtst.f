!
! SCHTST.F - test the SCORE hash function Fortran API
!
! Source Version: 3.0
! Software Release #: LLNL-CODE-422942
!
! include "cpyright.h"
!

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

! SCHTST - main test routine

      program schtst
      use pact_fortran
      implicit none 

      integer :: tab, sflgs
      real*8 :: y
!      real*8 :: x, z
!      pointer (ipx, x(3))
!      pointer (ipz, z(3))
      integer :: LAST
      data  LAST /0/

!      if (scmake(ipx, 3, 8) .eq. 0) then
!         stop 1
!      endif

!      x(1) =  8.0
!      x(2) =  9.0
!      x(3) = 10.0

      sflgs = 3
      tab   = scmkht(127, 0, sflgs)

      if (schins(tab, 1, 'a', 3, 3, 'int', 0) .eq. 0) &
         stop 1

      if (schins(tab, 1, 'b', 3.14159d0, 6, 'double', 0) .eq. 0) &
         stop 1

!      if (schins(tab, 1, 'c', x(1), 6, 'double', 0) .eq. 0) &
!         stop 1

!      if (schins(tab, 1, 'd', ipx, 8, 'double *', 0) .eq. 0) &
!         stop 1

      y = 0.0d0
      if (schlkp(tab, y, 1, 'b') .eq. 0) &
         stop 1

      write(6, 100) y

      y = 0.0d0
!      if (schlkp(tab, y, 1, 'c') .eq. 0) &
!         stop 1

!      write(6, 200) 0.0, x(1)
      write(6, 200) 0.0, y

!      ipz = 0
!      if (schlkp(tab, ipz, 1, 'd') .eq. 0) &
!         stop 1

!      write(6, 200) x(2), z(2)

 100  format(/, 'Expect 3.14159 and got ', f8.5)
 200  format(/, 'Expect ', f8.5, ' and got ', f8.5)

      stop 0
      end

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------
