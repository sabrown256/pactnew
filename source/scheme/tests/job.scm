;
; JOB.SCM - test of process group functionality in SCHEME
;

; IPC media - pipe, socket, pty
; (define (piper x)    (list 'piper x))
; (define (pipea x)    (list 'pipea x))
; (define (sio   x)    (list 'sio   x))
; (define (file  x)    (list 'file  x))
; (define (socket h p) (list 'socket h p))
; (define (pty x)      (list 'pty x))
; (define (url x)      (list 'url x))
; (define (var x)      (list 'var x))
; (define (fnc x)      (list 'fnc x))

; waitpid info
; (define (status x)   (list 'status x))
; (define (reason x)   (list 'reason x))

; send signals to processes
; (define (signal x)   (list 'signal x))

; set/get environment variables
; (define (env . x)    (list 'env x))

; get process info
; (define (pid x)      (list 'pid x))
; (define (uid x)      (list 'uid x))
; (define (gid x)      (list 'gid x))
; (define (pgrp x)     (list 'pgrp x))
; (define (rusage x)   (list 'rusage x))

; set process limits
; (define (rlimits x)  (list 'rlimits x))

(define st -1)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; simple relative pipeline tests

(printf nil "----- relative #1\n")
(set! st (syscmnd (sprintf "cat %s | grep \"syscmnd\" | wc -l " "job.scm")))
(printf nil "st = %s\n" st)

(printf nil "----- relative #2\n")
(set! st (syscmnd (sprintf "cat %s @ grep \"syscmnd\" @ wc -l " "job.scm")))
(printf nil "st = %s\n" st)

(printf nil "----- relative #3\n")
(set! st (syscmnd (sprintf "cat %s @e+2 grep \"syscmnd\" @ wc -l " "job.scm")))
(printf nil "st = %s\n" st)

(printf nil "----- relative #4\n")
(set! st (syscmnd (sprintf "cat %s @ grep \"syscmnd\" @ wc -l @i-2e" "job.scm")))
(printf nil "st = %s\n" st)

(printf nil "----- relative #5\n")
(set! st (syscmnd (sprintf "cat %s @o+1@e+2 grep \"syscmnd\" @ef/dev/null wc -l " "job.scm")))
(printf nil "st = %s\n" st)

(printf nil "----- relative #6\n")
(set! st (syscmnd (sprintf "cat %s @o+1@efcat.err grep \"syscmnd\" @ wc -l " "job.scm")))
(printf nil "st = %s\n" st)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; simple absolute pipeline test

(printf nil "----- absolute #1\n")
(set! st (syscmnd (sprintf "cat %s @e3 grep \"syscmnd\" @ wc -l " "job.scm")))
(printf nil "st = %s\n" st)

(printf nil "----- absolute #2\n")
(set! st (syscmnd (sprintf "cat %s @o2@e3 grep \"syscmnd\" @ef/dev/null wc -l " "job.scm")))
(printf nil "st = %s\n" st)

(printf nil "----- absolute #3\n")
(set! st (syscmnd (sprintf "cat %s @ grep \"syscmnd\" @ wc -l @i1e" "job.scm")))
(printf nil "st = %s\n" st)

(printf nil "----- absolute #4\n")
(set! st (syscmnd (sprintf "cat %s @o2@efcat.err grep \"syscmnd\" @ wc -l " "job.scm")))
(printf nil "st = %s\n" st)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; test input from an expression and output to an expression

; (define inp (list 1 "ta" 2 "tb" 3 "tc"))
; (define outp nil)
; (set! st (pipeline (var inp) (piper 1) #f "cat"
; 		   #t  (pipea 3) #f "grep \"t\""
; 		   #t outp #f "wc -l"))
; (printf nil "st = %s  out = %s\n" st outp)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; test output to function

; (define (awk)
;    (let* ((s (read)))
;          (if (not (eof s))
; 	     (begin (printf nil "-> %s\n" s)
; 		    (awk)))))
; 
; (set! st (pipeline #t awk #f (sprintf "cat %s" "job.scm")))
; (printf nil "st = %s\n" st)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; test input from function

; (define (echo)
;    (let* ((s (read)))
;          (if (not (eof s))
; 	     (begin (printf nil "-> %s\n" s)
; 		    (echo)))))
; 
; (set! st (pipeline echo #t #f "cat"))
; (printf nil "st = %s\n" st)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; test co-processes

; (set! st (pipeline (pipea 2) (pipea 2) #f "jabber to"
; 		   (pipea 1) (pipea 1) #f "jabber from"))
; (printf nil "st = %s\n" st)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; test input from open file

; (set! fp (open-input-file "foo.bar")
; (set! st (pipeline fp #t #f "cat"))
; (printf nil "st = %s\n" st)

; test output to open file

; (set! fp (open-output-file "foo.bar"))
; (set! st (pipeline #f fp #f "cat"))
; (printf nil "st = %s\n" st)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; test input from named file

; (set! st (pipeline (file "foo.bar") #t #f "cat"))
; (printf nil "st = %s\n" st)

; test output to named file

; (set! st (pipeline #f (file "foo.bar") #f "cat"))
; (printf nil "st = %s\n" st)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; test input from socket

; (set! st (pipeline (socket 15000 "foo.org") #t #f "cat"))
; (printf nil "st = %s\n" st)

; test output to socket

; (set! st (pipeline #f (socket 15000 "foo.org") #f "cat"))
; (printf nil "st = %s\n" st)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(quit)
