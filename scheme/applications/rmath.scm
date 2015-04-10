;
; RMATH.SCM - some runtime Scheme math procedures not implemented at
;	    - C level in the interpreter
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
;

(define %primes '( 2    3   5   7  11  13  17  19  23  29  31
                   37  41  43  47  53  59  61  67  71  73
		   79  83  89  97 101 103 107 109 113 127 131
                  137 139 149 151 157 163 167 173 179 181
                  191 193 197 199 211 223 227 229 233 239 241
                  251 257 263 269 271 277 281 283 293 307
                  311 313 317 331 337 347 349 353 359 367 373
                  379 383 389 397 401 409 419 421 431 433
                  439 443 449 457 461 463 467 479 487 491 499
		  503 509 521 523 541 547 557 563 569 571
                  577 587 593 599 601 607 613 617 619 631 641
                  643 647 653 659 661 673 677 683 691 701
                  709 710 727 733 739 743 751 757 761 769 773
                  787 797 809 811 821 823 827 829 839 853
                  857 859 863 877 881 883 887 907 911 919 929
                  937 941 947 953 967 971 977 983 991 997))

;--------------------------------------------------------------------------

;                         PRIMES AND FACTORING

;--------------------------------------------------------------------------

(define (find-divisor n d)
   (cond ((> (* d d) n)
	  n)
         ((= (remainder n d) 0)
	  d)
         (else
	  (find-divisor n (+ d 1)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (prime? n)
   (let* ((r (find-divisor n 2)))
         (= n r)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; FACTORS - return the number of times I divides N

(define (factors n i nt)
    (let* ((q (/ n i))
	   (j (floor q)))
          (if (= j q)
	      (factors j i (+ nt 1))
	      (list i nt n))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PRIME-FACTOR-AUX - do prime factorization of N

(define (prime-factor-aux n fl pl)
    (cond (pl
	   (let* ((sn (floor (sqrt n)))
		  (i  (list-ref pl 0))
		  (fc (factors n i 0))
		  (p  (list-ref fc 1))
		  (m  (list-ref fc 2))
		  (nf (if (> p 0) 
			  (cons (list i p) fl)
			  fl)))
	         (if (< m 2)
		     (reverse nf)
		     (prime-factor-aux m nf (list-tail pl 1)))))

	  ((prime? n)
	   (reverse (cons (list n 1) fl)))

	  (else
	   (printf nil "Exhausted list of primes - partial result only\n")
	   (reverse (cons (list n 1) fl)))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; PRIME-FACTOR - do prime factorization of N

(define (prime-factor n)
    (let* ((pc (map (lambda (x) x) %primes)))
          (prime-factor-aux n nil pc)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------
