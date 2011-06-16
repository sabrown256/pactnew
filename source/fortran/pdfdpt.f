!
! PDFDPT.F - test the Fortran application interface to
!          - the PACT pdb library DP routines
!
! Source Version: 3.0
! Software Release #: LLNL-CODE-422942
! include "cpyright.h"
!

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

! ERRPROC - handle errors
      
      subroutine errproc
!      use pact_fortran
      implicit none
      
      integer pfgerr

      integer, parameter :: MAXMSG = 256
      integer :: i, nchar
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
 102     format(/, 'Error in PDFDPT.', /)
      endif

      stop
      end subroutine

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

! WRTF - write a parallel file

      subroutine wrtf(rank, numtasks, comm)
      use pact_fortran
      implicit none

      integer :: rank, numtasks, comm

      include 'mpif.h'

! ... local variables
      integer, parameter :: N_ELEM = 256

      integer :: i
      integer :: fileid, nwrite
      integer :: dims(6)
      real*8 :: darray(N_ELEM)

      do i = 1, N_ELEM
         darray(i) = rank
      enddo

      fileid = pfmpop(15, "test_par_2d.pdb", "w", comm)
      
      dims(1) = 1
      dims(2) = N_ELEM

! ... reserve some disk space for a distributed array
      if (pfdefd(fileid, 6, 'darray', 6, 'double', 1, dims) .eq. 0) &
         call errproc

! ... each process writes its share of the distributed array      
      nwrite  = N_ELEM / numtasks
      dims(1) = (rank * nwrite) + 1
      dims(2) = (rank + 1) *  nwrite
      dims(3) = 1 
      if (pfwrtd(fileid, 6, 'darray', 6, 'double', darray, 1, dims) .eq. 0) &
         call errproc
      
      if (pfclos(fileid) .eq. 0) &
         call errproc

      write(6, 31) rank
 31   format('   data written on  ', i4)

      return
      end subroutine

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

! RDF - read the parallel file back

      subroutine rdf(rank, numtasks, comm)
      use pact_fortran
      implicit none

      integer :: rank, numtasks, comm

      include 'mpif.h'

! ... local variables
      integer, parameter :: N_ELEM = 256

      integer :: i, j
      integer :: fileid, nwrite
      real*8 :: rdarray(N_ELEM)

      fileid = pfmpop(15, "test_par_2d.pdb", "r", comm)
      
      if (pfread(fileid, 6, 'darray', rdarray) .eq. 0) &
         call errproc

      if (pfclos(fileid) .eq. 0) &
         call errproc

      nwrite  = N_ELEM / numtasks
      do i = 1, numtasks
         do j = 1, nwrite
            if (rdarray((i-1) * nwrite + j) .ne. (i - 1)) then
                write(6, 20) i, j
 20             format('input data error--quitting, i = ', i4, 'j = ', i4)
                stop
            endif
         enddo
      enddo

      write(6, 30) rank
 30   format('   data read back on', i4)

      return
      end

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

! PDFDPT - this program is designed to be run on 4 processors

      program pdfdpt
      use pact_fortran
      implicit none

      include 'mpif.h'

      integer, parameter :: TASKS  = 4

      integer numtasks, rank, ierr, st

      call MPI_INIT(ierr)

      call MPI_COMM_SIZE(MPI_COMM_WORLD, numtasks, ierr)
      if (numtasks .ne. TASKS) then
         write(6, 10) TASKS
 10      format('This program requires ', i4, ' processors')
         stop
      endif

      call MPI_COMM_RANK(MPI_COMM_WORLD, rank, ierr)

      st = pfinmp(0, 1, 0)

      call wrtf(rank, numtasks, MPI_COMM_WORLD)

      call MPI_BARRIER(MPI_COMM_WORLD, ierr)

      call rdf(rank, numtasks, MPI_COMM_WORLD)

      st = pftmmp()

      call MPI_FINALIZE(ierr)
      
      end
      
!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

