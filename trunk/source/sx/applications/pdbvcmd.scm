; 
; PDBVCMD.SCM - PDBView commands
; 
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
; 

;(autoload* 'file-manager   "pdbvio.scm" #f)
(autoload* 'window-device  "pdbvplt.scm" #f)
(autoload* 'window-manager "pdbvplt.scm" #f)
(autoload* 'plane          "pdbvplt.scm" #f)
(autoload* 'data-manager   "pdbvdat.scm" #f)
(autoload* 'map-resolve    "pdbvplt.scm" #f)

(define follow-directories #f)
(define directory-error-on-copy #t)

(define mapping-1d-1d (cons 1 1))
(define mapping-2d-1d (cons 2 1))
(define mapping-2d-2d (cons 2 2))

(define overlay-flag #f)

(define print-head "")
(define print-tail "")

(define viewport-area-box nil)
(define viewport-area-save nil)
(define overlay-box   (list 0.175 0.85 0.25 0.825))

(define vect-scale  2.0)
(define vect-headsz 0.25)
(define vect-color  red)
(define scale 1)
(define headsize 2)
(define color 3)
(define font-size-nxm 10)
(define data-id-save 1)

(define PD_MD5_OFF   0)
(define PD_MD5_FILE  1)
(define	PD_MD5_RW    2)
(define	PD_MD5_FULL  3)

;--------------------------------------------------------------------------

;                             AUXILLIARY FUNCTIONS

;--------------------------------------------------------------------------

; PRINT-NAME - return the print name of the object (a string)

(define (print-name x)
    (if (symbol? x)
	(symbol->string x)
	x))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PROCESS-FLAGS - get a variable number of flags from arg list

(define (process-flags args)
   (define rflags nil)
   (define rest args)

   (define (do-first args)
      (let* ((flags  (car args))
	     (sflags (sprintf "%s" flags))
	     (first  (substring sflags 0 1)))
	    (if (eqv? first "-")
		(begin (set! rflags (append rflags (list flags)))
		       (if (cdr args)
			   (begin (set! rest (cdr args))
				  (do-first (cdr args)))
			   (list rflags nil)))
		(list rflags rest))))

   (do-first args))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PLOT-HAND - handler for doing plots

(define (plot-hand func ran dom)
    (if (procedure? func)
	(display-mapping* (func (make-lr-mapping-direct ran dom)))
	(display-mapping* (make-lr-mapping-direct ran dom))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; STRING->DIMENSIONS - convert a string specifying dimensions into the
;                    - proper form for use with write-pdbdata

(define (dimension->list dms)
    (let* ((mn  (strtok dms ":"))
	   (mx  (strtok dms ""))
	   (mnv (string->number mn))
	   (mxv (string->number mx)))
          (if (number? mxv)
	      (cons mnv mxv)
	      mnv)))

(define (string->dimensions str dims)
    (if (or (null? str) (eqv? str ""))
        (reverse dims)
	(let* ((dms  (strtok str ","))
	       (rem  (strtok str ""))
               (dmp  (dimension->list dms)))
              (string->dimensions rem (cons dmp dims)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GET-SCALAR - helper for scripts which want specific values
;            - from scalar expressions

(define (get-scalar fp name)
   (let* ((rval (read-pdbdata (if fp fp current-file) name)))
     (if rval
	 (cddr (pdb->list rval)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SEQ-AUX - the auxiliary sequence function

(define (seq-aux strt len delta lst)
    (if (> len 0)
        (seq-aux (+ strt delta) (- len 1) delta (cons strt lst))
        (reverse lst)))

; SEQUENCE

(define (sequence start length step) 
    "Procedure: Return a list containing <length> numbers beginning with 
                <start> and incrementing by <step>.
     Usage: sequence <start> <length> <step>"

    (seq-aux start length step nil))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SYMENT-TYPE - return the syment's type

(define (syment-type ep)
    (if ep (list-ref (pdb->list ep) 0) nil))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SYMENT-N-ITEMS - return a list of the syment's number of items

(define (syment-n-items ep)
    (if ep (list-ref (pdb->list ep) 1) nil))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SYMENT-ADDRESS - return the syment's address

(define (syment-address ep)
    (if ep (list-ref (pdb->list ep) 2) nil))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SYMENT-DIMENSIONS - return a list of the syment's dimensions

(define (syment-dimensions ep)
    (if ep (list-tail (pdb->list ep) 3) nil))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; VARIABLE-DIMENSIONS - return a list of the variable's dimensions

(define (variable-dimensions name)
    (let* ((ep (read-syment current-file name)))
          (syment-dimensions ep)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; VARIABLE-TYPE - return the variable's type

(define (variable-type name)
    (let* ((ep (read-syment current-file name)))
          (syment-type ep)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; VARIABLE-ADDRESS - return the variable's address

(define (variable-address name)
    (let* ((ep (read-syment current-file name)))
          (syment-address ep)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; ELIMINATE-VARS - return a list of objects from the second list
;                - which are not member of the first list

(define (eliminate-vars reject src dst)
    (if src
        (if (memv (car src) reject)
	    (eliminate-vars reject (cdr src) dst)
	    (eliminate-vars reject (cdr src) (cons (car src) dst)))
	(reverse dst)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; LIST-TREE - return a list of objects from the second list
;           - which are not member of the first list

(define (list-tree file dir)
    (let* ((all (list-symtab file))
           (d   (if (eqv? dir "/")
		    dir
		    (sprintf "%s/" dir)))
	   (n   (string-length d))
	   (lst nil))

          (define (match x)

; do not take hidden directories - especially the pointers directory
              (if (not (string=? (substring x 0 2) "/&"))
		  (cond ((string=? d (substring x 0 n))
			 (set! lst (cons x lst)))

; for files without directories
			((and (not (string=? (substring x 0 1) "/"))
			      (string=? d "/"))
			 (set! lst (cons x lst))))))

          (for-each match all)

	  (reverse lst)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WHICH-VARIABLES - make a list of file variables from
;                 - the given specification

(define (which-variables file dir spec)
    (cond ((equal? (car spec) '*)
	   (if follow-directories
	       (list-tree current-file dir)
	       (list-variables current-file)))
	  ((equal? (car spec) '~)
	   (define-macro (stringify obj) (sprintf "%s" obj))
	   (eliminate-vars (map stringify (cdr spec))
			   (if follow-directories
			       (list-tree current-file dir)
			       (list-variables current-file))
			   nil))
	  (else
	   spec)))

;--------------------------------------------------------------------------

;                           PRINT AUXILLIARIES

;--------------------------------------------------------------------------

; CHE - change the print head

(define-macro (che . val)
   (if val
       (set! print-head (sprintf "%s" (car val)))
       print-head))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; CTA - change the print tail

(define-macro (cta . val)
   (if val
       (set! print-tail (sprintf "%s" (car val)))
       print-tail))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SPLIT-NAME-AT-TERMINAL - split the name at the terminal node
;                        - which is delineated by a "." or "->"
;                        - include the "." or "->" in the terminal
;                        - name
;                        - return a pair with the nonterminal and terminal
;                        - pieces

(define (split-name-at-terminal name)
    (let* ((nm (sprintf "%s" name))
	   (tm  (lasttok nm ".>"))
	   (nt  (lasttok nm "\n"))
	   (lnt (string-length nt))
	   (lnm (string-length name))
	   (chk (string-ref name lnt)))
          (cond ((eqv? chk #\>)
		 (cons (substring name 0 (- lnt 1))
		       (substring name (- lnt 1) lnm)))
		((eqv? chk #\.)
		 (cons (substring name 0 lnt)
		       (substring name lnt lnm)))
		(else
		 (cons name nil)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; STRIP-FIRST-TOKEN - take a pattern like
;                   -   [d]*.*
;                   - where .* means match zero or more characters
;                   - and [d]* means match one or more digits
;                   - and return a list consisting of
;                   - ( <the number corresponding to [d]*>
;                   -   <the match for the text .*> )

(define (strip-first-token name)
    (let* ((sm (sprintf name))
	   (sn (sprintf name))
	   (sb (strtok sm "]"))
	   (tb (strtok sm "\n"))
	   (sc (strtok sn ","))
	   (tc (strtok sn "\n")))
          (if (< (string-length sb) (string-length sc))
	      (list (string->number sb)
		    (sprintf "]%s" (if tb tb "")))
	      (list (string->number sc)
		    (sprintf ",%s" (if tc tc ""))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; STRIP-LAST-TOKEN - take a patterns like
;                  -   .*[d]*
;                  - where .* means match zero or more characters
;                  - and [d]* means match one or more digits
;                  - and return a list consisting of
;                  - ( <the number corresponding to [d]*>
;                  -   <the match for the text .*> )

(define (strip-last-token name)
    (let* ((sm (sprintf name))
	   (sn (sprintf name))
	   (sb (lasttok sm "["))
	   (tb (lasttok sm "\n"))
	   (sc (lasttok sn ","))
	   (tc (lasttok sn "\n")))
          (if (< (string-length sb) (string-length sc))
	      (list (string->number sb)
		    (sprintf "%s[" (if tb tb "")))
	      (list (string->number sc)
		    (sprintf "%s," (if tc tc ""))))))

;--------------------------------------------------------------------------

;                         USER LEVEL COMMANDS

;--------------------------------------------------------------------------

; ANIMATE - rotate the specified mapping thru 360 degrees

(define (animate theta dphi chi)
    "ANIMATE - Rotate the specified mapping thru 360 degress
     Usage: animate <theta> <dphi> <chi>
     Example: animate 30.0 10.0 0.0"
    (define (iter phi dp poff)
        (if (<= phi (+ 360 poff))
            (let* ((dev (window-device current-window)))
	          (view-angle theta phi chi)
		  (viewport-update)
		  (pg-finish-plot dev)
		  (pg-clear-window dev)
		  (iter (+ phi dp) dp poff))))

    (iter 0.0 dphi 10.0))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; ANNOT - add annotation

(define (annot labl clr xmn xmx ymn ymx)
   "ANNOT - add an annotation to the current windos
    Usage: annot <label> <color> <xmin> <xmax> <ymin> <ymax>
    Example: annot \"look here\" blue 0.4 0.6 0.95 0.98"
    (let* ((dev (window-device current-window)))
      (add-annotation dev labl clr xmn xmx ymn ymx)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; AVERAGE - average the specified mappings

(define (average . x)
   "AVERAGE - return a mapping which is the average of the
    given mappings.
    Usage: average [<mapping>]*
    Example: average 1 3 5"
   (let* ((n (length x))
	  (new (apply m+ x)))
         (label-drawable (sprintf "Average %s" x)
			 (divy n new))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; BUILD-ARRAY

(define-macro (build-array x)
    (build-array* x))

(define (build-array* x)
    (if x
      (let* ((data (read-pdbdata current-file x)))
	(if data
	    (let* ((vals (cddr (pdb->list data))))
	      (cond ((number? vals)
		     vals)
		    ((pm-array? vals)
		     (pm-array->list vals))
		    ((vector? vals)
		     (vector->list vals))))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; CD - change the current file directory

(define-macro (cd . rest)
    "CD - Change the current file directory.
     Usage: cd [<directory-name>]
     Examples: cd mydir
               cd /zoo/mammals
               cd ../reptiles
               cd"
    (plot-flag off)
    (if rest
	((io-function current-file "cd") current-file (car rest))
	((io-function current-file "cd") current-file)))

(define (cd* . args)
    "Procedure version of cd macro"
    (apply cd args))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; CHANGE-FILE / CF / OPEN - change the current file

; the change-file macro

(define-macro (change-file file . arg)
    "CHANGE-FILE / CF - Change the current file to be the named one.
                 If file is in the internal list of open files,
                 make it current, otherwise, open it.
                 If mode is \"r\", open file read only.
                 If mode is \"a\", open file read/write.
                 If mode is \"w\", overwrite any existing file.
                 Mode defaults to \"a\".
                 Alias defaults to \"fd\", d is an increasing decimal number.
                 Type is the file type (options are determined by output
                 spokes you have installed).
     Usage: change-file <filename> [<mode> [<alias> [<type>]]]
     Examples: change-file foo
               change-file foo.s00 r
               change-file foo.s00 w
               change-file foo.s00 w foo
               change-file foo.s00 w bar pdb"
    (plot-flag off)
    (let* ((name  (print-name file))
	   (mode  (if (and arg (> (length arg) 0))
		      (print-name (list-ref arg 0))
		      "a"))
	   (alias (if (and arg (> (length arg) 1))
		      (print-name (list-ref arg 1))
		      nil))
	   (type  (if (and arg (> (length arg) 2))
		      (print-name (list-ref arg 2))
		      "pdb"))
	   (fp    (file-manager "fopen" name mode type alias)))
      (set! current-file fp)
      fp))

; the change-file procedure

(define (change-file* file . arg)
    "Procedure version of change-file macro"
    (apply change-file (cons file arg)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; CHANGE-PALETTE

(define-macro (change-palette . pal)
    "CHANGE-PALETTE - Change the palette for the current window.
     Usage: change-palette palette
     Example: change-palette \"spectrum\""
    (window-manager "pchange" pal))

(define (change-palette* . pal)
    "Procedure version of change-palette macro"
    (apply change-palette pal))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; CHANGE-VIEWPORT

(define (change-viewport . x)
    "CHANGE-VIEWPORT - Change current viewport.
     Usage: change-viewport [name [x y dx dy]]
     Examples: change-viewport
               change-viewport \"B\" 0.01 0.01 0.98 0.48"
    (window-manager "vopen" x))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; CHANGE-WINDOW

(define (change-window . info)
    "CHANGE-WINDOW - Change current window.
     Usage: change-window [title [mode type x y dx dy]]
     Examples: change-window
               change-window \"B\" \"COLOR\" \"WINDOW\" 0.5 0.1 0.4 0.4"
    (window-manager "wopen" info))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; CLOSE - close a file

(define-macro (close . file)
    "CLOSE - Close a file.
             The default is to close the current file.
     Usage: close [file]
     Examples: close
               close foo
               close f1"

    (cond ((null? file)
	   (if current-file
	       (set! current-file (file-manager "fclose" current-file))))
	  (else
	   (let* ((fp (car file)))
	     (if (and current-file (file-manager "fmatch" fp current-file))
		 (set! current-file (file-manager "fclose" current-file))
		 (file-manager "fclose" fp)))))
    #f)

(define (close* . args)
    "Procedure version of close macro"
    (apply close args))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; CLOSE-VIEWPORT

(define (close-viewport . x)
    "CLOSE-VIEWPORT - Close a viewport.
     Usage: close-viewport [vname [wname]]"
    (window-manager "vclose" x))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; CLOSE-WINDOW / CLW

(define (close-window . window)
    "CLOSE-WINDOW  / CLW - Close a graphics window.
     Usage: close-window <window>
     Examples: close-window \"ABC\"
               clw"
    (window-manager "wclose" window))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; CMC - copy mapping as curve

(define-macro (cmc file . args)
    "CMC - Copy 1d mappings from the current file to another file
           as ULTRA curves. Mappings are referenced by the numbers
           displayed by the menu command. If the mapping list is *,
           copy all 1d mappings in the current directory.
     Usage: cmc file <mapping-list>
     Examples: cmc foo *
               cmc bar 1 5"

    (let* ((fp (stream-file (file-reference file stream-list))))

      (if (null? fp)
	  (let* ((tf current-file))
	        (set! fp (change-file* file "w"))
		(change-file* tf)))

      (define (cmc_seq x)
	(let* ((type (pg-menu-item-type current-file x))
	       (mapping (if (eqv? type "image")
			    ((io-function current-file "image-in")
			     current-file x)
			    ((io-function current-file "map-in")
			     current-file x)))
	       (dims (if (or (not type) (eqv? type "image"))
			 nil
			 (pm-mapping-dimension mapping))))
	  (cond ((or (eqv? type "curve") (pg-graph? mapping))
		 (if (equal? dims mapping-1d-1d)
		     ((io-function fp "curve-out") mapping fp))
		 (cmc_seq (+ x 1)))
		((eqv? type "image")
		 (cmc_seq (+ x 1)))
		(else #t))))

      (define-macro (map_conv x)
	(let* ((type (pg-menu-item-type current-file x))
	       (mapping (if (eqv? type "image")
			    ((io-function current-file "image-in")
			     current-file x)
			    ((io-function current-file "map-in")
			     current-file x)))
	       (dims (if (or (not type) (eqv? type "image"))
			 nil
			 (pm-mapping-dimension mapping))))
	  (cond ((or (eqv? type "curve") (pg-graph? mapping))
		 (if (equal? dims mapping-1d-1d)
		     ((io-function fp "curve-out") mapping fp)
		     (printf nil "\nMapping %s has wrong dimensionality\n" x)))
		(else 
		 (printf nil "\n%s is not a valid mapping\n" x)))))

      (if args
	  (if (equal? (car args) '*)
	      (cmc_seq 1)
	      (for-each map_conv args)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DATA-DESTINATION - parse specification of form
;                  -   <file>[:<dir>]
;                  - return (<file> <dir>)

(define (data-destination x)
    (let* ((file nil)
	   (dir  nil))
          (cond ((and (pair? x) (eqv? 'thru (list-ref x 0)))
		 (set! file (list-ref x 1))
		 (set! dir  (list-ref x 2)))
		(else
		  (let* ((s (sprintf "%s" x)))
		        (set! file (strtok s ":"))
			(set! dir  (strtok s ":\n")))))
	  (list file
		(if (null? dir) "/" (sprintf "%s/" dir)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; COPY-WORKER - do the work of the copy variants

(define (copy-worker which rest)

    (define (match-flag fl val rv)
        (if (and (pair? fl) (not rv))
	    (match-flag (cdr fl) val (eqv? (print-name (car fl)) val))
	    rv))

    (define (mode-flag fl rv) 
        (if (pair? fl)
	    (let* ((flag (print-name (car fl)))
		   (rest (list-tail fl 1)))
	          (cond ((eqv? flag "-w")
			 (mode-flag rest "w"))
			((eqv? flag "-a")
			 (mode-flag rest "a"))
			(else
			 (mode-flag rest rv))))
	    rv))

    (let* ((wh        (eqv? which "cp"))
	   (x         (if (pair? rest) (process-flags rest) nil))
	   (flags     (if (pair? rest) (list-ref x 0) nil))
	   (args      (if (pair? rest) (list-ref x 1) rest))
           (dst       (data-destination (if wh
					    (last args)
					    (list-ref args 0))))
	   (dstf      (list-ref dst 0))
	   (dstd      (list-ref dst 1))
	   (vars      (if wh
			  (reverse (list-tail (reverse args) 1))
			  (list-tail args 1)))
	   (thsf      current-file)
	   (rw        (mode-flag flags nil))
	   (rv        #t))

          (set! follow-directories (match-flag flags "-R" #f))

          (if vars
	      (let* ((fp (stream-file (file-reference dstf stream-list rw)))
		     (in-types (reverse (list-defstrs thsf #f)))
		     (the-vars (which-variables thsf
						(current-directory thsf)
						vars)))

		    (if (match-flag flags "-vfi" #f)
			(if (= (pd-verify current-file) 0)
			    (begin (printf nil "Checksum test failed on '%s'\n"
					   current-file)
				   (set! rv #f))))

; set the major order of the new file
; flag -rmo gives row major order
; flag -cmo gives column major order
		    (major-order fp
				 (cond ((match-flag flags "-rmo" #f)
					row-major)
				       ((match-flag flags "-cmo" #f)
					column-major)
				       (else
					(major-order fp))))

; set the default offset of the new file
; quick but too dirty - -d<n> for offset of <n>
; currently only 0 and 1 are handled
; otherwise the default
		    (default-offset fp
		                    (cond ((match-flag flags "-d0" #f)
					   0)
					  ((match-flag flags "-d1" #f)
					   1)
					  (else
					   (default-offset fp))))

; set the checksum mode of the new file
; flag -cf gives whole file checksum
; flag -cv gives per variable checksum
		    (pd-activate-cksum fp
				 (cond ((match-flag flags "-cf" #f)
					PD_MD5_FILE)
				       ((match-flag flags "-cv" #f)
					PD_MD5_RW)
				       (else
					(pd-activate-cksum fp))))

		    (define-macro (move-type x)
		        (write-defstr* fp (read-defstr* thsf x)))

		    (define-macro (move-data x)
		        (let* ((s    (print-name x))
			       (type (variable-type x))
			       (xd   (if (eqv? (substring s 0 1) "/")
					 (sprintf "%s%s"
					      dstd (substring s 1 -1))
					 (sprintf "%s%s"
					      dstd (substring s 0 -1)))))
			  (if (equal? type "Directory")
			      (if (not (equal? x "/"))
				  (make-directory fp xd directory-error-on-copy))
			      (let* ((data (read-pdbdata thsf x)))
				    (if (null? data)
					(printf nil "Variable %s not found\n" x)
					(write-pdbdata fp xd data))))))

		    (if in-types
			(let* ((derr (if (file-directory? fp dstd)
					 #f
					 directory-error-on-copy)))
			      (if (equal? (file-type fp) "PDBfile")
				  (pg-def-graph-file fp))
			      (for-each move-type in-types)
			      (if (not (equal? dstd "/"))
				  (make-directory fp dstd derr))
			      (if the-vars
				  (for-each move-data the-vars))))

		    (if (match-flag flags "-vfo" #f)
			(if (= (pd-verify fp) 0)
			    (begin (printf nil "Checksum test failed on '%s'\n"
					   dstf)
				   (set! rv #f))))))
		    rv))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; COPY - copy variables and types from the current file to another file

(define-macro (copy . rest)
    "COPY - Copy variables from the current file to another file.
            If variable list is *, copy all variables in current directory.
            If variable list starts with ~, copy all but the following variables
            in current directory.
            Deprecated.  Use the cp command instead
     Usage: copy [<flags>] file[:<dir>] <variable-list>
            where the value of flags is:
               -R  copy directories recursively
               -w  remove and create the destination file
               -a  append to the destination file
     Examples: copy foo *
               copy -R foo *
               copy -R foo:/new *
               copy -R -w foo:/new *
               copy foo ~ bird cat
               copy f3 bird cat dog"
    (copy-worker "copy" rest))

(define (copy* . args)
    "Procedure version of copy macro"
    (apply copy args))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; COPY-MAPPING / CM - copy mappings from the current file to another file

(define-macro (copy-mapping file . args)
    "COPY-MAPPING / CM - Copy mappings from the current file to another file.
                  Mappings are referenced by the numbers displayed
                  by the menu command. If the mapping list is *,
                  copy all mappings in the current directory.
     Usage: copy-mapping file <mapping-list>
     Examples: copy-mapping foo *
               copy-mapping bar 1 5"

    (file-manager "freplicate" "file" current-file file args))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; CP - copy variables and types from the current file to another file
;    - the arguments are ordered as in the UNIX cp utiltiy

(define-macro (cp . rest)
    "CP - Copy variables from the current file to another file.
          Like the UNIX cp command with destination last.
          If variable list is *, copy all variables in current directory.
          If variable list starts with ~, copy all but the following variables
          in current directory.
     Usage: cp [<flags>] <variable-list> file[:<dir>]
            where the value of flags is:
               -R  copy directories recursively
               -w  remove and create the destination file
               -a  append to the destination file
     Examples: cp * foo
               cp -R * foo
               cp -R * foo:/new
               cp -R -w * foo:/new
               cp ~ bird cat foo
               cp bird cat dog f3"
    (copy-worker "cp" rest))

(define (cp* . args)
    "Procedure version of cp macro"
    (apply cp args))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; CREATE-PALETTE - interactively create a new palette

(define-macro (create-palette name . rest)
    "CREATE-PALETTE - Create a new palette.  See the SAVE-PALETTE
                      command for saving to disk.
     Usage: create-palette name [<ndims> <nx> <ny>]
     Examples: create-palette new_palette
               create-palette new_palette 2 24 8"
     (if (not (defined? current-window))
         (change-window "A"))
     (if (null? rest)
         (pg-make-palette (cadr current-window) name)
         (let* ((ndims (car rest))
		(nx    (cadr rest))
		(ny    (if (not (null? (cddr rest)))
			   (caddr rest)
			   1)))
	   (if (< 194 (* nx ny))
	       (printf nil "There are only 194 colors available.\n")
	       (pg-make-palette (cadr current-window) name  1 ndims nx ny)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (curves->table name . lst)
    "CURVES->TABLE - Write the specified curves to the ASCII file
                     NAME.
     Usage: curves->table name crv1 crv2 ...
     Examples: curves->table \"foo.dat\" 1 3 6"

    (define (get-y-vars vlst ilst)
        (if ilst
	    (get-y-vars (cons (sprintf "yval%d" (- (list-ref ilst 0) 1))
			      vlst)
			(list-tail ilst 1))
	    (reverse vlst)))

    (define (get-x-var ilst)
        (sprintf "xval%d" (- (list-ref ilst 0) 1)))

    (let* ((fp (fopen name "w"))
	   (y  (get-y-vars nil lst))
	   (x  (get-x-var lst))
	   (g  (make-lr-mapping-direct y x)))
          (pm-mapping->table fp g)
	  (fclose fp)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DATA-ID - turn on/off display of data-id

(define (data-id . flag)
    "DATA-ID - Turn on/off the plotting of curve markers.
     Usage: data-id [<on> | <off>]"
    (if (not (defined? current-window))
        (change-window "A"))

    (if flag
        (set! data-id-save (car flag))
        (set! data-id-save 1))

    (if flag
        (let* ((f (car flag)))
	  (pg-set-data-id-flag! (cadr current-window) (cond ((boolean? f)
							     0)
							    ((integer? f)
							     f)
							    (else
							     1))))
        (pg-set-data-id-flag! (cadr current-window) 1))
     #t)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DATA-REFERENCE - refer to the nth mapping in the specified file

(define-macro (data-reference indx . rest)
    "DATA-REFERENCE - Refer to the nth mapping in the optionally 
                      specified file. (Macro)
     Usage: data-reference <n> [<file-name>|<file-alias>|<file-type>]"
    (let* ((i (eval indx)))
      (if rest
	  (let* ((file (file-manager "fnamed" (car rest))))
	    (mapping-ref file "mapping" i))
	  (mapping-ref current-file "mapping" i))))

(define (data-reference* indx . rest)
    "Procedure version of data-reference macro"
    (if rest
	(let* ((file (file-manager "fnamed" (car rest))))
	    (mapping-ref file "mapping" indx))
	(mapping-ref current-file "mapping" indx)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DEF - install object in the workspace

(define-macro (def name var)
    "DEF - Install an object in the workspace under the given name.
     Usage: def <name> <object>
     Example: def foo m3"

    (let* ((ev (apply define-global (list name var))))
      (cond ((pdbdata? ev)
	     name)
	    ((pm-array? ev)
	     (pm-array->pdbdata ev nil name))
	    ((pm-set? ev)
	     (pm-set->pdbdata ev nil name))
	    ((pm-mapping? ev)
	     (pm-mapping->pdbdata ev nil name))
	    ((pg-graph? ev)
	     (pg-graph->pdbdata ev nil name))
	    ((pg-image? ev)
	     (pg-image->pdbdata ev nil name))
	    ((hash-table? ev)
	     (hash->pdbdata ev nil name))
	    (else
	     (printf nil
		     "Object not suitable for workspace: %s\n"
		     ev)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DEFAULT-VIEWPORT-RENDERING

(define (default-viewport-rendering . x)
    "DEFAULT-VIEWPORT-RENDERING - Specify a default rendering mode for a viewport.
     Usage: default-viewport-rendering [<viewport> [<window>]] <info>
            for 1d domain - 1d range mappings
            <info> - cartesian | polar | insel
            for 2d domain - 1d range mappings
            <info> - contour [<n-levels>] |
                     image      |
                     dvb        |
                     fill-poly  |
                     shaded     |
                     points     |
                     wire-frame |
                     mesh
            for 2d domain - 2d range mappings
            <info> - vect   |
                     shaded |
                     mesh
            for 3d domain
            <info> - mesh   |
                     shaded |
                     contour [<n-levels>]
     Examples: default-viewport-rendering \"A\" \"A\" polar
               default-viewport-rendering contour
               default-viewport-rendering contour 15
               default-viewport-rendering image
               default-viewport-rendering fill-poly
               default-viewport-rendering wire-frame
               default-viewport-rendering shaded
               default-viewport-rendering vect"
    (window-manager "vrender" x))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DEFINE-FILE-VARIABLE

(define-macro (define-file-variable type name . vals)
    "DEFINE-FILE-VARIABLE - Create a new variable in a PDB file
     Usage: define-file-variable type name [values]*
     Examples: define-file-variable double foo 3.42
               define-file-variable double bar[1:10,2] 0.0
               define-file-variable \"char *\" bar \"Hi\" \"there\"
               define-file-variable \"int\" baz 1"
    (defv-aux type name vals))

(define (define-file-variable* type name . vals)
    "Procedure version of define-file-variable macro"
    (defv-aux type name vals))

(define (defv-aux type name vals)
    (if vals
	(let* ((sname (if (symbol? name) (symbol->string name) (sprintf name)))
	       (vname (strtok sname "[]"))
	       (dimsn (string->dimensions (strtok sname "[]") nil))
	       (carvals (car vals))
	       (values (if (pair? carvals) carvals vals))
               (dims (if dimsn dimsn (list (length values))))
	       (decl (append (list 'type type) dims))
	       (args (append (list current-file vname decl) values)))
	  (apply write-pdbdata args))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DELETE-MAPPING

(define (delete-mapping . x)
    "DELETE-MAPPING - Delete mappings from a viewport.
     Usage: delete-mapping [vname [wname]] <mapping>*
     Examples: delete-mapping 2 4 5"
    (window-manager "vdelete" (flatten-args x)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DEMO

(define (demo . x)
    "DEMO - Run the PDBView demo.
     Usage: demo
     Examples: demo"

; run the demo in its own process to avoid contaminating
; current session
   (system "pdbview -l pdbvdemo.scm"))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DESC - describe variables or structure members in current file directory

(define-macro (desc . rest)
    "DESC - Describe variables or struct members in current file directory.
            To describe part of a variable or member qualify the name with
            index expressions whose parts are in one of the three forms:
               <index>
               <index-min>:<index-max>
               <index-min>:<index-max>:<increment>
            Only the first form may be used with non-terminal members or
            with variables and terminal members with embedded pointers.
            If a selection pattern is specified in place of a variable
            name, all variables matching the pattern will be described.
            An \"*\" matches any zero or more characters and \"?\" matches
            any single character. A type qualifier may also be specified.
            A size threshold may optionally be supplied.
     Usage: desc <variable> | <structure-member> | <pattern> [<type> [<size>]]
     Examples: desc Mapping1
               desc a.b.c[12:14]
               desc * double
               desc var? integer
               desc var? integer 20"

    (plot-flag off)
    (let* ((fnc (io-function current-file "desc"))
	   (name (if (pair? rest) (car rest) nil))
	   (type (if (pair? rest)
		     (if (> (length rest) 1) (list-ref rest 1) nil)
		     nil))
	   (size (if (pair? rest)
		     (if (> (length rest) 2) (list-ref rest 2) nil)
		     nil))
	   (all  (if (pair? rest)
		     (if (> (length rest) 3) (list-ref rest 3) nil)
		     nil))
	   (tn (symbol->string name))
	   (ln (string-length tn))
	   (tn (if (string=? "/" (substring tn (- ln 1) ln))
		   tn
		   (string-append tn "/")))
	   (isdir (memv tn (list-variables current-file)))
	   (vars (if type		;12/23/97: revised - see below
		     (list-variables current-file nil name type size all)
		     (if isdir
			 (list tn)
			 (if name
			     (list-variables current-file nil name)
			     (list-variables current-file))))))
;
; 12/23/97: this was the original coding replaced by above coding
;
;	     (vars (if type
;		       (list-variables current-file name type)
;		       (if name
;			   (list-variables current-file name)
;			   (list-variables current-file)))))

      (define (desc-one var)
	(fnc current-file var))

      (if vars
	  (for-each desc-one vars)
	  (if (and name (file-variable? current-file name #t))
	      (if type
		  (if (eqv? (print-name type) (variable-type name))
		      (desc-one name))
		  (desc-one name))))))

(define (desc* . args)
    "Procedure version of desc macro"
    (apply desc args))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DISPLAY-DOMAIN

(define-macro (display-domain . x)
    "DISPLAY-DOMAIN - Plot a domain mesh in a viewport.
     Usage: display-domain [<viewport> [<window>]] <domain>*
     Example: display-domain f1.18"

; bag of cruft to make three cases work: 1, Mapping0, and f1.1
; formally: <integer>, <variable-name>, and (data-reference ...)
    (define-macro (prep y)
        (pm-mapping-domain
	 (cond ((pair? y)
		(eval (cadr y)))
	       ((integer? y)
		(data-reference* y))
	       ((printable? y)
		(if (eqv? (strtok (variable-type y) " *") "PM_set")
		    (pdbdata->pm-set current-file y)
		    (pdbdata->pg-graph current-file y)))
	       (else
		y))))

    (window-manager "vattach" (map prep x)))

; DISPLAY-DOMAIN

(define (display-domain* . x)
    "DISPLAY-DOMAIN* - Plot a domain mesh in a viewport.
     Usage: display-domain [<viewport> [<window>]] <domain>*
     Example: display-domain* f1.18"
    (apply display-domain x))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DISPLAY-MAPPING / DM / PL

(define-macro (display-mapping . x)
    "DISPLAY-MAPPING  / DM / PL - Plot the specified mappings.
     Usage: display-mapping [<viewport> [<window>]] <mapping>*
     Examples: display-mapping 5
               dm dir1/Mapping8
               pl 5 8"

; bag of cruft to make three cases work: 1, Mapping0, and f1.1
; formally: <integer>, <variable-name>, and (data-reference ...)
    (define-macro (prep y)
        (if (pair? y)
            (eval (cadr y))
	    y))

    (window-manager "vattach" (flatten-args (map prep x))))

(define (display-mapping* . x)
    "Procedure version of display-mapping macro"
    (window-manager "vattach" x))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DRAWABLE-RENDERING

(define-macro (drawable-rendering dr . info)
    "DRAWABLE-RENDERING - Specify the rendering mode for a drawable
     Usage: drawable-rendering <drawable> <info>
            <info> - [<spec>]*
            <spec> - (render <render>) |
                   - (view-angle <theta> <phi> <chi>) |
                   - (change-palette <palette>)
            for 1d domain - 1d range mappings
            <render> - cartesian | polar | insel
            for 2d domain - 1d range mappings
            <render> - contour [<n-levels>] |
                       image      |
                       dvb        |
                       fill-poly  |
                       shaded     |
                       points     |
                       wire-frame |
                       mesh
            for 2d domain - 2d range mappings
            <render> - vect   |
                       shaded
            for 3d domain
            <render> - mesh   |
                       shaded
     Examples: drawable-rendering 3 (render polar)
               drawable-rendering 1 (render contour)
               drawable-rendering f1.1 (render contour 15)
               drawable-rendering f1.12 (render image) (change-palette spectrum)
               drawable-rendering 18 (render fill-poly)
               drawable-rendering 4 (render wire-frame) (view-angle 60 45 0)
               drawable-rendering 1 (render shaded) (change-palette cyans)
               drawable-rendering 1 (render vect)"
    (window-manager "drender" (list dr info)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DOMM

(define (domm . args)
    "DOMM - Select plotting limits for the domain of a mapping.
            If invoked with no limit values the domain
            will default to that implied by the data set.
     Usage: domm [<viewport> [<window>]] <mapping> <x1_min> <x1_max> ...
     Examples: domm 1 -5.0 10.0 10 20
               domm 1"
    (window-manager "mdlimit" args))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DOMV

(define (domv . args)
    "DOMV - Select plotting limits for the domain of a viewport.
            If invoked with no limit values the domain
            will default to that implied by the data sets.
     Usage: domv [<viewport> [<window>]] <x1_min> <x1_max> ...
     Examples: domv -5.0 10.0 10 20
               domv"
    (window-manager "vdlimit" args))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; END / QUIT - end PDBView

(define (end . args)
    "END / QUIT - End the session of PDBView.
     Usage: end"

    (if stream-list
	(for-each (lambda (x)
		      (set! current-file (stream-file x))
		      ((io-function current-file "close-file") current-file))
		  stream-list))

    (if (and (defined? ps-device) ps-device)
        (pg-close-device ps-device))
    
    (if (and (defined? cgm-device) cgm-device)
        (pg-close-device cgm-device))
    
    (if (and (defined? mpeg-device) mpeg-device)
        (pg-close-device mpeg-device))
    
    (if (and (defined? jpeg-device) jpeg-device)
        (pg-close-device jpeg-device))
    
    (if (and (defined? png-device) png-device)
        (pg-close-device png-device))
    
    (printf nil "\n")
    (terminate (if args
		   (eval (car args))
		   0)))

(define terminate quit)
(define quit end)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; FILE - describe the current file

(define-macro (file . rest)
    "FILE - Describe the current file.
            Options: t or type displays the file type only
     Usage: file [t | type]
     Examples: file
               file type"
    (begin
        (if (null? rest)
	    (print-pdb nil current-file)
	    (let ((opt (car rest)))
	         (cond ((or (eqv? opt 't) (eqv? opt 'type))
			(printf nil "\nFile type: ")
			(if current-file
			    (display (file-type current-file))
			    (display "SX_vif"))
			(newline))
		       (#t (print-pdb nil current-file)))))
	#t))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; FONT - set/describe the current font of the current window

(define (font . args)
    "FONT - set/get the current font information.
     Usage: font [<face> <point-size> <style>]
            <face> - helvetica | times | courier
            <point-size> - 8 | 10 | 12 | 14 ...
            <style> - medium | bold | italic
     Examples: font
               font helvetica 12 medium"
    (window-manager "vfont" current-window args))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; FORMAT - set the printing format for a specified data type

(define-macro (format type format-string)
    "FORMAT - Set the printing format for a specified data type.
              If the type argument has 1 or 2 appended, then the
              specified format is applied only to arrays shorter
              than array-length or to arrays longer than or equal
              to array-length, respectively. Otherwise, the format
              applies to both. Invoking the format command with
              the single argument, default, causes the formats for
              all types to be reset to their defaults. The single argument,
              all, causes all the current formats to be listed. The format
              argument must be a standard C I/O library format
              string. Double quotes are only necessary if the
              format string contains embedded blanks. See the set
              command for more about the array-length variable.
              This command overrides the settings of the decimal-
              precision and bits-precision control parameters.
     Usage: format char[1 | 2] | bit[1 | 2] |
                   short[1 | 2] | int[1 | 2] | long[1 | 2] |
                   long_long[1 | 2] |
                   float[1 | 2] | double[1 | 2] | long_double[1 | 2] |
                   float_complex[1 | 2] | double_complex[1 | 2] |
                   long_double_complex[1 | 2] |
                   suppress-member <format>
     Usage: format default
     Usage: format all
     Examples: format double %12.5e
               format double2 %10.2e
               format char \"%s  \"
               format default
               format all"
    (if (null? type)
        (printf nil "\nNo type specified\n")
        (cond ((eqv? type 'default)
	       (set-format 'default))
	      ((eqv? type 'all)
	       (for-each '(lambda (x)
			      (printf nil "   %-18s \"%s\"\n"
				      x
				      (set-format x)))
			 type-list))
	      (else
	       (if (null? format-string)
		   (printf nil "   \"%s\"\n"
			   (set-format type))
		   (if (not (memv type type-list))
		       (printf nil "\n%s is not a valid type specifier\n"
			       type)
		       (if (interactive?)
			   (printf nil "   \"%s\"\n"
				   (set-format type format-string))
			   (set-format type format-string))))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GET-DOMAIN - get the domain of the given mapping

(define (get-domain . args)
    "GET-DOMAIN - get the domain of the given mapping.
     Usage: get-domain <mapping>
     Examples: get-domain 2"
    (window-manager "mdlimg" args))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GET-DOMAIN-NUMBER-POINTS - get the number of points in the
;                          - domain of the given mapping

(define (get-domain-number-points . args)
    "GET-DOMAIN-NUMBER-POINTS - get the domain of the given mapping.
     Usage: get-domain-number-points <mapping>
     Examples: get-domain-number-points 2"
    (window-manager "mdnptsg" args))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GET-LABEL - get the label of the given mapping

(define (get-label . args)
    "GET-LABEL - get the label of the given mapping.
     Usage: get-label <mapping>
     Examples: get-label 2"
    (window-manager "mlabg" args))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GET-RANGE - get the range of the given mapping

(define (get-range . args)
    "GET-RANGE - get the range of the given mapping.
     Usage: get-range <mapping>
     Examples: get-range 2"
    (window-manager "mrlimg" args))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GET-RANGE-NUMBER-POINTS - get the number of points in the
;                         - range of the given mapping

(define (get-range-number-points . args)
    "GET-RANGE-NUMBER-POINTS - get the range of the given mapping.
     Usage: get-range-number-points <mapping>
     Examples: get-range-number-points 2"
    (window-manager "mrnptsg" args))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HARDCOPY-VIEWPORT - send the current graph to current hard copy devices

(define-macro (hardcopy-viewport . rest)
    "HARDCOPY-VIEWPORT - Send the current graph to all open hardcopy devices.
                         Color output is indicated by an optional argument on
                         the first call for a given device.
                         The optional resolution scale factor is an integer
                         factor by which the resolution will be decreased
                         below the full resolution of the device.
     Usage: hardcopy-viewport [color | monochrome] [portrait | landscape] [resolution-scale-factor]]
     Examples: hardcopy-viewport
               hardcopy-viewport color
               hardcopy-viewport color 8"
    (interactive off)
    (let* ((psflag (ps-flag))
	   (cgmflag (cgm-flag))
	   (mpegflag (mpeg-flag))
	   (jpegflag (jpeg-flag))
	   (pngflag (png-flag))
	   (args (process-hc-args rest))
	   (color (list-ref args 0))
	   (mode (list-ref args 1))
	   (res (list-ref args 2))
	   (type (sprintf "%s %s" color mode)))

      (if (= jpegflag 1)
	  (begin
	    (if (not jpeg-device)
		(begin
		  (set! jpeg-device
			(pg-make-device "JPEG"
					type))
		  (pg-open-device jpeg-device
				  (window-origin-x-jpeg) (window-origin-y-jpeg)
				  (window-width-jpeg) (window-height-jpeg))))
	    (pg-set-palette! jpeg-device (pg-current-palette WIN-device))
	    (window-manager "vhardcopy" jpeg-device res)))

      (if (= pngflag 1)
	  (begin
	    (if (not png-device)
		(begin
		  (set! png-device
			(pg-make-device "PNG"
					type))
		  (pg-open-device png-device
				  (window-origin-x-png) (window-origin-y-png)
				  (window-width-png) (window-height-png))))
	    (pg-set-palette! png-device (pg-current-palette WIN-device))
	    (window-manager "vhardcopy" png-device res)))

      (if (= psflag 1)
	  (begin
	    (if (not ps-device)
		(begin
		  (set! ps-device
			(pg-make-device "PS"
					type))
		  (if (equal? color "COLOR")
		      (pg-set-color-type! ps-device "PS" color))
		  (pg-open-device ps-device
				  (window-origin-x-ps) (window-origin-y-ps)
				  (window-width-ps) (window-height-ps))))
	    (pg-set-palette! ps-device (pg-current-palette WIN-device))
	    (window-manager "vhardcopy" ps-device res)))

      (if (= mpegflag 1)
	  (begin
	    (if (not mpeg-device)
		(begin
		  (set! mpeg-device
			(pg-make-device "MPEG"
					type))
		  (pg-open-device mpeg-device
				  (window-origin-x-mpeg) (window-origin-y-mpeg)
				  (window-width-mpeg) (window-height-mpeg))))
	    (pg-set-palette! mpeg-device (pg-current-palette WIN-device))
	    (window-manager "vhardcopy" mpeg-device res)))

      (if (= cgmflag 1)
	  (begin
	    (if (not cgm-device)
		(begin
		  (set! cgm-device
			(pg-make-device "CGM"
					color))
		  (pg-open-device cgm-device
				  (window-origin-x-cgm) (window-origin-y-cgm)
				  (window-width-cgm) (window-height-cgm))))
	    (pg-set-palette! cgm-device (pg-current-palette WIN-device))
	    (window-manager "vhardcopy" cgm-device res)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PROCESS-HC-ARGS - process the arguments to an HC command and make
;                 - a uniform return list of values containing
;                 - (color mode res)

(define (process-hc-args args)
   (if args
       (let* ((color (memv 'color args))
	      (land (memv 'landscape args))
	      (res (list-ref args (- (length args) 1))))
	 (list (if color "COLOR" "MONOCHROME")
	       (if land  "LANDSCAPE" "PORTRAIT")
	       (if (number? res) res 0)))
       (list "MONOCHROME" "PORTRAIT" 0)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HARDCOPY-WINDOW / HC / HCW - draw the contents of the current window to
;                 to all open hardcopy devices

(define-macro (hardcopy-window . rest)
    "HARDCOPY-WINDOW - Draw the current window to all open hardcopy devices.
                       Color output is indicated by an optional argument.
                       Landscape mode is indicated by an optional argument.
                       The optional resolution scale factor is an integer
                       factor by which the resolution will be decreased
                       below the full resolution of the device (this
                       argument must appear last in the argument list).
     Usage: hardcopy-window [color | monochrome] [portrait | landscape] [resolution-scale-factor]
     Examples: hardcopy-window
               hc color 8"

    (interactive off)
    (let* ((psflag (ps-flag))
	   (cgmflag (cgm-flag))
	   (mpegflag (mpeg-flag))
	   (jpegflag (jpeg-flag))
	   (pngflag (png-flag))
	   (args (process-hc-args rest))
	   (color (list-ref args 0))
	   (mode (list-ref args 1))
	   (res (list-ref args 2))
	   (type (sprintf "%s %s" color mode)))

      (if (= jpegflag 1)
	  (begin
	    (if (not jpeg-device)
		(begin
		  (set! jpeg-device
			(pg-make-device "JPEG"
					type))
		  (pg-open-device jpeg-device
				  (window-origin-x-jpeg) (window-origin-y-jpeg)
				  (window-width-jpeg) (window-height-jpeg))))
	    (pg-set-palette! jpeg-device (pg-current-palette WIN-device))  
            (pg-set-data-id-flag! jpeg-device (cond ((boolean? data-id-save)
							     0)
							    ((integer? data-id-save)
							     data-id-save)
							    (else
							     1)))
	    (window-manager "whardcopy"
			    jpeg-device res current-window)))

      (if (= pngflag 1)
	  (begin
	    (if (not png-device)
		(begin
		  (set! png-device
			(pg-make-device "PNG"
					type))
		  (pg-open-device png-device
				  (window-origin-x-png) (window-origin-y-png)
				  (window-width-png) (window-height-png))))
	    (pg-set-palette! png-device (pg-current-palette WIN-device))  
            (pg-set-data-id-flag! png-device (cond ((boolean? data-id-save)
							     0)
							    ((integer? data-id-save)
							     data-id-save)
							    (else
							     1)))
	    (window-manager "whardcopy"
			    png-device res current-window)))

      (if (= psflag 1)
	  (begin
	    (if (not ps-device)
		(begin
		  (set! ps-device
			(pg-make-device "PS"
					type))
		  (if (equal? color "COLOR")
		      (pg-set-color-type! ps-device "PS" color))
		  (pg-open-device ps-device
				  (window-origin-x-ps) (window-origin-y-ps)
				  (window-width-ps) (window-height-ps))))
	    (pg-set-palette! ps-device (pg-current-palette WIN-device))
            (pg-set-data-id-flag! ps-device (cond ((boolean? data-id-save)
							     0)
							    ((integer? data-id-save)
							     data-id-save)
							    (else
							     1)))
	    (window-manager "whardcopy"
			    ps-device res current-window)))
      (if (= mpegflag 1)
	  (begin
	    (if (not mpeg-device)
		(begin
		  (set! mpeg-device
			(pg-make-device "MPEG"
					type))
		  (pg-open-device mpeg-device
				  (window-origin-x-mpeg) (window-origin-y-mpeg)
				  (window-width-mpeg) (window-height-mpeg))))
	    (pg-set-palette! mpeg-device (pg-current-palette WIN-device))
            (pg-set-data-id-flag! mpeg-device (cond ((boolean? data-id-save)
							     0)
							    ((integer? data-id-save)
							     data-id-save)
							    (else
							     1)))
	    (window-manager "whardcopy"
			    mpeg-device res current-window)))
      (if (= cgmflag 1)
	  (begin
	    (if (not cgm-device)
		(begin
		  (set! cgm-device
			(pg-make-device "CGM"
					color))
		  (pg-open-device cgm-device
				  (window-origin-x-cgm) (window-origin-y-cgm)
				  (window-width-cgm) (window-height-cgm))))
	    (pg-set-palette! cgm-device (pg-current-palette WIN-device))
            (pg-set-data-id-flag! cgm-device (cond ((boolean? data-id-save)
							     0)
							    ((integer? data-id-save)
							     data-id-save)
							    (else
							     1)))
	    (window-manager "whardcopy"
			    cgm-device res current-window)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HISTOGRAM

(define (histogram start . things)
    "HISTOGRAM - Display given mappings from current viewport as histograms
     Usage: histogram on | off | left | right | center [<integer>]+"
    (if (> start 0)
	(begin (window-manager "vsattr" "PLOT-TYPE"
			       "int *" histogram-plot things)
	       (window-manager "vsattr" "HIST-START"
			       "int *" (- start 1) things))
	(window-manager "vsattr" "PLOT-TYPE"
			"int *" cartesian-plot things)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; LABEL-DRAWABLE

(define (label-drawable . x)
    "LABEL-DRAWABLE - label a drawable.
     Usage: label-drawable <label> [vname [wname]] <lsv-mapping-number>
     Examples: label-drawable \"foo\" 2"
    (window-manager "vlabel" x))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; VIEWPORT-AREA

(define (viewport-area . box)
    "VIEWPORT-AREA - set drawing area, in NDC, of the viewport
     Usage: viewport-area x-min x-max y-min ymax"
    (if box
        (let* ((f (car box)))
	  (if (or (boolean? f) (integer? f))
	      (set! viewport-area-box nil)
	      (begin
		(set! viewport-area-box box)
		(window-manager "vsattr" "VIEW-PORT"
				"double *" box
				nil))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; LEVELS

(define (levels . clevs)
    "LEVELS - set the contour levels
     Usage: levels [<clev>]*"
    (if clevs
	(begin
	  (window-manager "vsattr" "N-LEVELS"
			  "int *" (length clevs)
			  nil)
	  (window-manager "vsattr" "LEVELS"
			  "double *" clevs
			  nil))
	(begin
	  (window-manager "vsattr" "N-LEVELS")
	  (window-manager "vsattr" "LEVELS"))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; LIGHT-SOURCE

(define (light-source theta phi . things)
    "LIGHT-SOURCE - set the theta and phi angles for the light source
     Usage: light-source <theta> <phi> <mapping>*"
    (window-manager "vsattr" "THETA-LIGHT"
		    "double *" theta
		     (flatten-args things))
    (window-manager "vsattr" "PHI-LIGHT"
		    "double *" phi
		     (flatten-args things)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; LINK

(define-macro (link oldname newname)
    "LINK - create a symbolic link to a variable in the current file
     Usage: link oldname newname"
    (create-link current-file oldname newname))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; LIST-FILES

(define (list-files . x)
    "LIST-FILES - List the open files.
     Usage: list-files
     Examples: list-files
               list-files"
    (file-manager "files"))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; LIST-PALETTES

(define (list-palettes . window)
    "LIST-PALETTES - List the available palettes.
     Usage: list-palettes [window]
     Examples: list-palettes \"A\"
               list-palettes"
    (let* ((pals (window-manager "plist" window)))
      (if pals
	  (begin
	    (newline)
	    (display pals)
	    (newline)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; LIST-WINDOWS

(define (list-windows . x)
    "LIST-WINDOWS - List the open windows.
     Usage: list-windows
     Examples: list-windows
               list-windows"
    (window-manager "wlist"))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HIDE

(define (hide . things)
    "HIDE - don't plot the specified mappings.
     Usage: hide [<integer>]+"
    (window-manager "vsattr" "HIDDEN"
		    "int *" 1
		    (flatten-args things)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; UNHIDE

(define (unhide . things)
    "UNHIDE - do plot the specified mappings.
     Usage: unhide [<integer>]+"
    (window-manager "vsattr" "HIDDEN"
		    "int *" 0
		    (flatten-args things)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; LNCOLOR

(define (lncolor clr . things)
    "LNCOLOR - set the line color used in drawing the specified
     mappings.
     Usage: lncolor <color> [<integer>]+"
    (window-manager "vsattr" "LINE-COLOR"
		    "int *" clr
		    (flatten-args things)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; LNSTYLE

(define (lnstyle sty . things)
    "LNSTYLE - set the line style used in drawing the specified
     mappings.
     Usage: lnstyle <style> [<integer>]+"
    (window-manager "vsattr" "LINE-STYLE"
		    "int *" sty
		    (flatten-args things)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; LNWIDTH

(define (lnwidth wid . things)
    "LNWIDTH - set the line width used in drawing the specified
     mappings.
     Usage: lnwidth <width> [<integer>]+"
    (window-manager "vsattr" "LINE-WIDTH"
		    "double *" wid
		    (flatten-args things)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; LOAD-PALETTE - read a palette from a file and make it the current
;                palette

(define-macro (load-palette . name)
    "LOAD-PALETTE - Load a palette from a file.
     Usage: load-palette fname
     Example: load-palette foo.pal"
     (if (not (defined? current-window))
         (change-window "A"))
     (if (null? name)
         (printf nil "No file name specified\n")
         (pg-read-palette (cadr current-window) (car name))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; LOGICAL

(define (logical flag . things)
    "LOGICAL - Display given mappings from current viewport versus index
     Usage: logical on | off [<integer>]+"
    (if (> flag 0)
	(window-manager "vsattr" "RENDERING-TYPE"
			"int *" logical-plot
			(flatten-args things))
	(window-manager "vsattr" "RENDERING-TYPE"
			"int *" cartesian-plot
			(flatten-args things))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; LS - list the variables in the current file directory

(define-macro (ls . rest)
    "LS - List variables, links, and directories in current file directory.
          A selection pattern may be specified for the terminal pathname
          node. A type qualifier may also be specified. Also a variable
          length threshold can be supplied to find variables larger than
          a certain size.
     Usage: ls [<flags>] [<pattern> [<type> [<size>]]] 
            where the value of flags is:
               -a  list all entries, even hidden ones
               -R  list entries recursively
     Examples: ls
               ls -a
               ls -aR
               ls curve* char
               ls var?
               ls * Directory
               ls ../mydir
               ls /foo/bar double
               ls /foo/bar double 1000"
    (define flags nil)
    (define newrest rest)
    (define procargs nil)
    (if (pair? rest)
        (begin (set! procargs (process-flags rest))
               (set! flags (car procargs))
               (set! newrest (cadr procargs))))

    (if (pair? flags)
        (set! flags (car flags)))

    (let* ((ls-fnc (io-function current-file "ls")))
          (if (pair? newrest)
	      (if (pair? (cdr newrest))
		  (cond ((= (length newrest) 2)
			 (ls-fnc current-file flags
				 (car newrest)
				 (cadr newrest)))
			((= (length newrest) 3)
			 (ls-fnc current-file flags
				 (car newrest)
				 (cadr newrest)
				 (caddr newrest))))
		  (ls-fnc current-file flags (car newrest)))
	      (ls-fnc current-file flags))
	  #t))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; LS-ATTR - list the attributes in the current file

(define-macro (ls-attr . rest)
    "LS-ATTR - List the attributes in the current file.
     Usage: ls-attr"
    (if (null? rest)
	(ls-attr-pdb current-file)
	(ls-attr-pdb current-file (car rest)))
    #t)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; LST - list the mappings in the work space

(define-macro (lst . rest)
    "LST - List the labels of mappings in the work space.
           A selection pattern may be specified.
     Usage: lst [<pattern>]
     Examples: lst
               lst ?*d
               lst *foo*"
    (apply display-menu (cons nil rest)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; LIST-MAPPINGS-IN-VIEWPORT

(define (list-mappings-in-viewport . info)
    "LIST-MAPPINGS-IN-VIEWPORT - List the mappings or images associated with a viewport.
     Usage: list-mappings-in-viewport [vname [wname]]"
    (window-manager "vlist" info))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MAKE-AC-MAPPING-DIRECT - make a direct mapping

(define (make-ac-mapping-direct ran dom . rest)
   "MAKE-AC-MAPPING-DIRECT / AC - construct an arbitrarily connect
                                  mapping from component sets
    Usage: make-ac-mapping-direct ran dom [centering [label]]"
   (if rest
       (let* ((cent (list-ref rest 0))
	      (labl (list-ref rest 1)))
	 (data-manager "acdirect" ran dom cent labl))
       (data-manager "acdirect" ran dom nil nil)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MAKE-LR-MAPPING-DIRECT - make a direct mapping

(define (make-lr-mapping-direct ran dom . rest)
   "MAKE-LR-MAPPING-DIRECT / LR - construct a logically rectangular
                                  mapping from component sets
    Usage: make-lr-mapping-direct ran dom [centering [existence-map [label]]]"
    (if rest
	(let* ((cent (list-ref rest 0))
	       (emap (list-ref rest 1))
	       (labl (list-ref rest 2)))
	  (data-manager "lrdirect" ran dom cent emap labl))
	(data-manager "lrdirect" ran dom nil nil nil)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MAKE-LR-MAPPING-SYNTH - make a direct mapping by synthesis

(define (make-lr-mapping-synth ran dom . rest)
   "MAKE-LR-MAPPING-SYNTH / LRS - construct a logically rectangular mapping
                                  by synthesis from lower dimensional datasets
    Usage: make-lr-mapping-synth ran dom ..."
   (data-manager "lrproduct" ran dom (if rest (car rest) rest)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; ARRAY-ND-SET - make a set with the specified dimensions out of
;              - a linear array

(define-macro (array-nd-set var . dims)
   "ARRAY-ND-SET - Construct a set from an array which has specific
                   dimensions.
    Usage: array-nd-set var [<dim>]*
    Example: array-nd-set x 10 20"
   (data-manager "ndset" var dims))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MAKE-CP-SET - make a cartesian product set

(define-macro (make-cp-set . rest)
   "MAKE-CP-SET - Construct a set which is the Cartesian product of
                  a list of sets or arrays.  Unary operations can
                  optionally be applied to the components.
    Usage: make-cp-set [<item>]*
           item := <var> | <oper> <var>
    Example: make-cp-set x y z
             make-cp-set log10 x y exp z"
   (data-manager "cpset" rest))

(define (cps* . args)
  "Procedure version of cps macro"
  (apply cps args))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MARKER-SCALE

(define (marker-scale sca . things)
    "MARKER-SCALE - set the marker scale for the specified mappings
     Usage: marker-scale <scale> [<integer>]+"
    (window-manager "vsattr" "MARKER-SCALE"
		    "double *" sca
		    (flatten-args things)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MENU - list the images and mappings in the current file

(define-macro (menu . rest)
    "MENU - List the labels of mappings in the current file directory.
            A selection pattern may be specified.
     Usage: menu [<pattern>]
     Examples: menu
               menu ?*d
               menu *foo*"
    (plot-flag off)
    (apply display-menu (cons current-file rest)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MINMAX - Print the minimum value, maximum value, and their offsets

(define-macro (minmax . rest)
   "MINMAX - print the minimum and maximum values and their offsets from
             the beginning of an array in a file
    Usage: minmax name"
  (if (printable? (car rest))
      (let* ((data (pdb-read-numeric-data current-file (car rest)))
	     (extr (pm-array-extrema data)))
	(printf nil "\nmin: %12.5e  imin: %s\nmax: %12.5e  imax: %s\n"
		(car extr) (caddr extr) (cadr extr) (cadddr extr))
	extr)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MKDIR - create a new file directory

(define-macro (mkdir . rest)
    "MKDIR - Create a new file directory.
     Usage: mkdir [<directory-name>]
     Examples: mkdir /foo/bar
               mkdir ../baz
               mkdir mydir"
    (if (null? rest)
	(printf nil "\nNo directory name\n")
	((io-function current-file "mkdir") current-file (car rest))))

(define (mkdir* . args)
    "Procedure version of mkdir macro"
    (apply mkdir args))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MODE - set structure display modes

(define-macro (mode type)
    "MODE - Set the print mode for structures.
     Modes : full-path - the full path name is printed at each branch
                         e.g. foo.bar[3].baz
             indent    - indent 4 spaces at each branch (default)
             tree      - display as a tree (lines connecting branches)
             no-type   - turns off the display of types
             type      - displays the type of each item and branch
             recursive - indent each level of recursive structures
             iterative - number each level of recursive structures
     Usage: mode full-path | indent | tree |
                 no-type | type | recursive | iterative
     Example: mode no-type"
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

; MOVE - return the number of symbol table entries in the current file

(define (move vname wname . x)
    "MOVE - Move items from the current viewport and window to the named
     viewport and window.
     Usage: move <vname> <wname> <item>*
     Example: move V0 B 1 2 3
              move V2 A 3"
    (window-manager "vmove" vname wname x))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; N-ENTRIES - return the number of symbol table entries in the current file

(define-macro (n-entries . x)
    "N-ENTRIES - Print the number of symbol table entries in the current file.
     Usage: n-entries"

    (printf nil
	    "\nNumber of symbol table entries in current file: %d\n"
	    ((io-function current-file "n-entries") current-file)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; NXM - prep a window with N by M viewports

(define (nxm n m . rest)
    "NXM - Split the current window into N by M viewports and
           optionally give them labels.
     Usage: nxm n m [<viewport label>]
     Examples: nxm 2 3
               nxm 2 2 \"V1\" \"V2\" \"V3\" \"V4\""

   (interactive off)
   (let* ((nm (* n m))
	  (inter 0.01)
	  (intrh (* 0.5 inter))
	  (flag (and rest (<= nm (length rest))))
	  (dx (- (/ 1.0 n) intrh))
	  (dy (- (/ 1.0 m) intrh)))

      (define (make-view i j)
	  (let* ((k (+ i (* n j)))
		 (label (if flag (list-ref rest k) (sprintf "V%d" k)))
		 (xmin (+ (/ i n) intrh))
		 (ymin (+ (/ j m) intrh)))
	      (cv label xmin ymin dx dy)))

      (define (view-row i j)
          (if (< 0 i)
	      (begin (make-view (- i 1) (- j 1))
		     (view-row (- i 1) j))))

      (define (view-all j)
	  (if (< 0 j)
	      (begin (view-row n j)
		     (view-all (- j 1)))))

; if no current-window create one, else close any existing vports
      (if (or (not (defined? current-window))
              (not current-window))
          (cw (sprintf "%dx%d" n m))
	  (let* ((vports (window-viewports current-window)))
	        (if vports
		    (for-each (lambda (x)
				      (window-manager "vclose" (car x)))
			      vports))))
                       
      (pg-set-text-font! (window-device current-window)
			 "helvetica" "medium" 8)
      (view-all m)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PLANE - analog to ULTRA "line" command

(define-macro (plane . lst)
    "PLANE - Create and draw a hyper plane.
     Usage: plane c0 (c1 x1min x1max npts1) ...
     Examples: plane 1.0 (2.0 -5.0 5.0 100)
               plane 0.0 (2.0 -5.0 5.0 10) (0.5 -5.0 5.0 10)"
    (let* ((f (apply hyper-plane lst)))
;          (display-mapping* f)
	  f))

(define (plane* . lst)
  (apply plane lst))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SAVE-PALETTE - write a pgs palette to a disk file

(define-macro (save-palette pname fname)
    "SAVE-PALETTE - Write palette pname out to disk file fname.
     Usage: save-palette pname fname
     Example: save-palette foo foo.pal"
     (if (not (null? pname))
         (if (not (null? fname))
             (if (not (null? current-window))
                 (pg-write-palette (cadr current-window) pname fname))))
     #t)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SCALE-FILE-VARIABLE - write a new variable into the current file which
;                     - has name = alias and values scaled by scale_factor

(define-macro (scale-file-variable name aliasname scale_factor)
    "SCALE-FILE-VARIABLE - Scale variable name by scale_factor and
                           write it into the current file with name
                           aliasname.
     Usage: scale-file-variable name aliasname scale_factor
     Examples: scale-file-variable foo two_foo 2.0
               scale-file-variable bar -bar -1.0"
    (let* ((syment  (read-syment current-file name))
	   (symlist (pdb->list syment))
	   (dims    (cddr symlist))
	   (type    (car symlist))
	   (nameout (sprintf "@#%s#@" aliasname))
	   (vals    (build-array* name))
	   (typeout (sprintf "%s" type))
	   (args    (list nameout typeout)))
      (for-each
       (lambda (x)
	 (set! args (append (list (* scale_factor x)) args)))
       vals)
      (set! args (reverse args))
      (apply define-file-variable args)

      (let* ((syment    (read-syment current-file nameout))
	     (symlist   (pdb->list syment))
	     (address   (cadr symlist))
	     (symout    (list address typeout aliasname current-file)))
	(for-each
	 (lambda (x)
	   (set! symout (append (list x) symout)))
	 dims)
	(set! symout (reverse symout))
	(apply write-syment symout)))) 

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; OVERLAY - turn on/off overlay mode

(define (overlay . flag)
   "OVERLAY - get/set the overlay flag.  If on, the overlay flag plots
              all mappings in the current window viewport in a common
              graphical viewport.  Otherwise the mappings will have their
              own graphical viewport and won't in general register with
              one another unless they have the same rendering mode.
    Usage: overlay [off | on]"
    (if flag
	(let* ((f (car flag)))
	      (set! overlay-flag (cond ((boolean? f)
					f)
				       ((integer? f)
					(not (= f 0)))
				       (else
					#t)))))
    (if overlay-flag
        (begin (set! viewport-area-save viewport-area-box)
               (if (null? viewport-area-box)
                   (set! viewport-area-box overlay-box)))
        (set! viewport-area-box viewport-area-save))

    overlay-flag)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PDB-COPY - make a PDB file whose contents match the current file

; GOTCHA - this macro only copies variables in the current directory

(define-macro (pdb-copy . rest)
    "PDB-COPY - Make a new PDB file whose contents match the current file
                Deprecated.  Use the cp command instead
     Usage: pdb-copy <filename>"
    (if rest
	(cp* '-R '* (car rest))
	(printf nil "\nNo output file specified\n")))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PLOT - plot one variable against another

(define-macro (plot . args)
    "PLOT - Plot one variable against another.
     Usage: plot [<func>] <range> [<domain>]
     Examples: plot log10 y x
               plot log10 z (cps log10 x log10 y)
               plot (nds x 10 15)
               plot y x
               plot z"
    (let* ((a1 (list-ref args 0))
	   (a2 (list-ref args 1))
	   (a3 (list-ref args 2))
	   (v1 (if (printable? a1) a1 (eval a1)))
	   (v2 (if (printable? a2) a2 (eval a2)))
	   (v3 (if (printable? a2) a3 (eval a3)))
	   (func (if (procedure? v1) v1 nil))
	   (ran (if func v2 v1))
	   (dom (if func v3 v2)))
      (plot-hand (eval func) ran (if (pair? dom) (eval dom) dom))))

(define (plot* . args)
  "Procedure version of plot macro"
  (apply plot args))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PLOTAC - plot an arbitrarily connected mapping
;          this command and connect-map need to be generalized

(define-macro (plotac . vars)
    (display-mapping* (apply connect-map vars)))


; CONNECT-MAP - make a mapping from a dump with the specified connectivity
;               this will be moved to pdbvdat.scm later

(define-macro (connect-map . vars)
    (let* ((cnnct (pdb-read-numeric-data current-file "zones"))
	   (dom (pm-connection->ac-domain current-file "x" "y"
					  "n_zones"
					  "n_nodes"
					  cnnct)))
      (ac vars dom zone)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TABLE - write all or part of the named variable or
;         structure member to an opened textfile

; the table macro

(define-macro (table name)
    "TABLE - Write out all or part of a variable or
             member structure to an opened textfile
             To write part of a variable or member qualify
             the name with index expressions whose parts
             are in one of the three forms:
               <index>
               <index-min>:<index-max>
               <index-min>:<index-max>:<increment>
             Only the first form may be used to qualify
             variables or terminal structure members with
             embedded pointers and non-terminal members.
     Usage: table <variable> | <structure-member>
     Examples: table Mapping2
               table Mapping4.domain.elements
               table Mapping2.range.elements[1]
               table a[5,10:20,1:8:3]
               table a.b[3].c[5,10:20,1:8:3]"
    (table-aux name))

; the table procedure
(define (table* name)
    "Procedure version of table macro"
    (table-aux name))

; the auxiliary table procedure
(define (table-aux name)
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
			   ((file-variable? current-file name #f)
					; last arg is #f so index expr errors
					; can be reported as such
			    (read-pdbdata current-file name))
			   (else
			    (printf nil "Variable %s unknown\n" name)))))
	  (if data
	      (wr-to-textfile data display-precision))))
    #f)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PRINT-MAPPING / PM - print the specified mappings

(define-macro (print-mapping . x)
    "PRINT-MAPPING / PM - Print the specified mappings.
                   Mappings are referenced by the numbers displayed
                   by the menu command.
     Usage: print-mapping <mapping-list>
     Examples: print-mapping 3 4"

    (define-macro (print-item item)
	(let* ((mapping-data
		(if (pair? item)
		    (let* ((mapping (eval (cadr item))))
		      (if (pg-image? mapping)
			  (pg-image->pdbdata mapping)
			  (pm-mapping->pdbdata mapping)))
		    ((io-function current-file "map-print")
		     current-file item))))
	  (if (not (pdbdata? mapping-data))
	      (printf nil "%s is not a valid mapping\n" item)
	      (begin 
		(newline)
		(print-pdb nil (list mapping-data display-precision))
		#t))))

    (if x (for-each print-item x)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PWD - print the current file directory

(define-macro (pwd . rest)
    "PWD - Print the current file directory.
     Usage: pwd"
    (printf nil "\n%s\n" ((io-function current-file "pwd") current-file)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; RANM

(define (ranm . args)
    "RANM - Select plotting limits for the range of a mapping.
            If invoked with no limit values the range
            will default to that implied by the data set.
     Usage: ranm [<viewport> [<window>]] <mapping> <x1_min> <x1_max> ...
     Examples: ranm 1 -5.0 10.0 10 20
               ranm 1"
    (window-manager "mrlimit" args))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; RANV

(define (ranv . args)
    "RANV - Select plotting limits for the range of a viewport.
            If invoked with no limit values the range
            will default to that implied by the data sets.
     Usage: ranv [<viewport> [<window>]] <x1_min> <x1_max> ...
     Examples: ranv -5.0 10.0 10 20
               ranv"
    (window-manager "vrlimit" args))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; REFMESH

(define (refmesh flag . things)
    "REFMESH - turn a reference mesh on or off
     Usage: refmesh on | off"
    (ref-mesh flag))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; REFMESHCOLOR

(define (refmeshcolor color . things)
    "REFMESHCOLOR - set the line color for the reference mesh
     Usage: refmeshcolor color"
    (ref-mesh-color color))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SAVE-MAPPING

(define-macro (save-mapping file . args)
    "SAVE-MAPPING / SM - Save mappings from the current viewport to a file.
                  Mappings are referenced by the numbers displayed
                  by the lsv command. If the mapping list is *,
                  save all mappings in the current viewport.
     Usage: save-mapping file <mapping-list>
     Examples: save-mapping foo *
               save-mapping bar 1 5"

    (file-manager "freplicate" "viewport" nil file args))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SCATTER

(define (scatter mrk . things)
    "SCATTER - display given mappings from current viewport as scatter plots
     Usage: scatter on | off | plus | star | triangle [<integer>]+"
    (if (> mrk 0)
	(begin (window-manager "vsattr" "SCATTER"
			       "int *" 1
			       (flatten-args things))
	       (window-manager "vsattr" "MARKER-INDEX"
			       "int *" (- mrk 1)
			       (flatten-args things)))
	(window-manager "vsattr" "SCATTER"
			"int *" 0
			(flatten-args things))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SET - set array display parameters

(define param-list '(array-length
		     bits-precision
		     decimal-precision
		     line-length))

(define-macro (set name value)
    "SET - Set the value of a control parameter.
           If the single argument, all, is given show the values
           of all parameters.
     Parameters:  line-length       - the number of array elements per line
                  array-length      - arrays shorter than value have each
                                      element labeled individually
                  bits-precision    - the number of bits displayed
                  decimal-precision - the number of digits displayed
     Usage: set line-length | array-length |
                bits-precision | decimal-precision <value>
     Usage: set all
     Examples: set line-length 3
               set decimal-precision 6
               set all"
    (cond ((null? name)
	   (printf nil "\nNo display parameter name specified\n"))

	  ((eqv? name 'all)
           (for-each '(lambda-macro (x)
			 (printf nil "   %-18s" x)
			 (apply set (list x)))
		     param-list))

	  ((eqv? name 'array-length)
	   (if (interactive?)
	       (printf nil "   %s\n" (set-switch 3 value))
	       (set-switch 3 value)))

	  ((eqv? name 'line-length)
	   (if (interactive?)
	       (printf nil "   %s\n" (set-switch 4 value))
	       (set-switch 4 value)))

	  ((eqv? name 'bits-precision)
	   (if value
	       (set! display-precision value))
	   (if (interactive?)
	       (printf nil "   %d\n" display-precision value)))

	  ((eqv? name 'decimal-precision)
	   (if value
	       (set! display-precision
		     (truncate (abs (/ (- value 1) (log 2)))))
	       (set! value (truncate (+ 1.5 (* display-precision (log 2))))))
	   (if (interactive?)
	       (printf nil "   %d\n" value)))

	  (else
	   (printf nil "\n%s is not a valid display parameter\n" name))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SHOW-LEGENDS

(define (show-legends . args)
    "SHOW-LEGENDS - display the legends for the specified drawables only.
     This spreads them out rather than plotting one on top of another.
     Usage: show-legends [<integer>]+"
    (window-manager "lplace" args))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SHOW-PALETTES - show available palettes

(define (show-palettes . window)
    "SHOW-PALETTES - Show the available palettes and make selection current
     Usage: show-palettes [window]
     Examples: show-palettes
               show-palettes \"B\""
    (window-manager "pshow" window))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SPAN

(define (span mn mx . npts)
    "SPAN - create a line with slope 1 and intercept 0
     Usage: span <xmin> <xmax>
     Example: span 0 10"
    (plane* 0.0 (list 1.0 mn mx (if npts (car npts) 100))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; STRUCT - display the structure of a data type

(define-macro (struct name)
    "STRUCT - Describe the named data type.
     Usage: struct <data-type>
     Examples: struct double
               struct PM_mapping"
    (newline)
    (if (null? name)
	(printf nil "No struct name specified\n")
	(begin (print-pdb nil (read-defstr* current-file name))
	       #t)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SYNONYM - define synonyms for the given command

(define-macro (synonym name . syn)
    "SYNONYM - Define synonyms for the given function.
     Usage: synonym <func> [<synonym> ...]
     Examples: synonym change-file cf"
    (define-macro (defsyn x)
	(apply define-global (list x name)))
    (if syn
	(for-each defsyn syn)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TYPES - list the data types in the current file

(define-macro (types . args)
    "TYPES - List the types in the current file.
     Usage: types"
    (newline)
    (display (list-defstrs current-file))
    (newline)
    #t)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; UPDATE-WINDOW

(define (update-window . windows)
    "UPDATE-WINDOW - Update all viewports in specified windows.
     Usage: update-window <window>*"
    (window-manager "wupdate" windows))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SET-VECTOR-ATT - set vector attributes

(define (set-vector-att attr value)
    "SET-VECTOR-ATT - Set vector plotting attributes.
     Usage: set-vector-att scale | headsize | color value"

     (cond ((eqv? attr scale)
            (set! vect-scale value))
           ((eqv? attr headsize)
            (set! vect-headsz value))
           ((eqv? attr color)
            (set! vect-color value))))
     
;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; VIEW-ANGLE

(define (view-angle theta phi chi)
    "VIEW-ANGLE - Set the three Euler viewing angles.
         The view angle is a rotation by <phi> about the z axis
         starting from the negative y axis counterclockwise
         followed by a rotation by <theta> about the x' axis
         counterclockwise and followed by a rotation by
         <chi> about the z'' axis again starting from the negative
         y'' axis counterclockwise.
     Usage: view-angle <theta> <phi> <chi>"
    (pg-set-view-angle! theta phi chi))

;--------------------------------------------------------------------------

; NOTE: these are deprecated
; the functionality is covered by fopen, print-pdb, and fclose

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; OPEN-TEXTFILE / OT - open a text file

(define textfile nil)

(define-macro (open-textfile file . arg)
    "OPEN-TEXTFILE / OT - Open the text file.
     Usage: open-textfile <filename>
     Example: open-textfile foo"
    (plot-flag off)
    (let* ((name (print-name file)))
          (set! textfile (fopen name "w"))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; CLOSE-TEXTFILE / CT - close a text file

(define-macro (close-textfile . arg)
    "CLOSE-TEXTFILE / CT - Close the opened text file.
     Usage: close-textfile
     Example: close-textfile"
    (plot-flag off)
    (fclose textfile)
    (set! textfile nil)
    #t)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WR-TO-TEXTFILE - write to the current textfile

(define (wr-to-textfile . arg)
    "WR-TO-TEXTFILE - Write to the current text file.
     Usage: wr-to-textfile <pdb object>
     Example: wr-to-textfile (read-pdbdata current-file \"foo\")"
    (apply print-pdb (cons textfile arg))
    #t)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

