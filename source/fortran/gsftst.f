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

      program gsftst
      use pact_fortran
      implicit none

      integer idev, idvp, st
      double precision x1, y1, x2, y2
      double precision x(5), y(5), u(4), v(4)
      character*8 names, namep
      character*10 name2, name3
      character*12 name4

! ... set up the window
      names = 'WINDOW'
      name2 = 'COLOR'
      name3 = 'PGS Test A'
      idev = pgmkdv(6, names, 5, name2, 10, name3)
      x1 = 0.05
      y1 = 0.2
      x2 = 0.45
      y2 = 0.45

! ... set use pixmap flag to false
      st = pgsupm(0)
      st = pgopen(idev, x1, y1, x2, y2)

! ... set up the PS device
      namep = 'PS'
      name2 = 'MONOCHROME'
      name3 = 'gsftst'
      idvp = pgmkdv(2, namep, 10, name2, 6, name3)
      x1 = 0.0
      y1 = 0.0
      x2 = 0.0
      y2 = 0.0
      st = pgopen(idvp, x1, y1, x2, y2)

      st = pgclsc(idev)
      st = pgclsc(idvp)

! ... set up the view port and world coordinate system
      x1 = 0.1
      x2 = 0.9
      y1 = 0.1
      y2 = 0.9
      st = pgsvwp(idev, x1, x2, y1, y2)
      st = pgsvwp(idvp, x1, x2, y1, y2)

      x1 =   0.0
      x2 =  10.0
      y1 = -15.0
      y2 =  30.0
      st = pgswcs(idev, x1, x2, y1, y2)
      st = pgswcs(idvp, x1, x2, y1, y2)

! ... draw a bounding box
      x1 =   0.0
      x2 =  10.0
      y1 = -15.0
      y2 =  30.0
      st = pgdrbx(idev, x1, x2, y1, y2)
      st = pgdrbx(idvp, x1, x2, y1, y2)

! ... write a string
      x1 = 5.0
      y1 = 0.0
      name4 = 'TEXT STRING'

      st = pgstxf(idev, 9, 'helvetica', 6, 'medium', 9)
      st = pgstxf(idvp, 9, 'helvetica', 6, 'medium', 9)
      st = pgwrta(idev, x1, y1, 11, name4)
      st = pgwrta(idvp, x1, y1, 11, name4)

! ... draw a line
      x1 =  1.0
      x2 =  9.0
      y1 = -4.0
      y2 = -1.0
      st = pgdrln(idev, x1, y1, x2, y2)
      st = pgdrln(idvp, x1, y1, x2, y2)

! ... do a vector plot
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

      st = pgplvc(idev, x, y, u, v, 4, 0)
      st = pgplvc(idvp, x, y, u, v, 4, 0)

! ... draw and fill a polygon (color 4 is blue)
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
      st = pgfply(idev, x, y, 5, 4)
      st = pgfply(idvp, x, y, 5, 4)

      st = pgfnpl(idev)
      st = pgfnpl(idvp)

!      pause

! ... close the device
      st = pgclos(idev)
      st = pgclos(idvp)

      stop 0
      end

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

