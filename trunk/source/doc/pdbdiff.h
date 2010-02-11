TXT: PDBDiff User's Manual
MOD: 12/19/2005

<CENTER>
<P>
<H1><font color="#ff0000">$TXT</font></H1>
<H2>A tool for comparing and displaying differences between PACT PDB files</H2>
<H4>Stewart A. Brown</H4>
</CENTER>


<BLOCKQUOTE>

<P>
<HR>

<ul>
<li><a href="#DIFFintro">Introduction</a></li>
<li><a href="#DIFFdata">Data Files with PDBDiff</a></li>
<ul>
<li><a href="#DIFFpva">PVA Files</a></li>
</ul>
<li><a href="#DIFFsyn">PDBDiff Syntax</a></li>
<li><a href="#DIFFstart">Starting PDBDiff</a></li>
<li><a href="#DIFFcoms">PDBDiff Commands</a></li>
<li><a href="#DIFFdflt">Default State</a></li>
<li><a href="#DIFFdocs">Other PACT Documents</a></li>

<a name="DIFFintro"></a>
<H1 ALIGN="CENTER">Introduction</H1>

The PACT system provides a number of tools for working with PDB files in a
fairly generic fashion. In particular, PDBDiff compares the contents of two
PDB files and displays the differences (in a manner similar but not identical
to the UNIX utility diff). PDBDiff can also be run in an interactive mode
which allows you to compare two PDB files on a variable by variable or even
leaf by leaf basis.<p>

PDBDiff is an SX program. SX is the SCHEME dialect of the LISP programming
language with PACT extensions. The extensions provide functionality for
graphics, binary data handling, and other areas of functionality.<p>

PDBDiff has a help command which provides information about available commands. <p>

Note: The files compared may have been created on or for different target machines.
Consequently, a given data type may be represented differently in the two files.
In this event values are compared to the precision in the first file, or the
precision in the second file, or the precision you specify, whichever is least.<p>

<a name="DIFFdata"></a>
<H1 ALIGN="CENTER">Data Files with PDBDiff</H1>

PDBDiff can be used to compare the contents of any two PDB files. It compares
files entry by entry. In the most commonly used mode of use it has no knowledge
of the semantics of the data in the files it compares.<p>

<a name="DIFFpva"></a>
<h2>2.1   PVA Files</h2>

There is a special kind of PDB file called a PVA (Portable Visualization and
Analysis) file which contains data sets structured in a particular way so as
to completely specify all information necessary to visualize the data set or
to perform mathematical operations such as differentiation and integration on
it. PANACEA codes produce PVA files as the PACT supported option for data output.
PDBDiff can compare these files in a special way. Since it has complete information
about the data sets, it can compare data sets which represent the same quantity
but, potentially, with different amounts of information. For example, two runs of
a numerical simulation code might compute a given quantity but with different
resolution on the computational mesh. PDBDiff can compare these data sets by
interpolating them to a common domain and computing the difference measure:<p>

<CENTER>
<img width="326" height="111" src="pdbdiff.gif"><p>
</CENTER>

If the difference measure is less than a specified threshold (specified by
the -t option), PDBDiff goes on to the next mapping in the file. However, if the
difference measure exceeds the threshold, the two mappings, a and b, are displayed
in separate windows along with the integrand above in its own window. The user
is prompted to specify whether or not the differences are acceptable. In this
way PDBDiff can be used as an aide to developing simulation codes and testing them.<p>

<p>

<a name="DIFFsyn"></a>
<H1 ALIGN="CENTER">PDBDiff Syntax</H1>

PDBDiff uses a slightly different data description syntax than PDBLib. With PDBLib
variable names and names of members of structures cannot contain the characters:
&#147;.&#148;, &#147;(&#147;, &#147;)&#148;, &#147;[&#147;, and &#147;]&#148;.
The characters &#147;(&#147;, &#147;)&#148;, &#147;[&#147;, and &#147;]&#148;
are used in array reference and dimension definition expressions. In PDBDiff,
&#147;(&#147; and &#147;)&#148; CANNOT be used in variable reference expressions
because they are special characters for PDBDiff. For example,<p>

<BLOCKQUOTE>
print a(2)
</BLOCKQUOTE>

is illegal and results in an error. The legal expression is:<p>

<BLOCKQUOTE>
print a[2]
</BLOCKQUOTE>

Other examples of legal expressions are:<p>

<BLOCKQUOTE>
print a.b[3,2]
<P>print a.b[3][2]
<P>print a[3].b[2][5].c
<P>print a[3].b[2][3].c[3:4,5:50:10]
</BLOCKQUOTE>

The first two forms are not identical. In the first form an element of a two
dimensional array is being referenced. In the second form, the third element
of the fourth array of arrays is being referenced (assuming zero based indexing
in the definition of the b member of a).<p>

