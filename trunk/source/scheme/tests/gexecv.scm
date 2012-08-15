;
; GEXECV.SCM - gexec variable tests
;
; include "cpyright.h"
;

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

;(process-group-debug 3)

(! osrc -c 1 -o 1 -t v1 @o vw:gexo1)
(! osrc -c 1 -o 1 -t v2 @b vw:gexb1)
(! osrc -c 1 -o 1 -t v3 @o2 @e3 vw:gexo2 @ vw:gexe2)

(printf nil "gexo1: '%s'\n" (getenv "gexo1"))
(printf nil "gexb1: '%s'\n" (getenv "gexb1"))
(printf nil "gexo2: '%s'\n" (getenv "gexo2"))
(printf nil "gexe2: '%s'\n" (getenv "gexe2"))

(quit)

