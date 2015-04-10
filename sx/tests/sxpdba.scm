;
; SXPDBA.SCM - test of PDB attributes under SX
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

(set-format "short"   "%4d")
(set-format "integer" "%6d")
(set-format "long"    "%8d")
(set-format "float"   "%11.3e")
(set-format "double"  "%11.3e")

; (def-attribute <file> <name> <type>)
; (rem-attribute <file> <name>)
; (set-attribute-value! <file> <name> <attribute> <value>)
; (get-attribute-value <file> <name> <attribute>)
; (list-attributes <file>)

;--------------------------------------------------------------------------

(printf nil "Defining attribute parent for VIF\n")
(def-attribute nil "parent" "char *")

(printf nil "Defining attribute child for VIF\n")
(def-attribute nil "child"  "integer")

(printf nil "Defining attribute sibling for VIF\n")
(def-attribute nil "sibling"  "double")

(printf nil "Defining attribute friend for VIF\n")
(def-attribute nil "friend"  "char")

(printf nil "\n")

;--------------------------------------------------------------------------

(printf nil "Setting parent attribute of foo\n")
(set-attribute-value! nil "foo" "parent" "Dad")
(printf nil "Setting friend attribute of foo\n")
(set-attribute-value! nil "foo" "friend" "B")

(printf nil "\n")

(printf nil "Setting parent attribute of bar\n")
(set-attribute-value! nil "bar" "parent" "Mom")
(printf nil "Setting child attribute of bar\n")
(set-attribute-value! nil "bar" "child" 3)
(printf nil "Setting sibling attribute of bar\n")
(set-attribute-value! nil "bar" "sibling" 6.022e23)
(printf nil "Setting friend attribute of bar\n")
(set-attribute-value! nil "bar" "friend" #\A)

(printf nil "\n")

;--------------------------------------------------------------------------

(printf nil "Attributes for VIF:\n")
(pp (list-attributes nil))
(printf nil "\n")

;--------------------------------------------------------------------------

(define (show-attr var attr)
   (let* ((x (get-attribute-value nil var attr)))
	   (if x
	       (print-pdb  nil x)
	       (printf nil "%s does not have the %s attribute\n"
		       var attr))))

(printf nil "Foo Attributes: \n")
(show-attr "foo" "parent")
(show-attr "foo" "sibling")
(show-attr "foo" "child")
(show-attr "foo" "friend")

(printf nil "\n")

(printf nil "Bar Attributes: \n")
(show-attr "bar" "parent")
(show-attr "bar" "sibling")
(show-attr "bar" "child")
(show-attr "bar" "friend")

(printf nil "\n")

;--------------------------------------------------------------------------

(printf nil "Removing the child attribute\n")
(rem-attribute nil "child")
(printf nil "\n")

;--------------------------------------------------------------------------

(printf nil "Attributes for VIF:\n")
(pp (list-attributes nil))
(printf nil "\n")

;--------------------------------------------------------------------------

(printf nil "Foo Attributes: \n")
(show-attr "foo" "parent")
(show-attr "foo" "sibling")
(show-attr "foo" "child")
(show-attr "foo" "friend")

(printf nil "\n")

(printf nil "Bar Attributes: \n")
(show-attr "bar" "parent")
(show-attr "bar" "sibling")
(show-attr "bar" "child")
(show-attr "bar" "friend")

(printf nil "\n")

;--------------------------------------------------------------------------

(quit)
