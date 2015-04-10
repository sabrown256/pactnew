;
; PDBVIEW.SCM - load the SX level part of PDBView
;
; Source Version: 4.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
;

(define pdbview #t)
(if (not (defined? current-file))
    (define current-file nil))

; be sure that the SCHEME runtime functions are loaded
(load "runt.scm" #t)

(interpolation-method -100)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; this holds the net memory leaked
(define $loss 0)

; this holds the size of the temporary cons between start and stop
(define $dela nil)

; this holds the size of memory tied up in the memory-usage calls themselves
(define $delb nil)

; this tells us whether it is the first call to measure
(define $first #t)

(define-macro (measure expr)
    (let* (start stop dloss ts dt)
        (if $first
	    (begin (set! start (memory-usage))
		   (eval #t)
		   (set! stop (memory-usage))
		   (set! $dela (- (list-ref stop 1) (list-ref start 1)))
		   (set! $delb (- (list-ref stop 2) (list-ref start 2)))
                   (printf nil "\n                  Memory Usage\n")
                   (printf nil "  Time      Alloc       Free     Diff     Expression\n")))
	(set! start (memory-usage))
        (set! ts (wall-clock-time))
	(eval expr)
        (set! dt (- (wall-clock-time) ts))
	(set! stop (memory-usage))
        (if $first
            (begin
                (set! dloss (- (list-ref stop 2) (list-ref start 2)))
                (printf nil "%6.2f %10ld %10ld %8ld  :  %s\n"
			dt
			(- (list-ref stop 0) (list-ref start 0) $dela $delb)
			(- (list-ref stop 1) (list-ref start 1) $dela $delb)
			(- (list-ref stop 2) (list-ref start 2))
			expr))
            (begin
                (set! dloss (- (list-ref stop 2) (list-ref start 2) $delb))
                (printf nil "%6.2f %10ld %10ld %8ld  :  %s\n"
			dt
			(- (list-ref stop 0) (list-ref start 0) $dela $delb)
			(- (list-ref stop 1) (list-ref start 1) $dela)
			dloss
			expr)))
        (set! $loss (+ $loss dloss))
        (set! $first #f)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SPLICE-OUT - splice the specified item out of the list

(define (splice-out item lst pred)
    "Procedure: Splice the item out of the given list. Identity determined
                by the supplied predicate (eqv? a good choice).
     Usage: splice-out <item> <list> <predicate>"
    (define (helper item old new)
        (if old
	    (let* ((first (car old))
		   (rest (cdr old))
		   (next (list first)))
	      (if (pred item first)
		  (append new rest)
		  (helper item rest (if new
					(append new next)
					next))))
	    new))

    (helper item lst nil))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; FLATTEN-ARGS - flatten an argument list to properly handle things like
;              - (thru n m) than may have been used

(define (flatten-args src dst)
   (if src
       (let* ((frst (list-ref src 0))
	      (rest (list-tail src 1)))
             (if (pair? frst)
		 (flatten-args rest (append (reverse frst) dst))
		 (flatten-args rest (cons frst dst))))
       (reverse dst)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; RESOLVE-TO-DRAWABLE - given a file and some kind of information leading
;                     - ta a drawable get it and return it

(define (resolve-to-drawable file x)
    (cond ((integer? x)
	   (let* ((type (pg-menu-item-type file x)))
	         (cond ((or (eqv? type "mapping") (eqv? type "curve"))
			((io-function file "map-in") file x))
		       ((eqv? type "image")
			((io-function file "image-in") file x)))))

	  ((pm-mapping? x)
	   (let* ((cent (list-ref (pg-get-attrs-mapping x "CENTERING" #f) 0)))
	         (pg-make-graph (pm-mapping-domain x)
				(pm-mapping-range x)
				(pm-mapping-name x)
				cent)))

	  ((pg-graph? x)
	   x)

	  ((pm-set? x)
	   x)

	  ((symbol? x)
	   (let* ((type (strtok (sprintf "%s" (variable-type x)) " *")))
	         (cond ((or (eqv? type "PM_mapping") (eqv? type "char"))
			(pdbdata->pg-graph file x))
		       ((eqv? type "PG_image")
			(pdbdata->pg-image file x))
		       ((eqv? type "PM_set")
			(pdbdata->pm-set file x)))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; IO-STATS - report the I/O stats on FP
;          - use collect-io-info to control gathering of timing info

(define (io-stats fp)
    "Procedure: Print the I/O operation statistics for the given file.
     Usage: io-stats <file>"

    (let* ((stats (get-io-info fp)))

          (define io-opers (list "none" "fopen" "fclose"
				 "ftell" "lftell" "fseek" "lfseek"
				 "fread" "lfread" "fwrite" "lfwrite"
				 "fprintf" "fputs" "fgets" "fgetc"
				 "ungetc" "fflush" "feof" "setvbuf"
				 "pointer" "segsize"))
          (define (do-one sl ol)
	      (if sl
		  (let* ((sd (list-ref sl 0))
			 (nh (car sd))
			 (ns (cdr sd))
			 (on (list-ref ol 0)))
		        (if (> nh 0)
			    (printf nil "      %-10s %8d %11.3e\n"
				    on nh (/ ns nh)))
			(do-one (cdr sl) (cdr ol)))))

	  (printf nil "I/O stats:\n")
	  (printf nil "   Operation       # Hits   Tave (sec)\n")
	  (do-one stats io-opers)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; IO-STATS - report the I/O stats on FP
;          - use collect-io-info to control gathering of timing info

(define (io-stats fp)
    "Procedure: Print the I/O operation statistics for the given file.
     Usage: io-stats <file>"

    (let* ((stats (get-io-info fp)))

          (define io-opers (list "none" "fopen" "fclose"
				 "ftell" "lftell" "fseek" "lfseek"
				 "fread" "lfread" "fwrite" "lfwrite"
				 "fprintf" "fputs" "fgets" "fgetc"
				 "ungetc" "fflush" "feof" "setvbuf"
				 "pointer" "segsize"))
          (define (do-one sl ol)
	      (if sl
		  (let* ((sd (list-ref sl 0))
			 (nh (car sd))
			 (ns (cdr sd))
			 (on (list-ref ol 0)))
		        (if (> nh 0)
			    (printf nil "      %-10s %8d %11.3e\n"
				    on nh (/ ns nh)))
			(do-one (cdr sl) (cdr ol)))))

	  (printf nil "I/O stats:\n")
	  (printf nil "   Operation       # Hits   Tave (sec)\n")
	  (do-one stats io-opers)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (safe-load file)
  (if (file? file nil nil "global")
      (load file #t)
      (printf nil "Can't find %s\n" file)))

; (define (safe-load file)
;    (load file #t)
;    (printf nil "%s loaded\n" file))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(safe-load "consts.scm")     ; load the constants
(safe-load "hardcopy.scm")   ; load the hardcopy commands
(safe-load "pdbvio.scm")     ; load the I/O commands
(safe-load "pdbvcmd.scm")    ; load the non-I/O commands
(safe-load "pdbvar.scm")     ; load the user level variable commands
(safe-load "synon.scm")      ; load the aliases
(safe-load "help.scm")       ; load the help package
(safe-load "ezn.scm")        ; load the Basis EZN interface
(safe-load "spokes.scm")     ; load the translation spokes support

(autoload* 'annot          "annotate.scm" #f)
