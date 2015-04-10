; 
; INTP-ND.SCM - test 1D, 2D, 3D, and 4D mesh function interpolation
;             - use refine as simplest look at interpolation process
;             - test inverse distance weighting and multi-quadric
; 
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(define win-name "intp")
;(cw win-name)
(cw win-name "COLOR" "PS" 0.0 0.0 1.0 1.0)
(plot-date off)
(grid on)

(interpolation-scale     1.1)
(interpolation-mq-scale  1.0e-10)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (diff-map a b)
    (let* ((za (map-resolve a))
	   (zb (map-resolve b))
	   (dm (abs (m- za zb)))
	   (av (my 0.5 (m+ za zb)))
	   (fd (label-drawable "Fractional diff" (m/ dm av)))
	   (id (label-drawable "Int(Fractional diff)" (integrate fd)))
	   (ei (pg-range-extrema id))
	   (iv (cadr ei)))
          (dl dm av)
	  iv))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (plane nd xmn xmx nx argl)
    (if (and (> nd 0) (pair? nx))
	(plane (- nd 1) xmn xmx (cdr nx) (cons (list 1 xmn xmx (car nx)) argl))
	(apply hyper-plane (append (list 0) (reverse argl)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (sublist nd lst res)
    (if (and (> nd 0) (pair? lst))
	(sublist (- nd 1) (cdr lst) (cons (car lst) res))
	(reverse res)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (test-nd nd nx nr n)
    (dl)
    (if (< n (length nr))
	(let* ((xmn 0.01)
	       (xmx 1.5)
	       (nrl (list-ref nr n))
	       (nrt (sublist nd nrl nil))
	       (np  (apply * nrt))
	       (zi  (sqr (plane nd xmn xmx nx nil)))
	       (zo  (sqr (plane nd xmn xmx nrl nil))))

              (interpolation-method idw)

	      (label-drawable "plane-i" zi)
	      (label-drawable "plane-o" zo)

	      (define t0 (wall-clock-time))
	      (refine zi nrt)
	      (define ti (- (wall-clock-time) t0))
	      (define fdi (diff-map 1 zo))
	      (dl 2)

	      (interpolation-method mq)

	      (define t0 (wall-clock-time))
	      (refine zi nrt)
	      (define tm (- (wall-clock-time) t0))
	      (define fdm (diff-map 1 zo))
	      (dl 2)

	      (printf nil "%2d %8d %10.3e %10.3e %10.3e    %10.3e %10.3e %10.3e\n"
		      nd np ti (/ ti np) fdi tm (/ tm np) fdm)

	      (test-nd nd nx nr (+ n 1)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TEST-RES - do a series of interpolation performance tests
;          - ND is the number of dimensions of the meshes
;          - NX is the shape of the base mesh
;          - NR is a list of target mesh shapes
;          - in each test a base mesh is constructed and a quadratic
;          - function computed on the mesh
;          - the function is interpolated onto a refined mesh
;          - the time use is measured
;          - the L1 norm of the difference between the
;          - interpolated function and the quadratic computed
;          - on the refined mesh is computed

(define (test-res nd nx nr)
    (printf nil "\n")
    (printf nil "# Input limits: %s\n" nx)
    (printf nil "# Nd   Npts     Tiwd      Tiwd/N     L1iwd          Tmq       Tmq/N      L1mq\n")

    (if (> nd 0)
	(test-nd 1 nx nr 0))
    (if (> nd 1)
	(test-nd 2 nx nr 0))
    (if (> nd 2)
	(test-nd 3 nx nr 0))
    (if (> nd 3)
	(test-nd 4 nx nr 0))

    #t)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(test-res 1
	  '(1001)
	  '((471)
	    (971)
	    (2231)
	    ))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(test-res 2
	  '(25 36)
	  '((11 13)
	    (47 53)
	    (97 103)
	    (223 227)
	    (491 503)
	    ))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(test-res 3
	  '(5 6 3)
	  '((11 13 7)
	    (47 53 43)
	    (97 103 101)
;	    (223 227 211)
;           (491 503 487)
	    ))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------


(test-res 4
	  '(5 6 3 4)
	  '((11 13 7 5)
	    (47 53 43 37)
;	    (97 103 101 89)
	    ))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(end)
