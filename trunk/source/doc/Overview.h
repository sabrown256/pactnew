TXT: PACT Overview
MOD: 03/13/2015

<blockquote>

<center>
<H1><font color="#ff0000">$TXT</font></H1>
</center>
<hr>

<a name="Cont"></a>
<TABLE>
<ul>
<TR><TD><li><A HREF="./Overview.html#Intro">Introduction</a></TD>
<TD></TD></TR>
<TR><TD><li><A HREF="./Overview.html#Score"> SCORE</a></TD>
<TD>a low level, environment balancing library</TD></TR>
<TR><TD><li><A HREF="./Overview.html#Pml"> PML</a></TD>
<TD>a math library</TD></TR>
<TR><TD><li><A HREF="./Overview.html#Pdb"> PDB</a></TD>
<TD>a portable binary database management library</TD></TR>
<TR><TD><li><A HREF="./Overview.html#Scheme"> SCHEME</a></TD>
<TD>an interpreter for the Scheme dialect of the LISP language</TD></TR>
<TR><TD><li><A HREF="./Overview.html#Pgs"> PGS</a></TD>
<TD>a graphics library</TD></TR>
<TR><TD><li><A HREF="./Overview.html#Panacea">  PANACEA</a></TD>
<TD>a simulation code development system</TD></TR>
<TR><TD><li><A HREF="./Overview.html#Ultra">ULTRA</a></TD>
<TD>a 1d data presentation, analysis, and manipulation tool</TD></TR>
<TR><TD><li><A HREF="./Overview.html#Sx">SX</a></TD>
<TD>Scheme with extensions (PDBView and PDBDiff are SX  programs)</TD></TR>
<TR><TD><li><A HREF="./Overview.html#Doc">Documentation</a></TD>
<TD>list of availiable documents</TD></TR>
</ul>
</TABLE>

<hr>

<H2 ALIGN="CENTER"><A NAME="Intro"> Introduction </a></H2>
<P>
In a computing landscape which has a plethora of different hardware architectures and 
supporting software systems ranging from compilers to operating systems, there is an 
obvious and strong need for a philosophy of software development that lends itself to the 
design and construction of portable code systems. The current efforts to standardize
software bear witness to this need.
<P>
PACT is an effort to implement a software development environment which is itself
portable and promotes the design and construction of portable applications. PACT does not 
include such important tools as editors and compilers. Well built tools of that kind are 
readily available across virtually all computer platforms. The areas that PACT addresses 
are at a higher level involving issues such as data portability, portable inter-process
communication, and graphics. These blocks of functionality have particular significance to the 
kind of code development done at LLNL. That is partly why the general computing
community has not supplied us with these tools already. This is another key feature of the
software development environments which we must recognize. The general computing 
community cannot and should not be expected to produce all of the tools which we 
require.
<P>
At the time that PACT was begun other groups at other institutions were also beginning 
efforts to address some of these issues. That is the reason that one can find some of the 
conceptual parts of PACT in the work done at some of these institutions. One of the 
strengths of PACT is the degree of integration of these which the component parts can 
achieve. This affords the application developer a relatively small and efficient set of tools. 
For this reason, the total capability of PACT is unmatched in other software systems.
In its current conception, PACT is a set of nine tools. Eight of these are implemented as 
libraries, and two of them provide both libraries and executable applications. PACT is 
entirely coded in C but was designed with a view toward support of other languages,
notably FORTRAN.
<P>
The design of PACT was and is functionally driven. The main idea is to identify generic 
blocks of functionality independent of considerations of language and specific applications.
When that has been done, the application program interface (API) for a particular 
block of functionality, e.g. inter-process communication, naturally emerges. With the API 
specified, the implementation proceeds fairly naturally. The most important concept in 
making this approach work is that of abstraction barriers. The API defines an abstraction 
barrier between the general purpose functionality and the applications built on top of it. 
The underlying implementation can be changed without necessitating changes to the 
applications using it. This goal has not always been achieved, but the fact is that it remains 
as a goal and PACT is always evolving toward that goal. Part of the success which PACT 
enjoys from the perspective of meeting its requirements is due to the use of abstraction 
barriers. For people interested in a marvelous exposition of these ideas, I highly
recommend Abelson and Sussman's book, Structure and Interpretation of Computer Programs 
which has been one of the core textbooks in MIT's Computer Science curriculum. All of 
PACT has been strongly influenced by that book.

