!
! PDFGTS.F - test the Fortran application interface to PDB
!          - use the wrappers generated by blang
!
! Source Version: 3.0
! Software Release #: LLNL-CODE-422942
!
! include "cpyright.h"
!

! --------------------------------------------------------------------------
! --------------------------------------------------------------------------

! ERRPROC - handle errors

      subroutine errproc
      use pact_pdb
      implicit none

! ... local variables
      integer, parameter :: MAXMSG = 256
      integer nchar
      character*256 err

      nchar = MAXMSG
      if ((wpdgerr(nchar, err) .eq. 1) .and. (nchar .gt. 0)) then
         nchar = min(nchar, MAXMSG)
         write(6, 100)
 100     format()
         write(6, *) err
      else
         write(6, 102)
 102     format(/, 'Error in PDWFTS', /)
      endif

      stop 1
      end

! --------------------------------------------------------------------------
! --------------------------------------------------------------------------

! WRTCUR - write a curve

      subroutine wrtcur(fileid)
      use pact_pdb
      implicit none

      integer(isizea) fileid

! ... local variables
      integer i, index, st
      real*8 dm(0:99), rm1(0:99), rm2(0:99)

      index = 0

      do i = 0, 99
         dm(i)  = 6.28*float(i)/99.
         rm1(i) = cos(6.28*float(i)/99.)
         rm2(i) = sin(6.28*float(i)/99.)
      enddo

      st = wpdwulc(fileid, "cosine curve", 100, dm, rm1, index)
      if (st .eq. 0) &
         call errproc

      st = wpdwuly(fileid, "sin curve", 100, index - 1, rm2, index)
      if (st .eq. 0) &
         call errproc

      return
      end subroutine wrtcur

! --------------------------------------------------------------------------
! --------------------------------------------------------------------------

! CHKCUR - check curve

      subroutine chkcur(fileid, tolerance)
      use pact_pdb
      implicit none

      integer(isizea) fileid
      real*8 tolerance

! ... local variables
      integer i
      real*8 dm(0:99), rm1(0:99), rm2(0:99)
      real*8 odm(0:99), orm1(0:99), orm2(0:99)

      do i = 0, 99
         dm(i)  = 6.28*float(i)/99.
         rm1(i) = cos(6.28*float(i)/99.)
         rm2(i) = sin(6.28*float(i)/99.)
      enddo

      if (wpdread(fileid, 5, "xval0", odm) .eq. 0) &
         call errproc

      if (wpdread(fileid, 5, "yval0", orm1) .eq. 0) &
         call errproc

      if (wpdread(fileid, 5, "yval1", orm2) .eq. 0) &
         call errproc

      do i = 0, 99
         if ((abs(dm(i) - odm(i)) .gt. tolerance) &
              .or. (abs(rm1(i) - orm1(i)) .gt. tolerance) &
              .or. (abs(rm2(i) - orm2(i)) .gt. tolerance)) &
            call errproc
      enddo

      return
      end subroutine chkcur

! --------------------------------------------------------------------------
! --------------------------------------------------------------------------

! WRTIMA - write an image

      subroutine wrtima(fileid)
      use pact_pdb
      implicit none

      integer(isizea) fileid

! ... local variables
      integer k, l, st
      real*8 xmin, xmax, ymin, ymax, data(0:10, 0:10)

      xmin = 0.
      xmax = 10.
      ymin = 0.
      ymax = 10.

      do l = 0, 10
         do k = 0, 10
            data(k, l) = float((k - 5)**2 + (l - 5)**2)
         enddo
      enddo

      st = wpdwima(fileid, 10, 'Test image', 0, 10, 0, 10,  &
                  data, xmin, xmax, ymin, ymax, 0)
      if (st .eq. 0) &
         call errproc

      return
      end subroutine wrtima

! --------------------------------------------------------------------------
! --------------------------------------------------------------------------

! CHKIMA - check image

      subroutine chkima(fileid)
      use pact_pdb
      implicit none

      integer fileid

! ... local variables
      integer k, l, st
      real*8 data(0:10, 0:10), odata(0:10, 0:10)

      do l = 0, 10
         do k = 0, 10
            data(k, l) = float((k - 5)**2 + (l - 5)**2)
         enddo
      enddo

      st = wpdrptr(fileid, 14, 'Image0->buffer', 121, odata)
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

