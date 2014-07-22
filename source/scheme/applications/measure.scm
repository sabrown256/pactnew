;
; MEASURE.SCM - routines to measure memory, time, etc
;             - of aspects of the scheme interpreter operation
;
; include "cpyright.h"
;

;--------------------------------------------------------------------------

; MEMORY

;--------------------------------------------------------------------------

; this holds the net memory leaked
(define :mmem-loss 0)

; this holds the size of the temporary cons between start and stop
(define :mmem-dm1 nil)

; this holds the size of memory tied up in the memory-usage calls themselves
(define :mmem-dm2 nil)

; this tells us whether it is the first call to measure
(define :mmem-first #t)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MEASUE-MEMORY - measure the net memory cost of evaluating EXPR
;               - returns #f

(define-macro (measure-memory expr)
    (let* (start stop dloss)
        (if :mmem-first
	    (begin (set! start (memory-usage))
		   (eval #t)
		   (set! stop (memory-usage))
		   (set! :mmem-dm1 (- (list-ref stop 1) (list-ref start 1)))
		   (set! :mmem-dm2 (- (list-ref stop 2) (list-ref start 2)))
                   (printf nil "\n  Memory Usage\n")
                   (printf nil "Alloc  Free  Diff     Expression\n")))
	(set! start (memory-usage))
	(eval expr)
	(set! stop (memory-usage))
        (if :mmem-first
            (begin
                (set! dloss (- (list-ref stop 2) (list-ref start 2)))
                (printf nil "%5ld %5ld %5ld  :  %s\n"
			(- (list-ref stop 0) (list-ref start 0) :mmem-dm1 :mmem-dm2)
			(- (list-ref stop 1) (list-ref start 1) :mmem-dm1 :mmem-dm2)
			(- (list-ref stop 2) (list-ref start 2))
			expr))
            (begin
                (set! dloss (- (list-ref stop 2) (list-ref start 2) :mmem-dm2))
                (printf nil "%5ld %5ld %5ld  :  %s\n"
			(- (list-ref stop 0) (list-ref start 0) :mmem-dm1 :mmem-dm2)
			(- (list-ref stop 1) (list-ref start 1) :mmem-dm1)
			dloss
			expr)))
        (set! :mmem-loss (+ :mmem-loss dloss))
        (set! :mmem-first #f)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; NET-MEMORY-LOSS - return the net memory loss

(define (net-memory-loss)
    :mmem-loss)

;--------------------------------------------------------------------------

; TIME

;--------------------------------------------------------------------------

; MEASURE-TIME - report the time it takes to evaluate FNC
;              - return the time as (<hour> <minute> <second>)

(define (second-timer)
   (let* ((ti (time))
          (hr (list-ref ti 3))
          (mn (list-ref ti 4))
          (sc (list-ref ti 5))
	  (dt (+ sc (* 60 (+ mn (* 60 hr))))))
         dt))

(define (time-diff tf ti)
   (let* ((dt (truncate (abs (- tf ti))))
          (sc (remainder dt 60))
	  (tm (quotient dt 60))
	  (mn (remainder tm 60))
	  (hr (quotient tm 60)))
         (list hr mn sc)))

(define-macro (measure-time fnc msg)
   (if msg
       (printf nil "   %s ... " msg))
   (let* ((ti (second-timer))
	  tf dt)
         (eval fnc)
	 (set! tf (second-timer))
	 (set! dt (time-diff tf ti))
	 (if msg
	     (printf nil "(%d:%02d)\n" (list-ref dt 1) (list-ref dt 2)))
	 dt))
		       
;--------------------------------------------------------------------------
;--------------------------------------------------------------------------
