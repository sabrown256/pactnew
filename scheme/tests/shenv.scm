#!/usr/bin/env scheme
;
; SHENV.SCM - executable nature of scheme scripts
;           - environment manipulation
;           - process execution with pipelines
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(printf nil "\ndefine environment variable FOO\n")
(define $FOO 3)

(define cmd (sprintf "ls -1 %s/shprm?.c" (getenv "SrcDir")))

(printf nil "\n")
(printf nil "-> %s\n" (printenv "FOO"))

(printf nil "\nverify that FOO made it out to the environment\n")
(printf nil "expect: 3\n")
(printf nil "-> %s\n" (syscmnd "echo $FOO"))

; test syscmnd running a shell for re expansion
(printf nil "\nlist SCM files\n")
(printf nil "-> %s\n" (syscmnd cmd))

(printf nil "\ntest pipelines without a shell\n")
(printf nil "expect: shprm2.c shprm3.c\n")
(printf nil "-> %s\n" (syscmnd (sprintf "%s | head -n 3 | tail -n 2" cmd)))

(quit)
