;
; MSYNT.SCM - helpers for BASIS language syntax mode
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

; MAKE-LEGAL-DECLARATION - make a legal define out of the argument

(define-macro (make-legal-declaration x)
    (if (pair? x)
	(cons 'define (cadr x))
	(list 'define x nil)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; M/SCALAR - define scalar variables in the
;          - current environment

(define-macro-ev (m/scalar . dcls)
    (let* ((dlst (reverse (map make-legal-declaration dcls))))
          `(for-each eval (list ,@dlst))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; M/ARRAY - define array variables in the
;         - current environment

(define-macro (m/array len)
    (cond ((not (pair? len))
	   (make-vector len))
	  ((>= (length len) 2)
	   (let* ((mne (list-ref len 0))
		  (mxe (list-ref len 1))
		  (mn  (if (pair? mne)
			   (eval mne)
			   mne))
		  (mx  (if (pair? mxe)
			   (eval mxe)
			   mxe)))
	         (make-vector (+ (- mx mn) 1))))

	  (else
	   (printf nil "Bad array declaration\n"))))
	   
;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; M/DEFUN - define a function complete with the
;                               - correct C return semantics

(define (-m-filter-params param body bdy)
   (if (null? body)
       (reverse bdy)
       (let* ((stmt (list-ref body 0))
	      (rest (list-tail body 1))
	      (assg (and (pair? stmt)
			 (eqv? (car stmt) 'm/scalar)))
	      (avar (if assg (cadr stmt))))
	     (if (not (memv avar param))
		 (-m-filter-params param rest (cons stmt bdy))
		 (-m-filter-params param rest bdy)))))

(define-macro-ev (m/defun . dcls)
    (let* ((proto (list-ref dcls 0))
	   (body  (list-tail dcls 1))
	   (param (list-tail proto 1))
	   (bdy   (-m-filter-params param body nil)))


          `(define ,proto
	           (call-with-cc (lambda (-return-)
			                 ,@bdy)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; M/LOOP - handle do loop tests

(define (m/loop vi v vf)
    (if (<= vi vf)
	(<= v vf)
	(>= v vf)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; M/AC - handle do array references or function calls

(define-macro-ev (m/ac x lst)
    `(cond ((procedure? ,x)
	    (,x ,@lst))
	   ((vector? ,x)
	    (vector-ref ,x (- ,@lst 1)))
	   (else
	    (printf nil "Unknown function or array %s (%s)\n" ,x ,lst))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; M/>> - handle input operator >>

(define-macro (m/>> dev exprs)
    (let* ((d (eval dev)))

          (define-macro (m-set-read var)
	      (if d
		  (apply set! (list var (read d)))
		  (apply set! (list var (read)))))

          (for-each m-set-read exprs)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; M/<< - handle output operator <<

(define (m/<< dev exprs)
    (for-each '(lambda (x) (printf dev "%s" x)) exprs)
    (printf dev "\n"))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; M/SET! - handle the an assigment possibly with
;        - array or struct members on the LHS

(define (m-arr-set! arr i imx val)
    (if (<= i imx)
	(let* ((v (car val))
	       (rst (cdr val)))
	      (vector-set! arr (- i 1) v)
	      (m-arr-set! arr (+ i 1) imx rst))))

(define-macro-ev (m/set! expr val)
    (if (pair? expr)
	(let* ((arr (list-ref expr 0))
	       (rng (list-ref expr 1))
	       (imn (if (pair? rng)
			(list-ref rng 0)
			rng))
	       (imx (if (pair? rng)
			(list-ref rng 1)
			rng)))

	      `(m-arr-set! ,arr ,imn ,imx (list ,@val)))

	`(set! ,expr ,val)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; M/LABEL - handle the label

(define-macro (m/label val) ())

;--------------------------------------------------------------------------

;                       BASIS TRANSLATION FOR SHOW

;--------------------------------------------------------------------------

; TRANSL-M-FUNCTION-DECLARATION - do function translation

(define (transl-m-function-declaration dcls)
    (let* ((proto (list-ref dcls 0))
	   (body  (list-tail dcls 1)))
;          (printf nil "%s\n"
          (format-expr nil 
		  `(define ,proto
		       (call-with-cc (lambda (-return-)
				             ,@body))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TRANSL-M-SCALAR-DECLARATION - do a scalar variable translation

(define (transl-m-scalar-declaration dcls)
    (let* ((dlst (reverse (map make-legal-declaration dcls))))
          (for-each '(lambda (x) (printf nil "%s\n" x))
		    dlst)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TRANSLATE-M - translate a BASIS language form to SCHEME and display it

(define (translate-m-expr expr)
    (let* ((fnc (list-ref expr 0)))
	  (cond ((eqv? fnc 'm/defun)
		 (transl-m-function-declaration (list-tail expr 1)))

		((eqv? fnc 'm/scalar)
		 (transl-m-scalar-declaration (list-tail expr 1)))

		(else
		 (printf nil "%s\n" expr)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TRANSLATE-M-FILE - translate a file of BASIS language forms to SCHEME

(define (translate-m-file name)
   (let* ((file (open-input-file name)))

         (m-mode)

         (define (rtpl file)
	     (let* ((expr (read file)))
	           (if (not (eof-object? expr))
		       (begin (if expr
				  (translate-m-expr expr))
			      (rtpl file)))))

	 (rtpl file)

	 (close-input-file file)

	 (scheme-mode)))

;--------------------------------------------------------------------------

;                              SYNONYMS

;--------------------------------------------------------------------------

(language-synonyms)
