;
; ULTRA.SCM - load up the ULTRA II SCHEME code
;
; Source Version: 4.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
;

(define ultra #t)

(load "runt.scm" #t)
(load "consts.scm" #t)
(load "func.scm" #t)
(load "hardcopy.scm" #t)
(load "slice.scm" #t)
(load "movie.scm" #t)
(load "synon.scm" #t)
(load "toolbox.scm" #t)

;(autoload  'do            "runt.scm" #f)
(autoload* 'annot         "annotate.scm" #f)
(autoload* 'clr-annot     "annotate.scm" #f)
(autoload  'help          "help.scm" #f)
(autoload  'compare       "compare.scm" #f)
(autoload* 'diff-measure  "compare.scm" #f)
(autoload* 'diff-lnnorm   "compare.scm" #f)
