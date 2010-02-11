;
; SXDM.SCM - define routines to compare PM_mappings in PDB files
;          - and display mappings whose difference measure exceeds
;          - the specified tolerance (default 1.0e-8)
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
; 

(define HUGE 1.0e99)
(define SMALL 1.0e-99)
(define TOLERANCE 1.0e-8)
(define WEIGHT_FACTOR 1.0e6)
(define PRECISION 1.0e15)
(define nname " ")
(define oname " ")
(define user-stop #f)
(define displays-off #t)
(define displays-size "")

(define contour    11)
(define image      12)
(define wire-frame 13)
(define shaded     14)
; NOTE: name vector would conflict with function
(define vect       15)

(define theta 45.0)
(define phi -45.0)

; --------------------------------------------------------------------------
; --------------------------------------------------------------------------

; FIND-SIGNIFICANT-VALUE-MASK - return a mapping with values 0 and 1
;                             - which will mask points where A
;                             - is below mp-limit relative
;                             - to the maximum value of A
;                             - more precisely:
;                             -          /
;                             -         |  1     |a(x)|/A > mp-limit
;                             -  f(x) = |
;                             -         |  0     otherwise
;                             -          \
;                             -
;                             -  A = |max(a)|
;                             -

(define (find-significant-value-mask a mp-limit)
   (let* ((ma (list-ref (pg-range-extrema a) 1))
	  (ca (divy (if (eqv? ma 0.0) mp-limit ma) (copy-map a)))
	  (mt (dy mp-limit (my 0.0 (copy-map a))))
	  (f  (m> ca mt)))
         (dl mt ca)
	 (pg-set-label! f "Precision Mask")))

; --------------------------------------------------------------------------
; --------------------------------------------------------------------------

; DIFF-MEASURE - something like the chi squared between two mappings

(define (diff-measure a b mp-limit)
   (interactive off)
   (let* ((ga (map-resolve a))
	  (gb (map-resolve b))
	  (tc (pg-set-label! (norm ga) "||a||"))
	  (td (pg-set-label! (norm gb) "||b||"))

; diff and den must both be zero volume mappings if the domain volume is zero
; otherwise diff is the norm of the difference and den is the sum of the norms
	  (domvol (pm-set-volume (pm-mapping-domain td)))
	  (diff   (if (eqv? domvol 0.0)
		      td
		      (let* ((dm (m- tc td))
			     (ndm (norm dm)))
			    (dl dm)
			    (pg-set-label! ndm "|a - b|"))))
	  (den    (if (eqv? domvol 0.0)
		      td
		      (pg-set-label! (my 0.5 (m+ tc td)) "ave(a,b)")))

; diff variables
	  (domain-volume     (pm-set-volume (pm-mapping-domain diff)))
	  (diff-range-volume (pm-set-volume (pm-mapping-range diff)))

; den variables
	  (ave-range-volume  (pm-set-volume (pm-mapping-range den)))

	  meas integ tp mask)

     (cond ((eqv? diff-range-volume 0.0)
	    (dl tc td diff den)
	    (list 0.0 nil nil nil))

	   ((or (> (- diff-range-volume ave-range-volume) PRECISION)
		(> diff-range-volume HUGE)
		(> ave-range-volume HUGE))
	    (dl tc td diff den)
	    (list PRECISION nil nil nil))

	   (#t (set! tp
		     (pg-set-label! (m/ diff (dy SMALL den))
				     "|a - b|/ave(a,b)"))
				     
	       (set! mask
		     (pg-set-label! (find-significant-value-mask den mp-limit)
				     "Mask"))
				     
								 
	       (set! meas
		     (pg-set-label! (m* tp mask)
				     "mask*|a - b|/ave(a,b)"))
				    

	       (set! integ
		     (pg-set-label! (integrate meas)
				     "int(mask*|a - b|/ave(a,b))"))
				    

	       (dl tc td tp diff den mask)
	       (list (/ (pm-set-volume (pm-mapping-range integ))
			domain-volume)
		     meas integ domain-volume)))))

; --------------------------------------------------------------------------
; --------------------------------------------------------------------------

; DIFF-MAPPINGS - the read-eval-print loop for the validation process
;               -
;               - the special commands are:
;               -
;               - NEXT/NO - these curves don't compare and go on to the next pair
;               - OK/YES  - these curves do compare and go on to the next pair
;

(define (diff-mappings file1 file2 batch mps)
  (let* ((correct 0)
	 (differ nil)
         (least-diff HUGE)
         (greatest-diff (- HUGE))
	 (mp-limit (* (machine-precision) WEIGHT_FACTOR)))

    (define (note-difference n dc)
        (set! differ
	      (cons (sprintf "Mapping %4d error: %10.3e > %10.3e\n"
			     n dc TOLERANCE)
		    differ)))

    (define (get-response n a dc frst)
        (if frst
	    (begin
	        (printf nil
			"\n[ Map# | Frac Diff | Label ]->  y-accept, n-reject, t-terminate, (command)\n")
		(set! frst #f)))
        (printf nil  "\n[%4d | %10.3e | %s ]-> "
		n dc a)
	(let (form fl)
	     (set! form (read))
	     (set! fl (substring (sprintf "%s" form) 0 1))
	     (cond ((eqv? fl "n")
		    (note-difference n dc))
		   ((eqv? fl "y")
		    (set! correct (+ correct 1)))
		   ((eqv? fl "t")
		    (set! user-stop #t))
		   (#t
		    (interactive on)
		    (eval form)
		    (interactive off)
		    (wu)
		    (set! frst (get-response n a dc frst)))))
	frst)

    (define (process-mapping-pair a b n file1 file2 frst)
        (if (and (pm-mapping? a) (pm-mapping? b))
	    (let* ((form (diff-measure a b mp-limit))
		   (dc   (car form))
		   (c    (cadr form))
		   (d    (caddr form))
		   (vol  (cadddr form)))

	          (set! least-diff    (min least-diff dc))
	          (set! greatest-diff (max greatest-diff dc))

	          (if (< dc TOLERANCE)
		      (set! correct (+ correct 1))
		      (if (= batch 1)
			  (note-difference n dc)
			  (let* ((dims (pm-mapping-dimension a)))
			    (if (and (= (car dims) 1) (= (cdr dims) 1))
				(begin
				  (if (not (eqv? displays-size "1x2"))
                                      (begin
                                        (close-devices) 
				        (open-devices-1x2)))
				  (show-mappings-1d-1d a b c d vol file1 file2))
				(begin
				  (if (not (eqv? displays-size "2x2")) 
                                      (begin
                                        (close-devices)
				        (open-devices-2x2)))
				  (show-mappings a b c d vol file1 file2)))
			    (set! frst (get-response n (pm-mapping-name a) dc frst)))))))
	frst)

    (define (diff-mappings-aux mps n file1 file2 frst)
        (let* ((m   (cond (mps (car mps))
			  ((> n 0) n)
			  (else -1)))
	       (mn  (if (and (not mps) (> n 0)) (+ n 1) -1))
	       (rst (if mps (cdr mps) nil))
	       (ga (resolve-to-drawable file1 m))
	       (gb (resolve-to-drawable file2 m))
	       (a  (cond ((pm-mapping? ga)
			  ga)
			 ((pg-graph? ga)
			  (pg-drawable-info ga "f"))
			 ((pg-image? ga)
			  #t)
			 ((pm-grotrian-mapping? ga)
			  #f)
                         (else #f)))
	       (b  (cond ((pm-mapping? gb)
			  gb)
			 ((pg-graph? gb)
			  (pg-drawable-info gb "f"))
			 ((pg-image? gb)
			  #t)
			 ((pm-grotrian-mapping? gb)
			  #f)
			 (else #f))))
	  (if (and a b)
	      (begin
		(set! frst (process-mapping-pair a b m file1 file2 frst))
		(if (not user-stop)
		    (diff-mappings-aux rst mn file1 file2 frst))))))

    (labels off)

    (diff-mappings-aux mps 1 file1 file2 #t)

    (if (not displays-off)
	(close-devices))
    (newline)

    (list correct differ least-diff greatest-diff)))

; --------------------------------------------------------------------------
; --------------------------------------------------------------------------

; OPEN-DEVICES-1x2 - open two devices to show the mappings from the
;                 - new and old files in one viewport, and the difference
;                 - and integral of the difference in another viewport.

(define-macro (open-devices-1x2)
  (cw "1x2" "COLOR" "WINDOW" 0.1 0.1 0.3 0.6)
  (nxm 1 2 "Analysis" "Data")

  (window-origin-x-ps 0.25)
  (window-origin-y-ps 0.05)
  (window-width-ps 0.5)
  (window-height-ps 1.0)
  (window-origin-x-cgm 0.25)
  (window-origin-y-cgm 0.0)
  (window-width-cgm 0.5)
  (window-height-cgm 1.0)

  (if (= (jpeg-flag) 1)
      (begin
	(window-width-jpeg 512.0)
	(window-height-jpeg 1024.0)))

  (if (= (png-flag) 1)
      (begin
	(window-width-png 512.0)
	(window-height-png 1024.0)))

  (window-width-mpeg 512.0)
  (window-height-mpeg 1024.0)

  (set! displays-off #f)
  (set! displays-size "1x2"))

; --------------------------------------------------------------------------
; --------------------------------------------------------------------------

; OPEN-DEVICES-2x2 - open four devices to show the mappings from the
;                  - new and old files, the difference, and the integral
;                  - of the difference in four separate viewports.

(define-macro (open-devices-2x2)
    (cw "2x2" "COLOR" "WINDOW" 0.1 0.1 0.6 0.6)
    (nxm 2 2 "Diff" "Int" "Data1" "Data2")

    (window-origin-x-ps 0.0)
    (window-width-ps 1.0)
    (window-origin-y-ps 0.05)
    (window-height-ps 1.01)

    (set! displays-off #f)
    (set! displays-size "2x2"))

; --------------------------------------------------------------------------
; --------------------------------------------------------------------------

; CLOSE-DEVICES - close the four devices

(define-macro (close-devices)
   (clw)
   (set! displays-off #t)
   (set! displays-size ""))

; --------------------------------------------------------------------------
; --------------------------------------------------------------------------

; SHOW-MAPPINGS - display the four mappings in four viewports

(define (show-mappings a b c d vol file1 file2)
  (let* ((pal default-palette))
    (rem-annotations (window-device current-window))
    (data-id off)
    (font helvetica font-size-nxm medium)

    (cv "Data2")
    (dl 1)
    (if (not (null? b))
	(begin (display-mapping* b)
	       (lncolor dark-green 1)
	       (palette standard)
	       (annot (sprintf (get-label 1)) dark-green 0.51 0.99 0.97 0.99)
	       (annot (sprintf ">  %s" oname) dark-green 0.51 0.99 0.95 0.97)
	       (annot (sprintf (file-info file2 "date")) dark-green 0.51 .99 0.93 0.95)
	       (viewport-update)
	       (palette pal)))

    (cv "Data1")
    (dl 1)
    (if (not (null? a))
	(begin (display-mapping* a)
	       (lncolor blue 1)
	       (palette standard)
	       (annot (sprintf (get-label 1)) blue 0.01 0.49 0.97 0.99)
	       (annot (sprintf "<  %s" nname) blue 0.01 0.49 0.95 0.97)
	       (annot (sprintf (file-info file1 "date")) blue 0.01 .49 0.93 0.95)
	       (viewport-update)
	       (palette pal)))

    (cv "Diff")
    (dl)
    (if (not (null? c))
	(begin (display-mapping* c)
	       (lncolor brown 1)
	       (palette standard)
	       (annot "Fractional Difference" brown 0.1 0.4 0.46 0.48)
	       (annot (sprintf "Min = %s     Max = %s"
			       (car (get-range 1)) (cadr (get-range 1)))
		      brown 0.1 0.4 0.44 0.46)
	       (viewport-update)
	       (palette pal)))

    (cv "Int")
    (dl)
    (if (not (null? d))
	(begin (display-mapping* d)
	       (lncolor red 1)
	       (palette standard)
	       (annot "Integrated Difference" red 0.6 0.9 0.46 0.48)
	       (annot (sprintf "Tot = %s     Ave = %s"
			       (cadr (get-range 1)) (/ (cadr (get-range 1)) vol))
		      red 0.6 0.9 0.43 0.46)
	       (viewport-update)
	       (palette pal)))

    (cv "Data1")))

; --------------------------------------------------------------------------
; --------------------------------------------------------------------------

; SHOW-MAPPINGS-1D-1D - display the four mappings in two viewports

(define (show-mappings-1d-1d a b c d vol file1 file2)
  (let* ((pal default-palette))
    (rem-annotations (window-device current-window))
    (data-id on)
    (font helvetica font-size-nxm medium)

    (cv "Data")
    (dl)
    (if (not (null? b))
	(begin (display-mapping* b)
	       (lncolor dark-green 1)))
    (if (not (null? a))
	(begin (display-mapping* a)
	       (lncolor blue 1)
	       (palette standard)
	       (annot (sprintf (get-label 1)) blue 0.01 0.49 0.97 0.99)
	       (annot (sprintf (get-label 2)) dark-green 0.51 0.99 0.97 0.99)
	       (annot (sprintf "< (1)  %s" nname) blue 0.01 0.49 0.95 0.97)
	       (annot (sprintf "> (2)  %s" oname) dark-green 0.51 .99 0.95 0.97)
	       (annot (sprintf (file-info file1 "date")) blue 0.01 .49 0.93 0.95)
	       (annot (sprintf (file-info file2 "date")) dark-green 0.51 .99 0.93 0.95)
	       (viewport-update)
	       (palette pal)))

    (cv "Analysis")
    (dl)
    (if (not (null? d))
	(begin (display-mapping* d)
	       (lncolor red 1)))
    (if (not (null? c))
	(begin (display-mapping* c)
	       (lncolor brown 1)
	       (palette standard)
	       (annot "Fractional Difference (1)" brown 0.01 0.49 0.46 0.48)
	       (annot "Integrated Difference (2)" red 0.51 0.99 0.46 0.48)
	       (annot (sprintf "Min = %s     Max = %s"
			       (car (get-range 1)) (cadr (get-range 1)))
		      brown 0.01 0.49 0.44 0.46)
	       (annot (sprintf "Tot = %s     Ave = %s"
			       (cadr (get-range 2)) (/ (cadr (get-range 2)) vol))
		      red 0.51 0.99 0.44 0.46)
	       (viewport-update)
	       (palette pal)))

    (cv "Data")))

; --------------------------------------------------------------------------
; --------------------------------------------------------------------------

; COMPARE - read in two files and display each pair of matching curves
;          - as prompted
;          - when all have been checked kill all of them and erase the
;          - screen
;          - there is some development to be done here

(define-macro (compare new old . rest)
  (autoplot off)
  (if (not (file? new))
      (printf nil "\nCannot open file %s\n\n" new)
      (if (not (file? old))
	  (printf nil "\nCannot open file %s\n\n" new)
	  (if (ascii-file? new)
	      (printf nil "\nFile %s is ascii\n\n" new)
	      (if (ascii-file? old)
		  (printf nil "\nFile %s is ascii\n\n" old)
		  (let* ((nrst (if rest (length rest) 0))
			 (tolerance (if (> nrst 0)
					(list-ref rest 0)
					TOLERANCE))
			 (batch (if (> nrst 1)
				    (list-ref rest 1)
				    0))
			 (extrema (if (> nrst 2)
				      (list-ref rest 2)
				      #f))
			 (mps     (if (> nrst 3)
				      (list-tail rest 3)
				      nil))
			 (file1 (change-file* new))
			 (file2 (change-file* old))
			 (snew  (if (symbol? new)
				    (symbol->string new)
				    new))
			 (file (string-append snew ".chk"))
			 (log (open-output-file file))
			 n-total n-correct n-differ differing result
			 least-diff greatest-diff)

		    (set! TOLERANCE tolerance)
		    (set! user-stop #f)
	    
		    (define (printl . args)
		      (apply printf (cons nil args))
		      (apply printf (cons log args)))

		    (define-global current-file file1)

		    (define (report-fail differing n-differ n-total new old)
		        (if mps
			    (begin (if (= n-differ 1)
				       (printl "%d mapping of %s differs between %s and %s:\n"
					       n-differ mps new old)
				       (printl "%d mappings of %s differ between %s and %s:\n"
					       n-differ mps new old))
				   (for-each '(lambda (n) (printl "   %s" n))
				      differing))
			    (begin (if (= n-differ 1)
				       (printl "%d mapping out of %d differs between %s and %s:\n"
					       n-differ n-total new old)
				       (printl "%d mappings out of %d differ between %s and %s:\n"
					       n-differ n-total new old))
				   (for-each '(lambda (n) (printl "   %s" n))
				      differing)))
		      (printl "\n"))

		    (define (report-success new old tol)
		        (if mps
			    (printl "Mappings %s in %s and %s agree to within one part in %s\n"
				    mps new old tol)
			    (printl "All mappings in %s and %s agree to within one part in %s\n"
				    new old tol)))

		    (printf nil "\nComparing %s and %s\n" new old)
		    (set! nname (sprintf "%s" new))
		    (set! oname (sprintf "%s" old))

		    (set! result (diff-mappings file1 file2 batch mps))

		    (set! n-correct     (list-ref result 0))
		    (set! differing     (reverse (list-ref result 1)))
		    (set! least-diff    (list-ref result 2))
		    (set! greatest-diff (list-ref result 3))

		    (set! n-differ (if (pair? differing) (length differing) 0))
		    (set! n-total (+ n-correct n-differ))

		    (if (pair? differing)
; some differ
			(report-fail differing n-differ n-total new old)
; all match
			(report-success new old TOLERANCE))

		    (if extrema
			(printl "     %.3e <= differences <= %.3e\n"
				least-diff greatest-diff))

		    (if user-stop
			(if (= batch 1)
			    (printf nil "Files %s and %s differ\n" new old)
			    (printf nil "Comparison terminated by user\n" new old)))

		    (printf nil "\n")

		    (close-output-file log)
		    (and (not user-stop) (= n-differ 0))))))))

; --------------------------------------------------------------------------
; --------------------------------------------------------------------------

