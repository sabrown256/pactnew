;
; SHTYP.SCM - test complex and quaternion types
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(printf nil "Cartesian complex test\n")
(define z (make-rectangular 3 4))
(printf nil "z = %s\n" z)
(printf nil "real(z)      = %g\n" (real-part z))
(printf nil "imag(z)      = %g\n" (imag-part z))
(printf nil "magnitude(z) = %g\n" (magnitude z))
(printf nil "angle(z)     = %g\n" (angle z))
(printf nil "exp(z)       = %s\n" (exp z))
(printf nil "ln(z)        = %s\n" (ln z))
(printf nil "\n")

(printf nil "Polar complex test\n")
(define z (make-polar 5 0.707))
(printf nil "z = %s\n" z)
(printf nil "real(z)      = %g\n" (real-part z))
(printf nil "imag(z)      = %g\n" (imag-part z))
(printf nil "magnitude(z) = %g\n" (magnitude z))
(printf nil "angle(z)     = %g\n" (angle z))
(printf nil "exp(z)       = %s\n" (exp z))
(printf nil "ln(z)        = %s\n" (ln z))
(printf nil "\n")

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(printf nil "Cartesian quaternion test\n")
(define q (make-rectangularq 1 2 3 4))
(printf nil "q = %s\n" q)
(printf nil "scalar part(q) = %g\n" (real-partq q))
(printf nil "i part(q)      = %g\n" (imag-partq q))
(printf nil "j part(q)      = %g\n" (jmag-partq q))
(printf nil "k part(q)      = %g\n" (kmag-partq q))
(printf nil "magnitude(q)   = %g\n" (magnitudeq q))
(printf nil "angle(q)       = %g\n" (angleq q))
(printf nil "colatitude(q)  = %g\n" (colatitudeq q))
(printf nil "longitude(q)   = %g\n" (longitudeq q))
(printf nil "exp(q)         = %s\n" (exp q))
(printf nil "ln(q)          = %s\n" (ln q))
(printf nil "\n")

(printf nil "Polar quaternion test\n")
(define q (make-polarq 1 0.1 0.2 0.3))
(printf nil "scalar part(q) = %g\n" (real-partq q))
(printf nil "i part(q)      = %g\n" (imag-partq q))
(printf nil "j part(q)      = %g\n" (jmag-partq q))
(printf nil "k part(q)      = %g\n" (kmag-partq q))
(printf nil "magnitude(q)   = %g\n" (magnitudeq q))
(printf nil "angle(q)       = %g\n" (angleq q))
(printf nil "colatitude(q)  = %g\n" (colatitudeq q))
(printf nil "longitude(q)   = %g\n" (longitudeq q))
(printf nil "exp(q)         = %s\n" (exp q))
(printf nil "ln(q)          = %s\n" (ln q))
(printf nil "\n")

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(quit)
