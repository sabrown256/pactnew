;
; TGENW.SCM - test pointer handling in BLANG generated wrappers
;           - a list, vector, C_array, or SX_pdbdata should be able
;           - to provide an array for a G_<type>_P_I type in SS_args
;           - NOTE: there is a remaining issue of memory leaking -
;           - the generated wrapper does not handle marking and freeing
;           - memory from the SS_arg calls

(load "measure.scm")

(define measure measure-memory)

(define tl (list 0.1 0.9 0.2 0.8))
(define tv (vector 0.2 0.9 0.2 0.8))
(define ta (list->c-array 0.3 0.9 0.2 0.8))
(define tp (c-array->pdbdata (list->c-array 0.4 0.9 0.2 0.8)))

(define dev  (pg-make-device "PS" "COLOR" "tgenw"))
(pg-open-device dev 0.02 0.04 0.3 0.3)

; void PG_set_viewspace(PG_device *dev ARG(,,cls), int nd,
;                       PG_coord_sys cs, double *box)
; the problem is to get values into BOX
;
; void PG_get_viewspace(const PG_device *dev ARG(,,cls), PG_coord_sys cs,
;                       double *box ARG([0.0,0.0,0.0,0.0,0.0,0.0],out))
; the problem is to return the values put into BOX

(measure (pg-set-viewspace dev 2 NORMC tl))
(printf nil "tl %s\n" (pg-get-viewspace dev NORMC))

(measure (pg-set-viewspace dev 2 NORMC tv))
(printf nil "tv %s\n" (pg-get-viewspace dev NORMC))

(measure (pg-set-viewspace dev 2 NORMC ta))
(printf nil "ta %s\n" (pg-get-viewspace dev NORMC))

(measure (pg-set-viewspace dev 2 NORMC tp))
(printf nil "tp %s\n" (pg-get-viewspace dev NORMC))

(pg-close-device dev)

(define loss (net-memory-loss))
(printf nil "\nNet memory loss: %d bytes\n\n" loss)
(quit loss)
