;
; NATURE.SCM - define constants of nature
;
; Source Version: 4.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
;

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; Fundamental mathematical constants

; correct answer: 3.141592653589793238462643383279502884197169399375
; GNU libc acos:  3.14159265358979311
(define %pi (acos -1.0))

; correnct answer: 2.718281828459045235360287471352662497757247093699959
; GNU libc exp:    2.7182818284590450
(define %e  (exp 1.0))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; Fundamental physical constants

; speed of light (cm/sec)
(define %c 2.99792458e10)

; Newtonian gravitational constant (cm^3/g-sec^2)
(define %Gn 6.673232e-8)

; Planck's constant divided by 2*pi - Hbar in erg-sec
(define %Hb 1.05457267e-27)

; Boltzman's constant in (erg/K)
(define %kB 1.380658e-16)

; Avagadro's number
(define %N0 6.02213665e23)

; electron mass in g
(define %Me 9.109389754e-28)

; fine structure constant e^2/HbC
; NOTE: this is the less fundamental constant
; but the value of alpha has been measured more accurately
(define %alpha 7.297353080e-3)

; fundamental charges per Coulomb
(define %Coulomb 6.241506363e18)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; Useful conversion constants

; eV to erg
(define %eV->erg 1.60217733e-12)

; Kelvin to eV
(define %K->eV (/ %kB %eV->erg))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; Useful constants derived from more fundamental ones

; electron charge in esu
; NOTE: this is the more fundamental constant
; but the value of alpha has been measured more accurately
(define %Qe (sqrt (* %alpha %Hb %c)))

; atomic mass unit in g
(define %Ma (/ 1.0 %N0))

; Hb*C in eV-cm
(define %HbC (/ (* %Hb %c) %eV->erg))

; Rydberg (%Me*c^2*alpha^2)/2 in eV
(define %Ryd (/ (* 0.5 %Me %c %c %alpha %alpha) %eV->erg))

; electron mass in eV - 5.10999065e5 */
(define %Me_eV (/ (* %Me %c %c) %eV->erg))
