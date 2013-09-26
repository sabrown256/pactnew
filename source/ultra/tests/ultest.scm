;
; ULTEST.SCM - ULTRA test suite
;
; Source Version: 4.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
;

(unlink "ultest.pdb" "ultest.asc" "ultest.err" "ultest.log"
	"ultest.pdb.chk" "ultest.ps.diff")

(transcript-on "ultest.log")

(define (exception test . rest)
   (if test
       (apply (lambda x (printf nil "ERROR: ")
		        (apply printf (cons nil x))
			(printf nil "\n")) rest)))

(define page 0)
(define xOff 0)
(define debug 0)

(ps-name "ultest")
(ps-flag on)
; Color PS files look really odd when sent to a B/W output device...
;(ps-type "color")
(plot-date off)

(if (= debug 1)
    (begin
	(data-id on)
	(plot-labels on)
	(ps-type "color"))
    (begin
	(data-id off)
	(plot-labels off)
	(ps-type "monochrome")))

(default-npts 18)

;--------------------------------------------------------------------------

; Basic Curve Creation Tests

;--------------------------------------------------------------------------

;(printf nil "Curve Creation Tests\n")
(line 0 0.33333 5 10 10)
(exception (> (abs (- (a 7.0) 0.33333)) 5.5e-9) "LINE CURVE EVALUATION")
(era)

(span 0 1)
(exception (> (abs (- (a 0.2) 0.2)) 5.5e-9) "SPAN CURVE EVALUATION")
(era)

;--------------------------------------------------------------------------

; Math operations not making new curve

;--------------------------------------------------------------------------

; Page #1

;(printf nil "Math operations not making new curve - Tests\n")
(acos (span .55 1))
(asin a)
(atan a)

(acosx (span .55 1))
(asinx b)
(atanx b)
(expx b)

(make-curve* (list 1. 2. -1.) (list -1.2 1. 2.))
(abs c)
(absx c)

(axis-line-style dotted)
(axis-line-width .5)
(axis-max-major-ticks 3)
(axis-number-minor-ticks 2)
(axis-tick-size .03) 
(axis-tick-type straddle-axis)
(axis-x-format "%10.6g")
(axis-y-format "%10.5g")

(grid on)
(axis-grid-style dashed)

(cos (span 0 %pi))
(cosh d)
(divx d 3.14159)
(cosx (span 0 %pi))
(coshx e)
(divy e 3.14159)

; The following (replot) saves the first (annot...) from looking different from the rest
(replot)
(set! page (+ page 1))
(set! xOff (+ xOff .05))
(annot (sprintf "%d" page) black xOff (+ xOff .05) .95 .98)

(replot)
(hardcopy)
(era)

;--------------------------------------------------------------------------

; Page #2

(axis on)
(axis-line-style dashed)
(axis-grid-style solid)
(axis-x-format "%10.2g")
(axis-y-format "%10.2g")

(j0 (span 1 10))
(j1 a)
(jn a 10)
(my a 1.e19)
(dy a 10)
(ln a)
(log10 a)
(powa a 2.5)
(powr a 2.5)

(j0x (span 1 10))
(j1x b)
(jnx b 10)
(mx b 1.e19)
(dx b 10)
(lnx b)
(log10x b)
(powax b 2.5)
(powrx b 2.5)

(recip (span 0.01 1))
(sin c)
(sinh c)
(recipx (span 0.01 1))
(sinx d)
(sinhx d)
(mx c d 5)
(my c d 5)
(dx c d 5)
(dy c d 5)

(set! page (+ page 1))
(set! xOff (+ xOff .05))
(annot (sprintf "%d" page) white xOff (+ xOff .05) .95 .98)

(replot)
(hardcopy)
(era)

;--------------------------------------------------------------------------

; Page #3

(grid off)
(if (not (= debug 1))
    (axis off))

(tan (span 5 10))
(smo a 3)
(smooth3 a)
(smooth-method "fft")
(smooth5 a 3)

