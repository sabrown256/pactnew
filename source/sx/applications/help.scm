;
; HELP.SCM - the help package for ULTRA II and PDBView
;
; Source Version: 4.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
;

(define funob (cond ((defined? pdbview)
		     "Mapping")
		    ((defined? ultra)
		     "Curve")
		    (else
		     "Nothing")))

(define (help-cmd)
   (printf nil " io  - I/O Commands\n")
   (printf nil " ma  - Math Operations Which Do Not Generate a New %s\n" funob)
   (printf nil " m+  - Math Operations Which Do Generate a New %s\n" funob)
   (printf nil " ie  - Environmental Inquiry Commands\n")
   (printf nil " ic  - %s Inquiry Commands\n" funob)
   (printf nil " ce  - Environmental Control Commands\n")
   (printf nil " cp  - Plot Control Commands\n")
   (printf nil " cc  - %s Control Commands\n" funob)
   (printf nil " ex  - Commands Useful for Writing Extensions\n")
   (printf nil " cn  - Constants\n")
   (printf nil " var - Variables\n")
   (printf nil " sy  - Synonyms\n")
   (printf nil "\n To list the members of a group, enter:    help <group-id>")
   (printf nil "\n To list the members of all groups, enter: help all")
   (printf nil "\n For information about a command, enter:   help <command-name>")
   (printf nil "\n For information about a variable, enter:  help <variable-name>")
   (printf nil "\n If you only remember part of a name, try: apropos <substring>\n"))

(define (help-io)
   (printf nil " I/O Commands:\n")
   (if (defined? pdbview)
       (begin
	  (printf nil "   autoload cf change change-dimension close close-textfile\n")
	  (printf nil "   cm command-log copy hcv hcw ld load-palette open-textfile\n")
	  (printf nil "   print save-palette table\n"))
       (begin
	  (printf nil "   autoload command-log compare hardcopy hc1 ld merge np rd\n")
	  (printf nil "   print-menu read-table resume save stop\n"))))

(define (help-ma)
   (printf nil " Math Operations Which Do Not Generate a New %s:\n" funob)
   (if (defined? pdbview)
       (begin
	 (printf nil "   abs absx acos acosx asin asinx atan atanx cos cosh coshx cosx dx dy divx\n")
	 (printf nil "   divy exp expx j0 j0x j1 j1x jn jnx ln lnx log10 log10x mx my powa powax\n")
         (printf nil "   powr powrx recip recipx sin sinh sinhx sinx sqr sqrt sqrtx sqrx tan tanh\n")
	 (printf nil "   tanhx tanx y0 y0x y1 y1x yn ynx\n"))
       (begin
	 (printf nil "   abs absx acos acosx asin asinx atan atanx cos cosh coshx cosx dx dy divx\n")
	 (printf nil "   divy error-bar exp expx filter filter-coef j0 j0x j1 j1x jn jnx ln lnx\n")
	 (printf nil "   log10 log10x mx my powa powax powr powrx recip recipx sin sinh sinhx sinx\n")
	 (printf nil "   smo smooth3 smooth5 sqr sqrt sqrtx sqrx tan tanh tanhx tanx xmax xmin\n")
	 (printf nil "   y0 y0x y1 y1x ymax ymin yn ynx\n"))))

(define (help-m+)
  (printf nil " Math Operations Which Do Generate a New %s:\n" funob)
   (if (defined? pdbview)
       (begin
	 (printf nil "   + - * / plane span\n"))
       (begin
	 (printf nil "   + - * / append-curves average cfft compose convol convolb\n")
	 (printf nil "   correl delta derivative diff-lnnorm diff-measure diffraction edit extract fft fit\n")
	 (printf nil "   fitcurve fode gaussian hypot ifft integrate max min normalize span\n")
	 (printf nil "   theta thin vs\n"))))

(define (help-ie)
   (printf nil " Environment Inquiry Commands:\n")
   (if (defined? pdbview)
       (begin
	  (printf nil "   apropos display desc file help list-files list-palettes ls\n")
	  (printf nil "   list-windows ls-attr lst lsv menu n-entries pwd struct types\n"))
       (begin
	   (printf nil "   apropos display file-info help lst menu menui table-attributes\n"))))

