!
! TPDFG.F - test the Fortran application interface to PDB
!          - use the wrappers generated by blang
!
! Source Version: 3.0
! Software Release #: LLNL-CODE-422942
!
! include "cpyright.h"
!

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

! ERRPROC - handle errors

      subroutine errproc
      use pact_fortran
      use pact_pdb
      implicit none

! ... local variables
      integer :: nc
      type(C_PTR) :: cp
      character(80), pointer :: err

      cp = pd_get_error_f()
      if (c_associated(cp)) then
         call c_f_pointer(cp, err)
         nc = c_strlenf(err)
         if (nc > 0) then
            write(6, 100)
            write(6, *) err
         else
            write(6, 102)
         endif
      else
         write(6, 102)
      endif

 100  format()
 102  format(/, 'Error in PDWFTS', /)

      stop 1
      end

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

! WRTCUR - write a curve

      subroutine wrtcur(fileid)
      use pact_pdb
      implicit none

      type(C_PTR) fileid

! ... local variables
      integer :: i, index, st
      real*8 :: dm(0:99), rm1(0:99), rm2(0:99)

      index = 0

      do i = 0, 99
         dm(i)  = 6.28*float(i)/99.
         rm1(i) = cos(6.28*float(i)/99.)
         rm2(i) = sin(6.28*float(i)/99.)
      enddo

      st = pd_wrt_pdb_curve_f(fileid, "cosine curve", 100, dm, rm1, index)
      if (st .eq. 0) &
         call errproc

      st = pd_wrt_pdb_curve_y_f(fileid, "sin curve", 100, index, rm2, index)
      if (st .eq. 0) &
         call errproc

      return
      end subroutine wrtcur

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

! CHKCUR - check curve

      subroutine chkcur(fileid, tolerance)
      use pact_pdb
      implicit none

      type(C_PTR) :: fileid
      real*8 :: tolerance

! ... local variables
      integer :: i
      real*8 :: dm(0:99), rm1(0:99), rm2(0:99)
      real*8 :: odm(0:99), orm1(0:99), orm2(0:99)

      do i = 0, 99
         dm(i)  = 6.28*float(i)/99.
         rm1(i) = cos(6.28*float(i)/99.)
         rm2(i) = sin(6.28*float(i)/99.)
      enddo

      if (pd_read_f(fileid, "xval0", odm) .eq. 0) &
         call errproc

      if (pd_read_f(fileid, "yval0", orm1) .eq. 0) &
         call errproc

      if (pd_read_f(fileid, "yval1", orm2) .eq. 0) &
         call errproc

      do i = 0, 99
         if ((abs(dm(i) - odm(i)) .gt. tolerance) &
              .or. (abs(rm1(i) - orm1(i)) .gt. tolerance) &
              .or. (abs(rm2(i) - orm2(i)) .gt. tolerance)) &
            call errproc
      enddo

      return
      end subroutine chkcur

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

! WRTIMA - write an image

      subroutine wrtima(fileid)
      use pact_pdb
      implicit none

      type(C_PTR) :: fileid

! ... local variables
      integer :: nd, w, h, bpp
      integer :: k, l, st
      real*8 :: xmin, xmax, ymin, ymax, zmin, zmax, data(0:10, 0:10)

      type(C_PTR) :: cp

      nd  = 2
      bpp = 24
      w   = 10
      h   = 10

      xmin = 0.0
      xmax = 10.0
      ymin = 0.0
      ymax = 10.0

      zmin = 0.0
      zmax = 50.0

      do l = 0, 10
         do k = 0, 10
            data(k, l) = float((k - 5)**2 + (l - 5)**2)
         enddo
      enddo

      st = pd_def_mapping_f(fileid)

      cp = pd_make_image_f("Test image", "double", data, w, h, bpp, &
                           xmin, xmax, ymin, ymax, zmin, zmax)
      st = pd_write_f(fileid, 'Image0[1]', 'PG_image *', cp)
      if (st .eq. 0) &
         call errproc

      return
      end subroutine wrtima

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

! CHKIMA - check image

      subroutine chkima(fileid)
      use pact_pdb
      implicit none

      type(C_PTR) :: fileid

! ... local variables
      integer :: k, l, st
      real*8 :: data(0:10, 0:10), odata(0:10, 0:10)

      do l = 0, 10
         do k = 0, 10
            data(k, l) = float((k - 5)**2 + (l - 5)**2)
         enddo
      enddo

      st = pd_read_f(fileid, "Image0[1]->buffer", odata)
      if (st .eq. 0) &
         call errproc

      do l = 0, 10
         do k = 0, 10
            if (data(k, l) .ne. odata(k, l)) &
               call errproc
         enddo
      enddo

      return
      end subroutine chkima

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

