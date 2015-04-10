; 
; TDRUN.SCM - test drun
;
; include "cpyright.h"
;

(define-macro (test . cmd)
    (printf nil "Command: %s\n" cmd)
    (eval cmd))

(test pa (stats-toggle))
(test pa (describe 'pd-open))
(test p1 (+ 1 1 2 3 5 8))
(test p2 (+ 1 1 2 3 5 8))
(test pa (+ 1 1 2 3 5 8))
(test define-global txt1 (r1 (+ 1 2 3)))
(test define-global txt2 (r2 (+ 2 3 4)))
(test printf nil "t1: %s\nt2: %s\n" txt1 txt2)
(test pa (quit))
(test done)

(quit)
