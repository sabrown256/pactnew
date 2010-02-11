;
; ULIOT.SCM - test interrupt driven I/O
;           - this is probably a no-op when a screen window is not opened
;           - at least it should be a reminder
;           - the problem is that we seem to get SIGIO interrupts for
;           - output events and we are really not prepared to do so
;           - if we have any problems with the semantics of the underlying
;           - signal handling this test can hang
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(do ((i 0 (+ i 1)))
    ((>= i 100) (printf nil "done\n"))
    (printf nil "foo\n"))

(quit)
