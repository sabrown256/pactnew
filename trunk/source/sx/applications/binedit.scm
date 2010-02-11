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
               Standard  defaults to 7 (Cray)
               Alignment defaults to 6 (Cray)
               Mode      defaults to r

               Allowable values for standard are:  1 IEEEA_STD
                                                   2 IEEEB_STD
                                                   3 IEEEC_STD
                                                   4 INTELA_STD
                                                   5 INTELB_STD
                                                   6 VAX_STD
                                                   7 CRAY_STD
                                                   8 IEEED_STD
                                                   9 IEEEE_STD

               Allowable values for alignment are: 1 M68000_ALIGNMENT
                                                   2 SPARC_ALIGNMENT
                                                   3 MIPS_ALIGNMENT
                                                   4 INTELA_ALIGNMENT
                                                   5 DEF_ALIGNMENT
                                                   6 UNICOS_ALIGNMENT
                                                   7 UNICOS_ALIGNMENT
                                                   8 RS6000_ALIGNMENT
                                                   9 MIPS64_ALIGNMENT
                                                  10 ALPHA64_ALIGNMENT
                                                  11 INTELB_ALIGNMENT
                                                  12 INTELC_ALIGNMENT
                                                  13 VAX_ALIGNMENT

     Usage: open-binary-file <filename> [<standard> <alignment> <mode>]
       
     Examples: open-binary-file foo 7 6 r 
               open binary file foo with Cray standard and alignment and 
                    read access."
(plot-flag off)

     (let* ((name (print-name file))
	   (std  (if (and arg (> (length arg) 0))
		      (list-ref arg 0)
		      7))
	   (align (if (and arg (> (length arg) 1))
		      (list-ref arg 1)
		      6))
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
