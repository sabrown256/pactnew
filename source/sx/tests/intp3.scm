; 
; INTP3.SCM - test 3D mesh function interpolation
;           - use refine as simplest look at interpolation process
;           - test inverse distance weighting and multi-quadric
; 
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(define win-name "intp3")
;(cw win-name)
(cw win-name "COLOR" "PS" 0.0 0.0 1.0 1.0)
(plot-date off)
(grid on)

(define nxi 5)
(define nyi 6)
(define nzi 3)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; 3D test - setup

(printf nil "---------------------------------------------------------------\n")
(printf nil "3D interpolations\n")

;(refmeshcolor gray)
;(refmesh on)
;(overlay on)

(define nd  3)
(define nr '(11 13 7))
;(define nr '(47 53 43))
;(define nr '(97 103 101))
;(define nr '(491 503 487))

(nxm 2 1)
(view-angle 60 30)
(vr contour)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; 3D test - inverse distance weighted interpolation

(dl)
(cv "V0")
(dl)
(printf nil "inverse distance weight\n")
(interpolation-method idw)
(interpolation-scale 1.1)

hyper-plane 0 (list 1 1 10 nxi) (list 1 1 11 nyi) (list 1 1 12 nzi)
(label-drawable "plane" (sqr 1))
(label-drawable "refine" (refine 1 nr))
(lsv)

(move "V1" win-name (copy-map 1))

(wu)
;(pause)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; 3D test - multiquadric interpolation

(cv "V1")
(dl)
(cv "V0")
(dl)
(printf nil "multiquadric\n")
(interpolation-method mq)
(interpolation-mq-scale 1.0e-10)

hyper-plane 0 (list 1 1 10 nxi) (list 1 1 11 nyi) (list 1 1 12 nzi)

(label-drawable "plane" (sqr 1))
(label-drawable "refine" (refine 1 nr))
(lsv)

(move "V1" win-name 1)

(wu)
;(pause)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(cv "V1")
(dl)
(cv "V0")
(dl)
(end)
