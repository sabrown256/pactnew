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
;       -    [psz]   - text point size
;       -    [style] - text style - bold, etc
;       -    [font]  - text font name

(define (annot txt . rest)
    "Procedure: add an annotation by writing text at a point specified in
     normalized coordinates.
     Usage: annot <text> [<color> [<xmin> <xmax> <ymin> <ymax> [<ang> [<aln> [psz [style [font]]]]]]]
     Example: annot \"look here\" blue 0.4 0.6 0.95 0.98"
    (let* ((dev (cond ((defined? pdbview)
		       (window-device current-window))
		      ((defined? screen-window)
		       screen-window)))
		       
	   (in  (if rest (length rest) 0))
	   (clr (if (> in 0) (list-ref rest 0) white))
	   (x1  (if (> in 1) (list-ref rest 1) 0.0))
	   (x2  (if (> in 2) (list-ref rest 2) 0.0))
	   (y1  (if (> in 3) (list-ref rest 3) 0.0))
	   (y2  (if (> in 4) (list-ref rest 4) 0.0))
	   (ang (if (> in 5) (list-ref rest 5) 0.0))
	   (aln (if (> in 6) (list-ref rest 6) center))
	   (psz (if (> in 7) (list-ref rest 7) -1))
	   (sty (if (> in 8) (list-ref rest 8) nil))
	   (fnt (if (> in 9) (list-ref rest 9) nil)))

          (if dev
	      (add-annotation dev txt clr x1 x2 y1 y2
			      aln ang psz sty fnt))))

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
