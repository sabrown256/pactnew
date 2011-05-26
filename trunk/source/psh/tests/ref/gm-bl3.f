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


   interface

      type (C_PTR) function fe1(a1, sa2) &
                bind(c, name='fe1')
         use iso_c_binding
         implicit none
         type (C_PTR), value :: a1
         integer (C_INT), value :: sa2
      end function fe1

      function wfe1f(a1, sa2) 
         use types_bl3
         implicit none
         integer(isizea) :: wfe1f
         integer(isizea) :: a1
         integer         :: sa2
      end function wfe1f

      subroutine fe2(dev, ax, ay, sn, ainfo, sl) &
                bind(c, name='fe2')
         use iso_c_binding
         implicit none
         type (C_PTR), value :: dev
         type (C_PTR), value :: ax
         type (C_PTR), value :: ay
         integer (C_INT), value :: sn
         type (C_PTR), value :: ainfo
         integer (C_INT), value :: sl
      end subroutine fe2

      subroutine wfe2f(dev, ax, ay, sn, ainfo, sl) 
         use types_bl3
         implicit none
         integer(isizea) :: dev
         real*8          :: ax(*)
         real*8          :: ay(*)
         integer         :: sn
         integer(isizea) :: ainfo
         integer         :: sl
      end subroutine wfe2f

   end interface

end module pact_bl3

