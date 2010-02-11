;
; AXISM.SCM - make an axis movie
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(define red  7)
(define blue 4)

(define AXIS_TICK_MINOR    1)
(define AXIS_TICK_MAJOR    2)
(define AXIS_TICK_LABEL    4)

(define AXIS_TICK_RIGHT     113)
(define AXIS_TICK_LEFT      114)
(define AXIS_TICK_STRADDLE  115)
(define AXIS_TICK_ENDS      116)

(define MAJOR_MINOR        (+ AXIS_TICK_MAJOR AXIS_TICK_MINOR))
(define MAJOR_LABEL        (+ AXIS_TICK_MAJOR AXIS_TICK_LABEL))
(define MINOR_LABEL        (+ AXIS_TICK_MINOR AXIS_TICK_LABEL))
(define MAJOR_MINOR_LABEL  (+ AXIS_TICK_MAJOR AXIS_TICK_MINOR AXIS_TICK_LABEL))

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
;		(pg-clear-region dev 0.01 0.99 0.01 0.99)
		(pg-draw-axis dev
			      0.0 0.0 cosa sina
			      0.4 1.0
			      -1.0 1.0 
			      1.0  "%4.1f"
			      tick-side label-side
			      MAJOR_MINOR_LABEL)

                (if (= dphi0 1.0)
		    (sleep 20))

		(if (< nphi (* nt twopi))
		    (angled-axis dev nphi))))

	(angled-axis dev phis)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

;(define devr (pg-make-device "SCREEN" "COLOR" "Axis Movie"))
(define devr (pg-make-device "PS" "COLOR" "sxaxs"))

(pg-open-device devr 0.000001 0.000001 0.999 0.999)
(pg-set-viewport! devr 0.05 0.95 0.05 0.95)
(pg-set-world-coordinate-system! devr -1.0 1.0 -1.0 1.0)
(pg-clear-window devr)
(pg-set-line-color! devr red)
(pg-set-text-color! devr blue)
(axis-number-minor-ticks 4)

;(axes devr AXIS_TICK_STRADDLE AXIS_TICK_RIGHT 0.0 1.0 4)
(axes devr AXIS_TICK_STRADDLE AXIS_TICK_RIGHT 20.0 45.0 1)

(pg-finish-plot devr)
(pg-close-device devr)

(quit)
