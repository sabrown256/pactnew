#!pdbview -l
;
; PDBVDEMO - PDBView demo
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(define (display name x y dx dy)
   (cw name "COLOR" "WINDOW" x y dx dy))

(define tag "DEMO")

(ld pdbvxmpl.scm)

