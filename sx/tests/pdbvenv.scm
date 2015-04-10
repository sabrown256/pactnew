#!/usr/bin/env pdbview
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;
; test:
;    executable nature of pdbview scripts
;    environment manipulation
;    process execution with pipelines

(printf nil "\ndefine environment variable FOO\n")
(define $FOO 3)
(printf nil "-> %s\n" (printenv "FOO"))

(printf nil "\nverify that FOO made it out to the environment\n")
(printf nil "expect: 3\n")
(printf nil "-> %s\n" (syscmnd "echo $FOO"))

; test syscmnd running a shell for re expansion
(printf nil "\nlist SCM files\n")
(printf nil "-> %s\n" (syscmnd "ls -1 ../../sxpdb[a-z].c"))

(printf nil "\ntest pipelines without a shell\n")
(printf nil "expect: ../../sxpdbd.c ../../sxpdbf.c\n")
(printf nil "-> %s\n" (syscmnd "ls -1 ../../sxpdb[a-z].c | head -n 4 | tail -n 2"))

(quit)
