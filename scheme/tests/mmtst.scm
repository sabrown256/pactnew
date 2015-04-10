#!/usr/bin/env scheme
;
; MMTST.SCM - test garbage collection
;
; include "cpyright.h"
;

(load "measure.scm")

(define measure measure-memory)

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

(define loss (net-memory-loss))
(printf nil "\nNet memory loss: %d bytes\n\n" loss)
(quit loss)