! WRTMAP - write some mappings

      subroutine wrtmap(fileid)
      use pact_pml
      use pact_pdb
      implicit none

      type(C_PTR) :: fileid

! ... local variables
      integer :: i, st
      integer :: dp(5), rp(5), pim(1)
      real*8, target :: dm(0:99), rm(0:99)

      do i = 0, 99
         dm(i) = 6.28*float(i)/99.0
         rm(i) = sin(6.28*float(i)/99.0)
      enddo

      pim = 0
      dp(1) = 7
      dp(2) = 1
      dp(3) = 1
      dp(4) = 100
      dp(5) = 100
      rp(1) = 6
      rp(2) = 1
      rp(3) = 1
      rp(4) = 100
      rp(5) = 100

      st = pd_wrt_map_f(fileid, "Domain0", dp, dm, "Range0", rp, rm, pim)
      if (st .eq. 0) &
         call errproc

      st = pd_wrt_set_f(fileid, "Domain1", dp, dm)
      if (st .eq. 0) &
         call errproc

      st = pd_wrt_map_ran_f(fileid, "Domain1", "Range1", rp, rm, C_NULL_PTR, pim)
      if (st .eq. 0) &
         call errproc

      return
      end subroutine wrtmap

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

! CHKMAP - check mappings

      subroutine chkmap(fileid, tolerance)
      use pact_pdb
      implicit none

      type(C_PTR) :: fileid
      real*8 :: tolerance

! ... local variables
      integer :: i, st
      real*8 :: dm(0:99), rm(0:99)
      real*8 :: odm(0:99), orm(0:99)

      do i = 0, 99
         dm(i) = 6.28*float(i)/99.0
         rm(i) = sin(6.28*float(i)/99.0)
      enddo

      st = pd_read_f(fileid, 'Mapping0->domain->elements[1]', odm)
      if (st .eq. 0) &
         call errproc

      st = pd_read_f(fileid, 'Mapping0->range->elements[1][1:100]', orm)
      if (st .eq. 0) &
         call errproc

      do i = 0, 99
         if ((abs(dm(i) - odm(i)) .gt. tolerance) &
              .or. (abs(rm(i) - orm(i)) .gt. tolerance)) &
            call errproc
      enddo

      st = pd_read_f(fileid, 'Domain1->elements[1][1:100]', odm)
      if (st .eq. 0) &
         call errproc

      st = pd_read_f(fileid, 'Mapping1->range->elements[1]', orm)
      if (st .eq. 0) &
         call errproc

      do i = 0, 99
         if ((abs(dm(i) - odm(i)) .gt. tolerance) &
              .or. (abs(rm(i) - orm(i)) .gt. tolerance)) &
            call errproc
      enddo

      return
      end subroutine chkmap

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

! TEST_1 - basic file testing

      subroutine test_1(outtype, date, PRINT, DIR, ATTR, OPTION)
      use pact_fortran
      use pact_pdb
      implicit none 

      character*8 :: outtype
      character*80 :: date
      logical :: PRINT
      logical :: DIR
      logical :: ATTR
      character*8 :: OPTION

! ... local variables
      integer :: i, j, k, l
      integer :: offset, order, st
      integer :: ndims, nc, istring, LAST
      
      integer(8) :: ldims(14)

      type(C_PTR) :: fileid, cp

      real*8 :: a, c, g, z, zz, zzx
      real*8 :: x(20)

      character*8 :: strings(4)

      character(80), pointer :: vnm

      logical :: ok

      common /abc/ a(2), c(2, 2:4), g(5, 2:10), z(4, 5, 2)
      common /abc/ zz(4, 5, 2), zzx(4, 5, 2)
      common /jkl/ j, k, l

      equivalence (istring, strings)

      data  LAST /0/

      if (PRINT) then
         write(6, *) '------------------------------------------------'
         write(6, *) 'Test #1'
      endif

! ... initialization
      do i = 1, 20
         x(i) = float(i)
      enddo

      do i = 1, 93
         a(i) = float(i)
      enddo

! ... initialize zz array (a(94)...)
      do i = 1, 40
         a(i+93)    = float(i) + 1000.0
         a(i+40+93) = 2*(float(i) + 1000.0)
      enddo

      j = 1
      k = 2
      l = 3

! ... open file for writing
      fileid = pd_open_f('file.pdb', 'w')
      if (.not. c_associated(fileid)) &
         call errproc

! ... faux fam
      if (.not. c_associated(pd_family_f(fileid, 1))) &
         call errproc

! ... check file mode
      if (pd_get_mode_f(fileid) .ne. 1) &
         call errproc

! ... set and get and verify major order
      order = 102
      if (pd_set_major_order_f(fileid, order) .ne. COLUMN_MAJOR_ORDER) &
         call errproc
      order = pd_get_major_order_f(fileid)