(define (help-ic)
   (printf nil " %s Inquiry Commands:\n" funob)
   (if (defined? pdbview)
       (begin
	  (printf nil "   get-domain get-domain-number-points get-label get-range\n")
	  (printf nil "   get-range-number-points pm\n"))
       (begin
	  (printf nil "   disp get-attributes get-domain get-label get-number-points get-range\n")
	  (printf nil "   getx gety stats\n"))))

(define (help-ce)
   (printf nil " Environment Control Commands:\n")
   (if (defined? pdbview)
       (begin
	  (printf nil "   cd end format mode set\n"))
       (begin
	  (printf nil "   close-device end erase kill open-device plots prefix replot screen\n")
          (printf nil "   syscmnd system\n"))))

(define (help-cp)
   (printf nil " Plot Control Commands:\n")
   (if (defined? pdbview)
       (begin
	  (printf nil "   animate change-palette clv clw create-palette cv cw\n")
	  (printf nil "   data-id dl domv font levels light-source nxm overlay pl pld plot\n")
	  (printf nil "   ranv refmesh refmeshcolor set-vector-att show-palettes\n")
	  (printf nil "   view-angle vr wu\n"))
       (begin
	  (printf nil "   annot autoplot axis data-id domain grid marker-scale mk-pal range\n")
	  (printf nil "   rd-pal x-log-scale y-log-scale\n"))))

(define (help-cc)
   (printf nil " %s Control Commands:\n" funob)
   (if (defined? pdbview)
       (begin
	  (printf nil "   domm fill histogram label-drawable\n")
	  (printf nil "   lncolor lnstyle lnwidth logical marker-scale\n")
	  (printf nil "   move ranm scatter\n"))
       (begin
	  (printf nil "   color copy copy-curve del dupx hide histogram label line lnstyle lnwidth\n")
	  (printf nil "   make-curve make-filter marker random re-color re-id rev scatter select set-id\n")
	  (printf nil "   show sort table-curve xindex xmm\n"))))

(define (help-ex)
   (printf nil " Commands Useful for Writing Extensions:\n")
   (if (defined? pdbview)
       (begin
	  (printf nil "   autoload* ac dref def defv lr lrs\n"))
       (begin
	  (printf nil "   autoload* close-device* curve? curve->list file-info* interactive ld* lst*\n")
	  (printf nil "   make-curve* menu* open-device* pre prefix* rd* read-table* save* synonym thru\n")
	  (printf nil "   ultra-file?\n"))))

(define (help-var)
   (printf nil " Variables:\n")
   (printf nil "   answer-prompt fix-output-format float-output-format complex-output-format\n")
   (printf nil "   axis-grid-style axis-line-style\n")
   (printf nil "   axis-line-width axis-max-major-ticks axis-n-decades axis-number-minor-ticks\n")
   (printf nil "   axis-number-minor-x-ticks axis-number-minor-y-ticks\n")
   (printf nil "   axis-tick-size axis-tick-type axis-type axis-x-format axis-y-format\n")
   (printf nil "   background-color-flag border-width botspace cgm-background-color-flag\n")
   (printf nil "   cgm-flag cgm-name cgm-type console-height console-origin-x console-origin-y\n")
   (printf nil "   console-width console-type default-color default-npts display-name\n")
   (printf nil "   display-title display-type error-bar-cap-size hide-rescale jpeg-flag\n")
   (printf nil "   jpeg-name jpeg-type label-color-flag label-length label-space\n")
   (printf nil "   label-type-size leftspace lines-page mpeg-flag mpeg-name mpeg-type\n")
   (printf nil "   n-curves n-curves-read plot-date plot-labels plot-type print-flag\n")
   (printf nil "   print-stats prompt ps-flag ps-name ps-type png-flag png-name png-type\n")
   (printf nil "   rightspace simple-append smooth-method squeeze-labels topspace type-face\n")
   (printf nil "   type-size type-style view-height view-origin-x view-origin-y view-width\n")
   (printf nil "   window-height window-height-cgm window-height-png window-height-ps\n")
   (printf nil "   window-height-jpeg window-height-mpeg window-origin-x window-origin-x-cgm\n")
   (printf nil "   window-origin-x-png window-origin-x-ps window-origin-x-jpeg window-origin-x-mpeg\n")
   (printf nil "   window-origin-y window-origin-y-cgm window-origin-y-png window-origin-y-ps\n")
   (printf nil "   window-origin-y-jpeg window-origin-y-mpeg window-width window-width-cgm\n")
   (printf nil "   window-width-png window-width-ps window-width-jpeg window-width-mpeg\n"))

