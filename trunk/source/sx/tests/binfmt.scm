; 
; BINFMT.SCM - test actual binary format conversions
;
; include "cpyright.h"
;

(define le-std 1)	; X86_64_STD
(define be-std 5)	; PPC64_STD
(define cray-std 9)	; CRAY_STD
(define kind "RAW")

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; VALUE-LIST - return the name variable's value from a raw binary file

(define (value-list fp type n addr)
   (let* ((rval (read-binary fp addr n type)))
           (if rval
	       (let* ((vals (cddr (pdb->list rval))))
		   (cond ((number? vals)
			  vals)
			 ((string? vals)
			  vals)
			 ((char? vals)
			  vals)
			 ((pm-array? vals)
			  (pm-array->list vals))
			 ((vector? vals)
			  (vector->list vals)))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GET-ENDIAN - sniff the first character of the file to decide if the file is
;            - big-endian or little-endian

(define (get-endian name)
  (if (file? name)
      (let* ((fp  (open-raw-binary-file name "r" kind 1 1))
             (jnk (seek-file fp 8 SEEK-SET))
	     (str (value-list fp "char" 1 SEEK-CUR))
	     (int (if (> (string-length str) 0)
		      (char->integer (car (string->list str)))
		      0)))
	(close-raw-binary-file fp)
	(if (>= int 63)
	    "big-endian"
	    "little-endian"))))
;(trace value-list char->integer string-length)
;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (read-doubles name endian std align)
    (let* ((fp (open-raw-binary-file name "r" kind std align))
	   (a0 (value-list fp "double" 1 SEEK-SET))
	   (a1 (value-list fp "double" 1 SEEK-CUR))
	   (a2 (value-list fp "double" 1 SEEK-CUR))
	   (a3 (value-list fp "double" 1 SEEK-CUR)))

          (printf nil "-> %-16s %-16s %d %d  (%s, %s, %s, %s)\n"
		  name endian std align a0 a1 a2 a3)
	  (close-raw-binary-file fp)
	  a0))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; BINFMT - test reading a raw binary file with doubles
;        - compare results

(define (binfmt name)
  (if (file? name)
      (let* ((endian (get-endian name)))
	    (cond ((string=? endian "little-endian")
                   (read-doubles name endian le-std 1))
		  ((string=? endian "big-endian")
                   (read-doubles name endian be-std 1))
		  (else
		   (printf nil "Unknown endianness for %s - exiting\n" name)
		   (quit))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

;(trace binfmt)
;(trace read-doubles)
;(trace get-endian)

(binfmt "dat.ppc64_std")
(binfmt "dat.x86_64_std")
(end)