! ... set and get and verify default offset
      offset = 1
      if (pd_set_offset_f(fileid, offset) .ne. 1) &
         call errproc
      offset = pd_get_offset_f(fileid)

      if (PRINT) then
         write(6, 300) offset
 300     format(/, 'Current default offset:', i4)
      endif

      if (offset .ne. 1) &
         call errproc

! ... write at least one variable before creating a directory
      if (pd_write_f(fileid, 'x(20)', 'double', x) .eq. 0) &
         call errproc

      if (pd_append_f(fileid, 'x(1:10)', x) .eq. 0) &
         call errproc

      if (PRINT) then 
         write(6, 301) 
 301     format(/, 'Variable X written and appended to')
      endif

! ... write curve, image, and mappings before creating directory
      if (OPTION .eq. "1") then
         call wrtcur(fileid)
         call wrtima(fileid)
         call wrtmap(fileid)
      endif

! ... create a directory
      if (DIR) then
         if (pd_mkdir_f(fileid, "dir1") .eq. 0) &
            call errproc
      endif

! ... get current working directory
      cp = pd_pwd_f(fileid)
      if (c_associated(cp)) then
         call c_f_pointer(cp, vnm)
         nc = c_strlenf(vnm)
         if (PRINT) then
            write(6, 352) vnm(1:nc)
 352        format(/, 'Current working directory:  ', a)
         endif

         if ((vnm(1:nc) .ne. '/') .or. (nc .ne. 1)) &
            call errproc
      else
         call errproc
      endif

! ... write curve, image, and mappings in root after creating directory
      if (OPTION .eq. "2") then
         call wrtcur(fileid)
         call wrtima(fileid)
         call wrtmap(fileid)
      endif

      x(1) = 100

! ... write  data item z   write_as,  append,  append_alt
      st = pd_write_as_f(fileid, 'z(4,5,1)', 'double', 'double', z)
      if (st .eq. 0) &
         call errproc

      if (pd_append_f(fileid, 'z(1:4,1:5,1:1)', z) .eq. 0) &
         call errproc

      ndims = 3
      ldims(1) = 1
      ldims(2) = 4
      ldims(3) = 1
      ldims(4) = 1
      ldims(5) = 5
      ldims(6) = 1
      ldims(7) = 1
      ldims(8) = 2
      ldims(9) = 1
      if (pd_append_alt_f(fileid, 'z', z, ndims, ldims) .eq. 0) &
         call errproc

! ... write  data item zz   write_as_alt, append_as, append_as_alt
      ndims = 3
      ldims(1) = 1
      ldims(2) = 4
      ldims(3) = 1
      ldims(4) = 1
      ldims(5) = 5
      ldims(6) = 1
      ldims(7) = 1
      ldims(8) = 1
      ldims(9) = 1
      st = pd_write_as_alt_f(fileid, 'zz', 'double', outtype, zz, ndims, ldims)
      if (st .eq. 0) &
         call errproc

      st = pd_append_as_f(fileid, 'zz(1:4,1:5,1:1)', 'double', zz(1, 1, 2))
      if (st .eq. 0) &
         call errproc

      ndims = 3
      ldims(1) = 1
      ldims(2) = 4
      ldims(3) = 1
      ldims(4) = 1
      ldims(5) = 5
      ldims(6) = 1
      ldims(7) = 1
      ldims(8) = 2
      ldims(9) = 1
      st = pd_append_as_alt_f(fileid, 'zz', 'double', zzx, ndims, ldims)
      if (st .eq. 0) &
         call errproc

      if (PRINT) then 
         write(6, 305) 
 305     format(/, 'Variables z and zz written', /)
      endif

! ... change directory
      if (DIR) then
         if (pd_cd_f(fileid, "dir1") .eq. 0) &
            call errproc
      endif

! ... get current working directory
      cp = pd_pwd_f(fileid)
      if (c_associated(cp)) then
         call c_f_pointer(cp, vnm)
         nc = c_strlenf(vnm)
         if (PRINT) then
            write(6, 351) vnm(1:nc)
 351        format(/, 'Current working directory:  ', a)
         endif

         if (((.not. DIR) .and. (nc .ne. 1)) .or. &
             (DIR .and. (nc .ne. 5))) &
            call errproc
      else
         call errproc
      endif

! ... create a link
      if (DIR) then
         if (pd_ln_f(fileid, '/x', 'x_link') .eq. 0) &
            call errproc
      else
         if (pd_ln_f(fileid, 'x', 'x_link') .eq. 0) &
            call errproc
      endif

