#!pdbview -l
;
; PDBVTBL.SCM - PDBView table tests
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(plot-date off)

; demo
(define (display name x y dx dy)
   (cw name "COLOR" "WINDOW" x y dx dy))

(define tag "DEMO")


; batch test

;(define (display name x y dx dy)
;   (cw name "COLOR" "PS" 0 0 1 1))

; redefine pause as a no-op
;(define (pause) #t)

;(define tag "BATCH")
;(define look "original")

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (remarks first . rest)
    (printf nil "\n\n(%s) %s\n" tag first)
    (if rest
	(for-each (lambda (x) (printf nil "       %s\n" x)) rest))
;    (pause)
    )

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (plot-1d-table fname ti ncl dname xi yi rname fi)
   (let* ((shape (read-table* fname ti ncl))
	  (np    (list-ref shape 0))
	  (f     (table->pm-mapping (sprintf "%s->%s" dname rname)
				    (list dname (list np) xi yi)
				    (list rname (list np) fi)
				    node)))

;	 (print-pdb nil (list (pm-mapping->pdbdata f) 4))
	 (display-mapping* f)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (plot-2d-table fname ti ncl imx jmx dname xi yi rname fi)
   (let* ((shape (read-table* fname ti ncl))
	  (f     (table->pm-mapping (sprintf "%s->%s" dname rname)
				    (list dname (list imx jmx) xi yi)
				    (list rname (list imx jmx) fi)
				    node)))

;	 (print-pdb nil (list (pm-mapping->pdbdata f) 4))
	 (display-mapping* f)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(if (or (not (file? "test_1.out"))
	(not (file? "test_2.out"))
	(not (file? "test_3.out"))
	(not (file? "test_4.out"))
	(not (file? "test_5.out")))
     (begin (remarks "Missing needed data files")
	    (quit 1)))

(define window-size 0.35)
(overlay on)

;--------------------------------------------------------------------------

(remarks "Plot test #4 data")

(plot-1d-table "test_4.out" 1 1 "x" 1 2 "f" 3)
(plot-2d-table "test_4.out" 2 1 10 15 "xi" 2 3 "fi" 4)

(lsv)
(dr 1 (render fill-poly))
(dr 2 (render scatter-plot))
(ref-mesh on)
(wu)
(pause)
(dl)

;--------------------------------------------------------------------------

(remarks "Plot test #5 data")

(plot-1d-table "test_5.out" 1 1 "x" 1 2 "f" 3)
(plot-2d-table "test_5.out" 2 1 10 15 "xi" 2 3 "fi" 4)

(lsv)
(dr 1 (render fill-poly))
(dr 2 (render scatter-plot))
(ref-mesh on)
(wu)
(pause)
(lsv)
(dl)

;--------------------------------------------------------------------------

(end)

