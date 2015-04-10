#!/usr/bin/env python2.4
# parse_scheme_files.py v4 louth pact Al Franz Mar 22 2007
# parse_scheme_files.py:  Extract doc strings from a scheme file.
# This program finds and saves text within quotation marks and discards
# everything else.
# 
# Usage:  parse_scheme_files.py infile outfile
# 
# E.g.
# cd-pact sx/applications
# ~/pact/manager/parse_scheme_files.py pdbvcmd.scm /tmp/pdbvcmd.doc

import IO
import os
import sys

# print "parse_scheme_files beginning."

usage = """
parse_scheme_files.py - Extract doc strings from a scheme file.
  
Usage: parse_scheme_files.py input_file output_file

    input_file  - an existing scheme file
    output_file - a new ASCII plain-text file
"""



class Parser :
    """
    A parser for scheme files.
    """


    def __init__( self, infile, outfile ) :
	"""
	Initial processing of input and output files.
	"""
        # print "  In Parser.__init__()"

	IF = IO.Input( infile )
	self.inlines = IF.Read()

	try :
	    self.OF = IO.Output( outfile )
	except IO.FileExistenceError, errmsg :
	    print errmsg
	    sys.exit( 2 )
	self.outlines = []

        return


    def __call__( self ) :
	"""
	Look for quotation marks inside a scheme function.
	Remove all characters outside quotation marks.
	Save the entire quote including the quotation marks.
	Insert a carriage-return between groups of text.
	"""
        # print "  In Parser.__call__()"

	c1     = '\0'
	COPY   = False
	DEFINE = False
	ARGS   = False
	FUNC   = False

	for inline in self.inlines :		# for each line in input
	    outline = ""			# begin a line
	    ii = 0
	    while ii != len( inline ) :		# for each char in line
		c0 = c1
		c1 = inline[ ii ]
		ii += 1
		if FUNC :
		    if c1 == '"' and not COPY :
			COPY = True		# copy chars after this
			outline += c1		# copy char
		    elif c1 == '"' and COPY :
			if c0 == '\\' :		# <escape><backslash>
			    outline += c1	# copy char
			else :
			    outline += c1	# copy char
			    outline += '\n'	# end a line
			    COPY = False	# do not copy chars after this
			    FUNC = False
			pass			# c1 == '"' and COPY
		    elif COPY :
			outline += c1		# copy char
		    pass			# FUNC
		if not COPY :
		    if c1 == '(' :
			if inline[ ii:ii+7 ] == "define " :
			    ii += 7
			    DEFINE = True
			elif inline[ ii:ii+13 ] == "define-macro " :
			    ii += 13
			    DEFINE = True
			elif DEFINE :
			    ARGS   = True
			    DEFINE = False
			else :
			    FUNC   = False
			pass			# '('
		    elif c1 == ')' :
			if ARGS :
			    FUNC = True
			    ARGS = False
			else :
			    DEFINE = False
			    FUNC   = False
			pass			# ')'
		    pass			# not COPY
		pass				# ii != len( inline )
	    self.outlines.append( outline )	# save a line
	    pass				# inline

        return


    def __del__( self ) :
	"""
        Post-processing tasks.
        Output the parsed data to a file.
	"""
        # print "  In Parser.__del__()"

	try :
	    self.OF.Write( self.outlines )
	except AttributeError :
	    # Ignore exception if Output instance simply does not exist.
	    pass

        return


class TestObject :
                                                                                
    def __init__( self ) :

	print "TestObject beginning."

	self.program = "parse_scheme_files"
	# infile  = "/tmp/pdbvcmd.scm"
	infile  = "/tmp/test.scm"
	outfile = "/tmp/test.doc"

	P = Parser( infile, outfile )
	P()
	del P

	print "TestObject complete."

        return


if __name__ == '__main__' :

    # Get command-line arguments.
    # 3 is the number of all arguments, including the program name.
    if len( sys.argv ) < 3 :
        print usage
        sys.exit( 1 )

    program = sys.argv[ 0 ]
    infile  = sys.argv[ 1 ]
    outfile = sys.argv[ 2 ]

    P = Parser( infile, outfile )
    P()
    del P

# print "parse_scheme_files complete."
