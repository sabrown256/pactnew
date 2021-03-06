!
! TPDFF.F - test the Fortran application interface to PDB
!
! Source Version: 3.0
! Software Release #: LLNL-CODE-422942
!
! include "cpyright.h"
!
! Usage: 
!   tpdf [help|-h]
!     Print this help package
!  
!   tpdf
!     Test attributes; make directory;
!     write curve, image and mappings in
!     directory dir1; write double as float
!     to CRAY Unicos file.
!  
!   tpdf [-p] [-a] [-d] [-o opt] [-as] [-t]
!     -p   PRINT progress to terminal          (df = false)
!     -a   Define, set, remove attributes      (df = true)
!     -d   Make Directory                      (df = false)
!     -o   Write curve, image and mappings:     (df = 0)
!            0 - none
!            1 - before directory created
!            2 - in root directory
!            3 - in directory dir1
!     -as f|d                                  (df = d)
!          Set write_as to float or double
!     -t   Set target to CRAY Unicos           (df = false)
!

! --------------------------------------------------------------------------
! --------------------------------------------------------------------------

! ERRPROC - handle errors

      subroutine errproc
!      use pdb
      implicit none

      integer pfgerr

      integer, parameter :: MAXMSG = 256
      integer i, nchar
      character err(MAXMSG)

      nchar = MAXMSG
      if ((pfgerr(nchar, err) .eq. 1) .and. (nchar .gt. 0)) then
         nchar = min(nchar, MAXMSG)
         write(6, 100)
 100     format()
         write(6, 101) (err(i), i=1, nchar)
 101     format(72a1)
      else
         write(6, 102)
 102     format(/, 'Error in TPDF.', /)
      endif

      stop 1
      end

! --------------------------------------------------------------------------
! --------------------------------------------------------------------------

! WRTCUR - write a curve

      subroutine wrtcur(fileid)
      implicit none

      integer fileid

      integer i, index
      real*8 dm(0:99), rm1(0:99), rm2(0:99)

      integer pfwulc, pfwuly

      index = 0

      do i = 0, 99
         dm(i)  = 6.28*float(i)/99.
         rm1(i) = cos(6.28*float(i)/99.)
         rm2(i) = sin(6.28*float(i)/99.)
      enddo

      if (pfwulc(fileid, 12, "cosine curve", 100, dm, rm1, index) &
           .eq. 0) &
         call errproc

      if (pfwuly(fileid, 9, "sin curve", 100, index - 1, rm2, index) &
           .eq. 0) &
         call errproc

      return
      end subroutine wrtcur

! --------------------------------------------------------------------------
! --------------------------------------------------------------------------

! CHKCUR - check curve

      subroutine chkcur(fileid, tolerance)
      implicit none

      integer fileid
      real*8 tolerance

      integer i
      real*8 dm(0:99), rm1(0:99), rm2(0:99)
      real*8 odm(0:99), orm1(0:99), orm2(0:99)

      integer pfread

      do i = 0, 99
         dm(i)  = 6.28*float(i)/99.
         rm1(i) = cos(6.28*float(i)/99.)
         rm2(i) = sin(6.28*float(i)/99.)
      enddo

      if (pfread(fileid, 5, "xval0", odm) .eq. 0) &
         call errproc

      if (pfread(fileid, 5, "yval0", orm1) .eq. 0) &
         call errproc

      if (pfread(fileid, 5, "yval1", orm2) .eq. 0) &
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
      implicit none

      integer fileid

      integer k, l
      real*8 xmin, xmax, ymin, ymax, data(0:10, 0:10)

      integer pfwima

      xmin = 0.
      xmax = 10.
      ymin = 0.
      ymax = 10.

      do l = 0, 10
         do k = 0, 10
            data(k, l) = float((k - 5)**2 + (l - 5)**2)
         enddo
      enddo

      if (pfwima(fileid, 10, 'Test image', 0, 10, 0, 10,  &
           data, xmin, xmax, ymin, ymax, 0) .eq. 0)       &
         call errproc

      return
      end subroutine wrtima

