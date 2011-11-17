#!pdbview -l
;
; PDBVXMPL - PDBView examples of usage
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(plot-date off)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (remarks first . rest)
    (printf nil "\n\n(%s) %s\n" tag first)
    (if rest
	(for-each (lambda (x) (printf nil "       %s\n" x)) rest))
    (pause))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (unlink file)
   (system (sprintf "rm -f %s" file)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MAKE-DATA-1 - make a file with no special data structures

(define (make-data-1)
    (let* ((file (change-file* "demo.d1" "w")))
     
        (make-defstr* file "tv"
		      (def-member integer n)
		      (def-member double x 4 5)
		      (def-member double y 4 5)
		      (def-member double f 4 5)
		      (def-member double u 4 5)
		      (def-member double v 4 5))

        (make-defstr* file "tw"
		      (def-member integer n)
		      (def-member double x 4)
		      (def-member double y 5)
		      (def-member double g 4 5))

; write a data structure
	(write-pdbdata file "arec"
		       (type "tv")
		       (list 10
; x values
			     '(1.0 2.0 3.0 4.0
			       1.0 2.0 3.0 4.0
			       1.0 2.0 3.0 4.0
			       1.0 2.0 3.0 4.0
			       1.0 2.0 3.0 4.0)
; v values
			     '(5.0 5.0 5.0 5.0
			       6.0 6.0 6.0 6.0
			       7.0 7.0 7.0 7.0
			       8.0 8.0 8.0 8.0
			       9.0 9.0 9.0 9.0)
; f values
			     '(1.00 0.80 0.50 0.20
			       0.80 0.70 0.40 0.10
			       0.50 0.40 0.20 0.02
			       0.20 0.10 0.03 0.01
			       0.10 0.02 0.01 0.00)

; u values
			     '(0.0 0.0 0.0 0.0
			       0.1 0.1 0.1 0.1
			       0.2 0.2 0.2 0.2
			       0.3 0.3 0.3 0.3
			       0.4 0.4 0.4 0.4)
; v values
			     '(0.0 0.1 0.2 0.3
			       0.0 0.1 0.2 0.3
			       0.0 0.1 0.2 0.3
			       0.0 0.1 0.2 0.3
			       0.0 0.1 0.2 0.3)))

; write a data structure
	(write-pdbdata file "brec"
		       (type "tw")
		       (list 10
; x values
			     '(1.0 2.0 3.0 4.0)

; y values
			     '(5.0 6.0 7.0 8.0 9.0)

; g values
			     '(1.50 0.10 0.50 0.90
			       0.90 0.70 0.80 0.95
			       0.60 0.40 0.60 0.97
			       0.40 0.30 0.70 0.98
			       0.30 0.2  0.80 1.00)))

	(close* file)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MAKE-DATA-2 - make a file with no special data structures

