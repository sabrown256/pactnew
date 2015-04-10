;
; NXM-ANN.SCM - demonstration of annotations with a 2x2 viewport window
;
; #include <cpyright.h>
; 

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (annotate-quad n qu)
    (let* ((xmn (cond ((= n 0) 0.0)
		      ((= n 1) 0.5)
		      ((= n 2) 0.0)
		      ((= n 3) 0.5)))
	   (ymn (cond ((= n 0) 0.0)
		      ((= n 1) 0.0)
		      ((= n 2) 0.5)
		      ((= n 3) 0.5)))

	   (qxn (+ xmn 0.22))
	   (qxx (+ xmn 0.32))
	   (qyn (+ ymn 0.420))
	   (qyx (+ ymn 0.450))

	   (wxn (+ xmn 0.18))
	   (wxx (+ xmn 0.32))
	   (wyn (+ ymn 0.440))
	   (wyx (+ ymn 0.470))

	   (axn (+ xmn 0.022))
	   (axx (+ xmn 0.032))
	   (ayn (+ ymn 0.290))
	   (ayx (+ ymn 0.291))

	   (bxn (+ xmn 0.250))
	   (bxx (+ xmn 0.260))
	   (byn (+ ymn 0.049))
	   (byx (+ ymn 0.059))

	   (isz 18)
	   (isty bold-italic)
	   (ifnt courier))

          (annot "     " black wxn wxx wyn wyx 0.0 center isz isty ifnt)
          (annot qu      black qxn qxx qyn qyx 0.0 center isz isty ifnt)

          (annot "Na" black axn axx ayn ayx 0.0 left isz isty ifnt)
          (annot "Nb" black bxn bxx byn byx 0.0 left isz isty ifnt)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (do-annotations)
    (let* ((st "The Big Picture"))

          (annot st black 0.41 0.61 0.98 0.99 0.0 center 24 bold times)

	  (annotate-quad 0 "Line #1")
	  (annotate-quad 1 "Line #2")
	  (annotate-quad 2 "Plane #1")
	  (annotate-quad 3 "Line #3")

          (wu)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (do-plots)
   (cv "V0")
   (span 0 1)
   (cv "V1")
   (plane 1.0 (2.0 -5.0 5.0 100))
   (cv "V2")
   (plane 0.0 (2.0 -5.0 5.0 100) (0.5 -5.0 5.0 10))
   (cv "V3")
   (sin (span 0 10))

   (wu))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define base "nxm-ann")
(jpeg-flag on)
(jpeg-name base)
(png-flag on)
(png-name base)
(ps-flag on)
(ps-name base)
(plot-date off)
(labels off)
(axis-number-minor-ticks 5)
(axis-line-width 0.8)
(line-width 0.8)
(type-size 16)

(system (sprintf "rm -f %s*.*" base))

(nxm 2 2)

(data-id off)

(do-plots)
(do-annotations)

;(trace-all #t)
;(trace wm-win-hardcopy vp-hardcopy)
;(trace viewport-graphs vp-update-drawable)

(hc color)
(end)


