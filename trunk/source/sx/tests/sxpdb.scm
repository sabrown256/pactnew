; 
; SXPDB.SCM - test all PDB functions in SX
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(ld sxpdbg.scm)

(define set (pdb-data-gen))
(pdb-data-print set nil)

(printf nil "Creating file foofoo\n")
(define file1 (open-pdbfile "foofoo" "w"))
(display (list-file))
(newline)
(pdb-data-write vars set file1 #t)
(io-stats file1)
(close-pdbfile file1)

(printf nil "reading file foofoo\n")
(define file2 (open-pdbfile "foofoo"))
(pdb-data-read file2 vars)
(io-stats file2)
(close-pdbfile file2)

(printf nil "testing pdb->list\n")
(pdb-data-list vars set nil)

(quit)

