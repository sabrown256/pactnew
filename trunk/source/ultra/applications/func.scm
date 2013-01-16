;
; FUNC.SCM - SCHEME level functions for ULTRA II
;
; Source Version: 4.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
;

;(printf nil "Loading FUNCS.SCM ... ")

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

; AUTOLOAD* - do real autoloading of procedures

(define (autoload* funcname filename)
    "Procedure: loads the specified file when the given procedure is invoked
     and calls the procedure (load on demand).
     Usage: autoload* <procedure> <file>"
    (apply define-global
	   (list (cons funcname 'x)
		 (list load filename #t)
		 (list apply (list eval funcname) 'x))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; COPY-CURVE - make a "copy" of the given curve with the specifed number
;            - of points

(define (copy-curve crv . num)
    "Procedure: Make a \"copy\" of the given curve with an optionally
     specifed number of evenly spaced points.
     This could be done with EXTRACT but not as quickly or conveniently.
     Usage: copy-curve <crv> [<# pts>]"
    (interactive off)
    (let* ((dmn (get-domain crv))
	   (xmn (car dmn))
	   (xmx (cdr dmn))
	   (npts (if num (car num) (default-npts)))
	   (spn (span xmn xmx npts))
	   (tgt (compose crv spn)))
	  (del spn)
	  (set! spn (copy tgt))
	  (del tgt)
	  (label spn (sprintf "Copy of %s (%d)" crv npts))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; EXTREMA - return coordinates of max and min points


(define (extrema . crvs)
   "EXTREMA - return coordinates of max and min points

     Usage: extrema <curve-list>"

   (define (extrema-helper crv)
       (if (not (curve? crv))
	   (printf nil "Argument is not curve: %s\n" crv)
	   (let* ((lst (curve->list crv))
		  (xvals (car lst))
		  (yvals (cadr lst))
		  (mxy -1.e99)
		  (mny 1.e99)
		  xmxy
		  xmny)

	       (define (extreme-one xv yv)
		   (if xv
		       (begin
			   (if (> (car yv) mxy)
			       (begin
				   (set! mxy (car yv))
				   (set! xmxy (car xv))))
			   (if (< (car yv) mny)
			       (begin
				   (set! mny (car yv))
				   (set! xmny (car xv))))
			   (extreme-one (cdr xv) (cdr yv)))
		       (begin
			   (printf nil "Max %s: (%12.5e, %12.5e )\n" crv xmxy mxy)
			   (printf nil "Min %s: (%12.5e, %12.5e )\n" crv xmny mny)
			   (list crv (list xmxy mxy) (list xmny mny)))))

	       (extreme-one xvals yvals))))

   (if crvs
       (begin
	   (if (not (curve? (car crvs)))
	       (set! crvs (car crvs)))
	   (if (pair? crvs)
	       (map extrema-helper crvs)
	       (printf nil "Argument is not curve: %s\n" crvs)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HARDCOPY - send list of curves to hardcopy devices

; Send the current plot or specified list of curves to open hardcopy devices.
; If no argument is specified, send out current plot.
; If argument is `all', send out all curves, one curve per plot.
; If curve list given, send out specified curves, one curve per plot.
; Lists refer to the curve numbers displayed in the menu.
; Lists may contain individual curve numbers and curve ranges.
; A curve range is a space delimited hyphen between two curve numbers.

; Usage:    hardcopy [all | <curve-number-list>]

; Examples: hardcopy
;           hardcopy all
;           hardcopy 1 3 5:10 14

(define-macro (hardcopy . rest)
  "Macro: Send list of curves to hardcopy devices.
     Usage: hardcopy [all | <curve-number-list>]"

  (interactive off)

  (if (and (= (jpeg-flag) 1) (jpeg-name))
      (begin
	(set! jpeg-count 0)
	(set! jpeg-type1 (jpeg-type))
	(set! jpeg-root (jpeg-name))))

;  (define (sequence start length step) 
;    (if (> length 0) (cons start (sequence (+ start step) (- length 1) step)) nil))

  (define (do-hardcopy nc)
    (if (and (integer? nc) (> nc 0) (<= nc (n-curves-read)))
	(begin
	  (erase)
	  (select nc)
	  (if (= (jpeg-flag) 1)
	      (begin
		(close-device "jpeg")
		(set! jpeg-count (+ jpeg-count 1))
		(open-device* "jpeg" jpeg-type1 (sprintf "%s%d" jpeg-root jpeg-count))))
	  (print))))

  (define (do-hardcopy0 nc)
    (cond ((integer? nc) (do-hardcopy nc))
	  ((eqv? 'thru (car nc)) (map do-hardcopy (eval nc)))))

  (if (null? rest)
      (begin
	(if (= (jpeg-flag) 1)
	    (begin
	      (set! jpeg-count (+ jpeg-count 1))
	      (open-device* "jpeg" jpeg-type1 (sprintf "%s%d" jpeg-root jpeg-count))))
	(print))
      (begin
	(if (eqv? 'all (car rest))
	    (set! rest (sequence 1 (n-curves-read) 1)))
	(map do-hardcopy0 rest)))

  (if (= (jpeg-flag) 1)
      (close-device "jpeg")))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HC1

(if (not (defined? ps-count))
    (begin
      (interactive off)
      (define ps-root (ps-name))
      (define ps-flag1 (ps-flag))
      (define ps-type1 (ps-type))
      (define ps-count 0)))
(if (not (defined? cgm-count))
    (begin
      (interactive off)
      (define cgm-root (cgm-name))
      (define cgm-flag1 (cgm-flag))
      (define cgm-type1 (cgm-type))
      (define cgm-count 0)))
(if (not (defined? mpeg-count))
    (begin
      (interactive off)
      (define mpeg-root (mpeg-name))
      (define mpeg-flag1 (mpeg-flag))
      (define mpeg-type1 (mpeg-type))
      (define mpeg-count 0)))
(if (= (jpeg-flag) 1)
    (begin
      (interactive off)))

(define jpeg-root (jpeg-name))
(define jpeg-type1 (jpeg-type))
(define jpeg-count 0)

(define-macro (hc1 . rest)
  "Macro: Send list of curves to hardcopy devices, one plot per file.
     Usage: hc1 [all | <curve-number-list>]"

  (interactive off)

  (if (ps-name)
      (begin
	(set! ps-count 0)
	(set! ps-type1 (ps-type))
	(set! ps-root (ps-name))))
  (if (cgm-name)
      (begin
	(set! cgm-count 0)
	(set! cgm-type1 (cgm-type))
	(set! cgm-root (cgm-name))))
  (if (mpeg-name)
      (begin
	(set! mpeg-count 0)
	(set! mpeg-type1 (mpeg-type))
	(set! mpeg-root (mpeg-name))))
  (if (and (= (jpeg-flag) 1) (jpeg-name))
      (begin
	(set! jpeg-count 0)
	(set! jpeg-type1 (jpeg-type))
	(set! jpeg-root (jpeg-name))))

  (set! ps-flag1 (ps-flag))
  (set! cgm-flag1 (cgm-flag))
  (set! mpeg-flag1 (mpeg-flag))

;  The following procedure is define elsewhere in toolbox.scm
;  (define (sequence start length step) 
;   (if (> length 0)
;	(cons start (sequence (+ start step) (- length 1) step)) nil))

  (define (do-print)
    (if (= ps-flag1 1)
	(begin
	  (close-device "ps")
	  (set! ps-count (+ ps-count 1))
	  (open-device* "ps" ps-type1 (sprintf "%s%d.ps" ps-root ps-count))))
    (if (= cgm-flag1 1)
	(begin
	  (close-device "cgm")
	  (set! cgm-count (+ cgm-count 1))
	  (open-device* "cgm" cgm-type1 (sprintf "%s%d.cgm" cgm-root cgm-count))))
    (if (= mpeg-flag1 1)
	(begin
	  (close-device "mpeg")
	  (set! mpeg-count (+ mpeg-count 1))
	  (open-device* "mpeg" mpeg-type1 (sprintf "%s%d.mpg" ps-root ps-count))))
    (if (= (jpeg-flag) 1)
	(begin
	  (close-device "jpeg")
	  (set! jpeg-count (+ jpeg-count 1))
	  (open-device* "jpeg" jpeg-type1 (sprintf "%s%d" jpeg-root jpeg-count))))
    (print))

  (define (do-hardcopy nc)
    (if (and (integer? nc) (> nc 0) (<= nc (n-curves-read)))
	(begin
	  (erase)
	  (select nc)
	  (do-print))))

  (define (do-hardcopy0 nc)
    (cond ((integer? nc) (do-hardcopy nc))
	  ((eqv? 'thru (car nc)) (map do-hardcopy (eval nc)))))

  (if (null? rest)
      (do-print)
      (begin
	(if (eqv? 'all (car rest))
	    (set! rest (sequence 1 (n-curves-read) 1)))
	(map do-hardcopy0 rest)))

  (if (= ps-flag1 1)
      (close-device "ps"))
  (if (= cgm-flag1 1)
      (close-device "cgm"))
  (if (= mpeg-flag1 1)
      (close-device "mpeg"))
  (if (= (jpeg-flag) 1)
      (close-device "jpeg")))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; KILLP - kill all curves with given prefix
; GOTCHA - This procedure does not always work as advertised, because the
;          association of prefix to filename is neither one-to-one nor
;          constant. Basically, killp is an ill-conceived concept; a killf
;          that kills all curves from a specified file could work.

(define (killp pref)
    "Procedure: Kill all curves from file associated with given prefix.
     Usage: killp <prefix>"

  (interactive off)
  (kill (menu* '* (caddr (prefix* pref)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MERGE - merge the curves from a list of files into a new file

(define-macro (merge tgt . srcs)
  "Macro: Merge the curves from a list of files into a new file.
     Usage: merge <target-file> <source-file-list>"
  (interactive off)
  (if (file? tgt)
      (printf nil "\nFile %s already exists\n\n" tgt)
      (if srcs
	  (let* ((first (+ (n-curves-read) 1)))
	      (for-each rd srcs)
	      (let* ((crvs (thru first (n-curves-read))))
		  (apply save (cons tgt crvs))
		  (kill crvs))))))

(define (merge* tgt . srcs)
  "Procedure: Merge the curves from a list of files into a new file.
     Usage: merge* <target-file> <source-file-list>"
  (apply merge (cons tgt srcs)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; NP - invoke procedure or macro in interactive off (non-printing) mode

(define-macro (np . rest)
  "Macro: Invoke command in non-printing mode.
     Usage: np <command> [<argument-list>]
     Example: np cur (menu t*)"
  (interactive off)
  (eval rest))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PRE - return a curve number given a prefix and an index

(define (pre name index)
  "Procedure: Return a curve number given a prefix and an index.
     Usage: pre <prefix> <index>"
	(interactive off)
	(+ (cadr (prefix* name)) index -1))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PLOTS - switch between text and graphics mode

(define (plots flag)
  "Procedure: Switch between text and graphics mode.
     Usage: plots on|off"
   (if (= flag off)
       (text)
       (begin
         (graphics)
         (replot))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PRINT-MENU - put a convenient user interface to the menu printing
;            - capability

(define-macro (print-menu . name)
  "Macro: Print the current menu of curves to the specified file.
     Usage: print-menu [<file>]"
  (if (null? name)
      (menu "*" "*" "ultra.menu")
      (eval (list menu "*" "*" (car name)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; RE-COLOR

(define (re-color)
    "Procedure: Reset curve colors to preset sequence.
     Usage: re-color"

    ; preset the color sequence to your preference
    ; the list may be any non-zero length, repeats are ok
    (define color-seq (list 4 5 6 7 8 9 10 11 12 13 14 15 1 2 3))

    (define (aux raz rc)
      (if raz
	  (if (curve? (car raz))
	      (begin
		(if (not rc)
		    (set! rc color-seq))
		(color (car raz) (car rc))
		(aux (cdr raz) (cdr rc)))
	      (aux (cdr raz) rc))))

    (aux (list a b c d e f g h i j k l m n o p q r s t u v w x y z) nil))
	      
;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; re-id replaced with a C level routine to re-id an arbitrary number of 
;       curves (not just a-z)  lee minner 3/9/2005
; RE-ID

;(define (re-id)
;    "Procedure: Reset data-id's to A, B, C, ... (no skipping)
;     Usage: re-id"

  ; return sub-list (saz) beginning with first id not in use
;  (define (aux1 saz)
;    (if saz
;	(if (curve? (car saz))
;	    (aux1 (cdr saz))
;	    saz)))

  ; move curves in remaining list (raz) to first id's not in use
;  (define (aux2 raz saz)
;    (if raz
;	(let* ((crv (car raz)))
;	  (if (curve? crv)
;	      (set-id crv (car saz)))
;	  (aux2 (cdr raz) (aux1 saz)))))

;  (define faz (aux1 (list a b c d e f g h i j k l m n o p q r s t u v w x y z)))
;  (aux2 faz faz))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

;RESUME

(define (resume)
  (return-level 1 nil))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SCREEN - swith out and back into graphics mode to make changes in
;        - window attributes take effect

(define-macro (screen . name)
  "Macro: Reset the display and make display variable setting changes
     take effect.  Optional screen name causes window to be drawn at a
     remote workstation.
     Usage: screen [<screen>]"
   (if (not (null? name))
       (display-name (car name)))
   (if (and (not (null? name)) (not (null? (cdr name))))
       (display-title (cadr name)))
   (text)
   (graphics)
   (replot))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SPAN - generate a line of slope one

(define (span low high . num)
    "Procedure: generates a straight line of slope 1 and y intercept 0
     in the specified domain with an optional number of points.
     Usage: span <xmin> <xmax> [<# pts>]"
    (if (null? num)
        (line 1 0 low high)
        (line 1 0 low high (car num))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; VS - plot A vs B

(define (vs c1 c2)
   "Procedure: Plot the range of the first curve
                against the range of the second curve.
     Usage: vs <curve1> <curve2>"
   (let* ((tmp (copy c1))
	  (dmn (get-common-domain c1 c2)))
         (if dmn
	     (let* ((xn (car dmn))
		    (xx (cdr dmn))
		    (ci1 (xmm c1 xn xx))
		    (ci2 (rev (xmm c2 xn xx))))
	           (del tmp)
		   (set! tmp
			 (label (compose ci1 ci2)
				(sprintf "%s vs %s" c1 c2)))
		   (del ci1 ci2)
		   tmp))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; XMAX

(define (xmax . crvs_limit)
    "Procedure: Filter out points in curves whose x-values > limit.
     Usage: xmax <curve-list> limit"
  (handle-crvs-limit crvs_limit <= nil))

; XMIN

(define (xmin . crvs_limit)
    "Procedure: Filter out points in curves whose x-values < limit.
     Usage: xmin <curve-list> limit"
  (handle-crvs-limit crvs_limit > nil))

; YMAX

(define (ymax . crvs_limit)
    "Procedure: Filter out points in curves whose y-values > limit.
     Usage: ymax <curve-list> limit"
  (handle-crvs-limit crvs_limit nil <=))

; YMIN

(define (ymin . crvs_limit)
    "Procedure: Filter out points in curves whose y-values < limit.
     Usage: ymin <curve-list> limit"
  (handle-crvs-limit crvs_limit nil >))

(define (handle-crvs-limit crvs_limit dom-op ran-op)
  ; handler for filter procs that take a curve list and limit
  (let* ((limit_crvs (reverse crvs_limit))
	 (limit (car limit_crvs))
	 (crvs (reverse (cdr limit_crvs)))
	 (dom-pred (if dom-op (lambda (x) (dom-op x limit)) (lambda (x) #t)))
	 (ran-pred (if ran-op (lambda (x) (ran-op x limit)) (lambda (x) #t))))
    (apply filter (append crvs (list dom-pred ran-pred)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

;                                   EXTRACT

;--------------------------------------------------------------------------

; EXTRACT - let the user extract points from a curve at specified intervals
;         - and make a curve out of them

(define (extract crv . rest)
    "Procedure: Extract points from a curve and make a new curve from them.
     This function takes either the number of points desired or a set of
     triples containing start, stop, and step values.
     Usage: extract <curve> <npoints> | [<xstart> <xstop> <xstep>]+"
    (interactive off)
    (if (curve? crv)
        (begin

	  (if (and (pair? rest) (= (length rest) 1)
		   (integer? (car rest)) (> (car rest) 1))
	      (let* ((dn (car (get-domain crv)))
		     (dm (cdr (get-domain crv)))
		     (st (/ (- dm dn) (- (car rest) 1))))
		    (set! rest (list dn dm st))))

	  (if (pair? rest)
	      (let* ((hold (line 1 0 0 1 2))    ; this is a place holder
		     (new (extract-level crv rest nil nil)))
		    (del hold)
		    (set! hold (copy new))
		    (del new)
		    (label hold (sprintf "Interpolated %s" crv)))

	      (printf nil "\nIllegal number of specifications\n\n")))

	(printf nil "\n%s is not a curve\n\n" crv)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; EXTRACT-LEVEL - recursivley process the point extraction specifications
;               - for the EXTRACT command

(define (extract-level crv specs new)
    (let* ((long (if (pair? specs) (length specs) 0)))
          (cond ((= long 0)
		 new)
		((>= long 3)
		 (let* ((dn (car (get-domain crv)))
			(dm (cdr (get-domain crv)))
			(xn (max dn (car specs)))
			(xx (min dm (cadr specs)))
			(st (caddr specs))
			(fnc (if (procedure? crv) crv (eval crv)))
			(ncv (extract-curve fnc xn xx st))
			(acv (if new
				 (append-curves new ncv)
				 (copy ncv))))
		     (if new
			 (del new))
		     (set! new acv)
		     (del ncv)
		     (extract-level crv (cdddr specs) new)))
		  (else
		   (printf nil
			   "\nIllegal number of specifications: %s\n\n"
			   specs)))))

;--------------------------------------------------------------------------

;                                    SMOOTH

;--------------------------------------------------------------------------

; SMO - the most general interface to the curve smoothing functionality

(define (smo . argl)
  "Procedure: Smooth curves using user specified smooth-method.
     For \"averaging\" uses an n point integral to average.
     For \"fft\" uses a Lorentzian filter with coefficient n/n_pts.
     Usage: smo <curve-list> <n> [<ntimes>]"
  (if (null? argl)
      (printf nil "\nNo curves specified\n\n")
      (if (equal? (length argl) 1)
	  (printf nil "\nCurve list or number of points not specified\n\n")
	  (let ((lgra (reverse argl)))
	    (if (not (integer? (car lgra)))
		(printf nil "\nNumber of points not specified\n\n")
		(if (curve? (cadr lgra))
		    (apply smoothn (reverse (cons 1 lgra)))
		    (apply smoothn (reverse lgra))))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SMOOTH3 - the 3 point smooth

(define (smooth3 . argl)
  "Procedure: Smooth curves using user specified smooth-method.
     For \"averaging\" uses an 3 point integral to average.
     For \"fft\" uses a Lorentzian filter with coefficient 3/n_pts.
     Usage: smooth3 <curve-list> [<ntimes>]"
  (if (null? argl)
      (printf nil "\nNo curves specified\n\n")
      (let ((lgra (reverse argl)))
	(if (curve? (car lgra))
	    (apply smoothn (reverse (cons 1 (cons 3 lgra))))
	    (apply smoothn (reverse (cons (car lgra) (cons 3 (cdr lgra)))))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SMOOTH5 - the 5 point smooth

(define (smooth5 . argl)
  "Procedure: Smooth curves using user specified smooth-method.
     For \"averaging\" uses an 5 point integral to average.
     For \"fft\" uses a Lorentzian filter with coefficient 5/n_pts.
     Usage: smooth5 <curve-list> [<ntimes>]"
  (if (null? argl)
      (printf nil "\nNo curves specified\n\n")
      (let ((lgra (reverse argl)))
	(if (curve? (car lgra))
	    (apply smoothn (reverse (cons 1 (cons 5 lgra))))
	    (apply smoothn (reverse (cons (car lgra) (cons 5 (cdr lgra)))))))))

;--------------------------------------------------------------------------

;                        MISCELLANEOUS

;--------------------------------------------------------------------------

; GAUSSIAN - gaussian curve generator
;          - Usage: gaussian <amplitude> <FWHM> <x position of peak>
;          -        [<# points> [<# half-widths>]]
;          -
;          - Note: # of points defaults to 100
;          -       # of half-widths defaults to 3

(define (gaussian amp wid center . rest)
   "Procedure: Generate a gaussian function.
     Usage: gaussian <amplitude> <width> <center> [<# points> [<# half-widths>]]"
   (let* ((num (if (and rest (< 0 (length rest)))
		   (list-ref rest 0)
		   100))
	  (nsd (if (and rest (< 1 (length rest)))
		   (list-ref rest 1)
		   3))
	  (crv-min (- center (* nsd wid)))
	  (crv-max (+ center (* nsd wid)))
	  (cc (span crv-min crv-max num))
	  (dd (- cc center)))
         (del cc)
	 (set! cc (* dd dd -1))
	 (del dd)
	 (set! dd (exp (/ cc (* wid wid))))
	 (del cc)
	 (set! cc (* dd amp))
	 (del dd)
	 (label cc "Gaussian")))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; LORENTZIAN - lorentzian curve generator
;            - Usage: lorentzian <amplitude> <FWHM> <x position of peak>
;            -        [<# points> [<# half-widths>]]
;            -
;            - Note: # of points defaults to 100
;            -       # of half-widths defaults to 3

(define (lorentzian amp wid center . rest)
   "Procedure: Generate a lorentzian function.
     Usage: lorentzian <amplitude> <width> <center> [<# points> [<# half-widths>]]"
   (let* ((num (if (and rest (< 0 (length rest)))
		   (list-ref rest 0)
		   100))
	  (nsd (if (and rest (< 1 (length rest)))
		   (list-ref rest 1)
		   3))
	  (crv-min (- center (* nsd wid)))
	  (crv-max (+ center (* nsd wid)))
	  (cc (span crv-min crv-max num))
	  (dd (- cc center)))
         (del cc)
	 (set! cc (* dd dd))
	 (del dd)
	 (set! dd (recip (dy (/ cc (* wid wid)) 1.0)))
	 (del cc)
	 (set! cc (* dd amp))
	 (del dd)
	 (label cc "Lorentzian")))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DIFFRACTION - diffraction pattern generator

(define (diffraction size . npts)
   "Procedure: Compute a diffraction pattern for a circular aperature.
     Usage: diffractions <radius> [<n-points>]"
    (let* ((num (if npts (car npts) 100))
	   (cv1 (span 0.0001 size num))
	   (cv2 (copy cv1))
	   (cv3 (sqr (/ (j1 cv1) cv2))))
         (del cv1 cv2)
         (set! cv1 (copy cv3))
         (del cv3)
         (label cv1 "Diffraction pattern")))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; THETA - step function generator

(define (theta xmin xstep xmax . npts)
    "Procedure: generate a unit step distribution such that
                d(theta(t - x0))/dt = delta(t - x0)
     Usage: theta <xmin> <x0> <xmax> [<# points>]"
    (let* ((num (if npts (car npts) 128))
	   (dxi (- xmax xmin))
	   (dxl (- xstep xmin))
	   (dxr (- xmax xstep))
	   (numl (* num (/ dxl dxi)))
	   (numr (* num (/ dxr dxi)))
	   (nml (if (> 2 numl) 2 numl))
	   (nmr (if (> 2 numr) 2 numr))
	   (crvl (line 0 0 xmin (- xstep (* dxi 5.0e-10)) nml))
	   (crvr (line 0 1 (+ xstep (* dxi 5.0e-10)) xmax nmr))
	   (crv (append-curves crvl crvr)))
         (del crvl crvr)
         (set! crvl (copy crv))
	 (del crv)
         (label crvl "Theta")))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DELTA - Dirac delta distribution generator

(define (delta xmin x0 xmax . npts)
    "Procedure: generate a Dirac delta distribution such that
                Int(xmin, xmax, dt*delta(t - x0)) = 1
     Usage: delta <xmin> <x0> <xmax> [<# points>]"
    (let* ((num (if npts (car npts) 128))
	   (dxt (- xmax xmin))
	   (dxi (/ dxt num))
	   (dxl (- x0 xmin))
	   (dnl (truncate (/ dxl dxi)))
	   (xv1 (+ (* dxi dnl) xmin))
	   (xv2 (+ xv1 dxi))
	   (ds (* dxi dxi))
	   (yv1 (/ (- xv2 x0) ds))
	   (yv2 (/ (- x0 xv1) ds))
	   (dxr (- xmax x0))
	   (numl (- dnl 1))
	   (numr (- (- num 2) numl))
	   (crvl (line 0 0 xmin (- xv1 dxi) numl))
	   (crvr (line 0 0 (+ xv2 dxi) xmax numr))
	   (crvm (make-curve* (list xv1 xv2) (list yv1 yv2)))
	   (crv (append-curves crvl crvm crvr)))
         (del crvl crvm crvr)
         (set! crvl (copy crv))
	 (del crv)
         (label crvl "Dirac Delta")))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; FODE - first order lineear ode solver

(define (fode a b xmin xmax)
    "Procedure: solves the first order differential equation
                y' + ay = b
     giving both the homogeneous and particular solutions.
     Usage: fode <a> <b> <xmin> <xmax>"
    (let* ((integrand-a (copy a))
	   (particular-y (copy a))
	   (homogeneous-y (copy a))
	   (integral-a (integrate integrand-a xmin xmax))
	   (expia (exp (copy integral-a)))
	   (expmia (exp (my (copy integral-a) -1)))
	   (integrand (* b expia)))
; 
; compute the homogeneous solution
; 
          (del expia integral-a homogeneous-y integrand-a)
	  (set! homogeneous-y (copy expmia))
	  (label homogeneous-y "Homogeneous solution")
; 
; compute the particular solution
; 
	  (set! expia (integrate integrand xmin xmax))
	  (del particular-y)
	  (set! particular-y (* expmia expia))
	  (label particular-y "Particular solution")

	  (del expia expmia integrand)
	  (list homogeneous-y particular-y)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; COMPLEX-CONJUGATE - return the pair of curve whose elements represent
;                   - the complex conjugates of the given pair of curves
;                   - representing the real and imaginary components of
;                   - a curve of complex number

(define (complex-conjugate ca)
    (list (car ca) (my (cadr ca) -1.0)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; COMPLEX-TIMES - return the pair of curve whose elements represent
;               - the complex product of the given pairs of curves
;               - representing the real and imaginary components of
;               - two curves of complex numbers

(define (complex-times ca cb)
    (let* ((ra (car ca))
	   (ia (cadr ca))
	   (rb (car cb))
	   (ib (cadr cb))
	   (rs1 (* ra rb))
	   (rs2 (* ia ib))
	   (is1 (* ra ib))
	   (is2 (* ia rb))
	   (sa (- rs1 rs2))
	   (sb (+ is1 is2)))
      (del rs1 rs2 is1 is2)
      (list sa sb)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; NORMALIZE - create a normalized version of the given curve

(define (normalize crv)
    "Procedure: construct a new curve with unit area in its domain.
     Usage: normalize crv"
   (interactive off)
   (let* ((icrv (integrate crv -1.e100 1.e100))
	  (domi (get-domain icrv))
	  (norm (/ 1.0 ((eval icrv) (cdr domi)))))
     (del icrv)
     (label (* crv norm) (sprintf "Normalized %s" crv))))
	    
;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; CONVOL-INT - do a slower convolution which doesn't use FFT's
;            - and hence avoids the padding and aliasing problems

(define (convol-int nrm cg ch rest)
    "Procedure: computes the convolution of the two given curves
                (g*h)(x) = Int(-inf, inf, dt*g(t)*h(x-t))
     This computes the integrals directly which avoid padding and aliasing
     problems associated with FFT methods (it is however slower).
     If NRM is #t then H is normalized to unit area.
     Usage: convol-int <nrm> <g> <h> [# points]"
    (interactive off)
    (let* ((ct (mx (if nrm (normalize ch) (copy ch)) -1))
	   (domg (get-domain cg))
	   (domh (get-domain ct))
	   (xmn (- (car domg) (cdr domh)))
	   (xmx (- (cdr domg) (car domh)))
	   (npt (if (pair? rest)
		    (car rest)
		    (default-npts)))
	   (delx (/ (- xmn xmx) npt))
	   (zot (label (dx (copy ct) (+ xmx delx)) "foo")))

      (define (integ crg crh xcur xval yval)
	(if (> xcur xmn)
	    (let* ((crb (* crg crh))
		   (cra (integrate crb -1.e100 1.e100))
		   (yva (if (curve? cra)
;			    (cdr (get-range cra))
			    (car (list-ref (gety cra (cdr (get-domain cra))) 2))
			    0)))
	      (if (curve? cra)
		  (del cra crb))
	      (integ crg (dx crh delx) (+ xcur delx)
		     (cons xcur xval)
		     (cons yva yval)))
	    (make-curve*  xval yval)))

      (del ct)
      (set! ct (integ cg zot (+ xmx delx) nil nil))
      (del zot)
      (label ct (sprintf "Conv %s*%s (Int)" cg ch))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; CONVOLB - do a slower convolution which doesn't use FFT's
;         - and hence avoids the padding and aliasing problems

(define (convolb cg ch . rest)
    "Procedure: computes the convolution of the two given curves
                (g*h)(x) = Int(-inf, inf, dt*g(t)*h(x-t))/
                           Int(-inf, inf, dt*h(t))
     This computes the integrals directly which avoid padding and aliasing
     problems associated with FFT methods (it is however slower).
     Usage: convolb <g> <h> [# points]"
    (convol-int #t cg ch rest))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; CONVOLC - do a slower convolution which doesn't use FFT's
;         - and hence avoids the padding and aliasing problems

(define (convolc cg ch . rest)
    "Procedure: computes the convolution of the two given curves
                (g*h)(x) = Int(-inf, inf, dt*g(t)*h(x-t))
     This computes the integrals directly which avoid padding and aliasing
     problems associated with FFT methods (it is however slower).
     Usage: convolc <g> <h> [# points]"
    (convol-int #f cg ch rest))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; CONVOL - compute and return the convolution of two real curves:
;        -
;        -    (g*h)(x) = Int(-inf, inf, dt, g(t)*h(x-t))
;        -
;        - this is carried out as the inverse FFT of the product of
;        - the FFT's of the two input curves

(define (convol cg ch)
    "Procedure: computes the convolution of the two given curves
                (g*h)(x) = Int(-inf, inf, dt*g(t)*h(x-t))
     Usage: convol <g> <h>"
    (interactive off)
    (let* ((domn (get-domain cg))
	   (delx (- (cdr domn) (car domn)))
	   (norm (/ delx 4))
	   (ftg (fft cg))
	   (fth (fft ch))
	   (ftc (complex-times ftg fth)))
      (del (car ftg) (cadr ftg))
      (del (car fth) (cadr fth))
      (set! ftg (ifft (car ftc) (cadr ftc)))
      (del (car ftc) (cadr ftc))
      (my (car ftg) norm)
      (label (car ftg) (sprintf "Conv %s*%s (FFT)" cg ch))
      (del (cadr ftg))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; CORREL - compute and return the correlation of two real curves:
;        -
;        -    Corr(g,h)(x) = Int(-inf, inf, dt, g(t+x)*h(t))
;        -
;        - this is carried out as the inverse FFT of the product of
;        - the FFT of the first curve with the complex conjugate of
;        - the FFT of the second curve
;        - NOTE: this only works for real curves where
;        -       H(-w) = Conjugate(H(w))

(define (correl cg ch)
    "Procedure: computes the correlation function of the two given curves
                Corr(g, h)(x) = Int(-inf, inf, dt*g(t+x)*h(t))
     Usage: correl <g> <h>"
    (interactive off)
    (let* ((domn (get-domain cg))
	   (delx (- (cdr domn) (car domn)))
	   (norm (/ delx 2))
	   (ftg (fft cg))
	   (fth (complex-conjugate (fft ch)))
	   (ftc (complex-times ftg fth)))
      (del (car ftg) (cadr ftg))
      (del (car fth) (cadr fth))
      (set! ftg (ifft (car ftc) (cadr ftc)))
      (del (car ftc) (cadr ftc))
      (my (car ftg) norm)
      (label (car ftg) (sprintf "Corr %s.%s (FFT)" cg ch))
      (del (cadr ftg))))

;--------------------------------------------------------------------------

;                  FILTERS FOR FILTER-COEF OR SMOOTH

;--------------------------------------------------------------------------

(define tchebyshev
    (make-filter  5  1.0     0.0     0.0
                     0.125   0.6875  0.25   -0.0625
                    -0.0625  0.25    0.625   0.25    -0.0625
                            -0.0625  0.25    0.6875   0.125
                                     0.0     0.0      1.0))

(define least-sqr
    (make-filter  5  1.0       0.0        0.0
                     (/ 6 35)  (/ 21 35)  (/ 12 35)  (/ -3 35)
                     (/ -3 35) (/ 12 35)  (/ 17 35)  (/ 12 35)  (/ -3 35)
                               (/ -3 35)  (/ 12 35)  (/ 21 35)  (/ 6 35)  
                                          0.0        0.0        1.0))

;--------------------------------------------------------------------------

;                       POINTWISE OPERATORS

;--------------------------------------------------------------------------

(define (pointwise-oper-list yo yn t oper)
    (if yo
	(let* ((yc (list-ref yo 0))
	       (yr (list-tail yo 1))
	       (yt (oper t yc)))
	      (pointwise-oper-list yr (cons yt yn) yt oper))
	(reverse yn)))
			   
(define (pointwise-oper crv oper ident)
    (let* ((xy (curve->list crv))
	   (x  (list-ref xy 0))
	   (y  (list-ref xy 1)))

	 (make-curve* x (pointwise-oper-list y nil 0.0 +))))

(define (pointwise+ crv)
    "Procedure: Make a new curve with consisting of
     the pointwise sum of the y values of the specified curve.
     Usage: pointwise+ <crv>"
    (pointwise-oper crv + 0.0))

(define (pointwise- crv)
    "Procedure: Make a new curve with consisting of
     the pointwise difference of the y values of the specified curve.
     Usage: pointwise- <crv>"
    (pointwise-oper crv - 0.0))

(define (pointwise* crv)
    "Procedure: Make a new curve with consisting of
     the pointwise product of the y values of the specified curve.
     Usage: pointwise* <crv>"
    (pointwise-oper crv * 1.0))

(define (pointwise/ crv)
    "Procedure: Make a new curve with consisting of
     the pointwise quotient of the y values of the specified curve.
     Usage: pointwise/ <crv>"
    (pointwise-oper crv / 1.0))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (pairwise-oper-list yo yn oper)
    (if (> (length yo) 1)
	(let* ((y1 (list-ref yo 0))
	       (y2 (list-ref yo 1))
	       (yr (list-tail yo 1))
	       (yt (oper y1 y2)))
	      (pairwise-oper-list yr (cons yt yn) oper))
	(reverse yn)))

(define (pairwise-oper crv oper)
    (let* ((xy (curve->list crv))
	   (x  (list-ref xy 0))
	   (y  (list-ref xy 1)))

	 (make-curve* (cdr x) (pairwise-oper-list y nil oper))))

(define (pairwise+ crv)
    "Procedure: Make a new curve with one fewer points consisting of
     the pairwise sum of the y values of the specified curve.
     Usage: pairwise+ <crv>"
    (pairwise-oper crv +))

(define (pairwise- crv)
    "Procedure: Make a new curve with one fewer points consisting of
     the pairwise difference of the y values of the specified curve.
     Usage: pairwise- <crv>"
    (pairwise-oper crv -))

(define (pairwise* crv)
    "Procedure: Make a new curve with one fewer points consisting of
     the pairwise product of the y values of the specified curve.
     Usage: pairwise* <crv>"
    (pairwise-oper crv *))

(define (pairwise/ crv)
    "Procedure: Make a new curve with one fewer points consisting of
     the pairwise quotient of the y values of the specified curve.
     Usage: pairwise/ <crv>"
    (pairwise-oper crv /))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; LDER - logarithmic derivative

(define (lder crvs)
    "Procedure: take the logarithmic derivative of the specified curves"
    (map (lambda (cv) (der (ln cv))) crvs))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; VS - versus command

(define (vs c1 c2)
    "Procedure: Plot the range of the first curve against the range of the
     second curve.
     Usage: vs <curve> <curve>"
    (let* ((tmp (copy c1))
	   (dmn (get-common-domain c1 c2)))
          (if dmn
	      (let* ((xn (car dmn))
		     (xx (cdr dmn))
		     (ci1 (xmm c1 xn xx))
		     (ci2 (rev (xmm c2 xn xx))))
		    (del tmp)
		    (set! tmp
			  (label (compose ci1 ci2)
				 (sprintf "%s vs %s" c1 c2)))
		    (del ci1 ci2)
		    tmp))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

;(printf nil "done\n")
