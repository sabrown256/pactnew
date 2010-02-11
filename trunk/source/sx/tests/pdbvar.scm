;
; PDBVAR.SCM - PDB Archive test
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
;

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

(quit)
