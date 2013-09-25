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
	 (set! status (+ status (apply + err)))))

(printf nil "\n")
(printf nil "String tokenizer tests:\n")
(test-null-tok strtok)
(test-null-tok lasttok)

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

; SUBSTRING

(define (test-substring)
    (let* ((src "0123456789"))

          (define (test-substring s n1 n2 res)
	      (let* ((r (substring s n1 n2)))
		    (printf nil "   %s %s  (%d,%d) -> %s\n"
			    (if (string=? r res)
				"ok"
				(begin (set! status (+ status 1))
				       "ng"))
			    s n1 n2 r)))

	  (define (test-substring-1 s res)
	      (let* ((r (substring s)))
		    (printf nil "   %s %s  -> %s\n"
			    (if (string=? r res)
				"ok"
				(begin (set! status (+ status 1))
				       "ng"))
			    s r)))

	  (define (test-substring-2 s n1 res)
	      (let* ((r (substring s n1)))
		    (printf nil "   %s %s  (%d) -> %s\n"
			    (if (string=? r res)
				"ok"
				(begin (set! status (+ status 1))
				       "ng"))
			    s n1 r)))

; left end limit from the left end

	  (test-substring "0123456789" 2 3 "2")
	  (test-substring "0123456789" 2 5 "234")
	  (test-substring "0123456789" 2 9 "2345678")

; off the right end
	  (test-substring "0123456789" 2 10 "23456789")
	  (test-substring "0123456789" 2 20 "23456789")

; bad right end limits
	  (test-substring "0123456789" 2  2 "")
	  (test-substring "0123456789" 2  1 "")
	  (test-substring "0123456789" 2  0 "")
	  (test-substring "0123456789" 2 -1 "2345678")

; left end limit from the right end

	  (test-substring "0123456789" -8 3 "2")
	  (test-substring "0123456789" -8 9 "2345678")
	  (test-substring "0123456789" -8 10 "23456789")
	  (test-substring "0123456789" -8 20 "23456789")

	  (test-substring "0123456789" -8 -7 "2")
	  (test-substring "0123456789" -8 -2 "234567")
	  (test-substring "0123456789" -8 -1 "2345678")
	  (test-substring "0123456789" -8  0 "")

; off the right end
	  (test-substring "0123456789" -8  9 "2345678")
	  (test-substring "0123456789" -8 10 "23456789")
	  (test-substring "0123456789" -8 20 "23456789")

; bad right end limits
	  (test-substring "0123456789" -8  2 "")
	  (test-substring "0123456789" -8  1 "")
	  (test-substring "0123456789" -8  0 "")
	  (test-substring "0123456789" -8 -1 "2345678")

; special cases

	  (test-substring-1 "0123456789" "0123456789")

	  (test-substring-2 "0123456789" 0 "0123456789")
	  (test-substring-2 "0123456789" 2 "23456789")
	  (test-substring-2 "0123456789" 20 "")
	  (test-substring-2 "0123456789" -2 "89")
	  (test-substring-2 "0123456789" -20 "0123456789")))

(printf nil "\n")
(printf nil "Substring tests:\n")
(test-substring)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(quit status)