(define (help-cn)
   (printf nil " Constants:\n")
   (printf nil "   cartesian (-1) polar (-2) insel (-3) left (-10) right (-11) center (-12)\n")
   (printf nil "   right-of-axis (9) left-of-axis (10) straddle-axis (11)\n")
   (printf nil "   solid (1) dashed (2) dotted (3) dotdashed (4)\n")
   (printf nil "   plus (0) star (1) triangle (2)\n")
   (printf nil "   all (-9) on (1) off (0) auto (-1) %%pi (3.141592654) %%e (2.718281828)\n")
   (printf nil "   helvetica ('helvetica') times ('times') courier ('courier')\n")                  
   (printf nil "   medium ('medium') italic ('italic')\n")
   (printf nil "   bold ('bold') bold-italic ('bold-italic')\n")
   (printf nil "   black (0) white (1) gray (2) dark-gray (3) blue (4) green (5) cyan (6)\n")
   (printf nil "   red (7) magenta (8) brown (9) dark-blue (10) dark-green (11)\n")
   (printf nil "   dark-cyan (12) dark-red (13) yellow (14) purple (15)\n"))

(define (help-sy)
   (printf nil " Synonyms:\n")
   (if (defined? pdbview)
       (begin
	  (printf nil "   (array-nd-set nds) (change-dimension chdim) (close-viewport clv)\n");
	  (printf nil "   (close-window clw) (change-file cf open) (change-palette palette)\n")
	  (printf nil "   (change-viewport cv) (change-window cw) (copy-mapping cm)\n")
	  (printf nil "   (data-reference dref) (default-viewport-rendering vr)\n")
	  (printf nil "   (define-file-variable defv) (define-file-variable* defv*)\n")
	  (printf nil "   (delete-mapping dl) (display-domain pld) (display-mapping pl dm)\n")
	  (printf nil "   (drawable-rendering dr) (domv fix-domain) (ranm fix-range)\n")
	  (printf nil "   (hardcopy-viewport hcv) (hardcopy-window hcw hc) (list-files lf)\n")
	  (printf nil "   (list-mappings-in-viewport lsv) (make-ac-mapping-direct ac)\n")
	  (printf nil "   (make-lr-mapping-direct lr) (make-lr-mapping-synth lrs)\n")
	  (printf nil "   (make-cp-set cps) (print-mapping pm) (update-window wu)\n")
	  (printf nil "   (view-angle va) (open-textfile ot) (close-textfile ct)\n")
	  (printf nil "   (table tab)\n"))
       (begin
	  (printf nil "   (+ sum) (- dif) (* prod) (/ quot ratio div) (compose comp)\n")
	  (printf nil "   (derivative der) (domain dom) (erase era) (integrate int)\n")
	  (printf nil "   (hardcopy hc) (kill expunge) (log10x logx) (powr pow) (powrx powx)\n")
	  (printf nil "   (range ran) (select # cur) (show unhide) (smooth3 smooth)\n"))))

(define	(help-all)
   (begin
     (help-io) (help-ma) (help-m+) (help-ie) (help-ic) (help-ce)
     (help-cp) (help-cc) (help-ex) (help-var) (help-cn) (help-sy)))

(define (eql? a b)
   (string=? (sprintf "%s" a) (sprintf "%s" b)))

(define-macro (help . args)
   "Usage: help [<group-id> | all | <command-name> | <variable-name>]"
   (printf nil "\n")

   (plot-flag off)

   (if (null? args)
       (help-cmd)
       (let* ((arg (car args))
	      (fname (string->symbol (sprintf "help-%s" arg)))
	      (v1 (apply defined? (list fname)))
	      (evarg (if v1 (eval fname) (eval arg))))

            (define (isit? x) (eql? evarg x))

	    (if (isit? 'help-all)
		(help-all)
	        (if (or (isit? help-io) (isit? help-ma) (isit? help-m+)
			(isit? help-ie)	(isit? help-ic) (isit? help-ce)
			(isit? help-cp) (isit? help-cc)	(isit? help-ex)
			(isit? help-var) (isit? help-cn) (isit? help-sy)
			(isit? help-cmd))
		    (evarg)
		    (describe* args)))))
   (printf nil "\n"))

