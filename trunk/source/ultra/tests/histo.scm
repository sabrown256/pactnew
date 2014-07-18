;
; HISTO.SCM - histogram test plotting
;
; include <cpyright.h>
;

(trace fill histogram)

(y-log-scale on)
(ps-type color)

(my (span 0 4) 0.005)
(make-curve* (list 0.5 1.0 1.5 2.0 2.5 3.0 3.5)
	     (list 0.0 1.0 0.0 0.2 0.0 0.1 0.0))


(printf nil "Hollow histogram:\n")
(fill b white)
(histogram b left)
(hc)
(histogram b right)
(hc)
(histogram b center)
(hc)

(histogram b -1)

(printf nil "Filled histogram:\n")
(fill b brown)
(hc)
(histogram b left)
(hc)
(histogram b right)
(hc)
(histogram b center)
(hc)

(era)
(my (span 0 4) 0.005)
(make-curve* (list 0.5 1.0 1.5 2.0 2.5 3.0 3.5)
	     (list 0.5 1.0 0.0 0.2 0.0 0.1 0.5))

(printf nil "Hollow histogram:\n")
(fill b white)
(histogram b left)
(hc)
(histogram b right)
(hc)
(histogram b center)
(hc)

(histogram b -1)

(printf nil "Filled histogram:\n")
(fill b dark-green)
(hc)
(histogram b left)
(hc)
(histogram b right)
(hc)
(histogram b center)
(hc)

(end)
