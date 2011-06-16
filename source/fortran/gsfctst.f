!
! GSFCTST.F - Fortran PGS contour plotting example
!
! Source Version: 3.0
! Software Release #: LLNL-CODE-422942
!
! include cpyright.h
!

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

      program gsfctst
      use pact_fortran
      implicit none

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
      integer idev, id, cp, igraph
      integer centering
      integer kmax, lmax, kxl

      real*8 x1, y1, x2, y2
      real*8 xmin, xmax, ymin, ymax, r, t
      real*8 x(KSZ), y(KSZ), f(KSZ)
      
      character*8 names
      character*20 name2, name3

! ... set up the window
      names = 'WINDOW'
      name2 = 'COLOR'
      name3 = 'PGS Contour Test'
      idev = pgmkdv(6, names, 5, name2, 16, name3)

! ... set use pixmap flag to false
      st = pgsupm(0)

! ... create the window on the screen
      x1 = 0.05
      y1 = 0.2
      x2 = 0.45
      y2 = 0.45
      st = pgopen(idev, x1, y1, x2, y2)

! ... clear the screen
      st = pgclsc(idev)

! ... set up the view port
      x1 = 0.1
      x2 = 0.9
      y1 = 0.1
      y2 = 0.9
      st = pgsvwp(idev, x1, x2, y1, y2)

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
      st = pgtdid(idev, 1);

! ... make a graph for PGS to plot
      igraph = pgmg21(id, 7, 'contour', cp, kmax, lmax, centering, &
                      x, y, f, 2, 'xy', 1, 'f')

! ... plot the graph
      st = pgplot(idev, igraph)

!      pause

! ... close the device
      st = pgclos(idev)

      stop 0
      end

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