<P>
<CENTER>
<TABLE>
<TR>
<TD WIDTH=100>SCORE</TD>
<TD>a low level, environment balancing library</TD>
</TR>
<TR>
<TD WIDTH=100>PML</TD>
<TD>a math library</TD>
</TR>
<TD WIDTH=100>PDB</TD>
<TD>a portable binary database management library</TD>
</TR>
<TR>
<TD WIDTH=100>SCHEME</TD>
<TD>an interpreter for the Scheme dialect of the LISP language</TD>
</TR>
<TR>
<TD WIDTH=100>PGS</TD>
<TD>a graphics library</TD>
</TR>
<TR>
<TD WIDTH=100>PANACEA</TD>
<TD>a simulation code development system</TD>
</TR>
<TR>
<TD WIDTH=100>ULTRA</TD>
<TD>a 1d data presentation, analysis, and manipulation tool</TD>
</TR>
<TR>
<TD WIDTH=100>SX</TD>
<TD>Scheme with extensions (PDBView and PDBDiff are SX programs)</TD>
</TR>
</TABLE>
</CENTER>


<P>
These tools facilitate the development of software which need portable support for graphics,
inter-process communication and networking, and portable binary data handling and 
storage. It is ideal for the kinds of software development which are done at academic, 
applied scientific, and engineering institutions.
The point cannot be overemphasized that these tools are part of a coherent approach to 
solving portability problems. They are modularized which allows application builders to 
use only the parts needed for particular projects. They cooperate together so that there is 
maximal code reuse in PACT with its attendant benefits (maintainability, shareability, and 
flexibility). They are very generic in that each module is constructed to address a specific 
block of functionality, e.g. graphics, and address it in a general and comprehensive way. 
<P>
<FONT SIZE="+1">
<CENTER>
<PRE>
ULTRA     SX
<BR><BR>
 PANACEA
<BR>
  PGS    SCHEME
<BR>
 PML       PDB
<BR>
 SCORE
</PRE>
</CENTER>
</FONT>

<BR>

As mentioned above, the PACT system is both modular and layered in design. In each 
layer the modules are independent of one another, but each layer can depend on any module
in the layers beneath it. This consideration has a strong influence on where certain 
functions reside. In general, if a function has no strong attachment to a functional group
ing (e.g. graphics), it goes into the lowest level layer to which all of the other parts of 
PACT that might be able to use that function have access. In spite of one's expectations, 
this doesn't mean that functions tend to gravitate to SCORE. There seems to be a real,
natural level for almost every function in PACT.
<P>
To avoid name conflicts and to aid application developers who use PACT, there are some 
naming conventions followed (although not fanatically so) in all of the coding of PACT. 
Most functions, external variables, and macros begin with either a two, upper case character
designator followed by an underscore or an underscore followed by the designator and 
an underscore. The main functions, variables, and macros of the API lack the leading 
underscore. Those functions, variables, and macros which are very useful but require 
greater understanding of the libraries or are simply not necessary to the main API have the 
leading underscore. Many software systems would not document this latter category of 
objects and protect the API with the freedom to change these around. I document these in 
PACT because they are too useful to brush off.
The list of designators for the PACT tools is:
<P>
<CENTER>
<TABLE>
<TR>
<TD WIDTH="80">Package</TD>
<TD>Designator</TD>
<TD>Example</TD>
</TR>
<TR>
<TD WIDTH="80">SCORE</TD>
<TD ALIGN="CENTER">SC</TD>
<TD>CSTRSAVE</TD>
</TR>
<TR>
<TR>
<TD WIDTH="80">PML</TD>
<TD ALIGN="CENTER">PM</TD>
<TD>PM_decompose</TD>
</TR>
<TR>
<TD WIDTH="80">PDB</TD>
<TD ALIGN="CENTER">PD</TD>
<TD>PD_read</TD>
</TR>
<TR>
<TD WIDTH="80">SCHEME</TD>
<TD ALIGN="CENTER">SS</TD>
<TD>SS_eval</TD>
</TR>
<TR>
<TD WIDTH="80">PGS</TD>
<TD ALIGN="CENTER">PG</TD>
<TD>PG_open_device</TD>
</TR>
<TR>
<TD WIDTH="80">PANACEA</TD>
<TD ALIGN="CENTER">PA</TD>
<TD>PA_simulate</TD>
</TR>
<TR>
<TD WIDTH="80">ULTRA</TD>
<TD ALIGN="CENTER">UL</TD>
<TD>UL_set_graphics_mode</TD>
</TR>
<TR>
<TD WIDTH="80">SX</TD>
<TD ALIGN="CENTER">SX</TD>
<TD>SX_draw_plot</TD>
</TR>
</TABLE>
</CENTER>

