;
; PDBDTU.SCM
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(define npts  20)
(define noise 1.0e-14)

(define (noisy amp lbl)
   (let* ((cva (theta 0.0 0.6 1.0 npts))
	  (cvb (my (random (span 0.0 1.0 npts)) amp))
	  (cvc (+ cva cvb))
	  cvd)
         (del cva cvb)
	 (set! cvd (label (copy cvc) lbl))
	 (del cvc)
	 cvd))

(define (make-data)
  (gaussian 1 0.2 0.5 npts)
  (theta 0.0 0.4 1.0 npts)
  (noisy noise "Theta + small noise")
  (noisy 1.0e-6 "Theta + big noise"))

(unlink "pdbda.dat")
(unlink "pdbdb.dat")

(make-data)
(save "pdbda.dat" (thru a z))

(era)
(make-data)
(save "pdbdb.dat" (thru a z))

(end)

