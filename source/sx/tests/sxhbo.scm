; 
; SXHBO.SCM - test binary operations
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;
 
(define solid 1)
(define dashed 2)
(define dotted 3)
(define dotdashed 4)

(define zone -1)
(define node -2)
(define face -3)

(define black         0)
(define blue          1)
(define green         2)
(define cyan          3)
(define red           4)
(define magenta       5)
(define brown         6)
(define white         7)
(define gray          8)
(define lght_blue     9)
(define lght_green   10)
(define lght_cyan    11)
(define lght_red     12)
(define yellow       13)
(define lght_magenta 14)
(define lght_white   15)

(define cartesian -1)
(define polar     -2)
(define insel     -3)

(define contour    11)
(define image      12)
(define wire-frame 13)
(define shaded     14)
(define vect       15)

(define dev-a  (pg-make-device "unix:0.0" "MONOCHROME" "Display A"))
(define dev-b  (pg-make-device "unix:0.0" "COLOR" "Display B"))
(define dev-c  (pg-make-device "unix:0.0" "COLOR" "Display C"))

; (define rx1 (list->pm-array 0.0 1.1 2.2  0.1 1.2 2.3  0.2 1.3 2.4))
; (define ry1 (list->pm-array 0.0 0.1 0.2  1.3 1.4 1.5  2.6 2.8 3.0))
; (define rz1 (list->pm-array 1.0 2.0 3.0  1.0 2.0 3.0  1.0 2.0 3.0))

; (define rx1 (list->pm-array 0.0 1.0 2.0  0.0 1.0 2.0  0.0 1.0 2.0))
; (define ry1 (list->pm-array 0.0 0.0 0.0  1.5 1.5 1.5  3.0 3.0 3.0))
; (define rz1 (list->pm-array 1.0 2.0 3.0  1.0 2.0 3.0  1.0 2.0 3.0))

; (define rx2 (list->pm-array 0.0 1.0 2.0  0.0 1.0 2.0  0.0 1.0 2.0  0.0 1.0 2.0))
; (define ry2 (list->pm-array 0.0 0.0 0.0  1.0 1.0 1.0  2.0 2.0 2.0  3.0 3.0 3.0))
; (define rz2 (list->pm-array 1.0 1.0 1.0  1.6 1.6 1.6  2.3 2.3 2.3  3.0 3.0 3.0))

(define rx1 (list->pm-array
                 0.0 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0
                 0.0 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0
                 0.0 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0
                 0.0 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0
                 0.0 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0

                 0.0 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0
                 0.0 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0
                 0.0 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0
                 0.0 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0
                 0.0 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0))

(define ry1 (list->pm-array
                 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0
                 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
                 2.0 2.0 2.0 2.0 2.0 2.0 2.0 2.0 2.0 2.0
                 3.0 3.0 3.0 3.0 3.0 3.0 3.0 3.0 3.0 3.0
                 4.0 4.0 4.0 4.0 4.0 4.0 4.0 4.0 4.0 4.0
                 5.0 5.0 5.0 5.0 5.0 5.0 5.0 5.0 5.0 5.0
                 6.0 6.0 6.0 6.0 6.0 6.0 6.0 6.0 6.0 6.0
                 7.0 7.0 7.0 7.0 7.0 7.0 7.0 7.0 7.0 7.0
                 8.0 8.0 8.0 8.0 8.0 8.0 8.0 8.0 8.0 8.0
                 9.0 9.0 9.0 9.0 9.0 9.0 9.0 9.0 9.0 9.0))

(define rz1 (list->pm-array
                 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0 10.0
                 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0 10.0
                 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0 10.0
                 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0 10.0
                 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0 10.0
                 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0 10.0
                 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0 10.0
                 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0 10.0
                 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0 10.0
                 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0 10.0))

(define rx2 (list->pm-array 2.7 5.7 8.7  2.7 5.7 8.7  2.7 5.7 8.7))
(define ry2 (list->pm-array 2.7 2.7 2.7  5.7 5.7 5.7  8.7 8.7 8.7))
(define rz2 (list->pm-array 3.7 3.7 3.7  6.7 6.7 6.7  9.7 9.7 9.7))

(define dom-a (pm-make-set "doma" '(10 10) (list rx1 ry1)))
(define ran-a (pm-make-set "rana" '(100) (list rz1)))

(define dom-b (pm-make-set "domb" '(3 3) (list rx2 ry2)))
(define ran-b (pm-make-set "ranb" '(9) (list rz2)))

(define gr-b (pg-make-graph dom-a ran-a))
(define gr-a (pg-make-graph dom-b ran-b))

(define gr-c (+ gr-a gr-b))
(define gr-d (- gr-a gr-b))
(define gr-e (* gr-a gr-b))
(define gr-f (/ gr-a gr-b))

(pg-open-device dev-a  0.02 0.04 0.3  0.3)
(pg-open-device dev-b  0.02 0.54 0.3  0.3)
; (pg-open-device dev-b  0.34 0.04 0.3  0.3)
; (pg-open-device dev-c  0.66 0.04 0.3  0.3)
(pg-open-device dev-c  0.5 0.04 0.45  0.45)

(define theta 45.0)
(define phi -45.0)

(pg-draw-graph dev-a   gr-a wire-frame 45 -45)
(pg-draw-graph dev-b   gr-b wire-frame 45 -45)

; (pg-draw-graph dev-a   gr-a contour)
; (pg-draw-graph dev-b   gr-b contour)

(pg-draw-graph dev-c   gr-c wire-frame theta phi)
; (pg-draw-graph dev-c   gr-c contour)
(printf nil "Plotted a+b\n")
(read)

(pg-draw-graph dev-c   gr-d wire-frame theta phi)
; (pg-draw-graph dev-c gr-d contour)
(printf nil "Plotted a-b\n")
(read)

(pg-draw-graph dev-c   gr-e wire-frame theta phi)
; (pg-draw-graph dev-c gr-e contour)
(printf nil "Plotted a*b\n")
(read)

(pg-draw-graph dev-c   gr-f wire-frame theta phi)
; (pg-draw-graph dev-c gr-f contour)
(printf nil "Plotted a/b\n")
(read)

(quit)

; (cos gr-a)
; (log10 gr-b)
; (exp gr-c)
; (printf nil "Graphs replotted\n")
; (pg-draw-graph dev-a   gr-a)
; (pg-draw-graph dev-b   gr-b contour)
; (pg-draw-graph dev-ba  gr-b wire-frame)
; (pg-draw-graph dev-c   gr-c vect)
; (read)
