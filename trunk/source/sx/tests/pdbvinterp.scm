;
; PDBVINTERP.SCM
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------

; LINE-POINTS - compute the points on a line of constant y

(define (line-points k x0 y0 dx lst)
    (if (= k 0)
	lst
	(let* ((km (- k 1))
	       (x (+ x0 (* km dx))))
	    (line-points km x0 y0 dx (cons (list x y0) lst)))))

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------

; PLANE-POINTS - compute the points on a cartesian plane

(define (plane-points l kx lx x1 x2 y1 y2 lst)
    (if (= l 0)
	lst
	(let* ((lm (- l 1))
	       (dx (/ (- x2 x1) (- kx 1)))
	       (dy (/ (- y2 y1) (- lx 1)))
	       (y (+ y1 (* lm dy))))
	    (plane-points lm kx lx x1 x2 y1 y2
			  (line-points kx x1 y dx lst)))))

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------

; CARTESIAN-NODES - compute the node list for a cartesian mesh

(define (cartesian-nodes k l x1 x2 y1 y2)
    (plane-points l k l x1 x2 y1 y2 nil))

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------

; CARTESIAN-EDGES - compute the edge list for a cartesian mesh

(define (cartesian-edges k l kx lx lst)
    (if (= k 1)
	(begin (set! k kx)
	       (set! l (- l 1))))
	       
    (if (= l 1)
	(reverse lst)
	(let* ((ke (- k 2))
	       (le (- l 2))
	       (n0 (+ ke (* kx le)))
	       (n1 (+ n0 1))
	       (n2 (+ n0 kx))
	       (n3 (+ n1 kx)))

	    (cartesian-edges (- k 1) l kx lx
			     (append lst
				     (list (list n2 n0)
					   (list n3 n2)
					   (list n1 n3)
					   (list n0 n1)))))))

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------

; CARTESIAN-FACES - compute the face list for a cartesian mesh

(define (cartesian-faces n lst)
    (if (= n 0)
	(reverse lst)
	(let* ((e1 (* 4 (- n 1))))
	    (cartesian-faces (- n 1)
			     (append lst
				     (list (list e1)))))))

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------

; AC-DOMAIN - generate a 2d arbitrarily connected set of zones
;           - Usage: (ac-domain m x1 x2 l y1 y2)

(define (ac-domain k x1 x2 l y1 y2)
    (let* ((n-points (* k l))
	   (n-zones (* (- k 1 ) (- l 1)))
	   (nodes (cartesian-nodes k l x1 x2 y1 y2))
	   (edges (cartesian-edges k l k l nil)))

	(pm-make-ac-set nodes
			edges
			(cartesian-faces (+ n-zones 1) nil))))

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------

; METRIC - compute the distance from x to the origin

(define (metric x)
    (sqrt (apply + (map (lambda (y) (* y y)) x))))

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------

; RANGE-VALUES - apply fnc to the square of the points

(define (range-values fnc nodes vals)
    (if nodes
	(let* ((x   (car nodes))
	       (rst (cdr nodes))
	       (s   (metric x))
	       (v   (if fnc (fnc s) s)))
	      (range-values fnc rst (cons v vals)))
	(reverse vals)))

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------

; AC-RANGE - generate a range set to go with the ac-domain
;          - Usage: (ac-range m x1 x2 l y1 y2)

(define (ac-range fnc k x1 x2 l y1 y2)
    (let* ((km (- k 1))
	   (lm (- l 1))
	   (n-zones (* km lm))
	   (nodes   (cartesian-nodes km lm x1 x2 y1 y2))
	   (vals    (apply list->pm-array (range-values fnc nodes))))
          (pm-make-set "AC" (list n-zones) (list vals))))

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------

; AC-MAPPING - generate an arbitrarily connected 2d mapping
;            - Usage: (ac-mapping fnc k x1 x2 l y1 y2)

(define (ac-mapping fnc k x1 x2 l y1 y2)
    (pm-make-mapping (ac-domain k x1 x2 l y1 y2)
		     (ac-range fnc k x1 x2 l y1 y2)
		     zone))

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------

;cw "3x1" "COLOR" "WINDOW" .4 .0 .6 .3
cw "3x1" "COLOR" "PS" 0 0 1 1
nxm 3 1

vr fill-poly ; palette rainbow
; refmesh on

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------

(define-macro (chk-ac fnc . newres)

   (font helvetica 10 medium)

   (cv "V0")
   (dl)
   (cv "V1")
   (dl)
   (cv "V2")
   (dl)

   (cv "V0")

   (display-mapping* (ac-mapping fnc 18 0 10 18 0 10))

   (copy-map 1)
   (move "V1" "3x1" 2)

   (cv "V1")
   (copy-map 1)
   (move "V2" "3x1" 2)

   (refine 1 newres)
   (dl 2)

   (cv "V2")
   (interpolation-weights 1 newres)
   (dl 2)

   (wu))

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------

(define-macro (chk-lr fnc . newres)

   (font helvetica 10 medium)

   (cv "V0")
   (dl)
   (cv "V1")
   (dl)
   (cv "V2")
   (dl)

   (cv "V0")
   (display-mapping (hyper-plane 0 '(1 0 10 18 1.1) '(1 0 10 18 1.2)))
   (label-drawable "(x,y)" 1)
   (if fnc
       (fnc 1))
   (copy-map 1)
   (move "V1" "3x1" 2)

   (cv "V1")
   (copy-map 1)
   (move "V2" "3x1" 2)

   (refine 1 newres)
   (dl 2)

   (cv "V2")
   (interpolation-weights 1 newres)
   (dl 2)

   (wu))

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------

(chk-lr nil 31 31)
(chk-ac nil 31 31)
(end)
