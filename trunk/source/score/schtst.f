*
* Fortran routine to test the Fortran application interface to
* the PACT score hash routines
*
* Source Version: 3.0
* Software Release #: LLNL-CODE-422942
*
* include "cpyright.h"
*


c --------------------------------------------------------------------------
c --------------------------------------------------------------------------

c SCHTST - main test routine

      program schtst
      implicit none 

      integer tab
      double precision x, y, z
      pointer (ipx, x(3))
      pointer (ipz, z(3))
      integer LAST
      data  LAST /0/

c ... SCORE functions
      integer schlkp, schins, scmkht, scmake

      if (scmake(ipx, 3, 8) .eq. 0) then
         stop 1
      endif

      x(1) =  8.0d0
      x(2) =  9.0d0
      x(3) = 10.0d0

      tab = scmkht(127, 0)

      if (schins(1, 'a', 3, 3, 'int', 0, tab) .eq. 0) then
         stop 1
      endif

      if (schins(1, 'b', 3.14159d0, 6, 'double', 0, tab) .eq. 0) then
         stop 1
      endif

      if (schins(1, 'c', x(1), 6, 'double', 0, tab) .eq. 0) then
         stop 1
      endif

      if (schins(1, 'd', ipx, 8, 'double *', 0, tab) .eq. 0) then
         stop 1
      endif

      y = 0.0d0
      if (schlkp(y, 1, 'b', tab) .eq. 0) then
         stop 1
      endif

      write(6, 100) y

      y = 0.0d0
      if (schlkp(y, 1, 'c', tab) .eq. 0) then
         stop 1
      endif

      write(6, 200) x(1), y

      ipz = 0
      if (schlkp(ipz, 1, 'd', tab) .eq. 0) then
         stop 1
      endif

      write(6, 200) x(2), z(2)

 100  format(/, 'Expect 3.14159 and got ', f8.5)
 200  format(/, 'Expect ', f8.5, ' and got ', f8.5)

      stop 0
      end

c --------------------------------------------------------------------------
c --------------------------------------------------------------------------
