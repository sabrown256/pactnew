; 
; SXCOMP.SCM - compare PDB files
; 
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
; 

(define TRUE 1)
(define true 1)
(define t 1)
(define on 1)

(define FALSE 0)
(define false 0)
(define f 0)
(define off 0)

(define integer "integer")
(define integer1 "integer1")
(define integer2 "integer2")
(define long "long")
(define long1 "long1")
(define long2 "long2")
(define float "float")
(define float1 "float1")
(define float2 "float2")
(define double "double")
(define double1 "double1")
(define double2 "double2")
(define short "short")
(define short1 "short1")
(define short2 "short2")
(define char "char")
(define char1 "char1")
(define char2 "char2")
(define type-list '(integer integer1 integer2 long long1 long2 float float1 float2
                    double double1 double2 short short1 short2 char char1 char2))

(define first #t)
(define	inputfile1 nil)
(define	inputfile2 nil)
(define transcript-on? #f)
(define promote-flag 0)
(define fselect-flag 0)
(define fselect-list nil)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (filter-entries str lst flag)
; (flag == 1) => remove from list, (flag == 2) => add to list
  (cond ((null? lst) lst)
	((not (string? str)) lst)
	(else (begin
		(let* ((strlen0 (string-length str))
		       (strlen1 (- strlen0 1))
		       (has_star (string=? "*" (substring str strlen1 strlen0)))
		       (pattern "")
		       (astring "")
		       (newlist '()))

; pat-match returns #t if there is a match, #f otherwise
	          (define (pat-match pat astr)
		    (if has_star
			(if (> strlen1 (string-length astr))
			    (set! astring astr)
			    (set! astring (substring astr 0 strlen1)))
			(set! astring astr))
;		    (printf nil "astring = %s\n" astring)
		    (if (not (string=? "/" (substring astring 0 1)))
			(if (string=? pat (string-append "/" astring))
			    #t
; check for both variables with no / at the beginning
			    (if (string=? pat astring) #t #f))
			(if (or (string=? pat astring)
                                (string=? (string-append "/" pat) astring))
			    #t
; check for directory such as "/dir1" without ending /
			    (if (string=? (string-append pat "/") astring) #t #f))))

	          (define (rm-list pat alist)
; done if alist is null, reverse newlist and end the recursion
	            (cond ((null? alist) (reverse newlist))
; if the pattern matches skip the list entry and check the next one
			  ((pat-match pat (car alist)) (rm-list pat (cdr alist)))
; if the pattern didn't match add the item to newlist then check the next one
		          (else (begin (set! newlist (cons (car alist) newlist))
;				       (printf nil "newlist = %s\n" newlist)
				       (rm-list pat (cdr alist))))))

	          (define (add-list pat alist)
; done if alist is null, reverse newlist and end the recursion
	            (cond ((null? alist) (reverse newlist))
; if the pattern doesn't match skip the list entry and check the next one
			  ((not (pat-match pat (car alist))) (add-list pat (cdr alist)))
; if the pattern matches add the item to newlist then check the next one
		          (else (begin (set! newlist (cons (car alist) newlist))
;				       (printf nil "newlist = %s\n" newlist)
				       (add-list pat (cdr alist))))))


		  (if has_star
		      (set! pattern (substring str 0 strlen1))
		      (set! pattern str))
		  (cond ((equal? flag 2) (add-list pattern lst))
                        (else       (rm-list pattern lst))))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (rm-topdir lst)
  (if (memv "/" lst)
      (if (string=? "/" (car lst))
	  (cdr lst)
	  (cons (car lst) (rm-topdir (cdr lst))))
      lst))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (add-slash lst)
  (cond ((null? lst) lst)
	((eqv? "/" (substring (car lst) 0 1)) (cons (car lst) (add-slash (cdr lst))))
	(else (cons (string-append "/" (car lst)) (add-slash (cdr lst))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

;(define (filter-entries-list vlst lst flag)
;  (cond ((null? vlst) lst)
;        ((null? lst)  lst)
;        (else (for-each (lambda (x)
;                          (filter-entries x lst)) vlst))))

;(define (filter-entries-list vlst lst flag)
;  (cond ((null? vlst) lst)
;        ((null? lst)  lst)
;        (else (for-each (lambda (x)
;                  (set! lst (filter-entries x lst flag))) vlst)))
;  lst)

(define (filter-entries-list vlst lst flag)
  (define retlist nil)
  (if (not (eqv? flag 2))
      (set! retlist (append lst retlist)))
  (cond ((null? vlst) retlist)
        ((null? lst)  retlist)
        (else (for-each (lambda (x)
                  (cond ((eqv? flag 2) (set! retlist (append retlist (filter-entries x lst flag))))
                        (else          (set! retlist (filter-entries x retlist flag))))) vlst)))
  retlist)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PDBDIFF - compare two files in batch mode
;         - return #t if there are no differences

(define-macro (pdbdiff file1 file2 . rest)

  (define retval #t)

  (if (eqv? file1 file2)
      (begin (printf nil "\nFile names are the same\n\n")
	     (set! retval #f))
      (begin

; look for precision control
	(if (not (null? rest))
	    (begin (let ((x (car rest)))
		     (if (and (real? x) (not (integer? x)))
			 (comparison-precision
			      (truncate (abs (/ (log x) (log 2)))))
			 (comparison-precision x)))
		   (set! rest (cdr rest))))

; look for individual differences display flag
	(if (not (null? rest))
	    (begin (display-individual-differences (car rest))
		   (set! rest (cdr rest))))

; look for promote flag
	(if (not (null? rest))
	    (begin (set! promote-flag (car rest))
		   (set! rest (cdr rest))))

        (define vselect-flag 0)
        (define vselect-list '())

; look for the variable selection flag
	(if (not (null? rest))
	    (begin (set! vselect-flag (car rest))
                   (set! vselect-list (cdr rest))
		   (set! rest '())))


	(if (not (file? file1))
            (begin (printf nil "\nCannot open file %s\n\n" file1)
                   (set! retval #f))
	    (if (not (file? file2))
                (begin (printf nil "\nCannot open file %s\n\n" file2)
                       (set! retval #f))
		(let* ((pdb1 (open-bin-file file1))
		       (pdb2 (open-bin-file file2))
		       (lst1 (filter-entries "/&ptrs/*" (list-symtab pdb1) 0))
		       (lst2 (filter-entries "/&ptrs/*" (list-symtab pdb2) 0))
		       (lst1 (filter-entries-list vselect-list lst1 vselect-flag))
		       (lst2 (filter-entries-list vselect-list lst2 vselect-flag))
		       (lst1 (rm-topdir lst1))
		       (lst2 (rm-topdir lst2)))

; function to compare a variable in two files
  		      (define (diff var)
			  (interactive off)
			  (let ((val (diff-variable pdb1 pdb2 var var
						    (comparison-precision)
						    (display-individual-differences)
						    promote-flag)))
; val is #t if variables compare
; #f if their contents differ
; a string describing any other difference or error condition
; encountered during the comparison
			       (cond ((string? val) 
				      (printf nil "\n%s\n" val)
				      (set! retval #f))
				     ((and (boolean? val) (not val))
				      (set! retval #f)))
			       val))

; function to compare lists of variables for commonality
		      (define (diff-list lst1 lst2)
			  (let* ((inter '())
				 (only '())
				 (len 0)
				 (chr "")
				 (xxx "")
				 (merge (lambda (x)
					  (set! len (string-length x))
					  (set! chr (substring x 0 1))
					  (set! xxx (substring x 1 len))
					  (if (memv x lst2)
					      (set! inter (cons x inter))
					      (if (eqv? "/" chr)
						  (if (memv xxx lst2)
						      (set! inter (cons x inter))
						      (set! only (cons x only)))
						  (if (memv (string-append "/" x) lst2)
						      (set! inter (cons x inter))
						      (set! only (cons x only))))))))
			    (for-each merge lst1)
			    (list only inter)))

; function to report various kinds of differences
		      (define (diff-report lst1 lst2)
			  (let* ((only1 (diff-list lst1 lst2))
				 (only2 (diff-list lst2 (cadr only1)))
				 (inter (cadr only2)))
			        (set! only1 (car only1))
				(set! only2 (car only2))
				(if (not (null? only1))
				    (begin (printf nil "\nOnly %s has:\n" file1)
					   (display only1)
					   (printf nil "\n\n")
					   (set! retval #f)))
				(if (not (null? only2))
				    (begin (printf nil "\nOnly %s has:\n" file2)
					   (display only2)
					   (printf nil "\n\n")
					   (set! retval #f)))
				(if (null? inter)
				    (printf nil "\nFiles have no variables in common\n\n")
				    (not (memq #f (map diff inter))))))

; at last compare the specified items
		      (if (equal? lst1 lst2)
			  (if (not (null? lst1))
			      (not (memq #f (map diff lst1))))
			  (diff-report lst1 lst2)))))))
    retval)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PDBCOMP - compare two files in interactive mode

(define-macro (pdbcomp file1 file2 . rest)
  (let ((doc-list nil)
	random-object)

; the start-up procedure
    (define (start-up)
      (if (and first (not (null? file1)))
	  (begin 
	    (open* file1 file2)
	    (if (not (null? rest))
		(begin (let ((x (car rest)))
			 (if (and (real? x) (not (integer? x)))
			     (comparison-precision
			          (truncate (abs (/ (log x) (log 2)))))
			     (comparison-precision x)))
		       (set! rest (cdr rest))))
	    (if (not (null? rest))
		(begin (display-individual-differences (car rest))
		       (set! rest (cdr rest))))
	    (if (not (null? rest))
		(begin (set! promote-flag (car rest))
		       (set! rest (cdr rest))))))
      (set! first #f)
      (main-loop))

; the documenter
    (define-macro (document command . rest)
      (set! doc-list (append doc-list (list (cons command rest)))))

; the reader
    (define (read-command)
      (let ((port (string->port (read-line)))
	    ret-val)
	(define (read-object prt lst)
	  (let ((x (read prt)))
	    (if (eof-object? x)
		lst
		(read-object prt (cons x lst)))))
	(set! ret-val (read-object port nil))
	(if (not (null? ret-val))
	    (reverse ret-val)
	    ret-val)))

; printable predicate
    (define (printable? file name)
      (let ((which (cond ((and (or (eqv? file 1)
				   (eqv? file 'a)
				   (eqv? file 'first))
			       (not (null? inputfile1)))
			  inputfile1)
			 ((and (or (eqv? file 2)
				   (eqv? file 'b)
				   (eqv? file 'second))
			       (not (null? inputfile2)))
			  inputfile2)
			 ((not (null? inputfile1))
			  inputfile1)
			 ((not (null? inputfile2))
			  inputfile2)
			 (#t #f))))
	(and which (file-variable? which name #t))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; the main loop
    (define (main-loop)
      (newline)
      (display "-> ")
      (set! random-object (read-command))
      (if (not (null? random-object))
	  (set! random-object
		(let* ((fp (if (> (length random-object) 1)
			       (cadr random-object)
			       nil))
		       (namep (car random-object)))
		  (if (printable? fp namep)
		      (print* namep fp)
		      (eval random-object)))))
      (if (eqv? random-object "stop")
	  #f
	  (main-loop)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; CLOSE - close current file

        (document close
                  "CLOSE      - Close the current pair of files."
                  " "
                  "Usage: close")

        (define-macro (close . args)
                (close-pdbfile inputfile1)
                (close-pdbfile inputfile2)
                (set! inputfile1 nil)
                (set! inputfile2 nil)
                #t)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; CD - change the current file directory

        (document cd
                  "CD         - Change the current file directory."
                  "             The first file is designated by 1, a, or first"
                  "             while the second file is designated by 2, b, or second."
                  "             If neither file is specified, the current file directory"
                  "             will be changed for both files."
                  " "
                  "Usage: cd [<directory-name>] [1 | a | first | 2 | b | second]"
		  " "
		  "Examples: cd mydir"
		  "          cd /zoo/mammals"
		  "          cd ../reptiles first"
		  "          cd")

	(define-macro (cd . rest)
	  (if rest
              (begin (if (eqv? (length rest) 2)
			 (let* ((which (cadr rest))
				(file (cond ((or (eqv? which 1)
						 (eqv? which 'a)
						 (eqv? which 'first))
					     inputfile1)
					    ((or (eqv? which 2)
						 (eqv? which 'b)
						 (eqv? which 'second))
					     inputfile2)
					    (else nil))))
			   (if (null? file)
			       (printf nil "\nFile not open\n")
			       (change-directory file (car rest)))))
		     (if (eqv? (length rest) 1)
			 (let* ((which (car rest))
				  (file (cond ((or (eqv? which 1)
						   (eqv? which 'a)
						   (eqv? which 'first))
					       inputfile1)
					      ((or (eqv? which 2)
						   (eqv? which 'b)
						   (eqv? which 'second))
					       inputfile2)
					      (else nil))))
			     (if (null? file)
				 (begin (change-directory inputfile1 which)
					(change-directory inputfile2 which))
				 (change-directory file)))))

	      (begin (change-directory inputfile1)
		     (change-directory inputfile2))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DESC - describe a specific variable in the current file

        (document desc 
                  "DESC       - Describe the named variable or structure member."
                  "             The first file is designated by 1, a, or first"
                  "             while the second file is designated by 2, b, or second."
                  "             To describe part of a variable or member qualify"
                  "             the name with index expressions whose parts"
                  "             are in one of the three forms:"
                  " "
                  "                 <index>"
                  "                 <index-min>:<index-max>"
                  "                 <index-min>:<index-max>:<increment>"
                  " "
                  "             Only the first form may be used to qualify"
                  "             variables or terminal structure members with"
                  "             embedded pointers and non-terminal members."
                  " "
		  "Usage: desc <variable> | <structure-member> [1 | a | first | 2 | b | second]"
                  " "
                  "Examples: desc Mapping1"
                  "          desc Mapping1.range 1"
                  "          desc baz[12:14] b")

        (define-macro (desc name which . rst)
            (if (null? name)
                (printf nil "\nNo variable name specified\n")
                (let ((file (cond ((or (eqv? which 1)
				       (eqv? which 'a)
				       (eqv? which 'first))
				   inputfile1)
				  ((or (eqv? which 2)
				       (eqv? which 'b)
				       (eqv? which 'second))
				   inputfile2)
				  (#t inputfile1))))
                  (if (null? file)
		      (printf nil "\nFile not open\n")
		      (let ((syment (read-syment file name)))
			(if (null? syment)
			    (printf nil "\nNo variable named %s\n" name)
			    (begin
			      (newline)
			      (print-pdb nil syment)
			      #t)))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DIFF - compare the named variable

        (document diff
                  "DIFF       - Compare the named variable(s) or structure member(s)."
		  "             If two variable names are provided, the values of the"
		  "             first variable in the first file will be compared with"
		  "             the values of the second variable in the second file."
		  "             Otherwise, the same variable will be compared in both"
                  "             files. To compare part of a variable or member, qualify"
                  "             the name with index expressions whose parts are in one"
                  "             of the three forms:"
                  " "
                  "                 <index>"
                  "                 <index-min>:<index-max>"
                  "                 <index-min>:<index-max>:<increment>"
                  " "
                  "             Only the first form may be used to qualify"
                  "             variables or terminal structure members with"
                  "             embedded pointers and non-terminal members."
                  " "
                  "Usage: diff <variable> | <structure-member>[<variable> | <structure-member>]"
                  " "
		  "Examples: diff Mapping3"
                  "          diff baz[12:14]"
                  "          diff foo.bar"
		  "          diff thisvar thatvar")

        (define-macro (diff var1 var2)
	  (interactive off)
	  (if (or (null? inputfile1) (null? inputfile2))
	      (printf nil "\nFile not open\n")
              (begin
                 (if (null? var2)
                     (set! var2 var1))
	         (let ((val (diff-variable inputfile1 inputfile2 var1 var2
					   (comparison-precision)
					   (display-individual-differences)
					   promote-flag)))
		   (if (string? val) (printf nil "\n%s\n" val))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; END - end PDBDiff

        (document end
                  "END        - End the session of PDBDiff."
                  " "
                  "Usage: end")

        (define-macro (end . args)
                (if (null? inputfile1)
                    #f
                    (close-pdbfile inputfile1))
                (if (null? inputfile2)
                    #f
                    (close-pdbfile inputfile2))
		"stop")

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; FILE - describe a PDB file

        (document file
                  "FILE       - Describe a PDB file."
                  "             The first file is designated by 1, a, or first"
                  "             while the second file is designated by 2, b, or second."
                  " "
                  "Usage: file [1 | a | first | 2 | b | second]"
                  " "
		  "Examples: file"
                  "          file second")

        (define-macro (file which . rst)
            (let ((file (cond ((or (eqv? which 1)
				   (eqv? which 'a)
				   (eqv? which 'first))
                               inputfile1)
                              ((or (eqv? which 2)
				   (eqv? which 'b)
				   (eqv? which 'second))
                               inputfile2)
                              (#t inputfile1))))
	      (if (null? file)
		  (printf nil "\nFile not open\n")
		  (print-pdb nil file))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; FORMAT - set the printing format for a specified data type

        (document format
		  "FORMAT     - Set the printing format for a specified data type."
		  "             If the type argument has 1 or 2 appended, then the"
		  "             specified format is applied only to arrays shorter"
		  "             than array-length or to arrays longer than or equal"
		  "             to array-length, respectively. Otherwise, the format"
		  "             applies to both. Invoking the format command with"
		  "             the single argument, default, causes the formats for"
		  "             all types to be reset to their defaults. The format"
		  "             argument must be a standard C I/O library format"
		  "             string. Double quotes are only necessary if the"
		  "             format string contains embedded blanks. See the set"
		  "             command for more about the array-length variable."
		  "             This command overrides the settings of the decimal-"
		  "             precision and bits-precision control parameters."
		  " "
		  "Usage: format integer[1 | 2] | long[1 | 2] | float[1 | 2] |"
		  "              double[1 | 2] | short[1 | 2] | char[1 | 2] <format>"
		  " "
		  "Usage: format default"
		  " "
		  "Examples: format double %12.5e"
		  "          format double2 %10.2e"
		  "          format char ''%s  ''"
		  "          format default")

	(define-macro (format type format-string)
	  (if (null? type)
	      (printf nil "\nNo type specified\n")
	      (if (eqv? type 'default)
		  (set-format 'default)
		  (if (null? format-string)
		      (printf nil "\nNo format string specified\n")
		      (if (not (memv type type-list))
			  (printf nil "\n%s is not a valid type specifier\n" type)
			      (set-format type format-string))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HELP - print help and documentation messages

        (document help
                  "HELP       - Print list of commands or command documentation."
                  " "
                  "Usage: help [<command>]"
                  " "
                  "Examples: help"
		  "          help open")

        (define-macro (help . rest)
	        (let ((command (if (null? rest) rest (car rest))))
		     (cond ((null? command)
 		            (printf nil "\nCommands : type help <command> for more information\n\n")
                            (for-each print-synopsis doc-list))
		           (else
                            (print-documentation command)))))

        (define (print-synopsis lst)
                (display (cadr lst))
                (newline))

        (define (print-documentation command)
                (let ((doc (assv command doc-list))
                      (pd (lambda (x)
                                  (display x)
                                  (newline))))
                     (if (not (pair? doc))
			 (printf nil "\n%s is not a known command\n" command)
                         (begin (newline)
                                (for-each pd (cdr doc))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; LS - list the variables in the current file directory

        (document ls
		  "LS         - List variables, links, and directories in current file directory."
		  "             A selection pattern may be specified for the terminal pathname"
		  "             node. A type qualifier may also be specified."
                  "             The first file is designated by 1, a, or first while the"
                  "             second file is designated by 2, b, or second. If neither"
                  "             file is specified, the variables will be listed for both files."
                  " "
                  "Usage: ls [<pattern> [<type> [1 | a | first | 2 | b | second]]]"
		  " "
		  "Examples: ls"
		  "          ls curve* char"
		  "          ls var?"
		  "          ls * Directory"
		  "          ls ../mydir"
		  "          ls /foo/bar double"
		  "          ls mydir short first"
		  "          ls * * second")

	(define-macro (ls . rest)
	  (if (and rest
		   (eqv? (length rest) 3))
	      (let* ((which (caddr rest))
		     (file (cond ((or (eqv? which 1)
				      (eqv? which 'a)
				      (eqv? which 'first))
				  inputfile1)
				 ((or (eqv? which 2)
				      (eqv? which 'b)
				      (eqv? which 'second))
				  inputfile2)
				 (else nil))))
		(if (null? file)
		    (printf nil "\nFile not open\n")
		    (begin
		      (if (eqv? file inputfile1)
			  (printf nil "\nFile 1 variables:\n\n")
			  (printf nil "\nFile 2 variables:\n\n"))
		      (if (eqv? (cadr rest) '*)
			  (pp (list-variables file (car rest)))
			  (pp (list-variables file (car rest) (cadr rest)))))))

	      (if (null? inputfile1)
		  (printf nil "\nFile not open\n")
		  (begin
		      (printf nil "\nFile 1 variables:\n\n")
		      (if (pair? rest)
			  (if (and (pair? (cdr rest))
				   (not (eqv? (cadr rest) '*)))
			      (pp (list-variables inputfile1 (car rest) (cadr rest)))
			      (pp (list-variables inputfile1 (car rest))))
			  (pp (list-variables inputfile1)))
		      (printf nil  "\nFile 2 variables:\n\n")
		      (if (pair? rest)
			  (if (and (pair? (cdr rest))
				   (not (eqv? (cadr rest) '*)))
			      (pp (list-variables inputfile2 (car rest) (cadr rest)))
			      (pp (list-variables inputfile2 (car rest))))
			  (pp (list-variables inputfile2)))
		      #t))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MODE - set structure display modes

        (document mode
                  "MODE       - Set the display mode."
                  "             modes full-path, indent, and tree control the"
                  "             overall printing format"
                  " "
                  "Modes: full-path   - the full path name is printed at each branch"
                  "                     e.g. foo.bar[3].baz"
                  "       indent      - indent 4 spaces at each branch (default)"
                  "       tree        - display as a tree (lines connecting branches)"
                  " "
                  "       no-type     - turns of the display of types (default)"
                  "       type        - displays the type of each item and branch"
                  " "
                  "       recursive   - indent each level of recursive structures"
                  "       iterative   - number each level of recursive structures (default)"
                  " "
                  "       individual  - display only differing elements (default)"
                  "       display-all - display all elements if any differ"
                  " "
                  "Usage: mode full-path | indent | tree | no-type | type |"
		  "            recursive | iterative | individual | display-all"
                  " "
		  "Example: mode no-type")

        (define-macro (mode type)
                (cond ((eqv? type 'full-path)
                       (set-switch 0 0))
                      ((eqv? type 'indent)
                       (set-switch 0 1))
                      ((eqv? type 'tree)
                       (set-switch 0 2))
                      ((eqv? type 'no-type)
                       (set-switch 1 0))
                      ((eqv? type 'type)
                       (set-switch 1 1))
                      ((eqv? type 'individual)
		       (display-individual-differences 1))
                      ((eqv? type 'display-all)
		       (display-individual-differences 0))
                      ((eqv? type 'recursive)
                       (set-switch 2 0))
                      ((eqv? type 'iterative)
                       (set-switch 2 1))
                      ((null? type)
                       (printf nil "\nNo print mode specified\n"))
                      (#t
                       (printf nil "\nPrint mode %s unknown\n", type))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; OPEN - open a new PDB file

        (document open
                  "OPEN       - Open a new pair of files."
                  " "
                  "Usage: open <filename1> <filename2>"
                  " "
		  "Example: open foo.s00 bar.s00")

        (define (open* file1 file2)
	    (if (or (null? file1) (null? file2))
		(printf nil "\nTwo file names must be specified\n")
		(if (not (file? file1))
		    (printf nil "\nCannot open file %s\n" file1)
		    (if (not (file? file2))
			(printf nil "\nCannot open file %s\n" file2)
			(if (eqv? file1 file2)
			    (printf nil "\nFile names are the same\n")
			    (begin
			        (set! inputfile1 (open-bin-file file1))
				(set! inputfile2 (open-bin-file file2))
				#t))))))

; the open macro
        (define-macro (open file1 file2)
	    (if (or (null? file1) (null? file2))
		(printf nil "\nTwo file names must be specified\n")
		(if (not (file? file1))
		    (printf nil "\nCannot open file %s\n" file1)
		    (if (not (file? file2))
			(printf nil "\nCannot open file %s\n" file2)
			(if (eqv? file1 file2)
			    (printf nil "\nFile names are the same\n")
			    (begin
			        (set! inputfile1 (open-bin-file file1))
				(set! inputfile2 (open-bin-file file2))
				#t))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PRINT - print all or part of the named variable or structure member

        (document print
                  "PRINT      - Print out all or part of a variable or member."
                  "             The first file is designated by 1, a, or first;"
                  "             the second file is designated by 2, b, or second."
                  "             Note that the command keyword may be omitted."
                  "             To print part of a variable or member qualify"
                  "             the name with index expressions whose parts"
                  "             are in one of the three forms:"
                  " "
                  "                 <index>"
                  "                 <index-min>:<index-max>"
                  "                 <index-min>:<index-max>:<increment>"
                  " "
                  "             Only the first form may be used to qualify"
                  "             variables or terminal structure members with"
                  "             embedded pointers and non-terminal members."
                  " "
                  "Usage: [print] <variable> | <structure-member> [1 | a | first | 2 | b | second]"
                  " "
                  "Examples: Mapping2 first"
                  "          print Mapping4.domain.elements"
                  "          print Mapping2.range.elements[1] 2"
                  "          a[5,10:20,1:8:3]"
                  "          print a.b[3].c[5,10:20,1:8:3] b")

        (define-macro (print name . rest)
	  (newline)
	  (if (null? name)
	      (printf nil "No variable name specified\n")
	      (let ((which (if (null? rest) 1 (car rest))))
		(let ((file (cond ((or (eqv? which 1)
				       (eqv? which 'a)
				       (eqv? which 'first))
				   inputfile1)
				  ((or (eqv? which 2)
				       (eqv? which 'b)
				       (eqv? which 'second))
				   inputfile2)
				  (#t inputfile1))))
		  (if (null? file)
		      (printf nil "File not open\n")
		      (print-pdb nil (read-pdbdata file name))))))
	  #f)

        (define (print* name . rest)
	  (newline)
	  (if (null? name)
	      (printf nil "No variable name specified\n")
	      (let ((which (if (null? rest) 1 (car rest))))
		(let ((file (cond ((or (eqv? which 1)
				       (eqv? which 'a)
				       (eqv? which 'first))
				   inputfile1)
				  ((or (eqv? which 2)
				       (eqv? which 'b)
				       (eqv? which 'second))
				   inputfile2)
				  (#t inputfile1))))
		  (if (null? file)
		      (printf nil "File not open\n")
		      (print-pdb nil (read-pdbdata file name))))))
	  #f)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PROMOTE - promote fixed and/or floating point variables

	(document promote
		  "PROMOTE    - Promote fixed and/or floating point variables."
		  "             Lesser precision types are promoted to higher"
		  "             precision for comparison purposes, if necessary."
		  "             Promotion is turned off for classes not specified."
		  " "
		  "Usage: promote [fixed] [float]"
		  " "
		  "Examples: promote"
		  "          promote fixed"
		  "          promote float"
		  "          promote fixed float")

	(define-macro (promote class1 class2)
	  (let ((promote-fixed #f)
		(promote-float #f)
		(flag #t))
	    (if (not (null? class1))
		(cond ((eqv? class1 'fixed)
		       (set! promote-fixed #t))
		      ((eqv? class1 'float)
		       (set! promote-float #t))
		      (#t (set! flag (printf nil "\n%s is not a valid class\n" class1)))))
	    (if (not (null? class2))
		(cond ((eqv? class2 'fixed)
		       (set! promote-fixed #t))
		      ((eqv? class2 'float)
		       (set! promote-float #t))
		      (#t (set! flag (printf nil "\n%s is not a valid class\n" class2)))))
	    (if flag
		(begin
		  (set! promote-flag 0)
		  (if promote-fixed (set! promote-flag (+ promote-flag 1)))
		  (if promote-float (set! promote-flag (+ promote-flag 2)))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PWD - print the current file directory

	(document pwd
		  "PWD        - Print the current file directory."
                  "             The first file is designated by 1, a, or first"
                  "             while the second file is designated by 2, b, or second."
                  "             If neither file is specified, the current file directory"
                  "             will be printed for both files."
 		  " "
		  "Usage: pwd [1 | a | first | 2 | b | second]"
		  " "
		  "Examples: pwd"
		  "          pwd first")

	(define-macro (pwd . rest)
	    (if rest
		(let* ((which (car rest))
		       (file (cond ((or (eqv? which 1)
					(eqv? which 'a)
					(eqv? which 'first))
				    inputfile1)
				   ((or (eqv? which 2)
					(eqv? which 'b)
					(eqv? which 'second))
				    inputfile2)
				   (else nil))))
		  (if (null? file)
		      (printf nil "File not open\n")
		      (printf nil "\n%s\n" (current-directory file))))
		(let* ((cwd1 (current-directory inputfile1))
			 (cwd2 (current-directory inputfile2)))
		    (if (not (eqv? cwd1 cwd2))
			(begin (printf nil "\nFile 1 directory: %s\n" cwd1)
			       (printf nil "\nFile 2 directory: %s\n" cwd2))
			(printf nil "\n%s\n" cwd1)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SET - set array display parameters

        (document set
                  "SET        - Set the value of a control parameter."
                  " "
                  "Parameters: line-length - the number of array elements per line"
                  "            array-length - arrays shorter than value have each"
                  "                           element labeled individually"
                  "            bits-precision - the number of mantissa bits compared"
                  "            decimal-precision - the limit on the fractional difference"
                  " "
                  "Usage: set line-length | array-length | bits-precision | decimal-precision <value>"
                  " "
		  "Examples: set line-length 3"
		  "          set array-length 20"
		  "          set bits-precision 10"
                  "          set decimal-precision 1.0e-6")

        (define-macro (set name value)
                (cond ((null? name)
                       (printf nil "\nNo display parameter name specified\n"))
                      ((null? value)
                       (printf nil "\nNo display parameter value specified\n"))
                      ((eqv? name 'array-length)
                       (set-switch 3 value))
                      ((eqv? name 'line-length)
                       (set-switch 4 value))
                      ((eqv? name 'bits-precision)
                       (comparison-precision value))
                      ((eqv? name 'decimal-precision)
                       (comparison-precision
			    (truncate (abs (/ (log value) (log 2))))))
                      (#t (printf nil "\n%s is not a valid display parameter\n" name))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; STRUCT - display the structure of a data type

        (document struct
                  "STRUCT     - Describe the named data type."
                  " "
                  "Usage: struct <data-type> [1 | a | first | 2 | b | second]"
                  " "
		  "Examples: struct double 1"
                  "          struct PM_mapping second")

	(define-macro (struct name which)
	  (newline)
	  (if (null? name)
	      (printf nil "No struct name specified\n")
	      (let ((file (cond ((or (eqv? which 1)
				     (eqv? which 'a)
				     (eqv? which 'first))
				 inputfile1)
				((or (eqv? which 2)
				     (eqv? which 'b)
				     (eqv? which 'second))
				 inputfile2)
				(#t inputfile1))))
		(if (null? file)
		    (printf nil "File not open\n")
		    (print-pdb nil (read-defstr* file name))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TRANSCRIPT - start/stop a transcript of the session

        (document transcript
                  "TRANSCRIPT - toggle a transcript of the PDBDiff session"
                  "           - the name of the transcript file can be given"
                  "           - a file named pdbdiff.trn will contain the"
                  "           - transcript of the session if no name is given"
                  " "
                  "Usage: transcript [<filename>]"
                  " "
		  "Examples: transcript"
                  "          transcript foo.bar")

        (define-macro (transcript . rst)
                (if transcript-on?
                    (begin (transcript-off)
                           (set! transcript-on? #f))
                    (begin (if (null? rst)
                               (transcript-on "pdbdiff.trn")
                               (transcript-on (car rst)))
                           (set! transcript-on? #t))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TYPES - list the data types in the current file

        (document types
                  "TYPES      - List the data types in the current file or pair of"
                  "             files. The first file is designated by 1, a, or first"
                  "             while the second file is designated by 2, b, or second."
                  "             If neither file is specified, the types will be printed"
                  "             for both files."
                  " "
                  "Usage: types [1 | a | first | 2 | b | second]"
		  " "
		  "Examples: types"
		  "          types second")

	(define-macro (types . args)
	    (if args
		(let* ((which (car args))
		       (file (cond ((or (eqv? which 1)
					(eqv? which 'a)
					(eqv? which 'first))
				    inputfile1)
				   ((or (eqv? which 2)
					(eqv? which 'b)
					(eqv? which 'second))
				    inputfile2)
				   (else nil))))
		  (if (null? file)
		      (printf nil "File not open\n")
		      (begin (if (eqv? file inputfile1)
				 (printf nil "\nFile 1 types:\n\n")
				 (printf nil "\nFile 2 types:\n\n"))
			     (display (list-defstrs file))
			     (newline))))

		(if (null? inputfile1)
		    (printf nil "\nFile not open\n")
		    (begin
			(printf nil "\nFile 1 types:\n\n")
			(display (list-defstrs inputfile1))
			(printf nil "\n\nFile 2 types:\n\n")
			(display (list-defstrs inputfile2))
			(newline)
			#t))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; set operating parameters to initial values

	(if first
	    (begin
	      (mode iterative)
	      (mode indent)
	      ; (mode no-type)
	      ;	(set line-length 4)
	      (set array-length 4)))

; run the session

	(if first
	    (banner "PDBDiff"))

	(start-up)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