! --------------------------------------------------------------------------
! --------------------------------------------------------------------------

! CHKIMA - check image

      subroutine chkima(fileid)
      implicit none

      integer fileid

      integer k, l, ret
      real*8 data(0:10, 0:10), odata(0:10, 0:10)

      integer pfrptr

      do l = 0, 10
         do k = 0, 10
            data(k, l) = float((k - 5)**2 + (l - 5)**2)
         enddo
      enddo

      ret = pfrptr(fileid, 14, 'Image0->buffer', 121, odata)
      if (ret .eq. 0) &
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
      implicit none

      integer fileid

      integer i, pim, dp(5), rp(5)
      real*8 dm(0:99), rm(0:99)

      integer pfwmap, pfwset, pfwran

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

      if (pfwmap(fileid, 'Domain0', dp, dm, 'Range0', rp, rm, pim) &
          .eq. 0) &
         call errproc

      pim = 1
      if (pfwset(fileid, 'Domain1', dp, dm) .eq. 0) &
         call errproc
      if (pfwran(fileid, 'Domain1', 7, 'Range1', rp, rm, pim) .eq. 0) &
         call errproc

      return
      end subroutine wrtmap

! --------------------------------------------------------------------------
! --------------------------------------------------------------------------

! CHKMAP - check mappings

      subroutine chkmap(fileid, tolerance)
      implicit none

      integer fileid
      real*8 tolerance

      integer i
      real*8 dm(0:99), rm(0:99)
      real*8 odm(0:99), orm(0:99)

      integer pfread, pfrptr

      do i = 0, 99
         dm(i) = 6.28*float(i)/99.
         rm(i) = sin(6.28*float(i)/99.)
      enddo

      if (pfrptr(fileid, 29, 'Mapping0->domain->elements[1]', &
           100, odm) .eq. 0) &
         call errproc

      if (pfread(fileid, 35, 'Mapping0->range->elements[1][1:100]', &
           orm) .eq. 0) &
         call errproc

      do i = 0, 99
         if ((abs(dm(i) - odm(i)) .gt. tolerance) &
              .or. (abs(rm(i) - orm(i)) .gt. tolerance)) &
            call errproc
      enddo

      if (pfread(fileid, 27, 'Domain1->elements[1][1:100]', odm) &
           .eq. 0) &
         call errproc

      if (pfrptr(fileid, 28, 'Mapping1->range->elements[1]', &
           100, orm) &
           .eq. 0) &
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
      implicit none 

      integer nout
      character*8 outtype
      character*80 date
      logical PRINT
      logical DIR
      logical ATTR
      character*8  OPTION

! ... PDBlib Fortran API
      integer pfopen, pfsoff
      integer pfgoff, pfdefs, pfwrta, pfclos
      integer pfpwd,  pfwrtd
      integer pfdefa, pfdefd, pfcd,   pfln
      integer pfmkdr, pfappa, pfappd
      integer pfgmod, pfflsh, pfdeft
      integer pfdatt, pfsvat, pffami
      integer pfapas, pfapad
      integer pfwras, pfwrad


      integer i, j, k, l
      integer fileid, offset
      integer ndims, dims(14), pairs(6)
      integer nchar
      integer istring
      integer LAST
      
      real*8 a, c, g, z, zz, zzx
      real*8 TOLERANCE
      real*8 x(20)

      character name(256)
      character*8 strings(4)

      common /abc/ a(2), c(2, 2:4), g(5, 2:10), z(4, 5, 2)
      common /abc/ zz(4, 5, 2), zzx(4, 5, 2)
      common /jkl/ j, k, l

      equivalence (istring, strings)

      data  LAST /0/
      data  TOLERANCE /3.e-14/

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
      fileid = pfopen(8, 'file.pdb', 'w')
      if (fileid .le. 0) &
         call errproc

