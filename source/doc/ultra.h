TXT: ULTRA User's Manual
MOD: 10/27/2009

<CENTER>
<P>
<H1><font color="#ff0000">$TXT</font></H1>
<H2>A program for the presentation, manipulation and analysis of
one-dimensional data sets, i.e. (x,y) pairs</H2>
<H4>Stewart A. Brown</H4>
</CENTER>

<BLOCKQUOTE>

<P>
<HR>

<ul>
<li><a href="#Uintro">Introduction</a>

<li><a href="#Ututor">An ULTRA II Tutorial</a>

<li><a href="#Ubasics">Basics of Data Files and Accessing Them</a>
<ul>
<li><a href="#U44">ULTRA II Input Data Files</a>
<ul>
<li><a href="#U46">ASCII ULTRA File Format</a>
<li><a href="#U99">Binary ULTRA File Format</a>
</ul>
<li><a href="#U12">Getting Input Data into ULTRA II</a>
</ul>

<li><a href="#Ucurves">ULTRA Curves</a>

<li><a href="#Unotes">ULTRA II Notes</a>

<li><a href="#Ucoms">Ultra Commands</a>
<ul>
<li><a href="#U88">I/O Commands</a>
<li><a href="#U13">Math Operations Which Do Not Generate a New Curve</a>
<li><a href="#U97">Math Operations Which Do Generate a New Curve</a>
<li><a href="#U30">Environmental Inquiry Commands</a>
<li><a href="#U34">Curve Inquiry Commands</a>
<li><a href="#U36">Environmental Control Commands</a>
<li><a href="#U38">Plot Control Commands</a>
<li><a href="#U41">Curve Control Commands</a>
<li><a href="#U45">Commands Useful for Writing Extensions</a>
</ul>

<li><a href="#Uvars">ULTRA Variables</a>

<li><a href="#Udflt">The Default ULTRA II Environment</a>
<ul>
<li><a href="#U64">Constants and Values</a>
<li><a href="#U66">Functions</a>
<li><a href="#U68">Synonyms</a>
<li><a href="#U69">Display and I/O Environment</a>
</ul>

<li><a href="#Ugraph">Graphical Output in ULTRA II</a>
<ul>
<li><a href="#U70">PostScript</a>
<li><a href="#U71">CGM</a>
</ul>

<li><a href="#Uextend">Extending ULTRA II</a>
<ul>
<li><a href="#U74">Cookbook Examples</a>
<ul>
<li><a href="#U75">Gaussian Curve Generator</a>
<li><a href="#U76">First Order Differential Equation Solver</a>
</ul>
</ul>

<li><a href="#Uinst">Configuration/Availability</a>
<ul>
<li><a href="#U77">UNIX</a>
</ul>

<li><a href="#Udocs1">Internal Documentation</a>

<li><a href="#Udocs2">Related Documents</a>
</ul>

<HR>

<a name="Uintro"></a>
<h1 ALIGN="CENTER">Introduction</h1>

ULTRA II is a program for the presentation, manipulation, and analysis of
1D data sets, i.e. (x,y) pairs. Presentation refers to the capability to
display, and make hard copies of data plots. Manipulation refers to the
capability to excerpt, shift, and scale data sets. Analysis refers to the
capability to combine data sets in various ways to create new data sets.
An example of this is the Fast Fourier Transform (FFT) capability in
ULTRA II.<p>

The principal object with which ULTRA II works is the curve. A curve is
an object which consists of an array of x values, an array of y values,
a number of points (the length of the x and y arrays), and an ASCII
label. ULTRA II operates on curves.<p>

ULTRA II is a portable tool. It runs on machines from supercomputers to PCs.
Its portability derives from the PACT
libraries which provide the portable graphics and the SCHEME interpreter
engine which are the main foundation of ULTRA II.<p>

ULTRA II can read and write ASCII data files or PDB files with ULTRA curves
in them. PDBLib is another PACT tool which provides portable,
self-describing, binary data files. PDBLib has specific functionality to
write ULTRA curves. This provides an easy and convenient way for
applications to produce ULTRA files. The advantage of PDB files over ASCII
files is that PDB files are about 1/3 the size of a corresponding ASCII
file and can be written or read about 10 times faster. Interested readers
will find references to PACT documentation later in this manual.<p>

Before plunging into descriptions of the commands and variables for
ULTRA II, a brief tutorial is given.<p>

<a name="Ututor"></a>
<h1 ALIGN="CENTER"> An ULTRA II Tutorial</h1>

This section gives a tutorial introduction to ULTRA II. A sample session
is run which highlights the basic ULTRA commands. <p>

NOTE: In ULTRA commands, spaces are used to delimit items on the input
line. More precisely items on a command line are either space delimited,
are preceded by a left parenthesis if the first item in a list, or
terminated by a right parenthesis if the last item in a list. Semicolons
may be used to stack multiple commands on a single interactive input line.
In interactive mode, ranges of curve numbers or data-id&#146;s may be
indicated using colon notation. For example,<p>

<BLOCKQUOTE>
a:f
</BLOCKQUOTE>

or<p>

<BLOCKQUOTE>
5:9
</BLOCKQUOTE>

This notation is shorthand for the thru procedure.<p>

To start up ULTRA II either type &#147;ultra&#148; at a command line prompt
or double click on the ULTRA II application icon. ULTRA II will start up
and print a banner. (Before using ULTRA II on UNIX systems, you must define
an ULTRA environment variable. This variable tells ULTRA II to look in the
specified directory or directories for various configuration and extension
files. See the <a href="#Uinst">Installation/Availability section</a> for
details.)<p>

<BLOCKQUOTE>
ULTRA II - 11.22.91
</BLOCKQUOTE>

<P>

<B>Create a curve consisting of a straight line y=x over the interval (0, 6.28).
</B>
<BR>
<UL> U-&gt; span 0 6.28 </UL>

<B>Print a list of curves on the display.</B>
<BR>
<UL> U-&gt; lst 
<BR>
<PRE>
A Straight line               0.00e+00   6.28e+00  0.00e+00   6.28e+00 
</PRE>
</UL>

<B>Take the sin of curve A.</B>
<BR>
<UL> U-&gt; sin a</UL>

<B>Similarly make a cosine curve. Note that this form doesn&#146;t require
you to know that curve B is the one being passed to the cos function.
</B>
<BR>
<UL> U-&gt; cos (span 0 6.28)</UL>

<B>Take the product of the curves A and B</B>
<BR>
<UL> U-&gt; * a b</UL>

<B>List the curves currently displayed.</B>
<BR>
<UL>
U-&gt; lst
<BR>
<PRE>
A Straight line               0.00e+00   6.28e+00 -1.00e+00   1.00e+00 

B Straight line               0.00e+00   6.28e+00 -1.00e+00   1.00e+00 

C * A B                       0.00e+00   6.28e+00 -5.00e-01   5.00e-01 
</PRE>
</UL>

<B>Write all of these curves to a file called foo.pdb which
is in PDB format.</B>
<BR>
<UL> U-&gt; save foo.pdb a:c </UL>

<B>Erase all of the curves currently displayed.</B>
<BR>
<UL> U-&gt; era </UL>

<B>List the curves currently displayed.</B>
<BR>
<UL> U-&gt; lst </UL>

<B>Read the file foo.pdb in.</B>
<BR>
<UL> U-&gt; rd foo.pdb </UL>

<B>Display the menu of curves in all open files.</B>
<BR>
<UL>
<PRE>
 U-&gt; menu

      MENU      ULTRA II - 11.22.91
 
 1 Straight line       0.00e+00   6.28e+00 -1.00e+00   1.00e+00 foo.pdb

 2 Straight line       0.00e+00   6.28e+00 -1.00e+00   1.00e+00 foo.pdb

 3 * A B               0.00e+00   6.28e+00 -5.00e-01   5.00e-01 foo.pdb
</PRE>
</UL>

<B>Select curves number 1 and 3 from the menu to display on the screen.</B>
<BR>
<UL> U-&gt; cur 1 3 </UL>

<B>List the curves currently displayed.</B>
<BR>
<UL>
<PRE>
 U-&gt; lst

 A Straight line       0.00e+00   6.28e+00 -1.00e+00   1.00e+00 foo.pdb

 B * A B               0.00e+00   6.28e+00 -5.00e-01   5.00e-01 foo.pdb
</PRE>
</UL>

<B>Shift curve A by one unit to the right.</B>
<BR>
<UL> U-&gt; dx a 1</UL>

<B>Delete curve B from the display.</B>
<BR>
<UL> U-&gt; del b </UL>

<B>Ask ULTRA II about FFT functionality</B>
<BR>
<UL>
<PRE>
 U-&gt; apropos fft
 
      APROPOS      ULTRA II - 11.22.91
 

 Apropos: fft
 

 cfft :

 Procedure: Compute Complex FFT. Return real and imaginary parts.

 Usage: cfft &lt;real-curve&gt; &lt;imaginary-curve&gt;
 

 ifft :
 Procedure: Compute Inverse FFT. Return real and imaginary parts.
 Usage: ifft &lt;real-curve&gt; &lt;imaginary-curve&gt;
 

 fft :

 Procedure: Compute Fast Fourier Transform of real curve. Return real 
            and imaginary parts.

 Usage: fft &lt;curve&gt;

</PRE>
</UL>

<B>Exit ULTRA II.</B>
<BR>
<UL> U-&gt; end </UL>

Hopefully this tutorial introduction gave you an orientation which will
make the discussions of ULTRA functionality in the following sections
clearer.<p>

<a name="Ubasics"></a>
<h1 ALIGN="CENTER"> Basics of Data Files and Accessing Them</h1>

In this section a discussion of input for ULTRA II and a general overview of getting it into a session are given.<p>

<a name="U44"></a>
<h2> ULTRA II Input Data Files</h2>

ULTRA II accepts two kinds of input files: ASCII and binary ULTRA files.<p>

<a name="U46"></a>
<h3>ASCII ULTRA File Format</h3>

There are two kinds of ASCII files which ULTRA II can understand. The
first contains curve data and the second contains tables which require
additional user input in order to obtain curves.<p>

<BR><B>Curves</B>
<P>
In ASCII files, data is stored as space delimited X,Y pairs. These pairs
may be on individual lines or there may be multiple pairs per line.
Identifiers for the curves begin the line with a # which is followed by
whatever identification label you like. For example:<p>

<PRE>
          # data set 1

          0.0 100.0

          1.0 200.0

          2.0 250.0

          3.0 275.0

          4.0 287.5


          #data set 2

          0.0 100.0    1.0 200.0   2.0 250.0

          3.0 275.0    4.0 287.5

</PRE>

This file has two curves which incidentally contain the same data.<p>

<BR
><B>Tables</B>
<P>
Tables of ASCII data are blocks of tabular data which conform to the
following rules:<p>

<ol>
<li>table rows must contain one or more numbers.
<P>
<li>all rows in a given table must have the same number of items.
<P>
<li>the first item in all rows of a given table may optionally be a
row heading.Tables are accessed in ULTRA II via the read-table and
table-curve commands.<p>
The following example illustrates a file with three tables:<p>
<p>

<PRE>

 indx    pos         vel       rho

   5 1.6313e+00  1.872e-01 2.8419e+00

   4 1.5002e+00  1.644e-01 2.9671e+00

   3 1.3487e+00  1.475e-01 3.2012e+00

   2 1.1692e+00  1.447e-01 3.4873e+00

   1 9.3365e-01  1.487e-01 3.3681e+00


c  random comments

c  just stuff


    id       1         2         3         4         5

    ab   5.774e+15 5.187e+15 1.469e+14 1.549e+14 1.154e+14

    cd   8.652e+16 7.627e+16 2.514e+15 2.870e+15 2.338e+15

    ef   6.529e+16 5.507e+16 1.753e+15 1.943e+15 1.525e+15

    xy   9.978e+16 9.009e+16 2.855e+15 3.164e+15 2.508e+15

    xx   7.537e+16 8.296e+16 2.579e+15 2.775e+15 2.227e+15

    lb   7.061e+16 1.123e+15 3.051e+15 2.837e+15 2.207e+15

    rf   1.204e+15 2.894e+15 6.902e+15 5.295e+15 3.738e+15


c some other data


    1   5.854 1.900 1.684 2.267

    2   1.669 4.895 4.182 5.647

    3   1.138 3.060 2.533 3.430

    4   1.910 5.190 4.192 5.621

    5   1.616 4.603 3.501 4.824

    6   1.346 4.426 3.028 3.883

    7   1.543 6.971 3.883 4.279

    8   2.545 2.180 1.021 7.488

    9   2.852 4.150 2.047 1.093


</PRE>

<p>

</ol>

<a name="U99"></a>
<h3>Binary ULTRA File Format</h3>

ULTRA uses PDBLib (another PACT tool) to read and write portable binary
data files. It also uses PDBLib&#146;s capabilities to organize the binary
curve data in a particular way so that it is correct to speak of a binary
ULTRA file instead of a generic PDB file. PDBLib provides functions for
both C and FORTRAN programs to write ULTRA curves into a PDB file.<p>

For <B>C programs</B> the functions are:<p>

<B>PD_wrt_pdb_curve</B>(PDBfile *fp, char *labl, int n, double *px,
double *py, int icurve)
<P>
The arguments are: fp, a pointer to the PDBfile structure corresponding to
the file into which the curve is to be written; labl, the ASCII string
containing the curve label; n, the number of points in the curve; px, the
array of x values; py, the array of y values; and icurve a counter which
must start at 0 and count the number of curves in the file. NOTE: there
may be several files into which curves are to be written and in each one
the count must start from 0 and proceed to the maximum number of curves
in the file.<p>

<B>PD_wrt_pdb_curve_y</B>(PDBfile *fp, char *labl, int n, int ix, double *py, int icurve)
<P>
The arguments are: fp, a pointer to the PDBfile structure corresponding to
the file into which the curve is to be written; labl, the ASCII string
containing the curve label; n, the number of points in the curve; ix, the
curve index for the x values; py, the array of y values; and icurve a
counter which must start at 0 and count the number of curves in the file.
The curve index for the x values refers to the icurve value for a prior
PD_wrt_pdb_curve call in which the desired x values were written out to
the file. This function is supplied to avoid having unnecessary duplication
of x data in a file (the space savings can be quite large). NOTE: there may
be several files into which curves are to be written and in each one the
count must start from 0 and proceed to the maximum number of curves in the
file.<p>

For <B>FORTRAN programs</B> the function to use is:<p>

<B>PFWULC</B>(fileid, nchr, labl, n, px, py, icurve)
<P>
The arguments are: fileid, an integer id for the PDB file into which the
curve is to be written; nchr; an integer number of characters in the label;
labl, a left justified ASCII character string containing nchr meaningful
characters; n, an integer number of points in the curve; px the array of
x values (the type should be REAL*8); py the array of y values (the type
should be REAL*8); and icurve, an integer counter for the number of curves
in the file. Unlike the C version this function increments icurve and
returns its new value through the argument list. Also see the note for
the C version above.<p>

<B>PFWULY</B>(fileid, nchr, labl, n, ix, py, icurve)
<P>
The arguments are: fileid, an integer id for the PDB file into which the
curve is to be written; nchr; an integer number of characters in the label;
labl, a left justified ASCII character string containing nchr meaningful
characters; n, an integer number of points in the curve; ix the curve index
for the x values py the array of y values (the type should be REAL*8); and
icurve, an integer counter for the number of curves in the file. The curve
index for the x values refers to the icurve value for a prior PFWULC call
in which the desired x values were written out to the file. This function
is supplied to avoid having unnecessary duplication of x data in a file
(the space savings can be quite large). NOTE: there may be several files
into which curves are to be written and in each one the count must start
from 0 and proceed to the maximum number of curves in the file. Unlike
the C version this function increments icurve and returns its new value
through the argument list. <p>

The reader should obtain and read a copy of the PDBLib User&#146;s Manual
to fully understand how to prepare a binary ULTRA file.<p>

<a name="U12"></a>
<h2>Getting Input Data into ULTRA II</h2>

A data file can be read by placing it on the command line. For example<p>

<UL> ultra test.dat </UL>

will read the file &#147;test.dat&#148; when ultra starts. Once in the
program, if you need data from another file, it can be read by the command<p>

