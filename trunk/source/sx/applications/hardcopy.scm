; 
; HARDCOPY.SCM - PGS based hardcopy commands for PDBView and ULTRA
; 
; #include <cpyright.h>
; 

;--------------------------------------------------------------------------

;                           PDBVIEW ROUTINES

;--------------------------------------------------------------------------

; HARDCOPY-VIEWPORT - send the current graph to current hard copy devices

(define-macro (hardcopy-viewport . rest)
    "HARDCOPY-VIEWPORT - Send the current graph to all open hardcopy devices.
                         Color output is indicated by an optional argument on
                         the first call for a given device.
                         The optional resolution scale factor is an integer
                         factor by which the resolution will be decreased
                         below the full resolution of the device.
     Usage: hardcopy-viewport [color | monochrome] [portrait | landscape] [resolution-scale-factor]]
     Examples: hardcopy-viewport
               hardcopy-viewport color
               hardcopy-viewport color 8"
    (interactive off)
    (let* ((psflag (ps-flag))
	   (cgmflag (cgm-flag))
	   (mpegflag (mpeg-flag))
	   (jpegflag (jpeg-flag))
	   (pngflag (png-flag))
	   (args (process-hc-args rest))
	   (color (list-ref args 0))
	   (mode (list-ref args 1))
	   (res (list-ref args 2))
	   (type (sprintf "%s %s" color mode)))

      (if (= jpegflag 1)
	  (begin
	    (if (not jpeg-device)
		(begin
		  (set! jpeg-device
			(pg-make-device "JPEG"
					type))
		  (pg-open-device jpeg-device
				  (window-origin-x-jpeg) (window-origin-y-jpeg)
				  (window-width-jpeg) (window-height-jpeg))))
	    (pg-set-palette! jpeg-device (pg-current-palette WIN-device))
	    (window-manager "vhardcopy" jpeg-device res)))

      (if (= pngflag 1)
	  (begin
	    (if (not png-device)
		(begin
		  (set! png-device
			(pg-make-device "PNG"
					type))
		  (pg-open-device png-device
				  (window-origin-x-png) (window-origin-y-png)
				  (window-width-png) (window-height-png))))
	    (pg-set-palette! png-device (pg-current-palette WIN-device))
	    (window-manager "vhardcopy" png-device res)))

      (if (= psflag 1)
	  (begin
	    (if (not ps-device)
		(begin
		  (set! ps-device
			(pg-make-device "PS"
					type))
		  (if (equal? color "COLOR")
		      (pg-set-color-type! ps-device "PS" color))
		  (pg-open-device ps-device
				  (window-origin-x-ps) (window-origin-y-ps)
				  (window-width-ps) (window-height-ps))))
	    (pg-set-palette! ps-device (pg-current-palette WIN-device))
	    (window-manager "vhardcopy" ps-device res)))

      (if (= mpegflag 1)
	  (begin
	    (if (not mpeg-device)
		(begin
		  (set! mpeg-device
			(pg-make-device "MPEG"
					type))
		  (pg-open-device mpeg-device
				  (window-origin-x-mpeg) (window-origin-y-mpeg)
				  (window-width-mpeg) (window-height-mpeg))))
	    (pg-set-palette! mpeg-device (pg-current-palette WIN-device))
	    (window-manager "vhardcopy" mpeg-device res)))

      (if (= cgmflag 1)
	  (begin
	    (if (not cgm-device)
		(begin
		  (set! cgm-device
			(pg-make-device "CGM"
					color))
		  (pg-open-device cgm-device
				  (window-origin-x-cgm) (window-origin-y-cgm)
				  (window-width-cgm) (window-height-cgm))))
	    (pg-set-palette! cgm-device (pg-current-palette WIN-device))
	    (window-manager "vhardcopy" cgm-device res)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PROCESS-HC-ARGS - process the arguments to an HC command and make
;                 - a uniform return list of values containing
;                 - (color mode res)

(define (process-hc-args args)
   (if args
       (let* ((color (memv 'color args))
	      (land (memv 'landscape args))
	      (res (list-ref args (- (length args) 1))))
	 (list (if color "COLOR" "MONOCHROME")
	       (if land  "LANDSCAPE" "PORTRAIT")
	       (if (number? res) res 0)))
       (list "MONOCHROME" "PORTRAIT" 0)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HARDCOPY-WINDOW / HC / HCW - draw the contents of the current window to
;                 to all open hardcopy devices

(define-macro (hardcopy-window . rest)
    "HARDCOPY-WINDOW - Draw the current window to all open hardcopy devices.
                       Color output is indicated by an optional argument.
                       Landscape mode is indicated by an optional argument.
                       The optional resolution scale factor is an integer
                       factor by which the resolution will be decreased
                       below the full resolution of the device (this
                       argument must appear last in the argument list).
     Usage: hardcopy-window [color | monochrome] [portrait | landscape] [resolution-scale-factor]
     Examples: hardcopy-window
               hc color 8"

    (interactive off)
    (let* ((psflag (ps-flag))
	   (cgmflag (cgm-flag))
	   (mpegflag (mpeg-flag))
	   (jpegflag (jpeg-flag))
	   (pngflag (png-flag))
	   (args (process-hc-args rest))
	   (color (list-ref args 0))
	   (mode (list-ref args 1))
	   (res (list-ref args 2))
	   (type (sprintf "%s %s" color mode)))

      (if (= jpegflag 1)
	  (begin
	    (if (not jpeg-device)
		(begin
		  (set! jpeg-device
			(pg-make-device "JPEG"
					type))
		  (pg-open-device jpeg-device
				  (window-origin-x-jpeg) (window-origin-y-jpeg)
				  (window-width-jpeg) (window-height-jpeg))))
	    (pg-set-palette! jpeg-device (pg-current-palette WIN-device))  
            (pg-set-data-id-flag! jpeg-device (cond ((boolean? data-id-save)
							     0)
							    ((integer? data-id-save)
							     data-id-save)
							    (else
							     1)))
	    (window-manager "whardcopy"
			    jpeg-device res current-window)))

      (if (= pngflag 1)
	  (begin
	    (if (not png-device)
		(begin
		  (set! png-device
			(pg-make-device "PNG"
					type))
		  (pg-open-device png-device
				  (window-origin-x-png) (window-origin-y-png)
				  (window-width-png) (window-height-png))))
	    (pg-set-palette! png-device (pg-current-palette WIN-device))  
            (pg-set-data-id-flag! png-device (cond ((boolean? data-id-save)
							     0)
							    ((integer? data-id-save)
							     data-id-save)
							    (else
							     1)))
	    (window-manager "whardcopy"
			    png-device res current-window)))

      (if (= psflag 1)
	  (begin
	    (if (not ps-device)
		(begin
		  (set! ps-device
			(pg-make-device "PS"
					type))
		  (if (equal? color "COLOR")
		      (pg-set-color-type! ps-device "PS" color))
		  (pg-open-device ps-device
				  (window-origin-x-ps) (window-origin-y-ps)
				  (window-width-ps) (window-height-ps))))
	    (pg-set-palette! ps-device (pg-current-palette WIN-device))
            (pg-set-data-id-flag! ps-device (cond ((boolean? data-id-save)
							     0)
							    ((integer? data-id-save)
							     data-id-save)
							    (else
							     1)))
	    (window-manager "whardcopy"
			    ps-device res current-window)))
      (if (= mpegflag 1)
	  (begin
	    (if (not mpeg-device)
		(begin
		  (set! mpeg-device
			(pg-make-device "MPEG"
					type))
		  (pg-open-device mpeg-device
				  (window-origin-x-mpeg) (window-origin-y-mpeg)
				  (window-width-mpeg) (window-height-mpeg))))
	    (pg-set-palette! mpeg-device (pg-current-palette WIN-device))
            (pg-set-data-id-flag! mpeg-device (cond ((boolean? data-id-save)
							     0)
							    ((integer? data-id-save)
							     data-id-save)
							    (else
							     1)))
	    (window-manager "whardcopy"
			    mpeg-device res current-window)))
      (if (= cgmflag 1)
	  (begin
	    (if (not cgm-device)
		(begin
		  (set! cgm-device
			(pg-make-device "CGM"
					color))
		  (pg-open-device cgm-device
				  (window-origin-x-cgm) (window-origin-y-cgm)
				  (window-width-cgm) (window-height-cgm))))
	    (pg-set-palette! cgm-device (pg-current-palette WIN-device))
            (pg-set-data-id-flag! cgm-device (cond ((boolean? data-id-save)
							     0)
							    ((integer? data-id-save)
							     data-id-save)
							    (else
							     1)))
	    (window-manager "whardcopy"
			    cgm-device res current-window)))))

;--------------------------------------------------------------------------

;                           ULTRA ROUTINES

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

