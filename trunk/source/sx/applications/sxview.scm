; 
; SXVIEW.SCM - List the contents of a binary file as a batch mode process.
; 
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
; 

(define display-precision 1000)

(define-macro (binls . rest)
  (define file (car rest))
  (if (not (file? file))
      (printf nil "\nCannot open file %s\n\n" file)
      (if (ascii-file? file)
	  (printf nil "\nFile %s is ascii\n" file)
	  (begin
	    (set! rest (cdr rest))
	    (if (not (null? rest))
		(set! display-precision (truncate (abs (/ (- (car rest) 1) (log 2))))))

	    (define pdb (open-bin-file file))
	    (define symbols (list-symtab pdb))
	    (define types (list-defstrs pdb))

	    (define (show-type name)
	      (printf nil "--------------------------------------------------------------------------\n\n")
	      (print-pdb nil (read-defstr* pdb name))
	      (printf nil "--------------------------------------------------------------------------\n"))

	    (define (show-variable name)
	      (printf nil "--------------------------------------------------------------------------\n\n")
	      ;	    (printf nil "Variable: ")
	      (display name)
	      (printf nil "\n\n")
	      (print-pdb nil (read-syment pdb name))
	      (print-pdb nil (list (read-pdbdata pdb name) display-precision))
	      (printf nil "--------------------------------------------------------------------------\n"))

	    (printf nil "--------------------------------------------------------------------------\n")
	    (printf nil "\n                                TYPES\n\n")
	    (for-each show-type types)
	    (printf nil "\n                              VARIABLES\n\n")
	    (for-each show-variable symbols)
	    (printf nil "--------------------------------------------------------------------------\n")))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