For completeness and clarity in the following discussion, an index expression
is defined as:<p>

<BLOCKQUOTE>
<PRE>
 index expression := [index list]

 index list := index | index, index list

 index := simple index |

          index-min : index-max |                    (*)

          index-min : index-max : increment          (*)

 simple index := integer

 index-min := integer

 index-max := integer

 increment := integer
</PRE>
</BLOCKQUOTE>

If an index expression uses either of the two starred forms it is said to be a
hyper-index expression. A hyper-index expression implies more than one data item.
Only the terminating index expression in a data reference may be a hyper-index
expression. An index expression is said to dereference an indirection (or pointer
or array). For each level of indirection a suitable index expression dereferences
the desired data.<p>

For example, this means that a variable defined as:<p>

<BLOCKQUOTE>
 char **s
</BLOCKQUOTE>

is said to have two levels of indirection and can have parts accessed as follows:<p>

<BLOCKQUOTE>
<TABLE>
<TR><TD>print s</TD><TD>prints the entire item</TD></TR>
<TR><TD>print s[2]</TD><TD>prints the third character array of s</TD></TR>
<TR><TD>print s[3][10]</TD><TD>prints the eleventh character of the fourth character array of s</TD></TR>
</TABLE>
</BLOCKQUOTE>

In the above example a zero based index is assumed.<p>

When referring to part of a variable, especially a structured variable, the
terminal node must be of primitive type or a structure containing no indirections
and whose descendant members contain no indirections.  Furthermore, the path to
the desired part must contain one array reference for each level of indirection
traversed.<p>

<a name="DIFFstart"></a>
<H1 ALIGN="CENTER">Starting PDBDiff</H1>

On UNIX systems a shell script called <B>pdbdiff</B> is provided; it starts up SX
and loads the PDBDiff SCHEME forms. You must add a line to your <B>.cshrc</B> or
<B>.profile</B> file that defines the environment variable SCHEME. This variable
tells SX where to find the PDBDiff program. Consult your system administrator or
some other knowledgeable source to find where this is kept. If, <U>for example</U>,
the directory <B>/usr/local/scheme</B> contains the PDBDiff program file,
add the line:<p>

<BLOCKQUOTE>
setenv SCHEME /usr/local/scheme
</BLOCKQUOTE>

to your <B>.cshrc</B> or <B>.profile</B> file.<p>

