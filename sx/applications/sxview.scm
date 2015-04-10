; 
; SXVIEW.SCM - list the contents of a binary file
; 
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
; 

(define display-precision 1000)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (get-files x files)
    (if (pair? x)
	(let* ((file (list-ref x 0))
	       (rest (list-tail x 1)))
              (if (file? file)
		  (get-files rest (cons file files))
		  (get-files rest files)))
	(reverse files)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (get-options x opts)
    (if (pair? x)
	(let* ((file (list-ref x 0))
	       (rest (list-tail x 1)))
              (if (file? file)
		  (get-options rest opts)
		  (get-options rest (cons file opts))))
	(reverse opts)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (ls-file file opts)
   (let* ((prec (if (> (length opts) 0) (list-ref opts 0) 30))
	  (full (if (> (length opts) 1) (list-ref opts 1) #f)))

         (set! display-precision (truncate (abs (/ (- prec 1) (log 2)))))

	 (define pdb (open-bin-file file))
	 (define symbols (list-symtab pdb))
	 (define types (list-defstrs pdb))

	 (define (show-type name)
	     (let* ((dp (pdb->list (read-defstr* pdb name)))
		    (nm (list-ref dp 0))
		    (size (list-ref dp 1))
		    (kind (list-ref dp 2))
		    (algn (list-ref dp 3))
		    (memb (list-ref dp 5)))
	           (if memb
		       (print-pdb nil (read-defstr* pdb name))
		       (printf nil "   %-20s %4d %20s %4d\n"
			       nm size
			       (cond ((eqv? kind 0) "char")
				     ((eqv? kind 1) "fixed point")
				     ((eqv? kind 2) "floating point")
				     ((eqv? kind 3) "non-converting")
				     ((eqv? kind 4) "struct"))
			       algn))))

	 (define (format-dims str dims)
	     (if dims
		 (let* ((first (list-ref dims 0))
			(rest  (list-tail dims 1))
			(mn    (car first))
			(mx    (cdr first)))
		       (if (string=? str "")
			   (format-dims (sprintf "%d:%d," mn mx) rest)
			   (format-dims (sprintf "%s:%d," str mn mx) rest)))
		 (sprintf "[%s]" (substring str 0 (- (string-length str) 1)))))

	 (define (show-variable name)
	     (let* ((ep (pdb->list (read-syment pdb name)))
		    (type (list-ref ep 0))
		    (ni   (list-ref ep 1))
		    (addr (list-ref ep 2))
		    (dims (list-tail ep 3)))

	           (if dims
		       (printf nil "%-10s %-30s %8ld  @ %10ld\n"
			       type
			       (sprintf "%s%s" name (format-dims "" dims))
			       ni addr)
		       (printf nil "%-10s %-30s           @ %10ld\n" type name addr))
		   (if full
		       (begin (print-pdb nil (list (read-pdbdata pdb name) display-precision))
			      (printf nil "--------------------------------------------------------------------------\n")))))

	 (printf nil "--------------------------------------------------------------------------\n")
	 (printf nil "\n                                TYPES\n\n")
	 (printf nil "--------------------------------------------------------------------------\n")
	 (for-each show-type types)
	 (printf nil "--------------------------------------------------------------------------\n")
	 (printf nil "\n                              VARIABLES\n\n")
	 (printf nil "--------------------------------------------------------------------------\n")
	 (for-each show-variable symbols)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define-macro (binls . rest)
    (let* ((files (get-files rest nil))
	   (opts  (get-options rest nil)))

          (define-macro (do-one x)
	      (if (not (file? x))
		  (printf nil "\nCannot open file %s\n\n" x)
		  (if (ascii-file? x)
		      (printf nil "\nFile %s is ascii\n" x)
		      (ls-file x opts))))

	  (for-each do-one files)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

