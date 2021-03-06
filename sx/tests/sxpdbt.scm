;
; SXPDBT.SCM - PDB TEXT test
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
;

(ld sxpdbg.scm)

(set-switch 0 0)
(set-switch 1 0)
(set-switch 2 0)

(define set (pdb-data-gen))

(printf nil "Creating file foofoo\n")
(define file1 (open-pdbfile "foofoo,tgt=txt" "w"))
(pdb-data-write vars set file1 #t)
(io-stats file1)
(close-pdbfile file1)

(printf nil "reading file foofoo\n")
(define file2 (open-pdbfile "foofoo"))
(pdb-data-read file2 vars)
(io-stats file2)
(close-pdbfile file2)

(quit)
