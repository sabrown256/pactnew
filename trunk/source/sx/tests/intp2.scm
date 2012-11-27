; 
; INTP2.SCM - test 2D mesh function interpolation
;           - use refine as simplest look at interpolation process
;           - test inverse distance weighting and multi-quadric
; 
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(define win-name "intp2")
;(cw win-name)
(cw win-name "COLOR" "PS" 0.0 0.0 1.0 1.0)
(plot-date off)
(grid on)

(define nxi 5)
(define nyi 6)
(define nzi 3)

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
(interpolation-scale 1.1)

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
(interpolation-mq-scale 1.0e-10)

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

(cv "V1")
(dl)
(cv "V0")
(dl)
(end)
