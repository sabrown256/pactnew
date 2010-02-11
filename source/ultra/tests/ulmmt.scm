#!ultra -l
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

; This holds the net memory leaked
(define loss 0)

; This holds the size of the temporary cons between strt and stp
(define delta-1 nil)

; This holds the size of memory tied up in the memory-usage calls themselves
(define delta-2 nil)

; This tells us whether it is the first call to measure
(define first #t)

(define-macro (measure expr)
    (let* (strt stp dloss)
        (if first
	    (begin (set! strt (memory-usage))
		   (eval #t)
		   (set! stp (memory-usage))
		   (set! delta-1 (- (list-ref stp 1) (list-ref strt 1)))
		   (set! delta-2 (- (list-ref stp 2) (list-ref strt 2)))
                   (printf nil "\n  Memory Usage\n")
                   (printf nil "Alloc  Free  Diff     Expression\n")))
	(set! strt (memory-usage))
	(eval expr)
	(set! stp (memory-usage))
        (if first
            (let* ((anomaly 0))
                (set! dloss (- (list-ref stp 2) (list-ref strt 2) anomaly)))
            (begin
                (set! dloss (- (list-ref stp 2) (list-ref strt 2) delta-2))
                (printf nil "%5ld %5ld %5ld  :  %s\n"
			(- (list-ref stp 0) (list-ref strt 0) delta-1 delta-2)
			(- (list-ref stp 1) (list-ref strt 1) delta-1)
			dloss
			expr)))
        (set! loss (+ loss dloss))
        (set! first #f)))

(begin
    (system "rm -f _tmp.dat")
    (erase)
    (wrt "_tmp.dat" (line 1 0 0 1))
    (del a))

;
; these should come and go with no net evaulation cost
; any variable creation gets charged to the reader NOT the evaluator
;
(measure #t)
(set! loss 0)
(measure (del (line 1 0 0 1)))                 ; test US handler
(measure (del (dx (line 1 0 0 1) 0.1)))        ; test OPXC handler
(measure (del (dy (line 1 0 0 1) 0.1)))        ; test OPYC handler
(measure (del (sin (line 1 0 0 1))))           ; test UOPXC handler
(measure (del (sinx (line 1 0 0 1))))          ; test UOPYC handler
(measure (del (color (line 1 0 0 1) 2)))       ; test BLTOC handler
(measure (del (smoothn (line 1 0 0 1))))       ; test ULNTOC handler
(measure (del (hide (line 1 0 0 1))))          ; test UC handler
(measure (begin (copy (line 1 0 0 1))          ; test BC handler
                (+ a b)
		(del a b c)))
(measure (begin (copy (line 1 0 0 1))          ; test BC handler
		(copy a b)
                (+ a b c d)
		(del a b c d e)))
(measure (begin (xmm (line 1 0 0 1) 0.1 0.9)   ; test UL2TOC handler
		(del a b)))
(measure (begin (rd _tmp.dat)
		(kill 0)))

(system "rm -f _tmp.dat")

(printf nil "\nNet memory loss: %d bytes\n\n" loss)
(end loss)
