;
; ULFNC.SCM - test math functions for correctness
;           - this a crash test only
;           - the output can be inspected for correctness
;           - this may be mined for examples
;
; Source Version: 4.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
;

(unlink "ulfnc.log")

(transcript-on "ulfnc.log")

(define page 0)
(define xOff 0)
(define debug 0)

(ps-name "ulfnc")
(ps-flag on)
(ps-type "color")
(plot-date off)

(default-npts 100)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; Page #1 - log(gamma)

(printf nil "Tests #1   Log(gamma)\n")

(lngamma (span 1.e-4 10))

(label (span 1.e-4 10) "Reference - ln(exp)")
(color b brown)

(set! page (+ page 1))
(set! xOff (+ xOff .05))
(annot (sprintf "%d" page) black xOff (+ xOff .05) .95 .98)

(replot)
(hardcopy)
(ran de)
(era)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; Page #2 - complete elliptic integrals

(printf nil "Tests #2   Complete elliptic integrals\n")

(label (cei1 (span 1.0e-2 10)) "Cei 1")
(label (cei2 (span 1.0e-2 10)) "Cei 2")

(color (thru a b) red)

(label (cei3 (span 1.0e-2 10) 0.5) "Cei 3 n=0.5")
(label (cei3 (span 1.0e-2 10) 1.0) "Cei 3 n=1.0")
(label (cei3 (span 1.0e-2 10) 1.5) "Cei 3 n=1.5")
(label (cei3 (span 1.0e-2 10) 2.0) "Cei 3 n=2.0")

(color (thru c f) blue)

(set! page (+ page 1))
(set! xOff (+ xOff .05))
(annot (sprintf "%d" page) white xOff (+ xOff .05) .95 .98)

(replot)
(hardcopy)
(ran de)
(era)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; Page #3 - Legendre elliptic integrals

(printf nil "Tests #3   Legendre elliptic integrals\n")

(label (lei1 (span 0.0 10) 1.0) "Lei 1 n=1.0")
(label (lei1 (span 0.0 10) 1.5) "Lei 1 n=1.5")
(label (lei1 (span 0.0 10) 2.0) "Lei 1 n=2.0")
(label (lei1 (span 0.0 10) 2.5) "Lei 1 n=2.5")

(color (thru a d) red)

(label (lei2 (span 0.0 10) 1.0) "Lei 2 n=1.0")
(label (lei2 (span 0.0 10) 1.5) "Lei 2 n=1.5")
(label (lei2 (span 0.0 10) 2.0) "Lei 2 n=2.0")
(label (lei2 (span 0.0 10) 2.5) "Lei 2 n=2.5")

(color (thru e h) blue)

(label (sqrt (span 0.0 10)) "Reference - sqrt(x)")
(label (ln   (span 1.0 10)) "Reference - ln(x)")

(color i j brown)

(set! page (+ page 1))
(set! xOff (+ xOff .05))
(annot (sprintf "%d" page) black xOff (+ xOff .05) .95 .98)

(replot)
(hardcopy)
(ran de)
(era)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; Page #4 - Jacobian elliptic functions sn

(printf nil "Tests #4   Jacobian elliptic functions sn\n")

(label (sn (span 0.0 10) 1.0) "sn n=1.0")
(label (sn (span 0.0 10) 1.5) "sn n=1.5")
(label (sn (span 0.0 10) 2.0) "sn n=2.0")
(label (sn (span 0.0 10) 2.5) "sn n=2.5")

(color (thru a d) blue)

(label (sin (span 0.0 10)) "Reference - sin(x)")

(color e brown)

(set! page (+ page 1))
(set! xOff (+ xOff .05))
(annot (sprintf "%d" page) white xOff (+ xOff .05) .95 .98)

(replot)
(hardcopy)
(ran de)
(era)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; Page #5 - Jacobian elliptic functions cn

(printf nil "Tests #5   Jacobian elliptic functions cn\n")

(label (cn (span 0.0 10) 1.0) "cn n=1.0")
(label (cn (span 0.0 10) 1.5) "cn n=1.5")
(label (cn (span 0.0 10) 2.0) "cn n=2.0")
(label (cn (span 0.0 10) 2.5) "cn n=2.5")

(color (thru a d) blue)

(label (cos (span 0.0 10)) "Reference - cos(x)")

(color e brown)

(set! page (+ page 1))
(set! xOff (+ xOff .05))
(annot (sprintf "%d" page) white xOff (+ xOff .05) .95 .98)

(replot)
(hardcopy)
(ran de)
(era)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; Page #6 - Jacobian elliptic functions dn

(printf nil "Tests #6   Jacobian elliptic functions dn\n")

(label (dn (span 0.0 10) 1.0) "cd n=1.0")
(label (dn (span 0.0 10) 1.5) "cd n=1.5")
(label (dn (span 0.0 10) 2.0) "cd n=2.0")
(label (dn (span 0.0 10) 2.5) "cd n=2.5")

(color (thru a d) blue)

(set! page (+ page 1))
(set! xOff (+ xOff .05))
(annot (sprintf "%d" page) white xOff (+ xOff .05) .95 .98)

(replot)
(hardcopy)
(ran de)
(era)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; Page #7 - Tchebyshev polynomials

(printf nil "Tests #7   Tchebyshev polynomials\n")

(label (tchn (span 0.0 1.0) 1.0) "Tch n=1.0")
(label (tchn (span 0.0 1.0) 2.0) "Tch n=2.0")
(label (tchn (span 0.0 1.0) 3.0) "Tch n=3.0")
(label (tchn (span 0.0 1.0) 4.0) "Tch n=4.0")

