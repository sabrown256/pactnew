#!/usr/bin/env scheme
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

; this holds the net memory leaked
(define loss 0)

; this holds the size of the temporary cons between start and stop
(define delta-1 nil)

; this holds the size of memory tied up in the memory-usage calls themselves
(define delta-2 nil)

; this tells us whether it is the first call to measure
(define first #t)

(define-macro (measure expr)
    (let* (start stop dloss)
        (if first
	    (begin (set! start (memory-usage))
		   (eval #t)
		   (set! stop (memory-usage))
		   (set! delta-1 (- (list-ref stop 1) (list-ref start 1)))
		   (set! delta-2 (- (list-ref stop 2) (list-ref start 2)))
                   (printf nil "\n  Memory Usage\n")
                   (printf nil "Alloc  Free  Diff     Expression\n")))
	(set! start (memory-usage))
	(eval expr)
	(set! stop (memory-usage))
        (if first
            (begin
                (set! dloss (- (list-ref stop 2) (list-ref start 2)))
                (printf nil "%5ld %5ld %5ld  :  %s\n"
			(- (list-ref stop 0) (list-ref start 0) delta-1 delta-2)
			(- (list-ref stop 1) (list-ref start 1) delta-1 delta-2)
			(- (list-ref stop 2) (list-ref start 2))
			expr))
            (begin
                (set! dloss (- (list-ref stop 2) (list-ref start 2) delta-2))
                (printf nil "%5ld %5ld %5ld  :  %s\n"
			(- (list-ref stop 0) (list-ref start 0) delta-1 delta-2)
			(- (list-ref stop 1) (list-ref start 1) delta-1)
			dloss
			expr)))
        (set! loss (+ loss dloss))
        (set! first #f)))

;
; these should come and go with no net evaulation cost
; any variable creation gets charged to the reader NOT the evaluator
;
(measure ())
(measure #t)
(measure 3)
(measure 1.4)
(measure "foo")
(measure 'foo)
(measure '(1 . 2))
(measure '(1))
(measure '(1 2 3 5 8 13 21))
(measure #\A)
(measure #(1 2 3))
(measure (make-hash-table 251))
(measure (make-vector 3))

(measure (and 0 1 #t ()))
(measure (or 0 1 #t ()))
(measure (cond (#f (list "x")) (#t (list "xx"))))

(measure `(1 ,2 3))
(measure `(car ,@(list cdr)))
(measure (append '(1 2 3) '(1.1 1.2)))
(measure (append '(1 2 3) '(1.1 1.2) '("a" "b" "c" "d")))
(measure (list->string (string->list "foo")))
(measure (vector->list (list->vector '(1 2 3))))
(measure (string-append "x" "y"))
(measure (string->symbol "tar"))
(measure (strstr "foobarbaz" "bar"))
(measure (strcasestr "foobarbaz" "bar"))
(measure (close-input-file (open-input-file "mmtst.scm")))
(measure (close-output-file (open-output-file "#tmp#")))
(measure (apply + '(1 2)))
(measure (lambda (x) x))
(measure ((lambda (x) x) 1))
(measure ((lambda (x) x) ((lambda (x) x) 1)))
(measure ((lambda lst (apply + lst)) 1 2 3))
(measure ((lambda (x . lst) (apply + lst)) 0 1 2 3))
(measure (for-each (lambda (x) x) '(1 2 3)))
(measure (map (lambda (x) x) '(1 2 3)))
(measure (let  ((x 3)) x))
(measure (let* ((x 3)) x))
(measure (let* ((x 3) (y x)) y))
(measure (let* (x) (define foo 2)))
(measure (let  () (define x (lambda () #t)) (x)))
(measure (let* () (define (foo x) x) (foo 3)))
(measure (let* ((x 3)) (define (foo x) x) (foo x)))

(measure (printenv))
(measure (printenv "HOME"))
(measure (system "echo $HOME"))
; GOTCHA: this started leaking during the thread reorg 2/25/2009
;(measure (syscmnd "echo $HOME"))

(measure (up-case "foo"))
(measure (down-case "foo"))

;
; we know a function will cost some memory to define (measure it later)
; but it should have no net memory cost to execute
;
(define (identity x) x)
(define (creator y)
   (if y
       (list "true")
       (list "false" 1 2)))

(measure (identity 1))
(measure (creator #t))
(measure (creator #f))
(measure (identity (identity 1.1)))
(measure (identity (identity (identity "x"))))
(measure (creator (identity #t)))
(measure (identity (creator #t)))

(measure (list-tail '(1 2 3 4) 0))
(measure (list-tail '(1 2 3 4) 1))
(measure (list-tail '(1 2 3 4) 2))
(measure (list-tail '(1 2 3 4) 3))
(measure (list-tail '(1 2 3 4) 4))
(measure (list-tail '(1 2 3 4) -1))
(measure (list-tail '(1 2 3 4) -2))
(measure (list-tail '(1 2 3 4) -3))
(measure (list-tail '(1 2 3 4) -4))

(measure (list-ref '(1 2 3 4) 0))
(measure (list-ref '(1 2 3 4) 1))
(measure (list-ref '(1 2 3 4) 2))
(measure (list-ref '(1 2 3 4) 3))
(measure (list-ref '(1 2 3 4) 4))
(measure (list-ref '(1 2 3 4) -1))
(measure (list-ref '(1 2 3 4) -2))
(measure (list-ref '(1 2 3 4) -3))
(measure (list-ref '(1 2 3 4) -4))

;
; these should have a permanent memory cost
;
;(measure (define foo 3))
;(measure (define (foo x) x))

(printf nil "\nNet memory loss: %d bytes\n\n" loss)
(quit loss)