<BR>
<A HREF="./Overview.html#Cont"><SMALL>Contents</SMALL></A>

</BLOCKQUOTE>

<!--                                                           -->
<!--                                                           -->
<!--                                                           -->

<HR size=2 width="50%" NOSHADE>

<BLOCKQUOTE>
<H2 ALIGN="CENTER"><A NAME="Score"> SCORE: A Low Level Foundation </a></H2>
<P>
SCORE (System  CORE) has two main functions. The first and perhaps most important is 
to smooth over the differences between different C implementations and define the
parameters
which drive most of the conditional compilations in the rest of PACT. Secondly, it 
contains several groups of functionality that are used extensively throughout PACT.
Although C is highly standardized now, that has not always been the case. Roughly speaking
C compilers fall into three categories: ANSI standard; derivative of the Portable C 
Compiler (Kernighan and Ritchie); and the rest. PACT has been successfully ported to 
many ANSI and PCC systems. It has never been successfully ported to a system in the last 
category. The reason is mainly that the "standard" C library supplied with such
implementations
is so far from true ANSI or PCC standard that PACT would have to include its own 
version of the standard C library in order to work at all. Even with standardized compilers
life is not dead simple. The ANSI standard leaves several crucial points ambiguous as 
"implementation defined". Under these conditions one can find significant differences in 
going from one ANSI standard compiler to another.
<P>
SCORE's job is to include the requisite standard headers and ensure that certain key
standard
library functions exist and function correctly (there are bugs in the standard library 
functions supplied with some compilers) so that, to applications which include the 
SCORE header(s) and load with SCORE, all C implementations look the same. This is a 
tall order, but in practice once SCORE has been successfully compiled only the areas of 
graphics, IPC, and binary data handling require special consideration! This has more of an 
impact on some programmers than on others. Those who prefer to specify only the exact 
headers to be included in each source file will find SCORE and PACT unusual. At the 
expense of a slight increase in compile time, the most commonly used headers are always 
included. This is crucial to getting the C implementation independence.
Typically, the SCORE header scstd.h includes the following:
<P>

<P>ANSI    stdlib.h stddef.h stdarg.h float.h
<P>PCC      sys/types.h varargs.h malloc.h
<P>Both     limits.h stdio.h string.h math.h ctype.h signal.h setjmp.h time.h
<P>
The single header, scstd.h, smooths over most of the generic problems that arise because 
of implementation defined behavior in the host C implementation. The remainder of the 
PACT sources ultimately include scstd.h. This strategy has been extremely successful for 
PACT and applications which use PACT.
<P>
There are basically three other areas which SCORE functions address: memory management;
hash table management; and extended string handling.
<P>See Also:  SCORE User's Manual
<P>Dependent PACT Libraries: None

<BR>
<BR>
<A HREF="./Overview.html#Cont"><SMALL>Contents</SMALL></A>

</BLOCKQUOTE>

<!--                                                           -->
<!--                                                           -->
<!--                                                           -->
<!--                                                           -->

