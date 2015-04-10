;
; CSYNT.SCM - helpers for C syntax mode
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
; 2) When debugging try first showing the translation of a C form into
;    its SCHEME counterpart to determine whether it is the parser or the
;    semantics defined here

(load "lang.scm")

(define -continue- 0)
(define -return-   0)
(define stdout     nil)
(define NULL       nil)
(define TRUE       1)
(define FALSE      0)

;(define -debug- #t)
(define -debug- #f)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DIAGNOSE - print diagnostics from converting parsed C into
;          - properly executing SCHEME code

(define (diagnose . args)
    (if -debug-
	(apply printf (cons nil args))))

;--------------------------------------------------------------------------

;                           DECLARATION ROUTINES

;--------------------------------------------------------------------------

; VARIABLE-IDENTIFIER - given an expression representing a variable
;                     - in the form:
;                     -    (<variable> <value>)
;                     - where:
;                     -    <variable> := (<id> <type-qualifiers>) | <id>
;                     - return <id>, the variable identifier

(define-macro (variable-identifier ve)
    (if (pair? ve)
	(if (eqv? (car ve) 'quote)
	    (caadr ve)
	    (let ((x (list-ref ve 0)))
	         (if (pair? x)
		     (car x)
		     x)))
	ve))

(define (variable-identifier* ve)
    (if (pair? ve)
	(let ((x (list-ref ve 0)))
	     (if (pair? x)
		 (car x)
		 x))
	ve))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; VARIABLE-VALUE - given an expression representing a variable
;                - in the form:
;                -    (<variable> <value>)
;                - return <value>, the variable value

(define-macro (variable-value ve)
    (let ((x (list-ref ve 1)))
         (if (and (pair? x) (not (procedure? (eval (car x)))))
	     (cons 'list x)
	     x)))

(define (variable-value* ve)
    (let ((x (list-ref ve 1)))
         (if (and (pair? x) (not (procedure? (eval (car x)))))
	     (cons 'list x)
	     x)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; C/DECL-SPEC - get and process declaration specification from parser

(define-macro-ev (c/decl-spec kind type members)
   (let* ((ltyp (if (symbol? type)
		    (symbol->string type)
		    type))
	  (typfn (string->symbol (sprintf "define-%s" type))))

         `(declare ,typfn (eval kind) ,ltyp members)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DECLARE - instantiate a declaration which means
;         - return a global function that defines a variable of
;         - the specified type
;         -
;         -   TYPFN   - name of the global function
;         -   KIND    - struct, union, enum, macro, or named
;         -   TYPE    - type name
;         -   MEMBERS - members for derived types

(define-macro-ev (declare typfn kind type members)
   (let* ((proto (cons typfn '(var val)))
	  (body  '(cond ((pair? var)
			 (let* ((expr (list-ref var 1))
				(vr   (variable-identifier* expr))
				(vl   (variable-value* expr)))
                              `(define ,vr ,vl)))
			((symbol? var)
			 `(define ,var ,val)))))

         (if (procedure? kind)
	     (apply kind (list type members)))
	 `(define-macro-ev ,proto ,body)))

;--------------------------------------------------------------------------

;                             C EVALUATION

;--------------------------------------------------------------------------

; SUBST - substitute new variable for old one in the given expression
;       - helper for c/macro

(define (subst-var new old expr)

    (define-macro (subst-var-list x)
        (cond ((pair? x)
	       (subst-var new old (cadr x)))
	      ((eq? x old) new)
	      (else x)))

    (map subst-var-list expr))

(define (subst new old expr)
    (if old
	(subst (cdr new)
	       (cdr old)
	       (subst-var (car new) (car old) expr))
	expr))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; C/DEFUN - define a function complete with the
;         - correct C return semantics

(define-macro-ev (c/defun . dcls)
    (let* ((p     (list-ref dcls 0))
	   (fnc   (list-ref p 0))
	   (argl  (list-tail p 1))
	   (args  (if argl
		      (map variable-identifier argl)
		      nil))
	   (proto (cons fnc args))
	   (body  (list-tail dcls 1)))
          `(define ,proto
	           (call-with-cc (lambda (-return-) ,@body)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; C/MACRO - define a macro complete with the
;         - correct CPP replacement semantics

(define-macro-ev (c/macro proto . body)
   (if (pair? proto)

; procedural macro branch
       (let* ((name (car proto))
	      (namf (string->symbol (sprintf "c/aux-%s" name)))
	      (params (cdr proto))
	      (bdy    (car body))
	      (call   (cons name '_t)))
             `(define-macro ,call
		  (define (,namf vals)
		          (subst vals (quote ,params) (quote ,bdy)))
		  (eval (,namf _t))))

; constant macro branch
       `(define ,proto ,(car body))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MAKE-LEGAL-DECLARATION - make a legal define out of the argument

(define-macro (make-legal-declaration x)
    (if (pair? x)
	(cons 'define (cadr x))
	(list 'define x nil)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DECLARE-VARIABLES - given a good type name
;                   - declare/define the specified C variables

(define (declare-variables type vars)
    (diagnose "-> Declaring %s: %s\n" type vars)
    (let* ((dlst (reverse (map make-legal-declaration vars))))
          `(begin ,@dlst)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; C/ARRAY - define array variables in the
;         - current environment

(define (c/array len)
    (make-vector len))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; C/SCALAR - define scalar variables in the
;          - current environment
;          - Usage:
;          -
;          -   (c/scalar <declaration> <varlist>)
;          -
;          - where
;          -     <declaration> is a function (actually macro) which
;          -                   was returned by DECLARE and does two
;          -                   things: 1) defines the type in whatever
;          -                   fashion this implementation requires;
;          -                   and 2) defines a variable or typename
;          -                   of this type in the calling environment.
;          -                   Yes, this is VERY, VERY tricky - mess
;          -                   with it at your risk - I recommend having
;          -                   a few free weeks before proceding.
;          -                   The reason for the complexity is to
;          -                   abstract out the type handling in such
;          -                   a way that future work on types can
;          -                   be accomplished solely in this function.
;          -                   Part (2) involved adding define-macro-ev
;          -                   and lambda-macro to the SCHEME language
;          -                   in order to get the scope of the definitions
;          -                   to be correct.
;          -                   It is the same problem as the difference
;          -                   in CSH between execing a script and
;          -                   sourcing it.
;          -
;          -     <varlist>     is the list of variables or typenames
;          -                   to be defined in the outer environmetn
;          -
;          -
;          - Example:
;          -    (declare define-int () "int" ())
;          -        defines the type "int" and defines the macro
;          -        define-int in the calling environment which
;          -        will define all variables of type "int"
;          -    (c/scalar define-int a (b 3))
;          -        call define-int to define the variable a and b
;          -        of type "int" with a initialized to '() and
;          -        b initialized to 3

(define-macro-ev (c/scalar dcl . vars)
    (let* ((varl (reverse vars)))
          (if (pair? varl)
	      `(apply begin (map ,dcl ',varl)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; C/STRUCT - handle structs

(define-macro (c/struct name body)
    (if body
	(diagnose "-> struct %s %s\n" name body)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; C/UNION - handle unions

(define-macro (c/union name body)
    (if body
	(diagnose "-> union %s %s\n" name body)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; C/ENUM - handle enums

(define-macro (c/enum name body)
    (if body
	(diagnose "-> enum %s %s\n" name body)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; C/DERIVED - handle derived-type

(define-macro (c/derived . x)
    (diagnose "-> Derived type handling not complete\n\n"))

;--------------------------------------------------------------------------

;                           STORAGE CLASSES

;--------------------------------------------------------------------------

; C/AUTO - handle autos
;        - Usage: (c/auto <decl-func>)

(define-macro (c/auto fnc)
    (fnc))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; C/EXTERN - handle externs
;          - Usage: (c/extern <decl-func>)

(define-macro (c/extern fnc)
    (fnc))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; C/REGISTER - handle registers
;            - Usage: (c/register <decl-func>)

(define-macro (c/register fnc)
    (fnc))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; C/STATIC - handle statics
;          - Usage: (c/static <decl-func>)

(define-macro (c/static fnc)
    (fnc))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; C/TYPEDEF - handle typedefs
;           - takes a defining a declaration FNC
;           - executes FNC and returns a function
;           - which takes a type name and adds it to the named types
;           - Usage: (c/typedef <decl-func>)

(define-macro (add-type x)
    (diagnose "-> Adding new type: %s\n" x)
    (c-add-type x))

(define (c/typedef fnc)
    (fnc)
    add-type)

;--------------------------------------------------------------------------

;                          TYPE QUALIFIERS

;--------------------------------------------------------------------------

; C/CONST - handle consts
;         - Usage: (c/const <decl-func>)

(define-macro (c/const fnc)
    (fnc))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; C/VOLATILE - handle volatiles
;            - Usage: (c/volatile <decl-func>)

(define-macro (c/volatile fnc)
    (fnc))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; C/GOTO - handle goto

(define-macro (c/goto . x)
    (diagnose "-> Goto handling not complete\n\n"))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; C/SET! - handle the an assigment possibly with
;        - array or struct members on the LHS

(define-macro-ev (c/set! expr val)
    (if (pair? expr)
	(let* ((fnc (list-ref expr 0))
	       (arr (list-ref expr 1))
	       (idx (list-ref expr 2)))
	      (cond ((eqv? fnc 'c/[])
		     `(vector-set! ,arr ,idx ,val))
		    ((eqv? fnc 'c/->)
		     `())
		    (else
		     (printf nil
			     "\nUnknown LHS in assignment %s\n"
			     ,expr))))
	`(set! ,expr ,val)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; C/IF - handle the C if which really tests against 0
;      - rather than #t/#f
;      - cases to be handled
;      -   pred is an atomic value (e.g. 1 or n)
;      -        it must a number and its value must be tested against 0
;      -   pred is a form (e.g (eqv? a b))
;      -        its evaluation must be tested against #t/#f
;      - NOTE: pred should only be evaluated once in case of side effects

(define-macro-ev (c/if pred cons alt)
    `(let* ((expr ,pred)
	    (test (cond ((number? expr)
			 (not (eqv? expr 0)))
			(else
			 expr))))
           (if test
	       ,cons
	       ,alt)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; C/[] - handle the reference of an array element

;(define-macro-ev (c/[] arr n)
;    `(vector-ref ,arr ,n))

(define-macro-ev (c/[] arr n)
    `(cond ((vector? ,arr)
	    (vector-ref ,arr ,n))
	   ((pair? ,arr)
	    (list-ref ,arr ,n))
	   (else
	    (printf nil "\nBad object for array dereference %s\n" ,arr))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; C/-> - handle the reference of a struct member

(define (c/-> . x)
    (diagnose "-> Member access not complete\n\n"))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; C/++X - handle the ++x

(define-macro-ev (c/++x expr)
    `(begin (set! ,expr (+ ,expr 1))
	    ,expr))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; C/--X - handle the --x

(define-macro-ev (c/--x expr)
    `(begin (set! ,expr (- ,expr 1))
	    ,expr))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; C/X++ - handle the x++

(define-macro-ev (c/x++ expr)
    `(let* ((temp-var ,expr))
           (set! ,expr (+ ,expr 1))
	   temp-var))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; C/X-- - handle the x--

(define-macro-ev (c/x-- expr)
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
				    'c/label))
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

; C/SWITCH - handle the switch

(define-macro-ev (c/switch expr body)
    (let* ((clauses (switch-body->cond expr (list-tail body 2) nil)))
          `(call-with-cc (lambda (-continue-)
			         (define temp-sym ,expr)
			         (cond ,@clauses)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; C/LABEL - handle the label

(define-macro (c/label val) ())

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; C/SIZEOF - handle the sizeof operator
;          - can probably improve this

(define-macro (c/sizeof expr kind)
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

; C/CAST - handle a cast
;        - can probably improve this

(define (c/cast type src)
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
;--------------------------------------------------------------------------

; C/CAT-VARS - make a new variable by concatenating the names of
;            - the given objects
;            - can probably improve this

(define-macro (c/cat-vars a b)
    (string->symbol (string-append (symbol->string a)
				   (symbol->string b))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; C/STR-VAR - make a new string out of the name of the given variable
;           - can probably improve this

(define-macro (c/str-var a)
    (symbol->string a))

;--------------------------------------------------------------------------

;                       C TRANSLATION FOR SHOW

;--------------------------------------------------------------------------

; TRANSL-C-FUNCTION-DECLARATION - do function translation

(define (transl-c-function-declaration dcls)
    (let* ((proto (list-ref dcls 0))
	   (body  (list-tail dcls 1)))
          (format-expr nil 
		  `(define ,proto
		       (call-with-cc (lambda (-return-) ,@body))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TRANSL-C-TYPEDEF - do a typedef

(define (transl-c-typedef oper tname)
    (apply add-type tname))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TRANSL-C-SCALAR-DECLARATION - do a scalar variable translation

(define (transl-c-scalar-declaration dcls)
    (let* ((op   (car dcls))
	   (dcl  (cdr dcls))
	   (dlst (if dcl (reverse (map make-legal-declaration dcl)))))

          (if (and (pair? op) (eqv? (car op) 'c/typedef))
	      (apply transl-c-typedef (list op dcl)))

          (if dlst
	      (for-each '(lambda (x) (format-expr nil x)) dlst))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TRANSLATE-C-EXPR - translate a C form to SCHEME and display it

(define (translate-c-expr expr)
    (let* ((fnc (list-ref expr 0)))
	  (cond ((eqv? fnc 'c/defun)
		 (transl-c-function-declaration (list-tail expr 1)))

		((eqv? fnc 'c/scalar)
		 (transl-c-scalar-declaration (list-tail expr 1)))

		(else
		 (format-expr nil expr)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TRANSLATE-C-FILE - translate a file of C forms to SCHEME

(define (translate-c-file name)
   (let* ((file (open-input-file name)))

         (c-mode)
	 (printf nil "(define NULL nil)\n")


         (define (rtpl file)
	     (let* ((expr (read file)))
	           (if (not (eof-object? expr))
		       (begin (translate-c-expr expr)
			      (rtpl file)))))

	 (rtpl file)

	 (close-input-file file)

	 (scheme-mode)))

;--------------------------------------------------------------------------

;                              SYNONYMS

;--------------------------------------------------------------------------

(language-synonyms)
