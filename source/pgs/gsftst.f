c
c GSTEST.F - test of PGS FORTRAN API
c          -
c          - load with the following options:
c          -
c          -    -L/usr/local/lib -lpgs -lX11 -lpml -lscore -lm
c          -
c          - some systems require other system dependent libraries
c          - but you have already probably discovered that
c
c Source Version: 3.0
c Software Release #: LLNL-CODE-422942
c
c #include <cpyright.h>
c
c

c --------------------------------------------------------------------------
c --------------------------------------------------------------------------

      program gsftst

      integer pgmkdv
      integer idev, idvp
      double precision x1, y1, x2, y2
      double precision x(5), y(5), u(4), v(4)
      character*8 names, namep
      character*10 name2, name3
      character*12 name4

c ... set up the window
      names = 'WINDOW'
      name2 = 'COLOR'
      name3 = 'PGS Test A'
      idev = pgmkdv(6, names, 5, name2, 10, name3)
      x1 = 0.05
      y1 = 0.2
      x2 = 0.45
      y2 = 0.45

c ... set use pixmap flag to false
      call pgsupm(0)
      call pgopen(idev, x1, y1, x2, y2)

c ... set up the PS device
      namep = 'PS'
      name2 = 'MONOCHROME'
      name3 = 'gsftst'
      idvp = pgmkdv(2, namep, 10, name2, 6, name3)
      x1 = 0.0
      y1 = 0.0
      x2 = 0.0
      y2 = 0.0
      call pgopen(idvp, x1, y1, x2, y2)

      call pgclsc(idev)
      call pgclsc(idvp)

c ... set up the view port and world coordinate system
      x1 = 0.1
      x2 = 0.9
      y1 = 0.1
      y2 = 0.9
      call pgsvwp(idev, x1, x2, y1, y2)
      call pgsvwp(idvp, x1, x2, y1, y2)
      x1 =   0.0
      x2 =  10.0
      y1 = -15.0
      y2 =  30.0
      call pgswcs(idev, x1, x2, y1, y2)
      call pgswcs(idvp, x1, x2, y1, y2)

c ... draw a bounding box
      x1 =   0.0
      x2 =  10.0
      y1 = -15.0
      y2 =  30.0
      call pgdrbx(idev, x1, x2, y1, y2)
      call pgdrbx(idvp, x1, x2, y1, y2)

c ... write a string
      x1 = 5.0
      y1 = 0.0
      name4 = 'TEXT STRING'
 100  format(1p, e10.2)
      call pgstxf(idev, 9, 'helvetica', 6, 'medium', 9)
      call pgstxf(idvp, 9, 'helvetica', 6, 'medium', 9)
      call pgwrta(idev, x1, y1, 11, name4)
      call pgwrta(idvp, x1, y1, 11, name4)

c ... draw a line
      x1 =  1.0
      x2 =  9.0
      y1 = -4.0
      y2 = -1.0
      call pgdrln(idev, x1, y1, x2, y2)
      call pgdrln(idvp, x1, y1, x2, y2)

c ... do a vector plot
      x(1) =  3.0
      y(1) =  4.0
      u(1) = -0.5
      v(1) = -0.25

      x(2) =  4.0
      y(2) =  4.0
      u(2) =  0.5
      v(2) = -0.25

      x(3) = 4.0
      y(3) = 5.0
      u(3) = 0.5
      v(3) = 0.0

      x(4) =  3.0
      y(4) =  5.0
      u(4) = -0.5
      v(4) =  0.5

      call pgplvc(idev, x, y, u, v, 4, 0)
      call pgplvc(idvp, x, y, u, v, 4, 0)

c ... draw and fill a polygon (color 4 is blue)
      x(1) = 5.0
      x(2) = 6.0
      x(3) = 6.0
      x(4) = 5.0
      x(5) = 5.0
      y(1) = 8.0
      y(2) = 8.0
      y(3) = 9.0
      y(4) = 9.0
      y(5) = 8.0
      call pgfply(idev, x, y, 5, 4)
      call pgfply(idvp, x, y, 5, 4)

      call pgfnpl(idev)
      call pgfnpl(idvp)

      pause

c ... close the device
      call pgclos(idev)
      call pgclos(idvp)

      call exit
      end

c --------------------------------------------------------------------------
c --------------------------------------------------------------------------

