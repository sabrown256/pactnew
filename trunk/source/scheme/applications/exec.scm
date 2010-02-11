;
; EXEC - execute jobs possibly in multiple directories and/or hosts
;      - with persistence
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
;

(define exec-n-attempts 3)
(define exec-delay      10000)  ; milliseconds

(define zero-dt (list 0 0 0 0 0))

;--------------------------------------------------------------------------

;                              HELPER ROUTINES

;--------------------------------------------------------------------------

; ABSOLUTE-PATH - fix a directory name to be an absolute path

(define (absolute-path dir)
   (if dir
       (if (eqv? (string-ref dir 0) #\/)
	   dir
	   (sprintf "%s/%s" (getenv "HOME") dir))
       (getcwd)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TIME->STRING - make a nice printing representation of a time

(define (time->string t)
    (if (pair? t)
	(let* ((w (list-ref t 0))
	       (d (list-ref t 1))
	       (h (list-ref t 2))
	       (m (list-ref t 3))
	       (s (list-ref t 4)))
	      (cond ((< 0 w)
		     (sprintf "%d:%02d:%02d:%02d:%02d" w d h m s))
		    ((< 0 d)
		     (sprintf "%d:%02d:%02d:%02d" d h m s))
		    ((< 0 h)
		     (sprintf "%d:%02d:%02d" h m s))
		    ((< 0 m)
		     (sprintf "%d:%02d" m s))
		    (else
		     (sprintf "%d" s))))
	(sprintf "%.2f" t)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; ELAPSED-TIME-MAX - return the greater of two times
;                  - the times must be in list form
;                  - as returned by the TIME-ELAPSED function

(define (elapsed-time-max ta tb)
    (if (and (pair? ta) (pair? tb))
	(let* ((wa (list-ref ta 0))
	       (da (list-ref ta 1))
	       (ha (list-ref ta 2))
	       (ma (list-ref ta 3))
	       (sa (list-ref ta 4))
	       (wb (list-ref tb 0))
	       (db (list-ref tb 1))
	       (hb (list-ref tb 2))
	       (mb (list-ref tb 3))
	       (sb (list-ref tb 4)))
	      (cond ((< wa wb) tb)  ; week
		    ((> wa wb) ta)
		    ((< da db) tb)  ; day
		    ((> da db) ta)
		    ((< ha hb) tb)  ; hour
		    ((> ha hb) ta)
		    ((< ma mb) tb)  ; minute
		    ((> ma mb) ta)
		    ((< sa sb) tb)  ; second
		    ((>= sa sb) ta)))))

;--------------------------------------------------------------------------

;                               WATCH ROUTINES

;--------------------------------------------------------------------------

(define watch-window nil)
(define watch-file nil)
(define watch-file-name (sprintf "%s/log.junk" (getcwd)))
(define watch-table (make-hash-table 31))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (watch-entry status dt nrun)
    (list status dt nrun))

(define (watch-entry-cfg entry)
    (list-ref entry 0))

(define (watch-entry-status entry)
    (list-ref entry 1))

(define (watch-entry-time entry)
    (list-ref entry 2))

(define (watch-entry-running entry)
    (list-ref entry 3))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WATCH-JOB - make the contribution of JOB to the watch entry
;           - for the configuration CFG of which JOB is a part

(define (watch-job job)
    (let* ((dir  (job-directory job))
	   (prc  (job-process job))
	   (na   (+ 1 (- exec-n-attempts (job-n-attempts job))))
	   (cfg  (get-sys-config (job-system job)))
	   (dt   (if (process? prc) zero-dt (job-elapsed-time job)))
           (ent  (hash-lookup cfg watch-table))
           (st   (if ent (watch-entry-status  ent) " 0"))
           (dtx  (if ent (watch-entry-time    ent) zero-dt))
           (nr   (if ent (watch-entry-running ent) 0))
	   (pck  (lasttok (sprintf "%s" dir) "/"))
	   (jsi  (if (process? prc)
		     " "
		     (if (job-succeeded? job)
			 (cond ((< na 2) ".")
			       ((< na 3) "o")
			       (else     "O"))
			 "x")))
	   (nrun (if (process? prc)
		     (+ nr 1)
		     nr))
	   (nq   (quotient (string-length st) 12))
	   (nr   (remainder (string-length st) 12))
	   (nst  (if (= nr 0)
		     (sprintf "%s%2d%s" st (* nq 10) jsi)
		     (sprintf "%s%s" st jsi))))
          (hash-install cfg
			(watch-entry nst
				     (elapsed-time-max dtx dt)
				     nrun)
			watch-table)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WATCH-OUT - print out the final watch entry for configuration CFG

(define (watch-out cfg)
    (let* ((ent (hash-lookup cfg watch-table))
	   (st  (watch-entry-status  ent))
	   (dt  (watch-entry-time    ent))
	   (nr  (watch-entry-running ent))
	   (nst (if (= (remainder (string-length st) 12) 0)
		     (sprintf "%s|" st)
		     st)))
          (if (= nr 0)
	      (printf watch-file " %-12s %s (%s)\n" cfg nst (time->string dt))
	      (printf watch-file " %-12s %s\n" cfg nst))
	  (hash-remove cfg watch-table)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WATCH - visually monitor the progress of the list of processes PRCS

(define (watch prcs)

; initialize the watch window one time
    (if watch-window
        (process-send-line watch-window "clear")
        (let* ((cmd (sprintf "tail -f %s" watch-file-name)))
	      (set! watch-file (fopen watch-file-name "w"))
	      (set! watch-window (process-open "w"
					       "xterm"
					       "-geometry" "90x15-10+10"
					       "-T" "Exec Watch"
					       "+sb"
					       "-e" cmd))))

; check on the jobs
    (for-each watch-job prcs)

; print the results
    (printf watch-file "\n\n\n\n\n\n\n\n\n\n\n\n\n")
    (printf watch-file "Command: %s\n"
            (lasttok (sprintf "%s" (job-command (list-ref prcs 0))) ";"))
    (printf watch-file "Time: %s\n"
            (time->string (time-elapsed (job-start-time (list-ref prcs 0)) #f)))
    (printf watch-file "--------------------------------------------------------------\n")
    (for-each watch-out (hash-dump watch-table nil #t))

    (printf watch-file "\n\n")
    (printf watch-file "Legend:\n")
    (printf watch-file " <sp> working\n")
    (printf watch-file "   .  ok 1rst try\n")
    (printf watch-file "   o  ok 2nd try\n")
    (printf watch-file "   O  ok 3->n try\n")
    (printf watch-file "   x  fail\n"))

;--------------------------------------------------------------------------

;                               EXEC ROUTINES

;--------------------------------------------------------------------------

; JOB structure: (<process>
;                 (<terminal-output-text> ...)
;                 <system>
;                 <host>
;                 <directory>
;                 <command>
;                 <environment>
;                 <cleanup-command>
;                 <number-of-attempts>
;                 <start-time>
;                 <output-filter-function>)

; JOB Methods

(define (make-job prc out sys hst dr cmd env clean na t0 flt)
    (list prc out sys hst dr cmd env clean na t0 flt))

(define (remake-job prc out x)
    (append (list prc out) (list-tail x 2)))

(define (job-process x)        (list-ref x 0))
(define (job-output  x)        (list-ref x 1))
(define (job-system x)         (list-ref x 2))
(define (job-host x)           (list-ref x 3))
(define (job-directory x)      (list-ref x 4))
(define (job-command x)        (list-ref x 5))
(define (job-environment x)    (list-ref x 6))
(define (job-cleanup-cmd x)    (list-ref x 7))
(define (job-n-attempts x)     (list-ref x 8))
(define (job-start-time x)     (list-ref x 9))
(define (job-output-filter x)  (list-ref x 10))

(define (job-proc-state p)     (cdddr (process-status p)))
(define (job-process-state x)  (job-proc-state (job-process x)))

; Finished JOB Methods

(define (job-finished msg pid status dt)
    (list msg pid status dt))

(define (fin-job-termination p)    (list-ref p 0))
(define (fin-job-signal p)         (list-ref p 1))
(define (fin-job-exit-status p)    (list-ref p 2))
(define (fin-job-time p)           (list-ref p 3))

(define (fin-job-failed? p)        (eqv? (fin-job-termination p) "fail"))
(define (fin-job-succeeded? p)     (and (eqv? (fin-job-termination p) "done")
					 (= (fin-job-exit-status p) 0)))

(define (job-termination x)    (fin-job-termination (job-process x)))
(define (job-signal x)         (fin-job-signal      (job-process x)))
(define (job-exit-status x)    (fin-job-exit-status (job-process x)))
(define (job-elapsed-time x)   (fin-job-time        (job-process x)))
(define (job-failed? x)        (fin-job-failed?     (job-process x)))
(define (job-succeeded? x)     (fin-job-succeeded?  (job-process x)))

; JOB System accessors

(define (get-sys-type   x)     (if (pair? x) (list-ref x 0) x))
(define (get-sys-env    x)     (if (pair? x) (list-tail x 1) nil))
(define (get-sys-config x)     (if (pair? x) (list-ref x 2) x))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MAKE-FULL-COMMAND - make out the full command to be run by CSH
;                   - including the cd to DIR and the setting of
;                   - environment variables ENV

(define (make-full-command dir env cmd txt)
    (if env
	(let* ((vr (list-ref env 0))
	       (vl (list-ref env 1))
	       (rest (list-tail env 3))
	       (sete (sprintf "setenv %s %s ; " vr vl))
	       (txte (sprintf "%s%s" txt sete)))
	      (make-full-command dir rest cmd txte))
	(if (or (null? txt) (< (string-length txt) 6))
	    (sprintf "(cd %s ; %s)" dir cmd)
	    (sprintf "(cd %s ; %s%s)" dir txt cmd))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; LAUNCH - run the command CMD in directory DR on system type SYS
;        - with cleanup command CLEAN in case of failure
;        - return a job structure

(define (launch sys dr cmd env clean na tstart flt)
    (let* ((hst  (get-host-name sys)))
          (if (> na 0)

; run attempt at CMD
	      (let* ((dir  (absolute-path dr))
		     (senv (get-sys-env sys))
		     (shst (get-sys-type sys))
		     (cl   (make-full-command dir (append env senv) cmd ""))
		     (prc  (cond ((and hst (not (eqv? hst "-none-")))
				  (process-open "r"
						"ssh" "-x" "-q" hst
						"/bin/csh"
						"-c"
						(sprintf "'%s'" cl)))
				 ((and hst (eqv? hst "-none-"))
				  (printf nil "\nCannot resolve %s\n" shst)
				  (job-finished "fail" 0 -1 zero-dt))
				 (else
				  (process-open "r"
						"/bin/csh"
						"-c"
						cl))))
		     (msg (if (= na exec-n-attempts)
			      "Command"
			      (sprintf "Command (attempt %d)"
				       (+ 1 (- exec-n-attempts na)))))
		     (out (if hst
			      (sprintf "\n%s: %s\nHost: %s  Directory: %s\n"
				       msg cmd hst dir)
			      (sprintf "\n%s: %s\nDirectory: %s\n"
				       msg cmd dir))))
		    (make-job prc (list out) sys hst dr cmd env clean
			      na tstart flt))

; failed after attempts are exhausted
	      (make-job (job-finished "fail" 0 -1 zero-dt) ""
			sys hst dr cmd env clean 0 tstart flt))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; RE-LAUNCH - re-run the command CMD in directory DR on host HST
;           - with cleanup command CLEAN in case of failure
;           - after EXEC-DELAY milliseconds

(define (re-launch prc)
    (sleep exec-delay)
    (let* ((sys   (job-system prc))
	   (dir   (job-directory prc))
	   (cmd   (job-command prc))
	   (env   (job-environment prc))
	   (clean (job-cleanup-cmd prc))
	   (na    (job-n-attempts prc))
	   (t0    (job-start-time prc))
	   (flt   (job-output-filter prc)))
          (launch sys dir cmd env clean (- na 1) t0 flt)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PRINT-OUTPUT-TEXT - print all of the output text in LST

(define (print-output-text fp lst flt)
    (if (pair? lst)
	(let* ((s (list-ref lst 0)))
	      (if (procedure? flt)
		  (flt fp s)
		  (printf fp "   %s" s))
	      (print-output-text fp (cdr lst) flt))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MAKE-LOG-FILE-NAME - default function to log output from the commands
;                    - the user may replace this function

(define (make-log-file-name sys cmd)
    (sprintf "log.%s" (get-sys-type sys)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; LOG-OUTPUT - write the output to a log file 
;            - the log file name is returned from a
;            - user replacable function MAKE-LOG-FILE-NAME

(define (log-output job prc lst)
    (let* ((sys    (job-system  job))
	   (cmd    (job-command job))
	   (flt    (job-output-filter job))
	   (t0     (job-start-time job))
	   (tf     (time->string (time-elapsed t0 #f)))
	   (failed (fin-job-failed?     prc))
	   (sig    (fin-job-signal      prc))
	   (status (fin-job-exit-status prc))
	   (log    (make-log-file-name sys cmd))
	   (fp     (if log
		       (fopen log "a")
		       nil)))
          (print-output-text fp lst flt)
	  (if failed
	      (printf fp "FAIL[%s,%s] (%s)\n" sig status tf)
	      (printf fp "OK (%s)\n" tf))
	  (if log
	      (fclose fp))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GRAB-OUTPUT - check PROC for output and concatenate it to LST
;             - return a list containing JOB and the augmented output list
;             - if PROC and JOB are not the same close PROC on exit

(define (grab-output proc x)
    (let* ((prc (job-process x))
	   (lst (job-output x))
	   (s   (if (process? prc) (process-read-line prc))))
          (if s
              (grab-output proc (remake-job prc (cons s lst) x))
	      (if (not (eqv? prc proc))
		  (let* ((failed (fin-job-failed? proc)))
		        (process-close prc)
			(log-output x proc (reverse lst))
			(if failed
			    (re-launch x)
			    (remake-job proc nil x)))
		  (remake-job proc lst x)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; ADD-TEXT - add the string S to the list LST of strings so that
;          - each string in the list ends in newline or is the
;          - first string in the list
;          - return a new list of strings
;          - NOTE: before printing the list will be reversed

(define (add-text s lst)
    (if lst
	(let* ((t (list-ref lst 0))
	       (n (string-length t))
	       (c (string-ref t (- n 1))))
	      (if (eqv? c c\n)
		  (cons s lst)
		  (cons (string-append t s) (list-tail lst 1))))
	(list s)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GRAB-OUTPUT - check PROC for output and concatenate it to LST
;             - return a list containing JOB and the augmented output list
;             - if PROC and JOB are not the same close PROC on exit

(define (grab-output proc x)
    (let* ((prc (job-process x))
	   (lst (job-output x))
	   (s   (if (process? prc) (process-read-line prc))))
          (if s
              (grab-output proc (remake-job prc (add-text s lst) x))
	      (if (not (eqv? prc proc))
		  (let* ((failed (fin-job-failed? proc)))
		        (process-close prc)
			(log-output x proc (reverse lst))
			(if failed
			    (re-launch x)
			    (remake-job proc nil x)))
		  (remake-job proc lst x)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GET-STATUS - if given a process X return:
;            -    the job if it is running
;            -    a job with status info instead of the process if not
;            - or return the argument X as is

(define (get-status x)
    (let* ((prc (job-process x))
	   (out (job-output x)))
	  (if (process? prc)
	      (let* ((state  (job-proc-state prc))
		     (sig    (list-ref state 0))
		     (status (list-ref state 1))
		     (t0     (job-start-time x))
		     (dt     (time-elapsed t0 #f)))
		    (cond ((or (= sig 0) (= sig 1) (= sig 2))
			   (grab-output prc x))

			  ((and (or (= sig 4) (= sig 6)) (= status 0))
			   (grab-output (job-finished "done" sig status dt)
					x))

			  (else
			   (grab-output (job-finished "fail" sig status dt)
					x))))
	      x)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; ALL-DONE? - return #t iff all processes in PLST are done

(define (all-done? plst res)
    (let* ((prc (list-ref plst 0)))
	  (if (and (pair? prc) res)
	      (all-done? (cdr plst) (and res (pair? (car prc))))
	      res)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; FINAL-RESULT - return the number of jobs in PLST that failed

(define (final-result plst st)

    (if (pair? plst)
	(let* ((job   (list-ref plst 0))
	       (rest  (list-tail plst 1))
	       (state (job-termination job))
	       (exit  (job-exit-status job)))
              (final-result rest (+ st (if (job-succeeded? job) 0 1))))
	st))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MONITOR-JOBS - monitor the jobs in PRCS and return a status value
;              - when they are all done

(define (monitor-jobs prcs watch)
    (sleep 5000)
    (let* ((plst (map get-status prcs)))

; do the watch operation 
          (if (procedure? watch)
	      (watch plst))

; check the status of all the jobs
          (if (all-done? plst #t)
	      (final-result plst 0)
	      (monitor-jobs plst watch))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; EXEC - run CMD in each directory listed in DIRS
;      - on the specfied hosts, HOSTS
;      - run with required persistence
;      - usage: (exec <cmd> [<dir-list> [<host-list>
;      -         [<watch> [<filter> [<env-list> [<cleanup-cmd>]]]]]])
;      - where
;      -   <cmd>         := string sent to 'system'
;      -   <dir-list>    := list of directories in which <cmd> is to be run
;      -   <host-list>   := list of system types on which <cmd> is to be run
;      -                    items on this list may be lists of the form:
;      -                    (<host> <var1> <val1> ... <varn> <valn>)
;      -                    this allows for system specific environment setttings
;      -   <watch>       := watch function
;      -   <filter>      := output filter function
;      -   <env-list>    := list of environment variables to be set prior to
;      -                    running <cmd>, the form is:
;      -                    (<var1> <val1> <var2> <val2> ... <varn> <valn>)
;      -   <cleanup-cmd> := a command to run before retrying <cmd>

(define (exec cmd . args)
   (if cmd
       (let* ((dirl   (item->list args 0))
	      (dirs   (if dirl dirl (list (getcwd))))
	      (hsts   (item->list args 1))
	      (watch  (item args 2))
	      (flt    (item args 3))
	      (env    (item->list args 4))
	      (clean  (item args 5))
	      (tc     (time)))

	     (define (exec-on-host h)
	         (define (exec-in-dir d)
		     (launch h d cmd env clean exec-n-attempts (time) flt))
		 (map exec-in-dir dirs))

	     (let* ((wrk (apply append (map exec-on-host hsts)))
		    (res (monitor-jobs wrk watch))
		    (tf  (time->string (time-elapsed tc #f))))
	           (if (= res 0)
		       (printf nil "Succeeded (%s)\n" tf)
		       (printf nil "Failed[%s] (%s)\n" res tf))
		   res))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------
