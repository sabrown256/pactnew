#!/usr/bin/env pdbview
;
; PDBVEXE.SCM - test command line args in PDBView scripts
;

(float-output-format "%Lg")

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (main args)
   (printf nil "command line args = %s\n" args))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(main argv)
(quit)

