;
; PDBVMESH.SCM - PDBView test 0, 1, 2, and 3 dimensional mesh manipulation
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
;

(autoplot off)
(plot-date off)

;-----------------------------------------------------------------------

;                         CYLINDER ROUTINES

;-----------------------------------------------------------------------

; ARC-POINTS - compute the points on an arc of constant r and z

(define (arc-points k r0 z0 phi0 dphi lst)
    (if (= k 0)
	lst
	(let* ((km (- k 1))
	       (phi (/ (* (+ phi0 (* km dphi)) %pi) 180))
	       (x (* r0 (cos phi)))
	       (y (* r0 (sin phi))))
	      (arc-points km r0 z0 phi0 dphi (cons (list x y z0) lst)))))

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------

; SURFACE-POINTS - compute the points on an cylindrical surface of constant r

(define (surface-points l kx lx r0 z1 z2 phi1 phi2 lst)
    (if (= l 0)
	lst
	(let* ((lm (- l 1))
	       (dphi (/ (- phi2 phi1) (- kx 1)))
	       (dz (- z2 z1) (- lx 1))
	       (z (+ z1 (* lm dz))))
	      (surface-points lm kx lx r0 z1 z2 phi1 phi2
			      (arc-points kx r0 z phi1 dphi lst)))))

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------

; SHELL-POINTS - compute the points on an cylindrical shell

(define (shell-points m kx lx mx r1 r2 z1 z2 phi1 phi2 lst)
    (if (= m 0)
	lst
	(let* ((mm (- m 1))
	       (dr (- r2 r1) (- mx 1))
	       (r (+ r1 (* mm dr))))
	      (shell-points mm kx lx mx r1 r2 z1 z2 phi1 phi2
			    (surface-points lx kx lx r z1 z2 phi1 phi2 lst)))))

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------

; CYLINDRICAL-NODES - compute the node list for a cylindrical mesh

(define (cylindrical-nodes k l m r1 r2 z1 z2 phi1 phi2)
    (shell-points m k l m r1 r2 z1 z2 phi1 phi2 nil))

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------

; CYLINDRICAL-EDGES - compute the edge list for a cylindrical mesh

(define (cylindrical-edges k l m kx lx mx lst)
    (if (= k 1)
	(begin (set! k kx)
	       (set! l (- l 1))
	       (if (= l 1)
		   (begin (set! l lx)
			  (set! m (- m 1))))))
	       
    (if (= m 1)
	lst
	(let* ((ke (- k 2))
	       (le (- l 2))
	       (me (- m 2))
	       (npl (* kx lx))
	       (n0 (+ ke (* kx (+ le (* lx me)))))
	       (n1 (+ n0 1))
	       (n2 (+ n0 kx))
	       (n3 (+ n1 kx))
	       (n4 (+ n0 npl))
	       (n5 (+ n1 npl))
	       (n6 (+ n2 npl))
	       (n7 (+ n3 npl)))

	      (cylindrical-edges (- k 1) l m kx lx mx
				 (append lst
					 (list (list n0 n1) ; front face
					       (list n1 n3)
					       (list n3 n2)
					       (list n2 n0)

					       (list n0 n4) ; bottom face
					       (list n4 n5)
					       (list n5 n1)
					       (list n1 n0)

					       (list n7 n5) ; back face
					       (list n5 n4)
					       (list n4 n6)
					       (list n6 n7)

					       (list n7 n6) ; top face
					       (list n6 n2)
					       (list n2 n3)
					       (list n3 n7)

					       (list n7 n3) ; right face
					       (list n3 n1)
					       (list n1 n5)
					       (list n5 n7)

					       (list n0 n2) ; left face
					       (list n2 n6)
					       (list n6 n4)
					       (list n4 n0)))))))

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------

; CYLINDRICAL-FACES - compute the face list for a cylindrical mesh

(define (cylindrical-faces n lst)
    (if (= n 0)
	lst
	(let* ((e1 (* 24 (- n 1)))
	       (e2 (+ e1 4))
	       (e3 (+ e2 4))
	       (e4 (+ e3 4))
	       (e5 (+ e4 4))
	       (e6 (+ e5 4)))
	      (cylindrical-faces (- n 1)
				 (append lst
					 (list (list e1 (+ e1 3))
					       (list e2 (+ e2 3))
					       (list e3 (+ e3 3))
					       (list e4 (+ e4 3))
					       (list e5 (+ e5 3))
					       (list e6 (+ e6 3))))))))

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------

