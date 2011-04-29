TXT: SX User's Manual
MOD: 04/29/2011

<CENTER>
<P>
<H1><font color="#ff0000">$TXT</font></H1>
<H2>Scheme with eXtensions</H2>
<H4>Stewart A. Brown
<BR>Dennis Braddy</H4>
</CENTER>

<BLOCKQUOTE>

<HR>

<h2><a href="#SXscheme">The PACT SCHEME Language</a></h2>
<ul>
<li><a href="#SXintro">Introduction</a>
<li><a href="#SXlang">SCHEME Language Guide</a>
<ul>
<li><a href="#SXconcepts">General Concepts of Scheme</a>
<li><a href="#SXdoccon">Documentation Conventions</a>
<li><a href="#SXsynnot">Syntax and Notation</a>
<ul>
<li><a href="#SXwhite">Whitespace</a>
<li><a href="#SXcomm">Comments</a>
<li><a href="#SXnums">Numbers</a>
<li><a href="#SXstrings">Strings</a>
<li><a href="#SXid">Identifiers</a>
<li><a href="#SXspecsyn">Special Syntax</a>
<li><a href="#SXnaming">Naming Conventions</a>
<li><a href="#SXresid">Reserved Identifiers</a>
</ul>
<li><a href="#SXdata">Scheme Data Types</a>
<li><a href="#SXexp">Scheme Expressions</a>
<ul>
<li><a href="#SXlit">Literal Expressions</a>
<li><a href="#SXvardef">Variable Definition</a>
<li><a href="#SXvarref">Variable Reference</a>
<li><a href="#SXvarassign">Variable Assignment</a>
<li><a href="#SXconexp">Conditional Expressions</a>
<li><a href="#SXcalls">Procedure Calls</a>
<li><a href="#SXanon">Anonymous Procedures</a>
</ul>
<li><a href="#SXscproc">Scheme Procedures</a>
<li><a href="#SXscprog">Scheme Programs</a>
</ul>
<li><a href="#SXtutor">A PACT Scheme Tutorial</a>
<ul>
<li><a href="#SXparen">About Those Parentheses</a>
<li><a href="#SXlists">Lists and Conses</a>
<li><a href="#SXwriting">Writing Scheme Functions</a>
<li><a href="#SXrecurs">Recursion in Scheme Functions</a>
<li><a href="#SXlists2">Lists, Symbols, and the car, cdr, and cond Functions</a>
<li><a href="#SXmatrixex">A Matrix Example</a>
</ul>
</ul>

<h2><a href="#SXscrefs">The PACT SCHEME/SX Reference</a></h2>
<ul>
<li><a href="#SXscprocs">PACT Scheme Procedures</a>
<ul>
<li><a href="#SXcore">The Core Set</a>
<li><a href="#538834">The Large Set</a>
</ul>
<li><a href="#539001">The PACT Scheme Library</a>
<ul>
<li><a href="#539887">Constants</a>
<ul>
<li><a href="#540032">Object Type Constants</a>
<li><a href="#SXevalcon">Evaluation Type Constants</a>
<li><a href="#SXproccon">Procedure Type Constants</a>
</ul>
<li><a href="#539899">Macros</a>
<ul>
<li><a href="#540092">Object Accessors</a>
<li><a href="#540122">Number Accessors</a>
<li><a href="#540131">Input_port/output_port Accessors</a>
<li><a href="#540151">Cons Accessors</a>
<li><a href="#540162">Variable Accessors</a>
<li><a href="#540184">String Accessors</a>
<li><a href="#540191">Procedure Accessors</a>
<li><a href="#540144">Vector Accessors</a>
<li><a href="#540284">Character Accessor</a>
<li><a href="#540290">Process Accessors</a>
<li><a href="#540377">Predicates</a>
<li><a href="#540531">Memory Management</a>
<li><a href="#540587">Execution Control</a>
<li><a href="#540593">Stack Control</a>
</ul>
<li><a href="#539909">Variables</a>
<ul>
<li><a href="#539917">Strings</a>
<li><a href="#539903">Objects</a>
</ul>
<li><a href="#539883">Handlers</a>
<ul>
<li><a href="#539884">Function Handlers</a>
<li><a href="#SXvarhand">Variable Handlers</a>
</ul>
<li><a href="#539906">Functions</a>
<ul>
<li><a href="#539908">Memory Management</a>
<li><a href="#540206">List Processors</a>
<li><a href="#540269">Input/Output</a>
<li><a href="#540405">C/SCHEME Interface</a>
<li><a href="#540346">Top Level</a>
<li><a href="#540432">Error Handling</a>
<li><a href="#539900">Helpers</a>
<li><a href="#540411">Registration Routines</a>
</ul>
</ul>
<li><a href="#SSsynt">Syntax Modes in SCHEME</a>
<ul>
<li><a href="#SScmode">C Syntax Mode</a>
</ul>
<li><a href="#SXscext">SCHEME + EXTENSIONS</a>
<ul>
<li><a href="#539004">SX Procedures</a>
<ul>
<li><a href="#539005">Pure SX Functions</a>
<li><a href="#539012">PML Functions</a>
<li><a href="#539076">Math Functions on Mappings</a>
<li><a href="#539193">PDB Functions</a>
<li><a href="#539305">PGS Functions</a>
<li><a href="#539360">PANACEA Functions</a>
</ul>
<li><a href="#539458">Variables</a>
<ul>
<li><a href="#539460">PANACEA Variables</a>
</ul>
</ul>
<li><a href="#539412">The SX Library</a>
<ul>
<li><a href="#540438">Constants</a>
<ul>
<li><a href="#540441">Object Type Constants</a>
</ul>
<li><a href="#540462">Macros</a>
<ul>
<li><a href="#540489">PDB Related Accessors</a>
<li><a href="#540544">Predicates</a>
</ul>
<li><a href="#540582">Variables</a>
<ul>
<li><a href="#540583">Integers</a>
<li><a href="#540584">REALs</a>
<li><a href="#540558">Strings</a>
</ul>
<li><a href="#540589">Functions</a>
<ul>
<li><a href="#540596">Input/Output</a>
<li><a href="#540372">Top Level</a>
</ul>
</ul>
</ul>

<h2><a href="#SXinterfacing">Interfacing Compiled and Interpreted Code</a></h2>
<ul>
<li><a href="#539606">Some PACT SCHEME Implementation Details</a>
<ul>
<li><a href="#539790">Objects</a>
<li><a href="#539795">C Level Macros</a>
<li><a href="#539791">Evaluation Types</a>
<li><a href="#539792">SCHEME Function Protocol</a>
<li><a href="#539833">Function Handlers</a>
</ul>
<li><a href="#539788">Defining New Objects to the Interpreter</a>
<ul>
<li><a href="#539625">Array Example</a>
</ul>
<li><a href="#539607">Compiling and Loading</a>
<li><a href="#540569">Calling Interpreted Routines from Compiled Routines</a>
</ul>

<h2><a href="#SXdocs">Related Documents</a></h2>

<hr>

<a name="SXack"></a>
<h2 ALIGN="CENTER">Acknowledgements</h2>

<p>
A special acknowledgement is due to Lee Taylor who designed and implemented
the internal interface between the PACT SCHEME interpreter and PDBLib. That
code eventually became SX when the remainder of PACT was thrown into the pot.</p>
<br>

<a name="SXscheme"></a>
<h1 ALIGN="CENTER">The PACT SCHEME Language</h1>

This section introduces the reader to the PACT implementation of the SCHEME
programming language. It does this through a formal description of the language
(section 1.2) and a tutorial (section 1.3). For readers who are already familiar
with SCHEME in general, this section may be skimmed. Note should be taken,
however, of the differences between PACT SCHEME and other SCHEME
implementations.<p>

<a name="SXintro"></a>
<h2 ALIGN="CENTER"> Introduction</h2>

<H3 ALIGN="CENTER"> SCHEME </H3>

The LISP programming language is one of the most powerful computer languages
ever developed. Somewhat ironically, it is also one of the simplest to implement,
at least to a certain level. Many applications would benefit by being able to
provide their users with the ability to extend the application via a macro or
scripting mechanism. LISP turns out to be ideal for this purpose. The language
is syntactically simple and can be molded to meet a variety of needs. It also
offers more power than most applications care to provide to their users, but one
would rather be in that position than in having an underpowered extension
mechanism.<p>

Scheme is a lexically scoped, properly tail recursive dialect of the LISP
programming language. The PACT implementation is described abstractly in
Abelson and Sussman&#146;s book, <I>Structure and Interpretation of Computer
Programs</I>. It features all of the &#147;essential procedures&#148; described
in the <I>Revised<SUP><FONT SIZE="small">3</FONT></SUP> Report on Scheme</I>
(Jonathan Rees and William Clinger, ed)
which defines the standard for Scheme.<p>