! ... define and write some data structures
      cp = pd_defstr_s_f(fileid, 'abc', &
                         'double a(2) ; double b ; double c(2, 2:4)')
      if (.not. c_associated(cp)) &
         call errproc

      if (pd_write_f(fileid, 'abc', 'abc', a) .eq. 0) &
         call errproc

      cp = pd_defstr_s_f(fileid, 'jkl', 'int j; int k; int l')
      if (.not. c_associated(cp)) &
         call errproc

      if (pd_write_f(fileid, 'jkl', 'jkl', j) .eq. 0) &
         call errproc

! ... define and write arrays of strings
      strings(1) = 'abcdefgh'
      strings(2) = 'hgfedcba'
      strings(3) = 'ijklmnop'
      strings(4) = 'ponmlkji'

      ndims = 2
      ldims(1) = 1
      ldims(2) = 8
      ldims(3) = 1
      ldims(4) = 1
      ldims(5) = 4
      ldims(6) = 1
      st = pd_write_alt_f(fileid, 'strings', 'char', istring, ndims, ldims)
      if (st .eq. 0) &
         call errproc

! ... define an attribute
      if (ATTR) then
         if (pd_def_attribute_f(fileid, 'date', 'char *') .eq. 0) &
            call errproc
      endif

! ... define some symbol table entries and reserve disk space
      ok = c_associated(pd_defent_f(fileid, 'f(0:3, 4:6, 1:2)', 'double'))
      if (.not. ok) &
         call errproc

      ndims = 2
      ldims(1) = 1
      ldims(2) = 5
      ldims(3) = 2
      ldims(4) = 10
      ok = c_associated(pd_defent_alt_f(fileid, 'g', 'double', ndims, ldims))
      if (.not. ok) &
         call errproc

      ndims = 2
      ldims(1) = 1
      ldims(2) = 1
      ldims(3) = 1
      ldims(4) = 2
      ldims(5) = 2
      ldims(6) = 1
      g(1, 2) = 100
      g(2, 2) = 101
      if (pd_write_alt_f(fileid, 'g', 'double', g, ndims, ldims) .eq. 0) &
         call errproc
      if (pd_write_alt_f(fileid, 'g[1, 2]', 'double', g, ndims, ldims) &
           .eq. 0) &
         call errproc

      ndims = 1
      ldims(1) = 1
      ldims(2) = 2
      ldims(3) = 1
      if (pd_write_alt_f(fileid, 'a', 'double', a, ndims, ldims) .eq. 0) &
         call errproc

      ndims = 2
      ldims(1) = 1
      ldims(2) = 2
      ldims(3) = 1
      ldims(4) = 2
      ldims(5) = 4
      ldims(6) = 1
      if (pd_write_alt_f(fileid, 'c', 'double', c, ndims, ldims) .eq. 0) &
         call errproc

! ... create a link
      if (pd_ln_f(fileid, 'jkl', '/jkl_link') .eq. 0) &
         call errproc

! ... set attribute value for member of struct
      if (ATTR) then
         if (pd_set_attribute_f(fileid, '/jkl_link.k', 'date', date) .eq. 0) &
            call errproc
      endif

! ... write curve, image, and mappings in a directory
      if (OPTION .eq. "3") then
         call wrtcur(fileid)
         call wrtima(fileid)
         call wrtmap(fileid)
      endif

! ... flush the file
      if (pd_flush_f(fileid) .eq. 0) &
         call errproc

! ... close the file
      if (pd_close_f(fileid) .eq. 0) &
         call errproc

      if (PRINT) then
         write(6, *)
      endif

      return
      end subroutine test_1

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

! TEST_2 - feature testing

      subroutine test_2(date, PRINT, DIR, ATTR)
      use pact_fortran
      use pact_pdb
      implicit none 

      character*80 :: date
      logical :: PRINT
      logical :: DIR
      logical :: ATTR

! ... local variables
      integer :: i, j, k, l, n
      integer :: nt, ni
      integer :: ndims, nitems
      integer :: typsiz, align, nptr
      integer :: kindx
      integer :: nchar, nc
       
      integer(C_LONG) :: dims(14)

      type(C_PTR) :: fileid, p, cp

      character*8 :: name2
      character*5 :: name3, name4
      character*80 :: dt

      character(80), pointer :: vnm, name1

      real*8 :: x(20)
      real*8 :: a, c, g, z, zz, zzx
      real*8 :: a2, c2, g2, z2, zz2
      real*8 :: v1, v2
      real*8 :: TOLERANCE

      common /abc/ a(2), c(2, 2:4), g(5, 2:10), z(4, 5, 2)
      common /abc/ zz(4, 5, 2), zzx(4, 5, 2)
      common /abc2/ a2(2), c2(2, 2:4), g2(5, 2:10), z2(4, 5)
      common /abc2/ zz2(4, 5, 2)
      common /jkl/ j, k, l

      data  TOLERANCE /3.0e-14/

      if (PRINT) then
         write(6, *) '------------------------------------------------'
         write(6, *) 'Test #2'
      endif