; CYLINDRICAL-ZONES - compute a list of zones for the shell
;               - zones have 6 faces from the face list

(define (cylindrical-zones n lst)
    (if (= n 0)
	lst
	(let* ((nm (- n 1))
	       (f (* 6 nm)))
	      (cylindrical-zones nm (cons (list f (+ f 5)) lst)))))

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------

; SHELL - generate a set of 4 sets associated with a cylindrical mesh
;       - one is a 0d set of nodes
;       - next is a 1d set of edges
;       - third is a 2d set of boundary faces
;       - fourth is a 3d set of the full mesh
;       - Usage: (shell m r1 r2 l z1 z2 k phi1 phi2)

(define (shell m r1 r2 l z1 z2 k phi2 phi1)
    (let* ((n-points (* k l m))
	   (n-zones (* (- k 1) (- l 1 ) (- m 1)))
	   (nodes (cylindrical-nodes k l m r1 r2 z1 z2 phi1 phi2))
	   (edges (cylindrical-edges k l m k l m nil)))

          (pm-set->pdbdata (pm-make-ac-set nodes)
			   nil "cylinder0")
	  (pm-set->pdbdata (pm-make-ac-set nodes
					   edges)
			   nil "cylinder1")
	  (pm-set->pdbdata (pm-make-ac-set nodes
					   edges
					   (cylindrical-faces n-zones nil))
			   nil "cylinder2")
	  (pm-set->pdbdata (pm-make-ac-set nodes
					   edges
					   (cylindrical-faces n-zones nil)
					   (cylindrical-zones n-zones nil))
			   nil "cylinder3")))

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------

;(trace arc-points surface-points shell-points)
;(trace cylindrical-nodes)
;(trace cylindrical-edges)
;(trace cylindrical-faces)
;(trace cylindrical-zones)
;(trace pm-make-ac-set)

;-----------------------------------------------------------------------

;                              HELIX ROUTINES

;-----------------------------------------------------------------------

; HELIX-POINTS - compute the points on a helix

(define (helix-points n r0 z0 phi0 dz dphi lst)
    (if (= n 0)
	lst
	(let* ((nm (- n 1))
	       (phi (/ (* (+ phi0 (* nm dphi)) %pi) 180))
	       (x (* r0 (cos phi)))
	       (y (* r0 (sin phi)))
	       (z (+ z0 (* nm dz))))
	      (helix-points nm r0 z0 phi0 dz dphi (cons (list x y z) lst)))))

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------

; HELICAL-NODES - compute the node list for a helical mesh

(define (helical-nodes n ro dr z1 z2 dzo phi0 phi1)
    (let* ((ri (- ro dr))
	   (dzi (/ (* ro dzo) (+ ro dr)))
	   (dphi (/ phi1 (- n 1)))
	   (ddz (* 0.5 (- dzo dzi) (- n 1)))
	   nodes)
	  (set! nodes (helix-points n ri (- z2 ddz) phi0 dzi dphi nil))
	  (set! nodes (helix-points n ri (- z1 ddz) phi0 dzi dphi nodes))
	  (set! nodes (helix-points n ro z2 phi0 dzo dphi nodes))
	  (set! nodes (helix-points n ro z1 phi0 dzo dphi nodes))
	  nodes))

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------

; HELICAL-EDGES - compute the edge list for a helical mesh

(define (helical-edges n npts lst)
    (if (= n 1)
	lst
	(let* ((noff (* 2 npts))
	       (n0 (- n 2))
	       (n1 (+ n0 1))
	       (n2 (+ n0 npts))
	       (n3 (+ n1 npts))
	       (n4 (+ n0 noff))
	       (n5 (+ n1 noff))
	       (n6 (+ n2 noff))
	       (n7 (+ n3 noff)))
	      (helical-edges n1 npts
			     (append lst
				     (list (list n0 n1) ; front face
					   (list n1 n3)
					   (list n3 n2)
					   (list n2 n0)

					   (list n0 n4) ; bottom face
					   (list n4 n5)
					   (list n5 n1)
					   (list n1 n0)

					   (list n7 n5) ; back face
					   (list n5 n4)
					   (list n4 n6)
					   (list n6 n7)

					   (list n7 n6) ; top face
					   (list n6 n2)
					   (list n2 n3)
					   (list n3 n7)

					   (list n7 n3) ; right face
					   (list n3 n1)
					   (list n1 n5)
					   (list n5 n7)

					   (list n0 n4) ; left face
					   (list n4 n5)
					   (list n5 n1)
					   (list n1 n0)))))))

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------

