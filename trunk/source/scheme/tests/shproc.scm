;
; SHPROC.SCM - test the process control capabilities both of PPC and SCHEME
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(define (send-command proc cmd)
    (printf nil "\nSending: %s\n" cmd)
    (process-send-line proc cmd)
    (display-response proc)
    (show-status proc))

(define (show-status proc)
    (let* ((stat (process-status proc))
	   (runs (cdddr stat))
	   (runp (car runs))
	   (reason (cadr runs)))
          (printf nil
		  "Status for process \#%d: %s\n"
		  (car stat)
		  (cond ((= runp 0)  "Running")
			((= runp 1)  (sprintf "Stopped (%d)" reason))
			((= runp 2)  (sprintf "Changed (%d)" reason))
			((or (= runp 4) (= runp 6))
			 (sprintf "Exited (%d)" reason))
			((or (= runp 8) (= runp 10))
			 (sprintf "Coredumped (%d)" reason))
			((= runp 16) (sprintf "Signaled (%d)" reason))
			(else        (sprintf "Unkown (%d %d)" runp reason))))))

(define (exit-status proc)
    (sleep 500)
    (display-response proc)
    (let* ((stat (process-status proc))
	   (runs (cdddr stat))
	   (runp (car runs))
	   (reason (cadr runs)))
          (if (= runp 0)
	      (exit-status proc)
	      reason)))

(define (display-response proc)
    (let* ((s (process-read-line proc)))
          (if s
	      (begin (printf nil "| %s" s)
		     (display-response proc))
	      (printf nil "\n"))))

(printf nil "Old # read attempts: %d\n" (process-read-tries))
(printf nil "New # read attempts: %d\n" (process-read-tries 10000))

(printf nil "\n")

(define child (process-open "a" "scheme"))

(show-status child)
(display-response child)

(send-command child "(car (list 1 2 3))")

(send-command child "(describe process-status)")

(send-command child "(quit)")

(define status (exit-status child))

(process-close child)

(printf nil "\n")

(quit status)

