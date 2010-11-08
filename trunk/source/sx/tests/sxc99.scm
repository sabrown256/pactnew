; 
; SXPDBP.SCM - primitive data type PDB test
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(ld sxpdbg.scm)

(set-format "short"                "%4d ")
(set-format "integer"              "%6d ")
(set-format "long"                 "%8ld ")
(set-format "long_long"            "%8lld ")
(set-format "float"                "%11.3e ")
(set-format "double"               "%11.3e ")
(set-format "long_double"          "%11.3Le ")
(set-format "float_complex"        "%11.3e + %11.3e*I ")
(set-format "double_complex"       "%11.3e + %11.3e*I ")
(set-format "long_double_complex"  "%11.3Le + %11.3Le*I ")

(define vars (list "cs" "ca" "cm" "is" "ia" "im" "ls" "la" "lm" "gs" "ga" "gm"
		   "fs" "fa" "fm" "ds" "da" "dm" "qs" "qa" "qm"
		   "fcs" "fca" "fcm" "dcs" "dca" "dcm" "qcs" "qca" "qcm"))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PDB-DATA-GEN - generate and return list of PDBDATA objects written
;              - to the internal file

(define (pdb-data-gen)

; variations on char
    (define cs (write-pdbdata nil "cs" (type "char" 1) "a"))
    (define ca (write-pdbdata nil "ca" (type "char" 2) "ab"))
    (define cm (write-pdbdata nil "cm" (type "char" 10) "abcdefghi"))

; variations on short
    (define ss (write-pdbdata nil "ss" (type "short" 1) 10000))
    (define sa (write-pdbdata nil "sa" (type "short" 2) 1 2))
    (define sm (write-pdbdata nil "sm" (type "short" 10) 1 2 3 4 5 6 7 8 9 10))

; variations on int
    (define is (write-pdbdata nil "is" (type "int" 1) 10000000))
    (define ia (write-pdbdata nil "ia" (type "int" 2) 1 2))
    (define im (write-pdbdata nil "im" (type "int" 10) 1 2 3 4 5 6 7 8 9 10))

; variations on long
    (define ls (write-pdbdata nil "ls" (type "long" 1) 10000000))
    (define la (write-pdbdata nil "la" (type "long" 2) 1 2))
    (define lm (write-pdbdata nil "lm" (type "long" 10) 1 2 3 4 5 6 7 8 9 10))

; variations on long long
    (define gs (write-pdbdata nil "gs" (type "long_long" 1) 10000000000))
    (define ga (write-pdbdata nil "ga" (type "long_long" 2) 1 2))
    (define gm (write-pdbdata nil "gm" (type "long_long" 10) 1 2 3 4 5 6 7 8 9 10))

; variations on float
    (define fs (write-pdbdata nil "fs" (type "float" 1) 1.0e23))
    (define fa (write-pdbdata nil "fa" (type "float" 2) 1 2))
    (define fm (write-pdbdata nil "fm" (type "float" 10) 1 2 3 4 5 6 7 8 9 10))

; variations on double
    (define ds (write-pdbdata nil "ds" (type "double" 1) 1.0e123))
    (define da (write-pdbdata nil "da" (type "double" 2) 1 2))
    (define dm (write-pdbdata nil "dm" (type "double" 10) 1 2 3 4 5 6 7 8 9 10))

; variations on long double
    (define qs (write-pdbdata nil "qs" (type "long_double" 1) 1.0e234))
    (define qa (write-pdbdata nil "qa" (type "long_double" 2) 1 2))
    (define qm (write-pdbdata nil "qm" (type "long_double" 10) 1 2 3 4 5 6 7 8 9 10))

; variations on float complex
    (define fcs (write-pdbdata nil "fcs" (type "float_complex" 1) 1.0e10))
    (define fca (write-pdbdata nil "fca" (type "float_complex" 2) 1 2))
    (define fcm (write-pdbdata nil "fcm" (type "float_complex" 10) 1 2 3 4 5 6 7 8 9 10))

; variations on double complex
    (define dcs (write-pdbdata nil "dcs" (type "double_complex" 1) 1.0e20))
    (define dca (write-pdbdata nil "dca" (type "double_complex" 2) 1 2))
    (define dcm (write-pdbdata nil "dcm" (type "double_complex" 10) 1 2 3 4 5 6 7 8 9 10))

; variations on long double complex
    (define qcs (write-pdbdata nil "qcs" (type "long_double_complex" 1) 1.0e30))
    (define qca (write-pdbdata nil "qca" (type "long_double_complex" 2) 1 2))
    (define qcm (write-pdbdata nil "qcm" (type "long_double_complex" 10) 1 2 3 4 5 6 7 8 9 10))

    (list cs ca cm is ia im ls la lm gs ga gm
	  fs fa fm ds da dm qs qa qm
	  fcs fca fcm dcs dca dcm qcs qca qcm))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define set (pdb-data-gen))
(pdb-data-print set nil)

(printf nil "\nwriting file foofoo\n")
(define file1 (open-pdbfile "foofoo" "w"))
(display (list-file))
(newline)
(pdb-data-write vars set file1 #f)
(close-pdbfile file1)

(printf nil "\nreading file foofoo\n")
(define file2 (open-pdbfile "foofoo"))
(pdb-data-read file2 vars)
(close-pdbfile file2)

; SCHEME has no complex type internally so this will not work yet
;(printf nil "\ntesting pdb->list\n")
;(pdb-data-list vars set nil)

(quit)

