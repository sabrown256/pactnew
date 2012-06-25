;
; JOB.SCM - test of pipeline function in SCHEME
;

(trace pipeline)
;(describe pipeline)

; IPC media - pipe, socket, pty
(define (piper x)    (list 'piper x))
(define (pipea x)    (list 'pipea x))
(define (sio   x)    (list 'sio   x))
(define (file  x)    (list 'file  x))
(define (socket h p) (list 'socket h p))
(define (pty x)      (list 'pty x))
(define (url x)      (list 'url x))
(define (var x)      (list 'var x))
(define (fnc x)      (list 'fnc x))

; waitpid info
(define (status x)   (list 'status x))
(define (reason x)   (list 'reason x))

; send signals to processes
(define (signal x)   (list 'signal x))

; set/get environment variables
(define (env . x)    (list 'env x))

; get process info
(define (pid x)      (list 'pid x))
(define (uid x)      (list 'uid x))
(define (gid x)      (list 'gid x))
(define (pgrp x)     (list 'pgrp x))
(define (rusage x)   (list 'rusage x))

; set process limits
(define (rlimits x)  (list 'rlimits x))

(define st -1)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; simple relative pipeline test

(set! st (pipeline #f (piper 1) #f (sprintf "cat %s" "job.scm")
		   #t (piper 1) #f "grep \"pipeline\""
		   #t #t #f "wc -l"))
(printf nil "st = %s\n" st)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; simple absolute pipeline test

(set! st (pipeline #f (pipea 2) (pipea 3) (sprintf "cat %s" "job.scm")
		   #t (pipea 3) (pipea 3) "grep \"pipeline\""
		   #t #t (sio 1) "wc -l"))
(printf nil "st = %s\n" st)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; test input from an expression and output to an expression

(define inp (list 1 "ta" 2 "tb" 3 "tc"))
(define outp nil)
(set! st (pipeline (var inp) (piper 1) #f "cat"
		   #t  (pipea 3) #f "grep \"t\""
		   #t outp #f "wc -l"))
(printf nil "st = %s  out = %s\n" st outp)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; test output to function

(define (awk)
   (let* ((s (read)))
         (if (not (eof s))
	     (begin (printf nil "-> %s\n" s)
		    (awk)))))

(set! st (pipeline #t awk #f (sprintf "cat %s" "job.scm")))
(printf nil "st = %s\n" st)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; test input from function

(define (echo)
   (let* ((s (read)))
         (if (not (eof s))
	     (begin (printf nil "-> %s\n" s)
		    (echo)))))

(set! st (pipeline echo #t #f "cat"))
(printf nil "st = %s\n" st)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; test co-processes

(set! st (pipeline (pipea 2) (pipea 2) #f "jabber to"
		   (pipea 1) (pipea 1) #f "jabber from"))
(printf nil "st = %s\n" st)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; test input from open file

(set! fp (open-input-file "foo.bar")
(set! st (pipeline fp #t #f "cat"))
(printf nil "st = %s\n" st)

; test output to open file

(set! fp (open-output-file "foo.bar"))
(set! st (pipeline #f fp #f "cat"))
(printf nil "st = %s\n" st)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; test input from named file

(set! st (pipeline (file "foo.bar") #t #f "cat"))
(printf nil "st = %s\n" st)

; test output to named file

(set! st (pipeline #f (file "foo.bar") #f "cat"))
(printf nil "st = %s\n" st)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; test input from socket

(set! st (pipeline (socket 15000 "foo.org") #t #f "cat"))
(printf nil "st = %s\n" st)

; test output to socket

(set! st (pipeline #f (socket 15000 "foo.org") #f "cat"))
(printf nil "st = %s\n" st)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(quit)
