!
! GM-BL6.F - F90 interfaces for bl6
!          - NOTE: this file was automatically generated
!          - any manual changes will not be effective
!

module types_bl6
   integer, parameter :: isizea = 8
end module types_bl6

module pact_bl6
   use iso_c_binding

! ... external declarations for generated wrappers

! ... C enum declarations
! ... C struct declarations


   interface

! ... declarations for generated wrappers
      function fa1f(a) 
         use iso_c_binding
         implicit none
         integer         :: fa1f
         integer         :: a
      end function fa1f

      function fa3f(a, b, c) 
         use iso_c_binding
         use types_bl6
         implicit none
         integer         :: fa3f
         integer         :: a(*)
         integer         :: b(*)
         integer         :: c(*)
      end function fa3f

      function fa4f(a, b, c) 
         use iso_c_binding
         use types_bl6
         implicit none
         integer         :: fa4f
         integer         :: a(*)
         integer         :: b(*)
         integer         :: c(*)
      end function fa4f

      subroutine fb1_f(a, b) 
         use iso_c_binding
         implicit none
         integer         :: a
         integer         :: b
      end subroutine fb1_f

      subroutine fb2_f(a, b) 
         use iso_c_binding
         implicit none
         integer         :: a
         integer         :: b
      end subroutine fb2_f

      subroutine fb3_f(a, b) 
         use iso_c_binding
         implicit none
         integer         :: a
         integer         :: b
      end subroutine fb3_f

      subroutine fc1_f(a, b, c, d) 
         use iso_c_binding
         use types_bl6
         implicit none
         integer         :: a
         integer         :: b(*)
         integer         :: c(*)
         integer         :: d(*)
      end subroutine fc1_f

      subroutine fc2_f(a, b, c, d) 
         use iso_c_binding
         use types_bl6
         implicit none
         integer         :: a
         integer         :: b(*)
         integer         :: c(*)
         integer         :: d(*)
      end subroutine fc2_f

      subroutine fc3_f(a, b, c, d) 
         use iso_c_binding
         use types_bl6
         implicit none
         integer         :: a
         integer         :: b(*)
         integer         :: c(*)
         integer         :: d(*)
      end subroutine fc3_f

      subroutine fc4_f(a, b, c, d) 
         use iso_c_binding
         use types_bl6
         implicit none
         integer         :: a
         integer         :: b(*)
         integer         :: c(*)
         integer         :: d(*)
      end subroutine fc4_f

      subroutine fc5_f(a, b, c, d) 
         use iso_c_binding
         use types_bl6
         implicit none
         integer         :: a
         integer         :: b(*)
         integer         :: c(*)
         integer         :: d(*)
      end subroutine fc5_f

      subroutine fc6_f(a, b, c, d) 
         use iso_c_binding
         use types_bl6
         implicit none
         integer         :: a
         integer         :: b(*)
         integer         :: c(*)
         integer         :: d(*)
      end subroutine fc6_f

      subroutine fc7_f(a, b, c, d) 
         use iso_c_binding
         use types_bl6
         implicit none
         integer         :: a
         integer         :: b(*)
         integer         :: c(*)
         integer         :: d(*)
      end subroutine fc7_f

      subroutine fc8_f(s1, s2, s3, s4) 
         use iso_c_binding
         use types_bl6
         implicit none
         character(*)    :: s1
         type(C_PTR)     :: s2
         character(*)    :: s3
         type(C_PTR)     :: s4
      end subroutine fc8_f

      subroutine fc9_f(c1, c2, c3, c4, c5) 
         use iso_c_binding
         use types_bl6
         implicit none
         character       :: c1
         character       :: c2
         character(*)    :: c3
         character(*)    :: c4
         character(*)    :: c5
      end subroutine fc9_f