<HR size=2 width="50%" NOSHADE>

<BLOCKQUOTE>
<H2 ALIGN="CENTER"><A NAME="Pml"> PML: Portable Mathematics Library </a></H2>
<P>
There are a great many numerical math libraries that are widely available. The PACT math 
library is more aimed at providing some useful mathematical structures, functions which 
deal with them, some geometry routines, interpolation functions, and the odd function 
from the standard C library which doesn't work right on all platforms. It should be viewed 
as a supplement to some of the other available math libraries rather than a would be
competitor.
At a certain level, the portability issue does arise in that PML should work (and 
work the same) on all platforms while some math libraries are not so portable or available. 
The other parts of PACT which need a certain math routine cannot be held hostage to the 
availability of other libraries. These then are the basic considerations behind PML.
PML defines several structures to take advantage of C's capabilities in that area. The effect 
that this produces on applications which use these mechanisms is one of simplifying the 
passing of data among functions, clarifying interfaces, and preventing certain classes of 
bugs. The main structures which PML defines are: complex numbers, matrices; sets; and 
mappings.
<P>
The matrix structure encapsulates an array of data with up to 2 dimensions. The set
structure
describes a collection of data elements in terms of type, set dimension, dimension of 
the elements, and other information. The mapping structure describes the generalization of 
a function which is an association of elements in a domain set with elements in a range set. 
The mapping is a nested structure containing pointers to two set structures.
These all are designed to be as mathematically faithful as possible. That means that they 
represent the fundamental abstract objects after which they are named. The sets and
mappings
in particular have a surprising application in the area of visualization. They turn out 
to be a good medium of exchange between data production systems such a simulation 
codes, data storage and transmission systems, and data visualization systems.
PML includes several useful matrix solvers. Some of these use the matrix structure and 
some of the sparse solvers do not. At this point, the math functions in PML have been 
more driven by specific needs than any systematic attempt at completeness. Perhaps in the 
future PML will be extended in this direction or perhaps a portable shell over some other 
math libraries will evolve.

<P>
<P>See Also: PML User's Manual
<P>Dependent PACT Libraries: SCORE

<BR>
<BR>
<A HREF="./Overview.html#Cont"><SMALL>Contents</SMALL></A>

</BLOCKQUOTE>

<!--                                                           -->
<!--                                                           -->
<!--                                                           -->
<!--                                                           -->

<HR size=2 width="50%" NOSHADE>

