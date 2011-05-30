!
! GM-BL2.F - F90 interfaces for bl2
!          - NOTE: this file was automatically generated
!          - any manual changes will not be effective
!

module types_bl2
   integer, parameter :: isizea = 8
end module types_bl2

module pact_bl2
   use iso_c_binding

! ... external declarations for generated wrappers


   interface

! ... declarations for generated wrappers
      function fbcv1_f(a, b, c, d, e, f) 
         use types_bl2
         implicit none
         integer(isizea) :: fbcv1_f
         integer         :: a
         real(4)         :: b
         real(8)         :: c
         character       :: d
         integer(isizea) :: e
         integer(isizea) :: f
      end function fbcv1_f

      function fbmcv1_f(a, b, c, d) 
         use types_bl2
         implicit none
         integer(isizea) :: fbmcv1_f
         integer         :: a
         real(4)         :: b
         real(8)         :: c
         character       :: d
      end function fbmcv1_f

      function fbmcr2_f(a, b, c, d) 
         use types_bl2
         implicit none
         integer(isizea) :: fbmcr2_f
         integer         :: a(*)
         real(4)         :: b(*)
         real(8)         :: c(*)
         character(*)    :: d
      end function fbmcr2_f

! ... declarations for interoperability
      type (C_PTR) function fbcv1_i(a, b, c, d, e, f) &
                bind(c, name='fbcv1')
         use iso_c_binding
         implicit none
         integer (C_INT), value :: a
         real (C_FLOAT), value :: b
         real (C_DOUBLE), value :: c
         integer (C_INT), value :: d
         type (C_FUNPTR), value :: e
         type (C_FUNPTR), value :: f
      end function fbcv1_i

      type (C_PTR) function fbmcv1_i(a, b, c, d) &
                bind(c, name='fbmcv1')
         use iso_c_binding
         implicit none
         integer (C_INT), value :: a
         real (C_FLOAT), value :: b
         real (C_DOUBLE), value :: c
         integer (C_INT), value :: d
      end function fbmcv1_i

      type (C_PTR) function fbmcr2_i(a, b, c, d) &
                bind(c, name='fbmcr2')
         use iso_c_binding
         implicit none
         type (C_PTR), value :: a
         type (C_PTR), value :: b
         type (C_PTR), value :: c
         type (C_PTR), value :: d
      end function fbmcr2_i

   end interface

end module pact_bl2

