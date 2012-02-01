;
; INTP-DF-PROC.SCM - process the raw tabular data into curves
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

; 1d iwd error ~ 1.0/n^1.1
(define (1d-iwd-err)
    (let* ((n 100)
	   (cx (powr (span 4 1000 n) -1.1)))
          (label (my cx 3.0) "1d iwd err")))

; 1d mq error ~ 1.0/n^2
(define (1d-mq-err)
    (let* ((n 100)
	   (cx (powr (span 4 1000 n) -2.0)))
          (label (my cx 5.0) "1d mq err")))

; 2d iwd error ~ 1.0/n^0.6
(define (2d-iwd-err)
    (let* ((n 100)
	   (cx (powr (span 4 1000 n) -0.6)))
          (label (my cx 2.5) "2d iwd err")))

; 2d mq error ~ 1.0/n^1.1
(define (2d-mq-err)
    (let* ((n 100)
	   (cx (powr (span 4 1000 n) -1.1)))
          (label (my cx 3.0) "2d mq err")))

; 3d iwd error ~ 1.0/n^0.4
(define (3d-iwd-err)
    (let* ((n 100)
	   (cx (powr (span 4 1000 n) -0.4)))
          (label (my cx 3.0) "3d iwd err")))

; 3d mq error ~ 1.0/n^1.1
(define (3d-mq-err)
    (let* ((n 100)
	   (cx (powr (span 4 1000 n) -1.1)))
          (label (my cx 10.0) "3d mq err")))

; 4d iwd error ~ -0.02*n + 25
(define (4d-iwd-err)
    (let* ((n 100)
	   (cx (line -0.02 25 4 1000 n)))
          (label (my cx 1.05) "4d iwd err")))

; 4d mq error ~ -0.013*n + 12.5
(define (4d-mq-err)
    (let* ((n 100)
	   (cx (line -0.012 12.5 4 1000 n)))
          (label (my cx 1.05) "4d mq err")))


(define (proc-tab nd)
   (let* ((tdm (read-table* "intp-df.dat" nd))
	  (nr  (car tdm)))
         (label (table-curve nr 2 8 1 8) (sprintf "%dd Tiwd"   nd))
	 (label (table-curve nr 3 8 1 8) (sprintf "%dd Tiwd/N" nd))
	 (label (table-curve nr 4 8 1 8) (sprintf "%dd L1iwd"  nd))
	 (label (table-curve nr 5 8 1 8) (sprintf "%dd Tmq"    nd))
	 (label (table-curve nr 6 8 1 8) (sprintf "%dd Tmq/N"  nd))
	 (label (table-curve nr 7 8 1 8) (sprintf "%dd L1mq"   nd))
	 #t))

(1d-iwd-err)
(1d-mq-err)
(2d-iwd-err)
(2d-mq-err)
(3d-iwd-err)
(3d-mq-err)
(4d-iwd-err)
(4d-mq-err)

(proc-tab 1)
(proc-tab 2)
(proc-tab 3)
(proc-tab 4)

(system "rm -f intp-df.u")
(save intp-df.u (thru a @100))
(end)
