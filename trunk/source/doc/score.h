TXT: SCORE User's Manual
MOD: 02/03/2010

<CENTER>
<P>
<H1><font color="#ff0000">$TXT</font></H1>
<H4>Stewart A. Brown<BR>
    Dennis Braddy<BR>
    Gregory Smethells<BR>
</H4>
</CENTER>

<BLOCKQUOTE>

<HR>

<ul>
<li><a href="#score.intro">Introduction</a>

<li><a href="#score.api">The SCORE API</a>
<ul>
<li><a href="#score.hash">Hash Array Handling</a>
<li><a href="#score.lex">Lexical Stream Handling</a>
<li><a href="#score.assoc">Association List Handling</a> 
<li><a href="#score.string">String Handling</a>
<ul>
<li><a href="#score.preds">Predicates</a>
<li><a href="#score.patt">Pattern Matching</a>
<li><a href="#score.sort">String Sorting</a>
<li><a href="#score.case">String Operations Involving Case</a>
<li><a href="#score.toke">Tokenizers</a>
<li><a href="#score.other">Other String Operations</a> 
</ul>
<li><a href="#score.type">Type Handling</a>
<li><a href="#score.time">Time and Date Handling</a>
<li><a href="#score.mem">Memory Management</a>
<ul>
<li><a href="#score.mmover">Overview of SCORE Memory Manager</a>
<li><a href="#score.mmdesc">Description of SCORE Memory Manager</a>
<li><a href="#score.mmconf">Configuring the Memory Manager</a>
<li><a href="#score.mmlk">Tracking Down Memory Leaks</a>
<li><a href="#score.mmfnc">Memory Manager Functions</a>
</ul>
<li><a href="#score.misc">Miscellaneous Routines</a>
<ul>
<li><a href="#score.bit">Bit Level Manipulations</a>
<li><a href="#score.nconv">Numeric Conversion</a>
<li><a href="#score.onum">Other Numeric</a>
<li><a href="#score.file">File Search</a>
<li><a href="#score.io">I/O</a>
<li><a href="#score.intrpt">Interrupts</a>
<li><a href="#score.other2">Other</a> 
</ul>
<li><a href="#SCPar">Parallel Programming with SCORE</a>
<ul>
<li><a href="#SCPar381">Low Level Thread Interface</a>
<li><a href="#SCPar382">High Level Thread Interface</a>
</ul>
</ul>

<li><a href="#score.cons">SCORE Constants</a>
<ul>
<li><a href="#score.scstd">scstd.h</a> 
</ul>

<li><a href="#score.examp">Examples</a>

<li><a href="#score.docs">Related Documentation</a>
</ul>

<hr>

<a name="score.intro"></a>
<h2 ALIGN="CENTER">Introduction</h2>

PACT is a set of tools to facilitate the development of portable scientific
software and to visualize scientific data. As with most constructs, PACT has
a foundation. In this case that foundation is SCORE.<p>

SCORE (System CORE) has two main functions. The first and perhaps most
important is to smooth over the differences between different C implementations
and define the parameters which drive most of the conditional compilations in
the rest of PACT. Secondly, it contains several groups of functionality that
are used extensively throughout PACT.<p>

Although C is highly standardized now, that has not always been the case.
Roughly speaking C compilers fall into three categories: ANSI standard;
derivative of the Portable C Compiler (Kernighan and Ritchie); and the rest.
PACT has been successfully ported to many ANSI and PCC systems. It has never
been successfully ported to a system in the last category. The reason is mainly
that the &#147;standard&#148; C library supplied with such implementations is
so far from true ANSI or PCC standard that PACT would have to include its own
version of the standard C library in order to work at all.<p>

Even with standardized compilers life is not dead simple. The ANSI standard
leaves several crucial points ambiguous as &#147;implementation defined&#148;.
Under these conditions one can find significant differences in going from one
ANSI standard compiler to another.<p>

SCORE&#146;s job is to include the requisite standard headers and ensure that
certain key standard library functions exist and function correctly (there are
bugs in the standard library functions supplied with some compilers) so that,
to applications which include the SCORE header(s) and load with SCORE, all C
implementations look the same. This is a tall order, but in practice once
SCORE has been successfully compiled only the areas of graphics, IPC, and
binary data handling require special handling! This has more of an impact on
some programmers than on others. Those who prefer to specify only the exact
headers to be included in each source file will find SCORE and PACT unusual.
At the expense of a slight increase in compile time, the most commonly used
headers are always included. This is crucial to getting the C implementation
independence. <font color="#FF0000">The upshot is that PACT headers MUST 
be included before all other headers.</font><p>

Typically, the SCORE header scstd.h includes the following:<p>

<BLOCKQUOTE>
<TABLE>
<TR><TD WIDTH="40"><B>ANSI</B></TD>    <TD>stdlib.h stddef.h stdarg.h float.h</TD></TR>
<TR><TD><B>PCC</B></TD>     <TD> sys/types.h varargs.h malloc.h</TD></TR>
<TR><TD><B>Both</B></TD>    <TD> limits.h stdio.h string.h math.h ctype.h signal.h setjmp.h time.h</TD></TR>
</TABLE>
</BLOCKQUOTE>

The single header, scstd.h, smooths over most of the generic problems that
arise because of implementation defined behavior in the host C implementation.
The remainder of the PACT sources ultimately include scstd.h. This strategy
has been extremely successful for PACT and applications which use PACT.<p>

There are basically three other areas which SCORE functions address: memory
management; hash array management; and extended string handling.<p>


<a name="score.api">
<h2 ALIGN="CENTER">The SCORE API</h2>
</a>

This section of the manual details the SCORE functions intended for use by C
application programs. 

<p> 

<!-------------------------------------------------------------------------->
<!-------------------------------------------------------------------------->

<a name="score.hash">
<h3>Hash Arrays</h3>
</a>

SCORE uses the concept of a hash array.  This is a data structure and
supporting routines which provide two kinds of access to its elements.
First is to be able to find elements by a hash key such as a name or
and address.  Second is to be able to access them via a index into
a linear array.
<p>
The first access mode has the advantage of rapid access when doing
lookups with keys as opposed to indeces.  It is especially well
suited to accessing one element at a time.
Hashing access routines are a generalized version of the hashing routines
described in Kernighan and Ritchie&#146;s <U>The C Programming Language</U>.
They are most conveniently used by a call by value language such as C.
<p>
The second access mode has the advantage of accessing all the elements
in some linear order.  This is especially useful when one wants to
apply a function to all elements in the hash array.
<p>

<pre>
<i>C Binding: </i>hasharr *SC_make_hasharr(int size, int docflag, char *lm)
<i>F77 Binding: </i>integer scmkht(integer size, integer docflag)
<i>SX Binding: </i>
<i>Python Binding: </i>
</pre>

Constructs a <tt>hasharr</tt> based upon the number of buckets to be
used and whether documentation should be used. Sets the key method to
one of <tt>SC_HA_NAME_KEY</tt> or <tt>SC_HA_ADDR_KEY</tt>.  In the first case text
strings will be used as the hash keys and in the second case memory
or disk addresses will be used as the hash keys.
<p>
The size of a hash array should be a prime number for greatest efficiency. 
For C based applications, there are four #define&#146;d sizes in the header, 
score.h. They are <tt>HSZSMALL</tt> (31), <tt>HSZSMINT</tt> (67),
<tt>HSZLRINT</tt> (127), and <tt>HSZLARGE</tt> (521).
<p>
Applications can have documentation associated with the installed objects, and for
efficiency in searching a hash array for documentation a flag is provided
in the hash array structure indicating whether or not the objects that are
installed have documentation. The predefined flags in C applications are
<tt>DOC</tt> (1) and <tt>NODOC</tt> (0).
<p>
Returns a pointer to the new <tt>hasharr</tt> if successful and a NULL
pointer otherwise.
<p>

<pre>
<i>C Binding: </i>void SC_free_hasharr(hasharr *ha, int (*f)(haelem *hp, void *a), void *a)
<i>F77 Binding: </i>integer scrlht(integer ha)
<i>SX Binding: </i>
<i>Python Binding: </i>
</pre>

Release the space associated with the hash elements and then release the
hash array itself.  The function <tt>f</tt> is applied to each element of
the hash array first.  This permits applications to free objects which they
have installed.  The argument <tt>a</tt> is passed through to each call to
<tt>f</tt>.
<p>

<pre>
<i>C Binding: </i>char **SC_hasharr_dump(hasharr *ha, char *pattern, char *type, int sort)
<i>F77 Binding: </i>
<i>SX Binding: </i>
<i>Python Binding: </i>
</pre>

Returns a NULL terminated array of specific keys installed in the given 
hash array defined by any pattern string given or any type.  The result
may optionally be sorted.
<p>
If the <i>pattern</i> specified is not NULL then only keys matching the pattern are
returned. In the pattern &#145;*&#146; matches any number of characters and
&#145;?&#146; matches any single character. 
<p>
If <i>sort</i> is true, then keys of type <tt>char*</tt> are alphabetically 
sorted according the C library function STRCMP.
<p>

<pre>
<i>C Binding: </i>int SC_hasharr_install(hasharr *ha, void *key, void *obj, char *type, int mark, int lookup)
<i>F77 Binding: </i>integer schins(integer nk, char *key, obj, integer nt,
                                   char *type, integer cp, integer ha)
<i>SX Binding: </i>
<i>Python Binding: </i>
</pre>

Insert an object, <i>obj</i>, of type <i>type</i> in the hash array and associate it
with key <i>key</i>. 
<p>
The procedure is an adaptation of the function described in
Kernighan and Ritchie in <U>The C Programming Language</U>. 
By including a type with the installation of an object, applications
can insert different types of objects in a single hash array and still
distinguish between types so they can be cast appropriately when they are returned
during look-ups.
<p>
NOTE: do NOT use literals or volatiles for the parameter named <i>type</i> as that
char pointer is stored directly in the hash array.
<p>
In the FORTRAN binding, the argument cp is a flag requesting that the installed object 
is a copy of the argument pointed to by obj. In that case, schins will allocate one 
item of type <i>type</i> and copy one such item from <i>obj</i> into the new space. 
The new space will be installed in the hash array.
<p>

<pre>
<i>C Binding: </i>int SC_hasharr_remove(hasharr *ha, void *key)
<i>F77 Binding: </i>integer schrem(integer nk, char *key, integer ha)
<i>SX Binding: </i>
<i>Python Binding: </i>
</pre>

Remove the object associated with key from the hash array.
<p>
<tt>TRUE</tt> is returned if the operation is successfully carried out;
otherwise, <tt>FALSE</tt> is returned.
<p>

<pre>
<i>C Binding: </i>void SC_hasharr_clear(hasharr *ha, int (*f)(haelem *hp, void *a), void *a)
<i>F77 Binding: </i>integer schclr(integer ha)
<i>SX Binding: </i>
<i>Python Binding: </i>
</pre>

Clear out all entries currently in the hash array. After this call, the 
hash array will be empty.  This function works exactly like SC_free_hasharr
except that when it returns the hash array has no elements and is ready
for more installs, lookups, and so on.
<p>

<pre>
<i>C Binding: </i>haelem *SC_hasharr_lookup(hasharr *ha, void *key)
<i>F77 Binding: </i>
<i>SX Binding: </i>
<i>Python Binding: </i>
</pre>

