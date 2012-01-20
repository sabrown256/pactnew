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

(cw "A")
(label-space 0.2)
(grid on)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; 1D test

(define nd 1)
(define ni 5)
(define nr 12)
(define nr 23)
;(interpolation-strength (* nd nd))

hyper-plane 0 (list 1 0 1.5 ni)
(label-drawable "line scatter" (sqr 1))
(scatter triangle 1)
(lncolor dark-green 1)
(label-drawable "line" (copy-map 1))
(lncolor dark-green 1)
(lsv)

(label-drawable "refined scatter" (refine 1 (list nr)))
(scatter star 1)
(lncolor gray 1)
(label-drawable "refined line" (copy-map 1))
(lncolor gray 1)
(lsv)

(label-drawable "weights scatter" (interpolation-weights 1 (list nr)))
(scatter star 1)
(lncolor blue 1)
(label-drawable "weights line" (copy-map 1))
(lncolor blue 1)
(lsv)
(wu)
(pause)
(dl)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; 2D test

(define nd  2)
(define nxi 5)
(define nyi 6)
(define nxr 11)
(define nyr 13)
;(define nxr 47)
;(define nyr 53)
;(define nxr 97)
;(define nyr 103)
;(define nxr 491)
;(define nyr 503)

(interpolation-method idw)

(nxm 2 1)
hyper-plane 0 (list 1 1 nxi nxi) (list 1 1 nyi nyi)

(label-drawable "plane scatter" (sqr 1))
(scatter triangle 1)
(lncolor dark-green 1)
(label-drawable "plane" (copy-map 1))
(lncolor dark-green 1)
(lsv)

(move "V1" "A" (copy-map 1))

(label-drawable "refine scatter" (refine 1 (list nxr nyr)))
(scatter star 1)
(lncolor red 1)
(label-drawable "refine" (copy-map 1))
(lncolor red 1)
(lsv)

(cv "V1")
(label-drawable "weights" (interpolation-weights 1 (list nxr nyr)))
(dl 2)

(dr 1 (render fill-poly))
(lsv)

(wu)
(pause)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; 2D test

(define nd  2)
(define nxi 5)
(define nyi 6)
(define nxr 11)
(define nyr 13)
;(define nxr 47)
;(define nyr 53)
;(define nxr 97)
;(define nyr 103)
;(define nxr 491)
;(define nyr 503)

(interpolation-method mq)

(nxm 2 1)
hyper-plane 0 (list 1 1 nxi nxi) (list 1 1 nyi nyi)

(label-drawable "plane scatter" (sqr 1))
(scatter triangle 1)
(lncolor dark-green 1)
(label-drawable "plane" (copy-map 1))
(lncolor dark-green 1)
(lsv)

(move "V1" "A" (copy-map 1))

(label-drawable "refine scatter" (refine 1 (list nxr nyr)))
(scatter star 1)
(lncolor red 1)
(label-drawable "refine" (copy-map 1))
(lncolor red 1)
(lsv)

(cv "V1")
(label-drawable "weights" (interpolation-weights 1 (list nxr nyr)))
(dl 2)

(dr 1 (render fill-poly))
(lsv)

(wu)
(pause)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(end)
