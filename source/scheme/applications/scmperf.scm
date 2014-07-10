(load "applications/runt.scm")
;
; PERFTEST.SCM - measure and report the performance of PACT SCHEME
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(define n-iter 10000)

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

(define (recursive-test n kernel)
   (if (> n 0)
       (begin (kernel n)
	      (recursive-test (- n 1) kernel))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (iterative-test n kernel)
   (eval (list do '((i 1 (+ i 1))) '((>= i n-iter) #t) (list kernel 'i))))

;(define (iterative-test n kernel)
;   (eval (list for 'i 1 n 1 (list kernel 'i))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define-macro (timer call msg)
   (printf nil "\nStarting %s ... " msg)
   (let* ((ti (reverse (time)))
	  (seci (car ti))
	  (mini (cadr ti))
	  tf dsec dmin)
     (eval call)
     (printf nil "done\n")
     (set! tf (reverse (time)))
     (set! dsec (- (car tf) seci))
     (set! dmin (- (cadr tf) mini))
     (printf nil "%s took %d seconds\n"
	     msg
	     (+ dsec (* 60 dmin)))))
		       
;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(timer (recursive-test n-iter kernel-1) "Kernel #1 Recursive")
(timer (iterative-test n-iter kernel-1) "Kernel #1 Iterative")

(timer (recursive-test n-iter kernel-2) "Kernel #2 Recursive")
(timer (iterative-test n-iter kernel-2) "Kernel #2 Iterative")

(printf nil "\n")
(quit)