! --------------------------------------------------------------------------
! --------------------------------------------------------------------------

! WRTMAP - write some mappings

      subroutine wrtmap(fileid)
      use pact_pdb
      implicit none

      integer fileid

! ... local variables
      integer i, pim, st
      integer dp(5), rp(5)
      real*8 dm(0:99), rm(0:99)

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

      do i = 0, 99
         dm(i) = 6.28*float(i)/99.
         rm(i) = sin(6.28*float(i)/99.)
      enddo

      st = wpdwmap(fileid, 'Domain0', dp, dm, 'Range0', rp, rm, pim)
      if (st .eq. 0) &
         call errproc

      pim = 1

      if (wpdwset(fileid, 'Domain1', dp, dm) .eq. 0) &
         call errproc

      if (wpdwran(fileid, 'Domain1', 7, 'Range1', rp, rm, pim) .eq. 0) &
         call errproc

      return
      end subroutine wrtmap

! --------------------------------------------------------------------------
! --------------------------------------------------------------------------

! CHKMAP - check mappings

      subroutine chkmap(fileid, tolerance)
      use pact_pdb
      implicit none

      integer fileid
      real*8 tolerance

! ... local variables
      integer i, st
      real*8 dm(0:99), rm(0:99)
      real*8 odm(0:99), orm(0:99)

      do i = 0, 99
         dm(i) = 6.28*float(i)/99.0
         rm(i) = sin(6.28*float(i)/99.0)
      enddo

      st = wpdrptr(fileid, 29, 'Mapping0->domain->elements[1]', 100, odm)
      if (st .eq. 0) &
         call errproc

      st = wpdread(fileid, 35, 'Mapping0->range->elements[1][1:100]', orm)
      if (st .eq. 0) &
         call errproc

      do i = 0, 99
         if ((abs(dm(i) - odm(i)) .gt. tolerance) &
              .or. (abs(rm(i) - orm(i)) .gt. tolerance)) &
            call errproc
      enddo

      st = wpdread(fileid, 27, 'Domain1->elements[1][1:100]', odm)
      if (st .eq. 0) &
         call errproc

      st = wpdrptr(fileid, 28, 'Mapping1->range->elements[1]', 100, orm)
      if (st .eq. 0) &
         call errproc

      do i = 0, 99
         if ((abs(dm(i) - odm(i)) .gt. tolerance) &
              .or. (abs(rm(i) - orm(i)) .gt. tolerance)) &
            call errproc
      enddo

      return
      end subroutine chkmap

! --------------------------------------------------------------------------
! --------------------------------------------------------------------------

! TEST_1 - basic file testing

      subroutine test_1(nout, outtype, date, PRINT, DIR, ATTR, OPTION)
      use pact_pdb
      implicit none 

      integer nout
      character*8 outtype
      character*80 date
      logical PRINT
      logical DIR
      logical ATTR
      character*8  OPTION

! ... local variables
      integer i, j, k, l
      integer fileid, offset, st
      integer ndims, dims(14), pairs(6)
      integer nchar
      integer istring
      integer LAST
      
      real*8 a, c, g, z, zz, zzx
      real*8 x(20)

      character*256 name
      character*8 strings(4)

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
      fileid = wpdopen(8, 'file.pdb', 'w')
      if (fileid .le. 0) &
         call errproc

! ... faux fam
      if (wpdfami(fileid, 1) .eq. 0) &
         call errproc

! ... check file mode
      if (wpdgmod(fileid) .ne. 4) &
         call errproc

! ... set and get and verify default offset
      offset = 1
      if (wpdsoff(fileid, offset) .eq. 0) &
         call errproc
      offset = wpdgoff(fileid)

      if (PRINT) then
         write(6, 300) offset
 300     format(/, 'Current default offset:', i4)
      endif

      if (offset .ne. 1) &
         call errproc

! ... write at least one variable before creating a directory
      if (wpdwrta(fileid, 5, 'x(20)', 6, 'double', x) .eq. 0) &
         call errproc

      if (wpdappa(fileid, 7, 'x(1:10)', x) .eq. 0) &
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
         if (wpdmkdr(fileid, 4, "dir1") .eq. 0) &
            call errproc
      endif

