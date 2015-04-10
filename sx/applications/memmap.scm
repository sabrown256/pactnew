;
; MEMMAP.SCM - display a memory map produced by SC_mem_map
;
; Source Version: 4.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
;

(define blsz 80)
(define zoom-factor 2.0)
(define ref-point nil)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MARK-POINT - remember the point X

(define (mark-point x)
    (interactive off)
    (set! ref-point x))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DRAG-POINT - drag to point X

(define (drag-point x)
    (interactive off)
    (let* ((xo  (list-ref x 0))
	   (xr  (list-ref ref-point 0))
	   (dm  (domain))
	   (xmn (car dm))
	   (xmx (cdr dm))
           (dx  (- xmx xmn))
	   (sx  (* 0.5 dx))

	   (rxo (+ xmn sx (- xr xo)))

	   (xnn (- rxo sx))
	   (xnx (+ rxo sx)))

          (domain xnn xnx)
	  (replot)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; ZOOM-OPER - zoom worker

(define (zoom-oper x op)
    (interactive off)
    (let* ((xo  (list-ref x 0))

; limits for all curves in the list
	   (cl  (lst))
	   (dma (get-data-domain cl))
	   (xan (car dma))
	   (xax (cdr dma))

; current domain limits
	   (dm  (domain))
	   (xmn (car dm))
	   (xmx (cdr dm))
           (dxl  (- xo xmn))
           (dxr  (- xmx xo))

; scaled width
	   (sxl (op dxl zoom-factor))
	   (sxr (op dxr zoom-factor))

; new domain limits
	   (xnn (max-value xan (- xo sxl)))
	   (xnx (min-value xax (+ xo sxr))))

;(printf nil "-> %s [%.0f,%.0f] (%.0f,%.0f) (%.0f,%.0f) (%.0f,%.0f)\n"
;	op xo sx
;	(- xo sx) (+ xo sx)
;	xan xax
;	xnn xnx)

          (domain xnn xnx)
	  (replot)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; ZOOM-IN - zoom in on the point X

(define (zoom-in x) (zoom-oper x /))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; ZOOM-OUT - zoom out on the point X

(define (zoom-out x) (zoom-oper x *))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HANDLE-MOUSE-EVENT - handle mouse events

(define-macro (handle-mouse-event type x btn qual)
   (cond ((eqv? type mouse-down)
	  (cond ((eqv? btn mouse-left)
		 (mark-point x))
		((eqv? btn mouse-middle)
		 (cond ((eqv? qual 0)
			(zoom-in x))
		       ((eqv? qual key-shift)
			(zoom-out x))))
		((eqv? btn mouse-wheel-up)
		 (zoom-in x))
		((eqv? btn mouse-wheel-down)
		 (zoom-out x))
		((eqv? btn mouse-right)
		 #t)))

	 ((eqv? type mouse-up)
	  (cond ((eqv? btn mouse-left)
		 (drag-point x))
		((eqv? btn mouse-middle)
		 #t)
		((eqv? btn mouse-right)
		 #t)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (mem-curve lbl x y lc fc)
   (if (and x (> (length x) 1))
       (let* ((crv (make-curve* (reverse x) (reverse y))))
	     (color crv lc)
	     (fill crv fc)
	     (label crv lbl))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PARSE-MAP - parse the memory map file

(define (parse-map fp xact yact xfr yfr xma yma n)

; dispose of the comment lines at the top
    (if (= n 0)
	(begin (read-line fp)
	       (read-line fp)
	       (read-line fp)))

    (let* ((t (read-line fp)))
          (if (eof-object? t)
	      (begin (mem-curve "Active"      xact yact dark-green green)
		     (mem-curve "Bare malloc" xma  yma  dark-red   red)
		     (mem-curve "Free"        xfr  yfr  dark-gray  gray)
		     n)
	      (let* ((s (sprintf "(%s)" t))
		     (prt (string->port s))
		     (lst (read prt))
		     (ad  (list-ref lst 1))
		     (nb  (list-ref lst 2))
		     (typ (list-ref lst 3))
		     (rc  (list-ref lst 4))
		     (tag (sprintf "%s" (list-tail lst 5))))
		    (if (and (number? ad) (number? nb))
			(let* ((ab (- ad blsz))
			       (ae (+ ad nb))
			       (nh blsz)
                               (isf (string=? tag "(-- free --)"))
			       (ism (string=? (substring tag 0 12) "(BARE_MALLOC"))
			       (isa (not (or isf ism)))
			       (xna (if isa
					(append (list ae ae ad ad ab ab) xact)
					xact))
			       (yna (if isa
					(append (list 0 1 1 0.2 0.2 0) yact)
					yact))
			       (xnf (if isf
					(append (list ae ae ad ad ab ab) xfr)
					xfr))
			       (ynf (if isf
					(append (list 0 1 1 0.2 0.2 0) yfr)
					yfr))
			       (xnm (if ism
					(append (list ae ae ad ad ab ab) xma)
					xma))
			       (ynm (if ism
					(append (list 0 1 1 0.2 0.2 0) yma)
					yma)))
			      (parse-map fp xna yna xnf ynf xnm ynm (+ n 1)))
			(parse-map fp xact yact xfr yfr xma yma (+ n 1)))))))
	   

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MEMORY-MAP - display a memory map

(define-macro (memory-map fname)
   (let* ((fp (fopen fname "r"))
	  (nl (parse-map fp nil nil nil nil nil nil 0)))
         (fclose fp)
	 (clipping off)
         (label-space 0.1)
	 (label-color-flag on)
         (axis-x-format "0x%x")
;         (x-log-scale on)
	 (data-id off)
	 (replot)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------