(color (thru a d) blue)

(set! page (+ page 1))
(set! xOff (+ xOff .05))
(annot (sprintf "%d" page) black xOff (+ xOff .05) .95 .98)

(replot)
(hardcopy)
(ran de)
(era)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; Page #8 - Bessel functions of the first kind

(printf nil "Tests #8   Bessel functions of the 1st kind\n")
(label (j0 (span 0.0 12))    "J0")
(label (j1 (span 0.0 12))    "J1")
(label (jn (span 0.0 12) 2)  "J2")
(label (jn (span 0.0 12) 3)  "J3")

(color (thru a d) blue)

(set! page (+ page 1))
(set! xOff (+ xOff .05))
(annot (sprintf "%d" page) white xOff (+ xOff .05) .95 .98)

(ran -1.0 1.2)

(replot)
(hardcopy)
(ran de)
(era)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; Page #9 - Bessel functions of the second kind

(printf nil "Tests #9   Bessel functions of the 2nd kind\n")

(label (y0 (span 1.0e-4 12))    "Y0")
(label (y1 (span 1.0e-4 12))    "Y1")
(label (yn (span 1.0e-4 12) 2)  "Y2")
(label (yn (span 1.0e-4 12) 3)  "Y3")

(color (thru a d) blue)

(ran -3.2 1.0)

(set! page (+ page 1))
(set! xOff (+ xOff .05))
(annot (sprintf "%d" page) white xOff (+ xOff .05) .95 .98)

(replot)
(hardcopy)
(ran de)
(era)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; Page #10 - modified Bessel functions of the first kind

(printf nil "Tests #10  Modified Bessel functions of the 1st kind\n")

(label (i0 (span 0.0 4.0))    "I0")
(label (i1 (span 0.0 4.0))    "I1")
(label (in (span 0.0 4.0) 2)  "I2")
(label (in (span 0.0 4.0) 3)  "I3")

(color (thru a d) blue)

(ran 0.0 6.0)

(set! page (+ page 1))
(set! xOff (+ xOff .05))
(annot (sprintf "%d" page) black xOff (+ xOff .05) .95 .98)

(replot)
(hardcopy)
(ran de)
(era)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; Page #11 - modified Bessel functions of the second kind

(printf nil "Tests #11  Modified Bessel functions of the 2nd kind\n")

(label (k0 (span 1.0e-4 4.0))    "K0")
(label (k1 (span 1.0e-4 4.0))    "K1")
(label (kn (span 1.0e-4 4.0) 2)  "K2")
(label (kn (span 1.0e-4 4.0) 3)  "K3")

(color (thru a d) blue)

(ran 0.0 6.0)

(set! page (+ page 1))
(set! xOff (+ xOff .05))
(annot (sprintf "%d" page) black xOff (+ xOff .05) .95 .98)

(replot)
(hardcopy)
(ran de)
(era)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; Page #12 - incomplete gamma functions

(printf nil "Tests #12  Incomplete Gamma functions\n")

(label (igamma-p (span 1.0e-8 15) 0.5)  "P a=0.5")
(label (igamma-p (span 1.0e-8 15) 1.0)  "P a=1.0")
(label (igamma-p (span 1.0e-8 15) 3.0)  "P a=3.0")
(label (igamma-p (span 1.0e-8 15) 10.0) "P a=10.0")

(color (thru a d) blue)

(label (igamma-q (span 1.0e-8 15) 0.5)  "Q a=0.5")
(label (igamma-q (span 1.0e-8 15) 1.0)  "Q a=1.0")
(label (igamma-q (span 1.0e-8 15) 3.0)  "Q a=3.0")
(label (igamma-q (span 1.0e-8 15) 10.0) "Q a=10.0")

(color (thru e h) red)

(set! page (+ page 1))
(set! xOff (+ xOff .05))
(annot (sprintf "%d" page) black xOff (+ xOff .05) .95 .98)

(replot)
(hardcopy)
(ran de)
(era)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; Page #13 - beta function

(printf nil "Tests #13  Beta function\n")

(y-log-scale on)
(label (beta (span 1.0e-3 10.0) 0.5)  "beta")
(label (beta (span 1.0e-3 10.0) 1.0)  "beta")
(label (beta (span 1.0e-3 10.0) 2.0)  "beta")
(label (beta (span 1.0e-3 10.0) 4.0)  "beta")

(color (thru a d) blue)

(set! page (+ page 1))
(set! xOff (+ xOff .05))
(annot (sprintf "%d" page) black xOff (+ xOff .05) .95 .98)

(replot)
(hardcopy)
(ran de)
(era)
(y-log-scale off)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; Page #14 - erf, erfc

(printf nil "Tests #14  Erf and erfc\n")

(label (erf  (span 0.0 3.0))  "erf")
(label (erfc (span 0.0 3.0))  "erfc")

(color (thru a b) blue)

(set! page (+ page 1))
(set! xOff (+ xOff .05))
(annot (sprintf "%d" page) black xOff (+ xOff .05) .95 .98)

(replot)
(hardcopy)
(ran de)
(era)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; Page #15 - convolb

(printf nil "Tests #15  Convolb\n")

(label (sin (span 0.0 6.28 100)) "sin(x)")
(delta -10 0 10 100)
(convolb a b 100)

(color (thru a c) blue)

(set! page (+ page 1))
(set! xOff (+ xOff .05))
(annot (sprintf "%d" page) black xOff (+ xOff .05) .95 .98)

(replot)
(hardcopy)
(ran de)
(era)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(printf nil "Tests Concluded\n")
(command-log off)
(end)
