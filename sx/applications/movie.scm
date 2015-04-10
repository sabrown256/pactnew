;
; MOVIE.SCM - SCHEME level functions for ULTRA II
;
; #include <cpyright.h>
;

;(printf nil "Loading MOVIE.SCM ... ")

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; MOVIE

(define (movie beg end incr ddd)
    "Procedure: Display the specified sequence of curve as a movie.
     Usage: movie [<start> [<stop> [<incr>]]]"
    (let* ((bg (if (null? beg) 1 beg))
	   (en (if (null? end) (n-curves-read) end))
	   (in (if (null? incr) 1 incr))
	   (dt (+ bg in)))
          (if (<= bg en)
	      (begin (set! bg (cur bg))
		     (if (not (null? ddd)) (del ddd))
		     (replot)
		     (movie dt en in bg)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SEQU

(define (sequ beg end incr)
    "Procedure: Display the specified sequence of curves from the menu.
     This is an alternative to the SELECT command.
     Usage: sequ [<start> [<stop> [<incr>]]]"
    (let* ((bg (if (null? beg) 1 beg))
	   (en (if (null? end) (n-curves-read) end))
	   (in (if (null? incr) 1 incr))
	   (dt (+ bg in)))
          (if (<= bg en)
	      (begin (set! bg (cur bg))
		     (replot)
		     (sequ dt en in)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

;(printf nil "done\n")
