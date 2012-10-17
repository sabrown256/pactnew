#!/usr/bin/env pdbview
;
; PDBVBATCH - batch the PDBView examples
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(define (display name x y dx dy)
   (cw name "COLOR" "PS" 0 0 1 1))

; redefine pause as a no-op
(define (pause) #t)

(define tag "BATCH")
(define look "original")

(system "rm -f demo.d* > /dev/null 2>&1")

(ld pdbvxmpl.scm)