<UL> rd file-name </UL>

where you insert your file name in place of file-name.<p>

<BR>

To see the menu of curves which are currently available to be plotted type<p>

<UL> menu</UL>

or if you have lots of curves you can have the menu sent to a text file
by typing<p>

<UL> print-menu file-name </UL>

The file file-name can then be sent to a printer.<p>

The data is plotted by selecting curves from the menu by number. To look
at curves 1 and 3 you would type <p>

<UL> select 1 3 </UL>

If you want to look at curves 1 through 4 inclusive, you would type<p>

<UL> select 1:4 </UL>

These two types of notation can be mixed to select any combination of
curves from the menu for plotting. <p>

The list of curves in a file is referred to as the curve menu or menu.
The menu can be viewed at any time by typing <p>

<UL> menu </UL>

Similarly, the list of displayed curves is referred to as the curve list
or list. The list can be seen by typing<p>

<UL> lst </UL>

The numbers shown to the right of the label are the number of points in
the curve, the minimum x, maximum x, the minimum y, and the maximum y.
It is sometimes useful to know these values.<p>

<a name="Ucurves"></a>
<h1 ALIGN="CENTER">ULTRA Curves</h1>

In the previous sections we have seen how to get data into ULTRA II
and had a brief glimpse of an ULTRA II session. Before proceeding further,
it will be useful to have a brief discussion of how ULTRA II works with
its data.<p>

The essential data object in ULTRA II is the curve. The aim of
almost every input operation is to get data from somewhere (a file or the
console) put into the form of a curve. A curve is a structure whose
principal members are an array of x values and an array of y
values. Internally ULTRA II can support an arbitrary number of curves
(limited only by the memory of your machine and disk). Once curves
are displayed they are referred to by a single alphabetic character.
This limits one to having only 26 curves visible on the display at a
time. This limitation may be removed in future versions of ULTRA II.<p>

ULTRA II defines 26 variables whose names are the single alphabetic
characters. It reserves these to use in referring to curves. These
variables should NOT be used in your own functions when extending
ULTRA II. In addition, the following rules apply:<p>

<p>

<ul>
<li>on input &#147;a&#148; and &#147;A&#148; are equivalent
<P>
<li>on output ULTRA prints &#147;a&#148; if there is <BR> no curve associated with the variable a
<P>
<li>on output ULTRA prints &#147;A&#148; if there is <BR> a curve associated with the variable a
</ul>

<p>
The excerpt from a session illustrates the above points.<p>

<BLOCKQUOTE>
U-&gt; span 0 1
<P>
U-&gt; printf nil &#147;%s\n&#148; (curve? a)
<P>
#t
<P>
U-&gt; printf nil &#147;%s\n&#148; (curve? A)
<P>
#t
<P>
U-&gt; printf nil &#147;%s\n&#148; (curve? b)
<P>
#f
<P>
U-&gt; printf nil &#147;%s\n&#148; (curve? B)
<P>
#f
<P>
U-&gt; printf nil &#147;%s\n&#148; a
<P>
A
<P>
U-&gt; printf nil &#147;%s\n&#148; b
<P>
b
</BLOCKQUOTE>

Most readers do not need to be concerned with this fine point of ULTRA
II. They may wish to come back to it in the event that they want to write
some functions to extend ULTRA II. The reason for bringing it up at this
point is that some readers will have noticed ULTRA II&#146;s behavior and
be wondering about it.<p>

Curves in ULTRA II behave like functions. They take a single numeric
argument and return a numeric value. Because of the treatment of curves
discussed above it is very easy to denote this sort of operation as the
following examples show:<p>

<PRE>             U-> sin (span 0 6.28)

             U-> a 1.57

             Curve A

                 1.57      0.999622

             U-> printf nil "%6f\n" (a 1.57)

             Curve A

                 1.57      0.999622

             0.999622

             U-> printf nil "%6f\n" (sin 1.57)

             1.000000
</PRE>


As the above example shows, curves do get one bit of exceptional treatment
as far as functions go in ULTRA II. When you ask for the value of a curve
at a certain point from the console, ULTRA II explicitly prints the result.
In your own procedures this behavior can be modified so that the extra
message is suppressed in which case the evaluation of a curve at a point
becomes indistinguishable from any other function call.<p>

NOTE: the printf function is a SCHEME level function which is used here
only for illustrating the points to be made as concisely as possible.
If you are using ULTRA II as is and are not writing your own function,
you will not need to worry about it. If you are writing your own
procedures, you should obtain a copy of the SX User&#146;s Manual which
documents all of the SCHEME functions.<p>

Curve labels as seen by the lst command are too short to record the
history of a curve. A curve may be read in or created, and then it may
be operated on by such functions as dx in arbitrary ways. Even a long
label may not be able to contain a sensible description of all of the
operations which have been performed on a curve. In acknowledgment of
this limitation and in order to inform the user that a curve has been
changed since it was read in, the lst command prints a &#147;*&#148;
in front of curves which have been changed.<p>



<a name="Unotes"></a>
<h1 ALIGN="CENTER">ULTRA II Notes</h1>

In this section we present some notes about various features of ULTRA
which need more explanation than can accompany a command description,
or which span many commands.<p>

<CENTER><B>Axis Labels</B></CENTER>
<P>
ULTRA II supports several plotting modes including linear-linear
Cartesian, log-linear Cartesian, etc. Drawing good-looking axes for all
the different possibilities is something of an art. Space is usually an
important constraint. To help the user there are several controls on
the detailed appearance of the axes.<p>

When the difference in the limits of the axis label values
approaches or falls below the precision with which the label
values are printed, the resulting axis would be less informative
than you would like.  PGS divides this problem into two cases:<p>

<B>i) the axis labels are printed with an E format (e.g. %10.2e)</B><p>

In this case PGS selects one label value (usually the minimum)
as a reference value and prints it with a '~' character.
All other label values on that axis are printed relative to the
reference value.  For example, suppose the axis limits were
1000.0 to 1001.0, the precision 2 decimal places, and three label
to be printed.  The labels would be printed as:<p>

<BLOCKQUOTE>
~1.00e+03 &nbsp&nbsp&nbsp 5.00e-01 &nbsp&nbsp&nbsp 1.00e-00<p>
</BLOCKQUOTE>

<B>ii) the axis labels are printed with an F or G format (e.g. %10.2g)</B><p>

In this case PGS selects one label value (usually the minimum)
as a reference value and prints it with a '>' character.
All other label values on that axis are printed relative to the
reference value.  For example, suppose the axis limits were
1000.0 to 1001.0, the precision 2 decimal places, and three label
to be printed.  The labels would be printed as:<p>

<BLOCKQUOTE>
&gt;1000.00 &nbsp&nbsp&nbsp 0.50 &nbsp&nbsp&nbsp 1.00<p>
</BLOCKQUOTE>

This scheme was chosen keeping the space limitations in mind. Putting a
separate label for the overall base or scale has serious ramifications
for plotting space.<p>

NOTE: this does not currently apply to log axes.<p>

<CENTER><B>Axis Types</B></CENTER>
<P>
The standard option for cartesian plots is to draw the axes around the
viewport where the curves are drawn. For polar plots the standard option
is to draw axes through the point (0,0). Now users can select either axis
option with either plot type. The variable axis-type can be set to select
the option desired. The axis-type options are the same as the plot-type
options. ULTRA does some consistency checking of the plot type and axis
type. For example, it will not let you attempt CARTESIAN or POLAR axes
with an INSEL plot. The plot type always wins if there is a problem.<p>

<CENTER><B>Color Palettes</B></CENTER>
<P>
ULTRA supports users in selecting the colors which they want to have
available in plots. By default ULTRA supplies a standard palette with
15 colors. Different users find some colors easier to see than others
and some find that certain colors are either invisible or
indistinguishable from other colors.<p>

The mk-pal command lets users build up a palette of colors from a set of
available colors. When invoked, a new window is created which shows the
available colors and at the top is a number of empty boxes corresponding
to the number of colors requested. Clicking with the left mouse button
on one of the available colors fills the next empty box with that color.
If you select more colors than requested, previously selected colors are
overwritten. When you have filled up the empty boxes and are satisfied
with the colors you have chosen, click the right mouse button. The new
window goes away and the palette you created becomes the current palette
for the window. An ASCII file is created with &#147;.pal&#148; appended
to the name which you selected for the palette. You may edit this file
if you choose.<p>

For subsequent sessions you may read in your palette with the rd-pal
command. If successful this palette becomes the current palette for the
window. By adding a rd-pal command to your .ultrarc file you can have
your favorite colors in every ULTRA session.<p>

The format of a palette file is very simple. The first line contains the
name of the palette and the number of colors in the palette, nc. This must
be followed by nc lines, each specifying a single entry in the palette.
Each line contains the red, green, and blue fractions (i.e. values from
0.0 to 1.0) of the color in that order.<p>

<a name="Ucoms"></a>
<h1 ALIGN="CENTER"> Ultra Commands</h1>

The Ultra commands are grouped according to a rough functional similarity
which is also somewhat reflected in their usages. In each group the
commands are listed alphabetically. The groups are:<p>

<PRE>


          I/O Commands

          Math Operations Which Do Not Generate a New Curve

          Math Operations Which Do Generate a New Curve

          Environmental Inquiry Commands

          Curve Inquiry Commands

          Environmental Control Commands

          Plot Control Commands

          Curve Control Commands

          Commands Useful for Writing Extensions

</PRE>

A brief description of each group is given with the functions.<p>

NOTE: If both macro and procedure versions of a command are provided, the
name of the procedure will end with an asterisk. In most cases you will
want to use the macro version of a particular command. See the section,
<a href="#Uextend">Extending ULTRA II</a>, for information on the
distinction between macros and procedures. <p>

<a name="U88"></a>
<h2>I/O Commands</h2>

These commands access disk files either for reading or writing.<p>

<BLOCKQUOTE>

<BR>
<B>autoload</B>
<BLOCKQUOTE>
<B>Macro:</B> Causes the file which defines the
named procedure to be loaded upon the first 
invocation of the procedure. The first invocation of the function causes file-
name to be read and the definition of procedure-name to be replaced by the one 
in the file, but it always has the same calling sequence. This can save lots of 
space by not filling memory with functions that are not used. Once an 
autoloaded procedure has been invoked it remains in memory until explicitly 
removed.
<P>
<B>Usage:</B> autoload procedure-name file-name
</BLOCKQUOTE>

<BR>
<B>command-log</B>
<BLOCKQUOTE>
<B>Macro:</B> If no argument is supplied or the argument is on, begin
logging to file ultra.log. 
If a file name is supplied, begin logging to that file. Append to
existing files. If 
the argument is off, terminate logging. Logging is initially off.
<P>
<B>Usage:</B> command-log [on | off | file-name]
<P>
<B>Default:</B> off
</BLOCKQUOTE>

<P><BR>
<B>compare</B>
<BLOCKQUOTE>
<B>Procedure:</B> Compare the curves from two ULTRA files. This function expects
the names of two files which should have comparable curves. The function 
diff-measure is mapped over each pair of curves (one from each file) with 
the following result. For the nth curve from each file a fractional difference 
measure is computed and if the average value of the fractional difference 
exceeds the error tolerance, the pair of curves is displayed along with the 
fractional difference and its integral along with a prompt consisting of the 
curve number, the label of the first curve and the value of the average
fractional difference. The user can respond with &#147;y&#148; to indicate
acceptance of 
the result or &#147;n&#148; to indicate non-acceptance of the result.
At the completion 
of the process, a log file ending with &#147;.chk&#148; is written summarizing the 
curves which differed and were not accepted. This can be used to automate 
the comparison of simulation code results from one run to another. The 
optional error tolerance defaults to 10-8. Note: compare has the side-effect 
of erasing all curves from the screen and killing any curves on the menu.
<P>
<B>Usage:</B> compare file-name1 file-name2 [tolerance]
</BLOCKQUOTE>

<P><BR>
<B>hardcopy</B>
<BLOCKQUOTE>
<B>Macro:</B> Send the current plot or the specified list
of curves to all active hardcopy output 
devices. If a list is given, send out the specified curves, one curve per plot. Lists 
refer to the curve numbers displayed by the menu command. The list must be 
space delimited. In interactive mode, ranges may be indicated with a colon. For 
example, 2:6 will generate 2 3 4 5 6. (The colon notation is shorthand for the 
thru procedure.) If no list is given, send out the current plot. If the argument is 
all, send out all curves in the menu, one curve per plot. The current options for 
hardcopy output device are: a PostScript file; or a CGM file. The default name 
for the PostScript file is plots.ps, and the ps-name command can be used to set 
the root part of the file name. The default name for the CGM file is plots.cgm, 
and the cgm-name command can be used to set the root part of the file name. 
Any open files remain open until closed by the close-device command or until 
ULTRA terminates. Note: No printing is actually accomplished until you send 
the file to the actual printing device.
<P>Usage: hardcopy [all | number-list]
</BLOCKQUOTE>

<P><BR>
<B>hc1</B>
<BLOCKQUOTE>
<B>Macro:</B> Send the current plot or the specified list of curves to
all active hardcopy output devices. If a list is given, send out the
specified curves, one plot per file. Lists refer to the curve numbers
displayed by the menu command. The list must be space delimited. (Colon
notation and thru procedure do not work in this context.) The current
options for hardcopy output device are: a PostScript file; or a CGM file.
The hc1 command creates a family of files; the default name for the
PostScript file family is plots.ps, producing files called plots1.ps,
plots2.ps, etc. The ps-name command can be used to set the root part of
the file name. The default name for the CGM file family is plots.cgm,
producing files called lots1.cgm, plots2.cgm, etc. The cgm-name command
can be used to set the root part of the file name. Note: No printing is
actually accomplished until you send the file(s) to the actual printing device.
<P>Usage: hc1 [all | number-list]
</BLOCKQUOTE>

<P><BR>
<B>ld</B>
<BLOCKQUOTE>
<B>Macro:</B> Read SCHEME forms from the specified ASCII disk file.
The &#146;-l&#146; execute line 
option can be used to cause ULTRA to read a file of SCHEME forms at start-
up.
<P>Usage: ld file-name
</BLOCKQUOTE>

<P><BR>
<B>merge</B>
<BLOCKQUOTE>
<B>Macro:</B> Merge the curves from a list of ULTRA files into a new ULTRA file.
<P>Usage: merge target-file-name source-file-name-list
</BLOCKQUOTE>

<P><BR>
<B>np</B>
<BLOCKQUOTE>
<B>Macro:</B> Invoke commands in non-printing mode. This permits the menu and
lst commands to be invoke from within other commands to get curve number
or data-id lists without the usual terminal output. For example, to remove
all curves on the menu that were read in from file &#147;foo.u&#148; without
output to the terminal, type:  np kill (menu * foo.u). Also see the
interactive command.
<P>Usage: np command [arguments]
</BLOCKQUOTE>

<P><BR>
<B>print-menu</B>
<BLOCKQUOTE>
<B>Macro:</B> Print the current menu of curves available for plotting to the specified file. The 
default file name is ultra.menu. Note: No printing is actually accomplished 
until you send the file to the actual printing device.
<P>Usage: print-menu [file-name]
</BLOCKQUOTE>

<P><BR><B>rd</B>
<BLOCKQUOTE>
<B>Macro:</B> Read curves from the specified ASCII or binary disk file. The next available 
prefix (see the prefix command) is automatically assigned the menu index of 
the first curve in each data file read.
<P>Usage: rd file-name
</BLOCKQUOTE>

<P><BR><B>read-table</B>
<BLOCKQUOTE>
<B>Macro:</B> Read the n<sup>th</sup> table that begins at or after the indicated line in the specified 
ASCII disk file. Display the m<sup>th</sup> line of text before the table if m &gt; 0 or display 
the |m|<sup>th</sup> line of text after the table if m &lt; 0.
<P><I>Usage: </I>read-table file-name [n [m [line]]]
</BLOCKQUOTE>

<P><BR><B>resume</B>
<BLOCKQUOTE>
<I>Procedure: </I>Resume reading SCHEME forms from an ASCII disk file. See stop.
<P><I>Usage: </I>resume
</BLOCKQUOTE>