Usage:<BR>
<BR>pdbdiff [-a] [-b n] [-d] [-i] [-l] [-p f] [-s] [-t #] [-v] pdb-file1 pdb-file2
<BR>
<BR>
Options:
<P>

<BLOCKQUOTE>
a - display all elements if any differ
<P>b - number of mantissa bits to use in floating point comparisons
<P>d - compare floating point types of differing precision
<P>i - enter interactive mode
<P>l - compare fixed point types of differing precision
<P>p - fractional decimal precision to use in floating point comparisons
<P>s - display only differing elements (default)
<P>t - comparison tolerance for -v option defaults to 1.0e-8
<P>v - compare PVA files (batch mode only currently)
</BLOCKQUOTE>

<a name="DIFFcoms"></a>
<H1 ALIGN="CENTER">PDBDiff Commands</H1>

In this section the PDBDiff commands are listed alphabetically. Each command is
given with a brief description of its function and examples of its usage. The
examples are intended to illustrate the various ways of invoking each command.<p>

<BR><B>CD</B>
<BR>
Change the current file directory in the designated file. The first file is
designated by 1, a, or first and the second file is designated by 2, b, or
second. If neither file is specified the file directory is changed for both
files. The new directory may be specified by either a relative path or a full path.<p>

Usage:	cd [directory] [1 | a | first | 2 | b | second]
<P>Examples:

<BLOCKQUOTE>
 cd /zoo/mammals
<P> cd ../reptiles first
<P> cd
</BLOCKQUOTE>

<BR><B>CLOSE</B>
<BR>
Close the current pair of files.<p>

Usage:	close
<P>

<BR><B>DESC</B>
<BR>
Describe the named variable or structure member in the designated file. The first
file is designated by 1, a, or first and the second file is designated by 2, b,
or second. To get a description of part of a variable or structure member, qualify
the name with index expressions (see the <a href="#DIFFsyn">PDBDiff Syntax</a>
section for a discussion of index expressions).<p>

Usage:	desc variable | structure-member [1 | a | first | 2 | b | second]
<P>Examples:

<BLOCKQUOTE>
desc Mapping1
<P>desc Mapping1.range 1
<P>desc baz second
</BLOCKQUOTE>

<BR><B>DIFF</B>
<BR>
Compare the values of the named variable(s) or structure member(s) in the two
open files. If two variable names are provided, the values of the first variable
in the first file will be compared with the values of the second variable in the
second file. To compare part of a variable or structure member, qualify the name
with index expressions (see the <a href="#DIFFsyn">PDBDiff Syntax</a> section for
a discussion of index expressions). You may specify the format of the output with
the mode and set commands.<p>

Usage:	diff variable | structure-member [variable | structure-member]
<P>Examples:

<BLOCKQUOTE>
diff Mapping3
<P>diff foo.bar
<P>diff abc def
</BLOCKQUOTE>

<BR><B>END</B>
<BR>
End the PDBDiff session.<p>

Usage:	end
<P>

<BR><B>FILE</B>
<BR>
Describe a PDB file. The first file is designated by 1, a, or first and the
second file is designated by 2, b, or second. <p>

Usage:	file [1 | a | first | 2 | b | second]
<P>Examples:

<BLOCKQUOTE>
file
<P>file second
</BLOCKQUOTE>


<BR><B>FORMAT</B>
<BR>
Set the printing format for a specified data type. If the argument specifying the
type has &#145;1&#146; or &#145;2&#146; appended, then the new format is applied
only to arrays shorter than array-length or to arrays longer than or equal to
array-length, respectively. Otherwise, the format applies to both. Invoking the
format command with the single argument, default, causes the formats for all types
to be reset to their default values. The format argument must be a Standard C I/O
Library format string. Double quotes are only necessary if the format string contains
embedded blanks. See the set command for more information about the array-length
display control parameter. This command overrides the settings of the
decimal-precision and bits-precision display control parameters.<p>

<TABLE>
<TR><TD>Usage:</TD>
<TD>format integer[1 | 2] | long[1 | 2] | float[1 | 2] |</TD></TR>
<TR><TD></TD>
<TD>double[1 | 2] | short[1 | 2] | char[1 | 2] format</TD></TR>
<TR><TD></TD></TR>
<TR><TD>Usage:</TD>
<TD>format default</TD></TR>
</TABLE>

<P>Examples:

<BLOCKQUOTE>
format double %12.5e
<P>format double2 %10.2e
<P>format char &#147;%s &#147;
<P>format default
</BLOCKQUOTE>

<BR><B>HELP</B>
<BR>
Print a list of the available commands or information about the specified command.<p>

Usage:	help [command]
<P>Examples:

<BLOCKQUOTE>
help
<P>help open
</BLOCKQUOTE>

<BR><B>LS</B>
<BR>
List the names of variables, links, and directories in the designated file. The
first file is designated by 1, a, or first and the second file is designated by
2, b, or second. If neither file is specified the information is listed for both
files. Directories have a terminal slash. An optional selection pattern may be
specified. Each &#145;?&#146; in the pattern matches any single character. Each
&#145;*&#146; in the pattern matches any string of zero or more characters. An
optional type qualifier may also be specified in order restrict the list to a
given type. A type qualifier of &#145;*&#146; may be used to match all types.
Both a selection pattern and a type qualifier must be specified if you wish to
designate a file.<p>

Usage:	ls [pattern [type [1 | a | first | 2 | b | second]]]
<P>Examples:

<BLOCKQUOTE>
ls dir1/curve*
<P>ls var? integer second
<P>ls * Directory
<P>ls * * first
</BLOCKQUOTE>

<BR><B>MODE</B>
<BR>
Set the print mode for arrays and structures.<p>

Display modes are:<p>

<BLOCKQUOTE>
<TABLE>
<TR><TD>full-path</TD><TD>- the full path name is printed at each branch, e.g. foo.bar[3].baz</TD></TR>
<TR><TD>indent</TD><TD>- indent 4 spaces at each branch (default)</TD></TR>
<TR><TD>tree</TD><TD>- display as a tree (lines connecting branches)</TD></TR>
</TABLE>
</BLOCKQUOTE>

Type display is controlled by:<p>

<BLOCKQUOTE>
no-type - turns off the display of types (default)
<P>type - displays the type of each item and branch
</BLOCKQUOTE>

Display of recursive structures is controlled by:<p>

<BLOCKQUOTE>
recursive - indent each level of recursive structures
<P>iterative - number each level of recursive structures (default)
</BLOCKQUOTE>

Display of differing elements is controlled by:<p>

<BLOCKQUOTE>
individual - display only differing elements (default)
<P>display-all - display all elements if even one differs
</BLOCKQUOTE>

<TABLE>
<TR><TD>Usage:</TD>
<TD>mode full-path | indent | tree | no-type | type |</TD></TR>
<TR><TD></TD>
<TD>recursive | iterative | individual | display-all </TD></TR>
</TABLE>

<BR>
Example:

<BLOCKQUOTE>
mode no-type
</BLOCKQUOTE>

<BR><B>OPEN </B>
<BR>
Open a new pair of files.<p>

Usage:	open filename1 filename2
<P>Example:

<BLOCKQUOTE>
open foo.s00 bar.s00
</BLOCKQUOTE>

<BR><B>PRINT</B> 
<BR>
Print out all or part of the specified variable or structure member from the
first or second file. The first file is designated by 1, a, or first and the
second file is designated by 2, b, or second. To print part of a variable or
structure member, qualify the name with index expressions
(see the <a href="#DIFFsyn">PDBDiff Syntax</a> section for a discussion of
index expressions).<p>

Usage:	print variable | structure-member [1 | a | first | 2 | b | second]
<P>Examples:

<BLOCKQUOTE>
Mapping2 first
<P>print Mapping2.range.elements[1] 2
<P>a[5,10:20,1:8:3]
<P>print a.b[3].c[5,10:20,1:8:3] b
</BLOCKQUOTE>

<BR><B>PROMOTE </B>
<BR>
Promote lower precision types in the specified class or classes to higher
precision to permit comparison, if necessary. Turn off promotion for classes
not specified. The default is no promotion.<p>

Usage:	promote [fixed] [float]
<P>Examples:

<BLOCKQUOTE>
promote
<P>promote fixed
<P>promote float
<P>promote fixed float
</BLOCKQUOTE>

<BR><B>PWD</B>
<BR>
Print the current file directory for the designated file.The first file is designated
by 1, a, or first and the second file is designated by 2, b, or second. If neither
file is specified the current file directory will be printed for both files. NOTE: If
the current file directory is the same for both files, it will only be printed once.<p>

Usage:	pwd [1 | a | first | 2 | b | second]
<P>Examples:

<BLOCKQUOTE>
pwd
<P>pwd first
</BLOCKQUOTE>

<BR><B>SET</B>
<BR>
Set the value of some display parameters. Parameters:<p>

<BLOCKQUOTE>
<TABLE>
<TR><TD><B>line-length</B></TD><TD>- number of array elements per line</TD></TR>
<TR><TD><B>array-length</B></TD><TD>- for arrays shorter than value, label each element individually</TD></TR>
<TR><TD><B>bits-precision</B></TD><TD>- number of mantissa bits compared in floating point numbers</TD></TR>
<TR><TD><B>decimal-precision</B></TD><TD>- limit on the fractional difference reported</TD></TR>
</TABLE>
</BLOCKQUOTE>

Usage:	set line-length | array-length | bits-precision | decimal-precision value
<P>Examples:

<BLOCKQUOTE>
set line-length 3
<P>set array-length 20
<P>set bits-precision 10
<P>set decimal-precision 1.0e-6
</BLOCKQUOTE>

<BR><B>STRUCT</B>
<BR>
Describe the named data type in the specified file. The first file is designated
by 1, a, or first and the second file is designated by 2, b, or second.<p>

Usage:	struct data-type [1 | a | first | 2 | b | second]
<P>Examples:

<BLOCKQUOTE>
struct double 1
<P>struct PM_mapping second
</BLOCKQUOTE>

<BR><B>TRANSCRIPT</B>
<BR>
Toggle a transcript of the PDBDiff session. The name of the transcript file may be
specified when the transcript is activated. A file named pdbdiff.trn will contain
the transcript of the session if no name is given.<p>

Usage:	transcript [filename]
<P>Examples:

<BLOCKQUOTE>
transcript
<P>transcript foo.bar
</BLOCKQUOTE>

<BR><B>TYPES</B>
<BR>
List the data types in the designated file.The first file is designated by 1, a, or
first and the second file is designated by 2, b, or second. If neither file is
specified the data types will be printed for both files.<p>

Usage:	types [1 | a | first | 2 | b | second]
<P>Examples:

<BLOCKQUOTE>
types
<P>types second
</BLOCKQUOTE>

<a name="DIFFdflt"></a>
<H1 ALIGN="CENTER">Default State</H1>

The initial state of PDBDiff is:<p>

<ul>
<li>mode indent
<li>mode no-type
<li>mode iterative
<li>mode individual
<li>set line-length 2
<li>set array-length 4
<li>set bits-precision 128 (i.e. use all mantissa bits in
floating point comparisons)
</ul>

<a name="DIFFdocs"></a>
<H1 ALIGN="CENTER"> Other PACT Documents</H1>

Interested readers may wish to refer to the other PACT documents which describe the
data structures and functionality underlying the more common PDB files upon which
PDBDiff operates. The PANACEA, PGS, and PDBLib manuals are of special interest to
people who wish to generate and view data files with PACT.<p>

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

</BLOCKQUOTE>

<br>
<hr>
For questions and comments, 
please contact the <A HREF="pactteam.html">PACT Development Team</A>.

<hr>
Last Updated: $MOD
