;
; NETCDF.SCM - the NETCDF spoke (SCHEME level only spoke)
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
; 

(define NETCDF-magic-number 1128547841)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DEF-NETCDF-TYPES

;(define (def-netcdf-types file)
;   (write-defstr* file "byte"))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SCALAR-VALUE

(define (scalar-value fp type addr)
   (let* ((rval (read-binary fp addr 1 type)))
     (if rval
	 (cddr (pdb->list rval)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; N-PAD-BYTES

(define (n-pad-bytes nc)
   (let* ((rem (remainder nc 4)))
     (if (= rem 0)
	 0
	 (- 4 rem))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; OPEN-NETCDF-FILE

(define-macro (open-netcdf-file name mode)
   (cond ((eqv? mode "r")
	  (open-netcdf-file-for-reading name mode))
	 ((eqv? mode "w")
	  (create-netcdf-file name mode))
	 (else (open-netcdf-file-for-reading name mode))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; OPEN-NETCDF-FILE-FOR-READING

(define (open-netcdf-file-for-reading name mode)
   (if (file? name)
       (let* ((nf (open-raw-binary-file name mode "NETCDF" x86-64-std byte-algn)))
	     (if nf
		 (let* ((magic (scalar-value nf "long" SEEK-SET))
			(numrecs (scalar-value nf "long" SEEK-CUR)))
	               (major-order nf column-major)
		       (default-offset nf 1)

		       (if (and (number? numrecs)
				(= magic NETCDF-magic-number))
			   (open-netcdf-worker nf numrecs)
			   (close-netcdf-file nf)))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; OPEN-NETCDF-WORKER

(define (open-netcdf-worker nf nr)
   (let* ((symtab (car (pdb->list nf)))
	  (chart  (cadr (pdb->list nf)))
	  (dims  (cdr (read-NC-array nf SEEK-CUR)))
	  (attrs (cdr (read-NC-array nf SEEK-CUR)))
	  (vars  (cdr (read-NC-array nf SEEK-CUR)))
	  (pdb-attrs (list-attributes nf)))

; dimension processor
     (define (process-dim x)
       (let* ((dval (NC-dimension-value (list-ref dims x))))
	 (if (= dval 0)
	     nr
	     dval)))

; dimension list processor
     (define (process-dims vardims)
       (if vardims
	   (map process-dim vardims)
	   ()))

; symbol table entry processor
     (define (map-netcdf-var x)
       (let* ((var-name  (NC-variable-name x))
	      (var-type  (NC-variable-type x))
	      (var-attrs (NC-variable-attributes x))
	      (address   (NC-variable-address x))
	      (var-dims  (process-dims (NC-variable-dimensions x)))
	      (syment    (append (list nf var-name var-type address)
				 var-dims)))
	 (define (attribute x)
	   (let* ((attr  (NC-attribute-name x))
		  (type  (NC-attribute-type x))
		  (value (NC-attribute-value x))
		  (exist (memv attr pdb-attrs)))
	     (if (not exist)
		 (begin
		   (def-attribute nf attr type)
		   (set! pdb-attrs (list-attributes nf))))
	     (set-attribute-value! nf var-name attr value)))

	 (if var-attrs
	     (for-each attribute var-attrs))
	 (apply write-syment syment)))

; attribute processor
     (define (map-netcdf-attr x)
       (let* ((attr-name (NC-attribute-name x))
	      (attr-type (NC-attribute-type x))
	      (attr-value (NC-attribute-value x)))
	 (def-attribute nf attr-name attr-type)
	 (set-attribute-value! nf "global" attr-name attr-value)))

; map the global attributes
     (for-each map-netcdf-attr attrs)
     (set! pdb-attrs (list-attributes nf))

; map the symbol table entries
     (for-each map-netcdf-var vars)

     nf))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; CREATE-NETCDF-FILE

(define (create-netcdf-file name mode)
   nil)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; CLOSE-NETCDF-FILE

(define (close-netcdf-file nf)
   (if (eqv? (file-info nf "mode") READ-MODE)
       (close-raw-binary-file nf)
       ()))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define NETCDF-functions
    '(("close-file"  close-netcdf-file)
      ("desc"        desc-pdb)
      ("ls"          ls-pdb)
      ("n-entries"   n-entries-pdb)
      ("print"       varprint)))

(install-file-option "SCHEME-coded" open-netcdf-file "NETCDF" NETCDF-functions)

;--------------------------------------------------------------------------

;                   ROUTINES TO READ NETCDF STRUCTURES

;--------------------------------------------------------------------------

(define (read-NC-primitive-array file addr)
   "Procedure: Read an NC_array of primtive types. A list of data items
      is returned.
      Usage: (read-NC-primitive-array <file> <addr>)"
   (let* ((index (scalar-value file "long" addr))
	  (type (NC-enum-type-name index))
	  (ni (scalar-value file "long" SEEK-CUR))
	  (rtyp (if (and (= index 2) (> ni 1))
		    "char *"
		    type))
	  (ret (list rtyp
		     (if (and (< index 7) (< 0 index))
			 (cddr (pdb->list (read-binary file
						       SEEK-CUR
						       ni type)))))))
     (if (or (= index 1) (= index 2))
	 (let* ((nr (n-pad-bytes ni)))
	   (if (> nr 0)
	       (read-binary file SEEK-CUR nr "char"))))
     ret))

(define (read-NC-item file index nrem ird)
   "Procedure: Iteratively read non-primitive items in an NC_array.
      Usage: (read-NC-item <file> <type-index> <n-remaining> <list>)
      Each item read is consed onto <list>"
   (if (< 0 nrem)
       (let* ((fnc (NC-enum-type-rd index))
	      (type (NC-enum-type-name index)))
	 (set! ird (cons (fnc file SEEK-CUR) ird))
	 (read-NC-item file index (- nrem 1) ird))
       ird))

(define (read-NC-array file addr)
   "Procedure: Read an NC_array of netCDF data. A list of data items
      is returned.
      Usage: (read-NC-array <file> <addr>)"
   (let* ((index (scalar-value file "integer" addr))
	  (type (NC-enum-type-name index))
	  (ni (scalar-value file "long" SEEK-CUR)))
     (if (> ni 0)
	 (cons type
	       (if (and (< index 7) (< 0 index))
		   (cddr (pdb->list (read-binary file SEEK-CUR
						 ni type)))
		   (read-NC-item file index ni nil)))
	 ())))

(define (read-NC-string file addr)
   "Procedure: Read an NC_string. A string is returned.
      Positions file pointer to next 4 byte word boundary.
      Usage: (read-NC-string <file> <addr>)"
   (let* ((nc (scalar-value file "long" addr))
	  (nr (n-pad-bytes nc))
	  (ret (cddr (pdb->list (read-binary file
					     SEEK-CUR nc "char")))))
     (if (> nr 0)
	 (read-binary file SEEK-CUR nr "char"))
     ret))

(define (read-NC-iarray file addr)
   "Procedure: Read an NC_iarray. A list of integers is returned.
      Usage: (read-NC-iarray <file> <addr>)"
   (let* ((ni (scalar-value file "long" addr)))
     (if (> ni 0)
	 (cddr (pdb->list (read-binary file
				       SEEK-CUR
				       ni "integer")))
	 ())))

(define (read-NC-dim file addr)
   "Procedure: Read an NC_dim. A list of containing the dimension
      name and value is returned.
      Usage: (read-NC-dim <file> <addr>)"
   (list (read-NC-string file addr)
	 (scalar-value file "long" SEEK-CUR)))

(define (read-NC-attr file addr)
   "Procedure: Read an NC_attr. A list of containing the attribute
      name and values is returned.
      Usage: (read-NC-attr <file> <addr>)"
   (cons (read-NC-string file addr)
	 (read-NC-primitive-array file SEEK-CUR)))

(define (read-NC-var file addr)
   "Procedure: Read an NC_var. A list of containing the variable
      name, dimension list, attribute list, byte length, and disk
      address is returned.
      Usage: (read-NC-var <file> <addr>)"
   (list (read-NC-string file addr)               ; name
	 (read-NC-iarray file SEEK-CUR)           ; dimensions
	 (read-NC-array file SEEK-CUR)            ; attributes
	 (NC-enum-type-name
	    (scalar-value file "long" SEEK-CUR))  ; type index
	 (scalar-value file "long" SEEK-CUR)      ; byte length
	 (scalar-value file "long" SEEK-CUR)))    ; disk address

(define (error-type file addr)
   (printf nil "Bad netCDF type\n"))

;--------------------------------------------------------------------------

;                   ROUTINES TO WRITE NETCDF STRUCTURES

;--------------------------------------------------------------------------

(define (write-NC-primitive-array file data)
   "Procedure: Write an NC_array of primtive types.
      Usage: (write-NC-primitive-array <file> <addr>)"
)

(define (write-NC-item file index nrem ird)
   "Procedure: Iteratively write non-primitive items in an NC_array.
      Usage: (write-NC-item <file> <type-index> <n-remaining> <list>)
      Each item write is consed onto <list>"
)

(define (write-NC-array file addr)
   "Procedure: Write an NC_array of netCDF data. A list of data items
      is returned.
      Usage: (write-NC-array <file> <addr>)"
)

(define (write-NC-string file addr)
   "Procedure: Write an NC_string. A string is returned.
      Usage: (write-NC-string <file> <addr>)"
)

(define (write-NC-iarray file addr)
   "Procedure: Write an NC_iarray. A list of integers is returned.
      Usage: (write-NC-iarray <file> <addr>)"
)

(define (write-NC-dim file addr)
   "Procedure: Write an NC_dim. A list of containing the dimension
      name and value is returned.
      Usage: (write-NC-dim <file> <addr>)"
)

(define (write-NC-attr file addr)
   "Procedure: Write an NC_attr. A list of containing the attribute
      name and values is returned.
      Usage: (write-NC-attr <file> <addr>)"
)

(define (write-NC-var file addr)
   "Procedure: Write an NC_var. A list of containing the variable
      name, dimension list, attribute list, byte length, and disk
      address is returned.
      Usage: (write-NC-var <file> <addr>)"
)

;--------------------------------------------------------------------------

;                          NETCDF TYPE HANDLERS

;--------------------------------------------------------------------------

(define (error-type file addr)
   (printf nil "Bad netCDF type\n"))

(define NC-enum-types 
    (list (list "void"        error-type          error-type)
	  (list "byte"        nil                 nil)
	  (list "char"        nil                 nil)
	  (list "short"       nil                 nil)
	  (list "long"        nil                 nil)
	  (list "float"       nil                 nil)
	  (list "double"      nil                 nil)
          (list "NC-bitfield" error-type          error-type)
	  (list "NC-string"   read-NC-string      write-NC-string)
	  (list "NC-iarray"   read-NC-iarray      write-NC-iarray)
	  (list "NC-dim"      read-NC-dim         write-NC-dim)
	  (list "NC-var"      read-NC-var         write-NC-var)
	  (list "NC-attr"     read-NC-attr        write-NC-attr)))

(define (NC-enum-type-name x)
   (car (list-ref NC-enum-types x)))

(define (NC-enum-type-rd x)
   (cadr (list-ref NC-enum-types x)))

(define (NC-enum-type-wr x)
   (caddr (list-ref NC-enum-types x)))

;
; NC-ATTR accessors
;

(define (NC-attribute-name x)
   (list-ref x 0))

(define (NC-attribute-type x)
   (list-ref x 1))

(define (NC-attribute-value x)
   (list-ref x 2))

;
; NC-DIMS accessors
;

(define (NC-dimension-name x)
   (list-ref x 0))

(define (NC-dimension-value x)
   (list-ref x 1))

;
; NC-VAR accessors
;

(define (NC-variable-name x)
   (list-ref x 0))

(define (NC-variable-dimensions x)
   (let* ((dlst (list-ref x 1)))
     (if dlst
	 (cond ((vector? dlst)
		(vector->list dlst))
	       ((pair? dlst)
		dlst)
	       ((number? dlst)
		(list dlst))
	       (else
		())))))

(define (NC-variable-attributes x)
   (let* ((alst (list-ref x 2)))
     (if alst
	 (cdr alst))))

(define (NC-variable-type x)
   (list-ref x 3))

(define (NC-variable-byte-length x)
   (list-ref x 4))

(define (NC-variable-address x)
   (list-ref x 5))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