<BLOCKQUOTE>
<H2 ALIGN="CENTER"><A NAME="Pdb"> PDB: Portable Binary Database Library </a></H2>
<P>
Binary data takes up about a factor of 3 less space than ASCII data and requires about a 
twentieth the time to read and write. Despite the obvious advantage of storing and
transmitting
binary data the majority of data is moved between dissimilar hardware/software 
platforms via ASCII format. The reason is that different hardware/software platforms use 
a variety of representations of binary data which are tuned to the hardware, operating
system, or compiler on the platform.
<P>
PDBLib is a library which addresses the issue of binary data portability. It also offers 
unique facilities for handling structured data including pointers. Almost all solutions to the 
data portability problem employ a hub and spoke conversion method (e.g. Sun's XDR) in 
which data is converted on output to a neutral format and converted to the host format on 
input. This is exactly what happens with ASCII data, that is ASCII is the neutral format. 
The conversion process is what takes most of the time difference between doing I/O with 
ASCII data and binary data. PDBLib's approach only does conversions when necessary. In 
that way an application which will use its own data files or will give them to a machine of 
the same architecture will not do any data conversions thus saving the majority of the time 
difference between ASCII and binary I/O.
<P>
PDBLib supports structured data with a mechanism similar to the C struct mechanism. 
Since data objects as diverse as FORTRAN common blocks and C structs are represent
able by this means, PDBLib's solution is a very general and flexible one. Furthermore, 
PDBLib supports the use of pointers, so that entire data trees can be read or written with a 
single PDBLib call. PDBLib is unique in this capability.
<P>
It is important to keep in mind that although there are many systems for handling portable 
binary data (e.g. HDF, netCDF, CDF, and FLUX) all of them have a particular set of 
requirements to address. PDBLib's requirement is to be as general and flexible as possible. 
PDBLib is also the only tool of its kind which is an integral part of so far reaching a set of 
tools. Its integration with the PACT tools is a significant part of its flexibility.
PDBLib is especially designed to be inherently able to work with other languages. It
concerns
itself with issues such as array index ordering, and call by reference/call by value 
argument passing.

<P>
<P>See Also: PDBLib User's Manual
<P>Dependent PACT Libraries: SCORE

<BR>
<BR>
<A HREF="./Overview.html#Cont"><SMALL>Contents</SMALL></A>

</BLOCKQUOTE>

<!--                                                           -->
<!--                                                           -->
<!--                                                           -->
<!--                                                           -->

<HR size=2 width="50%" NOSHADE>

<BLOCKQUOTE>
<H2 ALIGN="CENTER"><A NAME="Scheme"> SCHEME: An Interpreter for the Scheme Dialect of LISP </a></H2>
<P>
The LISP programming language is one of the most powerful computer languages ever 
developed. Somewhat ironically, it is also one of the simplest to implement, at least to a 
certain level. Many applications would benefit by being able to provide their users with 
the ability to extend the application via a macro or scripting mechanism. LISP turns out to 
be ideal for this purpose. The language is syntactically simple and can be molded to meet 
a variety of needs. It also offers more power than most applications care to provide to their 
users, but one would rather be in that position than in having an underpowered extension 
mechanism.
<P>
Scheme is a lexically scoped, properly tail recursive dialect of the LISP programming
language. The PACT implementation is described abstractly in Abelson and Sussman's book, 
Structure and Interpretation of Computer Programs. It features all of the "essential
procedures" described in the Revised, Revised, Revised Report on Scheme (Jonathan Rees and 
William Clinger, ed) which defines the standard for Scheme.
<P>
SCHEME is implemented as a library, however, a small driver delivers a stand alone 
Scheme interpreter. This is useful for people who want to learn the language as well as 
people who want to prototype algorithms (one of LISP's strong points).
<P>
The PACT implementation features a reference counting incremental garbage collector. 
This distributes the overhead of memory management throughout the running of Scheme 
code. It also tends to keep SCHEME from trying to grab the entire machine on which it is 
running which some garbage collection schemes will attempt to do.

<P>
<P>See Also: SX User's Manual
<P>Dependent PACT Libraries: PML, SCORE

<BR>
<BR>
<A HREF="./Overview.html#Cont"><SMALL>Contents</SMALL></A>

</BLOCKQUOTE>

<!--                                                           -->
<!--                                                           -->
<!--                                                           -->
<!--                                                           -->

<HR size=2 width="50%" NOSHADE>

<BLOCKQUOTE>
<H2 ALIGN="CENTER"><A NAME="Pgs"> PGS: Portable Graphics System  </a></H2>
<P>
One of the biggest headaches for portability is graphics. The objective evidence is that the 
field is immature. One cannot exactly say that there are no graphics standards. The real 
problem is that there are too many standards. Until such time that the world settles down, 
there will be the need for a tool like PGS.
PGS is an API that is independent of the underlying host graphics system. All of the 
graphics portability headaches are confined to PGS and applications which use PGS are 
completely portable. PGS currently sits on top of X Windows on UNIX platforms, 
PostScript, CGM, PNG, and JPEG.
<P>
PGS takes a least common denominator approach regarding what graphics functionality it 
supports. The goal is to run on the widest variety of machines. This lets out high level 
graphics capabilities such as real time 3D rotations which depend on specific hardware. 
On the other hand, any rendering capability that can be implemented with reasonable
efficiency
in software is fair game for PGS. This model will almost certainly change in time 
as both graphics hardware and software evolve and become ubiquitous across platforms.
PGS adopts a model in which graphics devices such as display windows and PostScript 
files are represented by a structure which contains all of their state information. Then PGS 
can manage an arbitrary number of devices simultaneously and any picture can be drawn 
to any device.
<P>
PGS also structures display surfaces with a viewport defined in normalized coordinates, an 
enclosing boundary where axes are drawn which is defined as a set of offsets from the 
viewport window, and a world coordinate system attached to the viewport. The enclosing 
boundary is useful for obtaining a standoff between rendered data such as line plots and 
the axes used to measure the rendering.
<P>
PGS supports both line and text drawing primitives, line and text attributes, and bit maps 
for handling images and other cell array data. Most functionality in PGS is either primitive 
operations such as moves and draws or at a very high level such as axis drawing and the 
rendering algorithms that it supports. These rendering algorithms have two interfaces: one 
for "raw" data; and one for PML type mappings. This gives a great deal of flexibility to the 
application developer.
<P>
PGS has the following rendering algorithms currently: 1D line plots; 2D contour plots; 2D 
vector plots; 2D image plots; 3D wire frame mesh plots (for 2D data sets); and Grotrian 
diagram plots.

<P>
<P>See Also: PGS User's Manual
<P>Dependent PACT Libraries: PML, SCORE


<BR>
<BR>
<A HREF="./Overview.html#Cont"><SMALL>Contents</SMALL></A>

</BLOCKQUOTE>

<!--                                                           -->
<!--                                                           -->
<!--                                                           -->
<!--                                                           -->

<HR size=2 width="50%" NOSHADE>

<BLOCKQUOTE>
<H2 ALIGN="CENTER"><A NAME="Panacea"> PANACEA: Portable Simulation Code Development Environment </a></H2>
<P>
An analysis of numerical simulation codes will demonstrate a great deal of commonality. 
The goal of PANACEA is to implement everything that is generic to "all" numerical
simulation
codes once and for all. In doing this a model of numerical simulation codes is necessary
as the context for defining a collection of services which defines the generic aspects 
of numerical simulation codes.
Some people like to refer to tools such as PANACEA as back-planes or shells. I think of 
PANACEA as an environment for code development. PANACEA is not so highly structured
that it must be taken in its entirety or left alone. The notions of shell and back-plane 
are suggestive of that higher degree of structure. PANACEA can be used as just a library 
of routines (up to a point).
<P>
PANACEA provides the following general categories of services: definition and control of 
a database of simulation variables and packages; access control to the database; support 
for initial value problem generation; source and initial value data management; output/plot 
request handling; simulation control; units and conversion handling; and error handling.
PANACEA draws heavily on PDB and PML to provide its services. This gives it tremendous
leverage in handling the database, state files, and output/graphics data files. This 
close coupling keeps the system small and compact while providing a fairly large range of 
services.

<P>
<P>See Also: PANACEA User's Manual
<P>Dependent PACT Libraries: PDB, PML, SCORE

<BR>
<BR>
<A HREF="./Overview.html#Cont"><SMALL>Contents</SMALL></A>

</BLOCKQUOTE>

<!--                                                           -->
<!--                                                           -->
<!--                                                           -->
<!--                                                           -->

<HR size=2 width="50%" NOSHADE>

<BLOCKQUOTE>
<H2 ALIGN="CENTER"><A NAME="Ultra"> ULTRA: 1D Data Presentation, Analysis, and Manipulation </a></H2>
<P>
ULTRA is a tool for the presentation, analysis, and manipulation of 1D data sets or 
curves (i.e. y=f(x)). It supports operations such as addition, subtraction, multiplication, 
and division of curves; applying functions to curves (e.g. sin, cos); Fourier transforms; and 
shifting and scaling of curves. The user can control the range and domain of the displayed 
data as well as such attributes as line color, style, and width.
<P>
Either ASCII or binary data can be written or read by ULTRA. ULTRA uses the 
SCHEME interpreter to provide users with a very powerful mechanism to configure and 
extend ULTRA to suit their needs. This also keeps the code small by not having to
compile in everybody's pet extension. Thus ULTRA is extremely flexible.

<P>
<P>See Also: ULTRA User's Manual
<P>Dependent PACT Libraries: SCHEME, PGS, PDB, PML, SCORE

<BR>
<BR>
<A HREF="./Overview.html#Cont"><SMALL>Contents</SMALL></A>

</BLOCKQUOTE>

<!--                                                           -->
<!--                                                           -->
<!--                                                           -->
<!--                                                           -->

<HR size=2 width="50%" NOSHADE>

<BLOCKQUOTE>
<H2 ALIGN="CENTER"><A NAME="Sx"> SX: Scheme with Extensions </a></H2>
<P>
SX is perhaps the ultimate PACT statement. It is simply SCHEME plus the other parts of 
PACT. A more precise way to describe it is as a dialect of LISP with extensions for PGS, 
PDB, PML, and PANACEA. What this yields is an interpretive language whose primitive 
procedures span the functionality of all of PACT.
Like the SCHEME implementation which it extends, SX provides both a library and a 
stand alone application. The stand alone interpreter is the engine behind applications such 
as PDBView and PDBDiff. The SX library is the heart of TRANSL a tool to translate data 
files from one database format to another. These examples demonstrate the extreme power 
of the PACT design. The modularization and layering make it possible to use the PACT 
components like building blocks.
<P>
In addition to the above mentioned functionality, SX contains functionality which is the 
generalization of that found in ULTRA. This means that as the development of SX proceeds,
an SX driven application will be able to perform arbitrary dimensional presentation,
analysis, and manipulation tasks.
PDBView is a tool to examine the contents of PDB files. This may be done either by printing
all or part of the entries in a PDB file or by displaying the data graphically. As such 
PDBView already performs the basic presentation tasks which the ULTRA successor 
will have.
<P>
PDBDiff is a tool that does a binary comparison of two PDB files. The user can specify the 
precision of floating point comparisons, but by default PDBDiff uses the lesser of the two 
files' precision in floating point comparisons. For example, comparing a CRAY generated 
file with 64 bit floats and a SUN generated file with 32 bit floats will be done to 32 bits not 
64. PDBDiff can be run in either a batch or interactive mode. In batch mode it can be 
thought of as very similar to the UNIX diff utility. In interactive mode, the user can 
browse the files and compare all or part of arbitrary file entries.

<P>
<P>See Also: SX User's Manual
<P>Dependent PACT Libraries: SCHEME, PANACEA, PGS, PDB, PML, SCORE

<BR>
<BR>
<A HREF="./Overview.html#Cont"><SMALL>Contents</SMALL></A>

</BLOCKQUOTE>

<!--                                                           -->
<!--                                                           -->
<!--                                                           -->
<!--                                                           -->

<HR size=2 width="50%" NOSHADE>

<BLOCKQUOTE>
<H2 ALIGN="CENTER"><A NAME="Doc">PACT Documentation </a></H2>
<P>
The individual components of PACT have their own documentation which is intended to 
give application developers and end users complete information about that component. It 
is important to remember that the parts of PACT are integrated so that they depend on one 
another.  Consequently, it is assumed that readers of any PACT component document may 
need to refer to the documents for any component on which the desired component 
depends.

<BLOCKQUOTE>
<a href=pact.html>PACT User&#146;s Guide</a><br><br>
<a href=score.html>SCORE User&#146;s Manual</a><br><br>
<a href=pml.html>PML User&#146;s Manual</a><br><br>
<a href=pdb.html>PDBLib User&#146;s Manual</a><br><br>
<a href=pgs.html>PGS User&#146;s Manual</a><br><br>
<a href=pan.html>PANACEA User&#146;s Manual</a><br><br>
<a href=ultra.html>ULTRA II User&#146;s Manual</a><br><br>
<a href=pdbdiff.html>PDBDiff User&#146;s Manual</a><br><br>
<a href=pdbview.html>PDBView User&#146;s Manual</a><br><br>
<a href=sx.html>SX User&#146;s Manual</a>
</BLOCKQUOTE>

<A HREF="./Overview.html#Cont"><SMALL>Contents</SMALL></A>

<hr>
For questions and comments, 
please contact the <A HREF="pactteam.html">PACT
Development Team</A>.

