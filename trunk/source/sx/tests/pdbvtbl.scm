;
; PDBVTBL.SCM - PDBView table tests
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(define look "original")

(plot-date off)

(define dbg #f)
(define mode 'batch)

; demo
(cond ((eqv? mode 'demo)
       (data-id on)
       (plot-title on)
       (plot-labels on)
       (define tag "DEMO"))

; batch test
      ((eqv? mode 'batch)
       (cw "pdbvtbl" "MONOCHROME" "MPEG" 0 0 256 256)
       (mpeg-flag off)          ; prevents MPEG being used with HC
       (data-id off)
       (plot-title off)
       (plot-labels off)
       (define tag "BATCH")))

(ps-name "pdbvtbl")
(ps-flag on)

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
    (overlay on)
    (ref-mesh on)
    (wu)
    (hc)
    (lsv)
    (dl))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (plot-1d-table fname ti ncl dname xi rname fi)
   (let* ((shape (read-table* fname ti ncl))
	  (np    (list-ref shape 0))
	  (f     (table->pm-mapping (sprintf "%s->%s" dname rname)
				    (list dname (list np) xi)
				    (list rname (list np) fi)
				    node)))
	 (display-mapping* f)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (plot-2d-table fname ti ncl imx jmx dname xi yi rname fi)
   (let* ((shape (read-table* fname ti ncl))
	  (f     (table->pm-mapping (sprintf "%s->%s" dname rname)
				    (list dname (list imx jmx) xi yi)
				    (list rname (list imx jmx) fi)
				    node)))
	 (display-mapping* f)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(if (or (not (file? "test_4.out"))
	(not (file? "test_5.out")))
     (begin (remarks "Missing needed data files")
	    (quit 1)))

;--------------------------------------------------------------------------

(remarks "Plot test #1 data")

(plot-1d-table "test_4.out" 1 1 "x" 1 "f" 3)

(show)

;--------------------------------------------------------------------------

; 2D table plots

(remarks "Plot test #2 data")

(plot-2d-table "test_5.out" 2 1 10 15 "xi" 2 3 "fi" 4)

(dr 1 (render fill-poly))

(show)

;--------------------------------------------------------------------------

(end)