; HELICAL-FACES - compute the face list for a helical mesh

(define (helical-faces n full lst)
    (if (= n 1)
	lst
	(let* ((e1 (* 24 (- n 2)))
	       (e2 (+ e1 4))
	       (e3 (+ e2 4))
	       (e4 (+ e3 4))
	       (e5 (+ e4 4))
	       (e6 (+ e5 4)))
	      (helical-faces (- n 1) full
			     (append lst
				     (if full
					 (list (list e1 (+ e1 3))
					       (list e2 (+ e2 3))
					       (list e3 (+ e3 3))
					       (list e4 (+ e4 3))
					       (list e5 (+ e5 3))
					       (list e6 (+ e6 3)))
					 (list (list e1 (+ e1 3))
					       (list e2 (+ e2 3))
					       (list e3 (+ e3 3))
					       (list e4 (+ e4 3)))))))))

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------

; HELICAL-ZONES - compute a list of zones for the helix
;               - zones have 6 faces from the face list

(define (helical-zones n lst)
    (if (= n 1)
	lst
	(let* ((f (* 6 (- n 2))))
	      (helical-zones (- n 1) (cons (list f (+ f 5)) lst)))))

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------

; HELIX - generate a set of 4 sets associated with a helical mesh
;       - one is a 0d set of nodes
;       - next is a 1d set of edges
;       - third is a 2d set of boundary faces
;       - fourth is a 3d set of the full mesh
;       - Usage: (helix 12 2.0 0.2 1.0)

