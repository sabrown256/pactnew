;
; SHSINT.SCM - integer syntax tests
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SCHEME number syntax from R5RS
;
; R is radix 2, 8, 10, or 16 (to avoid trivial replications)
; 
; <number>          := <num 2> | <num 8> | <num 10> | <num 16>
;
; <num R>           := <prefix R> <complex R>
; <complex R>       := <real R> | <real R> @ <real R>                    |
;                      <real R> + <ureal R> i | <real R> - <ureal R> i   |
;                      <real R> + i | <real R> - i                       |
;                      + <ureal R> i | - <ureal R> i                     |
;                      + i | - i
; <real R>          := <sign> <ureal R>
; <ureal R>         := <uinteger R>                                      |
;                      <uinteger R> / <uinteger R>                       |
;                      <decimal 10>
; <decimal 10>      := <uinteger 10> <suffix>                            |
;                      . <digit 10>+ #* <suffix>                         |
;                      <digit 10>+ . <digit 10>+ #* <suffix>             |
;                      <digit 10>+ #* . #* <suffix>
;
; <uinteger R>      := <digit R>+ #*
; <prefix R>        := <digit R>+ #*                                     |
;                      <exactness> <radix R>
; <suffix>          := <empty>                                           |
;                      <exponent marker> <sign> <digit 10>+
; <exponent marker> := e | s | f | d | l
; <sign>            := <empty> | + | -
; <exactness>       := <empty> | #i | #e
;
; <radix 2>         := #b
; <radix 8>         := #o
; <radix 10>        := <empty> | #d
; <radix 16>        := #x
;
; <digit 2>         := 0 | 1
; <digit 8>         := <digit 2> | 2 | 3 | 4 | 5 | 6 | 7
; <digit 10>        := <digit 8> | 8 | 9
; <digit 16>        := <digit 10> | a | b | c | d | e | f |
;                      A | B | C | D | E | F

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define (test x s)
    (if (integer? x)
        (printf nil "%-10s integer %d\n" s x)
        (printf nil "%-10s not integer\n" s)))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; these would fail if the variables to be defined were numbers
(define # "#")
(define #1 "#1")
(define #-1 "#-1")      ; GUILE uses this form but it is illegal number syntax
(define #l "#l")
(define 123u "123u")
(define #xabj "#xabj")

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(test 12 "12")
(test #b11 "#b11")
(test #o67 "#o67")
(test #x7f "#x7f")
(test +12 "+12")
(test +#b11 "+#b11")
(test +#o67 "+#o67")
(test +#x7f "+#x7f")
(test -12 "-12")
(test -#b11 "-#b11")
(test -#o67 "-#o67")
(test -#x7f "-#x7f")

; unimplemented until someone can explain what this means
; i.e. what is the value supposed to be
; one source had 15## = 1500.0
;(test 2# "2#")
;(test 2## "2##")
;(test #xf## "#xf##")

; error cases
(test + "+")
(test - "-")

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(quit)