<P><BR><B>save</B>
<BLOCKQUOTE>
<B>Macro:</B> Save currently plotted curves and/or curves from the menu in the specified disk 
file. An optional format may be specified: ascii or pdb (machine independent 
binary). The default is pdb. The file remains open until curves are saved to a 
different file with the same format or until the end command closes all open 
files. Once closed a file may not be appended to.
<P><I>Usage: </I>save [ascii | pdb] file-name curve-and/or-number-list
</BLOCKQUOTE>

<P><BR><B>stop</B>
<BLOCKQUOTE>
<I>Procedure: </I>Return control to the terminal. See resume.
<P><I>Usage: </I>stop
</BLOCKQUOTE>

</BLOCKQUOTE>

<P><a name="U13"></a>
<h2>Math Operations Which Do Not Generate a New Curve</h2>

These functions all take as arguments a list of curves (some take an additional
argument - most often a number). The specified operation is performed on the
curves without creating new ones; that is, they modify existing curves.<p>

The trigonometric functions (sin, cos, tan) use radians by default. If a user wants
to have the trigonometric functions use degrees instead of radians (or to be able
to switch between the two), the file trig.scm should be loaded using the ld command.
The use-radians variable determines whether the units are radians or degrees.<p>

<BLOCKQUOTE>

<BR><B>abs</B><BR>
<I>Procedure: </I>Take the absolute value of the y values of the curves.
<P>
<I>Usage: </I>abs curve-list
<P>

<BR><B>absx</B><BR>
<I>Procedure: </I>Take the absolute value of the x values of the curves.
<P>
<I>Usage: </I>absx curve-list
<P>

<BR><B>acos</B><BR>
<I>Procedure: </I>Take the cos<sup>-1</sup> of the y values of the curves.
<P>
<I>Usage: </I>acos curve-list
<P>

<BR><B>acosx</B><BR>
<I>Procedure: </I>Take the cos<sup>-1</sup> of the x values of the curves.
<P>
<I>Usage: </I>acosx curve-list
<P>

<BR><B>asin</B><BR>
<I>Procedure: </I>Take the sin<sup>-1</sup> of the y values of the curves.
<P>
<I>Usage: </I>asin curve-list
<P>

<BR><B>asinx</B><BR>
<I>Procedure: </I>Take the sin<sup>-1</sup> of the x values of the curves.
<P>
<I>Usage: </I>asinx curve-list
<P>

<BR><B>atan</B><BR>
<I>Procedure: </I>Take the tan<sup>-1</sup> of the y values of the curves.
<P>
<I>Usage: </I>atan curve-list
<P>

<BR><B>atanx</B><BR>
<I>Procedure: </I>Take the tan<sup>-1</sup> of the x values of the curves.
<P>
<I>Usage: </I>atanx curve-list
<P>

<BR><B>cos</B><BR>
<I>Procedure: </I>Take the cosine of the y values of the curves.
<P>
<I>Usage: </I>cos curve-list
<P>

<BR><B>cosh</B><BR>
<I>Procedure: </I>Take the hyperbolic cosine of the y values of the curves.
<P>
<I>Usage: </I>cosh curve-list
<P>

<BR><B>coshx</B><BR>
<I>Procedure: </I>Take the hyperbolic cosine of the x values of the curves.
<P>
<I>Usage: </I>coshx curve-list
<P>

<BR><B>cosx</B><BR>
<I>Procedure: </I>Take the cosine of the x values of the curves.
<P>
<I>Usage: </I>cosx curve-list
<P>

<BR><B>dx</B><BR>
<I>Procedure: </I>Shift the x values of the curves by a fixed value.
<P>
<I>Usage: </I>dx curve-list value
<P>

<BR><B>dy</B><BR>
<I>Procedure: </I>Shift the y values of the curves by a fixed value.
<P>
<I>Usage: </I>dy curve-list value
<P>

<BR><B>divx</B><BR>
<I>Procedure: </I>Divide the x values of the curves by a fixed value.
<P>
<I>Usage: </I>divx curve-list value
<P>

<BR><B>divy</B><BR>
<I>Procedure: </I>Divide the y values of the curves by a fixed value.
<P>
<I>Usage: </I>divy curve-list value
<P>

<BR><B>error-bar</B><BR>
<I>Procedure: </I>Draw error bars on the points of the specified curve. The positive and negative going errors for the data must be given as curves and combined 
together as shown in the usage below. NOTE: for this first cut if you delete 
the curves with the error information, the error-bars will be scrambled. You 
should use the hide command to obtain uncluttered plots with error bars. 
See error-bar-cap-size for some control over the appearance of the error 
bars.
<P>
<I>Usage: </I>error-bar curve pos-dy-curve [neg-dy-curve [pos-dx-curve [neg-dx-curve]]]
<P>

<BR><B>exp</B><BR>
<I>Procedure: </I>Take the exponential of the y
values of the curves, i.e., y = e<sup>y</sup>.
<P>
<I>Usage: </I>exp curve-list
<P>

<BR><B>expx</B><BR>
<I>Procedure: </I>Take the exponential of the x
values of the curves, i.e., x = e<sup>x</sup>.
<P>
<I>Usage: </I>expx curve-list
<P>

<BR><B>filter</B><BR>
<I>Procedure: </I>Remove points from the curves that fail the specified domain predicate or 
range predicate. The predicates must be procedures that return true or false 
when applied to elements of a domain or range.
<P>
<I>Usage: </I>filter curve-list domain-predicate range-predicate
<P>

<BR><B>filter-coef</B><BR>
<I>Procedure: </I>Return a curve filtered through coefficients. coeff-array is an array of filter 
coefficients set up using the make-filter routine.
<P>
<I>Usage: </I>filter-coef curve-list coeff-array ntimes
<P>

<BR><B>j0</B><BR>
<I>Procedure: </I>Take the zeroth order Bessel function of the first kind of the y values of the 
curves.
<P>
<I>Usage: </I>j0 curve-list
<P>

<BR><B>j0x</B><BR>
<I>Procedure: </I>Take the zeroth order Bessel function of the first kind of the x values of the 
curves.
<P>
<I>Usage: </I>j0x curve-list
<P>

<BR><B>j1</B><BR>
<I>Procedure: </I>Take the first order Bessel function of the first kind of the y values of the 
curves.
<P>
<I>Usage: </I>j1 curve-list
<P>

<BR><B>j1x</B><BR>
<I>Procedure: </I>Take the first order Bessel function of the first kind of the x values of the 
curves.
<P>
<I>Usage: </I>j1x curve-list
<P>

<BR><B>jn</B><BR>
<I>Procedure: </I>Take the nth order Bessel function of the first kind of the y values of the 
curves.
<P>
<I>Usage: </I>jn curve-list n
<P>

<BR><B>jnx</B><BR>
<I>Procedure: </I>Take the nth order Bessel function of the first kind of the x values of the 
curves.
<P>
<I>Usage: </I>jnx curve-list n
<P>

<BR><B>ln</B><BR>
<I>Procedure: </I>Take the natural logarithm of the y values of the curves.
<P>
<I>Usage: </I>ln curve-list
<P>

<BR><B>lnx</B><BR>
<I>Procedure: </I>Take the natural logarithm of the x values of the curves.
<P>
<I>Usage: </I>lnx curve-list
<P>

<BR><B>log10</B><BR>
<I>Procedure: </I>Take the base 10 logarithm of the y values of the curves.
<P>
<I>Usage: </I>log10 curve-list
<P>

<BR><B>log10x</B><BR>
<I>Procedure: </I>Take the base 10 logarithm of the x values of the curves.
<P>
<I>Usage: </I>log10x curve-list
<P>

<BR><B>mx</B><BR>
<I>Procedure: </I>Scale the x values of the curves by a fixed value.
<P>
<I>Usage: </I>mx curve-list value
<P>

<BR><B>my</B><BR>
<I>Procedure: </I>Scale the y values of the curves by a fixed value.
<P>
<I>Usage: </I>my curve-list value
<P>

<BR><B>powa</B><BR>
<I>Procedure: </I>Raise a fixed value, a, to the power of
the y values of the curves, i.e., y = a<sup>y</sup>.
<P>
<I>Usage: </I>powa curve-list a
<P>

<BR><B>powax</B><BR>
<I>Procedure: </I>Raise a fixed value, a, to the
power of the x values of the curves, i.e., x = a<sup>x</sup>.
<P>
<I>Usage: </I>powax curve-list a
<P>

<BR><B>powr</B><BR>
<I>Procedure: </I>Raise the y values of the curves to
a fixed power, i.e., y = y<sup>a</sup>.
<P>
<I>Usage: </I>powr curve-list a
<P>

<BR><B>powrx</B><BR>
<I>Procedure: </I>Raise the x values of the curves
to a fixed power, i.e., x = x<sup>a</sup>.
<P>
<I>Usage: </I>powrx curve-list a
<P>

<BR><B>recip</B><BR>
<I>Procedure: </I>Take the reciprocal of the y values of the curves.
<P>
<I>Usage: </I>recip curve-list
<P>

<BR><B>recipx</B><BR>
<I>Procedure: </I>Take the reciprocal of the x values of the curves.
<P>
<I>Usage: </I>recipx curve-list
<P>

<BR><B>sin</B><BR>
<I>Procedure: </I>Take the sine of the y values of the curves.
<P>
<I>Usage: </I>sin curve-list
<P>

<BR><B>sinh</B><BR>
<I>Procedure: </I>Take the hyperbolic sine of the y values of the curves.
<P>
<I>Usage: </I>sinh curve-list
<P>

<BR><B>sinhx</B><BR>
<I>Procedure: </I>Take the hyperbolic sine of the x values of the curves.
<P>
<I>Usage: </I>sinhx curve-list
<P>

<BR><B>sinx</B><BR>
<I>Procedure: </I>Take the sine of the x values of the curves.
<P>
<I>Usage: </I>sinx curve-list
<P>

<BR><B>smo</B><BR>
<I>Procedure: </I>Smooth the curves the specified number of times using a method specified 
by the variable smooth-method. For, &#147;averaging&#148; this does an n point integral average smooth, and for &#147;fft&#148; this uses a Lorentzian filter with coefficient (n/number_points).
<P>
<I>Usage: </I>smo curve-list n [ntimes]
<P>

<BR><B>smooth3</B><BR>
<I>Procedure: </I>Smooth the curves the specified number of times using a method specified 
by the variable smooth-method. For, &#147;averaging&#148; this does a 3 point integral average smooth, and for &#147;fft&#148; this uses a Lorentzian filter with coefficient (3/number_points).
<P>
<I>Usage: </I>smooth3 curve-list [ntimes]
<P>

<BR><B>smooth5</B><BR>
<I>Procedure: </I>Smooth the curves the specified number of times using a method specified 
by the variable smooth-method. For, &#147;averaging&#148; this does a 5 point integral average smooth, and for &#147;fft&#148; this uses a Lorentzian filter with coefficient (5/number_points).
<P>
<I>Usage: </I>smooth5 curve-list [ntimes]
<P>

<BR><B>sqr</B><BR>
<I>Procedure: </I>Take the square of the y values of the curves.
<P>
<I>Usage: </I>sqr curve-list
<P>

<BR><B>sqrt</B><BR>
<I>Procedure: </I>Take the square root of the y values of the curves.
<P>
<I>Usage: </I>sqrt curve-list
<P>

<BR><B>sqrtx</B><BR>
<I>Procedure: </I>Take the square root of the x values of the curves.
<P>
<I>Usage: </I>sqrtx curve-list
<P>

<BR><B>sqrx</B><BR>
<I>Procedure: </I>Take the square of the x values of the curves.
<P>
<I>Usage: </I>sqrx curve-list
<P>

<BR><B>tan</B><BR>
<I>Procedure: </I>Take the tangent of the y values of the curves.
<P>
<I>Usage: </I>tan curve-list
<P>

<BR><B>tanh</B><BR>
<I>Procedure: </I>Take the hyperbolic tangent of the y values of the curves.
<P>
<I>Usage: </I>tanh curve-list
<P>

<BR><B>tanhx</B><BR>
<I>Procedure: </I>Take the hyperbolic tangent of the x values of the curves.
<P>
<I>Usage: </I>tanhx curve-list
<P>

<BR><B>tanx</B><BR>
<I>Procedure: </I>Take the tangent of the x values of the curves.
<P>
<I>Usage: </I>tanx curve-list
<P>

<BR><B>xmax</B><BR>
<I>Procedure: </I>Filter out points in curves whose x-values &gt; limit.
<P>
<I>Usage: </I>xmax curve-list limit
<P>

<BR><B>xmin</B><BR>
<I>Procedure: </I>Filter out points in curves whose x-values &lt; limit.
<P>
<I>Usage: </I>xmin curve-list limit
<P>

<BR><B>y0</B><BR>
<I>Procedure: </I>Take the zeroth order Bessel function of the second kind of the y values of 
the curves.
<P>
<I>Usage: </I>y0 curve-list
<P>

<BR><B>y0x</B><BR>
<I>Procedure: </I>Take the zeroth order Bessel function of the second kind of the x values of 
the curves.
<P>
<I>Usage: </I>y0x curve-list
<P>

<BR><B>y1</B><BR>
<I>Procedure: </I>Take the first order Bessel function of the second kind of the y values of the 
curves.
<P>
<I>Usage: </I>y1 curve-list
<P>

<BR><B>y1x</B><BR>
<I>Procedure: </I>Take the first order Bessel function of the second kind of the x values of the 
curves.
<P>
<I>Usage: </I>y1x curve-list
<P>

<BR><B>ymax</B><BR>
<I>Procedure: </I>Filter out points in curves whose y-values &gt; limit.
<P>
<I>Usage: </I>ymax curve-list limit
<P>

<BR><B>ymin</B><BR>
<I>Procedure: </I>Filter out points in curves whose y-values &lt; limit.
<P>
<I>Usage: </I>ymin curve-list limit
<P>

<BR><B>yn</B><BR>
<I>Procedure: </I>Take the nth order Bessel function of the second kind of the y values of the 
curves.
<P>
<I>Usage: </I>yn curve-list n
<P>

<BR><B>ynx</B><BR>
<I>Procedure: </I>Take the nth order Bessel function of the second kind of the x values of the 
curves.
<P>
<I>Usage: </I>ynx curve-list n
<P>

</BLOCKQUOTE>

<P><a name="U97"></a>
<h2>Math Operations Which Do Generate a New Curve</h2>

Most of these functions take as arguments a list of the curves (some take
an additional argument, others take just two curves). The specified
operation is performed using the curves to create new ones. The original
curves are not modified.<p>

<BLOCKQUOTE>

<BR><B>+</B><BR>
<I>Procedure: </I>Take the sum of the curves, i.e., a + b + ... + n.
<P>
<I>Usage: </I>+ curve-list
<P>

<BR><B>-</B><BR>
<I>Procedure: </I>Take the difference of the curves, i.e., a - b - ... - n.
<P>
<I>Usage: </I>- curve-list
<P>

<BR><B>*</B><BR>
<I>Procedure: </I>Take the product of the curves, i.e., a x b x ... x n.
<P>
<I>Usage: </I>* curve-list
<P>

<BR><B>/</B><BR>
<I>Procedure: </I>Take the quotient of the curves, i.e., a / b / ... / n.
<P>
<I>Usage: </I>/ curve-list
<P>

<BR><B>append-curves</B><BR>
<I>Procedure: </I>Merge a list of curves over the union of their domains. Where domains 
overlap, take the average of the curves&#146; y values. Use simple-append to 
control behaviour in overlap regions.
<P>
<I>Usage: </I>append-curves curve-list
<P>

<BR><B>average</B><BR>
<I>Procedure: </I>Average the specified curves over the intersection of their domains.
<P>
<I>Usage: </I>average curve-list
<P>

<BR><B>cfft</B><BR>
<I>Procedure: </I>Compute the Complex Fast Fourier Transform of a complex curve. Return 
the real and imaginary parts.
<P>
<I>Usage: </I>cfft real-curve imaginary-curve
<P>

<BR><B>compose</B><BR>
<I>Procedure: </I>Compute the functional composition of two curves, i.e., f(g(x)).
<P>
<I>Usage: </I>compose curve-f curve-g
<P>

