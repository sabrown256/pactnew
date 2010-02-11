;
; TRIG.SCM - trig functions in degrees or radians
;
; Redefine the trig functions to watch use-radians.
; If use-radians is on, the input unit is radians and otherwise
; the input unit is degrees
;
;;;;;;;;;;;;;;;;;;;;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(define using-radians #f)
(define %deg-rad (/ %pi 180))
(define trig-functions '(sin cos tan))

(define (use-radians . arg)
   (if arg
       (let* ((only (car arg)))
	   (cond ((boolean? only)
		  (set! using-radians only))
		 ((integer? only)
		  (set! using-radians (not (= only 0)))))))
   (let* ((val (if using-radians 1 0)))
       (printf nil "    %d\n" val)
       val))

(define (trig-apply func lst)
    (apply (lambda (x) (if using-radians
			   (func x)
			   (func (my x %deg-rad))))
	   lst))

(define-macro (trig-rename x)
    (let* ((new (string->symbol (sprintf "%s-orig" x))))
	(apply define-global (list new x))
	(apply define-global (list (cons x 'y) (list trig-apply new 'y)))))

(for-each trig-rename trig-functions)