Look up the given key in the hash array and return the associated hash element.
<p>

<pre>
<i>C Binding: </i>void *SC_hasharr_def_lookup(hasharr *ha, void *key)
<i>F77 Binding: </i>integer schlkp(void *p, integer nk, char *key, integer ha)
<i>SX Binding: </i>
<i>Python Binding: </i>
</pre>

Look up the given key in the hash array and return the associated object.
Returns a pointer to the object if successful; otherwise, a NULL pointer is returned.
<p>
In the FORTRAN binding it is assumed that the pointer extension is present
and the object is returned via the argument <tt>p</tt>.
Returns TRUE successful; otherwise, FALSE.
<p>

<pre>
<i>C Binding: </i>void SC_hasharr_foreach(hasharr *ha, int (*f)(haelem *hp, void *a), void *a)
<i>F77 Binding: </i>
<i>SX Binding: </i>
<i>Python Binding: </i>
</pre>

Applies <tt>f</tt> to each entry in the hash array.
<p>

<pre>
<i>C Binding: </i>int SC_hasharr_next(hasharr *ha, long *pi, char **pname, char **ptype, void **po)
<i>F77 Binding: </i>
<i>SX Binding: </i>
<i>Python Binding: </i>
</pre>

Iterates thru hasharray <tt>ha</tt> return name, type, and object for each
non-NULL hash element in the hasharray.
<p>

<pre>
<i>C Binding: </i>void SC_hasharr_sort(hasharr *ha, int (*pred)(void *a, void *b))
<i>F77 Binding: </i>
<i>SX Binding: </i>
<i>Python Binding: </i>
</pre>

Sorts the linear array in <tt>ha</tt> using the supplied predicate function
<tt>pred</tt>.  This has no effect on accessing elements via SC_hasharr_lookup.
<p>

<pre>
<i>C Binding: </i>long SC_hasharr_get_n(hasharr *ha)
<i>F77 Binding: </i>
<i>SX Binding: </i>
<i>Python Binding: </i>
</pre>

Return the number of elements in the hash array <tt>ha</tt>.
<p>

<pre>
<i>C Binding: </i>int SC_haelem_data(haelem *hp, char **pname, char **ptype, void **po)
<i>F77 Binding: </i>
<i>SX Binding: </i>
<i>Python Binding: </i>
</pre>

This function return the name, type, and object from the hash element hp.
This is intended for use when writing functions which are to be mapped
over all elements in a hash array.
<p>

If successful, it returns TRUE and returns FALSE otherwise.
<p>


<!-------------------------------------------------------------------------->
<!-------------------------------------------------------------------------->

<p>
<a name="score.lex">
<h3>Lexical Stream Handling</h3>
</a>

The following routines assist code developers with integrating automatically
generated lexical scanners into their applications and controlling them.
The <tt>SC_lexical_stream</tt> structure encapsulates the lexical scanner, the I/O
stream, and various buffers so that applications can apply arbitrarily many
lexical scanners to any number of input streams. The <tt>SC_lexical_token</tt>
describes a lexical token for parsers and other applications.<p>
<p>

<pre>
<i>C Binding: </i>void SC_close_lexical_stream(SC_lexical_stream *str)
<i>F77 Binding:</i>integer scclls(integer strid)
<i>SX Binding: </i>
<i>Python Binding: </i>
</pre>

This function closes the given lexical stream.  Closing the lexical stream
means closing the I/O stream if it is not stdin and releasing the buffer
spaces as well as the <tt>SC_lexical_stream itself</tt>.<p>
This function takes <em>str</em>, a pointer to the <tt>SC_lexical_stream</tt>,
as argument.
<p>
This function has no return value.
<p>
<B>SEE ALSO: </B><tt>SC_open_lexical_stream</tt>, 
<tt>SC_get_next_lexical_token</tt>, <tt>SC_scan</tt>.
<p>

<pre>
<I>C Binding: </I>SC_lexical_token *SC_get_next_lexical_token(
                   SC_lexical_stream *str)
<I>F77 Binding: </I>
<I>SX Binding: </I>
<i>Python Binding: </i>
</pre>

This function permits applications to view an input stream as a sequence of
tokens returned by the lexical scanner bound in an <tt>SC_lexical_stream</tt>.  It
returns one token at a time until the end of the stream is encountered when
NULL is returned. The function handles all reads of the input stream.
<p>
The argument to this function is: <em>str</em>, a pointer to a
<tt>SC_lexical_stream</tt>.
<p>
A pointer to an <tt>SC_lexical_token</tt> is returned or NULL if the end of the
input stream has been reached.
<p>
<B>SEE ALSO: </B><tt>SC_open_lexical_stream</tt>, <tt>SC_close_lexical_stream</tt>, 
<tt>SC_scan</tt>.
<p>

<pre>
<I>C Binding: </I>SC_lexical_stream *SC_open_lexical_stream(
                   char *name, int inbfsz, int strbfsz,
                   PFInt scan, PFInt input, PFInt output, PFInt unput,
                   PFInt wrap, PFInt more, PFInt less)
<I>F77 Binding: </I>integer scopls(integer nchr, char *name, integer inbs,
                            integer strbs, integer function scan)
<I>SX Binding: </I>
<I>Python Binding: </I>
</pre>
<P>
This function initializes a new lexical stream and returns a pointer to it.
A lexical stream consists of: an input buffer; a string buffer for tokens;
an output buffer for text not handled by the lexical scanner; an
<tt>SC_lexical_token</tt> buffer; a lexical scanning function produced by a tool
such as FLEX or LEX; and additional integer valued functions (some optional)
to support the flexibility of the lexical scanner.<p>

The main attempt here is to encapsulate the functions and data structures
necessary to support an application in the use of multiple independent
lexical scanners.  Such capability can be used in conjunction with parsers
or other applications where some sort of input language must be broken down
into proper sized pieces.<p>

The arguments are:
<em>name</em>, the name of a file to attach to the lexical stream or NULL for stdin;
<em>inbfsz</em>, the integer byte size of the input buffer or MAXLINE if 0;
<em>strbfsz</em>, the integer byte size of string buffer or MAX_LEN_BUFFER if 0;
<em>scan</em>, the lexical scanning function to be used (for example yylex);
<em>input</em>, the single character supply function for the scanner  (defaults to 
<tt>SC_lex_getc</tt> if NULL);
<em>output</em>, the single character output function for the
scanner (defaults to  <tt>SC_lex_putc</tt> if NULL);
<em>unput</em>, a function which pushes a single character at a time back onto 
the input stream (defaults to <tt>SC_lex_push</tt> if NULL);
<em>wrap</em>, a function which handles end of input conditions
for the scanner (defaults to  <tt>SC_lex_wrap</tt> if NULL);
<em>more</em>, a function to process more input for the scanner if required 
by the associated lexical rules; 
and <em>less</em>, a function to back up the input stream if required by
the lexical scanner.<p>

In the C binding, this function returns a non-NULL pointer to a newly
allocated <tt>SC_lexical_stream</tt> if successful and NULL if not. In the FORTRAN
binding, this function returns a non-negative  integer identifier for the
lexical stream if successful and -1 otherwise.<p>

<B>SEE ALSO: </B><tt>SC_close_lexical_stream</tt>, <tt>SC_scan</tt>,
 <tt>SC_get_next_lexical_token</tt>.
<p>

<pre>
<I>C Binding: </I>
<I>F77 Binding: </I>integer scrdls(integer strid, integer nc, char *s)
<I>SX Binding: </I>
<I>Python Binding: </I>
</pre>

This function reads a line from the specified lexical stream&#146;s
input stream  into its input buffer.
<p>
The arguments to this function are:
<em>strid</em>, an integer identifier specifying the lexical stream as 
returned by scopls;
<em>nc</em>, an integer size of the character array <em>s</em>;
<em>s</em>, a character array for the contents of the input buffer to be
returned to the application.
<p>
This function returns the specified number of characters via the supplied
character array. The return value is <tt>TRUE</tt> if successful and <tt>FALSE</tt> otherwise.
<p>
<B>SEE ALSO: </B><tt>scopls</tt>, <tt>scclls</tt>, <tt>scrdls</tt>.
<p>

<pre>
<I>C Binding: </I>int SC_scan(SC_lexical_stream *str, int rd)
<I>F77 Binding: </I>integer scscan(integer strid,  integer mxtok, integer width,
                            char *tok, integer ntok, integer nctok, integer ixtok,
                            integer toktyp, REAL tokval)
<I>SX Binding: </I>
<I>Python Binding: </I>
</pre>
<P>
This function scans the current input buffer in the specified lexical
stream and returns arrays of tokens.<p>

The lexical rules are defined by the routine attached to the lexical stream
when it is opened. However a default set of rules implements a FORTRANish
syntax.<p>

The string to be scanned is contained in the lexical stream. A call to <tt>scrdls</tt>
can be made to read a new line of text from the input stream into the input
buffer, otherwise the current contents of the input buffer will be scanned.<p>

The default scanner, f77lxr, defines the following token types:<p>


<BLOCKQUOTE>
<TABLE>
<TR><TD><B>TYPE</B>	</TD><TD WIDTH="100"><B>NAME</B>		</TD><TD><B>EXAMPLE</B>	</TD></TR>
<TR><TD>1	</TD><TD>DELIMITER	</TD><TD>&amp; ( ) , : &lt; = &gt; _ |</TD></TR>
<TR><TD>2	</TD><TD>ALPHANUM	</TD><TD>abc</TD></TR>
<TR><TD>3	</TD><TD>INTEGER	</TD><TD>10</TD></TR>
<TR><TD>4	</TD><TD>REAL		</TD><TD>1.2 6.0e10</TD></TR>
<TR><TD>5	</TD><TD>OCTAL		</TD><TD>17b</TD></TR>
<TR><TD>6	</TD><TD>HEX		</TD><TD>#17</TD></TR>
<TR><TD>7	</TD><TD>OPERAND	</TD><TD>.and.</TD></TR>
<TR><TD>8	</TD><TD>STRING		</TD><TD>&#147;foo&#148;</TD></TR>
<TR><TD>1000	</TD><TD>HOLLERITH	</TD><TD>3hFOO</TD></TR>
</TABLE>
</BLOCKQUOTE>

Invoke the lexical scanner to traverse the current input buffer and
identify the lexical tokens as defined by the lexical rules which
generated the scanner.  These are placed in a buffer of <tt>SC_lexical_tokens</tt>.<p>

The other arguments to this function are:
<em>str</em>, a pointer to a <tt>SC_lexical_stream</tt>;
<em>rd</em>, an integer flag;
<em>mxtok</em>, and integer maximum number of tokens to be returned;
<em>width</em>, an integer character field width (e.g. char*8 =&gt; 8);
<em>tok</em>, character array char*width(mxtok) for returned tokens;
<em>ntok</em>, an integer count of tokens available;
<em>nctok</em>, an integer character length of each token;
<em>ixtok</em>, an integer index in tok for each token;
<em>toktyp</em>, type for each token;
and <em>tokval</em>, an array of REAL numerical values for numerical tokens.
<p>
If the <em>rd</em> argument it <tt>TRUE</tt> a new line is read from the
input stream into the input buffer before it is scanned.
<p>
In the C binding this function returns the number of lexical tokens found
in the current input buffer. In the FORTRAN binding this function returns
<tt>TRUE</tt> if successful and <tt>FALSE</tt> otherwise.
<p>
<B>SEE ALSO: </B><tt>SC_open_lexical_stream</tt>, <tt>SC_close_lexical_stream</tt>, 
<tt>SC_get_next_lexical_token</tt>.
<p>

