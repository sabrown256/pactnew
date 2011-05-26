!
! GM-BL1.F - F90 interfaces for bl1
!          - NOTE: this file was automatically generated
!          - any manual changes will not be effective
!

module types_bl1
   integer, parameter :: isizea = 8
end module types_bl1

module pact_bl1
   use iso_c_binding

   use types_bl1

   integer(isizea), external :: wfavp1f

   interface

      subroutine fav1() bind(c, name='fav1')
         use iso_c_binding
         implicit none
      end subroutine fav1

      subroutine wfav1f()
         implicit none
      end subroutine wfav1f

      integer (C_INT) function fav2() bind(c, name='fav2')
         use iso_c_binding
         implicit none
      end function fav2

      function wfav2f()
         implicit none
         integer         :: wfav2f
      end function wfav2f

      real (C_DOUBLE) function fav3() bind(c, name='fav3')
         use iso_c_binding
         implicit none
      end function fav3

      function wfav3f()
         implicit none
         real            :: wfav3f
      end function wfav3f

      integer (C_INT) function fav4() bind(c, name='fav4')
         use iso_c_binding
         implicit none
      end function fav4

      function wfav4f()
         implicit none
         integer         :: wfav4f
      end function wfav4f

      type (C_PTR) function favp1() bind(c, name='favp1')
         use iso_c_binding
         implicit none
      end function favp1

      type (C_PTR) function favp2() bind(c, name='favp2')
         use iso_c_binding
         implicit none
      end function favp2

      function wfavp2f()
         use types_bl1
         implicit none
         integer(isizea) :: wfavp2f
      end function wfavp2f

      type (C_PTR) function favp3() bind(c, name='favp3')
         use iso_c_binding
         implicit none
      end function favp3

      function wfavp3f()
         use types_bl1
         implicit none
         integer(isizea) :: wfavp3f
      end function wfavp3f

      type (C_PTR) function favp4() bind(c, name='favp4')
         use iso_c_binding
         implicit none
      end function favp4

      function wfavp4f()
         use types_bl1
         implicit none
         integer(isizea) :: wfavp4f
      end function wfavp4f

   end interface

end module pact_bl1