! ... faux fam
      if (pffami(fileid, 1) .eq. 0) &
         call errproc

! ... check file mode
      if (pfgmod(fileid) .ne. 1) &
         call errproc

! ... set and get and verify default offset
      offset = 1
      if (pfsoff(fileid, offset) .eq. 0) &
         call errproc
      offset = pfgoff(fileid)

      if (PRINT) then
         write(6, 300) offset
 300     format(/, 'Current default offset:', i4)
      endif

      if (offset .ne. 1) &
         call errproc

! ... write at least one variable before creating a directory
      if (pfwrta(fileid, 5, 'x(20)', 6, 'double', x) .eq. 0) &
         call errproc

      if (pfappa(fileid, 7, 'x(1:10)', x) .eq. 0) &
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
         if (pfmkdr(fileid, 4, "dir1") .eq. 0) &
            call errproc
      endif

! ... get current working directory
      if (pfpwd(fileid, nchar, name) .eq. 0) &
         call errproc

      if (PRINT) then
         write(6, 350) (name(i), i=1, nchar)
 350     format(/, 'Current working directory:  ', 256a1)
      endif

      if ((name(1) .ne. '/') .or. (nchar .ne. 1)) &
         call errproc

! ... write curve, image, and mappings in root after creating directory
      if (OPTION .eq. "2") then
         call wrtcur(fileid)
         call wrtima(fileid)
         call wrtmap(fileid)
      endif

      x(1) = 100

! ... write  data item z   write_as,  append,  append_alt
      if (pfwras(fileid, 8, 'z(4,5,1)', 6, 'double', 6, 'double', z) &
            .eq. 0)  call errproc

      if (pfappa(fileid, 14, 'z(1:4,1:5,1:1)', z) .eq. 0) &
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
      if (pfappd(fileid, 1, 'z', z, ndims, dims) .eq. 0) &
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
      if (pfwrad(fileid, 2, 'zz', 6, 'double', nout, outtype, zz, &
            ndims, dims) .eq. 0)  call errproc

      if (pfapas(fileid, 15, 'zz(1:4,1:5,1:1)', 6, 'double', zz(1, 1, 2)) &
            .eq. 0)  call errproc

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
      if (pfapad(fileid, 2, 'zz', 6, 'double', zzx, ndims, dims) .eq. 0) &
         call errproc


      if (PRINT) then 
         write(6, 305) 
 305     format(/, 'Variables z and zz written', /)
      endif

! ... change directory
      if (DIR) then
         if (pfcd(fileid, 4, "dir1") .eq. 0) &
            call errproc
      endif

! ... get current working directory
      if (pfpwd(fileid, nchar, name) .eq. 0) &
         call errproc

      if (PRINT) then
         write(6, 351) (name(i), i=1, nchar)
 351     format(/, 'Current working directory:  ', 256a1)
      endif

      if (((.not. DIR) .and. (nchar .ne. 1)) .or.&
          (DIR .and. (nchar .ne. 5))) &
         call errproc

! ... create a link
      if (DIR) then
         if (pfln(fileid, 2, '/x', 6, 'x_link') .eq. 0) &
            call errproc
      else
         if (pfln(fileid, 1, 'x', 6, 'x_link') .eq. 0) &
            call errproc
      endif

! ... define and write some data structures
      if (pfdefs(fileid, 3, 'abc', 11, 'double a(2)', 8, 'double b', &
           15, 'double c(2, 2:4)', LAST) .eq. 0) &
         call errproc

      if (pfwrta(fileid, 3, 'abc', 3, 'abc', a) .eq. 0) &
         call errproc

      pairs(1) = 0
      pairs(2) = 5
      pairs(3) = 5
      pairs(4) = 5
      pairs(5) = 10
      pairs(6) = 5
      if (pfdeft(fileid, 3, 'jkl', 3, pairs, 'int jint kint l') .eq. 0) &
         call errproc

      if (pfwrta(fileid, 3, 'jkl', 3, 'jkl', j) .eq. 0) &
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
      if (pfwrtd(fileid, 7, 'strings', 4, 'char', istring, &
           ndims, dims) .eq. 0) &
         call errproc

