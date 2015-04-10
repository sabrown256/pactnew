;
; MOUSE.SCM - mouse event handling for ULTRA
;
; Source Version: 4.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
;

(define zoom-factor 1.5)
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
	   (yo  (list-ref x 1))
	   (xr  (list-ref ref-point 0))
	   (yr  (list-ref ref-point 1))
	   (dm  (domain))
	   (xmn (car dm))
	   (xmx (cdr dm))
           (dx  (- xmx xmn))
	   (rn  (range))
	   (ymn (car rn))
	   (ymx (cdr rn))
           (dy  (- ymx ymn))
	   (sx  (* 0.5 dx))
	   (sy  (* 0.5 dy))

	   (rxo (+ xmn sx (- xr xo)))
	   (ryo (+ ymn sy (- yr yo)))

	   (xnn (- rxo sx))
	   (xnx (+ rxo sx))
	   (ynn (- ryo sy))
	   (ynx (+ ryo sy)))

          (domain xnn xnx)
	  (range ynn ynx)
	  (replot)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; ZOOM-OPER - zoom worker

(define (zoom-oper x op)
    (interactive off)
    (let* ((xo  (list-ref x 0))
	   (yo  (list-ref x 1))

; limits for all curves in the list
	   (cl  (lst))
	   (dma (get-data-domain cl))
	   (rna (get-data-range cl))
	   (xan (car dma))
	   (xax (cdr dma))
	   (yan (car rna))
	   (yax (cdr rna))

; current range/domain limits
	   (dm  (domain))
	   (rn  (range))
	   (xmn (car dm))
	   (xmx (cdr dm))
           (dx  (- xmx xmn))
	   (ymn (car rn))
	   (ymx (cdr rn))
           (dy  (- ymx ymn))

; scaled width/height
	   (sx  (* 0.5 (op dx zoom-factor)))
	   (sy  (* 0.5 (op dy zoom-factor)))

; new range/domain limits
	   (xnn (max-value xan (- xo sx)))
	   (xnx (min-value xax (+ xo sx)))
	   (ynn (max-value yan (- yo sy)))
	   (ynx (min-value yax (+ yo sy))))

          (domain xnn xnx)
	  (range ynn ynx)
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

