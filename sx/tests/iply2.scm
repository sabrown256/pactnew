; 
; IPLY2.SCM - intersecting polygon test
;           - intersection and union are explicit
; 
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(define dev (pg-make-device "PS" "COLOR" "iply2 2.0 2.0"))
;(define dev (pg-make-device "SCREEN" "COLOR" "Intersecting Polygon"))
(ps-dots-inch 100)
(axis-number-minor-ticks 2)
(pg-open-device dev 0 0 0 0)

(pg-set-viewport! dev 0.1 0.9 0.1 0.9)
(pg-set-world-coordinate-system! dev -0.3 1.2 -0.4 1.2)
(pg-set-clipping! dev #t)

; NOTE - polygons must be closed
(define tri     (pm-make-polygon 2  0.0 0.0   0.5  0.5
				    0.0 1.0   0.0 0.0))
(define quad    (pm-make-polygon 2  0.0 0.0   0.8 -0.2
				    0.8 1.0   0.1  0.9
				    0.0 0.0))
(define bowtie  (pm-make-polygon 2  0.0 0.0   0.8  1.0
				    0.1 0.9   0.8 -0.2
				    0.0 0.0))
(define chevron (pm-make-polygon 2  0.0 0.0   0.5  0.5
				    0.0 1.0   0.35 0.48
				    0.0 0.0))
(define saw     (pm-make-polygon 2  0.0 0.0   0.8 0.0
				    0.3 0.2   0.8 0.3
				    0.3 0.4   0.8 0.5
				    0.3 0.6   0.8 0.7
				    0.3 0.8   0.8 1.0
				    0.0 1.0   0.0 0.0))

(define (draw fnc ply xmn xmx ymn ymx)
    (let* ((bnd (pm-make-polygon 2  xmn ymn  xmx ymn  xmx ymx  xmn ymx  xmn ymn))
	   (ip  (fnc ply bnd)))

          (pg-clear-window dev)

	  (define (do-one x)
	          (apply pg-fill-polygon (list dev gray x)))

	  (if (pair? ip)
	      (for-each do-one ip))

          (pg-set-line-color! dev red)
	  (apply pg-draw-polyline-n (list dev 2 @world #t ply))

          (pg-set-line-color! dev blue)
	  (apply pg-draw-polyline-n (list dev 2 @world #t bnd))

	  (pg-axis dev)
	  (pg-finish-plot dev)
;	  (pause)
	  ))

; NOTE: the union and intersection algorithms have
; problems with intersection points that are also
; nodes of the polygon
; when that is fixed use 0.0
;(define ymn 0.0)
(define ymn -0.01)

(define (inter ply)
    (draw pm-intersect-polygon ply -0.2 1.0  -0.3 1.1)
    (draw pm-intersect-polygon ply -0.2 1.0   ymn 1.1)
    (draw pm-intersect-polygon ply -0.2 1.0   0.2 1.1)

    (draw pm-intersect-polygon ply -0.2 0.4  -0.3 1.1)
    (draw pm-intersect-polygon ply -0.2 0.4   ymn 1.1)
    (draw pm-intersect-polygon ply -0.2 0.4   0.2 1.1)

    (draw pm-intersect-polygon ply 0.32 1.0  -0.3 1.1)
    (draw pm-intersect-polygon ply 0.32 1.0   ymn 1.1)
    (draw pm-intersect-polygon ply 0.32 1.0   0.2 1.1)

    (draw pm-intersect-polygon ply -0.1 0.4   ymn 0.7)
    (draw pm-intersect-polygon ply -0.1 0.4   0.2 0.7))

(define (union ply)
    (draw pm-union-polygon ply -0.2 1.0  -0.3 1.1)
    (draw pm-union-polygon ply -0.2 1.0  ymn 1.1)
    (draw pm-union-polygon ply -0.2 1.0   0.2 1.1)

    (draw pm-union-polygon ply -0.2 0.4  -0.3 1.1)
    (draw pm-union-polygon ply -0.2 0.4  ymn 1.1)
    (draw pm-union-polygon ply -0.2 0.4   0.2 1.1)

    (draw pm-union-polygon ply 0.32 1.0  -0.3 1.1)
    (draw pm-union-polygon ply 0.32 1.0  ymn 1.1)
    (draw pm-union-polygon ply 0.32 1.0   0.2 1.1)

    (draw pm-union-polygon ply -0.1 0.4  ymn 0.7)
    (draw pm-union-polygon ply -0.1 0.4   0.2 0.7))

(inter tri)
(inter quad)
(inter chevron)
(inter saw)
(inter bowtie)

(union tri)
(union quad)
(union chevron)
(union saw)
(union bowtie)

(end)
