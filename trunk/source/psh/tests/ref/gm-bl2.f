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
      function fbcv1f(a, b, c, d, e, f) 
         use iso_c_binding
         use types_bl2
         implicit none
         type(C_PTR)     :: fbcv1f
         integer         :: a
         real(4)         :: b
         real(8)         :: c
         character       :: d
         type(C_PTR)     :: e
         type(C_PTR)     :: f
      end function fbcv1f

      function fbmcv1f(a, b, c, d) 
         use iso_c_binding
         use types_bl2
         implicit none
         type(C_PTR)     :: fbmcv1f
         integer         :: a
         real(4)         :: b
         real(8)         :: c
         character       :: d
      end function fbmcv1f

      function fbmcr2f(a, b, c, d) 
         use iso_c_binding
         use types_bl2
         implicit none
         type(C_PTR)     :: fbmcr2f
         integer         :: a(*)
         real(4)         :: b(*)
         real(8)         :: c(*)
         character(*)    :: d
      end function fbmcr2f

! ... declarations for interoperability
      function fbcv1f_i(a, b, c, d, e, f) &
                bind(c, name='fbcv1')
         use iso_c_binding
         implicit none
         type(C_PTR) :: fbcv1f_i
         integer(C_INT), value :: a
         real(C_FLOAT), value :: b
         real(C_DOUBLE), value :: c
         integer(C_INT), value :: d
         type(C_FUNPTR), value :: e
         type(C_FUNPTR), value :: f
      end function fbcv1f_i

      function fbmcv1f_i(a, b, c, d) &
                bind(c, name='fbmcv1')
         use iso_c_binding
         implicit none
         type(C_PTR) :: fbmcv1f_i
         integer(C_INT), value :: a
         real(C_FLOAT), value :: b
         real(C_DOUBLE), value :: c
         integer(C_INT), value :: d
      end function fbmcv1f_i

      function fbmcr2f_i(a, b, c, d) &
                bind(c, name='fbmcr2')
         use iso_c_binding
         implicit none
         type(C_PTR) :: fbmcr2f_i
         type(C_PTR), value :: a
         type(C_PTR), value :: b
         type(C_PTR), value :: c
         type(C_PTR), value :: d
      end function fbmcr2f_i

   end interface

end module pact_bl2

