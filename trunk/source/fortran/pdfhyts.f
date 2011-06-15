!
! PDFHYTS.F - test optimized hyper stride features of PDB Fortran API
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
!      use pact_pdb
      implicit none

      integer, parameter :: MAXMSG = 256
      integer i, nchar
      character err(MAXMSG)

      integer pfgerr

      if ((pfgerr(nchar, err) .eq. 1) .and. (nchar .gt. 0)) then
         nchar = min(nchar, MAXMSG)
         write(6,100)
 100     format()
         write(6,101) (err(i), i=1,nchar)
 101     format(72a1)
      else
         write(6,102)
 102     format(/,'Error in PDFHYTS.',/)
      endif

      stop 1
      end

! --------------------------------------------------------------------------
! --------------------------------------------------------------------------

! PDFHYTS - main test routine

      program pdfhyts
      use pact_score
      use pact_pdb
      implicit none

      integer noneW(24)     ! these are all half the size of the 
      integer middleW(24)   ! space that will be defent for them
      integer allW(24)      ! which will be of dim [2][6][4] 
      integer allWStep(48)
      integer allRStep(48)
      integer zeroStep(48) 
      integer noneR(24)     ! over the course of three cases we 
      integer middleR(24)   ! write and read back non-contiguously, 
      integer allR(24)      ! partially contiguously, and contiguously 
      integer zero(24)
      integer i, fd, stat

      fd = pfopen(11, 'pdfhyts.dat', 'w+') 

      if (fd .le. 0) then
        call errproc
      endif

! ... initialize the write vars to some semi-arbitrary values 
      do i = 1, 24
        noneW(i)   = i
        middleW(i) = i
        allW(i)    = i
        zero(i)    = 0
        noneR(i)   = 0
        middleR(i) = 0
        allR(i)    = 0
      enddo

! ... STEP == 1 CASES

! ... First Case: NONE of the dimension descriptor describe contiguity 
      stat = pfdefa(fd, 11, 'none[4,6,2]', 7, 'integer')

      if (stat .le. 0) then
         call errproc
      endif

! ... write out one "physics" variable such that there
! ... are no contiguous regions to optimize on
      stat = pfwrta(fd, 19, 'none[1:2, 1:6, 1:2]', 7, 'integer', noneW)

      if (stat .le. 0) then
        call errproc
      endif

! ... zero out the rest of it   
      stat = pfwrta(fd, 19, 'none[3:4, 1:6, 1:2]', 7, 'integer', zero)

      if (stat .le. 0) then
        call errproc
      endif

! ... read that variable back again such that there
! ... are no contiguous regions to optimize on
      stat = pfread(fd, 19, 'none[1:2, 1:6, 1:2]', noneR)

      if (stat .le. 0) then
        call errproc
      endif

! ... check it to make sure it is correct 
      do i = 1, 24 
        if (noneW(i) .ne. noneR(i)) then
           write(6, *) 'None case: check error at ', i
           write(6, *) noneW(i), '!=', noneR(i)
           call errproc
        endif
      enddo

! ... Second Case: MIDDLE dimension contains contiguity */
      stat = pfdefa(fd, 13, 'middle[4,6,2]', 7, 'integer')

      if (stat .le. 0) then
        call errproc
      endif

! ... write out on "physics" variable such that some 
! ... partial contiguous regions exist to optimize upon
      stat = pfwrta(fd,21,'middle[1:4, 4:6, 1:2]', 7,'integer', middleW)

      if (stat .le. 0) then
        call errproc
      endif

      stat = pfwrta(fd, 21, 'middle[1:4, 1:3, 1:2]', 7, 'integer', zero)

      if (stat .le. 0) then
        call errproc
      endif

! ... read that variable back again 
      stat = pfread(fd, 21, 'middle[1:4, 4:6, 1:2]', middleR)

      if (stat .le. 0) then
        call errproc
      endif

! ... check it to make sure it is correct 
      do i = 1, 24  
        if (middleW(i) .ne. middleR(i)) then
           write(6, *) 'Middle case: check error at ', i
           write(6, *) middleW(i), '!=', middleR(i)
           call errproc
        endif
      enddo


! ... Third Case: ALL of the region to write/read is contiguous 
      stat = pfdefa(fd, 10, 'all[4,6,2]', 7, 'integer')

      if (stat .le. 0) then
        call errproc
      endif

