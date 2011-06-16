!
! GSFTST.F - test of PGS FORTRAN API
!          -
!          - load with the following options:
!          -
!          -    -L/usr/local/lib -lpgs -lX11 -lpml -lscore -lm
!          -
!          - some systems require other system dependent libraries
!          - but you have already probably discovered that
!
! Source Version: 3.0
! Software Release #: LLNL-CODE-422942
!
! #include <cpyright.h>
!
!

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

      subroutine test(dev)
      use pact_fortran
      implicit none

      integer :: dev

      integer, parameter :: KMX = 20
      integer, parameter :: LMX = 20
      integer, parameter :: KSZ  = KMX*LMX

! ... PGS centering options
      integer, parameter :: Z_CENT = -1
      integer, parameter :: N_CENT = -2
      integer, parameter :: F_CENT = -3
      integer, parameter :: E_CENT = -4
      integer, parameter :: U_CENT = -5

! ... local variables
      integer i, k, l, st
      integer id, cp, igraph
      integer centering
      integer kmax, lmax, kxl

      real*8 x1, y1, x2, y2
      real*8 xmin, xmax, ymin, ymax, r, t
      real*8 x(KSZ), y(KSZ), f(KSZ)
      
! ... create the window on the screen
      x1 = 0.05
      y1 = 0.2
      x2 = 0.45
      y2 = 0.45
      st = pgopen(dev, x1, y1, x2, y2)

! ... clear the screen
      st = pgclsc(dev)

! ... set up the view port
      x1 = 0.1
      x2 = 0.9
      y1 = 0.1
      y2 = 0.9
      st = pgsvwp(dev, x1, x2, y1, y2)

      kmax = KMX
      lmax = LMX
      kxl  = KSZ
      xmin = -5.0
      xmax = 5.0
      ymin = -5.0
      ymax = 5.0
      id   = 1
      cp   = 0
      centering = N_CENT

! ... generate some data
      do l = 1, lmax
         do k = 1, kmax
            i = (k-1)*lmax + l
            x(i) = l/10.0 - 5.0
            y(i) = k/10.0 - 5.0
            r = x(i)*x(i) + y(i)*y(i)
            t = 5.0*atan(y(i)/x(i))
            r = r**0.125
            f(i) = exp(-r)*(1.0 + 0.1*cos(t))
         enddo
      enddo

! ... turn data ids on
      st = pgtdid(dev, 1);

! ... make a graph for PGS to plot
      igraph = pgmg21(id, 7, 'contour', cp, kmax, lmax, centering, &
                      x, y, f, 2, 'xy', 1, 'f')

! ... plot the graph
      st = pgplot(dev, igraph)

!      pause

! ... close the device
      st = pgclos(dev)

      return
      end

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

      subroutine ps
      use pact_fortran
      implicit none

      integer idvp
      character*8 namep
      character*10 name2, name3

! ... set up the PS device
      namep = 'PS'
      name2 = 'MONOCHROME'
      name3 = 'gsfcts'
      idvp = pgmkdv(2, namep, 10, name2, 6, name3)

      call test(idvp)

      return
      end

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

      subroutine screen
      use pact_fortran
      implicit none

      integer idev, st
      character*8 names
      character*10 name2, name3

! ... set up the window
      names = 'WINDOW'
      name2 = 'COLOR'
      name3 = 'PGS Test A'
      idev = pgmkdv(6, names, 5, name2, 10, name3)

      st = pgsupm(0)

      call test(idev)

      return
      end

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

      program gsftst
      use pact_fortran
      implicit none

      logical :: psf, scrf
      integer :: narg, iarg, iargc
      character*8 :: arg

      narg = iargc()
      iarg = 1

      psf  = .true.
      scrf = .true.

 10   if (iarg .le. narg) then
         call getarg(iarg, arg)
         if (arg .eq. "-h" .or. arg .eq. "help") then
            write(6, *) 'Usage: gsfcts [-p] [-s]'
            write(6, *) '    -h   this help message'
            write(6, *) '    -p   do not test PS device'
            write(6, *) '    -s   do not test SCREEN device'
            stop 1
         elseif (arg .eq. "-p") then
            psf = .false.
         elseif (arg .eq. "-s") then
            scrf = .false.
         endif
         iarg = iarg + 1
         go to 10
      endif

      if (psf) &
         call ps

      if (scrf) &
         call screen

      stop 0
      end

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

