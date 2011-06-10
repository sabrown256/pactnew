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

   type(C_PTR), external :: favp1f


   interface

! ... declarations for generated wrappers
      subroutine fav1f() 
         use iso_c_binding
         implicit none
      end subroutine fav1f

      function fav2f() 
         use iso_c_binding
         implicit none
         integer         :: fav2f
      end function fav2f

      function fav3f() 
         use iso_c_binding
         implicit none
         real            :: fav3f
      end function fav3f

      function fav4f() 
         use iso_c_binding
         implicit none
         integer         :: fav4f
      end function fav4f

      function favp2f() 
         use iso_c_binding
         use types_bl1
         implicit none
         type(C_PTR)     :: favp2f
      end function favp2f

      function favp3f() 
         use iso_c_binding
         use types_bl1
         implicit none
         type(C_PTR)     :: favp3f
      end function favp3f

      function favp4f() 
         use iso_c_binding
         use types_bl1
         implicit none
         type(C_PTR)     :: favp4f
      end function favp4f

      subroutine fav5_f() 
         use iso_c_binding
         implicit none
      end subroutine fav5_f

! ... declarations for interoperability
      subroutine fav1f_i() &
                bind(c, name='fav1')
         use iso_c_binding
         implicit none
      end subroutine fav1f_i

      function fav2f_i() &
                bind(c, name='fav2')
         use iso_c_binding
         implicit none
         integer(C_INT) :: fav2f_i
      end function fav2f_i

      function fav3f_i() &
                bind(c, name='fav3')
         use iso_c_binding
         implicit none
         real(C_DOUBLE) :: fav3f_i
      end function fav3f_i

      function fav4f_i() &
                bind(c, name='fav4')
         use iso_c_binding
         implicit none
         integer(C_INT) :: fav4f_i
      end function fav4f_i

      function favp1f_i() &
                bind(c, name='favp1')
         use iso_c_binding
         implicit none
         type(C_PTR) :: favp1f_i
      end function favp1f_i

      function favp2f_i() &
                bind(c, name='favp2')
         use iso_c_binding
         implicit none
         type(C_PTR) :: favp2f_i
      end function favp2f_i

      function favp3f_i() &
                bind(c, name='favp3')
         use iso_c_binding
         implicit none
         type(C_PTR) :: favp3f_i
      end function favp3f_i

      function favp4f_i() &
                bind(c, name='favp4')
         use iso_c_binding
         implicit none
         type(C_PTR) :: favp4f_i
      end function favp4f_i

      subroutine fav5_i() &
                bind(c, name='fav5')
         use iso_c_binding
         implicit none
      end subroutine fav5_i

      subroutine fav6_i() &
                bind(c, name='fav6')
         use iso_c_binding
         implicit none
      end subroutine fav6_i

   end interface

end module pact_bl1

