;
; ULFFT.SCM - Ultra FFT test
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(define (fft-test npt)
    (erase)
    (let* ((crvr (sin (span 0 6.283 npt)))
	   (crvi (line 0 0 0 6.283 npt))
	   (cfs  (cfft crvr crvi))
	   (icfs (apply ifft cfs))
	   (rfs  (fft crvr))
	   (irfs (apply ifft rfs))
	   (rerr (diff-measure (car icfs) (car irfs) #t))
	   (ierr (diff-measure (cadr icfs) (cadr irfs) #t)))
          (smooth (copy crvr))
	  (color crvr crvi white)
	  (for-each (lambda (cx)
			      (lnstyle cx dotted)
			      (color cx blue))
		      cfs)
	  (for-each (lambda (cx)
			      (lnstyle cx dotted)
			      (color cx blue))
		      icfs)
	  (for-each (lambda (cx)
			      (lnstyle cx dashed)
			      (color cx red))
		      rfs)
	  (for-each (lambda (cx)
			      (lnstyle cx dashed)
			      (color cx red))
		      irfs)
	  (if (> rerr 1.0e-3)
	      (printf nil "Sine Test Error: diff(Real Part) = %g\n" rerr))

	  (if (> ierr 1.0e-3)
	      (printf nil "Sine Test Error: diff(Imaginary Part) = %g\n" ierr))))

(define (fft-test-hard npt)
    (erase)
    (let* ((crvr (dy (gaussian 1 2 3 npt) 0.5))
	   (crvi (line 0 0 -3 9 npt))
	   (cfs  (cfft crvr crvi))
	   (icfs (apply ifft cfs))
	   (rfs  (fft crvr))
	   (irfs (apply ifft rfs))
	   (rerr (diff-measure (car icfs) (car irfs) #t))
	   (ierr (diff-measure (cadr icfs) (cadr irfs) #t)))
          (smooth (copy crvr))
	  (color crvr crvi white)
	  (for-each (lambda (cx)
			      (lnstyle cx dotted)
			      (color cx blue))
		      cfs)
	  (for-each (lambda (cx)
			      (lnstyle cx dotted)
			      (color cx blue))
		      icfs)
	  (for-each (lambda (cx)
			      (lnstyle cx dashed)
			      (color cx red))
		      rfs)
	  (for-each (lambda (cx)
			      (lnstyle cx dashed)
			      (color cx red))
		      irfs)
	  (if (> rerr 1.0e-3)
	      (printf nil "Gaussian Test Error: diff(Real Part) = %g\n" rerr))

	  (if (> ierr 1.0e-3)
	      (printf nil "Gaussian Test Error: diff(Imaginary Part) = %g\n" ierr))))

(do ((ipt 8 (+ ipt 1)))
    ((> ipt 16) (printf nil "FFT Tests Complete\n"))
    (printf nil "Number of Points: %d\n" ipt)
    (fft-test ipt)
    (fft-test-hard ipt))

(quit)