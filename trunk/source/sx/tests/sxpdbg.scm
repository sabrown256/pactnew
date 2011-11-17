; 
; SXPDBG.SCM - PDB test functions
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(define current-file nil)

(load "pdbvar.scm")

(set-format "short"   "%4d")
(set-format "integer" "%6d")
(set-format "long"    "%8d")
(set-format "float"   "%11.3e")
(set-format "double"  "%11.3e")

(define vars (list "c" "a" "e" "s"
		   "cn" "an" "en" "sn"
		   "cs" "as" "es" "ss"
		   "struct1" "struct2" "struct3" "struct3a" "struct4"))

(collect-io-info 0)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; IO-STATS - report the I/O stats on FP
;          - use collect-io-info to control gathering of timing info
;          - duplicate of function in pdbview.scm which we don't
;          - want to load

(define (io-stats fp)
    "Procedure: Print the I/O operation statistics for the given file.
     Usage: io-stats <file>"

    (let* ((stats (get-io-info fp)))

          (define io-opers (list "none" "fopen" "fclose"
				 "ftell" "lftell" "fseek" "lfseek"
				 "fread" "lfread" "fwrite" "lfwrite"
				 "fprintf" "fputs" "fgets" "fgetc"
				 "ungetc" "fflush" "feof" "setvbuf"
				 "pointer" "segsize"))
          (define (do-one sl ol)
	      (if sl
		  (let* ((sd (list-ref sl 0))
			 (nh (car sd))
			 (ns (cdr sd))
			 (on (list-ref ol 0)))

; GOTCHA: some 32 bit machines get a different number of tells than
; 64 bit machines
		        (if (and (> nh 0) (not (string=? on "lftell")))
			    (printf nil "      %-10s %8d %11.3e\n"
				    on nh (/ ns nh)))

			(do-one (cdr sl) (cdr ol)))))

	  (printf nil "I/O stats:\n")
	  (printf nil "   Operation       # Hits   Tave (sec)\n")
	  (do-one stats io-opers)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PDB-DATA-PRIMH - generate primitives using the high level interface
;                - using vardef 

(define (pdb-data-primh)

; variations on char
   (vardef char c (10) "bar")
   (vardef char *a "foo")
   (vardef char *e (3) "Foo" () "Bar")
   (vardef char **s ("Hello" "world"))

; variations on short
   (vardef short cs (10) 1 2 3 4 5 6 7 8 9 10)
   (vardef short *as (1 2 3 4))
   (vardef short *es (3) (-1 -2 -3) (4 5) (6 7 8 9))
   (vardef short **ss ((1 2 3 4 5) (6 7 8 9)))

; variations on integer
   (vardef int cn (10) 1 2 3 4 5 6 7 8 9 10)
   (vardef int *an (1 2 3 4))
   (vardef int *en (3) (-1 -2 -3) (4 5) (6 7 8 9))
   (vardef int **sn ((1 2 3 4 5) (6 7 8 9)))

   #t)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PDB-DATA-PRIML - generate primitives using the low level interface
;                - using write-pdbdata directly

(define (pdb-data-priml)

; variations on char
   (define-global c (write-pdbdata nil "c" (type "char" 10) "bar"))
   (define-global a (write-pdbdata nil "a" (type "char *") "foo"))
   (define-global e (write-pdbdata nil "e" (type "char *" 3) "Foo" () "Bar"))
   (define-global s (write-pdbdata nil "s" (type "char **") '("Hello" "world")))

; variations on short
   (define-global cs (write-pdbdata nil "cs" (type "short" 10) 1 2 3 4 5 6 7 8 9 10))
   (define-global as (write-pdbdata nil "as" (type "short *") '(1 2 3 4)))
   (define-global es (write-pdbdata nil "es" (type "short *" 3) '(-1 -2 -3) '(4 5) '(6 7 8 9)))
   (define-global ss (write-pdbdata nil "ss" (type "short **") '((1 2 3 4 5) (6 7 8 9))))

; variations on integer
   (define-global cn (write-pdbdata nil "cn" (type "integer" 10) 1 2 3 4 5 6 7 8 9 10))
   (define-global an (write-pdbdata nil "an" (type "integer *") '(1 2 3 4)))
   (define-global en (write-pdbdata nil "en" (type "integer *" 3) '(-1 -2 -3) '(4 5) '(6 7 8 9)))
   (define-global sn (write-pdbdata nil "sn" (type "integer **") '((1 2 3 4 5) (6 7 8 9))))

   #t)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PDB-DATA-GEN - generate and return list of PDBDATA objects written
;              - to the internal file

(define (pdb-data-gen)

    (if #t
	(pdb-data-primh)
	(pdb-data-priml))

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

(define (pdb-data-write vars set out struct)

    (define (do-next i n)
        (if (< i n)
	    (let* ((v (list-ref vars i))
		   (d (list-ref set i)))
	          (write-pdbdata out v d)
		  (do-next (+ i 1) n))))

;  copy over defstrs
    (if struct
	(begin (write-defstr* out (read-defstr* nil "cat"))
	       (write-defstr* out (read-defstr* nil "dog"))
	       (write-defstr* out (read-defstr* nil "bird"))
	       (write-defstr* out (read-defstr* nil "rabbit"))))

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
