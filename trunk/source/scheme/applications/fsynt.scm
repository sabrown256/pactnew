;
; FSYNT.SCM - helpers for Fortran syntax mode
;
; Source Version: 5.0
; Software Release #: LLNL-CODE-422942
;
; #include "cpyright.h"
;

; NOTES for future development:
;
; 1) The grammar is mostly complete and so the majority of the work
;    should be done here where the semantics is actually being defined
; 2) When debugging try first showing the translation of a Fortran form into
;    its SCHEME counterpart to determine whether its the parser or the
;    semantics defined here

(load "lang.scm")

(define -continue- 0)
(define -return- 0)

;--------------------------------------------------------------------------

;                             C EVALUATION

;--------------------------------------------------------------------------

; HANDLE-F-DECLARE - declare a list of variables possibly with initializers

(define-macro (handle-f-declare . dcls)
   (if (pair? (car dcls))
       (handle-f-variable-declaration (car dcls))
       (handle-f-function-declaration dcls)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MAKE-LEGAL-DECLARATION - make a legal define out of the argument

(define-macro (make-legal-declaration x)
    (if (pair? x)
	(cons 'define (cadr x))
	(list 'define x nil)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HANDLE-F-SCALAR-DECLARATION - define scalar variables in the
;                             - current environment

(define-macro-ev (handle-f-scalar-declaration . dcls)
    (let* ((dlst (reverse (map make-legal-declaration dcls))))
          `(begin ,@dlst)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HANDLE-F-ARRAY-DECLARATION - define array variables in the
;                            - current environment

(define (handle-f-array-declaration len)
    (make-vector len))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HANDLE-F-STRUCT-DECLARATION - define struct variables in the
;                            - current environment

(define-macro-ev (handle-f-struct-declaration dcls)
    dcls)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HANDLE-F-FUNCTION-DECLARATION - define a function complete with the
;                               - correct C return semantics

(define-macro-ev (handle-f-function-declaration . dcls)
    (let* ((proto (list-ref dcls 0))
	   (body  (list-tail dcls 1)))
          `(define ,proto
	           (call-with-cc (lambda (-return-)
			                 ,@body)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HANDLE-F-STRUCT - handle structs

(define (handle-f-struct . x)
    (printf nil "\nStruct handling not complete\n\n"))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HANDLE-F-UNION - handle unions

(define (handle-f-union . x)
    (printf nil "\nUnion handling not complete\n\n"))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HANDLE-F-TYPEDEF - handle typedefs

(define (handle-f-typedef . x)
    (printf nil "\nTypedef handling not complete\n\n"))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HANDLE-F-DERIVED-TYPE - handle derived-type

(define (handle-f-derived-type . x)
    (printf nil "\nDerived type handling not complete\n\n"))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HANDLE-F-ASSIGNMENT - handle the an assigment possibly with
;                     - array or struct members on the LHS

(define-macro-ev (handle-f-assignment expr val)
    (if (pair? expr)
	(let* ((fnc (list-ref expr 0))
	       (arr (list-ref expr 1))
	       (idx (list-ref expr 2)))
	      (cond ((eqv? fnc 'handle-f-array-ref)
		     `(vector-set! ,arr ,idx ,val))
		    ((eqv? fnc 'handle-f-member-ref)
		     `())
		    (else
		     (printf nil
			     "\nUnknown LHS in assignment %s\n"
			     ,expr))))
	`(set! ,expr ,val)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HANDLE-F-ARRAY-REF - handle the reference of an array element

(define-macro-ev (handle-f-array-ref arr n)
    `(vector-ref ,arr ,n))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HANDLE-F-MEMBER-REF - handle the reference of a struct member

(define (handle-f-member-ref . x)
    (printf nil "\nMember access not complete\n\n"))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HANDLE-F-PRE-INC - handle the ++x

(define-macro-ev (handle-f-pre-inc expr)
    `(begin (set! ,expr (+ ,expr 1))
	    ,expr))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HANDLE-F-PRE-DEC - handle the --x

(define-macro-ev (handle-f-pre-dec expr)
    `(begin (set! ,expr (- ,expr 1))
	    ,expr))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HANDLE-F-POST-INC - handle the x++

(define-macro-ev (handle-f-post-inc expr)
    `(let* ((temp-var ,expr))
           (set! ,expr (+ ,expr 1))
	   temp-var))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HANDLE-F-POST-DEC - handle the x--

(define-macro-ev (handle-f-post-dec expr)
    `(let* ((temp-var ,expr))
           (set! ,expr (- ,expr 1))
	   temp-var))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SWITCH-BODY->COND - process a switch body into the correct
;                   - cond clauses with the proper topology to
;                   - capture C switch semantics

(define (switch-body->cond n body clauses)
    (if body
        (let* ((frst (list-ref body 0))
	       (rest (list-tail body 1))
	       (var  (if (and (pair? frst)
			      (eqv? (list-ref frst 0)
				    'handle-f-label))
			 (list-ref frst 1)))
	       (labl (if var
			 (if (eqv? var #t)
			     var
			     (list 'eqv? 'temp-sym var)))))
	      (if labl
		  (let* ((newcl (cons labl rest)))
		        (switch-body->cond n
					   rest
					   (cons newcl clauses)))
		  (switch-body->cond n rest clauses)))
	(reverse clauses)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HANDLE-F-SWITCH - handle the switch

(define-macro-ev (handle-f-switch expr body)
    (let* ((clauses (switch-body->cond expr (list-tail body 2) nil)))
          `(call-with-cc (lambda (-continue-)
			         (define temp-sym ,expr)
			         (cond ,@clauses)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HANDLE-F-LABEL - handle the label

(define-macro (handle-f-label val) ())

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HANDLE-F-SIZEOF - handle the sizeof operator
;                 - can probably improve this

(define-macro (handle-f-sizeof expr kind)
    (if kind
	(sizeof expr)
	(cond ((string? expr)
	       (string-length expr))
	      ((vector? expr)
	       (vector-length expr))
	      ((pair? expr)
	       (length expr))
	      (else
	       1))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HANDLE-F-CAST - handle a cast
;               - can probably improve this

(define (handle-f-cast type src)
    (if (number? src)
	(cond ((or (string=? type "double")
		   (string=? type "float"))
	       (* src 1.0))
	      ((or (string=? type "short")
		   (string=? type "int")
		   (string=? type "long"))
	       (floor src))
	      (else
	       src))
	(printf nil "\nIllegal cast %s on %s\n\n" type src)))

;--------------------------------------------------------------------------

;                       C TRANSLATION FOR SHOW

;--------------------------------------------------------------------------

; TRANSL-F-FUNCTION-DECLARATION - do function translation

(define (transl-f-function-declaration dcls)
    (let* ((proto (list-ref dcls 0))
	   (body  (list-tail dcls 1)))
;          (printf nil "%s\n"
          (format-expr nil 
		  `(define ,proto
		       (call-with-cc (lambda (-return-)
				             ,@body))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TRANSL-F-SCALAR-DECLARATION - do a scalar variable translation

(define (transl-f-scalar-declaration dcls)
    (let* ((dlst (reverse (map make-legal-declaration dcls))))
          (for-each '(lambda (x) (printf nil "%s\n" x))
		    dlst)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TRANSLATE-C - translate a C form to SCHEME and display it

(define (translate-f-expr expr)
    (let* ((fnc (list-ref expr 0)))
	  (cond ((eqv? fnc 'handle-f-function-declaration)
		 (transl-f-function-declaration (list-tail expr 1)))

		((eqv? fnc 'handle-f-scalar-declaration)
		 (transl-f-scalar-declaration (list-tail expr 1)))

		(else
		 (printf nil "%s\n" expr)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TRANSLATE-F-FILE - translate a file of C forms to SCHEME

(define (translate-f-file name)
   (let* ((file (open-input-file name)))

         (f-mode)

         (define (rtpl file)
	     (let* ((expr (read file)))
	           (if (not (eof-object? expr))
		       (begin (translate-f-expr expr)
			      (rtpl file)))))

	 (rtpl file)

	 (close-input-file file)

	 (scheme-mode)))

;--------------------------------------------------------------------------

;                              SYNONYMS

;--------------------------------------------------------------------------

(language-synonyms)
