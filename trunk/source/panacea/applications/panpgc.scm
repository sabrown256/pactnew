;
; PANPGC.SCM - generate C coded PANACEA package definition functions
;            - from a package dictionary
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; FUNCTION-SEPARATOR - generate a function separator line

(define (function-separator file)
    (cond ((eqv? target-language 'C)
	   (printf file
		   "/*--------------------------------------------------------------------------*/\n"))
	  ((eqv? target-language 'C++)
	   (printf file
		   "//--------------------------------------------------------------------------\n"))
	  ((eqv? target-language 'F77)
	   (printf file
		   "c --------------------------------------------------------------------------\n"))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; FUNCTION-DEFINITION - generate the function definition code

(define (function-definition file type fncname args pckname unitname)

    (define (print-args args)
        (if (pair? args)
	    (cond ((eqv? target-language 'C)
		   (let* ((decl (parse-declaration (car args)))
			  (rest (cdr args))
			  (name (list-ref decl 1)))
		     (if rest
			 (begin (printf file "%s, " name)
				(print-args rest))
			 (printf file "%s" name))))
		  ((eqv? target-language 'C++)
		   (let* ((decl (car args))
			  (rest (cdr args)))
		     (if rest
			 (begin (printf file "%s, " decl)
				(print-args rest))
			 (printf file "%s" decl))))
		  ((eqv? target-language 'F77)
		   (let* ((decl (parse-declaration (car args)))
			  (rest (cdr args))
			  (name (list-ref decl 1)))
		     (if rest
			 (begin (printf file "      %s" name)
				(print-args rest))
			 (printf file "       %s" name)))))))

    (define (print-def arg)
        (printf file "   %s;\n" arg))

    (printf file "\n")
    (comment file "%s - define %s for %s package"
	    (up-case fncname)
	    unitname
	    (up-case pckname))
    (printf file "\n")

    (printf file "%s %s(" type (down-case fncname))

    (print-args args)

    (printf file ")\n")
    (if (and args (not (eqv? target-language 'C++)))
	(for-each print-def args)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; FUNCTION-RETURN - generate the function return code

(define (function-return file pck)
    (if pck
	(printf file "    pck = NULL;\n\n"))
    (printf file
	    "    return(TRUE);}\n\n"))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MESH->DIM-LIST - map the mesh specifications into a list of dimensions

(define (mesh->dim-list msh meshes)

    (define (proc-aux-map lst str vrs)
	(if vrs
	    (proc-aux-map lst
			  (sprintf "%s&%s, "
				   str
				   (car vrs))
			  (cdr vrs))
	    (proc-aux lst str)))

    (define (proc-aux lst str)
	(if (null? lst)
	    str
	    (let* ((vars (car lst)))
	      (if (pair? vars)
		  (proc-aux-map (cdr lst) str vars)
		  (proc-aux (cdr lst)
			    (sprintf "%s&%s, "
				     str
				     vars))))))

    (if msh
	(proc-aux (cdr (assoc msh meshes)) "")
	""))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PROC-ATTR-LIST - process the attribute list of the variable

(define (proc-attr-list lst)
    (define (proc-aux lst str)
	(if (null? lst)
	    str
	    (proc-aux (cdr lst)
		      (sprintf "%s%s, "
			       str
			       (eval (car lst))))))
    (proc-aux lst ""))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PROC-UNIT-LIST - process the unit list specification

(define (proc-unit-list lst)
    (define (proc-aux lst str)
	(if (null? lst)
	    str
	    (proc-aux (cdr lst)
		      (sprintf "%s%s, "
			       str
			       (up-case (symbol->string (car lst)))))))
    (if lst
	(proc-aux lst "")
	""))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PROC-INIT-VAL - process the initial value specification

(define (proc-init-val val)
    (if (null? val)
	NULL
	(sprintf "&%s" val)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PROC-INIT-FNC - process the initializer function specification

(define (proc-init-fnc fnc)
    (if (null? fnc)
	NULL
	(sprintf "%s" val)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DO-VARIABLE - print out the PA_def_var for a variable

(define (do-variable file x meshes)
    (let* ((var-name     (variable-name x))
	   (var-type     (sprintf "\"%s\""
				  (variable-type x)))
	   (var-dims     (mesh->dim-list (variable-meshes x)
					 meshes))
	   (var-attrs    (proc-attr-list
			  (variable-attributes x)))
	   (var-units    (proc-unit-list
			  (variable-units x)))
	   (var-init-val (proc-init-val
			  (variable-initial-value x)))
	   (var-init-fnc (proc-init-fnc
			  (variable-initializer-function x)))
	   (var-ptr      (variable-data-pointer x)))

      (define-macro (print-def name)
	(printf file "    PA_def_var(\"%s\", %s, %s, %s,\n"
		name
		var-type
		var-init-val
		var-init-fnc)
	(printf file "               %sATTRIBUTE,\n"
		var-attrs)
	(printf file "               %sDIMENSION,\n"
		var-dims)
	(printf file "               %sUNITS"
		var-units)

	(if var-ptr
	    (printf file ",\n               &%s);\n"
		    var-ptr)
	    (printf file ");\n")))

      (if (pair? var-name)
	  (for-each print-def var-name)
	  (apply print-def (list var-name)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DO-ALIASES - handle aliases

(define (do-aliases file aliases)
    (if aliases
	(let* ((name (list-ref aliases 0))
	       (type (list-ref aliases 1))
	       (val  (list-ref aliases 2))
	       (rest (list-tail aliases 3)))
	  (printf file "    PA_DEF_ALIAS(\"%s\", %s, \"%s\", %s);\n"
		  name type type val)
	  (do-aliases file rest))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GENERATE-DEFVAR - generate the package defvar routine code

(define (generate-defvar package file)
    (let* ((vars    (package-variables package))
	   (meshes  (package-meshes package))
	   (aliases (package-aliases package)))
      (if (or vars aliases)
	  (let* ((pckname  (package-name package))
		 (function (lookup-by-title "define-variables"
					    package))
		 (fncname  (declaration-name function))
		 (type     (declaration-type function))
		 (args     (declaration-arguments function)))

	    (define (print-variable x) (do-variable file x meshes))

	    (function-separator file)
	    (function-definition file type
				 fncname args pckname
				 "variables")

	    (printf file "   {\n")

	    (if vars
		(begin (if (pair? vars)
			   (for-each print-variable vars)
			   (print-variable vars))
		       (printf file "\n")))

	    (if aliases
		(begin (do-aliases file aliases)
		       (printf file "\n")))

	    (function-return file #t)
	    (function-separator file)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GENERATE-DEFCNT - generate the package defcnt routine

(define (generate-defcnt package file)
    (define switches (package-switches package))
    (define params   (package-parameters package))
    (define names    (package-names package))
    (if (or switches params names)
        (let* ((pckname  (package-name package))
	       (function (lookup-by-title "define-controls" package))
	       (fncname  (declaration-name function))
	       (type     (declaration-type function))
	       (args     (declaration-arguments function)))

; DO-CONTROL
	  (define (do-control x lst)
	    (printf file "    %s[%d] \t= %s;\t\t"
		    x
		    (car lst)
		    (cadr lst))
	    (comment file "%s"
		     (caddr lst)))

	  (define (max-length lst val)
	    (if lst
		(max-length (cdr lst) (max val (caar lst)))
		val))

	  (function-separator file)

	  (function-definition file type fncname args pckname "controls")

	  (printf file
		  "   {static int n_names = %d, n_params = %d, n_swtches = %d;\n\n"
		  (max-length names 0)
		  (max-length params 0)
		  (max-length switches 0))
	  (printf file
		  "    PA_mk_control(pck, \"%s\", n_names, n_params, n_swtches);\n\n"
		  (down-case pckname))

	  (if names
	      (for-each (lambda (lst) (do-control "NAME" lst)) names))
	  (printf file "\n")

	  (if params
	      (for-each (lambda (lst) (do-control "PARAM" lst)) params))
	  (printf file "\n")

	  (if switches
	      (for-each (lambda (lst) (do-control "SWTCH" lst)) switches))
	  (printf file "\n")

	  (if (strcmp? pckname "global")
	      (begin
		(printf file "    global_swtch = SWTCH;\n")
		(printf file "    global_param = PARAM;\n")
		(printf file "    global_name  = NAME;\n")
		(printf file "\n")))

	  (function-return file #f)
	  (function-separator file))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GENERATE-GENCMD - generate the package gencmd routine

(define (generate-gencmd package file)
    (define cmd (package-commands package))
    (if cmd
        (let* ((pckname  (package-name package))
	       (function (lookup-by-title "install-generator-commands" package))
	       (fncname  (declaration-name function))
	       (type     (declaration-type function))
	       (args     (declaration-arguments function)))

; DO-CMMND
	  (define (do-cmmnd x)
	    (let* ((name (command-name x))
		   (func (command-func x))
		   (hand (command-hand x)))
	      (if hand
		  (let* ((fname (declaration-name func))
			 (hname (handler-name (cdr hand))))
		    (printf file
			    "    PA_inst_c(\"%s\", NULL, FALSE, 0, (PFVoid) %s, (PFPanHand) %s);\n"
			    name
			    fname
			    hname))
		  (let* ((stype (declaration-type func))
			 (type (cond ((or (strcmp? stype "int") (strcmp? stype "integer"))
				      "SC_INTEGER_I")
				     ((or (strcmp? stype "real") (strcmp? stype "double"))
				      "SC_DOUBLE_I")
				     ((or (strcmp? stype "char *") (strcmp? stype "string"))
				      "SC_CHAR_I")
				     (else
				      (printf nil "ERROR: Bad type (%s %s %s) in COMMAND\n\n"
					      stype func x))))
			 (var  (declaration-name func))
			 (indx (car (declaration-arguments func))))
		    (printf file
			    "    PA_inst_c(\"%s\", %s, %s, %d, (PFVoid) PA_pshand, (PFPanHand) PA_sargs);\n"
			    name
			    var
			    type
			    indx)))))

	  (function-separator file)
	  (function-definition file type fncname args pckname "commands")

	  (printf file "   {\n")
	  (for-each do-cmmnd cmd)
	  (printf file "\n")

	  (function-return file #f)
	  (function-separator file))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GENERATE-DEFSTR - generate the package defstr routine

(define (generate-defstr package file)
    (let* ((str (package-structs package))
	   (cast (package-casts package)))
      (if str
	  (let* ((pckname  (package-name package))
		 (function (lookup-by-title "install-type-definitions" package))
		 (fncname  (declaration-name function))
		 (fncncom  (up-case (sprintf "%s" fncname)))
		 (type     (declaration-type function))
		 (args     (declaration-arguments function)))

	    (define (generate-typedef-error file type fnc)
	      (printf file "    PA_ERR((pd == NULL),\n           \"%%s")
	      (printf file "\\nCAN\'T DEFINE %s - %s\", PD_err);\n\n"
		      type
		      fnc))

	    (define (legal-type memb)
	      "Return #t iff the item is OK for a PD_typedef"
	      (not (or (strchr memb "(")
		       (strchr memb ")")
		       (strchr memb "[")
		       (strchr memb "]"))))

; DO-STRCT
	    (define (do-strct x)
	      (let* ((name (car x))
		     (members (cadr x)))
		(if (> (length members) 1)
		    (begin

		      (define (do-member memb)
			(printf file "                   \"%s\",\n"
				memb))

		      (printf file
			      "    pd = PD_defstr(file, \"%s\",\n"
			      name)
		      (for-each do-member members)
		      (printf file "                   LAST);\n")
		      (generate-typedef-error file name fncncom))
		    (let* ((memb (list-ref members 0))
			   (cmmb (sprintf "%s" memb))
			   (otype (strtok cmmb " \t\n"))
			   (ntype (strtok cmmb " \t\n")))
		      (if (legal-type memb)
			  (begin (printf file
					 "    pd = PD_typedef(file, \"%s\", \"%s\");\n"
					 otype ntype)
				 (generate-typedef-error file name fncncom)))))))

					; DO-CAST
	    (define (do-cast x)
	      (let* ((type (list-ref x 0))
		     (member (list-ref x 1))
		     (cast-member (list-ref x 2)))

		(printf file
			"    PD_cast(file, \"%s\", \"%s\", \"%s\");\n"
			type
			member
			cast-member)))

	    (function-separator file)
	    (function-definition file type fncname args pckname "types")

	    (printf file "   {defstr *pd ;\n\n");
	    (for-each do-strct str)
	    (if cast
		(begin (for-each do-cast cast)
		       (printf file "\n")))

	    (function-return file #f)
	    (function-separator file)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GENERATE-INCLUDE - emit #include's

(define (generate-include file hdr local)
    (if local
	(printf file "#include \"%s\"\n" hdr)
	(printf file "#include <%s>\n" hdr)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GENERATE-DEFINE - emit #define's

(define (generate-define file def)
    (printf file "#define %s\t%s\n"
	    (define-name def)
	    (define-value def)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GENERATE-STRUCT-MEMBERS - emit struct members

(define (generate-struct-members file str)
    (if (pair? str)
	(let* ((member (car str))
	       (rest (cdr str)))
	  (if rest
	      (begin (printf file "%s;\n    " member)
		     (generate-struct-members file rest))
	      (printf file "%s;" member)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GENERATE-STRUCT - emit structs

(define (generate-struct file str)
    (let* ((name   (list-ref str 0))
	   (strlst (list-ref str 1)))
      (if (> (length strlst) 1)
	  (begin (printf file "\nstruct s_%s\n" name)
		 (printf file "   {")
		 (generate-struct-members file strlst)
		 (printf file "};\n\n")))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GENERATE-TYPEDEF - emit typedef's

(define (generate-typedef file str)
    (let* ((name (list-ref str 0))
	   (strlst (list-ref str 1)))
      (if (= (length strlst) 1)
	  (let* ((type (list-ref strlst 0)))
	    (printf file "typedef %s;\n" type))
	  (printf file "typedef struct s_%s %s;\n" name name))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GENERATE-PROTO-ARGS - emit prototype arguments

(define (generate-proto-args file args)
    (if args
	(let* ((rest (cdr args)))
	  (printf file "%s" (car args))
	  (if rest
	      (printf file ", "))
	  (generate-proto-args file rest))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GENERATE-FUNC-DEF - emit function prototypes

(define (generate-func-def file fnc)
    (let* ((title (declaration-title fnc))
	   (name (declaration-name fnc))
	   (type (declaration-type fnc))
	   (spec (declaration-special fnc))
	   (args (declaration-arguments fnc)))

      (if (not (strcmp? title "variable"))
	  (if (and spec (eqv? target-language 'C++))
	      (begin
		(printf file "extern \"C\" { %s %s(" type name)
		(if args
		    (generate-proto-args file args)
		    (printf file "void"))
		(printf file "); }\n"))
	      (begin
		(printf file "extern %s %s(" type name)
		(if args
		    (generate-proto-args file args)
		    (printf file "void"))
		(printf file ");\n"))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GENERATE-VAR-DECL - emit variable declarations

(define (generate-var-decl file decl)
    (let* ((title (declaration-title decl))
	   (name  (declaration-name decl))
	   (type  (declaration-type decl))
	   (class (declaration-class decl)))

      (cond ((and (strcmp? title "variable")
		  (strcmp? class "extern"))
	     (printf file "extern %s %s;\n" type name))

	    ((and (strcmp? title "variable")
		  (not (strcmp? class "static")))
	     (if (not (or (strcmp? name "SWTCH")
			  (strcmp? name "PARAM")
			  (strcmp? name "NAME")))
		 (printf file "EXTERN %s %s;\n" type name))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GENERATE-PACKAGE-HEADER - generate the header file for the package

(define (generate-package-header pck interactive)
    (let* ((name   (package-name pck))
	   (funcs  (package-procedures pck))
	   (meshes (package-meshes pck))
	   (strcts (package-structs pck))
	   (decls  (package-declarations pck))
	   (defs   (package-defines pck))
	   (hdrs   (package-headers pck))
	   (fname  (sprintf "%s.h" (down-case name)))
	   (file   (open-output-file fname)))

; define the Curry'd versions of the code generators
      (define-macro (print-include inc) (generate-include file inc #t))
      (define (print-define def) (generate-define file def))
      (define (print-struct str) (generate-struct file str))
      (define (print-typedef str) (generate-typedef file str))
      (define (print-proto-args args) (generate-proto-args file args))
      (define (print-func-def fnc) (generate-func-def file fnc))
      (define (print-var-decl decl) (generate-var-decl file decl))

; generate the header coding
      (printf nil "Generating header %s ... " fname)

      (comment file "%s.H - support for %s package"
	       (up-case name) name)
      (printf file "\n")

      (printf file "#ifndef PCK_%s\n\n" name)
      (printf file "#define PCK_%s\n\n" name)

      (if interactive
	  (generate-include file "sx.h" #f)
	  (generate-include file "panace.h" #f))
      (if hdrs
	  (for-each print-include hdrs))
      (printf file "\n")

      (printf file "#undef EXTERN\n")
      (printf file "#ifdef DEFINE_%s\n" name)
      (printf file "#define EXTERN\n")
      (printf file "#else\n")
      (printf file "#define EXTERN extern\n")
      (printf file "#endif\n")
      (printf file "\n")

      (function-separator file)
      (printf file "\n")
      (comment file "                            MACRO DEFINITIONS                           ")
      (printf file "\n")
      (function-separator file)
      (printf file "\n")
      (if defs
	  (for-each print-define defs))
      (printf file "\n")

      (function-separator file)
      (printf file "\n")
      (comment file "                           STRUCT DEFINITIONS                           ")
      (printf file "\n")
      (function-separator file)
      (printf file "\n")
      (if strcts
	  (begin (for-each print-typedef strcts)
		 (for-each print-struct strcts)))

      (function-separator file)
      (printf file "\n")
      (comment file "                          VARIABLE DECLARATIONS                         ")
      (printf file "\n")
      (function-separator file)
      (printf file "\n")

      (if decls
	  (for-each print-var-decl decls))
      (printf file "\n")

      (function-separator file)
      (printf file "\n")
      (comment file "                          FUNCTION DECLARATIONS                         ")
      (printf file "\n")
      (function-separator file)
      (printf file "\n")

;            (for-each print-func-def funcs)
      (printf file "\n")
      (if decls
	  (for-each print-func-def decls))
      (printf file "\n")

      (printf file "#endif\n\n")

      (close-output-file file)
      (printf nil "done\n")))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GENERATE-PACKAGE - generate code for single package

(define (generate-package pck interactive)
    (if generate-package?
	(begin
	   (let* ((name (package-name pck))
		  (decls (package-declarations pck))
		  (fname (sprintf "%s.%s"
				  name
				  target-language-suffix))
		  (file (open-output-file fname)))

; PRINT-VAR-DECL
	     (define (print-var-decl decl)
	       (let* ((title (declaration-title decl))
		      (name  (declaration-name decl))
		      (type  (declaration-type decl))
		      (init  (declaration-init decl))
		      (class (declaration-class decl)))
		 (if (and (strcmp? title "variable")
			  (strcmp? class "static"))
		     (if init
			 (printf file "%s %s %s = %s;\n"
				 class type name init)
			 (printf file "%s %s %s;\n"
				 class type name)))))

	     (printf nil "Generating package %s ... " fname)

	     (comment file "%s.C - PANACEA wrapper code for %s package"
		      (up-case name)
		      name)
	     (printf file "\n")

	     (printf file "#define DEFINE_%s\n\n" name)
	     (printf file "#include \"%s.h\"\n" (down-case name))
	     (printf file "\n")

	     (up-case name)

	     (if decls
		 (begin (for-each print-var-decl decls)
			(printf file "\n")))

	     (function-separator file)
	     (generate-defvar pck file)
	     (generate-defcnt pck file)
	     (generate-gencmd pck file)
	     (generate-defstr pck file)
	     (function-separator file)
	     (printf file "\n")

	     (close-output-file file)
	     (printf nil "done\n")

	     (generate-package-header pck interactive)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GENERATE-NAMED-PACKAGE - generate code for single package

(define-macro (generate-named-package name lang . rest)
    (if generate-package?
	(begin
	   (set! target-language lang)
	   (cond ((eqv? target-language 'C)
		  (set! target-language-suffix 'c))
		 ((eqv? target-language 'C++)
		  (set! target-language-suffix 'C))
		 ((eqv? target-language 'F77)
		  (set! target-language-suffix 'f)))

	   (define (lookup name lst)
	       (let* ((pck (list-ref lst 0))
		      (rest (list-tail lst 1)))
		 (if (string=? name (package-name pck))
		     pck
		     (lookup name rest))))

	   (let* ((interactive (and rest (list-ref rest 0)))
		  (pck (lookup name pck-list)))
	     (generate-package pck interactive))

	   (printf nil "\n"))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GENERATE-CURRENT-PACKAGE - generate code for single package

(define-macro (generate-current-package lang . rest)
    (if generate-package?
	(begin
	   (set! target-language lang)
	   (cond ((eqv? target-language 'C)
		  (set! target-language-suffix 'c))
		 ((eqv? target-language 'C++)
		  (set! target-language-suffix 'C))
		 ((eqv? target-language 'F77)
		  (set! target-language-suffix 'f)))

	   (push-package)

	   (let* ((interactive (and rest (list-ref rest 0))))
	     (generate-package (car pck-list) interactive))

	   (printf nil "\n"))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------
