;
; PDPTR.SCM - test PDB eager pointer machinery
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
;

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define-macro (var? x)
    (printf nil "%s %s a file variable\n"
	    x
	    (if (file-variable? current-file x) "is" "is not")))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (var-check)
    (let* ((kv ())
	   (uv ())
	   (names '(/is /ia /ip
		    /ints/ia /ints/ip /ints/iap /ints/ipp
		    /ints/d/is /ints/d/ia /ints/d/ip
		    /floats/fa /floats/fp /floats/fap /floats/fpp
		    /floats/d/fs /floats/d/fa /floats/d/fp)))

          (define-macro (known-var x)
	      (if (file-variable? current-file x)
		  (set! kv (cons x kv))
		  (set! uv (cons x uv))))

          (define-macro (list-var x)
	      (printf nil "   %s\n" x))

	  (for-each known-var names)

          (if (pair? kv)
	      (begin (printf nil "Known variables:\n")
		     (for-each list-var kv)))
          (if (pair? uv)
	      (begin (printf nil "Unknown variables:\n")
		     (for-each list-var uv)))

	  #t))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (check-file file mode)
    (let* (cs)

;          (set! cs (memory-monitor -1 2 "Mode"))

          (change-file* file mode)

; these should not work with delayed symbols

	  (printf nil "---------------------------------------\n")
	  (printf nil "---------------------------------------\n")
	  (printf nil "   / directory, delay %s mode\n" mode)
	  (printf nil "\n")

	  (ls -Raf /)
	  (printf nil "\n")

	  (var-check)
	  (printf nil "\n")

; these should work in all cases

	  (printf nil "---------------------------------------\n")
	  (printf nil "   /ints directory, delay %s mode\n" mode)
	  (printf nil "\n")

	  (cd /ints)
	  (ls -Raf /)
	  (printf nil "\n")

	  (var-check)
	  (printf nil "\n")

	  (varprint ia)
	  (varprint ip)
	  (varprint iap)
	  (varprint ipp)
	  (printf nil "\n")

; these should work in all cases

	  (printf nil "---------------------------------------\n")
	  (printf nil "   /floats directory, delay %s mode\n" mode)
	  (printf nil "\n")

	  (cd /floats)
	  (ls -Raf /)
	  (printf nil "\n")

	  (var-check)
	  (printf nil "\n")

	  (varprint fa)
	  (varprint fp)
	  (varprint fap)
	  (varprint fpp)
	  (printf nil "\n")

	  (cd /)
	  (close* file)

;	  (memory-monitor cs 2 "Mode")

	  #t))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; create data file

cf pdptr.pdb "w"

integer is 1
integer ia(2) 10 11
integer *ip (1 2 3 4)

mkdir /ints
cd /ints
integer ia(2 5) 1 2 3 4 5 6 7 8 9 10
integer *ip (1 2 3 4)
integer *iap(3) (-1 -2 -3) (40 50) (6 7 8 9)
integer **ipp ((1 2 3 4 5) (6 7 8 9))

mkdir d
cd d
integer is 1
integer ia(2) 20 21
integer *ip (30 31)

cd /

mkdir /floats
cd /floats
float fa(5 2) 1 2 3 4 5 6 7 8 9 10
float *fp (1 2 3 4)
float *fap(3) (-1 -2 -3) (40 50) (6 7 8 9)
float **fpp ((1 2 3 4 5) (6 7 8 9))

mkdir d
cd d
integer fs 1
integer fa(2) 20 21
integer *fp (30 31)

cd /

close pdptr.pdb

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(check-file "pdptr.pdb" "rsa")
(check-file "pdptr.pdb" "rsc")
(check-file "pdptr.pdb" "rsd")
(check-file "pdptr.pdb" "rp")

(end)

