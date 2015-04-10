!
! TSCFOMP.F - OpenMP test
!
!
! Source Version: 3.0
! Software Release #: LLNL-CODE-422942
!
! include "cpyright.h"
!
!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

! SID - return the OMP thread id number
!     - this is a wrapper so that the threaded memory manager can know
!     - which thread we are working on

      subroutine sid(id)
      implicit none
      integer id

      integer np

      integer omp_get_num_threads, omp_get_thread_num
      external omp_get_num_threads, omp_get_thread_num

      np = omp_get_num_threads()
      id = omp_get_thread_num()

      return
      end

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

! MEMA - memory allocation subroutin

      subroutine mema
      use pact_fortran
      implicit none

      integer j, ok(10)
      integer nprc, iprc, nok
      integer ni, nb, nit
      integer*8 ia(10)

      integer omp_get_num_threads, omp_get_thread_num
      external omp_get_num_threads, omp_get_thread_num

      nprc = omp_get_num_threads()
      iprc = omp_get_thread_num()

      ni = 1000
      nb = 8

! ... do the allocs
      do j = 1, 10
         nit   = ni*j
         ok(j) = scmake(ia(j), nit, nb)
      enddo

! ... count the number of errorless allocs
      nok = 0
      do j = 1, 10
         nok = nok + ok(j)
      enddo

! ... report the allocs
      write(6, 101) iprc, nprc, nok
 101  format('   alloc .... (', i2, '/', i2, ') = ', i8)

! ... count the bytes allocated
      nit = 0
      do j = 1, 10
         nit = nit + scalen(ia(j))
      enddo

! ... report the number of bytes
      write(6, 102) iprc, nprc, nit
 102  format('   bytes .... (', i2, '/', i2, ') = ', i8)

! ... do the frees
      do j = 1, 10
         ok(j) = scfree(ia(j))
      enddo

! ... count the number of errorless frees
      nok = 0
      do j = 1, 10
         nok = nok + ok(j)
      enddo

! ... report the frees
      write(6, 103) iprc, nprc, nok
 103  format('   free ..... (', i2, '/', i2, ') = ', i8)

      return
      end

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

      program main
      use pact_fortran
      implicit none

      integer jotsk, nprc, rprc
      integer err

      integer omp_get_num_threads, omp_get_thread_num
      external omp_get_num_threads, omp_get_thread_num

      external sid

      print *, 'Enter the number of processes to use: '
      read *, rprc

      jotsk = rprc
      call omp_set_num_threads(jotsk)

      nprc = omp_get_num_threads()
      print *, 'number of threads available: ', nprc

      err = scinth(rprc, sid)
      if (err .eq. 1) then
         print *, 'SCORE initialization succeeded'
      else
         print *, 'SCORE initialization failed'
      endif

!$omp parallel &
!$omp&   default(private)
      call mema
!$omp end parallel

      print *, 'threads done'

      stop 0
      end

!--------------------------------------------------------------------------
!--------------------------------------------------------------------------

