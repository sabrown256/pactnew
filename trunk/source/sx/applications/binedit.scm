;  BINEDIT.SCM - a scheme level spoke to allow browsing
;                arbitrary binary files
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
; 

(define current-bfile nil)
(define start 0)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; OPEN-BINARY-FILE

(define-macro (open-binary-file file  . arg)
    "OPEN-BINARY-FILE - Open the named file for raw binary access.
               Standard  defaults to x86-64-std
               Alignment defaults to gnu4-x86-64-algn
               Mode      defaults to r

               Allowable values for standard are:
                   text-std, i386-std, i586l-std, i586o-std,
                   ppc32-std, x86-64-std, x86-64a-std, ppc64-std,
                   m68x-std, vax-std, cray-std

               Allowable values for alignment are:
                   text-algn, byte-algn, word2-algn, word4-algn,
                   word8-algn, gnu4-i686-algn, osx-10-5-algn,
                   sparc-algn, xlc32-ppc64-algn, cygwin-i686-algn,
                   gnu3-ppc64-algn, gnu4-ppc64-algn, xlc64-ppc64-algn,
                   gnu4-x86-64-algn, pgi-x86-64-algn

     Usage: open-binary-file <filename> [<standard> <alignment> <mode>]
       
     Examples: open-binary-file foo x86-64-std gnu4-x86-64-algn r 
               open binary file foo with X86-64 standard and alignment and 
                    read access."
(plot-flag off)

     (let* ((name (print-name file))
	   (std  (if (and arg (> (length arg) 0))
		      (list-ref arg 0)
		      x86-64-std))
	   (align (if (and arg (> (length arg) 1))
		      (list-ref arg 1)
		      gnu4-x86-64-algn))
	   (mode  (if (and arg (> (length arg) 2))
		      (print-name (list-ref arg 2))
		      "r"))
	   (fp    (open-raw-binary-file name mode "RAW" std align)))
      (set! current-bfile fp)
      fp))
              
;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(synonym open-binary-file obf)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SET-ORIGIN

(define-macro (set-origin n)
    (set! start n))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DOUBLE

(define-macro (double n)
    (print-pdb nil (read-binary current-bfile start n  "double")))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; FLOAT

(define-macro (float n)
    (print-pdb nil (read-binary current-bfile start n  "float")))
;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; CHAR

(define-macro (char n)
    (print-pdb nil (read-binary current-bfile start n  "char")))
;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; INT

(define-macro (int n)
    (print-pdb nil (read-binary current-bfile start n  "int")))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SHORT

(define-macro (short n)
    (print-pdb nil (read-binary current-bfile start n  "short")))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; LONG

(define-macro (long n)
    (print-pdb nil (read-binary current-bfile start n  "long")))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------
