# IO.py v5 glock pact Al Franz Mar 22 2007

import exceptions
import os
import sys

"""
IO.py:  Classes for safe input/output of data to/from files.

To expand an internally-generated file name to its full path, 
do something like:
    full_path = IO.FullPath( infile )
    IF = IO.Input( full_path )
"""



class FileExistenceError( exceptions.Exception ) :
    """
    Class of exceptions for attempting to:
    Write to a file that already exists; or
    Read a file that does not already exist.
    (Beazley 59-60)

    To force a write, do something like:
	import IO
	import os
	import sys
        try :
            OF = IO.Output( outfile )
        except IO.FileExistenceError :
            print "%s.  Trying again." % sys.exc_type
            os.remove( outfile )
            OF = IO.Output( outfile )
        OF.Write( reverse_lines )
    """
    def __init__( self, args = None ) :
	self.args = args
	return



class Input :
    """
    An Input instance is a data-set ready for reading from a file.
    """


    def __init__( self, input_file ) :
        """
	Initialize an Input object by checking the specified file.
        """
        # print "  In Input.__init__()"

        # Check the input file.
        if not ( os.path.exists( input_file )
          and    os.path.isfile( input_file ) ) :
	    raise FileExistenceError, \
	      "%s does not exist or is not a regular file." % input_file

	# I guess it's okay to proceed.
	self.input_file = input_file

	return


    def Read( self ) :
        """
	Get input data from the file.
        """
        # print "  In Input.Read()"

        # Read input file.
        ifd = open( self.input_file, 'r' )
        infile_list = ifd.readlines()
        ifd.close()

	return infile_list



class Output :
    """
    An Output instance is a data-set ready for writing to a file.
    """


    def __init__( self, output_file ) :
        """
	Initialize an Output object by checking the specified file.
        """
        # print "  In Output.__init__()"

        # Check the output file.
        if ( os.path.exists( output_file ) ) :
	    raise FileExistenceError, "%s already exists." % output_file

	# I guess it's okay to proceed.
	self.output_file = output_file

	return


    def Write( self, outfile_list ) :
        """
	Put output data into the file.
        """
        # print "  In Output.Write()"

        # Write output file.
        ofd = open( self.output_file, 'w' )
        ofd.writelines( outfile_list )
        ofd.close()

	return



class InputBinary :
    """
    An InputBinary instance is a data-set ready for reading from a binary file.
    """


    def __init__( self, input_file ) :
        """
	Initialize an InputBinary object by checking the specified binary file.
        """
        # print "  In InputBinary.__init__()"

        # Check the input binary file.
        if not ( os.path.exists( input_file )
          and    os.path.isfile( input_file ) ) :
	    raise FileExistenceError, \
	      "%s does not exist or is not a regular file." % input_file

	# I guess it's okay to proceed.
	self.input_file = input_file

	return


    def Read( self ) :
        """
	Get input data from the binary file.
        """
        # print "  In InputBinary.Read()"

        # Read input binary file.
        ifd = open( self.input_file, "rb" )
        binary_data = ifd.read()
        ifd.close()

	return binary_data



def UnpackFloat( binary_data, num_floats = 0, start = 0 ) :
    """
    Unpack the contents of binary_data as floats.
    If start is unspecified, unpacking begins at the beginning of the buffer.
    If num_floats is unspecified, the entire buffer is unpacked.
    """
    # print "  In UnpackFloat()"

    import struct

    if num_floats == 0 :
	num_floats = len( binary_data )/4

    # Make start an index to the selected float.
    start *= 4

    # To unpack num_floats, we need to calculate where to stop.
    stop = start + struct.calcsize( 'f'*num_floats )

    if sys.byteorder == "little" :
	# Unpack in little-endian byte order.
	float_tuple = struct.unpack( \
	  '<' + 'f'*num_floats, binary_data[ start:stop ] )
	pass			# little-endian
    else :
	# Unpack in big-endian byte order.
	float_tuple = struct.unpack( \
	  '>' + 'f'*num_floats, binary_data[ start:stop ] )
	pass			# big-endian

    return float_tuple



