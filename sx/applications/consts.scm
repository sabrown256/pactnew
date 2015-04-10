;
; CONSTS.SCM - symbolic constants for the ULTRA II environment
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
;

(define %pi 3.14159265358979323)
(define %e  2.71828182845904523)

(define all   0)
(define on    1)
(define off   0)
(define auto -1)
(define de  'de)

; interpolation methods
(define idw  0)
(define mq  -1)

;
; SCORE terminal related constants
;

(define TERM_UNKNOWN  SC_TERM_UNKNOWN)
(define TERM_COOKED   SC_TERM_COOKED)
(define TERM_RAW      SC_TERM_RAW)
(define TERM_LEH_RAW  SC_TERM_LEH_RAW)
(define TERM_CUSTOM   SC_TERM_CUSTOM)

;
; PGS graphics related constants
;

; key modifiers
(define key-shift KEY_SHIFT)
(define key-cntl  KEY_CNTL)
(define key-alt   KEY_ALT)
(define key-lock  KEY_LOCK)

; mouse buttons
(define mouse-left       MOUSE_LEFT)
(define mouse-middle     MOUSE_MIDDLE)
(define mouse-right      MOUSE_RIGHT)
(define mouse-wheel-up   MOUSE_WHEEL_UP)
(define mouse-wheel-down MOUSE_WHEEL_DOWN)
(define mouse-down       4)
(define mouse-up         5)

; colors
(define rotating   ROTATING)
(define black      BLACK)
(define white      WHITE)
(define gray       GRAY)
(define dark-gray  DARK_GRAY)
(define blue       BLUE)
(define green      GREEN)
(define cyan       CYAN)
(define red        RED)
(define magenta    MAGENTA)
(define brown      BROWN)
(define dark-blue  DARK_BLUE)
(define dark-green DARK_GREEN)
(define dark-cyan  DARK_CYAN)
(define dark-red   DARK_RED)
(define yellow     YELLOW)
(define purple     DARK_MAGENTA)
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
(define @world  WORLDC)
(define @norm   NORMC)
(define @pixel  PIXELC)
(define @bound  BOUNDC)
(define @frame  FRAMEC)

; axis types
(define cartesian     CARTESIAN_2D)
(define cartesian-3d  CARTESIAN_3D)
(define polar         POLAR)
(define insel         INSEL)

; markers
(if (defined? pdbview)
    (begin
      (define plus     1)
      (define star     2)
      (define triangle 3)

      (define no-std       PD_NO_STD)
      (define text-std     PD_TEXT_STD)
      (define i386-std     PD_I386_STD)
      (define i586l-std    PD_I586L_STD)
      (define i586o-std    PD_I586O_STD)
      (define ppc32-std    PD_PPC32_STD)
      (define x86-64-std   PD_X86_64_STD)
      (define x86-64a-std  PD_X86_64A_STD)
      (define ppc64-std    PD_PPC64_STD)
      (define m68x-std     PD_M68X_STD)
      (define vax-std      PD_VAX_STD)
      (define cray-std     PD_CRAY_STD)

      (define no-algn           PD_NO_ALGN)
      (define text-algn         PD_TEXT_ALGN)
      (define byte-algn         PD_BYTE_ALGN)
      (define word2-algn        PD_WORD2_ALGN)
      (define word4-algn        PD_WORD4_ALGN)
      (define word8-algn        PD_WORD8_ALGN)
      (define gnu4-i686-algn    PD_GNU4_I686_ALGN)
      (define osx-10-5-algn     PD_OSX_10_5_ALGN)
      (define sparc-algn        PD_SPARC_ALGN)
      (define xlc32-ppc64-algn  PD_XLC32_PPC64_ALGN)
      (define cygwin-i686-algn  PD_CYGWIN_I686_ALGN)
      (define gnu3-ppc64-algn   PD_GNU3_PPC64_ALGN)
      (define gnu4-ppc64-algn   PD_GNU4_PPC64_ALGN)
      (define xlc64-ppc64-algn  PD_XLC64_PPC64_ALGN)
      (define gnu4-x86-64-algn  PD_GNU4_X86_64_ALGN)
      (define pgi-x86-64-algn   PD_PGI_X86_64_ALGN))
    (begin
      (define plus     0)
      (define star     1)
      (define triangle 2)))

; relative position indicators
; used for text alignment, histograms, ...
(define left     DIR_LEFT)
(define right    DIR_RIGHT)
(define center   DIR_CENTER)
(define up       DIR_UP)
(define down     DIR_DOWN)
(define front    DIR_FRONT)
(define back     DIR_BACK)

; tick placement
(define right-of-axis  AXIS_TICK_RIGHT)
(define left-of-axis   AXIS_TICK_LEFT)
(define straddle-axis  AXIS_TICK_STRADDLE)

; line styles
(define solid     LINE_SOLID)
(define dashed    LINE_DASHED)
(define dotted    LINE_DOTTED)
(define dotdashed LINE_DOTDASHED)

; renderings
(define contour         PLOT_CONTOUR)
(define image           PLOT_IMAGE)
(define wire-frame      PLOT_WIRE_MESH)
(define shaded          PLOT_SURFACE)
; NOTE: name vector would conflict with function
(define vect            PLOT_VECTOR)
(define fill-poly       PLOT_FILL_POLY)
(define poly-fill       PLOT_FILL_POLY)
(define mesh            PLOT_MESH)
(define scatter-plot    PLOT_SCATTER)
(define points          PLOT_SCATTER)
(define dvb             PLOT_DV_BND)
(define cartesian-plot  PLOT_CARTESIAN)
(define polar-plot      PLOT_POLAR)
(define insel-plot      PLOT_INSEL)
(define histogram-plot  PLOT_HISTOGRAM)
(define logical-plot    PLOT_LOGICAL)

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
(define vector-scale           VEC_SCALE)
(define vector-angle           VEC_ANGLE)
(define vector-headsize        VEC_HEADSIZE)
(define vector-fixsize         VEC_FIXSIZE)
(define vector-maxsize         VEC_MAXSIZE)
(define vector-linestyle       VEC_LINESTYLE)
(define vector-linethick       VEC_LINETHICK)
(define vector-color           VEC_COLOR)
(define vector-fixhead         VEC_FIXHEAD)

