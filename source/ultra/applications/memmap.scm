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

; current range/domain limits
	   (dm  (domain))
	   (xmn (car dm))
	   (xmx (cdr dm))
           (dx  (- xmx xmn))

; scaled width
	   (sx  (* 0.5 (op dx zoom-factor)))

; new domain limits
	   (xnn (max-value xan (- xo sx)))
	   (xnx (min-value xax (+ xo sx))))

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

; PARSE-MAP - parse the memory map file

(define (parse-map fp xp yp n)

; dispose of the comment lines at the top
    (if (= n 0)
	(begin (read-line fp)
	       (read-line fp)
	       (read-line fp)))

    (let* ((t (read-line fp)))
          (if (eof-object? t)
	      (make-curve* (reverse xp) (reverse yp))
	      (let* ((s (sprintf "(%s)" t))
		     (prt (string->port s))
		     (lst (read prt))
		     (ad  (list-ref lst 1))
		     (nb  (list-ref lst 2)))
		    (if (and (number? ad) (number? nb))
			(let* ((ab (- ad blsz))
			       (ae (+ ad nb))
			       (nh blsz)
			       (xn (append (list ae ae ad ad ab ab) xp))
			       (yn (append (list 0 1 1 0.2 0.2 0) yp)))
			      (parse-map fp xn yn (+ n 1)))
			(parse-map fp xp yp (+ n 1)))))))
	   

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MEMORY-MAP - display a memory map

(define-macro (memory-map fname)
   (let* ((fp (fopen fname "r"))
	  (crv (parse-map fp nil nil 0)))
         (fclose fp)
         (axis-x-format "0x%x")
;         (x-log-scale on)
         (color crv gray)
	 (data-id off)
	 (replot)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------