<BR><B>convol</B><BR>
<I>Procedure: </I>Compute the convolution of the two curves,
i.e.
<br>
<CENTER><img width="231" height="50" src="ultra1.gif"></CENTER>
<br>
This fast method uses FFT&#146;s and the interpolations involved may give
incorrect results due to improper padding - use with caution.
<P>
<I>Usage: </I>convol curve-f curve-g
<P>

<BR><B>convolb</B><BR>
<I>Procedure: </I>Compute the convolution of the two curves, i.e.
<br>
<CENTER><img width="203" height="46" src="ultra2.gif"></CENTER>
<br>
This slower method uses direct integration and minimal interpolations.
The curve-g is normalized to unit area before doing the convolution.
<P>
<I>Usage: </I>convolb curve-f curve-g
<P>

<BR><B>convolc</B><BR>
<I>Procedure: </I>Compute the convolution of the two curves, i.e.
<br>
<CENTER><img width="203" height="46" src="ultra2.gif"></CENTER>
<br>
This slower method uses direct integration and minimal interpolations.
<P>
<I>Usage: </I>convolc curve-f curve-g
<P>

<BR><B>correl</B><BR>
<I>Procedure: </I>Compute the correlation function of the two curves, i.e. 
<br>
<CENTER><img width="317" height="45" src="ultra3.gif"></CENTER>
<br>
<I>Usage: </I>correl curve-f curve-g
<P>

<BR><B>delta</B><BR>
<I>Procedure: </I>Create a reasonable approximation
to a Dirac delta function from x<sub>min</sub> to 
x<sub>max</sub> with the singularity at x<sub>0</sub> where
<br>
<CENTER><img width="184" height="55" src="ultra4.gif"></CENTER>
<br>
This is computed so that: 
<br>
<CENTER><img width="297" height="102" src="ultra5.gif"></CENTER>
<p>
<I>Usage: </I>delta x<sub>min</sub> x<sub>0</sub>
x<sub>max</sub> [n-points]
<P>

<BR><B>derivative</B><BR>
<I>Procedure: </I>Take the derivative of the curves.
<P>
<I>Usage: </I>derivative curve-list
<P>

<BR><B>diff-measure
</B><BR><I>Procedure: </I>Compare two curves. For the given curves a fractional difference measure 
and its average is computed, i.e.
<br>

<CENTER><img width="297" height="102" src="ultra6.gif"></CENTER><p>

The first optional argument controls printing and color selection. The second optional argument specifies the error tolerance and defaults to 10-8<p>