; kidnap a filter from func.scm, but re-name it so we'll be sure we got this one.
(define george
    (make-filter  5  1.0     0.0     0.0
                     0.125   0.6875  0.25   -0.0625
                    -0.0625  0.25    0.625   0.25    -0.0625
                            -0.0625  0.25    0.6875   0.125
                                     0.0     0.0      1.0))
(sin (span 0 10))
(sqr b)
(sqrt b)
(tan b)
(tanh b)

(sinx (span 0 10))
(sqrx c)
(sqrtx c)
(tanx c)
(tanhx c)

(copy b)
(my d 2)
(dy d 5)
(ymin d 3.5)
(ymax d 6.75)
(xmin d 1.5)
(xmax d 8.5)
(filter-coef d george 3)

(y0 (span .01 10))
(dy e 5)
(y1 e)
(dy e 5)
(yn e 3)
(my e 25)

(y0x (span .01 10))
(dx f 5)
(y1x f)
(dx f 5)
(ynx f 3)
(mx f 25)

(set! page (+ page 1))
(set! xOff (+ xOff .05))
(annot (sprintf "%d" page) black xOff (+ xOff .05) .95 .98)

(replot)
(hardcopy)
(era)

;--------------------------------------------------------------------------

; Math operations making new curve

;--------------------------------------------------------------------------

; Page #4

;(printf nil "Math operations making new curve tests\n")
(sin (span 0 5))
(cos (span 1 6))
(append-curves a b)
(dy c .2)
(simple-append on)
(append-curves a b)
(average a b)
(cfft a b)
(hide a b)

(set! page (+ page 1))
(set! xOff (+ xOff .05))
(annot (sprintf "%d" page) white xOff (+ xOff .05) .95 .98)

(replot)
(hardcopy)
(era)

;--------------------------------------------------------------------------

; Page #5

(span 0 10)
(sin (span 0 10))
(compose a b)
(convol a b)
(convolb a b)
(mx e .5)
(copy-curve e 15)
(correl a b)
(delta 0 1.1 10)
(derivative d)
(diffraction .001 18)
(hide a b e)

(set! page (+ page 1))
(set! xOff (+ xOff .05))
(annot (sprintf "%d" page) black xOff (+ xOff .05) .95 .98)

(replot)
(hardcopy)

;--------------------------------------------------------------------------

; Page #6

(del e f g h i j)
(hide-rescale on)
(hide c d)

(extract b 10)
(fft d)
(mx f g 7)
(dx f g 3)
(fit b 3)
(fitcurve a b)
(fode c d 2 9)
(gaussian 5 1.1 1.2 18)
(hypot a d)
(+ e i)
(hide e i)

(set! page (+ page 1))
(set! xOff (+ xOff .05))
(annot (sprintf "%d" page) white xOff (+ xOff .05) .95 .98)

(replot)
(hardcopy)

;--------------------------------------------------------------------------

; Page #7

(del e f g h i j k l m n)

(ifft b c)
(mx e f 6)
(dy e f 5)
(integrate b 2 10)
(max a b c d)
(min a b c d)
(normalize b)
(theta 0 1.1 10 18)
(thin b "int" 10)
(vs a b)
(+ j l)
(hide j l)

(set! page (+ page 1))
(set! xOff (+ xOff .05))
(annot (sprintf "%d" page) black xOff (+ xOff .05) .95 .98)

(replot)
(hardcopy)

;--------------------------------------------------------------------------

; Page #8

(del (thru e n))

;(printf nil " Curve Inquiry Command - Tests\n")
(disp c 0.0 0.5)
(exception (not (equal? (get-attributes c) (list -1 0. 1))) "GET-ATTRIBUTES")
(exception (> (abs (- (car (get-domain c))  0.)) 1.e-13) "GET-DOMAIN #1")
(exception (> (abs (- (cdr (get-domain c)) 10.)) 2.e-13) "GET-DOMAIN #2")
(exception (not (equal? (get-label c) "compose A B")) "GET-LABEL")
(exception (not (= (get-number-points c) 18)) "GET-NUMBER-POINTS")
(exception (> (abs (- (car (get-range c)) -0.9999788319743369)) 1.e-8) "GET-RANGE #1")
(exception (> (abs (- (cdr (get-range c))  0.9812583778821230)) 2.e-8) "GET-RANGE #2")

