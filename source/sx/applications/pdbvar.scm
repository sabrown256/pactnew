; 
; PDBVAR.SCM - interactive commands to declare and manipulate
;            - user defined variables
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

;--------------------------------------------------------------------------

;                           CHANGE AUXILLIARIES

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

(define (change-scatter cmd indl vals)
    (define (do-one x)
        (set! cmd (append (list x) cmd)))
    (set! cmd (cons indl cmd))
    (for-each do-one vals)
    (set! cmd (reverse cmd))
    (print-pdb nil (list (apply scatter-pdbdata cmd))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; -CHANGE - the auxiliary change procedure

(define (-change name val)
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
			(change-scatter commlist indl vals))
		  (begin (for-each do-one val)
			 (set! commlist (reverse commlist))
			 (print-pdb nil (list (apply write-pdbdata commlist)
					      display-precision)))))))
    #f)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; CHANGE - change values in the named variable or structure member

(define-macro (change name . val)
    "CHANGE - Reset values in a variable or member.
              If fewer values are supplied than elements referenced,
              the last value is repeated. If more values are supplied
              than elements referenced, the extra values are ignored.
              Note that the command keyword, change, may be omitted.
     Usage: [change] <expr> <values>
            <expr>       :=  <variable> | <structure-member> | <find-expr>
            <find-expr>  :=  <arr> [<predicate> [<conjunction>]]*
            <arr>        :=  a pm-array of values
            <predicate>  :=  = | != | <= | < | >= | >
            <conjuntion> :=  and | or
     Examples: change a[10,15] 3.2
                  Reset the value of a[10,15] to 3.5
               change time 0.0
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


  (-change name val))

; the change procedure
(define (change* name . val)
    "Procedure version of change macro"
    (-change name val))

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
;--------------------------------------------------------------------------

; -PRINT - the auxiliary print procedure

(define (-print nm indx)
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
				((pm-array? name)
				 (pm-array->pdbdata name))
				((pg-graph? name)
				 (pg-graph->pdbdata name))
				((pg-image? name)
				 (pg-image->pdbdata name))

; last arg is #f so index expr errors
; can be reported as such
				((file-variable? current-file name #f)
				 (print-file-variable current-file
						      (print-name name)
						      indl))
				(else
				 (printf nil "Variable %s unknown\n" name)))))
	       (if data
		   (print-pdb nil (list data display-precision)))))
	 #f))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PRINT - print all or part of the named variable or structure member

(define-macro (print name . indl)
    "PRINT - Print out all or part of a variable or member.
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
               print Mapping4.domain.elements
               print Mapping2.range.elements[1]
               a[5,10:20,1:8:3]
               print a.b[3].c[5,10:20,1:8:3]"
    (-print name indl))

; the print procedure
(define (print* name . indl)
    "Procedure version of print macro"
    (-print name indl))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; -VAR-PTR - hopefully hidden helper for VAR
;          - return a string containing all '*' from S
;          - kind of like strtok(s, "[a-z][A-Z][0-9]")

(define (-var-ptr s)
    (let* ((sl (string->list s))
	   (p  (map (lambda (x) (if (char=? x #\*) x nil)) sl)))
          (list->string p)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; VAR - define a variable in the Scheme variable space
;     - and in the VIF
;     - gives a more useful user API for variable creation

(define-macro (var xtype xname . x)
    "VAR - define a variable in the current file.
     Usage: var <type> <name> [<shape>] <vals>
     Examples: var int cn (10) 1 2 3 4 5 6 7 8 9 10
               var int *an (1 2 3 4)
               var int *en (3) (-1 -2 -3) (4 5) (6 7 8 9)
               var int **sn ((1 2 3 4 5) (6 7 8 9))"
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
           (ptrs   (-var-ptr sname))
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

;(synonym vardef   var)
;(synonym varset!  change)
;(synonym varprint print)

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
; print cn
; print an
; print en
; print sn
; 
; print c
; print a
; print e
; print s
; 
; print cs
; 
; (quit)

