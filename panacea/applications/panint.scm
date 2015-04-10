;
; PANINT.SCM - PANACHE User Interface Functions
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

;--------------------------------------------------------------------------

;                     ABSTRACTION BARRIERS

;--------------------------------------------------------------------------

; MAKE-PACKAGE - return a list of items defining a PANACEA package
;              -
;              -      name
;              -      functions
;              -      variables
;              -      meshes
;              -      commands
;              -      switches
;              -      parameters
;              -      names
;              -      structs
;              -      casts
;              -      units
;              -      interns
;              -      inits
;              -      completions
;              -      pp
;              -      main
;              -      pp-commands
;              -      declared-functions
;              -      defined-constants
;              -      included-headers
;              -      aliases

(define make-package list)

;
; package accessor procedures
;

(define (package-name pck)         (list-ref pck 0))
(define (package-procedures pck)   (list-ref pck 1))
(define (package-variables pck)    (list-ref pck 2))
(define (package-meshes pck)       (list-ref pck 3))
(define (package-commands pck)     (list-ref pck 4))
(define (package-switches pck)     (list-ref pck 5))
(define (package-parameters pck)   (list-ref pck 6))
(define (package-names pck)        (list-ref pck 7))
(define (package-structs pck)      (list-ref pck 8))
(define (package-casts  pck)       (list-ref pck 9))
(define (package-units pck)        (list-ref pck 10))
(define (package-interns pck)      (list-ref pck 11))
(define (package-inits pck)        (list-ref pck 12))
(define (package-completions pck)  (list-ref pck 13))
(define (package-pp pck)           (list-ref pck 14))
(define (package-main pck)         (list-ref pck 15))
(define (package-pp-commands pck)  (list-ref pck 16))
(define (package-declarations pck) (list-ref pck 17))
(define (package-defines pck)      (list-ref pck 18))
(define (package-headers pck)      (list-ref pck 19))
(define (package-aliases pck)      (list-ref pck 20))

;
; variable accesor procedures
;

(define (variable-name var)                 (list-ref var 0))
(define (variable-type var)                 (list-ref var 1))
(define (variable-meshes var)               (list-ref var 2))
(define (variable-attributes var)           (list-ref var 3))
(define (variable-units var)                (list-ref var 4))
(define (variable-initial-value var)        (list-ref var 5))
(define (variable-initializer-function var) (list-ref var 6))
(define (variable-data-pointer var)         (list-ref var 7))

;
; MAKE-DECLARATION - return a list of items defining a C function
;                  - or variable declaration