(dx a 1)
(exception (not (= (car (list-ref (getx a 6) 2)) 7)) "GETX")
(exception (not (= (car (list-ref (gety a 6) 2)) 5)) "GETY")
(dx a -1)
(stats c)

(ps-flag off)

;(printf nil " Environment Control Commands:\n")

; ERROR: screen windows must NOT be tested in a pure batch mode situation such as ULTEST!!!!
;(open-device "window" "color" "temp")
;(plots off)
(span 0 10)
(sin (span 0 10))

; ERROR: plots must NOT be tested in a pure batch mode situation such as ULTEST!!!!
;(plots on)
(autoplot off)
(cos (span 2 12))
(autoplot on)

; (printf nil " Extensions:\n")
(open-device* "cgm" "color" "my")
(cgm-flag on)
(default-color red)
(span 0 10)
(sin (span 0 10))
(default-color -1)
(curve? a)
; Delete all curves with the word "line" in their label. (Should leave two curves.)
(del (lst* "*line"))
(re-id)
(show a b)
(curve->list a)

(hardcopy)

;--------------------------------------------------------------------------

; Page #9

(prefix* "c")
(close-device* "cgm")

(ultra-file? "my.cgm")
(cgm-name "testt.cgm")
(cgm-background-color-flag black)
(cgm-type color)
(hardcopy)

;--------------------------------------------------------------------------

; Page #10

(cgm-flag off)
(close-device "cgm")

;(printf nil "   synonym system\n")
(synonym kill die)
(printf nil "\n\n")
(system "echo System test")
(printf nil "\n")

(ps-flag on)

;(printf nil " Plot Control Commands:\n")
(domain .01 20)
(range .01 20)
(axis on)
(grid on)
(x-log-scale on)
(y-log-scale on)
(axis-line-style dotdashed)
(axis-grid-style dotted)

(set! page (+ page 1))
(set! xOff (+ xOff .05))
(annot (sprintf "%d" page) white xOff (+ xOff .05) .95 .98)

(replot)
(hardcopy)

;--------------------------------------------------------------------------

; Page #11

(axis-n-decades 2)
(plot-labels on)
(label-length 20)
(label-space .2)
; The following (replot) forces the window resizing from the label-space to occur NOW, so
; the annot below won't get swallowed.
(replot)
(label-type-size 8)
(label a "test    with    spaces")
(label b "label with more than 20 characters to test if label-length really worked")
(squeeze-labels on)
(leftspace .02)
(n-curves)
(axis-line-style solid)
(axis-grid-style dotdashed)

(set! page (+ page 1))
(set! xOff (+ xOff .05))
(annot (sprintf "%d" page) black xOff (+ xOff .05) .95 .98)

(replot)
(hardcopy)

;--------------------------------------------------------------------------

; Page #12

(grid off)
(x-log-scale off)
(y-log-scale off)
(domain de)
(range de)
(if (not (= debug 1))
    (begin
	(axis off)
	(plot-labels off)))

;(printf nil " Curve Control Commands:\n")
; We have two curves at this point
(color a 3)
(copy b)
(dx c 0.5)
(dy c 0.5)
(dupx c)
(histogram b center)
(line 2 5 0 10 18)
(border-width .3)
(botspace .1)
(rightspace .02)
(topspace .02)
(make-curve (0 10) (-1 1))
(copy e)
(del e)
(re-id)
(re-color)
(+ d e f)
(lnstyle f dotdashed)
(lnstyle e dotted)
(lnstyle d dashed)
(lnwidth b .5)
(type-face "times")
(type-size 12)
(type-style "bold")
(hide a c d e f)