<!-------------------------------------------------------------------------->
<!-------------------------------------------------------------------------->

<a name="score.assoc">
<h3>Association List Handling</h3>
</a>

Association lists are similar to hash tables in that they associate a key
(in this case an ASCII string) with a &#147;value&#148;.  They differ from
hash tables in that the elements are chained together in a linked list
instead of being organized in a hashed table. The principle advantage of
association lists is lower memory overhead. The disadvantage is the relative
inefficiency of checking each key until a match is found. The application
developer must decide.<p>
<p>

<pre>
<I>C Binding: </I>pcons *SC_change_alist(pcons *al, char *name, char *type, void *val)
<I>F77 Binding: </I>integer scchal(integer al, integer nn, char *pname, integer nt,
                            char *ptype, integer nv,  val)
<I>SX Binding: </I>
<I>Python Binding: </I>
</pre>

This routine changes the value associated with name on the specified
association list. If there is no value associated with the name key then
the value is added to the list under the name.  Because of this property
is is permissible to pass a NULL association list into this function. In
this way association list can be built up without explicitly creating them.
It also means that the return list may not have the same address as the
input list and consequently the return list must be used in all future
calls to these routines.
<p>
The other arguments are:
<em>type</em>, an ASCII string specifying the data type of the values(s) 
being added to the list (arrays may be used as values!);
<em>val</em>, a pointer to the values to be added to the list;
and, in FORTRAN, <em>nv</em>, the number of values to be added.
In the C binding val must have been
dynamically allocated with <tt>MAKE</tt>, <tt>MAKE_N</tt>, <tt>FMAKE</tt>, 
<tt>FMAKE_N</tt>, or <tt>SC_alloc</tt>.
<p>

<pre>
<I>C Binding: </I>pcons *SC_rem_alist(pcons *al, char *name)
<I>F77 Binding: </I>integer scrmal(integer al, integer nn, char *name)
<I>SX Binding: </I>
<I>Python Binding: </I>
</pre>

Removes the named element from the association list, <em>al</em>. Because the
specified element may be the first on the list, the modified list is
returned and must be used in all future calls to these functions!<p>
<p>

<pre>
<I>C Binding: </I>void SC_free_alist(pcons *al, int level)
<I>F77 Binding: </I>integer scrlal(integer al, integer level)
<I>SX Binding: </I>
<I>Python Binding: </I>
</pre>

Free the specified association list, <em>al</em>.
The <em>level</em> argument refers to how
much information is to be released.
If <em>level</em> is 1 the only the name key is released;
if <em>level</em> is 2 only the value is released;
and if <em>level</em> is 3 both are released.<p>
<p>

<pre>
<I>C Binding: </I>pcons *SC_copy_alist(pcons *al)
<I>F77 Binding: </I>integer sccpal(integer al)
<I>SX Binding: </I>
<I>Python Binding: </I>
</pre>

This routine returns a copy of the given association list if successful.
<p>

<pre>
<I>C Binding: </I>pcons *SC_append_alist(pcons *al1, pcons *al2)
<I>F77 Binding: </I>  
<I>SX Binding: </I>
<I>Python Binding: </I>
</pre>

This routine copies the contents of association list <em>al2</em> to association
list <em>al1</em>.
As always a NULL list for <em>al1</em> or <em>al2</em> is acceptable. The modified
assocation list <em>al1</em> is returned if successful.
<p>

<!-------------------------------------------------------------------------->
<!-------------------------------------------------------------------------->

<a name="score.string">
<h3>String Handling</h3>
</a>

The string handling routines in SCORE provide functionality which extends
or supplements that available from the standard C library.<p>

<a name="score.preds">
<b>Predicates</b>
</a>

The functions test strings for specific properties and return <tt>TRUE</tt> or
<tt>FALSE</tt> depending on the outcome.
<p>

<pre>
<I>C Binding: </I>int SC_numstrp(char *s)
</pre>

Return <tt>TRUE</tt> iff the string is a valid representation of a a number.
<p>

<pre>
<I>C Binding: </I>int SC_intstrp(char *s, int base)
</pre>

Return <tt>TRUE</tt> iff the string is a valid representation of an integer in
the specified <em>base</em>.
<p>

<pre>
<I>C Binding: </I>int SC_fltstrp(char *s)
</pre>

Return <tt>TRUE</tt> iff the string is a valid representation of a
floating point number.
<p>

<pre>
<I>C Binding: </I>int SC_chrstrp(char *s)
</pre>

Return <tt>TRUE</tt> iff the string contain printable characters only.
<p>

<pre>
<I>C Binding: </I>int SC_blankp(char *string1, char *string2)
</pre>

Return <tt>TRUE</tt> iff <em>string1</em> is  blank or a comment. A comment begins with
a character in <em>string2</em> followed by a blank, tab, or end of line.<p>
<p>

<a name="score.patt">
<b>Pattern Matching</b>
</a>

These functions do certain patterm matching operations.
<p>

<pre>
<I>C Binding: </I>char *SC_strstr(char *string1, char *string2)
</pre>

Find the first occurrence of <em>string2</em> in <em>string1</em>.
<p>

<pre>
<I>C Binding: </I>char *SC_strstri(char *string1, char *string2)
</pre>
<p>
Find the first case insensitive occurrence of <em>string2</em> in <em>string1</em>.
<p>

<pre>
<I>C Binding: </I>char *SC_str_replace(char *s, char * patto, char * pattn)
</pre>

Replace all non-overlapping occurrences of <em>patto</em> in string <em>s</em> 
with <em>pattn</em>.
<p>

<pre>
<I>C Binding: </I>int SC_regx_match(char *s, char *patt)
</pre>

Match string <em>s</em> against regular expression <em>patt</em>.
 &#145;*&#146; matches any
number of characters. &#145;?&#146; matches any single character.
<p>

<a name="score.sort">
<b>String Sorting</b>
</a>

These functions are involved in sorting operations on strings.
<p>

<pre>
<I>C Binding: </I>void SC_string_sort(char **s, int number)
</pre>

Sort an array of character pointers by what they point to. The arguments are:
<em>s</em>, an array of pointers to ASCII strings,
and <em>number</em>, the number of strings.
<p>

<a name="score.case">
<b>String Operations Involving Case</b>
</a>

These functions are used in connection with the case of the characters
in strings.
<p>

<pre>
<I>C Binding: </I>int SC_str_icmp(char *s, char *t)
</pre>

Compare two strings ignoring case.
<p>

<pre>
<I>C Binding: </I>char *SC_str_upper(char *s)
</pre>

Convert a string to all uppercase.
<p>

<pre>
<I>C Binding: </I>char *SC_str_lower(char *s)
</pre>

Convert a string to all lowercase.
<p>

<a name="score.toke">
<b>Tokenizers</b>
</a>

These functions supplement the string tokenizing capability of the
standard C library.<p>
<p>

<pre>
<I>C Binding: </I>char *SC_firsttok(char *s, char *delim)
</pre>

Find the first token in a string.
<p>

<pre>
<I>C Binding: </I>char *SC_firsttokq(char *s, char *delim, char *quote)
</pre>

Find the first token or quoted token string in a string.
<p>

<pre>
<I>C Binding: </I>char *SC_lasttok(char *s, char *delim)
</pre>

Find the last token in a string.
<p>

<a name="score.other">
<b>Other String Operations</b>
</a>

These additional string operations do not fit into any of the above categories.
<p>

<pre>
<I>C Binding: </I>char *SC_strrev(char *s)
</pre>
Reverse a string in place.<p>
<p>

<pre>
<I>C Binding: </I>int SC_char_count(char *s, int c)
</pre>

Count the occurrences of a specified character in a string.
<p>

<pre>
<I>C Binding: </I>char *SC_squeeze_blanks(char *s)
</pre>

Replace contiguous blanks in a stirng with a single blank and remove
leading and trailing blanks.
<p>

<!-------------------------------------------------------------------------->
<!-------------------------------------------------------------------------->

<a name="score.type">
<h3>Type Handling</h3>
</a>

Data types are not first class objects in C.  That means that you
cannot pass a type specifier to a function.  This in turn precludes
certain idioms and necessitates stamping out macro templates in order
to operate on different data types.
<p>
In order to help out with this, SCORE maintains a type registry which
is a hash array of data types and information describing them.  In the
registry each data type that is registered is assigned in an integer
identity.  With this in hand then, one can look up data types by name
or by the index.
<p>
The standard C99 types are automatically registered. Specifically:
<pre>
    bool                 char
    short                int                 long                    long long
    float                double              long double
    float _Complex       double _Complex     long double _Complex
</pre>
Many others are automatically registered as well.  SCORE provided
variables containing the index and the names.  They are:
<pre>
       Integer ID                  String Name
     SC_BOOL_I                   SC_BOOL_S
     SC_CHAR_I                   SC_CHAR_S
     SC_SHORT_I                  SC_SHORT_S
     SC_INT_I                    SC_INT_S
     SC_LONG_I                   SC_LONG_S
     SC_LONG_LONG_I              SC_LONG_LONG_S
     SC_FLOAT_I                  SC_FLOAT_S
     SC_DOUBLE_I                 SC_DOUBLE_S
     SC_LONG_DOUBLE_I            SC_LONG_DOUBLE_S
     SC_FLOAT_COMPLEX_I          SC_FLOAT_COMPLEX_S
     SC_DOUBLE_COMPLEX_I         SC_DOUBLE_COMPLEX_S
     SC_LONG_DOUBLE_COMPLEX_I    SC_LONG_DOUBLE_COMPLEX_S
     SC_POINTER_I                SC_POINTER_S
     SC_STRING_I                 SC_STRING_S
     SC_VOID_I                   SC_VOID_S
</pre>

With the identification of types with C objects which can be passed
to functions it is possible to handle certain common situations with
one function call rather than a decision tree and a collection of
macros.
<p>
The type manager functions provided are:

<pre>
<I>C Binding: </I>int SC_register_type(char *name, int bpi, ...)
</pre>

Registers the type specified by NAME.  The size of the type in bytes
is BPI.  No other arguments are processed at present, in future,
key/value pairs may be provided to add other descriptive information
of the type.  The integer id for the newly registered type is returned
if successful. Otherwise -1 is returned.
<p>

<pre>
<I>C Binding: </I>int SC_type_alias(char *name, int id)
</pre>

Sometimes you want to refer to a type by more than one name.  For
example, SC_ENUM_I is an alias for SC_INT_I (since C enums are
implemented as ints).  The name of the new type is given in NAME and
the id of the existing type is given in ID.
<p>

<pre>
<I>C Binding: </I>int SC_type_id(char *name, int unsg)
</pre>
<pre>
<I>C Binding: </I>char *SC_type_name(int id)
</pre>

These two functions are the way to lookup types.  The first returns
the integer id given the name.  The second returns the name given the
id.  The UNSG argument looks for the "unsigned" qualifier if TRUE.
Unsigned types do not differ from signed types as far as the type
manager is concerned.
<p>

