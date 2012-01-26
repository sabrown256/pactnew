; 
; INTP.SCM - test 1D and 2D mesh function interpolation
;          - use refine as simplest look at interpolation process
;          - test inverse distance weighting and multi-quadric
; 
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(define win-name "intp")
;(cw win-name)
(cw win-name "COLOR" "PS" 0.0 0.0 1.0 1.0)
(plot-date off)
(grid on)

(define nxi 5)
(define nyi 6)
(define nzi 3)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; 1D test - setup

(printf nil "---------------------------------------------------------------\n")
(printf nil "1D interpolations\n")

(label-space 0.2)

(define nd 1)
(define nr '(12))
(define nr '(23))
;(interpolation-strength (* nd nd))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; 1D test - inverse distance weighted interpolation

(printf nil "inverse distance weight\n")
(interpolation-method idw)

hyper-plane 0 (list 1 0 1.5 nxi)
(label-drawable "line" (sqr 1))
(label-drawable "line scatter" (copy-map 1))
(scatter triangle 1)
(lncolor dark-green 1 2)
(lsv)

(label-drawable "refined" (refine 1 nr))
(label-drawable "refined scatter" (copy-map 1))
(scatter star 1)
(lncolor gray 1 2)
(lsv)

(label-drawable "weights" (interpolation-weights 1 nr))
(label-drawable "weights scatter" (copy-map 1))
(scatter star 1)
(lncolor blue 1 2)
(lsv)
(wu)
;(pause)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; 1D test - multiquadric interpolation

(dl)
(printf nil "multiquadric\n")
(interpolation-method mq)

hyper-plane 0 (list 1 0 1.5 nxi)
(label-drawable "line" (sqr 1))
(label-drawable "line scatter" (copy-map 1))
(scatter triangle 1)
(lncolor dark-green 1 2)
(lsv)

(label-drawable "refined" (refine 1 nr))
(label-drawable "refined scatter" (copy-map 1))
(scatter star 1)
(lncolor gray 1 2)
(lsv)

(label-drawable "weights" (interpolation-weights 1 nr))
(label-drawable "weights scatter" (copy-map 1))
(scatter star 1)
(lncolor blue 1 2)
(lsv)
(wu)
;(pause)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; 2D test - setup

(printf nil "---------------------------------------------------------------\n")
(printf nil "2D interpolations\n")

(label-space 0)

(define nd  2)
(define nr '(11 13))
;(define nr '(47 53))
;(define nr '(97 103))
;(define nr '(491 503))

(nxm 2 1)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; 2D test - inverse distance weighted interpolation

(dl)
(printf nil "inverse distance weight\n")
(interpolation-method idw)

hyper-plane 0 (list 1 1 nxi nxi) (list 1 1 nyi nyi)

(label-drawable "plane" (sqr 1))
(label-drawable "plane scatter" (copy-map 1))
(scatter triangle 1)
(lncolor dark-green 1 2)
(lsv)

(move "V1" win-name (copy-map 1))

(label-drawable "refine" (refine 1 nr))
(label-drawable "refine scatter" (copy-map 1))
(scatter star 1)
(lncolor red 1 2)
(lsv)

(cv "V1")
(label-drawable "weights" (interpolation-weights 1 nr))
(dl 2)

(dr 1 (render fill-poly))
(lsv)

(wu)
;(pause)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; 2D test - multiquadric interpolation

(dl)
(cv "V0")
(dl)
(printf nil "multiquadric\n")
(interpolation-method mq)

hyper-plane 0 (list 1 1 nxi nxi) (list 1 1 nyi nyi)

(label-drawable "plane" (sqr 1))
(label-drawable "plane scatter" (copy-map 1))
(scatter triangle 1)
(lncolor dark-green 1 2)
(lsv)

(move "V1" win-name (copy-map 1))

(label-drawable "refine" (refine 1 nr))
(label-drawable "refine scatter" (copy-map 1))
(scatter star 1)
(lncolor red 1 2)
(lsv)

(cv "V1")
(label-drawable "weights" (interpolation-weights 1 nr))
(dl 2)

(dr 1 (render fill-poly))
(lsv)

(wu)
;(pause)

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

;(nxm 1 1)
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

hyper-plane 0 (list 1 1 10 nxi) (list 1 1 11 nyi) (list 1 1 12 nzi)

(label-drawable "plane" (sqr 1))
(label-drawable "refine" (refine 1 nr))
(lsv)

(move "V1" win-name 1)

(wu)
;(pause)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; 3D test - multiquadric interpolation

;(cv "V1")
;(dl)
;(cv "V0")
;(dl)
;(printf nil "multiquadric\n")
;(interpolation-method mq)

;hyper-plane 0 (list 1 1 10 nxi) (list 1 1 11 nyi) (list 1 1 12 nzi)

;(label-drawable "plane" (sqr 1))
;(label-drawable "refine" (refine 1 nr))
;(label-drawable "weights" (interpolation-weights 1 nr))
;(lsv)

;(move "V1" win-name 1)

;(wu)
;(pause)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(cv "V1")
(dl)
(cv "V0")
(dl)
(end)