! ... define an attribute
      if (ATTR) then
         if (pfdatt(fileid, 4, 'date', 6, 'char *') .eq. 0) &
            call errproc
      endif

! ... define some symbol table entries and reserve disk space
      if (pfdefa(fileid, 14, 'f(0:3, 4:6, 1:2)', 6, 'double') .eq. 0) &
         call errproc

      ndims = 2
      dims(1) = 1
      dims(2) = 5
      dims(3) = 2
      dims(4) = 10
      if (pfdefd(fileid, 1, 'g', 6, 'double', ndims, dims) .eq. 0) &
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
      if (pfwrtd(fileid, 1, 'g', 6, 'double', g, ndims, dims) .eq. 0) &
         call errproc
!      if (pfwrtd(fileid, 6, 'g[1, 2]', 6, 'double', g, ndims, dims) &
!           .eq. 0) &
!         call errproc

      ndims = 1
      dims(1) = 1
      dims(2) = 2
      dims(3) = 1
      if (pfwrtd(fileid, 1, 'a', 6, 'double', a, ndims, dims) .eq. 0) &
         call errproc

      ndims = 2
      dims(1) = 1
      dims(2) = 2
      dims(3) = 1
      dims(4) = 2
      dims(5) = 4
      dims(6) = 1
      if (pfwrtd(fileid, 1, 'c', 6, 'double', c, ndims, dims) .eq. 0) &
         call errproc

! ... create a link
      if (pfln(fileid, 3, 'jkl', 9, '/jkl_link') .eq. 0) &
         call errproc

! ... set attribute value for member of struct
      if (ATTR) then
         if (pfsvat(fileid, 11, '/jkl_link.k', 4, 'date', date) .eq. 0) &
            call errproc
      endif

! ... write curve, image, and mappings in a directory
      if (OPTION .eq. "3") then
         call wrtcur(fileid)
         call wrtima(fileid)
         call wrtmap(fileid)
      endif

! ... flush the file
      if (pfflsh(fileid) .eq. 0) &
         call errproc

! ... close the file
      if (pfclos(fileid) .eq. 0) &
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
      implicit none 

      character*80 date
      logical PRINT
      logical DIR
      logical ATTR

! ... PDBlib Fortran API
      integer pfopen, pfclos
      integer pfityp, pfivar, pfptrd
      integer pfpwd, pfcd
      integer pfgmod, pfgfnm
      integer pfgvat, pfratt
      integer pfimbr
      integer pfrdas, pfrdad

      integer i, n, nt, fileid
      integer ntype, ndims, dims(14), nitems
      integer typsiz, align, nptr
      integer j, k, l
      integer kindx
      integer nchar
      integer LAST
       
      character name(256)
      character*8 name1, name2
      character*5 name3, name4
      character*80 dt
      character type(72)

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

      data  LAST /0/
      data  TOLERANCE /3.e-14/

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
      fileid = pfopen(8, 'file.pdb', 'a')
      if (fileid .eq. 0) &
         call errproc

! ... get file name given id
      nchar = 8
      if (pfgfnm(fileid, nchar, name1) .eq. 0) &
         call errproc

      name2 = 'file.pdb'
      if (name1 .ne. name2) &
         call errproc

! ... check file mode
      if (pfgmod(fileid) .ne. 7) &
         call errproc

! ... retrieve and verify the value of an attribute
      if (ATTR) then
         if (pfcd(fileid, 4, "dir1") .eq. 0) &
            call errproc

         if (pfgvat(fileid, 11, '/jkl_link.k', 4, 'date', dt) .eq. 0) &
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
         if (pfratt(fileid, 4, 'date') .eq. 0) &
            call errproc

