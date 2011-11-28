;
; PDPTR.SCM - test PDB eager pointer machinery
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
;

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

(define-macro (var? x)
    (printf nil "%s %s a file variable\n"
	    x
	    (if (file-variable? current-file x) "is" "is not")))

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; create data file

cf pdptr.pdb "w"

mkdir /ints
cd /ints
integer cn(2 5) 1 2 3 4 5 6 7 8 9 10
integer *an (1 2 3 4)
integer *en(3) (-1 -2 -3) (40 50) (6 7 8 9)
integer **sn ((1 2 3 4 5) (6 7 8 9))
cd /

mkdir /floats
cd /floats
float cs(5 2) 1 2 3 4 5 6 7 8 9 10
float *as (1 2 3 4)
float *es(3) (-1 -2 -3) (40 50) (6 7 8 9)
float **ss ((1 2 3 4 5) (6 7 8 9))
cd /

close pdptr.pdb

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; open the file in delay pointer mode

cf pdptr.pdb "rp"

ls

; these should not work with delayed pointers

printf nil "---------------------------------------\n"
printf nil "  full path, delay pointers mode\n"

/ints/cn
/ints/an
/ints/en
/ints/sn

/floats/cs
/floats/as
/floats/es
/floats/ss

; these should work in all cases

printf nil "\n"
printf nil "---------------------------------------\n"
printf nil "  /ints directory, delay pointers mode\n"

cd /ints

varprint cn
varprint an
varprint en
varprint sn

; these should work in all cases

printf nil "\n"
printf nil "---------------------------------------\n"
printf nil "  /floats directory, delay pointers mode\n"

cd /floats

varprint cs
varprint as
varprint es
varprint ss

close pdptr.pdb

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; open the file in delay all mode

cf pdptr.pdb "rs"

ls

; these should not work with delayed symbols

printf nil "---------------------------------------\n"
printf nil "  full path, delay all mode\n"
printf nil "\n"

var? /ints/cn
var? /ints/an
var? /ints/en
var? /ints/sn

var? /floats/cs
var? /floats/as
var? /floats/es
var? /floats/ss

; these should work in all cases

printf nil "\n"
printf nil "---------------------------------------\n"
printf nil "  /ints directory, delay all mode\n"
printf nil "\n"

cd /ints

var? /ints/cn
var? /floats/cs

varprint cn
varprint an
varprint en
varprint sn

; these should work in all cases

printf nil "\n"
printf nil "---------------------------------------\n"
printf nil "  /floats directory, delay all mode\n"
printf nil "\n"

cd /floats

var? /ints/cn
var? /floats/cs

varprint cs
varprint as
varprint es
varprint ss

close pdptr.pdb

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

end
