; 
; PDBVPLT.SCM - window manager routines for PDBView
; 
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
; 

(define HUGE 1.0e100)
(define window-manager-table #f)
(define window-list nil)
(define must-clear-viewport #t)

; NOTE: the first one on the list will be taken as the default
;       rendering for the dimensionality
(define domain-1d (list cartesian-plot polar-plot insel-plot))
(define domain-2d (list contour image dvb fill-poly wire-frame shaded points mesh vect))
(define domain-3d (list shaded mesh))

(define current-window nil)
(define WIN-device #f)
(define ps-device #f)
(define cgm-device #f)
(define mpeg-device #f)
(define jpeg-device #f)
(define png-device #f)

; IMPORTANT NOTE
; Used by hardcopy? predicate to determine if a device is a hardcopy device.
; For our purposes a hardcopy device is one of PS, PNG, CGM, JPEG, or MPEG
; iff it wasn't explicitly opened via a CW command.
; Devices for HC are to be thought of as secondary or auxilliary devices
; as opposed to ones open with CW which are opened with the intention of
; being drawn to and they come under the WU command instead of the HC
; command.  See PDBVTEST.SCM as example of the intention here.

(define hard-copy-device-types (list (cons "PS"   ps-flag)
				     (cons "CGM"  cgm-flag)
				     (cons "PNG"  png-flag)
				     (cons "JPEG" jpeg-flag)
				     (cons "MPEG" mpeg-flag)))

(define WINDOW   -5)   ; clear window
(define VIEWPORT -6)   ; clear PGS viewport

(define window-index 65)
(define label-number 1)

(define horizontal    -10)
(define vertical      -11)

(define window-highlight-color black)
(define viewport-highlight-color black)
(define un-highlight-color dark-gray)

(define default-window-width   0.4)
(define default-window-height  0.4)

(if (not (defined? look))
    (define look "original"))

(ld pdbvthemes.scm)

(cond ((string=? look "basis")
       (basis-look))
      ((string=? look "ultra")
       (ultra-look))
      (else
       (original-look)))

(define contour-box-size    1)
(define image-box-size      2)
(define mesh-box-size       2)
(define dvb-box-size        2)
(define fill-poly-box-size  2)
(define points-box-size     2)
(define vector-box-size     3)
(define overlay-box-size    2)

(define default-palette  "spectrum")
(define default-vr       nil)

;--------------------------------------------------------------------------

;                          HELPER ROUTINES

;--------------------------------------------------------------------------

; VIEW-UP-G - viewport-update helper for graphs

(define (view-up-g device lst i dextr rextr info)
    (if lst
	(let* ((g   (car lst))
	       (rst (cdr lst)))
	      (pg-drawable-info (car g) "identifier" i)
	      (vp-update-drawable device pg-draw-graph
				  g dextr rextr info)
	      (if rst
		  (view-up-g device rst (+ i 1) dextr rextr info)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; VIEW-UP-S - viewport-update helper for sets

(define (view-up-s device lst i dextr info)
    (if lst
	(let* ((g (car lst))
	       (rst (cdr lst)))
	      (vp-update-drawable device pg-draw-domain
				  g dextr nil info)
	      (if rst
		  (view-up-s device rst (+ i 1) dextr info)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; VIEW-UP-I - viewport-update helper for images

(define (view-up-i device lst i dextr box)
    (if lst
	(let* ((g (car lst))
	       (rst (cdr lst)))
	      (vp-update-drawable device pg-draw-image
				  g dextr nil box)
	      (if rst
		  (view-up-i device rst (+ i 1) dextr box)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DRAW-ALL-IN-VIEWPORT - viewport update helper with all information
;                      - completely determined

(define (draw-all-in-viewport window device port shape
			      gr ms im
			      dextr rextr
			      cp info swin sdev)
    (set! current-window window)
    (set! WIN-device device)
    (pg-make-device-current WIN-device)

    (if must-clear-viewport
	(pg-clear-viewport device port))

    (if (eqv? cp port)
	(highlight-window-viewport
	 window
	 viewport-highlight-color
	 port)
	(highlight-window-viewport
	 window
	 un-highlight-color port))

    (if (eqv? window current-window)
	(highlight-window window window-highlight-color)
	(highlight-window window un-highlight-color))

    (setup-viewport device shape)
    (pg-set-finish-state! device off)
    (pg-set-clear-mode! device off)

    (if overlay-flag
	(begin (set-viewport-area! gr ms im)
	       (window-manager "vsattr" "VIEW-PORT"
			       "double *" viewport-area-box
			       nil)))

; draw the graphs
    (view-up-g device gr 1 dextr rextr info)

; draw the sets/meshes
    (view-up-s device ms 1 dextr info)

; draw the images
    (view-up-i device im 1 dextr viewport-area-box)

    (pg-set-clear-mode! device WINDOW)
    (pg-update-view-surface device)

    (set! current-window swin)
    (set! WIN-device sdev)
    (pg-make-device-current WIN-device))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; VIEWPORT-UPDATE - update the viewport following a command
;                 - NOTE: this is called out of the C code in SX_plot

(define (viewport-update . args)
    (let* ((x       (view-prep-args args))
	   (vname   (list-ref x 0))
	   (wname   (list-ref x 1))
	   (port    (get-viewport vname wname)))
      (if port
	  (let* ((window  (viewport-window port))
		 (cp      (current-viewport window))
		 (device  (window-device window))
		 (swin    current-window)
		 (sdev    WIN-device)
		 (shape   (viewport-shape port))
		 (info    (viewport-rendering port))
		 (rlimits (viewport-range port))
		 (dlimits (viewport-domain port))
		 (gr      (viewport-graphs port))
		 (ms      (viewport-meshes port))
		 (im      (viewport-images port))
		 (rextr   (if rlimits
			      rlimits
			      (get-range-extrema (append gr (append im ms))
						 nil)))
		 (dextr   (if dlimits
			      dlimits
			      (get-domain-extrema (append gr (append im ms))
						  nil))))

	    (if (ok-to-draw? device)
		(draw-all-in-viewport window device port shape
				      gr ms im
				      dextr rextr
				      cp info swin sdev))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; HARDCOPY? - return TRUE iff the device is HARDCOPY

(define (hardcopy? dev)
    (interactive off)
    (if dev
	(let* ((props (pg-device-properties dev))
	       (as    (assoc (list-ref props 0)
			     hard-copy-device-types))
	       (name  (if as (car as)))
	       (fnc   (if as (cdr as))))
	  (and fnc (= (fnc) 1)))))

(define (hardcopy? dev)
    (interactive off)
    (if dev
	(let* ((props (pg-device-properties dev))
	       (as    (assoc (list-ref props 0)
			     hard-copy-device-types)))
	  (and as (not (null? as))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; OK-TO-DRAW? - return TRUE iff the device is HARDCOPY or we are in
;             - graphics mode

(define (ok-to-draw? dev)
    (interactive off)
    (or (= (graphics-mode) 1) (hardcopy? dev)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; CYCLE-LIST - return the next item on the list after the specified
;            - one and assume the list is logically circular

(define (cycle-list item lst)

    (define (cycle item lst rem)
        (let* ((first (car rem))
	       (rest (cdr rem)))
	  (if (eqv? item first)
	      (if (pair? rest)
		  (car rest)
		  (car lst))
	      (cycle item lst rest))))

    (cycle item lst lst))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; IDENTIFY-ONE - return a mapping one way or the other

(define (identify-one x grimms)
    (if (number? x)
	(list-ref grimms (- x 1))
	(if grimms
	    (let* ((first (list-ref grimms 0)))
	          (if (eqv? x (car first))
		      first
		      (identify-one x (list-tail grimms 1)))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; VIEW-PREP-ARGS - prepare argument list for viewport operations
;                - input should be
;                -    ((a1 a2 ...))
;                -    ((vname a1 a2 ...))
;                -    ((vname wname a1 a2 ...))
;                - output should be
;                -    (<vname> <wname> (a-specs ... ))

(define (view-prep-args args)
    (let* ((x (if (pair? args)
		  (car args)
		  args))
	   (first (if (pair? x)
		      (car x)
		      nil))
	   vname wname)

      (if (or (pm-mapping? first) (pm-set? first) (pg-image? first)
	      (pg-graph? first) (number? first) (printable? first))
	  (set! vname nil)
	  (begin (set! vname first)
		 (set! x (if (pair? x)
			     (cdr x)
			     nil))
		 (set! first (if (pair? x)
				 (car x)
				 nil))))
		   
      (if (or (pm-mapping? first) (pm-set? first) (pg-image? first)
	      (pg-graph? first) (number? first) (printable? first))
	  (set! wname nil)
	  (begin (set! wname first)
		 (set! x (if (pair? x)
			     (cdr x)
			     nil))))

      (list vname wname x)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GET-EXTREMA - return the extrema of the two specified limits

(define (get-extrema l1 l2 lims)
    (if l1
	(let* ((l1n (list-ref l1 0))
	       (l1x (list-ref l1 1))
	       (l2n (if (pair? l2) (list-ref l2 0) HUGE))
	       (l2x (if (pair? l2) (list-ref l2 1) (- HUGE)))
	       (ln  (if (< l1n l2n) l1n l2n))
	       (lx  (if (> l1x l2x) l1x l2x)))
	  (get-extrema (list-tail l1 2)
		       (list-tail l2 2)
		       (append (list lx ln) lims)))
	(reverse lims)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GET-DOMAIN-EXTREMA - return the extrema of the given graphs domains

(define (get-domain-extrema lst lims)
    (if lst
	(let* ((g (car lst))
	       (rest (cdr lst))
	       (limits (pg-domain-limits g))
	       (extr (if limits limits (pg-domain-extrema (car g)))))
	  (get-domain-extrema rest
			      (get-extrema (if lims lims extr) extr nil)))
	lims))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GET-RANGE-EXTREMA - return the extrema of the given graphs ranges

(define (get-range-extrema lst lims)
    (if lst
	(let* ((g (car lst))
	       (rest (cdr lst))
	       (limits (pg-range-limits g))
	       (extr (if limits limits (pg-range-extrema (car g)))))
	  (get-range-extrema rest
			     (get-extrema (if lims lims extr) extr nil)))
	lims))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SET-DR-FROM-VR - set the drawable-rendering from the viewport-rendering

(define (set-dr-from-vr indx lst vport)
    (let* ((rendering (viewport-rendering vport))
	   (item      (list-ref lst indx)))
      (if rendering
	  (set! rendering (car rendering))
	  (set! rendering default-vr))
      (cond ((eqv? rendering cartesian-plot)
	     (set-cdr! item (list (list 'render 'cartesian-plot))))
	    ((eqv? rendering polar-plot)
	     (set-cdr! item (list (list 'render 'polar-plot))))
	    ((eqv? rendering insel-plot)
	     (set-cdr! item (list (list 'render 'insel-plot))))
	    ((eqv? rendering histogram-plot)
	     (set-cdr! item (list (list 'render 'histogram-plot))))
	    ((eqv? rendering scatter-plot)
	     (set-cdr! item (list (list 'render 'scatter-plot))))
	    ((eqv? rendering logical-plot)
	     (set-cdr! item (list (list 'render 'logical-plot))))
	    ((eqv? rendering contour)
	     (set-cdr! item (list (list 'render 'contour))))
	    ((eqv? rendering image)
	     (set-cdr! item (list (list 'render 'image))))
	    ((eqv? rendering wire-frame)
	     (set-cdr! item (list (list 'render 'wire-frame))))
	    ((eqv? rendering shaded)
	     (set-cdr! item (list (list 'render 'shaded))))
	    ((eqv? rendering vect)
	     (set-cdr! item (list (list 'render 'vect))))
	    ((eqv? rendering dvb)
	     (set-cdr! item (list (list 'render 'dvb))))
	    ((eqv? rendering fill-poly)
	     (set-cdr! item (list (list 'render 'fill-poly))))
	    ((eqv? rendering points)
	     (set-cdr! item (list (list 'render 'points))))
	    ((eqv? rendering mesh)
	     (set-cdr! item (list (list 'render 'mesh)))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MAP-RESOLVE - called from the math handlers to resolve an
;             - index into the mapping list of the current window
;             - and viewport into a mapping

(define (map-resolve n)
    (if (integer? n)
	(let* ((x     (view-prep-args nil))
	       (vname (list-ref x 0))
	       (wname (list-ref x 1))
	       (vp    (get-viewport vname wname))
	       (gr    (viewport-graphs vp))
               (m     (- n 1))
	       (gl    (if (<= 0 m) (list-ref gr m))))
	      (if (pair? gl)
		  (car gl)
		  n))
	n))

;--------------------------------------------------------------------------

;                       WINDOW STRUCTURE

;--------------------------------------------------------------------------

; WINDOW Structure - (name device viewports current-viewport status)

(define (window-name window)      (list-ref window 0))
(define (window-device window)    (list-ref window 1))
(define (window-viewports window) (list-ref window 2))
(define (current-viewport window) (list-ref window 3))
(define (window-clear? window)    (list-ref window 4))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (make-window name device viewports st)
    (list name device viewports nil st))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (add-window w)
    (set! window-list (cons w window-list)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (remove-window w)
    (set! window-list (splice-out w window-list equal?)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (get-window name)
    (assv name window-list))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (clear-window w)
    (if (not (window-clear? w))
	(pg-clear-window (window-device w)))
    (set-car! (list-tail w 4) #t))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (finish-window w)
    (pg-finish-plot (window-device w))
    (set-car! (list-tail w 4) #f))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (set-current-viewport! window p)
    (highlight-window-viewport window un-highlight-color)
    (set-car! (list-tail window 3) p)
    (if p
        (highlight-window-viewport window viewport-highlight-color)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (set-window-device! window dev)
    (set-car! (list-tail window 1) dev))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (set-window-viewports! window p)
    (set-car! (list-tail window 2) p))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (replace-window original window)
    (if window
	(set-cdr! original (cdr window))
	(set-cdr! original nil)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (default-window-info)
    (let* ((name (sprintf "%c" window-index)))
      (set! window-index (+ window-index 1))
      (list name "COLOR" "WINDOW" 0.1 0.1
	    default-window-width
	    default-window-height)))

;--------------------------------------------------------------------------

;                       VIEWPORT STRUCTURE

;--------------------------------------------------------------------------

; VIEWPORT Structure - (name window (x y dx dy)
;                                   (<rendering info>)
;                                   (<range info>)
;                                   (<domain info>)
;                                   (mappings <graphs>)
;                                   (meshes <sets>)
;                                   (images <images>))

(define (viewport-name viewp)        (list-ref viewp 0))
(define (viewport-shape viewp)       (list-ref viewp 2))
(define (viewport-rendering viewp)   (list-ref viewp 3))
(define (viewport-range viewp)       (list-ref viewp 4))
(define (viewport-domain viewp)      (list-ref viewp 5))
(define (viewport-graph-list viewp)  (list-ref viewp 6))
(define (viewport-mesh-list viewp)   (list-ref viewp 7))
(define (viewport-image-list viewp)  (list-ref viewp 8))

(define (viewport-drawables viewp n)
    (let* ((lst (list-ref viewp n)))
      (if lst
	  (cdr lst)
	  lst)))

(define (viewport-graphs viewp)
    (viewport-drawables viewp 6))

(define (viewport-meshes viewp)
    (viewport-drawables viewp 7))

(define (viewport-images viewp)
    (viewport-drawables viewp 8))

(define (viewport-window viewp)
    (get-window (list-ref viewp 1)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (make-viewport name window x y dx dy range-limit domain-limit
		       rendering graphs meshes images)
    (list name window
	  (list x y dx dy)
	  rendering range-limit domain-limit
	  (cons 'graphs graphs)
	  (cons 'meshes meshes)
	  (cons 'images images)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; GET-VIEWPORT - get the specified viewport
;                Usage: (get-viewport [viewport] | [viewport-name [window-name]])

(define (get-viewport vp wind)
    (cond ((null? vp)
	   (current-viewport (if wind
				 wind
				 current-window)))
	  ((pair? vp)
	   vp)
	  (else
	   (let* ((window (if wind
			      (get-window wind)
			      current-window))
		  (vports (window-viewports window)))
	     (assv vp vports)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; FIND-VIEWPORT - sort out the given specifications and try to come
;               - up with a viewport to match

(define (find-viewport spec)
    (let* ((cspec (car spec))
	   (arg (if (pair? cspec)
		    cspec
		    spec))
	   (vname (if arg (car arg) "a"))
	   (vp (get-viewport vname nil)))
      (if (not vp)
	  (let* ((info (if arg
			   (cdr arg)
			   (default-viewport-info)))
		 (x    (list-ref info 0))
		 (y    (list-ref info 1))
		 (dx   (list-ref info 2))
		 (dy   (list-ref info 3))
		 (rest (list-tail info 4))
		 (window (if rest
			     (get-window (car rest))
			     current-window))
		 (wname (window-name window))
		 (vports (window-viewports window)))
	    (set! vp (make-viewport vname wname x y dx dy nil
				    nil nil nil nil nil))
	    (set-window-viewports! window (cons vp vports))
	    (set-current-viewport! window vp)))
      vp))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (setup-viewport device shape)
    (if (and device shape)
	(let* ((ls   (if (hardcopy? device) 0.0 (label-space)))
	       (xmin (list-ref shape 0))
	       (ymin (list-ref shape 1))
	       (dx   (list-ref shape 2))
	       (dy   (list-ref shape 3))
	       (yn   (+ ymin (* ls dy)))
	       (yx   (+ ymin dy))
	       xmax ymax)

; non-optimal way to do this
	  (pg-set-vector-attributes! device vector-scale vect-scale)
	  (pg-set-vector-attributes! device vector-headsize vect-headsz)
	  (pg-set-vector-attributes! device vector-color vect-color)

	  (pg-set-frame! device xmin (+ xmin dx) yn yx)
	  (pg-set-viewport! device 0.2 0.9 0.2 0.9)

	  (label-y-position 1.0e100))))

;--------------------------------------------------------------------------

;                         DRAWABLE STRUCTURE 

;--------------------------------------------------------------------------

; DRAWABLE Structure - (<graph> | <image> | <set> [rendering-info]* )

;--------------------------------------------------------------------------

; MAKE-DRAWABLE - create a drawable
;               - (<graph> | <image> | <set> [rendering-info]* )

(define (make-drawable d . info)
   (cons d info))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DRAWABLE-DATA - return the data part of the drawable

(define (drawable-data dr)
    (if (pair? dr)
	(car dr)
	dr))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DRAWABLE-RENDER - return all rendering info associated with
;                 - the given drawable DR

(define (drawable-render dr)
    (if (pair? dr)
	(cdr dr)
	nil))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DRAWABLE-RENDERING - determine the rendering to use for the drawable DR
;                    - at plot time
;                    - rendering selection is hierarchical
;                    - if a rendering is specified for the given drawable
;                    - it is used
;                    - otherwise if there is a viewport rendering currently
;                    - in force it is used
;                    - otherwise the default rendering for the dimensionality
;                    - is chosen

(define (drawable-rendering dr info)
    (let* ((data   (drawable-data dr))
	   (do     (list-ref dr 0))
	   (im     (pg-image? do))
	   (gr     (pg-graph? do))
	   (ms     (or (pm-set? do) (pm-mapping? do)))
	   (vp     (get-viewport () ()))
	   (vrend  (viewport-rendering vp))
	   nd)

      (if info

; if we have specified rendering info use it
	  info

	  (if vrend

; if we have a viewport rendering info use it
	      vrend

; otherwise find the default rendering
	      (if im
		  ()
		  (if ms
		      (list mesh)

; checking the case when we have a mapping
		      (let* ((nd (car (pm-mapping-dimension do))))
			    (list (cond ((= nd 1)
					 (list-ref domain-1d 0))
					((= nd 2)
					 (list-ref domain-2d 0))
					((= nd 3)
					 (list-ref domain-3d 0)))))))))))

;--------------------------------------------------------------------------

;                     VIEWPORT HIGHLIGHT ROUTINES

;--------------------------------------------------------------------------

(define highlight-flag on)

(define (show-highlight . arg)
   (if arg
       (set! highlight-flag (car arg))
       highlight-flag))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (show-highlight?)
   (or (eqv? highlight-flag on) (eqv? highlight-flag #t)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (highlight-window window color)
    (if (and current-window (show-highlight?))
	(let* ((device (window-device window))
               (clip  (pg-clipping? device))
	       (pal   (pg-current-palette device)))
	  (pg-make-device-current device)
	  (pg-set-palette! device "standard")
	  (pg-set-line-color! device color)
	  (pg-set-line-width! device 2.0)
          (pg-set-clipping! device 0)
	  (pg-draw-box device @norm 0.0 1.0 0.0 1.0)
          (pg-set-clipping! device clip)
	  (pg-set-palette! device pal)
	  (pg-set-line-width! device 0.0)
	  (pg-update-view-surface device)
	  (pg-make-device-current WIN-device))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

;(define (highlight-port device xmin xmax ymin ymax title color)
;    (let* ((pal   (pg-current-palette device))
;	   (wmin  (pg-normalized->world device xmin ymin))
;	   (wmax  (pg-normalized->world device xmax ymax))
;	   (wxmin (car wmin))
;	   (wymin (cadr wmin))
;	   (wxmax (car wmax))
;	   (wymax (cadr wmax))
;	   (dx    (- wxmax wxmin))
;	   (dy    (- wymax wymin))
;	   (txmin (+ wxmin (* 0.01 dx)))
;	   (tymin (+ wymin (* 0.01 dy))))
;      (pg-make-device-current device)
;      (pg-set-palette! device "standard")
;      (pg-set-line-color! device color)
;      (pg-set-line-width! device 0.0)
;      (pg-draw-box device @norm xmin xmax ymin ymax)
;      (pg-set-text-color! device color)
;      (pg-draw-text device 2 @world txmin tymin title)
;      (pg-set-palette! device pal)
;      (pg-update-view-surface device)
;      (pg-make-device-current WIN-device)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (highlight-port device xmin xmax ymin ymax title color)
    (let* ((pal   (pg-current-palette device))
	   (wmin  (pg-normalized->world device xmin ymin))
	   (wmax  (pg-normalized->world device xmax ymax))
	   (wxmin (car wmin))
	   (wymin (cadr wmin))
	   (wxmax (car wmax))
	   (wymax (cadr wmax))
	   (dx    (- xmax xmin))
	   (dy    (- ymax ymin))
	   (txmin (+ xmin (* 0.01 dx)))
	   (tymin (+ ymin (* 0.01 dy))))
      (pg-make-device-current device)
      (pg-set-palette! device "standard")
      (pg-set-line-color! device color)
      (pg-set-line-width! device 0.0)
      (pg-draw-box device @norm xmin xmax ymin ymax)
      (pg-set-text-color! device color)
      (pg-draw-text device 2 @norm txmin tymin title)
      (pg-set-palette! device pal)
      (pg-update-view-surface device)
      (pg-make-device-current WIN-device)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (highlight-window-viewport window color . p)
    (let* ((port (if p
		     (car p)
		     (current-viewport window))))
      (if (and port (show-highlight?))
	  (let* ((device (window-device window))
		 (shape  (viewport-shape port))
		 (xmin   (list-ref shape 0))
		 (ymin   (list-ref shape 1))
		 (dx     (list-ref shape 2))
		 (dy     (list-ref shape 3))
		 (xmax   (+ xmin dx))
		 (ymax   (+ ymin dy)))
	    (highlight-port device xmin xmax ymin ymax
			    (viewport-name port) color)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (pg-clear-viewport device port)
    (let* ((shape (viewport-shape port)))
      (if shape
	  (let* ((xmin  (list-ref shape 0))
		 (ymin  (list-ref shape 1))
		 (dx    (list-ref shape 2))
		 (dy    (list-ref shape 3))
		 (xmax  (+ xmin dx))
		 (ymax  (+ ymin dy)))
	    (pg-clear-region device xmin xmax ymin ymax 4)
	    (pg-update-view-surface device)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (set-viewport-shape! viewp . rest)
    (set-car! (list-tail viewp 2) rest))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (set-viewport-rendering! viewp info)
    (let* ((rend (list-tail viewp 3)))
        (set-car! rend info)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (set-viewport-range! viewp info)
    (let* ((ran (list-tail viewp 4)))
        (set-car! ran info)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (set-viewport-domain! viewp info)
    (let* ((dom (list-tail viewp 5)))
        (set-car! dom info)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (set-viewport-graphs! viewp g)
    (set-cdr! (viewport-graph-list viewp) g))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (set-viewport-meshes! viewp g)
    (set-cdr! (viewport-mesh-list viewp) g))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (set-viewport-images! viewp i)
    (set-cdr! (viewport-image-list viewp) i))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (default-viewport-info)
    (list 0.0 0.0 1.0 1.0))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SET-VIEWPORT-AREA! - setup a common viewport for overlaid plots

(define (set-viewport-area! gr ms im)
    (if (eqv? viewport-area-save nil)
        (begin
           (set! overlay-box-size 100)
           (if im
               (if (> overlay-box-size image-box-size)
                   (begin (set! overlay-box image-box)
                          (set! overlay-box-size image-box-size))))

           (if ms
               (if (> overlay-box-size mesh-box-size)
                   (begin (set! overlay-box mesh-box)
                          (set! overlay-box-size mesh-box-size))))

           (if gr
               (for-each
                (lambda (x)
                    (let* ((dra (drawable-render x))
			   (rspec (if dra (cadar dra))))
		      (cond ((eqv? rspec 'image)
			     (if (> overlay-box-size image-box-size)
				 (begin (set! overlay-box image-box)
					(set! overlay-box-size image-box-size))))
			    ((eqv? rspec 'contour)
			     (if (> overlay-box-size contour-box-size)
				 (begin (set! overlay-box contour-box)
					(set! overlay-box-size contour-box-size))))
			    ((eqv? rspec 'mesh)
			     (if (> overlay-box-size mesh-box-size)
				 (begin (set! overlay-box mesh-box)
					(set! overlay-box-size mesh-box-size))))
			    ((eqv? rspec 'dvb)
			     (if (> overlay-box-size dvb-box-size)
				 (begin (set! overlay-box dvb-box)
					(set! overlay-box-size dvb-box-size))))
			    ((eqv? rspec 'fill-poly)
			     (if (> overlay-box-size fill-poly-box-size)
				 (begin (set! overlay-box fill-poly-box)
					(set! overlay-box-size fill-poly-box-size))))
			    ((eqv? rspec 'points)
			     (if (> overlay-box-size points-box-size)
				 (begin (set! overlay-box points-box)
					(set! overlay-box-size points-box-size))))
			    ((eqv? rspec 'vect)
			     (if (> overlay-box-size vector-box-size)
				 (begin (set! overlay-box vector-box)
					(set! overlay-box-size vector-box-size)))))))
                 gr))
           
           (set! viewport-area-box overlay-box)))) 
         
;--------------------------------------------------------------------------

;                       WINDOW MANAGER ROUTINES

;--------------------------------------------------------------------------

; WM-ADD - the window manager add command

(define (wm-add name fnc)
    (hash-install name fnc window-manager-table))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WM-WIN-LIST - the window manager "wlist" command

(define (wm-win-list)

    (define (print-win lst n)
        (if lst
	    (let* ((w     (list-ref lst 0))
		   (rest  (list-tail lst 1))
		   (name  (window-name w))
		   (dev   (window-device w))
		   (vp    (window-viewports w))
		   (prop  (pg-device-properties dev))
		   (type  (list-ref prop 0))
		   (clr   (list-ref prop 1))
		   (title (list-ref prop 2)))
	          (printf nil "%3d %s %s %s %3d\n" n name type clr (length vp))
		  (print-win rest (+ n 1)))))

    (print-win window-list 1))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WM-WIN-OPEN - the window manager "wopen" command

(define (wm-win-open . args)
    (highlight-window current-window un-highlight-color)
    (cond ((or (and args (car args)) (not window-list))
	   (let* ((args1 (if args
			     (car args)
			     nil))
		  (info  (if (and args1 (< 6 (length args1)))
			     args1
			     (default-window-info)))
		  (wname (if args1
			     (car args1)
			     (list-ref info 0)))
		  (type  (list-ref info 1))
		  (name  (list-ref info 2))
		  (winx  (list-ref info 3))
		  (winy  (list-ref info 4))
		  (windx (list-ref info 5))
		  (windy (list-ref info 6))
		  (window (get-window wname))
		  (neverbeen (not window))
		  new)
	     (if neverbeen
		 (let* ((title (if (string=? name "WINDOW")
				   (sprintf "PDBView Display %s" wname)
				   wname))
			(device (pg-make-device name type title)))
;		   (if (hardcopy? device)
;		       (pg-set-white-background! device))
		   (if (or (pg-open-device device winx winy windx windy)
			   (not (ok-to-draw? device)))
		       (set! new (make-window wname device nil #t))
		       (set! new #f))
		   (if new
		       (begin
			 (pg-set-palette! device default-palette)
			 (if neverbeen
			     (add-window new)
			     (replace-window window new)))
		       (printf nil "Can't open window %s\n" wname))
		   (set! window new)))
	     (set! current-window window)))
	  (else
	   (set! current-window (cycle-list current-window window-list))))

    (set! WIN-device (window-device current-window))

    (pg-make-device-current WIN-device)
    (pg-clear-window WIN-device)

    (highlight-window current-window window-highlight-color))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WM-WIN-CLOSE - the window manager "wclose" command
;              - Args: <window-list>

(define (wm-win-close windows)
    (if (null? windows)
        (set! windows (list current-window)))

    (define (close-one x)
        (let* ((window (if (pair? x)
			   x
			   (get-window x))))
	      (if window
		  (let* ((device (window-device window)))
		        (if device
			    (begin
			       (pg-close-device device)
			       (replace-window window nil)
			       (if (eqv? device WIN-device)
				   (set! WIN-device #f))
			       (if (eqv? window current-window)
				   (set! current-window nil))))
			(remove-window window)))))

    (if windows
	(begin
	  (map close-one windows)
	  (if (and (null? current-window) (not (null? window-list)))
	      (wm-win-open (car window-list))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WM-WIN-CONNECT - the window manager "wconnect" command
;                - Args: [<window>] <device>

(define (wm-win-connect window device)
    (if window
	(set-window-device! window device)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WM-WIN-DISCONNECT - the window manager "wdisconnect" command
;                   - Args: <window>

(define (wm-win-disconnect window)
    (if window
	(let* ((device (window-device window)))
	    (if device
		(begin
;		    (pg-close-device device)
		    (set-window-device! window nil)
;		    (if (eqv? device WIN-device)
;			(set! WIN-device #f))
;		    (if (eqv? window current-window)
;			(set! current-window #f)))))))
)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WM-WIN-HARDCOPY - the window manager "whardcopy" command

(define (wm-win-hardcopy device res window . rest)
    (let* ((orig   (window-device window))
	   (finfo  (cons device (pg-text-font orig)))
	   (vports (if window
		       (window-viewports window)
		       #f)))

; copy annotations to the hardcopy device
      (copy-interface-objects device orig #t)

      (if vports
	  (begin (if (> res 0)
		     (pg-set-resolution-scale-factor! device res))

		 (wm-win-disconnect window)
		 (wm-win-connect window device)

		 (apply pg-set-text-font! finfo)

		 (pg-clear-window device)
		 (pg-set-clear-mode! device off)
		 (pg-set-finish-state! device off)
		 (for-each (lambda (x)
			     (vp-hardcopy device res
					  (viewport-name x)
					  rest))
			   vports)

		 (pg-set-clear-mode! device WINDOW)
		 (pg-set-finish-state! device on)
		 (pg-finish-plot device)

		 (wm-win-disconnect window)
		 (wm-win-connect window orig))))
    window)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WM-WIN-UPDATE - the window manager "wupdate" command
;               - Args: <window-list>

(define (wm-win-update windows)
    (interactive off)
    (if (= (graphics-mode) 1)
	(begin
	    (if (and (null? windows) current-window)
		(set! windows (list current-window)))

	    (define (update-one x)
		(let* ((window (if (pair? x)
				   x
				   (get-window x)))
		       (vports (if window
				   (window-viewports window)
				   #f)))

		  (if (and window vports)
		      (let* ((wdev (window-device window)))
			    (for-each viewport-update vports)
			    (pg-finish-plot wdev)
			    (if (hardcopy? wdev)
				(pg-clear-window wdev))))

		  (if (eqv? window current-window)
		      (highlight-window window window-highlight-color)
		      (highlight-window window un-highlight-color)))

		(window-name x))

	    (if windows
		(map update-one windows)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WM-VP-OPEN - the window manager "vopen" command

(define (wm-vp-open . spec)
    (if (not current-window)
	(wm-win-open))
    (cond ((and spec (car spec))
	   (set-current-viewport! current-window
				  (find-viewport spec)))

	  (current-window
	     (let* ((cv    (current-viewport current-window))
		    (ports (window-viewports current-window)))
	           (if (not cv)
		       (let* ((name "A"))
			 (apply wm-vp-open (cons name (default-viewport-info)))
			 (set! cv (current-viewport current-window))
			 (set! ports (window-viewports current-window))))
		   (set-current-viewport! current-window
					  (cycle-list cv ports))))
	  
	  (else
	   (wm-win-open)
	   (wm-vp-open))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WM-VP-CLOSE - the window manager "vclose" command
;             - Args: [vname [wname]]

(define (wm-vp-close . args)
    (let* ((x     (view-prep-args args))
	   (vname (list-ref x 0))
	   (wname (list-ref x 1))
	   (vp (get-viewport vname wname)))
      (if vp
	  (let* ((window (viewport-window vp))
		 (device (window-device window))
		 (ports  (window-viewports window)))
	    (if (eqv? vp (current-viewport window))
		(let* ((next (cycle-list vp ports)))
		  (if (eqv? vp next)
		      (set-current-viewport! window nil)
		      (set-current-viewport! window next))))
	    (pg-clear-viewport device vp)
	    (set! ports (splice-out vp ports eqv?))
	    (set-window-viewports! window ports)))
      vname))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WM-VP-DLIMIT - the window manager "vdlimit" command
;              - Args: [vname [wname]] <x1_min> <x1_max> ...

(define (wm-vp-dlimit . args)
    (plot-flag on)
    (let* ((x     (view-prep-args args))
	   (vname (list-ref x 0))
	   (wname (list-ref x 1))
	   (vp    (get-viewport vname wname)))
      (if vp
	  (set-viewport-domain! vp (car (list-tail x 2))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WM-VP-RLIMIT - the window manager "vrlimit" command
;              - Args: [vname [wname]] <x1_min> <x1_max> ...

(define (wm-vp-rlimit . args)
    (plot-flag on)
    (let* ((x      (view-prep-args args))
	   (vname  (list-ref x 0))
	   (wname  (list-ref x 1))
	   (vp     (get-viewport vname wname)))
      (if vp
	  (set-viewport-range! vp (car (list-tail x 2))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WM-MAP-DLIM-S - the window manager "mdlims" command
;               - Args: [vname [wname]] <mapping> <x1_min> <x1_max> ...

(define (wm-map-dlim-s . args)
    (let* ((x      (view-prep-args args))
	   (vname  (list-ref x 0))
	   (wname  (list-ref x 1))
	   (items  (list-ref x 2))
	   (map    (car items))
	   (limits (cdr items))
	   (vp     (get-viewport vname wname))
	   (gr     (viewport-graphs vp)))
      (if (integer? map)
	  (if gr
	      (pg-set-domain-limits! (car (list-ref gr (- map 1))) limits))
	  (pg-set-domain-limits! map limits))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WM-MAP-RLIM-S - the window manager "mrlims" command
;               - Args: [vname [wname]] <mapping> <x1_min> <x1_max> ...

(define (wm-map-rlim-s . args)
    (let* ((x      (view-prep-args args))
	   (vname  (list-ref x 0))
	   (wname  (list-ref x 1))
	   (items  (list-ref x 2))
	   (map    (car items))
	   (limits (cdr items))
	   (vp     (get-viewport vname wname))
	   (gr     (viewport-graphs vp)))
      (if (integer? map)
	  (if gr
	      (pg-set-range-limits! (car (list-ref gr (- map 1))) limits))
	  (pg-set-range-limits! map limits))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PRINT-MAP-INFO

(define (print-map-info fnc lst x)
    (let* ((info (if (integer? x)
		     (if lst
			 (fnc (car (list-ref lst (- x 1))))
			 (fnc x)))))
          (if (= (interactive) on)
	      (cond ((eqv? fnc pg-domain-extrema)
		     (printf nil "Domain of %s: %s\n" x info))
		    ((eqv? fnc domain-npts)
		     (printf nil "Domain # pts of %s: %s\n" x info))
		    ((eqv? fnc range-npts)
		     (printf nil "Range # pts of %s: %s\n" x info))
		    ((eqv? fnc pg-get-label)
		     (printf nil "Label of %s: %s\n" x info))
		    ((eqv? fnc pg-range-extrema)
		     (printf nil "Range of %s: %s\n" x info))))
	  info))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; DOMAIN-NPTS - return the number of points in the domain

(define (domain-npts x)
    (pg-drawable-info (pm-mapping-domain x) "n-elements"))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; RANGE-NPTS - return the number of points in the range

(define (range-npts x)
    (pg-drawable-info (pm-mapping-range x) "n-elements"))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WM-MAP-DLIM-G - the window manager "mdlimg" command
;               - Args: [vname [wname]] <mapping>

(define (wm-map-dlim-g . args)
    (let* ((x      (view-prep-args args))
	   (vname  (list-ref x 0))
	   (wname  (list-ref x 1))
	   (items  (list-ref x 2))
	   (map    (car items))
	   (vp     (get-viewport vname wname))
	   (gr     (viewport-graphs vp)))
      (if (integer? map)
	  (if gr
	      (print-map-info pg-domain-extrema gr map))
	  (print-map-info pg-domain-extrema gr map))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WM-MAP-DNPTS-G - the window manager "mdnptsg" command
;                - Args: [vname [wname]] <mapping>

(define (wm-map-dnpts-g . args)
    (let* ((x      (view-prep-args args))
	   (vname  (list-ref x 0))
	   (wname  (list-ref x 1))
	   (items  (list-ref x 2))
	   (map    (car items))
	   (vp     (get-viewport vname wname))
	   (gr     (viewport-graphs vp)))
      (if (integer? map)
	  (if gr
	      (print-map-info domain-npts gr map))
	  (print-map-info domain-npts gr map))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WM-MAP-LABL-G - the window manager "mlabg" command
;               - Args: [vname [wname]] <mapping>

(define (wm-map-labl-g . args)
    (let* ((x      (view-prep-args args))
	   (vname  (list-ref x 0))
	   (wname  (list-ref x 1))
	   (items  (list-ref x 2))
	   (map    (car items))
	   (vp     (get-viewport vname wname))
	   (gr     (viewport-graphs vp)))
      (if (integer? map)
	  (if gr
	      (print-map-info pg-get-label gr map))
	  (print-map-info pg-get-label gr map))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WM-MAP-RLIM-G - the window manager "mrlimg" command
;               - Args: [vname [wname]] <mapping> <x1_min> <x1_max> ...

(define (wm-map-rlim-g . args)
    (let* ((x      (view-prep-args args))
	   (vname  (list-ref x 0))
	   (wname  (list-ref x 1))
	   (items  (list-ref x 2))
	   (map    (car items))
	   (vp     (get-viewport vname wname))
	   (gr     (viewport-graphs vp)))
      (if (integer? map)
	  (if gr
	      (print-map-info pg-range-extrema gr map))
	  (print-map-info pg-range-extrema gr map))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WM-MAP-RNPTS-G - the window manager "mrnptsg" command
;                - Args: [vname [wname]] <mapping> <x1_min> <x1_max> ...

(define (wm-map-rnpts-g . args)
    (let* ((x      (view-prep-args args))
	   (vname  (list-ref x 0))
	   (wname  (list-ref x 1))
	   (items  (list-ref x 2))
	   (map    (car items))
	   (vp     (get-viewport vname wname))
	   (gr     (viewport-graphs vp)))
      (if (integer? map)
	  (if gr
	      (print-map-info range-npts gr map))
	  (print-map-info range-npts gr map))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SET-WORLD-COORDINATES! 

(define (set-world-coordinates! device dlimits)
    (if dlimits
	(let* ((x1 (list-ref dlimits 0))
	       (x2 (list-ref dlimits 1))
	       (y1 (list-ref dlimits 2))
	       (y2 (list-ref dlimits 3)))
	  (pg-set-world-coordinate-system! device x1 x2 y1 y2))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; VP-UPDATE-DRAWABLE - do the work of plotting for graphs, images,
;                    - and domains

(define (vp-update-drawable device func dr dlimits rlimits info)
    (let* ((data   (drawable-data dr))
	   (rspec  (drawable-render dr))
	   (basic  (list device data))
	   (im     (pg-image? (car dr)))
	   (dlim   (if im
		       (pg-world-coordinate-system device)
		       (pg-domain-limits dr)))
	   (rlim   (if im
		       nil
		       (pg-range-limits dr)))
	   (drend  (drawable-rendering dr info)))

      (if im
	  (set-world-coordinates! device dlimits)
	  (begin
	    (pg-set-domain-limits! data dlimits)
	    (pg-set-range-limits! data rlimits)))

      (define (render-one x)
	  (let* ((f    (car x))
		 (args (cdr x)))
	        (cond ((eqv? f 'render)
		       (set! drend (map eval args)))
		      (else
		       (let* ((fnc (eval f)))
			     (cond ((eqv? fnc view-angle)
				    (apply pg-set-view-angle! args))
				   ((procedure? fnc)
				    (apply fnc args))))))))

; set the global rendering state from the render list
      (if rspec
	  (for-each render-one rspec))

      (apply func (append basic drend))
      (if im
	  (set-world-coordinates! device dlim)
	  (begin
	    (pg-set-domain-limits! data dlim)
	    (pg-set-range-limits! data rlim)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; VP-HARDCOPY - the worker for hardcopy of viewports

(define (vp-hardcopy device res vname rest)
    (let* ((port    (get-viewport vname rest))
	   (window  (viewport-window port))
	   (shape   (viewport-shape port))
	   (info    (viewport-rendering port))
	   (rlimits (viewport-range port))
	   (dlimits (viewport-domain port))
	   (gr      (viewport-graphs port))
	   (ms      (viewport-meshes port))
	   (im      (viewport-images port))
	   (rextr   (if rlimits
			rlimits
			(get-range-extrema gr nil)))
	   (dextr   (if dlimits
			dlimits
			(get-domain-extrema gr nil))))

      (setup-viewport device shape)
      (if gr
	  (begin
	    (set! gr (reverse gr))
            (set! label-number 1)
	    (for-each (lambda (x)
                        (begin
                          (pg-set-graph-attribute! (drawable-data x) "LABEL-NUMBER"
                                                 "int *" label-number)
			  (vp-update-drawable device pg-draw-graph
					    x dextr rextr info)
                          (set! label-number (+ label-number 1))))
		      gr)
	    (set! gr (reverse gr))))

      (if ms
	  (begin
	    (set! ms (reverse ms))
	    (for-each (lambda (x)
			(vp-update-drawable device pg-draw-domain
					    x dextr rextr info))
		      ms)
	    (set! ms (reverse ms))))

      (if im
	  (begin
	    (set! im (reverse im))
	    (for-each (lambda (x)
			(vp-update-drawable device pg-draw-image
					    x nil nil viewport-area-box))
		      im)
	    (set! im (reverse im))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WM-VP-HARDCOPY - the window manager "vhardcopy" command

(define (wm-vp-hardcopy device res vname . rest)
    (if (> res 0)
	(pg-set-resolution-scale-factor! device res))
    (pg-clear-window device)
    (pg-set-finish-state! device off)
    (pg-set-clear-mode! device off)

    (vp-hardcopy device res vname rest)

    (pg-set-finish-state! device on)
    (pg-set-clear-mode! device WINDOW)
    (pg-finish-plot device))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WM-VP-RENDER - the window manager "vrender" command
;              - Args: [vname [wname]] render-info

(define (wm-vp-render . args)
    (plot-flag on)
    (let* ((x     (view-prep-args args))
	   (vname (list-ref x 0))
	   (wname (list-ref x 1))
	   (info  (list-ref x 2))
	   (mode  (if info (car info) nil))
	   (vp (get-viewport vname wname))
	   (gr (viewport-graphs vp))
	   (gr (if gr (car gr)))
	   (g  (drawable-data gr))
	   (ms (viewport-meshes vp))
	   (ms (if ms (car ms)))
	   (im (viewport-images vp))
	   (im (if im (car im))))

      (if mode

; if given a mode set it
          (let* ((opt-1d (memv mode domain-1d))
		 (opt-2d (memv mode domain-2d))
		 (opt-3d (memv mode domain-3d)))
	    (if (cond (g
		       (or (and opt-1d
				(= (car (pm-mapping-dimension g)) 1))
			   (and opt-2d
				(= (car (pm-mapping-dimension g)) 2))
			   (and opt-3d
				(= (car (pm-mapping-dimension g)) 3))))
		      (ms #t)
		      (im
		       (memv mode domain-2d))
		      (else #f))
		(set-viewport-rendering! vp info)
		(cond (opt-1d (render-1d-1d mode))
		      (opt-2d (render-2d-1d mode))
		      (opt-3d (render-3d-1d mode)))))

; otherwise set it to nil
	  (set-viewport-rendering! vp nil))

      (if mode
          (set! default-vr mode))

      (set-viewport-rendering! vp (list default-vr))
      (viewport-rendering vp)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WM-VP-ATTACH - the window manager "vattach" command
;              - Args: [vname [wname]] mappings|images ...

(define (wm-vp-attach . args)
    (if (not current-window)
	(wm-win-open))
    (if (not (current-viewport current-window))
	(wm-vp-open))

    (let* ((x     (view-prep-args args))
	   (vname (list-ref x 0))
	   (wname (list-ref x 1))
	   (g     (list-ref x 2))
	   (vp (get-viewport vname wname))
	   (vr (viewport-rendering vp))
	   (gr (viewport-graphs vp))
	   (ms (viewport-meshes vp))
	   (im (viewport-images vp)))

      (define-macro (map-one x)
          (set! x (resolve-to-drawable current-file x))

; Add check of domain dimension of each new x for consistency
; with first existing entry in gr, ms, or im (any one is enough).
; Among other things this may mean writing a pm-set-dimension.
	  (let* ((y (make-drawable x)))
	    (cond ((pg-graph? x)
		   (if (or (not gr)
			   (= (car (pm-mapping-dimension x))
			      (car (pm-mapping-dimension
				    (drawable-data (car gr))))))
		       (set! gr (cons y gr))
		       (printf nil "\nDimensions don't match previous mapping\n")))
		  ((pg-image? x)
		   (set! im (cons y im)))
		  ((pm-set? x)
		   (set! ms (cons y ms))))))

      (if g
	  (begin
	    (plot-flag on)
	    (for-each map-one g)
	    (if gr
		(begin (if overlay-flag
			   (set-dr-from-vr 0 gr vp))
		       (set-viewport-graphs! vp gr)))
	    (if im
		(begin (if overlay-flag
			   (set-dr-from-vr 0 im vp))
		       (set-viewport-images! vp im)))
	    (if ms
		(begin (if overlay-flag
			   (set-dr-from-vr 0 ms vp))
		       (set-viewport-meshes! vp ms)))))

      (if (and (not vr) default-vr)
	  (set-viewport-rendering! vp (list default-vr)))

      vp))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WM-VP-LIST - the window manager "vlist" command

(define (wm-vp-list args)
    (let* ((vname (if args
		      (list-ref args 0)
		      (viewport-name (current-viewport current-window))))
;		      (nil))
	   (wname (if args
		      (list-ref args 1)
		      (window-name current-window)))
;		      nil))
	   (vp (get-viewport vname wname))
	   (gr (viewport-graphs vp))
	   (im (viewport-images vp))
	   (ms (viewport-meshes vp))
	   (i 1))

      (define (lst-map x)
	(if x
	    (let* ((obj (drawable-data (car x))))
	          (printf nil
			  "%4d m %s\n"
			  i
			  (pg-get-label obj))
		   (set! i (+ i 1))
		   (lst-map (cdr x) i))))

      (define (lst-image x)
	(if x
	    (begin (printf nil
			   "%4d i %s\n"
			   i
			   (pg-get-label (drawable-data (car x))))
		   (set! i (+ i 1))
		   (lst-image (cdr x) i))))

      (define (lst-mesh x)
	(if x
	    (let* ((first (drawable-data (car x))))
	      (cond ((pm-mapping? first)
		     (printf nil
			     "%4d d %s\n"
			     i
			     (pg-get-label first)))
		    ((pm-set? first)
		     (printf nil
			     "%4d s %s\n"
			     i
			     (pg-get-label first))))
	      (set! i (+ i 1))
	      (lst-mesh (cdr x) i))))

      (printf nil "\nPlots in viewport %s of window %s\n"
	      vname wname)

; The order gr, im, ms is significant. vm-vp-delete depends on it.
      (if (or gr im ms)
	  (begin
	    (if gr
		(lst-map gr))
	    (if im
		(lst-image im))
	    (if ms
		(lst-mesh ms))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WM-VP-APPLY - the window manager "vapply" command
;              - Args: oper ([vname [wname]] <lsv-mapping-numbers> ...)

(define (wm-vp-apply . args)
    (let* ((oper   (list-ref args 0))
	   (x      (view-prep-args (list-tail args 1)))
	   (vname  (list-ref x 0))
	   (wname  (list-ref x 1))
	   (specs  (list-ref x 2))
	   (vp     (get-viewport vname wname))
	   (gr     (viewport-graphs vp))
	   (im     (viewport-images vp))
	   (ms     (viewport-meshes vp))
	   (grimms (append (append gr im) ms)))

; The order gr, im, ms in grimms is required for consistency with wm-vp-list.

      (define (apply-one x)
	(let* ((y (drawable-data x)))
	  (if (pair? y)
	      (apply oper y)
	      (oper y))))

      (if (and grimms specs)
	  (for-each apply-one
		    (map (lambda (x) (identify-one x grimms))
			 specs)))

      specs))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WM-VP-DELETE - the window manager "vdelete" command
;              - Args: [vname [wname]] <lsv-mapping-numbers> ...

(define (wm-vp-delete . args)
    (plot-flag on)
    (let* ((x      (view-prep-args args))
	   (vname  (list-ref x 0))
	   (wname  (list-ref x 1))
	   (specs  (list-ref x 2))
	   (vp     (get-viewport vname wname))
	   (gr     (viewport-graphs vp))
	   (im     (viewport-images vp))
	   (ms     (viewport-meshes vp))
	   (grimms (append (append gr im) ms)))

; The order gr, im, ms in grimms is required for consistency with wm-vp-list.

      (define (gr-map-eqv? a b)
	  (cond ((eqv? a b))
		((mapping? a)
		 (eqv? a (pg-drawable-info b "f")))))

      (define (delete-one x)
	(let* ((y (drawable-data x)))
	  (cond ((pg-graph? y)
		 (set! gr (splice-out x gr eqv?)))
		((pg-image? y)
		 (set! im (splice-out x im eqv?)))
		((or (pm-set? y) (pm-mapping? x))
		 (set! ms (splice-out x ms eqv?))))))

      (if grimms
	  (begin
	     (if specs
		 (for-each delete-one
			   (map (lambda (x) (identify-one x grimms))
				specs))
		 (for-each delete-one grimms))

	     (set-viewport-graphs! vp gr)
	     (set-viewport-images! vp im)
	     (set-viewport-meshes! vp ms)))

      specs))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WM-VP-GET-ARGS - the window manager "vget-args" command
;                - Args: [vname [wname]] <lsv-mapping-numbers> ...

(define (wm-vp-get-args . args)
    (let* ((x      (view-prep-args args))
	   (vname  (list-ref x 0))
	   (wname  (list-ref x 1))
	   (specs  (list-ref x 2))
	   (vp     (get-viewport vname wname))
	   (gr     (viewport-graphs vp))
	   (im     (viewport-images vp))
	   (ms     (viewport-meshes vp))
	   (grimms (append (append gr im) ms)))

; The order gr, im, ms in grimms is required for consistency with wm-vp-list.

      (define (save-one x)
	(let* ((y (drawable-data x)))
	  (cond ((pg-graph? y)
		 (cons "graph" y))
		((pg-image? y)
		 (cons "image" y))
		((or (pm-set? y) (pm-mapping? x))
		 (cons "mapping" x)))))

      (if grimms
	  (begin
	     (if specs
		 (map save-one
		      (map (lambda (x) (identify-one x grimms))
			   specs))
		 (map save-one grimms))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WM-VP-MOVE - the window manager "vmove" command

(define (wm-vp-move vnamd wnamd . args)
    (let* ((vs (current-viewport current-window))
	   (ds (car args))
	   (gr     (viewport-graphs vs))
	   (im     (viewport-images vs))
	   (ms     (viewport-meshes vs))
	   (grimms (append (append gr im) ms))
	   (src    (if grimms
		       (map (lambda (x) (if (number? x)
					    (car (identify-one x grimms))
					    x))
				    ds))))

          (window-manager "vattach" (append (list vnamd wnamd) src))
	  (window-manager "vdelete" ds)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WM-VP-LABEL - the window manager "vlabel" command
;              - Args: <label> <lsv-mapping-number>

(define (wm-vp-label . args)
    (let* ((x      (car args))
	   (label  (list-ref x 0))
	   (spec   (list-ref x 1))
	   (vp     (get-viewport nil nil))
	   (gr     (viewport-graphs vp))
	   (im     (viewport-images vp))
	   (ms     (viewport-meshes vp))
	   (grimms (append (append gr im) ms)))

; The order gr, im, ms in grimms is required for consistency with wm-vp-list.

      (if (and grimms spec)
	  (let* ((y (drawable-data (identify-one spec grimms))))
	    (pg-set-label! y label)))

      spec))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WM-VP-SET-ATTR - the window manager "vsattr" command
;                - Args: <name> <type> <val> [<mappings>]*

(define (wm-vp-set-attr name type val things)
    (let* ((x      (view-prep-args things))
	   (vname  (list-ref x 0))
	   (wname  (list-ref x 1))
	   (vp     (get-viewport vname wname))
	   (gr     (viewport-graphs vp))
	   (ms     (viewport-meshes vp)))

      (define (do-item x l fnc)
	 (plot-flag on)
	 (if x
	     (let* ((y (if (number? x)
			   (list-ref l (- x 1))
			   x)))
	           (if y
		       (fnc (drawable-data y)
			    name type val)))))

      (if (or things gr)
	  (for-each (lambda (x) (do-item x gr pg-set-graph-attribute!))
		    (if things things gr)))
      (if (or things ms)
	  (for-each (lambda (x) (do-item x ms pm-set-set-attribute!))
		    (if things things ms)))

      things))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WM-VP-FONT - the window manager "vfont" command
;            - Args: <window> (<type> <size> <style>)

(define (wm-vp-font window args)
    (let* ((device (window-device window)))
      (if args
	  (let* ((type (list-ref args 0))
		 (size (list-ref args 1))
		 (style (list-ref args 2)))
	    (pg-set-text-font! device type style size))

	  (printf nil "Current font: %s\n" (pg-text-font device)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WM-DR-RENDER - the window manager "drender" command
;              - Args: [vname [wname]] drawable-index render-info

(define (wm-dr-render . args)
    (plot-flag on)
    (let* ((x      (view-prep-args args))
	   (vname  (list-ref x 0))
	   (wname  (list-ref x 1))
	   (specs  (list-ref x 2))
	   (dr     (- (list-ref specs 0) 1))
	   (info   (list-ref specs 1))
	   (vp     (get-viewport vname wname))
	   (gr     (viewport-graphs vp))
	   (im     (viewport-images vp))
	   (ms     (viewport-meshes vp))
	   (grimms (append (append gr im) ms))
	   (item   (list-ref grimms dr)))
      (set-cdr! item info)
      item))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WM-LEGEND-PLACE - the window manager "lplace" command
;                 - Args: legend <drawable>+

(define (wm-legend-place . args)
    (plot-flag on)
    (let* ((things (car args))
	   (n      (length things)))

        (define (place-one i lst)
	    (if lst
		(begin (window-manager "vsattr" "LEGEND-PLACE"
				       "int *" (list i n)
				       (list (car lst)))
		       (place-one (+ i 1) (cdr lst)))))

        (if things
	    (place-one 0 things))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WM-PAL-CHANGE - the window manager "pchange" command
;               - Args: palette <drawable>+

(define (wm-pal-change . args)
    (plot-flag on)
    (let* ((x (car args))
	   (pname  (sprintf "%s" (list-ref x 0)))
	   (things (list-tail x 1)))

        (if things
	    (window-manager "vsattr" "PALETTE"
			    "char *" pname
			    things)

	    (begin (set! default-palette pname)
		   (if current-window
		       (pg-set-palette! (window-device current-window)
					default-palette))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WM-PAL-LIST - the window manager "plist" command
;             - Args: [wname]

(define (wm-pal-list args)
    (let* ((window (if args
		       (get-window (car args))
		       current-window)))
      (if window
	  (pg-palettes (window-device window))
	  nil)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WM-PAL-SHOW - the window manager "pshow" command
;             - Args: [wname]

(define (wm-pal-show args)
    (let* ((window (if args
		       (get-window (car args))
		       current-window)))
      (if window
	  (begin
	    (pg-show-palettes (window-device window))
	    (wm-win-update (list window)))
	  nil)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; WINDOW-MANAGER - a message passing approach to window management
;                - legal window messages:
;                -
;                - add         - add a new function to the window manager
;                - wlist       - list the open graphics devices
;                - wopen       - open a new window with the given name
;                - wclose      - close the named window
;                - wconnect    - connect a new device to the window
;                - wdisconnect - disconnect the device from the window
;                - whardcopy   - map vhardcopy over the window viewports
;                - wupdate     - redraw everything in the window
;                - vopen       - open a new viewport in the specified window
;                - vclose      - close the named viewport
;                - vhardcopy   - draw the graphs in the viewport to a hc device
;                - vrender     - set the rendering mode for the viewport
;                - vapply      - apply an operator to a graph in a viewport
;                - vattach     - attach a graph to a viewport
;                - vdelete     - delete a graph from a viewport
;                - vget-args   - return a list of specified drawables
;                - vlabel      - label a drawable
;                - vlist       - list the graphs attached to a viewport

(define (window-manager msg . rest)
    (plot-flag off)
    (if (not window-manager-table)
	(begin (set! window-manager-table (make-hash-table 17))
	       (wm-add "add" wm-add)))

    (let* ((cmd (hash-lookup msg window-manager-table))
	   (proc (if cmd (cdr cmd))))
      (if (and proc (procedure? proc))
	  (apply proc rest)
	  (printf nil "Bad window manager command %s\n"
		  msg))))

;--------------------------------------------------------------------------

(window-manager "add" "drender"     wm-dr-render)
(window-manager "add" "lplace"      wm-legend-place)
(window-manager "add" "mdlims"      wm-map-dlim-s)
(window-manager "add" "mrlims"      wm-map-rlim-s)
(window-manager "add" "mdlimg"      wm-map-dlim-g)
(window-manager "add" "mdnptsg"     wm-map-dnpts-g)
(window-manager "add" "mlabg"       wm-map-labl-g)
(window-manager "add" "mrlimg"      wm-map-rlim-g)
(window-manager "add" "mrnptsg"     wm-map-rnpts-g)
(window-manager "add" "plist"       wm-pal-list)
(window-manager "add" "pshow"       wm-pal-show)
(window-manager "add" "pchange"     wm-pal-change)
(window-manager "add" "wlist"       wm-win-list)
(window-manager "add" "wopen"       wm-win-open)
(window-manager "add" "wclose"      wm-win-close)
(window-manager "add" "wconnect"    wm-win-connect)
(window-manager "add" "wdisconnect" wm-win-disconnect)
(window-manager "add" "whardcopy"   wm-win-hardcopy)
(window-manager "add" "wupdate"     wm-win-update)
(window-manager "add" "vapply"      wm-vp-apply)
(window-manager "add" "vopen"       wm-vp-open)
(window-manager "add" "vclose"      wm-vp-close)
(window-manager "add" "vhardcopy"   wm-vp-hardcopy)
(window-manager "add" "vrender"     wm-vp-render)
(window-manager "add" "vattach"     wm-vp-attach)
(window-manager "add" "vdelete"     wm-vp-delete)
(window-manager "add" "vdlimit"     wm-vp-dlimit)
(window-manager "add" "vrlimit"     wm-vp-rlimit)
(window-manager "add" "vmove"       wm-vp-move)
(window-manager "add" "vlabel"      wm-vp-label)
(window-manager "add" "vlist"       wm-vp-list)
(window-manager "add" "vget-args"   wm-vp-get-args)
(window-manager "add" "vsattr"      wm-vp-set-attr)
(window-manager "add" "vfont"       wm-vp-font)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