! ... get current working directory
      if (wpdpwd(fileid, nchar, name) .eq. 0) &
         call errproc

      if (PRINT) then
         write(6, 350) name(1:nchar)
 350     format(/, 'Current working directory:  ', a)
      endif

      if ((name(1:nchar) .ne. '/') .or. (nchar .ne. 1)) &
         call errproc

! ... write curve, image, and mappings in root after creating directory
      if (OPTION .eq. "2") then
         call wrtcur(fileid)
         call wrtima(fileid)
         call wrtmap(fileid)
      endif

      x(1) = 100

! ... write  data item z   write_as,  append,  append_alt
      st = wpdwras(fileid, 8, 'z(4,5,1)', 6, 'double', 6, 'double', z)
      if (st .eq. 0) &
         call errproc

      if (wpdappa(fileid, 14, 'z(1:4,1:5,1:1)', z) .eq. 0) &
         call errproc

      ndims = 3
      dims(1) = 1
      dims(2) = 4
      dims(3) = 1
      dims(4) = 1
      dims(5) = 5
      dims(6) = 1
      dims(7) = 1
      dims(8) = 2
      dims(9) = 1
      if (wpdappd(fileid, 1, 'z', z, ndims, dims) .eq. 0) &
         call errproc

! ... write  data item zz   write_as_alt
!                                     append_as,  append_as_alt

      ndims = 3
      dims(1) = 1
      dims(2) = 4
      dims(3) = 1
      dims(4) = 1
      dims(5) = 5
      dims(6) = 1
      dims(7) = 1
      dims(8) = 1
      dims(9) = 1
      st = wpdwrad(fileid, 2, 'zz', 6, 'double', nout, outtype, zz, &
                  ndims, dims)
      if (st .eq. 0) &
         call errproc

      st = wpdapas(fileid, 15, 'zz(1:4,1:5,1:1)', 6, 'double', zz(1, 1, 2))
      if (st .eq. 0) &
         call errproc

      ndims = 3
      dims(1) = 1
      dims(2) = 4
      dims(3) = 1
      dims(4) = 1
      dims(5) = 5
      dims(6) = 1
      dims(7) = 1
      dims(8) = 2
      dims(9) = 1
      if (wpdapad(fileid, 2, 'zz', 6, 'double', zzx, ndims, dims) .eq. 0) &
         call errproc


      if (PRINT) then 
         write(6, 305) 
 305     format(/, 'Variables z and zz written', /)
      endif

! ... change directory
      if (DIR) then
         if (wpdcd(fileid, 4, "dir1") .eq. 0) &
            call errproc
      endif

! ... get current working directory
      if (wpdpwd(fileid, nchar, name) .eq. 0) &
         call errproc

      if (PRINT) then
         write(6, 351) name(1:nchar)
 351     format(/, 'Current working directory:  ', a)
      endif

      if (((.not. DIR) .and. (nchar .ne. 1)) .or.&
          (DIR .and. (nchar .ne. 5))) &
         call errproc

! ... create a link
      if (DIR) then
         if (wpdln(fileid, 2, '/x', 6, 'x_link') .eq. 0) &
            call errproc
      else
         if (wpdln(fileid, 1, 'x', 6, 'x_link') .eq. 0) &
            call errproc
      endif

! ... define and write some data structures
      st = wpddefs(fileid, 3, 'abc', 11, 'double a(2)', 8, 'double b', &
                  15, 'double c(2, 2:4)', LAST)
      if (st .eq. 0) &
         call errproc

      if (wpdwrta(fileid, 3, 'abc', 3, 'abc', a) .eq. 0) &
         call errproc

      pairs(1) = 0
      pairs(2) = 5
      pairs(3) = 5
      pairs(4) = 5
      pairs(5) = 10
      pairs(6) = 5
      if (wpddeft(fileid, 3, 'jkl', 3, pairs, 'int jint kint l') .eq. 0) &
         call errproc

      if (wpdwrta(fileid, 3, 'jkl', 3, 'jkl', j) .eq. 0) &
         call errproc

