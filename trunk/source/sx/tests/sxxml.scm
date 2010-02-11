;
; SXXML.SCM - XML DB test
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
;

;----------------------------------------------------------------
;----------------------------------------------------------------

(printf nil "------------------------------\n")
(printf nil "Test #0\n")

(define fp (open-pdbfile "txml0.xml,fmt=xml" "r"))

(printf nil "Types: %s\n" (list-defstrs fp))
(printf nil "Vars: %s\n" (list-variables fp "-R" "*"))
(printf nil "Doubles: %s\n" (list-variables fp "-R" "*" "double"))

(print-pdb nil (read-pdbdata fp "/tst/doc-1/ts-1"))

(printf nil "Dir: %s\n" (list-variables fp nil "/tst/doc-1/cn-1/gr-1/pt-1"))

(desc-variable (read-pdbdata fp "/tst/doc-1/cn-1/gr-1/pt-1/x-1"))
(print-pdb nil (read-pdbdata fp "/tst/doc-1/cn-1/gr-1/pt-1/x-1"))

(change-directory fp "/tst/doc-1/cn-1/gr-1/pt-1")

(printf nil "\n%s\n" (current-directory fp))
(print-pdb nil (read-pdbdata fp "x-1"))

(close-pdbfile fp)

(printf nil "Test #0 complete\n\n")

;----------------------------------------------------------------
;----------------------------------------------------------------

(printf nil "------------------------------\n")
(printf nil "Test #1\n")

(define fp (open-pdbfile "txml1.xml,fmt=xml" "r"))

(printf nil "Types: %s\n" (list-defstrs fp))
(printf nil "Vars: %s\n" (list-variables fp))

(close-pdbfile fp)

(printf nil "Test #1 complete\n\n")

;----------------------------------------------------------------
;----------------------------------------------------------------

(printf nil "------------------------------\n")
(printf nil "Test #2\n")

(define fp (open-pdbfile "txml2.xml,fmt=xml" "r"))

(printf nil "Types: %s\n" (list-defstrs fp))
(printf nil "Vars: %s\n" (list-variables fp))

(close-pdbfile fp)

(printf nil "Test #2 complete\n\n")

;----------------------------------------------------------------
;----------------------------------------------------------------

(quit)
