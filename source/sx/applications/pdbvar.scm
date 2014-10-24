; 
; PDBVAR.SCM - interactive commands to declare and manipulate
;            - user defined variables
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(define index-only      1)
(define index+value     2)
(define find-mode-flag  index-only)

;--------------------------------------------------------------------------

;                           CHANGE ROUTINES

;--------------------------------------------------------------------------

(define (split-off-find-expression lst fex)
    (let* ((x   (list-ref lst 0))
	   (rst (list-tail lst 1)))
          (cond ((or (and (procedure? x) (memv x (list = != <= < >= >)))
		     (memv x '(= != <= < >= >)))
		 (split-off-find-expression (cdr rst)
					    (cons (car rst) (cons x fex))))
		((memv x (list and or))
		 (split-off-find-expression rst
					    (cons x fex)))
		(else
		 (list (reverse fex) lst)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (-varset-scatter cmd indl vals)
    (define (do-one x)
        (set! cmd (append (list x) cmd)))
    (set! cmd (cons indl cmd))
    (for-each do-one vals)
    (set! cmd (reverse cmd))
    (print-pdb nil (list (apply scatter-pdbdata cmd))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; -VARSET - the auxiliary change procedure

(define (-varset name val)
    (plot-flag off)
    (newline)
    (if (null? name)
	(printf nil "No variable name specified\n")
	(if (not (file-variable? current-file name #f))
	    (printf nil "Variable %s does not exist\n" name)
	    (let* ((ep       (read-syment current-file name))
		   (var-type (syment-type ep))
		   (var-addr (syment-address ep))
		   (var-dims (syment-dimensions ep))
		   (splt     (split-off-find-expression val nil)))

	      (define commlist nil)
	      (set! commlist (append (list current-file) commlist))
	      (set! commlist (append (list name) commlist))
	      (set! commlist (append (list (append (list type var-type) var-dims))
				     commlist))
	      (define (do-one x)
		      (set! commlist (append (list x) commlist)))

	      (if (not (null? (car splt)))
		  (let* ((fexpr (list-ref splt 0))
			 (vals  (list-ref splt 1))
			 (indl  (apply find (cons "yval0" fexpr))))
			(-varset-scatter commlist indl vals))
		  (begin (for-each do-one val)
			 (set! commlist (reverse commlist))
			 (print-pdb nil (list (apply write-pdbdata commlist)
					      display-precision)))))))
    #f)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; VARSET! - change values in the named variable or structure member

(define-macro (varset! name . val)
    "VARSET! - Reset values in a variable or member.
               If fewer values are supplied than elements referenced,
               the last value is repeated. If more values are supplied
               than elements referenced, the extra values are ignored.
               Note that the command keyword, varset!, may be omitted.
     Usage: [varset!] <expr> <values>
            <expr>       :=  <variable> | <structure-member> | <find-expr>
            <find-expr>  :=  <arr> [<predicate> [<conjunction>]]*
            <arr>        :=  a pm-array of values
            <predicate>  :=  = | != | <= | < | >= | >
            <conjuntion> :=  and | or
     Examples: varset! a[10,15] 3.2
                  Reset the value of a[10,15] to 3.5
               varset! time 0.0
                  Reset the value of time to 0.0
               x[1:2] 1 2
                  Reset the values x[1] and x[2] to 1 and 2 respectively
               a[5,10,3] 5.7e4
                  Reset the value of a[5,10,3] to 5.7e4
               dir1/jkl.k 2
                  Reset the value of the k member of struct instance
                  jkl in directory dir1 to 2
               a  (1 2 3)
                 Suppose a is an array of 5 integers.  This sets a[0]
                 to 1, a[1] to 2, and a[2], a[3], and a[4] to 3.  This is
                 why you must NOT refer array elements using parentheses.
                 An expression such as:
                    a(2)
                 would be interpreted as a request to set all values of a to 2!"


  (-varset name val))

; the varset! procedure
(define (varset!* name . val)
    "Procedure version of varset! macro"
    (-varset name val))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; CHANGE-DIMENSION - change the dimensions of the named variable in memory
;                     

(define-macro (change-dimension name . rest)
    "CHANGE-DIMENSION - Change the dimensions of a variable in memory.

     Usage: change-dimension name dimension_list
     Examples: change-dimension foo 20
               change-dimension bar 10 10
               change-dimension foobar (2 . 10) (3 . 5)"

    (define (change-dim rest)
        (let* ((syment  (read-syment current-file name))
	       (symlist (pdb->list syment))
	       (address (cadr symlist))
	       (type    (car symlist))
	       (symout  (list address type name current-file)))
	  (if rest
	      (begin (set! symout (append (reverse rest) symout))
		     (set! symout (reverse symout))
		     (apply write-syment symout)
		     (file-mode current-file "r")))))

    (plot-flag off)
    (if rest
        (change-dim rest)))

;--------------------------------------------------------------------------

;                           FIND ROUTINES

;--------------------------------------------------------------------------

(define (-varfind-expr msg var expr)
    (let* ((tst (cons var expr)))
          (printf nil "%s matching:" msg)

	  (define-macro (print-cond x)
	      (printf nil " %s" x))

	  (for-each print-cond tst)
	  (printf nil "\n")))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (-varfind-index-only file var lst n)
    (if lst
	(let* ((tuple (index->tuple-string file var
					   (list-ref lst 0))))
              (printf nil "   (%s)" tuple)
	      (if (= (remainder n 4) 0)
		  (printf nil "\n"))
	      (-varfind-index-only file var (cdr lst) (+ n 1)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (-varfind-index file var expr ind n)
    (if lst
	(cond ((eqv? find-mode-flag index+value)
	       (-varfind-expr "Values" var expr)
	       (varprint* var ind))
	      (else
	       (-varfind-expr "Indices of values" var expr)
	       (printf nil "\n")
	       (-varfind-index-only file var (pm-array->list ind) n)
	       (printf nil "\n")))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (-varfind-and var arr prd val rst indl)
    (-varfind var arr rst
	      (find-index arr prd val indl)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (-varfind-or var arr prd val rst indl)
    (let* ((aid (find-index arr prd val indl))
	   (bid (-varfind var arr rst))
	   (ai  (pm-array->list aid))
	   (bi  (pm-array->list bid)))
      (apply list->pm-array
	     (append ai bi))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (-varfind var arr expr indl)
    (let* ((nargs (length expr)))
          (if (and expr (= (remainder nargs 3) 2))
	      (cond ((= nargs 2)
		     (let* ((fnc (list-ref expr 0))
			    (prd (if (procedure? fnc) fnc (eval fnc)))
			    (val (list-ref expr 1))
			    (oid (find-index arr prd val indl)))
		       oid))
		    (else
		     (let* ((fnc (list-ref expr 0))
			    (prd (if (procedure? fnc) fnc (eval fnc)))
			    (val (list-ref expr 1))
			    (cnj (list-ref expr 2))
			    (rst (list-tail expr 3)))
		       (cond ((eqv? cnj 'and)
			      (-varfind-and var arr prd val rst indl))
			     ((eqv? cnj 'or)
			      (-varfind-or var arr prd val rst indl))
			     (else
			      (printf nil
				      "Bad search criteria: %s\n"
				      expr)))))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (find-mode . val)
    (if val
	(set! find-mode-flag (car val))
	find-mode-flag)
    (if (interactive?)
        (printf nil "   %s\n" find-mode-flag))
    find-mode-flag)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; VARFIND - find indices of arrays for which values match specified criteria

(define-macro (varfind var . expr)
    "VARFIND - find indices of arrays for which values match
            specified criteria
     Usage: varfind <arr> [<predicate> # [<conjunction>]]*
            <arr>        :=  a pm-array of values
            <predicate>  :=  = | != | <= | < | >= | >
            <conjuntion> :=  and | or
     Examples: varfind foo < 3.1
               varfind foo < 3.1 and > -1.2 and != 0"

    (let* ((arr (pdbdata->pm-array (read-pdbdata current-file var)))
	   (dms (variable-dimensions var))
	   (oid (-varfind var arr expr nil)))

          (if (interactive?)
	      (begin (-varfind-index current-file var expr oid 1)
		     (printf nil "\n")))
	  oid))

;--------------------------------------------------------------------------

;                         VARPRINT ROUTINES

;--------------------------------------------------------------------------

; -VARPRINT-INDEX-RANGE-EXPR - scan the string NAME for patterns like
;                            - .*[d]*:[d]*.*
;                            - where .* means match zero or more characters
;                            - and [d]* means match one or more digits
;                            - return a list consisting of
;                            - ( <the number corresponding to the first [d]*>
;                            -   <the match for the first .*>
;                            -   <the number corresponding to the second [d]*>
;                            -   <the match for the second .*> )

(define (-varprint-index-range-expr name)
    (let* ((nm (sprintf name))
	   (sa (strtok nm ":"))
           (sinc (strchr nm ":"))
	   (sb (if (string? sinc)
                   (strtok nm ":")
                   (strtok nm "\n")))
           (si (if (string? sinc)
                   (strtok nm "\n")
                   1)))
          (if (string=? sa name)
	      (list 0 name 0 "" si)
              (if (string? sinc)
                  (append (strip-last-token sa)
                          (list (string->number sb) (cadr (strip-first-token si))
                                   (car (strip-first-token si))))
                  (append (strip-last-token sa)
		          (list (car (strip-first-token sb)) 
                                (cadr (strip-first-token sb)) si))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; -VARPRINT-INDEX-RANGE - reduce non-terminal index range to loop over
;                   - single values and recurse on the printing of
;                   - the reduced expression

(define (-varprint-index-range file pa imn i imx pb inc tm indl)
   (if (and (<= imn i) (<= i imx))
       (let* ((name (sprintf "%s%d%s%s"
			     pa i
			     (if pb pb "")
			     (if tm tm ""))))
	     (-varprint-file-variable file name indl)
	     (-varprint-index-range file pa imn (+ i inc) imx pb inc tm))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; -VARPRINT-FILE-VARIABLE - recursively handle the printing of file variable
;                     - specification with non-terminal index ranges
;                     - which PDBLib will not handle

(define (-varprint-file-variable file name indl)
    (let* ((nm      (split-name-at-terminal name))
           (nosplit (string=? name (car nm)))
	   (nt      (car nm))
	   (tm      (cdr nm))
	   (wrk     (if nosplit
; if nosplit then generate a dummy list to handle the x[:] case
                       (list 0 1 2 3 4)
                       (-varprint-index-range-expr nt)))
	   (imn     (list-ref wrk 0))
	   (pa      (list-ref wrk 1))
	   (imx     (list-ref wrk 2))
	   (pb      (list-ref wrk 3))
           (inc     (list-ref wrk 4)))
          (if (or nosplit (= imn imx))
	      (let* ((data (read-pdbdata file name)))
		    (if data
			(print-pdb nil (list data display-precision indl))))
	      (-varprint-index-range file pa imn imn imx pb inc tm indl))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; -VARPRINT - the auxiliary varprint procedure

(define (-varprint nm indx)
    (let* ((name (sprintf "%s%s%s" (che) nm (cta)))
	   (indl (if indx (car indx) nil)))
      
         (plot-flag off)
	 (newline)
	 (if (null? name)
	     (printf nil "No variable name specified\n")
	     (let* ((data (cond ((pdbdata? name)
				 name)
				((pm-mapping? name)
				 (pm-mapping->pdbdata name))
				((pm-set? name)
				 (pm-set->pdbdata name))
				((c-array? name)
				 (pm-array->pdbdata name))
				((pg-graph? name)
				 (pg-graph->pdbdata name))
				((pg-image? name)
				 (pg-image->pdbdata name))

; last arg is #f so index expr errors
; can be reported as such
				((file-variable? current-file name #f)
				 (-varprint-file-variable current-file
						      (print-name name)
						      indl))
				(else
				 (printf nil "Variable %s unknown\n" name)))))
	       (if data
		   (print-pdb nil (list data display-precision)))))
	 #f))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; VARPRINT - print all or part of the named variable or structure member

(define-macro (varprint name . indl)
    "VARPRINT - Print out all or part of a variable or member.
                Note that the command keyword may be omitted.
                To print part of a variable or member qualify
                the name with index expressions whose parts
                are in one of the three forms:
                  <index>
                  <index-min>:<index-max>
                  <index-min>:<index-max>:<increment>
                Only the first form may be used to qualify
                variables or terminal structure members with
                embedded pointers and non-terminal members.
     Usage: [print] <variable> | <structure-member>
     Examples: Mapping2
               varprint Mapping4.domain.elements
               varprint Mapping2.range.elements[1]
               a[5,10:20,1:8:3]
               varprint a.b[3].c[5,10:20,1:8:3]"
    (-varprint name indl))

; the varprint procedure
(define (varprint* name . indl)
    "Procedure version of varprint macro"
    (-varprint name indl))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; -VARDEF-PTR - hopefully hidden helper for VARDEF
;             - return a string containing all '*' from S
;             - kind of like strtok(s, "[a-z][A-Z][0-9]")

(define (-vardef-ptr s)
    (let* ((sl (string->list s))
	   (p  (map (lambda (x) (if (char=? x #\*) x nil)) sl)))
          (list->string p)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; VARDEF - define a variable in the Scheme variable space
;        - and in the VIF
;        - gives a more useful user API for variable creation

(define-macro (vardef xtype xname . x)
    "VARDEF - define a variable in the current file.
     Usage: [vardef] <type> <name> [<shape>] <vals>
     Examples: vardef int cn (10) 1 2 3 4 5 6 7 8 9 10
               int *an (1 2 3 4)
               int *en(3) (-1 -2 -3) (4 5) (6 7 8 9)
               int **sn ((1 2 3 4 5) (6 7 8 9))"
    (let* ((n      (length x))
	   (stype  (sprintf "%s" xtype))
	   (sname  (sprintf "%s" xname))
	   (x1     (list-ref x 0))
	   (vshape (if (> n 1) x1 nil))
           (ni     (if vshape (apply * vshape) 1))
	   (vals   (if (null? vshape)
		       (if (> n 1)
			   (list-tail x 1)
			   (list x1))
		       (if (> n 1)
			   (list-tail x 1)
			   x1)))
           (ptrs   (-vardef-ptr sname))
	   (vname  (strtok sname "*"))
	   (vtype  (if (string=? ptrs "")
		       stype
		       (sprintf "%s %s" stype ptrs)))
           (vvar   (string->symbol vname))
	   (typd (append (list type vtype) vshape))
	   (argl (append (list current-file vname typd) vals))
	   (dp   (apply write-pdbdata argl))
	   (v    (apply define-global (list vvar dp))))
;(printf nil "-----------\n")
;(printf nil "-> %s\n" (eval vvar))
;(printf nil "-> %s\n" (pdb->list dp))
;(printf nil "-----------\n")
	  v))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(synonym vardef   var)
(synonym varfind  find)
(synonym varprint print)
(synonym varset!  change)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; (vardef short cs (10) 1 2 3 4 5 6 7 8 9 10)
; 
; (vardef int cn (10) 1 2 3 4 5 6 7 8 9 10)
; (vardef int *an (1 2 3 4))
; (vardef int *en (3) (-1 -2 -3) (4 5) (6 7 8 9))
; (vardef int **sn ((1 2 3 4 5) (6 7 8 9)))
; 
; (vardef char c (10) "bar")
; (vardef char *a "foo")
; (vardef char *e (3) "Foo" () "Bar")
; (vardef char **s ("Hello" "world"))
; 
; ls
; varprint cn
; varprint an
; varprint en
; varprint sn
; 
; varprint c
; varprint a
; varprint e
; varprint s
; 
; varprint cs
; 
; (quit)