<pre>
<I>C Binding: </I>int SC_fix_type_id(char *name, int unsg)
</pre>
<pre>
<I>C Binding: </I>int SC_fp_type_id(char *name)
</pre>
<pre>
<I>C Binding: </I>int SC_cx_type_id(char *name)
</pre>

These functions are provided with specific knowledge of just the
standard C99 types.  SC_fix_type returns the id of the type specified
by NAME only if it is a fixed point type: short, int, long, or long long.
SC_fp_type_id returns the id of the type speicified by NAME only if
it is a floating point type: float, double, or long double. Finally,
SC_cx_type_id returns the id the the type specified by NAME only if
it is a complex type: float _Complex, double _Complex, long double _Complex.
These all return -1 if the type is not of the requested kind.
<p>

<pre>
<I>C Binding: </I>int SC_is_type_fix(int id)
</pre>
<pre>
<I>C Binding: </I>int SC_is_type_fp(int id)
</pre>
<pre>
<I>C Binding: </I>int SC_is_type_cx(int id)
</pre>
<pre>
<I>C Binding: </I>int SC_is_type_num(int id)
</pre>
<pre>
<I>C Binding: </I>int SC_is_type_ptr(int id)
</pre>

These functions are provided with specific knowledge of just the
standard C99 types.  SC_is_type_fix returns TRUE if the type ID is
a fixed point type: short, int, long, or long long.
SC_is_type_fp returns TRUE if type ID is a floating point
type: float, double, or long double.
SC_is_type_cx returns TRUE if type ID is a complex
type: float _Complex, double _Complex, long double _Complex.
SC_is_type_num returns TRUE if type ID is a any of the above mention
numeric types.
SC_is_type_ptr returns TRUE if type ID is a any pointer type.
These all return FALSE if the type is not of the requested kind.
<p>

<pre>
<I>C Binding: </I>int SC_type_size_i(int id)
</pre>
<pre>
<I>C Binding: </I>int SC_type_size_a(char *name)
</pre>

These return the size in bytes of the specified type.  They are
analogous to the sizeof operator.
<p>

<pre>
<I>C Binding: </I>int SC_deref_id(char *name, int base)
</pre>
<pre>
<I>C Binding: </I>char *SC_dereference(char *name)
</pre>

These functions "dereference" the type specified in NAME by removing
the last '*' in the string if it exists.  If BASE is TRUE all
'*' are removed and the id of the base type is returned.
<p>

<pre>
<I>C Binding: </I>void *SC_convert_id(int did, void *d, long od, long sd,
		    int sid, void *s, long os, long ss,
		    long n, int flag)
</pre>
<pre>
<I>C Binding: </I>void *SC_convert(char *dtype, void *d, long od, long sd,
		 char *stype, void *s, long os, long ss,
		 long n, int flag)
</pre>

These functions convert arrays of data from one type to another. They
differ in that SC_convert specifies the types by name and SC_convert_id
specifies the types by id.  The arguments are:
<pre>
   DID/DTYPE  the destination type    
   D          a pointer to the destination data
   OD         an offset from the start of D
   SD         a stride thru D
   SID        the source type
   S          a pointer to the source data
   OS         an offset from the start of S
   SS         a stride thru S
   N          the number of elements to convert
   FLAG       if TRUE the source array is freed with SFREE
</pre>
If D is NULL a new space of the apropriate size will be allocated
and returned.
The return if the space D.
<p>

<pre>
<I>C Binding: </I>char *SC_ntos(char *t, int nc, int id, void *s, long n, int mode)
</pre>

This function renders the Nth data item of type ID in S as a string.  The
result goes into the buffer T which is NC characters long.  The format
specifications can be set by the application.  The values of MODE can be
1 or 2.  Mode 1 is for single values per line, mode 2 is used when printing
multiple values per line.
<p>

<pre>
<I>C Macro: </I>SC_TYPEOF(type)
</pre>

This returns the type id associated with the type TYPE.
<p>

<pre>
<I>C Macro: </I>SC_VA_ARG_FETCH(did, d, sid)
</pre>

This converts a variable arg item of type SID to an item of type index DID
and places the results in D.
<p>
Example:
<pre>
   int *get(int n, ...)
      {int id;
       int *a;
       char *name;

       SC_VA_START(n);

       a = MAKE_N(int, n);

       while ((id = SC_VA_ARG(int)) != -1)
          {if (SC_is_type_num(id) == TRUE)
	      {SC_VA_ARG_FETCH(SC_INT_I, a+i, id);};};

       SC_VA_END;

       return(a);}

   int *a;

   a = get(0, SC_INT_I, 3, SC_LONG_I, 4, SC_DOUBLE_I, 5.0, -1);

</pre>

<pre>
<I>C Macro: </I>SC_VA_ARG_STORE(id, s)
</pre>

This stores a value S of type ID to a the space pointed to by the pointer
obtained from the variable arg list.
<p>
Example:
<pre>

   void set(int n, ...)
      {int id;
       char *name;
       void *v;

       SC_VA_START(n);

       while ((id = SC_VA_ARG(int)) != -1)
         {v = SC_VA_ARG(void *);
          if (SC_is_type_num(id) == TRUE)
	     {SC_VA_ARG_STORE(id, v);};};

       SC_VA_END;

       return;}

   int a;
   long b;
   double c;

   set(0, SC_INT_I, &a, 3, SC_LONG_I, &b, 4, SC_DOUBLE_I, &c, 5.0, -1);

</pre>


<pre>
<I>C Macro: </I>SC_VA_ARG_NTOS(s, nc, id)
</pre>

This renders a variable arg item of type ID to a string S
of NC characters length.
<p>

Example:
<pre>
   void show(int n, ...)
      {int id;
       char bf[100];

       SC_VA_START(n);

       while ((id = SC_VA_ARG(int)) != -1)
         {if (SC_is_type_num(id) == TRUE)
	     {SC_VA_ARG_NTOS(bf, 100, id);
	      fputs(bf, stdout);};};

       SC_VA_END;

       return;}
</pre>

<!-------------------------------------------------------------------------->
<!-------------------------------------------------------------------------->

<a name="score.time">
<h3>Time and Date Handling</h3>
</a>

The SCORE routines dealing with time and date are provided mainly for
true portability and standardization.<p>
<p>

<pre>
<I>C Binding: </I>double SC_cpu_time(void)
<I>F77 Binding: </I>
<I>SX Binding: </I>
</pre>

Return the combined user and system processor time in seconds and microseconds
consumed since the first call to <tt>SC_cpu_time</tt>. Most systems limit actual time
resolution to 0.01 seconds to reduce overhead.
<p>

<pre>
<I>C Binding: </I>double SC_wall_clock_time(void)
<I>F77 Binding: </I>
<I>SX Binding: </I>
</pre>

Return the wall clock time in seconds and microseconds since the first call
to SC_wall_clock_time. Most systems limit actual time resolution to 0.01
seconds to reduce overhead.
<p>

<pre>
<I>C Binding: </I>char *SC_date(void)
<I>F77 Binding: </I>
<I>SX Binding: </I>
</pre>

Get the time and date. This is a guaranteed to work version of the
standard C library function ctime.
<p>

<!-------------------------------------------------------------------------->
<!-------------------------------------------------------------------------->

<a name="score.mem">
<h3>Memory Management</h3>
</a>

<a name="score.mmover">
<b>Overview of the SCORE Memory Manager</b>
</a>

SCORE implements a layer of memory management over the system memory manager
to provide several services that most system memory managers don't.  These services
provide information about managed memory which enable advanced usage of
dynamic memory or they assist applications in debugging situations involving
overwritten memory, leaked memory, or recycled memory.
<p>

System memory managers hand out pointers to dynamically allocated spaces.
Sometimes in a generic routine of an application it would be useful to
be able to lookup the number of bytes and the type of the data to which a
pointer refers.  The SCORE memory manager provides this service. The
routines <tt>SC_arrlen</tt> and <tt>SC_arrtype</tt> will return the number 
of bytes and an
integer type index for any pointer allocated by the SCORE memory manager.
The PACT PDB library uses this capability extensively.
<p>

Some applications use dynamic memory in very complex ways which make it
difficult to know when it is safe to release a piece of dynamically
allocated memory.  Reference counting is a good technique in many cases.
The SCORE memory manager provides functions to let applications tag
pointers with the number of references and <tt>SC_free</tt> (and <tt>SFREE</tt>)
act properly with respect to the number of references.
<p>

Application developers frequently need to monitor their memory usage for
one reason or another.  The SCORE memory manager provides functions to
keep track of how much memory has been allocated and freed.  It also
can compare snapshots of its own heaps of memory in order to detect and
report memory leaks.  Some versions of the SCORE memory manager allocation
routines allow you to supply a "name" with the allocation call.  In
such a case the memory leak report tells you the "name" of the leaked
memory.  This makes it easier to identify the leaked memory than a
simple memory address does.
<p>

The SCORE memory manager provides diagnostics to monitor the integrity
of the heap.  This can be extremely useful in detecting when some dynamic
space is over or under indexed and memory consequently corrupted.  This
scheme is not 100% effective because it isn't doing index checking on
writes, however it very often detects that memory has been corrupted by
an over or under indexed write.
<p>

Another useful feature is that the application can specify whether
dynamic memory bytes are to be initialized to zero on allocation 
and/or reset to zero after being freed.  This patterning of memory can
be very effective in finding problems where pointers are being incorrectly
used after being freed or when multiple pointers refer to the same
chunk of dynamic memory.
<p>

The SCORE memory manager manages its own heap(s) in such a way as to be
more efficient than many system memory managers.  Consequently in applications
where the dynamic memory is churned heavily, it can yield marked improvement
in performance.  In thread parallel applications it provides an even bigger
benefit by managing heaps on a per thread basis and avoiding the overhead
of locking and unlocking memory management operations.
<p>

<a name="score.mmdesc">
<b>Description of the SCORE Memory Manager</b>
</a>

In the model used by the SCORE memory manager, memory is thought of as
being dispensed from a bin.  Each bin contains blocks of memory of a
certain size.  A request for a certain number of bytes of memory finds
a block from the bin containing the smallest sized blocks big enough
to hold the requested number of bytes.  Requests for more space than
the largest bin holds are turned over to the system memory manager.
<p>

The SCORE memory manager maintains a data structure called a heap for each
active thread in an application.  Each heap contains an array of lists
of free memory blocks or free lists.  The array of free lists is indexed
by byte size.  Each free list represents the bin from which a certain size
of memory will be allocated.
<p>

In this strategy it is recognized that a fraction of the managed memory
will be wasted in that it will be unused some of the time.  The application
can control this by determining the byte sizes of the bins.  If you have
a large number of bins you will have a smaller amount of wasted space.  On
the other hand, a large number of bins means that the memory manager heaps
take up more space.  Fortunately you can control this with <tt>SC_configure_mm</tt>.
<p>

A common kind of memory usage involves allocating and freeing small things
repeatedly.  These may be single instances of structures, strings, or
small arrays of numbers.  You would like to be fairly space efficient for
such operations.  It is also common enough to have a range of large memory
spaces which are allocated and freed in cycles.  You want to be able to
cover a wide range of sizes but perhaps don't mind wasting a modest fraction
of the space.  These considerations led to a binning strategy in which
there is a linear region which increases in size by 8 bytes at a time
(i.e 8, 16, 24, 32, ...) and some point the binning cuts over to exponential
to permit a wide range of sizes to be covered.
<p>

