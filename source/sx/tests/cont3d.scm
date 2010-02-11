#!pdbview -l
;
; CONT3D - 3D contour plots
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(plot-date off)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MAKE-DATA-1 - make a file with no special data structures

(define (make-data-1)
    (let* ((file (change-file* "demo.d1" "w")))
     
        (make-defstr* file "tv"
		      (def-member integer n)
		      (def-member double x 4 5)
		      (def-member double y 4 5)
		      (def-member double f 4 5)
		      (def-member double u 4 5)
		      (def-member double v 4 5))

        (make-defstr* file "tw"
		      (def-member integer n)
		      (def-member double x 4)
		      (def-member double y 5)
		      (def-member double g 4 5))

; write a data structure
	(write-pdbdata file "arec"
		       (type "tv")
		       (list 10
; x values
			     '(1.0 2.0 3.0 4.0
			       1.0 2.0 3.0 4.0
			       1.0 2.0 3.0 4.0
			       1.0 2.0 3.0 4.0
			       1.0 2.0 3.0 4.0)
; v values
			     '(5.0 5.0 5.0 5.0
			       6.0 6.0 6.0 6.0
			       7.0 7.0 7.0 7.0
			       8.0 8.0 8.0 8.0
			       9.0 9.0 9.0 9.0)
; f values
			     '(1.00 0.80 0.50 0.20
			       0.80 0.70 0.40 0.10
			       0.50 0.40 0.20 0.02
			       0.20 0.10 0.03 0.01
			       0.10 0.02 0.01 0.00)

; u values
			     '(0.0 0.0 0.0 0.0
			       0.1 0.1 0.1 0.1
			       0.2 0.2 0.2 0.2
			       0.3 0.3 0.3 0.3
			       0.4 0.4 0.4 0.4)
; v values
			     '(0.0 0.1 0.2 0.3
			       0.0 0.1 0.2 0.3
			       0.0 0.1 0.2 0.3
			       0.0 0.1 0.2 0.3
			       0.0 0.1 0.2 0.3)))

; write a data structure
	(write-pdbdata file "brec"
		       (type "tw")
		       (list 10
; x values
			     '(1.0 2.0 3.0 4.0)

; y values
			     '(5.0 6.0 7.0 8.0 9.0)

; g values
			     '(1.50 0.10 0.50 0.90
			       0.90 0.70 0.80 0.95
			       0.60 0.40 0.60 0.97
			       0.40 0.30 0.70 0.98
			       0.30 0.2  0.80 1.00)))

	(close* file)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(autoplot off)

(if (not (file? "demo.d1"))
    (make-data-1))

(autoplot on)

(define window-size 0.35)

(cf demo.d1)

;--------------------------------------------------------------------------

;                          WINDOW #1 - A

;--------------------------------------------------------------------------

(cw "A" "COLOR" "WINDOW" 0.01 0.01 window-size window-size)
;(cw "A" "COLOR" "PS"     0.00 0.00 1.0 1.0)
;(cw "A" "COLOR" "PNG"    0.00 0.00 400.0 400.0)

;(plot arec.f ''(arec.x arec.y))
(dl)

(hyper-plane 0 '(1 0 1 20) '(1 0 1 20) '(1 0 1 20))
(sqrx 1)

(vr contour)

(va 0 -45)
(wu)
(pause)

(va 80 -5)
(wu)
(pause)

(va 70 -5)
(wu)
(pause)

(va 60 -5)
(wu)
(pause)

(va 50 -5)
(wu)
(pause)

(va 40 -5)
(wu)
(pause)

(va 30 -5)
(wu)
(pause)

(va 30 -10)
(wu)
(pause)

(va 30 -15)
(wu)
(pause)

(va 30 -20)
(wu)
(pause)

(va 30 -25)
(wu)
(pause)

(va 30 -30)
(wu)
(pause)

(close demo.d1)

(end)
