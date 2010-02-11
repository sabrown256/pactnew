;
; SHNUM.SCM - numeric tests
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(define status 0)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TEST-NUMERIC-COMPARISONS - test the numeric comparison predicates

(define (test-numeric-comparisons)
    #t)

(printf nil "\n")
(printf nil "Numeric comparison tests:\n")
(test-numeric-comparisons)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TEST-NUMERIC-TYPE - test numeric type handling

(define (test-numeric-type)

    (define (check res expect ok)
        (if res
	    (if (eq? (car res) (car expect))
		(check (cdr res) (cdr expect) #t)
		(begin (set! status 10)
		       "fail"))
	    "ok"))

    (define (test fnc . expect)
        (let* ((res (list (fnc 1)
			  (fnc 1.0)
			  (fnc 1.2)
			  (fnc -1)
			  (fnc -1.0)
			  (fnc -1.2))))
	      (printf nil "-> %-12s  %-6s %s %s\n"
		      fnc
		      (check res expect #t)
		      res expect)))

    (test real?     #t #t #t #t #t #t)
    (test integer?  #t #t #f #t #t #f))

(printf nil "\n")
(printf nil "Numeric type tests:\n")
(test-numeric-type)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(quit status)

