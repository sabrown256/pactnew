!
! SCFFNC.F - Fortran API support routines
!
! Source Version: 3.0
! Software Release #: LLNL-CODE-422942
!
! include "cpyright.h"
!

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

module pact_libc
   use iso_c_binding

   interface

      function strlen(s) bind(C)
         use iso_c_binding
         implicit none
         integer(C_SIZE_T) :: strlen
         type(C_PTR), intent(in) :: s
      end function strlen

   end interface

end module

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

! C_CHARP_F - wrap C char * variable CP in Fortran character(M)
!           - caller must free CP

      function c_charp_f(cp, m)

      use iso_c_binding
      implicit none

      character(m) :: c_charp_f
      type(C_PTR), value, intent(in) :: cp
      integer(C_INT), value, intent(in) :: m

! ... local variables
      integer :: ln(2)
      character(m), pointer :: rv

      allocate(rv)

      ln(1) = 1
      ln(2) = m

      call c_f_pointer(cp, rv)

      c_charp_f = rv

      end function

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

! C_CHARPP_F - wrap C char ** variable CP in Fortran character(M) A(N)
!            - caller must free CP and returned space

      function c_charpp_f(n, cp, m)

      use iso_c_binding
      implicit none

      character(m), allocatable :: c_charpp_f(:)
      integer(C_INT), value, intent(in) :: n
      type(C_PTR), intent(in) :: cp
      integer(C_INT), value, intent(in) :: m

! ... local variables
      integer :: i
      integer :: ln(2)
      character(m), pointer :: s
      character(m), allocatable :: rv(:)
      type(C_PTR) :: lcp(n)

      lcp = transfer(cp, lcp)

      allocate(s)
      allocate(rv(n))

      ln(1) = 1
      ln(2) = m

      do i = 1, n
         call c_f_pointer(lcp(i), s)
         rv(i) = s
      enddo

      deallocate(s)

      c_charpp_f = rv

      end function

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

! C_STRLENC - return the length of a C string

      function c_strlenc(cp)

      use pact_libc
      implicit none

      integer :: c_strlenc
      type(C_PTR) :: cp

      c_strlenc = transfer(strlen(cp), c_strlenc)

      end function

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

! C_STRLENF - return the length of a C string

      function c_strlenf(cp)

      use pact_libc
      implicit none

      integer :: c_strlenf
      character(*), intent(in) :: cp

! ... local variables
      type(C_PTR) :: lcp

      lcp = transfer(cp, lcp)
      c_strlenf = transfer(strlen(lcp), c_strlenf)

      end function

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

