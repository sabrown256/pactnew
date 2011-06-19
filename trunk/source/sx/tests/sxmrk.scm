;
; SXMRK.SCM - test PGS marker routines
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(define dev (pg-make-device "PS" "MONOCHROME" "sxmrk"))
(pg-open-device dev 0 0 0 0)

(define mrk (pg-define-marker -1.0  0.0  0.0  1.0  
			       0.0  1.0  1.0  0.0
			       1.0  0.0  0.0 -1.0
			       0.0 -1.0 -1.0  0.0))

(printf nil "Setting orientation: %s\n" (pg-set-marker-orientation! dev 60.0))
(printf nil "Setting scale: %s\n" (pg-set-marker-scale! dev 0.02))
(printf nil "Checking orientation: %s\n" (pg-get-marker-orientation dev))
(printf nil "Checking scale: %s\n" (pg-get-marker-scale dev))

(pg-clear-window dev)
(pg-axis dev)
(pg-draw-markers dev 2 @world mrk '(0.2 0.4 0.6 0.8) '(0.1 0.9 0.3 0.6))
(pg-finish-plot dev)

(quit)


