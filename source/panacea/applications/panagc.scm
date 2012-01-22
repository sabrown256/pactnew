;
; PANAGC.SCM - generate a PANACEA generator code
;            - from a package dictionary
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GENERATE-A-MAIN - generate the generator code A main

(define (generate-a-main file codename)
    (let* ((init-problem      (system-function "generator-init-problem"))
	   (init-problem-func (eqv? init-problem NULL))
	   (init-problem-name (cond ((pair? init-problem)
				     (declaration-name init-problem))
				    ((eqv? init-problem NULL)
				     "A_init_problem")
				    (else
				     init-problem)))

	   (init-par          (declaration-name (system-function "init-parallel")))

	   (unit-hook-name    (declaration-name (system-function
						 "generator-unit-setup")))
	   (term-hook         (declaration-name (system-function
						 "generator-termination")))
	   (gen-error-hook    (declaration-name (system-function
						 "generator-error-handler")))
	   (intr-handler-hook (declaration-name (system-function
						 "interrupt-handler")))
	   (reg-hook          (declaration-name (system-function
						 "region-name-to-index")))
	   (db-hook           (declaration-name (system-function "pre-intern")))
	   (name-hook         (declaration-name (system-function "base-name"))))

      (function-separator file)

      (function-definition file "int" "main" '("int c" "char **v") "top level" "main")
    
      (printf file "   {\n")
      (printf file "\n")
    
      (printf file "    PG_open_console(\"%s\", \"COLOR\", 1, 0.1, 0.8, 0.8, 0.2);\n"
	      codename)
      (printf file "\n")
    
      (comment file "set the error return catch")

      (printf file "    SC_init(\"%s: Exit with error\", %s,\n"
	      codename term-hook)
      (printf file "          TRUE, %s, NULL,\n" intr-handler-hook)
      (printf file "          TRUE, NULL, 0);\n")
      (printf file "\n")
    
      (if (and init-par (not (strcmp? init-par 'NULL)))
	  (begin (comment file "do initializations for parallel processing")
		 (printf file "    %s(c, v);\n\n" init-par)))
    
      (comment file "print the banner")

      (printf file "    SC_set_banner(\" %%s  -  %%s\\n\\n\", CODE, VERSION);\n")
      (printf file "    SC_banner(\"\");\n")
      (printf file "\n")
    
      (comment file "connect function pointers to initial functions")

      (if name-hook
	  (printf file "    PA_add_hook(\"base_name\",     %s);\n" name-hook))
      (if reg-hook
	  (printf file "    PA_add_hook(\"region_id\",    %s);\n" reg-hook))
      (if db-hook
	  (printf file "    PA_add_hook(\"setup_region\", %s);\n" db-hook))
      (printf file "\n")
    
      (generate-hook-install file application-hook-list)
    
      (comment file "define the code system for which we are generating")
      (printf file "    A_def_system();\n")
    
      (if (not (strcmp? unit-hook-name "NULL"))
	  (printf file "    %s();\n" unit-hook-name))
    
      (printf file "    PA_inst_com();\n")
      (printf file "    %s(c, v);\n" init-problem-name)
      (printf file " \n")
    
      (comment file "dispatch on commands from the deck")
      (printf file "    PA_get_commands(stdin, %s);\n" gen-error-hook)
      (printf file "\n")
    
      (comment file "close the parallel message passing system down")
      (printf file "    PC_close_member(PA_gs.pp);\n\n")
    
      (printf file "    LONGJMP(SC_gs.cpu, ERR_FREE);}\n")
      (printf file "\n")
    
      (function-separator file)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GENERATE-A-DEF-SYSTEM - generate the generator code A def_system

(define (generate-a-def-system file packages)
    (function-separator file)

    (printf file "\n")
    (comment file "A_DEF_SYSTEM - define PANACEA packages to generator code")
    (printf file "\n")

    (printf file "static void A_def_system(void)\n")

    (printf file "   {\n")
    (printf file "\n")

    (define (print-def pck)
        (let* ((fname NULL))

	  (define (print-fnc title)
	    (let* ((function (lookup-by-title title pck))
		   (name (declaration-name function)))
	      (printf file "                   %s,\n" name)))
     
	  (printf file "    PA_gen_package(\"%s\",\n"
		  (down-case (package-name pck)))
	  (print-fnc "install-generator-commands")
	  (print-fnc "install-type-definitions")
	  (print-fnc "define-units")
	  (print-fnc "define-variables")
	  (print-fnc "define-controls")
	  (print-fnc "intern-variables")
	  (printf file "                   %s);\n" fname)))

    (for-each print-def packages)

    (printf file "\n    return;}\n\n")
    (function-separator file))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GENERATE-A-INIT-PROBLEM - generate a default init_problem function

(define (generate-a-init-problem file)
    (let* ((edname-var (edit-file-name))
	   (sfname-var (state-file-name))
	   (ppname-var (pp-file-name))
	   (pvname-var (PVA-file-name)))

      (function-separator file)

      (printf file "\n")
      (comment file "A_INIT_PROBLEM - initialize this session with an initial value problem")
      (printf file "\n")

      (printf file "static int A_init_problem(int c, char **v)\n")

      (printf file "   {char *base_name;\n")
      (printf file "\n")
      (printf file "    if (c > 0)\n")
      (printf file "       PA_readh(v[1]);\n")

      (printf file "    else\n")
      (printf file "       {base_name = CSTRSAVE(\"gen\");\n")
      (printf file "        PA_name_files(base_name, &%s, &%s, &%s, &%s);};\n"
	      edname-var sfname-var ppname-var pvname-var)

      (printf file "\n")
      (printf file "    PA_trap_signals(4, SIGSEGV, PA_signal_handler,\n")
      (printf file "                       SIGABRT, PA_signal_handler,\n")
      (printf file "                       SIGBUS,  PA_signal_handler,\n")
      (printf file "                       SIGFPE,  PA_signal_handler);\n")

      (printf file "\n    return(TRUE);}\n\n")
      (function-separator file)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GENERATE-A - generate the generator code A

(define (generate-a packages syscodename codename)
    (let* ((file (open-output-file (sprintf "%s.%s"
					    (down-case codename)
					    target-language-suffix)))
	   (init-problem (system-function "generator-init-problem"))
	   (init-problem-func (eqv? init-problem NULL)))

      (printf nil "Generating %s generator code ... " codename)
      (comment file "%s.C - PANACEA generator code"
	       (up-case codename))
      (printf file "\n")
 
      (printf file "#undef CODE\n")
      (printf file "#define CODE \"%s\"\n" codename)
      (printf file "\n")

      (printf file
	      "#include \"%s.h\"\n"
	      (down-case syscodename))
      (printf file "\n")

      (generate-main-declarations file "A")

      (function-separator file)

; write the def-system function
      (generate-a-def-system file packages)

; write the init-problem function if it is not supplied
      (if init-problem-func
	  (generate-a-init-problem file))

; write the main function
      (generate-a-main file codename)

      (function-separator file)

      (printf file "\n")
      (close-output-file file)
      (printf nil "done\n")))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------
