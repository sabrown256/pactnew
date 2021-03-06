;
; AXISM.SCM - make an axis movie
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(define red  RED)
(define blue BLUE)

(define MAJOR              AXIS_TICK_MAJOR)
(define MINOR              AXIS_TICK_MINOR)
(define LABEL              AXIS_TICK_LABEL)
(define MAJOR_MINOR        3)
(define MAJOR_LABEL        6)
(define MINOR_LABEL        5)
(define MAJOR_MINOR_LABEL  7)
(define NO_TICKS           AXIS_TICK_NONE)

(define RIGHT_OF_AXIS     AXIS_TICK_RIGHT)
(define LEFT_OF_AXIS      AXIS_TICK_LEFT)
(define STRADDLE_AXIS     AXIS_TICK_STRADDLE)
(define ENDS              AXIS_TICK_ENDS)
(define NOTHING_ON_AXIS   32)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (axes dev tick-side label-side phi0 dphi0 nt)
    (let* ((twopi (* 2.0 %pi))
	   (d->r  (/ %pi 180.0))
	   (dphi  (* dphi0 d->r))
	   (phis  (* phi0 d->r)))

	(define (angled-axis dev phi)
	    (let* ((cosa (cos phi))
		   (sina (sin phi))
		   (nphi (+ phi dphi)))
		(pg-clear-region dev 0.01 0.99 0.01 0.99)
		(pg-draw-axis dev
			      0.0 0.0 cosa sina
			      0.4 1.0
			      -1.0 1.0 
			      1.0  "%4.1f"
			      tick-side label-side
			      MAJOR_MINOR_LABEL)
		(pg-finish-plot dev)

                (if (= dphi0 1.0)
		    (sleep 20))

		(if (< nphi (* nt twopi))
		    (angled-axis dev nphi))))

	(angled-axis dev phis)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define devr (pg-make-device "SCREEN" "COLOR" "Axis Movie"))
(pg-open-device devr 0.000001 0.000001 0.4 0.4)
(pg-set-viewport! devr 0.05 0.95 0.05 0.95)
(pg-set-world-coordinate-system! devr -1.0 1.0 -1.0 1.0)
(pg-set-line-color! devr red)
(pg-set-text-color! devr blue)
(axis-number-minor-ticks 4)

(axes devr STRADDLE_AXIS RIGHT_OF_AXIS 0.0 1.0 4)

(quit)
