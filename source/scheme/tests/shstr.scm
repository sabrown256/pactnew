;
; SHSTR.SCM - string handling tests
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(define status 0)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TEST-STRING-COMPARISONS - test the string comparison predicates

(define (test-string-comparisons)
    (let* ((s (make-string 10000)))

          (define (check res expect ok)
	      (if res
		  (if (eq? (car res) (car expect))
		      (check (cdr res) (cdr expect) #t)
		      (begin (set! status 10)
			     "fail"))
		  "ok"))

          (define (compare fnc . expect)
              (let* ((res (list (fnc "a" "a")
				(fnc "a" "A")
				(fnc "a" "abc")
				(fnc "a" "ABC"))))
		     (printf nil "-> %-12s  %-6s %s %s\n"
			     fnc
			     (check res expect #t)
			     res expect)))

	  (compare string=?     #t #f #f #f)
	  (compare string<?     #f #f #t #f)
	  (compare string<=?    #t #f #t #f)
	  (compare string>?     #f #t #f #t)
	  (compare string>=?    #t #t #f #t)

	  (compare string-ci=?  #t #t #f #f)
	  (compare string-ci<?  #f #f #t #t)
	  (compare string-ci<=? #t #t #t #t)
	  (compare string-ci>?  #f #f #f #f)
	  (compare string-ci>=? #t #t #f #f)))

(printf nil "\n")
(printf nil "String comparison primitives tests:\n")
(test-string-comparisons)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TEST-NULL-TOK - test string tokenizers in cases where they might return nil

(define (test-null-tok fnc)
   (let* ((err #f))

         (define (driver x)
	     (let* ((s (sprintf "%s" (list-ref x 0)))
		    (d (sprintf "%s" (list-ref x 1)))
		    (v #f))
		   (printf nil "-> %s(\"%s\", \"%s\") = " fnc s d)
	           (set! v (fnc s d))
		   (printf nil "|%s|\n" v)
		   (if (null? v) 0 11)))

	 (printf nil "   %s:\n" fnc)
	 (set! err (map driver '(("aaaaa" "a")
				 ("abc" "abc")
				 ("   " "  "))))
	 (apply + err)))

(printf nil "\n")
(printf nil "String tokenizer tests:\n")
(set! status (test-null-tok strtok))
(set! status (test-null-tok lasttok))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TEST-LONG-STRING - test primitives with long strings

(define (test-long-string)
    (let* ((s (make-string 10000)))

          (printf nil "-> make-string    |%s|\n" s)
          (string-fill! s #\-)
          (printf nil "-> string-fill!   |%s|\n" s)

          (string-set! s 0 #\A)
          (string-set! s 1 #\B)
          (string-set! s 2 #\C)
          (string-set! s 4 #\D)
          (string-set! s 5 #\E)
          (string-set! s 6 #\F)
          (string-set! s 9999 #\Z)
          (printf nil "-> string-set!    |%s|\n" s)

          (printf nil "-> string         |%s|\n" (string #\1 #\2 #\3))
          (printf nil "-> string?        |%s|\n" (string? s))
          (printf nil "-> substring      |%s|\n" (substring s 0 2))
          (printf nil "-> string-length  |%d|\n" (string-length s))
          (printf nil "-> string-ref     |%c|\n" (string-ref s 4))
          (printf nil "-> string-copy    |%s|\n" (string-copy (substring s 4 10)))
          (printf nil "-> list->string   |%s|\n" (list->string '(#\a #\b #\c)))
          (printf nil "-> string->list   |%s|\n" (string->list "foo"))
          (printf nil "-> string->append |%s|\n" (string-append "ab" "cd" "ef"))))

(printf nil "\n")
(printf nil "String primitives tests:\n")
(test-long-string)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; TEST-STRING-EXTR - test string extraction functions

(define (test-string-extr)
    (let* ((src "foo$bar$$baz")
	   (sc  (strchr src "$"))
	   (ss  (strstr src "$"))
	   (sic (istrchr src "$"))
	   (sis (istrstr src "$"))
	   (suc (substring src 0 sic))
	   (sus (substring src 0 sis)))

          (printf nil "-> source string  |%s|\n" src)
          (printf nil "-> strchr '$'     |%s|\n" sc)
          (printf nil "-> strstr '$'     |%s|\n" ss)
          (printf nil "-> istrchr '$'    |%d|\n" sic)
          (printf nil "-> istrstr '$'    |%d|\n" sis)
          (printf nil "-> sub istrchr    |%s|\n" suc)
          (printf nil "-> sub istrstr    |%s|\n" sus)))

(printf nil "\n")
(printf nil "Substring primitives tests:\n")
(test-string-extr)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(quit status)

