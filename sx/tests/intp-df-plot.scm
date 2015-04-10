;
; INTP-DF-PLOT.SCM - plot the interpolation performance data
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(x-log-scale on)
(y-log-scale on)
(axis-y-format "%10.2e")
(grid on)
;(label-space 0.2)
;(label-color-flag on)
(ps-type color)
(ps-name "intp-df.ps")

(define (pl-1d)
   (let (iwd mq)
        (set! iwd (color (cur (menu "1d*iwd")) red))
        (set! mq  (color (cur (menu "1d*mq")) blue))
        (lnstyle (lst "*L1*") dotted)
        (annot "1D Interpolations" 1 0.4 0.6 0.97 0.99)
        (replot)
        (hc)
        (era)))

(define (pl-2d)
   (let (iwd mq)
        (set! iwd (color (cur (menu "2d*iwd")) red))
        (set! mq  (color (cur (menu "2d*mq")) blue))
        (lnstyle (lst "*L1*") dotted)
        (annot "2D Interpolations" 1 0.4 0.6 0.97 0.99)
        (replot)
        (hc)
        (era)))

(define (pl-3d)
   (let (iwd mq)
        (set! iwd (color (cur (menu "3d*iwd")) red))
        (set! mq  (color (cur (menu "3d*mq")) blue))
        (lnstyle (lst "*L1*") dotted)
        (annot "3D Interpolations" 1 0.4 0.6 0.97 0.99)
        (replot)
        (hc)
        (era)))

(define (pl-4d)
   (let (iwd mq)
        (set! iwd (color (cur (menu "4d*iwd")) red))
        (set! mq  (color (cur (menu "4d*mq")) blue))
        (lnstyle (lst "*L1*") dotted)
        (annot "4D Interpolations" 1 0.4 0.6 0.97 0.99)
        (replot)
        (hc)
        (era)))

(define (pl-nd)
   (let (iwd mq)
        (set! iwd (color (cur (menu "*iwd")) red))
        (set! mq  (color (cur (menu "*mq")) blue))
        (lnstyle (lst "*L1*") dotted)
        (annot "Interpolations" 1 0.4 0.6 0.97 0.99)
        (replot)
        (hc)
        (era)))

(rd intp-df.u)

(pl-1d)
(pause)

(pl-2d)
(pause)

(pl-3d)
(pause)

(pl-4d)
(pause)

(pl-nd)
(pause)

(end)

