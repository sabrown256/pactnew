!
! TGSFG.F - test of PGS FORTRAN API
!
! Source Version: 3.0
! Software Release #: LLNL-CODE-422942
!
! #include <cpyright.h>
!

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

      subroutine test(dev)
      use pact_pgs
      implicit none

      type(C_PTR) :: dev

! ... local variables
      integer(8) :: np

      real*8 :: x1, y1, x2, y2
      real*8 :: box(6), p1(3), p2(3)
      real*8, target :: x(5), y(5), u(4), v(4)
      character*12 :: name4

      type(C_PTR), target :: r(2), w(2)
      type(C_PTR) :: rp, wp

! ... open the device
      x1 = 0.0
      y1 = 0.0
      x2 = 0.0
      y2 = 0.0
      dev = pg_open_device_f(dev, x1, y1, x2, y2)

      call pg_clear_window_f(dev)

! ... set up the view port and world coordinate system
      box(1) = 0.1
      box(2) = 0.9
      box(3) = 0.1
      box(4) = 0.9
      call pg_set_viewspace_f(dev, 2, NORMC, box)

      box(1) = 0.0
      box(2) = 10.0
      box(3) = -15.0
      box(4) = 30.0
      call pg_set_viewspace_f(dev, 2, WORLDC, box)

! ... draw a bounding box
      call pg_draw_box_n_f(dev, 2, WORLDC, box)

! ... write a string
      p1(1) = 5.0
      p1(2) = 0.0
      name4 = 'TEXT STRING'

      call pg_fset_font_f(dev, 'helvetica', 'medium', 9)
      call pg_move_tx_abs_n_f(dev, p1)
      call pg_write_text_f(dev, C_NULL_PTR, name4)

! ... draw a line
      p1(1) =  1.0
      p1(2) = -4.0
      p2(1) =  9.0
      p2(2) = -1.0
      call pg_draw_line_n_f(dev, 2, WORLDC, p1, p2, 1)

! ... do a vector plot
      np = 4

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

      r(1) = C_LOC(x)
      r(2) = C_LOC(y)
      rp   = C_LOC(r)

      w(1) = C_LOC(u)
      w(2) = C_LOC(v)
      wp   = C_LOC(w)

      call pg_draw_vector_n_f(dev, 2, WORLDC, np, rp, wp)

! ... draw and fill a polygon (color 4 is blue)
      np = 5
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

      r(1) = C_LOC(x)
      r(2) = C_LOC(y)
      rp   = C_LOC(r)

      call pg_fill_polygon_n_f(dev, 4, 1, 2, WORLDC, np, rp)

      call pg_finish_plot_f(dev)

!      pause

! ... close the device
      call pg_close_device_f(dev)

      return
      end

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

      subroutine ps
      use pact_pgs
      implicit none

      type(C_PTR) :: dev
      character*8 :: name
      character*10 :: type, title

! ... set up the PS device
      name  = 'PS'
      type  = 'MONOCHROME'
      title = 'tgsfg'

      dev = pg_make_device_f(name, type, title)

      call test(dev)

      return
      end

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

      subroutine screen
      use pact_pgs
      implicit none

      type(C_PTR) :: dev
      integer :: st
      character*8 :: name
      character*10 :: type, title

! ... set up the window
      name  = 'WINDOW'
      type  = 'COLOR'
      title = 'PGS Test A'

      dev = pg_make_device_f(name, type, title)

      st = pg_fset_use_pixmap_f(0)

      call test(dev)

      return
      end

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

      program tgsfg
      implicit none

      logical :: psf, scrf
      integer :: narg, iarg
      character*8 :: arg

!      narg = iargc()
      narg = command_argument_count()
      iarg = 1

      psf  = .true.
      scrf = .true.

 10   if (iarg .le. narg) then
!         call getarg(iarg, arg)
         call get_command_argument(iarg, arg)
         if (arg .eq. "-h" .or. arg .eq. "help") then
            write(6, *) 'Usage: tgsfg [-p] [-s]'
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