! ... verify that attribute was removed
         if (pfgvat(fileid, 11, '/jkl_link.k', 4, 'date', dt) &
              .ne. 0) then
            write(6, 380)
 380        format(/, 'Attribute was not removed.')
            stop 1
         endif
      
         if (pfcd(fileid, 1, "/") .eq. 0) &
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
         if (pfptrd(fileid, 1, 'z', z2, dims) .eq. 0) &
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

      if (pfrdad(fileid, 2, 'zz', 6, 'double', zz2, dims) .eq. 0) &
         call errproc
!      if (pfptrd(fileid, 2, 'zz', zz2, dims) .eq. 0) &
!         call errproc
      do kindx = 1, 2
         do j = 1, 5
            do i = 1, 4
               if (abs(zz2(i, j, kindx) - zz(i, j, kindx)) &
                   .gt. tolerance) &
                  call errproc
            enddo
         enddo
      enddo

      if (pfrdas(fileid, 15, 'zz(1:4,1:5,3:4)', 6, 'double', zz2) &
             .eq. 0) call errproc
!      if (pfread(fileid, 15, 'zz(1:4,1:5,3:4)', zz2) &
!             .eq. 0) call errproc

         do kindx = 1, 2
            do j = 1, 5
               do i = 1, 4
                 if (abs(zz2(i, j, kindx) - zzx(i, j, kindx)) &
                     .gt. tolerance) &
                    call errproc
            enddo
         enddo
      enddo

      if (PRINT) then
         write(6, 430)
 430     format(/, 'Values of zz read match values written.')
      endif


! ... get current working directory
      if (pfpwd(fileid, nchar, name) .eq. 0) &
         call errproc

      if (PRINT) then
         write(6, 352) (name(i), i=1, nchar)
 352     format(/, 'Current working directory:  ', 256a1)
      endif

      if ((name(1) .ne. '/') .or. (nchar .ne. 1)) &
         call errproc

! ... change directory
      if (DIR) then
         if (pfcd(fileid, 4, "dir1") .eq. 0) &
            call errproc
      endif

! ... get current working directory
      if (pfpwd(fileid, nchar, name) .eq. 0) &
         call errproc

      if (PRINT) then
         write(6, 352) (name(i), i=1, nchar)
      endif

      if (((.not. DIR) .and. (nchar .ne. 1)) .or.&
          (DIR .and. (nchar .ne. 5))) &
         call errproc

! ... inquire about a struct types
      if (pfityp(fileid, 3, 'abc', typsiz, align, nptr) .eq. 0) &
         call errproc

      if (PRINT) then
         write(6, 450) typsiz, align, nptr
 450     format(/, 'Size in bytes, alignment, and pointer members ', &
              'of type abc:', 3i5)
      endif

      if ((typsiz .ne. 72) .or. (nptr .ne. 0)) &
         call errproc

      nchar = 5
      if (pfimbr(fileid, 3, 'jkl', 3, nchar, name3) .eq. 0) &
         call errproc

      name4 = 'int l'
      if ((nchar .ne. 5) .or. (name3 .ne. name4)) &
         call errproc

! ... inquire about unwritten variable
      if (pfivar(fileid, 1, 'f', ntype, type, nitems, ndims, dims) &
           .eq. 0) &
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
      if (pfivar(fileid, 1, 'c', ntype, type, nitems, ndims, dims) &
           .eq. 0) &
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
      if (pfclos(fileid) .eq. 0) &
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
      implicit none 

      logical PRINT
      logical DIR
      character*8  OPTION