def UnpackUnsignedChar( binary_data, num_chars = 0, start = 0 ) :
    """
    Unpack the contents of binary_data as unsigned chars.
    If start is unspecified, unpacking begins at the beginning of the buffer.
    If num_chars is unspecified, the entire buffer is unpacked.
    """
    # print "  In UnpackUnsignedChar()"

    import struct

    if num_chars == 0 :
	num_chars = len( binary_data )

    # To unpack num_chars, we need to calculate where to stop.
    stop = start + struct.calcsize( 'B'*num_chars )

    if sys.byteorder == "little" :
	# Unpack in little-endian byte order.
	char_tuple = struct.unpack( \
	  '<' + 'B'*num_chars, binary_data[ start:stop ] )
	pass			# little-endian
    else :
	# Unpack in big-endian byte order.
	char_tuple = struct.unpack( \
	  '>' + 'B'*num_chars, binary_data[ start:stop ] )
	pass			# big-endian

    return char_tuple



class OutputBinary :
    """
    An OutputBinary instance is a data-set ready for writing to a binary file.
    """


    def __init__( self, output_file ) :
        """
	Initialize an OutputBinary object by checking the specified binary file.
        """
        # print "  In OutputBinary.__init__()"

        # Check the output binary file.
        if ( os.path.exists( output_file ) ) :
	    raise FileExistenceError, "%s already exists." % output_file

	# I guess it's okay to proceed.
	self.output_file = output_file

	return


    def Write( self, binary_data ) :
        """
	Put output data into the binary file.
        """
        # print "  In OutputBinary.Write()"

        # Write output binary file.
        ofd = open( self.output_file, "wb" )
        ofd.write( binary_data )
        ofd.close()

	return



def PackFloat( float_tuple ) :
    """
    Pack the contents of binary_data as floats.
    """
    # print "  In PackFloat()"

    import struct

    if sys.byteorder == "little" :
	# Pack in little-endian byte order.
	binary_data = ''
	for one_float in float_tuple :
	    binary_data += struct.pack( '<f', one_float )
	pass			# little-endian
    else :
	# Pack in big-endian byte order.
	binary_data = ''
	for one_float in float_tuple :
	    binary_data += struct.pack( '>f', one_float )
	pass			# big-endian

    return binary_data



def PackUnsignedChar( char_tuple ) :
    """
    Pack the contents of binary_data as unsigned chars.
    """
    # print "  In PackUnsignedChar()"

    import struct

    if sys.byteorder == "little" :
	# Pack in little-endian byte order.
	binary_data = ''
	for one_char in char_tuple :
	    binary_data += struct.pack( '<B', one_char )
	pass			# little-endian
    else :
	# Pack in big-endian byte order.
	binary_data = ''
	for one_char in char_tuple :
	    binary_data += struct.pack( '>B', one_char )
	pass			# big-endian

    return binary_data



def FileType( full_path ) :
    """
    Determine unix file type as in file(1).
    The file is tested to ensure that it is a regular file
    and then whether it is ASCII text or binary data.
    """
    # print "  In FileType()"

    import IO
    import os

    filetype = "unknown"

    if not os.path.exists( full_path ) :

	filetype = "nonexistent"

    elif os.path.isdir( full_path ) :

	filetype = "directory"

    elif os.path.isfile( full_path ) :

	# Read several bytes from the file.
	IB = IO.InputBinary( full_path )
	binary_data = IB.Read()
	binary_len = 512
	if len( binary_data ) < binary_len :
	    binary_len = len( binary_data )
	char_tuple = IO.UnpackUnsignedChar( binary_data, binary_len )

	# Test bytes for ASCII.
	filetype = "ASCII text"
	for one_char in char_tuple :
	    if ( one_char < 0x20 or 0x7e < one_char ) \
	      and ( one_char < 0x08 or 0x0d < one_char ) :
		filetype = "data"
		break
	    pass			# one_char

	pass				# isfile

    return filetype


def FullPath( file_name ) :
    """
    Return a string containing the full path to a file or directory.
    file_name can be an existing or nonexistent file.
    """
    # print "  In FullPath()"

    import os

    expanded_file = os.path.expanduser( file_name )
    full_path = os.path.abspath( expanded_file )

    return full_path