(define (helix n-points radius thickness dz phi1)
    (let* ((nodes (helical-nodes n-points radius thickness
				 0.0 0.5 dz 0.0 phi1))
	   (edges (helical-edges n-points n-points nil)))

          (pm-set->pdbdata (pm-make-ac-set nodes)
			   nil "helix0")
	  (pm-set->pdbdata (pm-make-ac-set nodes
					   edges)
			   nil "helix1")
	  (pm-set->pdbdata (pm-make-ac-set nodes
					   edges
					   (helical-faces n-points #f nil))
			   nil "helix2")
	  (pm-set->pdbdata (pm-make-ac-set nodes
					   edges
					   (helical-faces n-points #t nil)
					   (helical-zones n-points nil))
			   nil "helix3")))

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------

; (trace helical-nodes helical-edges helical-faces helical-zones pm-make-ac-set)

;-----------------------------------------------------------------------

;                            LINE ROUTINES

;-----------------------------------------------------------------------

; ARC-LINE-POINTS - compute the points on an arc of constant r and z

(define (arc-line-points l lx r z phi1 phi2 lst)
    (if (= l 1)
	lst
	(let* ((lm (- l 1))
	       (dphi (/ (- phi2 phi1) (- lx 1)))
	       (phi (/ (* (+ phi1 (* lm dphi)) %pi) 180))
	       (x (* r (cos phi)))
	       (y (* r (sin phi))))
	      (arc-line-points lm lx r z phi1 phi2 (cons (list x y z) lst)))))

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------

; LINE-POINTS - compute the points on an line of constant r and phi

(define (line-points k kx r z1 z2 phi lst)
    (if (= k 1)
	lst
	(let* ((km (- k 1))
	       (ph (/ (* phi %pi) 180))
	       (dz (/ (- z2 z1) (- kx 1)))
	       (x (* r (cos ph)))
	       (y (* r (sin ph)))
	       (z (+ z1 (* km dz))))
	      (line-points km kx r z1 z2 phi (cons (list x y z) lst)))))

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------

; LINE-NODES - compute the node list for a line mesh

(define (line-nodes k l kx lx r z1 z2 phi1 phi2)
    (let* (nodes)
          (set! nodes (arc-line-points l lx r z1 phi2 phi1 nil))
	  (set! nodes (line-points k kx r z2 z1 phi2 nodes))
	  (set! nodes (arc-line-points l lx r z2 phi1 phi2 nodes))
	  (set! nodes (line-points k kx r z1 z2 phi1 nodes))
	  nodes))

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------

; LINE-EDGES - compute the node list for a line mesh

(define (line-edges n nx lst)
    (if (= n 1)
	(cons (list (- nx 1) 0) lst)
	(let* ((n0 (- n 2))
	       (n1 (- n 1)))
	      (line-edges n1 nx (cons (list n0 n1) lst)))))

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------

; LINE - a constant (r,phi) set in 3 space
;      - Usage: (line k r z1 z2 phi1 phi2)

(define (line k l r z1 z2 phi1 phi2)
    (let* ((n-edges (* 2 (+ (- k 1) (- l 1))))
	   (nodes (line-nodes k l k l r z1 z2 phi1 phi2))
	   (edges (line-edges n-edges n-edges nil)))
          (pm-set->pdbdata (pm-make-ac-set nodes
					   edges)
			   nil "line1")))

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------

;(trace line-points arc-line-points)
;(trace line-nodes)
;(trace line-edges)
;(trace pm-make-ac-set)

(line 20 30 1.15 -1.5 6.7 5 175)
;(pld line1)

;-----------------------------------------------------------------------

;                           PART ROUTINES

;-----------------------------------------------------------------------

; RANDOM-VAL - pick a random value between the two numbers

(define (random-val x1 x2)
   (+ x1 (* 0.5 (+ (random 0) 1.0) (- x2 x1))))

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------

; PART-POINTS - compute the points on a part of constant r and phi

(define (part-points n r1 r2 z1 z2 phi1 phi2 lst)
    (if (= n 0)
	lst
	(let* ((nm (- n 1))
	       (phi (/ (* (random-val phi1 phi2) %pi) 180))
	       (r (random-val r1 r2))
	       (x (* r (cos phi)))
	       (y (* r (sin phi)))
	       (z (random-val z1 z2)))
	      (part-points nm r1 r2 z1 z2 phi1 phi2
			   (cons (list x y z) lst)))))

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------

; PART-NODES - compute the node list for a part mesh

(define (part-nodes n r1 r2 z1 z2 phi1 phi2)
    (part-points n r1 r2 z1 z2 phi1 phi2 nil))

; set of non-random points for test reproducibilty
(define fixed-points
    '((-1.251e-01  -9.452e-02   3.643e+00)
      (-1.895e-01   6.796e-02   1.213e-01)
      (-1.595e-01  -9.990e-02   2.020e+00)
      (-9.990e-02  -1.505e-02   5.179e-01)
      ( 2.702e-01  -9.596e-02   3.569e+00)
      ( 1.825e-01  -1.008e-01   3.796e+00)
      (-7.576e-03   7.838e-03   7.766e-02)
      (-1.450e-01  -2.740e-02  -5.975e-01)
      ( 2.345e-02   1.043e-02   4.787e+00)
      (-4.424e-03  -1.367e-02   1.091e-01)
      ( 6.923e-02  -7.592e-02   9.992e-01)
      ( 4.324e-02  -1.307e-01   3.115e+00)
      ( 2.131e-01  -1.326e-01   1.477e+00)
      ( 6.785e-04  -9.652e-02   5.332e+00)
      ( 2.745e-01  -8.427e-02   6.304e+00)
      ( 1.413e-02  -1.514e-01   2.934e+00)
      ( 2.046e-01   1.039e-01   6.291e+00)
      ( 2.372e-02  -2.632e-01  -4.018e-01)
      (-2.660e-01  -1.001e-02   1.768e+00)
      (-1.351e-02  -1.078e-01   3.975e+00)
      ( 1.540e-01  -4.619e-02   3.405e+00)
      ( 4.744e-02   4.542e-02   5.409e+00)
      ( 3.580e-02   8.186e-02   5.795e+00)
      (-5.656e-02  -1.394e-01   2.102e-01)
      ( 2.050e-01  -1.539e-01   5.760e+00)
      ( 1.709e-01  -1.688e-01  -1.138e+00)
      (-8.550e-02  -3.513e-02   2.626e+00)
      (-4.516e-02  -1.453e-01   4.863e+00)
      (-1.891e-01  -1.938e-02   4.792e+00)
      (-1.718e-01  -1.694e-01  -3.327e-01)
      (-2.455e-02   1.335e-01   4.828e-01)
      ( 1.698e-01   1.629e-01   3.507e+00)
      (-2.419e-01  -1.253e-01   3.363e+00)
      (-2.135e-01  -1.639e-01  -8.508e-01)
      ( 1.216e-01  -3.623e-02   1.794e+00)
      (-2.991e-03  -3.682e-03  -8.918e-01)
      ( 1.271e-01  -1.710e-01   4.639e+00)
      ( 4.686e-02   1.060e-01  -5.130e-01)
      (-9.830e-02  -1.686e-01   2.837e+00)
      (-2.024e-02   1.941e-02   1.158e+00)
      (-4.893e-02   5.799e-02   8.903e-01)
      ( 1.157e-01  -1.673e-01   6.373e+00)
      ( 6.190e-02   2.418e-01  -2.023e-01)
      (-1.394e-01   5.133e-02   6.335e+00)
      ( 2.170e-02   2.280e-01   3.525e+00)
      (-9.294e-02  -9.655e-02   2.392e+00)
      ( 6.689e-02   2.579e-01   2.445e+00)
      ( 1.935e-01  -2.014e-01   3.562e+00)
      ( 1.128e-01  -9.735e-02   1.870e+00)
      (-4.290e-02   1.702e-01   6.176e+00)
      (-9.000e-02   2.112e-01   6.503e+00)
      ( 6.806e-02  -1.472e-01  -1.340e+00)
      ( 9.248e-03   3.809e-02   5.832e+00)
      ( 2.064e-01  -2.694e-02   6.443e+00)
      (-1.821e-02  -1.720e-02   6.918e+00)
      (-2.566e-02  -1.474e-02   8.507e-01)
      (-2.952e-02   2.680e-02   5.571e+00)
      (-1.476e-02   9.050e-02   4.357e+00)
      (-4.012e-02   1.301e-01  -1.447e+00)
      (-2.498e-01  -1.376e-01   5.184e+00)))

;-----------------------------------------------------------------------
;-----------------------------------------------------------------------

; PARTICLES - compute a set of particles randomly distributed in the
;           - specified volume
;           - Usage: (particles k r z1 z2 phi1 phi2)

(define (particles n r1 r2 z1 z2 phi1 phi2)
    (let* ((nodes fixed-points))
;           (nodes (part-nodes n r1 r2 z1 z2 phi1 phi2)))
          (pm-set->pdbdata (pm-make-ac-set nodes)
			   nil "particle0")))

;-----------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (make-meshes)
    (printf nil "Computing cylindrical mesh ... ")
    (shell 3 1 1.2 5 -0.3 1.3 19 0 180)
    (printf nil "done\n")

    (printf nil "Computing helical mesh ... ")
    (helix 50 0.9 0.1 0.1 540)
    (printf nil "done\n")

    (printf nil "Computing curve mesh ... ")
    (line 20 30 1.15 -1.5 6.7 5 175)
    (printf nil "done\n")

    (printf nil "Computing particle mesh ... ")
    (particles 60 0 0.3 -1.5 7.0 0 360)
    (printf nil "done\n"))

