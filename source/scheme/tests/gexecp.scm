;
; GEXEC.SCM - gexec tests
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

(! /bin/echo a @ /bin/echo b @ /bin/echo c)
(! osrc -t p1 @o pw:cat 0 out)
(! osrc -t p2 @b pw:cat 0 bond)
(! osrc -t p3 @o2 @e3 pw:cat 0 out @ pw:cat 0 err)

(quit)