! ... initialization
      do i = 1, 20
         x(i) = float(i)
      enddo

      do i = 1, 93
         a(i) = float(i)
      enddo

! ... initialize zz array (a(94)...)
      do i = 1, 40
         a(i+93)    = float(i) + 1000.0
         a(i+40+93) = 2*(float(i) + 1000.0)
      enddo

      do i = 1, 2
         a2(i) = 0.0
      enddo

      j = 1
      k = 2
      l = 3

! ... open the file for reading
      fileid = pd_open_f('file.pdb', 'a')
      if (.not. c_associated(fileid)) &
         call errproc

! ... get file name given id
      cp = pd_get_file_name_f(fileid)
      if (.not. c_associated(cp)) &
         call errproc

      call c_f_pointer(cp, name1)
      name2 = 'file.pdb'
      if (name1(1:8) .ne. name2) &
         call errproc

! ... check file mode
      if (pd_get_mode_f(fileid) .ne. PD_APPEND) &
         call errproc

! ... retrieve and verify the value of an attribute
      if (ATTR) then
         if (pd_cd_f(fileid, "dir1") .eq. 0) &
            call errproc

         p = pd_get_attribute_f(fileid, '/jkl_link.k', 'date', dt)
         if (.not. c_associated(p)) &
            call errproc

         if (PRINT) then
            write(6, 2) dt
 2          format(/, 'A date: ', a)
         endif

         do i = 1, 25
            if (date(i:i) .ne. dt(i:i)) &
               call errproc
         enddo

! ... remove an attribute
         if (pd_rem_attribute_f(fileid, 'date') .eq. 0) &
            call errproc

! ... verify that attribute was removed
         p = pd_get_attribute_f(fileid, '/jkl_link.k', 'date', dt)
         if (.not. c_associated(p)) then
            write(6, 380)
 380        format(/, 'Attribute was not removed.')
            stop 1
         endif
      
         if (pd_cd_f(fileid, "/") .eq. 0) &
            call errproc

      endif

! ... verify z values
      dims(1) = 1
      dims(2) = 4
      dims(3) = 1
      dims(4) = 1
      dims(5) = 5
      dims(6) = 1
      dims(7) = 1
      dims(8) = 1
      dims(9) = 1

      do n = 1, 4
         dims(7) = n
         dims(8) = n
         ni = pd_read_alt_f(fileid, 'z', z2, dims)
         if (ni .eq. 0) &
            call errproc

         do j = 1, 5
            do i = 1, 4
               nt = n/4 + 1
               v1 = z2(i, j)
               v2 = z(i, j, nt)
               if (v1 .ne. v2) &
                  call errproc
            enddo
         enddo
      enddo

      if (PRINT) then
         write(6, 407)
 407     format(/, 'Values of z read match values written.')
      endif


! ... verify zz values  ('double' written as 'float')
!               read_as_alt  read_as

      dims(1) = 1
      dims(2) = 4
      dims(3) = 1
      dims(4) = 1
      dims(5) = 5
      dims(6) = 1
      dims(7) = 1
      dims(8) = 2
      dims(9) = 1

      ni = pd_read_as_alt_f(fileid, 'zz', 'double', zz2, dims)
      if (ni .eq. 0) &
         call errproc

      do kindx = 1, 2
         do j = 1, 5
            do i = 1, 4
               if (abs(zz2(i, j, kindx) - zz(i, j, kindx)) &
                   .gt. TOLERANCE) &
                  call errproc
            enddo
         enddo
      enddo

      ni = pd_read_as_f(fileid, 'zz(1:4,1:5,3:4)', 'double', zz2)
      if (ni .eq. 0) &
         call errproc

      do kindx = 1, 2
         do j = 1, 5
            do i = 1, 4
               if (abs(zz2(i, j, kindx) - zzx(i, j, kindx)) .gt. TOLERANCE) &
                  call errproc
            enddo
         enddo
      enddo

      if (PRINT) then
         write(6, 430)
 430     format(/, 'Values of zz read match values written.')
      endif


! ... get current working directory
      cp = pd_pwd_f(fileid)
      if (c_associated(cp)) then
         call c_f_pointer(cp, vnm)
         nc = c_strlenf(vnm)
         if (PRINT) then
            write(6, 352) vnm(1:nc)
 352        format(/, 'Current working directory:  ', a)
         endif

         if ((vnm(1:nc) .ne. '/') .or. (nc .ne. 1)) &
            call errproc
      else
         call errproc
      endif

! ... change directory
      if (DIR) then
         if (pd_cd_f(fileid, "dir1") .eq. 0) &
            call errproc
      endif

