; 
; SHOW-POLY-COMB.SCM - show polygon intersection/union
;                    - diagnostic demo code
; 
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; define the lists of points
; the polygons will be constructed from them
; NOTE - polygons must be closed

(define p1  (list -0.2 -0.2
		  -0.2  0.3   0.2  0.5  -0.2  0.8
		  -0.4 -0.4
		   0.8 -0.2    0.5  0.2   0.3 -0.2
		  -0.2 -0.2))

(define p2  (list  0.1 -0.2
		   0.1  0.3   0.2  0.5  -0.2  0.8
		  -0.4 -0.4
		   0.8 -0.2    0.5  0.2   0.3 -0.2
		   0.1 -0.2))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define dev (pg-make-device "SCREEN" "COLOR" "Intersecting Polygon"))
(pg-open-device dev 0 0 0 0)

(define (pt-extr prd pts off v)
    (if (pair? pts)
	(let* ((vt (list-ref pts off))
	       (vm (prd v vt)))
	      (pt-extr prd (list-tail pts 2) off vm))
	v))

(define (setup la lb)
    (let* ((pts (append la lb))
	   (xmn (* 1.2 (pt-extr min pts 0  1.0e100)))    ; -0.5
	   (ymn (* 1.2 (pt-extr min pts 1  1.0e100)))    ; -0.5
	   (xmx (* 1.2 (pt-extr max pts 0 -1.0e100)))    ; 1.2
	   (ymx (* 1.2 (pt-extr max pts 1 -1.0e100))))   ; 1.2

          (axis-number-minor-ticks 2)
          (pg-set-viewport! dev 0.1 0.9 0.1 0.9)
	  (pg-set-world-coordinate-system! dev xmn xmx ymn ymx)
	  (pg-set-clipping! dev #t)))

(define (draw fnc pa pb)
    (let* ((ip  (fnc pa pb)))

          (pg-clear-window dev)

	  (define (do-one x)
	          (apply pg-fill-polygon (list dev gray x)))

	  (for-each do-one ip)

          (pg-set-line-color! dev red)
	  (apply pg-draw-polyline-n (list dev 2 @world #t pa))

          (pg-set-line-color! dev blue)
	  (apply pg-draw-polyline-n (list dev 2 @world #t pb))

	  (pg-axis dev)
	  (pg-finish-plot dev)
	  (pause)))

(define (seq la lb)
    (let* ((pa (apply pm-make-polygon (cons 2 la)))
	   (pb (apply pm-make-polygon (cons 2 lb))))

          (setup la lb)

	  (draw pm-intersect-polygon pa pb)
	  (draw pm-union-polygon     pa pb)
	  (draw pm-intersect-polygon pb pa)
	  (draw pm-union-polygon     pb pa)))

(seq p1 p2)

(end)
