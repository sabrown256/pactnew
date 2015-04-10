; 
; SXPDBI.SCM - test VIF PDB functionality in SX
;
;  Stewart Brown 4/21/98
; 
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(define (test-array file)
    (printf nil "\nArray test on %s\n" file)
    (write-pdbdata file "hola"
		   (type "short" 3 4)
		   (list 1 2 3 4 5 6 7 8 9 10 11 12))
    (varset! hola[0:1,2:3] -1)
    (varprint hola))

(format short %8d)

(test-array (change-file* "foofoo" "w"))

(test-array (cf f0))

(end)
