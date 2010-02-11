;
; SLIB.SCM - standard stats definitions and procedures
;
; Source Version: 4.0
; Software Release #: LLNL-CODE-422942
;

; seconds from 00:00:00 1/1/1970 to 00:00:00 1/1/2000
; according to  'date -d "00:00:00 1/1/2000" +%s' on a linux box
; must be consistent with pact/scripts/do-stats
(define ref2000 946713600)

; time right now in seconds from 00:00:00 1/1/2000
(define refsecs  (- (string->number (strtok (list-ref (syscmnd "date +%s") 0) "\n")) ref2000))
(define refdays  (/ refsecs 86400))
(define refyears (/ refdays 365.2422))

(printf nil "Time since 00:00:00 1/1/2000: %s (secs)  %s (days)  %s (years)\n\n"
	refsecs	refdays	refyears)

(define (diva crv) (div a crv))
(define MINZERO #t)
(define MAXNDOM #t)
(define REPLOT #t)
(define TITLE "stats")
(define LABELSPACE 0)
(define CMDLISTS code-cmd-lists)
(define demo-list (list))
(define codehelp nil)

(define days 100)
(define label-scale-factor 1.0)
(type-face helvetica)
(label-type-size 10)
(label-length 60)
(label-color-flag TRUE)
(simple-append 1)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SH - stats help

(define (sh)
    (printf nil "\nCommands available in stats mode:\n\n")
    (printf nil "")
    (printf nil "    sc    [<pattern>]        - execute standard code command lists\n")
    (printf nil "                             - starting with first command list containing <pattern>\n")
    (printf nil "    scc                      - execute standard code command lists\n")
    (printf nil "                             - starting where previous sc left off\n")
    (printf nil "    scd   [<dir>]            - display or change stats directory (full path)\n")
    (printf nil "    sdays [<days>]           - set maximum trailing days for domain (default - 100)\n")
    (printf nil "    sh                       - display this list of commands\n")
    (printf nil "    sls   [<pattern-list>]   - list names of stats containing all patterns\n")
    (printf nil "    sobo  [<pattern-list>]   - show stats matching patterns one by one\n")
    (printf nil "    spl   <pattern-list>     - plot stats whose names contain all patterns\n")
    (printf nil "    spl+  <pattern-list>     - add plots of stats whose names contain all patterns\n")
    (printf nil "    ssf                      - display or change label space scale factor\n")
    (printf nil "    <ultra-command>          - ultra command\n")
    (printf nil "")
    (printf nil "\n    where\n\n")
    (printf nil "")
    (printf nil "    <pattern-list> - blank delimited list of patterns\n")
    (printf nil "                   - use pattern prefix < to match beginning of names\n")
    (printf nil "                   - use pattern suffix > to match end of names\n")
    (printf nil "                   - use pattern prefix ! to list/plot stats not matching pattern\n")
    (printf nil "\n")
    (if (procedure? codehelp) (codehelp)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; STITLE - display and save stat plot title

(define (stitle titl)
    (interactive off)
    (clr-annot)
    (set! TITLE titl)
    (annot TITLE black 0.0 1.0 (+ 0.96 (* LABELSPACE 0.02)) (+ 0.99 (* LABELSPACE 0.005))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SCD - display or set stats directory

(define-macro (scd . new-stats-directory)
    (if (null? new-stats-directory)
        (printf nil "\n%s\n\n" stats-directory)
        (let* ((dir (car new-stats-directory))
               (strdir (if (symbol? dir) (symbol->string dir) dir)))
            (set! stats-directory strdir))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SDAYS - set maximum number of trailing days for domain

(define (sdays . new-days)
    (if (null? new-days)
        (set! days 100)
        (set! days (car new-days)))
    (domain de)
    (let* ((mindom (car (domain)))
           (maxdom (cdr (domain))))
        (if (<= days (- maxdom mindom))
            (domain (- maxdom days) maxdom)))
    (replot))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SSF - display or set label-space scale factor

(define-macro (ssf . new-factor)
    (interactive off)
    (if (null? new-factor)
        (printf nil "\n%s\n\n" label-scale-factor)
        (let* ((old-factor label-scale-factor))
            (set! label-scale-factor (car new-factor))
            (set! LABELSPACE (label-space (* (/ label-scale-factor old-factor) LABELSPACE))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; RESIZE - set label-space to appropriate size

(define (resize)
    (interactive off)
    (set! LABELSPACE (label-space (* 0.00242 label-scale-factor (label-type-size) (length (lst))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; EMPTY? - empty list predicate

(define (empty? lszt)
    (if (= 1 (length (append (list 'a) lszt))) #t #f))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PAUSE - wait for user response

(define (pause . message)
    (if message
        (printf nil "%s" (car message)))
    (let* ((str1 (read-line)))
        (if (string=? str1 "y") #t #f)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; REM-LIST - return remainder of list starting with given element

(define (rem-list elem lszt)
    (if elem
        (if (eqv? elem (car lszt))
            lszt
            (if (cdr lszt) (rem-list elem (cdr lszt)) nil))
        nil))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PRINT-LIST - print list elements one per line

(define (print-list data)
    (if (and data (car data))
        (begin
            (printf nil "%s\n" (car data))
            (print-list (cdr data)))
        (printf nil "\n")))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; LIST-COMPACT - return list consisting of non-null elements of given list

(define (list-compact lszt)
    (if (empty? lszt)
        nil
        (if (list-ref lszt 0)
            (cons (list-ref lszt 0) (list-compact (list-tail lszt 1)))
            (list-compact (list-tail lszt 1)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; EQUALSTRING - return superstring if it matches substring exactly

(define-macro (equalstring super sub)
    (if (string=? super sub) super nil))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; NOTEQUALSTRING - return superstring if it does not match substring exactly

(define-macro (notequalstring super sub)
    (if (string=? super sub) nil super))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; BEGINSTRING - return superstring if it begins with substring

(define-macro (beginstring super sub)
    (if (and (strstr super sub) (string=? (substring super 0 (string-length sub)) sub)) super nil))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; NOTBEGINSTRING - return superstring if it does not begin with substring

(define-macro (notbeginstring super sub)
    (if (and (strstr super sub) (string=? (substring super 0 (string-length sub)) sub)) nil super))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; ENDSTRING - return superstring if it ends with substring

(define-macro (endstring super sub)
    (if (and (strstr super sub) (string=? (substring super (- (string-length super) (string-length sub)) (string-length super)) sub)) super nil))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; NOTENDSTRING - return superstring if it does not end with substring

(define-macro (notendstring super sub)
    (if (and (strstr super sub) (string=? (substring super (- (string-length super) (string-length sub)) (string-length super)) sub)) nil super))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SUPERSTRING - return superstring if it contains substring

(define-macro (superstring super sub)
    (if (strstr super sub) super nil))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; NOTSUPERSTRING - return superstring if it does not contain substring

(define-macro (notsuperstring super sub)
    (if (strstr super sub) nil super))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; LIST-STRING - make a list with n string elements

(define (list-string n s)
    (if (> n 0) (cons s (list-string (- n 1) s)) (list)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PATT-LIST - return list of elements in given list containing pattern

(define (patt-list lszt patt)
    (if (empty? lszt)
        (list)
        (let* ((ss (symbol->string patt))
               (sl (string-length ss))
               (pp (if (string=? "!" (substring ss 0 1)) (substring ss 1 sl) ss))
               (ll (string-length pp))
               (dl (not (= sl ll)))
               (bb (string=? "<" (substring pp 0 1)))
               (ee (string=? ">" (substring pp (- ll 1) ll)))
               (ff (cond ((and bb ee) (set! pp (substring pp 1 (- ll 1)))
                                      (if dl notequalstring equalstring))
                         (bb          (set! pp (substring pp 1 ll))
                                      (if dl notbeginstring beginstring))
                         (ee          (set! pp (substring pp 0 (- ll 1)))
                                      (if dl notendstring endstring))
                         (else        (if dl notsuperstring superstring)))))
            (list-compact (map ff lszt (list-string (length lszt) pp))))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MULTI-PATT-LIST - return list of elements in given list containing all specified patterns

(define (multi-patt-list lszt patts)
    (if (empty? lszt)
        (list)
        (if (and patts (car patts))
            (multi-patt-list (patt-list lszt (car patts)) (cdr patts))
            lszt)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; LSD - return list of files with names containing patterns

(define-macro (lsd dir patts)
    (define (clip arg)
        (strtok arg "\n"))
    (multi-patt-list (map clip (syscmnd (sprintf "ls %s" dir))) patts))

(define (lsd* . args) (apply lsd args))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SLS - list stats files containing all patterns

(define-macro (sls . patts)
    (printf nil "\n")
    (if patts
        (print-list (lsd* stats-directory patts))
        (system (sprintf "ls -1 %s" stats-directory))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; FC - plot first curve from file
;      convert seconds since 1/1/2000 to days ago (since right now)

(define-macro (fc file)
    (rd* (string-append stats-directory "/" file))
    (dx (divx (cur n-curves-read) 86400) (* refdays -1.0)))

(define (fc* . args) (apply fc args))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; FCP - plot curves from files with names matching all patterns

(define-macro (fcp patts eflag)
    (let* ((flist (lsd* stats-directory patts)))
        (if (empty? flist)
            #f
            (begin
                (if eflag (begin (erase) (kill all)))
		(map fc* flist)
		#t))))

(define (fcp* . args) (apply fcp args))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SET-LIMITS - set the domain and range

(define (set-limits)
    (let* ((odom (domain de))
           (omin (car odom))
           (maxdom (cdr odom))
           (mindom (max omin (- maxdom days))))
        (if (and (< mindom maxdom) (not (= mindom omin)))
            (domain mindom maxdom))
        (define (max-in-domain)
            (let* ((crva (append-curves (lst)))
                   (crvb (xmm (sort crva) mindom maxdom))
                   (maxran (apply max (cdr (curve->list crvb)))))
                (del crva)
                (del crvb)
                maxran))
        (let* ((oran (range de))
               (omin (car oran))
               (omax (cdr oran))
               (minran (if MINZERO 0 omin))
               (maxran (if MAXNDOM (max-in-domain) omax)))
            (if (and (< minran maxran) (or (not (= minran omin)) (not (= maxran omax))))
                (range minran maxran)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SPL - plot stats matching patterns
 
(define-macro (spl . args)
    (interactive off)
    (if (not (null? args))
        (let* ((eflag (not (eqv? (list-ref args (- (length args) 1)) 'noerase)))
               (patts (if eflag args (reverse (list-tail (reverse args) 1))))
               (olst (lst))
               (olen (if (empty? olst) 0 (length olst)))
               (ok (if patts (fcp* patts eflag) #f)))
            (if ok
                (begin
                    (set-limits)
                    (if (not (= (length (lst)) olen))
                        (resize))
                    (if REPLOT
                        (begin
                            (replot)
                            (stitle (sprintf "%s" (car args))))))))))

(define (spl* . args) (apply spl args))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SPL+ - plot stats matching patterns without erasing currently plotted curves

(define-macro (spl+ . args)
    (apply spl (append args (list 'noerase))))

(define (spl+* . args) (apply spl+ args))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SPLNR - render stats matching patterns but do not replot

(define-macro (splnr . args)
    (set! REPLOT #f)
    (apply spl args)
    (set! REPLOT #t))

(define (splnr* . args) (apply splnr args))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SPLNR+ - render stats matching patterns without erasing currently plotted curves
;        - but do not replot

(define-macro (splnr+ . args)
    (apply splnr (append args (list 'noerase))))

(define (splnr+* . args) (apply splnr+ args))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; OBO-DRIVER - driver for showing currently plotted curves one by one

(define (obo-driver sflag crvs)
    (if crvs
        (let* ((clst (lst))
               (carn (car crvs))
               (cdrn (cdr crvs)))
            (hide clst)
            (show carn)
            (if (or cdrn sflag)
                (begin
                    (replot)
                    (stitle (sprintf "%s" (get-label carn)))
                    (if (pause "   Enter y/n to continue/quit: ")
                        (obo-driver sflag cdrn)))
                (stitle (sprintf "%s" (get-label carn))))
            (if sflag (show clst)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; OBO - show currently plotted curves one by one

(define-macro (obo . crvs)
    (interactive off)
    (let* ((slst (if crvs crvs (lst))))
        (if (and slst (> (length slst) 1))
            (let* ((otitle TITLE))
                (printf nil "\n")
                (obo-driver #t slst)
                (stitle otitle)
                (printf nil "\n")))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SOBO - show stats matching patterns one by one

(define-macro (sobo . stats)
    (apply splnr stats)
    (printf nil "\n")
    (obo-driver #f (lst))
    (printf nil "\n"))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; EXEC-CMDS - execute a list of commands

(define (exec-cmds cmds)
    (let* ((cmd (car cmds))
           (remcmds (cdr cmds)))
        (eval cmd)
        (if remcmds
            (exec-cmds remcmds))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; EXEC-CMD-LISTS - execute a list of command lists

(define (exec-cmd-lists cmd-lists)
    (let* ((cmds (car cmd-lists))
           (rem-cmd-lists (cdr cmd-lists)))
        (exec-cmds cmds)
        (set! CMDLISTS rem-cmd-lists)
        (if rem-cmd-lists
            (if (pause "   Enter y/n to continue/quit: ")
                (exec-cmd-lists rem-cmd-lists)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; LIST-ELEMENT-SUBSTRING - pattern is substring of list element predicate

(define (list-element-substring lst patt)
    (if lst
	(let* ((ss (car lst))
	       (super (if (string? ss) ss (symbol->string ss))))
	    (if (strstr super (symbol->string (car patt)))
		#t
		(list-element-substring (cdr lst) patt)))
	#f))


;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; REM-LIST-PATT - return remainder of command lists
;               - starting at first list containing pattern

(define (rem-list-patt cmd-lists patt)
    (if cmd-lists
	(if (list-element-substring (apply append (car cmd-lists)) patt)
	    cmd-lists
	    (rem-list-patt (cdr cmd-lists) patt))))


;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SCC - execute code command lists continuation

(define (scc)
    (let* ((multi (if CMDLISTS (> (length CMDLISTS) 1) #f)))
        (if CMDLISTS
            (begin
                (if multi (printf nil "\n"))
                (exec-cmd-lists CMDLISTS)
                (if multi (printf nil "\n"))))))


;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SC - execute code command lists

(define-macro (sc . patt)
    (if patt
	(set! CMDLISTS (rem-list-patt code-cmd-lists patt))
	(set! CMDLISTS code-cmd-lists))
    (scc))


;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SD - show demos

(define-macro (sd . demo)
    (interactive off)
    (if (not (string=? stats-directory demo-directory))
        (begin
            (printf nil "\n   Changing to demo directory %s\n" demo-directory)
            (scd demo-directory)))
    (let* ((demos (if demo demo demo-list))
           (multi (> (length demos) 1)))
        (if multi (printf nil "\n"))
        (sdlist demos)
        (if multi (printf nil "\n"))))


;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SDLIST - show demo list

(define (sdlist demos)
    (let* ((nlst (if (empty? (lst)) 0 (length (lst))))
           (slabel (car (map (car demos) (list "phony")))))
        (re-id)
        (if (not (= (length (lst)) nlst)) (resize))
        (replot)
        (stitle slabel)
        (if (cdr demos)
            (if (pause "   Enter y/n to continue/quit: ")
                (sdlist (cdr demos)) #f) #t)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