! ... define and write arrays of strings
      strings(1) = 'abcdefgh'
      strings(2) = 'hgfedcba'
      strings(3) = 'ijklmnop'
      strings(4) = 'ponmlkji'

      ndims = 2
      dims(1) = 1
      dims(2) = 8
      dims(3) = 1
      dims(4) = 1
      dims(5) = 4
      dims(6) = 1
      st = wpdwrtd(fileid, 7, 'strings', 4, 'char', istring, &
                  ndims, dims)
      if (st .eq. 0) &
         call errproc

! ... define an attribute
      if (ATTR) then
         if (wpddatt(fileid, 4, 'date', 6, 'char *') .eq. 0) &
            call errproc
      endif

! ... define some symbol table entries and reserve disk space
      if (wpddefa(fileid, 14, 'f(0:3, 4:6, 1:2)', 6, 'double') .eq. 0) &
         call errproc

      ndims = 2
      dims(1) = 1
      dims(2) = 5
      dims(3) = 2
      dims(4) = 10
      if (wpddefd(fileid, 1, 'g', 6, 'double', ndims, dims) .eq. 0) &
         call errproc

      ndims = 2
      dims(1) = 1
      dims(2) = 1
      dims(3) = 1
      dims(4) = 2
      dims(5) = 2
      dims(6) = 1
      g(1, 2) = 100
      g(2, 2) = 101
      if (wpdwrtd(fileid, 1, 'g', 6, 'double', g, ndims, dims) .eq. 0) &
         call errproc
!      if (wpdwrtd(fileid, 6, 'g[1, 2]', 6, 'double', g, ndims, dims) &
!           .eq. 0) &
!         call errproc

      ndims = 1
      dims(1) = 1
      dims(2) = 2
      dims(3) = 1
      if (wpdwrtd(fileid, 1, 'a', 6, 'double', a, ndims, dims) .eq. 0) &
         call errproc

      ndims = 2
      dims(1) = 1
      dims(2) = 2
      dims(3) = 1
      dims(4) = 2
      dims(5) = 4
      dims(6) = 1
      if (wpdwrtd(fileid, 1, 'c', 6, 'double', c, ndims, dims) .eq. 0) &
         call errproc

! ... create a link
      if (wpdln(fileid, 3, 'jkl', 9, '/jkl_link') .eq. 0) &
         call errproc

! ... set attribute value for member of struct
      if (ATTR) then
         if (wpdsvat(fileid, 11, '/jkl_link.k', 4, 'date', date) .eq. 0) &
            call errproc
      endif

! ... write curve, image, and mappings in a directory
      if (OPTION .eq. "3") then
         call wrtcur(fileid)
         call wrtima(fileid)
         call wrtmap(fileid)
      endif

! ... flush the file
      if (wpdflsh(fileid) .eq. 0) &
         call errproc

! ... close the file
      if (wpdclos(fileid) .eq. 0) &
         call errproc

      if (PRINT) then
         write(6, *)
      endif

      return
      end subroutine test_1

! --------------------------------------------------------------------------
! --------------------------------------------------------------------------

! TEST_2 - feature testing

      subroutine test_2(date, PRINT, DIR, ATTR)
      use pact_pdb
      implicit none 

      character*80 date
      logical PRINT
      logical DIR
      logical ATTR

! ... local variables
      integer i, j, k, l, n
      integer nt, fileid, st
      integer ntype, ndims, dims(14), nitems
      integer typsiz, align, nptr
      integer kindx
      integer nchar
       
      character*256 name
      character*8 name1, name2
      character*5 name3, name4
      character*80 dt
      character*72 type

      real*8 x(20)
      real*8 a, c, g, z, zz, zzx
      real*8 a2, c2, g2, z2, zz2
      real*8 v1, v2
      real*8 TOLERANCE

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
         a(i+93) = float(i) + 1000.0
         a(i+40+93) = 2*(float(i) + 1000.0)
      enddo

      do i = 1, 73
         a2(i) = 0.
      enddo

      j = 1
      k = 2
      l = 3

! ... open the file for reading
      fileid = wpdopen(8, 'file.pdb', 'a')
      if (fileid .eq. 0) &
         call errproc

! ... get file name given id
      nchar = 8
      if (wpdgfnm(fileid, nchar, name1) .eq. 0) &
         call errproc

      name2 = 'file.pdb'
      if (name1 .ne. name2) &
         call errproc

! ... check file mode
      if (wpdgmod(fileid) .ne. 2) &
         call errproc

