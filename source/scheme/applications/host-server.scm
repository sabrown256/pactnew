;
; HOST-SERVER - serve up host names and respond to variou
;             - queries about hosts
;

(define host-server-process nil)
(define host-server-table   nil)
(define host-server-file    "extensions/host-server-db")

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GET-RESPONSE - get and return a string of length greater than 1 from
;              - the process PROC

(define (get-response proc)
    (let* ((s (process-read-line proc)))
          (if (and (string? s) (< 1 (string-length s)))
	      s
	      (get-response proc))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GET-TOKEN - get and return a space delimited token of
;           - length greater than 1 from the process PROC

(define (get-token proc)
    (let* ((t (process-read-line proc))
	   (u (sprintf "%s" (if (string? t) t "")))
	   (s (strtok u " \t\f\n")))
          (if (and (string? s) (< 1 (string-length s)))
	      s
	      (get-token proc))))

;--------------------------------------------------------------------------

;                             HOSTNAME ROUTINES

;--------------------------------------------------------------------------

(define (range base mn mx lst)
   (if (<= mn mx)
       (range base (+ mn 1) mx (cons (sprintf "%s%d" base mn) lst))
       (reverse lst)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HOST-SERVER-INSTALL - install a list of hostnames derived from X
;                     - under the designation TYPE

(define (host-server-install type . x)
    (cond ((and (= (length x) 1) (pair? (car x)))
	   (let* ((lst (car x))
		  (n  (length lst))
		  (hl (list->vector lst))
		  (hi (vector 0 n)))
	         (hash-install type (list hi hl) host-server-table)))
	  (else
	   (let* ((base (list-ref x 0))
		  (mn   (list-ref x 1))
		  (mx   (list-ref x 2))
		  (lst  (range base mn mx nil))
		  (n    (length lst))
		  (hl   (list->vector lst))
		  (hi   (vector 0 n)))
	         (hash-install type (list hi hl) host-server-table)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GET-NEXT-HOST - server side worker routine which maps X
;               - into an available host if it possibly can

(define (get-next-host x)
    (if host-server-table
	(let* ((h (hash-lookup x host-server-table)))
	      (if h
		  (let* ((hi (list-ref h 1))
			 (hl (list-ref h 2))
			 (current (vector-ref hi 0))
			 (ln      (vector-ref hi 1))
			 (host    (vector-ref hl current))
			 (to      1)
			 (out     (syscmnd (sprintf "ssh -x -q %s uptime" host)
					   to)))
		        (vector-set! hi 0 (remainder (+ current 1) ln))
			(if out
			    host
			    (get-next-host x)))
		  x))
	       
; if we don't have a host-server-table the best we can do
; is to see if x is a valid host
	(let* ((host x)
	       (to   1)
	       (out  (syscmnd (sprintf "ssh -x -q %s uptime" host) to)))
	      (if out
		  host
		  "-none-"))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HOST-SERVER-QUERY - runs infinite read/eval/print loop
;                   - expects system type or host name for input
;                   - returns name of host that can be sent work

(define (host-server-query)
    (if (not (null? host-server-table))
	(let* ((q (read)))
              (cond ((eqv? (sprintf "%s" q) "-types-")
		     (printf nil "%s\n" host-server-types))

; handle host queries
		    (else
		     (printf nil "%s\n" (get-next-host q))))

	      (host-server-query))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HOST-SERVER-START - load host-server table from FILE and
;                   - then call the host-server core query function

(define (host-server-start . argl)
    (let* ((name (item argl 0))
	   (file (if name name host-server-file)))
          (if (and (not host-server-table) (file? file nil nil #t))
	      (begin (set! host-server-table (make-hash-table 31))
		     (load file)))
	  (host-server-query)))
(trace host-server-start host-server-query read)
;--------------------------------------------------------------------------

;                              CLIENT SIDE

;--------------------------------------------------------------------------

; HOST-SERVER-INIT - start up the host server from the client side

(define (host-server-init)
    (if (not host-server-process)
	(let* ((call (sprintf "(host-server-start %s)" host-server-file)))
	      (set! host-server-process
		    (process-open "a" "scheme" call))
	      (sleep 100)
	      (process-block host-server-process #t))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GET-HOST-NAME - given a system type SYS return an available host

(define (get-host-name sys)
    (let ((hst (and sys (get-sys-type sys))))
         (if hst
	     (begin (host-server-init)
		    (process-send-line host-server-process hst)
		    (get-token host-server-process)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GET-HOST-TYPES - get the list of all available host types

(define (get-host-types)
    (host-server-init)
    (process-send-line host-server-process "-types-")
    
    (define (chomp x lst)
        (if (> (string-length x) 1)
	    (chomp x (cons (strtok x " \t\n\r()") lst))
	    (reverse lst)))

    (chomp (get-response host-server-process) nil))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HOSTNAME - return the name of the current node

(define (hostname)
    (car (syscmnd "uname -n")))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HOSTTYPE - return the system type of the current node
;          - with no argument:
;          -   return "<OS name> : <OS version>" as reported by uname
;          - with optional argument:
;          -   if procedure return result of calling the procedure
;          -   if string return result of issuing as shell command

(define (hosttype . x)
    (let* ((y (item x 0)))
          (cond ((procedure? y)
		 (y))
		((string? y)
		 (car (syscmnd y)))
		(else
		 (sprintf "%s : %s"
			  (car (syscmnd "uname -s"))
			  (car (syscmnd "uname -r")))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HOSTSYSTEM - return the PACT system type of the specified node

(define (hostsystem . x)
    (let* ((y (item x 0))
           (n (item x 1)))
          (cond ((procedure? y)
		 (y))
		((string? y)
		 (let* ((cmd (sprintf "ssh -x -q %s pact -info SYSTEM_ID" y))
			(res (syscmnd cmd)))
		       (if (pair? res)
			   (sprintf "%s" (car res)))))
		(else
		 system-id))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------
