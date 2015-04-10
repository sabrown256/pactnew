#!pdbview -l
;
; SURF2D.SCM - test 2d surface plots
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(plot-date off)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MAKE-DATA-2 - make a file with no special data structures

(define (make-data-2)
    (let* ((file (change-file* "demo.tmp" "w"))
	   (map  (hyper-plane 0.0
			      '(1.0 0.01 2.0 10))))
     
       (pg-def-graph-file file)

       (define (mk-curve n)
	  (if (> n 0)
	      (let* ((name (sprintf "Mapping%d" n))
		     (m    (+ -0.5 (/ n 5)))
		     (labl (sprintf "x ^ %s" m))
		     (smp  (label-drawable labl
					   (powr m (copy-map map)))))
		    (pm-mapping->pdbdata smp file)
		    (mk-curve (- n 1)))))

       (mk-curve 20)
       (close* file)

       (cf    "demo.tmp")
       (cmc   "demo.d2" *)
       (close "demo.d2")
       (close "demo.tmp")

       (dl)
       (unlink "demo.tmp")))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(autoplot off)

(if (not (file? "demo.d2"))
    (make-data-2))

(autoplot on)

(define window-size 0.35)

;--------------------------------------------------------------------------

(cf demo.d2)

(cw "C" "COLOR" "WINDOW" 0.01 0.01 window-size window-size)

(pl (lrs '("f"    0 (yval0 yval1 yval2 yval3 yval4 yval5
		     yval6 yval7 yval8 yval9
		     yval10 yval11 yval12 yval13 yval14 yval15
		     yval16 yval17 yval18 yval19))
	 '("{t,x}" 1 (xval0 xval0 xval0 xval0 xval0 xval0
		      xval0 xval0 xval0 xval0
		      xval0 xval0 xval0 xval0 xval0 xval0
		      xval0 xval0 xval0 xval0))))

(printf nil "domain> %s\n" (get-domain 1))
(printf nil "range>  %s\n" (get-range 1))
(label-drawable "f->{t,x}" 1)

(vr wire-frame)
(palette "rainbow")
(vr shaded)

(va 60 25 0)
(wu)
(pause)

(animate 0 1 0)
(pause)

(animate 30 1 0)
(pause)

(animate 60 1 0)
(pause)

(animate 90 1 0)
(pause)

(close demo.d2)

(end)
