;
; RUNT.SCM - some runtime Scheme procedures not implemented at
;	   - C level in the interpreter
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
;

; be sure to load this just once
(if (not (defined? caaaar)) (begin

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; some handy constants for string processing

(define-global c\g (integer->char  8))
(define-global c\t (integer->char  9))
(define-global c\n (integer->char 10))
(define-global c\f (integer->char 12))
(define-global c\r (integer->char 13))

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; -1+ - decrement by 1

(define-global (-1+ n)
   "Procedure: decrement the argument by one"
   (- n 1))

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; CAAAAR 

(define-global (caaaar x)
   "Take the car of the car of the car of the car of the argument"
   (car (caaar x)))

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; CDAAAR 

(define-global (cdaaar x)
   "Take the cdr of the car of the car of the car of the argument"
   (cdr (caaar x)))

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; CADAAR 

(define-global (cadaar x) (car (cdaar x)))

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; CDDAAR 

(define-global (cddaar x) (cdr (cdaar x)))

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; CAADAR 

(define-global (caadar x) (car (cadar x)))

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; CDADAR 

(define-global (cdadar x) (cdr (cadar x)))

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; CADDAR 

(define-global (caddar x) (car (cddar x)))

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; CDDDAR 

(define-global (cdddar x) (cdr (cddar x)))

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; CAAADR 

(define-global (caaadr x) (car (caadr x)))

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; CDAADR 

(define-global (cdaadr x) (cdr (caadr x)))

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; CADADR 

(define-global (cadadr x) (car (cdadr x)))

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; CDDADR 

(define-global (cddadr x) (cdr (cdadr x)))

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; CAADDR 

(define-global (caaddr x) (car (caddr x)))

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; CDADDR 

(define-global (cdaddr x) (cdr (caddr x)))

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; CADDDR 

(define-global (cadddr x) (car (cdddr x)))

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; CDDDDR 

(define-global (cddddr x) (cdr (cdddr x)))

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; DO - the do special form

(define-global-macro (do local-vars test-expr . body)
   (let* ((vars (lambda (x)
		  (if (null? x)
		      ()
		      (cons (caar x) (vars (cdr x))))))
	  (inits (lambda (x)
		   (if (null? x)
		       ()
		       (cons (cadar x) (inits (cdr x))))))
	  (repts (lambda (x)
		   (if (null? x)
		       ()
		       (cons (caddar x) (repts (cdr x))))))
	  (test (car test-expr))
	  (exit (cdr test-expr))
	  (form `(lambda ,(vars local-vars)
		   (if ,test
		       (begin ,@exit)
		       (begin ,@body
			      (loop ,@(repts local-vars))))))
	  (loop (eval form)))
     (apply loop (inits local-vars))))

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; LETREC - temporarily reassign letrec to let* to ease the transition from
;        - the original incorrect definition of letrec

(define-global-macro (letrec . args)
   (printf nil "\nUse let* instead of letrec. It is the correct function.\n")
   (printf nil "In the future letrec will be define-globald to its standard meaning.\n\n")
   (set! letrec let*)
   (apply let* args))

;-----------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PROCESS-LOOP-INITS - helper for looping constructs

(define-global (process-loop-inits int)
    (if int
	(if (pair? (car int))
	    (append int '((loop)))
	    (cons int '((loop))))
	'((loop))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PROCESS-LOOP-CLAUSE - helper for looping constructs

(define-global (process-loop-clause cls)
    (if cls
	(if (pair? (car cls))
	    (append cls '((loop)))
	    (cons cls '((loop))))
	'((loop))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PROCESS-LOOP-BODY - helper for looping constructs

(define-global (process-loop-body body expr)
    (let* ((frst  (list-ref body 0))
	   (scnd  (list-ref body 1))
	   (forms (list-tail body 2)))
          (if (and (eqv? frst 'let*) (null? scnd))
	      (append (list-tail body 2) (list #t))
	      (if expr
		  (list body expr)
		  (list body)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; FOR - handle the for loop

(define-macro-ev (for inits tests repeats body)
    (let* ((rpts  (process-loop-clause repeats))
	   (ints  (process-loop-clause inits))
	   (forms (process-loop-body body #t))
	   (tsts  (if tests tests #t)))
          `((lambda ()
	            (define (loop)
		      (if ,tsts
			  (if (call-with-cc (lambda (-continue-)
					            ,@forms))
			      (begin ,@rpts))))
		    ,@ints))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WHILE - handle the while loop
;       - the WHICH arg is #f for a while () {}
;       - the WHICH arg is #t for a do {} while ()

(define-macro-ev (while which tests body)
    (let* ((forms (process-loop-body body '(loop))))
          (if which

; handle do while
	      `((lambda ()
	          (define (loop)
		      (if (call-with-cc (lambda (-continue-)
					        ,body))
			  (if ,tests
			      (loop))))
		  (loop)))

; handle while
	      `((lambda ()
	          (define (loop)
		      (if ,tests
			  (if (call-with-cc (lambda (-continue-)
					            ,@forms))
			      (loop))))
		  (loop))))))

;--------------------------------------------------------------------------

;                         FORMATTING ROUTINES

;-----------------------------------------------------------------------------

; GOTCHA: these may be syntax mode dependent

(define-global special-format-forms
    `(let*    2
      define-global  5
      if      1
      and     2
      or      1
      cond    1
      for     2
      while   5
      lambda  6))

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

(define-global (format-emit file txt indt)
    (printf file "%s" txt)
    indt)

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

(define-global (format-special file head n form buffer)
    (let* ((prtl (sprintf "%s%s " buffer head))
	   (lng  (- (string-length prtl) n))
	   (vars (list-ref form 0))
	   (body (list-tail form 1))
	   (nbf  (sprintf "\n%s" (make-string lng))))
          (if (pair? vars)
	      (format-list file body #f #t
			   (format-list file vars #t #t
					(sprintf "%s%s " buffer head)
					nbf)
			   nbf)
	      (format-list file body #f #t
			   (sprintf "%s%s %s" buffer head vars)
			   nbf))))

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

(define-global (format-begin-list head buffer)
    (if head
	(sprintf "%s(" buffer)
	buffer))

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

(define-global (format-end-list file tail buffer indent)
    (if tail
	(let* ((end-line (strtok (string-copy buffer) " \n")))
	      (cond ((and (string? end-line) (string=? end-line ")"))
		     (format-emit file ")" indent))
		    ((not end-line)
		     indent)
		    (else
		     (format-emit file buffer indent))))
	buffer))

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

(define-global (format-list-forms file form head tail buffer indent)
    (cond ((pair? form)
	   (let* ((frst (list-ref form 0))
		  (rest (list-tail form 1))
		  (spec (memv frst special-format-forms)))

; format some special forms
	         (cond ((and spec (not (number? (car spec))))
			(format-special file frst (cadr spec) rest buffer))

; format lists
		       ((pair? frst)
			(let* ((in (if head
				       (sprintf "%s " indent)
				       indent))
			       (bf (if head
				       (sprintf "%s" buffer)
				       (sprintf "%s " buffer))))
			  (format-list file rest #f tail
				       (format-list file frst #t tail bf in)
				       in)))
; format strings
		       ((string? frst)
			(format-list file rest #f tail
				     (sprintf "%s \"%s\"" buffer frst)
				  indent))

; format anything else
		       (else
			(format-list file rest #f tail
				     (if head
					 (sprintf "%s%s" buffer frst)
					 (sprintf "%s %s" buffer frst))
				     indent)))))
	  ((symbol? form)
	   (if head
	       (sprintf "%s%s" buffer form)
	       (sprintf "%s %s" buffer form)))

	  (else
	   (sprintf "%s)" buffer))))

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

(define-global (format-list file form head tail buffer indent)
    (format-end-list file tail
		     (format-list-forms file form head tail
					(format-begin-list head buffer)
					indent)
		     indent))

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; FORMAT-EXPR - pretty print an expression

(define-global (format-expr file form . rest)
    (let* ((x      (list-ref rest 0))
	   (indent (if x x "")))
          (printf file "%s\n"
		  (if (pair? form)
		      (format-list file form #t #f indent "")
		      (sprintf "%s" form)))))

;-----------------------------------------------------------------------------

;                            LIST SORT

;-----------------------------------------------------------------------------

; QSORT-PIVOT - qsort pivot auxilliary

(define (qsort-pivot sl pred)
    (cond ((or (null? sl) (null? (cdr sl)))
	   '-eos-)
	  ((pred (car sl) (cadr sl))
	   (qsort-pivot (cdr sl) pred))
	  (else
	   (car sl))))

; QSORT-PART - qsort partition auxilliary

(define (qsort-part pl pred y p1 p2)
    (if (null? pl)
	(list p1 p2)
	(let* ((x  (car pl))
	       (lt (and (pred x y) (not (pred y x)))))
	      (if lt
		  (qsort-part (cdr pl) pred y (cons x p1) p2)
		  (qsort-part (cdr pl) pred y p1 (cons x p2))))))

; QSORT - generic quicksort for lists

(define (qsort src pred)
    "Procedure: Sort list LST according to PRED and return the result.
     Usage: qsort '(3 8 4 8 2 8) <=
            qsort '(3 8 4 8 2 8) >=
            qsort '(3 8 4 8 2 8) >=
            qsort '((\"m\" . 13) (\"z\" . 26) (\"a\". 1))
                  (lambda (x y) (string<=? (car x) (car y)))"
    (let* ((y (qsort-pivot src pred)))
          (if (equal? y '-eos-)
	      src
	      (let* ((parts (qsort-part src pred y nil nil)))
		    (append (qsort (list-ref parts 0) pred) 
			    (qsort (list-ref parts 1) pred))))))

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

(define (test-qsort src pred)

    (define (do-test src pred)
        (printf nil "unsorted: %s\n" src)
	(printf nil "sorted:   %s\n" (qsort src pred))
	(printf nil "\n"))

    (define (ass<= x y)
        (string<=? (car x) (car y)))

    (define (ass>= x y)
        (string>=? (car x) (car y)))

    (do-test '(3 8 5 4 8 2 4 1 9 4) <=)
    (do-test '(3 8 5 4 8 2 4 1 9 4) >=)
    (do-test '(("m" . 13) ("z" . 26) ("a". 1)) ass<=)
    (do-test '(("m" . 13) ("z" . 26) ("a". 1)) ass>=)

    (quit))

;-----------------------------------------------------------------------------

;                          MISCELLANEOUS

;--------------------------------------------------------------------------

; CONCATENATE - return a string from a list of strings

(define (concatenate l sep)
    "Procedure: Return a string obtained by concatenating a list of strings
                separating them by SEP.  This is the counterpart to the
                tokenize procedure.
     Usage: concatenate (list \"a\" \"b\" \"c\")
            concatenate (list \"a\" \"b\" \"c\" \"-*-\")"
    (let* ((d (if (null? sep) " " sep)))

          (define (do-one lst x)
	      (if (null? lst)
		  x
		  (do-one (cdr lst)
			  (if x
			      (sprintf "%s%s%s" x d (car lst))
			      (sprintf "%s" (car lst))))))
	  (do-one l nil)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TOKENIZE - return a list of tokens from S as delimited by DELIM

(define (tokenize s delim)
    "Procedure: Return a list of string tokens from S which are delimited
                by DELIM just as in the C strtok function.  This is the
                counterpart to the concatenate procedure.
     Usage: tokenize \"a b c\" \" \"
            tokenize \"a:b c\" \" :\""
    (let* ((cp (string-copy s)))

          (define (do-one x lst)
	      (let* ((t (strtok x delim)))
		    (if t
			(do-one x (cons t lst))
			(reverse lst))))

	  (do-one cp nil)))

;--------------------------------------------------------------------------
;-----------------------------------------------------------------------------

(define-global (document . rest)
   "Procedure: Print an alphabetized list of all functions in the system
               with their descriptions.
    Usage: document
           document <file>"

   (let* ((x    (list-ref rest 0))
	  (file (if x (open-output-file x) nil)))

         (define-global (do-it x)
	     (let* ((o (string->object x)))
	           (if (procedure? o)
		       (begin (printf file "%s\n" o)
			      (describe o file)
			      (printf file "\n")))))

	 (for-each do-it (hash-dump))
	 (if file
	     (close-output-file file))))

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; ERROR - some error reporting procedure

(define-global (error x) (newline) (display x) (newline) (break))

;-----------------------------------------------------------------------------
;--------------------------------------------------------------------------

; ITEM - helper to extract items from variable arg lists

(define-global (item x n)
    (cond ((and (pair? x) (< n (length x)))
	   (list-ref x n))
	  ((and (vector? x) (< n (vector-length x)))
	   (vector-ref x n))
	  (else
	   nil)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; ITEM->LIST - helper to extract items from variable arg lists
;            - if item is a list return it
;            - if it is an other object make a list containing the item

(define-global (item->list args n)
    (let* ((arg (item args n)))
          (cond ((symbol? arg)
		 (list (sprintf "%s" arg)))
		((pair? arg)
		 arg)
		((vector? arg)
		 (vector->list arg))
		((null? arg)
		 nil)
		(else
		 (list arg)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PAUSE - stop and think

(define-global (pause)
    "Procedure: Stop execution to consider next action.
     Anything but a response starting with 'y' exits the session."
    (printf nil "\nEnter y to proceed: ")
    (define rsp (read))
    (if (or (string? rsp) (symbol? rsp))
	(if (not (string=? (substring rsp 0 1) "y"))
	    (quit))))

;-----------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SCHEME-REPL - Scheme level read/eval/print loop

(define-global (scheme-repl in-prompt out-prefix)
    "Procedure: Run a Scheme level read/eval/print loop.
     Usage: (scheme-repl <in-prompt> <out-prefix>)
        IN-PROMPT is the prompt for input.
        OUT-PREFIX precedes each line of output.
     Example: (scheme-repl \"in> \" \"answer = \")"
    (printf stdout "%s%s\n\n" out-prefix (eval (read stdin in-prompt)))
    (scheme-repl in-prompt out-prefix))

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; AUTOLOAD* - do real autoloading of procedures

(define-global (autoload* funcname filename . quiet)
    "Procedure: loads the specified file when the given procedure is invoked
     and calls the procedure (load on demand).  Works silently if
     <quiet> is #t.
     Usage: autoload* <procedure> <file> [<quiet>]"
    (if quiet
	(apply define-global
	       (list funcname
		     (eval (list lambda 'x
				 (sprintf "Procedure: %s autoload* from %s"
					  funcname filename)
				 (list load filename #t)
				 (list apply (list eval funcname) 'x)))))
	(apply define-global
	       (list funcname
		     (eval (list lambda 'x
				 (sprintf "Procedure: %s autoload* from %s"
					  funcname filename)
				 (list printf nil "Auto-loading %s from %s ... "
				       (symbol->string funcname) filename)
				 (list load filename #t)
				 (list printf nil "done\n")
				 (list apply (list eval funcname) 'x)))))))

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; AUTOLOAD - do real autoloading of macros

(define-global (autoload funcname filename . quiet)
    "Macro: loads the specified file when the given macro is invoked
     and calls the macro (load on demand).  Works silently if
     <quiet> is #t.
     Usage: autoload* <procedure> <file> [<quiet>]"
    (if quiet
	(apply define-global-macro
	       (list funcname
		     (eval (list lambda 'x
				 (sprintf "Macro: %s autoload from %s"
					  funcname filename)
				 (list load filename #t)
				 (list apply (list eval funcname) 'x)))))
	(apply define-global-macro
	       (list funcname
		     (eval (list lambda 'x
				 (sprintf "Macro: %s autoload from %s"
					  funcname filename)
				 (list printf nil "Auto-loading %s from %s ... "
				       (symbol->string funcname) filename)
				 (list load filename #t)
				 (list printf nil "done\n")
				 (list apply (list eval funcname) 'x)))))))

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

(define-global (test-scheme)
   (do ((i 0 (+ i 1))) ((= i 50) (newline)) (display i)))

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

(autoload 'hostname          "host-server.scm" #t)
(autoload 'hosttype          "host-server.scm" #t)
(autoload 'host-server-query "host-server.scm" #t)
(autoload 'host-server-start "host-server.scm" #t)

(autoload 'exec              "exec.scm" #t)
;(autoload 'prime?            "rmath.scm" #t)
;(autoload 'prime-factor      "rmath.scm" #t)
(load "rmath.scm")

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

))

