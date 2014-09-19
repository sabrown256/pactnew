;
; ULFILL.SCM - Ultra FILL test
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(ps-flag on)
(ps-type color)

(make-curve (0 1 2 1 0) (0 0 0 1 0))
(dy (dx (copy a) .2) .5)

(color (fill a #x7f802080) black)

(color (fill b #x7fffffa0) black)

(annot "Foo"     red   0.45 0.55 0.50 0.55)
(annot "Harumph" green 0.45 0.55 0.45 0.50)
(hc)

(toggle-logical-op)
(replot)
(hc)

(end)
