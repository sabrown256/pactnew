;
; LANG.SCM - language extensions for alternate syntax
;
; Source Version: 5.0
; Software Release #: LLNL-CODE-422942
;
; #include "cpyright.h"
;

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; SCHEME->C name conversion conventions:
; this covers Fortranish languages also
;
;     SCHEME         C
;        -           _
;        *           _
;        ?           p
;        !           b
;       ->          _to_
;

(if (not (defined? have-language-synonyms))
    (define have-language-synonyms #f))

(define (language-synonyms)
  (if (not have-language-synonyms)
      (begin (set! have-language-synonyms #t)
             (synonym char?             charp)
	     (synonym char=?            char_eqp)
	     (synonym char>=?           char_gep)
	     (synonym char>?            char_gtp)
	     (synonym char<=?           char_lep)
	     (synonym char<?            char_ltp)
	     (synonym char->integer     char_to_integer)
	     (synonym hash-element?     hash_elementp)
	     (synonym hash-table?       hash_tablep)
	     (synonym integer->char     integer_to_char)
	     (synonym interactive?      interactivep)
	     (synonym list->string      list_to_string)
	     (synonym list->vector      list_to_vector)
	     (synonym string=?          string_eqp)
	     (synonym string>=?         string_gep)
	     (synonym string>?          string_gtp)
	     (synonym string<=?         string_lep)
	     (synonym string<?          string_ltp)
	     (synonym string->list      string_to_list)
	     (synonym string->number    string_to_number)
	     (synonym string->port      string_to_port)
	     (synonym string->symbol    string_to_symbol)
	     (synonym symbol->string    symbol_to_string)
	     (synonym vector?           vectorp)
	     (synonym vector->list      vector_to_list)
	     (synonym vector-set!       vector_setb)
	     (synonym even?             evenp)
	     (synonym negative?         negativep)
	     (synonym odd?              oddp)
	     (synonym positive?         positivep)
	     (synonym zero?             zerop)
	     (synonym process-running?  process_runningp)
	     (synonym process?          processp)
	     (synonym boolean?          booleanp)
	     (synonym defined?          definedp)
	     (synonym eof-object?       eof_objectp)
	     (synonym eq?               eqp)
	     (synonym equal?            equalp)
	     (synonym eqv?              eqvp)
	     (synonym file?             filep)
	     (synonym ascii-file?       ascii_filep)
	     (synonym input-port?       input_portp)
	     (synonym integer?          integerp)
	     (synonym null?             nullp)
	     (synonym number?           numberp)
	     (synonym output-port?      output_portp)
	     (synonym pair?             pairp)
	     (synonym procedure?        procedurep)
	     (synonym real?             realp)
	     (synonym set!              setb)
	     (synonym set-car!          set_carb)
	     (synonym set-cdr!          set_cdrb)
	     (synonym string?           stringp)
	     (synonym symbol?           symbolp))))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

