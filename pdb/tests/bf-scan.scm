;
; BF-SCAN.SCM - raw tables to plots
;             - invoked out of pdb/test/bf-scan
;             -   ultra -e -l bf-scan.scm
;             -   gs bf-plot.ps
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(define w 0.5)

(ps-type color)
(ps-name "bf-plot.ps 3 2")

(png-flag on)
(png-type color)

(jpeg-flag on)
(jpeg-type color)

(label-color-flag on)
(data-id off)
(plot-date off)
(axis-line-width w)
(type-style bold)

(define (do-all base)
    (let* ((files (syscmnd (sprintf "ls %s-*.tab" base)))
	   (unm   (sprintf "%s.u" base))
	   (hdf   #f))

          (define (do-table name n tag)
	      (let* ((dsc (read-table* name n))
		     (nr  (list-ref dsc 0))
		     (nc  (list-ref dsc 1))
		     (lbl (string-copy tag))
		     (tc  (table-curve nr 3 nc 0 nc))
		     (fc  (table-curve nr 1 nc 0 nc))
		     (rv #t))
		    (label tc (sprintf "%s time" tag))
		    (label fc (sprintf "%s size" tag))
		    rv))

	  (define (do-curves name)
	      (let* ((dat  (strtok (string-copy name) "\n"))
		     (base (strtok (string-copy name) "."))
		     (cbas (string-copy base))
		     (iv   (begin (strtok cbas "-") (strtok cbas "\n"))))
		    (era)

		    (if (syscmnd (sprintf "grep hdf %s" dat))
			(set! hdf #t))

		    (do-table dat 1 (sprintf "read-ascii %s" iv))
		    (do-table dat 2 (sprintf "read-binary %s" iv))

		    (if hdf
			(begin (do-table dat 3 (sprintf "read-hdf5 %s" iv))
			       (do-table dat 4 (sprintf "read-pdb %s" iv))
			       (do-table dat 5 (sprintf "write-ascii %s" iv))
			       (do-table dat 6 (sprintf "write-binary %s" iv))
			       (do-table dat 7 (sprintf "write-hdf5 %s" iv))
			       (do-table dat 8 (sprintf "write-pdb %s" iv)))
			(begin (do-table dat 3 (sprintf "read-pdb %s" iv))
			       (do-table dat 4 (sprintf "write-ascii %s" iv))
			       (do-table dat 5 (sprintf "write-binary %s" iv))
			       (do-table dat 6 (sprintf "write-pdb %s" iv))))
		    (save* unm (thru a z))))

	  (define (do-plot pat dmn sz)
	      (let* ((tag (sprintf "%s %s" pat dmn)))
		    (era)
		    (clr-annot)

		    (cur (menu* (sprintf "%s-asci*%s" pat dmn)))
		    (color (lst *asci*) blue )

		    (if hdf
			(begin (cur (menu* (sprintf "%s-hdf*%s" pat dmn)))
			       (color (lst *hdf*) yellow)))

		    (cur (menu* (sprintf "%s-pdb*%s" pat dmn)))
		    (color (lst *pdb*) green)

		    (cur (menu* (sprintf "%s-binary*%s" pat dmn)))
		    (color (lst *binary*) red)

		    (lnstyle (thru a z) dotted)
		    (lnstyle (lst "* 1 *") solid)
                    (lnwidth (thru a z) w)

		    (if (string=? dmn "time")
			(begin (lnstyle
				  (lnwidth
				     (color
				        (make-curve* (list sz sz)
						     (list 0.01 10.0))
					black)
				     w)
				  solid)

			       (lnstyle
				  (lnwidth
				     (color
				        (make-curve* (list (* sz 10) (* sz 10))
						     (list 0.01 10.0))
					black)
				     w)
				  dotted)))

		    (log10x (thru a z))
		    (log10 (thru a z))

		    (if (string=? dmn "time")

; GOTCHA: these values are good for the screen
;			(annot "log(T)" black 0.06 0.09 0.60 0.70 90.0)
;			(annot "log(kB)" black 0.06 0.09 0.60 0.70 90.0)

; these values are good for the PS file
			(annot "log(T)" black 0.06 0.09 0.55 0.65 90.0)
			(annot "log(kB)" black 0.06 0.09 0.55 0.65 90.0))

; GOTCHA: these values are good for the screen
;		    (annot "log(Bfsz)" black 0.5  0.6 0.31 0.34)

; these values are good for the PS file
		    (annot "log(Bfsz)" black 0.5  0.6 0.11 0.14)

		    (annot tag      black 0.5  0.6 0.97 0.99)

		    (replot)
		    (hc)
		    #t))

          (system (sprintf "rm -f %s" unm))
          (for-each do-curves files)
	  (kill all)

	  (open unm)
	  (do-plot "read*"  "time" 2.8e7)
	  (do-plot "write*" "time" 2.8e7)
	  (do-plot "write*" "size" 2.8e7)
	  (kill all)

	  #t))

(span 0 1)
(annot "foo" 0.3 0.4 0.5 0.6)
(era)

(do-all "bf")

(end)
