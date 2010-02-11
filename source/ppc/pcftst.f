c
c PCFTST.F - test connections child side in FORTRAN
c
c Source Version: 3.0
c Software Release #: LLNL-CODE-422942
c
c #include "cpyright.h"

c--------------------------------------------------------------------------
c--------------------------------------------------------------------------

      program pcftst

      character*80 msg

   10 write(6, 1000)
      read(5, 2000) msg
      write(6, 2000) msg
      if (msg .ne. 'end') goto 10

 1000 format('-> ')
 2000 format(a80)

      end

c--------------------------------------------------------------------------
c--------------------------------------------------------------------------