! ... get current working directory
      cp = pd_pwd_f(fileid)
      if (c_associated(cp)) then
         call c_f_pointer(cp, vnm)
         nc = c_strlenf(vnm)
         if (PRINT) then
            write(6, 352) vnm(1:nc)
         endif

         if (((.not. DIR) .and. (nc .ne. 1)) .or. &
             (DIR .and. (nc .ne. 5))) &
            call errproc
      endif

! ... inquire about a struct types
      cp = pd_inquire_type_f(fileid, "abc")
      if (.not. c_associated(cp)) &
         call errproc

      typsiz = pd_type_size_f(cp)
      align  = pd_type_alignment_f(cp)
      nptr   = pd_type_n_indirects_f(cp)

      if (PRINT) then
         write(6, 450) typsiz, align, nptr
 450     format(/, 'Size in bytes, alignment, and pointer members ', &
              'of type abc:', 3i5)
      endif

      if ((typsiz .ne. 72) .or. (nptr .ne. 0)) &
         call errproc

      nchar = 5
      name3 = "int l"
!      if (wpdimbr(fileid, 'jkl', nchar, name3) .eq. 0) &
!         call errproc

      name4 = 'int l'
      if ((nchar .ne. 5) .or. (name3 .ne. name4)) &
         call errproc

! ... inquire about unwritten variable
       cp = pd_inquire_entry_f(fileid, "q", 0, "")
       if (c_associated(cp)) &
          call errproc

       nitems = pd_entry_number_f(cp)
       ndims  = pd_entry_n_dimensions_f(cp)
       
      if (PRINT) then
         write(6, 500) nitems
 500     format(/, 'Number of items in variable q:', i5)
         write(6, 501) ndims
 501     format('Number of dimensions for variable q:', i5)
      endif

      if ((nitems .ne. 0) .or. (ndims .ne. 0)) &
         call errproc

! ... inquire about written variable
       cp = pd_inquire_entry_f(fileid, "c", 0, "")
       if (.not. c_associated(cp)) &
          call errproc

       nitems = pd_entry_number_f(cp)
       ndims  = pd_entry_n_dimensions_f(cp)
       
      if (PRINT) then
         write(6, 502) nitems
 502     format(/, 'Number of items in variable c:', i5)
         write(6, 503) ndims
 503     format('Number of dimensions for variable c:', i5)
      endif

      if ((nitems .ne. 6) .or. (ndims .ne. 2)) &
         call errproc

! ... close the file
      if (pd_close_f(fileid) .eq. 0) &
         call errproc

      if (PRINT) then
         write(6, *)
      endif

      return
      end subroutine test_2

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

! TEST_3 - feature testing

      subroutine test_3(PRINT, DIR, OPTION)
      use pact_fortran
      use pact_pdb
      implicit none 

      logical :: PRINT
      logical :: DIR
      character*8 :: OPTION

! ... local variables
      integer :: i, j, k, l, n
      integer :: j2, k2, l2
      integer :: ndims, nitems
      integer :: nvar, nc
      integer :: nvd(1)

      integer(8) :: dims(14)

      type(C_PTR) :: fileid, nfileid, cp
      type(C_PTR), pointer :: fcp(:)

      real*8 :: x(20)
      real*8 :: a, c, g, z, zz, zzx
      real*8 :: a2, c2, g2, z2, zz2
      real*8 :: abc_a, c_1_2
      real*8 :: TOLERANCE

      character(80), pointer :: vnm

      common /abc/ a(2), c(2, 2:4), g(5, 2:10), z(4, 5, 2)
      common /abc/ zz(4, 5, 2), zzx(4, 5, 2)
      common /abc2/ a2(2), c2(2, 2:4), g2(5, 2:10), z2(4, 5)
      common /abc2/ zz2(4, 5, 2)
      common /jkl/ j, k, l
      common /jkl2/ j2, k2, l2

      data  TOLERANCE /3.0e-14/

      if (PRINT) then
         write(6, *) '------------------------------------------------'
         write(6, *) 'Test #3'
      endif

! ... initialization
      do i = 1, 20
         x(i) = float(i)
      enddo

      do i = 1, 93
         a(i) = float(i)
      enddo

! ... initialize zz array (a(94)...)
      do i = 1, 40
         a(i+93) = float(i) + 1000.0
         a(i+40+93) = 2*(float(i) + 1000.0)
      enddo

      do i = 1, 73
         a2(i) = 0.0
      enddo

      j = 1
      k = 2
      l = 3

! ... open the original file for reading
      fileid = pd_open_f('file.pdb', 'a')
      if (.not. c_associated(fileid)) &
         call errproc

! ... change directory
      if (DIR) then
         if (pd_cd_f(fileid, "dir1") .eq. 0) &
            call errproc
      endif