! ... PDBlib Fortran API
      integer pfopen, pfwrta, pfclos
      integer pfivar, pfread, pfptrd
      integer pfvart, pfgvar, pfcd
      integer pflst,  pfgls,  pfdls
      integer pfdvar, pfctyp

      integer i, n, fileid, nfileid
      integer ntype, ndims, dims(14), nitems
      integer j, k, l
      integer j2, k2, l2
      integer pord, nvar, nchar
      integer LAST

      real*8 x(20)
      real*8 a, c, g, z, zz, zzx
      real*8 a2, c2, g2, z2, zz2
      real*8 abc_a, c_1_2
      real*8 TOLERANCE

      character type(72)
      character name(256)

      common /abc/ a(2), c(2, 2:4), g(5, 2:10), z(4, 5, 2)
      common /abc/ zz(4, 5, 2), zzx(4, 5, 2)
      common /abc2/ a2(2), c2(2, 2:4), g2(5, 2:10), z2(4, 5)
      common /abc2/ zz2(4, 5, 2)
      common /jkl/ j, k, l
      common /jkl2/ j2, k2, l2

      data  LAST /0/
      data  TOLERANCE /3.e-14/

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
      fileid = pfopen(8, 'file.pdb', 'a')
      if (fileid .eq. 0) &
         call errproc

! ... change directory
      if (DIR) then
         if (pfcd(fileid, 4, "dir1") .eq. 0) &
            call errproc
      endif

! ... copy type definition to new file and write variable
      nfileid = pfopen(9, 'file2.pdb', 'w')
      if (nfileid .le. 0) &
         call errproc

      if (pfctyp(fileid, nfileid, 3, 'jkl') .eq. 0) &
         call errproc

      j = 1
      k = 2
      l = 3
      if (pfwrta(nfileid, 3, 'jkl', 3, 'jkl', j) .eq. 0) &
         call errproc

      if (pfclos(nfileid) .eq. 0) &
         call errproc

! ... verify correctness of variable
      if (pfread(fileid, 1, 'a', a2) .eq. 0) &
         call errproc

      if ((a(1) .ne. 1.) .or. (a(2) .ne. 2.)) &
         call errproc

! ... verify correctness of partially written variable
      if (pfread(fileid, 1, 'g', g2) .eq. 0) &
         call errproc

      if ((g2(1, 2) .ne. 100) .or. (g2(2, 2) .ne. 0)) &
         call errproc

! ... inquire about a structure member
      if (pfivar(fileid, 3, 'abc.c', ntype, type, nitems, ndims, dims) &
           .eq. 0) &
         call errproc

      if (PRINT) then
         write(6, 504) nitems
 504     format(/, 'Number of items in abc.c:', i5)
         write(6, 505) ndims
 505     format('Number of dimensions for abc.c:', i5)
      endif

! ... read struct by link and verify
      if (pfread(fileid, 9, '/jkl_link', j2) .eq. 0) &
         call errproc

      if ((j2 .ne. 1) .or. (k2 .ne. 2) .or. (l2 .ne. 3)) &
         call errproc

! ... read struct member
      if (pfread(fileid, 5, 'jkl.k', k2) .eq. 0) &
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
      if (pfptrd(fileid, 5, 'abc.a', abc_a, dims) .eq. 0) &
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
      if (pfread(fileid, 8, 'abc.a(2)', abc_a) .eq. 0) &
         call errproc

      if (abc_a .ne. 2.0) &
         call errproc

      if (pfread(fileid, 10, 'abc.c(1,2)', c_1_2) .eq. 0) &
         call errproc

      if (c_1_2 .ne. 4.0) &
         call errproc

! ... get the variable names in alphabetic order and check last name
      pord = 1
      nvar = 0
      if (pfvart(fileid, pord, nvar) .eq. 0) &
         call errproc

      if (PRINT) then
         write(6, 700)
 700     format(/, 'Alphabetic list of variables:')
      endif

      do n = 1, nvar
         if (pfgvar(n, nchar, name) .eq. 0) &
            call errproc
         if (PRINT) then
            write(6, 702) (name(i), i=1, nchar)
 702        format('    ', 256a1)
         endif
      enddo

