; 
; IPLY.SCM - intersecting polygon test
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

(pg-set-viewport! dev 0.2 0.8 0.2 0.8)
(pg-set-clipping! dev #t)

; NOTE - polygons must be closed
(define tri     (list 0.0 0.0   0.5  0.5    0.0 1.0   0.0 0.0))
(define quad    (list 0.0 0.0   0.8 -0.2    0.8 1.0   0.1  0.9   0.0 0.0))
(define bowtie  (list 0.0 0.0   0.8  1.0    0.1 0.9   0.8 -0.2   0.0 0.0))
(define chevron (list 0.0 0.0   0.5  0.5    0.0 1.0   0.35 0.48  0.0 0.0))
(define saw     (list 0.0 0.0   0.8 0.0
		      0.3 0.2   0.8 0.3
		      0.3 0.4   0.8 0.5
		      0.3 0.6   0.8 0.7
		      0.3 0.8   0.8 1.0
		      0.0 1.0   0.0 0.0))

(define (draw ply xmn xmx ymn ymx)
    (let* ((rx1 0.0)
	   (ry1 0.0)
	   (rx2 1.0)
	   (ry2 1.0)
	   (rx3 0.0)
	   (ry3 2.0))

          (pg-clear-window dev)
          (pg-set-world-coordinate-system! dev xmn xmx ymn ymx)

	  (apply pg-fill-polygon (append (list dev blue) ply))
	  (apply pg-draw-polyline-n (append (list dev 2 @world #t) ply))

	  (pg-axis dev)
	  (pg-finish-plot dev)
;	  (pause)
	  ))

(define (seq ply)

    (draw ply -0.5 1.0  -1.0 2.0)
    (draw ply -0.5 1.0   0.0 2.0)
    (draw ply -0.5 1.0   0.2 2.0)

    (draw ply -0.5 0.4  -1.0 2.0)
    (draw ply -0.5 0.4   0.0 2.0)
    (draw ply -0.5 0.4   0.2 2.0)

    (draw ply 0.32 1.0  -1.0 2.0)
    (draw ply 0.32 1.0   0.0 2.0)
    (draw ply 0.32 1.0   0.2 2.0)

    (draw ply -0.1 0.4   0.0 0.7)
    (draw ply -0.1 0.4   0.2 0.7))

(seq tri)
(seq quad)
(seq chevron)
(seq saw)
(seq bowtie)

(end)