! ... retrieve and verify the value of an attribute
      if (ATTR) then
         if (wpdcd(fileid, 4, "dir1") .eq. 0) &
            call errproc

         if (wpdgvat(fileid, 11, '/jkl_link.k', 4, 'date', dt) .eq. 0) &
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
         if (wpdratt(fileid, 4, 'date') .eq. 0) &
            call errproc

! ... verify that attribute was removed
         if (wpdgvat(fileid, 11, '/jkl_link.k', 4, 'date', dt) &
              .ne. 0) then
            write(6, 380)
 380        format(/, 'Attribute was not removed.')
            stop 1
         endif
      
         if (wpdcd(fileid, 1, "/") .eq. 0) &
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
         if (wpdptrd(fileid, 1, 'z', z2, dims) .eq. 0) &
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

      if (wpdrdad(fileid, 2, 'zz', 6, 'double', zz2, dims) .eq. 0) &
         call errproc
!      if (wpdptrd(fileid, 2, 'zz', zz2, dims) .eq. 0) &
!         call errproc
      do kindx = 1, 2
         do j = 1, 5
            do i = 1, 4
               if (abs(zz2(i, j, kindx) - zz(i, j, kindx)) &
                   .gt. TOLERANCE) &
                  call errproc
            enddo
         enddo
      enddo

      if (wpdrdas(fileid, 15, 'zz(1:4,1:5,3:4)', 6, 'double', zz2) &
             .eq. 0) call errproc
!      if (wpdread(fileid, 15, 'zz(1:4,1:5,3:4)', zz2) &
!             .eq. 0) call errproc

         do kindx = 1, 2
            do j = 1, 5
               do i = 1, 4
                 if (abs(zz2(i, j, kindx) - zzx(i, j, kindx)) &
                     .gt. TOLERANCE) &
                    call errproc
            enddo
         enddo
      enddo

      if (PRINT) then
         write(6, 430)
 430     format(/, 'Values of zz read match values written.')
      endif


! ... get current working directory
      if (wpdpwd(fileid, nchar, name) .eq. 0) &
         call errproc

      if (PRINT) then
         write(6, 352) name(1:nchar)
 352     format(/, 'Current working directory:  ', a)
      endif

      if ((name(1:nchar) .ne. '/') .or. (nchar .ne. 1)) &
         call errproc

! ... change directory
      if (DIR) then
         if (wpdcd(fileid, 4, "dir1") .eq. 0) &
            call errproc
      endif

! ... get current working directory
      if (wpdpwd(fileid, nchar, name) .eq. 0) &
         call errproc

      if (PRINT) then
         write(6, 352) name(1:nchar)
      endif

      if (((.not. DIR) .and. (nchar .ne. 1)) .or.&
          (DIR .and. (nchar .ne. 5))) &
         call errproc

! ... inquire about a struct types
      if (wpdityp(fileid, 3, 'abc', typsiz, align, nptr) .eq. 0) &
         call errproc

      if (PRINT) then
         write(6, 450) typsiz, align, nptr
 450     format(/, 'Size in bytes, alignment, and pointer members ', &
              'of type abc:', 3i5)
      endif

      if ((typsiz .ne. 72) .or. (nptr .ne. 0)) &
         call errproc

      nchar = 5
      if (wpdimbr(fileid, 3, 'jkl', 3, nchar, name3) .eq. 0) &
         call errproc

      name4 = 'int l'
      if ((nchar .ne. 5) .or. (name3 .ne. name4)) &
         call errproc

! ... inquire about unwritten variable
      st = wpdivar(fileid, 1, 'f', ntype, type, nitems, ndims, dims)
      if (st .eq. 0) &
         call errproc

      if (PRINT) then
         write(6, 500) nitems
 500     format(/, 'Number of items in variable f:', i5)
         write(6, 501) ndims
 501     format('Number of dimensions for variable f:', i5)
      endif

      if ((nitems .ne. 0) .or. (ndims .ne. 0)) &
         call errproc

! ... inquire about written variable
      st = wpdivar(fileid, 1, 'c', ntype, type, nitems, ndims, dims)
      if (st .eq. 0) &
         call errproc

      if (PRINT) then
         write(6, 502) nitems
 502     format(/, 'Number of items in variable c:', i5)
         write(6, 503) ndims
 503     format('Number of dimensions for variable c:', i5)
      endif

      if ((nitems .ne. 6) .or. (ndims .ne. 2)) &
         call errproc

