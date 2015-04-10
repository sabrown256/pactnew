;
; REVCOORD.SCM
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(define black BLACK)
(define blue  BLUE)
(define red   RED)

(define devn (pg-make-device "SCREEN" "COLOR" "Canonical Coordinates"))
(pg-open-device devn 0.000001 0.000001 0.25 0.25)
(pg-set-viewport! devn 0.2 0.8 0.2 0.8)
(pg-set-world-coordinate-system! devn 0 2 0 2)
(pg-set-grid-flag! devn 1)

(define devx (pg-make-device "SCREEN" "COLOR" "Reverse X"))
(pg-open-device devx 0.25 0.0 0.25 0.25)
(pg-set-viewport! devx 0.2 0.8 0.2 0.8)
(pg-set-world-coordinate-system! devx 2 0 0 2)
(pg-set-grid-flag! devx 1)

(define devy (pg-make-device "SCREEN" "COLOR" "Reverse Y"))
(pg-open-device devy 0.5 0.0 0.25 0.25)
(pg-set-viewport! devy 0.2 0.8 0.2 0.8)
(pg-set-world-coordinate-system! devy 0 2 2 0)
(pg-set-grid-flag! devy 1)

(define devxy (pg-make-device "SCREEN" "COLOR" "Reverse X and Y"))
(pg-open-device devxy 0.75 0.0 0.25 0.25)
(pg-set-viewport! devxy 0.2 0.8 0.2 0.8)
(pg-set-world-coordinate-system! devxy 2 0 2 0)
(pg-set-grid-flag! devxy 1)

(define (test dev)
    (pg-axis dev)
    (pg-set-line-color! dev red)
    (pg-draw-line dev 0.0 0.0 2.0 0.0)
    (pg-draw-line dev 2.0 0.0 2.0 2.0)
    (pg-draw-line dev 2.0 2.0 0.0 0.0)

    (pg-fill-polygon dev blue
		     0.5 0.5
		     1.5 0.5
		     1.5 1.5
		     0.5 1.5
		     0.5 0.5)

    (pg-update-view-surface dev))

(for-each test '(devn devx devy devxy))

