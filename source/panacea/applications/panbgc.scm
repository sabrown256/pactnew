;
; PANBGC.SCM - generate a PANACEA simulation code in C
;            - from a package dictionary
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GENERATE-B-ARG-HANDLER

(define (generate-b-arg-handler file codename)
    (printf file "#ifdef MAC\n")
    (printf file "\n")

    (printf file "    PRINT(stdout, \"State File Name: \");\n")
    (printf file "    GETLN(s, MAXLINE, stdin);\n")
    (printf file "    strtok(s, \"\\n\\r\");\n")
    (printf file "\n")

    (printf file "#else\n")
    (printf file "   \n")

    (printf file "    if (c < 2)\n")
    (printf file "       {PRINT(stdout, \"\\nUsage: %s [-p] <restart-file-name>\\n\\n\");\n"
	    (down-case codename))
    (printf file "        return(1);};\n")
    (printf file "\n")

    (printf file "#endif\n")
    (printf file "\n"))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GENERATE-B-MAIN - generate the simulation code B main

(define (generate-b-main file codename)
    (let* ((init-problem      (system-function "simulation-init-problem"))
	   (init-problem-func (eqv? init-problem NULL))
	   (init-problem-name (cond ((pair? init-problem)
				     (declaration-name init-problem))
				    ((eqv? init-problem NULL)
				     "B_init_problem")
				    (else
				     init-problem)))

	   (init-par          (declaration-name (system-function "init-parallel")))

	   (run-problem       (system-function "simulation-run-problem"))
	   (run-problem-func  (eqv? run-problem NULL))
	   (run-problem-name  (cond ((pair? run-problem)
				     (declaration-name run-problem))
				    ((eqv? run-problem NULL)
				     "B_run_problem")
				    (else
				     run-problem)))

	   (fin-problem      (system-function "simulation-fin-problem"))
	   (fin-problem-func (eqv? fin-problem NULL))
	   (fin-problem-name (cond ((pair? fin-problem)
				    (declaration-name fin-problem))
				   ((eqv? fin-problem NULL)
				    "B_fin_problem")
				   (else
				    fin-problem)))
	   (intr-handler-hook (declaration-name (system-function
						 "interrupt-handler")))
	   (domain-hook  (declaration-name (system-function "domain-builder")))
	   (mapping-hook (declaration-name (system-function "mapping-builder")))
	   (exit-hook    (declaration-name (system-function
					    "simulation-termination"))))
      (function-separator file)

      (function-definition file "int" "main" '("int c" "char **v") "top level" "main")

      (printf file "   {double t;\n\n")

					; GOTCHA: this isn't strictly needed and causes problems on some platforms
					;	(printf file "    PG_open_console(\"%s\", \"MONOCHROME\", 1, 0.1, 0.1, 0.8, 0.8);\n"
					;		(up-case codename))
					;	(printf file "\n")
    
      (generate-b-arg-handler file (up-case codename))
    
      (printf file "    SC_zero_space_n(1, -2);\n")

      (printf file "    SC_init(\"%s: Exit with error\", %s,\n"
	      (up-case codename) exit-hook)
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
    
      (comment file "define the code system by setting up the packages")

      (printf file "    PA_add_hook(\"build_domain\",  %s);\n" domain-hook)
      (printf file "    PA_add_hook(\"build_mapping\", %s);\n" mapping-hook)
      (printf file "\n")

      (generate-hook-install file application-hook-list)
    
      (printf file "    B_def_system();\n")
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

; GENERATE-B-DEF-SYSTEM - generate the simulation code B def_system

(define (generate-b-def-system file packages)
    (function-separator file)

    (printf file "\n")
    (comment file "B_DEF_SYSTEM - define PANACEA packages to simulation code")
    (printf file "\n")

    (printf file "static void B_def_system(void)\n")

    (printf file "   {\n")
    (printf file "\n")

    (define (print-def pck)
        (let* ((fname NULL))

	  (define (print-fnc title)
	    (let* ((function (lookup-by-title title pck))
		   (name (declaration-name function)))
	      (printf file "                        %s,\n" name)))

	  (printf file "    PA_run_time_package(\"%s\",\n"
		  (down-case (package-name pck)))

	  (print-fnc "install-type-definitions")
	  (print-fnc "define-units")
	  (print-fnc "define-variables")
	  (print-fnc "define-controls")
	  (print-fnc "initialize")
	  (print-fnc "main-entry")
	  (print-fnc "post-process-output")
	  (print-fnc "finish")

	  (printf file "                        %s);\n" fname)))

    (for-each print-def packages)

    (printf file "\n    return;}\n\n")
    (function-separator file))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GENERATE-B-INIT-PROBLEM - generate a default init_problem function

(define (generate-b-init-problem file)
    (let* ((sfname-var (state-file-name)))

      (function-separator file)

      (printf file "\n")
      (comment file "B_INIT_PROBLEM - initialize this session with an initial value problem")
      (printf file "\n")

      (printf file "static int B_init_problem(int c, char **v)\n")

      (printf file "   {int i;\n")
      (printf file "    char *fname;\n")
      (printf file "\n")

      (comment file "process the command line arguments")
      (printf file "    fname = v[c-1];\n")
      (printf file "\n")

      (printf file "    if (fname != NULL)\n")
      (printf file "       {PA_rd_restart(fname, NONE);\n")
      (printf file "        %s = CSTRSAVE(fname);};\n"
	      sfname-var)

      (printf file "\n")
      (printf file "    PA_trap_signals(4, SIGSEGV, PA_signal_handler,\n")
      (printf file "                       SIGABRT, PA_signal_handler,\n")
      (printf file "                       SIGBUS,  PA_signal_handler,\n")
      (printf file "                       SIGFPE,  PA_signal_handler);\n")

      (printf file "\n    return(TRUE);}\n\n")
      (function-separator file)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GENERATE-B - generate the simulation code B

(define (generate-b packages codename)
    (let* ((file (open-output-file (sprintf "%s.%s"
					    (down-case codename)
					    target-language-suffix)))

	   (init-problem (system-function "simulation-init-problem"))
	   (init-problem-func (eqv? init-problem NULL))

	   (run-problem (system-function "simulation-run-problem"))
	   (run-problem-func (eqv? run-problem NULL))

	   (fin-problem (system-function "simulation-fin-problem"))
	   (fin-problem-func (eqv? fin-problem NULL)))

      (printf nil "Generating %s simulation code ... " codename)

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

      (generate-main-declarations file "B")

      (function-separator file)

; write the def-system function
      (generate-b-def-system file packages)

; write the init-problem function if it is not supplied
      (if init-problem-func
	  (generate-b-init-problem file))

; write the run-problem function if it is not supplied
      (if run-problem-func
	  (generate-run-problem file "B"))

; write the fin-problem function if it is not supplied
      (if fin-problem-func
	  (generate-fin-problem file "B"))

; write the main function
      (generate-b-main file (up-case codename))

      (function-separator file)

      (printf file "\n")
      (close-output-file file)
      (printf nil "done\n")))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------
