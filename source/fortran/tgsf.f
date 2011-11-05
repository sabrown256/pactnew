!
! TGSF.F - test of PGS FORTRAN API
!
! Source Version: 3.0
! Software Release #: LLNL-CODE-422942
!
! #include <cpyright.h>
!

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

      subroutine test(dev)
      use pact_fortran
      implicit none

      integer :: dev

! ... local variables
      integer :: st
      double precision :: x1, y1, x2, y2
      double precision :: x(5), y(5), u(4), v(4)
      character*12 :: name4

! ... open the device
      x1 = 0.0
      y1 = 0.0
      x2 = 0.0
      y2 = 0.0
      st = pgopen(dev, x1, y1, x2, y2)

      st = pgclsc(dev)

! ... set up the view port and world coordinate system
      x1 = 0.1
      x2 = 0.9
      y1 = 0.1
      y2 = 0.9
      st = pgsvwp(dev, x1, x2, y1, y2)

      x1 =   0.0
      x2 =  10.0
      y1 = -15.0
      y2 =  30.0
      st = pgswcs(dev, x1, x2, y1, y2)

! ... draw a bounding box
      x1 =   0.0
      x2 =  10.0
      y1 = -15.0
      y2 =  30.0
      st = pgdrbx(dev, x1, x2, y1, y2)

! ... write a string
      x1 = 5.0
      y1 = 0.0
      name4 = 'TEXT STRING'

      st = pgstxf(dev, 9, 'helvetica', 6, 'medium', 9)
      st = pgwrta(dev, x1, y1, 11, name4)

! ... draw a line
      x1 =  1.0
      x2 =  9.0
      y1 = -4.0
      y2 = -1.0
      st = pgdrln(dev, x1, y1, x2, y2)

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

      st = pgplvc(dev, x, y, u, v, 4, 0)

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
      st = pgfply(dev, x, y, 5, 4)

      st = pgfnpl(dev)

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
      name3 = 'tgsf'
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

      program tgsf
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
            write(6, *) 'Usage: tgsf [-p] [-s]'
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