(define (make-declaration title class type name init args special)
    (if (not declarations-table)
        (set! declarations-table (make-hash-table 127)))
    (let* ((decl (append (list title class type name init special)
			 args)))

      (if (not (and (eqv? title "variable")
		    (or (eqv? name "swtch") (eqv? name 'swtch)
			(eqv? name "name") (eqv? name 'name)
			(eqv? name "param") (eqv? name 'param))))
	  (hash-install name decl declarations-table))
      decl))

(define (lookup-declaration name)
    (if declarations-table
	(hash-lookup name declarations-table)))

;
; declaration accessor procedures
;

(define (declaration-title fnc)     (list-ref fnc 0))
(define (declaration-class fnc)     (list-ref fnc 1))
(define (declaration-type fnc)      (list-ref fnc 2))
(define (declaration-name fnc)      (if (eqv? fnc NULL)
					NULL
					(list-ref fnc 3)))
(define (declaration-init fnc)      (list-ref fnc 4))
(define (declaration-special fnc)   (list-ref fnc 5))
(define (declaration-arg-list fnc)  (list-tail fnc 5))
(define (declaration-arguments fnc) (list-tail fnc 6))

;
; MAKE-DEFINE - return a list of items defining a C macro definition
;

(define (make-define name value)
    (list name value))

;
; #define accessor procedures
;

(define (define-name def)   (list-ref def 0))
(define (define-value def)  (list-ref def 1))

;
; MAKE-COMMAND - return a list of items defining an installable command
;

(define (make-command name func-declaration hand-declaration)
    (set! cmmnd-list (cons (list name func-declaration hand-declaration)
			   cmmnd-list)))

(define (command-name cmd) (list-ref cmd 0))
(define (command-func cmd) (list-ref cmd 1))
(define (command-hand cmd) (list-ref cmd 2))

;
; system variable accessors
;

(define (time-variable)               (list-ref system-scalar-list 0))
(define (start-time-variable)         (list-ref system-scalar-list 1))
(define (stop-time-variable)          (list-ref system-scalar-list 2))
(define (time-step-fraction)          (list-ref system-scalar-list 3))
(define (minimum-time-step-fraction)  (list-ref system-scalar-list 4))
(define (maximum-time-step-fraction)  (list-ref system-scalar-list 5))
(define (time-step-increase-fraction) (list-ref system-scalar-list 6))
(define (cycle-variable)              (list-ref system-scalar-list 7))
(define (number-of-zones)             (list-ref system-scalar-list 8))
(define (state-file-name)             (list-ref system-file-list 0))
(define (edit-file-name)              (list-ref system-file-list 1))
(define (pp-file-name)                (list-ref system-file-list 2))
(define (PVA-file-name)               (list-ref system-file-list 3))
	  
;
; MAKE-HANDLER - return a list of items defining a C function handler
;

(define handlers-table nil)

(define (make-handler class type name args)
    (if (not handlers-table)
        (set! handlers-table (make-hash-table 5)))
    (let* ((decl (append (list class type name) args)))
      (hash-install name decl handlers-table)
      decl))

(define (lookup-handler name)
    (if handlers-table
	(if (pair? name)
	    (hash-lookup (handler-name name) handlers-table)
	    (hash-lookup name handlers-table))))

(define (handler-name hand)
    (list-ref hand 2))

;
; Pre-defined handlers
;

(define PA_zargs (make-handler
		     "extern"
		     "void"
		     "PA_zargs"
		     (list "PA_command *cp")))

(define PA_sargs (make-handler
		     "extern"
		     "void"
		     "PA_sargs"
		     (list "PA_command *cp")))

(define PA_strarg (make-handler
		      "extern"
		      "void"
		      "PA_strarg"
		      (list "PA_command *cp")))

;--------------------------------------------------------------------------

;                       SYSTEM LEVEL ROUTINES

;--------------------------------------------------------------------------

; SYSTEM-SCALARS - define the code system wide
;                - control variable (e.g. time/timestep variables)
;                - NOTE: this information must be provided!

(define-macro (system-scalars t
			      tstart
			      tstop
			      dtf
			      dtfn
			      dtfx
			      dtfi
			      cycle
			      nzones)
    (set! system-scalar-list (list t
				   tstart
				   tstop
				   dtf
				   dtfn
				   dtfx
				   dtfi
				   cycle
				   nzones)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SYSTEM-FILES - define the code system wide
;              - file names: state file, edit file,
;              - pp file, and PVA file
;              - NOTE: this information must be provided!

(define-macro (system-files sfname
			    edname
			    ppname
			    pvname)
    (set! system-file-list (list sfname
				 edname
				 ppname
				 pvname)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; INSTALL_FUNCT - add a list of function specification to the given hook list

(define (install-funct name fnc raw-lst hook-lst)
   (cond ((and name fnc (pair? raw-lst))
	  (set! hook-lst
		(cons (make-declaration
		       name
		       "extern"
		       "int"
		       fnc 
		       #f
		       (list "PA_package *pck")
		       #f)
		      hook-lst))
	  (set! hook-lst (install-funct (car raw-lst) (cadr raw-lst)
					(cddr raw-lst) hook-lst)))

	 ((and name fnc)
	  (set! hook-lst
		(cons (make-declaration
		       name
		       "extern"
		       "int"
		       fnc 
		       #f
		       (list "PA_package *pck")
		       #f)
		      hook-lst)))

	 ((pair? raw-lst)
	  (set! hook-lst (install-funct (car raw-lst) (cadr raw-lst)
					(cddr raw-lst) hook-lst)))

	 (else
	  (printf nil "ERROR: Bad arguments to system-functions\n")
	  (quit)))

   hook-lst)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; APPLICATION-FUNCTIONS - define the miscellaneous application
;                       - wide hook functions

(define-macro (application-functions . rest)
    (set! application-hook-list
	  (install-funct nil nil rest application-hook-list)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SYSTEM-FUNCTIONS - define the code system wide
;                  - hook functions

(define-macro (system-functions . rest)
    (set! system-hook-list (install-funct nil nil rest system-hook-list)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SYSTEM-FUNCTION - return the function associated with the given name

(define (system-function name)
    (let* ((fnc (assv name system-hook-list)))
      (if fnc
	  fnc
	  NULL)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SYSTEM-FUNCTION? - return #t iff the specified system function is defined

(define (system-function? x)
    (not (eqv? (system-function x) "NULL")))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; COMPLETE-SYSTEM-FUNCTIONS? - return #t iff there is a complete set
;                            - of system functions defined

(define (complete-system-functions?)
    (let* ((a-init-prob (system-function? "generator-init-problem"))
	   (b-init-prob (system-function? "simulation-init-problem"))
	   (b-run-prob  (system-function? "simulation-run-problem"))
	   (b-fin-prob  (system-function? "simulation-fin-problem"))
	   (c-init-prob (system-function? "combined-init-problem"))
	   (c-run-prob  (system-function? "combined-run-problem"))
	   (c-fin-prob  (system-function? "combined-fin-problem")))
      (or (and a-init-prob b-init-prob b-run-prob b-fin-prob)
	  (and c-init-prob c-run-prob c-fin-prob))))

;--------------------------------------------------------------------------

;                         PACKAGE LEVEL ROUTINES

;--------------------------------------------------------------------------

; PACKAGE-FUNCTIONS

(define-macro (package-functions . rest)
    "Macro: Associate a compiled function name with a formal package funtion.
     These are added to the current package function list
     so that declarations may be emitted in the package header.
     The formal package functions are:
          install-generator-commands
          install-type-definitions
          intern-variables
          define-units
          define-variables
          define-controls
          initialize
          main-entry
          post-process-output
          finish
          install-pp-commands
     Usage: (package-functions [<formal-name> <coded-name>]+)"

    (define (add-function name fnc)
        (set! function-list
	      (cons (make-declaration
		     name
		     "extern"
		     "int"
		     fnc 
		     #f
		     (list "PA_package *pck")
		     #f)
		    function-list)))

    (define (install-funct name fnc lst)
        (cond ((and name fnc (pair? lst))
	       (add-function name fnc)
	       (install-funct (car lst) (cadr lst) (cddr lst)))

	      ((and name fnc)
	       (add-function name fnc))

	      ((pair? lst)
	       (install-funct (car lst) (cadr lst) (cddr lst)))

	      (else
               (printf nil "ERROR: Bad arguments to package-functions\n")
	       (quit))))

    (install-funct nil nil rest))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PACKAGE - define a package

(define (package name)
    (push-package)
    (printf nil "Defining package %s ... " name)
    (set! current-package-name name))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TYPEDEF - define a type

(define-macro (typedef name . members)

  (apply define-global
	 (list name (list 'list
			  (symbol->string name)
			  (cons 'list members))))
  (set! struct-list (cons name struct-list)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; CAST - define a cast

(define-macro (cast type member contrl)
  (set! cast-list (cons (list type member contrl) cast-list)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MESH-VAR-DECLS - handle declaration for mesh definition scalars

(define-macro (mesh-var-decls vname)
    (if (not (lookup-declaration vname))
	(make-declaration "variable"
			  "implicit-extern"
			  "int"
			  vname
			  #f
			  nil
			  #f)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MESH - define a mesh for the package

(define-macro (mesh . rest)
    (if rest
	(let* ((mname (list-ref rest 0))
	       (vname (list-ref rest 1)))

	  (if (pair? vname)
	      (for-each mesh-var-decls vname)
	      (apply mesh-var-decls (list vname)))

	  (set! mesh-list (cons (list mname vname) mesh-list))
	  (apply mesh (list-tail rest 2)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; COMMAND - define a generator command for the package

(define-macro (command . rest)
    (if rest
	(let* ((cname  (list-ref rest 0))
	       (first  (list-ref rest 1))
	       (second (list-ref rest 2))
	       (func   (lookup-declaration first))
	       (hand   (lookup-handler second)))

; handle case of declared function
; prototype - name func hand
	  (cond (func
		 (if second
		     (let* ((hand (lookup-handler second)))
		       (make-command cname
				     (cdr func)
				     (if hand
					 hand
					 PA_zargs))
		       (apply command (list-tail rest 3)))
		     (printf nil "ERROR: Bad function (%s)\n" second)))

; handle case of undeclared function
; prototype - name func hand
		(hand
		 (let* ((fname first)
			(hname (handler-name (cdr hand)))
			(index (cond ((eqv? hname "PA_zargs")
				      "void")
				     ((eqv? hname "PA_sargs")
				      "PA_command *cp")
				     ((eqv? hname "PA_strarg")
				      "char *s"))))
		   (make-command cname
				 (make-declaration
				  "function"
				  "extern"
				  "void"
				  fname
				  #f
				  (list index)
				  #f)
				 hand))
		 (apply command (list-tail rest 3)))

; handle case of array element command
; prototype - name type variable index
		(else
		 (let* ((type first)
			(name (symbol->string second))
			(index (list-ref rest 3))
			(decl (make-declaration
			       "variable"
			       nil
			       type
			       name
			       #f
			       (list index)
			       #f)))

		   (make-command cname
				 decl
				 nil)

		   (apply command (list-tail rest 4))))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DECLARE-FUNCTION - declare a function

(define-macro (declare-function . rest)
    (if rest
	(let* ((type (list-ref rest 0))
	       (name (list-ref rest 1))
	       (args (list-ref rest 2)))
	  (make-declaration "function"
			    "extern"
			    type
			    name
			    #f
			    args
			    #f)
	  (apply declare-function (list-tail rest 3)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DECLARE-C-FUNCTION - declare a function to be extern "C" {} if generated
;                    - in a C++ environment

(define-macro (declare-C-function . rest)
    (if rest
	(let* ((type (list-ref rest 0))
	       (name (list-ref rest 1))
	       (args (list-ref rest 2)))
	  (make-declaration "function"
			    "extern"
			    type
			    name
			    #f
			    args
			    "C")
	  (apply declare-C-function (list-tail rest 3)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TYPESPEC? - return #t iff x has something to do with type specifications

(define (typespec? x)
   (or (memv x '("const" "static" "extern"))
       (memv x '("char" "short" "int" "integer" "long" "REAL" "float" "double" "void"))
       (memv x struct-list)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DECLARE-VARIABLE - declare a variable

(define-macro (declare-variable . rest)
    (if rest
	(let* ((s (list-ref rest 0))
	       (t (list-ref rest 1))
	       (u (list-ref rest 2)))
	  (cond ((or (strcmp? s "static")
		     (strcmp? s "extern"))
		 (let* ((type t)
			(name u)
			(dims (list-ref rest 3))
			(init (list-ref rest 4)))
		   (make-declaration "variable"
				     s
				     type
				     name
; GOTCHA: init can legally be a number or a #defined constant
;         we should check both cases
				     init
;					     (if (number? init)
;						 init
;						 #f)
				     dims
				     #f)
		   (if (typespec? init)
		       (apply declare-variable
			      (list-tail rest 4))
		       (apply declare-variable
			      (list-tail rest 5)))))

		((strcmp? s "const")
		 (let* ((name t)
			(value u))
		   (set! defines-list
			 (cons (make-define
				name
				value)
			       defines-list))
		   (apply declare-variable
			  (list-tail rest 3))))

		(else
		 (let* ((type s)
			(name t)
			(dims u)
			(init (list-ref rest 3)))
		   (make-declaration "variable"
				     "implicit-extern"
				     type
				     name
				     (if (number? init)
					 init
					 #f)
				     dims
				     #f)
		   (if (number? init)
		       (apply declare-variable
			      (list-tail rest 4))
		       (apply declare-variable
			      (list-tail rest 3)))))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DEF-VAR - define one or more variables for the package

(define-macro (def-var name type . rest)
    (let* ((mesh     (list-ref rest 0))
	   (attr     (list-ref rest 1))
	   (units    (list-ref rest 2))
	   (init-val (list-ref rest 3))
	   (init-fnc (list-ref rest 4))
	   (data     (list-ref rest 5)))
      (set! var-list
	    (cons (list name
			type
			mesh
			attr
			units
			init-val
			init-fnc
			data)
		  var-list))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DEF-SCALAR - define one or more scalars which are also generator commands
;            - NOTE: this handles the declaration, definition, and
;            - installation as variable and command

(define-macro (def-scalar . rest)
   (if rest
       (let* ((type (list-ref rest 0))
	      (name (list-ref rest 1))
	      (init (if (> (length rest) 2)
			(list-ref rest 2))))
	 (if (number? init)
	     (let* ((vlname (sprintf "%s_val" name)))
	       (apply declare-variable
		      (list "static" type vlname nil init
			    type name nil))
	       (apply def-var
		      (list name type nil '(requ static) nil
			    vlname nil name))
	       (apply def-scalar (list-tail rest 3)))
	     (begin
	       (apply declare-variable (list type name nil))
	       (apply def-var
		      (list name type nil '(requ static) nil
			    'N_zero nil name))
	       (apply def-scalar (list-tail rest 2)))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; INCLUDE - specify files to be included by package headers

(define-macro (include . rest)
    (set! headers-list (append rest headers-list)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; ALIAS - specify aliases for the current package

(define-macro (alias . rest)
    (set! alias-list (append rest alias-list)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; CONTROL - add a control to the package control list

(define (control . rest)
    (if rest
	(let* ((type  (list-ref rest 0))
	       (index (list-ref rest 1))
	       (value (list-ref rest 2))
	       (doc   (list-ref rest 3)))
	  (cond ((strcmp? type integer)
		 (set! switch-list
		       (cons (list index value doc)
			     switch-list)))
		((strcmp? type real)
		 (set! param-list
		       (cons (list index value doc)
			     param-list)))
		((strcmp? type ascii)
		 (set! name-list
		       (cons (list index value doc)
			     name-list))))
	  (apply control (list-tail rest 4)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------