<I>Usage: </I>diff-measure curve-a curve-b [#t|#f] [tolerance]
<P>

<BR><B>diffraction</B><BR>
<I>Procedure: </I>Compute a diffraction pattern corresponding to a circular aperture of radius 
r.
<P>
<I>Usage: </I>diffraction r [n-points]
<P>

<BR><B>edit</B><BR>
<I>Procedure: </I>Graphically edit a curve on the screen. This command takes a curve to be 
edited. A copy of the curve is made and drawn as a scatter plot. The markers of points to be retained are colored green and the points to be removed 
are colored red. By holding down the left mouse button and moving the 
cursor, the user can select points for removal. Points can be toggled back to 
green by holding down the shift key and the left mouse button and moving 
the cursor. When the all points are selected, clicking the right mouse button 
returns to ULTRA where the new curve is cleaned up and replotted as a line 
plot.
<P>
<I>Usage: </I>edit curve
<P>

<BR><B>extract</B><BR>
<I>Procedure: </I>Derive a new curve from an existing one by interpolating values from the 
existing curve. You may specify either the number of points desired or sets 
of triples x<sub>min</sub>, x<sub>max</sub>,
dx which specify sub-domains of the given curve.
<P>
<I>Usage: </I>extract curve npoints |
 x<sup>1</sup><sub>min</sub> x<sup>1</sup><sub>max</sub>
 dx<sup>1</sup> [x<sup>2</sup><sub>min</sub> x<sup>2</sup><sub>max</sub>
 dx<sup>2</sup> ...]
<P>

<BR><B>fft</B><BR>
<I>Procedure: </I>Compute the Fast Fourier Transform of a real curve. Return the real and 
imaginary parts.
<P>
<I>Usage: </I>fft curve
<P>

<BR><B>fit</B><BR>
<I>Procedure: </I>Find least-squares fits to the specified curves for a polynomial of order n. 
Enter &lt;carriage-return&gt; to continue.
<P>
<I>Usage: </I>fit curve-list n
<P>

<BR><B>fitcurve
</B>
<BR><I>Procedure: </I>Using all the curves in the argument list except the first, compute a fit to the 
first curve in the list using the least squares method. The result is a new 
curve which is a linear combination of all of the given curves but the first.
<P>
<I>Usage: </I>fitcurve curve-to-fit curves-for-fit
<P>

<BR><B>fode</B><BR>
<I>Procedure: </I>Compute the solution to the first order differential equation:
<P>

<CENTER><img width="141" height="56" src="ultra7.gif"></CENTER><p>

in the range x<sub>min</sub> to x<sub>max</sub> where a and b are given by
curve-a and curve-b respectively. The homogeneous and particular solutions
to the equation are plotted and returned.<p>

<I>Usage: </I>fode curve-a curve-b x<sub>min</sub> x<sub>max</sub>
<P>

<BR><B>gaussian</B><BR>
<I>Procedure: </I>Generate a gaussian function:
<br>
<CENTER><img width="180" height="64" src="ultra8.gif"></CENTER>
<P>
<I>Usage: </I>gaussian a w x<sub>0</sub> [n-points]
<P>

<BR><B>hypot</B><BR>
<I>Procedure: </I>Take the harmonic average of two curves, i.e.,
<br>
<CENTER><img width="93" height="40" src="ultra9.gif"></CENTER>
<P>
<I>Usage: </I>hypot curve-a curve-b
<P>

<BR><B>ifft</B><BR>
<I>Procedure: </I>Compute the Inverse Fast Fourier Transform of the curve. Return the real 
and imaginary parts.
<P>
<I>Usage: </I>ifft real-curve imaginary-curve
<P>

<BR><B>integrate</B><BR>
<I>Procedure: </I>Compute the definite integral of each curve in the list over the specified 
domain.
<P>
<I>Usage: </I>integrate curve-list low-lim high-lim
<P>

<BR><B>max</B><BR>
<I>Procedure: </I>Construct a curve from the maximum y values at each point.
<P>
<I>Usage: </I>max curve-list
<P>

<BR><B>min</B><BR>
<I>Procedure: </I>Construct a curve from the minimum y values at each point.
<P>
<I>Usage: </I>min curve-list
<P>

<BR><B>normalize</B><BR>
<I>Procedure: </I>Construct a new curve with unit area in its domain.
<P>
<I>Usage: </I>normalize curve
<P>

<BR><B>span</B><BR>
<I>Procedure: </I>Generate a curve of unit slope and zero y intercept in the given domain.
<P>
<I>Usage: </I>span x<sub>min</sub> x<sub>max</sub> [n-points]
<P>

<BR><B>theta</B><BR>
<I>Procedure: </I>Create a reasonable approximation
to a step function from x<sub>min</sub> to x<sub>max</sub> with 
the step at x<sub>0</sub>. 
<P>
<I>Usage: </I>
theta x<sub>min</sub> x<sub>0</sub> x<sub>max</sub> [n-points]
<P>

<BR><B>thin</B><BR>
<I>Procedure: </I>Construct new curves which retains the information content of the given 
curves but with fewer points.
<P>
<I>Usage: </I>thin curve-list &#147;int&#148; n-points | &#147;dif&#148; tolerance
<P>

<BR><B>vs</B><BR>
<I>Procedure: </I>Plot the range of the first curve against the range of the second curve.
<P>
<I>Usage: </I>vs curve-a curve-b
<P>

</BLOCKQUOTE>

<a name="U30"></a>
<h2> Environmental Inquiry Commands</h2>

These functions are provided to gain access to information about the state of the
Ultra session. Information such as the state of global variables and system help
packages is made available through these functions.<p>

<BLOCKQUOTE>

<BR><B>apropos</B><BR>
<I>Macro: </I>List all functions or variables containing the designated substring. See the help 
command.
<P>
<I>Usage: </I>apropos string
<P>

<BR><B>display</B><BR>
<I>Procedure: </I>Print an ULTRA II object to the terminal.
<P>
<I>Usage: </I>display expression
<P>

<BR><B>file-info</B><BR>
<I>Macro: </I>Print the descriptive information for a PDB ULTRA file to the terminal.
<P>
<I>Usage: </I>file-info file-name
<P>

<BR><B>help</B><BR>
<I>Macro: </I>Return information about the specified command, variable, or command category. If no argument is supplied, return a list of command categories. See the 
apropos command.
<P>
<I>Usage: </I>help [command | variable | category]
<P>

<BR><B>lst</B><BR>
<I>Macro: </I>Return a list of the curves currently displayed.
Print the list to the terminal if 
terminal output is on. See the interactive and np commands. A regular
expression may be supplied for matching against the curve label to be
listed. An additional expression may be specified to restrict listings
to curves from particular 
files.
<P>
<I>Usage: </I>lst [label-pattern [file-pattern]]
<P>

<BR><B>menu</B><BR>
<I>Macro: </I>Return a list of the curves available for plotting.
Print the list to the terminal if 
terminal output is on. See the interactive and np commands. A regular
expression may be supplied for matching against the curve label to be listed.
An additional expression may be specified to restrict menu listings to curves
from 
particular files.
<P>
<I>Usage: </I>menu [label-pattern [file-pattern]]
<P>

<BR><B>menui</B><BR>
<I>Macro: </I>Return a selection of the curves available for plotting. Print the list to the terminal if terminal output is on. See the interactive and np commands. In the interactive mode, ranges may be indicated with a colon. For example, 2:6 will 
generate 2 3 4 5 6.
<P>
<I>Usage: </I>menui number-list
<P>

<BR><B>table-attributes</B><BR>
<I>Procedure: </I>Return a list containing the attributes of the current table - (n-rows n-cols 
table-number).
<P>
<I>Usage: </I>table-attributes
<P>

</BLOCKQUOTE>

<a name="U34"></a>
<h2> Curve Inquiry Commands</h2>

These functions provide a mechanism for obtaining information about specified curves.
Some of the information is used internally in Ultra but can also be useful in SCHEME
procedures.<p>

<BLOCKQUOTE>

<BR><B>disp</B><BR>
<I>Procedure: </I>Display (to the terminal) the actual values in the specified curves in the 
specified domain. One or both domain arguments may be omitted and will 
default to the extrema for the first curve.
<P>
<I>Usage: </I>disp curve-list [xmin [xmax]]
<P>

<BR><B>get-attributes</B><BR>
<I>Procedure: </I>Return (to the terminal) the given curve&#146;s attributes: (color width style).
<P>
<I>Usage: </I>get-attributes curve
<P>

<BR><B>get-domain</B><BR>
<I>Procedure: </I>Return (to the terminal) the given curve&#146;s domain.
<P>
<I>Usage: </I>get-domain curve
<P>

<BR><B>get-label</B><BR>
<I>Procedure: </I>Return (to the terminal) the given curve&#146;s label.
<P>
<I>Usage: </I>get-label curve
<P>

<BR><B>get-number-points</B><BR>
<I>Procedure: </I>Return (to the terminal) the given curve&#146;s number of points.
<P>
<I>Usage: </I>get-number-points curve
<P>

<BR><B>get-range</B><BR>
<I>Procedure: </I>Return (to the terminal) the given curve&#146;s range.
<P>
<I>Usage: </I>get-range curve
<P>

<BR><B>getx</B><BR>
<I>Procedure: </I>Return (to the terminal) the x values for a given y. Enter &lt;carriage-return&gt; 
to continue.
<P>
<I>Usage: </I>getx curve-list value
<P>

<BR><B>gety</B><BR>
<I>Procedure: </I>Return (to the terminal) the y values for a given x. Enter &lt;carriage-return&gt; 
to continue.
<P>
<I>Usage: </I>gety curve-list value
<P>

<BR><B>stats</B><BR>
<I>Procedure: </I>Calculate the mean and standard deviation for the curves and display the 
results on the terminal.
<P>
<I>Usage: </I>stats curve-list
<P>

</BLOCKQUOTE>

<P><a name="U36"></a>
<h2> Environmental Control Commands</h2>

These functions allow you to manipulate the environment of Ultra on a global level.
They are useful to avoid repeated use of other commands or to change the state of
Ultra in dramatic ways.<p>

<BLOCKQUOTE>

<BR><B>close-device</B><BR>
<I>Macro: </I>Causes the specified graphics device to be
closed appropriately. The legal values for device are: window or an X server and
screen (host:display.screen), for 
screen windows; ps, for PostScript output; and cgm for CGM output.
<P>
<I>Usage: </I>close-device device
<P>

<BR><B>end</B><BR>
<I>Procedure: </I>Exit Ultra gracefully.
<P>
<I>Usage: </I>end
<P>

<BR><B>erase</B><BR>
<I>Procedure: </I>Erase all curves on the screen but leave the limits untouched.
<P>
<I>Usage: </I>erase
<P>

<BR><B>kill</B><BR>
<I>Procedure: </I>Delete the specified entries from the menu. Once gone, the curves must be 
read in from the file to be used again. The image of curves (the plot) is not 
changed. The menu is renumbered.
<P>
<I>Usage: </I>kill all | number-list
<P>

<BR><B>open-device</B><BR>
<I>Macro: </I>Causes the specified graphics device to be opened appropriately. The legal values for device are: window or an X server and screen (host:display.screen), for 
screen windows; ps, for PostScript output; and cgm for CGM output. The legal 
values for type are: monochrome, for black and white graphics; and color, for 
color graphics. The title is used in the title bar on windows and as the base of 
the file name for PostScript and CGM output.
<P>
<I>Usage: </I>open-device device type title
<P>

<BR><B>plots</B><BR>
<I>Procedure: </I>Turn plotting of curves to the screen on or off. Even when plotting is off the 
curves still exist and may be manipulated. The &#145;-s&#146; execute line option 
causes ULTRA to start-up with plotting turned off. The &#145;-u&#146; execute line 
option causes ULTRA to start-up with plotting turned on. This is the 
default.
<P>
<I>Usage: </I>plots on | off
<P>

<BR><B>prefix</B><BR>
<I>Macro: </I>Assign or list menu prefixes. If both a prefix and menu index (curve number) 
are supplied, the prefix is assigned to the specified index. If the command is 
invoked with a prefix and no index, the prefix and its assigned index and file 
name, if any, are displayed. If invoked without arguments, information on all 
active prefixes is listed. In interactive mode, in commands requiring a menu 
index, any integer prefaced by an active prefix and a period &#145;.&#146;, is interpreted as 
an index with value one less than the sum of the integer and the index assigned 
to the prefix. For example, if prefix &#145;b&#146; is associated with menu index 10, then 
the command &#145;select b.2&#146; would cause curve 11 in the menu to be plotted. The 
next available prefix is automatically associated with the menu index of the 
first curve in each data file read in. Also see the pre procedure.
<P>
<I>Usage: </I>prefix [a | b | ... | z [menu-index | off]]
<P>

<BR><B>replot</B><BR>
<I>Procedure: </I>Replot all curves in the list. A plain carriage-return also forces a replot.
<P>
<I>Usage: </I>replot
<P>

<BR><B>screen</B><BR>
<I>Macro: </I>Begin plotting graphics on the specified display in a window with the specified 
title. display-name is either the literal string window or an X server and device 
on which to display plots. display-title is a string displayed in the title bar of 
the plot window.
<P>
<I>Usage: </I>screen [display-name [display-title]]
<P>

<BR><B>system</B><BR>
<I>Procedure: </I>Pass commands through Ultra to the operating system. (This depends on the 
operating system having this capability - not available on Macintosh 
machines.) Enclose literal command strings in double quotes. For example, 
type:  system &#147;ls&#148;
<P>
<I>Usage: </I>system command
<P>

</BLOCKQUOTE>

<P><a name="U38"></a>
<h2> Plot Control Commands</h2>

These functions control the plotting characteristics of ULTRA II which affect all displayed curves.<p>

<BLOCKQUOTE>

<BR><B>annot</B><BR>
<I>Procedure: </I>Annotate a plot, writing text at a point specified in normalized (i.e. between 
0.0 and 1.0) coordinates. Enclose text in double quotes. If the xmin, etc. are 
not specified, ULTRA will wait for you to drag a rectangle in the screen 
window where you want the text to go. To move an annotation around the 
screen, grab the annotation with the left mouse button and drag it where 
you want it to go. To remove an annotation, click on it with the right mouse 
button. To edit the text of the annotation, put the mouse close enough to the 
text of the annotation that the text cursor is activated and use EMACS editing commands to change the text.
<P>
<I>Usage: </I>annot text [color [xmin xmax ymin ymax]]
<P>

<BR><B>autoplot</B><BR>
<I>Procedure: </I>Set the state of automatic replotting.
<P>
<I>Usage: </I>autoplot on | off
<P>
<I>Default: </I>on
<P>

<BR><B>axis</B><BR>
<I>Procedure: </I>Set whether or not to draw axes on graphs.
<P>
<I>Usage: </I>axis on | off
<P>
<I>Default: </I>on
<P>

<BR><B>data-id</B><BR>
<I>Procedure: </I>Set whether or not to plot curve identifiers. This was formerly the marker 
command.
<P>
<I>Usage: </I>data-id on | off
<P>
<I>Default: </I>on
<P>

<BR><B>domain</B><BR>
<I>Procedure: </I>Set the domain for plotting. Typing de (for default) will let the curves determine the domain.
<P>
<I>Usage: </I>domain low-lim high-lim
<P>
<I>Usage: </I>domain de
<P>
<I>Default: </I>de
<P>

<BR><B>grid</B><BR>
<I>Procedure: </I>Set whether or not to draw grid lines on graphs.
<P>
<I>Usage: </I>grid on | off
<P>
<I>Default: </I>off
<P>

<BR><B>marker-scale</B><BR>
<I>Procedure: </I>Set the marker character scale. 
<P>
<I>Usage: </I>marker-scale real
<P>
<I>Default: </I>0.01
<P>

<BR><B>mk-pal</B><BR>
<I>Procedure: </I>Graphically create a new palette of line colors. The palette name and number of colors must be supplied. The resulting palette will be written into an 
ASCII file name.pal and can be read in in subsequent session with the command, rd-pal.
<P>
<I>Usage: </I>mk-pal name integer
<P>

<BR><B>range</B><BR>
<I>Procedure: </I>Set the range for plotting. Typing de (for default) will let the curves determine the range.
<P>
<I>Usage: </I>range low-lim high-lim
<P>
<I>Usage: </I>range de
<P>
<I>Default: </I>de
<P>

<BR><B>rd-pal</B><BR>
<I>Procedure: </I>Read in an ASCII palette file and make it the current palette. The palette file 
may have been created by the mk-pal command or with an ASCII text editor.
<P>
<I>Usage: </I>rd-pal filename
<P>

<BR><B>x-log-scale </B><BR>
<I>Procedure: </I>Set log scale on or off for the x axis.
<P>
<I>Usage: </I>x-log-scale on | off
<P>
<I>Default: </I>off
<P>

<BR><B>y-log-scale </B><BR>
<I>Procedure: </I>Set log scale on or off for the y axis.
<P>
<I>Usage: </I>y-log-scale on | off
<P>
<I>Default: </I>off
<P>

</BLOCKQUOTE>

<P><a name="U41"></a>
<h2> Curve Control Commands</h2>

These functions control the individual curves that are currently being
displayed. They range in type from controlling the appearance of the
curve to deleting it. They also include the &#147;non-mathematical&#148;
mechanisms which may generate curves.<p>

<BLOCKQUOTE>

<BR><B>color</B><BR>
<I>Procedure: </I>Set the color of the specified curves. Color-number may be an integer or 
any of the color constants, e.g. black, red, blue.
<P>
<I>Usage: </I>color curve-list color-number
<P>

<BR><B>copy</B><BR>
<I>Procedure: </I>Copy the specified curves.
<P>
<I>Usage: </I>copy curve-list
<P>

<BR><B>copy-curve</B><BR>
<I>Procedure: </I>Make a &#147;copy&#148; of the specified curve with an optionally specified number 
of evenly spaced points (default is 100). This could be done using extract 
but not as quickly or conveniently.
<P>
<I>Usage: </I>copy-curve curve [num-points]
<P>

<BR><B>del</B><BR>
<I>Procedure: </I>Delete the specified curves.
<P>
<I>Usage: </I>del curve-list
<P>

<BR><B>dupx</B><BR>
<I>Procedure: </I>Duplicate x values so that y = x for each of the specified curves.
<P>
<I>Usage: </I>dupx curve-list
<P>

<BR><B>fill</B><BR>
<I>Procedure: </I>Fill the area under curves with the specified color.<P>
<I>Usage: </I>fill curve-list color-number
<P>
<I>Default: </I>off
<P>

<BR><B>hide</B><BR>
<I>Procedure: </I>Hide the specified curves from view (opposite of show).
<P>
<I>Usage: </I>hide curve-list
<P>

<BR><B>histogram</B><BR>
<I>Procedure: </I>Plot the specified curves as histograms. left, right, and center determine 
how to obtain the central bin values for the histograms.
<P>
<I>Usage: </I>histogram curve-list off | left | right | center
<P>

<BR><B>label</B><BR>
<I>Procedure: </I>Change the label displayed for a curve by lst. Enclose literal label strings in 
double quotes. NOTE: A label is not to be confused with the curve identifier which is the character that identifies the curve on the plot.
<P>
<I>Usage: </I>label curve new-label-string
<P>

<BR><B>line</B><BR>
<I>Procedure: </I>Generate a curve with y = mx + b.
<P>
<I>Usage: </I>line m b low-lim high-lim [no.-pts]
<P>

<BR><B>lnstyle</B><BR>
<I>Procedure: </I>Set the line styles of specified curves.
<P>
<I>Usage: </I>lnstyle curve-list solid | dotted | dashed | dotdashed
<P>
<I>Default: </I>solid
<P>

<BR><B>lnwidth</B><BR>
<I>Procedure: </I>Set the line widths of specified curves. A line width of 0 will always give 
the thinnest line which the host graphics system supports.
<P>
<I>Usage: </I>lnwidth curve-list width
<P>
<I>Default: </I>0
<P>

<BR><B>make-curve</B><BR>
<I>Macro: </I>Make a curve from two lists of numbers. Each list must be delimited by parentheses.
<P>
<I>Usage: </I>make-curve (x-values) (y-values)
<P>

<BR><B>make-filter</B><BR>
<I>Procedure: </I>Make an array of filter coefficients.
<P>
<I>Usage: </I>make-filter val1 val2 val3 ...
<P>

<BR><B>marker</B><BR>
<I>Procedure: </I>Set the marker characters of specified curves.
<P>
<I>Usage: </I>marker curve-list plus | star | triangle
<P>
<I>Default: </I>plus
<P>

<BR><B>random</B><BR>
<I>Procedure: </I>Generate random y values between -1 and 1 for the specified curves. 
<P>
<I>Usage: </I>random curve-list
<P>

<BR><B>re-color</B><BR>
<I>Procedure: </I>Reset curve colors to preset sequence.
<P>
<I>Usage: </I>re-color
<P>

<BR><B>re-id</B><BR>
<I>Procedure: </I>Reset data-id&#146;s to A, B, C, ... (no skipping).
<P>
<I>Usage: </I>re-id
<P>

<BR><B>rev
</B><BR><I>Procedure: </I>Swap x and y values for the specified curves. You may want to sort after this 
one.
<P>
<I>Usage: </I>rev curve-list
<P>

<BR><B>scatter</B><BR>
<I>Procedure: </I>Plot the specified curves as scatter diagrams or connected lines.
<P>
<I>Usage: </I>scatter curve-list on | off
<P>
<I>Default: </I>off
<P>

<BR><B>select</B><BR>
<I>Procedure: </I>Select curves from the menu for plotting. The list must be space delimited. 
In interactive mode, ranges may be indicated with a colon. For example, 
2:6 will generate 2 3 4 5 6. (The colon notation is shorthand for the thru 
procedure.)
<P>
<I>Usage: </I>select number-list
<P>

<BR><B>set-id</B><BR>
<I>Procedure: </I>Change the data-id of the specified curve.
<P>
<I>Usage: </I>set-id curve a | b | ... | z
<P>

<BR><B>show</B><BR>
<I>Procedure: </I>Reveal the specified curves (opposite of hide).
<P>
<I>Usage: </I>show curve-list
<P>

<BR><B>sort</B><BR>
<I>Procedure: </I>Sort the specified curves so that their points are plotted in order of ascending x values.
<P>
<I>Usage: </I>sort curve-list
<P>

<BR><B>table-curve</B><BR>
<I>Procedure: </I>Build a curve from data in the current table. Tables are regarded simply as 
contiguous blocks of numbers and you may specify how the x and y values 
are to be gathered from the table. Tables are read row-wise and the offsets 
are reckoned accordingly. The number of points must be specified. The 
zero based offset from the beginning of the table and the stride to be used in 
gathering the values may be given. The default offset and stride for the y 
values are zero and one, respectively. If no offset for the x values is specified, the y values are plotted versus index.
<P>
<I>Usage: </I>table-curve num-points [y-offset [y-stride [x-offset [x-stride]]]]
<P>

<BR><B>xindex</B><BR>
<I>Procedure: </I>Create curves with y values vs. integer index values.
<P>
<I>Usage: </I>xindex curve-list
<P>

<BR><B>xmm</B><BR>
<I>Procedure: </I>Excerpt a part of the specified curves.
<P>
<I>Usage: </I>xmm curve-list low-lim high-lim
<P>

</BLOCKQUOTE>

<P><a name="U45"></a>
<h2> Commands Useful for Writing Extensions</h2>

These functions are provided to help users who wish to write their own functions
for ULTRA II. They can be used at the command level, but they are really oriented
for use in procedures to gain access about curves or otherwise control ULTRA II
in ways that are not appropriate at the command level.<p>

<BLOCKQUOTE>

<BR><B>autoload*</B><BR>
<I>Procedure: </I>Causes the file which defines the named procedure to be loaded upon the 
first invocation of the procedure. The first invocation of the function causes 
file-name to be read and the definition of procedure-name to be replaced by 
the one in the file, but it always has the same calling sequence. This can 
save lots of space by not filling memory with functions that are not used. 
Once an autoloaded procedure has been invoked it remains in memory until 
explicitly removed.
<P>
<I>Usage: </I>autoload* procedure-name file-name
<P>

<BR><B>close-device*</B><BR>
<I>Procedure: </I>Causes the specified graphics device to be closed appropriately. The legal 
values for device are: window or an X server and screen (host:display.screen), for screen windows; ps, for PostScript output; and cgm for 
CGM output.
<P>
<I>Usage: </I>close-device* device
<P>

<BR><B>curve?</B><BR>
<I>Procedure: </I>Return #t (true) iff the argument is an Ultra curve object. Otherwise return 
#f (false). Print returned value only if print-flag is on.
<P>
<I>Usage: </I>curve? object
<P>

<BR><B>curve-&gt;list</B><BR>
<I>Procedure: </I>Return a list containing the list of x values and the list of y values for the 
specified curve. This function is the inverse of make-curve
<P>
<I>Usage: </I>curve-&gt;list curve
<P>

<BR><B>file-info*</B><BR>
<I>Procedure: </I>Print the descriptive information for a PDB ULTRA file to the terminal.
<P>
<I>Usage: </I>file-info* file-name
<P>

<BR><B>handle-mouse-event</B><BR>
<I>Macro: </I>User supplied macro called on mouse up/down events if it exists.
<P>
<I>Example: </I>
<pre>
    (define-macro (handle-mouse-event type loc btn qual)
        (let* ((ev  (cond ((eqv? type mouse-up)        "up")
    		          ((eqv? type mouse-down)      "down")
    		          (else                        "unknown")))
	       (mod (cond ((eqv? qual key-shift)       "shift")
    		          ((eqv? qual key-cntl)        "ctl")
    		          ((eqv? qual key-alt)         "alt")
                          ((eqv? qual key-lock)        "lock")
                          (else                        "")))
	       (key (cond ((eqv? btn mouse-left)       "left")
    		          ((eqv? btn mouse-middle)     "middle")
    		          ((eqv? btn mouse-right)      "right")
    		          ((eqv? btn mouse-wheel-up)   "wheel-up")
    		          ((eqv? btn mouse-wheel-down) "wheel-down")
       		          (else                        btn))))
              (printf nil "Mouse %s event %s %s at %s\n" ev mod key loc)))
</pre>
A mouse up or down event in a graphics window will print out a message
detailing the mouse button, qualifiers, and the location.
<P>

<BR><B>interactive </B><BR>
<I>Procedure: </I>Turn terminal output on or off in functions only.
<P>
<I>Usage: </I>interactive on | off
<P>

<BR><B>ld*</B><BR>
<I>Procedure: </I>Read SCHEME forms from the specified ASCII disk file. The argument 
must be a SCHEME expression which evaluates to a file name.
<P>
<I>Usage: </I>ld* file-name-expression
<P>

<BR><B>lst*</B><BR>
<I>Procedure: </I>Return a list of the curves currently displayed. Print the list to the terminal 
if terminal output is on. See the interactive and np commands. A regular 
expression may be supplied for matching against the curve label to be 
listed. An additional expression may be specified to restrict listings to 
curves from particular files.
<P>
<I>Usage: </I>lst* [label-pattern [file-pattern]]
<P>

<BR><B>make-curve*</B><BR>
<I>Procedure: </I>Make a curve. The arguments must be SCHEME expressions that evaluate 
to lists of numbers.
<P>
<I>Usage: </I>make-curve* x-values-expression y-values-expression
<P>

<BR><B>menu*</B><BR>
<I>Procedure: </I>Return a list of the curves available for plotting. Print the list to the terminal 
if terminal output is on. See the interactive and np commands. A regular 
expression may be supplied for matching against the curve label to be 
listed. An additional expression may be specified to restrict menu listings 
to curves from particular files.
<P>
<I>Usage: </I>menu* [label-pattern [file-pattern]]
<P>

<BR><B>merge*</B><BR>
<I>Procedure: </I>Merge the curves from a list of ULTRA files into a new ULTRA file.
<P>
<I>Usage: </I>merge* target-file-name source-file-name-list
<P>

<BR><B>open-device*</B><BR>
<I>Procedure: </I>Causes the specified graphics device to be opened appropriately. The legal 
values for device are: window or an X server and screen (host:display.screen), for screen windows; ps, for PostScript output; and cgm for 
CGM output. The legal values for type are: monochrome, for black and 
white graphics; and color, for color graphics. The title is used in the title 
bar on windows and as the base of the file name for PostScript and CGM 
output.
<P>
<I>Usage: </I>open-device* device type title
<P>

<BR><B>pre</B><BR>
<I>Procedure: </I>Given a prefix and an index, return a curve number. In interactive mode 
only, the shorthand notation prefix.index may be used. See the prefix command.
<P>
<I>Usage: </I>pre prefix index
<P>

<BR><B>prefix*</B><BR>
<I>Procedure: </I>Assign or list menu prefixes. If both a prefix and menu index (curve number) are supplied, the prefix is assigned to the specified index. If the command is invoked with a prefix and no index, the prefix and its assigned 
index and file name, if any, are displayed. If invoked without arguments, 
information on all active prefixes is listed. In interactive mode, in commands requiring a menu index, any integer prefaced by an active prefix and 
a period &#145;.&#146;, is interpreted as an index with value one less than the sum of 
the integer and the index assigned to the prefix. For example, if prefix &#145;b&#146; is 
associated with menu index 10, then the command &#145;select b.2&#146; would cause 
curve 11 in the menu to be plotted. The next available prefix is automatically associated with the menu index of the first curve in each data file read 
in. Also see the pre procedure.
<P>
<I>Usage: </I>prefix* [a | b | ... | z [menu-index | off]]
<P>

<BR><B>rd*</B><BR>
<I>Procedure: </I>Read curves from the specified ASCII or binary disk file. The argument 
must be a SCHEME expression which evaluates to a file name.
<P>
<I>Usage: </I>rd* file-name-expression
<P>

<BR><B>read-table*</B><BR>
<I>Procedure: </I>Read the nth table that begins at or after the indicated line in the specified 
ASCII disk file. The first argument must be a SCHEME expression which 
evaluates to a file name.
<P>
<I>Usage: </I>read-table* file-name-expression [n [line]]
<P>

<BR><B>save*</B><BR>
<I>Procedure: </I>Save currently plotted curves and/or curves from the menu in the specified 
disk file. An optional format may be specified: ascii or pdb (machine independent binary). The default is pdb. The file remains open until curves are 
saved to a different file with the same format or until the end command 
closes all open files. Once closed a file may not be appended to.
<P>
<I>Usage: </I>save* [&#145;ascii | &#145;pdb] file-name-expression curve-and/or-number-list
<P>

<BR><B>synonym</B><BR>
<I>Macro: </I>Install the name(s) in the hash table as synonyms for the given function.
<P>
<I>Usage: </I>synonym func syn1 ... synn
<P>

<BR><B>thru</B><BR>
<I>Procedure: </I>Given the first and last elements, return a range of curve numbers or data-
id&#146;s that are currently active. In interactive mode only, the shorthand notation first:last may be used.
<P>
<I>Usage: </I>thru first-curve-number last-curve-number
<P>
<I>Usage: </I>thru first-data-id last-data-id
<P>

<BR><B>ultra-file?</B><BR>
<I>Procedure: </I>Returns #t if the named file is a valid ULTRA II file, either ASCII or binary. 
This is mainly intended for use in ULTRA functions.
<P>
<I>Usage: </I>ultra-file? file-name
<P>

</BLOCKQUOTE>

<a name="Uvars"></a>
<h1 ALIGN="CENTER"> ULTRA Variables</h1>

ULTRA has many internal variables which control its actions. You may set some of
these variables to customize ULTRA to taste. All of the ULTRA variables behave as
functions. With no arguments, the current value of the variable is returned. With
arguments, the value of the variable is set to the value of the first argument. The
default values given reflect the defaults of ULTRA using the standard resource
(environment) files. You may customize the defaults of ULTRA by customizing the
resource files.<p>

To examine the value of an ULTRA variable, just give the name of the variable. For example,<p>

<PRE>
          U-&gt; label-length

                     25
</PRE>

To change a variable&#146;s value, give the variable name and the new value.
For example,<p>

<PRE>
          U-&gt; label-length 40
</PRE>

<BLOCKQUOTE>

<BR><B>answer-prompt</B><BR>
STRING - string printed before return value. Enclose literal strings in double quotes.
<P>
<I>Usage: </I>answer-prompt string
<P>
<I>Default: </I>&#147;(0):&#148;
<P>

<BR><B>ascii-output-format</B><BR>
STRING - string containing the C language style output format for
floating point numbers (used by save command with the ascii option and other
commands printing to terminal). Enclose literal format strings in double quotes.
<P>
<I>Usage: </I>ascii-output-format string
<P>
<I>Default: </I>&#147;%13.6&#148;
<P>

<BR><B>axis-grid-style</B><BR>
INTEGER - line style of axis grid lines.
<P>
<I>Usage: </I>axis-grid-style solid | dotted | dashed | dotdashed
<P>
<I>Default: </I>dotted
<P>

<BR><B>axis-line-style</B><BR>
INTEGER - line style of axis lines.
<P>
<I>Usage: </I>axis-line-style solid | dotted | dashed | dotdashed
<P>
<I>Default: </I>solid
<P>

<BR><B>axis-line-width</B><BR>
REAL - line width of axis lines.
<P>
<I>Usage: </I>axis-line-width real
<P>
<I>Default: </I>.1
<P>

<BR><B>axis-max-major-ticks</B><BR>
INTEGER - maximum number of major tick marks on the axes.
<P>
<I>Usage: </I>axis-max-major-ticks integer
<P>
<I>Default: </I>10
<P>

<BR><B>axis-n-decades</B><BR>
INTEGER - maximum number of log axis decades.
<P>
<I>Usage: </I>axis-n-decades integer
<P>
<I>Default: </I>8
<P>

<BR><B>axis-number-minor-ticks</B><BR>
INTEGER - number of minor tick spaces between major ticks on the axes.
<P>
<I>Usage: </I>axis-number-minor-ticks integer
<P>
<I>Default: </I>-1 (select the number automatically)
<P>

<BR><B>axis-number-minor-x-ticks</B><BR>
INTEGER - number of minor tick spaces between major ticks on the x axis. This overrides axis-number-minor-ticks when greater than -1.
<P>
<I>Usage: </I>axis-number-minor-x-ticks integer
<P>
<I>Default: </I>-1 (use axis-number-minor-ticks)
<P>

<BR><B>axis-number-minor-y-ticks</B><BR>
INTEGER - number of minor tick spaces between major ticks on the y axis. This overrides axis-number-minor-ticks when greater than -1.
<P>
<I>Usage: </I>axis-number-minor-y-ticks integer
<P>
<I>Default: </I>-1 (use axis-number-minor-ticks)
<P>

<BR><B>axis-tick-size</B><BR>
REAL - fractional size of major tick marks on the axes.
<P>
<I>Usage: </I>axis-tick-size real
<P>
<I>Default: </I>.018
<P>

<BR><B>axis-tick-type</B><BR>
INTEGER - placement of tick marks relative to axis.
<P>
<I>Usage: </I>axis-tick-type right-of-axis | left-of-axis | straddle-axis
<P>
<I>Default: </I>right-of-axis
<P>

<BR><B>axis-type</B><BR>
INTEGER - axis type.
<P>
<I>Usage: </I>axis-type cartesian | polar | insel
<P>
<I>Default: </I>cartesian
<P>

<BR><B>axis-x-format</B><BR>
STRING - C language style format of the x axis labels. Enclose literal format strings in 
double quotes.
<P>
<I>Usage: </I>axis-x-format format-string
<P>
<I>Default: </I>&#147;%10.2g&#148;
<P>

<BR><B>axis-y-format</B><BR>
STRING - C language style format of the y axis labels. Enclose literal format strings in 
double quotes.
<P>
<I>Usage: </I>axis-y-format format-string
<P>
<I>Default: </I>&#147;%10.2g&#148;
<P>

<BR><B>background-color-flag</B><BR>
INTEGER - background color on display. Takes effect when screen is invoked.
<P>
<I>Usage: </I>background-color-flag white | black
<P>
<I>Default: </I>white
<P>

<BR><B>border-width</B><BR>
INTEGER - window border width in pixels. Takes effect when screen is invoked.
<P>
<I>Usage: </I>border-width integer
<P>
<I>Default: </I>2
<P>

<BR><B>botspace</B><BR>
REAL - fractional space at bottom of plot, i.e. between curves and axes.
<P>
<I>Usage: </I>botspace real
<P>
<I>Default: </I>.01
<P>

<BR><B>cgm-background-color-flag</B><BR>
INTEGER - CGM device background color. If auto specified, use display background 
color as specified by background-color-flag.
<P>
<I>Usage: </I>cgm-background-color-flag white | black | auto
<P>
<I>Default: </I>white
<P>

<BR><B>cgm-flag</B><BR>
INTEGER - when on, hardcopy will generate a plot in the CGM file. See the cgm-
name command to control file name.
<P>
<I>Usage: </I>cgm-flag on | off
<P>
<I>Default: </I>off
<P>

<BR><B>cgm-name</B><BR>
STRING - the root part of the name ULTRA II uses when creating a CGM for output. 
The extension .cgm is always used. Enclose literal strings in double quotes.
<P>
<I>Usage: </I>cgm-name string
<P>
<I>Default: </I>&#147;plots&#148;
<P>

<BR><B>cgm-type</B><BR>
STRING - the type ULTRA II uses when creating a CGM for output. Enclose literal 
strings in double quotes.
<P>
<I>Usage: </I>cgm-type &#147;monochrome&#148; | &#147;color&#148;
<P>
<I>Default: </I>&#147;monochrome&#148;
<P>

<BR><B>console-height</B><BR>
REAL - console window height in fraction of screen height.
<P>
<I>Usage: </I>console-height real
<P>
<I>Default: </I>1.0
<P>

<BR><B>console-origin-x</B><BR>
REAL - x component of console window origin (fraction of screen width).
<P>
<I>Usage: </I>console-origin-x real
<P>
<I>Default: </I>0.0
<P>

<BR><B>console-origin-y</B><BR>
REAL - y component of console window origin (fraction of screen height).
<P>
<I>Usage: </I>console-origin-y real
<P>
<I>Default: </I>0.0
<P>

<BR><B>console-width</B><BR>
REAL - console window width in fraction of screen width.
<P>
<I>Usage: </I>console-width real
<P>
<I>Default: </I>1.0
<P>

<BR><B>console-type</B><BR>
STRING - console window type. Enclose literal strings in double quotes.
<P>
<I>Usage: </I>console-type &#147;monochrome&#148; | &#147;color&#148;
<P>
<I>Default: </I>&#147;monochrome&#148;
<P>

<BR><B>default-color</B><BR>
INTEGER - the default line color. With a value of -1 ULTRA rotates sequentially 
through the available colors of the current palette. The argument may be an 
integer or any of the color constants, e.g. black, red, blue.
<P>
<I>Usage: </I>default-color integer
<P>
<I>Default: </I>-1
<P>

<BR><B>default-npts</B><BR>
INTEGER - the default number of points used to make new curves (used by span, line, 
etc.). Takes effect for the next curve creation.
<P>
<I>Usage: </I>default-npts integer
<P>
<I>Default: </I>100
<P>

<BR><B>display-name</B><BR>
STRING - display name - either the literal string window or the X server and screen 
(host: display. screen) on which to display plots. Enclose literal strings in double quotes.
<P>
<I>Usage: </I>display-name string
<P>
<I>Default: </I>&#147;window&#148;
<P>

<BR><B>display-title</B><BR>
STRING - display title - string to display in title bar of plot window. Enclose literal 
strings in double quotes. Takes effect when screen is invoked.
<P>
<I>Usage: </I>display-title string
<P>
<I>Default: </I>&#147;ULTRA II&#148;
<P>

<BR><B>display-type</B><BR>
STRING - display type. Enclose literal strings in double quotes.
<P>
<I>Usage: </I>display-type &#147;color&#148; | &#147;monochrome&#148;
<P>
<I>Default: </I>&#147;color&#148;
<P>

<BR><B>error-bar-cap-size</B><BR>
REAL - size of the error bar caps in normalized coordinates.
<P>
<I>Usage: </I>error-bar-cap-size 0.0
<P>
<I>Default: </I>0.018
<P>

<BR><B>graphical-interface</B><BR>
INTEGER - turn on the graphical control panel for ULTRA
<P>
<I>Usage: </I>graphical-interface on | off
<P>
<I>Default: </I>off
<P>

<BR><B>hide-rescale</B><BR>
INTEGER - turn rescaling after hide command on or off.
<P>
<I>Usage: </I>hide-rescale on | off
<P>
<I>Default: </I>off
<P>

<BR><B>label-color-flag</B><BR>
INTEGER - turn printing of labels in same color as corresponding curves on or off.
<P>
<I>Usage: </I>label-color-flag on | off
<P>
<I>Default: </I>off
<P>

<BR><B>label-length</B><BR>
INTEGER - character length of the label shown by lst et al. If label-length is greater 
than or equal to 40, the extrema of the curves will not be printed for the lst or 
menu commands or in curve labels below plots. The extrema are not suppressed in curve labels on plots if the squeeze-labels option is on.
<P>
<I>Usage: </I>label-length integer
<P>
<I>Default: </I>25
<P>

<BR><B>label-space</B><BR>
REAL- fraction of screen window height to add to screen window height for the display 
of curve labels. If label-space is 0 (the default) no curve labels are shown in 
screen windows
<P>
<I>Usage: </I>label-space real
<P>
<I>Default: </I>0.0
<P>

<BR><B>label-type-size</B><BR>
INTEGER - point size for curve labels printed at the bottom of the plot
<P>
<I>Usage: </I>label-type-size integer
<P>
<I>Default: </I>8
<P>

<BR><B>leftspace</B><BR>
REAL - fractional space at left of plot, i.e. between curves and axes.
<P>
<I>Usage: </I>leftspace real
<P>
<I>Default: </I>.01
<P>

<BR><B>lines-page</B><BR>
INTEGER - the number of effective lines per page used by commands such as menu. 
The default value is 40. A value of 0 is used to imply an infinite number of 
lines per page; and any other value less than 26 implies a minimum page length 
of 26 (this is chosen so as not to interfere with the lst command).
<P>
<I>Usage: </I>lines-page integer
<P>
<I>Default: </I>40
<P>

<BR><B>n-curves</B><BR>
INTEGER - total number of curves in the system (returned only, not set).
<P>
<I>Usage: </I>n-curves
<P>
<I>Default: </I>100
<P>

<BR><B>n-curves-read</B><BR>
INTEGER - total number of curves read from files (returned only, not set).
<P>
<I>Usage: </I>n-curves-read
<P>
<BR><B>plot-date
</B<BR

>INTEGER - plot the date, time, and host (if available) on hardcopy devices (PS or CGM).
<P>
<I>Usage: </I>plot-date on | off
<P>
<I>Default: </I>on
<P>

<BR><B>plot-labels</B><BR>
INTEGER - plot the curve labels on output devices (PS or CGM).
<P>
<I>Usage: </I>plot-labels on | off
<P>
<I>Default: </I>on
<P>

<BR><B>plot-type</B><BR>
INTEGER - plot type.
<P>
<I>Usage: </I>plot-type cartesian | polar | insel
<P>
<I>Default: </I>cartesian
<P>

<BR><B>print-flag</B><BR>
INTEGER - turn printing of output values on or off.
<P>
<I>Usage: </I>print-flag on | off
<P>
<I>Default: </I>off
<P>

<BR><B>print-stats</B><BR>
INTEGER - turn printing of Ultra control statistics on or off.
<P>
<I>Usage: </I>print-stats on | off
<P>
<I>Default: </I>off
<P>

<BR><B>prompt</B><BR>
STRING - the prompt. Enclose literal strings in double quotes.
<P>
<I>Usage: </I>prompt string
<P>
<I>Default: </I>&#147;U-&gt;&#148;
<P>

<BR><B>ps-flag</B><BR>
INTEGER - when on, hardcopy will generate a plot in the PostScript file. See the ps-
name command to control file name.
<P>
<I>Usage: </I>ps-flag on | off
<P>
<I>Default: </I>on
<P>

<BR><B>ps-name</B><BR>
STRING - the root part of the name ULTRA II uses when creating a PostScript file for 
output. The extension.ps is always used. Enclose literal strings in double 
quotes.
<P>
<I>Usage: </I>ps-name string
<P>
<I>Default: </I>&#147;plots&#148;
<P>

<BR><B>ps-type</B><BR>
STRING - the type ULTRA II uses when creating a PostScript file for output. Enclose 
literal strings in double quotes.
<P>
<I>Usage: </I>ps-type &#147;monochrome&#148; | &#147;color&#148;
<P>
<I>Default: </I>&#147;monochrome&#148;
<P>

<BR><B>rightspace</B><BR>
REAL - fractional space at right of plot.
<P>
<I>Usage: </I>rightspace real
<P>
<I>Default: </I>.01
<P>

<BR><B>save-intermediate</B><BR>
INTEGER - control whether intermediate curves are kept when top level
expressions are evaluated
<P>
<I>Usage: </I>save-intermediate off
<P>
<I>Default: </I>on
<P>

<BR><B>simple-append</B><BR>
INTEGER - if on the append-curves command sorts and averages curves in any region 
of overlap, otherwise a simple concatenation is done.
<P>
<I>Usage: </I>simple-append on
<P>
<I>Default: </I>off
<P>

<BR><B>smooth-method</B><BR>
STRING - the smoothing method to use for the smo, smooth, smooth3, and, smooth5 
commands. Enclose literal strings in double quotes.
<P>
<I>Usage: </I>smooth-method &#147;fft&#148; | &#147;averaging&#148; | &#147;tchebyshev&#148; | &#147;least-sqr&#148;
<P>
<I>Default: </I>&#147;averaging&#148;
<P>

<BR><B>squeeze-labels</B><BR>
INTEGER - when on, replace contiguous blanks in curve labels on plots with a single 
blank and remove leading and trailing blanks. See the label-length command.
<P>
<I>Usage: </I>squeeze-labels on | off
<P>
<I>Default: </I>off
<P>

<BR><B>topspace</B><BR>
REAL - fractional space at top of plot.
<P>
<I>Usage: </I>topspace real
<P>
<I>Default: </I>0.01
<P>

<BR><B>type-face</B><BR>
STRING - the font type face to be used in plots. Enclose literal strings in double quotes.
<P>
<I>Usage: </I>type-face helvetica | times | courier | string
<P>
<I>Default: </I>helvetica
<P>

<BR><B>type-size</B><BR>
INTEGER - the font type size in points to be used in plots.
<P>
<I>Usage: </I>type-size integer
<P>
<I>Default: </I>12
<P>

<BR><B>type-style</B><BR>
STRING - the font type style to be used in plots. Enclose literal strings in double 
quotes.
<P>
<I>Usage: </I>type-style medium | italic | bold | bold-italic | string
<P>
<I>Default: </I>medium
<P>

<BR><B>use-radians</B><BR>
INTEGER - use radians as the unit of input to the trigonmetric functions if on and use 
degrees otherwise. WARNING: this only work when trig.scm has been loaded
<P>
<I>Usage: </I>use-radians on
<P>
<I>Default: </I>off
<P>

<BR><B>view-height</B><BR>
REAL - viewport height in fraction of window height. Takes effect when screen is 
invoked.
<P>
<I>Usage: </I>view-height real
<P>
<I>Default: </I>0.75
<P>

<BR><B>view-origin-x</B><BR>
REAL - x component of viewport origin (fraction of window width). Takes effect when 
screen is invoked.
<P>
<I>Usage: </I>view-origin-x real
<P>
<I>Default: </I>0.18
<P>

<BR><B>view-origin-y</B><BR>
REAL - y component of viewport origin (fraction of window height). Takes effect when 
screen is invoked.
<P>
<I>Usage: </I>view-origin-y real
<P>
<I>Default: </I>0.2
<P>

<BR><B>view-width</B><BR>
REAL - viewport width in fraction of window width. Takes effect when screen is 
invoked.
<P>
<I>Usage: </I>view-width real
<P>
<I>Default: </I>0.75
<P>

<BR><B>window-height</B><BR>
REAL - plot window height in fraction of screen width. Takes effect when screen is 
invoked.
<P>
<I>Usage: </I>window-height real
<P>
<I>Default: </I>0.48
<P>

<BR><B>window-height-CGM</B><BR>
REAL - CGM window height in fraction of &#147;page&#148; width.
<P>
<I>Usage: </I>window-height-CGM real
<P>
<I>Default: </I>0.0
<P>

<BR><B>window-height-PS</B><BR>
REAL - PostScript window height in fraction of &#147;page&#148; width.
<P>
<I>Usage: </I>window-height-PS real
<P>
<I>Default: </I>0.0
<P>

<BR><B>window-origin-x</B><BR>
REAL - x component of plot window origin (fraction of screen width). Takes effect 
when screen is invoked.
<P>
<I>Usage: </I>window-origin-x real
<P>
<I>Default: </I>0.5
<P>

<BR><B>window-origin-x-CGM</B><BR>
REAL - x component of CGM window origin (fraction of page width).
<P>
<I>Usage: </I>window-origin-x-CGM real
<P>
<I>Default: </I>0.0
<P>

<BR><B>window-origin-x-PS</B><BR>
REAL - x component of PostScript window origin (fraction of page width).
<P>
<I>Usage: </I>window-origin-x-PS real
<P>
<I>Default: </I>0.0
<P>

<BR><B>window-origin-y</B><BR>
REAL - y component of plot window origin (fraction of screen height). Takes effect 
when screen is invoked.
<P>
<I>Usage: </I>window-origin-y real
<P>
<I>Default: </I>0.15
<P>

<BR><B>window-origin-y-CGM</B><BR>
REAL - y component of CGM window origin (fraction of page height).
<P>
<I>Usage: </I>window-origin-y-CGM real
<P>
<I>Default: </I>0.0
<P>

<BR><B>window-origin-y-PS</B><BR>
REAL - y component of PostScript window origin (fraction of page height).
<P>
<I>Usage: </I>window-origin-y-PS real
<P>
<I>Default: </I>0.0
<P>

<BR><B>window-width</B><BR>
REAL - plot window width in fraction of screen width. Takes effect when screen is 
invoked.
<P>
<I>Usage: </I>window-width real
<P>
<I>Default: </I>0.48
<P>

<BR><B>window-width-CGM</B><BR>
REAL - CGM window width in fraction of page width.
<P>
<I>Usage: </I>window-width-CGM real
<P>
<I>Default: </I>0.0
<P>

<BR><B>window-width-PS</B><BR>
REAL - PostScript window width in fraction of page width.
<P>
<I>Usage: </I>window-width-PS real
<P>
<I>Default: </I>0.0
<P>

</BLOCKQUOTE>

<P>

<a name="Udflt"></a>
<h1 ALIGN="CENTER"> The Default ULTRA II Environment</h1>

ULTRA II functions and variables are defined at two levels. At the C level are the
most primitive objects or ones whose run time efficiency is critical. These functions
are compiled into the code. At the SCHEME (interpreter) level are a collection of
functions which can be derived from the primitive C level functions. These functions
are loaded at run time. The user of ULTRA II has no direct control over the C level
functionality; however, the user can change the SCHEME level functionality simply by
editing certain files and restarting ULTRA II.<p>

When ULTRA II starts up it loads in the SCHEME level functions and variables and
then loads in the user&#146;s configuration file. The environment defined by theseo
two parts makes ULTRA II easier to use for most people. This mechanism also gives
sites a convenient way of extending and customizing their environment. The part of
the environment created by the SCHEME level functions and variables can be examined
by any user, especially those seeking examples of how to extend ULTRA with their
own functions. The part of the environment controlled by the user&#146;s
configuration file has more to do with arranging the display and setting up of
each users personal environment. <p>

This section documents the SCHEME level functions and variables as well as the
default environment created by the configuration file distributed
with ULTRA II.<p>

<a name="U64"></a>
<h2>  Constants and Values</h2>

The following local constants are defined mainly to make ULTRA II easier to use.
These provide mnemonic names for quantities that you would otherwise have to
remember or avoid having to type unwanted characters (e.g. &#147;). They are a
part of the SCHEME environment.<p>

<BLOCKQUOTE>
<TABLE>
<TR><TD>cartesian-plot</TD><TD>value for plot-type variable</TD></TR>
<TR><TD>polar-plot</TD><TD>value for plot-type variable</TD></TR>
<TR><TD>insel-plot</TD><TD>value for plot-type variable</TD></TR>
<TR><TD>cartesian</TD><TD>value for axis-type variable</TD></TR>
<TR><TD>polar</TD><TD>value for axis-type variable</TD></TR>
<TR><TD>insel</TD><TD>value for axis-type variable</TD></TR>
<TR><TD>left</TD><TD>value for histogram plot control or text alignment</TD></TR>
<TR><TD>right</TD><TD>value for histogram plot control or text alignment</TD></TR>
<TR><TD>center</TD><TD>value for histogram plot control or text alignment</TD></TR>
<TR><TD>all</TD><TD>value for kill command</TD></TR>
<TR><TD>on</TD><TD>value for variables which toggle off and on</TD></TR>
<TR><TD>off</TD><TD>value for variables which toggle off and on</TD></TR>
<TR><TD>white</TD><TD>value for background color control</TD></TR>
<TR><TD>black</TD><TD>value for background color control</TD></TR>
<TR><TD>auto</TD><TD>value for background color control</TD></TR>
<TR><TD>right-of-axis</TD><TD>value for axis control</TD></TR>
<TR><TD>left-of-axis</TD><TD>value for axis control</TD></TR>
<TR><TD>straddle-axis</TD><TD>value for axis control</TD></TR>
<TR><TD>solid</TD><TD>value for lnstyle command</TD></TR>
<TR><TD>dashed</TD><TD>value for lnstyle command</TD></TR>
<TR><TD>dotted</TD><TD>value for lnstyle command</TD></TR>
<TR><TD>dotdashed</TD><TD>value for lnstyle command</TD></TR>
<TR><TD>plus</TD><TD>value for marker command</TD></TR>
<TR><TD>star</TD><TD>value for marker command</TD></TR>
<TR><TD>triangle</TD><TD>value for marker command</TD></TR>
<TR><TD>&#37;pi</TD><TD>value for pi to be used in expressions</TD></TR>
<TR><TD>&#37;e</TD</TD><TD>value for e to be used in expressions</TD></TR>
<TR><TD>helvetica</TD><TD>value for type-face command</TD></TR>
<TR><TD>times</TD><TD>value for type-face command</TD></TR>
<TR><TD>courier</TD><TD>value for type-face command</TD></TR>
<TR><TD>medium</TD><TD>value for type-style command</TD></TR>
<TR><TD>italic</TD><TD>value for type-style command</TD></TR>
<TR><TD>bold</TD><TD>value for type-style command</TD></TR>
<TR><TD>bold-italic</TD><TD>value for type-style command</TD></TR>
<TR><TD>key-shift</TD><TD>shift key modifier</TD></TR>
<TR><TD>key-cntl</TD><TD>control key modifier</TD></TR>
<TR><TD>key-alt</TD><TD>alt key modifier</TD></TR>
<TR><TD>key-lock</TD><TD>lock key modifier</TD></TR>
<TR><TD>mouse-left</TD><TD>left mouse button</TD></TR>
<TR><TD>mouse-middle</TD><TD>middle mouse button</TD></TR>
<TR><TD>mouse-right</TD><TD>right mouse button</TD></TR>
<TR><TD>mouse-up</TD><TD>mouse up event</TD></TR>
<TR><TD>mouse-down</TD><TD>mouse down event</TD></TR>
</TABLE>
</BLOCKQUOTE>

Color Constants: these are the default colors for ULTRA II and on black and
white systems they are all logically white (the background is logically black).
These may be used with the color command and the default-color command. ULTRA II
will not return the color names but only the values as a result of the
get-attributes command.<p>

<p>

<BLOCKQUOTE>
<TABLE>
<TR><TD>black</TD><TD>black for lines or text</TD></TR>
<TR><TD>white</TD><TD>white for lines or text</TD></TR>
<TR><TD>gray</TD><TD>gray in standard palette for lines or text</TD></TR>
<TR><TD>dark-gray</TD><TD>dark gray in standard palette for lines or text</TD></TR>
<TR><TD>blue</TD><TD>blue in standard palette for lines or text</TD></TR>
<TR><TD>green</TD><TD>green in standard palette for lines or text</TD></TR>
<TR><TD>cyan</TD><TD>cyan in standard palette for lines or text</TD></TR>
<TR><TD>red</TD><TD>red in standard palette for lines or text</TD></TR>
<TR><TD>magenta</TD><TD>magenta in standard palette for lines or text</TD></TR>
<TR><TD>brown</TD><TD>brown in standard palette for lines or text</TD></TR>
<TR><TD>dark-blue</TD><TD>dark blue in standard palette for lines or text</TD></TR>
<TR><TD>dark-green</TD><TD>dark green in standard palette for lines or text</TD></TR>
<TR><TD>dark-cyan</TD><TD>dark cyan in standard palette for lines or text</TD></TR>
<TR><TD>dark-red</TD><TD>dark red in standard palette for lines or text</TD></TR>
<TR><TD>yellow</TD><TD>yellow in standard palette for lines or text</TD></TR>
<TR><TD>purple</TD><TD>purple in standard palette for lines or text</TD></TR>
<TR><TD>no-fill</TD><TD>color value to turn off curve fill</TD></TR>
</TABLE>
</BLOCKQUOTE>

<a name="U66"></a>
<h2> Functions</h2>

The following functions are defined at the SCHEME level. Their descriptions
are found back with the other ULTRA II functions. This list is given for
completeness and to help users to see what kinds of functions can be written
at the SCHEME level. The hope is that this will enable new users of ULTRA II
to see how to extend ULTRA II to meet the needs of their particular application.
The user may look at the coding for these functions by looking in the
directory pointed to by their SCHEME environment variable for the files
func.scm or help.scm<p>

<p>

<B>
<BLOCKQUOTE>

append-na<br>
autoload<br>
autoload*<br>
compare<br>
convol<br>
convolb<br>
copy-curve<br>
correl<br>
delta<br>
diff-measure<br>
diffraction<br>
extract<br>
fode<br>
gaussian<br>
hardcopy<br>
hc1<br>
help<br>
normalize<br>
np<br>
plots<br>
print-menu<br>
re-color<br>
re-id<br>
resume<br>
screen<br>
smo<br>
smooth3<br>
smooth5<br>
span<br>
theta<br>
vs<br>
xmax<br>
xmin<br>
ymax<br>
ymin<br>
</BLOCKQUOTE>
</B>

<a name="U68"></a>
<h2> Synonyms</h2>

The following are the default synonyms for ULTRA II functions. In the list,
the native ULTRA II function is given as the heading for its set of synonyms.
These are defined as a part of the SCHEME environment.<p>

<BLOCKQUOTE>
<TABLE>
<TR><TD>+		</TD><TD>sum</TD></TR>
<TR><TD>-		</TD><TD>dif</TD></TR>
<TR><TD>*		</TD><TD>prod</TD></TR>
<TR><TD>/		</TD><TD>quot, ratio, div</TD></TR>
<TR><TD>compose		</TD><TD>comp</TD></TR>
<TR><TD>derivative	</TD><TD>der</TD></TR>
<TR><TD>domain		</TD><TD>dom</TD></TR>
<TR><TD>erase		</TD><TD>era</TD></TR>
<TR><TD>integrate	</TD><TD>int</TD></TR>
<TR><TD>hardcopy	</TD><TD>hc</TD></TR>
<TR><TD>kill		</TD><TD>expunge</TD></TR>
<TR><TD>log10x		</TD><TD>logx</TD></TR>
<TR><TD>powr		</TD><TD>pow</TD></TR>
<TR><TD>powrx		</TD><TD>powx</TD></TR>
<TR><TD>range		</TD><TD>ran</TD></TR>
<TR><TD>select		</TD><TD>&#35; cur</TD></TR>
<TR><TD>show		</TD><TD>unhide</TD></TR>
<TR><TD>smooth3		</TD><TD>smooth</TD></TR>
</TABLE>
</BLOCKQUOTE>

<a name="U69"></a>
<h2> Display and I/O Environment</h2>

The remainder of the set up has to do with the display and appearance of
ULTRA II at run time. This comes from the user&#146;s configuration file
which sets up the graphics display window and sets some of the variables
mentioned in the section on ULTRA II variables. The actual commands are
given here including the values (which are taken from the UNIX
configuration) as a further set of examples of the ULTRA II variables
involved. NOTE: the viewport variable setting commands are commented
out because the values given are the built-in C level defaults.<p>

<CENTER><B>Window Controls</B></CENTER>
<BLOCKQUOTE>
<PRE>
 (background-color-flag white)

 (border-width     2)

 (window-origin-x  0.5)

 (window-origin-y  0.15)

 (window-width     0.48)

 (window-height    0.48)
</PRE>
</BLOCKQUOTE>

<CENTER><B>Viewport Controls</B></CENTER>
<BLOCKQUOTE>
<PRE>
 ; (view-origin-x  0.18)

 ; (view-origin-y  0.2)

 ; (view-width     0.75)

 ; (view-height    0.75)
</PRE>
</BLOCKQUOTE>

<CENTER><B>Miscellaneous</B></CENTER>
<BLOCKQUOTE>
<PRE>
 (open-device &#147;WINDOW&#148; &#147;COLOR&#148; &#147;ULTRA II&#148;)

 (ps-flag on)

 (print-flag off)
</PRE>
</BLOCKQUOTE>

<a name="Ugraph"></a>
<h1 ALIGN="CENTER">Graphical Output in ULTRA II</h1>

ULTRA II supports two forms of graphical output apart from the screen display.
The first and recommended form is PostScript. The second form is CGM
(Computer Graphics Metafile).<p>

<a name="U70"></a>
<h2> PostScript</h2>

ULTRA II produces a compact (by ASCII text file standards) PostScript file which
conforms to the requirements for Encapsulated PostScript (EPS). These files have
been successfully imported into several desktop publishing systems (DTP). Because
different DTP&#146;s look for different levels of EPS conformance and
specification, ULTRA II cannot write a single canned DSC header for its
PostScript files. Users can specify the header that their target DTP requires
with the ps-name command. To do this they must ascertain the PostScript level
and the EPSF conformance level which their target DTP requires. These two numbers
are then added to the PostScript file name (separated by spaces). For example,
if a particular DTP requires Level 2 PostScript and Level 2 EPSF conformance,
then set the PostScript file name as follows:

<BLOCKQUOTE>
ps-name &#147;foobar 2.0 2.0&#148;
</BLOCKQUOTE>

The resulting PostScript file, foobar.ps, would have the following header:

<BLOCKQUOTE>
%!PS-Adobe-2.0 EPSF-2.0
</BLOCKQUOTE>

An alternate way of importing the PostScript file into your DTP system, is to
edit the first line in the PostScript file to reflect the appropriate level.
For example if your favorite DTP program requires level 2 conformance you
would change the first line of the PostScript file from 

<BLOCKQUOTE>
%!PS-Adobe-3.0
</BLOCKQUOTE>
to
<BLOCKQUOTE>
%!PS-Adobe-3.0 EPSF-2.0
</BLOCKQUOTE>

ULTRA II does not write its PostScript files to conform to the EPSI (Encapsulated
PostScript Interchange) standard. EPSI files contain a raster image of the page
in their header and DTP systems will display this raster image in the bounding
box when an EPSI file is imported into a document. With plain EPS files, the
bounding box indicating the placement of an imported PostScript file is typically
shown as a featureless (perhaps gray) region on the page.<p>

The authority for PostScript conformance is: <U>PostScript Language Reference
Manual</U>, 2nd edition, by Adobe Systems Incorporated and published by Addison
Wesley.<p>

<a name="U71"></a>
<h2> CGM</h2>

ULTRA II also will write a binary CGM file. It is our opinion that PostScript is
a superior standard for graphical output by virtue of its more complete and
unambiguous definition. Presumably, you might chose to write a binary CGM file
instead of a PostScript file because it is somewhat faster and produces a smaller
file. It makes no sense whatsoever to write a clear text CGM file which is larger
than a PostScript file, and compressing a PostScript file will result in a file
that is comparable to a binary CGM file in size.<p>

Every effort has been made to conform to the CGM standard, but because of certain
ambiguities and outright bugs in some popular CGM viewers, we discourage you from
using CGM as your graphical output medium of choice.<p>

The authority for CGM conformance is: ANSI X3.122-1986, <U>Computer Graphics
Metafile for the Storage and Transfer of Picture Description Information</U>.<p>

<a name="Uextend"></a>
<h1 ALIGN="CENTER">Extending ULTRA II</h1>

ULTRA II has a facility that allows you to build your own extensions to the code.
This is accomplished through a SCHEME interpreter. SCHEME is a dialect of the
LISP programming language. ULTRA II can be viewed as an extended SCHEME dialect
since you are talking to the interpreter when you talk to ULTRA II. It is ideal
for the sort of controlling capacity and extensibility it gives to ULTRA II. It
also permits you to customize ULTRA to suit your own tastes.<p>

For many of the purely presentation and manipulation functions that are done
with ULTRA II the discussion below may suffice. If you want to take full
advantage of the power of the SCHEME interpreter, see the SX User&#146;s Manual
listed in the <a href="#Udocs2">documentation section</a> and/or consult any book on
LISP for a grounding in the principles involved. You may also study the examples
provided with the distribution of ULTRA II.<p>

A rough analogy of the ability to customize ULTRA II is the facility for
customizing a UNIX environment which the C or Bourne shells give to their
users. Not only can new commands be made but old ones may be given additional
or alternate names with the synonym command. This allows you to define synonyms
for other functions so that you may create a set of commands with names which
are more mnemonic.<p>

For example, in the file synon.scm included with this release the following
synonyms are defined:<p>

<PRE>
          (synonym erase era)

          (synonym select # cur)
</PRE>

The first example defines era as a synonym for the erase procedure while
the second defines both # and cur as synonyms for the select procedure.<p>

The most important advantage of having the SCHEME interpreter in ULTRA II is
that you can write your own procedures and add any new capability to the system.
Furthermore, new capabilities happen right away, not whenever we (the maintainers
of ULTRA II) get around to it.<p>

Another benefit that accompanies the presence of the SCHEME interpreter is the
view of curves and commands. In SCHEME everything is an object including curves
and commands. Variables are objects, numbers are objects, and procedures are
objects. Furthermore all objects are treated the same way. This property, which
is shared with all LISP dialects, gives SCHEME the ability to manipulate programs
as data and data as programs. The fundamental function of the SCHEME interpreter
is to read in objects, evaluate them in some way which depends on the object, and
print out the result of that evaluation. The process of evaluating an object
results in another object being returned. This is the crucial point: evaluating
an object (especially a procedure object) yields another object! The following
table will demonstrate some of the object types and how they evaluate
(==&gt; means evaluates to):<p>

<PRE>
           integer		==&gt; integer (self evaluating)

           float		==&gt; float (self evaluating)

           string		==&gt; string (self evaluating)

           variable		==&gt; some assigned value

           procedure		==&gt; result of the procedure
</PRE>

A quick word on SCHEME syntax. Function calls are expressed as lists with the
procedure first and the arguments next (i.e., in prefix or Polish notation).
A SCHEME list begins with a left parenthesis, has space delimited entries,
and ends with a right parenthesis.<p>

<PRE>
          (foo 1 &#147;this is a string&#148; variable (list a b c) 3.14159)
</PRE>

Here is the point of all this talk about objects. All commands in ULTRA II now
return objects. The power of this can be hinted at with an example. Without the
SCHEME interpreter in ULTRA II if you wanted to read a curve in from a file,
take the sin of it and shift the result by 5 you would do the following:<p>

<PRE>
          cur 1

          sin a

          dx a 5
</PRE>

With the SCHEME interpreter, you would do the following:<p>

<PRE>
          dx (sin (cur 1)) 5
</PRE>
This is because the command cur returns a curve object which the sin command
takes as an argument just as the old version required. In turn the sin command
returns a curve object which the dx command requires. Ultimately, the dx command
returns a curve object as the final result of evaluating the expression in the
example. Notice that all of these things were done in a single command, but more
importantly we did not have to know whether it was curve A on the display or
curve N!<p>

There are two types of functions in ULTRA. The first type which will be referred
to as a procedure has all of its arguments evaluated before being passed to the
function. The second type called a macro does NOT evaluate its arguments but
passes them straight to the function. An example will illustrate the difference.
Consider two versions of a function which simply returns its argument,
proc-return which is a procedure and macro-return which is a macro.<p>

<PRE>
          (proc-return (+ 1 2 3)) ==&gt; 6

          (macro-return (+ 1 2 3)) ==&gt; (+ 1 2 3)
</PRE>

In the first case the argument, (+ 1 2 3), is evaluated (the result is 6) and 6
is handed to the function proc-return, while in the second case, the argument,
(+ 1 2 3), is not evaluated and (+ 1 2 3) is handed to the function
macro-return.<p>

Another example of immediate importance is:<p>

<PRE>
                rd testdata

                rd* testdata
</PRE>

These two commands rd and rd* open a file named by the argument and read the
curves in it. rd is a macro and the variable, testdata, is passed to it without
having been evaluated. rd will attempt to find a file with the same name as the
variable, i.e., &#147;testdata&#148;. On the other hand, rd* is a procedure and
will attempt to evaluate the variable, testdata. If the variable, testdata has
not been assigned a value SCHEME will complain about it and do nothing, and if
testdata has been assigned a value, unless the value is the string
&#147;testdata&#148; the desired result of opening the file named,
&#147;testdata&#148; will not happen. To use rd* from the keyboard you would
have to do:<p>

<PRE>
               rd* &#147;testdata&#148;
</PRE>

This can be a very confusing point especially since if you want to write a
procedure which opens up a data file whose name has been passed in as an
argument you will need to use rd* because it evaluates its argument!<p>

In ULTRA II, commands are expressed as lists as above except that the parentheses
around commands typed at the keyboard are assumed. For example the command cited
above:<p>

<PRE>
          dx (sin (cur 1)) 5
</PRE>

is transformed into<p>

<PRE>
          (dx (sin (cur 1)) 5)
</PRE>

before it is handed to the interpreter.<p>

<a name="U74"></a>
<h2> Cookbook Examples</h2>

In this section two examples of extending ULTRA II are given and discussed.<p>

In the source listing given you should note that the &#147;;&#148; character
means treat the remainder of the line as a comment. In these examples all
opening parentheses have a matching closing parenthesis and you should pay
very close attention to which pairs match up!<p>

These examples can be typed into a text file, for example test.scm, and
loaded into an ULTRA II session with the ld command by typing<p>

<PRE>
          ld test.scm
</PRE>

<a name="U75"></a>
<h3> Gaussian Curve Generator</h3>

Suppose you want ULTRA II to generate a gaussian function given an amplitude, a
half-width, and a center point (this is in fact the code for the gaussian command
and can be found in func.scm in the directory pointed to by the SCHEME
environment variable). The following function accomplishes this:<p>

<BLOCKQUOTE>
<PRE> ; 
 ; Usage: gaussian &lt;amplitude&gt; &lt;FWHM&gt; &lt;x position of peak&gt; &lt;optional # points&gt;
 ; 
 ; Note: # of points defaults to 100
 ; 

 (define (gaussian amp wid center . npts)

    (letrec ((crv-min (- center (* 3 wid)))
             (crv-max (+ center (* 3 wid)))
             (num (if npts (car npts) 100))
             (c (span crv-min crv-max num))
             (d (- c center)))

            (del c)
            (set! c (* d d -1))
            (del d)
            (set! d (exp (/ c (* wid wid))))
            (del c)
            (set! c (* d amp))
            (del d)

            (label c "Gaussian")))
</PRE>
</BLOCKQUOTE>

The first line of this function definition specifies that the function is
named gaussian and has three required arguments (amp, wid, and center) and
one optional argument, npts.<p>

The next line begins a block of local variable declarations (crv-min, crv-max,
num, c, and d). In this example all of the local variables are given initial
values. So, for example, crv-min is assigned to the result of evaluating the
expression (- center (* 3 wid)). The values given to the local variables are:
crv-min, the minimum x value for the resulting gaussian curve; crv-max, the
maximum x value of the resulting curve; num, the number of points in the curve;
c, a linear curve with num points from crv-min to crv-max; and d, a curve built
from the curve c and the value center.<p>

The next commands alternate between generating intermediate results and removing
results that are no longer needed (you probably don&#146;t care to see these
results when everything is finished). The set! command is a part of SCHEME and
assigns to the first argument the value of its second argument.<p>

The last step in this function is to use the label command to label the
resulting curve.<p>

<a name="U76"></a>
<h3> First Order Differential Equation Solver</h3>

In this example, the equation y&#146; + ay = b is solved over the domain from
xmin to xmax, where a and b are two curves currently displayed and xmin and xmax
are two scalar values. The result is two curves containing the homogeneous and
particular solutions to the equation. This example is also taken from the file
func.scm and shows the ULTRA II fode command.<p>

In contrast to the last example there are no optional arguments to
this function.<p>

Notice how curves and scalars mix in algebraic expressions.<p>


<BLOCKQUOTE>
<PRE> ; 
 ; first order line ode solver
 ; 

 (define (fode a b xmin xmax)

     (letrec ((integrand-a (copy a))
              (particular-y (copy a))
              (homogeneous-y (copy a))
              (integral-a (integrate integrand-a xmin xmax))
              (expia (exp (copy integral-a)))
              (expmia (exp (my (copy integral-a) -1)))
              (integrand (* b expia)))

 ; 
 ; compute the homogeneous solution
 ; 

             (del expia integral-a homogeneous-y integrand-a)
             (set! homogeneous-y (copy expmia))
             (label homogeneous-y "Homogeneous solution")

 ; 
 ; compute the particular solution
 ; 

             (set! expia (integrate integrand xmin xmax))
             (del particular-y)
             (set! particular-y (* expmia expia))
             (label particular-y "Particular solution")
             (del expia expmia integrand)
             (list homogeneous-y particular-y)))
</PRE>
</BLOCKQUOTE>

<a name="Uinst"></a>
<h1 ALIGN="CENTER">Configuration/Availability</h1>

ULTRA II runs on a variety of platforms. For the present purposes those
platforms are characterized as UNIX platforms and Apple
Macintosh platforms. The following sections tell how to configure 
ULTRA II to your needs.<p>

<a name="U77"></a>
<h2> UNIX</h2>

ULTRA II is built as a part of the PACT distribution and installation process.
The PACT User&#146;s Guide (see the <a href="#Udocs2">Related Documentation</a>
section) describes how to build and install the entire PACT distribution. When
the distribution is built, ULTRA II users must configure their environment to
run ULTRA II. In the following steps you will need to know where the default
configuration file is kept (see your system administrator or the person who
installed the PACT distribution). For the sake of example, suppose that the
default configuration file is kept in the directory, /usr/local/scheme. It is
assumed that you have put the directory containing the ULTRA II executable on
your path variable. You will also need to know how to use a text editor to
modify your configuration file, ultra.ini or .ultrarc.<p>

<B>1. Go to your home directory.</B>
<UL> cd ~ </UL>

<B>2. Copy the default configuration file from
the source directory into your home directory.</B>
<UL> cp /usr/local/scheme/ultra.ini .ultrarc </UL>

<B>3. Make desired changes to .ultrarc to customize your configuration file.</B>
<P>

<B>4. Add the ULTRA environment variable to your .cshrc file.</B>
<UL> setenv ULTRA /usr/local/scheme </UL>

<B>5. For X Windows sites, verify your DISPLAY environment variable.</B>
<P>

<B>6. Run ULTRA II.</B>
<UL> ultra </UL>

<P>

If the custom configuration file, .ultrarc, exists in your home directory, ULTRA
will load its contents; otherwise, ULTRA will load the contents of the default
configuration file, ultra.ini, located in the directory indicated by the ULTRA
environment variable. If you wish to use the default configuration file, omit
steps 2 and 3.<p>

<a name="Udocs1"></a>
<h1 ALIGN="CENTER">Internal Documentation</h1>

There are two facilities for obtaining on-line help, help and apropos. help
gives you the description of a command, a variable, a category, or a general
overview of the help system. Some examples of the usage of help are:<p>

<PRE>
          help range

          help ps-name

          help sy

          help

</PRE>

If you don&#146;t know the name of a command, then apropos is the command of
choice. It searches the names of the commands and their descriptions for the
string that you give it. If you wanted to know how to take the logarithm of
a curve but weren&#146;t sure of the command, you might type<p>

<PRE>
          apropos log
</PRE>

you would get a listing of all the commands that had anything to do with
&#147;log&#148;. You may also get a few surprises since any description where
the sequence &#147;log&#148; appears will be shown; nevertheless, you should
find the information you need. <p>

In these descriptions, the sequence curve-list means a list of space delimited
letters which are the data-id&#146;s of curves and/or ranges of data-id&#146;s.
If you wanted to exponentiate curves a and c you would type<p>

<PRE>
          exp a c
</PRE>

If you wanted the logarithm base 10 of curves a through f you would type<p>

<PRE>
          log10 a:f
</PRE>

The colon notation is shorthand for the thru procedure.<p>

<a name="Udocs2"></a>
<h1 ALIGN="CENTER">Related Documents</h1>

ULTRA II is one part of PACT, a set of tools for portable code development
and visualization. Users of ULTRA II may be interested in the other parts
of PACT especially since the various aspects of ULTRA II are derived from
PACT. That is the graphics is handled by PGS; the binary file handling is
done by PDBLib; and the main control and extensibility functionality is
provided by the PACT SCHEME interpreter.<p>

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

<p>

<br>
<hr>
For questions and comments, 
please contact the <A HREF="pactteam.html">PACT Development Team</A>.

<hr>
Last Updated: $MOD