! ... close the file
      if (wpdclos(fileid) .eq. 0) &
         call errproc

      if (PRINT) then
         write(6, *)
      endif

      return
      end subroutine test_2

! --------------------------------------------------------------------------
! --------------------------------------------------------------------------

! TEST_3 - feature testing

      subroutine test_3(PRINT, DIR, OPTION)
      use pact_pdb
      implicit none 

      logical PRINT
      logical DIR
      character*8  OPTION

! ... local variables
      integer i, j, k, l, n
      integer j2, k2, l2
      integer fileid, nfileid, st
      integer ntype, ndims, nitems
      integer pord, nvar, nchar
      integer dims(14)

      real*8 x(20)
      real*8 a, c, g, z, zz, zzx
      real*8 a2, c2, g2, z2, zz2
      real*8 abc_a, c_1_2
      real*8 TOLERANCE

      character*72 type
      character*128 vname

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
         a2(i) = 0.
      enddo

      j = 1
      k = 2
      l = 3

! ... open the original file for reading
      fileid = wpdopen(8, 'file.pdb', 'a')
      if (fileid .eq. 0) &
         call errproc

! ... change directory
      if (DIR) then
         if (wpdcd(fileid, 4, "dir1") .eq. 0) &
            call errproc
      endif

! ... copy type definition to new file and write variable
      nfileid = wpdopen(9, 'file2.pdb', 'w')
      if (nfileid .le. 0) &
         call errproc

      if (wpdctyp(fileid, nfileid, 3, 'jkl') .eq. 0) &
         call errproc

      j = 1
      k = 2
      l = 3
      if (wpdwrta(nfileid, 3, 'jkl', 3, 'jkl', j) .eq. 0) &
         call errproc

      if (wpdclos(nfileid) .eq. 0) &
         call errproc

! ... verify correctness of variable
      if (wpdread(fileid, 1, 'a', a2) .eq. 0) &
         call errproc

      if ((a(1) .ne. 1.) .or. (a(2) .ne. 2.)) &
         call errproc

! ... verify correctness of partially written variable
      if (wpdread(fileid, 1, 'g', g2) .eq. 0) &
         call errproc

      if ((g2(1, 2) .ne. 100) .or. (g2(2, 2) .ne. 0)) &
         call errproc

! ... inquire about a structure member
      st = wpdivar(fileid, 3, 'abc.c', ntype, type, nitems, ndims, dims)
      if (st .eq. 0) &
         call errproc

      if (PRINT) then
         write(6, 504) nitems
 504     format(/, 'Number of items in abc.c:', i5)
         write(6, 505) ndims
 505     format('Number of dimensions for abc.c:', i5)
      endif

! ... read struct by link and verify
      if (wpdread(fileid, 9, '/jkl_link', j2) .eq. 0) &
         call errproc

      if ((j2 .ne. 1) .or. (k2 .ne. 2) .or. (l2 .ne. 3)) &
         call errproc

! ... read struct member
      if (wpdread(fileid, 5, 'jkl.k', k2) .eq. 0) &
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
      if (wpdptrd(fileid, 5, 'abc.a', abc_a, dims) .eq. 0) &
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
      if (wpdread(fileid, 8, 'abc.a(2)', abc_a) .eq. 0) &
         call errproc

      if (abc_a .ne. 2.0) &
         call errproc

      if (wpdread(fileid, 10, 'abc.c(1,2)', c_1_2) .eq. 0) &
         call errproc

      if (c_1_2 .ne. 4.0) &
         call errproc

! ... get the variable names in alphabetic order and check last name
      pord = 1
      nvar = 0
      if (wpdvart(fileid, pord, nvar) .eq. 0) &
         call errproc

      if (PRINT) then
         write(6, 700)
 700     format(/, 'Alphabetic list of variables:')
      endif

      do n = 1, nvar
         if (wpdgvar(n, nchar, vname) .eq. 0) &
            call errproc
         if (PRINT) then
            write(6, 702) vname(1:nchar)
 702        format('    ', a)
         endif
      enddo

