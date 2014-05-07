; 
; PDBVIO.SCM - SCHEME level file I/O for C level PDBView
; 
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
; 

(define file-manager-table #f)
(define stream-number 0)
(define iofile-options nil)
(define iofuncs-list nil)

(define WRITE-MODE  1)
(define APPEND-MODE 2)
(define READ-MODE   3)

(define SEEK-SET -1)
(define SEEK-CUR -2)
(define SEEK-END -3)

(define integer  "integer")
(define integer1 "integer1")
(define integer2 "integer2")
(define long     "long")
(define long1    "long1")
(define long2    "long2")
(define float    "float")
(define float1   "float1")
(define float2   "float2")
(define double   "double")
(define double1  "double1")
(define double2  "double2")
(define short    "short")
(define short1   "short1")
(define short2   "short2")
(define char     "char")
(define char1    "char1")
(define char2    "char2")
;(define char2    "user-int")
(define user-int "user-int")
(define suppress-member "suppress-member")
(define type-list '(integer integer1 integer2 long long1 long2 float float1 float2
                    double double1 double2 short short1 short2 char char1 char2
		    user-int suppress-member))

(define row-major    101)
(define column-major 102)

(define display-precision 1000)

(define literal 1)
(define nopadding 2)

(define zone -1)
(define node -2)
(define face -3)

