; 
; SXTEST.SCM - look at what's in a PDB file
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

(define dev-a  (pg-make-device "unix:0.0" "MONOCHROME" "Display A"))
(define dev-b  (pg-make-device "unix:0.0" "COLOR" "Display B"))
(define dev-c  (pg-make-device "unix:0.0" "COLOR" "Display C"))

(pg-open-device dev-a 0.02 0.04 0.3 0.3)
(pg-open-device dev-b 0.34 0.04 0.3 0.3)
(pg-open-device dev-c 0.66 0.04 0.3 0.3)

(define rx1 (list->pm-array 0.0 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0))
(define ry1 (list->pm-array 0.0 1.0 4.0 9.0 16.0 23.0 28.0 31.0 32.0 32.0))

(define dom-a (pm-make-set "doma" '(10) (list rx1)))
(define ran-a (pm-make-set "rana" '(10) (list ry1)))

(define rx2 (list->pm-array 0.0 1.0 2.0 0.0 1.0 2.0 0.0 1.0 2.0))
(define ry2 (list->pm-array 0.0 0.0 0.0 1.0 1.0 1.0 2.0 2.0 2.0))

(define rz2 (list->pm-array 0.0 0.5 1.0 2.0 1.0 0.0 0.0 2.0 4.0))
(define dom-b (pm-make-set "domb" '(3 3) (list rx2 ry2)))
(define ran-b (pm-make-set "ranb" '(9) (list rz2)))

(define u2 (list->pm-array 0.0 0.0 0.0 0.1 0.1 0.1 0.2 0.2 0.2))
(define v2 (list->pm-array 0.2 0.1 0.0 0.2 0.1 0.0 0.2 0.1 0.0))
(define ran-c (pm-make-set "ranc" '(9 9) (list u2 v2)))

(define gr-a (pg-make-graph dom-a ran-a))
(define gr-b (pg-make-graph dom-b ran-b))
(define gr-c (pg-make-graph dom-b ran-c))

(pg-draw-graph dev-a gr-a)
(pg-draw-graph dev-b gr-b contour)
(pg-draw-graph dev-c gr-c vect)

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

(printf nil "Graphs replotted\n")

;(read)

;(pg-close-device dev-a)
;(pg-close-device dev-b)
;(pg-close-device dev-c)

