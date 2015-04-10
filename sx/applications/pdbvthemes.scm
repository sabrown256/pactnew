; 
; PDBVTHEMES.SCM - looks/themes for PDBView
; 
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
; 

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (basis-look)
    (printf nil "\nUsing BASIS look\n\n")
    (define-global overlay-box   (list 0.140 0.735 0.175 0.90))
    (define-global image-box     (list 0.140 0.735 0.175 0.90))
    (define-global contour-box   (list 0.140 0.735 0.175 0.90))
    (define-global mesh-box      (list 0.140 0.735 0.175 0.90))
    (define-global dvb-box       (list 0.140 0.735 0.175 0.90))
    (define-global fill-poly-box (list 0.140 0.735 0.175 0.90))
    (define-global points-box    (list 0.140 0.735 0.175 0.90))
    (define-global vector-box    (list 0.140 0.735 0.175 0.90))
    (set! default-window-width   0.5)
    (set! default-window-height  0.5)
    (axis-number-minor-ticks   5)
    (axis-tick-type            left-of-axis)
    (label-length              40)
    (label-space               0.2)
    (label-type-size           10)
    (numeric-data-ids          on)
    (palette-orientation       vertical)
    (plot-date                 off)
    (plot-labels               on)
    (overlay                   on))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (ultra-look)
    (printf nil "\nUsing ULTRA look\n\n")
    (define-global overlay-box   (list 0.140 0.735 0.175 0.90))
    (define-global image-box     (list 0.140 0.735 0.175 0.90))
    (define-global contour-box   (list 0.140 0.735 0.175 0.90))
    (define-global mesh-box      (list 0.140 0.735 0.175 0.90))
    (define-global dvb-box       (list 0.140 0.735 0.175 0.90))
    (define-global fill-poly-box (list 0.140 0.735 0.175 0.90))
    (define-global points-box    (list 0.140 0.735 0.175 0.90))
    (define-global vector-box    (list 0.140 0.735 0.175 0.90))
    (axis-number-minor-ticks  -1)
    (axis-tick-type           right-of-axis)
    (label-space              0.2)
    (label-type-size          8)
    (numeric-data-ids         on)
    (palette-orientation      vertical)
    (plot-date                on)
    (plot-labels              on)
    (overlay                  on))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (original-look)
;    (printf nil "Using original look\n")
    (define-global overlay-box   (list 0.175 0.85  0.25  0.825))
    (define-global image-box     (list 0.175 0.85  0.25  0.825))
    (define-global contour-box   (list 0.175 0.735 0.175 0.825))
    (define-global mesh-box      (list 0.175 0.85  0.25  0.825))
    (define-global dvb-box       (list 0.175 0.85  0.175 0.85))
    (define-global fill-poly-box (list 0.175 0.85  0.25  0.825))
    (define-global points-box    (list 0.175 0.85  0.25  0.825))
    (define-global vector-box    (list 0.175 0.9   0.175 0.9))
    (axis-number-minor-ticks  -1)
    (axis-tick-type           right-of-axis)
    (label-space              0.0)
    (label-type-size          8)
    (numeric-data-ids         on)
    (palette-orientation      horizontal)
    (plot-date                on)
    (plot-labels              on)
    (overlay                  off))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------


