;
; GEXECF.SCM - gexec file tests
;
; include "cpyright.h"
;

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; CAT - function to get process output

(define (cat mode in out err n tag)
    (let* ((s (read-line in)))

          (if (not (number? n))
	      (set! n (string->number n)))

          (if (not (eof-object? s))
	      (begin (set! n 0)
		     (printf out "%s #%d %d> %s\n" tag n mode s)))

	  (if (< n 4)
	      (cat mode in out err (+ n 1) tag)
	      0)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

;(process-group-debug 3)

(! osrc -t f1 @o fw:gexo1)
(! osrc -t f2 @b fw:gexb1)
(! osrc -t f3 @o2 @e3 fw:gexo2 @ fw:gexe2)

(quit)

