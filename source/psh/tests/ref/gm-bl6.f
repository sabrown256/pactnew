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


   interface

! ... declarations for generated wrappers
      function fa1f(a, b, c) 
         use iso_c_binding
         implicit none
         integer         :: fa1f
         integer         :: a
         real(4)         :: b
         integer(8)      :: c
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

! ... declarations for interoperability
      function fa1f_i(a, b, c) &
                bind(c, name='fa1')
         use iso_c_binding
         implicit none
         integer(C_INT) :: fa1f_i
         integer(C_INT), value :: a
         real(C_FLOAT), value :: b
         integer(C_LONG), value :: c
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

   end interface

end module pact_bl6

