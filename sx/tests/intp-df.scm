; 
; INTP-DF.SCM - test 1D, 2D, 3D, and 4D mesh function interpolation
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

; decline to do any test with more that nmax points in the mesh
;(define nmax 3000)
;(define nmax 1400)
(define nmax 800)

(interpolation-scale  1.1)
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

(define (sublist nd nx res)
    (if (> nd 0)
	(sublist (- nd 1) nx (cons nx res))
	res))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (test-nd nd nr n)
    (dl)
    (if (< n (length nr))
	(let* ((xmn 0.01)
	       (xmx 1.5)
               (nxi (list-ref nr n))
	       (nxo (+ nxi 1))
	       (dmi (sublist nd nxi nil))
	       (dmo (sublist nd nxo nil))
	       (ni  (apply * dmi))
	       (no  (apply * dmo))
	       (zi  (sqr (plane nd xmn xmx dmi nil)))
	       (zo  (sqr (plane nd xmn xmx dmo nil))))

	      (if (< ni nmax)
		  (begin (interpolation-method idw)

			 (label-drawable "plane-i" zi)
			 (label-drawable "plane-o" zo)

			 (define t0 (wall-clock-time))
			 (refine zi dmo)
			 (define ti (- (wall-clock-time) t0))
			 (define fdi (diff-map 1 zo))
			 (dl 2)

			 (interpolation-method mq)

			 (define t0 (wall-clock-time))
			 (refine zi dmo)
			 (define tm (- (wall-clock-time) t0))
			 (define fdm (diff-map 1 zo))
			 (dl 2)

			 (printf nil "%2d %8d %10.3e %10.3e %10.3e    %10.3e %10.3e %10.3e\n"
				 nd ni ti (/ ti ni) fdi tm (/ tm ni) fdm)

			 (test-nd nd nr (+ n 1)))))))

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

(define (test-res nd . nr)
    (printf nil "\n")
    (printf nil "# Input mesh sizes: %s\n" nr)
    (printf nil "# Nd   Npts     Tiwd      Tiwd/N     L1iwd          Tmq       Tmq/N      L1mq\n")

    (cond ((= nd 1)
	   (test-nd 1 nr 0))
	  ((= nd 2)
	   (test-nd 2 nr 0))
	  ((= nd 3)
	   (test-nd 3 nr 0))
	  ((= nd 4)
	   (test-nd 4 nr 0)))

    #t)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(test-res 1 4 8 16 32 64 128 256 512 1024 2048 4096)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(test-res 2 4 8 16 24 32 40 48)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(test-res 3 4 6 8 10 12 14)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(test-res 4 3 4 5 6 7 8)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(end)
