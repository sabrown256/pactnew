;
; SLICE.SCM - SCHEME level functions for ULTRA II
;
; #include <cpyright.h>
;

;(printf nil "Loading SLICE.SCM ... ")

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SLICE->CURVE - helper for plot-slice and vs-slice

(define (slice->curve xt curves xvals yvals)
    (if (pair? curves)
	(let* ((crv (car curves))
	       (yv  (cond ((number? crv)
			   (let* ((ncv (select crv))
				  (val ((eval ncv) xt)))
			         (del ncv)
				 val))
			  ((curve? crv)
			   (crv xt)))))
	      (slice->curve xt
			    (cdr curves)
			    (cons (length curves) xvals)
			    (cons yv yvals)))
	(list xvals (reverse yvals))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PLOT-SLICE

(define (plot-slice xt . rest)
    "Procedure: Generate a curve by taking the y values of a set of
     curves evaluated at a given time versus the ordinal index.
     Usage: plot-slice <time> <curve-list>"
    (interactive off)
    (let* ((crvs (if (pair? (car rest)) (map eval (car rest)) rest)))
          (label (apply make-curve* (slice->curve xt crvs nil nil))
		 (sprintf "Slice %s at x = %s" rest xt))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; VS-SLICE

(define (vs-slice xt rclst dclst)
    "Procedure: Generate a curve by taking the y values of a set of
     curves evaluated at a given time versus the y values of
     a different set of curves evaluated at the same time
     Usage: vs-slice <tiem> <range-cuve-list> <domain-curve-list>"
    (interactive off)
    (let* ((dc (apply make-curve* (clise->curve xt dclst nil nil)))
	   (rc (apply make-curve* (clise->curve xt rclst nil nil)))
	   (vc (vs rc dc)))
          (del dc rc)
	  (set! dc (copy (label vc (sprintf "Slice vs at x = %s" xt))))
	  (del vc)
	  dc))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

;(printf nil "done\n")
