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

! ... external declarations for generated wrappers
   use types_bl1

   integer(isizea), external :: favp1


   interface

! ... declarations for generated wrappers
      subroutine fav1() 
         implicit none
      end subroutine fav1

      function fav2() 
         implicit none
         integer         :: fav2
      end function fav2

      function fav3() 
         implicit none
         real            :: fav3
      end function fav3

      function fav4() 
         implicit none
         integer         :: fav4
      end function fav4

      function favp2() 
         use types_bl1
         implicit none
         integer(isizea) :: favp2
      end function favp2

      function favp3() 
         use types_bl1
         implicit none
         integer(isizea) :: favp3
      end function favp3

      function favp4() 
         use types_bl1
         implicit none
         integer(isizea) :: favp4
      end function favp4

! ... declarations for interoperability
      subroutine fav1_i() &
                bind(c, name='fav1')
         use iso_c_binding
         implicit none
      end subroutine fav1_i

      integer (C_INT) function fav2_i() &
                bind(c, name='fav2')
         use iso_c_binding
         implicit none
      end function fav2_i

      real (C_DOUBLE) function fav3_i() &
                bind(c, name='fav3')
         use iso_c_binding
         implicit none
      end function fav3_i

      integer (C_INT) function fav4_i() &
                bind(c, name='fav4')
         use iso_c_binding
         implicit none
      end function fav4_i

      type (C_PTR) function favp1_i() &
                bind(c, name='favp1')
         use iso_c_binding
         implicit none
      end function favp1_i

      type (C_PTR) function favp2_i() &
                bind(c, name='favp2')
         use iso_c_binding
         implicit none
      end function favp2_i

      type (C_PTR) function favp3_i() &
                bind(c, name='favp3')
         use iso_c_binding
         implicit none
      end function favp3_i

      type (C_PTR) function favp4_i() &
                bind(c, name='favp4')
         use iso_c_binding
         implicit none
      end function favp4_i

   end interface

end module pact_bl1

