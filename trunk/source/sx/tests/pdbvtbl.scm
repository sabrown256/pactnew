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

(define dbg #f)
(define mode 'batch)
(ps-name "pdbvtbl")

; demo
(cond ((eqv? mode 'demo)
;       (define (display name x y dx dy)
;	   (cw name "COLOR" "WINDOW" x y dx dy))

       (define tag "DEMO"))

; batch test
      ((eqv? mode 'batch)

;       (define (display name x y dx dy)
;	   (cw name "COLOR" "PS" 0 0 1 1))

       (define tag "BATCH")
       (define look "original")))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (remarks first . rest)
    (printf nil "\n\n(%s) %s\n" tag first)
    (if rest
	(for-each (lambda (x) (printf nil "       %s\n" x)) rest))
    (if dbg
	(pause)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (show)
    (ref-mesh on)
    (if (eqv? mode 'demo)
	(begin (wu)
	       (pause))
	(hc))
    (dl))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (plot-1d-table fname ti ncl dname xi yi rname fi)
   (let* ((shape (read-table* fname ti ncl))
	  (np    (list-ref shape 0))
	  (f     (table->pm-mapping (sprintf "%s->%s" dname rname)
				    (list dname (list np) xi yi)
				    (list rname (list np) fi)
				    node)))

         (if dbg
	     (print-pdb nil (list (pm-mapping->pdbdata f) 4)))
	 (display-mapping* f)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (plot-2d-table fname ti ncl imx jmx dname xi yi rname fi)
   (let* ((shape (read-table* fname ti ncl))
	  (f     (table->pm-mapping (sprintf "%s->%s" dname rname)
				    (list dname (list imx jmx) xi yi)
				    (list rname (list imx jmx) fi)
				    node)))

         (if dbg
	     (print-pdb nil (list (pm-mapping->pdbdata f) 4)))
	 (display-mapping* f)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(if (or (not (file? "test_1.out"))
	(not (file? "test_2.out")))
     (begin (remarks "Missing needed data files")
	    (quit 1)))

;(define window-size 0.35)
(overlay on)

;--------------------------------------------------------------------------
(trace plot-1d-table plot-2d-table wu hc dl)
(trace wm-win-hardcopy vp-hardcopy)
(remarks "Plot test #1 data")

(plot-1d-table "test_1.out" 1 1 "x" 1 2 "f" 3)
(plot-2d-table "test_1.out" 2 1 10 15 "xi" 2 3 "fi" 4)

(lsv)
(dr 1 (render fill-poly))
(dr 2 (render scatter-plot))
(show)

;--------------------------------------------------------------------------

; 2D table plots

(remarks "Plot test #2 data")

(plot-1d-table "test_2.out" 1 1 "x" 1 2 "f" 3)
(plot-2d-table "test_2.out" 2 1 10 15 "xi" 2 3 "fi" 4)

(lsv)
(dr 1 (render fill-poly))
(dr 2 (render scatter-plot))
(show)
(lsv)
(dl)

;--------------------------------------------------------------------------

(end)

