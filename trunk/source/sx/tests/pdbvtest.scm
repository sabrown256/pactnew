;
; PDBVTEST.SCM - PDBView test suite
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
;

(unlink "pdbvtest.pdb" "pdbvtest.asc" "pdbvtest.err"
	"pdbvtest.log" "pdbvtest.pdb.chk" "pdbvtest.ps.diff")

(transcript-on "pdbvtest.log")

(define look "original")
(define debug 0)
;(define debug 1)
(define page 0)
(define xOff 0)
(define npts 18)
(ps-name "pdbvtest")
(ps-flag on)

(define (exception test . rest)
   (if test
       (apply (lambda x (printf nil "ERROR: ")
		        (apply printf (cons nil x))
			(printf nil "\n")) rest)))

(define-macro (begin-test msg . body)
    (memory-trace)

    (set! page (+ page 1))
    (set! xOff (+ xOff 0.05))
    (if (= debug 1)
	(printf nil "Test #%d - %s\n" page msg))

    (plot-flag on)
    (if body
	(for-each eval body))

    (if (= debug 1)
	(printf nil "       %s\n" (list-ref (memory-usage) 2)))

    (memory-trace))

(if (= debug 1)
    (begin
	(cw "pdbvtest" "COLOR" "MPEG" 0 0 256 256)
        (mpeg-flag off)          ; prevents MPEG being used with HC
	(data-id on)
	(plot-title on)
	(plot-labels on))
    (begin
	(cw "pdbvtest" "MONOCHROME" "MPEG" 0 0 256 256)
        (mpeg-flag off)          ; prevents MPEG being used with HC
	(data-id off)
        (plot-date off)
	(plot-title off)
	(plot-labels off)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TEST_1 - math operations not making new mapping (page 1)

(begin-test "math operations not making new mapping"
	    (acos (span 0.55 1))
	    (asin 1)
	    (atan 1)

	    (acosx (span 0.55 1))
	    (asinx 2)
	    (atanx 2)
	    (expx 2)

;(make-curve* (list 1.0 2.0 -1.0) (list -1.2 1.0 2.0))
	    (span -1 1)
	    (abs 3)
	    (absx 3)

	    (axis-line-style dotted)
	    (axis-line-width 0.5)
	    (axis-max-major-ticks 3)
	    (axis-number-minor-ticks 2)
	    (axis-tick-size 0.03) 
	    (axis-tick-type straddle-axis)
	    (axis-x-format "%10.6g")
	    (axis-y-format "%10.5g")

	    (grid on)
	    (axis-grid-style dashed)

	    (cos (span 0 %pi))
	    (cosh 3)
	    (divx 3.14159 3)
	    (cosx (span 0 %pi))
	    (coshx 3)
	    (divy 3.14159 3)

	    (annot (sprintf "%d" page) white
		   xOff (+ xOff 0.05) 0.95 0.98)

	    (wu)
	    (hc)
	    (dl))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TEST_2 - math operations not making new mapping, grids (page 2)


(begin-test "math operations not making new mapping, grids"
	    (axis on)
	    (axis-line-style dashed)
	    (axis-grid-style solid)
	    (axis-x-format "%10.2g")
	    (axis-y-format "%10.2g")

	    (j0 (span 1 10))
	    (j1 1)
	    (jn 10 1)
	    (my 1.0e19 1)
	    (dy 10.0 1)
	    (ln 1)
	    (log10 1)
	    (powa 2.5 1)
	    (powr 2.5 1)

	    (j0x (span 1 10))
	    (j1x 2)
	    (jnx 10 2)
	    (mx 1.0e19 2)
	    (dx 10.0 2)
	    (lnx 2)
	    (log10x 2)
	    (powax 2.5 2)
	    (powrx 2.5 2)

	    (recip (span 0.01 1))
	    (sin 3)
	    (sinh 3)
	    (recipx (span 0.01 1))
	    (sinx 4)
	    (sinhx 4)
	    (mx 5.0 3 4)
	    (my 5.0 3 4)
	    (dx 5.0 3 4)
	    (dy 5.0 3 4)

	    (annot (sprintf "%d" page) white
		   xOff (+ xOff 0.05) 0.95 0.98)

	    (wu)
	    (hc)
	    (dl))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TEST_3 - basic binary function tests

(begin-test "binary functions"

	    (exp (span 0 1 18))
	    (dx 0.5 1)
	    (exp (span 0 1.1 23))
	    (dx 0.4 2)

	    (m+ 1 2)
	    (m- 1 2)
	    (m* 1 2)
	    (m/ 1 2)
	    (m* 3 4)
	    (m+ 5 6)
	    (m* 7 8)

	    (dl))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TEST_4 - math operations making new mapping (page 4)

(begin-test "math operations making new mapping"

	    (sin (span 0 5))
	    (cos (span 1 6))

;(append-curves 1 2)
;(dy 0.2 3)
;(simple-append on)
;(append-curves 1 2)
;(average 1 2)
;(cfft 1 2)
	    (hide 1 2)

	    (annot (sprintf "%d" page) white
		   xOff (+ xOff 0.05) 0.95 0.98)

	    (wu)
	    (hc)
	    (dl))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TEST_5 - mapping inquire commands

(begin-test "mapping inquire commands"

	    (j0 (plane 0 (1 0 5 15) (1 0 4 20)))

	    (exception (> (abs (- (list-ref (get-domain 1) 0) 0.0)) 1.0e-13)
		       "GET-DOMAIN #1")
	    (exception (> (abs (- (list-ref (get-domain 1) 1) 5.0)) 2.0e-13)
		       "GET-DOMAIN #2")
	    (exception (> (abs (- (list-ref (get-range 1) 0) -0.402755)) 1.0e-5)
		       "GET-RANGE #1")
	    (exception (> (abs (- (list-ref (get-range 1) 1)  1.000000)) 2.0e-8)
		       "GET-RANGE #2")
	    (exception (not (equal? (get-label 1) "D2_2->Plane"))
		       "GET-LABEL")
	    (exception (not (= (get-domain-number-points 1) 300))
		       "GET-DOMAIN-NUMBER-POINTS")
	    (exception (not (= (get-range-number-points 1) 300))
		       "GET-RANGE-NUMBER-POINTS")

;	    (disp c 0.0 0.5)
;	    (exception (not (equal? (get-attributes 1) (list -1 0.0 1))) "GET-ATTRIBUTES")
	    
;	    (dx 1.0 1)
;	    (exception (not (= (car (list-ref (getx 1 6) 2)) 7)) "GETX")
;	    (exception (not (= (car (list-ref (gety 1 6) 2)) 5)) "GETY")
;	    (dx -1.0 1)
;	    (stats 3)
	    
	    (dl))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TEST_6 - mapping inquire commands (page 6)

(begin-test "label and axis control"

	    (span 0 1000)
	    (der 1)
	    (axis-n-decades 2)
	    (plot-title on)
	    (plot-labels on)
	    (label-length 20)
	    (label-space 0.2)
	    (label-type-size 8)

	    (label-drawable "test    with    spaces"
			    1)
	    (label-drawable "label with more than 20 characters to test if label-length really worked"
			    2)
	    (squeeze-labels on)

	    (leftspace 0.02)
	    (n-curves)
	    (axis-line-style solid)
	    (axis-grid-style dotdashed)
	    
	    (annot (sprintf "%d" page) white
		   xOff (+ xOff 0.05) 0.95 0.98)

	    (wu)
	    (hc)
	    (dl))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TEST_7 - basic mapping I/O tests (page 7)

(begin-test "basic I/O"

	    (print-flag on)
	    (sin  (span 0 6.28))
	    (cos  (span 0 6.28))
	    (acos (span 0 1.0))
	    (asin (span 0 1.0))
	    (ln   (span 1.0 8.0))
	    (der 1 2 3 4 5)
	    
	    (save-mapping pdbvtest.pdb 1 2 3 4 5 6 8)

	    (apropos file-info)
	    (file pdbvtest.pdb)
	    (help apropos)

            (printf nil "-> struct MP_mapping\n")
	    (struct PM_mapping) 

	    (dl)
	    
	    (cf pdbvtest.pdb)
	    
	    (lines-page 8)
;kill straight lines only from file pdbvtest.pdb (test "menu" command)
;(np kill (menu *line pdbvtest.pdb))
;kill straight lines only from file pdbvtest.asc (test "menu*" command)
;(np kill (menu* "*line" "pdbvtest.asc"))
	    (pl 1 2 3 4 5 6 7)
;(set-id 1 16)
	    (lsv)
	    (dl 26)
;(prefix 2 5)
;(kill (pre 2 3))

;NOTE: the following (annot... ) goes to the screen; the plot produced by the (hc 5)
;      which follows does not. Don't worry that the annotation on the screen changes while
;      the picture stays the same!
	    (annot (sprintf "%d" page) white
		   xOff (+ xOff 0.05) 0.95 0.98)

	    (wu)
	    (hc 5)
	    (dl))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TEST_8 - scales, fonts, and styles (page 8)

(begin-test "scales, fonts, and styles"
	    (grid off)
	    (x-log-scale off)
	    (y-log-scale off)
	    (domv)
	    (ranv)
	    (if (= debug 0)
		(begin
		  (axis off)
		  (plot-title off)
		  (plot-labels off)))
	    
	    (exp (span 0.0 0.8))
	    (copy-map 1)
	    (copy-map 2)
	    (copy-map 3)
	    (lncolor red 1)
	    (dx 0.5 3)
	    (dy 0.5 3)

;(dupx 3)
	    (histogram center 2)
	    (plane 2.0 (5.0 0.0 10.0 18))
	    (border-width 0.3)
	    (botspace 0.1)
	    (rightspace 0.02)
	    (topspace 0.02)

;(make-curve (0 10) (-1 1))
	    (sin (copy-map 4))
	    (sinh (copy-map 5))
	    (copy-map 6)
	    (dl 5)

;(re-id)
;(re-color)
	    (m+ 4 5 6)
	    (lnstyle dotdashed 6)
	    (lnstyle dotted 5)
	    (lnstyle dashed 4)
	    (lnwidth 0.5 2)
	    (type-face "times")
	    (type-size 12)
	    (type-style "bold")
	    (hide 1 3 4 5 6)

	    (annot (sprintf "%d" page) white
		   xOff (+ xOff 0.05) 0.95 0.98)

	    (wu)
	    (hc)
	    (dl))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TEST_9 - window controls (page 9)

(begin-test "window controls"

	    (domv)
	    (ranv)
	    (span 0 10)
	    (sin (span 0 10))

	    (answer-prompt "S-> ")
	    (float-output-format "%25.15Le")

	    (axis on)

	    (background-color-flag black) 
	    (console-height 0.8)
	    (console-origin-x 0.1)
	    (console-origin-y 0.1)
	    (console-width 0.8)
	    (console-type "monochrome")

	    (default-npts 50)
	    (span 10 11)
	    (default-npts 18)

	    (display-name "My Window")
	    (display-title "Extreme PDBView")
	    (display-type "color")

	    (n-curves-read)
	    (plot-type polar)
	    (axis-type polar)
	    (print-flag on)
	    (print-stats on)
	    (prompt "P-> ")

	    (annot (sprintf "%d" page) white
		   xOff (+ xOff 0.05) 0.95 0.98)

	    (wu)
	    (hc)
	    (dl))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TEST_10 - viewport controls

(begin-test "viewport controls"
	    
	    (if (= debug 0)
		(axis off))
	    (plot-type cartesian-plot)
	    (axis-type cartesian)
	    (print-flag off)
	    (print-stats off)
	    
	    (view-height 0.8)
	    (view-origin-x 0.1)
	    (view-origin-y 0.1)
	    (view-width 0.8)
	    
	    (window-origin-x 0.1)
	    (window-origin-y 0.1)
	    (window-height 0.3)
	    (window-width 0.5)

; ERROR: screen windows must NOT be tested in a pure batch mode situation such as PDBVTEST!!!!
;(screen)
	    (dl))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TEST_11 - ascii forms (page 11)

(begin-test "ascii forms"

	    (j0 (span 0.1 5))
	    (jn 1 (span 0.1 5))
	    (m* 1 2)
	    (m* 1 2 3)

; NOTE: Don't run "random" unless we're in debug mode.
	    (if (= debug 1)
		(random (span 0 10 18))
		(sin (span 0 10 18)))

;(rev 4)
;(sort 4)
	    (mx 9.0 4)
	    (dx 5.0 4)
	    
	    (scatter on 2)
	    (marker-scale 0.005)

	    (scatter plus 2)
	    (unlink "table1.tmp")

;(save ascii table1.tmp 1 2)
;(read-table table1.tmp 1)
;(table-curve 10 1 2 0 2)
;(read-table* "table1.tmp" 2)
;(table-curve 10 1 2 0 2)
;(table-attributes)
;(unlink "table1.tmp")
;(xindex 3)
;(xmm 2 1 9)

	    (annot (sprintf "%d" page) white
		   xOff (+ xOff 0.05) 0.95 0.98)
	    
	    (wu)
	    (hc)
; NOTE: Run "random" anyway (make sure it doesn't crash), don't send it
; to file because it won't compare across platforms.
	    (random (span 0 10))
	    (dl))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TEST_12 - environment control commands

(begin-test "environment control commands"

; ERROR: screen windows must NOT be tested in a pure batch mode situation such as PDBVTEST!!!!
;(open-device "window" "color" "temp")
;(plots off)
	    (span 0 10)
	    (sin (span 0 10))

; ERROR: plots must NOT be tested in a pure batch mode situation such as PDBVTEST!!!!
;(plots on)
	    (autoplot off)
	    (cos (span 2 12))
	    (autoplot on)
	    (dl))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TEST_13 - plot control commands (page 13)

(begin-test "plot control commands"

	    (span 0 30)
	    (domv 0.01 20)
	    (ranv 0.01 20)

	    (axis on)
	    (grid on)
	    (x-log-scale on)
	    (y-log-scale on)
	    (axis-line-style dotdashed)
	    (axis-grid-style dotted)

	    (printf nil "%s" page)
	    (annot (sprintf "%d" page) white
		   xOff (+ xOff 0.05) 0.95 0.98)

	    (wu)
	    (hc)
	    (dl))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(printf nil "Tests Concluded\n")
(command-log off)
(end)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(begin-test "extensions"

	    (open-device* "cgm" "color" "my")
	    (cgm-flag on)
	    (default-color red)
	    (span 0 10)
	    (sin (span 0 10))
	    (default-color -1)
	    (mapping? 1)
; Delete all mappings with the word "line" in their label. (Should leave two curves.)
	    (dl (lst* "*line"))
	    (re-id)
	    (show 1 2)

;(curve->list 1)

	    (hc)
	    (prefix* "c")
	    (close-device* "cgm")

	    (ultra-file? "my.cgm")
	    (cgm-name "testt.cgm")
	    (cgm-background-color-flag black)
	    (cgm-type color)
	    (hc)
	    (cgm-flag off)
	    (close-device "cgm")
	    (dl))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; Mapping evaluation test

(begin-test "mapping evaluation test"

	    (plane 0.0 (0.33333 5.0 10.0 10))
	    (exception (> (abs (- (a 7.0) 0.33333)) 5.5e-9) "LINE MAPPING EVALUATION")
	    (dl)
	    (span 0.0 1.0)
	    (exception (> (abs (- (a 0.2) 0.2)) 5.5e-9) "SPAN MAPPING EVALUATION")
	    (dl))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(begin-test "smoothing, filtering"

	    (grid off)
	    (if (= debug 0)
		(axis off))
	    
	    (tan (span 5 10))
	    (smo 3 1)
	    (smooth3 1)
	    (smooth-method "fft")
	    (smooth5 3 1)

;kidnap a filter from func.scm, but re-name it so we'll be sure we got this one.
	    (define george
	      (make-filter  5  1.0     0.0     0.0
			    0.125   0.6875  0.25   -0.0625
			    -0.0625  0.25    0.625   0.25    -0.0625
                            -0.0625  0.25    0.6875   0.125
			    0.0     0.0      1.0))
	    (sin (span 0 10))
	    (sqr 2)
	    (sqrt 2)
	    (tan 2)
	    (tanh 2)
	    
	    (sinx (span 0 10))
	    (sqrx 3)
	    (sqrtx 3)
	    (tanx 3)
	    (tanhx 3)
	    
	    (copy-map 2)
	    (my 2.0 4)
	    (dy 5.0 4)
;(ymin 3.5  4)
;(ymax 6.75 4)
;(xmin 1.5  4)
;(xmax 8.5  4)
;(filter-coef (george 3) 4)

	    (y0 (span 0.01 10))
	    (dy 5.0 5)
	    (y1 5)
	    (dy  5.0 5)
	    (yn  3.0 5)
	    (my 25.0 5)
	    
	    (y0x (span 0.01 10))
	    (dx 5.0 6)
	    (y1x 6)
	    (dx 5.0 6)
	    (ynx 3 6)
	    (mx 25.0 6)
	    
	    (annot (sprintf "%d" page) white
		   xOff (+ xOff 0.05) 0.95 0.98)
	    
	    (wu)
	    (hc)
	    (dl))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(begin-test "various interpeter level functions"

	    (span 0 10)
	    (sin (span 0 10))

;(compose 1 2)
;(convol 1 2)
;(convolb 1 2)
;(mx 0.5 5)
;(copy-curve 15 5)
;(correl 1 2)
;(delta 0 1.1 10)
;(derivative 4)
;(diffraction 0.001 18)
;(hide 1 2 5)

	    (annot (sprintf "%d" page) white
		   xOff (+ xOff 0.05) 0.95 0.98)

	    (wu)
	    (hc)
	    (dl 5 6 7 8 9 10))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(begin-test "more interpeter level functions"

	    (hide-rescale on)
	    (hide 3 4)
	    (printf nil "a\n")
	    
;(extract 10 2)
;(fft 4)
;(mx 7.0 6 7)
;(dx 3.0 6 7)
;(fit 3 2)
;(fitcurve 1 2)
;(fode 2 9 3 4)
;(gaussian 5 1.1 1.2 18)
;(hypot 1 4)
;(+ 5 9)
;(hide 5 9)

	    (annot (sprintf "%d" page) white
		   xOff (+ xOff 0.05) 0.95 0.98)
	    
	    (wu)
	    (hc)
	    (dl (thru 5 15))
	    (dl))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(begin-test "FFTs and some special functions"

	    (ifft 2 3)
	    (mx 6.0 5 6)
	    (dy 5.0 5 6)
	    (integrate b 2 10)
	    (max 1 2 3 4)
	    (min 1 2 3 4)
	    (normalize b)
	    (theta 0 1.1 10 18)
	    (thin b "int" 10)
	    (vs 1 2)
	    (+ 10 12)
	    (hide 10 12)
	    
	    (annot (sprintf "%d" page) white
		   xOff (+ xOff 0.05) 0.95 0.98)
	    
	    (wu)
	    (hc)
	    (dl (thru 5 14))
	    (dl))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(begin-test "system command"

; ---------- NOTE ----------
; if you place any output commands other than "hc1" after
; this point, you must first re-set ps-name and ps-type

	    (printf nil " Test of 'print-menu' command:\n")
	    (print-menu temp2.tmp)
	    (system "cat temp2.tmp")
	    (unlink "pdbvtest1.ps" "pdbvtest2.ps" "temp2.tmp")

; test "interactive" and "ld" -- if "interactive on" is commented out,
; no menu info should be printed
; otherwise, this sequence should produce the same output as
; the "print-menu" above
	    (printf nil "\n Test of 'interactive' command:\n\n")
	    (system "echo '(begin (define (test-menu)    (interactive on)    (menu)) (test-menu))' > temp3.tmp")

; NOTE: for platforms lacking 'system'
            (define fp (fopen "temp3.tmp" "w"))
            (printf fp "(begin (define (test-menu)\n")
            (printf fp "               (interactive on)\n")
            (printf fp "               (menu))\n")
            (printf fp "       (test-menu))\n")
            (fclose fp)

	    (ld temp3.tmp)

; test "ld*"
; creates a file that, when loaded, deletes itself
	    (system "echo '(unlink \"temp3.tmp\" \"temp4.tmp\")' > temp4.tmp")

; NOTE: for platforms lacking 'system'
            (define fp (fopen "temp4.tmp" "w"))
            (printf fp "(unlink \"temp3.tmp\" \"temp4.tmp\")\n")
            (fclose fp)

	    (ld* "temp4.tmp")

	    (dl)

; these mappings should be the same ...
	    (pl 1 6)
	    (exception (> (diff-measure 1 2) 1.0e-10) "FILE MAPPING COMPARISON #1")
	    (dl)

; so should these ...
	    (pl 4 8)
	    (exception (> (diff-measure 1 2) 1.0e-10) "FILE MAPPING COMPARISON #2")
	    (compare pdbvtest.pdb pdbvtest.asc))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------
