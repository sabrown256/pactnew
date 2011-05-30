!
! GM-BL4.F - F90 interfaces for bl4
!          - NOTE: this file was automatically generated
!          - any manual changes will not be effective
!

module types_bl4
   integer, parameter :: isizea = 8
end module types_bl4

module pact_bl4
   use iso_c_binding

! ... external declarations for generated wrappers


   interface

! ... declarations for generated wrappers
      function fb1_f(a1, a2) 
         use types_bl4
         implicit none
         integer         :: fb1_f
         logical         :: a1
         logical         :: a2(*)
      end function fb1_f

      function fb2_f(a1, a2) 
         use types_bl4
         implicit none
         integer         :: fb2_f
         character       :: a1
         character(*)    :: a2
      end function fb2_f

      function fb3_f(a1, a2) 
         use types_bl4
         implicit none
         integer         :: fb3_f
         integer(2)      :: a1
         integer(2)      :: a2(*)
      end function fb3_f

      function fb4_f(a1, a2) 
         use types_bl4
         implicit none
         integer         :: fb4_f
         integer         :: a1
         integer         :: a2(*)
      end function fb4_f

      function fb5_f(a1, a2) 
         use types_bl4
         implicit none
         integer         :: fb5_f
         integer(8)      :: a1
         integer(8)      :: a2(*)
      end function fb5_f

      function fb6_f(a1, a2) 
         use types_bl4
         implicit none
         integer         :: fb6_f
         integer(8)      :: a1
         integer(8)      :: a2(*)
      end function fb6_f

      function fb7_f(a1, a2) 
         use types_bl4
         implicit none
         integer         :: fb7_f
         real(4)         :: a1
         real(4)         :: a2(*)
      end function fb7_f

      function fb8_f(a1, a2) 
         use types_bl4
         implicit none
         integer         :: fb8_f
         real(8)         :: a1
         real(8)         :: a2(*)
      end function fb8_f

      function fb10_f(a1, a2) 
         use types_bl4
         implicit none
         integer         :: fb10_f
         complex(4)      :: a1
         integer(isizea) :: a2
      end function fb10_f

      function fb11_f(a1, a2) 
         use types_bl4
         implicit none
         integer         :: fb11_f
         complex(8)      :: a1
         integer(isizea) :: a2
      end function fb11_f

      function fr1_f(a1) 
         implicit none
         integer         :: fr1_f
         integer         :: a1
      end function fr1_f

      function fr2_f(a1) 
         implicit none
         integer         :: fr2_f
         integer         :: a1
      end function fr2_f

      function fr3_f(a1) 
         implicit none
         integer         :: fr3_f
         integer         :: a1
      end function fr3_f

      function fr4_f(a1) 
         implicit none
         integer         :: fr4_f
         integer         :: a1
      end function fr4_f

      function fr5_f(a1) 
         implicit none
         integer         :: fr5_f
         integer         :: a1
      end function fr5_f

      function fr6_f(a1) 
         implicit none
         integer         :: fr6_f
         integer         :: a1
      end function fr6_f

      function fr7_f(a1) 
         implicit none
         real            :: fr7_f
         integer         :: a1
      end function fr7_f

      function fr8_f(a1) 
         implicit none
         real            :: fr8_f
         integer         :: a1
      end function fr8_f

      function fr10_f(a1) 
         implicit none
         complex         :: fr10_f
         integer         :: a1
      end function fr10_f

      function fr11_f(a1) 
         implicit none
         complex         :: fr11_f
         integer         :: a1
      end function fr11_f

      function fr13_f(a1) 
         implicit none
         integer         :: fr13_f
         integer         :: a1
      end function fr13_f

      function fp1_f(a1) 
         use types_bl4
         implicit none
         integer(isizea) :: fp1_f
         integer         :: a1
      end function fp1_f

      function fp2_f(a1) 
         use types_bl4
         implicit none
         integer(isizea) :: fp2_f
         integer         :: a1
      end function fp2_f

      function fp3_f(a1) 
         use types_bl4
         implicit none
         integer(isizea) :: fp3_f
         integer         :: a1
      end function fp3_f

      function fp4_f(a1) 
         use types_bl4
         implicit none
         integer(isizea) :: fp4_f
         integer         :: a1
      end function fp4_f

      function fp5_f(a1) 
         use types_bl4
         implicit none
         integer(isizea) :: fp5_f
         integer         :: a1
      end function fp5_f

      function fp6_f(a1) 
         use types_bl4
         implicit none
         integer(isizea) :: fp6_f
         integer         :: a1
      end function fp6_f

      function fp7_f(a1) 
         use types_bl4
         implicit none
         integer(isizea) :: fp7_f
         integer         :: a1
      end function fp7_f

      function fp8_f(a1) 
         use types_bl4
         implicit none
         integer(isizea) :: fp8_f
         integer         :: a1
      end function fp8_f

      function fp10_f(a1) 
         use types_bl4
         implicit none
         integer(isizea) :: fp10_f
         integer         :: a1
      end function fp10_f

      function fp11_f(a1) 
         use types_bl4
         implicit none
         integer(isizea) :: fp11_f
         integer         :: a1
      end function fp11_f

      function fp13_f(a1) 
         use types_bl4
         implicit none
         integer(isizea) :: fp13_f
         integer         :: a1
      end function fp13_f

