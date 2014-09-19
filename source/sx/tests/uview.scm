;
; VIEW.SCM - ULTRA user viewport control test
;
; Source Version: 4.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
;

(span 0 1)
(replot)
(printf nil "Original view height: %s\n" (view-height))
(pause)

(view-height 0.6)
(screen)
(printf nil "Reduced view height: %s\n" (view-height))
(pause)

(label-space 0.1)
(printf nil "View height with label space 0.1: %s\n" (view-height))
(replot)
(pause)

(label-space 0.0)
(printf nil "Restored label space: %s\n" (view-height))
(replot)
(pause)

(view-height 0.75)
(screen)
(printf nil "Restored view height: %s\n" (view-height))
(pause)

(end)
