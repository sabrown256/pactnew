c
c PATHFT.F - FORTRAN test of PANACEA Time History Functionality
c
c
c Source Version: 3.0
c Software Release #: LLNL-CODE-422942
c
c include "cpyright.h"
c

c--------------------------------------------------------------------------
c--------------------------------------------------------------------------

c MAIN - start it out here

      program pantest

      common / comi / ncycle, filid, recid
      common / comr / time, v1, v2, v3
      real*8 time, v1, v2, v3

      call initlz

c ... generate some data
      do 100 ncycle = 1, 10

         time = 0.01*float(ncycle)
	 v1   = 10.0 + 10.0*time
	 v2   = 20.0*time
	 v3   = v2*(1.0 - time)

         call output
 100  continue
	   
      call finish

      stop
      end

c--------------------------------------------------------------------------
c--------------------------------------------------------------------------

c FINISH - close the files and transpose

      subroutine finish

      common / comi / ncycle, filid, recid
      integer patrnf, pfclos, pathfm
      integer filid, nfid

      nfid = pathfm(filid)
      if (nfid .eq. -1)
     &    call errorh
      if (nfid .ne. filid)
     &   filid = nfid

      if (pfclos(filid) .eq. 0)
     & call errorh

      if (patrnf(4, 'thft', -1, 100) .eq. 0) then
         call errorh
         stop 8
      endif

      return
      end

c--------------------------------------------------------------------------
c--------------------------------------------------------------------------

c INITLZ - initialize the TH records

      subroutine initlz

      common / comi / ncycle, filid, recid
      integer paarec, pabrec, paerec, pathop
      integer filid, recid
      character*8 name, type, domain
      integer nf, nt, nd

c ... open a TH file family
      filid = pathop(8, 'thft.t00', 1, 'w', 100000, 0, ' ')
      if (filid .le. 0)
     &   stop 7
      
      name   = 'tdata'
      nf     = 5
      type   = 't-struct'
      nt     = 8
      domain = 'time'
      nd     = 4

c ... define a record structure to be written to the pdbfile
c ... this defines the domain variable (time in this case)
      recid = pabrec(filid, nf, name, nt, type, nd, domain)
      if (recid .eq. 0)
     &   call errorh

c ... add in any number of members to the record structure
      if (paarec(filid, recid, 4, 'v1_1', 15, 'region boundary') .eq. 0)
     &   call errorh

      if (paarec(filid, recid, 4, 'v1_2', 0, ' ') .eq. 0)
     &   call errorh

      if (paarec(filid, recid, 2, 'v2', 13, 'density*value') .eq. 0)
     &    call errorh
      
c ... end the record structure
      if (paerec(filid, recid) .eq. 0)
     &   call errorh

      return
      end

c--------------------------------------------------------------------------
c--------------------------------------------------------------------------

c OUTPUT - fill the buffer and dump it

      subroutine output

      common / comi / ncycle, filid, recid
      common / comr / time, v1, v2, v3
      real*8 time, v1, v2, v3
      integer pawrec
      real*8 tstruct(4)
      integer filid, recid, inst

c ... put the values into the proper order in the record structure
c ... the domain element is always the first member
      tstruct(1) = time
      tstruct(2) = v1
      tstruct(3) = v3
      tstruct(4) = v2

      inst = ncycle

      if (pawrec(filid, recid, inst, 1, tstruct(1)) .eq. 0)
     &   call errorh 

      return 
      end

c--------------------------------------------------------------------------
c--------------------------------------------------------------------------

c ERRORH - error reporter

      subroutine errorh

      character*(256) err
      integer nchr

      call pfgerr(nchr, err)

      write(*, *)err

      end

c--------------------------------------------------------------------------
c--------------------------------------------------------------------------
