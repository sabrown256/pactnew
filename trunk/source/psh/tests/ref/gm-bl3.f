!
! GM-BL3.F - F90 interfaces for bl3
!          - NOTE: this file was automatically generated
!          - any manual changes will not be effective
!

module types_bl3
   integer, parameter :: isizea = 8
end module types_bl3

module pact_bl3
   use iso_c_binding

! ... external declarations for generated wrappers

! ... C enum declarations
! ... C struct declarations


   interface

! ... declarations for generated wrappers
      function fe1f(a1, a2) 
         use iso_c_binding
         use types_bl3
         implicit none
         type(C_PTR)     :: fe1f
         type(C_PTR)     :: a1
         integer         :: a2
      end function fe1f

      subroutine fe2f(dev, x, y, n, info, l) 
         use iso_c_binding
         use types_bl3
         implicit none
         type(C_PTR)     :: dev
         real(8)         :: x(*)
         real(8)         :: y(*)
         integer         :: n
         type(C_PTR)     :: info
         integer         :: l
      end subroutine fe2f

! ... declarations for interoperability
      function fe1f_i(a1, a2) &
                bind(c, name='fe1')
         use iso_c_binding
         implicit none
         type(C_PTR) :: fe1f_i
         type(C_PTR), value :: a1
         integer(C_INT), value :: a2
      end function fe1f_i

      subroutine fe2f_i(dev, x, y, n, info, l) &
                bind(c, name='fe2')
         use iso_c_binding
         implicit none
         type(C_PTR), value :: dev
         type(C_PTR), value :: x
         type(C_PTR), value :: y
         integer(C_INT), value :: n
         type(C_PTR), value :: info
         integer(C_INT), value :: l
      end subroutine fe2f_i

   end interface

end module pact_bl3

