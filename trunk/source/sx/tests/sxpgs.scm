;
; SXPGS.SCM - test SX interface to PGS
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(load "consts.scm")

(define blue 4)
(define red 7)
(define black 0)
(define on    1)
(define off   0)
(define deg-rad (/ (acos 0.0) 90.0))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; EXTREMUM

(define (extremum op a seed)
    "Return the extremum with respect to the given operator of the elements
     of the specified vector.
     Usage: (extremum <op> <vector> <seed>)"
     (define (extremum-aux a i val)
         (if (> i 0)
	     (let* ((a0 (vector-ref a i)))
	           (if (op a0 val)
		       (extremum-aux a (- i 1) a0)
		       (extremum-aux a (- i 1) val)))
	     val))
     (extremum-aux a (vector-length a) seed))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TRIANGLE

(define (triangle dev rx ry scale fill)
    "Draw a triangle on the specified device
     Usage: (triangle <dev> <x> <y> <scale> <fill-flag>)"
    (let* ((a1 (* 90.0 deg-rad))
	   (a2 (* 210.0 deg-rad))
	   (a3 (* 330.0 deg-rad))
	   (rx1 (+ rx (* scale (cos a1))))
	   (ry1 (+ ry (* scale (sin a1))))
	   (rx2 (+ rx (* scale (cos a2))))
	   (ry2 (+ ry (* scale (sin a2))))
	   (rx3 (+ rx (* scale (cos a3))))
	   (ry3 (+ ry (* scale (sin a3)))))
          (if fill
	      (pg-fill-polygon dev black
			       rx1 ry1
			       rx2 ry2
			       rx3 ry3
			       rx1 ry1)
	      (pg-draw-polyline-n dev 2 @world #t
				  rx1 ry1
				  rx2 ry2
				  rx3 ry3
				  rx1 ry1))
	  (pg-set-line-color! dev black)
	  (pg-set-text-color! dev black)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DIAMOND

(define (diamond dev rx ry scale fill)
    "Draw a diamond on the specified device
     Usage: (diamond <dev> <x> <y> <scale> <fill-flag>)"
    (let* ((a1 (* 0.0 deg-rad))
	   (a2 (* 90.0 deg-rad))
	   (a3 (* 180.0 deg-rad))
	   (a4 (* 270.0 deg-rad))
	   (rx1 (+ rx (* scale (cos a1))))
	   (ry1 (+ ry (* scale (sin a1))))
	   (rx2 (+ rx (* scale (cos a2))))
	   (ry2 (+ ry (* scale (sin a2))))
	   (rx3 (+ rx (* scale (cos a3))))
	   (ry3 (+ ry (* scale (sin a3))))
	   (rx4 (+ rx (* scale (cos a4))))
	   (ry4 (+ ry (* scale (sin a4)))))
          (if fill
		(pg-fill-polygon dev black
			      rx1 ry1
			      rx2 ry2
			      rx3 ry3
			      rx4 ry4
			      rx1 ry1)
		(pg-draw-polyline-n dev 2 @world #t
				    rx1 ry1
				    rx2 ry2
				    rx3 ry3
				    rx4 ry4
				    rx1 ry1))
	    (pg-set-line-color! dev black)
	    (pg-set-text-color! dev black)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; BOX

(define (box dev rx ry scale fill)
    "Draw a box on the specified device
     Usage: (box <dev> <x> <y> <scale> <fill-flag>)"
    (let* ((a1 (* 45.0 deg-rad))
	   (a2 (* 135.0 deg-rad))
	   (a3 (* 225.0 deg-rad))
	   (a4 (* 315.0 deg-rad))
	   (rx1 (+ rx (* scale (cos a1))))
	   (ry1 (+ ry (* scale (sin a1))))
	   (rx2 (+ rx (* scale (cos a2))))
	   (ry2 (+ ry (* scale (sin a2))))
	   (rx3 (+ rx (* scale (cos a3))))
	   (ry3 (+ ry (* scale (sin a3))))
	   (rx4 (+ rx (* scale (cos a4))))
	   (ry4 (+ ry (* scale (sin a4)))))
          (if fill
		(pg-fill-polygon dev black
			      rx1 ry1
			      rx2 ry2
			      rx3 ry3
			      rx4 ry4
			      rx1 ry1)
		(pg-draw-polyline-n dev 2 @world #t
				    rx1 ry1
				    rx2 ry2
				    rx3 ry3
				    rx4 ry4
				    rx1 ry1))
	    (pg-set-line-color! dev black)
	    (pg-set-text-color! dev black)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; REVERSE COORDINATES WITH GRID

(define (rev-grid dev x1 x2 y1 y2)
    (pg-clear-window dev)
    (pg-set-world-coordinate-system! dev x1 x2 y1 y2)
    (pg-axis dev)
    (pg-set-line-color! dev red)
    (pg-draw-line dev x1 y1 x2 y1)
    (pg-draw-line dev x2 y1 x2 y2)
    (pg-draw-line dev x2 y2 x1 y1)

    (cond ((and (< x1 x2) (< y1 y2))
	   (pg-center-label dev 0.95 "Increasing Right and Up"))
	  ((and (< x1 x2) (< y2 y1))
	   (pg-center-label dev 0.95 "Increasing Right and Down"))
	  ((and (< x2 x1) (< y1 y2))
	   (pg-center-label dev 0.95 "Increasing Left and Up"))
	  ((and (< x2 x1) (< y2 y1))
	   (pg-center-label dev 0.95 "Increasing Left and Down")))

    (let* ((delx (/ (- x2 x1) 4))
	   (dely (/ (- y2 y1) 4))
	   (x3 (+ x1 delx))
	   (x4 (- x2 delx))
	   (y3 (+ y1 dely))
	   (y4 (- y2 dely)))
	(pg-fill-polygon dev blue
			 x3 y3
			 x4 y3
			 x4 y4
			 x3 y4
			 x3 y4))
;   (pg-update-view-surface dev)
    (pg-finish-plot dev))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(ps-dots-inch 600)
(define dev (pg-make-device "PS" "COLOR" "sxpgs 2.0 2.0"))
;(define dev (pg-make-device "CGM" "COLOR" "sxpgs"))
(pg-open-device dev 0 0 0 0)

(pg-clear-window dev)
(pg-set-viewport! dev 0.2 0.8 0.15 0.75)
(pg-set-world-coordinate-system! dev -4 4 0 4)

(define (label-figure dev rx1 rx2 ry1 ry2 scale)
    (let* ((clip (pg-clipping? dev))
	   (xo1 -3.5)
	   (xo2 -3.0)
	   (xo3  0.0)
	   (dy (abs (- ry1 ry2)))
	   (rdy (/ dy 7))
	   (ty1 (- ry1 rdy))
	   (ty2 (- ty1 rdy))
	   (ty3 (- ty2 rdy))
	   (ty4 (- ty3 rdy))
	   (ty5 (- ty4 rdy))
	   (ty6 (- ty5 rdy)))

          (pg-set-clipping! dev off)

	  (pg-draw-box dev @world rx1 rx2 ry1 ry2)

	    (triangle dev xo1 ty1 scale #t)
	    (pg-draw-text dev 2 @world xo2 ty1 "- Foo")
	    (pg-draw-text dev 2 @world  xo3 ty1 "3.00")

	    (diamond  dev xo1 ty2 scale #t)
	    (pg-draw-text dev 2 @world xo2 ty2 "- Bar")
	    (pg-draw-text dev 2 @world  xo3 ty2 "2.50")

	    (box      dev xo1 ty3 scale #t)
	    (pg-draw-text dev 2 @world xo2 ty3 "- Baz")
	    (pg-draw-text dev 2 @world  xo3 ty3 "2.00")

	    (diamond  dev xo1 ty4 scale #f)
	    (pg-draw-text dev 2 @world xo2 ty4 "- Blat")
	    (pg-draw-text dev 2 @world  xo3 ty4 "1.50")

	    (triangle dev xo1 ty5 scale #f)
	    (pg-draw-text dev 2 @world xo2 ty5 "- Tarfu")
	    (pg-draw-text dev 2 @world  xo3 ty5 "1.00")

	    (box      dev xo1 ty6 scale #f)
	    (pg-draw-text dev 2 @world xo2 ty6 "- Snafu")
	    (pg-draw-text dev 2 @world  xo3 ty6 "0.50")

	    (if clip
		(pg-set-clipping! dev on))))

(pg-axis dev)

(pg-set-line-width! dev 0.7)
(pg-draw-polyline-n dev 2 @world #t
   -4.0 0.0
   -1.0 0.0
    0.0 1.0
    1.0 0.0
    4.0 0.0
    0.0 4.0
   -4.0 0.0)

(pg-set-line-width! dev 0.0)
(pg-set-line-style! dev 2)
(pg-draw-polyline-n dev 2 @world #t
   -3.0 0.0
    0.0 3.0
    3.0 0.0)
(pg-draw-line dev
    0.0 1.0
    0.0 4.0)
(pg-set-line-style! dev 1)

(pg-set-line-width! dev 0.7)
(triangle dev 0.0 3.00 0.1 #t) ; Foo
(diamond  dev 0.0 2.50 0.1 #t) ; Bar
(box      dev 0.0 2.00 0.1 #t) ; Baz
(diamond  dev 0.0 1.25 0.1 #f) ; Blat
(triangle dev 0.0 0.63 0.1 #f) ; Tarfu
(box      dev 0.0 0.13 0.1 #f) ; Snafu

(label-figure dev -4.0 4.0 -0.5 -1.9 0.08)

(pg-update-view-surface dev)
(pg-finish-plot dev)

(pg-set-grid-flag! dev 1)

(rev-grid dev  4 -2  2 -4)
(rev-grid dev  4 -2 -2  4)
(rev-grid dev -4  2  2 -4)
(rev-grid dev -4  2 -2  4)

(pg-close-device dev)
(quit)
