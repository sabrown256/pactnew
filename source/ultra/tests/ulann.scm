;
; ULANN.SCM - ULTRA annotation tests
;           - NOTE: in the usual manner of the test suite the annotations will not
;           -       appear since they require a screen window for their computation
;           -       However, this test shows you how to use the annotation
;           -       functions.  To actually see the annotations, run the test
;           -       interactively:  ultra -l ulann.scm
;
; Source Version: 4.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
;

(system "rm -f ulann.log ulann.ps.diff")

(transcript-on "ulann.log")

(define (exception test . rest)
   (if test
       (apply (lambda x (printf nil "ERROR: ")
		        (apply printf (cons nil x))
			(printf nil "\n")) rest)))

(define page 0)
(define xOff 0)
(define debug 0)

(ps-name "ulann")
(ps-flag on)
;Color PS files look really odd when sent to a B/W output device...
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
;--------------------------------------------------------------------------

(sin (span 0 5))

(annot "X Axis" red 0.4 0.6 0.05 0.07)
(annot "Y Axis" red 0.01 0.1 0.55 0.58)

(set! page (+ page 1))
(set! xOff (+ xOff .05))
(annot (sprintf "%s" page) white xOff (+ xOff .05) .95 .98)

(replot)
(hardcopy)

(clr-annot)
(replot)
(era)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(span 0 10)
(sin (span 0 10))

(annot "X Axis" red 0.4 0.6 0.05 0.07)
(annot "Y Axis" red 0.03 0.06 0.4 0.6 90)

(set! page (+ page 1))
(set! xOff (+ xOff .05))
(annot (sprintf "%s" page) black xOff (+ xOff .05) .95 .98)

(replot)
(hardcopy)

(clr-annot)
(replot)
(era)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(printf nil "Tests Concluded\n")
(command-log off)
(end)