When memory is requested, the number of bytes is hashed to a bin.  The first
element of the free list for that bin is popped off the free list and
returned.  It may or may not be zeroed out depending on the flag set by
<tt>SC_zero_space</tt>.  It is also marked as having a reference count of 0.
Each block is also inserted into a doubly linked list of active blocks.
This permits the SCORE memory manager to check the integrity of managed
memory as the application runs (via calls to <tt>SC_mem_chk</tt>).
<p>

Each memory block has a header that contains, in addition to the reference
count, the size in bytes of allocation request.  This can be accessed
by <tt>SC_arrlen</tt> at any time until the block is freed.
<p>

When a pointer to allocated memory is handed to <tt>SC_free</tt>, several things
are done.  The reference count is decremented.  If the new count is less
than 1, the block is added to the free list of the bin whose size it
hashes to.  The block may or may not be zeroed out depending on the value
of the flag set by <tt>SC_zero_space</tt>.
<p>

When the SCORE memory manager finds an empty free list while trying to
honor an allocation request, it goes out to the system memory manager.
It gets a "big" block of space from the system memory manager and then
breaks it into the as many blocks as possible.  Each block is the size
appropriate to the bin it is intended for.  It will always get at least
one blocks worth of space.  Each of these blocks is added to the free
list for the bin.  The size of the "big" block can be set with
<tt>SC_configure_mm</tt>.  This is the maximum block size parameter discussed
below.
<p>

As an application is running, the SCORE memory manager has two kinds
of memory blocks: blocks on the free lists; and blocks in the linked
list of active blocks.  Using the function <tt>SC_mem_chk</tt>, each kind of
memory can be checked for integrity.  This is often useful to detect
when memory overwrites in the application have corrupted memory.
<p>

A new feature of the SCORE memory manager is that the memory allocated
in the "big" blocks and the memory allocated to honor requests above
the maximum bin size can be protected via the mprotect system call.
This will be referred to as guarded or protected memory.  The functions
which manage these protected blocks of memory form a subsystem of the
SCORE memory manager referred to as the Protected SCORE Memory
Manager (PSMM).  You can control this feature with the new call,
<tt>SC_use_guarded_mem</tt>.
<p>

All this means that attempts to access memory before or after the allocated
space results in a SIGSEGV being issued.  This is very handy for
tracking down certain application bugs involving dynamic memory usage.
Because the mprotect call deals in units of memory pages, usually
only access beyond the memory block cause a SIGSEGV to be raised.
By using <tt>SC_configure_mm</tt> to set the maximum bin size to 0 all memory
allocated by the SCORE memory manager can be protected.
<p>

Protected memory doesn't solve all memory related problems.  You can
still have bad memory accesses that don't hit protected memory.  The
use of <tt>SC_mem_chk</tt> is good for finding some other kinds of bad memory
accesses.  When trying to track down bad memory acccess bugs different
strategies will need to be employed in different situations.  Sometimes
the bad memory accesses come from other libraries which are loaded into
your application.  A new library has been added to PACT, libpsmm.a.
This provides a <tt>malloc</tt>, <tt>calloc</tt>, <tt>free</tt>,
and <tt>realloc</tt> interface to the
PSMM.  If you link with libpsmm.a before the system version (often
in libc.a) you will have all memory allocations, even from code you
don't control, protected.
<p>

It is a very sad fact of life that when an application corrupts memory,
it doesn't always immediately crash.  Instead, it wanders off
somewhere and dies.  Where it eventually dies may have nothing to do
with where the damage occurred.  In particular, it sometimes dies on
the next attempt to allocate memory.  If you see your application crash
in the SCORE memory manager, you should immediately suspect that your
application has corrupted memory.  The SCORE memory manager has been
used heavily in many different kinds of environments for many years.
There may still be bugs in it, but you are unlikely to have found one.
You should check your application first as hard as that may be to do.
<p>

<a name="score.mmconf">
<b>Configuring the Memory Manager</b>
</a>

You may configure the SCORE memory manager to optimize its performance
for your application's pattern of memory usage.  This is done by
calling <tt>SC_configure_mm</tt>.  It should be called before any other operations
by the SCORE memory manager.  The SCORE memory manager will call
<tt>SC_configure_mm</tt> itself to setup the defining parameters with default
values chosen to give good performance for an "average" memory usage
scenario.  <tt>SC_configure_mm</tt> works only once.  Any subsequent calls are
reduced to no-ops.
<p>

The controls that you exert are to set: the cut over point between the
linear and exponential region referred to as the maximum block size
for the linear region; the target maximum managed block size; the bin size
ratio in the exponential region; and the maximum block size for requests
from the system memory manager.
<p>

<tt>SC_configure_mm</tt> computes the actual number of bins to set up for each
heap.  You supply numbers that characterize or reflect the sizes of
blocks of memory you use and how much space you are willing to be wasted.
<p>

If the size of an allocation request exceeds the maximum managed block
size, this request is passed on to the system memory manager.  Such a
block will also be passed back to the system memory manager when freed.
In a parallel application, each such transaction would have a lock around
the allocation and free.  On the other hand, when requests are less than
the maximum managed block size, any transaction would only lock when
memory had to be obtained from the system memory manager to replenish
an empty free list.  This is why the SCORE memory manager performs well
in parallel applications.  It is also why spaces allocated in a thread
must be freed in the same thread.  Because there are no locks the
application must avoid the potential conflict inherent in having different
threads operate on the same heap at the same time.
<p>

The default values are: 128 bytes for the maximum block size for the
linear region; 4096 bytes for the target maximum managed block size; 4096
bytes for the maximum block size for requests from the system memory
manager; and 1.1 for the bin size ratio in the exponential region.

These values result in 51 bins with upper boundaries:
<br>
<pre>
    8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120, 128,
</pre>

for the linear region; and
<br>
<pre>
    144, 160, 176, 200, 224, 248, 272, 304, 336, 376, 416, 464, 512,
    568, 624, 688, 760, 840, 928, 1024, 1128, 1240, 1368, 1504, 1656,
    1824, 2008, 2208, 2432, 2680, 2952, 3248, 3576, 3936, 4336
</pre>

for the exponential region. Notice that the target maximum managed block
size was exceeded.  It is only a target.  We must guarantee that
a chunk of memory is aligned to an 8 byte boundary. As the bin boundaries
are being computed, each boundary is rounded up to the nearest 8 bytes
and the ratio is applied to get the next boundary.
<p>

Now observe that the worst case of absolute memory waste occurs when
you request a 3937 byte chunk of memory.  You will actually be getting
an 4336 byte chunk.  You will have wasted 399 bytes or 9.2% of
this memory.  On the other hand, you have sped up the allocation
considerably by making it possible to hash right to a pool of available
memory.  This is the tradeoff. If you select a smaller ratio, you get less
waste.  You also get more memory tied up in the memory manager.  These are
all choices that you get to make.
<p>


<a name="score.mmlk">
<b>Tracking Down Memory Leaks</b>
</a>

A nagging problem with dynamically allocated memory is improper accounting
of memory blocks.  The worst case of this is the memory leak. That is when an
application inadvertently loses any reference to a block of memory and
is therefore unable to free it.  The SCORE memory manager supplies two
functions, <tt>SC_mem_map</tt> and <tt>SC_mem_monitor</tt>, 
to help identify memory accounting problems.

<tt>SC_mem_map writes</tt> an ASCII text file listing all of the memory blocks
(not marked as permanent) currently being managed by the SCORE memory manager.
The address, byte length, and, in some cases, the name associated with
each memory block is listed.

<tt>SC_mem_monitor</tt> is used to monitor applications for memory leaks.  It takes a
differential look at the memory being managed by comparing the results of
an <tt>SC_mem_map</tt> before and after a region of code.  To use it
you make a call at the beginning of a section of code you want to monitor.
This first call returns the number of bytes of memory which the SCORE memory
manager currently thinks the applications has actively allocated.  The
second call at the end of the section of code being monitored takes that
number of bytes as the first argument.  If the number of bytes differs
<tt>SC_mem_monitor</tt> concludes there is a leak.  This may not be precisely true
in that the change in the number of bytes could be positive or negative.
In the first case memory was allocated between the two calls which was not
freed.  In the second case memory was freed between the two calls which
was not reallocated.
<p>
When a leak is detected there are 3 alternative treatments which <tt>SC_mem_monitor</tt>
does: 0, do nothing; 1, report the number of leaked bytes; and 2, report
the number of leaked bytes and list the blocks of memory involved.  This
last is most detailed and is very useful for tracking down which space(s)
leaked.  The report of the number of leaked bytes is made into a character
array supplied by the application.
<p>
Here is an example of the use of <tt>SC_mem_monitor</tt>:
<pre>
    int rv, ret;
    char s[MAXLINE];

    rv = SC_mem_monitor(-1, 2, "A", s);

         ... your code here ...

    ret = SC_mem_monitor(rv, 2, "A", s);
</pre>
In the example you want to determine whether or not a section of code
(i.e. ... your code here ...) is leaking memory.  You make a call to
<tt>SC_mem_monitor</tt> before the code of interest saving the return value which
will be used as input to the second call after the section you are
checking.  When you are doing the first call to get the current state of
the memory and reporting the number of bytes active, the first argument
must have a value of -1.  Now when you use <tt>SC_mem_monitor</tt>, you must decide
what or how much information you want from it.
In this case we are asking for level 2 information which is the
most detailed.  After the first call there will be a temporary file, mem-u-A.before
in the directory where the program is being run.  This is the raw output
of <tt>SC_mem_map</tt>.  This will be sorted alphabetically into the file,
mem-s-A.before. When the second call is made a file mem-u-A.after will be
written by <tt>SC_mem_map</tt>.
It will be sorted into mem-s-A.after.  Next the two sorted files are diff'ed
into a file mem-u-A.diff.  If there is a difference, this file is printed to
the terminal. All the files except, mem-u-A.diff are deleted as soon as possible.
<p>
If the second argument to <tt>SC_mem_monitor</tt> had been 1,
the <tt>SC_mem_map</tt> calls would
not have been made and there would have been no files created.  Only the number
of bytes difference would have been reported.
<p>
NOTE: All arguments but the first must be the same in the pairs of
<tt>SC_mem_monitor</tt> calls.
<p>

<a name="score.mmfnc">
<b>Memory Manager Functions</b>
</a>

<pre>
<I>C Binding: </I>int SC_arrlen(void *ptr)
<I>F77 Binding: </I>integer scalen(pointer ptr)
<I>SX Binding: </I>
<I>Python Binding: </I>
</pre>

Given a pointer to space allocated with <tt>SC_alloc</tt> or <tt>SC_realloc</tt> 
extract the number of
bytes to which the pointer points and return it. WARNING: this function can fail to
recognize the presence of a pointer allocated statically or with malloc, calloc, or
realloc! The FORTRAN binding is intended for use in FORTRAN implementations which
support the pointer extension.
<p>
The number of bytes pointed to by <em>ptr</em> is returned if successful,
and -1 if not.
<p>

<pre>
<I>C Binding: </I>void *SC_alloc(long nitems, long bytepitem)
<I>F77 Binding: use</I> scmake
<I>SX Binding: memory management is automatic</I> 
<I>Python Binding: </I>
</pre>
Allocate a new space in memory <em>nitems * bytepitem</em> long and return a pointer to it.
The arguments are:
<em>nitems</em>, the number of items (e.g. floats);
and <em>bytepitem</em>, the number of bytes per item.<p>
Returns a non-NULL pointer to a newly allocated space if successful and NULL if not.
The pointer should be cast to the appropriate type in C.
<p>