(set! page (+ page 1))
(set! xOff (+ xOff .05))
(annot (sprintf "%d" page) white xOff (+ xOff .05) .95 .98)

(replot)
(hardcopy)

;--------------------------------------------------------------------------

; Page #13

(del (thru d g))

; NOTE: Don't run "random" unless we're in debug mode.
(if (= debug 1)
    (random (span 0 10 18))
    (sin (span 0 10 18)))
(rev d)
(sort d)
(mx d 9)
(dx d 5)

(scatter b on)
(marker-scale 0.005)
(marker b plus)
(unlink "table1.tmp")
(save ascii table1.tmp a b)
(read-table table1.tmp 1)
(table-curve 10 1 2 0 2)
(read-table* "table1.tmp" 2)
(table-curve 10 1 2 0 2)
(table-attributes)
(unlink "table1.tmp")
(xindex c)
(xmm b 1 9)

(set! page (+ page 1))
(set! xOff (+ xOff .05))
(annot (sprintf "%d" page) black xOff (+ xOff .05) .95 .98)

(replot)
(hardcopy)
; NOTE: Run "random" anyway (make sure it doesn't crash), don't send it
; to file because it won't compare across platforms.
(random (span 0 10))
(era)

;--------------------------------------------------------------------------

; Page #14

(span 0 10)
(sin (span 0 10))
;(printf nil " Variables:\n")
(answer-prompt "U->")
(ascii-output-format "%25.15e")

(axis on)

(background-color-flag black) 
(console-height .8)
(console-origin-x .1)
(console-origin-y .1)
(console-width .8)
(console-type "monochrome")
(default-npts 50)
(span 10 11)
(default-npts 18)
(display-name "My Window")
(display-title "Extreme Ultra II")
(display-type "color")

(n-curves-read)
(plot-type polar-plot)
(axis-type polar)
(print-flag on)
(print-stats on)
(prompt "u->")

(set! page (+ page 1))
(set! xOff (+ xOff .05))
(annot (sprintf "%d" page) white xOff (+ xOff .05) .95 .98)

(replot)
(hardcopy)

;--------------------------------------------------------------------------

; Page #15

(if (not (= debug 1))
	(axis off))
(plot-type cartesian-plot)
(axis-type cartesian)
(print-flag off)
(print-stats off)

(view-height .8)
(view-origin-x .1)
(view-origin-y .1)
(view-width .8)

(window-origin-x .1)
(window-origin-y .1)
(window-height .3)
(window-width .5)

; ERROR: screen windows must NOT be tested in a pure batch mode situation such as ULTEST!!!!
;(screen)
(era)

;--------------------------------------------------------------------------

; Basic Binary Function Tests

;--------------------------------------------------------------------------

(exp (span 0 1 18))
(dx a 0.5)
(exp (span 0 1.1 23))
(dx b 0.4)

(+ a b)
(- a b)
(* a b)
(/ a b)

(* c d)
(+ e f)
(* g h)

;--------------------------------------------------------------------------

; Basic Curve I/O Tests

;--------------------------------------------------------------------------

(print-flag on)
(save ultest.pdb a b c d e f i)
(save* 'ascii "ultest.asc" a b c d e f i)

;(printf nil " Environment Inquiry Commands - Tests\n")
(apropos file-info)
(display "This is the display function in action. Rah.")
(file-info ultest.pdb)
(file-info* "ultest.pdb")
(help apropos)

(era)

;--------------------------------------------------------------------------

; Kill Tests

;--------------------------------------------------------------------------

; read in file, then kill all its curves, then read it in again
; NOTE: "killp" is UNDOCUMENTED because it doesn't work right
;       it actually deletes by filename rather than by prefix
;       It's only tested here out of a sense of morbid curiousity ...

(rd ultest.pdb)
(rd* "ultest.asc")
;(killp b)
;(rd* "ultest.asc")
; NOTE: "lines-page" has minimum value of 26, so no visible effect unless ncurves > 26.
(lines-page 8)

; kill straight lines only from file ultest.pdb (test "menu" command)
(np kill (menu *line ultest.pdb))

; kill straight lines only from file ultest.asc (test "menu*" command)
(np kill (menu* "*line" "ultest.asc"))

;--------------------------------------------------------------------------

; Annotation Tests

;--------------------------------------------------------------------------

; NOTE: the following (annot... ) goes to the screen; the plot produced by the (hardcopy 5)
;       which follows does not. Don't worry that the annotation on the screen changes while
;       the picture stays the same!

(set! page (+ page 1))
(set! xOff (+ xOff .05))
(annot (sprintf "%d" page) black xOff (+ xOff .05) .95 .98)

; make a set of curves
(cur 1 2 3 4 5 6 7)
(set-id a z)
(lst)
(del z)
(prefix b 5)
(kill (pre b 3))

(hardcopy 5)

;--------------------------------------------------------------------------

; Page #16

(hc1 2 4)

; ---------- NOTE ----------
; If you place any output commands other than "hc1" after
; this point, you must first re-set ps-name and ps-type.

;--------------------------------------------------------------------------

; Print-menu Tests

;--------------------------------------------------------------------------

(printf nil " Test of 'print-menu' command:\n")
(print-menu temp2.tmp)
(system "cat temp2.tmp")
(unlink "ultest1.ps" "ultest2.ps" "temp2.tmp")

;--------------------------------------------------------------------------

; Interactive Tests

;--------------------------------------------------------------------------

; test "interactive" -- if "interactive on" is commented out
; no menu info should be printed
; Otherwise, this sequence should produce the same output as the
; "print-menu" above

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

;--------------------------------------------------------------------------

; LD* Tests

;--------------------------------------------------------------------------

; Test "ld*". Creates a file that, when loaded, deletes itself.

(system "echo '(unlink \"temp3.tmp\" \"temp4.tmp\")'>temp4.tmp")

; NOTE: for platforms lacking 'system'
(define fp (fopen "temp4.tmp" "w"))
(printf fp "(unlink \"temp3.tmp\" \"temp4.tmp\")\n")
(fclose fp)

(ld* "temp4.tmp")

(era)

;--------------------------------------------------------------------------

; Diff-measure Tests

;--------------------------------------------------------------------------

; These curves should be the same...
(cur 1 6)
(exception (> (diff-measure a b) 1.e-10) "FILE CURVE COMPARISON #1")

(era)

; So should these....
(cur 4 8)
(exception (> (diff-measure a b) 1.e-10) "FILE CURVE COMPARISON #2")

(compare ultest.pdb ultest.asc)

(era)

;--------------------------------------------------------------------------

; Set-id/Re-id Tests

;--------------------------------------------------------------------------

(printf nil "Test of 'set-id' and 're-id' commands\n")

(define debug  #f)

(define (show msg expect)
   (let* ((vars (list a b c d))
	  (fncs (map curve? vars)))

         (define (get-results h e r)
	     (if (and h e r)
		 (get-results (cdr h) (cdr e)
			      (and r (eqv? (car h) (car e))))
		 r))

         (if debug
	     (begin (define (show-it x)
		        (printf nil "%s  %s\n" x (curve? x)))

		    (printf nil "%s\n" msg)
		    (for-each show-it vars)
		    (printf nil "\n")))

	 (exception (not (get-results fncs expect #t))
		    "SET-ID/RE-ID")))

(span 0 1)
(dx (copy a) 0.1)
(dx (copy b) 0.1)
(dx (copy c) 0.1)

(show "Created curves A, B, C, and D" '(#t #t #t #t))

(del a c)
(show "Deleted A and C leaving B and D" '(#f #t #f #t))

(re-id)
(show "Re-id to A and B" '(#t #t #f #f))

(set-id b "C")
(show "Set-id B to C" '(#t #f #t #f))

(printf nil "\n")

(era)

;--------------------------------------------------------------------------

(printf nil "Tests Concluded\n")
(command-log off)
(end)
