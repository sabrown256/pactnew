;
; COMPARE.SCM - define routines to compare ultra files for code
;             - validation and certification
;
; Source Version: 4.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
;

(define HUGE 1.0e100)
(define SMALL 1.0e-100)
(define TOLERANCE 1.0e-8)
(define PRECISION 1.0e-8)
(define ORDER 1)

; --------------------------------------------------------------------------
; --------------------------------------------------------------------------

; DIFF-MEASURE - something like the chi squared between two curves

(define (diff-measure crv-a crv-b . rest)
    "Procedure: computes a difference measure between the given curves and its
     integral. The difference measure is |A - B|/(|A| + |B|).
     If the optional third argument is #t the colors of the curves involved
     are set. The optional fourth argument is the difference threshold which
     defaults to 1.0e-8.
     Usage: diff-measure <curve-a> <curve-b> [#t|#f] [precision]
     Example: diff-measure a b
              diff-measure a b #f 1.0e-5"
   (interactive off)
   (let* ((ar (get-range crv-a))
	  (arn (car ar))
	  (arx (cdr ar))
	  (fa (* (- arx arn) TOLERANCE))

	  (br (get-range crv-b))
	  (brn (car br))
	  (brx (cdr br))
	  (fb (* (- brx brn) TOLERANCE))

	  (tc (dy (abs (copy crv-a)) fa))
	  (td (dy (abs (copy crv-b)) fb))
	  (den (+ tc td SMALL))

	  (diff (abs (- crv-a crv-b)))

; diff variables
	  (di (get-domain diff))
	  (dimin (car di))
	  (dimax (cdr di))
	  (dim (- dimax dimin))

	  (ri (get-range diff))
	  (rimin (car ri))
	  (rimax (cdr ri))
	  (rim (- rimax rimin))

; den variables
	  (rng (get-range den))
	  (remin (car rng))
	  (remax (cdr rng))
	  (rem (- remax remin))

	  (view (and rest (car rest)))
	  (precision (if (and rest (< 3 (length rest)))
			 (list-ref rest 1)
			 PRECISION))
			      
	  meas integ)

     (del tc td)
     (set! fa
	   (cond ((and (eqv? rim 0.0) (eqv? rimin 0.0))
		  (del diff den)
		  0.0)

; this did not account for curves which where constant!
;		       ((eqv? rim 0.0)
;			(del diff den)
;			0.0)

		 ((and (or (> (- rim rem) precision)
			   (> rim HUGE)
			   (> rem HUGE))
		       (> (/ rim rem) precision))
		  (printf nil "\nMeasure values exceed maximum precision: ")
		  (printf nil "Diff = %e, Sum = %e\n" rim rem)
		  (del diff den)
		  precision)

		 (t (set! meas (/ diff den))
		    (label meas "FD = |A - B|/(|A| + |B|)")
		    (set! integ (integrate meas dimin dimax))
		    (divy integ dim)
		    (label integ "Integral(FD)/dX")
		    (del diff den)
		    (if view
			(begin (color crv-a blue)
			       (color crv-b dark-green)
			       (color integ red)
			       (color meas  brown)))
		    ((eval integ) (- dimax (* 1.0e-5 dim))))))

     (if (not view)
	 (printf nil "\nDifference measure for curves %s and %s: %s\n\n"
		 crv-a crv-b fa))

     fa))

; --------------------------------------------------------------------------
; --------------------------------------------------------------------------

; DIFF-LNNORM - compute the Ln Norm of the difference of the two input
;               curves

(define (diff-lnnorm crv-a crv-b . rest)
    "Procedure: computes the Ln Norm of the difference of the two
     input curves.  If the optional third argument is present it
     specifies the order which defaults to 1.  If order is negative
     the Linfinity norm is returned.
     Usage: diff-lnnorm <curve-a> <curve-b> [order]
     Example: diff-lnnorm a b       L1 norm  
              diff-lnnorm a b 2     L2 norm
              diff-lnnorm a b -1    Linfinity norm"

     (interactive off)
;     (let* ((diff  (- crv-a crv-b))
;            (order (if (rest)
;                       (car rest)
;                       ORDER))))

     (define diff (- crv-a crv-b))
     (define order (if (pair? rest)
                       (car rest)
                       ORDER))

     (label diff (sprintf "%s - %s" crv-a crv-b))
     (define lnnrm (lnnorm diff order))
     (if (> order 0)
            (printf nil "\nL%s Norm measure for curves %s and %s: %s\n\n"
                         order crv-a crv-b lnnrm)
            (printf nil "\nLinfinity Norm measure for curves %s and %s: %s\n\n"
                         crv-a crv-b lnnrm))
     lnnrm)

; --------------------------------------------------------------------------
; --------------------------------------------------------------------------

; REPL - the read-eval-print loop for the validation process
;      -
;      - the special commands are:
;      -
;      - N* - these curves don't compare and go on to the next pair
;      - Y* - these curves do compare and go on to the next pair
;

(define (repl nc n1 precision batch)
  (let* (form
	 dc
	 (correct 0)
	 (differ nil))

    (define (repl-diff n dc batch frst)
      (if batch
	  (begin
	    (printf nil  "\n%4d | %10.3e | %s"
		    n dc (get-label a))
	    (set! differ (cons n differ))
	    (repl-aux (+ n 1) nc n1 frst))

	  (let* (form fl)
	    (plots on)
	    (if frst
                (begin
		    (printf nil
			    "\n[ Crv# | Frac Diff | Label ]->  y-accept, n-reject, (command)\n")
		    (set! frst #f)))
	    (printf nil
		    "\n[%4d | %10.3e | %s ]-> "
		    n dc (get-label a))
	    (set! form (read))
	    (set! fl (substring (sprintf "%s" form) 0 1))
	    (cond ((eqv? fl "n")
		   (set! differ (cons n differ))
		   (repl-aux (+ n 1) nc n1 frst))
		  ((eqv? fl "y")
		   (set! correct (+ correct 1))
		   (repl-aux (+ n 1) nc n1 frst))
		  (#t
		   (interactive on)
		   (eval form)
		   (interactive off)
		   (repl-aux n nc n1 frst))))))

    (define (repl-aux n nc n1 frst)
      (if (<= n nc)
	  (begin (erase)
		 (set! dc (apply diff-measure
				 (append (select n (+ n n1))
					 (list #t precision))))
		 (if (< dc precision)
		     (begin
		       (set! correct (+ correct 1))
		       (repl-aux (+ n 1) nc n1 frst))
		     (repl-diff n dc batch frst)))))

    (repl-aux 1 nc n1 #t)
    (list correct differ)))

; --------------------------------------------------------------------------
; --------------------------------------------------------------------------

; COMPARE - read in two files and display each pair of matching curves
;         - as prompted
;         - when all have been checked kill all of them and erase the
;         - screen
;         - there is some development to be done here

(define-macro (compare new old . rest)
    "Macro: Compare corresponding pairs of curves from two files. This
     maps diff-measure over the pairs of curves and stops to display
     pairs which do not match. Optional difference precision may be supplied
     and the default value is 1.0e-8.
     Usage: compare <file-1> <file-2> [precision]"
   (let* (log n1 n2 nc result
	      (precision (if rest
			     (list-ref rest 0)
			     PRECISION))
	      (batch     (if (and rest (> (length rest) 1))
			     (list-ref rest 1)
			     #f))
	      (snew (if (symbol? new)
			(symbol->string new)
			new))
	      (file snew))

     (define (report-fail file differing n-differ n-total new old)
         (printf file
		 "%d curve(s) out of %d differ between %s and %s:\n"
		 n-differ n-total new old)
	 (for-each '(lambda (n) (printf file " %d" n))
		   differing)
	 (printf file "\n\n"))

     (define (report-success file new old tol)
         (printf file "All curves in %s and %s agree to within one part in %s\n\n"
		 new old tol))

     (printf nil "\nComparing %s and %s\n" new old)

; force log file to be saved where ULTRA is being run
; if "new" is in a directory, pull off the
; file name and use that to create the log file name, rather than the whole path
     (if (memv #\/ (string->list file))
	 (set! file (string-append (list->string (reverse (string->list
							   (strtok (list->string (reverse (string->list file))) "/")))) ".chk"))
	 (set! file (string-append file ".chk")))
     (set! log (open-output-file file))
     (interactive off)
     (kill all)
     (apply rd (list new))
     (set! n1 (n-curves-read))
     (apply rd (list old))
     (set! n2 (- (n-curves-read) n1))
     (set! nc (if (< n1 n2)
		  n1
		  n2))
     (set! result (repl nc n1 precision batch))
     (printf nil "\n")

     (set! n1 (car result))
     (set! result (reverse (cadr result)))
     (if (eqv? n1 nc)

; all match
	 (begin
	     (report-success log new old precision)
	     (report-success nil new old precision)
	     (set! result #t))

; some differ
	 (begin
	     (report-fail log result (- nc n1) nc new old)
	     (report-fail nil result (- nc n1) nc new old)
	     (set! result #f)))

     (close-output-file log)
     (kill all)
     (erase)

     result))

; --------------------------------------------------------------------------
; --------------------------------------------------------------------------