<pre>
<I>C Binding: </I>void SC_configure_mm(long mxl, long mxm, long bsz, double r)
<I>F77 Binding: </I> scmmcf(integer mxl, integer mxm, integer bsz, real r)
<I>SX Binding: memory management is automatic</I>
<I>Python Binding: </I>
</pre>

Configure the SCORE memory manager. This is used to set parameters which control
the function of the SCORE memory manager.  When matched to the pattern of
memory usage in an application the result is improved performance with respect
to the allocation and freeing of dynamic memory.  See the discussion of the
<a href="#score.mmdesc">SCORE memory manager</a>
above.
The arguments are:
<em>mxl</em>, the maximum block size for the linear region;
<em>mxm</em>, the target maximum managed block size;
<em>bsz</em>, the maximum block size requested from the system;
and <em>r</em>, the bin size ratio in the exponential region.
<p>

<pre>
<I>C Binding: </I>int SC_free(void *ptr)
<I>F77 Binding: use</I> scfree
<I>SX Binding: memory management is automatic</I>
<I>Python Binding: </I>
</pre>

Release the space pointed to by <em>ptr</em>. 
Returns <tt>TRUE</tt> if successful, <tt>FALSE</tt> otherwise.
<p>

<pre>
<I>C Binding: </I>int SC_mem_chk(int n)
<I>F77 Binding: </I>integer scmemc(integer n)
<I>SX Binding: </I>
<I>Python Binding: </I>
</pre>

Return the number of chunks of memory being managed or an error condition.
The bits of the argument <em>n</em> determine exactly which checks are to
be done. The 1 bit causes the active allocated memory to be checked.  The 2 bit
causes all of the free lists to be checked.<p>  So a value of 3 does the most
extensive checking.
Return -1 if the active memory has been corrupted, -2 if free lists have
been corrupted, -3 if both have been corrupted; otherwise return
the total number of chunks of memory being managed. NOTE: memory may have
been sufficiently corrupted that this check will result in a segmentation
violation.  This fact does NOT invalidate this function as a diagnostic.
<p>

<pre>
<I>C Binding: </I>int SC_mem_map(FILE *fp, int flag)
<I>F77 Binding: </I>integer scmemp(integer flag)
<I>SX Binding: </I>(memory-map fp)
<I>Python Binding: </I>
</pre>

This function is used to dump a map of the memory blocks which the SCORE
memory manager currently tracks.  The map is printed to the supplied file
(could be stdout) in C and to the terminal in FORTRAN.  If <em>flag</em> has the
1 bit set, memory being used by PACT is not listed.  This helps applications
see only their own memory.
Return the number of blocks of memory that are in the map.
<p>

<pre>
<I>C Binding: </I>int SC_mem_monitor(int old, int lev, char *id, char *msg)
<I>F77 Binding: </I>integer scmemm(integer old, integer lev, integer ni,
                            character*(*) id, integer ncm, character*(*) msg)
<I>SX Binding: </I>(memory-monitor old lev id)
<I>Python Binding: </I>
</pre>

This function is used to monitor applications for memory leaks.
See the section
<a href="#score.mmlk">Tracking Down Memory Leaks</a>
above for a detailed discussion of how to use it.
When a leak is detected there are 3 alternative treatments which
<tt>SC_mem_monitor</tt>
does: 0, do nothing; 1, report the number of leaked bytes; and 2, report
the number of leaked bytes and list the blocks of memory involved.  This
last is most detailed and is very useful for tracking down which space(s)
leaked.  The report of the number of leaked bytes is made into a character
array supplied by the application.
<p>
The arguments are:
<em>old</em>, the number of bytes expected (-1 for the first call
and the return value from the first call for the second call);
<em>lev</em>, the treatment of a leak condition;
<em>id</em>, a base identifier for temporary files;
and <em>msg</em>, a character array from the application to hold an error message.
<p>

<pre>
<I>C Binding: </I>int SC_mem_stats(long *al, long *fr, long *df, long *mx)
<I>F77 Binding: </I>integer scmems(integer al, integer fr, integer df, integer mx)
<I>SX Binding: </I>
<I>Python Binding: </I>
</pre>

This function is used to obtain statistics about application memory usage.
The total number of bytes allocated is returned in <em>al</em>.  The number of
bytes freed is returned in <em>fr</em>.  The difference between bytes allocated
and freed is returned in <em>df</em>, and the maximum number of memory in use
at one time is returned in <em>mx</em>.
<p>

<pre>
<I>C Binding: </I>int SC_mem_stats_acc(long a, long f)
<I>F77 Binding: </I>
<I>SX Binding: </I>
<I>Python Binding: </I>
</pre>

This function is used to change the memory statistics counters. The arguments
<em>a</em> and <em>f</em> will be added to the allocated and freed counters
respectively.
<p>

<pre>
<I>C Binding: </I>int SC_mem_stats_set(long a, long f)
<I>F77 Binding: </I>
<I>SX Binding: </I>
<I>Python Binding: </I>
</pre>

This function is used to set the memory statistics counters.  It is useful
if an application wants to track memory usage in a particular section of
code, for example.  In this case the application can call <tt>SC_mem_stats_set</tt>
with arguments of zero to initialize the counters and then call <tt>SC_mem_stats</tt>
later to get the statistics on memory usage since the call to <tt>SC_mem_stats_set</tt>.
<p>
The arguments are:
<em>a</em>, the value to store in the allocated counter and <em>f</em>, the
value to store in the freed counter.
<p>

<pre>
<I>C Binding: </I>void *SC_realloc(void *ptr, long nitems, long bytepitem)
<I>F77 Binding: use</I> screma
<I>SX Binding: memory management is automatic</I>
<I>Python Binding: </I>
</pre>

Reallocate the space in memory associated with ptr so that it is nitems x bytepitem
long and return a pointer to it. Copy the contents of the old space into the new space
if necessary, but preserve the original contents pointed to.
<em>Ptr</em> must be a pointer to a space previously allocated by <tt>SC_alloc</tt>.
The other argument are:
<em>nitems</em>, the number of items (e.g. floats);
and <em>bytepitem</em>, the number of bytes per item.<p>
Returns a non-NULL pointer to a newly allocated space if successful and NULL if not.
The pointer should be cast to the appropriate type in C.
<p>

<pre>
<I>C Binding: </I>char *SC_strsave(char *s)
<I>F77 Binding: </I>
<I>SX Binding: </I>
<I>Python Binding: </I>
</pre>

Allocate a new space in memory large enough to contain the char array <em>s</em>,
copy its contents to the new space,
and return a pointer to it. <em>S</em> must be a null terminated
array of characters. <p>
Return a non-NULL pointer to a newly allocated space if successful and NULL if not.
<p>

<pre>
<I>C Binding: </I>char *SC_trap_pointer(void *p, int sig)
<I>F77 Binding: </I>
<I>SX Binding: </I>
<I>Python Binding: </I>
</pre>

Arrange for the memory manager to raise signal <em>sig</em> when address <em>p</em> is allocated, 
reallocated, or freed.  It is the responsibility of the application to handle the
signal that will be raised.  To turn off pointer trapping use <tt>SC_untrap_pointer</tt>.
<p>

<pre>
<I>C Binding: </I>char *SC_untrap_pointer(void *p)
<I>F77 Binding: </I>
<I>SX Binding: </I>
<I>Python Binding: </I>
</pre>

Undo the trap on pointer setup by a call to <tt>SC_trap_pointer</tt>.
<p>

<pre>
<I>C Binding: </I>void SC_use_c_mm(void)
<I>F77 Binding:</I>
<I>SX Binding: </I>
<I>Python Binding: </I>
</pre>

Use the C library memory management functions directly for allocation, reallocation
and freeing memory.  This function and <tt>SC_use_score_mm</tt> set the SCORE memory
manager hooks to the appropriate set of lower level functions to be called.  These
hooks can only be accessed by using the SCORE memory manager macros (<tt>MAKE</tt>, 
<tt>FMAKE</tt>, etc.) to allocate, reallocate and free memory. 
<p>

<pre>
<I>C Binding: </I>void SC_use_score_mm(void)
<I>F77 Binding:</I>
<I>SX Binding: </I>
<I>Python Binding: </I>
</pre>

Use the SCORE memory management functions for allocation, reallocation
and freeing memory.  This function and <tt>SC_use_c_mm</tt> set the SCORE memory
manager hooks to the appropriate set of lower level functions to be called.  These
hooks can only be accessed by using the SCORE memory manager macros to allocate,
reallocate and free memory.
<p>

<pre>
<I>C Binding: </I>void SC_use_guarded_mem(int on)
<I>F77 Binding:</I>integer scgmem(integer on)
<I>SX Binding: </I>
<I>Python Binding: </I>
</pre>

Turn on memory protection if the argument is not 0 and turn it off otherwise.
<p>
<p>

<pre>
<I>C Binding: </I>type *FMAKE(type, name)
<I>F77 Binding: </I> use scmakf
<I>SX Binding: memory management is automatic</I> 
<I>Python Binding: </I>
</pre>

Allocate a new space in memory the size of type and return a pointer to it which
has been cast to <em>type *</em>.  Associate name <em>name</em> with the returned block of
memory.  This name will be printed in the report written via a call to <tt>SC_mem_map</tt>
to aid in ferreting out memory leaks.  In the C binding this is a macro and type is a primitive
or derived type specifier.<p>
Returns a non-NULL pointer to a newly allocated space if successful and NULL if not.
<p>
Fortran binding returns 1 iff successful.
<p>

<pre>
<I>C Binding: </I>type *FMAKE_N(type, long ni, name)
<I>F77 Binding: </I>integer scmakf(pointer ptr, integer ni, integer bpi,
                                   integer nc, char *name)
<I>SX Binding: memory management is automatic</I>
<I>Python Binding: </I>
</pre>

Allocate a new space in memory for <em>ni</em> items the size of type (C) or
<em>bpi</em> bytes
each (FORTRAN) and return a pointer to it. In the C binding, which is a macro,
<em>type</em> is a primitive or derived type specifier,
and the return value is a pointer
cast to <em>type *</em>. Name <em>name</em> will be associated with the returned
block of memory.  See documentation of <tt>FMAKE</tt> above.  In the FORTRAN binding, 
which is intended for use in FORTRAN implementations which support the integer 
(a.k.a cray) pointer extension, <em>ptr</em> is a pointer (e.g. <tt>ipa</tt> of 
<tt>pointer (ipa, a)</tt>).  <em>Name</em> is the name to be associated with the
block of memory returned, and <em>nc</em> is the number of characters in <em>name</em>. 
<p>
The C binding returns a non-NULL pointer to a newly allocated space if successful
and NULL if not. The FORTRAN binding returns 1 if successful and 0 if not.
<p>

<pre>
<I>C Binding: </I>type *MAKE(type)
<I>F77 Binding: </I> use scmake
<I>SX Binding: memory management is automatic</I> 
<I>Python Binding: </I>
</pre>

