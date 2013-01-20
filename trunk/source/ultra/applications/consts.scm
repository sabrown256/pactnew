;
; CONSTS.SCM - symbolic constants for the ULTRA II environment
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
;

(define %pi 3.141592654)
(define %e  2.718281828)

(define all   0)
(define on    1)
(define off   0)
(define auto -1)
(define de  'de)

; interpolation methods
(define idw  0)
(define mq  -1)

;
; PGS graphics related constants
;

; key modifiers
(define key-shift #x10)
(define key-cntl  #x20)
(define key-alt   #x40)
(define key-lock  #x80)

; mouse buttons
(define mouse-left       1)
(define mouse-middle     2)
(define mouse-right      4)
(define mouse-wheel-up   8)
(define mouse-wheel-down 16)
(define mouse-down       4)
(define mouse-up         5)

; colors
(define black       0)
(define white       1)
(define gray        2)
(define dark-gray   3)
(define blue        4)
(define green       5)
(define cyan        6)
(define red         7)
(define magenta     8)
(define brown       9)
(define dark-blue  10)
(define dark-green 11)
(define dark-cyan  12)
(define dark-red   13)
(define yellow     14)
(define purple     15)
(define no-fill    -100)

; fonts
(define helvetica "helvetica")
(define times     "times")
(define courier   "courier")

(define medium      "medium")
(define italic      "italic")
(define bold        "bold")
(define bold-italic "bold-italic")

; coordinate systems
(define @world  1)
(define @norm   2)
(define @pixel  3)
(define @bound  4)
(define @frame  5)

; axis types
(define cartesian     -1)
(define cartesian-3d  -2)
(define polar         -3)
(define insel         -4)

; markers
(if (defined? pdbview)
    (begin
      (define plus     1)
      (define star     2)
      (define triangle 3)

      (define no-std       -1)
      (define text-std      0)
      (define i386-std      1)
      (define i586l-std     2)
      (define i586o-std     3)
      (define ppc32-std     4)
      (define x86-64-std    5)
      (define x86-64a-std   6)
      (define ppc64-std     7)
      (define m68x-std      8)
      (define vax-std       9)
      (define cray-std     10)

      (define no-algn           -1)
      (define text-algn          0)
      (define byte-algn          1)
      (define word2-algn         2)
      (define word4-algn         3)
      (define word8-algn         4)
      (define gnu4-i686-algn     5)
      (define osx-10-5-algn      6)
      (define sparc-algn         7)
      (define xlc32-ppc64-algn   8)
      (define cygwin-i686-algn   9)
      (define gnu3-ppc64-algn   10)
      (define gnu4-ppc64-algn   11)
      (define xlc64-ppc64-algn  12)
      (define gnu4-x86-64-algn  13)
      (define pgi-x86-64-algn   14))
    (begin
      (define plus     0)
      (define star     1)
      (define triangle 2)))

; text and histogram alignment
(define left     0)
(define right    1)
(define center   2)

; tick placement
(define right-of-axis  113)
(define left-of-axis   114)
(define straddle-axis  115)

; line styles
(define solid     1)
(define dashed    2)
(define dotted    3)
(define dotdashed 4)

; renderings
(define contour         11)
(define image           12)
(define wire-frame      13)
(define shaded          14)
; NOTE: name vector would conflict with function
(define vect            15)
(define fill-poly       16)
(define poly-fill       16)
(define mesh            17)
(define scatter-plot    19)
(define points          19)
(define dvb             20)
(define cartesian-plot  22)
(define polar-plot      23)
(define insel-plot      24)
(define histogram-plot  25)
(define logical-plot    26)

; palettes
(define standard "standard")
(define spectrum "spectrum")
(define rainbow  "rainbow")
(define bw       "bw")
(define magentas "magentas")
(define blues    "blues")
(define cyans    "cyans")
(define greens   "greens")
(define yellows  "yellows")
(define reds     "reds")

; vector attributes
(define vector-scale           1)
(define vector-angle           2)
(define vector-headsize        3)
(define vector-fixsize         4)
(define vector-maxsize         5)
(define vector-linestyle       6)
(define vector-linethick       7)
(define vector-color           8)
(define vector-fixhead         9)

