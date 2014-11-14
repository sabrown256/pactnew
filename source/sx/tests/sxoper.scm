; 
; SXOPER.SCM - basic testing of operations on object
;            - binary operator test:
;            -   test +, *, -, /, >, >=, ==, !=, <, <= operators
;            -   on mappings, sets, and arrays
;            -   of types int, long, float, and double
;            -   the dimensionalities are all 1 and that should
;            -   not matter to the testing of the operators
;            - unary operator test:
;            -   using the plot and cps commands
;            -   test the application of unary operator
;            -   specifically log10
;            -   plot the results
; 
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(autoplot off)

(define errt 0)
(define err  0)

(define standard-domain (list 0 10 20 30))
(define standard-range  (list 1  5 20 60))

;--------------------------------------------------------------------------

;                       BINARY OPERATION TEST

;--------------------------------------------------------------------------

(define (lt x y)
    (if (< x y) 1 0))
(define (le x y)
    (if (<= x y) 1 0))
(define (eq x y)
    (if (= x y) 1 0))
(define (ne x y)
    (if (not (= x y)) 1 0))
(define (ge x y)
    (if (>= x y) 1 0))
(define (gt x y)
    (if (> x y) 1 0))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (compare-results ld l1 l2 sop group typx typy name)
    (let* ((la (map sop l1 l2)))

          (if (memv typx '("double" "float"))
	      (define (check-entry x y)
		  (if (= x y)
		      #t
		      (begin (if (= errt 0)
                                 (printf nil "\n"))
			     (set! errt 1)
			     (printf nil
				     "      %s %s %s :\t %s != %s  (float)\n"
				     group typx name x y))))
	      (define (check-entry x y)
		  (if (= x (floor y))
		      #t
		      (begin (if (= errt 0)
                                 (printf nil "\n"))
			     (set! errt 1)
			     (printf nil
				     "      %s %s %s :\t %s != %s  (integer)\n"
				     group typx name x y)))))

          (not (memv #f (map check-entry ld la)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (map-range-elements map)
    (let* ((ran (pm-mapping-range map))
	   (lst (pdb->list (pm-set->pdbdata ran)))
	   (dat (car (list-ref lst 2)))
	   (arr (caar (list-ref dat 6))))
	  (vector->list arr)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (test-array typx typy name a1 a2 mop sop)
    (let* ((arr (mop a1 a2))
	   (ld  (c-array->list arr))
	   (l1  (c-array->list a1))
	   (l2  (c-array->list a2)))
          (compare-results ld l1 l2 sop "Array" typx typy name)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (test-arr-sequence rx1 ry1 typx typy)
    (test-array typx typy "x+y"  rx1 ry1  m+  +)
    (test-array typx typy "x-y"  rx1 ry1  m-  -)
    (test-array typx typy "x*y"  rx1 ry1  m*  *)
    (test-array typx typy "x/y"  rx1 ry1  m/  /)
    (test-array typx typy "x<y"  rx1 ry1  m<  lt)
    (test-array typx typy "x<=y" rx1 ry1  m<= le)
    (test-array typx typy "x==y" rx1 ry1  m== eq)
    (test-array typx typy "x!=y" rx1 ry1  m!= ne)
    (test-array typx typy "x>=y" rx1 ry1  m>= ge)
    (test-array typx typy "x>y"  rx1 ry1  m>  gt))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (test-mapping typx typy name m1 m2 mop sop)
    (let* ((map (mop m1 m2))
	   (ld  (map-range-elements map))
	   (l1  (map-range-elements m1))
	   (l2  (map-range-elements m2)))
          (compare-results ld l1 l2 sop "Mapping" typx typy name)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (test-map-sequence ma mb typx typy)
    (test-mapping typx typy "x+y"  ma mb  m+  +)
    (test-mapping typx typy "x-y"  ma mb  m-  -)
    (test-mapping typx typy "x*y"  ma mb  m*  *)
    (test-mapping typx typy "x/y"  ma mb  m/  /)
    (test-mapping typx typy "x<y"  ma mb  m<  lt)
    (test-mapping typx typy "x<=y" ma mb  m<= le)
    (test-mapping typx typy "x==y" ma mb  m== eq)
    (test-mapping typx typy "x!=y" ma mb  m!= ne)
    (test-mapping typx typy "x>=y" ma mb  m>= ge)
    (test-mapping typx typy "x>y"  ma mb  m>  gt))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (test-type rx ry typx typy)
   (let* ((dom-a (pm-make-set "doma" '(4) (list rx)))
	  (ran-a (pm-make-set "rana" '(4) (list ry)))
	  (ma (pm-make-mapping dom-a dom-a))
	  (mb (pm-make-mapping dom-a ran-a)))

         (set! errt 0)
         (test-arr-sequence rx ry typx typy)
	 (if (= errt 1)
	     (begin (set! err 1)
		    (printf nil "   Failed %s array test\n" typx)))

         (set! errt 0)
	 (test-map-sequence ma mb typx typy)
	 (if (= errt 1)
	     (begin (set! err 1)
		    (printf nil "   Failed %s mapping test\n" typx))
	     #t)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (make-data-array name type dlst)
   (let* ((tspec (list (sprintf "%s *" type)))
	  (nspec (length dlst))
	  (dspec (list dlst))
	  (dpdb  (write-pdbdata nil name
				'(type "C_array")
				(list tspec nspec dspec))))
         (pdbdata->c-array dpdb)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (do-test typx typy)
   (let* ((rx (make-data-array "f1" typx standard-domain))
	  (ry (make-data-array "f2" typy standard-range)))
         (printf nil "   %s and %s ... " typx typy)
         (if (test-type rx ry typx typy)
	     (printf nil "passed\n"))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (test-binary)
   (printf nil "Doing binary operator tests\n")
   (do-test "double" "double")
   (do-test "long"   "long")
   (do-test "float"  "float")
   (do-test "int"    "int")
   (do-test "short"  "short")

   (do-test "double" "float")
   (do-test "long"   "double")
   (do-test "float"  "long")
   (do-test "int"    "float")
   (printf nil "   done\n"))

;--------------------------------------------------------------------------

;                      UNARY OPERATION TEST

;--------------------------------------------------------------------------

(define (double x)
   (m* x x x))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (make-table fa tn xn yn cn)
    (write-pdbdata fa xn (type tn 8)
		   1  2  3  4  5  6  7  8)

    (write-pdbdata fa yn (type tn 10)
		   10  20  30  40  50  60  70  80  90 100)

    (write-pdbdata fa cn (type tn 8 10)
		   1  2  3  4  5  6  7  8
		   2  3  4  5  6  7  8  9
		   3  4  5  6  7  8  9 10
		   4  5  6  7  8  9 10 11
		   5  6  7  8  9 10 11 22
		   6  7  8  9 10 11 22 33
		   7  8  9 10 11 22 33 44
		   8  9 10 11 22 33 44 55
		   9 10 11 22 33 44 55 66
		  10 11 22 33 44 55 66 77))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (plot-family name oper cn xn yn)
    (let* ((dev (cw name "COLOR" "PS" 0 0 1 1)))
          (nxm 4 2)
	  (vr fill-poly)
	  (palette rgb)
	  (plot* cn (cps* xn yn))
	  (cv)
	  (plot* cn (cps* oper xn yn))
	  (cv)
	  (plot* cn (cps* xn oper yn))
	  (cv)
	  (plot* cn (cps* oper xn oper yn))
	  (cv)
	  (vr mesh)
	  (plot* oper cn (cps* xn yn))
	  (cv)
	  (plot* oper cn (cps* oper xn yn))
	  (cv)
	  (plot* oper cn (cps* xn oper yn))
	  (cv)
	  (plot* oper cn (cps* oper xn oper yn))
	  (wu)
	  (clw)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (prep-data)
   (let* ((fa (open-pdbfile "arr.dat" "w")))
         (default-offset fa 1)
	 (major-order fa "column")

	 (make-table fa "short"  "xs" "ys" "fs")
	 (make-table fa "long"   "xl" "yl" "fl")
	 (make-table fa "float"  "xf" "yf" "ff")
	 (make-table fa "double" "xd" "yd" "fd")

	 (close-pdbfile fa)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (test-unary)
   (printf nil "Doing unary operator tests ")

   (prep-data)

   (cf arr.dat)

   (printf nil ".")
   (plot-family "sxopers" double "fs" "xs" "ys")

   (printf nil ".")
   (plot-family "sxoperl" sqr    "fl" "xl" "yl")

   (printf nil ".")
   (plot-family "sxoperf" sqrt   "ff" "xf" "yf")

   (printf nil ".")
   (plot-family "sxoperd" log10  "fd" "xd" "yd")

   (printf nil " done\n"))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(test-binary)
(test-unary)

(end err)