Allocate a new space in memory the size of type and return a pointer to it which
has been cast to type *.  In the C binding this is a macro and type is a primitive
or derived type specifier.<p>
Returns a non-NULL pointer to a newly allocated space if successful and NULL if not.
<p>
Fortran binding returns 1 iff successful.
<p>

<pre>
<I>C Binding: </I>type *MAKE_N(type, long ni)
<I>F77 Binding: </I>integer scmake(pointer ptr, integer ni, integer bpi)
<I>SX Binding: memory management is automatic</I>
<I>Python Binding: </I>
</pre>

Allocate a new space in memory for <em>ni</em> items the size of type (C) or
<em>bpi</em> bytes
each (FORTRAN) and return a pointer to it. In the C binding, which is a macro,
<em>type</em> is a primitive or derived type specifier,
and the return value is a pointer
cast to <em>type *</em>. In the FORTRAN binding, which is intended for use in FORTRAN
implementations which support the integer (a.k.a cray) pointer extension,
<em>ptr</em> is a pointer (e.g. <tt>ipa</tt> of <tt>pointer (ipa, a)</tt>).<p>
The C binding returns a non-NULL pointer to a newly allocated space if successful
and NULL if not. The FORTRAN binding returns 1 if successful and 0 if not.
<p>

<pre>
<I>C Binding: </I>type *REMAKE(void *ptr, type)
<I>F77 Binding: use</I> screma
<I>SX Binding: memory management is automatic</I>
<I>Python Binding: </I>
</pre>

Reallocate the space in memory associated with <em>ptr</em> to the
size of type and return a pointer to it, which has been cast to
<em>type *</em>. Copy the contents of the old space into the new space
if necessary. In the C binding this is a macro and <em>type</em> is a
primitive or derived type specifier.
<p>
Returns a non-NULL pointer to a reallocated space if successful and NULL if not.
<p>

<pre>
<I>C Binding: </I>type *REMAKE_N(void *ptr, type, long ni)
<I>F77 Binding: </I>integer screma(pointer ptr, integer ni, integer bpi)
<I>SX Binding: memory management is automatic</I>
<I>Python Binding: </I>
</pre>

Reallocate the space in memory associated with <em>ptr</em> to be
<em>ni</em> items the size of type (C) or <em>bpi</em> bytes each
(FORTRAN) and return a pointer to it. Copy the contents of the old
space into the new space if necessary. In the C binding, which is a
macro, <em>type</em> is a primitive or derived type specifier, and the
returned pointer is cast to <em>type *</em>. The FORTRAN binding is
intended for use in FORTRAN implementations which support the integer
(a.k.a. cray) pointer extension.
<p>
The C binding returns a non-NULL pointer to a reallocated space if successful
and NULL if not. The FORTRAN binding returns 1 if successful and 0 if not.
<p>

<pre>
<I>C Binding: </I>void SFREE(void *ptr)
<I>F77 Binding: </I>integer scfree(pointer ptr)
<I>SX Binding: memory management is automatic</I>
<I>Python Binding: </I>
</pre>

Release the space pointed to by <em>ptr</em>. The FORTRAN binding is intended for use
in FORTRAN implementations which support the integer (a.k.a. cray) pointer extension
and always returns 1.
<p>

<pre>
<I>C Binding: </I>int SC_zero_space(int flag)
<I>F77 Binding: </I>integer sczrsp(integer flag)
<I>SX Binding: not applicable</I>
<I>Python Binding: </I>
</pre>

If input <em>flag</em> is set to 1 memory will be zeroed out when
allocated and when released.
If <em>flag</em> is 2 memory will be zeroed out on allocation only.
If <em>flag</em> is 3 memory will be zeroed out on release only.
If <em>flag</em> is 0 memory will not be zeroed out.
The zeroing of memory on release can be useful (in spite of the overhead) 
in order to spot the situation where space is freed when more than one pointer 
points to it.  The default is for space not to be zeroed out.
<p>
Returns the old value of the flag.
<p>

<!-------------------------------------------------------------------------->
<!-------------------------------------------------------------------------->

<a name="score.misc">
<h3>Miscellaneous Routines</h3>
</a>

<a name="score.bit">
<b>Bit Level Manipulations</b>
</a>

<p>

<pre>
<I>C Binding: </I>int SC_bit_count(long c, int n)
<I>F77 Binding: </I>
<I>SX Binding: </I>
</pre>

Count the number of set bits in the specified number of bytes of a given long.
<p>

<pre>
<I>C Binding: </I>unsigned int SC_bit_reverse(unsigned int i, int n)
<I>F77 Binding: </I>
<I>SX Binding: </I>
</pre>

Reverse the specified number of bits of a given unsigned int.
<p>

<a name="score.nconv">
<b>Numeric Conversion</b>
</a>

<p>

<pre>
<I>C Binding: </I>int SC_stoi(char *s)
<I>F77 Binding: </I>
<I>SX Binding: </I>
</pre>

Convert a string to an int. Return 0 if the string is null.
<p>

<pre>
<I>C Binding: </I>STRTOL(char *str, char **ptr, int base)
<I>F77 Binding: </I>
<I>SX Binding: </I>
</pre>

Convert a string to a long and return a pointer to any unconverted suffix.
This macro invokes either the standard C library function strtol or the
guaranteed to work SCORE equivalent.
<p>

<pre>
<I>C Binding: </I>double ATOF(char *s)
<I>F77 Binding: </I>
<I>SX Binding: </I>
</pre>

Convert a string to a double. This macro invokes either the standard C
library function atof or the guaranteed to work SCORE equivalent.
<p>

<pre>
<I>C Binding: </I>double SC_stof(char *s)
<I>F77 Binding: </I>
<I>SX Binding: </I>
</pre>

Convert a string to a double. Return 0.0 if the string is null.
<p>

<pre>
<I>C Binding: </I>double STRTOD(char *nptr, char **endptr)
<I>F77 Binding: </I>
<I>SX Binding: </I>
</pre>

Convert a string to a double and return a pointer to any unconverted suffix.
This macro invokes either the standard C library function strtod or the
guaranteed to work SCORE equivalent.
<p>

<a name="score.onum">
<b>Other Numeric</b>
</a>
<p>

<pre>
<I>C Binding: </I>double ABS(double x)
<I>F77 Binding: </I>
<I>SX Binding: </I>
</pre>

Return the absolute value of a double.
<p>

<pre>
<I>C Binding: </I>max(a, b)
<I>F77 Binding: </I>
<I>SX Binding: </I>
</pre>

Return the greater of the two arguments.
<p>

<pre>
<I>C Binding: </I>min(a, b)
<I>F77 Binding: </I>
<I>SX Binding: </I>
</pre>

Return the lesser of the two arguments.
<p>

<a name="score.file">
<b>File Search</b>
</a>
<p>

<pre>
<I>C Binding: </I>int SC_isfile(char *s)
<I>F77 Binding: </I>
<I>SX Binding: </I>
</pre>

Is the string the name of an existing file?
<p>

<pre>
<I>C Binding: </I>int SC_isfile_ascii(char *s)
<I>F77 Binding: </I>
<I>SX Binding: </I>
</pre>

Is the string the name of an existing ascii file?
<p>

<pre>
<I>C Binding: </I>char *SC_search_file(char **directory, char *s)
<I>F77 Binding: </I>
<I>SX Binding: </I>
</pre>

Search a list of directories for a file and return the full path name
if the file exists.  The return string is dynamically allocated and
the application is responsible for releasing it with <tt>SFREE</tt>.
<p>

<a name="score.io">
<b>I/O</b>
</a>
<p>

<pre>
<I>C Binding: </I>PRINT
</pre>

<pre>
<I>C Binding: </I>GETLN
</pre>

<a name="score.intrpt">
<b>Interrupts</b>
</a>
<p>

<pre>
<I>C Binding: </I>void SC_init(char *msg, PFByte fnc, int sighand, PFByte sigfnc, int 
bfhand, char *bf, int bfsize)
<I>F77 Binding: </I>
<I>SX Binding: </I>
</pre>

Setup the interrupt handler, top level longjump, and output
buffering for an application.
<p>

<pre>
<I>C Binding: </I>void SC_interrupt_handler(int sig)
<I>F77 Binding: </I>
<I>SX Binding: </I>
</pre>

Handle interrupts in a default sort of way.
<p>

<a name="score.other2">
<b>Other</b>
</a>
<p>

<pre>
<I>C Binding: </I>void SC_banner(char *s)
<I>F77 Binding: </I>
<I>SX Binding: </I>
</pre>

Display the input string as a banner on stdout.
<p>

<pre>
<I>C Binding: </I>void SC_pause(void)
<I>F77 Binding: </I>integer scpaus(void)
<I>SX Binding: </I>
</pre>

Pause until a character arrives on stdin.
<p>

<!-------------------------------------------------------------------------->
<!-------------------------------------------------------------------------->

<a name="SCPar">
<h3>Parallel Programming with SCORE</h3>
</a>

PACT supports parallel programming in both SMP and distributed models.
In SCORE there is a portable interface over various standards for
threads.  The purpose of this is to allow applications to deal with
threads at a very low level while remaining portable.  There is also
a higher level of thread related routines which help applications manage
threads (e.g. thread pools) and easily parallelize sections of code.
<p>
For distributed parallel support see PPC.
<p>
<a name="SCPar381">
<b>Low Level Thread Interface</b>
</a>

<a name="SCPar382">
<b>High Level Thread Interface</b>
</a>

<pre>
<i>C Binding: </i>void SC_init_threads(int np, PFVoid tid)
<i>F77 Binding: </i>
<i>SX Binding: </i>
<i>Python Binding: </i>
</pre>

Initialize <em>np</em> threads. If the function <em>tid</em> is
supplied it will be used to set its argument to a value between 0 and
<em>np</em>-1.  If <em>tid</em> is NULL SCORE will use its own built
in function.
<p>

<pre>
<i>C Binding: </i>void SC_init_tpool(int np, PFVoid tid)
<i>F77 Binding: </i>
<i>SX Binding: </i>
<i>Python Binding: </i>
</pre>

Initialize a pool of <em>np</em> threads. If the function <em>tid</em>
is supplied it will be used to set its argument to a value between 0
and <em>np</em>-1.  If <em>tid</em> is NULL SCORE will use its own
built in function.  The threads are created and wait to be given work
by <tt>SC_do_threads</tt>.
<p>

<pre>
<i>C Binding: </i>void SC_chunk_loop(PFPByte fnc, char *msg,
			      int mn, int mx,
			      int serial, byte *argl)
<i>F77 Binding: </i>
<i>SX Binding: </i>
<i>Python Binding: </i>
</pre>

This routine is used to break a loop into chunks each of which is done
by a different thread. <em>fnc</em> is a function which does the body
of the loop, <em>mn</em> and <em>mx</em> are the loop limits (i.e.
for (i = <em>mn</em>; i < <em>mx</em>; i++)), <em>serial</em> is a
flag which will force the loop to be done sequentially (this is useful
for debugging), and <em>argl</em> contains a pointer that will be
passed into <em>fnc</em>.  The intention is that the application will
define a struct which contains essentially the information that would
be passed as arguments to <em>fnc</em> if the underlying thread
mechanism were sufficiently general.  An instance of the struct will
be loaded up and a pointer to it passed to <em>fnc</em>.
<p>

<pre>
<i>C Binding: </i>void SC_chunk_split(int *pmn, int *pmx,
                               void **prv)