(define (make-data-2)
    (let* ((file (change-file* "demo.tmp" "w"))
	   (map  (hyper-plane 0.0
			      '(1.0 0.01 2.0 10))))
     
       (pg-def-graph-file file)

       (define (mk-curve n)
	  (if (> n 0)
	      (let* ((name (sprintf "Mapping%d" n))
		     (m    (+ -0.5 (/ n 5)))
		     (labl (sprintf "x ^ %s" m))
		     (smp  (label-drawable labl
					   (powr m (copy-map map)))))
		    (pm-mapping->pdbdata smp file)
		    (mk-curve (- n 1)))))

       (mk-curve 20)
       (close* file)

       (cf    "demo.tmp")
       (cmc   "demo.d2" *)
       (close "demo.d2")
       (close "demo.tmp")

       (dl)
       (unlink "demo.tmp")))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MAKE-DATA-3 - make a file with no special data structures

(define (make-data-3)
    (let* ((file (cf "demo.d3" "w"))
	   (map  (hyper-plane 0.0
			      '(1.0 0.0 2.0 10)
			      '(1.0 0.0 2.0 10)))
	   (smp  (label-drawable "a(x,y)" (sin map)))
	   (tmp  (label-drawable "b(x,y)" (my 1.01 (copy-map smp)))))
     
       (pg-def-graph-file file)

; write two mappings
       (pm-mapping->pdbdata smp file)
       (pm-mapping->pdbdata tmp file)
     
       (close* file)
       (dl)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(if (or (not (file? "demo.d1"))
	(not (file? "demo.d2"))
	(not (file? "demo.d3")))
     (remarks "Create some data files for the demo"))

(autoplot off)

(if (not (file? "demo.d1"))
    (make-data-1))

(if (not (file? "demo.d2"))
    (make-data-2))

(if (not (file? "demo.d3"))
    (make-data-3))

(autoplot on)

(define window-size 0.35)

;--------------------------------------------------------------------------

(remarks "Open a data file"
         "List the contents of the file")

(printf nil "-> cf demo.d1\n")
(cf demo.d1)

(printf nil "-> ls\n")
(ls)

(printf nil "-> ls -a\n")
(ls "-a")

;--------------------------------------------------------------------------

(remarks "Describe arec"
         "Display the structure of the tv type")

(printf nil "-> desc arec\n")
(desc arec)

; Move use of struct to pdbvtest.scm where platform dependent output
; won't be check using diff

;(printf nil "-> struct tv\n")
;(struct tv)

;--------------------------------------------------------------------------

;                          WINDOW #1 - A

;--------------------------------------------------------------------------

(remarks "Create a window to display some data"
	 "Display mapping #1 from the menu"
	 "Change the rendering of the mapping")

(printf nil "-> cw \"A\" \"COLOR\" \"WINDOW\" 0.1 0.01 %s %s\n"
        window-size window-size)
;(cw "A" "COLOR" DISPLAY 0.1 0.01 window-size window-size)
(display "A" 0.1 0.01 window-size window-size)

;--------------------------------------------------------------------------

(remarks "Construct and plot a mapping from data in a file"
         "First A: {(X,Y)} -> {f} a mapping with a 1d range and 2d domain")

(printf nil "-> plot arec.f ''(arec.x arec.y)\n")
(plot arec.f ''(arec.x arec.y))
(label-drawable "f->{x,y}" 1)
(wu)

(printf nil "-> palette \"rainbow\"\n")
(palette "rainbow")

(printf nil "-> vr fill-poly\n")
(vr fill-poly)
(wu)

;--------------------------------------------------------------------------

(remarks "Make 3D data set"
	 "Plot it as a 3D contour plot")

;(cw "A")
(dl)

(printf nil "-> hyper-plane 0 '(1 0 1 20) '(1 0 1 20) '(1 0 1 20)\n")
(hyper-plane 0 '(1 0 1 20) '(1 0 1 20) '(1 0 1 20))

(printf nil "-> vr contour\n")
(vr contour)

(printf nil "-> va 30 -30\n")
(va 30 -30)

(printf nil "-> sqrx 1\n")
(sqrx 1)

(printf nil "-> wu\n")
(wu)

;--------------------------------------------------------------------------

;                          WINDOW #2 - B

;--------------------------------------------------------------------------

(remarks "Create a second window to display other data"
	 "Display mapping #2 from the menu")

(printf nil "-> cw \"B\" \"COLOR\" \"WINDOW\" 0.5 0.01 %s %s\n"
        window-size window-size)
;(cw "B" "COLOR" DISPLAY 0.5 0.01 window-size window-size)
(display "B" 0.5 0.01 window-size window-size)

;--------------------------------------------------------------------------

(remarks "Construct and plot another mapping from data in a file"
	 "Second V: {(X,Y)} -> {u,v} a mapping with a 2d range and 2d domain")

(printf nil "-> plot '(arec.u arec.v) ''(arec.x arec.y)\n")
(plot '(arec.u arec.v) ''(arec.x arec.y))

(label-drawable "{u,v}->{x,y}" 1)
(vr vect)
(wu)

;--------------------------------------------------------------------------

(remarks "Construct and plot a mapping with a cartesian product domain"
	 "Second G: {(X,Y)} -> {real} a mapping with a 2d range and 1d domain")

(printf nil "-> dl\n")
(dl)

(printf nil "-> plot brec.g (cps brec.x brec.y)\n")
(plot brec.g (cps brec.x brec.y))

(printf nil "-> label-drawable \"g->{x,y}\" 1\n")
(label-drawable "g->{x,y}" 1)

(printf nil "-> vr shaded\n")
(vr shaded)

(printf nil "-> palette iron\n")
(palette iron)

(printf nil "-> va 60 -35 0\n")
(va 60 -35 0)

(printf nil "-> wu\n")
(wu)

;--------------------------------------------------------------------------

(remarks "Show the display list of the newly constructed mappings")

(printf nil "-> lsv\n")
(lsv)

;--------------------------------------------------------------------------

;                          WINDOW #3 - C

;--------------------------------------------------------------------------

(remarks "Open an ULTRA file"
	 "Create a third window to display more data"
	 "Plot a set of intensity curves")

(printf nil "-> cf demo.d2\n")
(cf demo.d2)

(printf nil "-> cw \"C\" \"COLOR\" \"WINDOW\" 0.1 0.5 %s %s\n"
        window-size window-size)
;(cw "C" "COLOR" DISPLAY 0.1 0.5 (* 2 window-size) window-size)
(display "C" 0.1 0.5 (* 2 window-size) window-size)
(cv "v1" 0.01 0.01 0.48 0.98)

(printf nil "-> pl 3 4 5 6 7 8 9 10 11 12\n")
(pl 3 4 5 6 7 8 9 10 11 12)

(wu)

;--------------------------------------------------------------------------

(remarks "Find indeces/values in the xval0 array")

(interactive on)

(printf nil "-> xval0\n")
(varprint xval0)

(printf nil "-> find xval0 < 1 and > 0.5\n")
(find xval0 < 1 and > 0.5)

(printf nil "-> xval0 > 0.1 and < 0.75 or > 1.25 and < 1.75\n")
(find xval0 > 0.1 and < 0.75 or > 1.25 and < 1.75)

(printf nil "-> find-mode index-only\n")
(find-mode index-only)

(printf nil "-> xval0 < 0.15 or > 0.75 and < 1.25 or > 1.75\n")
(find xval0 < 0.15 or > 0.75 and < 1.25 or > 1.75)

(printf nil "-> find-mode index+value\n")
(find-mode index+value)

(printf nil "-> xval0 < 0.15 or > 0.75 and < 1.25 or > 1.75\n")
(find xval0 < 0.15 or > 0.75 and < 1.25 or > 1.75)

(interactive off)

;--------------------------------------------------------------------------

(remarks "Create a second viewport in the window"
	 "Construct a mapping f: {t,x} -> {real} from the curves")

(printf nil "-> cv \"v2\" 0.51 0.01 0.48 0.98\n")
(cv "v2" 0.51 0.01 0.48 0.98)

(printf nil "-> pl (lrs '(\"f\"    0 (yval0 yval1 yval2 yval3 yval4 yval5\n")
(printf nil "                       yval6 yval7 yval8 yval9\n")
(printf nil "                       yval10 yval11 yval12 yval13 yval14 yval15\n")
(printf nil "                       yval16 yval17 yval18 yval19))\n")
(printf nil "           '(\"{t,x}\" 1 (xval0 xval0 xval0 xval0 xval0 xval0\n")
(printf nil "                        xval0 xval0 xval0 xval0\n")
(printf nil "                        xval0 xval0 xval0 xval0 xval0 xval0\n")
(printf nil "		             xval0 xval0 xval0 xval0)))\n")

(pl (lrs '("f"    0 (yval0 yval1 yval2 yval3 yval4 yval5
		     yval6 yval7 yval8 yval9
		     yval10 yval11 yval12 yval13 yval14 yval15
		     yval16 yval17 yval18 yval19))
	 '("{t,x}" 1 (xval0 xval0 xval0 xval0 xval0 xval0
		      xval0 xval0 xval0 xval0
		      xval0 xval0 xval0 xval0 xval0 xval0
		      xval0 xval0 xval0 xval0))))

(label-drawable "f->{t,x}" 1)

(printf nil "-> vr wire-frame\n")
(vr wire-frame)

(printf nil "-> va 60.0 25.0 0.0\n")
(va 60 25 0)

;--------------------------------------------------------------------------

(remarks "Animation - rotate the mapping through 360 degrees")

(if (string=? tag "BATCH")
    (define dphi 125.0)
    (define dphi 10.0))
(printf nil "-> animate 60.0 %s 0.0\n" dphi)
(animate 60 dphi 0)

;--------------------------------------------------------------------------

(remarks "Clear the viewport")

(printf nil "-> dl\n")
(dl)

;--------------------------------------------------------------------------

(remarks "Open a PVA file"
	 "Take the difference between two mappings (#1 and #2)"
	 "Take the ratio of the same two mappings"
	 "Plot the difference mapping as a contour plot"
	 "Change viewport"
	 "Plot the product of two mappings (#2 and #3) as a contour plot")

(printf nil "-> cf demo.d3\n")
(cf demo.d3)

(printf nil "-> pl 1 2\n")
(pl 1 2)

(printf nil "-> vr contour\n")
(vr contour)

(printf nil "-> m- 1 2\n")
(m- 1 2)

(printf nil "-> overlay on\n")
(overlay on)

(printf nil "-> dr 1 (render fill-poly)\n")
(dr 1 (render fill-poly))

(printf nil "-> m* 2 3\n")
(m* 2 3)

(let* ((dev (window-device current-window)))
      (viewport-update)
      (pg-finish-plot dev)
      (pg-clear-window dev))

;--------------------------------------------------------------------------

(remarks "Re-open a multi-viewport window")

(printf nil "-> nxm 2 1\n")
(nxm 2 1)

;--------------------------------------------------------------------------

(remarks "Generate a hyper-plane of data")

(printf nil "-> hyper-plane 0 '(1 0 1 20) '(1 0 1 30)\n")
hyper-plane 0 '(1 0 1 20) '(1 0 1 30)

(remarks "Make a copy of the hyperplane")
(printf nil "-> copy-map 1\n")
(copy-map 1)

(remarks "Move the copy to the other viewport")
(printf nil "-> move \"V1\" \"C\" 2\n")
(move "V1" "C" 2)

(remarks "Refine the hyper plane to a different resolution")
(printf nil "-> refine 1 '(25 25)\n")
(refine 1 '(25 25))
(printf nil "-> dl 2\n")
(dl 2)

(remarks "Go to the other frame, compute and plot the interpolation weights")
(printf nil "-> cv \"V1\"\n")
(cv "V1")
(printf nil "-> interpolation-weights 1 '(25 25)\n")
(interpolation-weights 1 '(25 25))
(printf nil "-> dl 2\n")
(dl 2)

(printf nil "-> wu\n")
(wu)

;--------------------------------------------------------------------------

(remarks "Plot curves in each viewport")

(printf nil "-> pl 1\n")
(pl 1)

(printf nil "-> cv ; pl 2\n")
(cv)
(pl 2)

(printf nil "-> wu\n")
(wu)

;--------------------------------------------------------------------------

(remarks "Turn highlights off"
	 "Make a hardcopy to a PostScript file")

(printf nil "-> show-highlight off\n")
(show-highlight off)

(printf nil "-> wu\n")
(wu)

(printf nil "-> ps-flag on\n")
(ps-flag on)

(printf nil "-> hc\n")
(hc)

;--------------------------------------------------------------------------

(close demo.d1)
(close demo.d2)
(close demo.d3)

(remarks "End of demonstration")

(end)
