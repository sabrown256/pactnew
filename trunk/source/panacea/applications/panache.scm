;
; PANACHE.SCM - generate C coded PANACEA package definition functions
;             - from a package dictionary
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(define true 1)
(define false 0)

(define NULL     "NULL")

(define generate-package? #t)
(define extern-C-list nil)

(define target-language 'C)
(define target-language-suffix 'c)

(define double   "double")
(define real     "double")
(define integer  "int")
(define ascii    "char *")
(define string   "char *")

(define restart     "SCOPE, RESTART")
(define runtime     "SCOPE, RUNTIME")
(define demand      "SCOPE, DMND")
(define edit        "SCOPE, EDIT")
(define defn        "SCOPE, DEFN")

(define requ        "CLASS, REQU")
(define optl        "CLASS, OPTL")
(define pseudo      "CLASS, PSEUDO")

(define keep        "PERSIST, KEEP")
(define cache       "PERSIST, CACHE")
(define cache-f     "PERSIST, CACHE_F")
(define cache-r     "PERSIST, CACHE_R")
(define rel         "PERSIST, REL")

(define static      "ALLOCATION, STATIC")
(define dynamic     "ALLOCATION, DYNAMIC")

(define zone        "CENTER, Z_CENT")
(define node        "CENTER, N_CENT")
(define edge        "CENTER, E_CENT")
(define face        "CENTER, F_CENT")
(define un-centered "CENTER, U_CENT")

(define system-scalar-list nil)
(define system-file-list nil)
(define system-hook-list nil)
(define application-hook-list nil)

(define current-package-name nil)

(define pck-list nil)
(define function-list nil)
(define var-list nil)
(define mesh-list nil)
(define cmmnd-list nil)
(define switch-list nil)
(define param-list nil)
(define name-list nil)
(define struct-list nil)
(define cast-list nil)
(define unit-list nil)
(define intern-list nil)
(define init-list nil)
(define fin-list nil)
(define pp-list nil)
(define inst-pp-list nil)
(define main-list nil)
(define declarations-table nil)
(define defines-list nil)
(define headers-list nil)
(define alias-list nil)

;--------------------------------------------------------------------------

;                                 HELPERS

;--------------------------------------------------------------------------

; STRCMP? - a "smart" string comparison predicate ala C standard library

(define (strcmp? s1 s2)
    (let* ((t1 (string-copy s1))
	   (t2 (string-copy s2)))
      (eqv? t1 t2)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; AUTOLOAD* - do real autoloading of procedures

(define (autoload* funcname filename)
    "Procedure: loads the specified file when the given procedure is invoked
     and calls the procedure (load on demand).
     Usage: autoload* <procedure> <file>"
    (apply define-global
           (list (cons funcname 'x)
;                (list printf nil "Autoloading %s ... " filename)
                 (list load filename #t)
;                (list printf nil "done\n")
                 (list apply (list eval funcname) 'x))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; AUTOLOAD - do real autoloading of macros

(define (autoload funcname filename)
    "Macro: loads the specified file when the given macro is invoked
     and calls the macro (load on demand).
     Usage: autoload <macro> <file>"
    (apply define-global-macro
           (list (cons funcname 'x)
                 (list load filename #t)
                 (list apply (list eval funcname) 'x))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; COMPLETE-FUNCTION-LIST - fill in the list of package functions as follows:
;                        - if a function name has not been provided and
;                        - their is code to be generated cons a computed
;                        - name onto the function-list

(define (complete-function-list)
    (let* ((abbrev (package-abbreviation current-package-name)))

      (define (check title lst type suffix . arg)
	(let* ((proc (assv title function-list)))
	  (if proc
	      (let* ((name (declaration-name proc)))
		(set-cdr! proc
			  (cdr (make-declaration title
						 "extern"
						 type
						 name
						 #f
						 arg
						 #f))))
	      (if lst
		  (set! function-list
			(cons (make-declaration title
                                                "extern"
                                                type
                                                (sprintf "%s_%s"
                                                         abbrev
                                                         suffix)
                                                #f
                                                arg
						#f)
			      function-list))))))

      (check "install-generator-commands"
	     cmmnd-list "int" "cmd")
      (check "install-type-definitions"
	     struct-list "int" "str" "PDBfile *file")
      (check "intern-variables"
	     intern-list "int" "intrn" "void")
      (check "define-units"
	     unit-list "int" "dun" "PA_package *pck")
      (check "define-variables"
	     var-list "int" "var" "PA_package *pck")
      (check "define-controls"
	     (or switch-list param-list name-list)
	     "int" "cnt" "PA_package *pck")
      (check "initialize"
	     init-list "int" "init" "PA_package *pck")
      (check "main-entry"
	     main-list "int" "main" "PA_package *pck")
      (check "post-process-output"
	     pp-list "PM_mapping *" "pp" "PA_plot_request *" "double t")
      (check "finish"
	     fin-list "int" "fin" "PA_package *pck")
      (check "install-pp-commands"
	     inst-pp-list "int" "inst_pp" "PA_package *pck")))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HASH->LIST - make a list of the elements of the specified hash table

(define (hash->list tab)
   (define (hash-element x) (cdr (hash-lookup x tab)))
   (map hash-element (hash-dump tab)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PUSH-PACKAGE

(define (push-package)
    (if (or function-list var-list mesh-list cmmnd-list
            switch-list param-list name-list)
        (begin (complete-function-list)
               (set! pck-list
                     (cons (make-package current-package-name
                                         (reverse function-list)
                                         (reverse var-list)
                                         (reverse mesh-list)
                                         (reverse cmmnd-list)
                                         (reverse switch-list)
                                         (reverse param-list)
                                         (reverse name-list)
                                         (reverse struct-list)
                                         (reverse cast-list)
                                         (reverse unit-list)
                                         (reverse intern-list)
                                         (reverse init-list)
                                         (reverse fin-list)
                                         (reverse pp-list)
                                         (reverse main-list)
                                         (reverse inst-pp-list)
					 (hash->list declarations-table)
                                         (reverse defines-list)
                                         (reverse headers-list)
					 alias-list)
                           pck-list))
               (printf nil "done\n"))
        (printf nil "\n"))
    (set! current-package-name nil)
    (set! function-list nil)
    (set! var-list nil)
    (set! mesh-list nil)
    (set! cmmnd-list nil)
    (set! switch-list nil)
    (set! param-list nil)
    (set! name-list nil)
    (set! struct-list nil)
    (set! cast-list nil)
    (set! unit-list nil)
    (set! intern-list nil)
    (set! init-list nil)
    (set! fin-list nil)
    (set! pp-list nil)
    (set! main-list nil)
    (set! inst-pp-list nil)
    (set! declarations-table nil)
    (set! defines-list nil)
    (set! headers-list nil)
    (set! alias-list nil))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PACKAGE-ABBREVIATION

(define (package-abbreviation name)
    (let* ((full-char-lst (string->list (down-case name)))
	   (char-lst (list (car full-char-lst)
			   (cadr full-char-lst)
			   (caddr full-char-lst))))
      (list->string char-lst)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; LOOKUP-BY-TITLE - return the item associated with the given list
;                 - association by title member
;                 - list should be replaced by hash table

(define (lookup-by-title title lst)
    (let* ((obj (assv title (package-procedures lst))))
      (if obj
	  obj
	  NULL)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; LOOKUP-BY-NAME - return the item associated with the name from the list
;                - association by name member
;                - list should be replaced by hash table

(define (lookup-by-name name lst)
    (if (and lst (not (pair? name)))
        (let* ((first (car lst))
	       (rest (cdr lst)))
	  (if (strcmp? name (declaration-name first))
	      first
	      (lookup-by-name name rest)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(load "panint.scm")

;--------------------------------------------------------------------------

;                        CODE GENERATION ROUTINES

;--------------------------------------------------------------------------

; COMMENT - emit a comment

(define (comment file fmt . args)
    "COMMENT - emit a comment in the target language"
    (let* ((text (cond ((eqv? target-language 'C)
			(sprintf "/* %s */\n"
				 (apply sprintf (cons fmt args))))
		       ((eqv? target-language 'C++)
			(sprintf "// %s\n"
				 (apply sprintf (cons fmt args))))
		       ((eqv? target-language 'F77)
			(sprintf "c %s\n"
				 (apply sprintf (cons fmt args)))))))
      (printf file text)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; EXTERN-C - declare a non-C++ package in a C++ code

(define-macro (extern-C pname)
    (set! extern-C-list (cons (symbol->string pname) extern-C-list)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GENERATE-HOOK-INSTALL - install requested hooks

(define (generate-hook-install file spec)

   (if (pair? spec)
       (begin (define (do-hook x)
		 (printf file "    PA_add_hook(\"%s\", %s);\n" 
			 (declaration-title x) (declaration-name x)))

	      (for-each do-hook spec)
	      (printf file "\n"))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GENERATE-SYS-HEADER - generate the system header

(define (generate-sys-header pck-list interactive codename)
    (let* ((file (open-output-file (sprintf "%s.h"
					    (down-case codename)))))
      (define (print-include pck method)
	(let* ((name (package-name pck)))
	  (if (method name extern-C-list)
	      (printf file "#include <%s.h>\n" (down-case name)))))

      (define (not-memv x lst) (not (memv x lst)))

      (define (do-extern-C file)
	(for-each (lambda (x) (print-include x not-memv)) pck-list)
	(printf file "\nextern \"C\" {\n")
	(for-each (lambda (x) (print-include x memv)) pck-list)
	(printf file "}\n"))

      (printf nil "Generating %s header ... " codename)
      (comment file
	       "%s.H - support for %s simulation code"
	       (up-case codename) codename)
      (printf file "\n")

      (if interactive
	  (printf file "#include <sx.h>\n\n")
	  (printf file "#include <panace.h>\n\n"))

      (if extern-C-list
	  (do-extern-C file)
	  (for-each (lambda (x) (print-include x list)) pck-list))

      (printf file "\n")
      (close-output-file file)
      (printf nil "done\n")))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GENERATE-MAIN-DECLARATIONS - generate the def-system declarations

(define (generate-main-declarations file id)
    (cond ((eqv? target-language 'C)
	   (printf file "static void\n")
	   (printf file " %s_def_system(void);\n\n" id))
	  ((eqv? target-language 'C++)
	   (printf file "static void\n")
	   (printf file " %s_def_system(void);\n\n" id))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GENERATE-PAN-SIMULATION - generate PANACEA driven simulation

(define (generate-pan-simulation file run-problem-name)
    (printf file "    %s();\n\n" run-problem-name))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GENERATE-RUN-PROBLEM - generate a default run_problem function

(define (generate-run-problem file id)
    (let* ((t-var      (time-variable))
	   (tstart-var (start-time-variable))
	   (tstop-var  (stop-time-variable))
	   (dtf-var    (time-step-fraction))
	   (dtfn-var   (minimum-time-step-fraction))
	   (dtfx-var   (maximum-time-step-fraction))
	   (dtfi-var   (time-step-increase-fraction))
	   (cycle-var  (cycle-variable))
	   (nzones-var (number-of-zones))
	   (sfname-var (state-file-name))
	   (edname-var (edit-file-name))
	   (ppname-var (pp-file-name))
	   (pvname-var (PVA-file-name)))

      (function-separator file)

      (printf file "\n")
      (comment file
	       "%s_RUN_PROBLEM - run the initial value problem problem"
	       id)
      (printf file "\n")

      (cond ((eqv? target-language 'C)
	     (printf file "static int %s_run_problem()\n" id))
	    ((eqv? target-language 'C++)
	     (printf file "static int %s_run_problem(void)\n" id)))

      (printf file "   {\n\n")
      (comment file "initialize the code system")

      (printf file "    PA_init_system(%s, 0.0, %s, %s, %s, %s);\n"
	      t-var cycle-var edname-var ppname-var pvname-var)
      (printf file "\n")
      (comment file "run the simulation")
      (printf file "    PA_simulate(%s, %s, %s, %s, %s,\n"
	      t-var cycle-var nzones-var tstart-var tstop-var)
      (printf file "                %s, %s, %s, %s,\n"
	      dtf-var dtfn-var dtfx-var dtfi-var)
      (printf file "                %s, %s, %s, %s);\n"
	      sfname-var edname-var ppname-var pvname-var)

      (printf file "\n    return(TRUE);}\n\n")
      (function-separator file)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GENERATE-FIN-PROBLEM - generate a default fin_problem function

(define (generate-fin-problem file id)
    (let* ((t-var      (time-variable))
	   (tstart-var (start-time-variable))
	   (tstop-var  (stop-time-variable))
	   (dtf-var    (time-step-fraction))
	   (cycle-var  (cycle-variable))
	   (sfname-var (state-file-name))
	   (edname-var (edit-file-name))
	   (ppname-var (pp-file-name))
	   (pvname-var (PVA-file-name)))

      (function-separator file)

      (printf file "\n")
      (comment file
	       "%s_FIN_PROBLEM - finish up the run of the initial value problem"
	       id)
      (printf file "\n")

      (cond ((eqv? target-language 'C)
	     (printf file "static int %s_fin_problem()\n" id))
	    ((eqv? target-language 'C++)
	     (printf file "static int %s_fin_problem(void)\n" id)))

      (printf file "   {char nm[MAXLINE], *pnm;\n")
      (printf file "\n")

      (printf file "    PA_terminate(%s, %s, %s, 0);\n"
	      edname-var ppname-var pvname-var)
      (printf file "\n")

      (comment file "transpose the time history files")
      (printf file "    strcpy(nm, %s);\n", ppname-var)
      (printf file "    pnm = strtok(nm, \".\");\n")
      (printf file "    PA_th_transpose(pnm, 1000);\n")
      (printf file "\n")

      (printf file "    PRINT(stdout, \"\\n\");\n")
      (printf file "\n")

      (comment file "advance the names of the edit, pp, and PVA files so the next run")
      (comment file "won't clobber them")
      (printf file " \n")

      (printf file "    PA_advance_name(%s);\n" edname-var)
      (printf file "    PA_advance_name(%s);\n" ppname-var)
      (printf file "    PA_advance_name(%s);\n" pvname-var)
      (printf file "\n")

      (comment file "write a restart dump before ending")

      (printf file "    %s = dt/(ABS(%s - %s) + SMALL);\n"
	      dtf-var tstop-var tstart-var)
      (printf file "    %s = t;\n" t-var)
      (printf file "    %s = cycle;\n" cycle-var)
      (printf file "\n")
      (printf file "    PA_wr_restart(%s);\n" sfname-var)

      (printf file "\n    return(TRUE);}\n\n")
      (function-separator file)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GENERATE - code generation controller

(define-macro (generate lang . rest)
    (set! target-language lang)
    (cond ((eqv? target-language 'C)
	   (set! target-language-suffix 'c))
	  ((eqv? target-language 'C++)
	   (set! target-language-suffix 'C))
	  ((eqv? target-language 'F77)
	   (set! target-language-suffix 'f)))

    (if (or (and system-scalar-list system-file-list)
	    (complete-system-functions?))
	(let* ((genpckp     (list-ref rest 0))
	       (interactive (list-ref rest 1))
	       (separate    (and rest (= (length rest) 4)))
	       (acodename   (if rest
				(symbol->string (list-ref rest 2))
				"A"))
	       (bcodename    (if rest
				 (if separate
				     (symbol->string (list-ref rest 3))
				     (symbol->string (list-ref rest 2)))
				 "B")))
          
	  (push-package)
	  (set! pck-list (reverse pck-list))

	  (if genpckp
	      (begin (printf nil "\n")
		     (for-each (lambda (x)
				 (generate-package x interactive))
			       pck-list)
		     (printf nil "\n")))

	  (if separate
	      (begin
		(generate-a pck-list
			    bcodename
			    acodename)
		(generate-b pck-list
			    bcodename))
	      (generate-ab pck-list bcodename))

	  (generate-sys-header pck-list interactive bcodename)

	  (printf nil "\n"))
        (printf nil "\nERROR: system-scalars and system-files call required\n\n")))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; COMPILE - code compilation controller

(define (compile invoke)
     (system "make -i system"))
;    (let* ((msg (sprintf "%s -o %s.c" invoke (down-case current-package-name))))
;            (printf nil "%s\n" msg)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(autoload* 'generate-package         "panpgc.scm")
(autoload  'generate-current-package "panpgc.scm")
(autoload  'generate-named-package   "panpgc.scm")

(autoload* 'generate-b       "panbgc.scm")
(autoload* 'generate-a       "panagc.scm")
(autoload* 'generate-ab      "panabgc.scm")