!      if ((DIR .and. (name(1) .ne. 'x')) .or.&
!          ((.not. DIR) .and. (name(1) .ne. 'z'))) &
!         call errproc

      if (pfdvar() .eq. 0) &
         call errproc

! ... get the variable and directory names in root
      if (pflst(fileid, 1, '/', 0, ' ', nvar) .eq. 0) &
         call errproc

      if (PRINT) then
         write(6, 800)
 800     format(/, 'Variables and directories in root:')
      endif

      do n = 1, nvar
         if (pfgls(n, nchar, name) .eq. 0) &
            call errproc
         if (PRINT) then
            write(6, 702) (name(i), i=1, nchar)
         endif
      enddo

      if (name(1) .ne. 'z') &
         call errproc

! ... get the variable and directory names in dir1
      nvar = 0

      if (DIR) then
         if (pflst(fileid, 5, '/dir1', 0, ' ', nvar) .eq. 0) &
            call errproc
      endif

      if (PRINT) then
         write(6, 850)
 850     format(/, 'Variables and directories in dir1:')
      endif

      do n = 1, nvar
         if (pfgls(n, nchar, name) .eq. 0) &
            call errproc
         if (PRINT) then
            write(6, 702) (name(i), i=1, nchar)
         endif
      enddo

      if (pfdls() .eq. 0) &
         call errproc

! ... check curve, image, mapping
      if (OPTION .ne. "0") then
         call chkcur(fileid, TOLERANCE)
         call chkima(fileid)
         call chkmap(fileid, TOLERANCE)
      endif

! ... close the file
      if (pfclos(fileid) .eq. 0) &
         call errproc

      if (PRINT) then
         write(6, *)
      endif

      return
      end subroutine test_3

! --------------------------------------------------------------------------
! --------------------------------------------------------------------------

! TPDF - main test routine

      program tpdf
      use parameters_pdb
      implicit none 

!     Program input command option flags:
      logical PRINT
      logical DIR
      logical TARGET
      logical ATTR

!     OPTION determines when curve, image and mappings are written:
!        0 - none
!        1 - before directory created
!        2 - in root directory
!        3 - in directory dir1
      character*8  OPTION
      integer LAST
      real*8 TOLERANCE

! ... PDBlib Fortran API
      integer pfsbfs, pfgbfs
      integer pftrgt

      integer scvers
      integer pfinth
      integer sczrsp
      external pfinth
      external sczrsp
       
      integer is, ia
      integer bufsiz1, bufsiz2, bufsiz3
      integer narg, iarg
      integer nout
      integer err

      character*8 outtype
      character*8 arg
      character*80 date
      character*12 vers

      data  LAST /0/
      data  TOLERANCE /3.e-14/

      err = pfinth(0, 0)
      err = sczrsp(0)

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
         if (arg .eq. "-p") then
            PRINT = .true.
         elseif (arg .eq. "-a") then
            ATTR = .true.
         elseif (arg .eq. "-o") then
            iarg = iarg + 1
!            call getarg(iarg, arg)
            call get_command_argument(iarg, arg)
            OPTION = arg
         elseif (arg .eq. "-d") then
            DIR = .true.
         elseif (arg .eq. "-t") then
            TARGET = .true.
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
         elseif (arg .eq. "-h" .or. arg .eq. "help") then
            write(6, *) 'Usage: tpdf [-a f|d] [-as] [-d] [-h] &
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

      call scftcs(date, 'Wednesday August 31, 1994', 25)

      if (PRINT) then
         write(6, 1) date
 1       format(/, 'A date: ', 1a25)
      endif

! ... set and get buffer size
      bufsiz1 = 4096
      bufsiz2 = pfsbfs(bufsiz1)
      bufsiz3 = pfgbfs()
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
         is = PD_CRAY_STD
         ia = PD_WORD8_ALGN

         if (pftrgt(is, ia) .eq. 0) &
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

