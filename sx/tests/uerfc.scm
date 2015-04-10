;
; ERFC.SCM - numerical examination of ERF and ERFC
;          - Display fractional and absolute differences for
;          - both functions
;          - Compare two rational polynomial fits which are
;          - less accurate but faster to compute
;          - with iterative result which is very accurate but slow
;
; Source Version: 4.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
;

(define np 100)
(define xmn -6)
(define xmx  6)

(define (test fnc)
    (interactive off)
    (era)
    (label-space 0.1)

    (igamma-tolerance 1.0e-14)
    (label (fnc (span xmn xmx np)) (sprintf "%s(x) 1.0e-14" fnc))
    (color a green)

    (igamma-tolerance 1.0e-7)
    (label (fnc (span xmn xmx np)) (sprintf "%s(x) 1.0e-7" fnc))

    (igamma-tolerance 1.0e-2)
    (label (fnc (span xmn xmx np)) (sprintf "%s(x) 1.0e-2" fnc))

    (diff-measure a b)
    (del e)
    (label d (sprintf "Frac diff %s medium/high" fnc))
    (label (- a b) (sprintf "Abs diff %s medium/high" fnc))
    (color d e red)
    (lnstyle e dotted)

    (diff-measure a c)
    (del g)
    (label f (sprintf "Frac diff %s low/high" fnc))
    (label (- a c) (sprintf "Abs diff %s low/high" fnc))
    (color f g blue)
    (lnstyle g dotted)

    (del b c)

    (axis-y-format "%.2e")
    (y-log-scale on)
    (grid on)
    (define ymx (cdr (get-range f)))
    (ran (* 1.0e-8 ymx) ymx)

    (replot)
    (hc))

(test erf)
;(pause)

(test erfc)
;(pause)

(end)
