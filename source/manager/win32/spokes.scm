;
; SPOKES.SCM - initialization of SX translation spokes for medic-ng
;

(define (safe-load file)
  (if (file? file nil nil "global")
      (load file #t)
      (printf nil "Cannot find %s\\n" file)))

(safe-load "netcdf.scm")