<i>F77 Binding: </i>
<i>SX Binding: </i>
<i>Python Binding: </i>
</pre>

This routine is used by the function <em>fnc</em> passed to
<tt>SC_chunk_loop</tt>.  It returns the index limits to be covered by
the current thread.  It also sets up the return value for the function
<em>fnc</em>.  This return value, <em>prv</em>, is used to check that
the entire loop range is completed.
<p>

<pre>
<i>C Binding: </i>void SC_queue_work(PFPByte fnc,
                              int serial, byte *argl)
<i>F77 Binding: </i>
<i>SX Binding: </i>
<i>Python Binding: </i>
</pre>

This routine is used to break a loop into chunks each of which is done
by a different thread. This function is used when the body of the loop
does different amounts of work depending on the data involved.  Each
thread asks for the next index using <tt>SC_queue_next_item</tt> or a similar
application supplied function which simply locks and dispenses the
next item of work (or an index to it).  In such a case, it is a good
idea to effect a sort with the greatest amount of work coming first
and the least coming last.  This gives good load balancing when the
number of threads is less than the number of work items to be processed.

<em>fnc</em> is a function which does the body of the loop,
<em>serial</em> is a flag which will force the loop to be done sequentially (this
is useful for debugging),
and <em>argl</em> contains a pointer that will be
passed into <em>fnc</em>.  The intention is that the application will define
a struct which contains essentially the information what would be
passed as arguments to <em>fnc</em> if the underlying thread mechanism were
sufficiently general.  An instance of the struct will be loaded
up and a pointer to it passed to <em>fnc</em>.
<p>

<pre>
<i>C Binding: </i>int SC_queue_next_item(int ng)
<i>F77 Binding: </i>
<i>SX Binding: </i>
<i>Python Binding: </i>
</pre>

This routine is used by the function fnc passed to <tt>SC_queue_work</tt>.
It returns the integer index of the next item of work to be done.
There is nothing special about this function.  The application can
dispense work anyway it sees fit.  This is simply a very useful
special case.
<p>

<a name="score.cons">
<h2 ALIGN="CENTER">SCORE Constants</h2>
</a>

<h4>General Constants</h4>

<h4>File Type Constants</h4>

The following constants are defined in the SC_file_type enumeration:
<BLOCKQUOTE>
<TABLE>
<TR><TD>SC_NO_FILE</TD><td>No file</td></TR>
<TR><TD>SC_ASCII</TD><td>ASCII file</td></TR>
<TR><TD>SC_BINARY</TD><td>Undifferentiated binary file</td></TR>
<TR><TD>SC_PDB</TD><td>PDB file</td></TR>
<TR><TD>SC_TAR</TD><td>TAR container file</td></TR>
<TR><TD>SC_AR</TD><td>AR container file</td></TR>
<TR><TD>SC_OTHER</TD><td>Other container file</td></TR>
<TR><TD>SC_UNKNOWN</TD><td>Unknow file type</td></TR>
</TABLE>
</BLOCKQUOTE>

<h4>Non-local Return Constants</h4>

The following constants are defined in the SC_jmp_return enumeration:
<BLOCKQUOTE>
<TABLE>
<TR><TD>ERR_FREE</TD><td>No error at return</td></TR>
<TR><TD>ABORT</TD><td>Serious error requiring an abort</td></TR>
<TR><TD>RETURN_OK</TD><td>Simple return - no indications</td></TR>
</TABLE>
</BLOCKQUOTE>

<h4>Inter Process Communication Constants</h4>

The following constants are defined in the SC_ipc_mode enumeration:
<BLOCKQUOTE>
<TABLE>
<TR><TD>NO_IPC</TD><td>No IPC</td></TR>
<TR><TD>USE_PTYS</TD><td>Use pseudo terminals for IPC</td></TR>
<TR><TD>USE_SOCKETS</TD><td>Use socketc for IPC</td></TR>
<TR><TD>USE_PIPES</TD><td>Use pipes for IPC</td></TR>
</TABLE>
</BLOCKQUOTE>


<h4>Hash Table Constants</h4>

The following #define'd constants are defined for specifying hash table sizes:
<BLOCKQUOTE>
<TABLE>
<TR><TD>HSZSMALL</TD><td>31</td></TR>
<TR><TD>HSZSMINT</TD><td>67</td></TR>
<TR><TD>HSZLRINT</TD><td>127</td></TR>
<TR><TD>HSZLARGE</TD><td>521</td></TR>
<TR><TD>HSZHUGE</TD><td>4483</td></TR>
</TABLE>
</BLOCKQUOTE>

The following #define'd constants are defined for specifying hash tables
have documentation:
<BLOCKQUOTE>
<TABLE>
<TR><TD>DOC</TD><td>1</td></TR>
<TR><TD>NODOC</TD><td>0</td></TR>
</TABLE>
</BLOCKQUOTE>

<h4>Lexical Scanning Constants</h4>

The following constants are defined in the SC_token_type enumeration:
<BLOCKQUOTE>
<TABLE>
<TR><TD>SC_WSPC_TOK</TD><td>White space</td></TR>
<TR><TD>SC_DELIM_TOK</td><td>delimiter token</td></tr>
<TR><TD>SC_IDENT_TOK</td><td>identifier token</td></tr>
<TR><TD>SC_DINT_TOK</td><td>decimal integer number token</td></tr>
<TR><TD>SC_REAL_TOK</td><td>decimal real number token</td></tr>
<TR><TD>SC_OINT_TOK</td><td>octal integer number token</td></tr>
<TR><TD>SC_HINT_TOK</td><td>hexidecimal integer number token</td></tr>
<TR><TD>SC_OPER_TOK</td><td>operator token</td></tr>
<TR><TD>SC_STRING_TOK</td><td>string token</td></tr>
<TR><TD>SC_KEY_TOK</td><td>keyword token</td></tr>
<TR><TD>SC_PRED_TOK</td><td>predicate token</td></tr>
<TR><TD>SC_CMMNT_TOK</td><td>comment token</td></tr>
<TR><TD>SC_HOLLER_TOK</td><td>hollerith token</td></tr>
</TABLE>
</BLOCKQUOTE>

<a name="score.scstd"></a>
<h3 ALIGN="CENTER">scstd.h Constants</h3>

The following are general purpose #define'd constants:

<BLOCKQUOTE>
<TABLE>
<TR><TD>TRUE</TD><td>1</td></TR>
<TR><TD>FALSE</TD><td>0</td></TR>
<TR><TD>ON</TD><td>1</td></TR>
<TR><TD>OFF</TD><td>0</td></TR>
<TR><TD>MAXLINE</TD><td>255</td></TR>
<TR><TD>MAX_BFSZ</TD><td>4096</td></TR>
<TR><TD>SC_BITS_BYTE</TD><td>8</td></TR>
<TR><TD>SMALL</TD><td>1.0e-100</td></TR>
<TR><TD>HUGE</TD><td>1.0e100</td></TR>
<TR><TD>HUGE_INT</TD><td>Maximum long int value</td></TR>
<TR><TD>FIXNUM</TD><td>long</td></TR>
<TR><TD>REAL</TD><td>double | float</td></TR>
<TR><TD>HUGE_REAL</TD><td>1.0e100 | 1.0e30</td></TR>
<TR><TD>RAND_MAX</TD><td>Maximum random number value</td></TR>
<TR><TD>TICKS_SECOND</TD><td>CPU clock ticks per second</td></TR>
</TABLE>
</BLOCKQUOTE>

The following #define'd constants help with fopen portability:

<BLOCKQUOTE>
<TABLE>
<TR><TD>BINARY_MODE_R</TD><td>&#147;rb&#148;</td></TR>
<TR><TD>BINARY_MODE_W</TD><td>&#147;wb&#148;</td></TR>
<TR><TD>BINARY_MODE_RPLUS</TD><td>&#147;r+b&#148;</td></TR>
<TR><TD>BINARY_MODE_WPLUS</TD><td>&#147;w+b&#148;</td></TR>
</TABLE>
</BLOCKQUOTE>

The following #define'd constants help with fseek portability:

<BLOCKQUOTE>
<TABLE>
<TR><TD>SEEK_SET</TD><td>0 - set file position relative to beginning</td></TR>
<TR><TD>SEEK_CUR</TD><td>1 - set file position relative to current position</td></TR>
<TR><TD>SEEK_END</TD><td>2 - set file position relative to end></TR>
</TABLE>
</BLOCKQUOTE>

The following #define'd constants help with path portability:

<BLOCKQUOTE>
<TABLE>
<TR><TD>director_delim</TD><td>&#147;/&#148; | &#147;\&#148; | &#147;:&#148;</td></TR>
<TR><TD>director_delim_c</TD><td>&#145;/&#146; | &#145;\&#146; | &#145;:&#146;</td></TR>
</TABLE>
</BLOCKQUOTE>


<a name="score.examp">
<h2 ALIGN="CENTER">Examples</h2>
</a>

This section will illustrate the use of the SCORE functions.<p>
<p>

Example of parallelizing a loop with <tt>SC_chunk_loop</tt>:
<p>
<pre>                  .
                  .
                  .

    /* a simple loop */
        for (i = 0; i < n; i++)
            a[i] = sqrt(b[i]);
                  .
                  .
                  .
</pre>
<p>
after adding a struct definition and a function embodying the loop
becomes:
<p>
<pre>    typedef struct sqd
       {double *a;
        double *b;};

    void *body(arg)
       void *arg;
       {int i, mn, mx;
        double *a, *b;
        void *rv;
        sqd *par;

        SC_chunk_split(&mn, &mx, &rv);

        par = (sqd *) arg;
        a = par->a;
        b = par->b;

        for (i = mn; i < mx; i++)
            a[i] = sqrt(b[i]);
   
        return(rv);}

                  .
                  .
                  .

    /* the same loop parallelized */
         {sqd par;
          par.a = a;
          par.b = b;
          SC_chunk_loop(body, 0, n, TRUE, &par);}
                  .
                  .
                  .
</pre>
<br>
<p>

Example of parallelizing a loop with <tt>SC_queue_work</tt>:
<p>
<pre>                  .
                  .
                  .

    /* a simple loop where work takes different times for different
     * input data values
     */
        for (i = 0; i < n; i++)
            a[i] = work(b[i]);
                  .
                  .
                  .
</pre>
<p>
after adding a struct definition and a function embodying the loop
becomes:
<p>
<pre>    typedef struct sqd
       {int n;
        double *a;
        double *b;};

    void *body(arg)
       void *arg;
       {int i, n;
        double *a, *b;
        sqd *par;

        par = (sqd *) arg;
        a = par->a;
        b = par->b;
	n = par->n;

        for (i = SC_queue_next_item(n); i != -1; i = SC_queue_next_item(n))
            a[i] = work(b[i]);
   
        return(NULL);}

                  .
                  .
                  .

    /* the same loop parallelized */
         {sqd par;
          par.a = a;
          par.b = b;
          par.n = n;
          SC_queue_work(body, TRUE, &par);}
                  .
                  .
                  .
</pre>
<p>

<a name="score.docs">
<h2 ALIGN="CENTER">Related Documentation</h2>
</a>

SCORE is one part of a collection of libraries called PACT. It provides
the foundation for PACT. Its functionality includes memory management,
hash table functions, and string handling support. <p>
The list of PACT Documents is:<p>


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
