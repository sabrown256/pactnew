c
c GSTEST.F - Fortran PGS contour plotting example
c
c Source Version: 3.0
c Software Release #: LLNL-CODE-422942
c
c include cpyright.h
c

      program gsfctst

      parameter(KMX = 20)
      parameter(LMX = 20)
      parameter(KSZ  = KMX*LMX)

c ... PGS centering options
      parameter(Z_CENT = -1)
      parameter(N_CENT = -2)
      parameter(F_CENT = -3)
      parameter(E_CENT = -4)
      parameter(U_CENT = -5)

c ... functions
      integer pgmkdv, pgmg21

      integer idev, id, cp, igraph
      integer centering
      integer kmax, lmax, kxl

      double precision x1, y1, x2, y2
      double precision xmin, xmax, ymin, ymax, r, t
      double precision x(KSZ), y(KSZ), f(KSZ)
      
      character*8 names, namep
      character*20 name2, name3
      character*12 name4

c ... set up the window
      names = 'WINDOW'
      name2 = 'COLOR'
      name3 = 'PGS Contour Test'
      idev = pgmkdv(6, names, 5, name2, 16, name3)
      x1 = 0.05
      y1 = 0.2
      x2 = 0.45
      y2 = 0.45

c ... set use pixmap flag to false
      call pgsupm(0)

c ... create the window on the screen
      call pgopen(idev, x1, y1, x2, y2)

c ... clear the screen
      call pgclsc(idev)

c ... set up the view port
      x1 = 0.1
      x2 = 0.9
      y1 = 0.1
      y2 = 0.9
      call pgsvwp(idev, x1, x2, y1, y2)

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

c... generate some data
      do 1 l = 1, lmax
         do 1 k = 1, kmax
            i = (k-1)*lmax + l
            x(i) = l/10.0 - 5.0
            y(i) = k/10.0 - 5.0
            r = x(i)*x(i) + y(i)*y(i)
            t = 5.0*atan(y(i)/x(i))
            r = r**0.125
            f(i) = exp(-r)*(1.0 + 0.1*cos(t))
 1    continue

c... turn data ids on
      call pgtdid(idev, 1);

c... make a graph for PGS to plot
      igraph = pgmg21(id, 7, 'contour', cp, kmax, lmax, centering,
     $            x, y, f, 2, 'xy', 1, 'f')

c... plot the graph
      call pgplot(idev, igraph)

      pause

c ... close the device
      call pgclos(idev)

      call exit
      end

c --------------------------------------------------------------------------
c --------------------------------------------------------------------------