PACT SCHEME is implemented as a library; however, a small driver delivers a
stand alone SCHEME interpreter. This is useful for people who want to learn
the language as well as people who want to prototype algorithms (one of
LISP&#146;s strong points).<p>

The PACT implementation features a reference counting incremental garbage
collector. This distributes the overhead of memory management throughout
the running of Scheme code. It also tends to keep Scheme from trying to grab
the entire machine on which it is running which some garbage collection
schemes will attempt to do.<p>

<H3 ALIGN="CENTER"> SX </H3>

SX is perhaps the ultimate PACT statement. It is simply Scheme plus the other
parts of PACT. A more precise way to describe it is as a dialect of LISP with
extensions for PGS, PDB, PDBX, PML, and PANACEA. What this yields is an
interpretive language whose primitive procedures span the functionality of all
of PACT.<p>

Like the PACT SCHEME implementation which it extends, SX provides both a library
and a stand alone application. The stand alone interpreter is the engine behind
applications such as PDBView and PDBDiff. These examples demonstrate the extreme
power of the PACT design. The modularization and layering make it possible to use
the PACT components like building blocks.<p>

In addition to the above mentioned functionality, SX contains functionality
which is the generalization of that found in ULTRA II. This means that as the
development of SX proceeds, an SX driven application will be able to perform
arbitrary dimensional presentation, analysis, and manipulation tasks.<p>

Because of the fundamental unity of these two PACT parts, they are documented
in a single manual. The first part will cover the &#147;standard&#148; Scheme
functionality and the second part will discuss the SX extensions.<p>

<a name="SXlang"></a>
<h2 ALIGN="CENTER"> SCHEME Language Guide</h2>

The PACT SCHEME implementation derives from two main sources. First,
Abelson and Sussman&#146;s book, <I>The Structure and Interpretation of
Computer Programs</I> gives an account of a basic Scheme interpreter.
To fill in the details and build a somewhat standard Scheme, I used the
<I>Revised<SUP><FONT SIZE="small">3</FONT></SUP> Report on the Algorithmic
Language Scheme</I>. This has
subsequently been updated with some of the features of the
<I>Revised<SUP><FONT SIZE="small">4</FONT></SUP> Report on the Algorithmic
Language Scheme</I> (R4RS).<p>

This implementation differs from standard Scheme in a few fundamental ways.
First, the numerics defined in R4RS are not suited to the main audience of PACT.
For example, the notions of exactness and inexactness of floating point numbers
are not practical concepts in actual numerical simulation codes. Furthermore,
for ease of implementation and good matching to the needs of applications using
the library version of the interpreter, the numeric types implemented map to the
standard types of the C language. In short, I implemented integers, reals, and
characters as C longs, doubles, and chars respectively. Second, keeping in mind
that this interpreter is implemented in C and needs to be as compatible with C
usages as possible, I chose to follow C conventions in situations where it was
the convenient and &#147;obvious&#148; thing to do.<p>

Throughout this manual little will be said of the differences between Scheme
and LISP in general. The reader may assume that unless otherwise explicitly
stated comments about the character of Scheme apply also to LISP in general.
The functions in Scheme differ in many cases from those in LISP but the
character of the languages is similar. For example, they are both fully
parenthesized and they both use prefix notation.<p>

<a name="SXconcepts"></a>
<h3>  General Concepts of Scheme</h3>

The term object is used to refer to an instance of any type of Scheme variable,
data, or expression. This usage is similar to that used in connection with other
programming languages. One of the distinguishing characteristics of Scheme is
that every object in Scheme has a definite (although sometimes only implicit)
type which can be queried. For example, an object, x, can be asked if it is a
real number<p>

<BLOCKQUOTE>
(real? x)
</BLOCKQUOTE>

All objects in Scheme have an internal representation and printing representation.
That is to say that every object that is represented internally can be indicated
in printed form in output operations. Most objects can be indicated in input
operations by their printing representation. For example, the string &#147;foo
bar baz&#148; has some internal representation in Scheme and has a printing
representation that can be used in input or appears in output as<p>

<BLOCKQUOTE>
&#147;foo bar baz&#148;
</BLOCKQUOTE>

By contrast, an object such as a hash table has an internal representation and
a printing representation, but the printing representation cannot be used in
input operations to indicate that a complete hash table should be inferred
from the printing representation. The printing representation might look like<p>

<BLOCKQUOTE>
&lt;HASH_ARRAY|521&gt;
</BLOCKQUOTE>

This clearly does not contain enough information to infer the entire contents of
the hash table whose internal representation it is associated with.<p>

The most important data structure in LISP is the list (LISP stands for LISt
Processing language or, if you choose to believe, Lots of Irritating Silly
Parentheses). This is also true of Scheme. Both programs and data are represented
by lists. This feature of the language permits Scheme programs to manipulate
Scheme programs as data. This is the source of a great deal of
Scheme&#146;s power.<p>

Scheme is a tail recursive dialect of LISP. This means that certain recursively
posed procedures execute as simple loops. This permits the Scheme programmer to
write procedures using recursion and the interpreter (or compiler) to run these
procedures in fixed space. The function call stack is never blown with tail
recursive procedures.<p>

Variables in Scheme are said to be bound when they are assigned to locations in
which a value may be stored. Variables in Scheme may be either bound, in which
case references to the variable yield a value, or unbound, in which case
references to the variable yield an UNBOUND VARIABLE error.<p>

The collection of all variables visible at any point in the execution of a
program is called the environment in effect at that point. When interacting
directly with the Scheme interpreter at the top level (no procedures executing)
the environment in effect is called the global environment.<p>

Scheme like C is a call by value language.<p>

Scheme has a class of procedures called predicates. Predicate procedures return
one of the two boolean values: TRUE or FALSE (represented by #t or #f
respectively). Most languages have syntactic constructs to express this
functionality. In Scheme which has a simple and uniform syntax (except for
quote and quasi-quote) this functionality must be expressed as procedures.<p>

<a name="SXdoccon"></a>
<h3> Documentation Conventions</h3>

In this manual the following notational conventions will apply.<p>

<ul>
<li>=&gt;        when used in connection with a Scheme expression means &#147;evaluates
to&#148;. This will be used to show what the interpreter returns when it evaluates
an expression.

<li>the terms expression and form will be used interchangeably.

</ul>

<a name="SXsynnot"></a>
<h3> Syntax and Notation</h3>

It is a characteristic of LISP dialects to use prefix notation, that is
operators precede their operands. LISP and its dialects also use parentheses
to group and associate objects. Parentheses are the only syntactic mechanisms
for separating groups of objects. So to add four numbers, 3, 25.2, 0.015,
and -1.3e-4, one would use the expression

<BLOCKQUOTE>
(+ 3 25.2 0.015 -1.3e-4)
</BLOCKQUOTE>

Part of the power of the language derives from the fact that such expressions
return values that can be used in other expressions, for example

<BLOCKQUOTE>
(* 5 (+ 3 25.2 0.015 -1.3e-4))
</BLOCKQUOTE>

which multiplies the result of adding 3, 25.2, 0.015, and -1.3e-4 by 5.<p>

<a name="SXwhite"></a>
<h4>  Whitespace</h4>


The characters (expressed in C style notation) &#145; &#146;
(blank),&#146;\t&#146; (tab), &#145;\f&#146; (formfeed),
and &#145;\n&#146; (newline) are considered to be whitespace
in Scheme programs.<p>

<a name="SXcomm"></a>
<h4>  Comments</h4>

A semicolon begins a comment, and the comment extends to the end of the line
containing the semicolon.<p>

<a name="SXnums"></a>
<h4> Numbers</h4>

Numbers begin with any of the following characters: &#145;0&#146;,
&#145;1&#146;, &#145;2&#146;, &#145;3&#146;, &#145;4&#146;,
&#145;5&#146;, &#145;6&#146;, &#145;7&#146;, &#145;8&#146;,
&#145;9&#146;, &#145;.&#146;, or &#145;-&#146;. They must
correspond to the standard representations of either integer
or floating point types used in C (and most FORTRANs).<p>

<a name="SXstrings"></a>
<h4> Strings</h4>

Strings are arbitrary groups of characters which are delimited with
double quotes &#145;&#148;&#146;.<p>

<a name="SXid"></a>
<h4>  Identifiers</h4>

Subject to the special case listed below, all groups of characters delimited
by whitespace which are neither numbers nor strings are identifiers.
Case is significant in PACT Scheme unlike R4RS.<p>

<a name="SXspecsyn"></a>
<h4>  Special Syntax</h4>

The following characters have special meanings in some contexts. They should
not be used in identifiers unless escaped by &#145;\&#146;.<p>

<BLOCKQUOTE>
<TABLE>
<TR><TD WIDTH="20">.</TD>
<TD>used in the notation of pairs and dotted argument lists</TD></TR>
<TR><TD>(</TD>
<TD>begins a list of objects</TD></TR>
<TR><TD>)</TD>
<TD>ends a list of objects</TD></TR>
<TR><TD>&#146;</TD>
<TD>quote (don&#146;t evaluate) an object</TD></TR>
<TR><TD>&#145;</TD>
<TD>quasi-quote an object</TD></TR>
<TR><TD>,</TD>
<TD>used in conjunction with quasi-quotation</TD></TR>
<TR><TD>,@</TD>
<TD>used in conjunction with quasi-quotation</TD></TR>
<TR><TD>#</TD>
<TD>used with character constants, boolean constants, and vector constants</TD></TR>
<TR><TD>\</TD>
<TD>used with character constants and in strings as an escape character</TD></TR>
</TABLE>
</BLOCKQUOTE>
<P>

<a name="SXnaming"></a>
<h4>  Naming Conventions</h4>

Predicate functions (returning true or false) end with &#145;?&#146;.<p>

Functions which store values end with &#145;!&#146;.<p>

Functions which map objects of different types have &#147;-&gt;&#148;
in their names. For example, the function which maps a list into a vector
is called list-&gt;vector.<p>

<a name="SXresid"></a>
<h4>  Reserved Identifiers</h4>

Identifiers typically occur as the names of variables (or symbols) or functions
(or procedures). PACT Scheme allows programmers to use identifiers that have
been assigned to the built in procedures of the language for their own
variables and functions, but this has the effect of hiding (possibly for the
duration of the session or program) the built in procedure. It is recommended
that this not be done in the case of the following built in functions.
They can be thought of as reserved keywords.<p>

<BLOCKQUOTE>          
<TABLE>
<TR><TD WIDTH="100">and	</TD><TD WIDTH="100">if	</TD><TD>quasiquote</TD></TR>
<TR><TD>begin	</TD><TD>lambda	</TD><TD>quote
<TR><TD>define	</TD><TD>let	</TD><TD>set!
<TR><TD>do	</TD><TD>let*	</TD><TD>unquote
<TR><TD>else	</TD><TD>or	</TD><TD>unquote-splicing
</TABLE>
</BLOCKQUOTE>

<a name="SXdata"></a>
<h3> Scheme Data Types</h3>

The fundamental data types of PACT Scheme are:<p>

<BLOCKQUOTE>
<TABLE>

<TR><TD><B>boolean</B></TD>
<TD>there are two boolean objects in Scheme, #t and #f, representing 
a TRUE and FALSE logical value respectively</TD></TR>
<TR><TD><B>char</B></TD>
<TD>a character object is a single character and is represented by its 
printing representation preceded by a #\, e.g. #\A represents &#145;A&#146;</TD></TR>
<TR><TD><B>number</B></TD>
<TD>a number is any object with a numeric type such as integer or real</TD></TR>
<TR><TD><B>pair</B></TD>
<TD>an object with two parts called the car and cdr. This is the primitive
object from which lists are built</TD></TR>
<TR><TD><B>string</B></TD>
<TD>a collection of characters (not character objects) delimited by 
double quotes.</TD></TR>
<TR><TD><B>symbol</B></TD>
<TD>a synonym for variable</TD></TR>
<TR><TD><B>procedure</B></TD>
<TD>an object which specifies a procedure</TD></TR>
<TR><TD><B>vector</B></TD>
<TD>a 1 dimensional array of objects</TD></TR>
<TR><TD><B>guest</B></TD>
<TD>an object that encapsulates C variables and structures as a part 
of the interface between PACT Scheme and C applications</TD></TR>

</TABLE>
</BLOCKQUOTE>

Each of these types has a corresponding predicate function. It is guaranteed
that no Scheme object will return TRUE for more than one of the associated
predicates.<p>

In addition the number type is subdivided into<p>

<BLOCKQUOTE>
<TABLE>

<TR><TD><B>integer</B>
<TD>an object representing an integer number</TD></TR>
<TR><TD><B>real</B>
<TD>an object representing a floating point number</TD></TR>

</TABLE>
</BLOCKQUOTE>

PACT Scheme does not implement <p>

<B>
<BLOCKQUOTE>
rational
<P>complex
</BLOCKQUOTE>
</B>

The reason for this is that in order to keep the implementation reasonably
small and efficient, I chose to adhere closely to underlying C concepts
and data types. Both of these types could be implemented as C structures,
but handling the arithmetic with them is not consistent with the goals of
the implementation.<p>

Predicates exist for the integer and real types, but any object that is
either an integer or a real is also a number.<p>

<a name="SXexp"></a>
<h3>  Scheme Expressions</h3>

An expression in Scheme is an object which when evaluated returns a value.
Scheme has a small set of primitive expression types and all other
expressions in Scheme can be built up or derived from more primitive
expressions. The primitive expression types are enumerated in this section.<p>

This is the heart of Scheme, and although this is not by any means a tutorial,
once the concept of expression is mastered along with the primitive expression
types, the remainder of the work of this manual is filling in details on the
set of procedures supplied in PACT Scheme.<p>

<a name="SXlit"></a>
<h4>  Literal Expressions</h4>

An expression consisting of a constant or a quoted expression is a literal
expression. Literal expressions evaluate to themselves. That is a string
evaluates to itself, a number evaluates to itself, and so on.<p>

Another kind of literal expression comes about by specifying that an
expression should not be evaluated. This is a quoted expression. Its syntax
is:<p>

<BLOCKQUOTE>
(quote expression)
</BLOCKQUOTE>
or
<BLOCKQUOTE>
&#145;expression
</BLOCKQUOTE>

<a name="SXvardef"></a>
<h4> Variable Definition</h4>

Variables are unbound until they are defined in a variable definition
expression. The syntax is:<p>
<BLOCKQUOTE>
(define variable value)
</BLOCKQUOTE>
This expression binds variable to a location where the value is then placed.<p>

<a name="SXvarref"></a>
<h4> Variable Reference</h4>

An expression representing a reference to a variable consists of the
variable itself.<p>
<BLOCKQUOTE>
variable
</BLOCKQUOTE>
It is an error to reference an unbound variable, i.e. a variable which
has not be been bound in a variable definition expression.<p>

<a name="SXvarassign"></a>
<h4>  Variable Assignment</h4>

The value in the location to which a variable is bound can be changed
by the use of a variable assignment expression.<p>
<BLOCKQUOTE>
(set! variable value)
</BLOCKQUOTE>
It is an error to try to assign a value to an unbound variable.<p>

<a name="SXconexp"></a>
<h4> Conditional Expressions</h4>

In Scheme there is a single primitive conditional expression form from
which all others may be derived.<p>
<BLOCKQUOTE>
(if test consequent alternate)
</BLOCKQUOTE>
If test is not FALSE (evaluates to anything but #f) the consequent expression
is evaluated and returned. Otherwise the alternate expression is evaluated
and returned. The alternate expression is optional in which case if test is
FALSE nothing is evaluated and the conditional expression returns #f.<p>

<a name="SXcalls"></a>
<h4> Procedure Calls</h4>

Since procedures are a fundamental &#147;data&#148; type in Scheme, the
problem of procedure calls splits into two parts. The first part is
obtaining a procedure. The next section explores that issue in more detail.
The second part is invoking a procedure once you have one. A procedure call
is posed as a list consisting of the procedure object as the first element
of the list and the arguments to the procedure as the remaining elements of
the list.<p>

<BLOCKQUOTE>
(procedure argument1 ...)
</BLOCKQUOTE>
Some examples of procedure calls are:<p>
<BLOCKQUOTE>
(* 2 3 4) =&gt; 24
<P>((if #t + -) 20 10 5) =&gt; 5
</BLOCKQUOTE>
In the first example the procedure object is &#147;*&#148; and the procedure
call expression multiplies the numbers 2, 3, and 4 to obtain the value 24.
In the second example, the conditional expression<p>
<BLOCKQUOTE>
(if #f + -) =&gt; -
</BLOCKQUOTE>
is evaluated to return the procedure &#147;-&#148; which is then applied to
the numbers 20, 10, and 5 to yield the result 5.<p>

<a name="SXanon"></a>
<h4>  Anonymous Procedures</h4>

A characteristic of LISP which Scheme shares is the notion of an anonymous
procedure. An expression called a lambda expression evaluates to a procedure.
A lambda expression is the irreducibly primitive procedure definition
mechanism for Scheme. All procedures and most of the special expressions
except those mentioned in this section can be defined in terms of lambda
expressions.<p>

The syntax of a lambda expression is<p>
<BLOCKQUOTE>
(lambda arguments body)
</BLOCKQUOTE>
The arguments are a list of formal parameters to the procedure, and the
body is the set of expressions which define the action of the procedure.<p>

The arguments can be one of the following:<p>
<BLOCKQUOTE>
variable
<P>(variable1 ...)
<P>(variable1 ... variablen . variablen+1)
</BLOCKQUOTE>
In the last case variablen+1 is bound to a list of the arguments in the
procedure call which remain after the first n arguments are bound to the
first n formal parameters.<p>

Some examples of lambda expressions in procedure calls are:<p>
<BLOCKQUOTE>
((lambda x x) 1 &#147;foo&#148; 2.3) =&gt; (1 &#147;foo&#148; 2 3)
<P>((lambda (x) (+ 1 x)) 4) =&gt; 5
<P>((lambda (x y . z) z) 3 4 5 6) =&gt; (5 6)
</BLOCKQUOTE>
An anonymous procedure can be bound to a variable in order to give a more
convenient way of using procedure objects.<p>
<BLOCKQUOTE>
(define foo (lambda x x))
</BLOCKQUOTE>
This would bind the variable foo to the lambda expression for the identity
function. It can then be used as<p>
<BLOCKQUOTE>
(foo 3) =&gt; 3
</BLOCKQUOTE>
This usage is so common that an alternate form of the define expression
was created. Its usage is<p>
<BLOCKQUOTE>
(define (variable arguments) body)
</BLOCKQUOTE>
Here the variable is bound to the procedure being made. The arguments are
like those of the lambda expression. <p>
<BLOCKQUOTE>
none
<P>variable1 ...
<P>variable1 ... variablen . variablen+1
</BLOCKQUOTE>
Note the absence of parentheses here. Compare with the lambda expression
syntax. The first variant means that there are no arguments.<p>

The body of the procedure is identical with that for the lambda expression.<p>
<BLOCKQUOTE>
(define (foo x) x)
</BLOCKQUOTE>

<a name="SXscproc"></a>
<h3> Scheme Procedures</h3>

In most LISP dialects there are two kinds of procedures: those which evaluate
their arguments before binding them to the formal parameters; and those which
do not evaluate their arguments before binding them to the formal parameters.
There are also procedures which carry out an additional evaluation of the
result of the procedure call.<p>

PACT SCHEME permits all combinations of evaluating arguments and evaluating
results. This makes for a great deal of flexibility in the implementation
and in the use of the library in applications. More will be said about this
later in this manual.<p>

The majority of Scheme functions evaluate their arguments and do not evaluate
the result of the call. These will be referred to simply as procedures. All
other functions will be referred to as macros (R4RS calls them
special forms).<p>

<a name="SXscprog"></a>
<h3> Scheme Programs</h3>

The conceptual structure of a Scheme program differs from that of a C program
primarily in the entry into the program. Both will have a collection of
function or procedure definitions, and both have a main entry point into
the program.<p>

In the case of C there is a special procedure called main which serves as
the main entry point into the program. Arguments from the command line
invocation of the C program are made available to main. The main procedure
may call no other functions and simply do the work of the program itself or
it may do nothing but call other functions.<p>

In Scheme, their is no special function like main in C. A file is read in
and variables and functions are defined sequentially. Functions and
variables must be defined before they are invoked or referenced.
Consequently the analog of the main entry point into a Scheme program
must come at the end of a file or function definition, and it is just
an ordinary procedure call.<p>

<B>Example C code fragment illustrating structure of a C program
</B>
<HR>
<BLOCKQUOTE>

<PRE>

main()
   {foo(1);
    bar();
    exit(0);
foo(int x)
   {...
bar()
   {...

</PRE>
</BLOCKQUOTE>

<B>Example Scheme code fragment illustrating the structure of the same program
</B>
<HR>
<P>
<BLOCKQUOTE>
(define (foo x) ...)
<P>(define (bar) ...)
<P>(foo 1)
<P>(bar)
</BLOCKQUOTE>

<p>

NOTE: a common complaint about LISP programming is that one has to count
parentheses carefully which is a lot of bother. The answer to that problem
is to use a text editor which indicates balancing parentheses. One common
editor with this feature is GNU Emacs.<p>

<a name="SXtutor"></a>
<h2 ALIGN="CENTER"> A PACT Scheme Tutorial</h2>

<a name="SXparen"></a>
<h3> About Those Parentheses</h3>

Scheme evaluates expressions: <p>

<BLOCKQUOTE>
<TABLE>
<TR><TD>=&gt;</TD><TD>3</TD></TR>
<TR><TD>3</TD></TR>
<TR><TD>=&gt;</TD><TD>(+ 9 2)</TD></TR>
<TR><TD>11</TD></TR>
<TR><TD>=&gt;</TD><TD>; Comments can be added after a semicolon.</TD></TR>
<TR><TD></TD><TD>; Note that =&gt;  is the SX prompt.</TD></TR>
<TR><TD></TD><TD>; For clarity, responses from SX will be outlined.</TD></TR>
<TR><TD></TD><TD>(+ 9 2)   ; Scheme uses Polish notation.</TD></TR>
<TR><TD>11</TD></TR>
</TABLE>
</BLOCKQUOTE>

Scheme evaluates expressions from left to right and from inside nested
pairs of parentheses out:<p>

<BLOCKQUOTE>
=&gt; (- (+ 4 2) 7)
<P> -1
</BLOCKQUOTE>

The first argument (+ 4 2) is evaluated to 6, and the second argument 7
is evaluated to 7.  Then (- 6 7) is evaluated to -1.<p>

A little more integer arithmetic will show how the expression  
(9 + 2) (7 - 4)/3 might be written in Scheme:<p>

<BLOCKQUOTE>
<TABLE>
<TR><TD>=&gt; (+ 9 2)</TD>
<TD>; i.e., 9 + 2</TD></TR>
<TR><TD>11</TD></TR>
<TR><TD>=&gt; (- 7 4)</TD>
<TD>; i.e., 7 - 4</TD></TR>
<TR><TD>3</TD></TR>
<TR><TD>=&gt; (* (+ 9 2) (- 7 4))</TD>
<TD>; i.e., (9 + 2) (7 - 4)</TD></TR>
<TR><TD>33</TD></TR>
<TR><TD>=&gt; (/ (* (+ 9 2) (- 7 4)) 3)</TD>
<TD>; Q.E.D., (9 + 2) (7 - 4)/3</TD></TR>
<TR><TD>11</TD></TR>
</TABLE>
</BLOCKQUOTE>

Thus,   (9 + 2) (7 - 4)/3   is evaluated in three steps: <p>

<BLOCKQUOTE>
<TABLE>
<TR><TD>(* (+ 9 2) (- 7 4)) 3)   </TD><TD>-&gt;  (/ (* 11 3) 3)</TD></TR>
<TR><TD>(/ (* 11 3) 3)           </TD><TD>-&gt;  (/ 33 3)</TD></TR>
<TR><TD>(/ 33 3)                 </TD><TD>-&gt;  11</TD></TR>
</TABLE>
</BLOCKQUOTE>

<a name="SXlists"></a>
<h3> Lists and Conses</h3>

One of the fundamental and powerful data structures of Scheme is a pair
of pointers referred to variously as a cons, cell, or pair. To talk about
Scheme you have to talk about conses. The printed representation of cons
containing a and b is<p>

<BLOCKQUOTE>
(a . b)
</BLOCKQUOTE>

The first part of the pair is called the car of the cons and the second part
is called the cdr of the cons.<p>

Lists consist of chains of conses. For example:<p>

<BLOCKQUOTE>
(a . (b . (c . ())))
</BLOCKQUOTE>

This is really rather tedious, so a more compact notation is used to
represent lists<p>

<BLOCKQUOTE>
(a . (b . c . ()))) &lt;==&gt; (a b c)
</BLOCKQUOTE>

The rule is that if the cdr of a cons is another cons the &#147;.
(&#147; is suppressed. This also means that you will see things like:<p>

<BLOCKQUOTE>
(a b c . d)
</BLOCKQUOTE>

which is equivalent to<p>

<BLOCKQUOTE>
(a . (b . (c . d)))
</BLOCKQUOTE>

Compare this with the above to see another rule about lists and conses.
If the final conses cdr is (), then suppress the &#147; . ()&#148;. 
For example:<p>

<BLOCKQUOTE>
(a . ()) &lt;==&gt; (a)
</BLOCKQUOTE>

The Scheme function cons is used to create a new cons:<p>

<BLOCKQUOTE>
=&gt; (cons 1 2)
<P>(1 . 2)
</BLOCKQUOTE>

You can always use cons to build up lists:<p>

<BLOCKQUOTE>
=&gt; (cons 1 (cons 2 (cons 3 ())))
<P>(1 2 3)
</BLOCKQUOTE>

However there is a better way. Use the list function:<p>

<BLOCKQUOTE>
=&gt; (list 1 2 3)
<P>(1 2 3)
</BLOCKQUOTE>


<a name="SXwriting"></a>
<h3> Writing Scheme Functions</h3>

Scheme provides predefined primitive functions such as sqrt, equal?, and cons:<p>

<BLOCKQUOTE>
 =&gt; (sqrt 25)
<P> 5.0
<P> =&gt; (equal? 3 5)
<P> #f
<P> =&gt; (equal? (* 3 6) (+ 9 9))
<P> #t
<P> =&gt; (cons &#145;fish &#145;())
<P> (fish)
<P> =&gt; (cons &#145;fish1 (cons &#145;fish2 (cons &#145;fish3 &#145;(fish4))))
<P> (fish1 fish2 fish3 fish4)
</BLOCKQUOTE>

The function  equal? is one of a number of Boolean functions that return a
true-or-false Boolean object:  either #t or #f.  The input of a decision-making
function like  cond (see below) is often the output of a Boolean function: <p>

<BLOCKQUOTE>
<TABLE>
<TR><TD>=&gt; (cond</TD>
<TD>((equal? 2 2) &#147;The numbers are equal.&#148;)</TD></TR>
<TR><TD></TD>
<TD>(else &#147;The numbers are not equal.&#148;))</TD></TR>
<TR><TD COLSPAN="2">&#147;The numbers are equal.&#148;</TD></TR>
<TR><TD>=&gt; (cond</TD>
<TD>((equal? 2 3) &#147;The numbers are equal.&#148;)</TD></TR>
<TR><TD></TD>
<TD>(else &#147;The numbers are not equal.&#148;))</TD></TR>
<TR><TD COLSPAN="2">&#147;The numbers are not equal.&#148;</TD></TR>
</TABLE>
</BLOCKQUOTE>

In the two examples above, if  equal? evaluates to #t, the first message is
printed.  Otherwise, the second message is printed. <p>

Scheme programming consists of writing your own functions, e.g.: <p>

<BLOCKQUOTE>
 =&gt; (define (net-vol x2 y2 z2 x1 y1 z1)
<BLOCKQUOTE>
(- (* x2 y2 z2) (* x1 y1 z1)))
</BLOCKQUOTE>
 net-vol
</BLOCKQUOTE>

The name of the function (net-vol) was declared in the define statement, and
the arguments (six, in this example) were listed.   Then the algorithm was
stated, giving the locations of the arguments in the algorithm.  SX returned
the name of the function to confirm that the definition has been accepted. <p>

After  net-vol has been defined, it is called with its arguments like any
other function: <p>

<BLOCKQUOTE>
=&gt; (net-vol 6 9 27 4 3 1) 
<P>1446 
</BLOCKQUOTE>

Once a function has been defined, it can be included in definitions of other functions.
If we defined function  square as <p>

<BLOCKQUOTE>
=&gt; (define (square n) 
<BLOCKQUOTE>
(* n n)) 
</BLOCKQUOTE>
square
</BLOCKQUOTE>

we can define function  lim-test as: <p>

<PRE>
 =&gt; (define (lim-test lim n)

        (&lt;= lim (square n)))    ; Second argument less than

                                ; or equal to first argument? 
 lim-test

 
 =&gt; (lim-test 9 100)

 #f

 =&gt; (lim-test 11 100)

 #t

</PRE>

<a name="SXrecurs"></a>
<h3>  Recursion in Scheme Functions</h3>

A Scheme function becomes recursive if it calls itself within its own definition.
It takes the general form: <p>

<BLOCKQUOTE>
(define  (name ...)
<BLOCKQUOTE>
...
</BLOCKQUOTE>
( name ...))
</BLOCKQUOTE>


A conditional statement may be placed in the definition before the function
calls itself, in order to eventually escape from the loop.  Here is a function
that uses the  if statement, which takes the logical form: <p>

<BLOCKQUOTE>
<PRE>
(if (condition)  true false) 

=> (define (query n) 
      (if (zero? n) 
        "The number is zero." 
        "The number is not zero.")) 
query 
=> (query 0) 
"The number is zero." 
=> (query 5) 
"The number is not zero." 
</PRE>
</BLOCKQUOTE>

Now we&#146;ll define  <B>n-fact</B>, a function that uses recursion to
calculate  n!: <p>

<BLOCKQUOTE>
<PRE>
=> (define (n-fact n)
       (if (= 0 n)
           1
           (* n (n-fact (- n 1)))))
n-fact
=> (n-fact 0)
1
=> (n-fact 1)
1
=> (n-fact 5)
120
</PRE>
</BLOCKQUOTE>

<a name="SXlists2"></a>
<h3>  Lists, Symbols, and the car, cdr, and cond Functions</h3>

Scheme, which is a dialect of LISP (LISt Processor), processes  lists! <p>

A list is a set of space-delimited elements enclosed within a pair of left
and right parentheses, e.g.: <p>

<BLOCKQUOTE>
(a b c d e) 
<P>(Look here) 
<P>(1 3 5 7 9) 
</BLOCKQUOTE>

A list can include other lists: <p>

<BLOCKQUOTE>
((a b c) d (e f)) 
</BLOCKQUOTE>

List elements that are not lists are called atoms, e.g.: <p>

<BLOCKQUOTE>
<TABLE>
<TR><TD>(a b c d e) </TD><TD>contains the atoms a, b, c, d, and e </TD></TR>
<TR><TD>(Look here) </TD><TD>contains the atoms Look and here </TD></TR>
<TR><TD>(1 3 5 7 9) </TD><TD>contains the atoms 1, 3, 5, 7, and 9 </TD></TR>
<TR><TD>((a b c) d (e f)) </TD><TD>contains the atom d. </TD></TR>
<TR><TD>(<U>(a b c)</U> d (e f)) </TD><TD>contains the atoms a, b, and c. </TD></TR> 
<TR><TD>((a b c) d <U>(e f)</U>) </TD><TD>contains the atoms e and f. </TD></TR>
</TABLE>
</BLOCKQUOTE>

The Scheme function  <B>symbol?</B> tests for a symbol: <p>

<BLOCKQUOTE>
=&gt;(symbol? &#145;(a b c d e))
<P> #f 
<P> =&gt;(symbol? &#145;a)
<P> #t
</BLOCKQUOTE>

An important list,  the null list, contains no elements: <p>

<BLOCKQUOTE>
() 
</BLOCKQUOTE>

In recursive functions, the elements in a list are often removed from the
list and operated on one at a time until nothing remains but the null list.
The  <B>null?</B> function can test for the null list to provide an exit from a
recursive loop.  The looping action would produce results similar to the
following non-looping sequence:<p>

<BLOCKQUOTE>
<PRE>
=>(null? `(1 2 3))

#f                      ; Loop again.

=>(null? `(2 3))

#f                      ; Loop again.

=>(null? `(3))

#f                      ; Loop again.

=>(null? `())

#t                      ; Stop.
</PRE>
</BLOCKQUOTE>

The output of the function  <B>car</B> is the first element in a list: <p>

<BLOCKQUOTE>
 =&gt;(car &#145;(a b c d e))
<P> a
<P> =&gt; (car &#145;((a b c) d (e f)))
<P> (a b c)
</BLOCKQUOTE>

The output of the function  <B>cdr</B> is the list without the first element: <p>

<BLOCKQUOTE>
 =&gt; (cdr &#145;(a b c d e))
<P> (b c d e)
<P> =&gt; (cdr &#145;((a b c) d (e f)))
<P> (d (e f))
</BLOCKQUOTE>

The  <B>cond</B> function uses the following logic: <p>

<BLOCKQUOTE>
if condition A is true then action 1 
<P>else if condition B is true then action 2 
<P>else if condition C is true then action 3 
<P> ... 
<P>else action n
</BLOCKQUOTE>

<P>
The following function uses the  <B>car</B> and  <B>cdr</B> functions
to determine the truth of each condition in a <B>cond</B> function:  <p>

<BLOCKQUOTE>
<PRE>=>(define (look-see list1)
; The variable "list1" will be the name given to a list that we will
; examine.
      (cond
; If the first element in the list is "d", print message.
            ((equal? (car list1) `d)
             "The d is the first element.")
; If the second element (the first of the rest of the list) is "d", 
; print message. 
            ((equal? (car (cdr list1)) `d)
             "The d is the second element.")
; If the third element (the first of the rest of the rest of the list)
; is "d", print message.
            ((equal? (car (cdr (cdr list1))) `d)
             "The d is the third element.")
; If the fourth element (the first of the rest of the rest of the rest
; of the list) is "d", print message.
            ((equal? (car (cdr (cdr (cdr list1)))) `d)
             "The d is the fourth element.")
; If the fifth element (the first of the rest of the rest of the rest
; of  the rest of the list) is "d", print message.
            ((equal? (car (cdr (cdr (cdr (cdr list1))))) `d)
             "The d is the fifth element.")
; All five tests failed; print message.
          (else "The d is not present in the list.")))
look-see
</PRE>
</BLOCKQUOTE>

Next we&#146;ll define string  <B>list1</B> (to which we&#146;ll assign the
five elements "a b c d e"), and then we&#146;ll
execute function  <B>look-see</B>:<p>

<BLOCKQUOTE>
<PRE>=> (define list1

       `(a b c d e))

list1

=>(look-see list1)

d-in-fourth-element
</PRE>
</BLOCKQUOTE>

The following recursive function uses the null list and the  <B>car</B>
and   <B>cdr</B>
functions to add the numbers in a vector (a list whose elements are numbers).
If  <B>v-sum</B> is null, the result is 0.  If  <B>v-sum</B> is non-null, the first element
is added to the  <B>cdr</B> of <B>v-sum</B> recursively until the  <B>cdr</B> of 
<B>v-sum</B> sum is the null
list:<p>

<BLOCKQUOTE>
<TABLE>
<TR><TD>=&gt;(define</TD><TD>(v-sum v) </TD></TR>
<TR><TD></TD><TD>(cond </TD></TR>
<TR><TD></TD><TD>((null? v) 0) </TD></TR>
<TR><TD></TD><TD>(else (+ (car v) (v-sum (cdr v)))))) </TD></TR>
<TR><TD>v-sum</TD><TD></TD><TD></TD></TR> 
</TABLE>
</BLOCKQUOTE>

Variable  v is replaced by a vector when function  <B>v-sum</B> is executed:<p>

<BLOCKQUOTE>
<TABLE>
<TR><TD> =&gt; (define</TD><TD>v</TD></TR>
<TR><TD></TD><TD> &#145;(1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 </TD></TR>
<TR><TD></TD><TD> 16 17 18 19 20 21 22 23 24 25 26 </TD></TR>
<TR><TD></TD><TD> 27 28 29 30)) </TD></TR>
<TR><TD> v </TD><TD></TD><TD></TD></TR>
<TR><TD> =&gt;(v-sum v) </TD><TD></TD><TD></TD></TR>
<TR><TD> 465 </TD><TD></TD><TD></TD></TR>
</TABLE>
</BLOCKQUOTE>

<a name="SXmatrixex"></a>
<h3> A Matrix Example</h3>

In this example we create  <B>matrix-one</B>, a 5-by-4 matrix.  Then, using
<B>matrix-one</B>, we create  <B>matrix-two</B>, a 5-by-3 matrix whose first column is the
product of columns 1 and 2 in  <B>matrix-one</B>, and whose second and third columns
are columns 3 and 4 in  <B>matrix-one</B>. <p>

In Scheme, the matrix <p>

<BLOCKQUOTE>
   2 6 87 92 
<P>          5 23 15 8 
<P>          2 32 45 1 
<P>          56 7 29 4 
<P>          8 48 25 6 
</BLOCKQUOTE>

may be written as: <p>

<PRE>
 =&gt;(define matrix-one 

          &#145;((2 6 87 92) 

          (5 23 15 8) 

          (2 32 45 1) 

          (56 7 29 4) 

          (8 48 25 6))) 

 matrix-one 

</PRE>

We can test this by evaluating  <B>matrix-one</B>: <p>

<PRE>
  =&gt; matrix-one

  ((2 6 87 92) (5 23 15 8) (2 32 45 1) (56 7 29 4) (8 48 25 6))

</PRE>

The following expression will select column  n in any  matrix that contains
at least  n columns:  <p>

<PRE>
 =&gt; (define (col-n n matrix)

        (cond ((null? matrix) &#145;())

              (else (cons (list-ref (car matrix) n)

                          (col-n n (cdr matrix))))))

 col-n

</PRE>

What&#146;s going on here?  First, the  <B>list-ref</B> function, which
hasn&#146;t been introduced before, selects the  nth element
in a list, e.g., <p>

<PRE>
 =&gt; (list-ref &#145;(101 102 103 104 105) 3)

 104

</PRE>

Returning to  <B>col-n</B>, the  <B>cond</B> function tests  matrix
to see if it is null. 
If  matrix isn&#146;t null, the  <B>list-ref</B> function selects element  n in
the current vector (first row of the [current] matrix = "first matrix"),
which will be  <B>cons</B>-ed with the results of subsequent iterations of
<B>col-n</B>
of the  <B>cdr</B> of  matrix.  When a new iteration yields a null, the list of
elements is  <B>cons</B>-ed with (). <p>

Let&#146;s test column 1 in  <B>matrix-one</B>: <p>

<PRE>
 =&gt;(col-n 1 matrix-one)
<P>
 (2 5 2 56 8)
</PRE>

Let&#146;s test column 2 in  <B>matrix-one</B>: <p>

<PRE>
 =&gt;(col-n 2 matrix-one)
<P>
 (6 23 32 7 48)
</PRE>

Now that we know how to get columns 1 and 2, we can modify <B>col-n</B> slightly
to get the product of columns 1 and 2:<p>

<BLOCKQUOTE>
<PRE>=> (define (m-times-n m n matrix)

              (cond ((null? matrix) ())

                    (else

                     (cons (* (list-ref (car matrix) m)

                              (list-ref (car matrix) n)) 

                     (m-times-n m n (cdr matrix)))))) 

       m-times-n
</PRE>
</BLOCKQUOTE>

We now have two column parameters,  m and  n, and now we <B>cons</B> the products
of the <B>list-ref</B> of m and n of the <B>car</B> of  matrix.<p>

Let&#146;s  test  <B>m-times-n</B> for columns 1 and 2: <p>

<BLOCKQUOTE>
 =&gt; (m-times-n 1 2 matrix-one)
<P> (12 115 64 392 384)
</BLOCKQUOTE>

Now we can get everything we need from <B>matrix-one</B>:  any column and the
product of any two columns.  We need:  the product of columns 1 and 2;
column 3; and column 4.  We get these columns as vectors, so first
we&#146;ll lay them out as rows in an intermediate matrix named
<B>matrix-temp</B>: <p>

<BLOCKQUOTE>
<PRE>=> (define matrix-temp

              (list (m-times-n 1 2 matrix-one)

                    (col-n 3 matrix-one)

                    (col-n 4 matrix-one)))

matrix-temp
</PRE>
</BLOCKQUOTE>

We got <B>matrix-temp</B> by  cons-ing the product of columns 1 and 2, column 3,
column 4, and left &amp; right parentheses. <p>

Let&#146;s see  <B>matrix-temp</B>: <p>

<BLOCKQUOTE>
 =&gt; matrix-temp
<P> ((12 115 64 392 384) (87 15 45 29 25) (92 8 1 4 6))
</BLOCKQUOTE>

Finally, we&#146;ll use <B>col-n</B> to get columns 1, 2, 3, 4, and 5 from 
<B>matrix-temp</B>, and we&#146;ll <B>cons</B> them all together
within left and right
parentheses to get  <B>matrix-two</B>, our objective: <p>

<BLOCKQUOTE>
<PRE>=> (define matrix-two 

              (list (col-n 1 matrix-temp)

                    (col-n 2 matrix-temp)

                    (col-n 3 matrix-temp)

                    (col-n 4 matrix-temp)

                    (col-n 5 matrix-temp)))

       matrix-two
</PRE>
</BLOCKQUOTE>

Now, let&#146;s see  <B>matrix-two</B>: <p>

<BLOCKQUOTE>
 =&gt; matrix-two
<P> ((12 87 92) (115 15 8) (64 45 1) (392 29 4) (384 25 6))
</BLOCKQUOTE>

<BR>

<a name="SXscrefs"></a>
<h1 ALIGN="CENTER"> The PACT SCHEME/SX Reference</h1>

This section constitutes a more formal reference for using PACT SCHEME and SX.
There are two main sections (one for SCHEME and one for SX) each of which is
divided into two sections (SCHEME level use and C level use).<p>

<a name="SXscprocs"></a>
<h2 ALIGN="CENTER"> PACT Scheme Procedures</h2>

Although many of the procedures of Scheme can be expressed in terms of an
irreducible set of primitive Scheme functions, the PACT Scheme implementation
(as well as the R4RS description) provides a fairly rich set of Scheme functions
which are coded at the C level. These run faster by virtue of running from
compiled rather than interpreted code, and they make life a little bit easier
for programmers by providing many procedures which would otherwise have to be
supplied by the programmer.<p>

In PACT Scheme, I implemented two sets of functions. The first set constitutes
the core of the interpreter. The second set are not so essential to the majority
of applications and on small memory machines are expendable given the choice
between having these functions and not fitting on the machine. This second set
will be referred to as the large set and the first set will be referred to as
the core set. I will document these two sets separately.<p>

The description of the functions consists of the following information:<p>

<BLOCKQUOTE>
<B>name</B>
<P>Information for C based applications wanting to use this function
<P>Description of the function
<P>Usage of the function as a formal description
<P>Examples
</BLOCKQUOTE>
<P>
For example:<p>

<BLOCKQUOTE>
<B>if</B>
<TABLE>
<TR><TD>Macro: </TD><TD>Evaluate and return the consequent expression if 
the test evaluates to #t and evaluate and return the alternate expression
otherwise.</TD></TR>
<TR><TD>Usage: </TD><TD>(if test consequent alternate).</TD></TR>
<TR><TD>Example: </TD><TD>(if (= i 3) (display &#147;yes&#148;)
(display &#147;no&#148;))</TD></TR>
</TABLE>
</BLOCKQUOTE>
<p>

<a name="SXcore"></a>
<h3 ALIGN="CENTER"> The Core Set</h3>

<B>+</B>
<BR><I>Procedure: </I>Return sum of args or 0 if no args are supplied.
<P>
<B>-</B>
<BR><I>Procedure: </I>Return difference of args.
<P>
<B>*</B>
<BR><I>Procedure: </I>Return product of args or 1 if no args are supplied.
<P>
<B>/</B>
<BR><I>Procedure: </I>Return quotient of args (left associative).
<P>
<B>^</B>
<BR><I>Procedure: </I>Return the first argument raised to the power
of the second.
<P>
<B>&amp</B>
<BR><I>Procedure: </I>Return bitwise and of the arguments (left associative)."
<P>
<B>|</B>
<BR><I>Procedure: </I>Return bitwise or of the arguments (left associative)."
<P>
<B>~</B>
<BR><I>Procedure: </I>Return bitwise complement of the argument."
<P>
<B>&lt;&lt;</B>
<BR><I>Procedure: </I>Return left shift of first argument by the number of bits"
specified by the second.
<P>
<B>&gt;&gt;</B>
<BR><I>Procedure: </I>Return right shift of first argument by the number of bits"
specified by the second.
<P>

<B>&lt;</B>
<BR><I>Procedure: </I>Return #t iff the first argument is less than the second.
<P>
<B>&lt;=</B>
<BR><I>Procedure: </I>Return #t iff the first argument is less than
or equal to the second.
<P>
<B>=</B>
<BR><I>Procedure: </I>Return #t iff the first argument
is equal to the second.
<P>
<B>&gt;</B>
<BR><I>Procedure: </I>Return #t iff the first argument is
greater than the second.
<P>
<B>&gt;=</B>
<BR><I>Procedure: </I>Return #t iff the first argument
is greater than or equal to the second.
<P>

<B>abs</B>
<BR><I>Procedure: </I>Return the absolute value of a numeric object.
<P>
<B>acos</B>
<BR><I>Procedure: </I>Return the arc cosine of the argument.
<P>
<B>and</B>
<BR><I>Macro: </I>Evaluate expressions from left to right until one evaluates
to #f. The value of the last evaluated expression is returned.
<P>
<UL><I>Usage: </I>(and expression<SUB>1</SUB> ... expression<SUB>n</SUB>)
<P><I>Example: </I>(and (&lt; 3 4) (list 1 2 3) (&lt; 5 1) (display 5)) =&gt; #f
</UL>
<P>
<B>append</B>
<BR><I>Procedure: </I>Return a new list made from a copy of the first and the
second arguments.
<P>
<B>apply</B>
<BR><I>Macro: </I>Cons the procedure onto the args and evaluate the result.
<P>
<B>apropos</B>
<BR><I>Procedure: </I>Search the symbol table for documentation.
<P>
<B>ascii-file?</B>
<BR><I>Procedure: </I>Return #t if the object is an ascii file and #f otherwise.
<P>
<B>asin</B>
<BR><I>Procedure: </I>Return the arc sine of the argument.
<P>
<B>assoc</B>
<BR><I>Procedure: </I>Return the first list whose car is equal? to the first arg.
<P>
<B>assq</B>
<BR><I>Procedure: </I>Return the first list whose car is eq? to the first arg.
<P>
<B>assv</B>
<BR><I>Procedure: </I>Return the first list whose car is eqv? to the first arg.
<P>
<B>atan</B>
<BR><I>Procedure: </I>Return the arc tangent of the argument.
<P>
<B>begin</B>
<BR><I>Macro: </I>Evaluate a list of expressions and return the value
of the last one.
<P>
<B>boolean?</B>
<BR><I>Procedure: </I>Return #t if the object is a boolean and #f otherwise.
<P>
<B>break</B>
<BR><I>Procedure: </I>Enter a Scheme break, return with <B>return-level</B>.
A Scheme break is simply a recursively executed <B>read-eval-print</B> loop.
It constitutes a separate loop n which to perform tasks before returning to
the preceding evaluation.
<UL>
<I>Usage: </I>(break)
<P><I>Example: </I>(break)
</UL>
<P>
<B>caaar</B>
</B><BR><I>C Information: </I>SS_caaar
<BR><I>Procedure: </I>Return the caaar of the argument.
<P>
<B>caadr</B>
</B><BR><I>C Information: </I>SS_caadr
<BR><I>Procedure: </I>Return the caadr of the argument.
<P>
<B>caar</B>
</B><BR><I>C Information: </I>SS_caar
<BR><I>Procedure: </I>Return the caar, i.e. the car of the car, of the argument.
<P>
<B>cadar</B>
</B><BR><I>C Information: </I>SS_cadar
<BR><I>Procedure: </I>Return the cadar, i.e. the car of the cdr
of the car, of the argument.
<P>
<B>caddr</B>
</B><BR><I>C Information: </I>SS_caddr
<BR><I>Procedure: </I>Return the caddr, i.e. the car of the cdr
of the cdr, of the argument.
<P>
<B>cadr</B>
</B><BR><I>C Information: </I>SS_cadr
<BR><I>Procedure: </I>Return the cadr, i.e. the car of the cdr, of the argument.
<P>
<B>car</B>
</B><BR><I>C Information: </I>SS_car
<BR><I>Procedure: </I>Return the first member of the pair which is the
argument. For a simple pair this is the left hand member and for a list
it is the leftmost member of the list.
<UL>
<I>Usage: </I>(car pair)
<P><I>Example: </I>(car &#145;(3 . 4)) =&gt; 3
<P><PRE>         (car &#145;(1 2 3)) =&gt; 1</PRE>
</UL>
<P>
<B>call-with-cc</B>
<BR><I>Procedure: </I>Pass an escape procedure (a special procedure of
one argument) to the argument.
<P>
<B>cdar</B>
</B><BR><I>C Information: </I>SS_cdar
<BR><I>Procedure: </I>Return the cdar, i.e. the cdr of the car, of the argument.
<P>
<B>cdaar</B>
</B><BR><I>C Information: </I>SS_cdaar
<BR><I>Procedure: </I>Return the cdaar, i.e. the cdr of the car of
the car, of the argument.
<P>
<B>cdadr</B>
</B><BR><I>C Information: </I>SS_cdadr
<BR><I>Procedure: </I>Return the cdadr, i.e. the cdr of the car
of the cdr, of the argument.
<P>
<B>cddar</B>
</B><BR><I>C Information: </I>SS_cddar
<BR><I>Procedure: </I>Return the cddar, i.e. the cdr of the cdr
of the car, of the argument.
<P>
<B>cdddr</B>
</B><BR><I>C Information: </I>SS_cdddr
<BR><I>Procedure: </I>Return the cdddr, i.e. the cdr of the cdr
of the cdr of the argument.
<P>
<B>cddr</B>
</B><BR><I>C Information: </I>SS_cddr
<BR><I>Procedure: </I>Return the cddr, i.e, the cdr of the cdr, of the argument.
<P>
<B>cdr</B>
</B><BR><I>C Information: </I>SS_cdr
<BR><I>Procedure: </I>Return the second member of the pair which
is the argument.
<UL>
<I>Usage: </I>(cdr pair)
<P><I>Example: </I>(cdr &#145;(3 . 4)) =&gt; 4
<P><PRE>         (cdr &#145;(1 2 3)) =&gt; (2 3)</PRE>
</UL>
<P>
<B>ceiling</B>
<BR><I>Procedure: </I>Return the smallest integer greater than the argument.
<P>
<B>close-input-file</B>
<BR><I>Procedure: </I>Close the specified input port and release the
<B>IN_PORT</B> object.</B>
<P>
<B>close-output-file</B>
<BR><I>Procedure: </I>Close the specified output port and release the
<B>OUT_PORT</B> object.</B>
<P>
<B>cond</B>
<BR><I>Macro: </I>Evaluate the list of cond clauses returning the value of
the first one whose first expression does not evaluate to #f. A cond clause
is a list of expressions whose irst element is evaluated and if the result
is not #f, the remaining expressions are evaluated and the value of the last
one is returned as the value of the clause, otherwise the cond clause
evaluates to #f.
<UL>
<I>Usage: </I>(cond clause<SUB>1</SUB> ... clause<SUB>n</SUB>)
<P><I>Example: </I>(cond ((= i 1) expr ...)
<P><PRE>               ((&lt; i 1) expr ...)</PRE>
<P><PRE>               ((&gt; i 1) expr ...))</PRE>
</UL>
<P>
<B>cons</B>
<BR><I>Procedure: </I>Return a new pair whose car and cdr are the arguments.
<UL>
<I>Usage: </I>(cons expr<SUB>1</SUB> expr<SUB>2</SUB>)
<P><I>Example: </I>(cons 3 4) =&gt; (3 . 4)
<P><PRE>         (cons 3 &#145;(2 1)) =&gt; (3 2 1)</PRE>
</UL>
<P>
<B>cos</B>
<BR><I>Procedure: </I>Return the cosine of the argument.
<P>
<B>define</B>
<BR><I>Macro: </I>Define variables and procedures in the current environment.
<P>
<B>define-macro</B>
<BR><I>Macro: </I>Define a macro in the current environment.
<P>
<B>describe</B>
<BR><I>Procedure: </I>Print the documentation for a procedure to the
specified device.
<P>
<B>display</B>
<BR><I>Procedure: </I>Print an object to the specified device in
human readable form.
<P>
<B>eof-object?</B>
<BR><I>Procedure: </I>Return #t if the object is an end-of-file
and #f otherwise.
<P>
<B>eq?</B>
<BR><I>Procedure: </I>Return #t iff the two objects are identical,
i.e. the two objects are the same object.
<UL>
<I>Usage: </I>(eq? expr<SUB>1</SUB> expr<SUB>2</SUB>)
<P><I>Example: </I>(define foo 3)
<P><PRE>         (define bar 3)</PRE>
<P><PRE>         (eq? foo bar) =&gt; #f</PRE>
<P><PRE>         (eq? foo foo) =&gt; #t</PRE>
</UL>
<P>
<B>equal?</B>
<BR><I>Procedure: </I>Return the result of recursively applying eqv?
to the arguments.
<P>
<B>eqv?</B>
<BR><I>Procedure: </I>Return #t iff the two objects are equivalent,
i.e. the values of the two objects are the same.
<UL>
<I>Usage: </I>(eqv? expr<SUB>1</SUB> expr<SUB>2</SUB>)
<P><I>Example: </I>(define foo 3)
<P><PRE>         (define bar 3)</PRE>
<P><PRE>         (eqv? foo bar) =&gt; #t</PRE>
</UL>
<P>
<B>eval</B>
<BR><I>Procedure: </I>Evaluate the given form and return the value.
<P>
<B>even?</B>
<BR><I>Procedure: </I>Return #t iff the argument is a number divisible
exactly by 2.
<P>
<B>exp</B>
<BR><I>Procedure: </I>Return the exponential of the argument.
<P>
<B>expt</B>
<BR><I>Procedure: </I>Return the first argument raised to the power
of the second.
<P>
<B>file?</B>
<BR><I>Procedure: </I>Return #t if the object is a file and #f otherwise.
<P>
<B>floor</B>
<BR><I>Procedure: </I>Return the greatest integer less than the argument.
<P>
<B>for-each</B>
<BR><I>Procedure: </I>Apply a procedure to a set of lists (for side effect).
<P>
<B>guest-object?</B>
<BR><I>Procedure: </I>Return #t if the object is a <B>GUEST</B> object</B>
and #f otherwise.
<P>
<B>if</B>
<BR><I>Macro: </I>Evaluate and return the consequent expression if
the test evaluates to #t and 
evaluate and return the alternate expression otherwise.
<UL>
<I>Usage: </I>(if test consequent alternate).
</UL>
<P>
<B>input-port?</B>
</B><BR><I>C Information: </I>SS_inportp
<BR><I>Procedure: </I>Return #t if the object is an input port
(<B>IN_PORT</B>) and #f otherwise.</B>
<P>
<B>integer?</B>
<BR><I>Procedure: </I>Return #t if the object is an integer
number and #f otherwise.
<P>
<B>lambda</B>
<BR><I>Macro: </I>Define an anonymous procedure.
<UL>
<I>Usage: </I>(lambda parameters
expression<SUB>1</SUB> ... expression<SUB>n</SUB>).
</UL>
<P>
<B>last</B>
<BR><I>Procedure: </I>Return the last element of a list or
return any other object.
<P>
<B>length</B>
</B><BR><I>C Information: </I>SS_length, SS_nargs
<BR><I>Procedure: </I>Return the number of elements in the given list.
<P>
<B>let</B>
</B><BR><I>C Information: </I>SS_let, UE_MACRO, SS_nargs
<BR><I>Macro: </I>Define a region with variables of local scope.
<UL>
<I>Usage: </I>(let local-variables body)
<P><I>Example: </I>(let ((x (car &#145;(3 . 4)))
<P><PRE>                (y (cdr &#145;(3 . 4))))</PRE>
<P><PRE>               (+ x y)) =&gt; 7</PRE>
</UL>
<P>
<B>let*</B>
<BR><I>Macro: </I>Define a region with variables of local scope.
The variables can be used in the 
definition of other variables as soon as they are defined.
<UL>
<I>Usage: </I>(let* local-variables body)
<P><I>Example: </I>(let* ((x (cdr &#145;(1 2 3)))
<P><PRE>                 (y (cdr x)))</PRE>
<P><PRE>                (append x y)) =&gt; (2 3 3)</PRE>
</UL>
<P>
<B>list</B>
<BR><I>Procedure: </I>Return a new list made up of the arguments.
<P>
<B>ln</B>
<BR><I>Procedure: </I>Return the natural logarithm of the argument.
<P>
<B>load</B>
<BR><I>Procedure: </I>Open a file of Scheme forms and eval all the objects in it.
If the optional flag is #t load all objects into the global environment.
<UL>
<I>Usage: </I>(load file flag)
<p>
<I>Example: </I>
<PRE>
         (load "foo.scm")
         (load "foo.scm" #t)
</PRE>
<P>
<B>log</B>
<BR><I>Procedure: </I>Return the logarithm base 10 of the argument.
<P>
<B>make-new-symbol</B>
<BR><I>Procedure: </I>Generate a new symbol.
<P>
<B>map</B>
<BR><I>Procedure: </I>Map a procedure over a set of lists.
<P>
<B>member</B>
<BR><I>Procedure: </I>Return the first sublist of the second arg
whose car is equal? to the first arg.
<P>
<B>memq</B>
<BR><I>Procedure: </I>Return the first sublist of the
second arg whose car is eq? to the first arg.
<P>
<B>memv</B>
<BR><I>Procedure: </I>Return the first sublist of the second arg whose
car is eqv? to the first arg.
<P>
<B>negative?</B>
<BR><I>Procedure: </I>Return #t iff the argument is a number less than 0.
<P>
<B>newline</B>
<BR><I>Procedure: </I>Print a &lt;CR&gt;&lt;LF&gt; or equivalent to the
specified device.
<P>
<B>not</B>
<BR><I>Procedure: </I>Return #t if object is #f and #f for any other object.
<P>
<B>null?</B>
</B><BR><I>C Information: </I>SS_nullobjp
<BR><I>Procedure: </I>Return #t iff the object is the empty list, ().
<P>
<B>number?</B>
</B><BR><I>C Information: </I>SS_numbp
<BR><I>Procedure: </I>Return #t if the object is a number and #f otherwise.
<P>
<B>odd?</B>
<BR><I>Procedure: </I>Return #t iff the argument is a number that is not even.
<P>
<B>open-input-file</B>
<BR><I>Procedure: </I>Open the specified file for input and return an
<B>IN_PORT</B> object.</B>
<P>
<B>open-output-file</B>
<BR><I>Procedure: </I>Open the specified file for output and return
an <B>OUT_PORT</B> object.</B>
<P>
<B>or</B>
<BR><I>Macro: </I>Evaluate the forms until one evaluates to
something other than #f and return 
that value as the result and return #f otherwise.
<UL>
<I>Usage: </I>(or expression<SUB>1</SUB> ... expression<SUB>n</SUB>)
<P><I>Example: </I>(or (= 3 4) (display &#147;foo&#148;) (&lt; 1 5)
(display &#147;bar&#148;)) =&gt; #t
</UL>
<P>
<B>output-port?</B>
</B><BR><I>C Information: </I>SS_outportp
<BR><I>Procedure: </I>Return #t if the object is an output port
(<B>OUT_PORT</B>) and #f otherwise.</B>
<P>
<B>pair?</B>
</B><BR><I>C Information: </I>SS_consp
<BR><I>Procedure: </I>Return #t if the object is a cons or list and #f otherwise.
<P>
<B>positive?</B>
<BR><I>Procedure: </I>Return #t iff the argument is a number greater than 0.
<P>
<B>print-toggle</B>
<BR><I>Procedure: </I>Toggle the printing of values.
<P>
<B>printf</B>
<BR><I>Procedure: </I>C-like formatted print function.
<UL>
<I>Usage: </I>(printf port format . rest)
<P><I>Example: </I>(printf nil &#147;%s - %d\n&#148;
&#147;foo&#148; 3) =&gt; #f
</UL>
<P>
<B>procedure?</B>
</B><BR><I>C Information: </I>SS_procedurep
<BR><I>Procedure: </I>Return #t if the object is a procedure
object and #f otherwise.
<P>
<B>quasiquote</B>
<BR><I>Macro: </I>Like <B>quote</B> except that <B>unquote</B></B>
and <B>unquote-splicing</B> forms are eval&#146;d.</B>
<P>
<B>quit</B>
<BR><I>Macro: </I>Exit from Scheme.
<P>
<B>quote</B>
<BR><I>Macro: </I>Return the expression without first evaluating it.
<UL>
<I>Usage: </I>(quote expression)
</UL>
<P>
<B>quotient</B>
<BR><I>Procedure: </I>Return quotient of two integers.
<P>
<B>read</B>
<BR><I>Procedure: </I>Read an ASCII representation of an object
and return the object.
<P>
<B>read-guest</B>
<BR><I>Procedure: </I>Return a guest object using the supplied function rd_guest.
<P>
<B>real?</B>
</B><BR><I>C Information: </I>SS_floatp
<BR><I>Procedure: </I>Return #t if the object is a real number and #f otherwise.
<P>
<B>remainder</B>
<BR><I>Procedure: </I>Return remainder of division of the two arguments.
<P>
<B>reset</B>
<BR><I>Procedure: </I>Unwind the Error/Break Stack and return to top level.
<P>
<B>return-level</B>
<BR><I>Procedure: </I>Pop n levels off the Error/Break Stack and return
the second arg as value.
<P>
<B>reverse</B>
</B><BR><I>C Information: </I>SS_reverse
<BR><I>Procedure: </I>Destructively reverse the list and return it.
<P>
<B>set!</B>
<BR><I>Macro: </I>Replace the contents of the location the variable
points to with the value.
<P>
<B>set-car!</B>
<BR><I>Procedure: </I>Replace the car of the first argument with
the second and return the new car.
<P>
<B>set-cdr!</B>
<BR><I>Procedure: </I>Replace the cdr of the first argument with the
second and return the new cdr.
<P>
<B>sin</B>
<BR><I>Procedure: </I>Return the sine of the argument.
<P>
<B>sqrt</B>
<BR><I>Procedure: </I>Return the principal square root of the argument.
<P>
<B>stats-toggle</B>
<BR><I>Procedure: </I>Toggle the printing of control statistics.
<P>
<B>string?</B>
</B><BR><I>C Information: </I>SS_stringp
<BR><I>Procedure: </I>Return #t if the object is a string and #f otherwise.
<P>
<B>symbol?</B>
</B><BR><I>C Information: </I>SS_variablep
<BR><I>Procedure: </I>Return #t if the object is a variable and #f otherwise.
<P>
<B>system</B>
<BR><I>Procedure: </I>Have the operating system carry out the command
contained in the string.
<P>
<B>tan</B>
<BR><I>Procedure: </I>Return the tangent of the argument.
<P>
<B>trace</B>
<BR><I>Procedure: </I>Trace calls to the procedures in the list of arguments.
<P>
<B>transcript-off</B>
<BR><I>Macro: </I>Close the transcript file to stop recording a Scheme session.
<P>
<B>transcript-on</B>
<BR><I>Procedure: </I>Open the specified transcript file to start
recording a Scheme session.
<P>
<B>truncate</B>
<BR><I>Procedure: </I>Return the integer resulting from the truncation
of the argument.
<P>
<B>unquote</B>
<BR><I>Macro: </I>In a quasiquote&#146;d form inserts the result of
evaluating its argument.
<P>
<B>unquote-splicing</B>
<BR><I>Macro: </I>In a quasiquote&#146;d form splices the evaluated list
into the quoted form.
<P>
<B>untrace</B>
<BR><I>Procedure: </I>Stop tracing the procedures in the argument list.
<P>
<B>write</B>
<BR><I>Procedure: </I>Put the printed representation of an object to the
specified device.
<P>
<B>xor</B>
<BR><I>Procedure: </I>Return bitwise exclusive or of the arguments (left associative)."
<P>
<B>zero?</B>
<BR><I>Procedure: </I>Return #t iff the argument is a number equal to 0.
<P>

<a name="538834"></a>
<h3 ALIGN="CENTER"> The Large Set</h3>

<B>call-with-input-file</B>
<BR><I>Procedure: </I>Open the named file and evaluate a procedure
using the port for input. This 
is analogous to input redirection in certain operating systems
<UL>
<I>Usage: </I>(call-with-input-file string procedure)
<P><I>Example: </I>(call-with-input-file &#147;foobar.scm&#148; my-reader)
</UL>
<P>
<B>call-with-output-file</B>
<BR><I>Procedure: </I>Open the named file and evaluate a procedure using
the port for output. This is analogous to output redirection in certain
operating systems.
<UL>
<I>Usage: </I>(call-with-output-file string procedure)
</UL>
<P>
<B>char?</B>
<BR><I>Procedure: </I>Return #t iff the object is of type char.
<P>
<B>char=?</B>
<BR><I>Procedure: </I>Return #t iff the chars are the same.
<P>
<B>char&gt;=?</B>
<BR><I>Procedure: </I>Return #t iff the first character is &#145;greater
than or equal to&#146; the second.
<P>
<B>char&gt;?</B>
<BR><I>Procedure: </I>Return #t iff the first character is &#145;greater
than&#146; the second.
<P>
<B>char&lt;=?</B>
<BR><I>Procedure: </I>Return #t iff the first character is &#145;less
than or equal to&#146; the second.
<P>
<B>char&lt;?</B>
<BR><I>Procedure: </I>Return #t iff the first character is &#145;less
than&#146; the second.
<P>
<B>char-&gt;integer</B>
<BR><I>Procedure: </I>Return the integer representation of the given integer.
<P>
<B>current-input-port</B>
<BR><I>Procedure: </I>Return the current default input port.
<P>
<B>current-output-port</B>
<BR><I>Procedure: </I>Return the current default output port.
<P>
<B>define-global</B>
<BR><I>Macro: </I>Define variables and procedures in the global environment.
<P>
<B>hash-dump</B>
<BR><I>Procedure: </I>Return a list of the names in the given hash table.
<P>
<B>hash-element?</B>
</B><BR><I>C Information: </I>SS_haelemp
<BR><I>Procedure: </I>Return #t if the object is a hash-element.
<P>
<B>hash-info</B>
<BR><I>Procedure: </I>Return the specified hash table&#146;s size, number
of hash-elements, and documentation flag.
<P>
<B>hash-install</B>
<BR><I>Procedure: </I>Install the given object in the given hash table.
<P>
<B>hash-lookup</B>
<BR><I>Procedure: </I>Look up and return the named object in the given
hash table.
<P>
<B>hash-remove</B>
<BR><I>Procedure: </I>Remove the named object from the given hash table.
<P>
<B>hash-table?</B>
</B><BR><I>C Information: </I>SS_hasharrp
<BR><I>Procedure: </I>Return #t if the object is a hash array.
<P>
<B>integer-&gt;char</B>
<BR><I>Procedure: </I>Return the character representation of the given integer.
<P>
<B>list-&gt;string</B>
<BR><I>Procedure: </I>Return a string constructed from a list of characters.
<P>
<B>list-&gt;vector</B>
<BR><I>Procedure: </I>Return a vector whose elements are the same as
the list&#146;s.
<P>
<B>make-hash-table</B>
<BR><I>Procedure: </I>Return a new hash table.
<P>
<B>make-vector</B>
<BR><I>Procedure: </I>Return a new vector whose length is specified by
the argument.
<P>
<B>process?</B>
</B><BR><I>C Information: </I>SS_processp
<BR><I>Procedure: </I>Return #t if the object is a <B>PROCESS_OBJ</B>.</B>
<P>
<B>process-close</B>
<BR><I>Procedure: </I>Terminate a process.
<P>
<B>process-open</B>
<BR><I>Procedure: </I>Exec and return a process object
which can be used much like a port in 
those I/O functions which take a process object as the source or sink of 
messages.
<UL>
<I>Usage: </I>(process-open string)
<P><I>Example: </I>(define pp (process-open &#147;ls&#148;))
</UL>
<P>
<B>process-read-line</B>
<BR><I>Procedure: </I>Return a string received from a process.
<UL>
<I>Usage: </I>(process-read-line process)
<P><I>Example: </I>(print (process-read-line pp))
</UL>
<P>
<B>process-running?</B>
<BR><I>Procedure: </I>Return #t if the process is still running.
<P>
<B>process-send-line</B>
<BR><I>Procedure: </I>Send a string to a process.
<P>
<B>process-status</B>
<BR><I>Procedure: </I>Return a list of the process id, in, out, status, and
reason members of the <B>PROCESS</B> structure.</B>
<P>
<B>read-char</B>
<BR><I>Procedure: </I>Read and return a single character.
<P>
<B>read-line</B>
<BR><I>Procedure: </I>Read a line of text and return a string.
<P>
<B>string=?</B>
<BR><I>Procedure: </I>Return #t iff the strings are the same (length too).
<P>
<B>string&gt;=?</B>
<BR><I>Procedure: </I>Return #t iff the first string is &#145;greater
than or equal to&#146; the second.
<P>
<B>string&gt;?</B>
<BR><I>Procedure: </I>Return #t iff the first string is &#145;greater
than&#146; the second.
<P>
<B>string&lt;=?</B>
<BR><I>Procedure: </I>Return #t iff the first string is &#145;less
than or equal to&#146; the second.
<P>
<B>string&lt;?</B>
<BR><I>Procedure: </I>Return #t iff the first string is &#145;less
than&#146; the second.
<P>
<B>string-&gt;list</B>
<BR><I>Procedure: </I>Construct a list of the characters in the given string.
<P>
<B>string-&gt;port</B>
<BR><I>Procedure: </I>Encapsulate a string as a pseudo input port for reading.
<P>
<B>string-&gt;symbol</B>
<BR><I>Procedure: </I>Make a new variable with name given by the string.
<P>
<B>string-append</B>
<BR><I>Procedure: </I>Append the argument strings together into a
new string and return it.
<P>
<B>string-length</B>
<BR><I>Procedure: </I>Return the number of characters in the given string.
<P>
<B>string-ref</B>
<BR><I>Procedure: </I>Return the nth character in the given string.
<P>
<B>substring</B>
<BR><I>Procedure: </I>Extract the substring zero-origin indexed by
the last two args.
<P>
<B>symbol-&gt;string</B>
<BR><I>Procedure: </I>Make a new string out of the given variable name.
<P>
<B>vector</B>
<BR><I>Procedure: </I>Analog to list procedure for vectors.
<P>
<B>vector?</B>
</B><BR><I>C Information: </I>SS_vectorp
<BR><I>Procedure: </I>Return #t iff the object is of type vector.
<P>
<B>vector-length</B>
<BR><I>Procedure: </I>Return the number of elements in the specified vector.
<P>
<B>vector-&gt;list</B>
<BR><I>Procedure: </I>Return a list whose elements are the same
as the vector&#146;s.
<P>
<B>vector-ref</B>
<BR><I>Procedure: </I>Return the nth element of the given vector.
<P>
<B>vector-set!</B>
<BR><I>Procedure: </I>Sets the nth element of the given vector.
<P>
<B>write-char</B>
<BR><I>Procedure: </I>Write a single character to the specified port.
<P>

<a name="539001"></a>
<h2 ALIGN="CENTER">  The PACT Scheme Library</h2>

In this section we list some of the functions in the PACT SCHEME library.
Although there are many functions here, only a relative handful are useful
to application developers wishing to integrate the interpreter into their
product. All of the relevant declarations are made in the header file
scheme.h which should be #include&#146;d in your source code.<p>

<a name="539887"></a>
<h3 ALIGN="CENTER"> Constants</h3>

These constants are part of the essential glue of PACT SCHEME
and the other parts of PACT which use it (i.e. SX and ULTRA).<p>

<a name="540032"></a>
<h4> Object Type Constants</h4>

These integer variables define the type indices for SCHEME objects. They are used
in conjunction with <B>SS_args, SS_call_scheme</B>, and <B>SS_make_list</B>.</B>
They are also implicitly used by the C and SCHEME level predicate functions.<p>

<BLOCKQUOTE>
<TABLE>
<TR><TD>SS_OBJECT_I</TD><TD>Generic object type</TD></TR>
<TR><TD>CONS</TD><TD>Cons object type</TD></TR>
<TR><TD>VARIABLE</TD><TD>Symbol object type</TD></TR>
<TR><TD>PROC_OBJ</TD><TD>Procedure object type</TD></TR>
<TR><TD>BOOLEAN</TD><TD>Boolean object type</TD></TR>
<TR><TD>IN_PORT</TD><TD>Input port object type</TD></TR>
<TR><TD>OUT_PORT</TD><TD>Output port object type</TD></TR>
<TR><TD>VECTOR</TD><TD>Vector object type</TD></TR>
<TR><TD>CHAR_OBJ</TD><TD>Character object type</TD></TR>
<TR><TD>HAELEM</TD><TD>Hash element object type</TD></TR>
<TR><TD>HASHARR</TD><TD>Hash array object type</TD></TR>
<TR><TD>PROCESS_OBJ</TD><TD>Process object type</TD></TR>
</TABLE>
</BLOCKQUOTE>


<a name="SXevalcon"></a>
<h4> Evaluation Type Constants</h4>

The following constants are described in detail in the
section on interfacing compiled and interpreted code.  They
are defined in the SS_eval_mode enumeration<p>

<BLOCKQUOTE>
<TABLE>
<TR><TD>SELF_EV</TD><TD>Objects evaluate to themselves</TD></TR>
<TR><TD>VAR_EV</TD><TD>Symbol objects evaluate to their value binding</TD></TR>
<TR><TD>PROC_EV</TD><TD>Procedure call (conses are the only things that have this)</TD></TR>
<TR><TD>NO_EV</TD><TD>No evaluation</TD></TR>
</TABLE>
</BLOCKQUOTE>

<a name="SXproccon"></a>
<h4> Procedure Type Constants</h4>

These constants are used to distinguish among different ways of handling
procedures and their calls. They are described in more detail in the
section on interfacing compiled and interpreted code.  They are defined
by the SS_form enumeration<p>

<BLOCKQUOTE>
<TABLE>
<TR><TD>SS_PR_PROC</TD><TD>Evaluate arguments, don&#146;t re-evaluate the result</TD></TR>
<TR><TD>SS_EE_MACRO</TD><TD>Evaluate arguments, re-evaluate the result</TD></TR>
<TR><TD>SS_UR_MACRO</TD><TD>Don&#146;t evaluate the arguments, don&#146;t re-evaluate the result</TD></TR>
<TR><TD>SS_UE_MACRO</TD><TD>Don&#146;t evaluate the arguments, re-evaluate the result</TD></TR>
</TABLE>
</BLOCKQUOTE>

<a name="539899"></a>
<h3> Macros</h3>

These macros facilitate interfacing with the interpreter and writing C level
routines to be used by the interpreter.<p>

<a name="540092"></a>
<h4> Object Accessors</h4>

<BLOCKQUOTE>
<TABLE>
<TR><TD>SS_GET(type, x)</TD>
<TD>cast the value part of x to be a type pointer and return it</TD></TR>
<TR><TD>SSS_INQUIRE_OBJECT(name)</TD>
<TD>lookup name in the system table and return a pointer to the
associated object</TD></TR>
<TR><TD>SS_OBJECT_GC(x)</TD>
<TD>return the reference count of x (how many things point to it)</TD></TR>
<TR><TD>SS_UNCOLLECT(x)</TD>
<TD>make sure that x can never be garbage collected</TD></TR>
<TR><TD>SS_OBJECT_TYPE(x)</TD>
<TD>return the type index of x</TD></TR>
<TR><TD>SS_OBJECT_NAME(x)</TD>
<TD>return the print name of x</TD></TR>
<TR><TD>SS_OBJECT(x)</TD>
<TD>return the value part of x</TD></TR>
<TR><TD>SS_OBJECT_ETYPE(x)</TD>
<TD>return the evaluation type of x</TD></TR>
<TR><TD>SS_OBJECT_PRINT(x, strm)</TD>
<TD>invoke the print method of x on the output port strm</TD></TR>
<TR><TD>SS_OBJECT_FREE(x)</TD>
<TD>invoke the release method of x</TD></TR>
</TABLE>
</BLOCKQUOTE>

<a name="540122"></a>
<h4> Number Accessors</h4>

<BLOCKQUOTE>
<TABLE>
<TR><TD>SS_INTEGER_VALUE(x)</TD>
<TD>return the value of the integer object x (as a long)</TD></TR>
<TR><TD>SS_FLOAT_VALUE(x)</TD>
<TD>return the value of the float object x (as a double)</TD></TR>
<TR><TD>SS_int_val(x)</TD>
<TD>return TRUE iff the number x has an integer value</TD></TR>
</TABLE>
</BLOCKQUOTE>

<a name="540131"></a>
<h4> Input_port/output_port Accessors</h4>

<BLOCKQUOTE>
<TABLE>
<TR><TD WIDTH="150">SS_OUTSTREAM(x)</TD>
<TD>return the FILE * from the output port x </TD></TR> 
<TR><TD>SS_INSTREAM(x)</TD>                                
<TD>return the FILE * from the input port x </TD></TR>
</TABLE>
</BLOCKQUOTE>

<P>
<P>

<a name="540151"></a>
<h4> Cons Accessors</h4>

<BLOCKQUOTE>
<TABLE>
<TR><TD WIDTH="150">SS_CAR_MACRO(x)</TD>                                
<TD>return the car of x (no error checking) </TD></TR>
<TR><TD>SS_CDR_MACRO(x)</TD>                                
<TD>return the cdr of x (no error checking) </TD></TR>
</TABLE>
</BLOCKQUOTE>

<P>
<P>

<a name="540162"></a>
<h4> Variable Accessors</h4>


<BLOCKQUOTE>
<TABLE>
<TR><TD WIDTH="150">SS_VARIABLE_NAME(x)</TD>
<TD>return the name of the symbol object x </TD></TR>
<TR><TD>SS_VARIABLE_VALUE(x)</TD>
<TD>return the value of the symbol object x </TD></TR>
</TABLE>
</BLOCKQUOTE>

<P>
<P>

<a name="540184"></a>
<h4> String Accessors</h4>


<BLOCKQUOTE>
<TABLE>
<TR><TD WIDTH="150">SS_STRING_TEXT(x)</TD>
<TD>return the text of the string object x </TD></TR>
<TR><TD>SS_STRING_LENGTH(x)</TD>
<TD>return the length of the text of x </TD></TR>
</TABLE>
</BLOCKQUOTE>

<P>
<P>

<a name="540191"></a>
<h4> Procedure Accessors</h4>

<BLOCKQUOTE>
<TABLE>
<TR><TD WIDTH="150">SS_PROCEDURE_TYPE(x)</TD>
<TD>return the type of the procedure object x </TD></TR>
<TR><TD>SS_PROCEDURE_NAME(x)</TD>
<TD>return the name of the procedure object x </TD></TR>
<TR><TD>SS_PROCEDURE_DOC(x)</TD>
<TD>return the documentation for the procedure x </TD></TR>
<TR><TD>SS_PROCEDURE_TRACEDP(x)</TD>
<TD>return TRUE if the procedure x is traced </TD></TR>
<TR><TD>SS_PROCEDURE_PROC(x)</TD>
<TD>return the actual procedure for x </TD></TR>
</TABLE>
</BLOCKQUOTE>

<P>

<a name="540144"></a>
<h4> Vector Accessors</h4>

<BLOCKQUOTE>
<TABLE>
<TR><TD WIDTH="150">SS_VECTOR_LENGTH(x)</TD>
<TD>return the length of the vector object x </TD></TR>
<TR><TD>SS_VECTOR_ARRAY(x)</TD>
<TD>return the array of objects of the vector x </TD></TR>
</TABLE>
</BLOCKQUOTE>

<P>

<a name="540284"></a>
<h4> Character Accessor</h4>

<BLOCKQUOTE>
<TABLE>
<TR><TD WIDTH="150">SS_CHARACTER_VALUE(x)</TD>
<TD>return the value of the character object x </TD></TR>
</TABLE>
</BLOCKQUOTE>

<P>

<a name="540290"></a>
<h4> Process Accessors</h4>

<BLOCKQUOTE>
<TABLE>
<TR><TD WIDTH="150">SS_PROCESS_VALUE(x)</TD>
<TD>return the PROCESS * of the process object x </TD></TR>
<TR><TD>SS_PROCESS_STATUS(x)</TD>
<TD>return the status of the process x </TD></TR>
</TABLE>
</BLOCKQUOTE>

<P>

<a name="540377"></a>
<h4> Predicates</h4>

<BLOCKQUOTE>
<TABLE>
<TR><TD WIDTH="150">SS_integerp(x)</TD>
<TD>TRUE iff x is of type INTEGER </TD></TR>
<TR><TD>SS_floatp(x)</TD>
<TD>TRUE iff x is of type FLOAT </TD></TR>
<TR><TD>SS_stringp(x)</TD>
<TD>TRUE iff x is of type STRING </TD></TR>
<TR><TD>SS_consp(x)</TD>
<TD>TRUE iff x is of type CONS </TD></TR>
<TR><TD>SS_variablep(x)</TD>
<TD>TRUE iff x is of type VARIABLE (symbol) </TD></TR>
<TR><TD>SS_inportp(x)</TD>
<TD>TRUE iff x is of type IN_PORT (input port) </TD></TR>
<TR><TD>SS_outportp(x)</TD>
<TD>TRUE iff x is of type OUT_PORT (output port) </TD></TR>
<TR><TD>SS_eofobjp(x)</TD>
<TD>TRUE iff x is of type EOF_OBJ (#eof) </TD></TR>
<TR><TD>SS_nullobjp(x)</TD>
<TD>TRUE iff x is of type NULL_OBJ (() or nil) </TD></TR>
<TR><TD>SS_procedurep(x)</TD>
<TD>TRUE iff x is of type PROC_OBJ </TD></TR>
<TR><TD>SS_booleanp(x)</TD>
<TD>TRUE iff x is of type BOOLEAN (#t or #f) </TD></TR>
</TABLE>
</BLOCKQUOTE>

<P>
<P>

The following are available with the LARGE option (default):<p>

<BLOCKQUOTE>
<TABLE>
<TR><TD WIDTH="150">SS_hasharrp(x)</TD>
<TD>TRUE iff x is of type HASH_ARRAY </TD></TR>
<TR><TD>SS_haelemp(x)</TD>
<TD>TRUE iff x is of type HASH_ELEMENT </TD></TR>
<TR><TD>SS_charobjp(x)</TD>
<TD>TRUE iff x is of type CHAR_OBJ </TD></TR>
<TR><TD>SS_vectorp(x)</TD>
<TD>TRUE iff x is of type VECTOR </TD></TR>
<TR><TD>SS_processp(x)</TD>
<TD>TRUE iff x is of type PROCESS_OBJ </TD></TR>
</TABLE>
</BLOCKQUOTE>

<P>

<a name="540531"></a>
<h4> Memory Management</h4>

<BLOCKQUOTE>
<TABLE>
<TR><TD WIDTH="150">SS_GC(x)</TD>
<TD>decrement the reference count of x and release it if the count is zero </TD></TR>
<TR><TD>SS_mark(x)</TD>
<TD>increment the reference count of x </TD></TR>
<TR><TD>SS_Assign(x, val)</TD>
<TD>assign object x to object val making sure to get the reference counts right </TD></TR>
</TABLE>
</BLOCKQUOTE>

<P>

<a name="540587"></a>
<h4> Execution Control</h4>

<BLOCKQUOTE>
<TABLE>
<TR><TD WIDTH="150">SS_set_cont(_si, _go, _return)</TD>
<TD>setup a new continuation </TD></TR>
<TR><TD>SS_go_cont</TD>
<TD>go to the current continuation </TD></TR>
<TR><TD>SS_jump(x)</TD>
<TD>jump to the label x </TD></TR>
<TR><TD>SS_tracedp(x)</TD>
<TD>return the trace field in the procedure struct x </TD></TR>
</TABLE>
</BLOCKQUOTE>

<P>

<a name="540593"></a>
<h4> Stack Control</h4>

<BLOCKQUOTE>
<TABLE>
<TR><TD WIDTH="100">SS_Save(si, x)</TD>
<TD>save x on the stack </TD></TR>
<TR><TD>SS_Restore(x)</TD>
<TD>pop an object off the stack into x and release
the stack entry </TD></TR>
</TABLE>
</BLOCKQUOTE>

<P>

<a name="539909"></a>
<h3 ALIGN="CENTER"> Variables</h3>

<a name="539917"></a>
<h4> Strings</h4>

<BLOCKQUOTE>
<TABLE>
<TR><TD WIDTH="150">SS_OBJECT_S</TD>
<TD>string containing &#147;object&#148; </TD></TR>
<TR><TD>SS_POBJECT_S</TD>
<TD>string containing &#147;object *&#148; </TD></TR>
</TABLE>
</BLOCKQUOTE>

<P>

<a name="539903"></a>
<h4> Objects</h4>

<BLOCKQUOTE>
<TABLE>
<TR><TD WIDTH="150">SS_quoteproc</TD>
<TD>pointer to the quote procedure</TD></TR>
<TR><TD>si->indev</TD>
<TD>pointer to the standard input port object (stdin equivalent)</TD></TR>
<TR><TD>si->outdev</TD>
<TD>pointer to the standard output port object (stdout equivalent)</TD></TR>
<TR><TD>si->histdev</TD>
<TD>pointer to the history output port object (see transcript)</TD></TR>
<TR><TD>SS_null</TD>
<TD>pointer to the () object</TD></TR>
<TR><TD>SS_eof</TD>
<TD>pointer to the #eof object</TD></TR>
<TR><TD>SS_t</TD>
<TD>pointer to the #t object</TD></TR>
<TR><TD>SS_f</TD>
<TD>pointer to the #f object</TD></TR>
<TR><TD>SS_else</TD>
<TD>pointer to the else object </TD></TR>
</TABLE>
</BLOCKQUOTE>

<P>

<a name="539883"></a>
<h3 ALIGN="CENTER"> Handlers</h3>

The following functions are described in detail in the section on
interfacing compiled and interpreted code.<p>

<a name="539884"></a>
<h4> Function Handlers</h4>

<B></B>
<BLOCKQUOTE>
object *SS_zargs(SS_psides *si, C_procedure *cp, object *argl)
<P>
object *SS_sargs(object *argl)
<P>
object *SS_nargs(object *argl)
</BLOCKQUOTE>
</B>

<a name="SXvarhand"></a>
<h4> Variable Handlers</h4>

It is possible to give the following SCHEME level functional interpretation
to compiled code variables: suppose you have a variable x bound to the value
3 then:<p>

<BLOCKQUOTE>
(x) =&gt; 3
<P>(x 2) =&gt; 2
</BLOCKQUOTE>

and x is now bound to 2.  These handlers are supplied so that you may
install your application variables as implicit functions. They are used
with SS_install_cf.<p>

<B></B>
<BLOCKQUOTE>
object *SS_acc_double(C_procedure *cp, object *argl)
<P>
object *SS_acc_int(C_procedure *cp, object *argl)
<P>
object *SS_acc_long(C_procedure *cp, object *argl)
<P>
object *SS_acc_char(C_procedure *cp, object *argl)
<P>
object *SS_acc_string(C_procedure *cp, object *argl)
<P>
object *SS_acc_ptr(C_procedure *cp, object *argl)
</BLOCKQUOTE>
</B>

<a name="539906"></a>
<h3 ALIGN="CENTER"> Functions</h3>

The following functions are just some of the many functions in the PACT
SCHEME library. They are selected because they are the ones that developers
will use to integrate the interpreter into their applications. The remainder
of the functions declared in scheme.h are C level implementations of SCHEME
level functions and as such are only accessed by the interpreter.<p>

<a name="539908"></a>
<h4> Memory Management</h4>

These functions support the creation and destruction of SCHEME objects.
Although these handle the native PACT SCHEME data types, they also provide
some guidance for developers who wish to make their own data types known to
the interpreter.<p>

<B></B>
<BLOCKQUOTE>
object *SS_mk_variable(char *name, object *v)
<P>object *SS_mk_string(char *s)
<P>object *SS_mk_inport(FILE *str)
<P>object *SS_mk_outport(FILE *str)
<P>object *SS_mk_integer(long i)
<P>object *SS_mk_float(double d)
<P>object *SS_mk_cons(object *car, object *cdr)
<P>object *SS_mk_char(int i)
<P>object *SS_mk_vector(int l)
<P>object *SS_mk_hasharr(HASHTAB *tb)
<P>object *SS_mk_haelem(hashel *hp)
<P><BR>
<P>object *SS_mk_object(byte *p, int type, int ev_type, char *name)
</BLOCKQUOTE>
</B>

<B>SS_mk_object</B> creates and returns a new object of type type which has the</B>
pointer p as its value. The object type is taken from the list of defined
type indices some of which are discussed in the section
<a href="#540032">Object Type Constants</a>
above. Also specified are its evaluation type (ev_type) which is required and
its print name (name) if any. The evaluation type is one of the values from
the <a href="#SXevalcon">Evaluation Type Constants</a>
section above. All of the SS_mk_xxxx functions
above call <B>SS_mk_object</B> as the final step to wrap an object around whatever</B>
other data type they create and initialize.<p>

<B></B>
<BLOCKQUOTE>
void SS_rl_object(object *obj)
</BLOCKQUOTE>
</B>

<P><a name="540206"></a>
<h4> List Processors</h4>

These functions set the value of the car or cdr of the specified cons object.<p>

<B></B>
<BLOCKQUOTE>
object *SS_car(object *obj)
<P>object *SS_cdr(object *obj)
<P>object *SS_caar(object *obj)
<P>object *SS_cadr(object *obj)
<P>object *SS_cdar(object *obj)
<P>object *SS_cddr(object *obj)
<P>object *SS_caaar(object *obj)
<P>object *SS_caadr(object *obj)
<P>object *SS_cadar(object *obj)
<P>object *SS_caddr(object *obj)
<P>object *SS_cdaar(object *obj)
<P>object *SS_cdadr(object *obj)
<P>object *SS_cddar(object *obj)
<P>object *SS_cdddr(object *obj)
</BLOCKQUOTE>
</B>

<B></B>
<BLOCKQUOTE>
int SS_length(object *lst)
</BLOCKQUOTE>
</B>

Return the length of the given list.<p>

<a name="540269"></a>
<h4> Input/Output </h4>

These functions handle most of the I/O chores which straddle the
interpreter and the application.<p>

<B></B>
<BLOCKQUOTE>
void SS_unget_ch(int c, object *str)
<P>void SS_put_ch(int c, object *str)
<P>int SS_get_ch(object *str, int ign_ws)
</BLOCKQUOTE>
</B>

These functions are used analogously to getc, putc, and ungetc in the C
standard library. They are provided as the standard operators used by the
SCHEME reader.  Applications may define their own and attach them to the
relevant hooks instead of these.<p>

<B></B>
<BLOCKQUOTE>
void SS_wr_atm(object *obj, object *strm)
</BLOCKQUOTE>
</B>

Print an atomic object to the specified output port object.<p>

<B></B>
<BLOCKQUOTE>
object *SS_load(object *port)
</BLOCKQUOTE>
</B>

Read and evaluate the expressions in the specified input port (usually a file).<p>

<a name="540405"></a>
<h4> C/SCHEME Interface</h4>

These are the functions used to communicate between the interpreter and
compiled code. See the section on interfacing interpreted and compiled code
later in this manual.<p>

<B></B>
<BLOCKQUOTE>
object *SS_make_list(int first, ...)
</BLOCKQUOTE>
</B>

Given a list of type index, variable pointer pairs create and return a
SCHEME list. A pair with a type index of 0 terminates the list. The pointers
in the pairs must be pointers to the types indicated by the type index.
Thus SC_INT_I would be accompanied by and int *.<p>

<B></B>
<BLOCKQUOTE>
object *SS_call_scheme(char *func, ...)
</BLOCKQUOTE>
</B>

Call the function named by func with the arguments supplied. The arguments
are specified the same way that they are for <B>SS_make_list</B>.<p></B>

<B></B>
<BLOCKQUOTE>
int SS_args(object *s, ...)
</BLOCKQUOTE>
</B>

Extract arguments from the specified object which may be atomic or a list.
The objects to be extracted are specified as for
<B>SS_make_list</B> and <B>SS_call_scheme</B></B>
as type index, pointer pairs. A type index of zero terminates the list. The
number of items successfully extracted is returned. Only one thing can be
extracted from an atomic object. It is not required that the length of the
list match the number of specifications in the call. The extraction terminates
at the earlier of the exhaustion of the list s or the end of the specifications
for extraction.<p>

<B></B>
<BLOCKQUOTE>
int SS_run(char *s)
</BLOCKQUOTE>
</B>

Parse and evaluate a single expression represented by s. The integer
return value is decided based on SCHEME return value as follows:
<p>

            SCHEME             C
<pre>
          () | nill            0
          <boolean>   #f -> 0 and #t -> 1
          <integer>         <value>
</pre>
<p>

<B></B>
<BLOCKQUOTE>
int SS_load_scm(char *name)
</BLOCKQUOTE>
</B>

Open and load (in the sense of <B>SS_load</B>) the file specified by name.<p></B>

<a name="540346"></a>
<h4> Top Level</h4>

These functions are high level routines for initializing the interpreter and
entering into read-eval-print loops.<p>

<B></B>
<BLOCKQUOTE>
void SS_inst_prm(byte)
<P>void SS_repl(byte)
<P>void SS_end_scheme(int val)
<P>void SS_init_scheme(char *Code, char *Vers)
<P>void SS_init_path(byte)
<P>void SS_inst_const(byte)
<P>void SS_init_stack(byte)
<P>void SS_init_cont(byte)
<P>void SS_interrupt_handler(int sig)
</BLOCKQUOTE>
</B>

<a name="540432"></a>
<h4> Error Handling</h4>

These routines are for controlling error handling.<p>

<B></B>
<BLOCKQUOTE>
int SS_err_catch(PFInt func, PFInt errf)
<P>object *SS_pop_err(int n, int flag)
<P>void SS_push_err(int flag, int type)
<P>void SS_error(char *s, object *obj)
<P>PFPrintErrMsg SS_set_print_err_func(PFPrintErrMsg *fnc, int dflt)
<P>PFPrintErrMsg SS_get_print_err_func(void)
</BLOCKQUOTE>
</B>

<a name="539900"></a>
<h4> Helpers</h4>

<B></B>
<BLOCKQUOTE>
int SS_numberp(object *obj)
<P>char *SS_get_string(object *obj)
</BLOCKQUOTE>
</B>

<a name="540411"></a>
<h4> Registration Routines</h4>

These routines make C level objects known to the interpreter.<p>

<BLOCKQUOTE>
<TABLE>
<TR><TD><B>void SS_install(</B></TD>
<TD><B>char *name, char *doc,</B></TD></TR>
<TR><TD></TD><TD><B>PFPObject hand, PFPObject proc,</B></TD></TR>
<TR><TD></TD><TD><B>int type)</B></TD></TR>
</TABLE>
</BLOCKQUOTE>

Install a procedure in the system table under the name, name, and with
documentation, doc. The handler and procedure (hand and proc respectively)
must be previously declared. The procedure type is one of those given in
the <a href="#SXproccon">Procedure Type Constants</a> section above.<p>

<BLOCKQUOTE>
<TABLE>
<TR><TD><B>object *SS_install_cf(</B></TD>
<TD><B>char *name, char *doc,</B></TD></TR>
<TR><TD></TD><TD><B>PFPObject handler, ...)</B></TD></TR>
</TABLE>
</BLOCKQUOTE>

Install a variable in the system table under the name, name, and with
documentation, doc. The handler, hand, must be one of those given in the
<a href="#SXvarhand">Variable Handlers</a> section above.<p>

<B>
<BLOCKQUOTE>
object *SS_install_cv(char *name, byte *pval, int type)
</BLOCKQUOTE>
</B>

Install a variable in the system table to be used as a conventional SCHEME
variable. This is equivalent to doing (define name value) at the SCHEME
level. It simply lets you compile into the application any variables that
you wish to handle at the compiled and interpreted level.<p>

<a name="SSsynt"></a>
<h2 ALIGN="CENTER">Syntax Modes in SCHEME</h2>

The PACT SCHEME interpreter has the hooks to allow applications to
install alternate parsers.  The effect of this is to allow the interpreter
to interpret other languages.  The idea is to write a YACC-style grammar
and perhaps a LEX-style analyzer to parse an input language.  The grammar
actions in effect translate the input language into SCHEME for evaluation.
<p>
As stated before this can be used to create an interpreter for your favorite
language.  Alternatively it can be thought of as a means of translating your
(second) favorite language into SCHEME.  
<p>
To add a mode, you supply the appropriate parser and analyzer.  You supply
a function to switch the reader into the new syntax mode.  You can
register a file suffix to tell the interpreter to switch to your new
mode when loading source files with that suffix.
<p>
As an example of how this works a C syntax mode is supplied.  Other modes
can be patterned after C mode.
<p>

<a name="SScmode"></a>
<h3 ALIGN="CENTER">C Syntax Mode</h3>

C mode is based on the grammar of ANSI standard C.  It features all of the
syntax of ANSI standard C except:

<blockquote>
<strong>enum</strong>
<P>
type <strong>char</strong> ; although <strong>char *</strong> is supported
<P>
multi-dimensional arrays semantics (syntax is there)
<P>
<strong>a.b</strong> syntax ; although <strong>a->b</strong> is supported
<P><strong>&</strong> and <strong>*</strong> ; i.e. address of and dereference
</blockquote>
<p>
Also the C preprocessor directives are not included at this time.
<p>

<p>
Because C identifiers are more restricted that SCHEME identifiers, the
following conventions are used to map names of SCHEME functions into
a form which the C parser/analyzer will accept:

<center>
<table>
<tr>
<td><B>SCHEME</B></td></B>
<td width="50"></td>
<td align="center"><B>C</B></td></B>
<tr>
<td align="center">-</td>
<td width="50"></td>
<td align="center">_</td>
<tr>
<td align="center">*</td>
<td width="50"></td>
<td align="center">_</td>
<tr>
<td align="center">?</td>
<td width="50"></td>
<td align="center">p</td>
<tr>
<td align="center">!</td>
<td width="50"></td>
<td align="center">b</td>
<tr>
<td align="center">-></td>
<td width="50"></td>
<td align="center">_to_</td>
</table>
</center>
<p>

Files ending in <strong>.c</strong> or  <strong>.h</strong> are interpreted
in C mode.
<p>
Because SCHEME objects carry their own type information, the use of types
in C mode should be thought of more as a reminder to you than as a mandate
to the interpreter.  For example:

<BLOCKQUOTE>
<pre>
void foo(char *name)
   {void *x;
    int y;

    x = open_input_file(name);
    if (input_portp(x))
       close_input_file(x);

    return;}
</pre>
</BLOCKQUOTE>

and

<BLOCKQUOTE>
<pre>
void foo(int name)
   {int x;
    int y;

    x = open_input_file(name);
    if (input_portp(x))
       close_input_file(x);

    return;}
</pre>
</BLOCKQUOTE>

in spite of having wildly different types for the variables <em>name</em>
and <em>x</em> behave the same as the SCHEME coding

<BLOCKQUOTE>
<pre>
(define (foo name)
   (let* (x y)
         (set! x (open-input-file name))
	 (if (input-port? x)
	     (close-input-file x))))
</pre>
</BLOCKQUOTE>

The first version should be preferred because the coding more clearly
states in C terms the intentions for the data types.  That is,
<strong>char *name</strong> is a better way (in C at least) to think of
a character array than <strong>int name</strong>.
<p>
Notice also the function names in compliance with the convention stated above.
<p>
It is also possible to mix SCHEME and C syntax in a file.  There are two
ways to do this.  First by calling a function to set the parsing mode.  For
example:

<BLOCKQUOTE>
<pre>
(scheme-mode)

(define (foo x)
    (+ x 3))

(c-mode)

foo(2);

scheme_mode();

(foo 2)

(c-mode)

quit();

</pre>
</BLOCKQUOTE>

<p>
The second method of mixing SCHEME and C is special to the C mode.  The
SCHEME interpreter has been programmed to look for <strong>{</strong> and
switch to C mode until the corresponding <strong>}</strong>.  For example:

<BLOCKQUOTE>
<pre>
(define x 3)

(if (= x 3)
    {int y;
     y = x + 2;
     printf(nil, "%s\n", y);})
</pre>
</BLOCKQUOTE>

This is handy especially for doing complicated arithmetic expressions in
algebraic (infix) notation instead of Polish (prefix) notation.
<p>

Another detail about C mode pertains to strict adherence to the C standard.
We gave in to the temptation to use '^' as an infix operator for
exponentiation (i.e. pow).  The proper or strict C usage is bitwise
exclusive or.   There is a control to enforce strict C conformance:

<BLOCKQUOTE>
<pre>
(strict-c on)
</pre>
</BLOCKQUOTE>

to enforce strict C conformance or

<BLOCKQUOTE>
<pre>
(strict-c off)
</pre>
</BLOCKQUOTE>

to permit SX deviations from standard C.


<a name="SXscext"></a>
<h2 ALIGN="CENTER">   SCHEME + EXTENSIONS</h2>

The functions of SX are presented in groupings which correspond to the
PACT functionalities to which they interface. The reader is assumed to be
familiar with the functionality and structures of the part of PACT which
these functions cover. See the end of this manual for references to the
other <a href="#SXdocs">PACT documentation</a>.<p>

<a name="539004"></a>
<h3 ALIGN="CENTER"> SX Procedures</h3>

<a name="539005"></a>
<U><h4> Pure SX Functions</h4></U>

<B>memory-trace</B>
<BR><I>Procedure: </I>Return the number of allocated memory blocks.
<P>
<B>pp</B>
<BR><I>Procedure: </I>Print a list in nice even columns.
<P>

<a name="539012"></a>
<U><h4> PML Functions</h4></U>

<B>list-&gt;pm-array</B>
<BR><I>Procedure: </I>Return a numeric array built from a list of numbers.
<P>
<B>pm-array?</B>
</B><BR><I>C Information: </I>SX_NUMERIC_ARRAYP
<BR><I>Procedure: </I>Return #t if the object is a numeric array and #f otherwise.
<P>
<B>pm-array-&gt;list</B>
<BR><I>Procedure: </I>Return a list of numbers built from a numeric array.
<P>
<B>pm-array-length</B>
<BR><I>Procedure: </I>Return the length of the given numeric array.
<P>
<B>pdbdata-&gt;pm-mapping</B>
<BR><I>Procedure: </I>Read a PML mapping object from a PDB file.
<UL>
<I>Usage: </I>(pdbdata-&gt;pm-mapping file name)
</UL>
<P>
<B>pm-grotrian-mapping?</B>
<BR><I>Procedure: </I>Return #t if the object is a PML grotrian mapping, and #f otherwise.
<P>
<B>pm-make-mapping</B>
<BR><I>Procedure: </I>Return a PML mapping.
<UL>
<I>Usage: </I>(pm-make-mapping domain range [centering category name emap next])
</UL>
<P>
<B>pm-make-set</B>
<BR><I>Procedure: </I>Return a PML set.
<UL>
<I>Usage: </I>(pm-make-set name mesh-shape-list element-arrays)
</UL>
<P>
<B>pm-mapping?</B>
</B><BR><I>C Information: </I>SX_MAPPINGP
<BR><I>Procedure: </I>Return #t if the object is a PML mapping, and #f otherwise.
<P>
<B>pm-mapping-&gt;pdbdata</B>
<BR><I>Procedure: </I>Write a PML mapping object to a PDB file.
<UL>
<I>Usage: </I>(pm-mapping-&gt;pdbdata mapping file)
</UL>
<P>
<B>pm-mapping-dimension</B>
<BR><I>Procedure: </I>Return a pair containing the dimensionality of the
domain and range of the specified mapping.
<P>
<B>pm-mapping-domain</B>
<BR><I>Procedure: </I>Return the domain of the given mapping.
<P>
<B>pm-mapping-name</B>
<BR><I>Procedure: </I>Return the name of the given mapping.
<P>
<B>pm-mapping-range</B>
<BR><I>Procedure: </I>Return the range of the given mapping.
<P>
<B>pm-scale-domain</B>
<BR><I>Procedure: </I>Multiply all components of a mapping domain by a scalar value.
<P>
<B>pm-scale-range</B>
<BR><I>Procedure: </I>Multiply all components of a mapping range by a scalar value.
<P>
<B>pm-set?</B>
</B><BR><I>C Information: </I>SX_SETP
<BR><I>Procedure: </I>Return #t if the object is a PML set, and #f otherwise.
<P>
<B>pm-set-mapping-type</B>
<BR><I>Procedure: </I>Set the type of a mapping object to the given string.
<P>
<B>pm-set-volume</B>
<BR><I>Procedure: </I>Return the product of the extrema of the given set.
<P>
<B>pm-shift-domain</B>
<BR><I>Procedure: </I>Add a scalar value to all components of a mapping domain.
<P>
<B>pm-shift-range</B>
<BR><I>Procedure: </I>Add a scalar value to all components of a mapping range.
<P>

<a name="539076"></a>
<U><h4> Math Functions on Mappings</h4></U>

<B>+</B>
<BR><I>Procedure: </I>Sum of range values, i.e. a+b.
<UL>
<I>Usage: </I>+ a [b ...]
</UL>
<P>
<B>-</B>
<BR><I>Procedure: </I>Difference of range values, i.e. a-b. If there is
only one argument the result is unary negation.
<UL>
<I>Usage: </I>- a [b ...]
</UL>
<P>
<B>*</B>
<BR><I>Procedure: </I>Product of range values, i.e. a*b. If there is only one
argument this is an 
identity operation.
<UL>
<I>Usage: </I>* a [b ...]
</UL>
<P>
<B>/</B>
<BR><I>Procedure: </I>Quotient of range values, i.e. a/b. If there is only one argument
this takes the multiplicative inverse of the mapping.
<UL>
<I>Usage: </I>/ a [b ...]
</UL>
<P>
<B>abs</B>
<BR><I>Procedure: </I>Absolute value of range values.
<UL>
<I>Usage: </I>abs mappings
</UL>
<P>
<B>acos</B>
<BR><I>Procedure: </I>Arccos of range values.
<UL>
<I>Usage: </I>acos mappings
</UL>
<P>
<B>asin</B>
<BR><I>Procedure: </I>Arcsin of range values.
<UL>
<I>Usage: </I>asin mappings
</UL>
<P>
<B>atan</B>
<BR><I>Procedure: </I>Arctan of range values.
<UL>
<I>Usage: </I>atan mappings
</UL>
<P>
<B>cos</B>
<BR><I>Procedure: </I>Cosine of range values.
<UL>
<I>Usage: </I>cos mappings
</UL>
<P>
<B>cosh</B>
<BR><I>Procedure: </I>Hyperbolic cosine of range values.
<UL>
<I>Usage: </I>cosh mappings
</UL>
<P>
<B>exp</B>
<BR><I>Procedure: </I>Exponential of range values.
<UL>
<I>Usage: </I>exp mappings
</UL>
<P>
<B>integrate</B>
<BR><I>Procedure: </I>Compute the integrals of the given mappings.
<UL>
<I>Usage: </I>integrate mappings
</UL>
<P>
<B>j0</B>
<BR><I>Procedure: </I>Zeroth order Bessel function of the first kind on range values.
<UL>
<I>Usage: </I>j0 mappings
</UL>
<P>
<B>j1</B>
<BR><I>Procedure: </I>First order Bessel function of the first kind on range values.
<UL>
<I>Usage: </I>j1 mappings
</UL>
<P>
<B>jn</B>
<BR><I>Procedure: </I>Nth order Bessel function of the first kind on range values.
<UL>
<I>Usage: </I>jn mappings n
</UL>
<P>
<B>ln</B>
<BR><I>Procedure: </I>Natural log of range values.
<UL>
<I>Usage: </I>ln mappings
</UL>
<P>
<B>log10</B>
<BR><I>Procedure: </I>Base 10 log of range values.
<UL>
<I>Usage: </I>log10 mappings
</UL>
<P>
<B>norm</B>
<BR><I>Procedure: </I>Compute the euclidean norms of the given mappings.
<UL>
<I>Usage: </I>norm mappings
</UL>
<P>
<B>random</B>
<BR><I>Procedure: </I>Generate random range values between -1 and 1 for the mappings.
<UL>
<I>Usage: </I>random mappings
</UL>
<P>
<B>recip</B>
<BR><I>Procedure: </I>Reciprocal of range values.
<UL>
<I>Usage: </I>recip mappings
</UL>
<P>
<B>sin</B>
<BR><I>Procedure: </I>Sine of range values.
<UL>
<I>Usage: </I>sin mappings
</UL>
<P>
<B>sinh</B>
<BR><I>Procedure: </I>Hyperbolic sine of range values.
<UL>
<I>Usage: </I>sinh mappings
</UL>
<P>
<B>sqr</B>
<BR><I>Procedure: </I>Square of range values.
<UL>
<I>Usage: </I>sqr mappings
</UL>
<P>
<B>sqrt</B>
<BR><I>Procedure: </I>Square root of range values.
<UL>
<I>Usage: </I>sqrt mappings
</UL>
<P>
<B>tan</B>
<BR><I>Procedure: </I>Tangent of range values.
<UL>
<I>Usage: </I>tan mappings
</UL>
<P>
<B>tanh</B>
<BR><I>Procedure: </I>Hyperbolic tangent of range values.
<UL>
<I>Usage: </I>tanh mappings
</UL>
<P>
<B>y0</B>
<BR><I>Procedure: </I>Zeroth order Bessel function of the second kind on range values.
<UL>
<I>Usage: </I>y0 mappings
</UL>
<P>
<B>y1</B>
<BR><I>Procedure: </I>First order Bessel function of the second kind on range values.
<UL>
<I>Usage: </I>y1 mappings
</UL>
<P>
<B>yn</B>
<BR><I>Procedure: </I>Nth order Bessel function of the second kind on range values.
<UL>
<I>Usage: </I>yn mappings n
</UL>
<P>

<a name="539193"></a>
<U><h4> PDB Functions</h4></U>

The functions in this section are divided into two parts.  The first part
deals with PDB functionality somewhat in parallel to the PDB C API. The
second part describes functions that go beyond the PDB C API and allow
more arbitrary binary files to be accessed with lower levl PDB machinery.<p>

<CENTER><B>High Level API Functions</B></CENTER></B>

<B>change-directory</B>
<BR><I>Procedure: </I>Change the current PDB file directory.
<UL>
<I>Usage: </I>(change-directory file directory)
</UL>
<P>
<B>close-pdbfile</B>
<BR><I>Procedure: </I>Close a PDB file.
<UL>
<I>Usage: </I>(close-pdbfile file)
</UL>
<P>
<B>create-link</B>
<BR><I>Procedure: </I>Create a link to a variable in a PDB file.
<UL>
<I>Usage: </I>(create-link file variable-name link-name)
</UL>
<P>
<B>create-pdbfile</B>
<BR><I>Procedure: </I>Create and return a PDB file.
<UL>
<I>Usage: </I>(create-pdbfile expression)
</UL>
<P>
<B>current-directory</B>
<BR><I>Procedure: </I>Return the current directory in a PDB file.
<UL>
<I>Usage: </I>(current-directory file)
</UL>
<P>
<B>current-pdbfile</B>
<BR><I>Procedure: </I>Return the current default PDB file.
<P>
<B>def-common-types</B>
<BR><I>Procedure: </I>Define some common internal SX data types
to the given file.
<P>
<B>def-member</B>
<BR><I>Procedure: </I>Special Form: Create a member list.
<P>
<B>default-offset</B>
<BR><I>Procedure: </I>Set and/or return the default offset for the given file.
<P>
<B>defstr?</B>
</B><BR><I>C Information: </I>SX_DEFSTRP
<BR><I>Procedure: </I>Return #t if the object is a DEFSTR, and #f otherwise.
<P>
<B>desc-variable</B>
<BR><I>Procedure: </I>Describe a PDB variable.
<P>
<B>diff-variable</B>
<BR><I>Procedure: </I>Compare an entry in two files
<UL>
<I>Usage: </I>(diff-variable file<SUB>1</SUB> file<SUB>2</SUB>
name [comparison-precision [display-mode]]).
</UL>
<P>
<B>display-differences</B>
<BR><I>Procedure: </I>Display information returned by diff-variable.
<P>
<B>display-menu</B>
<BR><I>Procedure: </I>Display a menu of mappings, images, and curves.
<P>
<B>file-variable?</B>
<BR><I>Procedure: </I>Return #t if the variable is in the specified
file, and #f otherwise.
<UL>
<I>Usage: </I>(file-variable? file variable)
</UL>
<P>
<B>find-types</B>
<BR><I>Procedure: </I>Return a list of the derived data types in a variable.
<P>
<B>hash-&gt;pdbdata</B>
<BR><I>Procedure: </I>Convert a hash table to a PDBDATA object.
<P>
<B>indirection</B>
<BR><I>Procedure: </I>Create a type list.
<P>
<B>list-defstrs</B>
<BR><I>Procedure: </I>Return a list of the data types in a file.
<P>
<B>list-file</B>
<BR><I>Procedure: </I>Return a list of open pdbfiles.
<UL>
<I>Usage: </I>(list-file)
</UL>
<P>
<B>list-variables</B>
<BR><I>Procedure: </I>Return a list of the variables in a file directory.
<UL>
<I>Usage: </I>(list-variables file pattern type)
</UL>
<P>
<B>major-order</B>
<BR><I>Procedure: </I>Set and/or return the major order (row or column)
for the given file.
<P>
<B>make-defstr</B>
<BR><I>Procedure: </I>Special Form: Create a <B>DEFSTR</B> object</B>
from the list (macro version).
<P>
<B>make-defstr*</B>
<BR><I>Procedure: </I>Create a <B>DEFSTR</B> object from the</B>
list (procedure version).
<P>
<B>make-directory</B>
<BR><I>Procedure: </I>Create a directory in a PDB file.
<UL>
<I>Usage: </I>(make-directory file directory)
</UL>
<P>
<B>make-syment</B>
<BR><I>Procedure: </I>Create a <B>SYMENT</B> object from the list.</B>
<P>
<B>open-pdbfile</B>
<BR><I>Procedure: </I>Open and return a PDB file.
<UL>
<I>Usage: </I>(open-pdbfile expression)
</UL>
<P>
<B>pdb-&gt;list</B>
<BR><I>Procedure: </I>Convert some PDB type to a list.
<P>
<B>pdb-read-numeric-data</B>
<BR><I>Procedure: </I>Read a numeric array from a PDB file.
<P>
<B>pdbdata?</B>
</B><BR><I>C Information: </I>SX_PDBDATAP
<BR><I>Procedure: </I>Return #t if the object is a <B>PDBDATA</B>,</B>
and #f otherwise.
<P>
<B>pdbdata-&gt;hash</B>
<BR><I>Procedure: </I>Convert a <B>PDBDATA</B> object to a hash object.</B>
<P>
<B>pdbfile?</B>
<BR><I>Procedure: </I>Return #t if the object is a PDB file, and #f otherwise.
<P>
<B>set-format</B>
<BR><I>Procedure: </I>Set a format printing control value. These values
control the printed representation of numerical types.
<UL>
<I>Usage: </I>(set-format format value)
</UL>
<P>
<B>set-switch</B>
<BR><I>Procedure: </I>Set a switch which controls the behavior of SX.
The switches are:
<BLOCKQUOTE>
<B>0 --</B> structure printing prefix (0 -&gt; full path, 1 -&gt;</B>
space only, 2-&gt; current leaf)
<br>
<B>1 --</B> structure printing information (0 -&gt; print name only, 1-&gt; print name and </B>
type)
<br>
<B>2 --</B> control recursive indentation (0 -&gt; recurse and indent,</B>
1-&gt; print 
the recursion depth count but don&#146;t indent)
<br>
<B>3 --</B> maximum number items a </B>
variable can have before being printed in array format
<br>
<B>4 --</B> number of </B>
items printed on a line.
</BLOCKQUOTE>
<UL>
<I>Usage: </I>(set-switch switch value)
<P><I>Example: </I>(set-switch 3 10)
</UL>
<P>
<B>show-pdb</B>
<BR><I>Procedure: </I>Display the contents of a guest variable.
<P>
<B>syment?</B>
</B><BR><I>C Information: </I>SX_SYMENTP
<BR><I>Procedure: </I>Return #t if the object is a <B>SYMENT</B>,</B>
and #f otherwise.
<P>
<B>target</B>
<BR><I>Procedure: </I>Set and/or return the data standard and
alignment to be applied to  next file created.
<P>
<B>type</B>
<BR><I>Procedure: </I>Create a type list for <B>write-pdbdata</B>.</B>
<P>
<B>write-pdbdata</B>
<BR><I>Procedure: </I>Write PDB data to a file and encapsulate it
as a <B>PDBDATA</B> object.</B>
<P>
<B>write-ultra-curve</B>
<BR><I>Procedure: </I>Create an ULTRA curve.
<P>

<CENTER><B>Low Level Binary File Access Functions</B></CENTER></B>

<B>close-raw-binary-file</B>
<BR><I>Procedure: </I>Close a file opened with open-raw-binary-file.
<UL>
<I>Usage: </I>(close-raw-binary-file file)
<P><I>Example: </I>(close-raw-binary-file file-obj)
</UL>
<P>
<B>open-raw-binary-file</B>
<BR><I>Procedure: </I>Open an arbitrary file as a PDB file without a symbol
table and possesing a 
minimal structure chart. Subsequent data access calls will provide
the missing information. This file can be closed with
<B>close-raw-binary-file</B>.</B>
<UL>
<I>Usage: </I>(open-raw-binary-file name mode type
data_standard<SUB>i</SUB> data_alignment<SUB>i</SUB>)
<P><I>Example: </I>(open-raw-binary-file &#147;foo&#148; &#147;r&#148;
NETCDF 1 1)
</UL>
<P>
<B>read-binary</B>
<BR><I>Procedure: </I>Read arbitrary binary data from file using the
specified information and 
return it as a SCHEME object. This is a low level access mechanism. See 
<B>read-pdbdata</B> for the main high level PDB interface call.</B>
<UL>
<I>Usage: </I>(read-binary file address nitems file-type [mem-type])
<P><I>Example: </I>(read-binary data 10 1 &#147;integer&#148;)
<P><PRE>         (read-binary data 14 5 &#147;integer&#148; &#147;long&#148;)</PRE>
</UL>
<P>
<B>write-binary</B>
<BR><I>Procedure: </I>Write arbitrary binary data
encapsulated as a SCHEME object to a file 
using the specified information. This is a low level access mechanism. See 
<B>write-pdbdata</B> for the main high level PDB inteface call.</B>
<UL>
<I>Usage: </I>(write-binary file object address nitems file-type [mem-type])
<P><I>Example: </I>(write-binary data 3 10 1 &#147;integer&#148;)
</UL>

<a name="539305"></a>
<U><h4> PGS Functions</h4></U>

The majority of the PGS functions are documented in the PGS User&#146;s
Manual.  You are encouraged to read that manual since it explains not only
the SX bindings of the graphical functionality but the drawing model and
general approach used in PGS.<p>

The functions which are documented here are ones that have less to do with
the PGS API than the usage of graphical objects in the SX context.  For
example, the predicates for the PGS objects are irrelevant to the PGS API
but are an important part of using them in SX applications.<p>

<B>pdbcurve-&gt;pg-graph</B>
<BR><I>Procedure: </I>Read an ULTRA curve object from a PDB file.
<UL>
<I>Usage: </I>(pdbcurve-&gt;pg-graph file name)
</UL>
<P>
<B>pdbdata-&gt;pg-graph</B>
<BR><I>Procedure: </I>Read a PGS graph object from a PDB file.
<UL>
<I>Usage: </I>(pdbdata-&gt;pg-graph file name)
</UL>
<P>
<B>pdbdata-&gt;pg-image</B>
<BR><I>Procedure: </I>Read a PGS image object from a PDB file.
<UL>
<I>Usage: </I>(pdbdata-&gt;pg-image file name)
</UL>
<P>
<B>pg-curve-name</B>
<BR><I>Procedure: </I>Return the curve referenced by name or menu number.
<P>
<B>pg-def-graph-file</B>
<BR><I>Procedure: </I>Set up a PDB file to receive PGS graph objects.
<P>
<B>pg-device?</B>
</B><BR><I>C Information: </I>SX_DEVICEP
<BR><I>Procedure: </I>Return #t if the object is a PGS device, and #f otherwise.
<P>
<B>pg-device-attributes?</B>
</B><BR><I>C Information: </I>SX_DEV_ATTRIBUTESP
<BR><I>Procedure: </I>Return #t if the object is a set of PGS
device attributes, and #f otherwise.
<P>
<B>pg-graph?</B>
</B><BR><I>C Information: </I>SX_GRAPHP
<BR><I>Procedure: </I>Return #t if the object is a PGS graph, and #f otherwise.
<P>
<B>pg-graph-&gt;pdbcurve</B>
<BR><I>Procedure: </I>Write an ULTRA curve object to a PDB file.
<UL>
<I>Usage: </I>(pg-graph-&gt;pdbcurve curve file)
</UL>
<P>
<B>pg-graph-&gt;pdbdata</B>
<BR><I>Procedure: </I>Write a PGS graph object to a PDB file.
<UL>
<I>Usage: </I>(pg-graph-&gt;pdbdata graph file)
</UL>
<P>
<B>pg-grotrian-graph?</B>
<BR><I>Procedure: </I>Return #t if the object is a PGS grotrian
graph, and #f otherwise.
<P>
<B>pg-image?</B>
</B><BR><I>C Information: </I>SX_IMAGEP
<BR><I>Procedure: </I>Return #t if the object is a PGS image, and #f otherwise.
<P>
<B>pg-image-&gt;pdbdata</B>
<BR><I>Procedure: </I>Write a PGS image object to a PDB file.
<UL>
<I>Usage: </I>(pg-image-&gt;pdbdata image file)
</UL>
<P>
<B>pg-image-name</B>
<BR><I>Procedure: </I>Return the image referenced by name or menu number.
<P>
<B>pg-mapping-name</B>
<BR><I>Procedure: </I>Return the mapping referenced by name or menu number.

<P>

<a name="539360"></a>
<U><h4>PANACEA Functions</h4></U>

<B>pa-advance-name</B>
<BR><I>Procedure: </I>Advance the given file name according to the PANACEA
naming convention.  For a file, foo.xdd, where dd is a two digit base 36
number, the number dd is incremented.
<P>
<B>pa-advance-time</B>
<BR><I>Procedure: </I>Set the problem time and time step.
<P>
<B>pa-command</B>
<BR><I>Procedure: </I>Hand a command line off to PANACEA for processing
by the installed commands.
<P>
<B>pa-define-variable</B>
<BR><I>Procedure: </I>Define a new variable to the PANACEA database.
<P>
<B>pa-describe-entity</B>
<BR><I>Procedure: </I>Display a description of a PANACEA variable or package.
<P>
<B>pa-display</B>
<BR><I>Procedure: </I>Display a PANACEA object in nice form.
<P>
<B>pa-finish-simulation</B>
<BR><I>Procedure: </I>Gracefully conclude a numerical simulation.
<P>
<B>pa-init-simulation</B>
<BR><I>Procedure: </I>Initialize a numerical simulation.
<P>
<B>pa-install-commands</B>
<BR><I>Procedure: </I>Install the package generator commands so that
they may be invoked by subsquent operations.
<P>
<B>pa-intern-packages</B>
<BR><I>Procedure: </I>Return a list of variables which are bound
to the PANACEA packages.
<P>
<B>pa-iv-specification?</B>
</B><BR><I>C Information: </I>SX_IV_SPECIFICATIONP
<BR><I>Procedure: </I>Return #t if the object is a PANACEA initial
value specification, and #f otherwise.
<P>
<B>pa-package?</B>
</B><BR><I>C Information: </I>SX_PACKAGEP
<BR><I>Procedure: </I>Return #t if the object is a PANACEA package,
and #f otherwise.
<P>
<B>pa-package-name</B>
<BR><I>Procedure: </I>Return the name of the PANACEA package.
<P>
<B>pa-read-commands</B>
<BR><I>Procedure: </I>Read and process the user installed commands
in the named file.
<P>
<B>pa-read-state-file</B>
<BR><I>Procedure: </I>Read the named state file and do the specified
conversions.
<P>
<B>pa-run-package</B>
<BR><I>Procedure: </I>Execute the given package and return its time
step and controlling zone.
<P>
<B>pa-save-to-pp</B>
<BR><I>Procedure: </I>Save the data for the output requests from this cycle.
<P>
<B>pa-simulate</B>
<BR><I>Procedure: </I>Run a simulation from Ti to Tf.
<P>
<B>pa-source-variable?</B>
</B><BR><I>C Information: </I>SX_SOURCE_VARIABLEP
<BR><I>Procedure: </I>Return #t if the object is a PANACEA source
variable, and #f otherwise.
<P>
<B>pa-variable?</B>
</B><BR><I>C Information: </I>SX_PANVARP
<BR><I>Procedure: </I>Return #t if the object is a PANACEA variable,
and #f otherwise.
<P>
<B>pa-variable-&gt;numeric-array</B>
<BR><I>Procedure: </I>Save the data for the output requests from this cycle.
<P>
<B>pa-write-restart-dump</B>
<BR><I>Procedure: </I>Write a restart dump.
<P>

<a name="539458"></a>
<h3 ALIGN="CENTER"> Variables</h3>

The following variables provide SX level access to compiled PACT variables.<p>

<a name="539460"></a>
<h4> PANACEA Variables</h4>

The following variables are used primarily in conjunction with the definition
of PANACEA database variables. The following example illustrates their use:<p>

<B>
<BLOCKQUOTE>
(pa-define-variable &#147;foo&#148; &#147;double&#148; nil nil
<BLOCKQUOTE>scope runtime class optional attribute
<P>number-of-zones dimension
<P>cm per eV units)
</BLOCKQUOTE>
</BLOCKQUOTE>
</B>

<B>dimension
</B><BR>A string used to delimit the dimensions of variable definitions.
Its value is &#147;dimension&#148;. 
(Required)
<P><B>upper-lower
</B><BR>A string used to indicate that the following two parameters in a
dimension specification 
form a (min index, max index) pair. It&#146;s value is &#147;upper-lower&#148;.
<P><B>offset-number
</B><BR>A string used to indicate that the following two parameters in a
dimension specification 
form a (offset, number of elements) pair. It&#146;s value is &#147;offset-number&#148;.
<P><B>units
</B><BR>An integer value used to denote the end of the unit specifications.  Its value is -1.
<P><B>per
</B><BR>An integer value used to denote the end of the numerator unit specifications.
Its&#146;s value is -2.
<P><B>attribute
</B><BR>An integer value used to denote the end of the variable attribute specifications.
It&#146;s value is 102.
<P><CENTER><B>Scope variables</B></CENTER><B>scope
</B><BR>An integer used to denote that the next argument will specify the SCOPE attribute of 
the variable. It&#146;s value is 97.
<P><B>definition
</B><BR>Denotes the DEFN SCOPE for the variable. It&#146;s value is -1.
<P><B>restart
</B><BR>Denotes the RESTART SCOPE for the variable. It&#146;s value is -2.
<P><B>demand
</B><BR>Denotes the DMND SCOPE for the variable. It&#146;s value is -3.
<P><B>runtime
</B><BR>Denotes the RUNTIME SCOPE for the variable. It&#146;s value is -4.
<P><B>edit
</B><BR>Denotes the EDIT SCOPE for the variable. It&#146;s value is -5.
<P><B>scratch
</B><BR>Denotes the SCRATCH SCOPE for the variable. It&#146;s value is -6.
<P><CENTER><B>Class variables</B></CENTER><B>class
</B><BR>An integer used to denote that the next argument will specify the CLASS attribute of 
the variable. It&#146;s value is 98.
<P><B>required
</B><BR>Denotes the REQU CLASS for the variable. It&#146;s value is 1.
<P><B>optional
</B><BR>Denotes the OPTL CLASS for the variable. It&#146;s value is 2.
<P><B>pseudo
</B><BR>Denotes the PSEUDO CLASS for the variable. It&#146;s value is 3.
<P><CENTER><B>Persistence variables</B></CENTER><B>persist
</B><BR>An integer used to denote that the next argument will specify the PERSIST attribute of 
the variable. It&#146;s value is 99.
<P><B>release
</B><BR>Denotes the REL PERSIST for the variable. It&#146;s value is -10.
<P><B>keep
</B><BR>Denotes the KEEP PERSIST for the variable. It&#146;s value is -11.
<P><B>cache
</B><BR>Denotes the CACHE PERSIST for the variable. It&#146;s value is -12.
<P><CENTER><B>Centering variables</B></CENTER><B>center
</B><BR>An integer used to denote that the next argument will specify the CENTER attribute of 
the variable. It&#146;s value is 100.
<P><B>zone-centered
</B><BR>Denotes the Z_CENT CENTER for the variable. It&#146;s value is -1.
<P><B>node-centered
</B><BR>Denotes the N_CENT CENTER for the variable. It&#146;s value is -2.
<P><B>face-centered
</B><BR>Denotes the F_CENT CENTER for the variable. It&#146;s value is -3.
<P><B>edge-centered
</B><BR>Denotes the E_CENT CENTER for the variable. It&#146;s value is -4.
<P><B>uncentered
</B><BR>Denotes the U_CENT CENTER for the variable. It&#146;s value is -5.
<P><CENTER><B>Allocation variables</B></CENTER><B>allocation
</B><BR>An integer used to denote that the next argument will specify the ALLOCATION 
attribute of the variable. It&#146;s value is 101.
<P><B>static
</B><BR>Denotes the STATIC ALLOCATION for the variable. It&#146;s value is -100.
<P><B>dynamic
</B><BR>Denotes the DYNAMIC ALLOCATION for the variable. It&#146;s value is -101.
<P><CENTER><B>Units variables</B></CENTER>
<BR>The units variable here only reflect the default unit set of PANACEA.
Applications are responsible for defining their own units with a compiled
call such as:<p>

<B>
<UL>
SS_install_cv(&#147;foo-unit&#148;,&amp;foo_unit, SC_INT_I);
</UL>
</B>

<BR><B>radian
</B><BR>An integer denoting the index of the RAD unit. Connected to PA_radian.
<P><B>steradian
</B><BR>An integer denoting the index of the STER unit. Connected to PA_steradian.
<P><B>mole
</B><BR>An integer denoting the index of the MOLE unit. Connected to PA_mole.
<P><B>Q
</B><BR>An integer denoting the index of the Q unit. Connected to PA_electric_charge.
<P><B>cm
</B><BR>An integer denoting the index of the CM unit. Connected to PA_cm.
<P><B>sec
</B><BR>An integer denoting the index of the SEC unit. Connected to PA_sec.
<P><B>g
</B><BR>An integer denoting the index of the G unit. Connected to PA_gram.
<P><B>eV
</B><BR>An integer denoting the index of the EV unit. Connected to PA_eV.
<P><B>K
</B><BR>An integer denoting the index of the K unit. Connected to PA_kelvin.
<P><B>erg
</B><BR>An integer denoting the index of the ERG unit. Connected to PA_erg.
<P><B>cc
</B><BR>An integer denoting the index of the CC unit. Connected to PA_cc.

<BR>

<P>

<a name="539412"></a>
<h2 ALIGN="CENTER">  The SX Library</h2>

In this section we list some of the functions in the PACT SX library.
Although there are many functions here, only a relative handful are useful
to application developers wishing to integrate the interpreter into their
product. All of the relevant declarations are made in the header file sx.h
which should be #include&#146;d in your source code.<p>

<a name="540438"></a>
<h3 ALIGN="CENTER"> Constants</h3>

These #define&#146;d constants are part of the essential glue of SX. These
items by and large are the interface between the interpreter and the data
structures of the other parts of PACT. See the discussion in the section
<A HREF="#SXinterfacing">Interfacing Compiled and Interpreted Code</A>

<a name="540441"></a>
<h4> Object Type Constants</h4>

These constants define the type indices for various PACT objects. They are
used in conjunction with <B>SS_args, SS_call_scheme</B>,
and <B>SS_make_list</B>.  They
are also implicitly used by the C and SCHEME level predicate functions.
They are defined by the SX_object_type enumeration<p>

<BLOCKQUOTE>
<TABLE>
<TR><TD>G_FILE</TD><TD>SX generic file object type</TD></TR>
<TR><TD>G_DEFSTR</TD><TD>PDB defstr object type</TD></TR>
<TR><TD>G_SYMENT</TD><TD>PDB syment object type</TD></TR>
<TR><TD>G_DIMDES</TD><TD>PDB dimdes object type</TD></TR>
<TR><TD>G_MEMDES</TD><TD>PDB memdes object type</TD></TR>
<TR><TD>G_PDBDATA</TD><TD>SX pdbdata object type</TD></TR>
<TR><TD>G_PANVAR</TD><TD>PANACEA PA_variable object type</TD></TR>
<TR><TD>G_PACKAGE</TD><TD>PANACEA PA_package object type</TD></TR>
<TR><TD>G_SOURCE_VARIABLE</TD><TD>PANACEA PA_source_variable object type</TD></TR>
<TR><TD>G_IV_SPECIFICATION</TD><TD>PANACEA PA_iv_specification object type</TD></TR>
<TR><TD>G_PLOT_REQUEST</TD><TD>PANACEA PA_plot_request object type</TD></TR>
<TR><TD>G_PLT_CRV</TD><TD>PGS curve object type</TD></TR>
<TR><TD>G_GRAPH</TD><TD>PGS PG_graph object type</TD></TR>
<TR><TD>G_DEVICE</TD><TD>PGS PG_device object type</TD></TR>
<TR><TD>G_DEV_ATTRIBUTES</TD><TD>PGS PG_dev_attributes object type</TD></TR>
<TR><TD>G_NUM_ARRAY</TD><TD>PML C_array object type</TD></TR>
<TR><TD>G_MAPPING</TD><TD>PML PM_mapping object type</TD></TR>
<TR><TD>G_SET</TD><TD>PML PM_set object type</TD></TR>
<TR><TD>G_IMAGE</TD><TD>PGS PG_image object type</TD></TR>
<TR><TD>G_INTERFACE_OBJECT</TD><TD>PGS PG_interface_object object type</TD></TR>
</TABLE>
</BLOCKQUOTE>
<P>

<a name="540462"></a>
<h3> Macros</h3>

These macros facilitate interfacing with the interpreter and writing C
level routines to be used by the interpreter.<p>

<a name="540489"></a>
<h4> PDB Related Accessors</h4>

<BLOCKQUOTE>
<TABLE>
<TR><TD>FILE_FILE(type, x)</TD>
<TD>the file pointer cast to type * in the g_file object x</TD></TR>
<TR><TD>FILE_TYPE(x)</TD>
<TD>the type string from the g_file object x</TD></TR>
<TR><TD>FILE_EXT_TYPE(x)</TD>
<TD>the external type string from the g_file object x</TD></TR>
<TR><TD>FILE_NAME(x)</TD>
<TD>the name string from the g_file object x</TD></TR>
<TR><TD>FILE_STREAM(type, x)</TD>
<TD>the FILE * from the thing in the g_file object x</TD></TR>
<TR><TD></TD><TD></TD></TR>
<TR><TD>PDBDATA_NAME(x)</TD>
<TD>the name string of a pdbdata object x</TD></TR>
<TR><TD>PDBDATA_DATA(x)</TD>
<TD>the data pointer of a pdbdata object x</TD></TR>
<TR><TD>PDBDATA_EP(x)</TD>
<TD>the symbol table entry of a pdbdata object x</TD></TR>
<TR><TD>PDBDATA_FILE(x)</TD>
<TD>the PDBfile of a pdbdata object x</TD></TR>
</TABLE>
</BLOCKQUOTE>

<P>

<a name="540544"></a>
<h4> Predicates</h4>

<B>PDBLib Types</B>

<BLOCKQUOTE>
<TABLE>
<TR><TD>SX_DEFSTRP(x)</TD><TD>	TRUE iff x is a defstr object</TD></TR>
<TR><TD>SX_SYMENTP(x)</TD><TD>	TRUE iff x is a syment object</TD></TR>
<TR><TD>SX_PDBDATAP(x)</TD><TD>	TRUE iff x is a pdbdata object</TD></TR>
</TABLE>
</BLOCKQUOTE>

<B>PANACEA Types</B>
<BLOCKQUOTE>
<TABLE>
<TR><TD>SX_PANVARP(x)</TD>
<TD>TRUE iff x is a PA_variable object</TD></TR>
<TR><TD>SX_PACKAGEP(x)</TD>
<TD>TRUE iff x is a PA_package object</TD></TR>
<TR><TD>SX_SOURCE_VARIABLEP(x)</TD>
<TD>TRUE iff x is a PA_source_variabl</TD></TR>
<TR><TD>SX_IV_SPECIFICATIONP(x)</TD>
<TD>TRUE iff x is a PA_iv_specificatio</TD></TR>
<TR><TD>SX_PLOT_REQUESTP(x)</TD>
<TD>TRUE iff x is a PA_plot_reques</TD></TR>
</TABLE>
</BLOCKQUOTE>

<B>PGS Types</B>
<BLOCKQUOTE>
<TABLE>
<TR><TD>SX_GRAPHP(x)</TD>
<TD>TRUE iff x is a PG_graph object</TD></TR>
<TR><TD>SX_IMAGEP(x)</TD>
<TD>TRUE iff x is a PG_image object</TD></TR>
<TR><TD>SX_DEVICEP(x)</TD>
<TD>TRUE iff x is a PG_device object</TD></TR>
<TR><TD>SX_DEV_ATTRIBUTESP(x)</TD>
<TD>TRUE iff x is a PG_dev_attributes object</TD></TR>
</TABLE>
</BLOCKQUOTE>

<B>PML Types</B>
<BLOCKQUOTE>
<TABLE>
<TR><TD>SX_NUMERIC_ARRAYP(x)</TD><TD>TRUE iff x is a C_array object</TD></TR>
<TR><TD>SX_MAPPINGP(x)</TD><TD>TRUE iff x is a PM_mapping object</TD></TR>
<TR><TD>SX_SETP(x)</TD><TD>TRUE iff x is a PM_set object</TD></TR>
</TABLE>
</BLOCKQUOTE>

<P>

<a name="540582"></a>
<h3 ALIGN="CENTER"> Variables</h3>

<a name="540583"></a>
<h4> Integers</h4>

<BLOCKQUOTE>
<TABLE>
<TR><TD>SX_autorange</TD>
<TD>	flag controlling auto ranging of plots </TD></TR>
<TR><TD>SX_autodomain</TD>
<TD>	flag controlling auto domaining of plots </TD></TR>
<TR><TD>SX_autoplot</TD>
<TD>	flag controlling replotting </TD></TR>
<TR><TD>SX_background_color_white</TD>
<TD>	flag controlling white backgrounds for screens </TD></TR>
<TR><TD>SX_border_width</TD>
<TD>	window border width in pixels </TD></TR>
<TR><TD>SX_cgm_background_color</TD>
<TD>	flag controlling white backgrounds for CGM </TD></TR>
<TR><TD>SX_cgm_flag</TD>
<TD>	CGM flag </TD></TR>
<TR><TD>SX_default_color</TD>
<TD>	default color to use </TD></TR>
<TR><TD>SX_grid</TD>
<TD>	flag controlling grids on plots </TD></TR>
<TR><TD>SX_label_length</TD>
<TD>	number of label characters to print </TD></TR>
<TR><TD>SX_label_type_size</TD>
<TD>	size in points of label font </TD></TR>
<TR><TD>SX_lines_page</TD>
<TD>	number of lines per page in window </TD></TR>
<TR><TD>SX_plot_flag</TD>
<TD>	flag controlling plottting </TD></TR>
<TR><TD>SX_plot_type_size</TD>
<TD>	size in points of plot related fonts </TD></TR>
<TR><TD>SX_ps_flag</TD>
<TD>	PostScript flag </TD></TR>
<TR><TD>SX_render_1d_1d</TD>
<TD>	default 1d domain vs 1d range plot rendering </TD></TR>
<TR><TD>SX_render_2d_1d</TD>
<TD>	default 2d domain vs 1d range plot rendering </TD></TR>
<TR><TD>SX_render_2d_2d</TD>
<TD>	default 2d domain vs 2d range plot rendering </TD></TR>
<TR><TD>SX_show_mouse_location</TD>
<TD>	flag controlling display of mouse location </TD></TR>
<TR><TD>SX_squeeze_labels</TD>
<TD>	flag controlling whitespace compression in labels </TD></TR>
<TR><TD>SX_log_scale[id]</TD>
<TD>	flag controlling log scale in ID direction (1d-1d only) </TD></TR>
</TABLE>
</BLOCKQUOTE>

<P>

<a name="540584"></a>
<h4> REALs</h4>

<BLOCKQUOTE>
SX_botspace
<P>SX_label_space
<P>SX_leftspace
<P>SX_marker_orientation
<P>SX_phi
<P>SX_rightspace
<P>SX_show_mouse_location_x
<P>SX_show_mouse_location_y
<P>SX_theta
<P>SX_topspace
</BLOCKQUOTE>

<P>

<a name="540558"></a>
<h4> Strings</h4>

<BLOCKQUOTE>
SX_cgm_name
<P>SX_cgm_type
<P>SX_palette
<P>SX_plot_type_style
<P>SX_ps_name
<P>SX_ps_type
<P>SX_smooth_method
</BLOCKQUOTE>

<a name="540589"></a>
<h3 ALIGN="CENTER"> Functions</h3>

The following functions are just some of the many functions in the SX
library. They are declared in sx.h.<p>

<a name="540596"></a>
<U><h4> Input/Output </h4></U>

These functions augment the I/O capabilities of the basic interpeter.<p>

<B>
<BLOCKQUOTE>
void SX_parse(PFByte replot, PFPChar reproc)
</BLOCKQUOTE>
</B>

<P>
<a name="540372"></a>
<h4> Top Level</h4>
<BLOCKQUOTE>
void SX_init(char *code, char *vers)
</BLOCKQUOTE>


<BR>

Initialize the SX system. This includes all the initialization of the
interpreter done with <B>SS_init</B>.<p>

<BR>

<a name="SXinterfacing"></a>
<h1 ALIGN="CENTER"> Interfacing Compiled and Interpreted Code</h1>

In this section we will explain how to make your C (or Fortran) data and
routines available to the SCHEME or SX interpreter (referred to simply as the
interpreter from now on). We will also explain how to have your C or Fortran
routines (referred to as compiled routines or compiled code) call interpreted
functions (that is SCHEME code).<p>

This information will enable developers to build knowledge of their compiled
code into the interpreter so that they or their users can write interpreted
code that uses the full power of SCHEME. SX itself is an example of this
approach to development. SX is largely involved with defining the structures
of PACT to the interpreter and supplying routines to manipulate them. As a
result of this, PDBView is an SX program which manipulates binary data files
in more general terms than any fixed set of compiled routines could do.<p>

The other interesting capability which we will discuss is the ability to have
compiled routines call interpreted ones.  This component gives a complete
interface by which compiled and interpreted routines interact. Developers
then have maximal flexibility in using SCHEME or SX with their applications<p>

<a name="539606"></a>
<h2 ALIGN="CENTER"> Some PACT SCHEME Implementation Details</h2>

We present here a discussion of details of the implementation of the
interpreter which are necessary in order to be able to successfully
interface your applications with the interpreter.<p>

<a name="539790"></a>
<h3> Objects</h3>

All SCHEME objects carry certain information with themselves. In particular,
all SCHEME objects know: what their data type is; how they are to be evaluated;
how to print themselves; and how to release themselves. The object type is
defined in the header file scheme.h.<p>

With the exception of lists (actually CONS cells) and numbers all objects also
have a printing name.  This is the easiest way to handle the printing of objects
in that the print name can be set when the object is created and a single
printing method can be used for all objects with a non-NULL print name.<p>

<a name="539795"></a>
<h3> C Level Macros</h3>

The following C macros (#define&#146;d in scheme.h) define the application
access to SCHEME objects:<p>

<BLOCKQUOTE>
<B>SS_INQUIRE_OBJECT(x) 
</B><BR>Return a pointer to the SCHEME object known to the interpreter&#146;s symbol 
table by the name given in x.
<P><B>SS_OBJECT_GC(x)
</B><BR>Return the number of references to the given object.
<P><B>SS_UNCOLLECT(x)
</B><BR>Render the memory management facility unable to release the given object.
<P><B>SS_GET(type, x)
</B><BR>Return the value part of the given object (a pointer) cast to the
specified type.
<P><B>SS_OBJECT_TYPE(x)
</B><BR>Return the integer type value for the given object.
<P><B>SS_OBJECT_NAME(x)
</B><BR>Return the print name of the given object.
<P><B>SS_OBJECT(x)
</B><BR>Return a pointer to the value part of the given object.
<P><B>SS_OBJECT_ETYPE(x)
</B><BR>Return the integer evaluation type for the given object.
<P><B>SS_OBJECT_PRINT(x, strm)
</B><BR>Print the given object on the specified I/O stream. You should not have to use 
this since the interpreter will use it on your behalf
<P><B>SS_OBJECT_FREE(x)
</B><BR>Release the given object. You should not have to use this, the interpreter will 
invoke it when necessary.
</BLOCKQUOTE>

<P>

<a name="539791"></a>
<h3> Evaluation Types</h3>

Every SCHEME object has an evaluation type. This tells the evaluator how to
treat the object. There are three evaluation types (defined in scheme.h):<p>

<BLOCKQUOTE>
<B>SELF_EV
</B><BR>	Such objects evaluate to themselves (this is what most SCHEME objects do)
<P><B>VAR_EV
</B><BR>	Such objects are variables and evaluate to the value to which they are bound
<P><B>PROC_EV
</B><BR>	Such objects are evaluated as procedure calls (only the CONS should have this 
evaluation type!!!)
</BLOCKQUOTE>

<P>

<a name="539792"></a>
<h3> SCHEME Function Protocol</h3>

PACT SCHEME has a non-standard mechanism for specifying how the interpreter
will process the arguments to procedure calls and how the return value will
be processed. This property is assigned to primitive (i.e. C level) procedures
when they are installed in the SCHEME symbol table.  This permits one C level
procedure to be installed in more than one way (under different names) to
achieve different effects. The following constants are #define&#146;d in
scheme.h.<p>

<BLOCKQUOTE>
<TABLE>
<TR><TD>SS_PR_PROC</TD><TD>Evaluate the arguments to the call and do not re-evaluate the result of the call</TD></TR>
<TR><TD>SS_EE_MACRO</TD><TD>Evaluate the arguments to the call and re-evaluate the result of the call</TD></TR>
<TR><TD>SS_UR_MACRO</TD><TD>Do not evaluate the arguments to the call and do not re-evaluate the result of the call</TD></TR>
<TR><TD>SS_UE_MACRO</TD><TD>Do not evaluate the arguments to the call and re-evaluate the result of the call</TD></TR>
</TABLE>
</BLOCKQUOTE>

SCHEME procedures defined using <B>define</B> are SS_PR_PROC and macros defined
using <B>define-macro</B> are SS_UR_MACRO. <p>

To illustrate some of the protocol uses:

<BLOCKQUOTE>
<TABLE>
<TR><TD WIDTH="150"><B>read</B> and <B>pair?</B></TD>
<TD>are implemented as SS_PR_PROC</TD></TR>
<TR><TD><B>let</B> and <B>let*</B></TD>
<TD>as SS_UE_MACRO</TD></TR>
<TR><TD><B>lambda</B> and <B>quote</B></TD>
<TD>as SS_UR_MACRO</TD></TR>
<TR><TD><B>apply</B></TD>
<TD>as SS_EE_MACRO.</TD></TR>
</TABLE>
</BLOCKQUOTE>

A loose convention for functions installed as both macros and procedures is
for the procedure name to be the same as the macro name only with a trailing
&#145;*&#146; added. For example, foo and foo* would be the macro and procedure
versions of some functionality called foo. In applications involving user
interactions (SCHEME level read-eval-print loops) the macro form is generally
to be preferred in that users generally type simple expressions at command line
prompts and they generally don&#146;t want to deal with any more punctuation
than absolutely necessary.  By contrast, when users write their own functions
they generally want the procedure form. Since these functions are prepared
&#147;one time&#148; with a text editor, the extra syntax if any is tolerable.<p>

<a name="539833"></a>
<h3> Function Handlers</h3>

To factor out commonality among different functions, the interpreter calls
primitive procedures in a two step process. Each C level function is called
by a C level function called a handler.  It is the job of the handler to do
all of the work generic to the class of functions for which the handler may
be used. The function itself does the specific operations only. This approach
guarantees that all of the functions invoked by a handler are uniformly treated.
This lends a great deal of semantic consistency to applications. Application
developers may supply both handlers and functions thus giving them great
flexibility in designing their products.<p>

For example, consider the case of the operators +, -, *, /. Only one handler
is needed to process the argument lists which each of the operators would get.
Each of  the basic functions only need be written to operate on a pair of
numbers and return a numeric result.  It is the handler that performs the
looping over the argument list, carries out type checking; and accumlates
the result.<p>

PACT SCHEME supplies three very fundamental handlers:
<UL>
<B>SS_zargs</B> - which calls
functions that take no arguments
<P><B>SS_sargs</B> - which calls functions which take
a single argument
<P><B>SS_nargs</B> - which calls functions which take a list of
arguments.
</UL>
All handlers must return a pointer to an object. All handlers take
two arguments. First is a pointer to the basic function. The return type is
specified as pointer to an object. However, since the handler is responsible
for the actual value returned to the interpreter, the function can be cast to
any type which the handler wants to deal with. The second argument to a handler
is a SCHEME list of the arguments supplied.  There is always a list given to
the handler even if it is empty. How the handler processes that list and how
it invokes the function that is the first argument is entirely up to the
application developer!<p>

<a name="539788"></a>
<h2 ALIGN="CENTER"> Defining New Objects to the Interpreter</h2>

To fully exploit the PACT SCHEME interpreter as a user extensibility mechanism,
you should consider defining your own data structures (C structs) to the
interpreter so that they become (or can be treated as) true SCHEME objects.
Once this is done, the interpreter can operate on these objects in the ways
which you define, and your users can extend your application to match their
requirements.<p>

To define your data structures, supply the following code elements:<p>

<BLOCKQUOTE>
<B>
1. Define an index value for your type (start at 1000 or more)
<P>2. Define a C level predicate macro for this type (see ARRAYP below)
<P>3. Define any useful structure accessor macros (see ARRAY_TYPE and others 
below)
<P>4. Write three functions to: instantiate, release, and print the object.
<P>5. Write a function implementing the SCHEME level predicate (see arrayp below)
<P>6. Write any other functions and/or handlers to manipulate the object at the 
SCHEME level
<P>7. Install the functions from steps 5 and 6
<BR>(see install_array_functions below)
</B>
</BLOCKQUOTE>

<P>
You are now able to have the interpreter understand your data structures well
enough to perform whatever operations you have defined for them. By doing a
careful layering of functionality, it is possible to write a very basic and
thin interface between C and SCHEME level coding and develop more elaborate
functionality entirely in SCHEME.<p>

<a name="539625"></a>
<h3> Array Example</h3>

Here is an example illustrating the above procedure.  This is very close to
how SX handles the PML defined C_array structure (in fact this is a simplified
version of that code).<p>

<BLOCKQUOTE>
<PRE>#include &lt;sx.h&gt;

typedef struct s_array
   {long length;
    char *type;
    void *data; array;

/* this is the index from step #1 */
#define G_NUM_ARRAY         1000

/* this is the predicate mentioned in step #2 */
#define ARRAYP(obj)         (SS_OBJECT_TYPE(obj) == G_NUM_ARRAY)

/* these are macros suggested by step #3 */
#define ARRAY(obj)          SS_GET(array, obj)
#define ARRAY_TYPE(obj)     (SS_GET(array, obj)->type)
#define ARRAY_LENGTH(obj)   (SS_GET(array, obj)->length)
#define ARRAY_DATA(obj)     (SS_GET(array, obj)->data)

/*-----------------------------------------------------------------*/
/*-----------------------------------------------------------------*/

/* _WR_GNUM_ARRAY - print a g_num_array
 *                - this is the function invoked by SS_OBJECT_PRINT
 *                - this is part of step #4
 */

static void _wr_gnum_array(obj, strm)
   object *obj, *strm;
   {PRINT(SS_OUTSTREAM(strm), "&lt;ARRAY|%s&gt;", ARRAY_TYPE(obj));

    return;

/*-----------------------------------------------------------------*/
/*-----------------------------------------------------------------*/

/* _RL_GNUM_ARRAY - release a g_num_array
 *                - this is the function invoked by SS_OBJECT_RELEASE
 *                - this is part of step #4
 */

static void _rl_gnum_array(obj)
   object *obj;
   {array *arr;

    arr = ARRAY(obj);

    CFREE(arr->data);
    CFREE(arr);

    SS_rl_object(obj);

    return;

/*-----------------------------------------------------------------*/
/*-----------------------------------------------------------------*/

/* _MK_ARRAY - encapsulate an array as an object
 *           - this is part of step #4
 */

object *_mk_array(arr)
   array *arr;
   {object *op;

    op = SS_mk_object(arr, G_NUM_ARRAY, SELF_EV, arr->type);
    op->print   = _wr_gnum_array;
    op->release = _rl_gnum_array;;

    return(op);

/*-----------------------------------------------------------------*/
/*-----------------------------------------------------------------*/

/* ARRAYP - function version of ARRAYP macro
 *        - this is the function mentioned in step #5
 */

object *arrayp(obj)
   object *obj;
   {return(ARRAYP(obj) ? SS_t : SS_f);

/*-----------------------------------------------------------------*/
/*-----------------------------------------------------------------*/

/* MK_ARRAY - allocate and return a array
 *          - form: (make-array <type> <size> <bpi>)
 *          - this is a step #6 function
 */

static object *mk_array(argl)
   object *argl;
   {array *arr;
    long size, bpi;
    char *type, ltype[MAXLINE];

    type = NULL;
    size = 0L;
    SS_args(argl,
            SC_STRING_I, &type,
            SC_LONG_I, &size,
            SC_LONG_I, &bpi,
            0);

    arr = CMAKE(array);

    sprintf(ltype, "%s *", type);

    arr->type   = CSTRSAVE(ltype);
    arr->length = size;
    arr->data   = CMAKE_N(char, size*bpi);

    return(_mk_array(arr));

/*-----------------------------------------------------------------*/
/*-----------------------------------------------------------------*/
 
/* INSTALL_ARRAY_FUNCS - install some array extensions to SX
 *                     - this is step #7
 */
 
void install_array_funcs()
   {

/* install the fundamental array operations */
    SS_install("array?",
               "Return #t if the object is a numeric array, and #f otherwise",
               SS_sargs, arrayp, SS_PR_PROC);

    SS_install("make-array",
               "Allocate and return an array of the specified type and size",
               SS_nargs, mk_array, SS_PR_PROC);

/* suggestions for other SCHEME level operations with arrays */

#if 0
    SS_install("list->array",
               "Return a numeric array built from a list of numbers",
               SS_nargs, list_array, SS_PR_PROC);

    SS_install("array->list",
               "Return a list of numbers built from a numeric array",
               SS_sargs, array_list, SS_PR_PROC);

    SS_install("resize-array",
               "Reallocate the given array to the specified size",
               SS_nargs, resz_array, SS_PR_PROC);

    SS_install("array-ref",
               "Reference the nth element of an array",
               SS_nargs, array_ref, SS_PR_PROC);

    SS_install("array-set!",
               "Set the nth element of an array",
               SS_nargs, array_set, SS_PR_PROC);

    SS_install("array-length",
               "Return the length of the given numeric array",
               SS_sargs, num_arr_len, SS_PR_PROC);
#endif

    return;

/*-----------------------------------------------------------------*/
/*-----------------------------------------------------------------*/
</PRE>
</BLOCKQUOTE>

<a name="539607"></a>
<h2 ALIGN="CENTER"> Compiling and Loading</h2>

To compile your C programs you must use one or the other (not both) of the
following<p>

<B>
<BLOCKQUOTE>
#include &lt;scheme.h&gt;
<P>#include &lt;sx.h&gt;
</BLOCKQUOTE>
</B>

in the source files which deal with the interpreter.<p>

To link your application you must use the following libraries in the order
specified. For SCHEME only programs use:<p>

<B>
<BLOCKQUOTE>
-lscheme -lppc -lpdb -lpml -lscore [-lm ...]
</BLOCKQUOTE>
</B>

For SX programs use:<p>

<B>
-lsx -lscheme -lpanacea -lpgs [-lX11] -lppc -lpdb -lpml -lscore [-lm ...]
</B>

<P>

Although this is expressed as if for a UNIX linker, the order would be the
same for any system with a single pass linker. The items in [] are optional
or system dependent.<p>

Each system has different naming conventions for its libraries and the reader
is assumed to understand the appropriate naming conventions as well as knowing
how to tell the linker to find the installed PACT libraries on each system
that they use.<p>

<a name="540569"></a>
<h2 ALIGN="CENTER"> Calling Interpreted Routines from Compiled Routines</h2>

The only real trick to having a compiled routine call an interpreted one is to
map the compiled parameters or arguments into a representation which the
evaluator is able to use. Naturally, that means making a list of SCHEME objects
out of the arguments and applying the  interpreted function to them.  The
interpreted function can be specified by name so that part it easy. To make the
correct SCHEME objects (referred to simply as objects from now on - sorry but
the LISP folks co-opted that term long before the OOP folks) from the compiled
variables, you need only give the integer object type designation (see the
previous section) and a pointer to the variable for each argument. The function
<B>SS_call_scheme</B> does the rest. Specifically,
it builds a list of objects from the
supplied information, looks up the named procedure, applies it to the list of
arguments, and returns the value which the interpreted routine returns. The
important part to note here is that <B>SS_call_scheme</B>
returns a SCHEME object!
Note also the similarity between SS_call_scheme and <B>SS_args</B>.
These two functions
are complementary and realize the majority of the run time interface between
interpreted and compiled functions.<p>

Here is an example continuing the array example of the last section:<p>

<BLOCKQUOTE>
<PRE>    object *rv;
    char *type;
    int n, bpi;

/* invoke the SCHEME level function create-array
 * this is the same as (create-array "float" 10 4)
 */
    type = "float";
    n    = 10;
    bpi  = sizeof(float);
    rv = SS_call_scheme("create-array",
                        SC_STRING_I, type,
                        SC_INT_I, &n,
                        SC_INT_I, &bpi,
                        0);
                        .
                        .
                        .
    long length;
    char *ntype;
    double *data;

    length = ARRAY_LENGTH(rv);
    ntype  = ARRAY_TYPE(rv);
    data   = ARRAY_DATA(rv);
</PRE>
</BLOCKQUOTE>

The SCHEME level use of this example is limited only by the number of functions
defined so far to manipulate such objects. Here is a fragment of SCHEME code
showing their use:<p>

<BLOCKQUOTE>
<PRE> (let* ((x (make-array "double" 10 8)))

     (if (array? x)

          (printf nil "We have an array: %s\n" x)))
</PRE>
</BLOCKQUOTE>


If we assume the existence of the functions alluded to in install_array_funcs
we can show a more useful example:<p>

<BLOCKQUOTE>
<PRE>(let* ((x (make-array "double" 10 8)))

    (array-set! x 5 15.34)

    (examine x 5)

(define (examine x n)

   (cond ((and (pair? x) (< n (length x)))

          (printf nil "List element %d = %s\n" n (list-ref x n)))

         ((and (array? x) (< n (array-length x)))

          (printf nil "Array element %d = %s\n" n (array-ref x n)))))
</PRE>
</BLOCKQUOTE>

<p>

<br>

<a name="SXdocs"></a>
<h1 ALIGN="CENTER"> Related Documents</h1>

SX is one part of PACT, a set of tools for portable code development and
visualization. Users of SX may be interested in the other parts of PACT
especially since the various aspects of SX are derived from PACT. That is
the graphics is handled by PGS; and the binary file handling is done
by PDBLib.<p>

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
