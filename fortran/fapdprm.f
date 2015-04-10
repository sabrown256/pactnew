!
! FPDPRM.F - F90 parameters for PDB
!
! Source Version: 9.0
! Software Release #: LLNL-CODE-422942
!
! #include "cpyright.h"
!

module parameters_pdb

! data standard indeces
      integer, parameter :: PD_NO_STD      = -1
      integer, parameter :: PD_TEXT_STD    =  0
      integer, parameter :: PD_I386_STD    =  1
      integer, parameter :: PD_I586L_STD   =  2
      integer, parameter :: PD_I586O_STD   =  3
      integer, parameter :: PD_PPC32_STD   =  4
      integer, parameter :: PD_X86_64_STD  =  5
      integer, parameter :: PD_X86_64A_STD =  6
      integer, parameter :: PD_PPC64_STD   =  7
      integer, parameter :: PD_M68X_STD    =  8
      integer, parameter :: PD_VAX_STD     =  9
      integer, parameter :: PD_CRAY_STD    = 10

! data alignment indeces
      integer, parameter :: PD_NO_ALGN          = -1
      integer, parameter :: PD_TEXT_ALGN        =  0
      integer, parameter :: PD_BYTE_ALGN        =  1
      integer, parameter :: PD_WORD2_ALGN       =  2
      integer, parameter :: PD_WORD4_ALGN       =  3
      integer, parameter :: PD_WORD8_ALGN       =  4
      integer, parameter :: PD_GNU4_I686_ALGN   =  5
      integer, parameter :: PD_OSX_10_5_ALGN    =  6
      integer, parameter :: PD_SPARC_ALGN       =  7
      integer, parameter :: PD_XLC32_PPC64_ALGN =  8
      integer, parameter :: PD_CYGWIN_I686_ALGN =  9
      integer, parameter :: PD_GNU3_PPC64_ALGN  = 10
      integer, parameter :: PD_GNU4_PPC64_ALGN  = 11
      integer, parameter :: PD_XLC64_PPC64_ALGN = 12
      integer, parameter :: PD_GNU4_X86_64_ALGN = 13
      integer, parameter :: PD_PGI_X86_64_ALGN  = 14

! byte order
      integer, parameter :: NO_ORDER      = 0
      integer, parameter :: NORMAL_ORDER  = 1
      integer, parameter :: REVERSE_ORDER = 2
      integer, parameter :: TEXT_ORDER    = 3
      integer, parameter :: SPEC_ORDER    = 4

! major order
      integer, parameter :: NO_MAJOR_ORDER     = 0
      integer, parameter :: ROW_MAJOR_ORDER    = 101
      integer, parameter :: COLUMN_MAJOR_ORDER = 102

! character standard
      integer, parameter :: PD_ITA2_UPPER    = -5
      integer, parameter :: PD_ITA2_LOWER    =  5
      integer, parameter :: PD_ASCII_6_UPPER = -6
      integer, parameter :: PD_ASCII_6_LOWER =  6
      integer, parameter :: PD_ASCII_7       =  7
      integer, parameter :: PD_UTF_8         =  8
      integer, parameter :: PD_EBCDIC        =  157

end module parameters_pdb
