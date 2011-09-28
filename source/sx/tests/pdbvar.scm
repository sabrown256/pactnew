;
; PDBVAR.SCM - PDB Archive test
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
;

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; ARCHIVE

(printf nil "-------------------------------\n")
(printf nil " archive file demo.d1\n")

cf pdbvar.a~demo.d1
ls

(printf nil "-------------------------------\n")
(printf nil " archive file demo.d2\n")

cf pdbvar.a~demo.d2
ls
types
menu

(printf nil "-------------------------------\n")
(printf nil " archive file demo.d3\n")

cf pdbvar.a~demo.d3
ls
types
menu

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TARFILE

(printf nil "-------------------------------\n")
(printf nil " tar file demo.d1\n")

cf pdbvar.tar~demo.d1
ls

(printf nil "-------------------------------\n")
(printf nil " tar file demo.d2\n")

cf pdbvar.tar~demo.d2
ls
types
menu

(printf nil "-------------------------------\n")
(printf nil " tar file demo.d3\n")

cf pdbvar.tar~demo.d3
ls
types
menu

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; BLOB

(define-global sa 0)
(define-global sb 0)

(printf nil "-------------------------------\n")
(printf nil " blob demo.d1\n")

(set! sa 0)
(set! sb (+ sa (file-length "demo.d1") -1))
(change-file* (sprintf "pdbvar.blob~%d:%d" sa sb))
ls

(printf nil "-------------------------------\n")
(printf nil " blob demo.d2\n")

(set! sa (+ sb 1))
(set! sb (+ sa (file-length "demo.d2") -1))
(change-file* (sprintf "pdbvar.blob~%d:%d" sa sb))
ls
types
menu

(printf nil "-------------------------------\n")
(printf nil " blob demo.d3\n")

(set! sa (+ sb 1))
(set! sb (+ sa (file-length "demo.d3") -1))
(change-file* (sprintf "pdbvar.blob~%d:%d" sa sb))
ls
types
menu

(quit)
