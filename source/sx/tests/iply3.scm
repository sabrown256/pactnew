; 
; IPLY3.SCM - intersecting polygon test
;           - test multiple polygon result
; 
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(define dev (pg-make-device "PS" "COLOR" "iply 2.0 2.0"))
;(define dev (pg-make-device "SCREEN" "COLOR" "Intersecting Polygon"))
(ps-dots-inch 100)
(axis-number-minor-ticks 2)
(pg-open-device dev 0 0 0 0)

(pg-set-viewport! dev 0.1 0.9 0.1 0.9)
(pg-set-world-coordinate-system! dev -0.5 1.2 -0.5 1.2)
(pg-set-clipping! dev #t)

; NOTE - polygons must be closed
(define quad    (pm-make-polygon 2  0.0 0.0   1.0 0.0
				    1.0 1.0   0.0 1.0
				    0.0 0.0))

(define p1  (pm-make-polygon 2  -0.2 -0.2
                                -0.2  0.3   0.2  0.5  -0.2  0.8
                                -0.4 -0.4
                                 0.8 -0.2    0.5  0.2   0.3 -0.2
                                -0.2 -0.2))

(define p2  (pm-make-polygon 2   0.1 -0.2
                                 0.1  0.3   0.2  0.5  -0.2  0.8
                                -0.4 -0.4
                                 0.8 -0.2    0.5  0.2   0.3 -0.2
                                 0.1 -0.2))

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
;	  (pause)
	  ))

(define (seq pa pb)
    (draw pm-intersect-polygon pa pb)
    (draw pm-union-polygon     pa pb)
    (draw pm-intersect-polygon pb pa)
    (draw pm-union-polygon     pb pa))

(seq p1 quad)
(seq p2 quad)
(seq p1 p2)

(end)
