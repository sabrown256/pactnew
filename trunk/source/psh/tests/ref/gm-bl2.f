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


   interface

      type (C_PTR) function fbcv1(sa, sb, sc, sd, fe, ff) bind(c, name='fbcv1')
         use iso_c_binding
         implicit none
         integer (C_INT), value :: sa
         real (C_FLOAT), value :: sb
         real (C_DOUBLE), value :: sc
         integer (C_INT), value :: sd
         type (C_FUNPTR), value :: fe
         type (C_FUNPTR), value :: ff
      end function fbcv1

      function wfbcv1f(sa, sb, sc, sd, fe, ff)
         use types_bl2
         implicit none
         integer(isizea) :: wfbcv1f
         integer         :: sa
         real*4          :: sb
         real*8          :: sc
         character       :: sd
         integer(isizea) :: fe
         integer(isizea) :: ff
      end function wfbcv1f

      type (C_PTR) function fbmcv1(sa, sb, sc, sd) bind(c, name='fbmcv1')
         use iso_c_binding
         implicit none
         integer (C_INT), value :: sa
         real (C_FLOAT), value :: sb
         real (C_DOUBLE), value :: sc
         integer (C_INT), value :: sd
      end function fbmcv1

      function wfbmcv1f(sa, sb, sc, sd)
         use types_bl2
         implicit none
         integer(isizea) :: wfbmcv1f
         integer         :: sa
         real*4          :: sb
         real*8          :: sc
         character       :: sd
      end function wfbmcv1f

      type (C_PTR) function fbmcr2(aa, ab, ac, cd) bind(c, name='fbmcr2')
         use iso_c_binding
         implicit none
         type (C_PTR), value :: aa
         type (C_PTR), value :: ab
         type (C_PTR), value :: ac
         type (C_PTR), value :: cd
      end function fbmcr2

      function wfbmcr2f(aa, ab, ac, cd)
         use types_bl2
         implicit none
         integer(isizea) :: wfbmcr2f
         integer         :: aa(*)
         real*4          :: ab(*)
         real*8          :: ac(*)
         character(*)    :: cd
      end function wfbmcr2f

   end interface

end module pact_bl2

