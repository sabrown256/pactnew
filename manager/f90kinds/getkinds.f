!
! Source Version: 3.0
! Software Release #: LLNL-CODE-422942
!
! include "cpyright.h"
!

!
! Determine the available KIND parameters.
!
! Subroutines find_default_kinds, find_integer_kinds, and find_real_kinds are
! derived from 'Migrating to Fortran 90' by James F. Kerrigan
! Chapter 9: Numeric Models
!


      program getkinds

        implicit none

        integer, parameter :: max_kinds = 16

        integer :: current
        integer :: range
        type kind_structure
           integer precision
           integer range
        end type kind_structure

        integer :: ikinds(max_kinds)
        type(kind_structure) :: rkinds(max_kinds)

!       call find_default_kinds
        call find_integer_kinds
        call find_real_kinds
!       call print_kinds
        call make_kind_sub

      contains

!-----------------------------------------------------------------------
!-----------------------------------------------------------------------

        subroutine find_default_kinds
          character a
          complex c
          double precision d
          integer i
          logical l
          real x

          print 100, "character (KIND)", kind(a)
          print 100, "complex (KIND)", kind(c)
          print 100, "double precision (KIND)", kind(d)
          print 100, "integer (KIND)", kind(i)
          print 100, "logical (KIND)", kind(l)
          print 100, "real (KIND)", kind(x)
          
100       format(a40, ':  ', g24.10)

        end subroutine find_default_kinds

!-----------------------------------------------------------------------
!-----------------------------------------------------------------------

        subroutine find_integer_kinds

          ikinds = 0
          
          do range = 1, 128
             current = selected_int_kind(range)
             if (current .eq. -1) exit
             if (current .gt. max_kinds) then
                print *, "current > max_kinds", current, max_kinds
                stop
             else
                ikinds(current) = range
             endif
          enddo
          
        end subroutine find_integer_kinds

!-----------------------------------------------------------------------
!-----------------------------------------------------------------------

        subroutine find_real_kinds
          integer :: precision

          rkinds(:)%precision = 0
          rkinds(:)%range = 0

          R: do range = 1, 152
             P: do precision = 1, 128
                current = selected_real_kind(precision, range)
                if (current .gt. max_kinds) then
                   print *, "current > max_kinds", current, max_kinds
                   stop
                else if (current .gt. 0) then
                   if (precision .gt. rkinds(current)%precision)        &
     &                  rkinds(current)%precision = precision
                   if (range .gt. rkinds(current)%range)                &
     &                  rkinds(current)%range = range
                endif
                if (current .eq. -1) exit
             enddo P
             if (current .eq. -2) exit
          enddo R

        end subroutine find_real_kinds

!-----------------------------------------------------------------------
!-----------------------------------------------------------------------

        subroutine print_kinds

          do current = 1, max_kinds
             if (ikinds(current) .eq. 0) cycle
             print 300, current, ikinds(current)
300          format("Valid kind (SELECTED_INT_KIND):   ",               &
     &            i5, ' => +/- 10**', i3.3)
          enddo

          do current = 1, max_kinds
             if (rkinds(current)%precision .eq. 0 .or.                  &
     &           rkinds(current)%range     .eq. 0) cycle
             print 500, current, rkinds(current)%precision,             &
     &                           rkinds(current)%range
500          format("Valid kind (SELECTED_REAL_KIND):   ", i5, " => p=",&
     &              i3.3, " r=",i3.3)
          enddo

        end subroutine print_kinds

!-----------------------------------------------------------------------
!-----------------------------------------------------------------------

        subroutine make_kind_sub

          write(6, 1100) "program xxx"

          ! print type declarations
      
          write(6, 1001) "integer", "ii"
          do current = 1, max_kinds
             if (ikinds(current) .eq. 0) cycle
             write(6, 1002) "integer", current, "ii", current
          enddo

          write(6, 1001) "real", "rr"
          do current = 1, max_kinds
             if (rkinds(current)%precision .eq. 0 .or.                  &
     &           rkinds(current)%range     .eq. 0) cycle
             write(6, 1002) "real", current, "rr", current
          enddo

          write(6, 1001) "double precision", "dd"

          write(6, 1100) "  open(7, file='from_f', status='replace')"
          write(6, 1100) "  call fcsizeofinit"

          ! call C subroutine to determine size

          write(6, 1003) "ii", "ii"
          write(6, 1005) "integer", 0, 0, 0

          do current = 1, max_kinds
             if (ikinds(current) .eq. 0) cycle
             write(6, 1004) "ii", current, "ii", current
             write(6, 1005) "integer", current, ikinds(current), 0
          enddo

          write(6, 1003) "rr", "rr"
          write(6, 1005) "real", 0, 0, 0
          write(6, 1003) "dd", "dd"
          write(6, 1005) "double", 0, 0, 0

          do current = 1, max_kinds
             if (rkinds(current)%precision .eq. 0 .or.                  &
     &           rkinds(current)%range     .eq. 0) cycle
             write(6, 1004) "rr", current, "rr", current
             write(6, 1005) "real", current,                            &
     &            rkinds(current)%precision, rkinds(current)%range
          enddo
         
          write(6, 1100) "  close(7)"
          write(6, 1100) "  call fcexternal"

          write(6, 1101) "100     format(a20,3i5)"
          write(6, 1100) "end program xxx"

1001      format(8x,a," :: ",a,"(2)")
1002      format(8x,a,"(kind=",i3,") :: ",a,i3.3,"(2)")
1003      format(8x,"call fcsizeof(",a,"(1),",a,"(2))")
1004      format(8x,"call fcsizeof(",a,i3.3,"(1),",a,i3.3,"(2))")
          !1005  format("  write(7,*) '",a,"', ",i5)
1005      format(8x,"write(7,100) '",a,"'", 3(", ",i5))
1100      format(6x,a)
1101      format(a)

        end subroutine make_kind_sub


!-----------------------------------------------------------------------
!-----------------------------------------------------------------------

      end program getkinds

