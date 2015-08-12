;
; PREC.SCM - test floating point precision controls for PDBDiff
;          -   minimum-positive-float and maximum-positive-float
;          - set in ~/.pdbdiffrc

(define err 0)

(define (gen-ascii fp lbl i n ma dma)
    (if (string? fp)
	(set! fp (fopen fp "w")))
    (if (= i 0)
        (printf fp "# %s\n" lbl))
    (if (< i n)
	(begin (printf fp "%6.1f %25.16e\n" i (* i ma (+ 1.0 dma)))
	       (gen-ascii fp lbl (+ i 1) n ma dma))
	(fclose fp)))

(define (gen-dat n fpmn)
    (printf nil "      generate ascii ... ")
    (gen-ascii "fa.dat" (sprintf "y at %.2e" fpmn) 0 n fpmn 1.6e-16)
    (gen-ascii "fb.dat" (sprintf "y at %.2e" fpmn) 0 n fpmn 8.0e-16)
    (printf nil "done\n")

    (printf nil "      generate fa.u .... ")
    (printf nil "done (%s)\n"
            (system "ultra -s fa.dat \(end\)"))
    (rename "curves.a" "fa.u")

    (printf nil "      generate fb.u .... ")
    (printf nil "done (%s)\n"
            (system "ultra -s fb.dat \(end\)"))
    (rename "curves.a" "fb.u"))

(define (cmp n fpmn)
    (printf nil "\n----------------------------------------------------------\n\n")
    (printf nil "   compare .......... %g\n" fpmn)
    (gen-dat n fpmn)
    (set! err (+ err (system "pdbdiff fa.u fb.u"))))


(define np 4)

(cmp np 1.0e-300)
(cmp np 1.0e-285)

(cmp np 1.0e-100)
(cmp np 1.0e-85)

(cmp np 1.0e-30)

(quit err)
