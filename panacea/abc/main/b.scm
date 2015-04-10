; 
; B.SCM - run B the smart way
; 
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(define off 0)
(define on 1)

(define solid 1)
(define dashed 2)
(define dotted 3)
(define dotdashed 4)

(define black         0)
(define blue          1)
(define green         2)
(define cyan          3)
(define red           4)
(define magenta       5)
(define brown         6)
(define white         7)
(define gray          8)
(define lght_blue     9)
(define lght_green   10)
(define lght_cyan    11)
(define lght_red     12)
(define yellow       13)
(define lght_magenta 14)
(define lght_white   15)

(define cartesian -1)
(define polar -2)
(define insel -3)

(define contour -1)
(define wire-frame -2)
(define shaded -3)
(define image -4)
(define vector -5)

(define internal->cgs -20)
(define internal->external -21)
(define external->cgs -22)
(define external->internal -23)
(define cgs->internal -24)
(define cgs->external -25)
(define no-conversion -26)

; the B packages
(define code-packages nil)

; the graphics device
(define device nil)

; the PVA file
(define plotfile nil)

; the current problem
(define current-problem nil)
(define commands-installed #f)

; the def macro
;        (define-macro (def . rest)
;                (apply define-global rest)
;                #t)
(define def define-global)

;--------------------------------------------------------------------------

;                           INTERNAL ROUTINES

;--------------------------------------------------------------------------

; RUN-PACKAGE

(define (run-package name)
    (interactive off)
    (pa-run-package
        name
	(current-time) (current-timestep) (current-cycle)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; ADVANCE-TIME

(define (advance-time)
    (interactive off)
    (pa-advance-time (current-timestep)
			  (maximum-dtf)
			  (minimum-dtf)))

;--------------------------------------------------------------------------

;                           BS ROUTINES

;--------------------------------------------------------------------------

; ANIMATE

(define-macro (animate var tf . pck)
    "Macro: Run a package or combination of packages repeatedly until a
     specified stop time and plot a variable each cycle.
     Usage: animate <var> <Tstop> [<package1> ... <packagen>]"
    (interactive off)
    (let ((name (if (null? pck)
		    code-packages
		    pck))
	  (dtmax (* 0.1 tf)))

         (define (stepper var t)
  	     (if (< (car (time-data)) t)
		 (begin (for-each run-package name)
			(advance-time)
			(cond ((pair? var)
			       (apply plot-var var))
			      (#t
			       (plot-var var)))
			(display ".")
			(stepper var t))
		 #t))

	 (printf nil "\nAnimate ")
	 (stepper var tf)
	 (printf nil " done\n")

	 #t))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; CLOSE-PLOT-FILE

(define (close-plot-file file)
    "Procedure: Close the specified PVA file.
     Usage: close-plot-file <PVA file>"
    (close-pdbfile file)
    (set! plotfile nil)
    #t)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DESC

(define-macro (desc name)
    "Macro: Describe the specified database entity.
     Usage: desc <name>"
    (pa-describe-entity name)
    #t)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; END

(define (end)
    "Procedure: gracefully end the session being sure to close open files.
     Usage: end"
    (interactive off)
    (if current-problem
	(begin (pa-finish-simulation
		   (number-of-zones) (current-cycle))
	       (pa-write-state-file
		   (pa-advance-name (state-file-name)))))
    (if plotfile
	(close-plot-file plotfile))
    nil)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; FILES

(define b-files files)

(define-macro (files)
    "Macro: Print the current state, pp, PVA, and edit files names.
     Usage: files"
    (interactive off)
    (let* ((st (state-file-name))
	     (pp (pp-file-name))
	     (ed (edit-file-name))
	     (pv (PVA-file-name)))
            (if st
		(begin (printf nil "State: %s\nPP: %s\nPVA: %s\nEdit: %s\n"
			       st pp pv ed)
		       (list st pp pv ed))
		(begin (printf nil "No current files.\n")
		       #f))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HELP

(define-macro (help . rest)
    (let ((command (if (null? rest)
		       rest
		       (car rest))))
         (cond ((eqv? command 'mode)
		(printf nil "\nModes :\n")
		(printf nil "full-path - \n")
		(printf nil "indent    - \n")
		(printf nil "tree      - \n")
		(printf nil "no-type   - \n")
		(printf nil "type      - \n")
		(printf nil "recursive - \n")
		(printf nil "iterative - \n"))
	       ((not command)
		(printf nil "\nCommands :\n")
		(printf nil "animate         - simulate and display named variable\n")
		(printf nil "                  animate <var> <stop-time> [packages ...]\n")
		(printf nil "close-plot-file - close the post processor file\n")
		(printf nil "desc            - describe the named variable\n")
		(printf nil "end             - end the session of PDBView\n")
		(printf nil "files          - print state, pp, PVA, and edit file names\n")
;                (printf nil "hc              - make a hard copy of the current graph\n")
		(printf nil "help            - print this message\n")
;                (printf nil "levels          - set the number of levels\n")
		(printf nil "ld              - load a program file\n")
		(printf nil "ls              - list the contents of the current file\n") 
		(printf nil "mode            - set the print mode for structures\n")
		(printf nil "open            - open a new state file\n")
		(printf nil "open-plot_file  - create a new post processor file\n")
		(printf nil "packages        - print the names of the current packages\n")
		(printf nil "plot            - plot the named variable\n")
		(printf nil "print            - print out the values for a variable\n")
		(printf nil "rd              - read a generator command file\n")
		(printf nil "run             - run the simulation to a certain time\n")
;                (printf nil "screen-off      - close the current graphics window\n")
		(printf nil "status          - print current t, dt, and cycle\n")
		(printf nil "step            - advance the state a single time step\n"))
	       (else
		(describe command)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; LD - load Scheme forms from a file

(define-macro (ld . rest)
    "Macro: Load Scheme forms from a file.
     Forms in file must be delimited by parentheses.
     Usage: ld <filename>"
    (if (null? rest)
	(printf nil "\nNo filename specified\n")
	(load (car rest))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; LS

(define-macro (ls . rest)
    "Macro: List the variables in the data base.
     Usage: ls"
    (if rest
	(pp (hash-dump pa-variable-table (car rest)))
	(pp (hash-dump pa-variable-table)))
    #t)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; OPEN

(define (open* file)
    "Procedure: Open the specified state file.
     Usage open* <file>"
    (interactive off)
    (pa-read-state-file file no-conversion)
    (def-cc-vars)
    (pa-init-simulation
        (current-time) (current-timestep) (current-cycle)
	(edit-file-name) (pp-file-name) (PVA-file-name))
    (set! code-packages (pa-intern-packages))
    (set! current-problem (strtok file "." #t))
    #t)

(define-macro (open file)
    "Macro: Open the specified state file.
     Usage open <file>"
    (interactive off)
    (pa-read-state-file file no-conversion)
    (def-cc-vars)
    (pa-init-simulation
        (current-time) (current-timestep) (current-cycle)
	(edit-file-name) (pp-file-name) (PVA-file-name))
    (set! code-packages (pa-intern-packages))
    (set! current-problem (strtok file "." #t))
    #t)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; OPEN-PLOT-FILE

(define-macro (open-plot-file file)
    "Macro: Open the specified FILE as a PVA file.
     Usage: open-plot-file <file>"
    (set! plotfile (create-pdbfile file))
    (pgs-def-graph-file plotfile)
    #t)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PACKAGES

(define-macro (packages)
    "Macro: Print the names of the current packages.
     Usage: packages"
    (pp code-packages)
    code-packages)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PLOT

(define-macro (plot . rest)
    "Macro: Plot the specified variable.
     The following define plot specifications:
         <spec> = <plot_type>|<set>|(device <device>)|(file <PDBfile>)
         <set>  = <name>|([domain] <name> [<color> <width> <style>])
     Usage: plot <spec1> ... <specn>"
    (apply plot-var rest)
    #t)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PRINT

(define-macro (print name)
    "Macro: Print the named variable's values to the console.
     Usage: print <name>"
    (pa-display name)
    #t)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; RD - read a file full of input/generator commands

(define-macro (rd . rest)
    "Macro: Read commands from the given file.
     Usage: rd <filename>"
    (if (null? rest)
	(printf nil "\nNo filename specified\n")
        (begin (if (not commands-installed)
		   (pa-install-commands))
	       (set! commands-installed #t)
	       (pa-read-commands (car rest)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; READ-COMMAND

(define (read-command)
   (let ((port (string->port (read-line)))
	 ret-val)
        (define (read-object prt lst)
	    (let ((x (read prt)))
	         (if (eof-object? x)
		     lst
		     (read-object prt (cons x lst)))))
	(set! ret-val (read-object port nil))
	(if (not (null? ret-val))
	    (reverse ret-val)
	    ret-val)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; RUN

(define-macro (run t1 . rest)
    "Macro: Run the simulation from Tstart to Tstop. If only one argument
     is given, Tstart is taken to be 0.
     Usage: run <Tstop>
            run <Tstart> <Tstop>"
    (interactive off)
    (let ((tstart (if (null? rest)
		      0.0
		      t1))
	  (tstop  (if (null? rest)
		      t1
		      (car rest))))
         (pa-simulate tstart tstop
			   (minimum-dtf)
			   (maximum-dtf)
			   (increment-dtf)
			   (state-file-name)
			   (edit-file-name)
			   (pp-file-name)
			   (PVA-file-name))
	 #t))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SCREEN-OFF

(define-macro (screen-off)
    "Macro: Close the current graphics device.
     Usage: screen-off"
    (if (not (null? device))
	(pgs-close-device device))
    (set! device nil)
    #f)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; STATUS

(define-macro (status)
    "Macro: Print the current t, dt, and cycle number.
     Usage: status"
    (interactive off)
    (printf nil "Time: %s\ndt: %s\nCycle: %s\n"
	    (current-time)
	    (current-timestep)
	    (current-cycle)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; STEP

(define-macro (step . pck)
    "Macro: Run the specified packages a single major time step.
     Usage: step [<package1> ... <packagen>]"
    (interactive off)
    (let ((name (if (null? pck)
		    code-packages
		    pck)))

         (define (stepper proc)
	     (printf nil "%s " (pa-package-name proc))
	     (run-package proc))

	 (printf nil "\nRunning: ")
	 (for-each stepper name)
	 (printf nil "done\n\n")
	 #t))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; BS - appropriate place to start B under Sx

(define-macro (bs . file)

    (define-macro (b-error func args bad msg)
        (printf nil "\nError in call: %s\n" (cons func args))
	(printf nil "Bad operand: %s\n" bad)
	(printf nil "Internal message: %s\n" msg)
	#t)

; the main loop
    (define (main-loop)
        (let (read-object eval-object)
	     (newline)
	     (interactive on)
	     (display "-> ")
	     (set! read-object (read-command))
	     (cond ((eqv? (car read-object) 'define)
		    read-object
;		    (apply define (cdr read-object))
		    (set! eval-object #t))
		   (#t
		    (set! eval-object (eval read-object))))
	     (newline)
	     (if (null? eval-object)
		 #f
		 (main-loop))))

    (define (start-b)
        (if (err-catch b-error (main-loop))
	    (start-b)))

    (if file
	(open* (car file))
        (def-cc-vars))

    (start-b))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

