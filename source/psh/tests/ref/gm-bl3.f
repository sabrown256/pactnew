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


   interface

! ... declarations for generated wrappers
      function fe1_f(a1, a2) 
         use iso_c_binding
         use types_bl3
         implicit none
         integer(isizea) :: fe1_f
         integer(isizea) :: a1
         integer         :: a2
      end function fe1_f

      subroutine fe2_f(dev, x, y, n, info, l) 
         use iso_c_binding
         use types_bl3
         implicit none
         integer(isizea) :: dev
         real(8)         :: x(*)
         real(8)         :: y(*)
         integer         :: n
         integer(isizea) :: info
         integer         :: l
      end subroutine fe2_f

! ... declarations for interoperability
      type (C_PTR) function fe1_i(a1, a2) &
                bind(c, name='fe1')
         use iso_c_binding
         implicit none
         type (C_PTR), value :: a1
         integer (C_INT), value :: a2
      end function fe1_i

      subroutine fe2_i(dev, x, y, n, info, l) &
                bind(c, name='fe2')
         use iso_c_binding
         implicit none
         type (C_PTR), value :: dev
         type (C_PTR), value :: x
         type (C_PTR), value :: y
         integer (C_INT), value :: n
         type (C_PTR), value :: info
         integer (C_INT), value :: l
      end subroutine fe2_i

   end interface

end module pact_bl3

