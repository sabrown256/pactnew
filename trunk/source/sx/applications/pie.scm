;
; PIE.SCM - pie diagram renderer for PDBView
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(load "pdbvplt.scm")

(define current-palette "spectrum")
(define current-range nil)
(define current-domain nil)
(define current-label nil)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MIN-MAX - return a pair containing the minimum and maximum values from
;         - the given list
;         - it also take an initial min and max limit

(define (min-max x mn mx)
    (if x
	(let* ((xv (car x)))
	  (if (pair? xv)
	      (let* ((lims (min-max xv mn mx)))
		(min-max (cdr x) (car lims) (cdr lims)))
	      (min-max (cdr x)
		       (if (< xv mn) xv mn)
		       (if (< mx xv) xv mx))))
	(cons mn mx)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; ITER - make a list of the first n numbers

(define (iter n lst)
    (if (>= n 0)
	(iter (- n 1) (cons n lst))
	lst))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; BUILD-INDEX-ARRAY - return an array of index numbers
;                   - to be used as a domain

(define (build-index-array x)
    (let* ((len (length x)))
      (iter len nil)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; BUILD-ARRAY - return the values of the named file variable as a list

(define-macro (build-array x)
    (build-array* x))

(define (build-array* x)
    (if x
	(let* ((data (read-pdbdata current-file x)))
	  (if data
	      (let* ((vals (cddr (pdb->list data))))
		(cond ((number? vals)
		       vals)
		      ((c-array? vals)
		       (pm-array->list vals))
		      ((vector? vals)
		       (vector->list vals))))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PALETTE-MAP - return a pair containing the slope and intercept of
;             - the linear transformation from variable value in X
;             - to color map index

(define (palette-map x vmn vmx)
    (let* ((pal (pg-palette->list WIN-device current-palette))
	   (len (- (/ (length pal) 3) 2)))
      (cons (/ len (- vmx vmn)) vmn))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PALETTE-ARRAY - convert the given list of real values into a list
;               - of color map indeces

(define (palette-array x vmn vmx)
    (let* ((mm (palette-map x vmn vmx))
	   (a (car mm))
	   (b (cdr mm)))
      (map (lambda (x) (+ 2 (floor (* a (- x b))))) x)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PIE-PALETTE - set the palette for a pie plot

(define-macro (pie-palette pal)
    "PIE-PALETTE - Set the palette for a pie plot or sequence and redraw.
     Usage: pie-palette <palette>
     Examples: pie-palette iron
               pie-palette rgb"
    (set! current-palette pal)
    (pie-seq))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PIE-HELP - print some help for pie plotting package

(define (pie-help)
    "PIE-HELP - print out basic information for the pie plotting package.
     Usage: pie-help
     Examples: pie-help"

    (printf nil "\n")
    (printf nil "PIE         - Plot a pie slice rendering of the specified variable(s).\n")
    (printf nil "PIE-HC      - Make a hardcopy of the current pie plot or sequence.\n")
    (printf nil "PIE-LABEL   - Set the label for a pie plot or sequence and redraw.\n")
    (printf nil "PIE-PALETTE - Set the palette for a pie plot or sequence and redraw.\n")
    (printf nil "PIE-SEQ     - Plot a sequence of pie slice renderings.\n"))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PIE-LABEL - set the label for a pie plot

(define (pie-label s)
    "PIE-LABEL - Set the label for a pie plot or sequence and redraw.
     Usage: pie-label <label>
     Examples: pie-label \"Some stuff\""
    (set! current-label s)
    (pie-seq))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PIE-HC - make hardcopy of a pie plot

(define-macro (pie-hc . rest)
    "PIE-HC - Make a hardcopy of the current pie plot or sequence.
              Monochrome is the default but the optional color flag
              produces color hardcopy.
     Usage: pie-hc [color]
     Examples: pie-hc
               pie-hc color"
    (interactive off)
    (let* ((psflag (ps-flag))
	   (cgmflag (cgm-flag))
	   (color (cond ((null? rest)
			 "MONOCHROME")
			((eqv? (print-name (car rest)) "color")
			 "COLOR")
			(else
			 "MONOCHROME")))
	   (res (if (and rest (> (length rest) 1))
		    (list-ref rest 1)
		    0)))

      (if (= psflag 1)
	  (begin
	    (if (not ps-device)
		(begin
		  (set! ps-device
			(pg-make-device "PS"
					color
					"plots"))
		  (pg-open-device ps-device
				  (window-origin-x-ps) (window-origin-y-ps)
				  (window-width-ps) (window-height-ps))))
	    (pie-update ps-device
			current-label
			current-range current-domain)))

      (if (= cgmflag 1)
	  (begin
	    (if (not cgm-device)
		(begin
		  (set! cgm-device
			(pg-make-device "CGM"
					color
					"plots"))
		  (pg-open-device cgm-device
				  (window-origin-x-cgm) (window-origin-y-cgm)
				  (window-width-cgm) (window-height-cgm))))
	    (pie-update cgm-device
			current-label
			current-range current-domain)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PIE - plot a single pie slice

(define-macro (pie ran . rest)
    "PIE - Plot a pie slice rendering of the specified variable(s).
           An optional domain variable may be specified.
     Usage: pie <range-var> [<domain-var>]
     Examples: pie yval0
               pie yval0 xval0"
    (let* ((dom (if (and rest (> (length rest) 0))
		    (list-ref rest 0)))
	   (label (if dom
		      (sprintf "%s->%s" dom ran)
		      (sprintf "%s" ran)))

	   (ra (build-array* ran))
	   (da (if dom
		   (build-array* dom)
		   (build-index-array ra))))

      (if (and ra da)
	  (begin (if (not WIN-device)
		     (wm-win-open))
		 (pie-update WIN-device label (list ra) da))
	  (printf nil "\nBad component sets\n"))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PIE-SEQ - plot a sequence of pie slices

(define-macro (pie-seq . args)
    "PIE-SEQ - Plot a sequence of pie slice renderings of the
               specified variables.  The sequence proceeds in a
               counter-clockwise direction from the positive y axis
               around to the positive x axis (i.e. 270 degrees).
               If no arguments are specified the last arguments
               are used. This is to make experimentation with
               palettes and making hardcopy easier.
     Usage: pie-seq [<domain-var> [<range-var>]*]
     Examples: pie-seq xval0 yval0 yval1 yval2 yval3 yval4
               pie-seq"
    (if args
	(let* ((ran (list-tail args 1))
	       (dom (list-ref args 0))
	       (ra (map build-array ran))
	       (da (build-array* dom)))

	  (if (not WIN-device)
	      (wm-win-open))

	  (if (not current-label)
	      (set! current-label (sprintf "%s->%s"
					   dom (car ran))))

	  (if (and ra da)
	      (pie-update WIN-device current-label ra da)
	      (printf nil "\nBad component sets\n")))

	(if (and current-range current-domain)
	    (pie-update WIN-device
			current-label
			current-range current-domain))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PIE-UPDATE - pie sequence worker
;            - it takes a device, a label, a domain, and a list of ranges
;            - the domain and ranges are saved for future calls
;            - this draws axes and labels the plot

(define (pie-update dev label ra da)
    (let* ((nr (length ra))
	   (dang (if (> nr 1) (/ 270 nr) 30))
	   (ang0 (if (> nr 1) 90 75))

	   (deg-rad (/ (acos -1.0) 180.0))
	   (a0 (* deg-rad ang0))

	   (r (list-ref da (- (length da) 1)))
	   (rx (* r (cos a0)))
	   (ry (* r (sin a0)))

	   (xmn (- r))
	   (xmx r)
	   (ymn (if (> nr 1) xmn 0.0))
	   (ymx xmx))

      (set! current-range ra)
      (set! current-domain da)
      (set! current-label label)

      (let* ((extr (min-max ra 1.0e100 -1.0e100))
	     (vmn (car extr))
	     (vmx (cdr extr)))
	(pg-set-world-coordinate-system! dev xmn xmx ymn ymx)
	(pg-clear-window dev)
	(pg-set-palette! dev current-palette)
	(pie-wedge dev ra da ang0 dang vmn vmx)
	(pg-set-line-color! dev 0)
	(pg-draw-axis dev
		      0.0 0.0 rx ry
		      0.0 1.0
		      (list-ref da 0) r
		      1.0  "%10.2e"
		      9 9 7)
	(pg-draw-palette dev
			 0.2 0.1 0.8 0.1
			 vmn vmx 0.02)
	(pg-center-label dev 0.95 label)
	(pg-finish-plot dev))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PIE-WEDGE - draw a single pie slice in a sequence

(define (pie-wedge dev ran dom a0 da vmn vmx)
    (if ran
	(let* ((deg-rad (/ (acos -1.0) 180.0))
	       (ang1 a0)
	       (ang2 (+ a0 da))
	       (a1 (* deg-rad ang1))
	       (a2 (* deg-rad ang2))
	       (ca1 (cos a1))
	       (sa1 (sin a1))
	       (ca2 (cos a2))
	       (sa2 (sin a2))
	       (ra (car ran)))
	  (pie-plot dev dom (palette-array ra vmn vmx)
		    ca1 sa1 ca2 sa2)
	  (pie-wedge dev (cdr ran) dom ang2 da vmn vmx))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PIE-PLOT - draw a single pie slice with specified variables and angles

(define (pie-plot dev x y ca1 sa1 ca2 sa2)
   (if (and y x (> (length x) 1))
       (let* ((x1 (list-ref x 0))
	      (x2 (list-ref x 1))
	      (y1 (list-ref y 0))
	      (rx (list-tail x 1))
	      (ry (list-tail y 1)))
	 (draw-pie-zone dev x1 x2 y1 ca1 sa1 ca2 sa2)
	 (pie-plot dev rx ry ca1 sa1 ca2 sa2))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DRAW-PIE-ZONE - draw a single zone of a pie slice

(define (draw-pie-zone dev r1 r2 v1 ca1 sa1 ca2 sa2)
    (let* ((x1 (* r1 ca1))
	   (y1 (* r1 sa1))
	   (x2 (* r2 ca1))
	   (y2 (* r2 sa1))
	   (x3 (* r2 ca2))
	   (y3 (* r2 sa2))
	   (x4 (* r1 ca2))
	   (y4 (* r1 sa2)))
      (pg-fill-polygon dev v1
		       x1 y1
		       x2 y2
		       x3 y3
		       x4 y4
		       x1 y1)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