! ... copy type definition to new file and write variable
      nfileid = pd_open_f('file2.pdb', 'w')
      if (.not. c_associated(nfileid)) &
         call errproc

      if (pd_copy_type_f(fileid, nfileid, 'jkl') .eq. 0) &
         call errproc

      j = 1
      k = 2
      l = 3
      if (pd_write_f(nfileid, 'jkl', 'jkl', j) .eq. 0) &
         call errproc

      if (pd_close_f(nfileid) .eq. 0) &
         call errproc

! ... verify correctness of variable
      if (pd_read_f(fileid, 'a', a2) .eq. 0) &
         call errproc

      if ((a(1) .ne. 1.) .or. (a(2) .ne. 2.)) &
         call errproc

! ... verify correctness of partially written variable
      if (pd_read_f(fileid, 'g', g2) .eq. 0) &
         call errproc

      if ((g2(1, 2) .ne. 100) .or. (g2(2, 2) .ne. 0)) &
         call errproc

! ... inquire about a structure member
       cp = pd_effective_entry_f(fileid, "abc.c", 0, "")
       if (.not. c_associated(cp)) &
          call errproc

       nitems = pd_entry_number_f(cp)
       ndims  = pd_entry_n_dimensions_f(cp)
       
      if (PRINT) then
         write(6, 504) nitems
 504     format(/, 'Number of items in abc.c:', i5)
         write(6, 505) ndims
 505     format('Number of dimensions for abc.c:', i5)
      endif

! ... read struct by link and verify
      if (pd_read_f(fileid, '/jkl_link', j2) .eq. 0) &
         call errproc

      if ((j2 .ne. 1) .or. (k2 .ne. 2) .or. (l2 .ne. 3)) &
         call errproc

! ... read struct member
      if (pd_read_f(fileid, 'jkl.k', k2) .eq. 0) &
         call errproc

      if (PRINT) then
         write(6, 600) k
 600     format(/, 'Value of jkl.k written:', i5)
         write(6, 601) k2
 601     format('Value of jkl.k read:', i5)
      endif

      if (k .ne. k2) &
         call errproc

      dims(1) = 2
      dims(2) = 2
      dims(3) = 1
      if (pd_read_alt_f(fileid, 'abc.a', abc_a, dims) .eq. 0) &
         call errproc

      if (PRINT) then
         write(6, 602)
 602     format(/, 'Value of abc.a(2) written:  2.0')
         write(6, 603) abc_a
 603     format('Value of abc.a(2) read:', f5.1)
      endif

      if (abc_a .ne. 2.0) &
         call errproc

! ... read elements of struct members
      if (pd_read_f(fileid, 'abc.a(2)', abc_a) .eq. 0) &
         call errproc

      if (abc_a .ne. 2.0) &
         call errproc

      if (pd_read_f(fileid, 'abc.c(1,2)', c_1_2) .eq. 0) &
         call errproc

      if (c_1_2 .ne. 4.0) &
         call errproc

! ... get the variable names in alphabetic order and check last name
      if (PRINT) then
         write(6, 700)
 700     format(/, 'Alphabetic list of variables:')
      endif

      cp   = pd_ls_f(fileid, "/", "*", nvd)
      nvar = nvd(1)
      call c_f_pointer(cp, fcp, (/nvar/))
      do n = 1, nvar
         call c_f_pointer(fcp(n), vnm)
         nc = c_strlenf(vnm)
         if (PRINT) then
            write(6, 702) vnm(1:nc)
 702        format('    ', a)
         endif
         if ((n .eq. 1) .and.                           &
             (DIR .and. (vnm(1:nc) .ne. 'x')))          &
            call errproc
      enddo

      call sc_free_strings_f(cp)

! ... get the variable and directory names in dir1
      nvar = 0

      if (DIR) then
         if (PRINT) then
            write(6, 850)
 850        format(/, 'Variables and directories in dir1:')
         endif

         cp   = pd_ls_f(fileid, "/dir1", "*", nvd)
         nvar = nvd(1)
         call c_f_pointer(cp, fcp, (/nvar/))
         do n = 1, nvar
            call c_f_pointer(fcp(n), vnm)
            nc = c_strlenf(vnm)
            if (PRINT) then
               write(6, 702) vnm(1:nc)
            endif
            if ((n .eq. 1) .and.                           &
                ((DIR .and. (vnm(1:nc) .ne. 'x')) .or.     &
                ((.not. DIR) .and. (vnm(1:nc) .ne. 'a')))) &
               call errproc
            enddo

            call sc_free_strings_f(cp)
      endif

! ... check curve, image, mapping
      if (OPTION .ne. "0") then
         call chkima(fileid)
         call chkcur(fileid, TOLERANCE)
         call chkmap(fileid, TOLERANCE)
      endif

! ... close the file
      if (pd_close_f(fileid) .eq. 0) &
         call errproc

      if (PRINT) then
         write(6, *)
      endif

      return
      end subroutine test_3

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

