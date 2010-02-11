; 
; SXPDBG.SCM - PDB test functions
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(set-format "short"   "%4d")
(set-format "integer" "%6d")
(set-format "long"    "%8d")
(set-format "float"   "%11.3e")
(set-format "double"  "%11.3e")

(define vars (list "c" "a" "e" "s"
		   "cn" "an" "en" "sn"
		   "cs" "as" "es" "ss"
		   "struct1" "struct2" "struct3" "struct3a" "struct4"))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PDB-DATA-GEN - generate and return list of PDBDATA objects written
;              - to the internal file

(define (pdb-data-gen)

; variations on char
    (define c (write-pdbdata nil "c" (type "char" 10) "bar"))
    (define a (write-pdbdata nil "a" (type "char *") "foo"))
    (define e (write-pdbdata nil "e" (type "char *" 3) "Foo" () "Bar"))
    (define s (write-pdbdata nil "s" (type "char **") '("Hello" "world")))

; variations on short
    (define cs (write-pdbdata nil "cs" (type "short" 10) 1 2 3 4 5 6 7 8 9 10))
    (define as (write-pdbdata nil "as" (type "short *") '(1 2 3 4)))
    (define es (write-pdbdata nil "es" (type "short *" 3) '(-1 -2 -3) '(4 5 6) '(7 8 9)))
    (define ss (write-pdbdata nil "ss" (type "short **") '((1 2 3 4 5) (6 7 8 9))))

; variations on integer
    (define cn (write-pdbdata nil "cn" (type "integer" 10) 1 2 3 4 5 6 7 8 9 10))
    (define an (write-pdbdata nil "an" (type "integer *") '(1 2 3 4)))
    (define en (write-pdbdata nil "en" (type "integer *" 3) '(-1 -2 -3) '(4 5 6) '(7 8 9)))
    (define sn (write-pdbdata nil "sn" (type "integer **") '((1 2 3 4 5) (6 7 8 9))))

; simple struct without pointers
    (make-defstr* nil "cat" (def-member integer i)
		            (def-member integer i1 2)
			    (def-member float   f)
			    (def-member double  dd 2))

    (define struct1 (write-pdbdata nil "struct1"
				   (type "cat")
				   '(10
				     (1 2)
				     20.0
				     (3.1415926  5.0))))

; simple struct without pointers, with harder alignment challenges
    (make-defstr* nil "dog" (def-member integer i)
		            (def-member char c 10) 
			    (def-member integer i1 2)
			    (def-member float   f))

    (define struct2 (write-pdbdata nil "struct2"
				   (type "dog")
				   '(10
				     "test"
				     (1 2)
				     20.0)))

; struct with pointers and primitive types only
    (make-defstr* nil "bird" (def-member integer i)
                             (def-member char c 10) 
			     (def-member integer *i1)
			     (def-member char *a)
			     (def-member char **s)
			     (def-member float   f))

    (define struct3a (write-pdbdata nil "struct3a"
				    (type "bird" 3)
				    '(10 "test"
					 ((1 2))
					 ("doggie")
					 (("big" "ugly"))
					 20.0)
				    '(20 "baz"
					 ((3 4 5 6))
					 ("kitty")
					 (("nice" "soft" "warm"))
					 22.0)
				    '(30 "foo"
					 (7)
					 ("birdy")
					 (("small"))
					 24.0)))

; array of structs with pointers and primitive members
    (define struct3 (write-pdbdata nil "struct3"
				   (type "bird **")
				   '(((10 "test"
					  ((1 2))
					  ("doggie")
					  (("big" "ugly"))
					  20.0)
				      (20 "baz"
					  ((3 4 5 6))
					  ("kitty")
					  (("nice" "soft" "warm"))
					  22.0)
				      (30 "foo"
					  (7)
					  ("birdy")
					  (("small"))
					  24.0)))))

    (make-defstr* nil "rabbit" (def-member integer i)
		               (def-member char c 10) 
			       (def-member dog **ddd)
			       (def-member integer x 4))

; struct with pointers and struct members
    (define struct4 (write-pdbdata nil "struct4"
				   (type "rabbit")
				   '(10 "test"
					((((11 "first" (1 2) 20.1)
					   (21 "second" (3 4) 30.2)
					   (31 "third" (5 6) 40.3))))
					(12 13 14 15))))

    (list c a e s
	  cn an en sn
	  cs as es ss
	  struct1 struct2 struct3a struct3 struct4))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PDB-DATA-PRINT - print the list of PDBDATA objects

(define (pdb-data-print set out)

    (define (do-next i n)
        (if (< i n)
	    (let* ((d (list-ref set i)))
	          (if (= i 14)
		      (set-switch 0 2))
		  (print-pdb out d)
		  (do-next (+ i 1) n))))

    (do-next 0 (length vars)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PDB-DATA-LIST - test pdb->list

(define (pdb-data-list vars set out)

    (define (wr-data name x)
        (apply write-pdbdata (cons nil (cons name (cdr x)))))

    (define (do-next i n)
        (if (< i n)
	    (let* ((v (list-ref vars i))
		   (d (list-ref set i)))
	          (print-pdb out (wr-data v (pdb->list d)))
		  (do-next (+ i 1) n))))

    (do-next 0 (length vars)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PDB-DATA-WRITE - write to a file

(define (pdb-data-write vars set out)

    (define (do-next i n)
        (if (< i n)
	    (let* ((v (list-ref vars i))
		   (d (list-ref set i)))
	          (write-pdbdata out v d)
		  (do-next (+ i 1) n))))

;  copy over defstrs
    (write-defstr* out (read-defstr* nil "cat"))
    (write-defstr* out (read-defstr* nil "dog"))
    (write-defstr* out (read-defstr* nil "bird"))
    (write-defstr* out (read-defstr* nil "rabbit"))

    (do-next 0 (length vars)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PDB-DATA-READ - read from a file

(define (pdb-data-read in vars)

    (define (do-one x)
        (print-pdb nil (read-pdbdata in x)))

    (for-each do-one vars))

;-----------------------------------------------------------------;
;-----------------------------------------------------------------;
