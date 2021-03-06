;
; ULAXS.SCM - test axis drawing
;           - this also serves to demonstrate proper calibration of ticks
;
; Source Version: 4.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
;

(unlink "ulaxs.log")

(transcript-on "ulaxs.log")

(ps-name "ulaxs")
(ps-flag on)
(ps-type "color")
(plot-date off)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (test-curve ln lx xl yl)
    (if (= ln lx)
	(make-curve* xl yl)
	(let* ((xp (abs (- lx ln)))
	       (yp (expt 10.0 lx))
	       (nn (if (< ln lx) (- lx 1.0) (+ lx 1.0)))
	       (nx ln))

              (test-curve nn nx (cons xp xl) (cons yp yl)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (axis-test ln lx)
    (let* ((crv (test-curve ln lx nil nil)))
          (y-log-scale on)
          (grid on)
          (hc)
	  (era)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(axis-test -2 0)
(axis-test -2 4)
(axis-test -2 8)
(axis-test -5 -2)
(axis-test -9 -2)
(axis-test -13 -2)
(axis-test -50 -42)

(interactive off)
(era)
(grid on)
(y-log-scale on)
(axis-n-decades 12)
(span 1 1000)
(hc)
(era)
(span 0.1 1.0e8)
(hc)
(era)
(span 1.0e-8 1)
(hc)
(era)
(span 1.0e-10 0.1)
(hc)

(command-log off)
(end)

