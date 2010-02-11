c
c PDFDPT.F - test the Fortran application interface to
c          - the PACT pdb library DP routines
c
c Source Version: 3.0
c Software Release #: LLNL-CODE-422942
c include "cpyright.h"
c
c This program is designed to be run on 4 processors.

      program pdfdpt

      implicit none

      include 'mpif.h'

      integer TASKS, N_ELEM
      parameter(TASKS  = 4)
      parameter(N_ELEM = 256)

      integer pfmpop, pfwrad, pfappd, pfdefd, pfwrtd
      integer pfclos, pfread
      integer dims(6), nwrite
      integer i, j, numtasks, rank, ierr
      integer fileid
      real*8 darray(N_ELEM), rdarray(N_ELEM)

      call MPI_INIT(ierr)

      call MPI_COMM_SIZE(MPI_COMM_WORLD, numtasks, ierr)
      if(numtasks .ne. TASKS) then
         write(6,10) TASKS
 10      format('This program requires ', i4, ' processors')
         stop
      endif

      call MPI_COMM_RANK(MPI_COMM_WORLD, rank, ierr)

      call pfinmp(0,1,0)

      do 100 i = 1, N_ELEM
         darray(i) = rank
 100  continue

      fileid = pfmpop(15,"test_par_2d.pdb","w",MPI_COMM_WORLD)
      
      dims(1) = 1
      dims(2) = N_ELEM

c     ..........reserve some disk space for a distributed array
      if(pfdefd(fileid, 6, 'darray', 6, 'double', 1, dims) .eq. 0)
     $    call errproc

c     ..........each process writes its share of the distributed array      
      nwrite  = N_ELEM / numtasks
      dims(1) = (rank * nwrite) + 1
      dims(2) = (rank + 1) *  nwrite
      dims(3) = 1 
      if(pfwrtd(fileid, 6, 'darray', 6, 'double', darray, 1, dims) 
     $    .eq. 0)
     $    call errproc
      
      if (pfclos(fileid) .eq. 0)
     $     call errproc


      call MPI_BARRIER(MPI_COMM_WORLD, ierr)

      fileid = pfmpop(15,"test_par_2d.pdb","r",MPI_COMM_WORLD)
      
      if (pfread(fileid, 6, 'darray', rdarray) .eq. 0)
     $     call errproc

      if (pfclos(fileid) .eq. 0)
     $     call errproc

      do 200 i = 1, numtasks
         do 200 j = 1, nwrite
            if (rdarray((i-1) * nwrite + j) .ne. (i - 1)) then
                write(6, 20) i, j
 20             format('input data error--quitting, i = ', i4,
     $                 'j = ', i4)
                stop
            endif
 200  continue

      write(6, 30)
 30   format('Read test passed')

      call pftmmp()

      call MPI_FINALIZE(ierr)

      
      end
      
      
!     ERRPROC - handle errors
      
      subroutine errproc
      
      parameter(MAXMSG = 256)
      integer i, nchar, pfgerr
      character err(MAXMSG)

      if ((pfgerr(nchar, err) .eq. 1) .and. (nchar .gt. 0)) then
         nchar = min(nchar, MAXMSG)
         write(6,100)
 100     format()
         write(6,101) (err(i), i=1,nchar)
 101     format(72a1)
      else
         write(6,102)
 102     format(/,'Error in PDFDPT.',/)
      endif

      stop
      end subroutine

