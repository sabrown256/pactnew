#
# CMakeLists.txt - targets for PACT documentation
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#

#--HTML    = *.html *.gif unix-README
#--HTMLDir = /usr/gapps/pact/distributions/web

#--MAN     = *.1

#--install :
#--	make-html
#--	pact man

#--tar :
#--	tar cvf pact-doc-html.tar *.html *.gif


#--html :
#--	make-html -i
#--	cp ${HTML} ${HTMLDir}
#--	chmod 664 ${HTMLDir}/*.*  ${HTMLDir}/unix-README
#--	make-html

#--man :
#--	cp ${MAN} ${Man1Dir}

