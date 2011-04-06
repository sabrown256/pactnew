;
; PANABGC.SCM - generate a PANACEA combined generator simulation code
;             - from a package dictionary
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GENERATE-AB-MAIN - generate the combined A and B main

(define (generate-ab-main file codename)
    (let* ((init-problem      (system-function "combined-init-problem"))
	   (init-problem-func (eqv? init-problem NULL))
	   (init-problem-name (cond ((pair? init-problem)
				     (declaration-name init-problem))
				    ((eqv? init-problem NULL)
				     "AB_init_problem")
				    (else
				     init-problem)))

	   (run-problem      (system-function "combined-run-problem"))
	   (run-problem-func (eqv? run-problem NULL))
	   (run-problem-name (cond ((pair? run-problem)
				    (declaration-name run-problem))
				   ((eqv? run-problem NULL)
				    "AB_run_problem")
				   (else
				    run-problem)))

	   (fin-problem      (system-function "combined-fin-problem"))
	   (fin-problem-func (eqv? fin-problem NULL))
	   (fin-problem-name (cond ((pair? fin-problem)
				    (declaration-name fin-problem))
				   ((eqv? fin-problem NULL)
				    "AB_fin_problem")
				   (else
				    fin-problem)))

	   (init-par          (declaration-name (system-function "init-parallel")))

	   (unit-hook-name    (declaration-name (system-function
						 "generator-unit-setup")))
	   (term-hook         (declaration-name (system-function
						 "simulation-termination")))
	   (intr-handler-hook (declaration-name (system-function
						 "interrupt-handler")))
	   (reg-hook          (declaration-name (system-function
						 "region-name-to-index")))
	   (db-hook           (declaration-name (system-function "pre-intern")))
	   (domain-hook       (declaration-name (system-function "domain-builder")))
	   (mapping-hook      (declaration-name (system-function "mapping-builder")))
	   (name-hook         (declaration-name (system-function "base-name"))))

      (function-separator file)

      (function-definition file "int" "main" '("int c" "char **v") "top level" "main")

      (printf file "   {double t;\n\n")
    
; GOTCHA: this isn't strictly needed and causes problems on some platforms
;	(printf file "    PG_open_console(\"%s\", \"COLOR\", 1, 0.1, 0.8, 0.8, 0.2);\n"
;		codename)
;	(printf file "\n")
    
      (comment file "set the error return catch")

      (printf file "    SC_init(\"%s: Exit with error\", %s,\n"
	      (up-case codename) term-hook)
      (printf file "            TRUE, (PFSignal_handler) %s,\n"
	      intr-handler-hook)
      (printf file "            TRUE, NULL, 0);\n")
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
	  (printf file "    PA_add_hook(\"region_id\",     %s);\n" reg-hook))
      (if db-hook
	  (printf file "    PA_add_hook(\"setup_region\",  %s);\n" db-hook))
      (if domain-hook
	  (printf file "    PA_add_hook(\"build_domain\",  %s);\n" domain-hook))
      (if mapping-hook
	  (printf file "    PA_add_hook(\"build_mapping\", %s);\n" mapping-hook))
      (printf file "\n")
    
      (generate-hook-install file application-hook-list)
    
      (comment file "define the code system for which we are generating")
      (printf file "    AB_def_system();\n")

      (if (not (strcmp? unit-hook-name "NULL"))
	  (printf file "    %s();\n" unit-hook-name))
    
      (printf file "    PA_inst_com();\n")
      (printf file "    %s(c, v);\n" init-problem-name)
      (printf file "\n")
    
      (printf file "    t = SC_wall_clock_time();\n\n")
      (generate-pan-simulation file run-problem-name)
      (printf file "    t = SC_wall_clock_time() - t;\n")
      (printf file "    PRINT(STDOUT, \"Wall clock time: %%9.3e\\n\", t);\n\n")
    
      (printf file "    %s();\n" fin-problem-name)
    
      (printf file "    PRINT(STDOUT, \"\\n\");\n\n")
    
      (comment file "close the parallel message passing system down")
      (printf file "    PC_close_member(PA_pp);\n\n")
    
      (printf file "    LONGJMP(SC_gs.cpu, ERR_FREE);}\n")
      (printf file "\n")
    
      (function-separator file)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GENERATE-AB-DEF-SYSTEM - generate the combined A and B code def_system

(define (generate-ab-def-system file packages)
    (function-separator file)

    (printf file " \n")
    (comment file "AB_DEF_SYSTEM - define PANACEA packages to combined simulation code")
    (printf file " \n")

    (printf file "static void AB_def_system(void)\n")

    (printf file "   {\n")
    (printf file "\n")

    (define (print-def pck)
        (let* ((fname NULL))

	  (define (print-fnc title . rest)
	    (let* ((function (lookup-by-title title pck))
		   (name (declaration-name function)))
	      (if (and rest (not (eqv? name "NULL")))
		  (printf file "                   %s %s,\n"
			  (car rest) name)
		  (printf file "                   %s,\n" name))))
     
	  (printf file "    PA_def_package(\"%s\",\n"
		  (down-case (package-name pck)))

	  (print-fnc "install-generator-commands" "(PFPkgGencmd)")
	  (print-fnc "install-type-definitions" "(PFPkgDfstrc)")
	  (print-fnc "define-units" "(PFPkgDefun)")
	  (print-fnc "define-variables" "(PFPkgDefvar)")
	  (print-fnc "define-controls" "(PFPkgDefcnt)")
	  (print-fnc "intern-variables" "(PFPkgIntrn)")
	  (print-fnc "initialize" "(PFPkgInizer)")
	  (print-fnc "main-entry" "(PFPkgMain)")
	  (print-fnc "post-process-output" "(PFPkgPpsor)")
	  (print-fnc "finish" "(PFPkgFinzer)")
	  (print-fnc "install-pp-commands" "(PFPkgPpcmd)")

	  (printf file "                   %s);\n" fname)))

    (for-each print-def packages)

    (printf file "\n    return;}\n\n")
    (function-separator file))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GENERATE-AB-INIT-PROBLEM - generate a default init_problem function

(define (generate-ab-init-problem file)
    (let* ((sfname-var (state-file-name))
	   (edname-var (edit-file-name))
	   (ppname-var (pp-file-name))
	   (pvname-var (PVA-file-name)))

      (function-separator file)

      (printf file "\n")
      (comment file "AB_INIT_PROBLEM - initialize this session with an initial value problem")
      (printf file "\n")

      (printf file "static int AB_init_problem(int c, char **v)\n")

      (printf file "   {char *base_name;\n")
      (printf file "\n")
      (printf file "    if (c > 0)\n")
      (printf file "       PA_readh(v[1]);\n")
      (printf file "    else\n")
      (printf file "       {base_name = CSTRSAVE(\"gen\");\n")
      (printf file "        PA_name_files(base_name, &%s, &%s, &%s, &%s);};\n"
	      edname-var sfname-var ppname-var pvname-var)
      (printf file "\n")

      (comment file "write an initial state file")

      (printf file "    PA_wrrstrth();\n")
      (printf file "\n")
      (printf file "    PA_trap_signals(4, SIGSEGV, PA_signal_handler,\n")
      (printf file "                       SIGABRT, PA_signal_handler,\n")
      (printf file "                       SIGBUS,  PA_signal_handler,\n")
      (printf file "                       SIGFPE,  PA_signal_handler);\n")

      (printf file "\n    return(TRUE);}\n\n")
      (function-separator file)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GENERATE-AB - generate the combined A and B code

(define (generate-ab packages codename)
    (let* ((file (open-output-file (sprintf "%s.%s"
					    (down-case codename)
					    target-language-suffix)))

	   (init-problem (system-function "combined-init-problem"))
	   (init-problem-func (eqv? init-problem NULL))

	   (run-problem (system-function "combined-run-problem"))
	   (run-problem-func (eqv? run-problem NULL))

	   (fin-problem (system-function "combined-fin-problem"))
	   (fin-problem-func (eqv? fin-problem NULL)))

      (printf nil "Generating %s combined code ... " codename)

      (comment file "%s.C - PANACEA simulation code"
	       (up-case codename))
      (printf file "\n")

      (printf file "#undef CODE\n")
      (printf file "#define CODE \"%s\"\n" codename)
      (printf file "\n")

      (printf file
	      "#include \"%s.h\"\n"
	      (down-case codename))
      (printf file "\n")

      (function-separator file)

; write the def-system function
      (generate-ab-def-system file packages)

; write the init-problem function if it is not supplied
      (if init-problem-func
	  (generate-ab-init-problem file))

; write the run-problem function if it is not supplied
      (if run-problem-func
	  (generate-run-problem file "AB"))

; write the fin-problem function if it is not supplied
      (if fin-problem-func
	  (generate-fin-problem file "AB"))

; write the main function
      (generate-ab-main file (up-case codename))

      (function-separator file)

      (printf file "\n")
      (close-output-file file)
      (printf nil "done\n")))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

