;
; TAILR.SCM - tail recursion tests
;

(load "measure.scm")

(define measure measure-memory)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; F1 - simplest non-infinite recursive function

(define (f1 n)
    (if (> n 0)
	(f1 (- n 1))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; F2 - non-infinite recursive function which makes internal
;    - let* frame that completes before the recursive call

(define (f2 n)
    (let* ((m (- n 1)))
          (printf nil "   f2(%d) -> %s\n" m (memory-usage)))
    (if (> n 0)
	(f2 (- n 1))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; F3 - non-infinite recursive function which makes internal
;    - let* frame from which the recursive call is made

(define (f3 n)
    (if (> n 0)
	(let* ((m (- n 1))
               (sz 1000000)
	       (v (make-vector sz)))
              (vector-set! v 1 n)
              (vector-set! v (- sz 1) m)
	      (printf nil "   f3(%d) -> %s\n" m (memory-usage))
	      (f3 m))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(measure (f1 0))
(measure (f1 1))
(measure (f1 2))

(measure (f2 0))
(measure (f2 1))
(measure (f2 2))

(measure (f3 0))
(measure (f3 1))
(measure (f3 4))

(define loss (net-memory-loss))
(printf nil "\nNet memory loss: %d bytes\n\n" loss)
(quit loss)