! ... declarations for interoperability
      integer (C_INT) function fb1_i(a1, a2) &
                bind(c, name='fb1')
         use iso_c_binding
         implicit none
         integer (C_INT), value :: a1
         type (C_PTR), value :: a2
      end function fb1_i

      integer (C_INT) function fb2_i(a1, a2) &
                bind(c, name='fb2')
         use iso_c_binding
         implicit none
         integer (C_INT), value :: a1
         type (C_PTR), value :: a2
      end function fb2_i

      integer (C_INT) function fb3_i(a1, a2) &
                bind(c, name='fb3')
         use iso_c_binding
         implicit none
         integer (C_SHORT), value :: a1
         type (C_PTR), value :: a2
      end function fb3_i

      integer (C_INT) function fb4_i(a1, a2) &
                bind(c, name='fb4')
         use iso_c_binding
         implicit none
         integer (C_INT), value :: a1
         type (C_PTR), value :: a2
      end function fb4_i

      integer (C_INT) function fb5_i(a1, a2) &
                bind(c, name='fb5')
         use iso_c_binding
         implicit none
         integer (C_LONG), value :: a1
         type (C_PTR), value :: a2
      end function fb5_i

      integer (C_INT) function fb6_i(a1, a2) &
                bind(c, name='fb6')
         use iso_c_binding
         implicit none
         integer (C_LONG_LONG), value :: a1
         type (C_PTR), value :: a2
      end function fb6_i

      integer (C_INT) function fb7_i(a1, a2) &
                bind(c, name='fb7')
         use iso_c_binding
         implicit none
         real (C_FLOAT), value :: a1
         type (C_PTR), value :: a2
      end function fb7_i

      integer (C_INT) function fb8_i(a1, a2) &
                bind(c, name='fb8')
         use iso_c_binding
         implicit none
         real (C_DOUBLE), value :: a1
         type (C_PTR), value :: a2
      end function fb8_i

      integer (C_INT) function fb10_i(a1, a2) &
                bind(c, name='fb10')
         use iso_c_binding
         implicit none
         complex (C_FLOAT_COMPLEX), value :: a1
         type (C_PTR), value :: a2
      end function fb10_i

      integer (C_INT) function fb11_i(a1, a2) &
                bind(c, name='fb11')
         use iso_c_binding
         implicit none
         complex (C_DOUBLE_COMPLEX), value :: a1
         type (C_PTR), value :: a2
      end function fb11_i

      integer (C_INT) function fr1_i(a1) &
                bind(c, name='fr1')
         use iso_c_binding
         implicit none
         integer (C_INT), value :: a1
      end function fr1_i

      integer (C_INT) function fr2_i(a1) &
                bind(c, name='fr2')
         use iso_c_binding
         implicit none
         integer (C_INT), value :: a1
      end function fr2_i

      integer (C_SHORT) function fr3_i(a1) &
                bind(c, name='fr3')
         use iso_c_binding
         implicit none
         integer (C_INT), value :: a1
      end function fr3_i

      integer (C_INT) function fr4_i(a1) &
                bind(c, name='fr4')
         use iso_c_binding
         implicit none
         integer (C_INT), value :: a1
      end function fr4_i

      integer (C_LONG) function fr5_i(a1) &
                bind(c, name='fr5')
         use iso_c_binding
         implicit none
         integer (C_INT), value :: a1
      end function fr5_i

      integer (C_LONG_LONG) function fr6_i(a1) &
                bind(c, name='fr6')
         use iso_c_binding
         implicit none
         integer (C_INT), value :: a1
      end function fr6_i

      real (C_FLOAT) function fr7_i(a1) &
                bind(c, name='fr7')
         use iso_c_binding
         implicit none
         integer (C_INT), value :: a1
      end function fr7_i

      real (C_DOUBLE) function fr8_i(a1) &
                bind(c, name='fr8')
         use iso_c_binding
         implicit none
         integer (C_INT), value :: a1
      end function fr8_i

      complex (C_FLOAT_COMPLEX) function fr10_i(a1) &
                bind(c, name='fr10')
         use iso_c_binding
         implicit none
         integer (C_INT), value :: a1
      end function fr10_i

      complex (C_DOUBLE_COMPLEX) function fr11_i(a1) &
                bind(c, name='fr11')
         use iso_c_binding
         implicit none
         integer (C_INT), value :: a1
      end function fr11_i

      integer (C_INT) function fr13_i(a1) &
                bind(c, name='fr13')
         use iso_c_binding
         implicit none
         integer (C_INT), value :: a1
      end function fr13_i

      type (C_PTR) function fp1_i(a1) &
                bind(c, name='fp1')
         use iso_c_binding
         implicit none
         integer (C_INT), value :: a1
      end function fp1_i

      type (C_PTR) function fp2_i(a1) &
                bind(c, name='fp2')
         use iso_c_binding
         implicit none
         integer (C_INT), value :: a1
      end function fp2_i

      type (C_PTR) function fp3_i(a1) &
                bind(c, name='fp3')
         use iso_c_binding
         implicit none
         integer (C_INT), value :: a1
      end function fp3_i

      type (C_PTR) function fp4_i(a1) &
                bind(c, name='fp4')
         use iso_c_binding
         implicit none
         integer (C_INT), value :: a1
      end function fp4_i

      type (C_PTR) function fp5_i(a1) &
                bind(c, name='fp5')
         use iso_c_binding
         implicit none
         integer (C_INT), value :: a1
      end function fp5_i

      type (C_PTR) function fp6_i(a1) &
                bind(c, name='fp6')
         use iso_c_binding
         implicit none
         integer (C_INT), value :: a1
      end function fp6_i

      type (C_PTR) function fp7_i(a1) &
                bind(c, name='fp7')
         use iso_c_binding
         implicit none
         integer (C_INT), value :: a1
      end function fp7_i

      type (C_PTR) function fp8_i(a1) &
                bind(c, name='fp8')
         use iso_c_binding
         implicit none
         integer (C_INT), value :: a1
      end function fp8_i

      type (C_PTR) function fp10_i(a1) &
                bind(c, name='fp10')
         use iso_c_binding
         implicit none
         integer (C_INT), value :: a1
      end function fp10_i

      type (C_PTR) function fp11_i(a1) &
                bind(c, name='fp11')
         use iso_c_binding
         implicit none
         integer (C_INT), value :: a1
      end function fp11_i

      type (C_PTR) function fp13_i(a1) &
                bind(c, name='fp13')
         use iso_c_binding
         implicit none
         integer (C_INT), value :: a1
      end function fp13_i

   end interface

end module pact_bl4