! ... declarations for interoperability
      function fa1f_i(a) &
                bind(c, name='fa1')
         use iso_c_binding
         implicit none
         integer(C_INT) :: fa1f_i
         integer(C_INT), value :: a
      end function fa1f_i

      function fa3f_i(a, b, c) &
                bind(c, name='fa3')
         use iso_c_binding
         implicit none
         integer(C_INT) :: fa3f_i
         type(C_PTR), value :: a
         type(C_PTR), value :: b
         type(C_PTR), value :: c
      end function fa3f_i

      function fa4f_i(a, b, c) &
                bind(c, name='fa4')
         use iso_c_binding
         implicit none
         integer(C_INT) :: fa4f_i
         type(C_PTR), value :: a
         type(C_PTR), value :: b
         type(C_PTR), value :: c
      end function fa4f_i

      subroutine fb1_i(a, b) &
                bind(c, name='fb1')
         use iso_c_binding
         implicit none
         integer(C_INT), value :: a
         integer(C_INT), value :: b
      end subroutine fb1_i

      subroutine fb2_i(a, b) &
                bind(c, name='fb2')
         use iso_c_binding
         implicit none
         integer(C_INT), value :: a
         integer(C_INT), value :: b
      end subroutine fb2_i

      subroutine fb3_i(a, b) &
                bind(c, name='fb3')
         use iso_c_binding
         implicit none
         integer(C_INT), value :: a
         integer(C_INT), value :: b
      end subroutine fb3_i

      subroutine fc1_i(a, b, c, d) &
                bind(c, name='fc1')
         use iso_c_binding
         implicit none
         integer(C_INT), value :: a
         type(C_PTR), value :: b
         type(C_PTR), value :: c
         type(C_PTR), value :: d
      end subroutine fc1_i

      subroutine fc2_i(a, b, c, d) &
                bind(c, name='fc2')
         use iso_c_binding
         implicit none
         integer(C_INT), value :: a
         type(C_PTR), value :: b
         type(C_PTR), value :: c
         type(C_PTR), value :: d
      end subroutine fc2_i

      subroutine fc3_i(a, b, c, d) &
                bind(c, name='fc3')
         use iso_c_binding
         implicit none
         integer(C_INT), value :: a
         type(C_PTR), value :: b
         type(C_PTR), value :: c
         type(C_PTR), value :: d
      end subroutine fc3_i

      subroutine fc4_i(a, b, c, d) &
                bind(c, name='fc4')
         use iso_c_binding
         implicit none
         integer(C_INT), value :: a
         type(C_PTR), value :: b
         type(C_PTR), value :: c
         type(C_PTR), value :: d
      end subroutine fc4_i

      subroutine fc5_i(a, b, c, d) &
                bind(c, name='fc5')
         use iso_c_binding
         implicit none
         integer(C_INT), value :: a
         type(C_PTR), value :: b
         type(C_PTR), value :: c
         type(C_PTR), value :: d
      end subroutine fc5_i

      subroutine fc6_i(a, b, c, d) &
                bind(c, name='fc6')
         use iso_c_binding
         implicit none
         integer(C_INT), value :: a
         type(C_PTR), value :: b
         type(C_PTR), value :: c
         type(C_PTR), value :: d
      end subroutine fc6_i

      subroutine fc7_i(a, b, c, d) &
                bind(c, name='fc7')
         use iso_c_binding
         implicit none
         integer(C_INT), value :: a
         type(C_PTR), value :: b
         type(C_PTR), value :: c
         type(C_PTR), value :: d
      end subroutine fc7_i

      subroutine fc8_i(s1, s2, s3, s4) &
                bind(c, name='fc8')
         use iso_c_binding
         implicit none
         type(C_PTR), value :: s1
         type(C_PTR), value :: s2
         type(C_PTR), value :: s3
         type(C_PTR), value :: s4
      end subroutine fc8_i

      subroutine fc9_i(c1, c2, c3, c4, c5) &
                bind(c, name='fc9')
         use iso_c_binding
         implicit none
         integer(C_INT), value :: c1
         integer(C_INT), value :: c2
         type(C_PTR), value :: c3
         type(C_PTR), value :: c4
         type(C_PTR), value :: c5
      end subroutine fc9_i

   end interface

end module pact_bl6