(define (write-meshes)
    (cf mesh.dat w)
    (cf nil)
    (cp particle0 line1 helix2 cylinder3 mesh.dat)
    (close mesh.dat))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(unlink "mesh.dat")
(make-meshes)
(write-meshes)
(cf mesh.dat)

;(cw "A" "COLOR" "WINDOW" 0.01 0.01 0.5 0.5)
(cw "pdbvmesh" "COLOR" "PS" 0.0 0.0 1.0 1.0)
(pld particle0 helix2 cylinder3 line1)
;      4         3        2       1

(domv -1.4 1.4 -1.4 1.4 -1.5 7)

; set the particle properties
(marker-scale 0.1)
(lncolor dark-red 4)
(lnwidth 1.0 4)

; set the helix properties
(lnwidth 1.0 3)

; set the mesh properties
(lncolor brown 2)
(lnwidth 0 2)
(lnstyle dotted 2)

; set the curve properties
(lncolor red 1)
(lnwidth 1.0 1)

(restore-viewport 0)

;(dr 1 (render mesh) (va 60 20 0))
;(dr 2 (render mesh) (va 60 20 0))
;(dr 3 (render shaded) (palette yellows) (va 60 20 0))
;(dr 4 (render mesh) (va 60 20 0))

;(wu)

(dr 1 (render mesh))
(dr 2 (render mesh))
(dr 3 (render shaded) (palette yellows))
(dr 4 (render mesh))

;(va 0 0 0)
;(wu)

(va 60 20 0)
(wu)

;(va 60 200 0)
;(wu)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(end)
