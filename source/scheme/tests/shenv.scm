#!/usr/bin/env scheme
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;
; test:
;    executable nature of scheme scripts
;    environment manipulation
;    process execution with pipelines

(printf nil "\ndefine environment variable FOO\n")
(define $FOO 3)
(printf nil "\n")
(printf nil "-> %s\n" (printenv "FOO"))

(printf nil "\nverify that FOO made it out to the environment\n")
(printf nil "expect: 3\n")
(printf nil "-> %s\n" (syscmnd "echo $FOO"))

; test syscmnd running a shell for re expansion
(printf nil "\nlist SCM files\n")
(printf nil "-> %s\n" (syscmnd "ls -1 ../../shpr*.c"))

(printf nil "\ntest pipelines without a shell\n")
(printf nil "expect: shprc2.c shprm1.c\n")
(printf nil "-> %s\n" (syscmnd "ls -1 ../../shpr*.c | head -n 3 | tail -n 2"))

(quit)
