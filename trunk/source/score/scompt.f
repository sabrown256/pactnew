c
c SCOMPT.F - IRIX test of "threads" using sproc and OpemMP
c
c
cccccccccccccccccccc
c Source Version: 3.0
c Software Release #: LLNL-CODE-422942
c
c include "cpyright.h"
c
c--------------------------------------------------------------------------
c--------------------------------------------------------------------------

c SID - return the OMP thread id number
c     - this is a wrapper so that the threaded memory manager can know
c     - which thread we are working on

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

c--------------------------------------------------------------------------
c--------------------------------------------------------------------------

c MEMA - memory allocation subroutin

      subroutine mema
      implicit none

      integer j, ok(10)
      integer nprc, iprc, nok
      integer ni, nb, nit
      integer*8 ia(10)

      integer omp_get_num_threads, omp_get_thread_num
      integer scmake, scfree, scalen
      external omp_get_num_threads, omp_get_thread_num
      external scmake, scfree, scalen

      nprc = omp_get_num_threads()
      iprc = omp_get_thread_num()

      ni = 1000
      nb = 8

c ... do the allocs
      do j = 1, 10
         nit   = ni*j
         ok(j) = scmake(ia(j), nit, nb)
      enddo

c ... count the number of errorless allocs
      nok = 0
      do j = 1, 10
         nok = nok + ok(j)
      enddo

c ... report the allocs
c      write(6, 101) iprc, nprc, nok
c 101  format('Allocs(', i2, '/', i2, ') = ', i2)

c ... count the bytes allocated
      nit = 0
      do j = 1, 10
         nit = nit + scalen(ia(j))
      enddo

c ... report the number of bytes
c      write(6, 102) iprc, nprc, nit
c 102  format('Count(', i2, '/', i2, ') = ', i8)

c ... do the frees
      do j = 1, 10
         ok(j) = scfree(ia(j))
      enddo

c ... count the number of errorless frees
      nok = 0
      do j = 1, 10
         nok = nok + ok(j)
      enddo

c ... report the frees
c      write(6, 103) iprc, nprc, nok
c 103  format('Frees(', i2, '/', i2, ') = ', i2)

      return
      end

c--------------------------------------------------------------------------
c--------------------------------------------------------------------------

      program main
      implicit none

      integer jotsk, nprc, rprc
      integer nt, err

      integer omp_get_num_threads, omp_get_thread_num
      external omp_get_num_threads, omp_get_thread_num

      integer scinth
      external scinth, sid

      print *, 'Enter the number of processes to use: '
      read *, rprc

      jotsk = rprc
      call omp_set_num_threads(jotsk)

      nprc = omp_get_num_threads()
      print *, 'Number of processes available: ', nprc

      err = scinth(rprc, sid)
      print *, 'SCORE initialized'

!$omp parallel
!$omp&   default(private)
      call mema
!$omp end parallel

      print *, 'Done'

      end

c--------------------------------------------------------------------------
c--------------------------------------------------------------------------

