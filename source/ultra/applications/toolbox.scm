;
; TOOLBOX.SCM - a set of useful tools that make building other
;             - ULTRA II extensions easier
;             - feel free to add useful functions to this file
;             - please observe the following rules in order to
;             - make it easier for people to read:
;             -
;             - 1) Observe and obey the documentation standard of the
;             -    file. The uniformity makes it easy to spot patterns
;             -    such as beginning and ending of function definitions.
;             -
;             - 2) Document your additions both with the function and
;             -    with a summary in the contents section. The CONTENTS
;             -    section should be kept in alphabetical order to
;             -    help users find things they want.
;
;;;;;;;;;;;;;;;;;;;;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

;
; CONTENTS:
;
; (domain-intersection <curve> <x-min> <x-max>)
;   Return a cons which defines the intersection of a curve domain with
;   a specified interval. The car is the minimum and the cdr is the
;   maximum. This is like the built in GET-DOMAIN. Returns nil if the
;   intersection is empty.
;
; (get-common-domain <curve-list>)
;   Return a cons which defines the intersection of the domains of all the
;   curves on the list. The car is the minimum and the cdr is the maximum.
;   This is like the built in GET-DOMAIN. Returns nil if the intersection
;   is empty.
;
; (line->list <port>)
;   Return a list of objects corresponding to the tokens from a line of
;   text read from the input port
;
; (max-value <number> <number>)
;   Return the largerer of the two given numbers.
;   (this should be expanded to return the largest from a list)
;
; (min-value <number> <number>)
;   Return the smaller of the two given numbers.
;   (this should be expanded to return the smallest from a list)
;
; (sequence <start> <length> <step>)
;   Return a list containing <length> numbers beginning with <start> and
;   incrementing by <step>.
;

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DOMAIN-INTERSECTION

(define (domain-intersection c1 xmn xmx)
    "Procedure: Return a cons which defines the intersection of a curve
     domain with a specified interval. The car is the minimum and the cdr
     is the maximum. This is like the built in GET-DOMAIN. Returns nil if
     the intersection is empty.
     Usage: domain-intersection <curve> <x-min> <x-max>"

    (interactive off)
    (let* ((c1d (get-domain c1))
	   (c1dxn (car c1d))
	   (c1dxx (cdr c1d)))
          (if (and (> xmx c1dxn) (< xmn c1dxx))
	      (cons (max-value xmn c1dxn)
		    (min-value xmx c1dxx)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GET-COMMON-DOMAIN

(define (get-common-domain . lst)
    "Procedure: Return a cons which defines the intersection of the domains
     of all the curves on the list. The car is the minimum and the cdr is
     the maximum. This is like the built in GET-DOMAIN. Returns nil if the
     intersection is empty.
     Usage: get-common-domain <curve-list>"

    (interactive off)
    (let* ((sdm (get-domain (car lst))))

          (define (get-common-domain-aux xmn xmx lst)
	      (if (pair? lst)
		  (let* ((dmn (domain-intersection (car lst) xmn xmx)))
		        (get-common-domain-aux (car dmn)
					       (cdr dmn)
					       (cdr lst)))
		  (cons xmn xmx)))

	  (get-common-domain-aux (car sdm) (cdr sdm) (cdr lst))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; LINE->LIST

(define (line->list file)
    "Procedure: Read a line of text from the given INPUT-PORT <file> and
     return a list of objects corresponding to the space delimited
     tokens in the text line.
     Usage: line->list <port>"
    (let* ((line (read-line file))
	   (port (if (not (eof-object? line)) (string->port line) nil))
	   ret-val)

          (define (read-object prt lvals)
	      (let* ((xv (read prt)))
		    (if (eof-object? xv)
			lvals
			(read-object prt (cons xv lvals)))))
	  (if port
	      (begin (set! ret-val (read-object port nil))
		     (if (not (null? ret-val))
			 (reverse ret-val)
			 ret-val))
	      #f)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MAX-VALUE

(define (max-value x1 x2)
    "Procedure: Return the larger of the two given numbers.
     Usage: max-value <number> <number>"

    (if (< x1 x2)
	x2
	x1))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MIN-VALUE

(define (min-value x1 x2)
    "Procedure: Return the smaller of the two given numbers.
     Usage: min-value <number> <number>"

    (if (> x1 x2)
	x2
	x1))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SEQ-AUX - the auxiliary sequence function

(define (seq-aux strt len delta lst)
    (if (> len 0)
        (seq-aux (+ strt delta) (- len 1) delta (cons strt lst))
        (reverse lst)))

; SEQUENCE

(define (sequence start length step) 
    "Procedure: Return a list containing <length> numbers beginning with 
                <start> and incrementing by <step>.
     Usage: sequence <start> <length> <step>"

    (seq-aux start length step nil))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

