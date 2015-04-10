; 
; EZN.SCM - EZN command API for Basis users
; 
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
; 

; Window operations
;    x   change-window               (cw)
;    x   close-window                (clw)
;        list-windows                (lw)
;    x   update-window               (wu)
;        hardcopy-window             (hc)
;        nxm
;
; Viewport operations
;        change-viewport             (cv)
;        close-viewport              (clv)
;    x   list-mappings-in-viewport   (lsv)
;        hardcopy-viewport           (hcv)
;        default-viewport-rendering  (vr)
;        change-palette              (palette)
;    x   delete-mapping              (dl)
;        view-angle                  (va)
;        display-domain              (pld)
;        display-mapping             (pl)
;        drawable-rendering          (dr)
;        domv                        (fix-domain)
;        ranm                        (fix-range)
;    *   re-id
;
; Mapping operations
;        make-ac-mapping-direct      (ac)
;        make-lr-mapping-direct      (lr)
;        make-lr-mapping-synth       (lrs)
;        array-nd-set                (nds)
;        make-cp-set                 (cps)
;        copy-mapping                (cm)
;        print-mapping               (pm)
;        save-mapping                (sm)

; Legend
;   x - implemented
;   * - partially implemented

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (ezn-mode)
   (if (not (defined? ezn-env))
       (begin (define-global ezn-env #t)
	      (synonym update-window sf)
	      (synonym delete-mapping nf))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (win oper . rest)
   (cond ((or (eqv? oper "on") (= oper 1))
	  (let* ((name (if (pair? rest) (car rest) "")))
	        (change-window name)))
	 ((or (eqv? oper "off") (= oper 0))
	  (let* ((name (if (pair? rest) (car rest) "")))
	        (close-window name)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (ps oper . rest)
   (cond ((or (eqv? oper "on") (= oper 1))
	  (let* ((name (if (pair? rest) (car rest) "")))
	        (ps-name name)
		(ps-flag #t)))
	 ((or (eqv? oper "off") (= oper 0))
	  (let* ((name (if (pair? rest) (car rest) "")))
		(ps-flag #f)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

;(define reid re-id)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(ezn-mode)

