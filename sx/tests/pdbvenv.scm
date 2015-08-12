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
(define cmd (sprintf "ls -1 %s/sxpdb[a-z].c | sed 's|^.*/||'" (getenv "SrcDir")))
(printf nil "-> %s\n" (syscmnd cmd))

(printf nil "\ntest pipelines without a shell\n")
(printf nil "expect: sxpdbd.c sxpdbf.c\n")
(define cmd (sprintf "ls -1 %s/sxpdb[a-z].c | sed 's|^.*/||' | head -n 4 | tail -n 2" (getenv "SrcDir")))
(printf nil "-> %s\n" (syscmnd cmd))

(quit)
