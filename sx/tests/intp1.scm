; 
; INTP1.SCM - test 1D mesh function interpolation
;           - use refine as simplest look at interpolation process
;           - test inverse distance weighting and multi-quadric
; 
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(define win-name "intp1")
;(cw win-name)
(cw win-name "COLOR" "PS" 0.0 0.0 1.0 1.0)
(plot-date off)
(grid on)

(define nxi 5)

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
(interpolation-scale 1.1)

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
(interpolation-mq-scale 1.0e-10)

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

(end)
