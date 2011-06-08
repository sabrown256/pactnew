!
! GM-BL5.F - F90 interfaces for bl5
!          - NOTE: this file was automatically generated
!          - any manual changes will not be effective
!

module types_bl5
   integer, parameter :: isizea = 8
end module types_bl5

module pact_bl5
   use iso_c_binding

! ... external declarations for generated wrappers


   interface

! ... declarations for generated wrappers
      subroutine a_f1_f(a) 
         use iso_c_binding
         use types_bl5
         implicit none
         integer(isizea) :: a
      end subroutine a_f1_f

      subroutine a_f2_f(b) 
         use iso_c_binding
         use types_bl5
         implicit none
         integer(isizea) :: b
      end subroutine a_f2_f

      subroutine a_f3_f(a, b) 
         use iso_c_binding
         use types_bl5
         implicit none
         integer(isizea) :: a
         integer(isizea) :: b
      end subroutine a_f3_f

      subroutine a_f4_f(b, a) 
         use iso_c_binding
         use types_bl5
         implicit none
         integer(isizea) :: b
         integer(isizea) :: a
      end subroutine a_f4_f

      subroutine b_f1_f(a, b) 
         use iso_c_binding
         use types_bl5
         implicit none
         integer(isizea) :: a
         integer         :: b
      end subroutine b_f1_f

      subroutine b_f2_f(a, b) 
         use iso_c_binding
         use types_bl5
         implicit none
         integer(isizea) :: a
         integer         :: b
      end subroutine b_f2_f

      subroutine b_f3_f(a, b, c) 
         use iso_c_binding
         use types_bl5
         implicit none
         integer(isizea) :: a
         integer(isizea) :: b
         integer         :: c
      end subroutine b_f3_f

      subroutine b_f4_f(a, b, c) 
         use iso_c_binding
         use types_bl5
         implicit none
         integer(isizea) :: a
         integer(isizea) :: b
         integer         :: c
      end subroutine b_f4_f

      subroutine b_f5_f(a, b, c) 
         use iso_c_binding
         use types_bl5
         implicit none
         integer         :: a
         integer(isizea) :: b
         integer(isizea) :: c
      end subroutine b_f5_f

      subroutine b_f6_f(a, b, c) 
         use iso_c_binding
         use types_bl5
         implicit none
         integer         :: a
         integer(isizea) :: b
         integer(isizea) :: c
      end subroutine b_f6_f

! ... declarations for interoperability
      subroutine a_f1_i(a) &
                bind(c, name='a_f1')
         use iso_c_binding
         implicit none
         type (C_FUNPTR), value :: a
      end subroutine a_f1_i

      subroutine a_f2_i(b) &
                bind(c, name='a_f2')
         use iso_c_binding
         implicit none
         type (C_FUNPTR), value :: b
      end subroutine a_f2_i

      subroutine a_f3_i(a, b) &
                bind(c, name='a_f3')
         use iso_c_binding
         implicit none
         type (C_FUNPTR), value :: a
         type (C_FUNPTR), value :: b
      end subroutine a_f3_i

      subroutine a_f4_i(b, a) &
                bind(c, name='a_f4')
         use iso_c_binding
         implicit none
         type (C_FUNPTR), value :: b
         type (C_FUNPTR), value :: a
      end subroutine a_f4_i

      subroutine b_f1_i(a, b) &
                bind(c, name='B_f1')
         use iso_c_binding
         implicit none
         type (C_FUNPTR), value :: a
         integer (C_INT), value :: b
      end subroutine b_f1_i

      subroutine b_f2_i(a, b) &
                bind(c, name='B_f2')
         use iso_c_binding
         implicit none
         type (C_FUNPTR), value :: a
         integer (C_INT), value :: b
      end subroutine b_f2_i

      subroutine b_f3_i(a, b, c) &
                bind(c, name='B_f3')
         use iso_c_binding
         implicit none
         type (C_FUNPTR), value :: a
         type (C_FUNPTR), value :: b
         integer (C_INT), value :: c
      end subroutine b_f3_i

      subroutine b_f4_i(a, b, c) &
                bind(c, name='B_f4')
         use iso_c_binding
         implicit none
         type (C_FUNPTR), value :: a
         type (C_FUNPTR), value :: b
         integer (C_INT), value :: c
      end subroutine b_f4_i

      subroutine b_f5_i(a, b, c) &
                bind(c, name='B_f5')
         use iso_c_binding
         implicit none
         integer (C_INT), value :: a
         type (C_FUNPTR), value :: b
         type (C_FUNPTR), value :: c
      end subroutine b_f5_i

      subroutine b_f6_i(a, b, c) &
                bind(c, name='B_f6')
         use iso_c_binding
         implicit none
         integer (C_INT), value :: a
         type (C_FUNPTR), value :: b
         type (C_FUNPTR), value :: c
      end subroutine b_f6_i

   end interface

end module pact_bl5

