; 
; SXGROT.SCM - look at what's in a PDB file
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;
 
;(ld consts.scm)

(define zone Z_CENT)
(define node N_CENT)
(define face F_CENT)

(define dev-a  (pg-make-device "unix:0.0" "COLOR" "Display A"))

(pg-open-device dev-a  0.02 0.04 0.3  0.3)

(define iso (list->c-array 2 2 2
			   3 3 3 3 3 3
			   4 4 4 4 4 4 4 4))
(define en (list->c-array -280 -260 -250
			  -270 -230 -210 -205 -202 -200
			  -240 -200 -170 -150 -140 -135 -132 -130))

(define tr (list->c-array 5 6 7 8 9 1 2 3 4))
(define up (list->c-array 5 7 9 10 11 12 13 14 15 16))
(define lo (list->c-array 0 1 3 3 4 4 4 5 5))

(define dom-a (pm-make-set "Grotrian-Domain" '(17) (list iso en)))
(define ran-a (pm-make-set "Transitions" '(9) (list tr up lo)))

(define gr-a (pg-make-graph dom-a ran-a "Test Data"))

(pm-set-mapping-type gr-a "Grotrian-Diagram")

(pg-draw-graph dev-a   gr-a)

(define outfile (create-pdbfile "test-grot.pdb"))

(pg-def-graph-file outfile)

(display (list-defstrs outfile))
(newline)

(display (pg-graph->pdbdata gr-a outfile "grotrian"))
(newline)

(close-pdbfile outfile)

(define infile (open-pdbfile "test-grot.pdb"))

(display (list-defstrs infile))
(newline)

(define gr-a-r (pdbdata->pg-graph infile "grotrian"))

(printf nil "Graphs read from file\n")

(pg-draw-graph dev-a gr-a-r)

(pg-draw-graph dev-a   gr-a-r)
