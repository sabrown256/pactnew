TXT: PDBLib User's Manual
MOD: 08/15/2014

<CENTER>
<P>
<H1><font color="#ff0000">$TXT</font></H1>
<H2>Portable Data Base Library</H2>
<H4>Stewart A. Brown</H4>
</CENTER>

<BLOCKQUOTE>

<P>
<HR>

<a href="#pdb_intro"><H3>Introduction</H3></a>

<dl>
<a href="#pdb_design"><dt><dd>PDBLib Design Philosophy</a>

<a href="#pdb_overlib"><dt><dd>PDBLib API Overview</a>

<a href="#pdb_major"><dt><dd>PDBLib and Index Ordering</a>

<a href="#pdb_offset"><dt><dd>PDBLib and Index Offsets</a>

<a href="#pdb_types"><dt><dd>PDBLib and Data Types</a>

<a href="#pdb_compile"><dt><dd>Compiling and Loading</a>

<a href="#pdb_mm"><dt><dd>Memory Management Considerations</a>

<a href="#pdb_udl"><dt><dd>PDB Universal Data Locators</a>

<a href="#pdb_dsy"><dt><dd>Delayed Symbol Table Management</a>

<a href="#pdb_txt"><dt><dd>Binary and Text Data</a>

<a href="#pdb_syntax"><dt><dd>PDB Syntax</a>

<a href="#pdb_rules"><dt><dd>PDBLib Rules</a>

<a href="#pdb_fmt"><dt><dd>PDB File Format</a>

<a href="#pdb_terms"><dt><dd>Terms</a>

<a href="#pdb_conv"><dt><dd>Data Conversion and Compression</a>
</dl>

<!-- .................................................................... --> 

<a href="#pdb_guidance"><h3>Using PDBLib</h3></a>

<dt><dd>Functions in each group are listed in functional order.
<dt><dd><font color="#ff0000">o </font> <font color="#000000">
indicates functions that are mandatory or nearly so
</font>
<dt><dd><font color="#009900">o </font> <font color="#000000">
indicates functions for optional use
</font>

<p>
<a href="#pdb_opening"><dl><dt><dd>Opening, Creating, Closing, and Controlling PDB Files</a>

<dl>
<dt><dd><font color="#009900">o </font>
<a href="#pd_set_buffer_size">PD_SET_BUFFER_SIZE</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_target">PD_TARGET</a>

<dt><dd><font color="#ff0000">o </font>
<a href="#pd_isfile">PD_ISFILE</a>

<dt><dd><font color="#ff0000">o </font>
<a href="#pd_open">PD_OPEN</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_activate_cksum">PD_ACTIVATE_CKSUM</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_verify">PD_VERIFY</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_autofix_denorm">PD_AUTOFIX_DENORM</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_fix_denorm">PD_FIX_DENORM</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_set_fmt_version">PD_SET_FMT_VERSION</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_set_major_order">PD_SET_MAJOR_ORDER</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_set_max_file_size">PD_SET_MAX_FILE_SIZE</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_set_offset">PD_SET_OFFSET</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_family">PD_FAMILY</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_flush">PD_FLUSH</a>

<dt><dd><font color="#ff0000">o </font>
<a href="#pd_close">PD_CLOSE</a>

<dt><dd><font color="#ff0000">o </font>
<a href="#pd_remove">PD_REMOVE_ENTRY</a>

</dl>

<a href="#pdb_writing"><dt><dd>Writing Data to PDB Files</a>

<dl>
<dt><dd><font color="#ff0000">o </font>
<a href="#pd_write">PD_WRITE</a>

<dt><dd><font color="#ff0000">o </font>
<a href="#pd_write_alt">PD_WRITE_ALT</a>

<dt><dd><font color="#ff0000">o </font>
<a href="#pd_write_as">PD_WRITE_AS</a>

<dt><dd><font color="#ff0000">o </font>
<a href="#pd_write_as_alt">PD_WRITE_AS_ALT</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_append">PD_APPEND</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_append_alt">PD_APPEND_ALT</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_append_as">PD_APPEND_AS</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_append_as_alt">PD_APPEND_AS_ALT</a>

</dl>

<a href="#pdb_defent"><dt><dd>Reserving Space for Future Write</a>

<dl>
<dt><dd><font color="#009900">o </font>
<a href="#pd_defent">PD_DEFENT</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_defent_alt">PD_DEFENT_ALT</a>

</dl>

<a href="#pdb_reading"><dt><dd>Reading Data from PDB Files</a>

<dl>
<dt><dd><font color="#ff0000">o </font>
<a href="#pd_read">PD_READ</a>

<dt><dd><font color="#ff0000">o </font>
<a href="#pd_read_alt">PD_READ_ALT</a>

<dt><dd><font color="#ff0000">o </font>
<a href="#pd_read_as">PD_READ_AS</a>

<dt><dd><font color="#ff0000">o </font>
<a href="#pd_read_as_alt">PD_READ_AS_ALT</a>

</dl>

<a href="#pdb_deftyp"><dt><dd>Defining New Data Types</a>

<dl>
<dt><dd><font color="#009900">o </font>
<a href="#pd_cast">PD_CAST</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_defix">PD_DEFIX</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_defloat">PD_DEFLOAT</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_defncv">PD_DEFNCV</a>

<dt><dd><font color="#ff0000">o </font>
<a href="#pd_defstr">PD_DEFSTR</a>

<dt><dd><font color="#ff0000">o </font>
<a href="#pd_defstr_alt">PD_DEFSTR_ALT</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_typedef">PD_TYPEDEF</a>

</dl>

<a href="#pdb_defattr"><dt><dd>Defining Attributes</a>

<dl>
<dt><dd><font color="#009900">o </font>
<a href="#pd_def_attribute">PD_DEF_ATTRIBUTE</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_get_attribute">PD_GET_ATTRIBUTE</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_rem_attribute">PD_REM_ATTRIBUTE</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_set_attribute">PD_SET_ATTRIBUTE</a>

</dl>

<a href="#pdb_queries"><dt><dd>Queries</a>

<dl>
<dt><dd><font color="#009900">o </font>
<a href="#pd_get_buffer_size">PD_GET_BUFFER_SIZE</a>

<dt><dd><font color="#ff0000">o </font>
<a href="#pd_get_error">PD_GET_ERROR</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_get_file_length">PD_GET_FILE_LENGTH</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_get_file_name">PD_GET_FILE_NAME</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_get_fmt_version">PD_GET_FMT_VERSION</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_get_max_file_size">PD_GET_MAX_FILE_SIZE</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_get_major_order">PD_GET_MAJOR_ORDER</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_get_mode">PD_GET_MODE</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_get_offset">PD_GET_OFFSET</a>

<dt><dd><font color="#ff0000">o </font>
<a href="#PD_query_entry">PD_QUERY_ENTRY</a>

<dt><dd><font color="#ff0000">o </font>
<a href="#PD_get_entry_info">PD_GET_ENTRY_INFO</a>

<dt><dd><font color="#ff0000">o </font>
<a href="#PD_free_entry_info">PD_FREE_ENTRY_INFO</a>

<dt><dd><font color="#ff0000">o </font>
<a href="#pd_inquire_type">PD_INQUIRE_TYPE</a>

<dt><dd><font color="#ff0000">o </font>
<a href="#pfimbr">PFIMBR</a>

</dl>

<a href="#pdb_ptrs"><dt><dd>Using Pointers</a>

<dl>
<dt><dd><font color="#009900">o </font>
<a href="#pd_get_track_pointers">PD_GET_TRACK_POINTERS</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_set_track_pointers">PD_SET_TRACK_POINTERS</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_reset_ptr_list">PD_RESET_PTR_LIST</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_free">PD_FREE</a>

</dl>

<a href="#pdb_dirs"><dt><dd>Using Directories</a>

<dl>
<dt><dd><font color="#009900">o </font>
<a href="#pd_cd">PD_CD</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_ln">PD_LN</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_ls">PD_LS</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_ls_alt">PD_LS_ALT</a>

<dt><dd><font color="#ff0000">o </font>
<a href="#pfvart">PFVART</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_mkdir">PD_MKDIR</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_pwd">PD_PWD</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_isdir">PD_ISDIR</a>

</dl>

<a href="#pdb_par"><dt><dd>Parallel Programming with PDB</a>

<dl>
<dt><dd><font color="#009900">o </font>
<a href="#pd_init_threads">PD_INIT_THREADS</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_init_mpi">PD_INIT_MPI</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_term_mpi">PD_TERM_MPI</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_mp_open">PD_MP_OPEN</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_mp_create">PD_MP_CREATE</a>

</dl>

<a href="#pdb_spec"><dt><dd>Special Purpose I/O</a>

<dl>
<dt><dd><font color="#009900">o </font>
<a href="#pd_put_image">PD_PUT_IMAGE</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_put_mapping">PD_PUT_MAPPING</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_put_set">PD_PUT_SET</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_make_image">PD_MAKE_IMAGE</a>

<dt><dd><font color="#009900">o </font>
<a href="#pd_rel_image">PD_REL_IMAGE</a>

<dt><dd><font color="#009900">o </font>
<a href="#pfwran">PFWRAN</a>

</dl>

</dl>

<!-- .................................................................... --> 
 
<a href="#pdb_datas"><H3>Data Structures in PDBLib</H3></a>

<dl>
<a href="#pdb_align_s"><dt><dd>DATA_ALIGNMENT</a>

<a href="#pdb_std_s"><dt><dd>DATA_STANDARD</a>

<a href="#pdb_defstr_s"><dt><dd>DEFSTR</a>

<a href="#pdb_dimdes_s"><dt><dd>DIMDES</a>

<a href="#pdb_memdes_s"><dt><dd>MEMDES</a>

<a href="#pdb_pdbfile_s"><dt><dd>PDBFILE</a>

<a href="#pdb_syment_s"><dt><dd>SYMENT</a>

</dl>

<!-- -------------------------------------------------------------------- --> 
 
<a href="#pdb_examp"><H3>PDBLib by Example</H3></a>

<dl>
<a href="#pdb_ex_wrk"><dt><dd>Working with PDB files</a>

<a href="#pdb_ex_write"><dt><dd>Writing Data to PDB files</a>

<a href="#pdb_ex_read"><dt><dd>Reading Data from PDB files</a>

<a href="#pdb_ex_append"><dt><dd>Appending Data to PDB file variables</a>

<a href="#pdb_ex_inq"><dt><dd>Inquiries in PDBLib</a>

<a href="#pdb_ex_brws"><dt><dd>Browsing PDB files</a>

<a href="#pdb_ex_map"><dt><dd>Writing <tt>PM_mappings</tt> with <tt>PFWMAP</tt></a>

<a href="#pdb_ex_wsr"><dt><dd>Writing <tt>PM_mappings</tt> with <tt>PFWSET</tt> and <tt>PFWRAN</tt></a>

<a href="#pdb_ex_par_smp"><dt><dd>SMP Parallel Example</a>

<a href="#pdb_ex_par_mpi"><dt><dd>MPI Parallel Example</a>

</dl>

<!-- -------------------------------------------------------------------- -->

<a href="#pdb_docs"><H3>Related Documentation</H3></a>

<!-- this tag is required to terminate last href tag -->


<!-- -------------------------------------------------------------------- --> 
 
<hr>

<a name="pdb_intro"><h2>Introduction</h2></a>

PDBLib is a small library of file management routines useful for storing
and retrieving binary data in a portable format. It is intended to supply
a flexible means of managing binary databases without the user having to
be concerned with the machines on which they are written or read. 
<font color="#FF0000">Please note that PACT headers MUST be included before
all other headers.</font> See the SCORE introduction for more information.
<p>

The specific goals for this library are:
<p>

<ul>
<li>
The data files are portable. Using only simple binary file
transfers, these files may be passed to any machine supporting
an implementation of this library and the data in them immediately
read.
<li>
The library routines are portable. The library is written in C which
is the most standardized language which also enjoys a broad range
of implementations. In fact, a C compiler is probably the easiest
piece of software to find for any given machine.
<li>
The library will support call-by-reference or call-by-value
interfaces. In this way the library can be used from FORTRAN
(a call-by-reference language) or C (a call-by-value language)
programs.
<li>
`Arbitrary' data structures may be defined to the PDB system so that
there is maximum flexibility and ease of use for application
programming. C structs or FORTRAN common blocks may be written
or read as a whole. Record structured variables are thus supported.
<li>
Data files may be generated for a particular machine type. By
default when files are created they are written with the format
of the machine that does so. However, they can be written in the
format of a designated machine. Thus a more powerful machine can
prepare files for a less powerful one.
<li>
The library will be small so that machines with as little memory as
about 512 Kbytes will be able to profitably use the PDB system.

The binary data format of the files is not machine independent, but it is
portable. For the sake of efficiency, the data in any file is expressed 
in terms of some data format, e.g. IEEE format with 32 bit integers and
floats. When PDB files are passed around on machines with the same data
format, the data reads and writes involve no conversions. Only when one
of these files is read on a machine with a different data format are
conversions performed on the data it contains.
<p>
This document is divided into several sections. The next section gives
an overview of the use of PDBLib and describes some programming practices
which are necessary to the successful use of the library. It is followed
by a section summarizing the C interface routines in PDBLib and a section
giving a more formal and complete description of the C interface with
information about normal and error return values, functionality, and
rationale. Next are similar sections for the FORTRAN interface routines.
Subsequent sections discuss PDBLib design philosophy, data structures,
examples, and related documentation.
<p>

</ul>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_design"><h2>PDBLib Design Philosophy</h2></a>


Perhaps the most fundamental element in the design of PDBLib is the
concept of modularity through abstraction barriers. In essence, the
functionality is broken down into modular units and abstraction
barriers are used to preserve the integrity of the units.
<p>
An abstraction barrier is simply a technique or device which allows
a section of code to be written assuming other functions or
routines are defined and that their internal workings are
irrelevant to their use. In this way a routine or module can be
changed without any other part of the code which uses it being
affected (so long as the definition of its function does not
change). Abstraction barriers are most effectively created by a
careful choice of the basic functional units and by the interfaces
between them.
<p>
For example, if all variables in the code were global there would
be little or no chance of having any abstraction barriers at all.
Similarly, monolithic functions which are defined to `solve the
worlds problems' do not lend themselves to the more easy maintenance
that abstraction barriers afford a program. For a good discussion of
the principles and applications of these ideas see Abelson and
Sussman's marvelous book, The Structure and Interpretation of
Computer Programs.
<p>
The main functional units in the PDBLib system are: the hash package
which controls the maintenance of the data in both the symbol table
and structure chart; the conversion package which handles all data
format conversions; the data reading routine which is defined to
bring data in from the disk, perform any necessary conversion, and
store it in the specified location(s) of memory; and the data
writing routine which does the reverse of the reading routine.
<p>
These units are put together in such a way that they are used over
and over again in the process of doing any of the high level
functions. In this way the code size was kept relatively small.
Again this is one of the rewards of modularity.
<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_overlib"><h2>Overview of PDBLib API</h2></a>


The functional PDBLib interface was designed as a minimal extension of
the standard C library binary I/O interface. The relevant C library
functions are fopen, fclose, fwrite, and fread. The extensions are
driven mainly by some missing features in C. Because C doesn't
handle data types as types, PDBLib must be told the type of a variable
when it is written to a file. Because C variables don't know about
their dimensions, PDBLib must be given the dimensions when a variable
is written out. On the other hand, because PDBLib knows both type and
dimension information about file variables, PDBLib requires less
information from the programmer when reads are done.
<p>
Since C doesn't have the needed type handling facilities, data structures
must be described to PDBLib. Therefore, in addition to the basic I/O
functions mentioned above, there is a function in PDBLib to be used
when a C data structure or a FORTRAN common block must be described
in a file.
<p>
Next, to give the user the most flexibility and efficiency regarding
data conversions, PDBLib has a function which lets the user specify
the binary data format of the file. In this way, a file can be targeted
for a particular machine type. A common use for this might be when a
powerful machine produces some data which is going to be reprocessed
on a less powerful machine. The more powerful machine which writes
the file might target the file for the less powerful one so that
the conversions are done by the faster machine and not the slower one.
<p>
In some applications with structures, some members of a
structure may be pointers, and the actual type to which a pointer
points may be changed by a cast depending on some other member's
value. PDBLib supports the notion of a "cast" in that it can be
told to ignore the type of a structure member and get the actual
type from another member when it writes out the structure.
<p>
There are also routines to manage data attributes. An attribute table
will be created for a PDB file the first time PD_def_attribute is
called. The attribute table is kept in the PDBfile structure. Variables
can be added to PDB files solely for their attribute values. Attributes
can be given to entire variables or any part of them. This works because
the attribute table is a separate hash table, like the symbol table,
which associates names with attribute values. It also is used to manage
the attributes themselves. It is not exactly clear why one should use
these attributes. Most of the utility of attributes is already inherent
in the structures or records which can by built with PDBLib's
mechanism for defining derived types. Since the structure handling
mechanisms are much more efficient, the reason for using attributes
instead of structures should be very clear in the mind of the
application programmer.
<p>
Because all of the information about files and their contents is contained
in PDBLib structures and hash tables, the C API (Application Program
Interface) has no inquiry functions. Users can extract information
directly from the PDBfile structure and they can use the hash table
lookup function, <tt>SC_def_lookup</tt>, to get access to the <tt>syment</tt> and <tt>defstr</tt>
structures which describe variables and types, respectively.
<p>
The FORTRAN API to PDBLib has most of the same functions as the C API.
There are some differences due to the differences in the languages.
<p>
SX, a version of the SCHEME programming language with extensions for
PACT, has a PDBLib API.  In fact, PDBView is largely an SX program
which means that it is immanently suited to user customization.  The
SX interface to PDBLib is largely documented here.  More documentation
may be found in other PACT manuals.
<p>
A Python module is also available to access PDBLib's functionality.
<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_get_fmt_version"><h2>PD_GET_FMT_VERSION</h2></a>


<p>
<pre>
<i>C Binding: </i> int PD_get_fmt_version(int v)
<i>Fortran Binding: </i>integer PFGFMV(integer v)
<i>SX Binding: </i> (format-version)
<i>Python Binding: </i> XXX - missing
</pre>
<p>

Get the format version to be used when creating new PDB files
to <em>v</em>.  See the section <a href="#pdb_fmt">PDB File Format</a>
for a discussion of the PDB file formats.
<p>
Input to this function is:
<em>v</em>, an integer in which to return the format version.
<p>
Return the value of the format version.
<p>

<hr>
<b>C Example</b>
<pre>
       int v;
             .
             .
             .

       v = PD_get_fmt_version();
             .
             .
             .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfgfmv
       integer v
             .
             .
             .

       v = pfgfmv()
       if (v .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<b>SX Example</b>
<pre>
             .
             .
             .

    (format-version)
             .
             .
             .
</pre>
<hr>

<b>Python Example</b>
<pre>
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_major"><h2>PDBLib and Index Ordering</h2></a>

Languages such as C and FORTRAN differ about the major ordering of arrays.
For example, C is row major and FORTRAN is column major. To accommodate
the use of PDB files among applications in different languages, each PDB
file contains a flag that says which ordering to use.
<p>
The C header file #define's two constants:
<pre>
   #define ROW_MAJOR_ORDER     101
   #define COLUMN_MAJOR_ORDER  102
</pre>
These values define how PDB treats index expressions when reading and
writing data. PDB does NOT transpose arrays when reading or writing,
it transposes the index expressions.
<p>
By default PDB files created via the C interface are set with
ROW_MAJOR_ORDER and files created via the FORTRAN interface are set with
COLUMN_MAJOR_ORDER.  Applications can query or change the value at any
time after the file is opened/created.  See the functions
<a href="#pd_set_major_order">PD_SET_MAJOR_ORDER</a>
and
<a href="#pd_get_major_order">PD_GET_MAJOR_ORDER</a>.
<p>
Using PDBView you can check the mode of a given file using the
<tt>file</tt> command.
<p>
We recommend that when you know that you will be using files between
applications in different languages that you use column major ordering
consistently.  There is no deep reason for this.  It simply helps you to
be consistent and to have an expectation of how to think about your files.
<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_offset"><h2>PDBLib and Index Offsets</h2></a>

Languages such as C and FORTRAN differ about the default value for
the first index value in an array.  For example, C uses 0 based indexing
and FORTRAN uses 1 based indexing.  To accommodate the use of PDB files
among applications in different languages, each PDB file contains a flag
that specifies the default offset.
<p>
The offset determines how PDB treats index expressions when reading and
writing data.  It doesn't affect the number of items specified by a dimension.
It only affects the range of index values.
<p>
By default PDB files created via the C interface are set with
a default offset of 0 and files created via the FORTRAN interface have
a default offset of 1.  Applications can query or change the value at any
time after the file is opened/created.  The value can be set to any integer
value so it is more general than would be strictly necessary if there were only
C and FORTRAN.  See the functions
<a href="#pd_set_offset"><tt>PD_SET_OFFSET</tt></a>
and
<a href="#pd_get_offset"><tt>PD_GET_OFFSET</tt></a>.
<p>
Using PDBView you can check the default offset of a given file using the
<tt>file</tt> command.
<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_types"><h2>PDBLib and Data Types</h2></a>

PDBLib supports the full set of C99 primitive data types:
<pre>
   C Type                      PDB Name

   bool                        bool
   char                        char
   unsigned char               u_char
   wchar_t                     wchar
   unsigned wchar_t            u_wchar
   short                       short
   unsigned short              u_short
   int                         int
   unsigned int                u_int
   long                        long
   unsigned long               u_long
   long long                   long_long
   unsigned long long          u_long_long
   int8_t                      int8_t
   unsigned int8_t             u_int8_t
   int16_t                     int16_t
   unsigned int16_t            u_int16_t
   int32_t                     int32_t
   unsigned int32_t            u_int32_t
   int64_t                     int64_t
   unsigned int64_t            u_int64_t
   float                       float
   double                      double
   long double                 long_double
   float _Complex              float_complex
   double _Complex             double_complex
   long double _Complex        long_double_complex
   <any data pointer>          *
   <function pointer>          function
</pre>

The following additional fixed width types are defined in
analogy with the C99 standard fixed width integer types:
<pre>
   C Type                      PDB Alias

   float32_t                   float32_t
   float64_t                   float64_t
   float128_t                  float128_t
   complex32_t                 complex32_t
   complex64_t                 complex64_t
   complex128_t                complex128_t
</pre>

The following types are defined as aliases to C99 types:
<pre>
   C Type                      PDB Alias

   int                         integer
   double                      REAL
</pre>

All of these data types are automatically defined by PDBLib and
applications may simply refer to them without any special setup.
<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_compile"><h2>Compiling and Loading</h2></a>


To compile your C programs you must use
<p>

<PRE> #include &lt;pdb.h&gt;
</PRE>

in the source files which deal with the library.
<p>
To link your application you must use the following libraries in the
order specified.

<dl>
<dt>-lpdb -lpml -lscore [-lm ...]
</dl>

Although this is expressed as if for a UNIX linker, the order would be
the same for any system with a single pass linker. The items in [] are
optional or system dependent.
<p>
Each system has different naming conventions for its libraries and the
reader is assumed to understand the appropriate naming conventions as
well as knowing how to tell the linker to find the installed PACT
libraries on each system that they use.
<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_mm"><h2>Memory Management Considerations</h2></a>


PDBLib is capable of correctly handling data that is indirectly referenced
through arbitrary levels of pointers. In order to do this it is necessary
to put an extra layer of memory management over the standard C library
routines. The basic requirement is that given a pointer, one would like
to know how many bytes of data it points to. The functions, <tt>CMAKE</tt>,
<tt>CREMAKE</tt>, <tt>CSTRSAVE</tt>, <tt>CFREE</tt>, and <tt>SC_arrlen</tt>,
built on top of the
standard C library functions, malloc and free, provide this capability.
For C programmers, macros are provided which offer a nice and intuitive
way of using these functions (they also provide an abstraction barrier
against the details of any memory management scheme). These functions
and macros are documented in the SCORE User's Manual.
<p>
A brief discussion of the procedure for writing and reading indirectly
referenced data follows. Although the discussion will use integers as
an example, the ideas apply to all data types, primitive or derived.
<p>
Consider the following:
<p>

<dd><tt>int a[10], *b;</tt><P>
<dd><tt>b = CMAKE_N(int, 10);</tt><P>

Both a and b are pointers to 10 integers (macro <tt>CMAKE_N</tt> is used to
allocate the necessary space). The difference as far as an application
is concerned is that the space that <tt>a</tt> points to was set aside by the
compiler at compile time (for all practical purposes) while the space
the <tt>b</tt> points to is created at run time. There is no possibility, given
the definition of the C language, of asking the pointer <tt>a</tt> how many
bytes it points to. On the other hand, since <tt>b</tt> is dynamically allocated,
an extra layer of memory management could be provided so that a
function, <tt>SC_arrlen</tt>, could be defined to return the number of bytes
that a dynamically allocated space has. In particular,
<p>

<dd><tt>SC_arrlen(a)</tt> =&gt; -1 (indicating an error)<P>
<dd><tt>SC_arrlen(b)/sizeof(int)</tt> =&gt; 10<P>
<dd><tt>SC_arrlen(b+5)</tt> =&gt; -1 (indicating an error)<P>

These functions and macros can be found in SCORE. The general utility
of something like <tt>SC_arrlen</tt> made it desirable to put it in the lowest
possible level library. This could be used, for example, to implement
some dynamic array bound checking.
<p>
PDBLib uses this idea to be able to trace down arbitrary layers of
indirection and obtain at each level the exact number of bytes to
write out to a file. Of course, it also writes this information out
so that the correct amount of space can be allocated by read operations
as well as re-creating the correct connectivity of data structures.
<p>
Great care must be taken that pointers to fixed arrays not be embedded
in a chain of indirects unless their dimension specifications are
included either in the I/O request or the definition of a structure.
This point cannot be over-emphasized! The extra memory management layer
may fail to detect a statically allocated array and return an erroneous
byte count. This in turn will cause very obscure incorrect behavior (in
the worst of circumstances) or a direct crash (the best outcome possible).
<p>
Also, note that subsets of dynamically allocated arrays cannot know how
many bytes they contain and hence care should be taken in their use.
 <p>
The example on the following page shows the different ways that statically
allocated arrays, dynamically allocated arrays, statically allocated
arrays of pointers, and dynamically allocated arrays of pointers are
handled by PDBLib. Note: The function <tt>CSTRSAVE</tt> invokes the
<tt>CMAKE_N</tt> macro.
<p>

<pre>
 /* define variables in pairs - one to write and one to read into */
    PDBfile *strm;
    char *a, *b;
    char c[10], d[10];
    char *e[3], *f[3];
    char **s, **t;

    s = CMAKE_N(char *, 2);

/* fill statically and dynamically allocated arrays */
    strncpy(c, "bar", 10);
    a = CSTRSAVE("foo");

/* fill statically and dynamically allocated arrays of pointers */
    e[0] = CSTRSAVE("Foo");
    e[1] = NULL;
    e[2] = CSTRSAVE("Bar");
    s[0] = CSTRSAVE("Hello");
    s[1] = CSTRSAVE("world");

/* write these variables out
 * note the dimension specifications and the type
 */
    PD_write(strm, "c(10)", "char", c);
    PD_write(strm, "a", "char *", &amp;a);
    PD_write(strm, "e(3)", "char *", e);
    PD_write(strm, "s", "char **", &amp;s);

/* read the file variables into fresh memory spaces
 * note that the pointers to the pointers are passed in since the
 * space is to be allocated and the value of the pointer here must
 * be set to point to the new space
 */
    PD_read(strm, "c", d);
    PD_read(strm, "a", &amp;b);
    PD_read(strm, "e", f);
    PD_read(strm, "s", &amp;t);
</pre>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_udl"><h2>PDB Universal Data Locators</h2></a>

As of PDB version 22, PDBLib can access data from a variety of sources.
For example, PDBLib can access PDB files in TAR files and AR archives.
In conjunction with the translation spokes, PDBLib can access various
other data formats such as HDF and mySQL.
PDB version 27 adds the more general file address range capability.
<p>

In order to make sense of all the possible data sources, PDBLib is using
a Uniform Data Locator modelled after the URL used in the World Wide Web.
The UDL syntax is as follows:<p>

<pre>
   [&lt;proto&gt;://&lt;host&gt;/]&lt;path&gt;[@&lt;addr&gt;][,&lt;attr&gt;]*

   &lt;proto&gt;     := http | https | ftp |  (via curl)
                  mysql                 (via mysql)
                  webdav
   &lt;path&gt;      := &lt;file&gt; | &lt;container&gt; '~' &lt;file&gt; |
                        &lt;container&gt; '~' &lt;start&gt : &lt;end&gt;
   &lt;file&gt;      := full or relative path to file
                  relative means relative to $HOME or
                  whatever the base path is as determined by
                  the server or container in question
   &lt;container&gt; := tar | ar
   &lt;addr&gt;      := offset into the file - allows accessing
                  remote files without having to download
                  them in toto
   &lt;attr&gt;      := key/value pair specifications
                  e.g. tgt=ieee96 or tgt=txt
</pre>

Calls to PD_open, PD_create, and so on now take a UDL in the place
of the file name.  Based on the UDL, the appropriate data source or
destination is used.<p>

Some examples are:<p>

<pre>
   "/home/you/a.pdb"        full path to a.pdb
   "a.pdb"                  file a.pdb in current directory
   "b.tar~a.pdb"            file a.pdb in TAR file b.tar in current directory
   "a.pdb,tgt=txt"          write a.pdb in TXT mode
   "mysql://server/data"    MySQL database 'data' on host 'server'
   "a~1059:43946"           file a from address 1059 to 43946
</pre>

Note that access to some data sources depends on whether various libraries
are available on the system on which PACT is installed.  The following
data sources/destinations have such dependencies:

<pre>
   HDF5      hdf5.h, libhdf5.a
   mySQL     mysql.h, libmysqlclient.a
   SQLite    sqlite3.h, libsqlite3.a
</pre>

The C macro can be PD_get_file_type can be used to query a PDBfile structure
returned by PD_open regarding the type of file actually opened.  The
standard kinds of files have the following types:
<p>

<center>
<table>
<tr><td align="center" width="100"><b>Type</b></td><td align="center" width="100"><b>Type Name</b></td></tr>
<tr><td align="center" width="100">PDB</td><td align="center" width="100">PDBfile</td></tr>
<tr><td align="center" width="100">HDF5</td><td align="center" width="100">HDF5file</td></tr>
<tr><td align="center" width="100">SQL</td><td align="center" width="100">SQLdatabase</td></tr>
<tr><td align="center" width="100">XML</td><td align="center" width="100">XMLfile</td></tr>
</table>
</center>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_dsy"><h2>Delayed Symbol Table Management</h2></a>

As of PDB version 28, PDBLib has additional options for reading and
managing the symbol table of a PDB file.  By default when you open
a PDB file, PDBLib reads the entire symbol table and constructs a
hash array of the entries for optimimum lookup speed.  For very large
databases where it may be prohibitive to hold the entire symbol table
in memory, there are now options for keeping only part of the
symbol table in memory at any given time.
<p>
Making symbol table entries that PDBLib can lookup is delayed until
you call PD_cd to enter a directory.  There are special built in
modes:
<p>
<pre>
    np    for format version 2 files only
          do not read in, "rnp", or write out, "wnp" the pointer
          entries from the symbol table
    sa    accept all entries for the current directory and its
          ancestors (i.e. towards the root directory)
    sc    accept all entries for the current directory only
    sd    accept all entries for the current directory and its
          descendants (i.e. away from the root directory)
    p     accept pointer entries for the current directory only
</pre>
When you cd to a directory, the symbol
table is read in and only the entries specified are added to the
symbol table.  When you call
<a href="#pd_cd"><tt>PD_cd</tt></a>
to enter another directory the current entries are purged
from the hash array before adding the new entries.  In this way
only the specified entries for the current directory tree are available.
<p>
This give users a way to manage very large database files with
a smaller footprint in main memory.  It should be clear that
this comes at the expense of re-reading the entire symbol table on the
disk file when you change directories.  PDBLib will not keep
entries that are not in the specified directories.  The memory
savings can be very large.  As with all database operations, it
depends on your database.
<p>
Doing delays at the directory level is a compromise.  If the
delay were at the variable level, re-reading the symbol table from
the file for every variable would be ruinously expensive. Also
navigating or browsing the file would be practically impossible.
At the directory level, you have a chance to access several
entries for each symbol table read.
<p>
In order for this to be a useful mechanism there is a key exception.
All of the entries for directories are added to the hash array.
This permits you do use
<a href="#pd_ls"><tt>PD_ls</tt></a>
in combination with
<a href="#pd_cd"><tt>PD_cd</tt></a>
 to navigate around the file.
<p>
To give the user additional flexibility, PDBLib provides a function
to register custom filters on symbol table entries.  The function
PD_set_delay_method can be called before a PD_open to specify the
entry filter that will be used for the next file.  It takes a
function with prototype:
<p>
int match(PDBFile *file, char *name, char *type, char *acc, char *rej)
<p>
The function is called with the file, name, and type of the entry and
a string specifying patterns that may be accepted and a string specifying
patterns that may be rejected.  The user function can do whatever it
needs to do with this information.  If it returns TRUE the entry is
accepted and if FALSE the entry is rejected.
<p>
The old value of this default function is returned by PD_set_delay_method.
With this you can manage the filters in a general way.
<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_txt"><h2>Binary and Text Data</h2></a>

For all of its history up to version 22, PDBLib dealt only with binary
files.  As of version 22, PDBLib can write and read a text version of
data.  The capability is new and there are important restrictions.<p>

First of all PDBLib has considered ASCII characters as binary data
which might need to be converted to other character formats such as
the wchar or various internationalizations.  None of those conversions
have been implemented, merely provided for.<p>

Now a text or TXT mode, means that the entire data file is represented in
printable characters.  You can look at the data with a text editor.  With
restrictions you can edit a text mode file with a text editor and read
it back in.<p>

In text mode each primitive data type has fixed number of bytes to
represent it.  For example, a short requires 7 bytes - a leading space,
a sign, and 5 decimal digits to represent values from -32768 to 32767.<p>

The editing restrictions are that the you cannot violate the field width,
i.e. the number of characters to represent each primitive type.  You
may change the values in the field, but not the field width.<p>

In practice this capability is rather unrefined at this point.  Future
improvements with be made to increase flexibility and user friendliness.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_syntax"><h2>PDB Syntax</h2></a>


This section contains discussion of some PDB concepts that are used
elsewhere in this manual.
<p>
In the following discussion and elsewhere in the manual a BNF style
notation is used. In such places the following constructs are used:
<p>

<dl>
<dt><tt>|</tt>	or
<dt><tt>[]</tt>	items between the brackets are optional
<dt><tt>[]*</tt>	zero or more instances of the items between the brackets
<dt><tt>[]+</tt>	one or more instances of the items between the brackets
<dt><tt>"x"</tt>	literal x

Since a data entry can be of a structured type, in general, a data entry
is a tree. A part of the data tree is specified by a path to that part.
A path specification is of the form:
<p>

<tt>
<dt>path		:= node_spec | path.node_spec
<dt>node_spec		:= name["[" index_expr "]"]* | name["(" index_expr ")"]*
<dt>name		:= entry-name | member-name
</tt>
<p>
The last node specified is referred to as the terminal node.
<p>
</dl>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_rules"><h2>PDBLib Rules</h2></a>


There are a few crucial rules to be followed with PDBLib. If they are
not obeyed, PDBLib may crash or the results desired will not occur.
<p>

<h4>Rule #1</h4>
The following reserved characters or sequences are not allowed in variable names
or <tt>defstr</tt> member names:<BR>
<PRE>'<tt>.</tt>',  '<tt>(</tt>',  '<tt>)</tt>', '<tt>[</tt>',  '<tt>]</tt>'  and  '<tt>-></tt>'.</PRE>
They are reserved by the grammar of the data description language which
follows some C and some FORTRAN conventions.  It is recommended that you
try to be somewhat conservative
in choosing identifier names.  Some punctuation characters are perfectly legal
to use but may cause confusion when browsing files or reading your own code.
<p>

<h4>Rule #2</h4>
In each read and write operation, the type of the argument corresponding
to the variable to be written or read must be a pointer to an object of
the type specified for the entry. For example,
<p>
<pre>
     int *a, *b;
            .
            .
     PD_write(strm, "a", "integer *", &amp;a);
     PD_read(strm, "a", &amp;b);
            .
            .
</pre>

<h4>Rule #3</h4>
When using pointers and dynamically allocated memory with PDBLib, use
<tt>CSTRSAVE</tt>, <tt>CMAKE</tt>,
<tt>CMAKE_N</tt>, or <tt>CREMAKE</tt> to allocate 
memory. These functions and macros are documented in the SCORE User's Manual.
<p>

<h4>Rule #4</h4>
When reading or writing part of a variable, especially a structured variable, the terminal 
node must be of primitive type or a structure containing no indirections and whose 
descendant members contain no indirections. Furthermore, the path to the desired part 
must contain one array reference for each level of indirection traversed. For example,
<br>
<pre>
     char **s, c;
 
 /* fill s so that s[0] = "Hello" and s[1] = "World" */
            .
            .
     PD_write(strm, "s", "char **", &amp;s);
 
 /* read the `o' in "Hello" */
     PD_read(strm, "s[0][4]", &amp;c);
 
 /* read the `o' in "World" */
     PD_read(strm, "s[1][1]", &amp;c);
            .
            .
</pre>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_fmt"><h2>PDB File Format</h2></a>

Up until PDBLib version 19 the file format has gone through only one
major revision.  These two formats were deeply connected to the internal
workings of PDBLib itself.  As such those file formats were not documented.

Over the years certain problems and experience have led to the design
of a next generation PDB file format.  This is the third file format
version for PDB.  Its design addresses three main goals:
<p>
  1) Total separation of data and metadata.
<p>
     Earlier versions of the PDB file format had metadata at
     the beginning and end of the file.  If pointers were
     written, there was metadata interspersed amidst the
     data itself.
<p>
     With the new format, all PDB metadata is concatenated at
     the end of the file, including pointer metadata.  This means
     that PDB metadata can be attatched to the end of any file
     rendering it a valid PDB file.  A simple extension of this
     will permit the metadata to exist in a separate file.  This
     design strategy opens the door to greater flexibility in using
     PDBLib to access arbitrary data files.  It also promises
     benefits in dealing with data files in parallel applications.
<p>
  2) More efficient operation.
<p>
     The organization of the metadata improves on the scheme
     used in earlier versions.  Earlier versions introduced a
     metadata section seperate from the structure chart and
     symbol table called the extras.  The extras provide a means
     to accrete new features into PDBLib without invalidating
     existing files.
<p>
     Several extras dealt with extentions to the description of
     the data types.  This led to a requirement that the extras
     had to be read in order to interpret the data types correctly.
     The new format generalizes this idea to make the description
     of data types more extensible and remove any data type
     description elements from the extras.  In effect, this
     gives data types their own independent extras.  This in
     turn means that the interpretation of the type metadata
     is self contained and in a way that can be extended in the
     future.
<p>
     The structure chart has been broken down into two distinct
     sections for: primitive types and compound types.  The
     syntax of these sections has been refined for their respective
     needs.
<p>
     The new design for handling pointers improves I/O performance
     by reducing the amount of seeking around on disk to find the
     pointer metadata.  All of the pointer metadata is contained
     in the /&ptrs directory of the symbol table.  As such the
     pointers are able to be processed using more of the general
     PDBLib functionality and require less specialized coding.
<p>
  3) Human readable and editable.
<p>
     The new design of the metadata section features 100% ASCII
     representation of the information.  A syntax for the metadata
     was chosen to make the metadata more human readable.  Also,
     where possible the elements of C language syntax were used
     in order to make the metadata more intuitive to users
     familiar with that language.
<p>
     Occassionally, file system errors corrupt data files.  By
     redesigning the metadata section to be both ASCII and
     as human readable as possible, the goal is to make it as
     easy as possible for a user, program or shell script
     to repair files which have damaged metadata.
<p>
     Also to fully realize the potential of having separated
     the metadata into a single, complete section, it makes sense
     to have it human readable.  A person has a higher
     probability of composing a description of some pure binary
     file and turning it into a PDB file with this design.
     A script or a program to do this becomes relatively simple
     in comparison to the previous PDB formats. By emphasizing
     the human readability and comprehensibility of the
     metadata, the entire PDBLib machinery becomes even
     more accessible for a wider range of applications.
<p>

In order to meet these goals (especially goal 3) it is imperative
that the format of the metadata be documented. You can also use
a text editor such as emacs to look at a PDB file.  If you peruse
the end of a format version I or II file you will recognize some
elements of those formats.  If you look at the end of a format
version III file you will plainly see the metadata spelled out.
<p>
The current default is to write format version II files.  This
is a transitional measure to allow users to become familiar with
format version III.  An application can select which format
version to use when creating new PDB files.  See the
<a href="#pd_set_fmt_version">PD_set_fmt_version</a> routine for details.

<h3>Format Version III Syntax</h3>
<p>
A) PrimitiveTypes
<p>
   The primitive data types are perhaps the most fundamental of
   all the metadata.  The compound types are defined in terms of
   primitive types and other compound types.  The symbol table
   cannot be interpreted without understanding the types.
<p>
   The metadata section for the primitive types starts with a
   line containing only the tag, "PrimitiveTypes:".  Each primitive
   type is defined by a single line of the form:
<pre>
      &#60;type&#62; &#60;Nbytes&#62; &#60;alignment&#62; &#60;attributes&#62;;
</pre>
   where
<pre>
      &#60;type&#62;       := the name of the data type, e.g. int
      &#60;Nbytes&#62;     := the length in bytes of an instance of the type
      &#60;alignment&#62;  := the byte alignment of the type
      &#60;attributes&#62; := &#60;attribute&#62;[|&#60;attribute&#62;]*
      &#60;attribute&#62;  := &#60;order&#62; | FIX | &#60;float&#62; | NO-CONV | UNSGNED |
                      &#60;typedef&#62; | ONESCMP
      &#60;order&#62;      := ORDER(&#60;ord-spec&#62;)
      &#60;ord-spec&#62;   := big | little | &#60;Nbytes comma delimited integers&#62;
      &#60;float&#62;      := FLOAT(&#60;float-spec&#62;)
      &#60;float-spec&#62; := #bits,#exponent-bits,#mantiss-bits,offset-sign-bit,
                      offset-exponent-field,offset-mantisssa-field,
                      leading-bit-flag,exponent-bias
      &#60;typedef&#62;    := TYPEDEF(&#60;type&#62;)
</pre>


   Three other fundamental pieces of information exist with the 
   primitive types:
<pre>
      StructAlignment    - additional alignment for structs
      DefaultIndexOffset - the value of the default array index offset
                           typically 0 for C codes and 1 for Fortran
      MajorOrder         - the major ordering for multi-dimensional arrays
                         - typically "row" for C and "column" for Fortran
</pre>
   
   The following excerpt from an actual file illustrates:
<pre>
      PrimitiveTypes:
         *          4   4  ORDER(little)|FIX;
         char       1   1  NO-CONV|FIX;
         int        4   4  ORDER(little)|FIX;
         u_int      4   4  ORDER(little)|FIX|UNSGNED;
         double     8   8  ORDER(little)|FLOAT(64,11,52,0,1,12,0,1023);
         function   4   4  ORDER(little)|FIX;
         Directory  1   0  NO-CONV|FIX;

      StructAlignment: 0
      DefaultIndexOffset: 0
      MajorOrder: row
</pre>

B) CompoundTypes
<p>

   Compound types are constructed from primitive types and other compound
   types.  This section begins with a line containing just the tag,
   "CompoundTypes:".  Each compound type begins with a line of the form:
<pre>

      &#60;type&#62; (&#60;Nbytes&#62;)
</pre>

   where
<pre>
      &#60;type&#62;   := the name of the compound type
      &#60;Nbytes&#62; := the number of bytes required to represent one
                  instance of &#60;type&#62; on disk
</pre>
   The members of the compound type are specified one per line.  The
   syntax is:
<pre>
       {&#60;member&#62;
        &#60;member&#62;
           .
           .
           .
        &#60;member&#62;};
</pre>
   where
<pre>
      &#60;member&#62; := &#60;type&#62; &#60;name&#62; [&#60;- &#60;cast&#62;];
      &#60;type&#62;   := the name of the type of the member
      &#60;name&#62;   := the name of the member
      &#60;cast&#62;   := the name of the member whose true type
                  this member contains (this member must
                  have &#60;type&#62; "char *")
</pre>
   The following example illustrates:
<pre>
      CompoundTypes:
         pcons (16)
            {char *car_type;
             char *car &#60;- car_type;
             char *cdr_type;
             char *cdr &#60;- cdr_type;};
</pre>

C) SymbolTable
<p>
   The symbol table associates a name with the information
   describing an object written to the file. This section begins
   with a line containing just the tag, "SymbolTable:".  Each
   entry is described in a single line of the form:
<pre>
     &#60;type&#62; &#60;name&#62;[&#60;dimensions&#62;] @ &#60;address&#62; (&#60;Nitems&#62;);
</pre>
   where
<pre>
     &#60;type&#62;       := is the name of the type of the data
     &#60;name&#62;       := is the name of the entry
                     all names are fit into a hierarchy on the
                     model of a file system and as such begin
                     with '/'
     &#60;dimensions&#62; := &#60;dim_start&#62;&#60;dimension&#62;[,&#60;dimension&#62;]*&#60;dim_end&#62;
     &#60;dim_start&#62;  := '(' | '['
     &#60;dimension&#62;  := &#60;length&#62; | &#60;min&#62;:&#60;max&#62;
     &#60;dim_end&#62;    := ')' | ']'
</pre>
   The following example illustrates some of the syntax:
<pre>
      SymbolTable:
         Directory / @ 0 (1);
         Directory /&ptrs/ @ 1 (1);
         integer /ia[0:4] @ 41 (5);
         float /fa2[0:3,0:2] @ 61 (12);
         double /da[0:3] @ 109 (4);
         char /ca[0:9] @ 21 (10);
         integer /is @ 5 (1);
         short /sa[0:4] @ 31 (5);
         char * /cap[0:2] @ 141 (3);
         st62 /d71 @ 2 (1);
         double /&ptrs/ia_2 @ 98 (10);
         double /&ptrs/ia_1 @ 10 (10);
</pre>

D) Extras
<p>
   Certain metadata does not specifically describe symbol table
   entries or data types.  Also, from time to time extra information
   is added to PDB files to adapt to new requirements.  This
   information is placed in the extras table.  The data in the
   extras table is optional and PDBLib has default values for each
   piece of information which permit backwards compatibility to
   the greatest possible degree.
<p>
   The extras appear after the symbol table.  The syntax of the
   entries varies according to the needs of the data, but as
   a rule of thumb the syntax is:
<pre>
     &#60;key&#62;: &#60;value&#62;
</pre>

   The following example illustrates some of the syntax:
<pre>
      UseItags: 0
      Version: 19 (Fri Apr 28 14:04:36 2006)
</pre>

E) Addresses and Trailer
<p>
   The last three elements of version III syntax are the
   addresses of the structure chart (beginning with the
   PrimitiveTypes section), the symbol table, and a tag
   which identifies the file as being a PDB file using
   format version III.
<p>
   The following example illustrates this syntax:
<pre>
      StructureChartAddress: 40247
      SymbolTableAddress: 41442
      !&#60&#60PDB:3&#62&#62!
</pre>

F) Blocks
<p>
   PDBLib supports notions like PD_append which allow
   applications to add more data to an entry in the file.
   In order to do this with the greatest flexibility
   and economy of disk space, PDBLib allows a variable
   to be stored in discontiguous blocks of space in the
   file.  Logically, the variable is contiguous, only
   the underlying storage pattern is discontiguous.
<p>
   If a variable is completely contiguous, its symbol
   table entry in the file has a complete specification.
   If a variable has discontiguous parts, its symbol
   table entry in the file completely describes the
   first continguous part (this makes non-PDB readers
   able to make sense of at least some of the data).
   The remainder of the variable is described by
   a list of blocks in the extras section of the file.
<p>
   By virtue of the fact the PDB only allows extending
   the most slowly increasing dimension, it is sufficient
   that a block specify the number of items in the
   discontiguous piece and its starting address.
<p>
   A section in the extras part of the file which begins
   with the tag "Blocks:" contains all of the block
   information for all variables in the file.  Each
   entry has the form:
<pre>
       &#60name&#62 &#60n-blocks&#62
           &#60addr&#62 &#60n-items&#62          n-blocks pairs
</pre>
   where
<pre>
       &#60name&#62     := the name of the variable
       &#60n-blocks&#62 := the number of block specifcations
                     to follow
       &#60addr&#62     := the starting address of the data
                     for the block
       &#60n-items&#62  := the number of items in the block
</pre>

G) Checksums
<p>
   PDBLib supports per variable checksumming.  In practice,
   since checksumming works with PD_append, it is actually
   per block checksumming (see the above Blocks section).
<p>
   Because checksumming is independent of having blocks
   the per variable checksum information has its own
   extras section and tag, "Checksums:". Each
   entry has the form:
<pre>
       &#60name&#62 &#60n-blocks&#62 &#60n-entries&#62
           &#60block-n&#62 &#60block-checksum&#62        n-entries 
</pre>
   where
<pre>
       &#60name&#62            := the name of the variable
       &#60n-blocks&#62        := the number of block specifcations to follow
       &#60n-entries&#62       := the number of checksum specifcations to follow
       &#60block-n&#62         := the index of the block whose checksum follow
       &#60block-checksum&#62  := the checksum for the specified block
</pre>
   The checksums are laid out this way because it is possible to
   reserve a block of data with PD_defent and not write to it.
   Consequently, a variable can have blocks with no checksum and
   entries are made only for blocks possessing a valid checksum
   at the time the file is closed.
<p>

H) Hidden Directories
<p>
   PDBLib has the notion of a directory (after the file
   system model used by most operating systems).  All variables
   are entered in the symbol table as a "full path".  This
   means all symbol table entry names begin with '/'.
<p>
   PDBLib stores some of its metadata using PDBLib API
   calls such as PD_write.  This has several advantages:
   minimizes special coding for metadata; and exposes
   some metadata to generic PDB file browsers such as
   PDBView.
<p>
   In order to make plain which information is data and which
   metadata, PDBLib uses a convention that root level
   directories whose name begins with '&' contain metadata.
   By default, such directories will not be listed by
   PDBView which has earned them the nickname "hidden
   directories".
<p>
   Current hidden directories are:
<pre>
      /&ptrs
      /&etc
</pre>

I) Pointers
<p>
   With regard to the handling of pointers, earlier
   versions of PDBLib did not write the actual pointers
   into the file (and struct members that were pointers
   were simply copied unconverted to the file).  They
   wrote a simplified version of a symbol table entry
   called an ITAG, into the file at the start of the
   pointee's data. This had the bad effect of mingling
   data and metadata in the file.
<p>
   Starting with format version III, PDBLib offers
   a new model which converts pointers to integer indices
   that are used to make symbol table entries
   for the pointees which are then entered in the
   /&ptrs directory.  The pointers are therefore not
   ignored as they were previously.  They are simply
   another kinds of data which has a well defined
   conversion algorithm, just as floats or integers do.
<p>
   With this model it is not necessary to read from the
   disk to ascertain the location, type, and number
   information which describe the pointees.  That resides
   in the symbol table which is kept in memory.  As
   a result the performance of PDBLib when handling
   pointers is much improved.
<p>
   Pointers are assigned an index, more or less, in the
   order in which they are written.  So as the Nth
   pointer is written to the file, the number N is what
   the pointer is converted to and the pointee gets
   an entry in the symbol table /&ptrs/ia_N.
<p>
J) Attributes
<p>
   The metadata supporting PDBLib's attribute handling
   is written out in the /&etc hidden directory.  The
   pointers in the attribute table are not written in
   the /&ptrs directory, but in /&etc along with the
   attribute table itself.
<p>
NOTE: It is PDBLib policy that files with all prior
format versions will be readable by all subsequent PDBLib versions.
Current PDBLib design allows applications to select the format
version they wish to use when creating new files (except for
version I - current PDBLib cannot create a version I file any
longer).  We actively test this in our regression test suite.
<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_terms"><h2>Terms</h2></a>


A few definitions must be given first to keep the following summary
concise. Some of these will be elaborated upon in the next section.
<p>

PDBfile: for the purposes of a program this is a collection of all
the relevant information about one of these data files (in the actual
coding it is a C struct). See the section on Data Structures for
more information.
<p>

ENTRY: for the purposes of a program this is a
collection of all the relevant information about a variable written
to a PDB file.
<p>

DEFSTR: for the purposes of a program this is a
collection of all the relevant information about a data type in
a PDB file.
<p>

ASCII: a string
<p>

VOID: any data type
<p>

TRUE: a value of 1 (defined by PDBLib)
<p>

FALSE: a value of 0 (defined by PDBLib)
<p>

<h4>C context</h4>

LAST: a pointer to 0 (declared and defined by PDBLib)
<p>

<h4>FORTRAN context</h4>

LAST: the value 0
<p>

REAL*8: an eight byte floating point number.
<p>

<h4>SX context</h4>

TRUE: #t or anything that it not #f or nil.
<p>

FALSE: #f or nil.
<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_conv"><h2>Data Conversion and Compression</h2></a>


PDBLib has a quasi-universal data translation capability. It is
called Parametrized Data Conversion (PDC). A set of parameters
which characterizes a large set of integer and floating point
formats was developed. It describes the byte size and order of
integer types.	For floating point data it describes the bit location
and width of the sign, exponent, and mantissa field as well as the
byte size and order of the data. Using this information a single
integer conversion routine and a single floating point conversion
routine handle all of the data conversions in PDBLib. An advantage
of this approach is that there is no increase in the size of the
library for each port to a new environment. Furthermore, it will
allow future releases to auto-configure themselves to the machines
on which they run. Another benefit is that a data representation
may be targeted without regard to its implementation. This provides
a vehicle for developing data representations, evaluating them, or
using them in a highly abstract manner. One drawback of this
approach is that it makes assumptions about the representation of
data in computers. While the assumptions are general and the result
of incorporating data representations outside these assumptions is
more work on the library itself, it is philosophically unsatisfying
to make any assumptions about how things are to be done or data to
be represented. Another drawback is that by being general purpose
the conversion routines are slightly slower than specific ones. This
is more than made up for in the saving in library size and ease of
porting the library.
<p>
Alternative data conversion strategies are either hub and spoke (such
as Sun's XDR) or specific format to format. The latter suffers from
an N<sup>2</sup> growth in the number of conversion routines where N is the
number of machine/architectures which the library supports. On the
other hand, hub and spoke strategies necessitate a conversion on
each read or write operation.
<p>
PDC prevents the conversion problem from being N<sup>2</sup>. At worst, the PDC
method should grow like N in the number of parameter sets required.
In practice, it is even better than that. Most computer systems
today are based on a handful of CPU's which are the most
constraining factor in binary data formats. For the convenience of
the users of PDBLib, several data_standard's and data_alignment's
are predefined by the library itself.
<p>
A significant advantage to PDC is that a class of data compression
algorithms is implicit in the method. By simply describing a format
which describes data in the correct range (up to a possible overall
offset for each type), PDBLib can do all of the work to store and
retrieve the data in a compressed form.
<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_guidance"><h2>Using the PDBLib API</h2></a>


There is a hierarchy of routines in PDBLib from high to low level. The
high level routines form the API while the lower level routines are
modularized to perform the actual work. It should be noted that the
lower level routines are sufficiently well modularized so as to make
it possible to build entirely different API's for PDBLib.
<p>

<h3>Comments on the C API</h3>

The high level PDBLib routines have a strict naming convention. All
high level routines begin with `<tt>PD_</tt>'. Some routines have several
related forms. These are referred to as families of functions. For
example the <tt>PD_write</tt> family.
<p>
Most routines maintain a thread of control's error message variable. The 
value of the error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 
The error messages include the name of the routine in which they are made 
thus eliminating the need for error codes which must be cross referenced 
with some other document. In this way application programs can check for 
error conditions themselves and decide in what manner to use the PDBLib 
error messages instead of having error messages printed by the system 
routines. An error message variable is maintained per thread for 
multi-threaded applications. Error messages are not stacked and must be 
processed by the application before any other PDBLib calls are made in 
order to avoid potential overwrites. See the descriptions of individual 
routines for more information about error handling and messages.
<p>
Programs written in C must include a header which makes certain
declarations and definitions needed to use the library. Much in the
same spirit as one includes stdio.h to use printf and others,
include pdb.h as follows:
<p>

<pre>
  #include "pdb.h"
</pre>

The file pdb.h #include's some other files which must either be in
your directory space or pointed to in some manner which your compiler
can recognize. The auxiliary #include files are schash.c, scstd.h,
and score.h. These files are a part of the SCORE package which you
must have to use PDBLib.
<p>

<h3>Comments on the FORTRAN API</h3>

The high level PDBLib routines have a strict naming convention. All
routines in the FORTRAN API begin with `<tt>PF</tt>'.
<p>
Note: many of these functions return integer values. The implicit
typing convention in FORTRAN would indicate that they return real
values. Application programs must explicitly type these functions
as integers.
<p>
When an error condition is detected by PDBLib it saves a message in a
global C character string. FORTRAN programs can access this error
message by invoking function <tt>PFGERR</tt>. The message contains
the name of the function in which the error occurred thus eliminating
the need for a cross reference document on error codes.  In this way
applications programs can check for error conditions themselves and
decide in what manner to use the PDBLib error messages instead of
having error messages printed by the system routines. Error messages
are not stacked and must be processed by the application before any
other PDBLib calls are made in order to avoid potential overwrites.
See the FORTRAN API section for more information about which routines
return error messages.
<p>

<!-- Comments on the Python Module>


<h3>Organization Guide</h3>

PDBLib offers a great deal of flexibility to its users. In some cases
the flexibility results in a bewildering assortment of function calls
from which the application developer must choose. The functions of
the PDBLib API are described in detail but are grouped according to
a general kind of intended usage, for example, writing to a PDB file.
<p>

<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_opening"><h3>Opening, Creating, Closing, and Controlling PDB Files</h3></a>


These are the most fundamental operations involving PDBLib. The function
<tt>PD_open</tt> is used to either open an existing file or create a new one.
<tt>PD_close</tt> is used to close a PDB file so that it can be recognized by
PDBLib for future operations.
<p>
PDBLib allows applications to specify the binary format in which a newly
created file will be written. <tt>PD_target</tt> does this work. It is not
necessary to invoke <tt>PD_target</tt> before creating a new PDB file. In this
case the binary format is that appropriate for the host system.
<p>
During the run of an application code, PDBLib can complete the information
in an open PDB file so that in the event of a code crash, the file will
be a valid PDB file. This functionality is provided by <tt>PD_flush</tt>.
<p>

<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_set_buffer_size"><h2>PD_SET_BUFFER_SIZE</h2></a>


<p>
<pre>
<i>C Binding: </i>  int64_t PD_set_buffer_size(int64_t v)
<i>Fortran Binding: </i>integer PFSBFS(integer v)
<i>SX Binding: </i> (pd-set-buffer-size! v)
<i>Python Binding: </i> XXX - missing
</pre>
<p>

Set the buffer size which PDBLib will use for all PDB files
to <em>v</em>.
<p>
The arguments to this function are:
<em>v</em>, an integer value for the buffer size in bytes.

<p>
Return the integer value of the buffer size in bytes.
<p>

<hr>
<b>C Example</b>
<pre>
       int64_t v;
             .
             .
             .

       PD_set_buffer_size(v);
             .
             .
             .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfsbfs
       integer v, sz
             .
             .
             .

       sz = pfsbfs(v)
             .
             .
             .
</pre>
<hr>

<b>SX Example</b>
<pre>
             .
             .
             .

       (pd-set-buffer-size! 4096)
             .
             .
             .
</pre>
<hr>

<b>Python Example</b>
<pre>
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_target"><h2>PD_TARGET</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_target(data_standard *std, data_alignment *align)
<i>Fortran Binding: </i>integer PFTRGT(integer is, integer ia)
<i>SX Binding: </i> (target is ia)
<i>Python Binding: </i> XXX - missing
</pre>
<p>

Write the next PDB file according to the specified data standard and
alignment. PDBLib has a general binary data conversion mechanism
called parametrized data conversion (PDC). An integer type is
described by one set of parameters and a floating point type is
described by another. A general purpose conversion routine takes
the description of the input type and a description of the desired
output type and does the conversion. In this way, PDBLib avoids an
N<sup>2</sup> increase in data conversion routines as it ports to new machines.
In fact, the number of data standards and alignments grows more
slowly than N because many machines share common formats.
<p>
An additional advantage to PDC is that by specifying a format
involving the minimal number of bits to represent the data for a
file, PDBLib can carry out a large class of data compressions.
<p>

<p>
Available builtin data standards are:

<BLOCKQUOTE>
<TABLE>

<TR>
<TD ALIGN="CENTER">C</TD>
<TD ALIGN="CENTER">Fortran</TD>
<TD WIDTH="105" ALIGN="CENTER">Scheme</TD>
<TD ALIGN="CENTER">Byte Order</TD>
<TD ALIGN="CENTER">S/I/L/LL</TD>
<TD ALIGN="CENTER">F/D</TD>
</TR>

<TR>
<TD>TEXT_STD</TD>
<TD ALIGN="CENTER">0</TD>
<TD WIDTH="105" ALIGN="CENTER">text-std</TD>
<TD>REVERSE_ORDER</TD>
<TD WIDTH="80" ALIGN="CENTER">2,4,8,8</TD>
<TD WIDTH="40" ALIGN="CENTER">4,8</TD>
</TR>

<TR>
<TD>I386_STD</TD>
<TD ALIGN="CENTER">1</TD>
<TD WIDTH="105" ALIGN="CENTER">i386-std</TD>
<TD>REVERSE_ORDER</TD>
<TD WIDTH="80" ALIGN="CENTER">2,2,4,4</TD>
<TD WIDTH="40" ALIGN="CENTER">4,8</TD>
</TR>

<TR>
<TD>I586L_STD</TD>
<TD ALIGN="CENTER">2</TD>
<TD WIDTH="105" ALIGN="CENTER">i586l-std</TD>
<TD>REVERSE_ORDER</TD>
<TD WIDTH="80" ALIGN="CENTER">2,4,4,8</TD>
<TD WIDTH="40" ALIGN="CENTER">4,8</TD>
</TR>

<TR>
<TD>I586O_STD</TD>
<TD ALIGN="CENTER">3</TD>
<TD WIDTH="105" ALIGN="CENTER">i586o-std</TD>
<TD>REVERSE_ORDER</TD>
<TD WIDTH="80" ALIGN="CENTER">2,4,4,8</TD>
<TD WIDTH="40" ALIGN="CENTER">4,8</TD>
</TR>

<TR>
<TD>PPC32_STD</TD>
<TD ALIGN="CENTER">4</TD>
<TD WIDTH="105" ALIGN="CENTER">ppc32-std</TD>
<TD>NORMAL_ORDER</TD>
<TD WIDTH="80" ALIGN="CENTER">2,4,4,8</TD>
<TD WIDTH="40" ALIGN="CENTER">4,8</TD>
</TR>

<TR>
<TD>X86_64_STD</TD>
<TD ALIGN="CENTER">5</TD>
<TD WIDTH="105" ALIGN="CENTER">x86-64-std</TD>
<TD>REVERSE_ORDER</TD>
<TD WIDTH="80" ALIGN="CENTER">2,4,8,8</TD>
<TD WIDTH="40" ALIGN="CENTER">4,8</TD>
</TR>

<TR>
<TD>PPC64_STD</TD>
<TD ALIGN="CENTER">7</TD>
<TD WIDTH="105" ALIGN="CENTER">ppc64-std</TD>
<TD>NORMAL_ORDER</TD>
<TD WIDTH="80" ALIGN="CENTER">2,4,8,8</TD>
<TD WIDTH="40" ALIGN="CENTER">4,8</TD>
</TR>

<TR>
<TD>M68X_STD</TD>
<TD ALIGN="CENTER">8</TD>
<TD WIDTH="105" ALIGN="CENTER">m68x-std</TD>
<TD>NORMAL_ORDER</TD>
<TD WIDTH="80" ALIGN="CENTER">2,2,4,4</TD>
<TD WIDTH="40" ALIGN="CENTER">4,12</TD>
</TR>

<TR>
<TD>VAX_STD</TD>
<TD ALIGN="CENTER">9</TD>
<TD WIDTH="105" ALIGN="CENTER">vax-std</TD>
<TD>REVERSE_ORDER</TD>
<TD WIDTH="80" ALIGN="CENTER">2,4,4,8</TD>
<TD WIDTH="40" ALIGN="CENTER">4,8</TD>
</TR>

<TR>
<TD>CRAY_STD</TD>
<TD ALIGN="CENTER">10</TD>
<TD WIDTH="105" ALIGN="CENTER">cray-std</TD>
<TD>NORMAL_ORDER</TD>
<TD WIDTH="80" ALIGN="CENTER">8,8,8,8</TD>
<TD WIDTH="40" ALIGN="CENTER">8,8</TD>
</TR>

</TABLE>
</BLOCKQUOTE>

<pre>
NOTE: S/I/L/LL - short/int/long/longlong  byte sizes
      F/D      - float/double byte sizes
</pre>
<br>

Available data alignments are:
<p>

<BLOCKQUOTE>
<TABLE>
<TR>
<TD ALIGN="CENTER">C</TD>
<TD WIDTH="60" ALIGN="CENTER">Fortran</TD>
<TD WIDTH="85" ALIGN="CENTER">Scheme</TD>
<TD WIDTH="240" ALIGN="CENTER">P/C/S/I/L/LL/F/D/LD/Str</TD>
</TR>

<TR>
<TD>TEXT_ALIGNMENT</TD>
<TD WIDTH="60" ALIGN="CENTER">0</TD>
<TD WIDTH="85" ALIGN="CENTER">text-algn</TD>
<TD WIDTH="240" ALIGN="CENTER">1, 1, 1, 1, 1, 1, 1, 1,  1, 0</TD>
</TR>

<TR>
<TD>BYTE_ALIGNMENT</TD>
<TD WIDTH="60" ALIGN="CENTER">1</TD>
<TD WIDTH="85" ALIGN="CENTER">byte-algn</TD>
<TD WIDTH="240" ALIGN="CENTER">1, 1, 1, 1, 1, 1, 1, 1,  1, 0</TD>
</TR>

<TR>
<TD>WORD2_ALIGNMENT</TD>
<TD WIDTH="60" ALIGN="CENTER">2</TD>
<TD WIDTH="85" ALIGN="CENTER">word2-algn</TD>
<TD WIDTH="240" ALIGN="CENTER">2, 1, 2, 2, 2, 2, 2, 2,  2, 0</TD>
</TR>

<TR>
<TD>WORD4_ALIGNMENT</TD>
<TD WIDTH="60" ALIGN="CENTER">3</TD>
<TD WIDTH="85" ALIGN="CENTER">word4-algn</TD>
<TD WIDTH="240" ALIGN="CENTER">4, 1, 4, 4, 4, 4, 4, 4,  4, 0</TD>
</TR>

<TR>
<TD>WORD8_ALIGNMENT</TD>
<TD WIDTH="60" ALIGN="CENTER">4</TD>
<TD WIDTH="85" ALIGN="CENTER">work8-algn</TD>
<TD WIDTH="240" ALIGN="CENTER">8, 4, 8, 8, 8, 8, 8, 8,  8, 8</TD>
</TR>

<TR>
<TD>GNU4_I686_ALIGNMENT</TD>
<TD WIDTH="60" ALIGN="CENTER">5</TD>
<TD WIDTH="85" ALIGN="CENTER">gnu4-i686-algn</TD>
<TD WIDTH="240" ALIGN="CENTER">4, 1, 2, 4, 4, 8, 4, 4,  4, 0</TD>
</TR>

<TR>
<TD>OSX_10_5_ALIGNMENT</TD>
<TD WIDTH="60" ALIGN="CENTER">6</TD>
<TD WIDTH="85" ALIGN="CENTER">osx-10-5-algn</TD>
<TD WIDTH="240" ALIGN="CENTER">4, 1, 2, 4, 4, 4, 4, 4, 16, 0</TD>
</TR>

<TR>
<TD>SPARC_ALIGNMENT</TD>
<TD WIDTH="60" ALIGN="CENTER">7</TD>
<TD WIDTH="85" ALIGN="CENTER">sparc-algn</TD>
<TD WIDTH="240" ALIGN="CENTER">4, 1, 2, 4, 4, 4, 4, 8,  8, 0</TD>
</TR>

<TR>
<TD>XLC32_PPC64_ALIGNMENT</TD>
<TD WIDTH="60" ALIGN="CENTER">8</TD>
<TD WIDTH="85" ALIGN="CENTER">xlc32-ppc64-algn</TD>
<TD WIDTH="240" ALIGN="CENTER">4, 1, 2, 4, 4, 8, 4, 4,  4, 0</TD>
</TR>

<TR>
<TD>CYGWIN_I686_ALIGNMENT</TD>
<TD WIDTH="60" ALIGN="CENTER">9</TD>
<TD WIDTH="85" ALIGN="CENTER">cygwin-i686-algn</TD>
<TD WIDTH="240" ALIGN="CENTER">4, 1, 2, 4, 4, 8, 4, 8,  4, 0</TD>
</TR>

<TR>
<TD>GNU3_PPC64_ALIGNMENT</TD>
<TD WIDTH="60" ALIGN="CENTER">10</TD>
<TD WIDTH="85" ALIGN="CENTER">gnu3-ppc64-algn</TD>
<TD WIDTH="240" ALIGN="CENTER">4, 1, 2, 4, 4, 8, 4, 8, 16, 0</TD>
</TR>

<TR>
<TD>XLC64_PPC64_ALIGNMENT</TD>
<TD WIDTH="60" ALIGN="CENTER">12</TD>
<TD WIDTH="85" ALIGN="CENTER">xlc64-ppc64-algn</TD>
<TD WIDTH="240" ALIGN="CENTER">4, 1, 2, 4, 4, 8, 4, 4,  4, 0</TD>
</TR>

<TR>
<TD>GNU4_X86_64_ALIGNMENT</TD>
<TD WIDTH="60" ALIGN="CENTER">13</TD>
<TD WIDTH="85" ALIGN="CENTER">gnu4-x86-64-algn</TD>
<TD WIDTH="240" ALIGN="CENTER">4, 1, 2, 4, 4, 8, 4, 8, 16, 0</TD>
</TR>

</TABLE>
</BLOCKQUOTE>

<pre>
NOTE: P/C/S/I/L/LL/F/D/LD/Str - pointer/char/short/int/long/longlong/float/double/longdouble/struct
</pre>

<br>
<p>
In the FORTRAN API, these structures are placed in two arrays and the
indices into these arrays are passed into <tt>PFTRGT</tt> to specify the
binary format to be targeted.
<p>
Some common configurations are:
<p>

<TABLE>
<TR>
<TD>Platform</TD>
<TD WIDTH=100 ALIGN=CENTER>Method</TD>
<TD>Call</TD>
</TR>

<TR>
<TD>GCC 4.0 and later X86_64</TD>
<TD WIDTH=100 ALIGN=CENTER>C</TD>
<TD><tt>PD_target(&amp;X86_64_STD, &amp;GNU4_X86_64_ALIGNMENT)</tt></TD>
</TR>
<TR>
<TD></TD>
<TD WIDTH=100 ALIGN=CENTER>Fortran</TD>
<TD><tt>PFTRGT(5, 13)</tt></TD>
</TR>
<TR>
<TD></TD>
<TD WIDTH=100 ALIGN=CENTER>Scheme</TD>
<TD><tt>(target x86-64-std gnu4-x86-64-algn)</tt></TD>
</TR>

<TR>
<TD>GCC 4.0 and later Ix86</TD>
<TD WIDTH=100 ALIGN=CENTER>C</TD>
<TD><tt>PD_target(&amp;I586L_STD, &amp;GNU4_X86_64_ALIGNMENT)</tt></TD>
</TR>
<TR>
<TD></TD>
<TD WIDTH=100 ALIGN=CENTER>Fortran</TD>
<TD><tt>PFTRGT(2, 13)</tt></TD>
</TR>
<TR>
<TD></TD>
<TD WIDTH=100 ALIGN=CENTER>Scheme</TD>
<TD><tt>(target i586l-std gnu4-x86-64-algn)</tt></TD>
</TR>

<TR>
<TD>Mac OSX 10.6 and later</TD>
<TD WIDTH=100 ALIGN=CENTER>C</TD>
<TD><tt>PD_target(&amp;X86_64_STD, &amp;GNU4_X86_64_ALIGNMENT)</tt></TD>
</TR>
<TR>
<TD></TD>
<TD WIDTH=100 ALIGN=CENTER>Fortran</TD>
<TD><tt>PFTRGT(5, 13)</tt></TD>
</TR>
<TR>
<TD></TD>
<TD WIDTH=100 ALIGN=CENTER>Scheme</TD>
<TD><tt>(target x86-64-std gnu4-x86-64-algn)</tt></TD>
</TR>

<TR>
<TD>Mac OSX 10.5</TD>
<TD WIDTH=100 ALIGN=CENTER>C</TD>
<TD><tt>PD_target(&amp;I586O_STD, &amp;OSX_10_5_ALIGNMENT)</tt></TD>
</TR>
<TR>
<TD></TD>
<TD WIDTH=100 ALIGN=CENTER>Fortran</TD>
<TD><tt>PFTRGT(3, 6)</tt></TD>
</TR>
<TR>
<TD></TD>
<TD WIDTH=100 ALIGN=CENTER>Scheme</TD>
<TD><tt>(target i586o-std osx-10-5-algn)</tt></TD>
</TR>

<TR>
<TD>Cygwin i686</TD>
<TD WIDTH=100 ALIGN=CENTER>C</TD>
<TD><tt>PD_target(&amp;I586L_STD, &amp;CYGWIN_I686_ALIGNMENT)</tt></TD>
</TR>
<TR>
<TD></TD>
<TD WIDTH=100 ALIGN=CENTER>Fortran</TD>
<TD><tt>PFTRGT(2, 9)</tt></TD>
</TR>
<TR>
<TD></TD>
<TD WIDTH=100 ALIGN=CENTER>Scheme</TD>
<TD><tt>(target i586l-std cygwin-i686-algn)</tt></TD>
</TR>

<TR>
<TD>IBM PPC64 XLC 64 bit</TD>
<TD WIDTH=100 ALIGN=CENTER>C</TD>
<TD><tt>PD_target(&amp;PPC64_STD, &amp;XLC64_PPC64_ALIGNMENT)</tt></TD>
</TR>
<TR>
<TD></TD>
<TD WIDTH=100 ALIGN=CENTER>Fortran</TD>
<TD><tt>PFTRGT(7, 12)</tt></TD>
</TR>
<TR>
<TD></TD>
<TD WIDTH=100 ALIGN=CENTER>Scheme</TD>
<TD><tt>(target ppc64-std xlc64-ppc64-algn)</tt></TD>
</TR>

<TR>
<TD>IBM PPC64 XLC 32 bit</TD>
<TD WIDTH=100 ALIGN=CENTER>C</TD>
<TD><tt>PD_target(&amp;PPC32_STD, &amp;XLC32_PPC64_ALIGNMENT)</tt></TD>
</TR>
<TR>
<TD></TD>
<TD WIDTH=100 ALIGN=CENTER>Fortran</TD>
<TD><tt>PFTRGT(4, 8)</tt></TD>
</TR>
<TR>
<TD></TD>
<TD WIDTH=100 ALIGN=CENTER>Scheme</TD>
<TD><tt>(target ppc32-std xlc32-ppc64-algn)</tt></TD>
</TR>

<TR>
<TD>SPARC</TD>
<TD WIDTH=100 ALIGN=CENTER>C</TD>
<TD><tt>PD_target(&amp;PPC32_STD, &amp;SPARC_ALIGNMENT)</tt></TD>
</TR>
<TR>
<TD></TD>
<TD WIDTH=100 ALIGN=CENTER>Fortran</TD>
<TD><tt>PFTRGT(4, 7)</tt></TD>
</TR>
<TR>
<TD></TD>
<TD WIDTH=100 ALIGN=CENTER>Scheme</TD>
<TD><tt>(target ppc32-std sparc-algn)</tt></TD>
</TR>

<TR>
<TD>DOS</TD>
<TD WIDTH=100 ALIGN=CENTER>C</TD>
<TD><tt>PD_target(&amp;I386_STD, &amp;WORD2_ALIGNMENT)</tt></TD>
</TR>
<TR>
<TD></TD>
<TD WIDTH=100 ALIGN=CENTER>Fortran</TD>
<TD><tt>PFTRGT(1, 2)</tt></TD>
</TR>
<TR>
<TD></TD>
<TD WIDTH=100 ALIGN=CENTER>Scheme</TD>
<TD><tt>(target i386-std word2-algn)</tt></TD>
</TR>

</TABLE>

<br>
<p>
The arguments to this function are:
<em>std</em>, a pointer to a data_standard structure;
<em>is</em>, an index corresponding one of the data_standards;
<em>align</em>, a pointer to a data_alignment structure;
   and
<em>ia</em>, an index corresponding to one of the data_alignment structures
   above.
See the section on Data Structures.
<p>
The return value is <tt>TRUE</tt>, if successful; otherwise, <tt>FALSE</tt> is returned
and an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 
<p>
See also:
<a href="#pdb_std_s">DATA_STANDARD</a>
and
<a href="#pdb_align_s">DATA_ALIGNMENT</a>
.
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
void test_target(char *tgt, char *base, int n,
		 char *fname, char *datfile, long nb)
    {int rv;

     if (tgt != NULL)
        {rv = PD_target_platform(tgt);
         snprintf(fname, nb, "%s-%s.rs%d", base, tgt, n);
         snprintf(datfile, nb, "%s-%s.db%d", base, tgt, n);}
     else
        {snprintf(fname, nb, "%s-nat.rs%d", base, n);
         snprintf(datfile, nb, "%s-nat.db%d", base, n);};
 
     return;}
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pftrgt
       integer is, ia
             .
             .
             .

 c ... set target architecture
 c ... PPC32_STD
       is = 4
 c ... GNU3_PPC64_ALIGNMENT
       ia = 10
       if (pftrgt(is, ia) .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<b>SX Example</b>
<pre>
             .
             .
             .

 ; set target architecture
 ; PPC32_STD and GNU3_PPC64_ALIGNMENT
     (target ppc32-std gnu3-ppc64-algn)
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_isfile"><h2>PD_ISFILE</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_isfile(char *fname)
<i>Fortran Binding: </i>none
<i>SX Binding: </i> none
<i>Python Binding: </i> none
</pre>
<p>

Check whether or not the named file is a PDB file.  This is different
than the question of whether or not PDBLib can access the data in the
file.  PDBLib can open and read an SQLITE database file, but that file
is not a PDB file.
<p>
The argument to <tt>PD_isfile()</tt> is:<BR>
<em>fname</em> - an ASCII string, which is the name of the file to be
 checked<BR>
<p>
The function returns TRUE if the file is a PDBfile and FALSE otherwise.
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
    PDBfile *file;
           .
           .
           .

    if (PD_isfile("filename") == FALSE)
       printf("'filename' is not a PDB file\n");
           .
           .
           .
</pre>

<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_open"><h2>PD_OPEN</h2></a>


<p>
<pre>
<i>C Binding: </i>  PDBfile *PD_open(char *filename, char *mode)
<i>Fortran Binding: </i>integer PFOPEN(integer nchr, character name, character mode)
<i>SX Binding: </i> (open-pdbfile name mode)
<i>Python Binding: </i> pact.pdb.open(name, mode='r')
</pre>
<p>

Open an existing PDB file or create a new PDB file. Depending on the
value of the mode argument, PDBLib attempts to open the file <em>filename</em>
in read-only binary mode, open the file in append binary mode, or
create a new file in read-write binary mode. Any string which begins
with "r" causes the file to be opened in read-only mode, any string
beginning with "a" causes the file to be opened in append mode, and
any string beginning with "w" causes a file to be created in
read-write mode. Next the beginning of the file is searched for the
header which identifies the file as having been generated by PDBLib.
The addresses of the structure chart and symbol table are then sought.
<p>
The structure chart from the file is read in. The structure chart
contains information about data types (e.g. floats), their sizes
in bytes and their structures if any. By default there are six
primitive data types that PDBLib knows about: short integers,
integers, long integers, floating point numbers, double precision
floating point numbers, characters, and pointers. The sizes of these
types vary from machine to machine, but PDBLib hides this from
the user.
<p>
The symbol table from the file is read in. The symbol table contains
the list of variables in the file, their types as defined in the
structure chart, and dimensioning information for arrays. Each read
from the file first consults the symbol table to see if the requested
variable is present in the PDB file.
<p>
Both the structure chart and the symbol table are implemented as
hash tables, although their shapes are different. This makes
lookups as efficient as possible given an unknown amount of
data in the file.
<p>
The arguments to <tt>PD_open()</tt> are:<BR>
<em>nchr</em> - the number of characters in <em>filename</em>;<BR>
<em>filename</em> - an ASCII string, which is the name of the file to be
 created or opened;<BR>
<em>mode</em> - an ASCII string, which is the mode (either "w" for
 create/write, "r" for read-only or "a" for append).
<p>
In the C binding the function returns a pointer to a PDBfile.  In the
FORTRAN binding the function returns an integer identifier for the PDBfile
opened/created. In the Python binding the funtion returns a PDBfile
object.  This PDBfile identifies the particular file to PDBLib. As
such, if it is overwritten, the file is lost. The number of PDB files
which can be open simultaneously is machine or operating system
dependent, but each open file has a unique PDBfile associated with it.
<p>
If any aspect of the PDB file opening process fails, a NULL pointer
is returned and an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
    PDBfile *file;
           .
           .
           .

    if ((file = PD_open("filename", "r")) == NULL)
       printf("%s", PD_get_error());
           .
           .
           .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfopen
       integer fileid
             .
             .
             .

       fileid = pfopen(8, 'file.pdb', 'r')
       if (fileid .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<b>SX Example</b>
<pre>
             .
             .
             .

    (define file (open-pdbfile "foo.pdb" "w"))
             .
             .
             .
</pre>
<hr>

<b>Python Example</b>
<pre>
    import pact.pdb
             .
             .
             .

    file = pact.pdb.open("foo.pdb" "w")
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_set_major_order"><h2>PD_SET_MAJOR_ORDER</h2></a>


<p>
<pre>
<i>C Binding: </i>  void PD_set_major_order(PDBfile *file, int v)
<i>Fortran Binding: </i>integer PFSMJO(integer fileid, integer v)
<i>SX Binding: </i> (major-order file v)
<i>Python Binding: </i> XXX - missing
</pre>
<p>

Set the major order for PDB file<em> fileid</em> to <em>v</em>.  The
major order refers to the order of data in multidimensional arrays.
See the discussion of this topic in the section
<a href="#pdb_major">PDBLib and Index Ordering</a>.
<p>
Input to this function is:
<em>fileid</em>, an integer PDBfile identification number,
<em>v</em>, an integer value for the major order.
<p>
In the C binding return the value of the major order.
In the FORTRAN binding return 1 if successful, 0 otherwise.
<p>

<hr>
<b>C Example</b>
<pre>
       PDBfile *file;
             .
             .
             .

       PD_set_major_order(file, COLUMN_MAJOR_ORDER);
             .
             .
             .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfsmjo
       integer fileid, v
             .
             .
             .

       v = 102
       if (pfsmjo(fileid, v) .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<b>SX Example</b>
<pre>
             .
             .
             .

    (major-order file "column-major-order")
             .
             .
             .
</pre>
<hr>

<b>Python Example</b>
<pre>
             .
             .
             .

    file.major_order = "column-major-order"
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_set_fmt_version"><h2>PD_SET_FMT_VERSION</h2></a>


<p>
<pre>
<i>C Binding: </i> int PD_set_fmt_version(int v)
<i>Fortran Binding: </i>integer PFSFMV(integer v)
<i>SX Binding: </i> (format-version v)
<i>Python Binding: </i> XXX - missing
</pre>
<p>

Set the format version to be used when creating new PDB files
to <em>v</em>.  See the section <a href="#pdb_fmt">PDB File Format</a>
for a discussion of the PDB file formats.
<p>
Input to this function is:
<em>v</em>, an integer value for the format version.
<p>
Return the old value of the format version.
<p>

<hr>
<b>C Example</b>
<pre>
             .
             .
             .

       PD_set_fmt_version(3);
             .
             .
             .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfsfmv
       integer v
             .
             .
             .

       pfsfmv(3)
             .
             .
             .
</pre>
<hr>

<b>SX Example</b>
<pre>
             .
             .
             .

    (format-version 3)
             .
             .
             .
</pre>
<hr>

<b>Python Example</b>
<pre>
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_set_max_file_size"><h2>PD_SET_MAX_FILE_SIZE</h2></a>


<p>
<pre>
<i>C Binding: </i> int64_t PD_set_max_file_size(PDBfile *file, int64_t v)
<i>Fortran Binding: </i>integer PFSMXS(integer fileid, integer v)
<i>SX Binding: </i> (set-maximum-file-size! file v)
<i>Python Binding: </i> XXX - missing
</pre>
<p>

Set the maximum file size for PDB file <em>fileid</em> to <em>v</em>.  This
is used to allow applications to make families of related PDB files and
to control the size of the members of the family.
<p>
Input to this function is:
<em>fileid</em>, an integer PDBfile identification number.
Output from this function is:
<em>v</em>, an integer value for the maximum file size.
<p>
Return the old value of the maximum file size.
<p>
See also:
<a href="#pd_family">PD_family</a>
.
<p>

<hr>
<b>C Example</b>
<pre>
       PDBfile *file
             .
             .
             .

       PD_set_max_file_size(fileid, 2000000);
             .
             .
             .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfsmxs
       integer fileid, v, sz
             .
             .
             .

       if (pfsmxs(fileid, sz) .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<b>SX Example</b>
<pre>
             .
             .
             .

     (set-maximum-file-size! file 100000000)
             .
             .
             .
</pre>
<hr>

<b>Python Example</b>
<pre>
             .
             .
             .

     file.maximum_size = 100000000
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_set_offset"><h2>PD_SET_OFFSET</h2></a>


<p>
<pre>
<i>C Binding: </i>  void PD_set_offset(PDBfile *file, int v)
<i>Fortran Binding: </i>integer PFSOFF(integer fileid, integer v)
<i>SX Binding: </i> (default-offset file v)
<i>Python Binding: </i> XXX - missing
</pre>
<p>

Set the default offset for PDB file<em> fileid</em> to <em>v</em>.  The
default offset refers to the minimum value of a dimension index.  PDBLib
allows dimensions to be specfied either in "min:max" form, in which case
the minimum value is explicit, or in "max" form, in which case the minimum
value or offset is assumed.
See the discussion in
<a href="#pdb_major">PDBLib and Index Offsets</a>
for more details.
<p>
Input to this function is:
<em>fileid</em>, an integer PDBfile identification number,
<p>
Output from this function is:
<em>v</em>, an integer value for the default offset.
<p>
Return the old value of the default offset.
<p>

<hr>
<b>C Example</b>
<pre>
       PDBfile *file;
             .
             .
             .

       PD_set_offset(file, 1);
             .
             .
             .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfsoff
       integer fileid, v, sz
             .
             .
             .

       if (pfsoff(fileid, sz) .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<b>SX Example</b>
<pre>
             .
             .
             .

    (default-offset file 0)
             .
             .
             .
</pre>
<hr>

<b>Python Example</b>
<pre>
             .
             .
             .

    file.default-offset = 0
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_family"><h2>PD_FAMILY</h2></a>


<p>
<pre>
<i>C Binding: </i>  PDBfile *PD_family(PDBfile *file, int flag)
<i>Fortran Binding: </i>integer pffami(integer fileid, integer flag)
<i>SX Binding: </i> (family-file file flag)
<i>Python Binding: </i> XXX - missing
</pre>
<p>

This function checks to see whether the specified file has exceeded
it size limit as set by <tt>PD_set_max_file_size</tt>.
If it has then a new file
is opened and returned. If not the given file pointer is returned. The
flag is set to <tt>TRUE</tt> if you want <tt>PD_family</tt> to close the file it is given.
Otherwise the application is responsible for closing the file.
<p>
In order to make a family of files the file name is constrained to follow
a form that clearly identifies the sequence of files in the family. The
name of the file in the original <a href="#pd_open">PD_open</a> must be of
the form:
<pre>
          <em>base</em>.[a-zA-Z]dd
</pre>
where
<pre>
          d = [0-9a-zA-Z]
</pre>
This is a 3 digit base 36 integer and permits upto 46656 files in a family.
<p>
The arguments to this function are:
<em>file</em>, the pointer to the PDBfile structure returned by a
 previous call to <tt>PD_open</tt>;
<em>fileid</em>, an integer file identifier returned by a previous call
 to <tt>PFOPEN</tt>;
and
<em>flag</em>, an integer value (either <tt>TRUE</tt> or <tt>FALSE</tt>).
<p>
In the C binding this function returns a pointer to a PDBfile.
In the FORTRAN binding this function returns an integer file identifier.
<p>
See also:
<a href="#pd_set_max_file_size"><tt>PD_set_max_file_size</tt></a>
.
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
    PDBfile *old, *new;
            .
            .

     PD_set_max_file_size(old, 10000000);
            .
            .
            .

     new = PD_family(old, FALSE);
     if (new != old)
        {PD_close(old);
              .
              .
              .

         old = new;};
            .
            .
            .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
    integer old, new
    integer pffami
            .
            .
            .

     new = pffami(old, 0)
     if (new .ne. old) then
        pfclos(old)
              .
              .
              .

        old = new
     endif
            .
            .
            .
</pre>
<hr>

<b>SX Example</b>
<pre>
            .
            .
            .

     (set! old (family-file old #t))
            .
            .
            .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_fix_denorm"><h2>PD_FIX_DENORM</h2></a>

<p>
<pre>
<i>C Binding: </i> int PD_fix_denorm(data_standard* std, char *type, long len, void *vr)
<i>Fortran Binding: </i>integer PFDNRM(integer istd, integer nc, character type,
                 integer len, void vr)
</pre>
<p>

This function checks to see whether the floating point values pointed to by
var are denormalized, and if so, sets all denormalized values to 0.0 (zero).
<p>
The arguments to this function are:
<em>std</em>, the pointer to a data_standard (if NULL the data standard for the
current platform will be used);
<em>istd</em>, integer id of the data_standard (if -1 the data standard for the
current platform will be used);
<em>nc</em>, an integer number of characters in the <em>type</em> string;
<em>type</em>, the type of values pointed to by <i>var</i> (either "float" or "double");
<em>len</em>, the number of values pointed to by <i>var</i>; 
and
<em>vr</em>, a pointer to the floating point values.

<p>
See <a href="#pd_target">PD_TARGET</a> for available data standards.

<pre>
NOTE: S/I/L/LL - short/int/long/longlong  byte sizes
      F/D      - float/double byte sizes
</pre>

<p>
This function returns 1 on success and 0 on failure.
<p>
See also:
<a href="#pdb_std_s">DATA_STANDARD</a>
and
<a href="#pdb_align_s">DATA_ALIGNMENT</a>
.
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
    PDBfile *file;
    float* fvalue;
    double* dvalue;

    fvalue = CMAKE_N(float, 3);
    dvalue = CMAKE_N(double, 1);
            .
            .
            .

    PD_fix_denorm(file->std, "float", 3, fvalue);
    PD_fix_denorm(NULL, "double", 1, dvalue);
            .
            .
            .

</pre>

<hr>
<b>Fortran Example</b>
<pre>
 #include "pdb.h"
       integer pfdnrm
       integer is
       doubleprecision d(100)
             .
             .
             .

       is = -1
       if (pfdnrm(is, 6, "double", 100, d) .eq. 0)
      &amp;   call errproc
             .
             .
             .

</pre>

<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_flush"><h2>PD_FLUSH</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_flush(PDBfile *file)
<i>Fortran Binding: </i>integer PFFLSH(integer fileid)
<i>SX Binding: </i> (flush-pdbfile file)
<i>Python Binding: </i> file.flush()
</pre>
<p>

This function writes out the information which describes the contents
of the PDB file specified. Normally, <tt>PD_close</tt> calls this routine, but
applications that want to protect themselves from system failures or
other problems may chose to periodically use this function. After a
successful return and until or unless more data is written to the file
or space reserved for future writes, the PDB file is valid in the sense
that if the application terminates unexpectedly before calling <tt>PD_close</tt>,
the file can be <tt>PD_open</tt>'d successfully.
<p>
NOTE: this call does NOT obviate <tt>PD_close</tt>!
<p>
The argument to this function is:
<em>file</em>, the pointer to the PDBfile structure returned by a previous
 call to <tt>PD_open</tt>;
or
<em>fileid</em>, the pointer to the PDBfile structure returned by a previous
 call to <tt>PFOPEN</tt>.
<p>
This function returns <tt>TRUE</tt> if successful and <tt>FALSE</tt> is returned and
FORTRAN programs may retrieve an error message by invoking function <tt>PFGERR</tt>.
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
    PDBfile *file;
            .
            .
            .

     if (PD_flush(file) == FALSE)
        printf("%s", PD_get_error());
            .
            .
            .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfflsh
       integer fileid
             .
             .
             .

       if (pfflsh(fileid) .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<b>SX Example</b>
<pre>
             .
             .
             .

     (flush-pdbfile file)
             .
             .
             .
</pre>
<hr>

<b>Python Example</b>
<pre>
             .
             .
             .

     file.flush()
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_close"><h2>PD_CLOSE</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_close(PDBfile *file)
<i>Fortran Binding: </i>integer PFCLOS(integer fileid)
<i>SX Binding: </i> (close-pdbfile file)
<i>Python Binding: </i> file.close()
</pre>
<p>

Close a PDB file.
<p>
After all data is written to the PDB file, the structure
chart and symbol table must be written out to the file and their disk
addresses recorded in the file header. Without these operations the
file cannot be read back in by PDBLib and all data is lost. All open
PDB files must be closed with <tt>PD_close()</tt> before exiting the program.
<p>
The arguments to <tt>PD_close()</tt> are:<BR>
<em>file</em> - a pointer to a PDBfile;<BR>
<em>fileid</em> - an integer file identifier
<p>
The function returns <tt>TRUE</tt> if the PDB file is correctly written and closed;<BR>
otherwise, <tt>FALSE</tt> is returned 
and an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
    PDBfile *file;
            .
            .
            .

     if (PD_close(file) == FALSE)
        printf("%s", PD_get_error());
            .
            .
            .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfclos
       integer fileid
             .
             .
             .

       if (pfclos(fileid) .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<b>SX Example</b>
<pre>
             .
             .
             .

     (close-pdbfile file)
             .
             .
             .
</pre>
<hr>

<b>Python Example</b>
<pre>
             .
             .
             .

     file.close()
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_remove"><h2>PD_REMOVE_ENTRY</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_remove_entry(PDBfile *file, char *name)
<i>Fortran Binding: </i>integer PFRENT(integer fileid, char name)
<i>SX Binding: </i> (remove-entry file name)
</pre>
<p>

Remove an entry from the symbol table of a PDB file.
<p>
This function only removes an entry from the symbol table of the file.
It does not do anything about the space which the entry took up in
the file.
<p>
If the unused regions of the file on disk is a problem, then the file
could be copied by opening a new file, reading each variable found in
the symbol table, and writing it to the new file.  The resulting file
would be compact.  PDBView has just such a copy command.
<p>
The arguments to <tt>PD_remove_entry()</tt> are:<BR>
<em>file</em> - a pointer to a PDBfile;<BR>
<em>fileid</em> - an integer file identifier
<em>name</em> - a string containing the name of the entry to remove
<p>
The function returns <tt>TRUE</tt> if the entry is successfully removed;<BR>
otherwise, <tt>FALSE</tt> is returned.
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
    PDBfile *file;
            .
            .
            .

     if (PD_remove_entry(file, "foo") == FALSE)
        printf("Cannot remove 'foo'\n");
            .
            .
            .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfrent
       integer fileid
             .
             .
             .

       if (pfrent(fileid, 'foo') .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<b>SX Example</b>
<pre>
             .
             .
             .

     (remove-entry file "foo")
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_writing"><h3>Writing Data to PDB Files</h3></a>


The following paragraphs describe a sequence of increasingly more
elaborate output operations for PDB files. They are all compatible
with one another so users can select the ones which match their
needs best. The most straightforward operations are first.
<p>
There are two forms for the most basic data writing operations.
These have to do with how the application wants to handle the
dimension specifications. The two functions are <tt>PD_write</tt> and
<tt>PD_write_alt</tt>.
<p>
PDBLib supports the notion of writing data of one type out into a
file as another type. More precisely, an integer type of data can
be written to a file in the format of any other integer type, and
similarly for floating point types. The application must take all
responsibility for ensuring the appropriateness of this type of
conversion (e.g. underflows and overflows). The functions which
support this are <tt>PD_write_as</tt> and <tt>PD_write_as_alt</tt>.
<p>
PDBLib allows applications to append data to existing entries. This
is handy in situations where the total amount of data is not known
in advance, but a logical ordering of the data is apparent which
matches the order in which data will be written. The functions which
do this are the <tt>PD_append</tt> family of functions.
<p>
Finally, PDBLib allows applications to reserve space on disk and then
let subsequent writes fill in that space with values. This is handy
in instances where an application knows a logical structure for a data
set but needs to write it out in smaller pieces and not necessarily
in the order implied by its logical structure. The functions which
let applications reserve space are <tt>PD_defent</tt> and 
<tt>PD_defent_alt</tt>.
Reserved spaces may be written to with any of the <tt>PD_write</tt> family
of commands.
<p>

<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_write"><h2>PD_WRITE</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_write(PDBfile *file, char *name,
                          char *type, void *var)
<i>Fortran Binding: </i>integer PFWRTA(integer fileid, integer nchr, character name,
                            integer ntype, character type, void space)
<i>SX Binding: </i> (write-pdbdata file (type dimension) data)
                    (write-pdbdata file pdbdata)
<i>Python Binding: </i> file.write(name, var, [outtype, ind])
</pre>
<p>

Write data to a PDB file.
<p>
If an entry already exists in the file,
the data overwrites the specified file data; otherwise, before
writing data to the PDB file an entry is prepared for the symbol
table consisting of the name, the type, the dimension information,
the disk address to which the data will be written, and the total
number of bytes as computed with the help of the structure chart.
After the entry is installed in the symbol table the data from
memory is converted (only if the target machine type is different
from the current machine type) and then written out to disk starting
at the current disk address.
<p>
See the section
<a href="#pdb_types">PDBLib and Data Types</a>
for legal primitive data types to use here.
Additional types may be added using <tt>PD_defstr()</tt>.
<p>
PDBLib supports arbitrary levels of indirections. This means that all
types of pointers (except function pointers) can be traced down to
the actual data to which they point and that data will be written
out into the PDB file in such a way that the read operations can
reconstruct the data as it exists prior to the write operation. There
is one crucial restriction. That is that the memory associated with
any pointer must have been allocated using a SCORE memory management
function or macro. See the memory management section near the
beginning of this document.
<p>
Rationale: When writing out scalar variables (i.e. non-dimensioned
variables - structured variables are scalars unless specifically
dimensioned) this function is the most convenient to use since it
involves no variable argument list and hence no worries about
terminating the list. Another situation, which is more common
than expected, in which <tt>PD_write()</tt> would be preferred is when it
is desirable to make entries in a PDB file which do not correspond
to any variables in the application program. Since string
manipulations might be involved in preparing the name under which
to write the data, coding in the dimensional information is not
any less efficient.
<p>
Dimensions can be given in two ways. If the default offset value for
the PDB file can be taken as the minimum value for the range which
a dimension index can legally run, the maximum value may be
specified alone. Alternatively, the minimum value, maximum value,
and stride (separated by colons) may be specified. The stride is
optional and defaults to 1. For example:
<p>
<pre>
    PD_write(file, "u[30,1:10]", "float", u);

    pfwrta(pid, 10, 'u(30,1:10)', 5, 'float', u)
</pre>
The arguments to <tt>PD_write()</tt> are:<BR>
<em>file</em> - a pointer to a PDBfile which designates the PDB file to
   which to attempt to write;<BR>
<em>fileid</em> - an integer identifier which designates the PDB file
   to which to write;<BR>
<em>nchr</em> - an integer number of characters in the <em>name</em>
   string;<BR>
<em>name</em> - an ASCII string containing the name of the variable to
   install in the symbol table;<BR>
<em>ntype</em> - an integer number of characters in the <em>type</em>
   string;<BR>
<em>type</em> - an ASCII string specifying the variable type;<BR>
<em>var</em> - a pointer to the data to be written.
This pointer must be consistent with the type specified, that is
it must be a pointer to data with type <em>type</em>. For example:
<p>
<pre>
   char **s, **t;
   int *u;

   PD_write(file, "s", "char **", &amp;s);

   PD_write(file, "t(3)", "char *", t);

   PD_write(file, "u(30,1:10)", "integer", u);
</pre>
The dimension information is encoded in the ASCII string <em>name</em>, as
if in a FORTRAN dimension statement.
<p>
Note: PDBLib can only write part of an entry if the type of the
terminal node is primitive or a structure which contains no
indirections and whose descendant members contain no indirections.
<p>
Note: When writing part of a variable, especially a structured
variable, the path to the desired part must contain one array
reference for each level of indirection traversed.
<p>
The return value is <tt>TRUE</tt> if successful;<BR>
otherwise, <tt>FALSE</tt> is returned
and an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 
<p>
pdbdata is described in the PDB section of:
<a href="pypact.html#pypact_pdb">Python Bindings</a>
.
<p>
Troubleshooting:
<a href="#pdb_rules"><font color=#dd00dd>Rules</font></a>
and
<a href="#pdb_ptrs"><font color=#dd00dd>Pointers</font></a>
<p>
See also:
<a href="#pd_write_alt"><tt>PD_write_alt</tt></a>
 and
<a href="#pd_defent"><tt>PD_defent</tt></a>
.
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
     PDBfile *file;
     float x[20];
           .
           .
           .

     if (!PD_write(file, "x(20)", "float", x))
        printf("%s", PD_get_error());
           .
           .
           .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfwrta
       integer fileid
       real x(20)
       common /abc/ a(2), b, c(2,2:4)
       real a, b, c
             .
             .
             .

 c ... write array x
       if (pfwrta(fileid, 5, `x(20)', 5, `float', x) .eq. 0)
      &amp;   call errproc
 
 c ... write entire structure abc (previous defined with pfdefs)
       if (pfwrta(fileid, 3, `abc', 3, `abc', a) .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<b>SX Example</b>
<pre>
             .
             .
             .

    (define file (open-pdbfile "foo.pdb" "w"))

; write various kinds of integer variables
    (write-pdbdata file "cn" (type "integer" 10) 1 2 3 4 5 6 7 8 9 10))
    (write-pdbdata file "an" (type "integer *") '(1 2 3 4)))
    (write-pdbdata file "en" (type "integer *" 3) '(-1 -2 -3) '(4 5 6) '(7 8 9)))
    (write-pdbdata file "sn" (type "integer **") '((1 2 3 4 5) (6 7 8 9))))

; create pdbdata objects c, a, s, and e
    (define c (write-pdbdata nil "c" (type "char" 10) "bar"))
    (define a (write-pdbdata nil "a" (type "char *") "foo"))
    (define e (write-pdbdata nil "e" (type "char *" 3) "Foo" () "Bar"))
    (define s (write-pdbdata nil "s" (type "char **") '("Hello" "world")))

; write pdbdata objects c, a, s, and e
    (write-pdbdata file "c" c)
    (write-pdbdata file "a" a)
    (write-pdbdata file "e" e)
    (write-pdbdata file "s" s)
             .
             .
             .
</pre>
<hr>

<b>Python Example</b>
<pre>
    import pact.pdb
             .
             .
             .

    file = pact.pdb.open( "/tmp/foo.pdb", "w" )

# Write various kinds of simple integers directly.
    file.write("prime",  9973, "integer")
    file.write("cn",  [1, 2, 3, 4, 5, 6, 7, 8, 9, 10], "integer")

# Create pdbdata nested integer objects an, en and sn.
    an = pact.pdb.pdbdata( [1, 2, 3, 4], "integer *" )
    en = pact.pdb.pdbdata( [[-1, -2, -3], [4, 5, 6], [7, 8, 9]], "integer *[3]" )
    sn = pact.pdb.pdbdata( [[1, 2, 3, 4, 5], [6, 7, 8, 9]], "integer **" )

# Write pdbdata nested integer objects an, en and sn.
    file.write( "an", an )
    file.write( "en", en )
    file.write( "sn", sn )

# Create pdbdata nested string objects c, a, s, and e.
    c = pact.pdb.pdbdata( "bar", "char[3]" )
    a = pact.pdb.pdbdata( "foo", "char *" )
    e = pact.pdb.pdbdata( [ "Foo", None, "Bar" ], "char *[3]" )
    s = pact.pdb.pdbdata( [ "Hello", "world" ], "char **" )

# Write pdbdata nested string objects c, a, s, and e.
    file.write("c", c)
    file.write("a", a)
    file.write("e", e)
    file.write("s", s)
             .
             .
             .
    file.close()
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_write_alt"><h2>PD_WRITE_ALT</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_write_alt(PDBfile *file, char *name, char *type,
                              void *var, int nd, long *ind)
<i>Fortran Binding: </i>integer PFWRTD(integer fileid,
                            integer nchr, character name,
                            integer ntype, character type,
                            void vr, integer nd, integer ind)
<i>SX Binding: </i> none
<i>Python Binding: </i> none
</pre>
<p>

Write data to a PDB file. This is an alternate form to <tt>PD_write</tt>. If
an entry already exists in the file, the data overwrites the
specified file data; otherwise, before writing data to the PDB
file an entry is prepared for the symbol table consisting of the
name, the type, the dimension information, the disk address to
which the data will be written, and the total number of bytes as
computed with the help of the structure chart. After the entry is
installed in the symbol table the data from memory is converted
(only if the target machine type is different from the current
machine type) and then written out to disk starting at the current
disk address.
<p>
See the section
<a href="#pdb_types">PDBLib and Data Types</a>
for legal primitive data types to use here.
Additional types may be added using <tt>PD_defstr()</tt>.
<p>
PDBLib supports arbitrary levels of indirections. This means that,
subject to the restrictions spelled out in the section on rules,
pointers (except function pointers) can be traced down to the
actual data to which they point and that data will be written
out into the PDB file in such a way that the read operations
can reconstruct the data as it exists prior to the write operation.
There is one crucial restriction. That is that the memory
associated with any pointer must have been allocated by a SCORE
memory management function or macro. See the Memory Management
section near the beginning of this document.
<p>
The rationale for this function is that in some situations, it is
desirable to be able to specify the dimensions without building
them into an ASCII string.
<p>
The arguments to <tt>PD_write_alt are</tt>:
<em>file</em>, a pointer to a PDBfile which designates the PDB file
   to which to attempt to write;
<em>fileid</em>, an integer identifier which designates the PDB file
   to which to write;
<em>nchr</em>, an integer number of characters in the <em>name</em> string;
<em>name</em>, an ASCII string containing the name of the variable
   to install in the symbol table;
<em>ntype</em>, an integer number of characters in the <em>type</em> string;
<em>type</em>, an ASCII string specifying the variable type;
<em>var</em>, a pointer to the data to be written;
<em>nd</em>, then number of dimensions for the variable;
 and
<em>ind</em>, an array of long integers containing (min, max, stride)
 triples specifying the ranges and strides of the dimensions.
The pointer, <em>var</em>, must be consistent with the type specified,
that is it must be a pointer to data with type, <em>type</em>.
For example:
<p>
<pre>
   char **s, **t;
   int *u;

   PD_write_alt(file, "s", "char **", &amp;s, ...);

   PD_write_alt(file, "t", "char *", t, ...);

   PD_write_alt(file, "u", "integer", u, ...);
</pre>
Note: PDBLib can only write part of an entry if the type of the
terminal node is primitive or a structure which contains no
indirections and whose descendant members contain no indirections.
<p>
Note: When writing part of a variable, especially a structured
variable, the path to the desired part must contain one array
reference for each level of indirection traversed.
<p>
The return value is <tt>TRUE</tt> if successful; otherwise, <tt>FALSE</tt> is returned
and an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 
<p>
Troubleshooting:
<a href="#pdb_rules"><font color=#dd00dd>Rules</font></a>
and
<a href="#pdb_ptrs"><font color=#dd00dd>Pointers</font></a>
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
     PDBfile *file;
     long ind[6];
     float x[20][5];
           .
           .
           .

     ind[0] = 0L;
     ind[1] = 19L;
     ind[2] = 1L;
     ind[3] = -2L;
     ind[4] = 2L;
     ind[5] = 1L;
     if (!PD_write_alt(file, "x", "float", x, 2, ind))
        printf("%s", PD_get_error());
           .
           .
           .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfwrtd
       integer fileid, nd, ind(6)
       real c(2,2:4)
             .
             .
             .

 c ... write array c
       nd = 2
       ind(1) = 1
       ind(2) = 2
       ind(3) = 1
       ind(4) = 2
       ind(5) = 4
       ind(6) = 1
       if (pfwrtd(fileid, 1, `c', 5, `float', c, nd, ind) .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_write_as"><h2>PD_WRITE_AS</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_write_as(PDBfile *file, char *name,
                             char *intype, char *outtype, void *var)
<i>Fortran Binding: </i>integer PFWRAS(integer fileid,
                            integer nchr, character name,
                            integer ntypin, character intype,
                            integer ntypout, character outtype, void vr)
<i>SX Binding: </i> none
<i>SX Python: </i> none
</pre>
<p>

Write the data pointed to by <em>var</em> under <em>name</em> and
with <em>intype</em> in PDB file <em>file</em> as data of
<em>outtype</em>.
<p>
The rationale for this function is that in some situations, it is
desirable to not only convert the formats of data of a specified
type, but to convert between types. An example that occurs in
practice often enough is converting a 32 bit int to a 32 bit
long on a machine which only has a 16 bit int.
<p>
Input to <tt>PD_write_as</tt> is:
<em>file</em>, a pointer to a PDBfile;
<em>fileid</em>, an integer identifier which designates the PDB file
   to which to write;
<em>nchr</em>, an integer number of characters in the name string;
<em>name</em>, an ASCII string containing the name of the
   variable in the PDB file;
<em>ntypin</em>, an integer number of characters in the
   <em>intype</em> string;
<em>intype</em>, an ASCII string containing the type of the variable;
<em>ntypout</em>, an integer number of characters in the
   <em>outtype</em> string;
<em>outtype</em>, an ASCII string containing the output type of
   the variable;
and
<em>var</em>, a pointer to the location where the data is to be
   stored in memory.
<p>
Note: PDBLib can only write part of an entry if the type of the
terminal node is primitive or a structure which contains no
indirections and whose descendant members contain no indirections.
<p>
Note: When writing part of a variable, especially a structured
variable, the path to the desired part must contain one array
reference for each level of indirection traversed.
<p>
<tt>PD_write_as</tt> returns <tt>TRUE</tt> if successful; otherwise, <tt>FALSE</tt> is returned
and an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 
<p>
Troubleshooting:
<a href="#pdb_rules"><font color=#dd00dd>Rules</font></a>
and
<a href="#pdb_ptrs"><font color=#dd00dd>Pointers</font></a>
<p>
See also:
<a href="#pd_write_as_alt"><tt>PD_write_as_alt</tt></a>
 and 
<a href="#pd_defent_alt"><tt>PD_defent_alt</tt></a>
.
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
     PDBfile *file;
     float x[20];
           .
           .
           .
     if (!PD_write_as(file, "x(20)", "float", "double", x))
        printf("%s", PD_get_error());
           .
           .
           .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfwras
       integer fileid
       real*8 x(20)
             .
             .
             .

 c ... write array x of type double as type float
       if (pfwras(fileid, 5, `x(20)', 6, `double', 
      &amp;                               5, `float', x) .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_write_as_alt"><h2>PD_WRITE_AS_ALT</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_write_as_alt(PDBfile *file,
                                 char *name, char *intype, char *outtype,
                                 void *var, int nd, long *ind)
<i>Fortran Binding: </i>integer PFWRAD(integer fileid,
                            integer nchr, character name,
                            integer ntypin, character intype,
                            integer ntypout, character outtype,
                            void vr, integer nd, integer ind)
<i>SX Binding: </i> none
<i>Python Binding: </i> none
</pre>
<p>

Write the data pointed to by <em>var</em> under <em>name</em> and
with <em>intype</em> in PDB file <em>file</em> as data of outtype.
This is an alternate form of <tt>PD_write_as</tt>.
<p>
The rationale for this function is:
<br>
&nbsp;&nbsp;&nbsp;
1) that in some situations, it is desirable to not only
convert the formats of data of a specified type, but to convert
between types. An example that occurs in practice often enough
is converting a 32 bit int to a 32 bit long on a machine which
only has a 16 bit int.
<br>
&nbsp;&nbsp;&nbsp;
2) that in some situations, it is desirable to be able to specify
the dimensions without building them into an ASCII string.
<p>
Input to <tt>PD_write_as_alt</tt> is:
<em>file</em>, a pointer to a PDBfile;
<em>fileid</em>, an integer identifier which designates the PDB file to
   which to write;
<em>nchr</em>, an integer number of characters in the name string;
<em>name</em>, an ASCII string containing the name of
   the variable in the PDB file;
<em>ntypin</em>, an integer number of characters in the intype string;
<em>intype</em>, an ASCII string containing the type
   of the variable;
<em>ntypout</em>, an integer number of characters in the outtype string;
<em>outtype</em>, an ASCII string containing the output
   type of the variable;
<em>var</em>, a pointer to the location where the data
   is to be stored in memory;
<em>nd</em>, an integer number of dimensions;
and
<em>ind</em>, an array of long integers specifying the ranges of each
   dimension (min, max, stride).
<p>
Note: PDBLib can only write part of an entry if the type of the
terminal node is primitive or a structure which contains no
indirections and whose descendant members contain no indirections.
<p>
Note: When writing part of a variable, especially a structured
variable, the path to the desired part must contain one array
reference for each level of indirection traversed.
<p>
<tt>PD_write_as_alt</tt> returns <tt>TRUE</tt> if successful; otherwise, <tt>FALSE</tt> is
returned and an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 
<p>
Troubleshooting:
<a href="#pdb_rules"><font color=#dd00dd>Rules</font></a>
and
<a href="#pdb_ptrs"><font color=#dd00dd>Pointers</font></a>
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
     PDBfile *file;
     long ind[6];
     float x[20][5];
           .
           .
           .

     ind[0] = 0L;
     ind[1] = 19L;
     ind[2] = 1L;
     ind[3] = -2L;
     ind[4] = 2L;
     ind[5] = 1L;
     if (!PD_write_as_alt(file, "x", "float", "double", x, 2, ind))
        printf("%s", PD_get_error());
           .
           .
           .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfwrad
       integer fileid, nd, ind(6)
       real*8 c(2,2:4)
             .
             .
             .

 c ... write array c of type double as type float
       nd = 2
       ind(1) = 1
       ind(2) = 2
       ind(3) = 1
       ind(4) = 2
       ind(5) = 4
       ind(6) = 1
       if (pfwrad(fileid, 1, `c', 6, `double',
      &amp;                           5, `float', c, nd, ind) .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_append"><h2>PD_APPEND</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_append(PDBfile *file, char *name, void *vr)
<i>Fortran Binding: </i>integer pfappa(integer fileid, integer nchr,
                            character name, void vr)
<i>SX Binding: </i> none
<i>Python Binding: </i> XXX - none
</pre>
<p>

Append data to an entry in the specified file. The type is taken to be the same
as for the original entry. The dimensions of the appended data are specified in
name. They must match the original entry except for the most slowly varying one.
The specification of the most slowly varying dimension must be one of the
following:<p>
<dl>
<dt>&nbsp;&nbsp;&nbsp;min:max =&gt; new dimension is old_min:old_max+(max-min+1)
<dt>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
if min is the default_offset for the file
<dt>or
<dt>&nbsp;&nbsp;&nbsp;min:max =&gt; new dimension is old_min:max
<dt>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
if min is old_max+1
<dt>
</dl>
<p>
The rationale for this function is that some data sets are of unknown size
until they are completely written. PDBLib permits any entry to reside in
discontiguous blocks of disk space. The library is responsible for reading
and writing data correctly across these blocks.<p>
The shape or dimensional information of the entry is a part of the name
string. In this respect <tt>PD_append</tt> behaves just like <tt>PD_write</tt>.<p>

Input to this function is:
 <em>file</em>, a pointer to a PDBfile,
 <em>fileid</em>, an integer identifier for a PDBfile,
 <em>nchr</em>, an integer character length for the <em>name</em> argument,
 <em>name</em>, an ASCII string containing the name of the variable and any dimensional information,
 and
 <em>vr</em>, a pointer to the data to be appended.<p>
This function returns <tt>TRUE</tt>, if successful; otherwise, <tt>FALSE</tt>
and an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 
<p>
See also:
<a href="#pd_append_alt"><tt>PD_append_alt</tt></a>,
<a href="#pd_append_as"><tt>PD_append_as</tt></a>,
<a href="#pd_append_as_alt"><tt>PD_append_as_alt</tt></a>,
<a href="#pd_defent"><tt>PD_defent</tt></a>,
<a href="#pd_defent_alt"><tt>PD_defent_alt</tt></a>,
<a href="#pd_write"><tt>PD_write</tt></a>,
<a href="#pd_write_alt"><tt>PD_write_alt</tt></a>,
<a href="#pd_write_as"><tt>PD_write_as</tt></a>,
 and
<a href="#pd_write_as_alt"><tt>PD_write_as_alt</tt></a>.
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
     PDBfile *file;
     float *fv;
           .
           .
           .

     if (PD_append(file, "x(20)", fv)) == FALSE)
        printf("%s", PD_get_error());
           .
           .
           .
</pre>
Compare this with the example of <tt>PD_write</tt>.
<p>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfappa, pfwrta
       integer fileid
       real y(10)
             .
             .
             .

 c ... append to x
       if (pfappa(fileid, 7, `x(1:10)', y) .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_append_alt"><h2>PD_APPEND_ALT</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_append_alt(PDBfile *file, char *name,
                               void *vr, int nd, long *ind)
<i>Fortran Binding: </i>integer pfappd(integer fileid,
                            integer nchr, character name,
                            void vr, integer nd, integer ind)
<i>SX Binding: </i> none
<i>Python Binding: </i> none
</pre>
<p>
Append data to an entry in the specified file. The type is taken to be
the same as for the original entry. The dimensions of the appended data
are specified in the ind array. They must match the original entry except
for the most slowly varying one. The specification of the most slowly
varying dimension must be one of the following:
<p>
<dl>
<dt>&nbsp;&nbsp;&nbsp;min:max =&gt; new dimension is old_min:old_max+(max-min+1)
<dt>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
if min is the default_offset for the file
<dt>or
<dt>&nbsp;&nbsp;&nbsp;min:max =&gt; new dimension is old_min:max
<dt>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
if min is old_max+1
<dt>
</dl>
The rationale for this function is that some data sets are of unknown
size until they are completely written. PDBLib permits any entry to
reside in discontiguous blocks of disk space. The library is responsible
for reading and writing data correctly across these blocks.
<p>
The shape or dimensional information of the entry is specified in nd
and ind. In this respect <tt>PD_append_alt</tt> behaves just like <tt>PD_write_alt</tt>.
<p>
Input to this function is:
<em>file</em>, a pointer to a PDBfile;
<em>nchr</em>, an integer number of characters in the name string;
<em>name</em>, an ASCII string containing the name of the variable;
<em>vr</em>, a pointer to the data to be appended;
<em>nd</em>, an integer number of dimensions;
and
<em>ind</em>, an array of longs with triples (start, stop, step) defining
dimension information.
<p>
This function returns <tt>TRUE</tt>, if successful; otherwise, <tt>FALSE</tt> and
an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 
For FORTRAN programs an error message may be retrieved by invoking
the function <tt>PFGERR</tt>.
<p>
See also:
<a href="#pd_append"><tt>PD_append</tt></a>,
<a href="#pd_append_as"><tt>PD_append_as</tt></a>,
<a href="#pd_append_as_alt"><tt>PD_append_as_alt</tt></a>,
<a href="#pd_defent"><tt>PD_defent</tt></a>,
<a href="#pd_defent_alt"><tt>PD_defent_alt</tt></a>,
<a href="#pd_write"><tt>PD_write</tt></a>,
<a href="#pd_write_alt"><tt>PD_write_alt</tt></a>,
<a href="#pd_write_as"><tt>PD_write_as</tt></a>,
 and
<a href="#pd_write_as_alt"><tt>PD_write_as_alt</tt></a>.
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
     PDBfile *file;
     float *fv;
     long ind[3]
           .
           .
           .

     ind[0] = 0L;
     ind[1] = 20L;
     ind[2] = 1L;
     if (PD_append_alt(file, "x", fv, 1, ind)) == FALSE)
        printf("%s", PD_get_error());
           .
           .
           .
</pre>
Compare this with the example of <tt>PD_write_alt</tt>.
<p>

<hr>
<b>Fortran Example</b>
<pre>
      integer pfappd
      integer fileid, ind(3)
      real y(20)
            .
            .
            .

c ... append to x
      ind(1) = 1
      ind(2) = 20
      ind(3) = 1
      if (pfappd(fileid, 1, `x', y, 1, ind) .eq. 0)
     &amp;   call errproc
            .
            .
            .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_append_as"><h2>PD_APPEND_AS</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_append_as(PDBfile *file
                              char *name,
                              char *intype,
                              void *vr)
<i>Fortran Binding: </i>integer PFAPAS(integer fileid,
                              integer nchr,
                              character name,
                              integer ntype,
                              character intype,
                              void vr)
<i>SX Binding: </i> none
<i>Python Binding: </i> none
</pre>
<p>

Append data to an entry in the specified file. The output type is taken
to be the same as for the original entry. The dimensions of the appended
data are specified in the name. They must match the original entry
except for the most slowly varying one. The specification of the most
slowly varying dimension must be one of the following:
<p>
<dl>
<dt>&nbsp;&nbsp;&nbsp;min:max =&gt; new dimension is old_min:old_max+(max-min+1)
<dt>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
if min is the default_offset for the file
<dt>or
<dt>&nbsp;&nbsp;&nbsp;min:max =&gt; new dimension is old_min:max
<dt>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
if min is old_max+1
<dt>
</dl>
The rationale for this function is that some data sets are of unknown
size until they are completely written. PDBLib permits any entry to
reside in discontiguous blocks of disk space. The library is responsible
for reading and writing data correctly across these blocks.
<p>
The shape or dimensional information of the entry is a part of the name
string. In this respect <tt>PD_append_as</tt> behaves just like <tt>PD_write_as</tt>.
<p>
Input to this function is:
<em>file</em>, a pointer to a PDBfile;
<em>fileid</em>, an integer identifier which designates the PDB file
 to which to write;
<em>nchr</em>, an integer number of characters in the name string;
<em>name</em>, an ASCII string containing the name of the variable
 and any dimensional information;
<em>ntype</em>, an integer number of characters in the intype string;
<em>intype</em>, an ASCII string specifying the type of the data to
 which <em>vr</em> points;
and
<em>vr</em>, a pointer to the data to be appended.
<p>
This function returns <tt>TRUE</tt>, if successful; otherwise, <tt>FALSE</tt> is returned
and an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 
For FORTRAN applications an error message can be retrieved by
invoking the function <tt>PFGERR</tt>.
<p>
See also:
<a href="#pd_append"><tt>PD_append</tt></a>,
<a href="#pd_append_alt"><tt>PD_append_alt</tt></a>,
<a href="#pd_append_as_alt"><tt>PD_append_as_alt</tt></a>,
<a href="#pd_defent"><tt>PD_defent</tt></a>,
<a href="#pd_defent_alt"><tt>PD_defent_alt</tt></a>,
<a href="#pd_write"><tt>PD_write</tt></a>,
<a href="#pd_write_alt"><tt>PD_write_alt</tt></a>,
<a href="#pd_write_as"><tt>PD_write_as</tt></a>,
 and
<a href="#pd_write_as_alt."><tt>PD_write_as_alt</tt></a>.
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
     PDBfile *file;
     double *dv;
           .
           .
           .

     if (PD_append_as(file, "x(20)", "double", dv)) == FALSE)
        printf("%s", PD_get_error());
           .
           .
           .
</pre>
Compare this with the example of <tt>PD_write_as</tt>.
<p>

<hr>
<b>Fortran Example</b>
<pre>
      integer fileid
      real*8 y(10)
      integer pfapas
            .
            .
            .

c ... append to x
      if (pfapas(fileid, 7, `x(1:10)', 6, `double', y) .eq. 0)
     &amp;   call errproc
            .
            .
            .
</pre>
Compare this with the example of <tt>PFWRAS</tt>.
<p>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_append_as_alt"><h2>PD_APPEND_AS_ALT</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_append_as_alt(PDBfile *file char *name,
                                  char *intype, void *vr,
                                  int nd, long *ind)
<i>Fortran Binding: </i>integer PFAPAD(integer fileid,
                            integer nchr, character name,
                            integer ntype, character intype,
                            void vr, integer nd, integer ind)
<i>SX Binding: </i> none
<i>Python Binding: </i> none
</pre>
<p>

Append data to an entry in the specified file. The output type is taken to
be the same as for the original entry. The dimensions of the appended data
are specified in the ind array. They must match the original entry except
for the most slowly varying one. The specification of the most slowly
varying dimension must be one of the following:
<p>
<dl>
<dt>&nbsp;&nbsp;&nbsp;min:max =&gt; new dimension is old_min:old_max+(max-min+1)
<dt>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
if min is the default_offset for the file
<dt>or
<dt>&nbsp;&nbsp;&nbsp;min:max =&gt; new dimension is old_min:max
<dt>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
if min is old_max+1
<dt>
</dl>
The rationale for this function is that some data sets are of unknown size
until they are completely written. PDBLib permits any entry to reside in
discontiguous blocks of disk space. The library is responsible for reading
and writing data correctly across these blocks.
<p>
The shape or dimensional information of the entry is specified in nd
and ind. In this respect <tt>PD_append_as_alt</tt> behaves just like <tt>PD_write_alt</tt>.
<p>
Input to this function is:
<em>file</em>, a pointer to a PDBfile;
<em>fileid</em>, an integer identifier which designates the PDBfile;
<em>nchr</em>, an integer number of characters in the <em>name</em> string;
<em>name</em>, an ASCII string containing the name of the variable;
<em>ntype</em>, an integer number of characters in the <em>intype</em> string;
<em>intype</em>, an ASCII string specifying the type of data to which <em>vr</em> points;
<em>vr</em>, a pointer to the data to be appended;
<em>nd</em>, an integer number of dimensions;
 and
<em>ind</em>, an array of longs with triples (start, stop, step) defining dimension
 information.
<p>
This function returns <tt>TRUE</tt>, if successful; otherwise, <tt>FALSE</tt> is returned
and an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 
For FORTRAN applications an error message can be retrieved by
invoking the function <tt>PFGERR</tt>.
<p>
See also:
<a href="#pd_append"><tt>PD_append</tt></a>,
<a href="#pd_append_alt"><tt>PD_append_alt</tt></a>,
<a href="#pd_append_as"><tt>PD_append_as</tt></a>,
<a href="#pd_defent"><tt>PD_defent</tt></a>,
<a href="#pd_defent_alt"><tt>PD_defent_alt</tt></a>,
<a href="#pd_write"><tt>PD_write</tt></a>,
<a href="#pd_write_alt"><tt>PD_write_alt</tt></a>,
<a href="#pd_write_as"><tt>PD_write_as</tt></a>,
 and
<a href="#pd_write_as_alt"><tt>PD_write_as_alt</tt></a>.
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
     PDBfile *file;
     float *fv;
     long ind[3]
           .
           .
           .

     ind[0] = 0L;
     ind[1] = 20L;
     ind[2] = 1L;
     if (PD_append_as_alt(file, "x", "float", fv, 1, ind)) == FALSE)
        printf("%s", PD_get_error());
           .
           .
           .
</pre>
Compare this with the example of <tt>PD_write_as_alt</tt>.
<p>

<hr>
<b>Fortran Example</b>
<pre>
      character*8 intype
      integer pfapad, pfwrtd
      integer fileid, ntype, nd, ind(3)
      real x(20) 
      real*8 y(10)
            .
            .
            .

c ... append to x
      ind(2) = 10
      if (pfapad(fileid, 1, `x', 6, `double', y, 1, ind) .eq. 0)
     &amp;     call errproc
            .
            .
            .
</pre>
Compare this with the example of <tt>PFWRAD</tt>.
<p>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_defent"><h3>Reserving Space for Future Writes</h3></a>

In some applications one knows the size and shape of a variable
in advance of having the data to write.  PDBLib permits the user to
reserve space (including 0 sized space) for a variable and then later
write all or parts of it using one or more <tt>PD_write</tt> calls.
<p>
This is different from appending to a variable which PDBLib also
supports.  See the discussion of the <tt>PD_append</tt> family of functions.
<p>

<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_defent"><h2>PD_DEFENT</h2></a>


<p>
<pre>
<i>C Binding: </i>  syment *PD_defent(PDBfile *file,
                               char *name, char *outtype)
<i>Fortran Binding: </i>integer PFDEFA(integer fileid,
                            integer nc, char *name,
                            integer nt, char *type)
<i>SX Binding: </i> none
<i>Python Binding: </i> none
</pre>
<p>

Define an entry in the symbol table of the PDB file specified by
<em>file</em>. This function reserves space on disk but writes no
data. The data can be written with later calls to <tt>PD_write</tt>,
<tt>PD_write_alt</tt>, <tt>PD_write_as</tt>, or <tt>PD_write_as_alt</tt>.
<p>
The shape or dimensional information of the entry is a part of the
name string. In this respect it behaves as <tt>PD_write</tt>.
<p>
The size of the space reserved can be zero.  What this means is
that a symbol table entry will be setup, but no space on disk
will be set aside.  In this case the application must use one of
the <tt>PD_append</tt> family of functions instead of one of the 
<tt>PD_write</tt> family.
<p>
The rationale for this function is to block out space in a PDB file
corresponding to some logical layout of a piece of data. The data
may not exist at the time the space is reserved or for some reason
it may be desirable to write out the data in pieces. In any case if
the type and shape of a variable is known at some point, an entry
may be made in the PDB file without writing any data. The space may
filled with other PDBLib calls at some later time.
<p>
Input to this function is:
<em>file</em>, a pointer to a PDBfile,
<em>fileid</em>, an integer identifier which designates the PDB file
 in which to define an entry;
<em>nchr</em>, an integer number of characters in the <em>name</em> string;
<em>name</em>, an ASCII string containing the name of the variable and
 any dimensional information,
and
<em>outtype</em>, an ASCII string specifying the type of data in the file.
<p>
In the C binding this function returns a symbol table entry (<tt>syment</tt>) pointer, if
successful; otherwise, NULL is returned 
and an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 
In the FORTRAN binding the return value is 1, if successful; otherwise,
0 is returned and an error message may be retrieved by invoking
function <tt>PFGERR</tt>.
<p>
See also:
<a href="#pd_defent_alt"><tt>PD_defent_alt</tt></a>,
<a href="#pd_write"><tt>PD_write</tt></a>,
<a href="#pd_write_alt"><tt>PD_write_alt</tt></a>,
<a href="#pd_write_as"><tt>PD_write_as</tt></a>,
 and
<a href="#pd_write_as_alt"><tt>PD_write_as_alt</tt></a>.
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
     PDBfile *file;
     syment *ep;
           .
           .
           .

     if ((ep = PD_defent(file, "x(20)", "float")) == NULL)
        printf("%s", PD_get_error());
           .
           .
           .
</pre>

Compare this with the example of <tt>PD_write</tt>.
<p>

<hr>
<b>Fortran Example</b>
<pre>

       integer pfdefa
       integer fileid
             .
             .
             .

 c ... define and reserve array x
 c ... declaration for x would be: real x(20)
 
       if (pfdefa(fileid, 5, `x(20)', 5, `float') .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>

Compare this with the example of <tt>PFWRTA</tt>.
<p>

<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_defent_alt"><h2>PD_DEFENT_ALT</h2></a>

	
<p>
<pre>
<i>C Binding: </i>  syment *PD_defent_alt(PDBfile *file, char *name,
                                   char *outtype, int nd, long *ind)
<i>Fortran Binding: </i>integer PFDEFD(integer fileid,
                            integer nc, char *name,
                            integer nt, char *outtype,
                            integer nd, integer ind)
<i>SX Binding: </i> none
<i>Python Binding: </i> none
</pre>
<p>

Define an entry in the symbol table of the PDB file specified by <em>file</em>.
This function reserves space on disk but writes no data. The data can
be written with later calls to <tt>PD_write</tt>, <tt>PD_write_alt</tt>,
<tt>PD_write_as</tt>, or <tt>PD_write_as_alt</tt>.
<p>
This is an alternate form of <tt>PD_defent</tt>. The difference is that the
dimension information is supplied via the nd and ind arguments
instead of being a part of the name string. In this respect it
behaves as <tt>PD_write_alt</tt> does.
<p>
The size of the space reserved can be zero.  What this means is
that a symbol table entry will be setup, but no space on disk
will be set aside.  In this case the application must use one of
the <tt>PD_append</tt> family of functions instead of one of the <tt>PD_write</tt>
family.
<p>
The rationale for this function is to block out space in a PDB file
corresponding to some logical layout of a piece of data. The data
may not exist at the time the space is reserved or for some reason
it may be desirable to write out the data in pieces. In any case if
the type and shape of a variable is known at some point, an entry
may be made in the PDB file without writing any data. The space may
filled with other PDBLib calls at some later time.
<p>
Input to this function is:
<em>file</em>, a pointer to a PDBfile;
<em>fileid</em>, an integer identifier which designates the PDB file in
 which to define an entry;
<em>nchr</em>, an integer number of characters in the <em>name</em> string;
<em>name</em>, an ASCII string containing the name of the variable only;
<em>ntype</em>, an integer number of characters in the <em>outtype</em> string;
<em>outtype</em>, an ASCII string specifying the type of data in the file;
<em>nd</em>, an integer specifying the number of dimensions;
 and
<em>ind</em>, an array of long integers containing the minimum and maximum
 values of the index for each dimension pairwise.
<p>
In the C binding this function returns a symbol table entry (<tt>syment</tt>) pointer,
if successful; otherwise, NULL is returned 
and an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 
In the FORTRAN binding the return value is 1, if successful; otherwise,
0 is returned and an error message may be retrieved by invoking
function <tt>PFGERR</tt>.
<p>
<p>
See also:
<a href="#pd_defent"><tt>PD_defent</tt></a>,
<a href="#pd_write"><tt>PD_write</tt></a>,
<a href="#pd_write_alt"><tt>PD_write_alt</tt></a>,
<a href="#pd_write_as"><tt>PD_write_as</tt></a>,
 and
<a href="#pd_write_as_alt"><tt>PD_write_as_alt</tt></a>.
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
     PDBfile *file;
     syment *ep;
     long ind[4];
           .
           .
           .

     ind[0] = 0L;
     ind[1] = 19L;
     ind[2] = -2L;
     ind[3] = 2L;
     if ((ep = PD_defent_alt(file, "x", "float", 2, ind)) == NULL)
        printf("%s", PD_get_error());
           .
           .
           .
</pre>
Compare this with the example of <tt>PD_write_alt</tt> and note the absence of
a two dimensional array in this call.
<p>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfdefd
       integer fileid, nd, ind(4)
             .
             .
             .

 c ... define and reserve array c
 c ... declaration for c would be: real c(2,2:4)
       nd = 2
       ind(1) = 1
       ind(2) = 2
       ind(3) = 2
       ind(4) = 4
       if (pfdefd(fileid, 1, `c', 5, `float', nd, ind) .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
Compare with the example for <tt>PFWRTD</tt>.
<p>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_reading"><h3>Reading Data from PDB Files</h3></a>


Since data in a file has a definite size and shape, the reading
operations in PDBLib are somewhat simpler than the writing
operation. The <tt>PD_read</tt> family of operations which is the counterpart
of the <tt>PD_write</tt> family does all of the work in reading data from
files.
<p>

<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_read"><h2>PD_READ</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_read(PDBfile *file, char *name, void *var)
<i>Fortran Binding: </i>integer PFREAD(integer fileid,
                            integer nchr, character name, void var)
<i>SX Binding: </i> (read-pdbdata file name)
<i>Python Binding: </i> file.read(name)
</pre>
<p>

Read all or part of a data entry from an open PDB file.
<p>
The symbol
table of the given PDB file is searched for the given name and if
it is found the information there is used to read the proper number
of bytes from the file, do any conversions, and put the result in
memory pointed to by <em>var</em>.
<p>
The arguments to <tt>PD_read</tt> are:<BR>
<em>file</em> - a pointer to a PDBfile which designates the PDB file
   from which to attempt the read;<BR>
<em>fileid</em> - an integer identifier which designates the PDB
   file from which to attempt the read;<BR>
<em>nchr</em> - an integer number of characters in the <em>name</em>
   string;<BR>
<em>name</em> - an ASCII string containing the specification of data
   to be read;<BR>
<em>var</em> - a pointer to the location where the data is to be
   placed.
<p>
Note: In each <tt>PD_read</tt> operation, the type of <em>var</em> must be a pointer
to the type of the variable <em>name</em>.
<p>
Note: PDBLib can only read part of an entry if the type of the
terminal node is primitive or a structure which contains no
indirections and whose descendant members contain no indirections.
<p>
Note: When reading part of a variable, especially a structured
variable, the path to the desired part must contain one array
reference for each level of indirection traversed.
<p>
The return value is the number of items successfully read.<BR>
An ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a> if 0 items were
read.
<p>
Troubleshooting:
<a href="#pdb_rules"><font color=#dd00dd>Rules</font></a>
and
<a href="#pdb_ptrs"><font color=#dd00dd>Pointers</font></a>
<p>
<p>
pdbdata is described in the PDB section of:
<a href="pypact.html#pypact_pdb">Python Bindings</a>
.
<p>
See also:
<a href="#pd_read_alt"><tt>PD_read_alt</tt></a>,
<a href="#pd_read_as"><tt>PD_read_as</tt></a>,
 and
<a href="#pd_read_as_alt"><tt>PD_read_as_alt</tt></a>.
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
     PDBfile *file;
     float x[20];
           .
           .
           .
     if (PD_read(file, "x", x) == 0)
        printf("%s", PD_get_error());
           .
           .
           .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfread
       integer fileid
       real x(20)
       common /abc/ a(2), b, c(2,2:4)
       real a, b, c
       common /jkl/ j, k, l
       integer j, k, l
             .
             .
             .

 c ... read array x
       if (pfread(fileid, 1, 'x', x) .eq. 0)
      &amp;   call errproc
 
 c ... read first element of member c of structure abc
       if (pfread(fileid, 10, 'abc.c(1,2)', c) .eq. 0)
      &amp;   call errproc
 
 c ... read entire structure jkl
       if (pfread(fileid, 3, 'jkl', j) .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<b>SX Example</b>
<pre>
             .
             .
             .

    (define file (open-pdbfile "foo.pdb"))
    (define c1 (read-pdbdata file "c"))
    (define a1 (read-pdbdata file "a"))
    (define e1 (read-pdbdata file "e"))
    (define s1 (read-pdbdata file "s"))
             .
             .
             .
</pre>
<hr>

<b>Python Example</b>
<pre>
    import pact.pdb
             .
             .
             .

    file = pact.pdb.open("foo.pdb")
    c1 = file.read("c")
    a1 = file.read("a")
    e1 = file.read("e")
    s1 = file.read("s")
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_read_alt"><h2>PD_READ_ALT</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_read_alt(PDBfile *file, char *name,
                             void *var, long *ind)
<i>Fortran Binding: </i>integer PFPTRD(integer fileid,
                            integer nchr, character name,
                            void var, integer ind)
<i>SX Binding: </i> none
<i>Python Binding: </i> none
</pre>
<p>

Read all or part of a data entry from an open PDB file. The symbol
table of the given PDB file is searched for the given name and if
it is found the information there is used to read the proper number
of bytes from the file, do any conversions, and put the result in
memory pointed to by <em>var</em>.
<p>
The arguments to <tt>PD_read_alt</tt> are:
<em>file</em>, a pointer to a PDBfile which designates the PDB file from
   which to attempt the read;
<em>fileid</em>, an integer identifier which designates the PDB file from
   which to attempt the read;
<em>nchr</em>, an integer number of characters in the <em>name</em> string;
<em>name</em>, an ASCII string containing the specification of the data to
   be read;
<em>var</em>, a pointer to the location where the data is to be placed;
 and
<em>ind</em>, an array of long integers consisting of three indexes
   (start, stop, and step) for each dimension of the entry.
<p>
Note: In each <tt>PD_read_alt</tt> operation, the type of var must be a
pointer to the type of the variable name.
<p>
Note: PDBLib can only read part of an entry if the type of the
terminal node is primitive or a structure which contains no
indirections and whose descendant members contain no indirections.
<p>
Note: When reading part of a variable, especially a structured
variable, the path to the desired part must contain one array
reference for each level of indirection traversed.
<p>
The return value is the number of items successfully read. 
An ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a> if 0 items were
read.
<p>
Troubleshooting:
<a href="#pdb_rules"><font color=#dd00dd>Rules</font></a>
and
<a href="#pdb_ptrs"><font color=#dd00dd>Pointers</font></a>
<p>
See also:
<a href="#pd_read"><tt>PD_read</tt></a>,
<a href="#pd_read_as"><tt>PD_read_as</tt></a>,
 and
<a href="#pd_read_as_alt"><tt>PD_read_as_alt</tt></a>.
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
     PDBfile *file;
     long ind[3];
     float x[20];
           .
           .
           .

     ind[0] = 3;
     ind[1] = 18;
     ind[2] = 2;
     if (PD_read_alt(file, "x", x, ind) == FALSE)
        printf("%s", PD_get_error());
           .
           .
           .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfptrd
       integer fileid, ind(3)
       real xodd(10)
             .
             .
             .

 c ... read the first 10 odd elements of x into array xodd
       ind(1) = 1
       ind(2) = 20
       ind(3) = 2
       if (pfptrd(fileid, 1, 'x', xodd, ind) .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_read_as"><h2>PD_READ_AS</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_read_as(PDBfile *file,
                            char *name, char *intype, void *var)
<i>Fortran Binding: </i>integer PFRDAS(integer fileid,
                            integer nchr, character name,
                            integer ntype, character intype, void var)
<i>SX Binding: </i> none.
<i>Python Binding: </i> none, but see example.
</pre>
<p>

Read data from an open PDB file.
<p>
The symbol table of the given PDB
file is searched for the given name.  If the name is found, the
information there is used with the type specified by type to
read the proper number of bytes from the file, do any conversions,
and put the result in memory pointed to by var.  If it is not found, a zero
is returned and the error message will indicate "MISSING ENTRY".
The type specified
overrides the type in the symbol table entry as far as deciding on
data conversions goes.
<p>
This function is generally used to read floats as doubles and so on.
However with sufficient care and understanding of both the file data
and C data structuring, it can be used to transmute structured data.
<p>
The arguments to <tt>PD_read_as</tt> are:<BR>
<em>file</em> - a pointer to a PDBfile which designates the PDB file from
   which to attempt the read;<BR>
<em>fileid</em> - an integer identifier which designates the PDB file from
   which to attempt the read;<BR>
<em>nchr</em> - an integer number of characters in the <em>name</em> string;<BR>
<em>name</em> - an ASCII string containing the specification for the data
   to be read;<BR>
<em>ntype</em> - an integer number of characters in the <em>intype</em> string;<BR>
<em>intype</em> - an ASCII string containing the type of the data
   desired;<BR>
<em>var</em> - a pointer to the location where the data is to be placed.
<p>
Note: In each <tt>PD_read_as</tt> operation, the type of <em>var</em> must be a pointer
to the type specified by <em>type</em>.
<p>
Note: PDBLib can only read part of an entry if the type of the
terminal node is primitive or a structure which contains no
indirections and whose descendant members contain no indirections.
<p>
Note: When reading part of a variable, especially a structured
variable, the path to the desired part must contain one array
reference for each level of indirection traversed.
<p>
The return value is the number of items successfully read.<BR>
An ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a> if 0 items were
read.
<p>
Troubleshooting:
<a href="#pdb_rules"><font color=#dd00dd>Rules</font></a>
and
<a href="#pdb_ptrs"><font color=#dd00dd>Pointers</font></a>
<p>
See also:
<a href="#pd_read"><tt>PD_read</tt></a>,
<a href="#pd_read_alt"><tt>PD_read_alt</tt></a>,
 and
<a href="#pd_read_as_alt"><tt>PD_read_as_alt</tt></a>.
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
     PDBfile *file;
     float x[20];
           
           .
           .

 /* x is a double in the file */
     if (PD_read_as(file, "x", "float", x) == FALSE)
        printf("%s", PD_get_error());
           .
           .
           .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfrdas
       integer fileid
       real*8  xx(20)
       integer ierror
       character*128 errmsg
             .
             .
             .

 c     Read array x into array xx as type double.
       if (pfrdas(fileid, 1, 'x', 6, 'double', xx) .eq. 0) then
           ierror = pfgerr( 128, errmsg )
           print *, errmsg
           call exit ( -1 )
       endif
             .
             .
             .
</pre>
<hr>

<b>Python Example</b>
<pre>
# There is no Python binding for PD_read_as() 
# but it can be faked by using the Python conversion methods
# int(), long(), float() and complex(). 

    import pact.pdb
             .
             .
             .

    file = pact.pdb.open( "foo.pdb", "r" )

# Read the variables as is.
    int1     = file.read( "int0" )
    long1    = file.read( "long0" )
    float1   = file.read( "float0" )
    double1  = file.read( "double0" )

# Coerce them into floats.
    x_int    = float( int1 )
    x_long   = float( long1 )
    x_float  = float( float1 )
    x_double = float( double1 )
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_read_as_alt"><h2>PD_READ_AS_ALT</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_read_as_alt(PDBfile *file,
                                char *name, char *type,
                                void *var, long *ind)
<i>Fortran Binding: </i>integer PFRDAD(integer fileid,
                            integer nchr, character name,
                            integer ntype, character intype,
                            void var, integer ind)
<i>SX Binding: </i> none
<i>Python Binding: </i> none
</pre>
<p>

Read all or part of a data entry from an open PDB file. The symbol
table of the given PDB file is searched for the given name and if
it is found the information there is used with the type specified
by type to read the proper number of bytes from the file, do any
conversions, and put the result in memory pointed to by var. The
type specified overrides the type in the symbol table entry as far
as deciding on data conversions goes.
<p>
This function is generally used to read floats as doubles and so on.
However with sufficient care and understanding of both the file data
and C data structuring, it can be used to transmute structured data.
<p>
The arguments to <tt>PD_read_as_alt</tt> are:
<em>file</em>, a pointer to a PDBfile which designates the PDB file
   from which to attempt the read;
<em>fileid</em>, an integer identifier which designates the PDB file
   from which to attempt the read;
<em>nchr</em>, an integer number of characters in the <em>name</em>
   string;
<em>name</em>, an ASCII string containing the specification of the
   data to be read;
<em>type</em>, an ASCII string containing the type of the data
   desired;
<em>var</em>, a pointer to the location where the data is to be
   placed;
and
<em>ind</em>, an array of long integers consisting of three indexes
   (start, stop, and step) for each dimension of the entry.
<p>
Note: In each <tt>PD_read_as_alt</tt> operation, the type of <em>var</em> must
be a pointer to the type specified by type.
<p>
Note: PDBLib can only read part of an entry if the type of the
terminal node is primitive or a structure which contains no
indirections and whose descendant members contain no indirections.
<p>
Note: When reading part of a variable, especially a structured
variable, the path to the desired part must contain one array
reference for each level of indirection traversed.
<p>
The return value is the number of items successfully read. 
An ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a> if 0 items were
read.
<p>
Troubleshooting:
<a href="#pdb_rules"><font color=#dd00dd>Rules</font></a>
and
<a href="#pdb_ptrs"><font color=#dd00dd>Pointers</font></a>
<p>
See also:
<a href="#pd_read"><tt>PD_read</tt></a>,
<a href="#pd_read_as"><tt>PD_read_as</tt></a>,
 and
<a href="#pd_read_alt"><tt>PD_read_alt</tt></a>.
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
     PDBfile *file;
     long ind[3];
     float x[20];
           .
           .
           .

 /* x is a double in the file */
     ind[0] = 2;
     ind[1] = 10;
     ind[2] = 2;
     if (PD_read_as_alt(file, "x", "float", x, ind) == FALSE)
        printf("%s", PD_get_error());
           .
           .
           .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfrdad
       integer fileid, ind(3)
       real*8 xdd(10)
             .
             .
             .

 c ... read the first 10 elements of float x into array xdd 
 c ... as type double
       ind(1) = 1
       ind(2) = 10
       ind(3) = 1
       if (pfrdad(fileid, 1, 'x', 6, `double', xdd, ind) .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_deftyp"><h3>Defining New Data Types</h3></a>


To aid application developers in using structured data PDBLib lets
applications define new data types in a PDBfile. New data types may
either be derived from other existing data types or they may be
primitive (integer types, floating point types, or byte sequences).
To define derived types applications should use <tt>PD_defstr</tt> or
<tt>PD_defstr_alt.</tt> To define primitive types use <tt>PD_defix</tt>,
<tt>PD_defloat</tt>, or <tt>PD_defncv</tt>.
<p>
Since PDBLib supports pointered data types, it is often the case that
a pointer in a derived type may point to data of any kind. In C, casts
are used to control this behavior. PDBLib permits a member of a struct
which is of type "char *" specify the actual type of another pointered
member. The function <tt>PD_cast</tt> is used to set up this behavior.
<p>
<b>WARNING:</b> with the addition of alignment controls in C11,
<b>_Alignas</b>, <b>alignof</b>, and <b>aligned_alloc</b>, users have to
be aware of data alignment issues.  In principal, one can use these
features of C11 with PDBLib.  BUT, in practice you should NOT use them.
The problem stems from the fact that when PDBLib is built, it computes
the alignment information it needs with the default state of the compiler.
If your C11 code defines structs using the new controls it may lead to
conflicting specifications and a malfunctioning code.
<p>

<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_cast"><h2>PD_CAST</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_cast(PDBfile *file, char *type,
                         char *memb, char *contr)
<i>Fortran Binding: </i>none
<i>SX Binding: </i> (make-cast file type memb contr)
<i>Python Binding: </i> XXX - missing
</pre>
<p>

Dynamically change the type of a structure member. PDBLib supports an
 extended data typing mechanism called a structure. A structure is a set
 of declarations of members. Each member is in turn a data type known to
 the system. In some applications, a structure member is used to point to
 data of a type which is specified by another member. In the C coding a
 cast is used to obtain a pointer to the desired data type.
<p>
PDBLib supports this same practice by allowing the programmer to override
 the type of a member as given in the structure definition
 (see <a href="#pd_defstr"><tt>PD_defstr</tt></a>)
 by supplying the name of a member, whose type must be "char *", which
 will contain an ASCII string specifying the actual type of the data to
 which the first member points.
<p>
The arguments to <tt>PD_cast</tt> are:
<em>file</em>, a pointer to a PDBfile;
<em>type</em>, an ASCII string containing the name of the data structure
               type in the PDB file;
<em>memb</em>, an ASCII string containing the name of the member whose type
               is to be overridden;
 and
<em>contr</em>, an ASCII string containing the name of the member (whose
                type must be "char *") which will provide the actual type for memb.
<p>
The return value is a <tt>TRUE</tt> if the cast is successful; otherwise, <tt>FALSE</tt> is returned 
and an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 

<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
    PDBfile *file;
    struct sample
       {char *type;
        int *a;};
          .
          .
          .

     PD_defstr(file, "sample", "char *type", "int *a", LAST);
     PD_cast(file, "sample", "a", "type");
          .
          .
          .
</pre>
<hr>

<b>SX Example</b>
<pre>
          .
          .
          .

    (make-cast file "sample" "char *type" "int *a")
          .
          .
          .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_defix"><h2>PD_DEFIX</h2></a>


<p>
<pre>
<i>C Binding: </i>  defstr *PD_defix(PDBfile *file, char *name,
                              long bpi, int align, int flg)
<i>Fortran Binding: </i>none
<i>SX Binding: </i> none
<i>Python Binding: </i> none
</pre>
<p>

Define a primitive integral type (fixed point type) in the PDB file
specified by <em>file</em>.
<p>
Input to <tt>PD_defix</tt> is:
<em>file</em>, a pointer to a PDBfile;
<em>name</em>, an ASCII string containing the name of the new data type;
<em>bpi</em>, the number of bytes required for 1 item of the new type;
<em>align</em>, the byte alignment for the type;
 and
<em>flg</em>, a flag indicating whether the byte ordering of the type is
 normal or reverse ordered.
<p>
PDBLib supplies two #define'd constants which define the two ordering
schemes used for fixed point types: <tt>NORMAL_ORDER</tt> and <tt>REVERSE_ORDER</tt>.
<tt>NORMAL_ORDER</tt> means that the byte ordering from lowest to highest
address as occurs on most CPU's. <tt>REVERSE_ORDER</tt> means that the byte
order goes from highest to lowest address as happens with INTEL and
other CPU's.
<p>
Compare this information with that found in the discussion of data
conversion later in this manual.
<p>
A pointer to the new type's <tt>defstr</tt> is returned if the call is
successful; otherwise, NULL is returned 
and an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
    PDBfile *file;
    defstr *ptr;
           .
           .
           .

     ptr = PD_defix(file, "int40", 5, 1, NORMAL_ORDER);
           .
           .
           .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_defloat"><h2>PD_DEFLOAT</h2></a>


<p>
<pre>
<i>C Binding: </i>  defstr *PD_defloat(PDBfile *file, char *name,
                                long bpi, int align, int *ordr,
                                long expb, long mantb, long sbs, long sbe,
                                long sbm, long hmb, long bias)
<i>Fortran Binding: </i>none
<i>SX Binding: </i> none
<i>Python Binding: </i> none
</pre>
<p>

Define a new floating point type to the PDB file specified by <em>file</em>.<p>
Input to <tt>PD_defloat</tt> is:
<em>file</em>, a pointer to a PDBfile;
<em>name</em>, an ASCII string containing the name of the variable
 in the PDB file;
<em>bpi</em>, the number of bytes required for an item of the new type;
<em>align</em>, the byte alignment for this type;
<em>ordr</em>, an array of bpi integers specifying the byte order;
<em>expb</em>, the number of exponent bits;
<em>mantb</em>, the number of mantissa bits;
<em>sbs</em>, the position of the sign bit;
<em>sbe</em>, the starting bit of the exponent;
<em>sbm</em>, the starting bit of the mantissa;
<em>hmb</em>, the value of the high order mantissa bit;
 and
<em>bias</em>, the bias of the exponent.
<p>
Compare this information with that found in the discussion of data
conversion later in this manual.
<p>
A pointer to the new type's <tt>defstr</tt> is returned if the call is
successful; otherwise, NULL is returned 
and an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
 int ord_int24[] = {1, 3, 2};
 
    PDBfile *file;
    defstr *ptr;
           .
           .
           .

     ptr = PD_defloat(file, "fp24", 3, 1, ord_int24,
                      7L, 16L, 0L, 1L, 8L, 0L, 0x3F)
           .
           .
           .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_defncv"><h2>PD_DEFNCV</h2></a>


<p>
<pre>
<i>C Binding: </i>  defstr *PD_defncv(PDBfile *file, char *name,
                               long bpi, int align)
<i>Fortran Binding: </i>none
<i>SX Binding: </i> none
<i>Python Binding: </i> none
</pre>
<p>

Define a primitive type that will not undergo format conversion from platform to platform in the PDB file specified by <em>file</em>. Certain data types commonly defined in C programs are used as flags or character holders. With such data types the actual bit pattern contains the meaningful information. This information would be lost under a data conversion operation. This function provides users with a means to define primitive types which will not be converted under any circumstances and therefore preserve the meaningful bit patterns which constitute the intended data.<p>
Input to <tt>PD_defncv</tt> is:
<em>file</em>, a pointer to a PDBfile;
<em>name</em>, an ASCII string containing the name of the new data type;
<em>bpi</em>, the number of bytes required for 1 item of the new type;
 and
<em>align</em>, the byte alignment for the type.
<p>
Compare this information with that found in the discussion of data
conversion later in this manual.
<p>
A pointer to the new type's <tt>defstr</tt> is returned if the call is
successful; otherwise, NULL is returned 
and an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
    PDBfile *file;
    defstr *ptr;
           .
           .
           .
 /* define a type like the FORTRAN character*8 */
     ptr = PD_defncv(file, "char_8", 8, 1);
           .
           .
           .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_defstr"><h2>PD_DEFSTR</h2></a>


<p>
<pre>
<i>C Binding: </i>  defstr *PD_defstr(PDBfile *file,
                               char *name,
                               char *mem1, ..., memn,
                               int *LAST)
<i>Fortran Binding: </i>integer PFDEFS(integer fileid,
                            integer nchr,
                            character name,
                            integer ncm1, character mem1,
                                  ...,
                            integer ncmn, character memn,
                            integer LAST)
<i>SX Binding: </i> (make-defstr name mem1 ... memn)
             (make-defstr* name mem1 ... memn)
<i>Python Binding: </i> pact.pdb.defstr(name, (mem1, ..., memn), file=vif)
                 file.defstr(name, (mem1, ..., memn))
</pre>
<p>

Define a data structure for a PDB file. As a matter of programming
efficiency and clarity it is useful to be able to refer to more
complex structural units than the primitive types: short integers,
integers, long integers, floating point numbers, double precision
floating point numbers, and characters. Arrays do this in a very
simple-minded way. Many modern languages support extended types
or structures which allow the programmer to group diverse types
of data together in a very sophisticated way.
<p>
PDBLib supports an extended data typing mechanism called a structure.
A structure is a set of declarations of members. Each member is in
turn a data type known to the system. Much of the style and usage
of structures comes from the C struct. Note: for FORTRAN versions
which do not define any kind of pointer (e.g. ANSI FORTRAN 77)
structures defined with this function should not contain pointered
members. Because of the memory management features upon which PDBLib
now depends, even members whose types are pointers are allowed.
The only restrictions on member types are that they not be function
pointers and that they be expressible without parentheses. Again
any member which is a pointer must have its memory allocated by
a SCORE memory management function or macro. See the Memory
Management section near the beginning of this manual.
<p>
<tt>PD_defstr</tt> defines structures to the PDB system so that they can be
read and written as a whole in a single statement. The members of
the structure are processed and an entry in the structure chart is
made. Then subsequent references to the new structure type are
processed using information from the structure chart. The syntax
by which members of a structure are specified is like that for C
structs. The formal definition is given below ([ ] enclose optional
elements). Self-referential structures are allowed providing the
reference is through pointers (like C). The actual type name is
used in the reference since PDBLib checks that all member types
are already known or are the type being defined.
<p>

&nbsp;&nbsp;&nbsp;
&#60;;member&gt;
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
:=
&#60;;type&gt; [*...*]&#60;;member name&gt;[(&#60;;dimensions&gt;)]
<br>
&nbsp;&nbsp;&nbsp;
&#60;;type&gt;
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
:=
&#60;;primitive type&gt; | &#60;;derived type&gt;
<br>
&nbsp;&nbsp;&nbsp;
&#60;;member name&gt;
&nbsp;&nbsp;
:=
an ASCII string representing the name of the member
<br>
&nbsp;&nbsp;&nbsp;
&#60;;primitive type&gt;
:=
short | integer | long | float | double | char
<br>
&nbsp;&nbsp;&nbsp;
&#60;;derived type&gt;
&nbsp;
:=
any <tt>PD_defstr</tt>'d type
<br>
&nbsp;&nbsp;&nbsp;
&#60;;dimensions&gt;
&nbsp;&nbsp;&nbsp;
:=
&#60;;integer&gt; |
<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
&#60;;integer : integer&gt; |
<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
&#60;;integer&gt;, &#60;;dimensions&gt; |
<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
&#60;;integer : integer&gt; &#60;;dimensions&gt;
<br>
<p>

Dimensions can be given in two ways. If the default offset value for
the PDB file can be taken as the minimum value for the range which
a dimension index can legally run, the maximum value may be specified
alone. Alternatively, the minimum value followed by a colon and the
maximum value may be specified. For example,
<p>
&nbsp;&nbsp;&nbsp;
integer a(30,1:10)
<p>
The arguments to <tt>PD_defstr</tt> are:
<em>file</em>, a pointer to a PDBfile;
<em>fileid</em>, an integer identifier which designates the PDB file to which
 to write;
<em>nchr</em>, the integer number of characters in the <em>name</em> string;
<em>name</em>, an ASCII string containing the name of the data structure
 type in the PDB file;
 and
<em>memi</em>, a list of ASCII strings each representing the declaration of
 a member of a structure are defined above.
<tt>LAST</tt> must terminate the list of members.
<p>
In the C binding the return value is a pointer to the entry made in
the structure chart if the call is successful; otherwise, NULL is
returned and an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 
In the FORTRAN binding the return value is 1,
if successful; otherwise, 0 is returned and an error message may
be retrieved by invoking function <tt>PFGERR</tt>.
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
    PDBfile *file;
    defstr *ptr;
    struct sample
       {float x[20];
        float y[20];
        int number;};
           .
           .
           .

     ptr = PD_defstr(file, "sample", "float x(20)", "float y(20)",
                           "int number", LAST);
           .
           .
           .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       parameter(LAST = 0)
       integer pfdefs
       integer fileid
       common /abc/ j, a(2), b, c(2,2:4)
       real a, b, c
       integer j
             .
             .
             .

       if (pfdefs(fileid,
      &amp;           3,  'abc',
      &amp;           9,  'integer j',
      &amp;           10, 'float a(2)',
      &amp;           7,  'float b',
      &amp;           14, 'float c(2,2:4)',
      &amp;           LAST) .eq. 0)
      &amp;     call errproc
             .
             .
             .
</pre>
<hr>

<b>SX Example</b>
<pre>
             .
             .
             .

    (make-defstr* file "abc" (def-member integer i)
                             (def-member char c 10) 
                             (def-member integer *i1)
                             (def-member char *a)
                             (def-member char **s)
                             (def-member float f))
             .
             .
             .
</pre>
<hr>

<b>Python Example</b>
<pre>
             .
             .
             .


    file.make_defstr("abc", ['integer i',
                             'char c(10)',
                             'integer *i1',
                             'char *a',
                             'char **s',
                             'float f])
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_defstr_alt"><h2>PD_DEFSTR_ALT</h2></a>


<p>
<pre>
<i>C Binding: </i>  defstr *PD_defstr_alt(PDBfile *file,
                                   char *name,
                                   int nmemb,
                                   char **members)
<i>Fortran Binding: </i>integer PFDEFT(integer fileid,
                            integer nchr,
                            character name,
                            integer nmemb,
                            integer nc(2*nm),
                            character *memb)
<i>SX Binding: </i> none
<i>Python Binding: </i> none
</pre>
<p>

Define a data structure for a PDB file. As a matter of programming efficiency
and clarity it is useful to be able to refer to more complex structural
units than the primitive types: short integers, integers, long integers,
floating point numbers, double precision floating point numbers, and
characters. Arrays do this in a very simple-minded way. Many modern
languages support extended types or structures which allow the programmer
to group diverse types of data together in a very sophisticated way.
<p>
PDBLib supports an extended data typing mechanism called a structure.
A structure is a set of declarations of members. Each member is in
turn a data type known to the system. Much of the style and usage of
structures comes from the C struct. Because of the memory management
features upon which PDBLib now depends, even members whose types are
pointers are allowed. The only restrictions on member types are that
they not be function pointers and that they be expressible without
parentheses. Again any member which is a pointer must have its memory
allocated by a SCORE memory management function or macro. See the
Memory Management section near the beginning of this manual.
<p>
<tt>PD_defstr</tt> defines structures to the PDB system so that they can be read
and written as a whole in a single statement. The members of the
structure are processed and an entry in the structure chart is made.
Then subsequent references to the new structure type are processed
using information from the structure chart. The syntax by which members
of a structure are specified is like that for C structs. The formal
definition is given below ([ ] enclose optional elements).
Self-referential structures are allowed providing the reference
is through pointers (like C). The actual type name is used in the
reference since PDBLib checks that all member types are already
known or are the type being defined.
<p>

&nbsp;&nbsp;&nbsp;
&#60;;member&gt;
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
:=
&#60;;type&gt; [*...*]&#60;;member name&gt;[(&#60;;dimensions&gt;)]
<br>
&nbsp;&nbsp;&nbsp;
&#60;;type&gt;
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
:=
&#60;;primitive type&gt; | &#60;;derived type&gt;
<br>
&nbsp;&nbsp;&nbsp;
vvv&#60;;member name&gt;
&nbsp;&nbsp;
:=
an ASCII string representing the name of the member
<br>
&nbsp;&nbsp;&nbsp;
&#60;;primitive type&gt;
:=
short | integer | long | float | double | char
<br>
&nbsp;&nbsp;&nbsp;
&#60;;derived type&gt;
&nbsp;
:=
any <tt>PD_defstr</tt>'d type
<br>
&nbsp;&nbsp;&nbsp;
&#60;;dimensions&gt;
&nbsp;&nbsp;&nbsp;
:=
&#60;;integer&gt; |
<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
&#60;;integer : integer&gt; |
<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
&#60;;integer&gt;, &#60;;dimensions&gt; |
<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
&#60;;integer : integer&gt; &#60;;dimensions&gt;
<br>
<p>

Dimensions can be given in two ways. If the default offset value for the
PDB file can be taken as the minimum value for the range which a
dimension index can legally run, the maximum value may be specified
alone. Alternatively, the minimum value followed by a colon and the
maximum value may be specified. For example,
<p>
&nbsp;&nbsp;&nbsp;
integer a(30,1:10)
<p>
The arguments to <tt>PD_defstr</tt> are:
<em>file</em>, a pointer to a PDBfile;
<em>fileid</em>, an integer identifier which designates the PDB file to which
 to write;
<em>nchr</em>, the integer number of characters in the <em>name</em> string;
<em>name</em>, an ASCII string containing the name of the data structure type
 in the PDB file;
<em>nmemb</em>, an integer number of strings in the members array;
<em>nc</em>, an array of integer pairs specifying the 0 based offset
 into the <em>memb</em> array and the number of characters for each member;
<em>members</em>, an array of ASCII strings each representing the declaration
 of a member of a structure are defined above;
 and
<em>memb</em>, an array of characters containing the member specifications.
<p>
In the C binding the return value is a pointer to the entry made in
the structure chart if the call is successful; otherwise, NULL is
returned and an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 
In the FORTRAN binding the return value is 1,
if successful; otherwise, 0 is returned and an error message may
be retrieved by invoking function <tt>PFGERR</tt>.
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
    PDBfile *file;
    defstr *ptr;
    char **members;
    struct sample
       {float x[20];
        float y[20];
        int number;};
           .
           .
           .

     members = CMAKE_N(char *, 3);
     members[0] = CSTRSAVE("float x[20]");
     members[1] = CSTRSAVE("float y[20]");
     members[2] = CSTRSAVE("integer number");
     ptr = PD_defstr_alt(file, "sample", 3, members);
 
     CFREE(members[0]);
     CFREE(members[1]);
     CFREE(members[3]);
     CFREE(members);
           .
           .
           .
</pre>
Compare with the example for <tt>PD_defstr</tt>.
<p>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfdeft
       integer fileid, nc(6)
       character*8 nm(4)
       common /abc/ a(2), b, c(2,2:4)
             .
             .
             .

       nc(1) = 0
       nc(2) = 10
       nc(3) = 10
       nc(4) = 7
       nc(5) = 17
       nc(6) = 14
       nm(1) = `float a('
       nm(2) = `2)float '
       nm(3) = `bfloat c'
       nm(4) = `(2,2:4) '
       if (pfdeft(fileid, 3, `abc', 3, nc, nm) .eq. 0)
      &amp;    call errproc
             .
             .
             .
</pre>
Compare with the example for <tt>PFDEFS</tt>.
<p>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_typedef"><h2>PD_TYPEDEF</h2></a>


<p>
<pre>
<i>C Binding: </i>  defstr *PD_typedef(PDBfile *file, char *oname, char *tname)
<i>Fortran Binding: </i>none
<i>SX Binding: </i> (make-typedef file oname tname)
<i>Python Binding: </i> XXX - missing
</pre>
<p>

Define a an alternate name for an existing type. The intended use of
this function is to allow users to make their PDB data types match
their C types as closely as possible. It does this by mimicking the
C typedef mechanism in a limited way. More accurately it provides
an aliasing capability. This can be used in conjunction with either
<tt>PD_defix</tt> or <tt>PD_defloat</tt> to install a definition of a data type in
the host chart (<tt>PD_defix</tt> and <tt>PD_defloat</tt> define their types to the
file chart only).
<p>
Input to <tt>PD_typedef</tt> is:
<em>file</em>, a pointer to a PDBfile;
<em>oname</em>, an ASCII string containing the name of the original
   data type;
<em>tname</em>, an ASCII string containing the name of the alias.
<p>
A pointer to the original type's <tt>defstr</tt> is returned if the call is
successful; otherwise, NULL is returned 
and an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
    PDBfile *file;
    defstr *ptr;
           .
           .
           .

 /* define "enum" as an alias for "integer" */
     ptr = PD_typedef(file, "integer", "enum");
           .
           .
           .
</pre>
<hr>

<b>SX Example</b>
<pre>
           .
           .
           .

    (make-typedef file "integer" "enum")
           .
           .
           .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_defattr"><h3>Defining Attributes</h3></a>


PDBLib supports a general mechanism for managing a class of data
which is variously referred to as attributes or meta data. In a
great many cases, the careful design of data structures obviates
the need for this kind of data. Nevertheless, PDBLib supplies four
functions to manage attributive data: <tt>PD_def_attribute</tt>,
<tt>PD_rem_attribute</tt>, <tt>PD_get_attribute</tt>, <tt>PD_set_attribute</tt>.
<p>

<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_def_attribute"><h2>PD_DEF_ATTRIBUTE</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_def_attribute(PDBfile *file, char *attr, char *type)
<i>Fortran Binding: </i>integer PFDATT(integer fileid,
                            integer na, char *attr,
                            integer nt, char *type)
<i>SX Binding: </i> (def-attribute file name type)
<i>Python Binding: </i> XXX - missing
</pre>
<p>

Define an attribute to the given PDB file. The model of an attribute
in PDBLib is an entity that has a name and type. The two supported
operations on attributes are to create them and to remove them. An
entity in a PDB file can be assigned an attribute value simply by
making a call which specifies the entity name, the attribute name,
and the attribute value (which is determined by the type). The only
association between an entry in a PDB file and any attribute is made
by the name in the attribute table and the entry in the symbol table.
In particular, this mechanism allows the application developer to
define and use entities in a PDB file solely in terms of attributes.
<p>
The arguments to this function are:
<em>file</em>, a pointer to a PDBfile;
<em>na</em>, an integer number of characters in the attr string;
<em>attr</em>, an ASCII string containing the name of the attribute being defined;
<em>nt</em>, an integer number of characters in the type string;
and
<em>type</em>, an ASCII string containing the name of the data type in the PDB file.
<p>
The return value is <tt>TRUE</tt>, if successful; otherwise, <tt>FALSE</tt> is returned
and an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 
FORTRAN programs may retrieve an error message by invoking the function <tt>PFGERR</tt>.
<p>
See also:
<a href="#pd_rem_attribute"><tt>PD_rem_attribute</tt></a>,
<a href="#pd_set_attribute"><tt>PD_set_attribute</tt></a>,
 and
<a href="#pd_get_attribute"><tt>PD_get_attribute</tt></a>.
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
    PDBfile *file;
            .
            .
            .

     if (PD_def_attribute(file, "date", "char *") == FALSE)
        printf("%s", PD_get_error());
            .
            .
            .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfdatt
       integer fileid
             .
             .
             .

       if (pfdatt(fileid, 4, `date', 6, `char *') .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<b>SX Example</b>
<pre>
             .
             .
             .

    (def-attribute file "date" "char *")
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_get_attribute"><h2>PD_GET_ATTRIBUTE</h2></a>


<p>
<pre>
<i>C Binding: </i>  void *PD_get_attribute(PDBfile *file,
                                    char *vr, char *attr)
<i>Fortran Binding: </i>integer PFGVAT(integer fileid,
                            integer nv, char *vr,
                            integer na char *attr,
                            void value)
<i>SX Binding: </i> (get-attribute-value file vr attr)
<i>Python Binding: </i> XXX - missing
</pre>
<p>

Return the value of the specified attribute for the named entity.
<p>
The model of an attribute in PDBLib is an entity that has a name and
type. The two supported operations on attributes are to create them
and to remove them. An entity in a PDB file can be assigned an
attribute value simply by calling <tt>PD_set_attribute</tt>. The only
association between an entry in a PDB file and any attribute
is made by the name in the attribute table and the entry in
the symbol table. In particular, this mechanism allows the
application developer to define and use entities in a PDB file
solely in terms of attributes.
<p>
Attribute values are always assigned by reference and
<tt>PD_get_attribute</tt> returns them the same way. The application may have
to make a cast on the returned pointer.
<p>
The arguments to this function are:
<em>file</em>, a pointer to a PDBfile;
<em>fileid</em>, an integer identifier which designates the PDB file from
 which to get the attribute;
<em>nv</em>, an integer number of characters in the var string;
<em>vr</em>, an ASCII string containing the name of an
 entity in the PDB file;
<em>na</em>, an integer number of characters in the attr string;
<em>attr</em>, an ASCII string containing the name of the attribute being sought;
 and
<em>value</em>, into which the attribute value will be copied.
<p>
The return value is a pointer to the value of the attribute if
one exists; otherwise, NULL is returned 
and an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 
<p>
See also:
<a href="#pd_def_attribute"><tt>PD_def_attribute</tt></a>,
<a href="#pd_rem_attribute"><tt>PD_rem_attribute</tt></a>,
 and
<a href="#pd_set_attribute"><tt>PD_set_attribute</tt></a>.
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
    PDBfile *file;
    char *dt;
            .
            .
            .

     dt = (char *) PD_get_attribute(file, "foo", "date");
     if (dt == NULL)
        printf("%s", PD_get_error());
            .
            .
            .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfgvat
       integer fileid
       character*8 dt(10)
             .
             .
             .

       if (pfgvat(fileid, 3, `foo', 4, `date', dt) .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<b>SX Example</b>
<pre>
            .
            .
            .

    (define dt (get-attribute-value file "foo" "date"))
            .
            .
            .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_rem_attribute"><h2>PD_REM_ATTRIBUTE</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_rem_attribute(PDBfile *file, char *attr)
<i>Fortran Binding: </i>integer PFRATT(integer fileid, integer na, char *attr)
<i>SX Binding: </i> (rem-attribute file attr)
<i>Python Binding: </i> XXX - missing
</pre>
<p>

Remove the specified attribute. Any entities which have a value for
this attribute will have it removed by PDBLib.
<p>
The model of an attribute in PDBLib is an entity that has a name
and type. The two supported operations on attributes are to create
them and to remove them. An entity in a PDB file can be assigned an
attribute value simply by calling <tt>PD_set_attribute</tt>. The only
association between an entry in a PDB file and any attribute is
made by the name in the attribute table and the entry in the symbol
table. In particular, this mechanism allows the application
developer to define and use entities in a PDB file solely in terms
of attributes.
<p>
The arguments to this function are:
<em>file</em>, a pointer to a PDBfile;
<em>fileid</em>, an integer identifier which designates the PDB file
   from which to remove the attribute;
<em>na</em>, an integer number of characters in the attr string;
and
<em>attr</em>, an ASCII string containing the name of the attribute
   being removed.
<p>
The return value is <tt>TRUE</tt> if successful; otherwise, <tt>FALSE</tt> is returned 
and an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 
<p>
See also:
<a href="#pd_def_attribute"><tt>PD_def_attribute</tt></a>,
<a href="#pd_get_attribute"><tt>PD_get_attribute</tt></a>,
 and
<a href="#pd_set_attribute"><tt>PD_set_attribute</tt></a>.
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
    PDBfile *file;
            .
            .
            .

     if (PD_rem_attribute(file, "date") == FALSE)
        printf("%s", PD_get_error());
            .
            .
            .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfratt
       integer fileid
             .
             .
             .

       if (pfratt(fileid, 4, `date') .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<b>SX Example</b>
<pre>
            .
            .
            .

    (rem-attribute file "date")
            .
            .
            .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_set_attribute"><h2>PD_SET_ATTRIBUTE</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_set_attribute(PDBfile *file,
                                  char *vr, char *attr, void *vl)
<i>Fortran Binding: </i>integer PFSVAT(integer fileid,
                            integer nv, char *vr,
                            integer na char *attr, void vl)
<i>SX Binding: </i> (set-attribute-value! file vr attr vl)
<i>Python Binding: </i> XXX - missing
</pre>
<p>

Set the value of the specified attribute for the named entity.
Attribute values are always assigned by reference.
<p>
The model of an attribute in PDBLib is an entity that has a name
and type. The two supported operations on attributes are to create
them and to remove them. An entity in a PDB file can be assigned an
attribute value simply by calling <tt>PD_set_attribute</tt>. The only
association between an entry in a PDB file and any attribute is
made by the name in the attribute table and the entry in the symbol
table. In particular, this mechanism allows the application
developer to define and use entities in a PDB file solely in terms
of attributes.
<p>
The arguments to this function are:
<em>file</em>, a pointer to a PDBfile;
<em>fileid</em>, an integer identifier which designates the PDB file
   in which the attribute is being set;
<em>nv</em>, an integer number of characters in the var string;
<em>vr</em>, an ASCII string containing the name of an entity in the
   PDB file;
<em>na</em>, an integer number of characters in the attr string;
<em>attr</em>, an ASCII string containing the name of the attribute
   being set;
and
<em>vl</em>, a pointer to data whose type matches the attribute type.
<p>
The return value is <tt>TRUE</tt> if successful; otherwise, <tt>FALSE</tt> is returned 
and an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 
<p>
See also:
<a href="#pd_def_attribute"><tt>PD_def_attribute</tt></a>,
<a href="#pd_rem_attribute"><tt>PD_rem_attribute</tt></a>,
 and
<a href="#pd_get_attribute"><tt>PD_get_attribute</tt></a>.
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
    PDBfile *file;
    char *dt;
            .
            .
            .

     dt = CSTRSAVE("Mon March 23, 1921");
     if (PD_set_attribute(file, "foo", "date", dt) == FALSE)
        printf("%s", PD_get_error());
            .
            .
            .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfsvat
       integer scftcs
       integer fileid
       character*80 dt
             .
             .
             .

       scftcs(dt, `Mon March 27, 1921', 18)
       if (pfsvat(fileid, 3, `foo', 4, `date', dt) .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<b>SX Example</b>
<pre>
             .
             .
             .

       (set-attribute-value! file "foo" "date" "Mon 3/27/21")
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_queries"><h3>Queries</h3></a>


<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PD_get_buffer_size"><h2>PD_GET_BUFFER_SIZE</h2></a>


<p>
<pre>
<i>C Binding: </i>  int64_t PD_get_buffer_size(void)
<i>Fortran Binding: </i>integer PFGBFS()
<i>SX Binding: </i> (pd-get-buffer-size)
<i>Python Binding: </i> XXX - missing
</pre>
<p>

Get the current buffer size which PDBLib uses for all PDB files and
return it.
<p>
This function has no arguments.
<p>
Return the current buffer size in bytes, if previously set;
otherwise, return -1.
<p>

<hr>
<b>C Example</b>
<pre>
       int64_t sz;
             .
             .
             .

       sz = PD_get_buffer_size;
             .
             .
             .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfgbfs
       integer nb
             .
             .
             .

       nb = pfgbfs()
       if (nb .eq. -1)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<b>SX Example</b>
<pre>
             .
             .
             .

       (define sz (pd-get-buffer-size))
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_get_error"><h2>PD_GET_ERROR</h2></a>


<p>
<pre>
<i>C Binding: </i>  char *PD_get_error(void)
<i>Fortran Binding: </i>integer PFGERR(integer nchr, character err)
<i>SX Binding: </i> (pd-error-message)
<i>Python Binding: </i> XXX - missing
</pre>
<p>

Get the current PDBLib error message. Return the length of the error
message in <em>nchr</em> and the actual message in <em>err</em>. The
space for <em>err</em> should be 255 characters long.
<p>
The arguments to this function are:
<em>nchr</em>, the length of the error message;
and
<em>err</em>, an ASCII string into which the message is copied.
<p>
In the C binding this function returns a pointer to the error message.
In the FORTRAN binding this function returns 1 if successful, 0 otherwise.
<p>

<hr>
<b>C Example</b>
<pre>
       char *err;
             .
             .
             .

       err = PD_get_error();
             .
             .
             .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfgerr
       integer ierr
       character*128 err
             .
             .
             .

       ierr = pfgerr(128, err)
             .
             .
             .
</pre>
<hr>

<b>SX Example</b>
<pre>
             .
             .
             .

       (define err (pd-error-message))
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_get_file_length"><h2>PD_GET_FILE_LENGTH</h2></a>


<p>
<pre>
<i>C Binding: </i>  int64_t PD_get_file_length(PDBfile *file)
<i>Fortran Binding: </i>integer PFGFLN(integer fileid,
                            integer len)
<i>SX Binding: </i> 
<i>Python Binding: </i> XXX - missing
</pre>
<p>

Return the length of the specified PDB file.
<p>
The arguments to <tt>PFGFLN</tt> are:
<em>file</em>, PDBfile designating the desired file;
<em>len</em>, the integer byte length of the PDB file in question.
<p>
In the C binding returns the byte length of the file.
In the FORTRAN binding return value is 1, if successful; otherwise, 0 is
returned and an error message may be retrieved by invoking function <tt>PFGERR</tt>.
<p>

<hr>
<b>C Example</b>
<pre>
       PDBfile *file;
       int64_t ln;
             .
             .
             .

       ln = PD_get_file_length(file);
             .
             .
             .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfgfln
       integer fileid, ln
             .
             .
             .

       if (pfgfln(fileid, ln) .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<b>SX Example</b>
<pre>
             .
             .
             .

        none currently
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_get_file_name"><h2>PD_GET_FILE_NAME</h2></a>


<p>
<pre>
<i>C Binding: </i>  char *PD_get_file_name(PDBfile *file)
<i>Fortran Binding: </i>integer PFGFNM(integer fileid,
                            integer nchr, character name)
<i>SX Binding: </i> (file-info file "name")
<i>Python Binding: </i> file.name
</pre>
<p>

Return the name of the specified PDB file. The nchr argument contains
the length of the buffer on input and the length of the file name
on output. If the buffer is not long enough, the length of the file
name is returned in nchr and a value of 0 is returned.
<p>
The arguments to <tt>PFGFNM</tt> are:
<em>file</em>, PDBfile designating the desired file;
<em>fileid</em>, an integer identifier which designates the PDB file
   in question;
<em>nchr</em>, on input an integer number of characters in the <em>name</em>
   string, on output the length of the actual file name string;
<em>name</em>, an ASCII string which will contain the file name upon
   successful completion.
<p>
In the C binding returns the name of the file.
In the FORTRAN binding return value is 1, if successful; otherwise, 0 is
returned and an error message may be retrieved by invoking function <tt>PFGERR</tt>.
<p>

<hr>
<b>C Example</b>
<pre>
       PDBfile *file;
       char *nm;
             .
             .
             .

       nm = PD_get_file_name(file);
             .
             .
             .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfgfnm
       integer fileid, nc
       character*32 name
             .
             .
             .

       nc = len(name)
       if (pfgfnm(fileid, nc, name) .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<b>SX Example</b>
<pre>
             .
             .
             .

    (define name (file-info file "name"))
             .
             .
             .
</pre>
<hr>

<b>Python Example</b>
<pre>
             .
             .
             .

    name = file.name
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_get_major_order"><h2>PD_GET_MAJOR_ORDER</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_get_major_order(PDBfile *file)
<i>Fortran Binding: </i>integer PFGMJO(integer fileid, integer val)
<i>SX Binding: </i> (file-info file "major-order")
<i>Python Binding: </i> file.major_order
</pre>
<p>

Get the major order for PDB file<em> fileid</em> to <em>v</em>.  The
major order refers to the order of data in multidimensional arrays.
See the discussion of this topic in the section
<a href="#pdb_major">PDBLib and Index Ordering</a>.
<p>
Input to this function is:
<em>fileid</em>, an integer PDBfile identification number.
<p>
Output from this function is:
<em>val</em>, an integer major order.
<p>
In the FORTRAN API <tt>TRUE</tt> is returned if successful and <tt>FALSE</tt>
otherwise.
<p>

<hr>
<b>C Example</b>
<pre>
       PDBfile *file;
       int mjo;
             .
             .
             .

       mjo = PD_get_major_order(file);
             .
             .
             .
</pre>
<hr>

<b>Fortran Example</b>
<pre>
       integer pfgmjo
       integer mjo, fileid
             .
             .
             .

       mjo = pfgmjo(fileid)
             .
             .
             .
</pre>
<hr>

<b>SX Example</b>
<pre>
             .
             .
             .

    (define mjo (file-info file "major-order"))
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_get_max_file_size"><h2>PD_GET_MAX_FILE_SIZE</h2></a>


<p>
<pre>
<i>C Binding: </i>  int64_t PD_get_max_file_size(PDBfile *file)
<i>Fortran Binding: </i>integer PFGMXS(integer fileid)
<i>SX Binding: </i> (file-info file "maximum-size")
<i>Python Binding: </i> XXX - missing
</pre>
<p>

Get the current maximum file size of PDB file <em>fileid</em> in bytes.
<p>
The arguments to this function are:
<em>fileid</em>, an integer PDBfile identification number.
<p>
Return the integer value of the current maximum file size.

<hr>
<b>C Example</b>
<pre>
       PDBfile *file;
             .
             .
             .

       sz = PD_get_max_file_size(file);
             .
             .
             .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfgmxs
       integer sz, fileid
             .
             .
             .

       sz = pfgmxs(fileid)
             .
             .
             .
</pre>
<hr>

<b>SX Example</b>
<pre>
             .
             .
             .

    (define size (file-info file "maximum-size"))
             .
             .
             .
</pre>
<hr>

<b>Python Example</b>
<pre>
             .
             .
             .

    size = file.maximum_size
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_get_mode"><h2>PD_GET_MODE</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_get_mode(PDBfile *file)
<i>Fortran Binding: </i>integer PFGMOD(integer fileid)
<i>SX Binding: </i> (file-info file "mode")
<i>Python Binding: </i> file.mode
</pre>
<p>

Get the current mode of PDB file <em>fileid</em>: 7 (append - 'a'),
0 (open - 'r'), 1 (create - 'w').
<p>

The arguments to this function are:
<em>fileid</em>, an integer PDBfile identification number.
<p>
Return the integer value of the current mode.
<p>

<hr>
<b>C Example</b>
<pre>
       PDBfile *file;
       int md;
             .
             .
             .

       md = PD_get_mode(file);
             .
             .
             .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfgmod
       integer md, fileid
             .
             .
             .

       md = pfgmod(fileid)
             .
             .
             .
</pre>
<hr>

<b>SX Example</b>
<pre>
             .
             .
             .

    (define mode (file-info file "mode"))
             .
             .
             .
</pre>
<hr>

<b>Python Example</b>
<pre>
             .
             .
             .

    mode = file.mode
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_get_offset"><h2>PD_GET_OFFSET</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_get_offset(PDBfile *file)
<i>Fortran Binding: </i>integer PFGOFF(integer fileid)
<i>SX Binding: </i> (file-info file "default-offset")
<i>Python Binding: </i> file.default_offset
</pre>
<p>

Get the current default offset for PDB file <em>fileid</em>.
See the discussion in
<a href="#pdb_major">PDBLib and Index Offsets</a>
for more details.
<p>
The arguments to this function are:
<em>fileid</em>, an integer PDBfile identification number.
<p>
Return the integer value of the default offset.
<p>

<hr>
<b>C Example</b>
<pre>
       PDBfile *file;
       int off;
             .
             .
             .

       off = PD_get_offset(file);
             .
             .
             .
</pre>
<hr>

<b>Fortran Example</b>
<pre>
       integer pfgoff
       integer off, fileid
             .
             .
             .

       off = pfgoff(fileid)
             .
             .
             .
</pre>
<hr>

<b>SX Example</b>
<pre>
             .
             .
             .

    (define offs (file-info file "default-offset"))
             .
             .
             .
</pre>
<hr>

<b>Python Example</b>
<pre>
             .
             .
             .

    offs = file.default_offset
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PD_query_entry"><h2>PD_query_entry</h2></a>


<p>
<pre>
<i>C Binding: </i>  syment *PD_query_entry(PDBfile *file, char *name, char *fullname)
<i>Fortran Binding: </i>none
<i>SX Binding: </i> (read-syment file name)
<i>Python Binding: </i> XXX - 
</pre>
<p>
Inquire about variable <em>name</em> in the specified PDB file. This
lookup can be done with an absolute path or relative to the current directory
(in the specified PDB file).
<p>
Input to this function is:<BR>
<em>file</em> - a pointer to a PDBfile;<BR>
<em>name</em> - an ASCII string containing the path to the variable
(absolute or relative to the current directory);<BR>
<em>fullname</em> - a character string which will hold the full path to the
variable upon output (use NULL if not desired).<BR>
<p>
Output from this function is:<BR>
In the C binding, return a pointer to a <tt>syment</tt> (a symbol table entry structure)
if successful;<BR>
otherwise, NULL is returned 
and an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>.<BR>
<em>fullname</em> will contain the full path to this variable.

<hr>
<b>C Example</b>
<pre>
       PDBfile  *file;                      /* PDB file pointer */
       char     *path;                      /* the name of the variable */
       char     fullpath[MAXLINE];          /* full path to the variable */
       syment   *ep;                        /* symbol table entry pointer */
       long     size;                       /* no. of elements in variable */
       dimdes   *dims;                      /* array of dims; (max,min) pairs */
       char     *type;                      /* variable type in ASCII */
       long     addr;                       /* address of variable */
             .
             .
             .

       path = "abc";
       ep = PD_query_entry(file, path, fullpath);
       if (ep == NULL)
           printf("%s", PD_get_error());
       else
          {printf( "fullpath (output) = %s\n", fullpath );
           size = PD_entry_number(ep);
           dims = PD_entry_dimensions(ep);
           type = PD_entry_type(ep);
           addr = PD_entry_address(ep);}
             .
             .
             .  
       ep = PD_query_entry(file, "./xyz", fullpath);
       ep = PD_query_entry(file, "/top_dir/docs/xyz_doc", NULL);
</pre>

<hr>
<b>SX Example</b>
<pre>
             .
             .
             .

     (define ep (read-syment file "abc"))

; el will contain (type dimensions number address)
     (define el (pdb->list ep))
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PD_get_entry_info"><h2>PD_get_entry_info</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_get_entry_info(syment *ep, char **type, long *size,
                                   int *ndims, long **dims)
<i>Fortran Binding: </i>integer PFIVAR(integer fileid,
                            integer nchr, character name,
                            integer ntype, character type,
                            integer size, integer ndims, integer dims)
<i>SX Binding: </i> none
<i>Python Binding: </i> none
</pre>
<p>
Extract the descriptive information about a variable.  In the C binding,
a symbol table entry (possibly obtained by 
<a href="#PD_query_entry"><tt>PD_query_entry</tt></a>
) is the input.
However, in the FORTRAN binding the <em>name</em> is used.
<p>
The C binding deals in dynamically allocated spaces which must be freed
by a call to 
<a href="#PD_free_entry_info"><tt>PD_free_entry_info</tt></a>.
<p>
Input to this function is:<BR>
<em>ep</em> - a symbol table entry;<BR>
<em>fileid</em> - an integer PDBfile identification number;<BR>
<em>nchr</em> - the number of characters in <em>name</em>;<BR>
<em>name</em> - an ASCII string containing the name of the variable.
<p>
The function returns 1 if successful, 0 otherwise
and an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 
<p>
Output from this function is:<BR>
<em>ntype</em> - the number of characters in <em>type</em>;<BR>
<em>type</em> - an ASCII string containing the type of the variable;<BR>
<em>size</em> - the number of elements in the variable;<BR>
<em>ndims</em> - the number of dimensions which the variable has;<BR>
<em>dims</em> - an array of the dimensions given as (min, max) pairs.
<p>

<hr>
<b>C Example</b>
<pre>
       PDBfile  *file;
       char     path[MAXLINE];
       syment   *ep;
       char     *type;
       long     ni;
       int      nd;
       long     *dims;
       int      rv;
       int      ii;
             .
             .
             .

       ep = PD_query_entry(file, "abc", path);
       rv = PD_get_entry_info(ep, &amp;type, &amp;ni, &amp;nd, &amp;dims);
       if (!rv)
          printf("%s", PD_get_error());
       else 
          {printf("Variable type:  %s\n", type);
           printf("Number of elements:  %ld\n", ni);
           printf("Number of dimensions:  %d\n", nd);
           for ( ii = 0; ii < nd; ii++ )
               printf("    Dimension:  %ld:%ld\n", dims[2*ii], dims[2*ii+1]);
             .
             .
             .

           PD_free_entry_info(type, dims);}

</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfivar
       integer fileid, sz, al, ind
             .
             .
             .

       if (pfivar(fileid, 3, `abc', 5 `float',
      &amp;           sz, nd, dims) .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PD_free_entry_info"><h2>PD_free_entry_info</h2></a>


<p>
<pre>
<i>C Binding: </i>  void PD_free_entry_info(char *type, long *dims)
<i>Fortran Binding: </i> XXX - missing
<i>SX Binding: </i> XXX - missing
<i>Python Binding: </i> XXX - missing
</pre>
<p>
Deallocate the space pointed to by <em>type</em> and <em>dims</em>, 
previously allocated by a successful call to 
<a href="#PD_get_entry_info"><tt>PD_get_entry_info</tt></a>.
<p>
See <a href="#PD_get_entry_info"><tt>PD_get_entry_info</tt></a> for examples.
<p>

<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_inquire_type"><h2>PD_INQUIRE_TYPE</h2></a>


<p>
<pre>
<i>C Binding: </i>  defstr *PD_inquire_type(PDBfile *file, char *type)
              defstr *PD_inquire_host_type(PDBfile *file, char *type)
<i>Fortran Binding: </i>integer PFITYP(integer fileid,
                            integer ntype, character type,
                            void size, integer align, integer ind)
<i>SX Binding: </i> (read-defstr file type)
<i>Python Binding: </i> XXX - missing
</pre>
<p>

Inquire about type <em>type</em> in the specified PDB file.
<tt>PD_inquire_type</tt> and <tt>PFITYP</tt>
do the lookup in the file chart in order to give information about
data in the file rather than the host chart which would give information about
the data type in the hardware on which the application is running. Instead
<tt>PD_inquire_host_type</tt> does the lookup in the host chart to give information
about the data type in the hardware on which the application is running.
<p>
The input for this function is:
<em>file</em>, a PDBfile;
<em>fileid</em>, an integer PDBfile identification number;
<em>ntype</em>, the number of characters in <em>type</em>;
<em>type</em>, an ASCII string containing the name of the type.
<p>
In the C binding return a pointer to a <tt>defstr</tt> which contains the description
of the data type.
In the FORTRAN binding return 1 if successful, 0 otherwise.
<p>
In the FORTRAN binding output for this function is:
<em>size</em>, the number of bytes necessary to represent the type;
<em>align</em>, the alignment of <em>type</em> in bytes;
<em>ind</em>, the number of members which are pointers if
   <em>type</em> is a derived type.
<p>

<hr>
<b>C Example</b>
<pre>
       PDBfile *file;
       long sz;
       int al, ind;
       memdes *md;
             .
             .
             .

       dp = PD_inquire_type(file, "float");
       if (dp != NULL)
          {sz  = dp->size;
           al  = dp->alignment;
           ind = dp->n_indirects;
           md  = dp->members;};
             .
             .
             .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfityp
       integer fileid, sz, al, ind
             .
             .
             .

       if (pfityp(fileid, 5, `float', sz, al, ind) .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<b>SX Example</b>
<pre>
             .
             .
             .

     (define dp (read-defstr file "float"))

; dl will contain (type size members)
     (define dl (pdb->list dp))
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pfimbr"><h2>PFIMBR</h2></a>


<p>
<pre>
<i>C Binding: </i>  none
<i>Fortran Binding: </i>integer PFIMBR(integer fileid, integer ntype, char *type,
                            integer n, integer size, char *space)
<i>SX Binding: </i> none
</pre>
<p>

Inquire about the nth member of the type <em>type</em> in PDB
file <em>fileid</em>. Return the description of the member in the
character buffer, space. On input size is the number of characters
in the buffer space. If the member description requires more space,
size is set to the number of bytes required and 0 is returned.
<p>
Input to this function is:
<em>fileid</em>, an integer PDBfile identification number;
<em>ntype</em>, the number of characters in <em>type</em>;
<em>type</em>, an ASCII string containing the name of the type;
<em>n</em>, an integer specifying the member;
<em>size</em>, an integer character size of the space buffer;
   and
<em>space</em>, an character buffer to hold the member description.
<p>

Output from this function is:
<em>size</em>, the number of characters in the member description;
   and
<em>space</em>, the member description.
<p>
The return value is 1, if successful; otherwise, 0 is returned and
an error message may be retrieved by invoking function <tt>PFGERR</tt>.
<p>
See also:
<a href="#pd_defstr"><tt>PD_defstr</tt></a>
 and
<a href="#pd_defstr_alt"><tt>PD_defstr_alt</tt></a>
.
<p>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfimbr
       integer fileid
       character*8 type(10), desc(10), bdesc(100)
             .
             .
             .

       size = 80
       if (pfimbr(fileid, 3, `foo', 4, size, desc) .eq. 0) then
          if (size .le. 800) then
             if (pfimbr(fileid, 3, `foo', 4, size, bdesc) .eq. 0) then
      &amp;         call errproc
             endif
          endif
       endif
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_ptrs"><h3>Using Pointers</h3></a>


The ability to use pointers with PDBLib is very powerful.  It is
unfortunate perhaps that it can also be a little bit tricky. Application
developers should be aware of a key issue when using pointers with
PDBLib. In general, when you want to write a data tree, you want to
faithfully reproduce the connectivity of the tree.  That means that
if many pointers refer to the same chunk of memory then the file will
exhibit that same pattern. To do this, PDBLib remembers the pointers
that it has written and if handed a pointer that it has already written
it does NOT write that memory into the file again.  It instead makes
note of the connection to an already existing region of disk.  These
same considerations apply to reading with the requisite changes in
sense.
<p>
The difference between files and memory is that data trees may be
created, destroyed, and pointers recycled.  For instance, an application
may wish to allocate an array, write it as an indirect, change it
contents in memory, and write it out again under another name.  This
is a very common pattern of usage.  If PDBLib remembers the pointers,
only the first write will put any data on disk. Now PDBLib has no way
of knowing what the application wants.
<p>
By default PDBLib tracks pointers in order to preserve connectivity
patterns in data trees. Applications can exert two kinds of control
over the default behavior. First, they can turn off the pointer tracking
on a per file basis. Second, they can tell PDBLib to remove or reset
its lists of remembered pointers (again on a per file basis).
<p>

<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_get_track_pointers"><h2>PD_GET_TRACK_POINTERS</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_get_track_pointers(PDBfile *file)
<i>Fortran Binding: </i>integer PFGTPT(integer fileid)
<i>SX Binding: </i> (file-info file "track-pointers")
<i>Python Binding: </i> XXX - missing
</pre>
<p>

Get the track pointer flag for PDB file <em>fileid</em>.
If the track pointer flag is set to <tt>FALSE</tt>, PDBLib will not record
pointers and disk addresses as described above.  This obviates the
need for calls to <tt>PD_reset_ptr_list</tt>.
<p>
Input to this function is:
<em>fileid</em>, an integer PDBfile identification number,
<p>
Return the value of the flag.
<p>

<hr>
<b>C Example</b>
<pre>
       PDBfile *file;
       int fl;
             .
             .
             .

       fl = PD_get_track_pointers(file);
             .
             .
             .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfgtpt
       integer fileid, fl
             .
             .
             .

       fl = pfgtpt(fileid)
             .
             .
             .
</pre>
<hr>

<b>SX Example</b>
<pre>
             .
             .
             .

    (define trk (file-info file "track-pointers"))
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_set_track_pointers"><h2>PD_SET_TRACK_POINTERS</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_set_track_pointers(PDBfile *file, int v)
<i>Fortran Binding: </i>integer PFSTPT(integer fileid, integer v)
<i>SX Binding: </i> (set-track-pointers! file v)
<i>Python Binding: </i> XXX - missing
</pre>
<p>

Set the track pointer flag for PDB file<em> fileid</em> to <em>v</em>.
If the track pointer flag is set to <tt>FALSE</tt>, PDBLib will not record
pointers and disk addresses as described above.  This obviates the
need for calls to <tt>PD_reset_ptr_list</tt>.
<p>
Input to this function is:
<em>fileid</em>, an integer PDBfile identification number,
<p>
Output from this function is:
<em>v</em>, an integer value for the track pointer flag.
<p>
Return the old value of the flag.
<p>

<hr>
<b>C Example</b>
<pre>
       PDBfile *file;
             .
             .
             .

       PD_set_track_pointers(file, TRUE);
             .
             .
             .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfstpt
       integer fileid
             .
             .
             .

       if (pfstpt(fileid, 1) .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<b>SX Example</b>
<pre>
             .
             .
             .

    (set-track-pointers! file #t)
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_reset_ptr_list"><h2>PD_RESET_PTR_LIST</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_reset_ptr_list(PDBfile *file)
<i>Fortran Binding: </i>integer PFRSPL(integer fileid)
<i>SX Binding: </i> (reset-pointer-list! file)
<i>Python Binding: </i> XXX - missing
</pre>
<p>

Free the list of pointers which the PDB file file knows about. This
includes both pointers in memory acquired during write operations
and pointers in the file acquired during read operations.
<p>
Rationale: When reading or writing indirectly referenced data,
PDBLib, maintains an array of pointers encountered in write
operations and an array of pointers encountered in read operations.
This is done on a per file basis. These arrays are the basis on which
PDBLIb can determine how to preserve the connectivity of data trees
when they are moved between memory and files. Because of the
difference between memory and disk files, it is important for
applications to be able to clear out these arrays and start over.
See the discussion on Using Pointers earlier in the manual.
<p>
The argument to this function is:
<em>file</em> a pointer to a PDBfile.
<p>
The return value is <tt>TRUE</tt> if successful; otherwise, <tt>FALSE</tt> is returned 
and an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
    PDBfile *file;
            .
            .
            .

     if (PD_reset_ptr_list(file) == FALSE)
        printf("%s", PD_get_error());
            .
            .
            .
</pre>
<hr>

<b>Fortran Example</b>
<pre>
       integer pfrspl
       integer fileid
             .
             .
             .

       if (pfrspl(fileid) .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<b>SX Example</b>
<pre>
             .
             .
             .

    (reset-pointer-list! file)
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_free"><h2>PD_FREE</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_free(PDBfile *file, char *type, void *var)
<i>Fortran Binding: </i>integer PFREE(integer fileid,
                            integer nchr, character type, void var)
<i>SX Binding: </i>
<i>Python Binding: </i>
</pre>
<p>

Free memory which was allocated by PDBLib when reading pointers.
<p>
Rationale: When reading a variable from a file which has pointers,
PDBLib allocates space to hold the actual data.  When it comes
time to release the memory, the application could walk the data
tree which was read by PDBLib for the variable and free any
dynamically allocated spaces.  PDBLib is also capable of doing
this since it understands the structure of the data.  Consequently,
this function has been included as a convenience for application
developers.
See the discussion on Using Pointers earlier in the manual.
<p>
The argument to this function is:
<em>file</em> a pointer to a PDBfile.
<em>type</em> the type of the data pointed to by <em>var</em>.
<em>var</em> a variable previously read by PDBLib.
<p>
The return value is <tt>TRUE</tt> if successful; otherwise, <tt>FALSE</tt> is returned 
and an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
    PDBfile *file;
    foo vr;
            .
            .
            .

     if (PD_write(file, "bar", "foo", &vr) == FALSE)
        printf("%s", PD_get_error());
            .
            .
            .

     if (PD_read(file, "bar", &vr) == FALSE)
        printf("%s", PD_get_error());
            .
            .
            .

     if (PD_free(file, "foo", &vr) == FALSE)
        printf("%s", PD_get_error());
            .
            .
            .
</pre>
<hr>
<b>Fortran Example</b>
<pre>
       integer pffree
       integer fileid
             .
             .
             .

 c ... read array x
       if (pfread(fileid, 3, 'bar', a) .eq. 0)
      &amp;   call errproc
             .
             .
             .

 c ... free array x
       if (pffree(fileid, 3, 'foo', a) .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_dirs"></a>
<h3>Using Directories</h3>


<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_cd"><h2>PD_CD</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_cd(PDBfile *file, char *dir)
<i>Fortran Binding: </i>integer PFCD(integer fileid,
                          integer nchr, char *dir)
<i>SX Binding: </i> (change-directory file dir)
<i>Python Binding: </i> file.cd(dir)
</pre>
<p>

Change the current directory in the specified PDB file.
<p>
PDBLib supports an optional hierarchical directory structure inside
PDB files. A directory or a variable in a directory may be specified
by either a relative path or an absolute path. Slashes separate nodes
in a path name. Absolute paths begin with a slash. Nodes consisting
of two periods, "..", refer to the next higher level directory.
<p>
The arguments to <tt>PD_cd</tt> are:<BR>
<em>file</em> - a pointer to a PDBfile;<BR>
<em>fileid</em> - an integer file identifier;<BR>
<em>nchr</em> - an integer number of characters in string <em>dir</em>;<BR>
<em>dir</em> - an ASCII string containing the path name of the
 directory to change to.
<p>
If dir is NULL or an empty string or a slash, it refers to the
 top level or root directory.
<p>
The return value is <tt>TRUE</tt> if successful;<BR>
otherwise, <tt>FALSE</tt> is returned 
and an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 
<p>
See also:
<a href="#pd_ln"><tt>PD_ln</tt></a>,
<a href="#pd_ls"><tt>PD_ls</tt></a>,
<a href="#pd_ls_alt"><tt>PD_ls_alt</tt></a>,
<a href="#pd_mkdir"><tt>PD_mkdir</tt></a>,
 and
<a href="#pd_pwd"><tt>PD_pwd</tt></a>.
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
    PDBfile *file;
          .
          .
          .

     if (PD_cd(file, "") == FALSE)
        printf("%s", PD_get_error());
     if (PD_cd(file, "/animals/mammals") == FALSE)
        printf("%s", PD_get_error());
          .
          .
          .

     if (PD_cd(file, "../reptiles") == FALSE)
        printf("%s", PD_get_error());
          .
          .
          .

     if (PD_cd(file, NULL) == FALSE)
        printf("%s", PD_get_error());
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfcd
       integer fileid
             .
             .
             .

       if (pfcd(fileid, 16, `/animals/mammals') .eq. 0)
      &amp;   call errproc
             .
             .
             .

       if (pfcd(fileid, 11, `../reptiles') .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<b>SX Example</b>
<pre>
             .
             .
             .

    (change-directory file "/animals/mammals")
             .
             .
             .
</pre>
<hr>

<b>Python Example</b>
<pre>
             .
             .
             .

    file.cd( None )
    file.cd( "/animals/mammals" )
             .
             .
             .

    file.cd( "../reptiles" )
             .
             .
             .

    file.cd( "/" )
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_ln"><h2>PD_LN</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_ln(PDBfile *file,
                       char *var, char *link)
<i>Fortran Binding: </i>integer PFLN(integer fileid,
                          integer nvar, char *var,
                          integer nlink, char *link)
<i>SX Binding: </i> (create-link file var link)
<i>Python Binding: </i> file.ln(var, link)
</pre>
<p>

Create a link to a variable in the specified PDB file.
<p>
PDBLib supports an optional hierarchical directory structure inside
PDB files. A directory or a variable in a directory may be specified
by either a relative path or an absolute path. Slashes separate nodes
in a path name. Absolute paths begin with a slash. Nodes consisting of
two periods, "..", refer to the next higher level directory.
<p>
The arguments to <tt>PD_ln</tt> are:
<em>file</em>, a pointer to a PDBfile;
<em>fileid</em>, an integer file identifier;
<em>nvar</em>, an integer number of characters in string <em>var</em>;
<em>var</em>, an ASCII string containing the path name of an existing variable;
<em>nlink</em>, an integer number of characters in string <em>link</em>;
 and
<em>link</em>, an ASCII string containing the path name of the new link.
<p>
The return value is a <tt>TRUE</tt> if successful; otherwise, <tt>FALSE</tt> is returned
and an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 
<p>
See also:
<a href="#pd_cd"><tt>PD_cd</tt></a>,
<a href="#pd_ls"><tt>PD_ls</tt></a>,
<a href="#pd_ls_alt"><tt>PD_ls_alt</tt></a>,
<a href="#pd_mkdir"><tt>PD_mkdir</tt></a>,
 and
<a href="#pd_pwd"><tt>PD_pwd</tt></a>.
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
    PDBfile *file;
          .
          .
          .

     if (PD_ln(file, "/animals/mammals/chimpanzee", "/chimp") == FALSE)
        printf("%s", PD_get_error());
          .
          .
          .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfln
       integer fileid
             .
             .
             .

       if (pfln(fileid, 27, `/animals/mammals/chimpanzee',
      &amp;                  6, `/chimp') .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<b>SX Example</b>
<pre>
             .
             .
             .

    (create-link file "/animals/mammals/chimpanzee" "/chimp")
             .
             .
             .
</pre>
<hr>

<b>Python Example</b>
<pre>
             .
             .
             .

    file.ln("/animals/mammals/chimpanzee", "/chimp")
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_ls"><h2>PD_LS</h2></a>


<p>
<pre>
<i>C Binding: </i>  char **PD_ls(PDBfile *file,
                          char *path, char *type, int *num)
<i>Fortran Binding: </i>integer PFLST(integer fileid,
                           integer npath, char *path,
                           integer ntype, char *type, integer num)
             integer PFGLS(integer n,
                           integer nchr, char *name)
             integer PFDLS()
<i>SX Binding: </i> (list-variables file path type)
<i>Python Binding: </i> file.ls()
</pre>
<p>

Return a list of names of entries (variables and directories) in PDB
file file that are of type type and that are in the directory and
match the variable name pattern specified by path.
<p>
In FORTRAN applications use <tt>PFLST</tt> to generate an internal table of
names of entries (variables and/or directories) in a PDB file that
are of a specified type and that are in the directory and
match the variable name pattern specified.  Then use <tt>PFGLS</tt> to
get the name of the nth entry in the internal table generated
by <tt>PFLST</tt>.  Once done with the list use <tt>PFDLS</tt> to release the internal
table.
<p>
PDBLib supports an optional hierarchical directory structure inside
PDB files. A directory or a variable in a directory may be specified
by either a relative path or an absolute path. Slashes separate nodes
in a path name. Absolute paths begin with a slash. Nodes consisting
of two periods, "..", refer to the next higher level directory.
<p>
The arguments to <tt>PD_ls</tt> are:
<em>file</em>, a pointer to a PDBfile;
<em>fileid</em>, an integer file identifier;
<em>npath</em>, an integer number of characters in string path;
<em>path</em>, an ASCII string containing the path name of the
 directory to search and/or the variable name pattern to match;
<em>ntype</em>, an integer number of characters in string type;
<em>type</em>, an ASCII string containing the type of entries to return;
and
<em>num</em>, a pointer to an integer to contain the number of entries returned.
<p>
The arguments to <tt>PFGLS</tt> are:
<em>n</em>, an integer ordinal index into the internal table;
<em>nchr</em>, an integer to contain the number of characters
 returned in string <em>name</em>;
and
<em>name</em>, an ASCII string to contain the returned entry name.
<p>
If path is NULL, the contents of the current directory are listed.
If type is NULL, all types are returned.
<p>
<tt>PFGLS</tt> copies the name of a symbol table entry into the name,
if successful. The application must ensure that name is large enough
to contain the longest name in the symbol table (although PDBLib permits
names of arbitrary length, 256 characters would probably be more than enough).
<p>
The terminal node of path may contain meta characters "*" and "?".
Each "*" matches any zero or more characters and each "?" matches
any single character.
<p>
For the sake of efficiency, the returned names are not duplicated.
That is, the caller should not free the space associated with each
of the individual strings, but should free the char ** pointer. This
should be done using the <tt>CFREE</tt> macro as shown in the example.
<p>
The return value is a pointer to an array of strings,
if successful; otherwise, NULL is returned 
and an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 
<p>
See also:
<a href="#pd_cd"><tt>PD_cd</tt></a>,
<a href="#pd_ln"><tt>PD_ln</tt></a>,
<a href="#pd_mkdir"><tt>PD_mkdir</tt></a>,
 and
<a href="#pd_pwd"><tt>PD_pwd</tt></a>.
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
    PDBfile *file;
    char **list;
    int num;
          .
          .
          .

 /* get a list of all directories in the current directory */
     list = PD_ls(file, NULL, "Directory", &amp;num);
     if (list == NULL)
        printf("%s", PD_get_error());
          .
          .
          .

     CFREE(list);
          .
          .
          .

 /* get a list of the variables of type char * in directory animals */
     list = PD_ls(file, "animals", "char *", &amp;num);
     if (list == NULL)
        printf("%s", PD_get_error());
          .
          .
          .

     CFREE(list);
          .
          .
          .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pflst, pfgls, pfdls
       integer fileid
       integer i, nvar, nchr
       character name(256)
             .
             .
             .

 c ... generate a table of all directories in the current directory
       if (pflst(fileid, 0, `', 9, `Directory', nvar) .eq. 0)
      &amp;   call errproc

 c ... get the elements
       do 100 i = 1, nvar
          if (pfgls(i, nchr, name) .eq. 0)
      &amp;      call errproc
             .
             .
             .

   100 continue

 c ... free the table
       if (pfdls() .eq. 0)
      &amp;   call errproc

             .
             .
             .

 c ... generate a table of the variables of type char * in directory animals
       if (pflst(fileid, 7, `animals', 6, `char *', nvar) .eq. 0)
      &amp;     call errproc
             .
             .
             .
</pre>
<hr>

<b>SX Example</b>
<pre>
             .
             .
             .

    (define vars (list-variables file "/animals"))
             .
             .
             .
</pre>
<hr>

<b>Python Example</b>
<pre>
             .
             .
             .

    vars = file.ls("/animals")
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_ls_alt"><h2>PD_LS_ALT</h2></a>


<p>
<pre>
<i>C Binding: </i>  char **PD_ls_alt(PDBfile *file,
                                     char *path, char *type, int *num,
                                     char *flags)
<i>Fortran Binding: </i>none
<i>SX Binding: </i> none
<i>Python Binding: </i> none
</pre>
<p>

Return a list of names of entries (variables and directories) in PDB
file file that are of type type and that are in the directory and
match the variable name pattern specified by path. Similar to
<tt>PD_ls</tt> but with additional control.
<p>
PDBLib supports an optional hierarchical directory structure inside
PDB files. A directory or a variable in a directory may be specified
by either a relative path or an absolute path. Slashes separate nodes
in a path name. Absolute paths begin with a slash. Nodes consisting
of two periods, "..", refer to the next higher level directory.
<p>
The arguments to <tt>PD_ls_alt</tt> are:
<em>file</em>, a pointer to a PDBfile;
<em>fileid</em>, an integer file identifier;
<em>npath</em>, an integer number of characters in string path;
<em>path</em>, an ASCII string containing the path name of the
 directory to search and/or the variable name pattern to match;
<em>ntype</em>, an integer number of characters in string type;
<em>type</em>, an ASCII string containing the type of entries to return;
and
<em>num</em>, a pointer to an integer to contain the number of entries returned.
<em>flags</em>, flags '-a' lists hidden entries, '-R' recursively lists
directories.
<p>
The return value is a pointer to an array of strings,
if successful; otherwise, NULL is returned 
and an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 
<p>
See also:
<a href="#pd_cd"><tt>PD_cd</tt></a>,
<a href="#pd_ln"><tt>PD_ln</tt></a>,
<a href="#pd_ls"><tt>PD_ls</tt></a>,
<a href="#pd_mkdir"><tt>PD_mkdir</tt></a>,
 and
<a href="#pd_pwd"><tt>PD_pwd</tt></a>.
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
    PDBfile *file;
    char **list;
    int num;
          .
          .
          .

 /* get a list of all directories in the current directory */
     list = PD_ls_alt(file, NULL, "Directory", &amp;num, "-R");
     if (list == NULL)
        printf("%s", PD_get_error());
          .
          .
          .

     CFREE(list);
          .
          .
          .

 /* get a list of the variables of type char * in directory animals */
     list = PD_ls_alt(file, "animals", "char *", &amp;num, "-a");
     if (list == NULL)
        printf("%s", PD_get_error());
          .
          .
          .

     CFREE(list);
          .
          .
          .
</pre>

<hr>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pfvart"><h2>PFVART</h2></a>


<p>
<pre>
<i>C Binding: </i>  none
<i>Fortran Binding: </i>integer PFVART(integer fileid,
                            integer order, integer nvars)
                  integer PFGVAR(integer n, integer nchar, character name)
                  integer PFDVAR()
<i>SX Binding: </i> none
<i>Python Binding: </i> none
</pre>
<p>

<tt>PFVART</tt> generates an internal table of variables in the specified PDB file.
With subsequent calls to <tt>PFGVAR</tt>, each entry can be obtained one
at a time by ordinal index. The table is sorted according to a
specified scheme. The current choices are alphabetic and by disk
address.
<p>
<tt>PFGVAR</tt> gets the name of the nth variable in the internal table generated by
a previous call to <tt>PFVART</tt>. The table will have been sorted in a
particular order and this function allows applications to access
the variables in the sorted order, not the default hash ordering
that would normally apply.
<p>
<tt>PFGVAR</tt> copies the name into the array name, if successful.
The application must ensure that name is large enough to contain
the longest name in the symbol table (although PDBLib permits
names of arbitrary length, 256 characters would probably be more
than enough).
<p>
Use <tt>PFDVAR</tt> to release the table created by <tt>PFVART</tt>.
<p>
Rationale: these functions are similar the <tt>PD_ls</tt> functions.  The difference
being in the ability to select the ordering criterion.  The <tt>PD_lst</tt>
functions sort solely by alphabetical order.  It has no counterpart
in the C or SX interface because those languages provide other means
of accomplishing this capability.
<p>
The arguments to <tt>PFVART</tt> are:
<em>fileid</em>, an integer which identifies the PDBfile;
<em>order</em>, an integer specifying the sort ordering;
   and
<em>nvars</em>, an integer in which the number of variables in
 the file is returned.
<p>
The choices for order are: 1, for an alphabetic sort; and 2, for a
disk address order sort.
<p>
The arguments to <tt>PFGVAR</tt> are:
<em>n</em>, an integer ordinal index into the internal sorted name
   table;
<em>nchr</em>, an integer in which the number of characters in the
   <em>name</em> string is returned;
<em>name</em>, an ASCII string which will contain the variable name
   upon successful completion.
<p>
The return value is 1, if successful; otherwise, 0 is returned and
an error message may be retrieved by invoking function <tt>PFGERR</tt>.
<p>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfvart, pfgvar, pfdvar
       integer i, n, nvar, nchar
       character vname(256)
       integer fileid, nvar, order
             .
             .
             .

 c ... create table of variable names in alphabetic order
       order = 1
       if (pfvart(fileid, order, nvar) .eq. 0)
      $   call errproc

 c ... print out the table of variable names created by pfvart
       write(6,700)
  700  format(/,'Alphabetic list of variables:')
       do 701 n = 1, nvar
          if (pfgvar(n, nchar, vname) .eq. 0)
      &amp;        call errproc
          write(6,702) (vname(i), i=1,nchar)
  702     format('            ',256a1)
  701  continue

c ... release the table now
       if (pfdvar() .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_mkdir"><h2>PD_MKDIR</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_mkdir(PDBfile *file, char *dir)
<i>Fortran Binding: </i>integer PFMKDR(integer fileid, integer nchr, char *dir)
<i>SX Binding: </i> (make-directory file dir flag)
<i>Python Binding: </i> file.mkdir(dir)
</pre>
<p>

Create a new directory in the specified PDB file.
<p>
PDBLib supports an optional hierarchical directory structure inside
PDB files. A directory or a variable in a directory may be specified
by either a relative path or an absolute path. Slashes separate nodes
in a path name. Absolute paths begin with a slash. Nodes consisting of
two periods, "..", refer to the next higher level directory.
<p>
The arguments to <tt>PD_mkdir</tt> are:<BR>
<em>file</em> - a pointer to a PDBfile;<BR>
<em>fileid</em> - an integer file identifier;<BR>
<em>nchr</em> - an integer number of characters in string <em>dir</em>;<BR>
<em>dir</em> - an ASCII string containing the path name of the new directory;<BR>
<em>flag</em> - if <tt>TRUE</tt> report an error if the directory cannot be created.
<p>
The root directory, "/", does not have to be created.
<p>
The return value is <tt>TRUE</tt> if successful;<BR>
otherwise, <tt>FALSE</tt> is returned
and an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 
<p>
See also:
<a href="#pd_cd"><tt>PD_cd</tt></a>,
<a href="#pd_ln"><tt>PD_ln</tt></a>,
<a href="#pd_ls"><tt>PD_ls</tt></a>,
<a href="#pd_ls_alt"><tt>PD_ls_alt</tt></a>,
 and
<a href="#pd_pwd"><tt>PD_pwd</tt></a>.
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
    PDBfile *file;
          .
          .
          .

     if (PD_mkdir(file, "/animals/mammals") == FALSE)
        printf("%s", PD_get_error());
          .
          .
          .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfmkdr
       integer fileid
             .
             .
             .

       if (pfmkdr(fileid, 16, `/animals/mammals') .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<b>SX Example</b>
<pre>
             .
             .
             .

    (make-directory file "/animals/mammals")
             .
             .
             .
</pre>
<hr>

<b>Python Example</b>
<pre>
             .
             .
             .

    file.mkdir("/animals/mammals")
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_pwd"><h2>PD_PWD</h2></a>


<p>
<pre>
<i>C Binding: </i>  char *PD_pwd(PDBfile *file)
<i>Fortran Binding: </i>integer PFPWD(integer fileid, integer nchr, char *dir)
<i>SX Binding: </i> (current-directory file)
<i>Python Binding: </i> file.pwd()
</pre>
<p>

Return the current directory for the specified PDB file.
<p>
PDBLib supports an optional hierarchical directory structure inside
PDB files. A directory or a variable in a directory may be specified
by either a relative path or an absolute path. Slashes separate
nodes in a path name. Absolute paths begin with a slash. Nodes
consisting of two periods, "..", refer to the next higher level
directory.
<p>
The arguments to <tt>PD_pwd</tt> are:
<em>file</em>, a pointer to a PDBfile;
<em>fileid</em>, an integer file identifier;
<em>nchr</em>, an integer to contain the number of characters returned in
 string <em>dir</em>;
and
<em>dir</em>, an ASCII string to contain the path name of the current directory.<p>
<p>
If no directory has been created, "/" is returned.
In the FORTRAN binding this function copies the path name of a
directory into the <em>dir</em>, if successful. The application
must ensure that <em>dir</em> is large enough to contain the
longest directory name (although PDBLib permits names of arbitrary
length, 256 characters would probably be more than enough).
<p>
The return value is a pointer to a string containing the path name
of the current directory if successful; otherwise, NULL is returned
and an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 
<p>
See also:
<a href="#pd_cd"><tt>PD_cd</tt></a>,
<a href="#pd_ln"><tt>PD_ln</tt></a>,
<a href="#pd_ls"><tt>PD_ls</tt></a>,
<a href="#pd_ls_alt"><tt>PD_ls_alt</tt></a>,
 and
<a href="#pd_mkdir"><tt>PD_mkdir</tt></a>.
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
    PDBfile *file;
    char *dir;
          .
          .
          .

     if ((dir = PD_pwd(file)) == NULL)
        printf("%s", PD_get_error());
          .
          .
          .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfpwd
       integer fileid, nchr
       character dir(256)
             .
             .
             .

       if (pfpwd(fileid, nchr, dir) .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<b>SX Example</b>
<pre>
             .
             .
             .

    (define dir (current-directory file))
             .
             .
             .
</pre>
<hr>

<b>Python Example</b>
<pre>
             .
             .
             .

    dir = file.pwd()
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_isdir"><h2>PD_ISDIR</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_isdir(PDBfile *file, char *dir)
<i>Fortran Binding: </i>integer PFISDR(integer fileid, integer nchr, char *dir)
<i>SX Binding: </i> (file-directory? file dir)
<i>Python Binding: </i> file.isdir(dir)
</pre>
<p>

Return TRUE if the specified path is a directory in the specified PDB file.
<p>
PDBLib supports an optional hierarchical directory structure inside
PDB files. A directory or a variable in a directory may be specified
by either a relative path or an absolute path. Slashes separate nodes
in a path name. Absolute paths begin with a slash. Nodes consisting of
two periods, "..", refer to the next higher level directory.
<p>
The arguments to <tt>PD_isdir</tt> are:<BR>
<em>file</em> - a pointer to a PDBfile;<BR>
<em>fileid</em> - an integer file identifier;<BR>
<em>nchr</em> - an integer number of characters in string <em>dir</em>;<BR>
<em>dir</em> - an ASCII string containing the path name of the new directory;<BR>
<p>
The return value is <tt>TRUE</tt> if successful;<BR>
otherwise, <tt>FALSE</tt> is returned.
<p>
See also:
<a href="#pd_mkdir"><tt>PD_mkdir</tt></a>,
<a href="#pd_cd"><tt>PD_cd</tt></a>,
<a href="#pd_ln"><tt>PD_ln</tt></a>,
<a href="#pd_ls"><tt>PD_ls</tt></a>,
<a href="#pd_ls_alt"><tt>PD_ls_alt</tt></a>,
 and
<a href="#pd_pwd"><tt>PD_pwd</tt></a>.
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
    PDBfile *file;
          .
          .
          .

     if (PD_isdir(file, "/animals/mammals") == FALSE)
        printf("%s", PD_get_error());
          .
          .
          .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfmkdr
       integer fileid
             .
             .
             .

       if (pfisdr(fileid, 16, `/animals/mammals') .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<b>SX Example</b>
<pre>
             .
             .
             .

    (if (file-directory? file "/animals/mammals")
        (printf nil "/animals/mammals is a directory\n")
             .
             .
             .
</pre>
<hr>

<b>Python Example</b>
<pre>
             .
             .
             .

    ok = file.isdir("/animals/mammals")
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_par"><h3>Parallel Programming with PDB</h3></a>


PDBLib supports parallel programming on SMP systems using Pthreads or Sprocs
and on distributed memory systems using MPI.

<h4>Parallel Programming on SMP Systems</h4>

When using the thread-safe version of PDBLib the application program must
call the library's initialization routine, <tt>PD_init_threads</tt>, before
making any other PDBLib calls.  This routine will initialize PDB and
SCORE thread specific data structures.  With the exception of this
one additional initialization call, PDBLib can be used similarly in
threaded and serial programs.  In multi-threaded applications care must
be taken to make sure a file is opened before any read or write calls
are issued.  Applications must also be careful not to issue <tt>PD_close</tt>
or <tt>PD_flush</tt> calls while other threads are actively reading or writing.
<p>
The thread-safe version of PDBLib can be used by applications to read
or write the same file in parallel, or to operate on different files
simultaneously.  Reading and writing the same file in parallel is,
of course, a risky thing to do.
<p>

<h4>Parallel Programming on Distributed Memory Systems</h4>
<p>
Applications using this distributed memory library must initialize the library
for MPI by calling the routine, <tt>PD_init_mpi</tt>, before making any other PDBLib
calls.  This routine will initialize PDB and SCORE for MPI and will initialize the
parallel file manager on the specified processor.  After all other PDBLib activity
is finished, applications must call the termination routine, <tt>PD_term_mpi</tt>,
which will perform library clean-up, including terminating the parallel file manager.
<p>
In addition to the special initialization and termination routines, applications
performing distributed parallel I/O must use the special MPI versions of the
library open and create calls, <tt>PD_mp_open</tt> and <tt>PD_mp_create</tt>, in place of <tt>PD_open</tt>
and <tt>PD_create</tt> when multiple processes will perform I/O to a common file.  These
routines have an additional argument, an intra-communicator.
All participating processes must make the open or create calls, since the MPI-IO
functions which provide the low-level I/O define these as collective operations.
Each participating process will block on these calls until all processes have
made the call.  Care must be taken to avoid deadlock when utilizing these
collective operations.
<p>
Other PDBLib routines that are collective operations include <tt>PD_flush</tt>, <tt>PD_close</tt>,
and <tt>PD_defent</tt>.  Each of these require that all participating processes make the
call before any continue.  PDBLib applications that make frequent calls to
<tt>PD_flush</tt> when running in serial mode must examine their flush logic carefully
to avoid deadlock when converting to MPI systems.  Additionally, <tt>PD_flush</tt> is an
expensive operation in parallel, so should be minimized in any case.
<p>
For applications that utilize "distributed arrays", that is, arrays that are
decomposed across processes, it is possible to write these to a single array
in the PDB file.  The <tt>PD_defent</tt> routine is used to lay out the space in the
file that will contain the whole array.  The size must be known and specified
in the <tt>PD_defent</tt> call.  Subsequent calls to <tt>PD_write</tt> allow each process to
contribute its piece of the distributed array to the file.  Remember that since
<tt>PD_defent</tt> is a collective call it must be made by each process and will be
a synchronization point in the code.
<p>
On systems where threadsafe MPI libraries are available, such as the IBM systems,
PDBlib calls to read and/or write can occur asynchronously (i.e. the calls
will not block until all processes have made them).  On other systems, such
as the Compaqs and most others, the lack of threadsafe MPI implementations 
make it necessary for applications to run in "lock-step", and have all participating
processes make the calls before proceeding.  This is because on systems with
threadsafe MPI PDBlib spawns a thread where the parallel file manager runs.
On systems without threadsafe MPI the process designated as the master process
in the initialization call will service the parallel file manager requests from 
other participating processes before returning from PDBlib calls to read or write.

<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_init_threads"><h2>PD_INIT_THREADS</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_init_threads(int np, void (*tid)(int *))
<i>Fortran Binding: </i>integer PFINTH(integer np, external tid)
<i>SX Binding: </i> none
<i>Python Binding: </i> none
</pre>
<p>

Initialize a set of threads for an SMP parallel application.
A function, <em>tid</em>, may be supplied to identify the current
thread in a parallel execution region. It takes a single integer pointer
argument which is used to return a thread index between 0 and np-1.
If <em>tid</em> is NULL the built in SCORE function will be used.
In this way PDBLib will be able to know which thread it is working
on when it is being used in a parallel execution sequence.
<p>
PDBLib can be safely used in threaded programs.  <tt>PD_init_threads</tt> should
be called before any other PDBLib calls are made so that the thread
related PDB and SCORE data structures are properly initialized.
Applications which use a thread pool to achieve their parallelism
can safely pass a NULL value for the tid argument.  The built in SCORE
function for returning a thread index is designed to work in such
situations.  Applications that repeatedly create and destroy threads
during the course of a run should provide a function that always sets
its argument to a value between 0 and np-1.
<p>
The arguments to this function are:
<em>np</em>, the number of threads to initialize;
and
<em>tid</em>, a function which identifies the current thread.
<p>
The return value is <tt>TRUE</tt> if successful; otherwise, <tt>FALSE</tt> is returned.
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
    int np;
            .
            .
            .

     if (!PD_init_threads(np, NULL))
        printf("Couldn't initialize threads\n");
            .
            .
            .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfinth
       integer np
             .
             .
             .

       if (pfinth(np, 0) .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 
<a name="pd_init_mpi"></a>
<h2>PD_INIT_MPI</h2>


<p>
<pre>
<i>C Binding: </i>  int PD_init_mpi(int masterproc, int nthreads, void (*tid)(int *))
<i>Fortran Binding: </i>integer PFINMP(integer masterproc, integer nthreads, external tid)
</pre>
<p>

Initialize PDBLib for MPI use.  The first argument, <em>masterproc</em>, should
be the rank of the process that will spawn a control thread.  For the current
implementation <em>nthreads</em> and <em>tid</em> should be set to 1 and NULL,
respectively.  Future implementations will allow threads and MPI to coexist in
the application.
<p>
The call to <tt>PD_init_mpi</tt> should precede any other PDBLib calls in the application.
<p>
The arguments to this function are:
<em>masterproc</em>, the rank of the process where the control thread should run;
<em>nthreads</em>, the number of threads the application will use (to be implemented,
    use 1 for now);
and
<em>tid</em>, a function which identifies the current thread (to be implemented).
<p>
The return value is <tt>TRUE</tt> if successful; otherwise, <tt>FALSE</tt> is returned.
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
    int masterproc;
    int nthreads = 1;
            .
            .
            .

     if (!PD_init_mpi(masterproc, nthreads, NULL))
        printf("Couldn't initialize PDBLib for MPI\n");
            .
            .
            .
</pre>
<hr>

<b>Fortran Example</b>
<pre>
       integer pfinmp
       integer masterproc, nthreads,
             .
             .
             .

       if (pfinmp(masterproc, nthreads, 0) .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_term_mpi"><h2>PD_TERM_MPI</h2></a>


<p>
<pre>
<i>C Binding: </i>  void PD_term_mpi()
<i>Fortran Binding: </i>integer PFTMMP()

</pre>
<p>

Perform any clean-up operations necessary and terminate PDBLib in an MPI application.
This call will terminate the PDBLib control thread and perform any other
necessary activities when the application is finished using the library.
No further PDBLib calls should be made after this call.
<p>
The arguments to this function are:
<em>none</em>
<p>
There is no return value.
<p>
<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
            .
            .
            .

     PD_term_mpi();
            .
            .
            .
</pre>
<hr>

<b>Fortran Example</b>
<pre>
             .
             .
             .

       call pftmmp()       
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_mp_open"><h2>PD_MP_OPEN</h2></a>


<p>
<pre>
<i>C Binding: </i>  PDBfile *PD_mp_open(char *filename, char *mode, MPI_Comm comm)
<i>Fortran Binding: </i>integer PFMPOP(integer nchr, character name, character mode, integer comm)
</pre>
<p>

Open an existing PDB file in MP parallel mode.  With the exception of the additional
argument <em>comm</em>, the semantics of this call are identical to those of
<a href="#pd_open">PD_open</a>.  The <em>comm</em> argument should be an MPI communicator
which specifies the group of participating processes.  The communicator must include
the process where the control thread runs in the current implementation.  This call
is a collective call, therefore every participating process must make the call before
any can proceed.
<p> 
<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
    PDBfile *file;
    MPI_Comm comm;
           .
           .
           .

    if ((file = PD_mp_open("filename", "r", comm)) == NULL)
       printf("%s", PD_get_error());
           .
           .
           .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfmpop
       integer fileid
       integer comm
             .
             .
             .

       fileid = pfmpop(8, 'file.pdb', 'r', comm)
       if (fileid .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_mp_create"><h2>PD_MP_CREATE</h2></a>


<p>
<pre>
<i>C Binding: </i>  PDBfile *PD_mp_create(char *filename, SC_communicator *comm)
</pre>
<p>

Create a PDB file in MP parallel mode.  With the exception of the additional
argument <em>comm</em>, the semantics of this call are identical to those of
<a href="#pd_create"><tt>PD_create</tt></a>.  The <em>comm</em> argument should be an MPI communicator
which specifies the group of participating processes.  The communicator must include
the process where the control thread runs in the current implementation.  This call
is a collective call, therefore every participating process must make the call before
any can proceed.
<p> 
<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
    PDBfile *file;
    SC_communicator comm;
           .
           .
           .

    if ((file = PD_mp_open("filename", "r", comm)) == NULL)
       printf("%s", PD_get_error());
           .
           .
           .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfmpop
       integer fileid
       integer comm
             .
             .
             .

       fileid = pfmpop(8, 'file.pdb', 'r', comm)
       if (fileid .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_spec"><h3>Special Purpose I/O</h3></a>


PDBLib is frequently used in connection with visualization of scientific
data.  To facilitate this four special data structures have special
functions dedicated to gathering and writing of this kind of data.  The
structures are: a curve, a set, a mapping, and an image. The latter
three have representations in C structs in various parts of PACT.
<p>
If an application produces 1d data sets, writing curves into files
would be the general intent.  The PACT utility ULTRA can carry out
analysis and visualization of such data.
<p>
If an application produces multi-dimensional data sets, writing mappings
into files would the natural thing to do.  Mappings include 1d datasets
although they are represented differently than curves.  The PACT utility
PDBView can do analysis and visualization operations with such data.
<p>

<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_put_image"><h2>PD_PUT_IMAGE</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_put_image(PDBfile *file, PG_image *image,
                              int index)
<i>Fortran Binding: </i>integer PFWIMA(integer fileid,
                            integer nchr, character name,
                            integer pkn, integer pkx,
                            integer pln, integer plx,
                            real*8 data,
                            real*8 pxn, real*8 pxx,
                            real*8 pyn, real*8 pyx,
                            integer pim)
<i>SX Binding: </i> none
<i>Python Binding: </i> none
</pre>
<p>

Build a <tt>PG_image</tt> structure out of the given input data and write it
to a PDB file.
<p>
Rationale: The <tt>PG_image</tt> structure is a useful and general purpose
representation of a raster image. The nature of the generalization
is that the values in the raster are floating point numbers. So in
addition to the standard data sets that can be rasterized, the
<tt>PG_image</tt> can be used to display the computational matrix of
some system of equations, for example. This function is a
convenient way for FORTRAN programs to put out their data into
PDB files as <tt>PG_image</tt>'s for later visualization and processing
by other programs. It allows a rectangular subset of a two
dimensional array to be specified for the <tt>PG_image</tt>.
<p>
The calling application must keep track of how many <tt>PM_image</tt>'s have
been written to each file. PDBLib will write each <tt>PM_image</tt> under
the name composed of the string, `Image', and the integer pim. For
example if pim is 9, the <tt>PM_image</tt> will be written under the
name `Image9'. If the application passes the same value for
pim more than once only the last one will survive in the symbol
table even though the data for each <tt>PM_image</tt> will persist in the
file!
<p>
The arguments to <tt>PFWIMA</tt> are:
<em>file</em>, PDBfile for writing;
<em>image</em>, <tt>PG_image</tt> for writing;
<em>ind</em>, integer index count;
<em>fileid</em>, an integer identifier which designates the PDB file
   to which to attempt to write;
<em>nchr</em>, an integer number of characters in name;
<em>name</em>, an ASCII string containing the name of the image;
<em>pkn</em>, the integer minimum column index of the data array;
<em>pkx</em>, the integer maximum column index of the data array;
<em>pln</em>, the integer minimum row index of the data array;
<em>plx</em>, the integer maximum row index of the data array;
<em>data</em>, an array of real*8 values containing the image data;
<em>pxn</em>, a real*8 value specifying the minimum column index in image;
<em>pxx</em>, a real*8 value specifying the maximum column index in image;
<em>pyn</em>, a real*8 value specifying the minimum row index in image;
<em>pyx</em>, a real*8 value specifying the maximum row index in image;
   and
<em>pim</em>, a counter specifying the number of the image being
   written out.
<p>
The return value is 1, if successful; otherwise, 0 is returned 
and an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 
For FORTRAN programs an error message may be retrieved by invoking function <tt>PFGERR</tt>.
<p>

<hr>
<b>C Example</b>
<pre>
       PDBfile *file;
       PG_image *image;
       static int count = 0;
             .
             .
             .

       if (!PD_put_image(file, image, count++))
          printf("%s", PD_get_error());
             .
             .
             .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfwima
       integer fileid, k, l
 c ... these arguments to pfwima must have 8 byte element size
       double precision xmin, xmax, ymin, ymax, data(0:10,0:10)
             .
             .
             .

       xmin = 0.0
       xmax = 10.0
       ymin = 0.0
       ymax = 10.0
       do 100 l = 0, 10
          do 101 k = 0, 10
             data(k, l) = (k - 5)**2 + (l - 5)**2
  101     continue
  100  continue
 
       if (pfwima(fileid, 10, 'Test image', 0, 10, 0, 10,
      &amp;           data, xmin, xmax, ymin, ymax, 1) .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_put_mapping"><h2>PD_PUT_MAPPING</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_put_mapping(PDBfile *file,
                                PM_mapping *mapping, int index)
<i>Fortran Binding: </i>integer PFWMAP(integer fileid,
                            character dname, integer dp, real*8 dm,
                            character rname, integer rp, real*8 rm,
                            integer pim)
<i>SX Binding: </i> none
<i>Python Binding: </i> none
</pre>
<p>
Build a <tt>PM_mapping</tt> structure out of the given input data and write
it to a PDB file.
<p>
Rationale: The <tt>PM_mapping</tt> structure is a convenient medium of
exchange between data production systems such as simulation
codes, storage systems such as PDBLib, and visualization systems
such as PDBView. This function is a convenient way for FORTRAN
programs to put out their data into PDB files as <tt>PM_mapping</tt>'s for
later visualization and processing by other programs.
<p>
The <tt>PM_mapping</tt> is a structure with two main parts: a domain and
a range. These two parts are in turn represented by a structure
called a <tt>PM_set</tt>. Because they are both represented as the same
type of data object, they are specified similarly in <tt>PFWMAP</tt>.
For each of the domain and range sets the following information
is given: a name; an array of integer quantities specifying such
information as the dimensionality of the set, the dimensionality
of the elements, the number of elements, and so on; and a linear
array containing the elements of the set.
<p>
The entries in the arrays <em>dp</em> and <em>rp</em> are as follows:
<p>

<BLOCKQUOTE>
<TABLE>
<TR>
<TD>1</TD>
<TD>the number of characters in the corresponding set name</TD>
</TR>
<TR>
<TD>2</TD>
<TD>the dimensionality of the set, nd</TD>
</TR>
<TR>
<TD>3</TD>
<TD>the dimensionality of the set elements, nde</TD>
</TR>
<TR>
<TD>4</TD>
<TD>the number of elements in the set, ne</TD>
</TR>
<TR>
<TD>5 thru 5+ nd-1</TD>
<TD>the sizes in each dimension</TD>
</TR>
</TABLE>
</BLOCKQUOTE>

The layout of the set elements in <em>dm</em> and <em>rm</em> is:<p>

<BLOCKQUOTE>
<TABLE>
<TR>
<TD>1 thru ne</TD>
<TD>values of the first component</TD>
</TR>
<TR>
<TD>ne+1 thru 2*ne</TD>
<TD>values of the second component</TD>
</TR>
<TR>
<TD>...</TD>
<TD>values of components</TD>
</TR>
<TR>
<TD>(nde-1)*ne+1 thru nde*ne</TD>
<TD>values of the nde'th component</TD>
</TR>
</TABLE>
</BLOCKQUOTE>

The calling application must keep track of how many <tt>PM_mapping</tt>'s
have been written to each file. PDBLib will write each <tt>PM_mapping</tt>
under the name composed of the string, `Mapping', and the integer
<em>pim</em>. For example if <em>pim</em> is 9,
the <tt>PM_mapping</tt> will be written under
the name `Mapping9'. If the application passes the same value for
<em>pim</em> more than once only the last one will survive in the symbol
table even though the data for each <tt>PM_mapping</tt> will persist in
the file!
<p>
The arguments to <tt>PFWMAP</tt> are:
<em>file</em>, PDBfile for writing;
<em>map</em>, <tt>PM_mapping</tt> to write;
<em>ind</em>, <tt>PM_mapping</tt> index;
<em>fileid</em>, an integer identifier which designates the PDB file
   to which to attempt to write;
<em>dname</em>, an ASCII string containing the name of the domain set;
<em>dp</em>, an integer array of parameters defining the domain set;
<em>dm</em>, an array of real*8 values containing the set elements
   component by component;
<em>rname</em>, an ASCII string containing the name of the range set;
<em>rp</em>, an integer array of parameters defining the range set;
<em>rm</em>, an array of real*8 values containing the range elements
   component by component;
   and
<em>pim</em> a counter specifying the number of the mapping being
   written out.
<p>
The return value is 1, if successful; otherwise, 0 is returned 
and an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 
For FORTRAN programs an error message may be retrieved by invoking function <tt>PFGERR</tt>.
<p>

<hr>
<b>C Example</b>
<pre>
       PDBfile *file;
       PM_mapping *f;
       static int count = 0;
             .
             .
             .

       if (!PD_put_mapping(file, f, count++))
          printf("%s", PD_get_error());
             .
             .
             .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfwmap
       integer fileid, dp(5), rp(5)
       double precision dm(0:99), rm(0:99)
             .
             .
             .

       dp(1) = 6
       dp(2) = 1
       dp(3) = 1
       dp(4) = 100
       dp(5) = 100
       rp(1) = 6
       rp(2) = 1
       rp(3) = 1
       rp(4) = 100
       rp(5) = 100
 
       do 100 i = 0, 99
          dm(i) = 6.28*float(i)/99.
          rm(i) = sin(6.28*float(i)/99.)
  100  continue
 
       if (pfwmap(fileid, `Domain', dp, dm, `Range', rp, rm, 0)
      &amp;    .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pfwran"><h2>PFWRAN</h2></a>


<p>
<pre>
<i>C Binding: </i>  none
<i>Fortran Binding: </i>integer PFWRAN(integer fileid,
                            character dname, integer nchr,
                            character rname, integer rp, real*8 rm,
                            integer pim)
<i>SX Binding: </i> none
<i>Python Binding: </i> none
</pre>
<p>

Build a <tt>PM_mapping</tt> structure out of the given input data and write
it to a PDB file.
<p>
Rationale: The <tt>PM_mapping</tt> structure is a convenient medium of
exchange between data production systems such as simulation
codes, storage systems such as PDBLib, and visualization systems
such as PDBView. This function is a convenient way for FORTRAN
programs to put out their data into PDB files as <tt>PM_mapping</tt>'s
for later visualization and processing by other programs.
<p>
The <tt>PM_mapping</tt> is a structure with two main parts: a domain and
a range. These two parts are in turn represented by a structure
called a <tt>PM_set</tt>. In many cases a number of <tt>PM_mapping</tt>'s share a
common domain set. It is therefore more efficient to write the
unique domain sets out separately and use <tt>PFWRAN</tt> to write out
the <tt>PM_mapping</tt>'s without their domains. Post processor codes
such as PDBView (by definition) know how to put the full <tt>PM_mapping</tt>
back together. Note: the domain name given for <tt>PFWRAN</tt> must be the
same as the domain name passed to the corresponding <tt>PFWSET</tt> call.
<p>
For each range set the following information is given:
a name; an array of integer quantities specifying such information
as the dimensionality of the set, the dimensionality of the
elements, the number of elements, and so on; and a linear array
containing the elements of the set.
<p>
The entries in the array <em>rp</em> are as follows:
<p>

<BLOCKQUOTE>
<TABLE>
<TR>
<TD>1</TD>
<TD>the number of characters in the corresponding set name</TD>
</TR>
<TR>
<TD>2</TD>
<TD>the dimensionality of the set, nd</TD>
</TR>
<TR>
<TD>3</TD>
<TD>the dimensionality of the set elements, nde</TD>
</TR>
<TR>
<TD>4</TD>
<TD>the number of elements in the set, ne</TD>
</TR>
<TR>
<TD>5 thru 5+ nd-1</TD>
<TD>the sizes in each dimension</TD>
</TR>
</TABLE>
</BLOCKQUOTE>
<p>
The layout of the set elements in rm is:<p>


<BLOCKQUOTE>
<TABLE>
<TR>
<TD>1 thru ne</TD>
<TD>values of the first component</TD>
</TR>
<TR>
<TD>ne+1 thru 2*ne</TD>
<TD>values of the second component</TD>
</TR>
<TR>
<TD>...</TD>
<TD>values of components</TD>
</TR>
<TR>
<TD>(nde-1)*ne+1 thru nde*ne</TD>
<TD>values of the nde'th component</TD>
</TR>
</TABLE>
</BLOCKQUOTE>

The calling application must keep track of how many <tt>PM_mapping</tt>'s have
been written to each file. PDBLib will write each <tt>PM_mapping</tt> under
the name composed of the string, `Mapping', and the integer <em>pim</em>.
For example if <em>pim</em> is 9, the <tt>PM_mapping</tt> will be written under the
name `Mapping9'. If the application passes the same value for <em>pim</em>
more than once only the last one will survive in the symbol table
even though the data for each <tt>PM_mapping</tt> will persist in the file.
<p>
The arguments to <tt>PFWRAN</tt> are:
<em>fileid</em>, an integer identifier which designates the PDB file to
   which to attempt to write;
<em>dname</em>, an ASCII string containing the name of the domain set;
<em>nchr</em>, an integer number of characters in dname;
<em>rname</em>, an ASCII string containing the name of the range set;
<em>rp</em>, an integer array of parameters defining the range set;
<em>rm</em>, an array of real*8 values containing the range elements
   component by component;
   and
<em>pim</em> a counter specifying the number of the mapping being written
   out.
<p>
The return value is 1, if successful; otherwise, 0 is returned and
an error message may be retrieved by invoking function <tt>PFGERR</tt>.
<p>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfwran
       integer fileid, i, pim, rp(5)
       double precision rm(0:99)
             .
             .
             .

       pim = 0
       rp(1) = 6
       rp(2) = 1
       rp(3) = 1
       rp(4) = 100
       rp(5) = 100
 
       do 100 i = 0, 99
          rm(i) = sin(6.28*float(i)/99.)
  100  continue
 
 c ... `Domain' written previously by pfwset
       if (pfwran(fileid, `Domain', 6, `Range', rp, rm, pim)
      &amp;    .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_put_set"><h2>PD_PUT_SET</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_put_set(PDBfile *file, PM_set *set)
<i>Fortran Binding: </i>integer PFWSET(integer fileid,
                            character dname, integer dp, real*8 dm)
<i>SX Binding: </i> none
<i>Python Binding: </i> none
</pre>
<p>

Build a <tt>PM_set</tt> structure out of the given input data and write it to
a PDB file.
<p>
Rationale: The <tt>PM_set</tt> structure is a fundamental component of the
<tt>PM_mapping</tt> structure which is a convenient medium of exchange
between data production systems such as simulation codes, storage
systems such as PDBLib, and visualization systems such as PDBView.
Although the function <tt>PFWMAP</tt> most conveniently writes a mapping
out to a PDB file, it does not make the best use of limited
storage space. In many cases a number of <tt>PM_mapping</tt>'s share a
common domain set. It is therefore more efficient to write the
unique domain sets out separately and use <tt>PFWRAN</tt> to write out
the <tt>PM_mapping</tt>'s without their domains. Post processor codes
such as PDBView (by definition) know how to put the full <tt>PM_mapping</tt>
back together. Note: the domain name given for <tt>PFWSET</tt> must be the
same as the domain name passed to the corresponding <tt>PFWRAN</tt> call!
<p>
For each <tt>PM_set</tt> the following information is given: a name; an array
of integer quantities specifying such information as the
dimensionality of the set, the dimensionality of the elements,
the number of elements, etc.; and a linear array containing the
elements of the set.
<p>
The entries in the array <em>dp</em> are as follows:
<p>

<BLOCKQUOTE>
<TABLE>
<TR>
<TD>1</TD>
<TD>the number of characters in the corresponding set name</TD>
</TR>
<TR>
<TD>2</TD>
<TD>the dimensionality of the set, nd</TD>
</TR>
<TR>
<TD>3</TD>
<TD>the dimensionality of the set elements, nde</TD>
</TR>
<TR>
<TD>4</TD>
<TD>the number of elements in the set, ne</TD>
</TR>
<TR>
<TD>5 thru 5+ nd-1</TD>
<TD>the sizes in each dimension</TD>
</TR>
</TABLE>
</BLOCKQUOTE>

The layout of the set elements in <em>dm</em> is:<p>

<BLOCKQUOTE>
<TABLE>
<TR>
<TD>1 thru ne</TD>
<TD>values of the first component</TD>
</TR>
<TR>
<TD>ne+1 thru 2*ne</TD>
<TD>values of the second component</TD>
</TR>
<TR>
<TD>...</TD>
<TD>values of components</TD>
</TR>
<TR>
<TD>(nde-1)*ne+1 thru nde*ne</TD>
<TD>values of the nde'th component</TD>
</TR>
</TABLE>
</BLOCKQUOTE>

The arguments to <tt>PFWSET</tt> are:
<em>file</em>, the PDBfile for writing;
<em>set</em>, the <tt>PM_set</tt> to be written;
<em>fileid</em>, an integer identifier which designates the PDB file to
   which to write;
<em>dname</em>, an ASCII string containing the name of the <tt>PM_set</tt>;
<em>dp</em>, an integer array of parameters defining the <tt>PM_set</tt>;
   and
<em>dm</em>, an array of real*8 values containing the set elements
   component by component.
<p>
The return value is 1, if successful; otherwise, 0 is returned 
and an ASCII string error message can be retrieved through a call to 
<a href="#pd_get_error"><tt>PD_get_error</tt></a>. 
For FORTRAN programs an error message may be retrieved by invoking function <tt>PFGERR</tt>.
<p>

<hr>
<b>C Example</b>
<pre>
       PDBfile *file;
       PM_set *set;
             .
             .
             .

       if (!PD_put_set(file, set))
          printf("%s", PD_get_error());
             .
             .
             .
</pre>

<hr>
<b>Fortran Example</b>
<pre>
       integer pfwset
       integer fileid, i, dp(5)
       double precision dm(0:99)
             .
             .
             .

       dp(1) = 6
       dp(2) = 1
       dp(3) = 1
       dp(4) = 100
       dp(5) = 100
 
       do 100 i = 0, 99
          dm(i) = 6.28*float(i)/99.
  100  continue
 
       if (pfwset(fileid, `Domain', dp, dm) .eq. 0)
      &amp;   call errproc
             .
             .
             .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_make_image"><h2>PD_MAKE_IMAGE</h2></a>


<p>
<pre>
<i>C Binding: </i>  PD_image *PD_make_image(char *name, char *type, void *data,
                                     double dx, double dy, int bpp,
                                     double xmin, double xmax,
                                     double ymin, double ymax,
                                     double zmin, double zmax)
<i>Fortran Binding: </i>none
<i>SX Binding: </i> none
<i>Python Binding: </i> none
</pre>
<p>

Create and initialize a <tt>PD_image</tt> data structure.
<p>
Input for this function is:
<em>name</em>, an ASCII string containing the name of the image;
<em>type</em>, an ASCII string containing the data type of the image
   elements (e.g. "char", "float", "complex");
<em>data</em>, pointer to array of image data of type type;
<em>dx</em>, width of image;
<em>dy</em>, height of image;
<em>bpp</em>, bits per pixel of the image;
<em>xmin</em>, minimum value of coordinate associated with image
   x direction;
<em>xmax</em>, maximum value of coordinate associated with image
   x direction;
<em>ymin</em>, minimum value of coordinate associated with image
   y direction;
<em>ymax</em>, maximum value of coordinate associated with image
   y direction;
<em>zmin</em>, minimum value of image data (for palette labeling);
   and
<em>zmax</em>, maximum value of image data (for palette labeling).
<p>
Returns	a pointer to a <tt>PD_image</tt>.
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
     PDBfile *file;
     PD_image *im;

     im = PD_make_image();
           .
           .
           .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_rel_image"><h2>PD_REL_IMAGE</h2></a>


<p>
<pre>
<i>C Binding: </i>  void PD_rel_image(PD_image *image)
<i>Fortran Binding: </i>none
<i>SX Binding: </i> none
<i>Python Binding: </i> none
</pre>
<p>

Release the space associated with a PD_image data structure.

Input to this function is:
<em>image</em>, a pointer to a <tt>PD_image</tt>.

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
     PD_image *image;
           .
           .
           .

     PD_rel_image(image);
           .
           .
           .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_autofix_denorm"><h2>PD_AUTOFIX_DENORM</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_autofix_denorm(PDBfile *file, int flag)
<i>Fortran Binding: </i>none
<i>SX Binding: </i> none
<i>Python Binding: </i> none
</pre>
<p>
Activate automatic zeroing out of denormalized float point primitives
during PDB read calls (PD_read, PD_read_as, PD_read_alt, PD_read_as_alt).
Does not handle C structures.

<p>
Input to this function is:
<em>file</em>, a PDB file handle.
<em>flag</em>, if flag is <tt>TRUE</tt>, turn automatic 
fixing of denormalized floats on for the file; otherwise turn it off.
<p>
Returns the old flag value that existed before the function was called.
<p>

<hr>
<b>C Examples</b>
<p>
File example:
<pre>
     #include "pdb.h"
 
     PDBfile *file;
     int old_value;
           .
           .
     /* Turn automatic fixing of denormalized floats on for subsequent read calls */         
     old_value = PD_autofix_denorm(file, TRUE);
           .
           .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_activate_cksum"><h2>PD_ACTIVATE_CKSUM</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_activate_cksum(PDBfile *file, int flag)
<i>Fortran Binding: </i>integer PFSCSM(integer fileid, int flag)
<i>SX Binding: </i> (pd-activate-cksum file flag)
<i>Python Binding: </i> XXX - missing
</pre>
<p>
Activate checksum handling functionality. There are three (3) modes that PDB
has available (which can be OR'd together):

<p>
<ul>
 <li>PD_MD5_OFF or 0</li>
 <li>PD_MD5_FILE or 1</li>
 <li>PD_MD5_RW or 2</li>
</ul>
 
<p> 
If <tt>flag</tt> is set to <tt>PD_MD5_OFF</tt> or <tt>FALSE</tt>, then nothing
is enabled and no checksums are calculated or added to the file. 
If <tt>flag</tt> is set to <tt>PD_MD5_FILE</tt> or <tt>TRUE</tt> on any open, 
writable file, then an MD5 checksum for the entire file will be placed in the file 
during any subsequent flush or close operation. 
If <tt>flag</tt> is set to <tt>PD_MD5_RW</tt> on any open, writable file, then
any subsequent PD_write will create a hidden MD5 checksum variable associated 
with the variable being written out and any subsequent PD_read will recalculate and 
compare MD5 checksums for variables read in (returning -1 and setting PD_error 
if the checksums do not match). 
If <tt>flag</tt> is set to <tt>(PD_MD5_FILE | PD_MD5_RW)</tt>, then both file and
variable checksums are handled.

<p>
Defaults to PD_MD5_OFF for any file that never had PD_MD5_FILE applied to it. 
PD_activate_cksum acts like a toggle, hence checksums can be turned off or on
for the file (PD_MD5_FILE) and/or for individual variables (PD_MD5_RW) at any time.
Note that the MD5 checksum done during PD_write/PD_read calls for variables
only currently works for data that contains no pointers. This is because memory 
addresses in pointers will change from run to run.

<p>
Input to this function is:
<em>file</em>, a PDB file handle.
<em>flag</em>, if flag is <tt>PD_MD5_FILE</tt> and/or <tt>PD_MD5_RW</tt>, turn checksum 
handling on for the file or individual variables, respectively; otherwise turn it off.
<p>
Returns the old flag value that existed before the function was called.
<p>
See also:
<a href="#pd_verify"><tt>PD_verify</tt></a>
<p>

<hr>
<b>C Examples</b>
<p>
File example:
<pre>
     #include "pdb.h"
 
     PDBfile *file;
     int old_value;
           .
           .

/* calculate a checksum for the entire file (only) on PD_flush or PD_close */
     old_value = PD_activate_cksum(file, PD_MD5_FILE);
           .
           .

/* this also calculates a checksum for the entire file and adds it in */      
     PD_close(file);

</pre>

<p>
Variable example:
<pre>
     #include "pdb.h"
 
     PDBfile *file;
     int old_value, retval;
     float x;
           .
           .

/* turn on both file and variable (read/write) checksums */      
     old_value = PD_activate_cksum(file, PD_MD5_FILE | PD_MD5_RW);
           .
           .

     if (!PD_write(file, "x", "float", &x))
        printf("%s", PD_get_error());
           .
           .

/* read and compares x's checksum with freshly computed one */      
     retval = PD_read(file, "x", &x); 

/* determine if read failed or checksum failed: check error */     
     if ((retval == FALSE) || (retval == -1))
        printf("%s", PD_get_error());         
           .
           .

/* this also calculates a checksum for the entire file and adds it in */      
     PD_close(file);      

</pre>
<hr>


<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pd_verify"><h2>PD_VERIFY</h2></a>


<p>
<pre>
<i>C Binding: </i>  int PD_verify(PDBfile *file)
<i>Fortran Binding: </i>none
<i>SX Binding: </i> none
<i>Python Binding: </i> none
</pre>
<p>

Verifies a file's integrity and determines if the file is corrupt. Uses MD5 checksums.

<p>
Input to this function is:
<em>file</em>, a PDB file handle.
<p>
Returns <tt>TRUE</tt> if there is an MD5 checksum for the entire file and it 
matches a freshly computed checksum, returns <tt>FALSE</tt> if there is an 
MD5 checksum for the entire file and it does not match 
a freshly computed checksum, and returns -1 if there is no checksum in the file to 
verify with.
<p>
See also:
<a href="#pd_activate_cksum"><tt>PD_activate_cksum</tt></a>
<p>

<hr>
<b>C Example</b>
<pre>
 #include "pdb.h"
 
     PDBfile *file;
     int valid;
           .
           .
           .
     valid = PD_verify(file);

     if (valid == -1)
       printf("PDB file contains no checksum to verify with\n");

     if (!valid)
       printf("PDB file is corrupt\n");
           .
           .
           .
</pre>
<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_datas"><h2>Data Structures in PDBLib</h2></a>


The data structures with which the PDBLib system works are passed back to the
application program as the result of the high level calls. This gives the
programmer access to a great deal of information about the PDB file, the
symbol table and the structure chart. Hopefully, this also makes the package
more powerful without pushing any special responsibility onto the programmer.
<p>
For application developers who require all of the information from a PDB
file, the hash lookup function, <tt>SC_def_lookup</tt>, can be used to obtain symbol
table entries (<tt>syment</tt>) and structure definitions (<tt>defstr</tt>) from the symbol
table or structure chart. Examples can be found in the section at the end
of the manual.
<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_align_s"><h2>DATA_ALIGNMENT</h2></a>


The set of alignments for the primitive types is kept in a structure called
data_alignment. Its actual definition is:
<p>

<pre>
 struct s_data_alignment
    {int char_alignment;
     int ptr_alignment;
     int bool_alignment;
     int fx[N_PRIMITIVE_FIX];
     int fp[N_PRIMITIVE_FP];};
     int struct_alignment;};
 
 typedef struct s_data_alignment data_alignment;
</pre>

The term alignment refers to the fact that many CPU's require certain
data types to begin at memory locations whose addresses are even multiples
of some integer number of bytes. So for example, to say that the alignment
of a double is 8 means that a double must begin at an address which is a
multiple of 8.
<p>
Compilers hide this concept from almost all applications. PDBLib is one
that must know data alignments precisely. It employs a structure called
a data_alignment to record the alignments of the default primitive data
types which PDBLib supports. See the discussion of the data_alignment
structure in the section on data structures.
<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_std_s"><h2>DATA_STANDARD</h2></a>


The set of information describing all of the primitive data types is
organized into a structure called a data_standard. The data_standard
characterizes the CPU architecture because all types are either primitive
or derived from known types.
<p>
Its actual definition is:
<p>

<pre>

struct s_fixdes
   {int bpi;
    PD_byte_order order;};

struct s_fpdes
   {int bpi;
    long *format;
    int *order;};

struct s_data_standard
   {int bits_byte;
    int ptr_bytes;
    int bool_bytes;
    fixdes fx[N_PRIMITIVE_FIX];
    fpdes  fp[N_PRIMITIVE_FP];
    PDBfile *file;};
 
typedef struct s_data_standard data_standard;
</pre>

<h4>Floating Point Format Descriptor</h4>

The integer types only require a number of bytes and their order. The
floating point types require additional information describing the bit
layout of the components of the number: the sign bit; the exponent; and
the mantissa. These are given as an array of 8 long integers as follows:
<p>

<dd>format[0] = # of bits per number
<dd>format[1] = # of bits in exponent
<dd>format[2] = # of bits in mantissa
<dd>format[3] = start bit of sign
<dd>format[4] = start bit of exponent
<dd>format[5] = start bit of mantissa
<dd>format[6] = high order mantissa bit
<dd>format[7] = bias of exponent


<h4>Byte Ordering</h4>

There is much discussion in the literature about little endian and big
endian machines. Those two refer to two possible byte orderings for
binary data. That is not the most general way to talk about byte ordering
however. In fact the VAX format exemplifies the need for generality. PDBLib
simply uses an array of integers which describe the order of the bytes
in memory relative to CPU's such as the Motorola and SPARC families.
<p>

<tt>
<BLOCKQUOTE>
<TABLE>
<TR><TD>float4_nrm_order</TD><TD>= {1, 2, 3, 4}</TD></TR>
<TR><TD>float8_nrm_order</TD><TD>= {1, 2, 3, 4, 5, 6, 7, 8}</TD></TR>
<TR><TD>float12_nrm_order</TD><TD>= {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}</TD></TR>
<TR><TD>float4_rev_order</TD><TD>= {4, 3, 2, 1}</TD></TR>
<TR><TD>float8_rev_order</TD><TD>= {8, 7, 6, 5, 4, 3, 2, 1}</TD></TR>
<TR><TD>float4_vax_order</TD><TD>= {2, 1, 4, 3}</TD></TR>
<TR><TD>float8_vax_order</TD><TD>= {2, 1, 4, 3, 6, 5, 8, 7}</TD></TR>
</TABLE>
</BLOCKQUOTE>
</tt>

<p>

The following is a partial list of the data_standard's which PDBLib provides
by default. The purpose is twofold: to help users identify target
formats; and to guide users who wish to create their own data_standard's.
<p>
The members of the data_standard are indicated in the template:
<p>

<dl>

<dt> STANDARD

<dd>{size of pointer,
<dd>	 size and order of short,
<dd>	 size and order of int,
<dd>	 size and order of long,
<dd>	 size and order of long long,
<dd>	 size, format, and order of float,
<dd>	 size, format, and order of double,
<dd>	 size, format, and order of long double}
</dl>

The various variables indicated are defined by PDBLib.
<p>
<p>

<tt>
<dl>
<dt> X86_64_STD
<dd>{8,
<dd>&nbsp;2, REVERSE_ORDER,
<dd>&nbsp;4, REVERSE_ORDER,
<dd>&nbsp;8, REVERSE_ORDER,
<dd>&nbsp;8, REVERSE_ORDER,
<dd>&nbsp;4, float4_ieee, float4_rev_order,
<dd>&nbsp;8, float8_ieee, float8_rev_order}
<dd>16, float16_ieee, float16_rev_order}
<p>

<dt> PPC64_STD
<dd>{8,
<dd>&nbsp;2, NORMAL_ORDER,
<dd>&nbsp;4, NORMAL_ORDER,
<dd>&nbsp;8, NORMAL_ORDER,
<dd>&nbsp;8, NORMAL_ORDER,
<dd>&nbsp;4, float4_ieee, float4_nrm_order,
<dd>&nbsp;8, float8_ieee, float8_nrm_order,
<dd>16, float16_ieee, float16_nrm_order}
<p>

<dt> M68X_STD
<dd>{4,
<dd>&nbsp;2, NORMAL_ORDER,
<dd>&nbsp;2, NORMAL_ORDER,
<dd>&nbsp;4, NORMAL_ORDER,
<dd>&nbsp;4, NORMAL_ORDER,
<dd>&nbsp;4, float4_ieee, float4_nrm_order,
<dd>12, float12_ieee, float12_nrm_order,
<dd>16, float16_ieee, float16_nrm_order}
<p>

<dt> VAX_STD
<dd>{4,
<dd>&nbsp;2, REVERSE_ORDER,
<dd>&nbsp;4, REVERSE_ORDER,
<dd>&nbsp;4, REVERSE_ORDER,
<dd>&nbsp;4, REVERSE_ORDER,
<dd>&nbsp;4, float4_vax, float4_vax_order,
<dd>&nbsp;8, float8_vax, float8_vax_order,
<dd>&nbsp;8, float8_vax, float8_vax_order}
<p>

<dt> CRAY_STD
<dd>{8,
<dd>&nbsp;8, NORMAL_ORDER,
<dd>&nbsp;8, NORMAL_ORDER,
<dd>&nbsp;8, NORMAL_ORDER,
<dd>&nbsp;8, NORMAL_ORDER,
<dd>&nbsp;8, float8_cray, float8_nrm_order,
<dd>&nbsp;8, float8_cray, float8_nrm_order,
<dd>16, float16_cray, float16_nrm_order}

</dl>
</tt>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_defstr_s"><h2>DEFSTR</h2></a>


An entry in the structure chart is represented by a structure called a
<tt>defstr</tt>. It contains information about the data type such as the type
name, the byte size and alignment, and a list of members.
<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_dimdes_s"><h2>DIMDES</h2></a>


A dimdes or dimension descriptor contains the information necessary
to characterize a list of dimension specifications. It contains such
information as the minimum and maximum values the dimension index may
have and the net size of the dimension index range.
<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_memdes_s"><h2>MEMDES</h2></a>


A <tt>memdes</tt> or member descriptor is the structure used to contain the
information about a member of a <tt>defstr</tt>. It contains information about
the type of the member, the name of the member, any dimensions which
the member may have, and any casts which have been defined via <tt>PD_cast</tt>.
<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_pdbfile_s"><h2>PDBFILE</h2></a>


The PDBfile is the analog to the FILE structure in standard C I/O. In
fact, the PDBfile contains a FILE pointer to access the file via the
standard C library functions. In addition, the PDBfile contains
information such as: the symbol table, the structure charts for the file
and the host platform; data_standard's and data_alignment's for the file
and the host platform; and a modification date.
<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_syment_s"><h2>SYMENT</h2></a>


Just as the <tt>defstr</tt> type describes entries in the structure chart the
<tt>syment</tt> type describes entries in the symbol table. The <tt>syment</tt> includes
information about the data type of the entry, the number of elements,
the dimensions of the entry, and its disk address.
<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_examp"><h2>PDBLib by Example</h2></a>


The following code fragments illustrate the functionality of PDBLib. Some
of the code is taken from the validation suite and some from the library
itself.
<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_ex_wrk"><h2>Working with PDB files</h2></a>


This routine is taken from the validation suite for PDBLib. In it, a target
for the PDB file is chosen with the routine <em>test_target</em> (see the
section on <tt>PD_target</tt> for the definition of this function), a PDB
file created, some structures defined, data written, and the file closed.
The file is then reopened in append mode, some more data written to the file,
and the file is closed again. Finally, the file is opened in read mode, the
data read, some comparisons done, and the file is closed. The read and write
operations are hidden in this example. The significance of the example is
that a PDB file is created, closed, and opened in both append and read-only
mode.
<p>

<pre>

 /*-----------------------------------------------------------------*/
 /*-----------------------------------------------------------------*/
 
 test_1(base, tgt, n)
    char *base, *tgt;
    int n;
    {PDBfile *strm;
     char datfile[MAXLINE], fname[MAXLINE];
     int err;
     FILE *fp;
 
 /* target the file as asked */
     test_target(tgt, base, n, fname, datfile);
 
     fp = fopen(fname, "w");
 
 /* create the named file */
     if ((strm = PD_open(datfile, "w")) == NULL)
        {fprintf(fp, "Test couldn't create file %s\r\n", datfile);
         exit(1);};
     fprintf(fp, "File %s created\n", datfile);
 
     prep_test_1_data();
 
 /* make a few defstructs */
     PD_defstr(strm, "l_frame",
               "float x_min", "float x_max", "float y_min",
               "float y_max", LAST);
     PD_defstr(strm, "plot",
               "float x_axis(10)", "float y_axis(10)", "integer npts", 
               "char * label", "l_frame view", LAST);
 
 /* write the test data */
     write_test_1_data(strm);
 
 /* close the file */
     if (!PD_close(strm))
        {fprintf(fp, "Test couldn't close file %s\r\n", datfile);
         exit(1);};
     fprintf(fp, "File %s closed\n", datfile);
 
 /* reopen the file to append */
     if ((strm = PD_open(datfile, "a")) == NULL)
        {fprintf(fp, "Test couldn't open file %s to append\r\n",
                 datfile);
         exit(1);};
     fprintf(fp, "File %s opened to append\n", datfile);
 
     append_test_1_data(strm);
 
 /* close the file after append */
     if (!PD_close(strm))
        {fprintf(fp, "Test couldn't close file %s after append\r\n",
                 datfile);
         exit(1);};
     fprintf(fp, "File %s closed after append\n", datfile);
 
 /* reopen the file */
     if ((strm = PD_open(datfile, "r")) == NULL)
        {fprintf(fp, "Test couldn't open file %s\r\n", datfile);
         exit(1);};
     fprintf(fp, "File %s opened\n", datfile);
 
 /* dump the symbol table */
     dump_test_symbol_table(fp, strm-&gt;symtab, 1);
 
 /* read the data from the file */
     read_test_1_data(strm);
 
 /* compare the original data with that read in */
     err = compare_test_1_data(strm, fp);
 
 /* close the file */
     if (!PD_close(strm))
       {fprintf(fp, "Test couldn't close file %s\r\n", datfile);
        exit(1);};
     fprintf(fp, "File %s closed\n", datfile);
 
 /* print it out to stdout */
     print_test_1_data(fp);
 
     fclose(fp);
 
     return(err);}

 /*-----------------------------------------------------------------*/
 /*-----------------------------------------------------------------*/
 
</pre>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_ex_write"><h2>Writing Data to PDB files</h2></a>


These two routines exemplify the various write routines of PDBLib. In
particular, they were built to test the spectrum of write calls. Notice
their demonstration of the rules for write operations: the variable must
be a pointer to data of the type specified.
<p>
The identifiers beginning with `N_' are `#defined' constants whose values
are irrelevant to these examples.
<p>

<pre>

 static char
  cs_w,
  ca_w[N_CHAR],
  *cap_w[N_DOUBLE];
 
 static short
  ss_w,
  sa_w[N_INT];
 
 static int
  is_w,
  ia_w[N_INT],
  p_w[N_INT],
  len;
 
 static float
  fs_w,
  fa2_w[N_FLOAT][N_DOUBLE];
 
 static double
  ds_w,
  da_w[N_FLOAT];
 
 static plot
  graph_w;
 
 static l_frame
  view_w;
 
 static lev1
  *tar_w;
 
 /*-----------------------------------------------------------------*/
 /*-----------------------------------------------------------------*/
 
 void write_test_1_data(strm)
    PDBfile *strm;
    {
 
 /* write scalars into the file */
     PD_write(strm, "cs", "char", &amp;cs_w);
     PD_write(strm, "ss", "short", &amp;ss_w);
     PD_write(strm, "is", "integer", &amp;is_w);
     PD_write(strm, "fs", "float", &amp;fs_w);
     PD_write(strm, "ds", "double", &amp;ds_w);
 
 /* write primitive arrays into the file */
     PD_write(strm, "sa(5)", "short", sa_w);
     PD_write(strm, "ia(5)", "integer", ia_w);
 
 /* write structures into the file */
     PD_write(strm, "view", "l_frame", &amp;view_w);
     PD_write(strm, "graph", "plot", &amp;graph_w);
 
     return;}
 
 /*-----------------------------------------------------------------*/
 /*-----------------------------------------------------------------*/
 
 void write_test_2_data(strm)
    PDBfile *strm;
    {
 
    PD_write(strm, "tar", "lev1 *", &amp;tar_w);
 
     return;}
 
 /*-----------------------------------------------------------------*/
 /*-----------------------------------------------------------------*/

</pre>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_ex_read"><h2>Reading Data from PDB files</h2></a>


These two routines exemplify the various read routines of PDBLib. In
particular, they were built to test the spectrum of read operations.
They read the data written out in the previous example. Notice their
demonstration of the rules for read operations: the variable must be a
pointer to data of the type specified. This is often a more difficult
proposition for reads since the type information isn't supplied in the
read call.
<p>
The member read operations at the end of the first routine should be studied
carefully. They are not the most general read example, but they are among
the most useful.
<p>
The second routine reads not only the entire structure, but picks out each
part individually. It demonstrates the rule about dereferencing pointers
in the partial read operation. Study the structures of this example carefully!
<p>
The identifiers beginning with `N_' are `#defined' constants whose values are
irrelevant to these examples. Also, take for granted that the unspecified
variables to contain parts of the structures have the correct declarations.
<p>

<pre>
 
 struct s_lev2
  {char **s;
  int type;};
 
 typedef struct s_lev2 lev2;
 
 struct s_lev1
  {int *a;
  double *b;
  lev2 *c;};
 
 typedef struct s_lev1 lev1;
 
 static char
  cs_r,
  ca_r[N_CHAR],
  *cap_r[N_DOUBLE];
 
 static short
  ss_r,
  sa_r[N_INT];
 
 static int
  is_r,
  ia_r[N_INT];
 
 static float
  fs_r,
  fs_app_r,
  fs_p1_r,
  fs_p2_r,
  fs_p3_r,
  fa2_r[N_FLOAT][N_DOUBLE],
  fa2_app_r[N_FLOAT][N_DOUBLE];
 
 static double
  ds_r,
  da_r[N_FLOAT];
 
 static plot
  graph_r;
 
 static l_frame
  view_r;
 
 static lev1
  *tar_r;
 
 /*-----------------------------------------------------------------*/
 /*-----------------------------------------------------------------*/
 
 void read_test_1_data(strm)
    PDBfile *strm;
    {
 
 /* read the scalar data from the file */
     PD_read(strm, "cs", &amp;cs_r);
     PD_read(strm, "ss", &amp;ss_r);
     PD_read(strm, "is", &amp;is_r);
     PD_read(strm, "fs", &amp;fs_r);
     PD_read(strm, "ds", &amp;ds_r);
 
 /* read the primitive arrays from the file */
     PD_read(strm, "ca", ca_r);
     PD_read(strm, "sa", sa_r);
     PD_read(strm, "ia", ia_r);
     PD_read(strm, "fa2", fa2_r);
     PD_read(strm, "da", da_r);
     PD_read(strm, "cap", cap_r);
 
 /* read the entire structures from the file */
     PD_read(strm, "view", &amp;view_r);
     PD_read(strm, "graph", &amp;graph_r);
 
 /* read the appended data from the file */
     PD_read(strm, "fs_app", &amp;fs_app_r);
     PD_read(strm, "fa2_app", fa2_app_r);
 
 /* struct member test */
     PD_read(strm, "graph.view.x_max", &amp;fs_p2_r);
     PD_read(strm, "view.y_max", &amp;fs_p3_r);
 
     return;}
 
 /*-----------------------------------------------------------------*/
 /*-----------------------------------------------------------------*/
 
 /* READ_TEST_2_DATA - read the test data from the file */
 
 void read_test_2_data(strm)
    PDBfile *strm;
    {do_r = strm-&gt;default_offset;
 
     PD_read(strm, "tar", &amp;tar_r);
     PD_read(strm, "p", p_r);
 
     PD_read(strm, "tar(0).a", &amp;ap1);
     PD_read(strm, "tar(1).a", &amp;ap2);
 
     PD_read(strm, "tar(0).a(0)", &amp;aa[0]);
     PD_read(strm, "tar(0).a(1)", &amp;aa[1]);
     PD_read(strm, "tar(1).a(0)", &amp;aa[2]);
     PD_read(strm, "tar(1).a(1)", &amp;aa[3]);
 
     PD_read(strm, "tar(0).b", &amp;bp1);
     PD_read(strm, "tar(1).b", &amp;bp2);
 
     PD_read(strm, "tar(0).b(0)", &amp;ba[0]);
     PD_read(strm, "tar(0).b(1)", &amp;ba[1]);
     PD_read(strm, "tar(1).b(0)", &amp;ba[2]);
     PD_read(strm, "tar(1).b(1)", &amp;ba[3]);
 
     PD_read(strm, "tar(0).c", &amp;cp1);
     PD_read(strm, "tar(1).c", &amp;cp2);
 
     PD_read(strm, "tar(0).c(0)", &amp;ca[0]);
     PD_read(strm, "tar(0).c(1)", &amp;ca[1]);
     PD_read(strm, "tar(1).c(0)", &amp;ca[2]);
     PD_read(strm, "tar(1).c(1)", &amp;ca[3]);
 
     PD_read(strm, "tar(0).c(0).s", &amp;sp1);
     PD_read(strm, "tar(0).c(1).s", &amp;sp2);
     PD_read(strm, "tar(1).c(0).s", &amp;sp3);
     PD_read(strm, "tar(1).c(1).s", &amp;sp4);
 
     PD_read(strm, "tar(0).c(0).s(0)", &amp;tp1);
     PD_read(strm, "tar(0).c(0).s(1)", &amp;tp2);
     PD_read(strm, "tar(0).c(1).s(0)", &amp;tp3);
     PD_read(strm, "tar(0).c(1).s(1)", &amp;tp4);
 
     PD_read(strm, "tar(0).c(0).s(0)(2)", &amp;ta[0]);
     PD_read(strm, "tar(0).c(0).s(1)(1)", &amp;ta[1]);
     PD_read(strm, "tar(0).c(1).s(0)(3)", &amp;ta[2]);
     PD_read(strm, "tar(0).c(1).s(1)(2)", &amp;ta[3]);
 
     PD_read(strm, "tar(1).c(0).s(0)", &amp;tp5);
     PD_read(strm, "tar(1).c(0).s(1)", &amp;tp6);
     PD_read(strm, "tar(1).c(1).s(0)", &amp;tp7);
     PD_read(strm, "tar(1).c(1).s(1)", &amp;tp8);
 
     return;}

 /*-----------------------------------------------------------------*/
 /*-----------------------------------------------------------------*/
 
</pre>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_ex_append"><h2>Appending Data to PDB file variables</h2></a>


The functions <a href="#pd_defent"><tt>PD_defent</tt></a>,
<a href="#pd_write"><tt>PD_write</tt></a>, and
<a href="#pd_append"><tt>PD_append</tt></a> (and their alternate forms)
may be used in several
combinations.  Users have a variety of scenarios for writing their
data.  These scenarios are driven by considerations such as: data flow in
the application; efficiency of writing the data; or requirements of the
application reading the data.
<p>
The following examples are taken from the PDB test suite and demonstrate
several combinations leaving the rationale to the user.  In each example,
we are writing a variable IA in three pieces of 5 integers each.  The
net result is an array which is of dimension (3,5) if the file is set
to use row major ordering or (5,3) if the file is set to use column
major ordering.
<p>

<h3>Append Example #1</h3>

This is perhaps the simplest case.  A variable is written with PD_write
and subsequently append to twice with PD_append.

<pre>
 
 /*-----------------------------------------------------------------*/
 /*-----------------------------------------------------------------*/

 /* WRITE_TEST_1_DATA - write out the data into the PDB file
  *                   - this is write, append, append
  *                   - row major order
  */

 static int write_test_1_data(PDBfile *strm)
    {

 /* write an array into the file */
     if (!PD_write(strm, "ia(1,5)", "integer", ia_w0))
        {printf("IA WRITE FAILED - WRITE_TEST_1_DATA\n");
         return(FALSE);};

 /* append data to ia */
     if (!PD_append(strm, "ia(1:1,0:4)", ia_w1))
        {printf("IA APPEND 1 FAILED - WRITE_TEST_1_DATA\n");
         return(FALSE);};

 /* append more data to ia */
     if (!PD_append(strm, "ia(2:2,0:4)", ia_w2))
        {printf("IA APPEND 2 FAILED - WRITE_TEST_1_DATA\n");
         return(FALSE);};

     return(TRUE);}

 /*-----------------------------------------------------------------*/
 /*-----------------------------------------------------------------*/
 
</pre>


<h3>Append Example #2</h3>

This is the same as Example #1 above except that the file has been set
to use column major ordering. Compare the index expressions with those
in Example #1.

<pre>
 
 /*-----------------------------------------------------------------*/
 /*-----------------------------------------------------------------*/

 /* WRITE_TEST_2_DATA - write out the data into the PDB file
  *                   - this is write, append, append
  *                   - row major order
  */

 static int write_test_2_data(PDBfile *strm)
    {

 /* write an array into the file */
     if (!PD_write(strm, "ia(5,1)", "integer", ia_w0))
        {printf("IA WRITE FAILED - WRITE_TEST_2_DATA\n");
         return(FALSE);};

 /* append data to ia */
     if (!PD_append(strm, "ia(0:4,1:1)", ia_w1))
        {printf("IA APPEND 1 FAILED - WRITE_TEST_2_DATA\n");
         return(FALSE);};

 /* append more data to ia */
     if (!PD_append(strm, "ia(0:4,2:2)", ia_w2))
        {printf("IA APPEND 2 FAILED - WRITE_TEST_2_DATA\n");
         return(FALSE);};

     return(TRUE);}

 /*-----------------------------------------------------------------*/
 /*-----------------------------------------------------------------*/
 
</pre>


<h3>Append Example #3</h3>

In this example, some space is reserved for variable.  Data is then
written to into the reserved space.  Subsquently, more data is appended
to variable.

<pre>
 
 /*-----------------------------------------------------------------*/
 /*-----------------------------------------------------------------*/

 /* WRITE_TEST_3_DATA - write out the data into the PDB file
  *                   - this is defent, write, append, append
  *                   - row major order
  */

 static int write_test_3_data(PDBfile *strm)
    {

 /* reserve space for an array in the file */
     if (!PD_defent(strm, "ia(1,5)", "integer"))
        {printf("IA WRITE FAILED - WRITE_TEST_3_DATA\n");
         return(FALSE);};

 /* write data to the reserved space */
     if (!PD_write(strm, "ia(0:0,0:4)", "integer", ia_w0))
        {printf("IA WRITE FAILED - WRITE_TEST_3_DATA\n");
         return(FALSE);};

 /* append data to ia */
     if (!PD_append(strm, "ia(1:1,0:4)", ia_w1))
        {printf("IA APPEND 1 FAILED - WRITE_TEST_3_DATA\n");
         return(FALSE);};

 /* append more data to ia */
     if (!PD_append(strm, "ia(2:2,0:4)", ia_w2))
        {printf("IA APPEND 2 FAILED - WRITE_TEST_3_DATA\n");
         return(FALSE);};

     return(TRUE);}

 /*-----------------------------------------------------------------*/
 /*-----------------------------------------------------------------*/
 
</pre>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<h3>Append Example #4</h3>

In this example, some space is reserved for variable.  Data is then
written to into the reserved space.  Subsquently, another block of
space is reserved for the variable and later filled in with
calls to PD_write.

<pre>
 
 /*-----------------------------------------------------------------*/
 /*-----------------------------------------------------------------*/

 /* WRITE_TEST_4_DATA - write out the data into the PDB file
  *                   - this is defent, write, defent, write, write
  *                   - column major order
  */

 static int write_test_4_data(PDBfile *strm)
    {

 /* reserve space for an array in the file */
     if (!PD_defent(strm, "ia(5,1)", "integer"))
        {printf("IA DEFENT 1 FAILED - WRITE_TEST_4_DATA\n");
         return(FALSE);};

 /* write data to the reserved space */
     if (!PD_write(strm, "ia(0:4,0:0)", "integer", ia_w0))
        {printf("IA WRITE FAILED - WRITE_TEST_4_DATA\n");
        return(FALSE);};

 /* reserve more space for ia */
     if (!PD_defent(strm, "ia(0:4,1:2)", "integer"))
        {printf("IA DEFENT 2 FAILED - WRITE_TEST_4_DATA\n");
         return(FALSE);};

 /* write data to the some of the additional reserved space */
     if (!PD_write(strm, "ia(0:4,1:1)", "integer", ia_w1))
        {printf("IA APPEND 1 FAILED - WRITE_TEST_4_DATA\n");
         return(FALSE);};

 /* write data to the rest of the additional reserved space */
     if (!PD_write(strm, "ia(0:4,2:2)", "integer", ia_w2))
        {printf("IA APPEND 2 FAILED - WRITE_TEST_4_DATA\n");
         return(FALSE);};

     return(TRUE);}

 /*-----------------------------------------------------------------*/
 /*-----------------------------------------------------------------*/
 
</pre>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_ex_inq"><h2>Inquiries in PDBLib</h2></a>


The following fragments show how to obtain information about PDB files and
their contents.
<p>

<pre>
 
 /*-----------------------------------------------------------------*/
 /*-----------------------------------------------------------------*/
 
 myread(PDBfile *file, char *name, void *var,
	long offset, long number)
    {long addr, num;
     char *token, *type, memb[MAXLINE];
     dimdes *dims;
     syment *ep;
 
     strncpy(memb, name, MAXLINE);
     token = strtok(memb, ".([");
 
 /* look up the variable name */
     ep = PD_query_entry(file, token, NULL);
     if (ep == NULL)
        printf("%s", PD_get_error());
 
     addr = PD_entry_address(ep);
     dims = PD_entry_dimensions(ep);
     num  = PD_entry_number(ep);
     type = PD_entry_type(ep);
 
 /* with ep in hand, we know the variable type, number of elements,
  * dimensions, and disk address
  */
               .
               .
               .
 
 /*-----------------------------------------------------------------*/
 /*-----------------------------------------------------------------*/
 
 _PD_wr_leaf(PDBfile *file, char *var, long nitems, char *type)
    {char *svar;
     int size;
     FILE *fp;
     defstr *dp;
     memdes *desc, *mem_lst;
 
     fp = file-&gt;stream;
            .
            .
            .
 
 /* dispatch all other writes */
     if (file-&gt;conversions)
            .
            .
            .
 
 /* obtain a pointer to the defstr associated with type */
     dp = PD_inquire_host_type(file, type);
     if (dp == NULL)
        printf("%s", PD_get_error());
 
 /* if the structure has any pointered members loop over the members */
     if (dp-&gt;n_indirects &amp;&amp; ((mem_lst = dp-&gt;members) != NULL))
        {size = dp-&gt;size;
         for (svar = var, offset = 0L, i = 0L; i &#60;; nitems; i++)
             {for (desc = mem_lst; desc != NULL; desc = desc-&gt;next)
                  { ... };
              svar += size;};};
            .
            .
            .
 
 /*-----------------------------------------------------------------*/
 /*-----------------------------------------------------------------*/

</pre>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_ex_brws"><h2>Browsing PDB files</h2></a>


The following fragment shows how to browse around in PDB files.  Note the
use of <tt>PD_ls</tt>.  What this example doesn't show is checking for the directory
type and perhaps using <tt>PD_cd</tt> to move to other directories.
<p>

<pre>

    int i, nd, ne;
    long ni;
    long *dims;
    char **names, *fname, *type;
    syment *ep;
    PDBfile *file;

    fname = "/tmp/fubar.pdb";
    file = PD_open(fname, "r");
    if (file == NULL)
       {printf("Error opening %s\n", fname);
	exit(1);};

    printf("\nSymbol Table:\n");

/* print contents of symbol table */
    names = PD_ls(file, NULL, NULL, &amp;ne);
    for (i = 0; i < ne; i++)
	{if (names[i] != NULL)
            {ep = PD_query_entry(file, names[i], NULL);
	     rv = PD_get_entry_info(ep, &amp;type, &amp;ni, &amp;nd, &amp;dims);
	     if (rv)
	        {printf("%20s %10s %8ld %2d (%p)\n",
			names[i], type, ni, nd, dims);
		 PD_free_entry_info(type, dims);};};};

    CFREE(names);

/* close file */
    PD_close(file);

</pre>
 
<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_ex_map"><h2>Writing <tt>PM_mappings</tt> with <tt>PFWMAP</tt></h2></a>


The following fragment shows how to write a <tt>PM_mapping</tt> to a PDB file from
a FORTRAN program. This example uses <tt>PFWMAP</tt>.
<tt>PFWMAP</tt> writes out both domain
and range as a complete mapping. The drawback to this approach is that
many mappings might have the same domain which would be written out over
and over again. This could lead to unacceptably large data files. See the
next example for a more space saving approach.
<p>

<pre>
 
 c-----------------------------------------------------------------
 c-----------------------------------------------------------------

  subroutine sxdmp(namesx, namelen)
 
 c traverse plot list to make dumps suitable for sx
 
  integer pfopen, pfclos, pfgerr, pfwmap, pfwset, pfwran
  integer zscanleq
  
  integer pderr(32), rp(6), dp(6)
  integer fileid, pim, dmnsn
 
  double precision rm(nsavept), dm(nsavept)
 
 c ... set up
             .
             .
             .
 
 c ... open the file
  fileid = pfopen(namelen, namesx, 'w')
  if (fileid .eq. -1) then
     iplpderr = pfgerr(nchr, pderr)
     call errproc(pderr, nchr, 0, 0)
  endif
 
  pim = 0
 
 c ... loop over plots
  do ...
 
 c ... setup domain
  dname = &#60;;domain name&gt;
  dp(1) = &#60;;length of domain name&gt;
 
  select case (dmnsn)
  case ('d2r1')
     dp(2) = 2
     dp(3) = 2
     dp(4) = nr
     dp(5) = kmax
     dp(6) = lmax
     call zmovewrd(dm(1), x2d, nr)
     call zmovewrd(dm(nr+1), y2d, nr)
  case ('d1r1')
     dp(2) = 1
     dp(3) = 1
     dp(4) = nr
     dp(5) = nr
     call zmovewrd(dm(1), &#60;;domain data&gt;, nr)
  endselect
 
 c ... set up range
  rname = &#60;;range name&gt;
  rp(1) = &#60;;length of range name&gt;
 
  rp(2) = dp(2)
  rp(3) = 1
  rp(4) = nr
  rp(5) = dp(5)
  rp(6) = dp(6)
  call zmovewrd(rm(1), &#60;;range data&gt;, nr)
 
 c ... write the mapping
  ierr = pfwmap(fileid, dname, dp, dm, rname, rp, rm, pim)
  if (ierr.eq.0) then
     pfgerr(nchr, pderr)
     call errproc(pderr, nchr, 0, 0)
  endif
  pim = pim + 1
  repeat
 
 c ... close the file
  icloseok = pfclos(fileid)
  if (icloseok.eq.0) then
     pfgerr(nchr, pderr)
     call errproc(pderr, nchr, 0, 0)
  endif
 
  return
  end

 c-----------------------------------------------------------------
 c-----------------------------------------------------------------

</pre>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_ex_wsr"><h2>Writing <tt>PM_mappings</tt> with <tt>PFWSET</tt> and <tt>PFWRAN</tt></h2></a>


The following fragment shows how to write a <tt>PM_mapping</tt> to a
PDB file from a FORTRAN program. This example uses <tt>PFWSET</tt> and
<tt>PFWRAN</tt>.  With <tt>PFWSET</tt> and <tt>PFWRAN</tt> an
application can write mappings in the most space efficient way.
Instead of writing the same domains over and over again as would be
done with <tt>PFWMAP</tt>, the application can select the unique
domains and write them out with <tt>PFWSET</tt>. Then all mappings can
be written using <tt>PFWRAN</tt> which writes out a
<tt>PM_mapping</tt> with a null domain. The post processing tools
(such as PDBView) reconstruct the complete <tt>PM_mapping</tt> by
looking for the domain as a variable with the same name as the domain
component of the mapping name.  This approach is clearly a little more
involved than using <tt>PFWMAP</tt> and application developers should
weigh the advantages and disadvantages before selecting one method
over the other. See the previous example for an illustration of the
use of <tt>PFWMAP</tt>.
<p>

<pre>
 
 c-----------------------------------------------------------------
 c-----------------------------------------------------------------

  subroutine sxdmp(namesx, namelen)
 
 c traverse plot list to make dumps suitable for sx
 
  integer pfopen, pfclos, pfgerr, pfwmap, pfwset, pfwran
  integer zscanleq
  
  integer pderr(32), rp(6), dp(6)
  integer fileid, pim, dmnsn
 
  double precision rm(nsavept), dm(nsavept)
 
 c ... set up
             .
             .
             .
 
 c ... open the file
  fileid = pfopen(namelen, namesx, 'w')
  if (fileid .eq. -1) then
     iplpderr = pfgerr(nchr, pderr)
     call errproc(pderr, nchr, 0, 0)
  endif
 
  pim = 0
 
 c ... loop over plots
  do ...
 
 c ... setup domain
  dname = &#60;;domain name&gt;
  dp(1) = &#60;;length of domain name&gt;
 
  select case (dmnsn)
  case ('d2r1')
     dp(2) = 2
     dp(3) = 2
     dp(4) = nr
     dp(5) = kmax
     dp(6) = lmax
     call zmovewrd(dm(1), x2d, nr)
     call zmovewrd(dm(nr+1), y2d, nr)
  case ('d1r1')
     dp(2) = 1
     dp(3) = 1
     dp(4) = nr
     dp(5) = nr
     call zmovewrd(dm(1), &#60;;domain data&gt;, nr)
  endselect
 
  if &#60;;unique domain&gt; then
     ierr = pfwset(fileid, dname, dp, dm)
     if (ierr.eq.0) then
        iplpderr = pfgerr(nchr, pderr)
        call errproc(pderr, nchr, 0, 0)
     endif
  endif
 
 c ... set up range
  rname = &#60;;range name&gt;
  rp(1) = &#60;;length of range name&gt;
 
  rp(2) = dp(2)
  rp(3) = 1
  rp(4) = nr
  rp(5) = dp(5)
  rp(6) = dp(6)
  call zmovewrd(rm(1), &#60;;range data&gt;, nr)
 
 c ... write out range
  ierr = pfwran(fileid, dname, dp(1), rname, rp, rm, pim)
  if (ierr.eq.0) then
     pfgerr(nchr, pderr)
     call errproc(pderr, nchr, 0, 0)
  endif
  pim = pim+1
  repeat
 
 c ... close the file
  icloseok = pfclos(fileid)
  if (icloseok.eq.0) then
     pfgerr(nchr, pderr)
     call errproc(pderr, nchr, 0, 0)
  endif
 
  return
  end

 c-----------------------------------------------------------------
 c-----------------------------------------------------------------

</pre>
 
<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_ex_par_smp"><h2>SMP Parallel Example</h2></a>


This section will illustrate the use of PDBLib in a threaded program.
<p>

<pre>

/*
 * PDSMP.C - Test smp i/o in PDBLib
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb.h"

#define NPTS 10

#define check_myplot1(xplot, rxplot)                                                                 \
        {int i;                                                                                      \
         if (strcmp(xplot.label, rxplot.label) != 0)                                                 \
            {printf("xplot.label differs from rxplot.label in %s\n", sname1);                        \
             printf("xplot.label: %s, rxplot.label: %s\n", xplot.label, rxplot.label);               \
             exit(1);}                                                                               \
                                                                                                     \
         if (xplot.view->x_min != rxplot.view->x_min)                                                \
            {printf("xplot.view->xmin differs from rxplot.view->xmin in %s\n", sname1);              \
             exit(1);}                                                                               \
                                                                                                     \
         if (xplot.view->x_max != rxplot.view->x_max)                                                \
            {printf("xplot.view->x_max differs from rxplot.view->x_max in %s\n", sname1);            \
             exit(1);}                                                                               \
                                                                                                     \
         if (xplot.view->y_min != rxplot.view->y_min)                                                \
            {printf("xplot.view->y_min differs from rxplot.view->y_min in %s\n", sname1);            \
             exit(1);}                                                                               \
                                                                                                     \
         if (xplot.view->y_max != rxplot.view->y_max)                                                \
            {printf("xplot.view->y_max differs from rxplot.view->y_max in %s\n", sname1);            \
             exit(1);}                                                                               \
                                                                                                     \
         if (xplot.npts != rxplot.npts)                                                              \
            {printf("xplot.npts differs from rxplot.npts in %s\n", sname1);                          \
             exit(1);}                                                                               \
                                                                                                     \
         for (i = 0; i < 10; i++)                                                                    \
             if (xplot.x_axis[i] != rxplot.x_axis[i])                                                \
                {printf("xplot.x_axis[%d] differs from rxplot.x_axis[%d] in %s\n", i, i, sname1);    \
                exit(1);};}            

#define check_myplot2(xplot, rxplot)                                                                 \
        {int i;                                                                                      \
         if (strcmp(xplot->label, rxplot->label) != 0)                                                   \
            {printf("xplot->label differs from rxplot->label in %s\n", sname1);                      \
             exit(1);}                                                                               \
                                                                                                     \
         if (xplot->view->x_min != rxplot->view->x_min)                                              \
            {printf("xplot->view->xmin differs from rxplot->view->xmin in %s\n", sname1);            \
             exit(1);}                                                                               \
                                                                                                     \
         if (xplot->view->x_max != rxplot->view->x_max)                                              \
            {printf("xplot->view->x_max differs from rxplot->view->x_max in %s\n", sname1);          \
             exit(1);}                                                                               \
                                                                                                     \
         if (xplot->view->y_min != rxplot->view->y_min)                                              \
            {printf("xplot->view->y_min differs from rxplot->view->y_min in %s\n", sname1);          \
             exit(1);}                                                                               \
                                                                                                     \
         if (xplot->view->y_max != rxplot->view->y_max)                                              \
            {printf("xplot->view->y_max differs from rxplot->view->y_max in %s\n", sname1);          \
             exit(1);}                                                                               \
                                                                                                     \
         if (xplot->npts != rxplot->npts)                                                            \
            {printf("xplot->npts differs from rxplot->npts in %s\n", sname1);                        \
             exit(1);}                                                                               \
                                                                                                     \
         for (i = 0; i < 10; i++)                                                                    \
             if (xplot->x_axis[i] != rxplot->x_axis[i])                                              \
                {printf("xplot->x_axis[%d] differs from rxplot->x_axis[%d] in %s\n", i, i, sname1);  \
                exit(1);};}        

struct s_l_frame
   {float x_min;
    float x_max;
    float y_min;
    float y_max;};

typedef struct s_l_frame l_frame;

struct s_plot
   {float x_axis[NPTS];
    float y_axis[NPTS];
    int npts;
    char *label;
    l_frame *view;};

typedef struct s_plot myplot;

float d[100], d2[100];

int   j[100], j2[100];
int n_iter = 10;

myplot mypl;

PDBfile *file, *file2;

int
 ok_count;

void *writeit(void *x);
void *writeit2(void *x);
void *readit(void *x);
void *readit2(void *x);
void print_help();

/*-----------------------------------------------------------------*/
/*-----------------------------------------------------------------*/
 
/* Write variables to the already opened file.  The number of variables written */
/* is a function of the number of iterations specified.                         */

void *writeit(void *arg)
   {int i, n;
    myplot *xp;
    char sname1[100], sname2[100], sname3[100], suffix[10];

    xp = CMAKE(myplot);
    xp->view = CMAKE(l_frame);

    xp->label = CSTRSAVE("Dynamic XP label");
    xp->view->x_min = 0.2;
    xp->view->x_max = 0.9;
    xp->view->y_min = 0.33;
    xp->view->y_max = 0.99;
    xp->npts       = 10;

    for (i = 0; i < 10; i++)
        xp->x_axis[i] = xp->y_axis[i] = (float)i * 4.0;
         
    strncpy(sname1, "mypl_wr1a", 100); 
    strncpy(sname2, "xpl_wr1a", 100); 
    strncpy(sname3, "mypl_wr1b", 100); 

    for (n = 0; n < n_iter; n++)    
        {snprintf(suffix, 10, "%d", n);

         strncpy(sname1+9, suffix, 91);
         strncpy(sname2+8, suffix, 92);
         strncpy(sname3+9, suffix, 91);

        if (!PD_write(file, sname1, "myplot", &amp;mypl))
           {printf("Error writing %s-exiting\n", sname1);
            exit(1);}

        if (!PD_write(file, sname2, "myplot *", &amp;xp))
           {printf("Error writing %s-exiting\n", sname2);
            exit(1);}

        if (!PD_write(file, sname3, "myplot", &amp;mypl))
           {printf("Error writing %s-exiting\n", sname3);
            exit(1);};}

    ok_count++;

    CFREE(xp->label);
    CFREE(xp->view);
    CFREE(xp);

    return(NULL);}
     
/*-----------------------------------------------------------------*/
/*-----------------------------------------------------------------*/
 
void *writeit2(void *arg)
   {int i, n;
    char sname1[100], sname2[100], suffix[100];
    myplot xplot;

    xplot.label = CSTRSAVE("Myplot Xplot label");
    xplot.view = CMAKE(l_frame);

    xplot.view->x_min = 0.15;
    xplot.view->x_max = 0.85;
    xplot.view->y_min = 0.25;
    xplot.view->y_max = 0.75;
    xplot.npts       = 10;

    for (i = 0; i < 10; i++)
        xplot.x_axis[i] = xplot.y_axis[i] = (float)i;

    strncpy(sname1, "mypl_wr2a", 100); 
    strncpy(sname2, "xpl_wr2a", 100); 

    for (n = 0; n < n_iter; n++)
        {snprintf(suffix, 10, "%d", n);

         strncpy(sname1+9, suffix, 91);
         strncpy(sname2+8, suffix, 92);

         if (!PD_write(file, sname1, "myplot", &amp;xplot))
            {printf("Error writing %s-exiting\n", sname1);
             exit(1);}       

         if (!PD_write(file, sname2, "myplot", &amp;xplot))
            {printf("Error writing %s-exiting\n", sname2);
             exit(1);};}

    ok_count++;

    CFREE(xplot.label);
    CFREE(xplot.view);
 
    return NULL;}
     
/*-----------------------------------------------------------------*/
/*-----------------------------------------------------------------*/
 
void *readit(void *arg)
   {int i, n;
    myplot *xp, *rxp, rmypl;
    char sname1[100], sname2[100], sname3[100], suffix[10];

    xp = CMAKE(myplot);
    xp->view = CMAKE(l_frame);

    xp->label = CSTRSAVE("Dynamic XP label");
    xp->view->x_min = 0.2;
    xp->view->x_max = 0.9;
    xp->view->y_min = 0.33;
    xp->view->y_max = 0.99;
    xp->npts       = 10;

    for (i = 0; i < 10; i++)
        xp->x_axis[i] = xp->y_axis[i] = (float)i * 4.0;

    strncpy(sname1, "mypl_wr1a", 100); 
    strncpy(sname2, "xpl_wr1a", 100); 
    strncpy(sname3, "mypl_wr1b", 100); 

    for (n = 0; n < n_iter; n++)    
        {snprintf(suffix, 10, "%d", n);

         strnpy(sname1+9, suffix, 91);
         strncpy(sname2+8, suffix, 92);
         strncpy(sname3+9, suffix, 91);

        if (!PD_read(file, sname1, &amp;rmypl))
           {printf("Error writing %s-exiting\n", sname1);
            exit(1);}

/* compare with original */
        check_myplot1(mypl, rmypl);

/* free the memory */    
        CFREE(rmypl.label);
        CFREE(rmypl.view);

        if (!PD_read(file, sname2, &amp;rxp))
           {printf("Error writing %s-exiting\n", sname2);
            exit(1);}

/* compare with original */
        check_myplot2(xp, rxp);

/* free the memory   */ 
        CFREE(rxp->label);
        CFREE(rxp->view);
        CFREE(rxp);

        if (!PD_read(file, sname3, &amp;rmypl))
           {printf("Error writing %s-exiting\n", sname3);
            exit(1);};

/* compare with original */
	 check_myplot1(mypl, rmypl);

/* free the memory    */
	 CFREE(rmypl.label);
	 CFREE(rmypl.view);};

    ok_count++;

    CFREE(xp->label);
    CFREE(xp->view);
    CFREE(xp);

    return NULL;}
     
/*-----------------------------------------------------------------*/
/*-----------------------------------------------------------------*/
 
void *readit2(void *arg)
   {int i, n;
    char sname1[100], sname2[100], suffix[100];
    myplot xplot, rxplot;

    xplot.label = CSTRSAVE("Myplot Xplot label");
    xplot.view = CMAKE(l_frame);

    xplot.view->x_min = 0.15;
    xplot.view->x_max = 0.85;
    xplot.view->y_min = 0.25;
    xplot.view->y_max = 0.75;
    xplot.npts       = 10;

    for (i = 0; i < 10; i++)
        xplot.x_axis[i] = xplot.y_axis[i] = (float)i;

    strncpy(sname1, "mypl_wr2a", 100); 
    strncpy(sname2, "xpl_wr2a", 100); 

    for (n = 0; n < n_iter; n++)
        {snprintf(suffix, 10, "%d", n);

         strncpy(sname1+9, suffix, 91);
         strncpy(sname2+8, suffix, 92);

         if (!PD_read(file, sname1, &amp;rxplot))
            {printf("Error writing %s-exiting\n", sname1);
             exit(1);}       

/* compare with original */
         check_myplot1(xplot, rxplot);

/* free the memory    */
         CFREE(rxplot.label);
         CFREE(rxplot.view);

         if (!PD_read(file, sname2, &amp;rxplot))
            {printf("Error writing %s-exiting\n", sname2);
             exit(1);};

/* compare with original */
         check_myplot1(xplot, rxplot);

/* free the memory    */
	 CFREE(rxplot.label);
	 CFREE(rxplot.view);};

    ok_count++;

    CFREE(xplot.label);
    CFREE(xplot.view);
 
    return(NULL);}
     
/*-----------------------------------------------------------------*/
/*-----------------------------------------------------------------*/
 
/* PRINT_HELP - print a help message */

void print_help(void)
   {

    PRINT(STDOUT, "\nPDSMP - run basic PDB smp test\n\n");
    PRINT(STDOUT, "Usage: pdsmp [-h] [-i n]\n");
    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "       h - print this help message and exit\n");
    PRINT(STDOUT, "       i - set number of iterations in reading and writing loops to n\n");
    PRINT(STDOUT, "\n");

    return;}

/*-----------------------------------------------------------------*/
/*-----------------------------------------------------------------*/
 
int main(int argc, char **argv)
   {int nthreads;
    int i;
    int nt[2];
    void *(*fnc[2])(void *);

    for (i = 1; i < argc; i++)
        {if (argv[i][0] == '-')
            {switch (argv[i][1])
                {case 'h' :
		      print_help();
		      return(1);

                 case 'i' :
		      n_iter = atol(argv[++i]);
		      break;};}
         else
            break;};

    ok_count = 0;

    printf("%d iterations per thread\n", n_iter);

/* load up the output arrays */
    for (i = 0; i < 100; i++)
	{d[i] = d2[i] = (float) i;
         j[i] = j2[i] = i;}

    nthreads = 5;

/* initialize PDBLib for 5 threads (includes main thread) */
    PD_init_threads(nthreads, NULL);

/* create the output file */
    if ((file = PD_open("ptest.pdb", "w")) == NULL)
       {printf("Error creating ptest.pdb\n");
	return(1);};

/* define the structures to the library */
    PD_defstr(file, "l_frame",
              " float x_min",
              "float x_max",
              "float y_min",
              "float y_max",
              LAST);

    PD_defstr(file, "myplot",
              "float x_axis(10)",
              "float y_axis(10)",
              "integer npts", 
              "char * label",
              "l_frame * view",
               LAST);

/* put some values in the structures */
    mypl.label = CSTRSAVE("Myplot MYPL label");
    mypl.view  = CMAKE(l_frame);

    mypl.view->x_min = 0.1;
    mypl.view->x_max = 0.8;
    mypl.view->y_min = 0.2;
    mypl.view->y_max = 0.7;
    mypl.npts       = 10;
   
    for (i = 0; i < 10; i++)
        mypl.x_axis[i] = mypl.y_axis[i] = (float)i;

    nt[0] = 1;
    nt[1] = 1;

/* two threads write the file */
    fnc[0] = writeit;
    fnc[1] = writeit2;
    SC_do_threads(2, nt, fnc, NULL, NULL);

/* flush the tables to the file */
    if (!PD_flush(file))
       {printf("Error flushing file before reads\n");
        return(1);};

/* two threads read the file */
    fnc[0] = readit;
    fnc[1] = readit2;
    SC_do_threads(2, nt, fnc, NULL, NULL);

/* close file */
    PD_close(file);

    return((ok_count != 4));}

</pre>

<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_ex_par_mpi"><h2>MPI Parallel Example</h2></a>

This section illustrates the use of PDBLib in an MPI program.
<p>

<pre>
/*
 * PDDMP.C - Test dmp i/o in PDBLib
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb.h"

#define N_INT 100
#define N_FLOAT 100
#define N_DOUBLE 100
#define N_PTS 10
#define N_ITER 10

#define TEST_TYPE(_x_)                                         \
   {if (_x_ == SC_COMM_SELF)                                  \
       printf("MPI_COMM_SELF test\n");                         \
    else                                                       \
       printf("MPI_COMM_WORLD test\n");}

#define check_myplot1(xplot, rxplot, _comm_)                                                                 \
        {int i;                                                                                              \
         if (strcmp(xplot.label, rxplot.label) != 0)                                                         \
            {printf("xplot.label differs from rxplot.label in %s\n", name);                                  \
             printf("xplot.label: %s, rxplot.label: %s\n", xplot.label, rxplot.label);                       \
             TEST_TYPE(_comm_);}                                                                             \
                                                                                                             \
         if (xplot.view->x_min != rxplot.view->x_min)                                                        \
            {printf("xplot.view->xmin differs from rxplot.view->xmin in %s\n", name);                        \
             TEST_TYPE(_comm_);}                                                                             \
                                                                                                             \
         if (xplot.view->x_max != rxplot.view->x_max)                                                        \
            {printf("xplot.view->x_max differs from rxplot.view->x_max in %s\n", name);                      \
             TEST_TYPE(_comm_);}                                                                             \
                                                                                                             \
         if (xplot.view->y_min != rxplot.view->y_min)                                                        \
            {printf("xplot.view->y_min differs from rxplot.view->y_min in %s\n", name);                      \
             TEST_TYPE(_comm_);}                                                                             \
                                                                                                             \
         if (xplot.view->y_max != rxplot.view->y_max)                                                        \
            {printf("xplot.view->y_max differs from rxplot.view->y_max in %s\n", name);                      \
             TEST_TYPE(_comm_);}                                                                             \
                                                                                                             \
         if (xplot.npts != rxplot.npts)                                                                      \
            {printf("xplot.npts differs from rxplot.npts in %s\n", name);                                    \
             TEST_TYPE(_comm_);}                                                                             \
                                                                                                             \
         for (i = 0; i < N_PTS; i++)                                                                         \
             if (xplot.x_axis[i] != rxplot.x_axis[i])                                                        \
                {printf("xplot.x_axis[%d] differs from rxplot.x_axis[%d] in %s\n", i, i, name);              \
                TEST_TYPE(_comm_);};}     

struct s_l_frame
   {float x_min;
    float x_max;
    float y_min;
    float y_max;};

typedef struct s_l_frame l_frame;

struct s_plot
   {float x_axis[N_PTS];
    float y_axis[N_PTS];
    int npts;
    char *label;
    l_frame *view;};

typedef struct s_plot myplot;
    
static float x[100], xr[100];
static int *pi1, *pi1r;
static float *pf1, *pf1r;
static myplot mypl, myplr;

static void write_data(char *path, int rank, int size, MPI_Comm comm);
static void read_data(char *path, int rank, int size, MPI_Comm comm);

int main(int argc, char **argv)
   {int rank, size;
    char path[MAXLINE];
    PDBfile *file;


    MPI_Init(&amp;argc, &amp;argv);
    MPI_Comm_rank(SC_COMM_WORLD, &amp;rank);
    MPI_Comm_size(SC_COMM_WORLD, &amp;size);

/* if there is a command line argument it is the path of the directory
 * to write in
 */
    if (argc > 1)
       strncpy(path, argv[1], MAXLINE);
    else
       strncpy(path, "/home/foo", MAXLINE);
       
/* initialize PDBLib for MPI 
 * master process:    0
 * number of threads: 1
 * tid function:      NULL
 */
    PD_init_mpi(0, 1, NULL);

    write_data(path, rank, size, SC_COMM_WORLD);

    write_data(path, rank, size, SC_COMM_SELF);

    read_data(path, rank, size, SC_COMM_WORLD);

    read_data(path, rank, size, SC_COMM_SELF);

    PD_term_mpi();

    MPI_Finalize();

    return(0);}

/*-----------------------------------------------------------------*/
/*-----------------------------------------------------------------*/
 
static void write_data(char *path, int rank, int size, MPI_Comm comm)
   {char name[MAXLINE], temp[MAXLINE];
    char sx_axis[MAXLINE], sy_axis[MAXLINE];
    PDBfile *file;
    float frank;
    int i, nwrite;

    strncpy(name, path, MAXLINE);
    strncat(name, "/", MAXLINE);

    if (comm != SC_COMM_SELF)
       strncat(name, "test_mult.pdb", MAXLINE);
    else
       {snprintf(temp, MAXLINE, "test_single%d.pdb", rank);
        strncat(name, temp, MAXLINE);}

/* create the output file */
    if ((file = PD_mp_create(name, comm))
         == NULL)
       {printf("Error creating %s\n", name);
	return;};

/* define the structures to the library */
    PD_defstr(file, "l_frame",
              " float x_min",
              "float x_max",
              "float y_min",
              "float y_max",
              LAST);

    snprintf(sx_axis, MAXLINE, "float x_axis(%d)", N_PTS);
    snprintf(sy_axis, MAXLINE, "float y_axis(%d)", N_PTS);

    PD_defstr(file, "myplot",
              sx_axis,
              sy_axis,
              "integer npts", 
              "char * label",
              "l_frame * view",
               LAST);

/* put some values in the structures */
    mypl.label = CSTRSAVE("Myplot MYPL label");
    mypl.view  = CMAKE(l_frame);

    mypl.view->x_min = 0.1;
    mypl.view->x_max = 0.8;
    mypl.view->y_min = 0.2;
    mypl.view->y_max = 0.7;
    mypl.npts       = N_PTS;
   
    for (i = 0; i < N_PTS; i++)
        mypl.x_axis[i] = mypl.y_axis[i] = (float)i;

/* write a struct */
    snprintf(name, MAXLINE, "myplot_%d", rank);
    if (!PD_write(file, name, "myplot", &amp;mypl))
       {printf("Error writing mypl: process %d\n", rank);}

/* write an integer */
    snprintf(name, MAXLINE, "rank_%d", rank);
    if (!PD_write(file, name, "integer", &amp;rank))
       {printf("Error writing rank: process %d\n", rank);}

/* write a float */
    frank = rank;
    snprintf(name, MAXLINE, "frank_%d", rank);
    if (!PD_write(file, name, "float", &amp;frank))
       {printf("Error writing frank: process %d\n", rank);}

/* write an int array */
    pi1 = CMAKE_N(int, N_INT);
    if (pi1 == NULL)
       {printf("Error allocating pi1, process %d--quitting\n", rank);
        exit(1);}
    for (i = 0; i < N_INT; i++)
        pi1[i] = rank * i;
    snprintf(name, MAXLINE, "pi1_%d", rank);
    if (!PD_write(file, name, "int *", &amp;pi1))
       {printf("Error writing pi1: process %d\n", rank);}

/* write an float array */
    pf1 = CMAKE_N(float, N_FLOAT);
    if (pf1 == NULL)
       {printf("Error allocating pf1, process %d--quitting\n", rank);
        exit(1);}
    for (i = 0; i < N_FLOAT; i++)
        pf1[i] = (float)(rank * i);
    snprintf(name, MAXLINE, "pf1_%d", rank);
    if (!PD_write(file, name, "float *", &amp;pf1))
       {printf("Error writing pf1: process %d\n", rank);}

/* try a PD_defent */
    if (!PD_defent(file, "x[100]", "float"))
       {printf("PD_defent failed for x[100], process %d\n",
                rank);}

    nwrite = (comm == SC_COMM_SELF) ? 100 : 100 / size;
    for (i = 0; i < 100; i++)
        x[i] = (float)rank;

    if (comm != SC_COMM_SELF)
       snprintf(name, MAXLINE, "x[%d:%d]", rank*nwrite, (rank+1)*nwrite - 1);
    else
       strncpy(name, "x[0:99]", MAXLINE);

    if (!PD_write(file, name, "float", x))
       {printf("Error writing x, process %d\n", rank);}

/* close file */
    PD_close(file);
    
    return;}

/*-----------------------------------------------------------------*/
/*-----------------------------------------------------------------*/
 
static void read_data(char *path, int rank, int size, MPI_Comm comm)
   {char name[MAXLINE], temp[MAXLINE];
    PDBfile *file;
    int rankr;
    float frank;
    int i, nread;

    strncpy(name, path, MAXLINE);
    strncat(name, "/", MAXLINE);

    if (comm != SC_COMM_SELF)
       strncat(name, "test_mult.pdb", MAXLINE);
    else
       {snprintf(temp, MAXLINE, "test_single%d.pdb", rank);
        strncat(name, temp, MAXLINE);}

/* open the input file */
    if ((file = PD_mp_open(name, "r", comm)) == NULL)
       {printf("Error opening %s, rank: %d\n", name, rank);
        TEST_TYPE(comm); 
	return;};

/* read a struct */
    snprintf(name, MAXLINE, "myplot_%d", rank);
    if (!PD_read(file, name, &amp;myplr))
       {printf("Error reading mypl: process %d\n", rank);
        TEST_TYPE(comm);}

    check_myplot1(mypl, myplr, comm);

/* read an integer */
    snprintf(name, MAXLINE, "rank_%d", rank);
    if (!PD_read(file, name, &amp;rankr))
       {printf("Error reading rank: process %d\n", rank);
        TEST_TYPE(comm);}

    if (rankr != rank)
       {printf("Error: rankr != rank: process %d\n", rank);
        TEST_TYPE(comm);}

/* read a float */
    snprintf(name, MAXLINE, "frank_%d", rank);
    if (!PD_read(file, name, &amp;frank))
       {printf("Error reading frank: process %d\n", rank);
        TEST_TYPE(comm);}

    if (frank != (float)rank)
       {printf("Error: frank != rank: process %d\n", rank);
        TEST_TYPE(comm);}

/* read an int *array */
    snprintf(name, MAXLINE, "pi1_%d", rank);
    if (!PD_read(file, name, &amp;pi1r))
       {printf("Error reading pi1r: process %d\n", rank);
        TEST_TYPE(comm);}

    for (i = 0; i < N_INT; i++)
        if (pi1r[i] != pi1[i])
           {printf("Error: pi1r[%d] != pi1[%d], pi1r[%d] = %d, pi1[%d] = %d",
                    i, i, i, pi1r[i], i, pi1[i]);
            printf(" Process %d\n", rank);
            TEST_TYPE(comm);
            break;}

/* read a float array */
    snprintf(name, MAXLINE, "pf1_%d", rank);
    if (!PD_read(file, name, &amp;pf1r))
       {printf("Error reading pf1: process %d\n", rank);}

    for (i = 0; i < N_FLOAT; i++)
        if (pf1r[i] != pf1[i])
           {printf("Error: pf1r[%d] != pf1[%d], pf1r[%d] = %d, pf1[%d] = %d",
                    i, i, i, pf1r[i], i, pf1[i]);
            printf(" Process %d\n", rank);
            TEST_TYPE(comm);
            break;}

/* read the PD_defented array */
    nread = 100;
    strncpy(name, "x[0:99]", MAXLINE);

    if (!PD_read(file, name, xr))
       {printf("Error reading x, process %d\n", rank);
        TEST_TYPE(comm);}

    if (comm == SC_COMM_SELF)
       {for (i = 0; i < 100; i++)
            if (xr[i] != x[i])
               {printf("Error: xr[%d] != x[%d], xr[%d] = %d, x[%d] = %d\n",
                       i, i, i, xr[i], i, x[i]);
                printf(" Process %d\n", rank);
                TEST_TYPE(comm);
                break;};}
    else
       {int nchunk = 100 / size;
        for (i = rank*nchunk; i < (rank+1)*nchunk; i++)
            if (xr[i] != (float)rank)
               {printf("Error: xr[%d] != rank, xr[%d] = %f, rank = %d\n",
                       i, i, xr[i], rank);
                printf(" Process %d\n", rank);
                TEST_TYPE(comm);
                break;};}

/* close file */
    PD_close(file);
    
    return;}

</pre>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="pdb_docs"><h2>Related Documentation</h2></a>


PDBLib is one part of a collection of libraries called PACT. PDBLib
uses the SCORE library in PACT for memory management, hash table,
and string handling support. Interested readers should consult the
SCORE manual as well as the PDBView manual, the ULTRA II manual,
and the PANACEA manual for more information on how PDBLib is used.
<p>
The list of PACT documents is:

<BLOCKQUOTE>
<a href=pact.html>PACT User&#146;s Guide</a><br><br>
<a href=score.html>SCORE User&#146;s Manual</a><br><br>
<a href=ppc.html>PPC User&#146;s Manual</a><br><br>
<a href=pml.html>PML User&#146;s Manual</a><br><br>
<a href=pdb.html>PDBLib User&#146;s Manual</a><br><br>
<a href=pgs.html>PGS User&#146;s Manual</a><br><br>
<a href=pan.html>PANACEA User&#146;s Manual</a><br><br>
<a href=ultra.html>ULTRA II User&#146;s Manual</a><br><br>
<a href=pdbdiff.html>PDBDiff User&#146;s Manual</a><br><br>
<a href=pdbview.html>PDBView User&#146;s Manual</a><br><br>
<a href=sx.html>SX User&#146;s Manual</a>
</BLOCKQUOTE>

<br>
<hr>
For questions and comments, 
please contact the <A HREF="pactteam.html">PACT Development Team</A>.

<hr>
Last Updated: $MOD
