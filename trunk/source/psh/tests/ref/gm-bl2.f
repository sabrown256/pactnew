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
      function fbcv1(sa, sb, sc, sd, fe, ff) 
         use types_bl2
         implicit none
         integer(isizea) :: fbcv1
         integer         :: sa
         real*4          :: sb
         real*8          :: sc
         character       :: sd
         integer(isizea) :: fe
         integer(isizea) :: ff
      end function fbcv1

      function fbmcv1(sa, sb, sc, sd) 
         use types_bl2
         implicit none
         integer(isizea) :: fbmcv1
         integer         :: sa
         real*4          :: sb
         real*8          :: sc
         character       :: sd
      end function fbmcv1

      function fbmcr2(aa, ab, ac, ad) 
         use types_bl2
         implicit none
         integer(isizea) :: fbmcr2
         integer         :: aa(*)
         real*4          :: ab(*)
         real*8          :: ac(*)
         character(*)    :: ad
      end function fbmcr2

! ... declarations for interoperability
      type (C_PTR) function fbcv1_i(sa, sb, sc, sd, fe, ff) &
                bind(c, name='fbcv1')
         use iso_c_binding
         implicit none
         integer (C_INT), value :: sa
         real (C_FLOAT), value :: sb
         real (C_DOUBLE), value :: sc
         integer (C_INT), value :: sd
         type (C_FUNPTR), value :: fe
         type (C_FUNPTR), value :: ff
      end function fbcv1_i

      type (C_PTR) function fbmcv1_i(sa, sb, sc, sd) &
                bind(c, name='fbmcv1')
         use iso_c_binding
         implicit none
         integer (C_INT), value :: sa
         real (C_FLOAT), value :: sb
         real (C_DOUBLE), value :: sc
         integer (C_INT), value :: sd
      end function fbmcv1_i

      type (C_PTR) function fbmcr2_i(aa, ab, ac, ad) &
                bind(c, name='fbmcr2')
         use iso_c_binding
         implicit none
         type (C_PTR), value :: aa
         type (C_PTR), value :: ab
         type (C_PTR), value :: ac
         type (C_PTR), value :: ad
      end function fbmcr2_i

   end interface

end module pact_bl2

