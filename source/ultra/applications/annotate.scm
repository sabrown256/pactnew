;
; ANNOTATE.SCM - handle textual annotations for ULTRA
;
; Source Version: 4.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
;

;(define-global annotation-list '())

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; ANNOT - add an annotation to the screen
;       - arguments:
;       -    txt     - the text of the annotation
;       -    [color] - the color of the annotation text
;       -    [x, y]  - the location of the lower left insertion point of
;       -              the text block (NDC)
;       -    [ang]   - angle along which to write text
;       -    [aln]   - text alignment

(define (annot txt . rest)
    "Procedure: annotate a plot writing text at a point specified in
     normalized coordinates.
     Usage: annot <text> [<color> [<xmin> <xmax> <ymin> <ymax> [<ang> [<aln>]]]]"
    (let* ((in  (if rest (length rest) 0))
	   (clr (if (> in 0) (list-ref rest 0) white))
	   (x1  (if (> in 1) (list-ref rest 1) 0.0))
	   (x2  (if (> in 2) (list-ref rest 2) 0.0))
	   (y1  (if (> in 3) (list-ref rest 3) 0.0))
	   (y2  (if (> in 4) (list-ref rest 4) 0.0))
	   (ang (if (> in 5) (list-ref rest 5) 0.0))
	   (aln (if (> in 5) (list-ref rest 6) center)))

      (if (defined? screen-window)
	  (add-annotation screen-window txt clr x1 x2 y1 y2 aln ang))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; CLR-ANNOT - clear the current annotations

(define (clr-annot)
    "Procedure: remove any current annotations.
     Usage: clr-annot"
    (if (defined? screen-window)
	(rem-annotations screen-window)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------