!      if ((DIR .and. (vname(1) .ne. 'x')) .or.&
!          ((.not. DIR) .and. (vname(1) .ne. 'z'))) &
!         call errproc

      if (wpddvar() .eq. 0) &
         call errproc

! ... get the variable and directory names in root
      if (wpdlst(fileid, 1, '/', 0, ' ', nvar) .eq. 0) &
         call errproc

      if (PRINT) then
         write(6, 800)
 800     format(/, 'Variables and directories in root:')
      endif

      do n = 1, nvar
         if (wpdgls(n, nchar, vname) .eq. 0) &
            call errproc
         if (PRINT) then
            write(6, 702) vname(1:nchar)
         endif
      enddo

      if (vname(1:1) .ne. 'z') &
         call errproc

! ... get the variable and directory names in dir1
      nvar = 0

      if (DIR) then
         if (wpdlst(fileid, 5, '/dir1', 0, ' ', nvar) .eq. 0) &
            call errproc
      endif

      if (PRINT) then
         write(6, 850)
 850     format(/, 'Variables and directories in dir1:')
      endif

      do n = 1, nvar
         if (wpdgls(n, nchar, vname) .eq. 0) &
            call errproc
         if (PRINT) then
            write(6, 702) vname(1:nchar)
         endif
      enddo

      if (wpddls() .eq. 0) &
         call errproc

! ... check curve, image, mapping
      if (OPTION .ne. "0") then
         call chkcur(fileid, TOLERANCE)
         call chkima(fileid)
         call chkmap(fileid, TOLERANCE)
      endif

! ... close the file
      if (wpdclos(fileid) .eq. 0) &
         call errproc

      if (PRINT) then
         write(6, *)
      endif

      return
      end subroutine test_3

! --------------------------------------------------------------------------
! --------------------------------------------------------------------------

! PDFGTS - main test routine

      program pdfgts
      use score
      use pact_pdb
      implicit none 

! ... program input command option flags
      logical PRINT, DIR, TARGET, ATTR

      integer is, ia, st
      integer bufsiz1, bufsiz2, bufsiz3
      integer narg, iarg, iargc
      integer nout, err

      character*8 outtype
      character*8 arg
      character*80 date
      character*12 vers

! ... OPTION determines when curve, image and mappings are written:
!        0 - none
!        1 - before directory created
!        2 - in root directory
!        3 - in directory dir1
      character*8  OPTION

      err = wpdinth(0, 0)
      err = sczrsp(0)

! ... initialize input command option flags
      PRINT  = .false.
      DIR    = .false.
      TARGET = .false.
      ATTR   = .false.
      OPTION = "0"

      nout    = 6
      outtype = 'double'
       
      narg = iargc()
      iarg = 1

 10   if (iarg .le. narg) then
         call getarg(iarg, arg)
         if (arg .eq. "-p") then
            PRINT = .true.
         elseif (arg .eq. "-a") then
            ATTR = .true.
         elseif (arg .eq. "-o") then
            iarg = iarg + 1
            call getarg(iarg, arg)
            OPTION = arg
         elseif (arg .eq. "-d") then
            DIR = .true.
         elseif (arg .eq. "-t") then
            TARGET = .true.
         elseif (arg .eq. "-as") then
            iarg = iarg + 1
            call getarg(iarg, arg)
            if (arg .eq. "d") then
               nout    = 6
               outtype = 'double'
            elseif (arg.eq."f") then
               nout    = 5
               outtype = 'float'
            endif
         elseif (arg .eq. "-h" .or. arg .eq. "help") then
            write(6, *) 'Usage: pdfgts [-a f|d] [-as] [-d] [-h] &
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
      bufsiz2 = wpdsbfs(bufsiz1)
      bufsiz3 = wpdgbfs()
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
         ia = 4

         if (wpdtrgt(is, ia) .eq. 0) &
            call errproc
      endif

      call test_1(nout, outtype, date, PRINT, DIR, ATTR, OPTION)
      call test_2(date, PRINT, DIR, ATTR)
      call test_3(PRINT, DIR, OPTION)

      if (PRINT) then
         write(6, *) '------------------------------------------------'
         write(6, *)
      endif

      stop 0
      end

! --------------------------------------------------------------------------
! --------------------------------------------------------------------------