! TPDFG - main test routine

      program tpdfg
      use pact_fortran
      use pact_score
      use pact_pdb
      implicit none 

! ... program input command option flags
      logical :: PRINT, DIR, TARGET, ATTR

      integer :: is, st
      integer :: narg, iarg
      integer :: nout, err

      integer*8 :: bufsiz1, bufsiz2, bufsiz3
      type(C_PTR) :: ftid

      character*8 :: outtype
      character*8 :: arg
      character*80 :: date
      character*12 :: vers

! ... OPTION determines when curve, image and mappings are written:
!        0 - none
!        1 - before directory created
!        2 - in root directory
!        3 - in directory dir1
      character*8 :: OPTION

      ftid = C_NULL_PTR
      err  = pd_init_threads_f(0, ftid)
      err  = sc_zero_space_n_f(0, -1)

! ... initialize input command option flags
      PRINT  = .false.
      DIR    = .false.
      TARGET = .false.
      ATTR   = .false.
      OPTION = "0"

      nout    = 6
      outtype = 'double'
       
!      narg = iargc()
      narg = command_argument_count()
      iarg = 1

 10   if (iarg .le. narg) then
!         call getarg(iarg, arg)
         call get_command_argument(iarg, arg)
         if (arg .eq. "-a") then
            ATTR = .true.
         elseif (arg .eq. "-as") then
            iarg = iarg + 1
!            call getarg(iarg, arg)
            call get_command_argument(iarg, arg)
            if (arg .eq. "d") then
               nout    = 6
               outtype = 'double'
            elseif (arg.eq."f") then
               nout    = 5
               outtype = 'float'
            endif
         elseif (arg .eq. "-d") then
            DIR = .true.
         elseif (arg .eq. "-h" .or. arg .eq. "help") then
            write(6, *) 'Usage: tpdfg [-a f|d] [-as] [-d] [-h] &
&[-o opt] [-p] [-t]'
            write(6, *) '    Test attributes; make directory;'
            write(6, *) '    write curve, image and mappings in'
            write(6, *) '    directory dir1; write double as float'
            write(6, *) '    to CRAY Unicos file.'
            write(6, *) ' '
            write(6, *) '    -h   this help message'
            write(6, *) '    -p   print progress to terminal (df=false)'
            write(6, *) '    -a   define, set, remove attributes (df=true)'
            write(6, *) '    -d   make directory (df=false)'
            write(6, *) '    -o   write curve, image and mappings: (df=0)'
            write(6, *) '           0 - none'
            write(6, *) '           1 - before directory created'
            write(6, *) '           2 - in root directory'
            write(6, *) '           3 - in directory dir1'
            write(6, *) '    -as  set write_as to float or double (df=d)'
            write(6, *) '    -t   set target to CRAY (df=false)'
            stop 1
         elseif (arg .eq. "-o") then
            iarg = iarg + 1
!            call getarg(iarg, arg)
            call get_command_argument(iarg, arg)
            OPTION = arg
         elseif (arg .eq. "-p") then
            PRINT = .true.
         elseif (arg .eq. "-t") then
            TARGET = .true.
         endif
         iarg = iarg + 1
         go to 10
      endif

      if (scvers(12, vers) .gt. 8) then
         write(6, 2)
 2       format(/, 'Version call failed')
         stop 1
      endif

      st = scftcs(date, 'Wednesday August 31, 1994', 25)

      if (PRINT) then
         write(6, 1) date
 1       format(/, 'A date: ', 1a25)
      endif

! ... set and get buffer size
      bufsiz1 = 4096
      bufsiz2 = pd_set_buffer_size_f(bufsiz1)
      bufsiz3 = pd_get_buffer_size_f()
      if (bufsiz3 .eq. -1) then
         write(6, 200)
 200     format(/, 'Buffer size not previously set.')
         stop 1
      endif

      if (PRINT) then 
         write(6, 201) bufsiz3
 201     format(/, 'Buffer size in bytes:', i8)
      endif

      if ((bufsiz1 .ne. bufsiz2) .or. (bufsiz2 .ne. bufsiz3)) &
         call errproc

! ... set target architecture
      if (TARGET) then
         if (PRINT) then
            write(6, 205)
 205        format(/, 'TARGET is CRAY')
         endif

! ... CRAY standard and alignment
         is = 9

         if (pd_target_platform_n_f(is) .eq. 0) &
            call errproc
      endif

      call test_1(outtype, date, PRINT, DIR, ATTR, OPTION)
      call test_2(date, PRINT, DIR, ATTR)
      call test_3(PRINT, DIR, OPTION)

      if (PRINT) then
         write(6, *) '------------------------------------------------'
         write(6, *)
      endif

      stop 0
      end

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

