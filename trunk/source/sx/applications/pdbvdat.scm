; 
; PDBVDAT.SCM - data manager routines for PDBView
; 
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
; 

(define data-manager-table #f)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PROCESS-SET-OPER - support for SET-OPERATOR

(define (process-set-oper func var out)
    (if func
	(cond ((or (pm-set? var) (pm-mapping? var) (pg-graph? var))
	       (func var))
	      (var
	       (let* ((data (pdb-read-numeric-data current-file var))
		      (n    (pm-array-length data)))
		 (cons (pm-make-set var (list n) (list (func data))) out))))
	(cond ((or (pm-set? var) (pm-mapping? var) (pg-graph? var))
	       var)
	      (var
	       (let* ((data (pdb-read-numeric-data current-file var))
		      (n    (pm-array-length data)))
		 (cons (pm-make-set var (list n) (list data)) out))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SET-OPERATOR - process specifications such as
;              - x log y  ->  x (log y)

(define (set-operator in out)
    (if in
	(let* ((a1 (list-ref in 0))
	       (a2 (list-ref in 1)))
	  (if (printable? a1)
	      (set-operator (list-tail in 1)
			    (process-set-oper nil a1 out))
	      (set-operator (list-tail in 2)
			    (process-set-oper (eval a1) a2 out))))
	(reverse out)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DIMENSIONS->SHAPE

(define (dimensions->shape x)
    (let* ((dims (if (or (string? x) (symbol? x))
		     (variable-dimensions x)
		     x)))
      (if dims
	  (map (lambda (x) (- (cdr x) (car x) -1)) dims))))

; this doesn't work because of a bug in map having to do with the
; handling of the result of the expr evaluation and the garbage collector
;	(map (lambda (x) (- (cdr x) (car x) -1)) (variable-dimensions x)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PARSE-SPECS - process specifications such as
;             - x log y  ->  x (log y)

(define (parse-specs argl)
    (let* (oper pnm vnm data len dms rest)
      (if (pair? argl)
	  (let* ((a1 (list-ref argl 0)))
	    (if (printable? a1)
		(begin
		  (set! oper nil)
		  (set! vnm a1)
		  (set! pnm a1)
		  (set! rest (list-tail argl 1)))
		(begin
		  (set! oper (eval a1))
		  (set! vnm (list-ref argl 1))
		  (set! pnm (sprintf "%s(%s)" a1 vnm))
		  (set! rest (list-tail argl 2)))))
	  (begin
	    (set! oper nil)
	    (set! vnm argl)
	    (set! pnm argl)
	    (set! rest nil)))

      (set! data
	    (if (procedure? oper)
		(oper (pdb-read-numeric-data current-file vnm))
		(pdb-read-numeric-data current-file vnm)))
      (set! len  (pm-array-length data))
      (set! dms  (dimensions->shape vnm))

      (if (not dms)
	  (set! dms (list len)))

      (list pnm data dms rest)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; BUILD-SET - turn a list of names into a set

(define (build-set x lst dims name labl)
    (cond ((pm-set? x)
	   x)
	  (x
	   (let* ((specs (parse-specs x))
		  (vnm   (list-ref specs 0))
		  (data  (list-ref specs 1))
		  (dms   (list-ref specs 2))
		  (rest  (list-ref specs 3))
		  (nm    (if (string=? name "")
			     (sprintf "{%s" vnm)
			     (sprintf "%s,%s" name vnm))))

	     (if (or (not dims) (equal? dims dms))
		 (build-set rest (cons data lst) dms nm labl)
		 (printf nil
			 "\nComponent %s has different dimensions\n"
			 first))))
	  (else
	   (pm-make-set (if labl
			    (sprintf "%s,%s}" name labl)
			    (sprintf "%s}" name))
			dims
			(reverse lst)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DM-ADD - the data manager add command

(define (dm-add name fnc)
    (hash-install name fnc data-manager-table))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DM-AC-DIRECT - the data manager "acdirect" command
;              - make mapping from arrays which are set components

(define (dm-ac-direct ran dom centr labl)

    (let* ((rs (build-set ran nil nil "" nil))
	   (ds dom)
	   (cent (if centr centr node)))
      (if (and rs ds)
	  (pg-make-graph ds rs labl cent blue 0.1 solid nil)
	  (printf nil "\nBad component sets\n"))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DM-LR-DIRECT - the data manager "lrdirect" command
;              - make mapping from arrays which are set components

(define (dm-lr-direct ran dom centr emap labl)

    (define (build-lr-set x)
        (let* ((set (caaddr (pdb->list (pm-set->pdbdata x))))
	       (dims-spec (car (list-ref set 3)))
	       (dims (if (vector? dims-spec)
			 (vector->list dims-spec)
			 (list dims-spec))))
	  (pm-make-set "Domain" dims)))

    (let* ((rs (build-set ran nil nil "" nil))
	   (ds (cond ((pm-set? dom)
		      dom)
		     (dom
		      (build-set dom nil nil "" nil))
		     (else
		      (build-lr-set rs))))
	   (cent (if centr centr node)))
      (if (and rs ds)
	  (pg-make-graph ds rs labl cent blue 0.1 solid emap)
	  (printf nil "\nBad component sets\n"))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DM-LR-PRODUCT - the data manager "lrproduct" command
;               - make mapping from Cartesian product of arrays

(define (dm-lr-product ran dom . emap-name)

    (define (proc-comp comp shape ret ldval error)
	(if comp
	    (let* ((first (car comp))
		   (rest (cdr comp))
		   (var (read-pdbdata current-file first))
		   (dims (dimensions->shape
			  (cddadr (pdb->list var))))
		   (data (cons (pdbdata->pm-array var) ret)))
	      (if shape
		  (if (equal? shape dims)
		      (proc-comp rest dims data ldval error)
		      (error "Dimensions do not match"))
		  (proc-comp rest dims data (length comp) error)))
	    (cons (reverse ret)
		  (reverse (cons ldval (reverse shape))))))

    (define (fix-arrays set error)
        (let* ((name  (list-ref set 0))
	       (type  (list-ref set 1))
	       (comp  (proc-comp (list-ref set 2) nil nil nil error))
	       (data  (car comp))
	       (shape (cdr comp)))
	  (list name type shape data)))

    (define (product-aux error)
	(let* ((rs (apply pm-arrays->set (fix-arrays ran error)))
	       (ds (apply pm-arrays->set (fix-arrays dom error))))
	  (if (and rs ds)
	      (pg-make-graph ds rs nil node blue 0.1 solid)
	      (printf nil "\nBad component sets\n"))))

    (let* ((val (call-with-cc product-aux)))
      (if (string? val)
	  (printf nil "Error: %s\n" val)
	  val)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DM-CP-SET - make a Cartesian product set from a given list
;           - of sets

(define (dm-cp-set sets)
    (apply pm-make-cartesian-product-set (set-operator sets nil)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DM-ND-SET - make a higher dimensional set from a given list
;           - of dimensions

(define (dm-nd-set var dims)
    (let* ((data (pdb-read-numeric-data current-file var))
	   (n    (pm-array-length data))
	   (m    (apply * dims)))
      (if (= n m)
	  (pm-make-set var dims (list data)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DATA-MANAGER - a message passing approach to data management
;              - legal data messages:
;              -
;              - add       - add a new function to the data manager
;              - acdirect  - make an AC mapping from arrays of same shape
;              - lrdirect  - make a LR mapping from arrays of same shape
;              - lrproduct - make a LR mapping via (pseudo) Cartesian product
;              - lrsum     - make a LR mapping by piecing sets together
;              - acarr     - make an AC mapping from arrays of same shape

(define (data-manager msg . rest)
   (if (not data-manager-table)
       (begin (set! data-manager-table (make-hash-table 17))
              (dm-add "add" dm-add)))

   (let* ((cmd (hash-lookup msg data-manager-table))
	  (proc (if cmd (cdr cmd))))
     (if (and proc (procedure? proc))
	 (apply proc rest)
	 (printf nil "Bad data manager command %s\n"
		 msg))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(data-manager "add" "acdirect"    dm-ac-direct)
(data-manager "add" "lrdirect"    dm-lr-direct)
(data-manager "add" "lrproduct"   dm-lr-product)
(data-manager "add" "cpset"       dm-cp-set)
(data-manager "add" "ndset"       dm-nd-set)