! ... write out on "physics" variable such that the  
! ... entire region is contiguous to check full optimization 
      stat = pfwrta(fd, 18, 'all[1:4, 1:6, 1:1]', 7, 'integer', allW)

      if (stat .le. 0) then
        call errproc
      endif

      stat = pfwrta(fd, 18, 'all[1:4, 1:6, 2:2]', 7, 'integer', zero)

      if (stat .le. 0) then
        call errproc
      endif

! ... read that variable back again 
      stat = pfread(fd, 18, 'all[1:4, 1:6, 1:1]', allR)

      if (stat .le. 0) then
        call errproc
      endif

! ... check it to make sure it is correct */
      do i = 1, 24
        if (allW(i) .ne. allR(i)) then
           write(6, *) 'All case: check error at ', i
           write(6, *) allW(i), '!=', allR(i)
           call errproc
        endif
      enddo


! ... STEP != 1 CASES

! ... First Case: NONE of the dimension descriptor describe contiguity
      stat = pfdefa(fd, 15, 'noneStep[4,6,2]', 7, 'integer')

      if (stat .le. 0) then
         call errproc
      endif

! ... write out one "physics" variable such that there
! ... are no contiguous regions to optimize on
      stat = pfwrta(fd,25,'noneStep[1:3:2, 1:6, 1:2]',7,'integer',noneW)

      if (stat .le. 0) then
        call errproc
      endif

! ... zero out the rest of it
      stat = pfwrta(fd,25,'noneStep[2:4:2, 1:6, 1:2]',7,'integer', zero)

      if (stat .le. 0) then
        call errproc
      endif

! ... read that variable back again such that there
! ... are no contiguous regions to optimize on
      stat = pfread(fd, 25, 'noneStep[1:3:2, 1:6, 1:2]', noneR)

      if (stat .le. 0) then
        call errproc
      endif

! ... check it to make sure it is correct
      do i = 1, 24
        if (noneW(i) .ne. noneR(i)) then
           write(6, *) 'None step case: check error at ', i
           write(6, *) noneW(i), '!=', noneR(i)
           call errproc
        endif
      enddo


! ... Second Case: MIDDLE dimension contains step, still no optimization 
      stat = pfdefa(fd, 17, 'middleStep[4,6,2]', 7, 'integer')

      if (stat .le. 0) then
        call errproc
      endif

! ... write out on "physics" variable such that some
! ... partial contiguous regions exist to optimize upon
      stat = pfwrta(fd, 27, 'middleStep[1:4, 1:5:2, 1:2]', &
                    7, 'integer', middleW)

      if (stat .le. 0) then
        call errproc
      endif

      stat=pfwrta(fd,27,'middleStep[1:4, 2:6:2, 1:2]',7,'integer', zero)

      if (stat .le. 0) then
        call errproc
      endif

! ... read that variable back again
      stat = pfread(fd, 27, 'middleStep[1:4, 1:5:2, 1:2]', middleR)

      if (stat .le. 0) then
        call errproc
      endif

! ... check it to make sure it is correct
      do i = 1, 24
        if (middleW(i) .ne. middleR(i)) then
           write(6, *) 'Middle step case: check error at ', i
           write(6, *) middleW(i), '!=', middleR(i)
           call errproc
        endif
      enddo


! ... Third Case: OUTTER dim steps, 2nd dim can be optimized
! ...             This is the only step != 1 case that causes optimization
      do i = 1, 48
        allWStep(i) = i
        allRStep(i) = 0
        zeroStep(i) = 0
      enddo
     
      stat = pfdefa(fd, 14, 'allStep[4,6,4]', 7, 'integer')

      if (stat .le. 0) then
        call errproc
      endif

! ... write out on "physics" variable such that the
! ... entire region is contiguous to check full optimization
      stat = pfwrta(fd, 24, 'allStep[1:4, 1:6, 1:3:2]', &
                    7, 'integer',allWStep)

      if (stat .le. 0) then
        call errproc
      endif

      stat = pfwrta(fd, 24, 'allStep[1:4, 1:6, 0:2:2]', &
                    7, 'integer',zeroStep)

      if (stat .le. 0) then
        call errproc
      endif

! ... read that variable back again
      stat = pfread(fd, 24, 'allStep[1:4, 1:6, 1:3:2]', allRStep)

      if (stat .le. 0) then
        call errproc
      endif

! ... check it to make sure it is correct */
      do i = 1, 48
        if (allWStep(i) .ne. allRStep(i)) then
           write(6, *) 'All step case: check error at ', i
           write(6, *) allWStep(i), '!=', allRStep(i)
           call errproc
        endif
      enddo

      stat = pfclos(fd)

      end

! --------------------------------------------------------------------------
! --------------------------------------------------------------------------

