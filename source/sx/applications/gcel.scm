;
; GCEL.SCM - glyph plot table visualization routines
;
; Source Version: 4.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
;

(define tdims nil)
(define labelsave "")
(define sort_col "")
(define labelflag 0)
(define batch_multiplot 1)
(define tablename nil)
(define seqnum 0)
(define first #t)
(define multi_colnum 0)
(define vp_num 0)
(define vp_id "")
(define multisort_flag #f)
(define mpeg "mpg")
(define MPEG "mpg")
(define ps "ps")
(define PS "ps")

(autoplot off)
(plot-flag off)
(plot-date off)
(labels off)

; set the default palette to be "rgb"
(change-palette "rgb")

; debug
;(define (dl)
;   (printf nil "dl called\n"))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GREAD-TABLE - read an ascii table and save the returned dimensions

(define-macro (gread-table tfile)
    "GREAD-TABLE - Read an ASCII table and save the dimensions for later plotting
     Usage: gread-table <file>
     Example: gread-table myfile"
    (let* ((tname (print-name tfile)))
          (set! tablename tname)
          (set! tdims (read-table* tname 1 1))

	  (col-normalize)))
           
;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (make-gp-domain nx ny)
    (pm-make-set "{x,y}" (list (+ nx 1) (+ ny 1))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MAKE-GP-RANGE - make the range data for a glyph plot
;               -   NAME   plot label
;               -   DIMS   dimensions of range elements - usualy just 1
;               -   TR     number of table rows
;               -   TC     number of table columns
;               -   CMP    built up list of components (output)
;               -   IC     current component index
;               -   NC     number of components expected
;               -   CO     column offset - used when FLST is nil
;               -   FLST   list of columns to use - most general

(define (make-gp-range name dims tr tc cmp ic nc co flst)
    (cond ((pair? flst)
	   (make-gp-range name dims tr tc
			  (cons (list (car flst) tr tc) cmp)
			  (+ ic 1) nc co (cdr flst)))
	  ((< ic nc)
	   (make-gp-range name dims tr tc
			  (cons (list (+ ic co) tr tc) cmp)
			  (+ ic 1) nc co flst))
	  (else
	   (apply table->pm-set (append (list name dims) (reverse cmp))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GCELL - generate a 2-d plot of the mapping created referencing
;       - all the points of the current table

(define-macro (gcell nx ny lbl gx gy . flds)
    "GCELL - Make a 2-D plot glyph plot of the current table.
     Usage: gcell <nx> <ny> <lbl> <gx> <gy> [<flds>]
              nx    width of plot in glyphs
              ny    height of plot in glyphs
              lbl   plot label
              gx    width of glyph in fields
              gy    height of glyph in fields
              flds  which table columns to use for fields
     Example: gcell 4 3 \"foo\" 3 3
              gcell 4 3 \"foo\" 2 3 3
              gcell 4 3 \"foo\" 2 2 (0 2 4 6)"
    (let* ((xa    (if flds (car flds)))
	   (co    (if (number? xa) xa 0))
	   (flst  (if (number? xa) nil xa))
	   (np    (* nx ny))
	   (nc    (* gx gy))
           (slbl  (print-name lbl))
	   (dom   (make-gp-domain nx ny))
	   (ran   (make-gp-range slbl (list 1)
				 (list-ref tdims 0) (list-ref tdims 1)
				 nil 0 nc co flst))
           (mp    (pm-make-mapping dom ran zone)))

	  (pm-set-set-attribute! ran "GLYPH" "int *" (list gx gy))

          (set! labelsave slbl)
          (dl)
          (if current-window
              (clear-ann))
          (display-mapping* mp)
          (if first
              (begin
                 (vr fill-poly)
                 (set! first #f)))
          (label-the-plot)
          (wu)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; CLEAR-ANN - clear the column label annotations

(define (clear-ann)
    (let* ((dev (window-device current-window)))
          (rem-annotations dev)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define outfile "")

(define (unique_name root . type)
    (let* ((typesave  (sprintf "%s" (if type (car type) "ps"))))

          (set! seqnum 0)

; make sure the output file goes in the current directory
          (if (memv #\/ (string->list root))
	      (set! root (list->string (reverse (string->list
			  (strtok (list->string (reverse (string->list root))) "/"))))))

          (define (testname root type)
              (if (file? (sprintf "%s%d.%s" root seqnum type))
                  (begin
                     (set! seqnum (+ seqnum 1))
                     (testname root type))
                  (begin 
                     (if (eqv? type "ps")             
                         (set! outfile (sprintf "%s%d.%s" root seqnum type))
                         (set! outfile (sprintf "%s%d" root seqnum))))))

          (testname root typesave)
           outfile))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (m-get-args . args)
    (let* ((len     (if (car args) (length (car args)) 0))
           (type    (if (> len 0) (list-ref (car args) 0) "ps"))
           (nx      (if (= len 3) (list-ref (car args) 1) 1))
           (ny      (if (= len 3) (list-ref (car args) 2) 1)))
           (list type nx ny)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MULTISORT - sort the current table on each column
;           - creating a frame for each column

(define (multisort . rest)
    "MULTISORT - Make plots of the current table sorted on each of its columns
                 creating a frame for each column.
                 Hardcopy file type can optionally be specified.
                 Hardcopy file type can be either ps or mpeg.
                 The default output type is PostScript.
                 Output can optionally be plotted to a window with multiple
                 viewports.  Arguments nx and ny specify the number of viewports
                 in a row and column respectively.  For example setting nx to
                 3 and ny to 4 would result in a window with 12 viewports, arranged
                 in 3 rows and 4 columns.
     Usage: multisort [<hc_type> <nx> <ny>]
     Examples: multisort
               multisort ps
               multisort mpeg 3 3"
    (let* ((save-current (if current-window current-window nil))
           (ncol         (if tdims (cadr tdims) 0))
           (args         (m-get-args rest))
           (hc_type      (list-ref args 0))
           (nx           (list-ref args 1))
           (ny           (list-ref args 2))
           (n_per_page   (* nx ny))
           (hc_file      (unique_name tablename hc_type))
           (outfile      (if (eqv? hc_type "mpg")
                             (cw hc_file "COLOR" "MPEG" 0 0 256 256)
                             (cw hc_file "COLOR" "PS" 0 0 1 1)))
           (outdevice    (window-device current-window)))


           (define (setup-label colnum)
; this formats up a label and either saves it in labelsave where it will be plotted at the top of the
; plot as the title, or saves it in sort_col where it will be plotted at the top left of the plot axis
; under the column labels due to the fact that column labels conflict with the location of the title.
               (let* ((clabels   (col-labels)))
                     (if (= labelflag 1)
                         (begin
                            (set! sort_col (if clabels (sprintf "Sorted on: %s" (list-ref clabels colnum))
                                                    (sprintf "Sorted on: %d" colnum)))
                            (set! labelsave ""))
                            (set! labelsave (if clabels (sprintf "Sorted on: %s" (list-ref clabels colnum))
                                                     (sprintf "Sorted on: %d" colnum))))))

           (define (sort_plot cnum)
               (if (< cnum ncol)
                   (begin 
                      (set! vp_num (remainder cnum n_per_page))
                      (if (and (= vp_num 0) (> cnum 0))
                          (begin
                             (wu)
                             (pg-clear-window outdevice)
                             (nxm nx ny)))
                      (set! multi_colnum cnum)
                      (cv (sprintf "V%d" vp_num))
                      (dl 1)
                      (setup-label cnum)
                      (scol cnum #f)
                      (sort_plot (+ cnum 1)))
                   (begin
                      (wu)
                      (clw hc_file))))                            

           (show-highlight off)            
           (nxm nx ny)
           (set! multisort_flag #t)
           (sort_plot 0)
           (set! multisort_flag #f)
           (if save-current
               (begin
                  (cw (car save-current))
                  (wu)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (replot2d-aux label)
    (if tdims
        (let* ((d1 (car tdims))
               (d2 (cadr tdims))
               (tnpts (* d1 d2))
               (mp (table->pm-mapping label (list label nil)
				      (list label
					    (list 1)
					    (list 0 tnpts 1))
				      zone)))
              (if current-window
                  (clear-ann))
              (dl)
              (display-mapping* mp)
              (if first
                  (begin
                     (vr fill-poly)
                     (set! first #f)))
              (label-the-plot))))    

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; REPLOT2D - replot the data after manipulation using the
;          - original label passed to gcell

(define (replot2d label)
    (if tdims
        (begin
           (replot2d-aux label)
           (wu))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TDEL-COLUMN - delete a list of columns from the current table
;             - and replot the new mapping created from its data

(define (tdel-column . rest)
    "TDEL-COLUMN - Delete a list of columns from the current table and replot 
     Usage: tdel-column <list>
     Example: tdel-column 1 (sequence 3 5 1) 11"
    (let*  ((clist    (tlflatten-list rest))
            (carray   (apply list->pm-array clist)))
 
        (dl)
        (set! tdims (del-column carray))
        (replot2d labelsave)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; flatten a list of arguments (i.e. return a list containing
;      all the elements of the input list, which may itself
;      include lists)

(define returnlist nil)

(define (tlflatten-list inlist)
   (set! returnlist nil)
   (tlprocess-args inlist))

(define (tprocess-list arglist)
   (if arglist
       (begin
          (set! returnlist (append returnlist (list (car arglist))))
          (tprocess-list (cdr arglist)))
       returnlist))

(define (tlprocess-args args)
   (if args   
       (if (pair? (car args))
           (begin
              (tprocess-list (car args))
              (tlprocess-args (cdr args)))
           (begin
              (set! returnlist (append returnlist (list (car args))))
              (tlprocess-args (cdr args))))
        returnlist))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TSORT-ON-COLUMN - sort the current table on a column
;                 - and replot the new mapping

(define (tsort-on-column n . update)
    "TSORT-ON-COLUMN - Sort the current table on a column and replot 
                       Optional update flag allows control over frame
                       advance.  Default is to do frame advance.
     Usage: tsort-on-column <n> [<update>]
     Example: tsort-on-column 1"
     (let* ((page_update (if update (car update) #t)))
           (dl)
           (set! tdims (sort-on-column n))
           (if page_update 
               (replot2d labelsave)
               (replot2d-aux labelsave))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; CNORMALIZE - normalize the current table columnwise and replot 

(define (cnormalize)
    "CNORMALIZE - normalize the current table columnwise
     Usage: cnormalize"
     (dl)
     (set! tdims (col-normalize))
     (replot2d labelsave))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (plot-limits)
    (list 0 (list-ref tdims 1) 0 (list-ref tdims 0)))

(define (column-labels . flag)
    "COLUMN-LABELS - Turn column labeling on/off
     Usage: column-labels [<on off>]"
 
   (if flag
        (if (= (car flag) 1)
            (begin (set! labelflag 1)
                   (if current-window
                       (replot2d labelsave)))
            (begin (set! labelflag 0)
                   (if current-window
                       (replot2d labelsave)))))
     labelflag)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (label-columns)
    (if (= labelflag 1)
        (begin (define xincr  .02)
               (define yincr  .02)
               (define yaxoff .025)
               (define plim (plot-limits))

               (let* ((clabels (col-labels))
                      (xstart  (car plim))
                      (xend    (cadr plim))
                      (ystart  (cadddr plim))
                      (dev     (window-device current-window))
                      (dummy0  (set-world-coordinates! dev plim))
                      (dummy2  (viewport-update))
                      (lmin    (pg-world->normalized dev xstart ystart))
                      (x1      (list-ref lmin 0))
                      (x2      (+ x1 xincr))
                      (y1      (+ yaxoff (list-ref lmin 1)))
                      (y2      (+ y1 yincr))
                      (clr      black))

                     (define (label-column lab cnum xmin ymin xmax ymax)
                         (if (and lab (< cnum xend))
                             (let* ((lmin_next  (pg-world->normalized dev (+ cnum 1) ystart))
                                    (x1_next    (list-ref lmin_next 0))
                                    (x2_next    (+ x1_next xincr)))
                                   (add-annotation dev lab clr xmin xmax ymin ymax -10 90)
                                   (label-column (list-ref clabels (+ cnum 1)) (+ cnum 1) x1_next y1 x2_next y2))))

                    (define (label-sort)
                        (add-annotation dev sort_col clr x1 x2 (- y1 (* yaxoff .85)) (- y2 (* yaxoff .85))))
                    
                    (rem-annotations dev)
                    (if multisort_flag
                        (label-sort))
                    (label-column (list-ref clabels xstart) xstart x1 y1 x2 y2)))))

(define (label-the-plot)
    (label-columns))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (tdomv . rest)
    "TDOMV - Wrapper for domv that allows for correct label generation and replot
     Usage:  tdomv <x_min x_max y_min y_max>
     Example: tdomv 0 9 3 8
              tdomv"
   (if rest
       (if (eqv? (length rest) 4)
           (let* ((d1 (list-ref rest 0))
                  (d2 (list-ref rest 1))
                  (d3 (list-ref rest 2))
                  (d4 (list-ref rest 3)))
                 (domv d1 d2 d3 d4)
                 (replot2d labelsave)))
        (begin
           (domv)
           (replot2d labelsave))))
               
;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (help-gcell)
    (printf nil "\nGCELL commands:\n")
    (printf nil "    gread-table     - Read a table from disk into current table\n")
    (printf nil "    column-labels   - Turn column labeling on/off\n")
    (printf nil "    gcell           - Plot the current table as a poly-fill rendering\n")
    (printf nil "    tdel-column     - Delete a column from the current table and replot\n")
    (printf nil "    tsort-on-column - Sort the current table on a column and replot\n")
    (printf nil "    tdomv           - Domv for gcell\n")
    (printf nil "    cnormalize      - Normalize the current table columnwise and replot\n")
    (printf nil "    print-column    - Print column n from the current table\n")
    (printf nil "    multisort       - Plot the current table sorted on each of its columns--output to PostScript or Mpeg file\n")
    (printf nil "\n\nSynonyms:\n")
    (printf nil "    rtab  - gread-table\n")
    (printf nil "    dcol  - tdel-column\n")
    (printf nil "    pcol  - print-column\n")
    (printf nil "    scol  - tsort-on-column\n")
    (printf nil "    cnorm - cnormalize\n")
    (printf nil "    msort - multisort\n"))
    
;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(synonym gread-table rtab)
(synonym multisort msort)
(synonym tdel-column dcol)
(synonym tsort-on-column scol)
(synonym cnormalize cnorm)
(synonym print-column pcol)     
 

