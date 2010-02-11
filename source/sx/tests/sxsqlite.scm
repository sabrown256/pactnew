;
; SXSQLITE.SCM - SQLite DB test
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
;

;----------------------------------------------------------------
;----------------------------------------------------------------

; write database

(define fp (open-bin-file "foo.sqlite,fmt=sqlite" "w"))

(make-defstr* fp "work" (def-member integer id)
	                (def-member float   fval)
	                (def-member double  dval)
			(def-member char name 12))

(define w1 (write-pdbdata fp "work"
			     (type "work")
				   '(1
				     3.14
				     3.1415
				     "pi")))

(define w2 (write-pdbdata fp "work"
			     (type "work")
				   '(2
				     2.72
				     2.7183
				     "e")))

(close-pdbfile fp)

;----------------------------------------------------------------
;----------------------------------------------------------------

(set-switch 0 0)
(set-switch 1 0)
(set-switch 2 0)

; read back

cf foo.sqlite
ls
types
struct work
desc *
work
end
