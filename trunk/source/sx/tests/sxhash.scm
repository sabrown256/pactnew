;
; SXHASH.SCM -  this file is to test writing hash tables to a pdbfile
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(set-format "short"   "%4d")
(set-format "integer" "%6d")
(set-format "long"    "%8d")
(set-format "float"   "%11.3e")
(set-format "double"  "%11.3e")

; make a hash table and install three strings
(define hhh (make-hash-table 3))
(hash-install "dog" "doggie" hhh)
(hash-install "cat" "kitty"  hhh)
(hash-install "bird" "birdy" hhh)

; convert the hash table to a data object "aaa"
(define aaa (hash->pdbdata nil hhh "hash_aaa"))

; write the hash table again as variable "bbb"
(define bbb (write-pdbdata nil "hash_bbb" aaa))

(define ccc (find-types bbb))
;(printf nil "%s\n" ccc)

;  install additional number into the hash table.
;  NOTE: hash table "hhh" and pdbdata "aaa" are actually the
;        same memory, only repackaged.  So these are also added
;        to "aaa".  "bbb" is actually a copy of "aaa", i.e. it
;        occupies different memory, so it will not change.
(hash-install "double-number" 3.1415926 hhh)
(hash-install "long-number"   23        hhh)

(printf nil "\n----- HASH TABLE A\n\n")
(print-pdb nil aaa)

(printf nil "\n----- HASH TABLE B\n\n")
(print-pdb nil bbb)

;  open a file
(define file3 (open-pdbfile "foofoo" "w"))
(def-common-types file3)

; write out both hash tables
(printf nil "\n----- A TYPES\n\n")
(for-each (lambda (i)
            (write-defstr* file3 (read-defstr* nil i)))
          (find-types aaa))

(printf nil "\n----- B TYPES\n\n")
(for-each (lambda (i)
            (write-defstr* file3 (read-defstr* nil i)))
          (find-types bbb))

(write-pdbdata file3 "aaa" aaa)
(write-pdbdata file3 "bbb" bbb)

; read back both hash tables
(close-pdbfile file3)
(define file3 (open-pdbfile "foofoo" "a"))

;
; GOTCHA: the current definition of object cannot be read
; even though it can be written. Nobody knows how to set
; SC_arrtype on the object currently.
;
;(define ccc (read-pdbdata file3 "aaa"))
;(define ddd (read-pdbdata file3 "bbb"))

;(print-pdb nil ccc)
;(print-pdb nil ddd)

; read out the symtab table and structure chart
(define symtab (car (pdb->list file3)))
(define symsym (hash->pdbdata nil symtab "symtab"))

(set-switch 0 2)
(set-switch 2 1)

; GOTCHA: the content of the symbol table seems to be different
; between threaded and unthreaded versions making this unsuitable
; for the test
;(printf nil "\n----- SHOW THE SYMBOL TABLE\n\n")
;(print-pdb nil symsym)

(define chart (cadr (pdb->list file3)))
(define chrtchrt (hash->pdbdata nil chart "chart"))

(printf nil "\n----- SHOW THE STRUCTURE CHART\n\n")
(print-pdb nil chrtchrt)

(printf nil "\n----- PRINT A HASH TABLE\n\n")
(write (pdb->list aaa))

; write out the symbol table
; GOTCHA: this is frought with dire peril
; the symtab is added to whenever a pointer is written
; so it makes no sense to try to write this and in fact
; PDB will do very strange things
;(write-pdbdata file3 "symtab" symsym)

; NOTE: writing out the chart will fail currently because the primitive
; types refer to statically allocated names such as SC_INTEGER_S
; this will die in _PD_wr_indirection since SC_arrlen will return
; -1 for these strings
;(write-pdbdata file3 "chart" chrtchrt)

(hash-lookup "dog" hhh)

(close-pdbfile file3)
(newline)
(quit)





