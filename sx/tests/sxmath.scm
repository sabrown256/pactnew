; 
; SXMATH.SCM - look at math functionality and structures
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;
 
; (ld const.scm)

(define zone Z_CENT)
(define node N_CENT)
(define face F_CENT)

(define rx1 (list->c-array 0.0 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0))
(define ry1 (list->c-array 0.1 1.0 4.0 9.0 16.0 23.0 28.0 31.0 32.0 32.0))

(define dom-a (pm-make-set "doma" '(10) (list rx1)))
(define ran-a (pm-make-set "rana" '(10) (list ry1)))

(define rx2 (list->c-array 0.0 1.0 2.0 0.0 1.0 2.0 0.0 1.0 2.0))
(define ry2 (list->c-array 0.0 0.0 0.0 1.0 1.0 1.0 2.0 2.0 2.0))

(define rz2 (list->c-array 1.0 5.0 20.0 10.0 50.0 10.0 50.0 20.0 5.0))
(define dom-b (pm-make-set "domb" '(3 3) (list rx2 ry2)))
(define ran-b (pm-make-set "ranb" '(9) (list rz2)))

(define u2 (list->c-array 0.0 0.0 0.0 0.1 0.1 0.1 0.2 0.2 0.2))
(define v2 (list->c-array 0.2 0.1 0.0 0.2 0.1 0.0 0.2 0.1 0.0))
(define ran-c (pm-make-set "ranc" '(9 9) (list u2 v2)))

(define imd (list->c-array 0.0 0.1 0.2 0.3 0.4 0.5 0.6 0.7
			    0.0 0.1 0.2 0.3 0.4 0.5 0.5 0.6
			    0.0 0.1 0.2 0.3 0.4 0.4 0.5 0.5
			    0.0 0.1 0.1 0.2 0.3 0.4 0.5 0.4
			    0.0 0.0 0.1 0.2 0.2 0.3 0.3 0.3
			    0.0 0.0 0.1 0.1 0.1 0.2 0.2 0.2
			    0.0 0.0 0.0 0.0 0.1 0.1 0.1 0.1
			    0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0))

;
; OPEN some devices
;
(define dev-a  (pg-make-device "unix:0.0" "MONOCHROME" "Display A"))
(pg-open-device dev-a  0.02 0.04 0.3  0.3)

;
; IMAGE construction and rendering
;

(define im-a (pg-make-image imd 8 8
			    "Test Image"
			    -5.0 5.0 10.0 20.0 1.0e-3 1.0e-2))

(pg-draw-image dev-a im-a)

(printf nil "q to Quit: ")
(if (eqv? (read) 'q)
    (quit))

;
; GRAPH construction and rendering
;

(define gr-a (pg-make-graph dom-a ran-a))
(define gr-b (pg-make-graph dom-b ran-b))
(define gr-c (pg-make-graph dom-b ran-c))

;
; OPEN more devices
;
(define dev-b  (pg-make-device "unix:0.0" "COLOR" "Display B"))
(define dev-ba (pg-make-device "unix:0.0" "COLOR" "Display B-A"))
(define dev-c  (pg-make-device "unix:0.0" "COLOR" "Display C"))

(pg-open-device dev-b  0.34 0.04 0.3  0.3)
(pg-open-device dev-ba 0.34 0.47 0.3  0.3)
(pg-open-device dev-c  0.66 0.04 0.3  0.3)

(pg-draw-graph dev-a   gr-a)
(pg-draw-graph dev-b   gr-b contour)
(pg-draw-graph dev-ba  gr-b wire-frame 1 -135)
(pg-draw-graph dev-c   gr-c vect)

(define outfile (create-pdbfile "test.pdb"))

(pg-def-graph-file outfile)

(display (list-defstrs outfile))
(newline)

(display (pg-graph->pdbdata gr-a outfile "lplot"))
(newline)
(display (pg-graph->pdbdata gr-b outfile "cplot"))
(newline)
(display (pg-graph->pdbdata gr-c outfile "vplot"))
(newline)

(close-pdbfile outfile)

(define infile (open-pdbfile "test.pdb"))

(display (list-defstrs infile))
(newline)

(define gr-a-r (pdbdata->pg-graph infile "lplot"))
(define gr-b-r (pdbdata->pg-graph infile "cplot"))
(define gr-c-r (pdbdata->pg-graph infile "vplot"))

(printf nil "Graphs read from file\n")

(pg-draw-graph dev-a gr-a-r)
(pg-draw-graph dev-b gr-b-r contour)
(pg-draw-graph dev-c gr-c-r vect)

(printf nil "Operate on graphs (y to continue)\n")
(read)

(define gr-d-r (+ gr-b-r gr-b-r))
(pg-draw-graph dev-b gr-d-r contour)
(printf nil "Plotted b+b\n")
(read)

(cos gr-a-r)
(log10 gr-b-r)
(exp gr-c-r)

(printf nil "Graphs replotted\n")

(pg-draw-graph dev-a   gr-a-r)
(pg-draw-graph dev-b   gr-b-r contour)
(pg-draw-graph dev-ba  gr-b-r wire-frame)
(pg-draw-graph dev-c   gr-c-r vect)

;(read)

;(pg-close-device dev-a)
;(pg-close-device dev-b)
;(pg-close-device dev-c)