(define (no-op . lst) #t)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; STREAM Structure - (name alias mode file type type-alias)

(define (stream-name stream)       (list-ref stream 0))
(define (stream-alias stream)      (list-ref stream 1))
(define (stream-mode stream)       (list-ref stream 2))
(define (stream-file stream)       (list-ref stream 3))
(define (stream-type stream)       (list-ref stream 4))
(define (stream-type-alias stream) (list-ref stream 5))
(define (stream-functions stream)  (list-ref stream 6))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MAKE-STREAM - stream constructor

(define (make-stream name alias mode file type type-alias funcs)
    (if type-alias
        (list name alias mode file type type-alias funcs)
        (list name alias mode file type type funcs)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; FILE-REFERENCE - return a stream which matches the given name
;                - by type, name, or any other property

(define (file-reference x lst . opt)

    (define (is-file? x files)
        (if files
	    (if (this-file? x (car files))
		(car files)
		(is-file? x (cdr files)))
	    nil))

    (let* ((name (print-name x))
	   (ref  (is-file? name lst)))
          (if (and (null? ref) (pair? opt))
	      (let* ((thsf   current-file)
		     (order  (major-order thsf))
		     (offset (default-offset thsf))
		     (newf   (apply change-file* (cons name opt))))

; setup the new file as the original file is
		    (major-order current-file order)
		    (default-offset current-file offset)

; change back to the original file
		    (change-file* thsf)

		    (file-reference newf stream-list))

	      ref)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GET-FILE-OPTION - return the function associated with the specified
;                 - file option

(define (get-file-option x)
    (let* ((option (assv x iofile-options)))
          (if option (cadr option) nil)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; INSTALL-FILE-OPTION - keep a list of file types and how to open them

(define (install-file-option tag func type procs)
    (if iofile-options
	(let* ((entry (assv tag iofile-options))
	       (options (if entry (cdr entry) ())))
	      (if entry
		  (if (not (memv func options))
		      (set-cdr! entry (cons func options)))
		  (set! iofile-options (cons (list tag func) iofile-options))))
	(set! iofile-options (list (list tag func))))

    (if (not (assv type iofuncs-list))
	(set! iofuncs-list (cons (cons type procs) iofuncs-list))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; IO-FUNCTION - return the io function associated with the argument

(define (io-function fp x)
    (let* ((proc (assv x
		       (cdr (stream-functions
			     (file-reference fp stream-list))))))
          (if proc
	      (eval (cadr proc))
	      no-op)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; NEXT-ALIAS - return the next default alias

(define (next-alias)
    (set! stream-number (+ stream-number 1))
    (sprintf "f%d" stream-number))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PRINTABLE? - determine if token is printable data from file entry

(define (printable? name)
    "Procedure: determine is NAME is printable data from file entry.
     Usage: (printable? <name>)"
    (file-variable? current-file name #t))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SCHEME-SPOKE-OPEN - open a file using a SCHEME level spoke

(define (scheme-spoke-open file mode)
    (let* ((scheme-spokes (cdar iofile-options)))
	
      (define (attempt-open file func)
	(if func
	    (apply func (list file mode))))
    
      (define (try-spokes file lst)
	(if lst
	    (let* ((fp (attempt-open file (car lst))))
	      (if fp
		  fp
		  (try-spokes file (cdr lst))))))

      (try-spokes file scheme-spokes)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; THIS-FILE? - does the thing match the stream name, alias, or file?

(define (this-file? x file)
    (if (pair? x)
	(equal? x file)
	(memv x file)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; FM-ADD - the file manager add command

(define (fm-add name fnc)
    (hash-install name fnc file-manager-table))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; FM-CLOSE - handle "fclose" commands

(define (fm-close file)
    (let* ((name (file-reference file stream-list))
	   (fp   (stream-file name)))
      (if (and name (not (eqv? (stream-name name) "virtual-internal")))
	  (begin
	    ((io-function fp "close-file") fp)
	    (set! stream-list
		  (splice-out name stream-list this-file?))))
      nil))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; FM-FILES - handle "files" commands

(define (fm-files . args)

   (define (print-file x)
       (if (this-file? current-file x)
	   (printf nil "  * ")
	   (printf nil "    "))
       (printf nil "%s  \"%s\"  %s  \t%s\n"
	       (stream-alias x)
	       (stream-mode x)
	       (stream-type-alias x)
	       (stream-name x)))

   (if stream-list
       (begin
	   (printf nil "\n")
	   (set! stream-list (reverse stream-list))
	   (for-each print-file stream-list)
	   (set! stream-list (reverse stream-list))))
   #f)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; FM-MATCH - handle "fmatch" commands

(define (fm-match f1 f2)
   (let* ((s1 (stream-name (file-reference f1 stream-list)))
	  (s2 (stream-name (file-reference f2 stream-list))))
     (and s1 s2 (eqv? s1 s2))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; FM-NAMED - handle "fnamed" commands

(define (fm-named f)
   (let* ((s (stream-file (file-reference f stream-list))))
     (if s s #f)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; FM-OPEN - open a file

(define (fm-open name mode type alias)
    (let* ((strm (file-reference name stream-list))
	   (fp   (stream-file strm)))

; if not on the list, open it
      (if (not fp)
	  (begin
	    (if (eqv? mode "w")
		(if (get-file-option type)
		    (set! fp ((get-file-option type) name mode))
		    (printf nil "\nNo output spoke for type %s\n" type))
		(if (file? name)
		    (begin
		      (if (not (file? name mode))
			  (set! mode "r"))
		      (set! fp

; try C coded first to optimize performance by minimizing opens
; which is crucial for inferior HPC machines such as BG/P
			    (or (open-bin-file name mode)
				(scheme-spoke-open name mode)
                                (printf nil
					"\nFile %s of unknown format\n"
					name))))
		    (if (and name (not (eqv? name "f0")))
			(printf nil "\nFile %s not found\n" name))))
	    (if fp
		(let* ((funcs (assv (file-type fp) iofuncs-list)))
		  (if funcs
		      (begin
			(set! strm
			      (make-stream name
					   (if alias alias (next-alias))
					   mode fp
					   (file-type fp) nil funcs))
			(set! stream-list (cons strm stream-list)))
		      (begin
			(set! fp nil)
			(printf nil
				"\nFile %s not found, access denied or\n"
				name)
			(printf nil
				"spoke for file type %s not loaded\n"
				(file-type fp))))))))
      fp))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; FM-REPLICATE - replicate a drawable in another file
;              - this is the freplicate method
;              - form: (fm-replicate (src infile outfile (mappings ...)))
;              - if infile is nil 

(define (fm-replicate . args)
    (if args
	(let* ((src     (list-ref args 0))
	       (infile  (list-ref args 1))
	       (outfile (list-ref args 2))
	       (things  (list-ref args 3)))

	  (if things
	      (let* ((fp (stream-file (file-reference outfile stream-list)))
		     (fc current-file))

; if the file hasn't been opened, open it now
		(if (not fp)
		    (begin
		      (if (file? outfile)
			  (change-file* outfile "a")
			  (change-file* outfile "w"))
		      (set! fp
			    (stream-file (file-reference outfile stream-list)))))

		(define (rep-file-out type item)
		    (cond ((eqv? type "curve")
			   ((io-function fp "curve-out") item fp)
			   #t)
			  ((pg-graph? item)
			   ((io-function fp "map-out") item fp)
			   #t)
			  ((pg-image? item)
			   ((io-function fp "image-out") item fp)
			   #t)))

		(define (rep-file-one x)
		  (let* ((type (pg-menu-item-type infile x))
			 (item (if (eqv? type "image")
				   ((io-function infile "image-in")
				    infile x)
				   ((io-function infile "map-in")
				    infile x))))
		    (rep-file-out type item)))

		(define (rep-vp-one x)
		  (let* ((things (window-manager "vget-args" x)))
		        (if (pair? (car things))
			    (rep-file-out (caar things) (cdar things)))))

		(define (cm-seq fnc x)
		  (if (fnc x)
		      (cm-seq fnc (+ x 1))))

		(if (equal? (file-type fp) "PDBfile")
		    (pg-def-graph-file fp))

		(cond ((eqv? src "file")
		       (if (equal? (car things) '*)
			   (cm-seq rep-file-one 1)
			   (for-each rep-file-one things)))

		      ((eqv? src "viewport")
		       (if (equal? (car things) '*)
			   (cm-seq rep-vp-one 1)
			   (for-each rep-vp-one things))))

		(change-file* fc))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; CD-PDB - change the current file directory

(define (cd-pdb . rest)
    (plot-flag off)
    (apply change-directory rest))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DATA-IMAGE - wrap a PDBDATA around an image

;(define (data-image file x)
;  (if (number? x)
;      (read-pdbdata file (pg-image-name file x))
;      (read-pdbdata file x)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DATA-MAPPING - wrap a PDBDATA around a mapping

(define (data-mapping file x)
  (if (number? x)
      (read-pdbdata file (pg-mapping-name file x))
      (read-pdbdata file x)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DESC-PDB - do an error checked description of a file entry

(define (desc-pdb file name)
    (plot-flag off)
    (let* ((syment (read-syment file name))
	   (attributes (list-attributes file)))
          (if (null? syment)
	      (printf nil "\nNo variable named %s\n" name)
	      (begin (newline)
		     (printf nil "Name: %s\n" name)
		     (print-pdb nil syment)
		     (if attributes
			 (let* ((first #t))
                     
			   (define (show-attr at-name)
			     (let* ((av (get-attribute-value
					 file name at-name)))
			       (if av
				   (begin
				     (if first
					 (begin
					   (printf nil "Attributes:\n")
					   (set! first #f)))
				     (printf nil "   ")
				     (print-pdb nil av)))))
                     
			   (for-each show-attr attributes)))
		     #t))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; LS-ATTR-PDB - do a pretty-printed list of the attributes

(define (ls-attr-pdb . rest)
    (plot-flag off)
    (let* ((atts (apply list-attributes rest)))
      (if atts
	  (begin
	    (newline)
	    (pp atts)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; LS-PDB - do a pretty-printed list of file entries

(define (ls-pdb . rest)
    (plot-flag off)
    (let* ((vars (apply list-variables rest)))
      (if vars
	  (begin
	    (newline)
	    (pp vars)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MKDIR-PDB - create a new file directory

(define (mkdir-pdb . rest)
    (plot-flag off)
    (apply make-directory rest))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; N-ENTRIES-PDB - return the number of entries in the file

(define (n-entries-pdb file)
    (plot-flag off)
    (cadr (hash-info (car (pdb->list file)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PWD-PDB - print the current file directory

(define (pwd-pdb . file)
    (plot-flag off)
    (apply current-directory file))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define PDB-functions
  '(("close-file"  close-bin-file)
    ("desc"        desc-pdb)
    ("ls"          ls-pdb)
    ("n-entries"   n-entries-pdb)
    ("print"       print)
    ("struct"      struct)
;    ("set-out"     pm-set->pdbdata)
    ("mkdir"       mkdir-pdb)
    ("curve-out"   pg-graph->pdbcurve)
    ("image-out"   pg-image->pdbdata)
    ("map-out"     pm-mapping->pdbdata)
    ("cd"          cd-pdb)
    ("pwd"         pwd-pdb)
    ("menu"        menu)
    ("image-in"    pdbdata->pg-image)
    ("map-in"      pdbdata->pg-graph)
    ("image-print" data-image)
    ("map-print"   data-mapping)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PDBVIEW CONFIGURATION

(install-file-option "C-coded" open-bin-file "PDBfile" PDB-functions)
(install-file-option "pdb"     open-pdbfile  "PDBfile" PDB-functions)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; FILE-MANAGER - a message passing approach to file management

(define (file-manager msg . rest)
    (plot-flag off)
    (if (not file-manager-table)
	(begin (set! file-manager-table (make-hash-table 17))
	       (fm-add "add" fm-add)))

    (let* ((cmd (hash-lookup msg file-manager-table))
	   (proc (if cmd (cdr cmd))))
          (if (and proc (procedure? proc))
	      (apply proc rest)
	      (printf nil "Bad file manager command %s\n"
		      msg))))

(file-manager "add" "fopen"      fm-open)
(file-manager "add" "fmatch"     fm-match)
(file-manager "add" "fnamed"     fm-named)
(file-manager "add" "freplicate" fm-replicate)
(file-manager "add" "fclose"     fm-close)
(file-manager "add" "files"      fm-files)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define stream-list (cons (make-stream "virtual-internal" "f0" "a" nil
				       "PDBfile" "PDBfile" PDB-functions)
			  nil))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

