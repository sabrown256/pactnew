;
; IPERF.SCM - measure and report the performance of PACT SCHEME interpreter
;
; include "cpyright.h"
;

(load "runt.scm")
(load "measure.scm")

(define timer measure-time)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (kernel-1 n)
    (+ 3.1415 n))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (kernel-2 n)
    (+ (* (sin n) (ln n)) (* (cos n) (sqrt n))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; RECURSIVE-TEST - call KERNEL N times in a tail recursive fashion

(define (recursive-test n kernel)
   (if (> n 0)
       (begin (kernel n)
	      (recursive-test (- n 1) kernel))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; ITERATIVE-TEST - call KERNEL N times in an apparently iterative fashion
;                - the iteration is really done as a tail recursion
;                - using the DO loop construct

(define (iterative-test n kernel)
   (eval (list do '((i 1 (+ i 1))) '((>= i _n) #t) (list kernel 'i))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define-macro (timer fnc n msg)
    (printf nil "   %s ... " msg)
    (let* ((dt (apply measure-time (list fnc nil)))
	   (mn (list-ref dt 1))
	   (sc (list-ref dt 2))
	   (tf (+ sc (* 60 mn)))
	   (ni (eval n))
	   (rt (/ ni (max tf 1))))
          (printf nil "(%d:%02d)" (list-ref dt 1) (list-ref dt 2))
          (cond ((> rt 1000000)
		 (printf nil " %.2f/usec\n" (/ rt 1000000.0)))
		((> rt 1000)
		 (printf nil " %.2f/msec\n" (/ rt 1000.0)))
		(else
		 (printf nil " %.2f/sec\n" rt)))))

(define (tester n)
   (define-global _n n)
   (printf nil "%d iterations:\n" n)
   (timer (recursive-test _n kernel-1) _n "Kernel #1 Recursive")
   (timer (iterative-test _n kernel-1) _n "Kernel #1 Iterative")
   (timer (recursive-test _n kernel-2) _n "Kernel #2 Recursive")
   (timer (iterative-test _n kernel-2) _n "Kernel #2 Iterative")
   (printf nil "\n"))

(tester 10000)
;(tester 100000)
;(tester 1000000)

(quit)
