;
; PDPTR.SCM - test PDB eager pointer machinery
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
;

;(eager-pdb-pointers off)

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; create data file

cf pdptr.pdb "w"

mkdir /ints
cd /ints
var int cn(2 5) 1 2 3 4 5 6 7 8 9 10
var int *an (1 2 3 4)
var int *en(3) (-1 -2 -3) (40 50) (6 7 8 9)
var int **sn ((1 2 3 4 5) (6 7 8 9))
cd /

mkdir /floats
cd /floats
var float cs(5 2) 1 2 3 4 5 6 7 8 9 10
var float *as (1 2 3 4)
var float *es(3) (-1 -2 -3) (40 50) (6 7 8 9)
var float **ss ((1 2 3 4 5) (6 7 8 9))
cd /

close pdptr.pdb

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------

; open the file

cf pdptr.pdb

ls

; these should not work with eager pointers off

printf nil "---------------------------------------\n"
printf nil "  full path\n"

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
printf nil "  /ints directory\n"

cd /ints

print cn
print an
print en
print sn

; these should work in all cases

printf nil "\n"
printf nil "---------------------------------------\n"
printf nil "  /floats directory\n"

cd /floats

print cs
print as
print es
print ss

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------
end
