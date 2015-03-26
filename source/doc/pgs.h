TXT: PGS User's Manual
MOD: 03/26/2015

<CENTER>
<P>
<H1><font color="#ff0000">$TXT</font></H1>
<H2>Portable Graphics System</H2>
<H4>Stewart A. Brown
</CENTER>

<P>
<HR>

<ul>
<li><a href="#PGSintro">Introduction</a>

<li><a href="#PGSover">Overview of PGS</a>
<ul>
<li><a href="#PGS21">The PGS Device Model</a>
<li><a href="#PGS22">The PGS Drawing Model</a>
<ul>
<li><a href="#PGS23">PGS Window Placement</a>
<li><a href="#PGS24">Frames, Viewspaces, and View Boundaries</a>
<li><a href="#PGS25">Coordinate Systems</a>
<li><a href="#PGS26">Axis Drawing Notes</a>
</ul>
</ul>

<li><a href="#PGSuser">The PGS User Interface Model</a>
<ul>
<li><a href="#PGS3a">A Textual Interface</a>
<li><a href="#PGS3b">Adding Abstraction Barriers</a>
<li><a href="#PGS0075">Multiplexed I/O and Interrupt Driven I/O</a>
<li><a href="#PGS0080">Event Handling</a>
<ul>
<li><a href="#PGS0093">Event Handling Functions</a>
<li><a href="#PGS0113">Event Related Macros</a>
</ul>
<li><a href="#PGS0132">Interface Objects</a>
<ul>
<li><a href="#PGS0149">Event Handling and Interface Objects</a>
<li><a href="#PGS0154">Portable User Interface Description</a>
</ul>
<li><a href="#PGS0195">PGS Interface Objects</a>
<ul>
<li><a href="#PGS0217">Buttons</a>
<li><a href="#PGS0222">Enumerable Variables</a>
<li><a href="#PGS0246">Nonenumerable Variables</a>
<li><a href="#PGS0252">Sliders</a>
<li><a href="#PGS0270">Transients</a>
</ul>
</ul>

<li><a href="#PGSrender">Rendering Model</a>
<ul>
<li><a href="#PGS2129">Data Structures</a>
<li><a href="#PGS2130">Rendering Modes</a>
<li><a href="#PGS0983">Rendering Attributes</a>
<ul>
<li><a href="#PGS2131a">Examples of Setting and Getting Attribute Values</a>
<li><a href="#PGS2131b">Attributes</a>
<li><a href="#PGS2161">Plots and associated attributes</a>
</ul>
</ul>

<li><a href="#PGSapi">The PGS API</a>
<ul>
<li><a href="#PGS0307">Compiling and Loading</a>
<li><a href="#PGS0316">PGS Functionality</a>
<ul>
<li><a href="#PGS0318">Global State Setting Routines</a>
<li><a href="#PGS0330">Global State Query Routines</a>
<li><a href="#PGS0331">Memory Management Routines</a>
<li><a href="#PGS0373">Device Control Routines</a>
<li><a href="#PGS0525">Device Query Routines</a>
<li><a href="#PGS0581">Coordinate Transformation Routines</a>
<li><a href="#PGS0604">Coordinate System and Viewspace Control Routines</a>
<li><a href="#PGS0652">Line Attribute Control Routines</a>
<li><a href="#PGS0675">Text Attribute Control Routines</a>
<li><a href="#PGS0762">Graphical Text I/O Routines</a>
<li><a href="#PGS0785">Point Move Routines</a>
<li><a href="#PGS0799">Primitive Drawing Routines</a>
<li><a href="#PGS0830">Basic Line Drawing Routines</a>
<li><a href="#PGS0857">Axis Drawing Routines</a>
<li><a href="#PGS0915">Colormap Related Routines</a>
<li><a href="#PGS0941">Graph Control Routines</a>
<li><a href="#PGS0975">Line Plot Routines</a>
<li><a href="#PGS1002">Contour Plotting Routines</a>
<li><a href="#PGS1019">Filled Polygon Plot Routines</a>
<li><a href="#PGS1026">Image Plot Routines</a>
<li><a href="#PGS1042">Surface Plot Routines</a>
<li><a href="#PGS1070">Vector Plot Routines</a>
<li><a href="#PGS1096">Level Diagram Plot Routine</a>
<li><a href="#PGS1103">Graphical Interface Routines</a>
</ul>
<li><a href="#PGSgen">PGS Functions</a>
<li><a href="#PGS1135">Structures</a>
<li><a href="#PGS1154">PGS Constants</a>
</ul>

<li><a href="#PGSgloss">Glossary</a>

<li><a href="#PGSexample">PGS By Example</a>
<ul>
<li><a href="#PGS1267">Common Call Sequences</a>
<ul>
<li><a href="#PGS2354">Initializing a Device</a>
<li><a href="#PGS2358">Making a Picture</a>
<li><a href="#PGS2434">Setting Attributes</a>
</ul>
<li><a href="#PGS1268">Text Placement and Drawing</a>
<li><a href="#PGS1409">Line Drawing</a>
<li><a href="#PGS1499">Line Plots</a>
<li><a href="#PGS1612">Contour Plots</a>
<li><a href="#PGS1701">Image Plots</a>
<li><a href="#PGS1793">Surface Plots</a>
<li><a href="#PGS1946">Vector Plots</a>
<li><a href="#PGS1948">FORTRAN API Example</a>
</ul>

<li><a href="#PGSdocs">Other PACT Documentation</a>
</ul>

<hr>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGSintro"></a>
<h1 ALIGN="CENTER"> Introduction</h1>

One of the biggest headaches for portability is graphics. The objective
evidence is that the field is immature. One cannot exactly say that there
are no graphics standards. The real problem is that there are too many
standards.<p>

PGS is an application program interface (API) that is independent of the
underlying host graphics system. All of the graphics portability headaches
are confined to PGS and applications which use PGS are completely portable.
PGS currently sits on top of X Windows on UNIX platforms, PostScript,
CGM, PNG, and JPEG.<p>

PGS uses a model in which graphics devices such as display windows and
PostScript files are represented by a structure which contains all of their
state information. Then PGS can manage an arbitrary number of devices
simultaneously and any picture can be drawn to any device.<p>

The current version of PGS supports 2D and 3D renderings.  To do this the
concepts of windows and viewports have been generalized.  See the section
<a href="#PGS24">Frames, Viewspaces, and View Boundaries</a> for details.

PGS supports both line and text drawing primitives, line and text attributes,
and bit maps for handling images and other cell array data. Most functionality
in PGS is either primitive operations such as moves and draws or at a very
high level such as axis drawing and the rendering algorithms that it supports.
These rendering algorithms have two interfaces: one for &#147;raw&#148; data;
and one for PML type mappings. This gives a great deal of flexibility to the
application developer.<p>

PGS has the following rendering algorithms currently: 1D line plots; 2D and 3D
contour plots; 2D vector plots; 2D image plots; 3D wire frame mesh and
surface plots (for 2D data sets); and Grotrian diagram plots.<p>

PGS has primitives for 2D and 3D line, polyline, disjointed polyline, and
text drawing.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGSover"></a>
<h1 ALIGN="CENTER"> Overview of PGS</h1>

PGS has two main goals: to provide a portable interface to various host
graphics systems; and to provide high level functionality to applications
which most host graphics systems do not provide. To meet these goals it was
necessary to develop a model of the kinds of devices which the various host
graphics systems support so that the functional interface could be defined
and implemented.<p>

It was also necessary to identify which graphics primitives to support. Some
host graphics systems have a very rich supply of graphics primitives. In fact,
some go way beyond supplying primitive graphics functionality and provide
high level rendering capabilities. Other host graphics systems have a
relatively small set of primitive graphics operations. The decision was made
to design PGS so as to depend on as small a subset of graphics primitives as
possible. This makes PGS extremely portable. It also forces PGS to either
implement or forego higher level graphics functionality. Hopefully, a useful
balance has been struck on this issue.<p>

As the above discussion implies, PGS has two obvious layers. The first layer
is a low level one that communicates directly with the host graphics system.
The other layer is oriented more toward the application and includes the high
level rendering and axis drawing functions. This layer actually breaks down
into several layers. The details of this breakdown will be discussed as
appropriate.<p>

The remainder of this section discusses the PGS device model and the
PGS drawing model.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS21"></a>
<h2> The PGS Device Model</h2>

To provide the maximum degree of portability in what is an inherently platform
dependent field, the attempt has been made to isolate all of the platform
dependencies behind abstraction barriers. The functional interface provides
one set of abstraction barriers. This however does not help with the problem
of the notion of graphics state. Graphics state consists of information such
as the current default line attributes, default text attributes, coordinate
systems, and so on. Furthermore, in a general purpose setting, an application
may wish to manage multiple independent devices each with its own separate
graphics state.<p>

First, PGS defines a graphics device as an abstract entity containing a
logical two dimensional drawing surface and a set of parameters which
describe how and where all drawing functions are to be performed on
the drawing surface.<p>

Nearly all host graphics systems have their own abstraction barrier wrapped
around their drawing surface(s). However, they almost all provide a pointer
or index to applications to specify which drawing surface is intended for a
particular operation. This latter point is most relevant to windows on
display screens, although a file indicator is the correct analog for
PostScript or CGM type devices. In any case, PGS handles the interface to
the host graphics system and hides it from applications. In the place of
the host graphics device indicator, PGS supplies a structure called a
PG_device. Applications open and manipulate PG_device&#146;s only. This way
all PGS based applications have a single portable interface to all devices
supported by PGS.<p>

The PG_device not only contains the host graphics device indicators, it also
contains the graphics state for each device. In this way each PG_device is
independent of every other PG_device. This gives applications the ability to
draw the same picture on every device by simply changing the PG_device passed
to the drawing functions. No device conditional logic is required of
applications.<p>

The host graphics systems supported by PGS currently are:<p>

<BLOCKQUOTE>
X Windows
<P>
PostScript
<P>
CGM (Computer Graphics Metafile)
<P>
Quickdraw (Apple Macintosh)
<P>
Microsoft Graphics Library
</BLOCKQUOTE>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS22"></a>
<h2> The PGS Drawing Model</h2>

Almost all host graphics systems employ a drawing model. This specifies
information such as coordinate systems and their origins, how clipping is
done, and so on. PGS has a somewhat more difficult time coming up with a
drawing model since it must present a drawing model that is compatible with
all host graphics systems even when the various host graphics systems are
in conflict with one another. PGS accomplishes this task by using the least
common denominator of the host systems, defining as much of the drawing model
as possible, and mapping host graphics systems models into the PGS model.<p>

A PG_device can be thought of as a window on the display area of the host
graphics system. In that view, a PGS window can be defined as the region of
the host graphics system display surface controlled by PGS during drawing
operations. PGS windows then map naturally onto the kinds of windows associated
with such host graphics systems as X Windows and Quickdraw. This
idea also has application to a PostScript or CGM device.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS23"></a>
<h3> PGS Window Placement</h3>

In placing a PGS window on a display
screen or a PostScript page, the position of the upper left corner of the
PGS window is given in normalized coordinates relative to a coordinate
origin in the upper left corner of the host graphics system device.<p>

<img src="pgs1.gif">

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS24"></a>
<h3> Frames, Viewspaces, and View Boundaries</h3>

Any part of the interior of a PGS window may be drawn on by PGS
routines. Windows do have some additional structure to help applications
conveniently handle high level plotting constructs.<p>

PGS starts with a "generalized" window - which can be a screen window
or a page in a PostScript file.  A window is divided into frames so
that applications can draw multiple pictures (one per frame) in a window.
A frame contains a viewport which is defined in coordinates normalized to
unity with respect to the frame.  The use may also define a "world"
coordinate system with respect to the viewport.  Most renderings are done
inside the viewport.  Legends and annotations are done outside the viewport
for the most part.  The viewport is surrounded by a "boundary" region.  The
axes are drawn on the boundary.  This permits a user controlled standoff
between data and the axes.

The current version of PGS supports 2D and 3D renderings.  To do this the
window, frame, boundary, and viewport are generalized to be 3D rectangular
boxes.  To help emphasize the point we now use the term viewspace rather
than viewport.  Data is rendered in the viewspace.  For 3D renderings the
view angle and lighting angles are handled by the renderers as the data
is projected onto the 2D surface of the actual output media.

The API has been restructured so that the relevant drawing and rendering
routines take the number of dimensions and the coordinate system.  The
following constants are used to specify the coordinate system:

<blockquote>
<center>
<table>
<tr><td>C</td><td>Scheme</td></tr>
<tr><td>FRAMEC</td><td>@frame</td></tr>
<tr><td>BOUNDC</td><td>@bound</td></tr>
<tr><td>WORLDC</td><td>@world</td></tr>
<tr><td>NORMC</td><td>@norm</td></tr>
<tr><td>PIXELC</td><td>@pixel</td></tr>
</table>
</blockquote>
</center>
<p>

A window may be partitioned into frames with a view to drawing more
than one plot at a time. Within each frame there is a preferred drawing
area called the viewspace. The viewspace is defined relative to its
enclosing frame and by default each window has a single frame which is
the same size as the window. PGS supports clipping which can render
it impossible to draw to any part of the window but the current viewspace.<p>

In addition to the viewspace there is a bounding region of the viewspace
which is used to offset axes from the viewspace so that there is a nice,
application controllable separation between the axes and whatever is drawn
in the viewspace. This bounding region is called the view boundary.<p>


<img src="pgs2.gif">

<P>
The viewspace and view boundary are tied together. When an application
defines the viewspace, the view boundary is implicitly defined in terms of
offsets from the viewspace. These are referred to as topspace, leftspace,
rightspace, and botspace.<p>

The application can move the frame or viewspace around in the PGS window
at any time as well as altering its size.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS25"></a>
<h3> Coordinate Systems</h3>

Inside a PGS window there are three coordinate systems: world
coordinates, normalized coordinates, and pixel coordinates. World
coordinates are application defined and have whatever meaning the application
requires. The lower left corner of the viewspace corresponds to the
minium x and y values of the world coordinate domain. Normalized coordinates
represent the fraction of the PGS window width and height that a point is
from the origin which is in the lower left corner of the PGS window. Pixel
coordinates represent the integer number of pixels that a point is from the
origin which is in the lower left corner of the PGS window.<p>

PGS supplies a set of macros to convert between all of the coordinate
systems which a PGS window may have.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS26"></a>
<h3> Axis Drawing Notes</h3>

When the difference in the limits of the axis label values
approaches or falls below the precision with which the label
values are printed, the resulting axis would be less informative
than you would like.  PGS divides this problem into two cases:<p>

<B>i) the axis labels are printed with an E format (e.g. %10.2e)</B><p>

In this case PGS selects one label value (usually the minimum)
as a reference value and and prints it with a '~' character.
All other label values on that axis are printed relative to the
the reference value.  For example, suppose the axis limits were
1000.0 to 1001.0, the precision 2 decimal places, and three label
to be printed.  The labels would be printed as:<p>

<BLOCKQUOTE>
~1.00e+03 &nbsp&nbsp&nbsp 5.00e-01 &nbsp&nbsp&nbsp 1.00e-00<p>
</BLOCKQUOTE>

<B>ii) the axis labels are printed with an F or G format (e.g. %10.2g)</B><p>

In this case PGS selects one label value (usually the minimum)
as a reference value and and prints it with a '>' character.
All other label values on that axis are printed relative to the
the reference value.  For example, suppose the axis limits were
1000.0 to 1001.0, the precision 2 decimal places, and three label
to be printed.  The labels would be printed as:<p>

<BLOCKQUOTE>
&gt;1000.00 &nbsp&nbsp&nbsp 0.50 &nbsp&nbsp&nbsp 1.00<p>
</BLOCKQUOTE>

This scheme was chosen keeping the space limitations in mind. Putting a
separate label for the overall base or scale has serious ramifications
for plotting space.<p>

NOTE: this does not currently apply to log axes.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGSuser"></a>
<h1 ALIGN="CENTER"> The PGS User Interface Model</h1>

The subject of user interfaces can be a rather complicated one. For many
applications the most natural interface is a graphical one. However, some
systems make the mistake of insisting that a graphical interface is the only
interface which an application may have. PGS supports a model of user
interfaces that permits the application developer to seek the natural
interface for his or her application. This means that PGS supports development
of textual interfaces, graphical interfaces, and hybrids. In the
following sections, we will discuss the concepts underlying user interfaces
from PGS&#146;s point of view (PGS emphasizes portability and flexibility
especially).<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS3a"></a>
<h2> A Textual Interface</h2>

Consider the following program fragment which is typical of an application
with a textual interface:<p>

<BLOCKQUOTE>
<PRE>
char s[MAXLINE], *t;
char *dispatch(char *s);

printf("-> ");
while (fgets(s, MAXLINE, stdin) != NULL)
   {t = dispatch(s);
    printf("%s\n-> ", t);};
</PRE>             
</BLOCKQUOTE>

This code prints a prompt, gets some input (fgets), processes it (dispatch), and
prints the result. It does this in a loop until something ends the program.<p>

For a program so simple and ordinary, it is astonishing how difficult some systems
make it to run this code. One of the peg points of PGS is that it must be
&#147;easy&#148; to run such an application whether the system wants to make it easy
or hard. Some of the graphical user interface models can be ported with some careful
abstraction barriers (and PGS does this too), but this example is something of a
lowest common denominator. So we will start with this and build up a model that
supports this simple text driven style and the most elaborate graphical application.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS3b"></a>
<h2> Adding Abstraction Barriers</h2>

By adding two macros and two function pointers we can make an enormous shift
in the portability of this program:<p>

<UL>
#define GETLN (*getln)
<P>#define PRINT (*putln)
<P>char *(*getln)(char *s, int n, FILE *fp)
<P>int (*putln)(FILE *fp, char *fmt, ...)
</UL>

<P>

These items are defined in score.h which is #included by pgs.h<p>

With these elements we can modify the original example as follows:<p>

<BLOCKQUOTE>
<PRE>char s[MAXLINE], *t;
char *dispatch(char *s);

getln = fgets;
putln = fprintf;

PRINT(stdout, "-> ");

while (GETLN(s, MAXLINE, stdin) != NULL)
    {t = dispatch(s);
     PRINT(stdout, "%s\n-> ", t);};
</PRE>
</BLOCKQUOTE>

This doesn&#146;t look like much at all, but the impact of this change is
enormous! Now we can insert other functions which are call compatible with
the standard C library functions, fgets and fprintf. PGS supplies two such
functions: PG_wind_fgets and PG_fprintf. In fact, when a call to
PG_open_device or PG_open_console is made these functions are connected
to getln and putln for you!<p>

PG_wind_fgets, in addition to looking for input from the terminal as fgets
does, also looks for events from the windowing system under which the
application is running. PG_fprintf prints your formatted text to a terminal
or to a screen window depending on what is appropriate to the system on
which the application is running.<p>

This example can be filled out to a complete program (modulo the
definition of the dispatch function) which is completely portable:<p>

<BLOCKQUOTE>
<PRE>#include &lt;pgs.h&gt;

main(int c, char **v)
   {char s[MAXLINE], *t;
    char *dispatch(char *s);

    PG_open_console("test", "COLOR", TRUE, 0.1, 0.7, 0.5, 0.3);

    PRINT(stdout, "-> ");
    while (GETLN(s, MAXLINE, stdin) != NULL)
       {t = dispatch(s);
        PRINT(stdout, "%s\n-> ", t);};

    return(0);}
</PRE>
</BLOCKQUOTE>

There is an important issue remaining here and that is the subject
of the next section.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS0075"></a>
<h2> Multiplexed I/O and Interrupt Driven I/O</h2>


In the program we have been discussing, input is gathered from either the
terminal or the windowing system. This is an example of multiplexed I/O.
Many applications use multiplexed I/O. It is common in networking programs
for example. With multiplexed I/O a variety of input sources are polled to
see whether there is any input ready. Depending on the device that has input,
the application takes the appropriate action as it becomes available. In the
more efficient applications the operating system is usually involved since
it is better able to control machine resources than any application.<p>

Our sample program doesn&#146;t necessarily need to do multiplexed I/O (on
the other hand we haven&#146;t said what the dispatch function does!). If it
were a graphical application however the chances are that it would have to
handle input from both the terminal and from the windowing system. GETLN,
more specifically PG_wind_fgets, does just that. If the specified FILE
pointer is stdin, it obtains input from either source, and copies terminal
input into the buffer passed in as an argument or dispatches input from the
windowing system (also referred to as events) to functions which are
registered with PGS to handle specific kinds of events. If the specified
FILE pointer is in fact something beside stdin it simple performs an
&#147;fgets&#148; on that file. PG_wind_fgets only returns when a newline
or an end of file condition is encountered. For terminal input this means
typing a carriage return.<p>

It appears that input can only be handled when GETLN is called. However, it
is often desirable to have input handled whenever it comes in. Some operating
systems support this through the use of assignable interrupts. Input handled
this way is said to be interrupt driven. In PGS, when a screen window is
opened all of the machinery is put into place to allow interrupt driven I/O.
The application switches interrupt handling on and off through the macro
SC_set_io_interrupts which take a value of TRUE to turn it on and FALSE to turn
it off. When I/O interrupts are on input from the terminal is saved in a
buffer to be copied into the buffer of the next GETLN call, and input from
the windowing system is dispatched to the appropriate event handler. After
the input is processed the interrupt handler returns and execution resumes
from the point where the interrupt occurred.<p>

With interrupt driven I/O activated, our simple program has all the
capabilities of a vastly more complicated application written for certain
specific operating environments which enforce a graphical interface only
mode of programming. The fact that PGS runs on such systems as wells as
those that support text only or hybrid interfaces should give some idea
of the idea of portability and flexibility which PGS aims to provide.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS0080"></a>
<h2> Event Handling</h2>


Now that we have seen how input is handled in the broadest terms and how
PGS presents a portable application interface for input handling, let&#146;s
turn to an closer examination of the way in which input from a windowing
system is dealt with. Generically, window input is said to consist of
sequences of events. Events can be such things as key presses when the
mouse or locator is in a window, mouse button presses and releases, or
the locator entering or leaving a window.<p>

Different windowing system define varying sets of events. PGS supports
the following set of events everywhere:<p>



<BLOCKQUOTE>
<TABLE>
<TR><TD>KEY_DOWN_EVENT</TD><TD>a key on the keyboard is pressed</TD></TR>
<TR><TD>KEY_UP_EVENT</TD><TD>a key on the keyboard is released</TD></TR>
<TR><TD>MOUSE_DOWN_EVENT</TD><TD>a mouse button is pressed</TD></TR>
<TR><TD>MOUSE_UP_EVENT</TD><TD>a mouse button is released</TD></TR>
<TR><TD>UPDATE_EVENT</TD><TD>the window system says that the window has changed in some way</TD></TR>
<TR><TD>EXPOSE_EVENT</TD><TD>the window has become fully visible (is no longer obscured by another window)</TD></TR>
<TR><TD>MOTION_EVENT</TD><TD>the mouse has moved in the window</TD></TR>
</TABLE>
</BLOCKQUOTE>

<P>
It should be understood that all events have a context. They all happen in
or relate to a particular screen window. So when PGS get notified by the
windowing system that there is an event present, it determines which window
is effected. It then passes both the pointer to the effected window and the
event on to the function which is going to handle the event.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS0093"></a>
<h3> Event Handling Functions</h3>


Given the above list of recognized events, PGS defines a function pointer
(hook) associated with each type of event so that applications may control
what is done with specific events. The following functions let applications
assign their function to these hooks.<p>

<I>C Binding: </I>PFVoid PG_set_key_down_event_handler(PG_device *d, void 
(*fnc)())
<BR><I>Fortran Binding: </I>integer pgsekd(integer d, fnc)
<BR><I>SX Binding: </I>

<P>
<I>C Binding: </I>PFVoid PG_set_key_up_event_handler(PG_device *d, void (*fnc)())
<BR><I>Fortran Binding: </I>integer pgseku(integer d, fnc)
<BR><I>SX Binding: </I>

<P>          
<I>C Binding: </I>PFVoid PG_set_mouse_down_event_handler(PG_device *d, void 
(*fnc)())
<BR><I>Fortran Binding: </I>integer pgsemd(integer d, fnc)
<BR><I>SX Binding: </I>

<P>          
<I>C Binding: </I>PFVoid PG_set_mouse_up_event_handler(PG_device *d, void 
(*fnc)())
<BR><I>Fortran Binding: </I>integer pgsemu(integer d, fnc)
<BR><I>SX Binding: </I>

<P>          
<I>C Binding: </I>PFVoid PG_set_update_event_handler(PG_device *d, void (*fnc)())
<BR><I>Fortran Binding: </I>integer pgseup(integer d, fnc)
<BR><I>SX Binding: </I>

<P>          
<I>C Binding: </I>PFVoid PG_set_expose_event_handler(PG_device *d, void (*fnc)())
<BR><I>Fortran Binding: </I>integer pgseex(integer d, fnc)
<BR><I>SX Binding: </I>

<P>          
<I>C Binding: </I>PFVoid PG_set_default_event_handler(PG_device *d, void (*fnc)())
<BR><I>Fortran Binding: </I>integer pgsedf(integer d, fnc)
<BR><I>SX Binding: </I>

<P>          

These assign the specified function fnc to be the event handler for the
device, d. The function fnc is a pointer to a function returning nothing
which takes a PG_device pointer and a PG_event pointer as arguments.The
C routines all return the old value of the hook and the following
typedef applies:<p>

<UL>typedef void (*PFVoid)();</UL>

<P>
<P>

To explicitly call these functions in a generic way (i.e. regardless of
the specific function attached to the hook) use the following macros:<p>

<UL>
<B>
void PG_handle_key_down_event(PG_device *d, PG_event *ev)
<BR><BR>
void PG_handle_key_up_event(PG_device *d, PG_event *ev)
<BR><BR>
void PG_handle_mouse_down_event(PG_device *d, PG_event *ev)
<BR><BR>
void PG_handle_mouse_up_event(PG_device *d, PG_event *ev)
<BR><BR>
void PG_handle_update_event(PG_device *d, PG_event *ev)
<BR><BR>
void PG_handle_expose_event(PG_device *d, PG_event *ev)
<BR><BR>
void PG_handle_default_event(PG_device *d, PG_event *ev)
</B>
</UL>

<P>

These simply call the specified event handler with the PG_device and PG_event.
The default event handler is an additional way to handle events. The
application can have a single handler for all events. For example, when PGS
gets a mouse down event it first checks to see whether there is a mouse down
handler. If so it is called. If not it then checks to see whether there is
a default handler and if so calls it.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS0113"></a>
<h3>  Event Related Macros</h3>


In addition to the above which have to do with routing events off to handlers,
there are some other macros which the event handlers or user call-back
functions can use to access event information.<p>

<p>
<I>C Binding: </I>int PG_get_next_event(PG_event *ev)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>
<P>
Does a blocking read of the next event from the windowing system and fills
in the specified PG_event structure, ev.<p>

<p>
<I>C Binding: </I>void PG_key_event_info(PG_device *d, PG_event ev, int *iev, char *bf, int n, int *mod)
<BR><I>Fortran Binding: </I>integer pgqkbd(integer d, integer x, integer y, integer c, integer mod)
<BR><I>SX Binding: </I>

<P>
Return the state of the  keyboard for the specified window, d. The x and y
coordinates of the mouse or locator, which key is pressed, and which modifiers
are present are returned in iev, c, and mod respectively.  In the Fortran Binding
this is associated only with one specific keyboard event. It should be called
only a single time from a key-down-event handler. In the C binding the actual
keyboard event is supplied in ev.<p>

<p>
<I>C Binding: </I>void PG_query_pointer(PG_device *d, int *ir, int *pb, int *pq)
<BR><I>Fortran Binding: </I>integer pgqptr(integer d, integer x, integer y, integer b, integer q)
<BR><I>SX Binding: </I>

<P>
Return the state of the mouse locator or pointer for the specified window, d.
The x and y coordinates, which button is pressed, and with modifiers are
present are returned in px, py, pb, and pq respectively. <p>

<BLOCKQUOTE>

The buttons are:<p>

<BLOCKQUOTE>
MOUSE_LEFT
<P>MOUSE_MIDDLE
<P>MOUSE_RIGHT
</BLOCKQUOTE>

</BLOCKQUOTE>


<BLOCKQUOTE>

The modifiers are:<p>

<BLOCKQUOTE>
KEY_SHIFT
<P>KEY_CNTL
<P>KEY_ALT
<P>KEY_LOCK
</BLOCKQUOTE>

</BLOCKQUOTE>


<P>
<BR>
<I>C Binding: </I>PG_device *PG_get_event_device(PG_event *ev)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>
<UL>
Return a pointer to the PG_device (screen window) in which the event,
ev, occurred or to which it relates.
</UL>

<p>

<I>C Binding: </I>int PG_get_char(PG_device *d)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>
<UL>
Return the ASCII code of the next character entered from the keyboard
into the specified PG_device, d.<p>
</UL>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS0132"></a>
<h2>  Interface Objects</h2>


As it stands we have explained the basic machinery by which PGS provides
applications with the capability to get input from the terminal or from the
windowing system and how various events may be handled. This is a foundation
upon which graphical user interfaces may be built. PGS also provides more
machinery (which is layered on top of what we have already discussed) to
support the construction and editing of graphical user interfaces. The goal
goes beyond portability and flexibility to that of enabling the design,
construction, testing, and modification of graphical interfaces. This is done
in a way that is extremely compact to implement and to use. It lets application
developers pass on to their users the ability to modify user interfaces to suit
their own individual needs. This can even be done at run time! It is also
portable!!!<p>

The principal notion underlying this facility is the abstraction called an
interface object. It identifies and encapsulates the fundamental interactions
of a user with the windowing system which underlies everything and the
&#147;atoms&#148; of such a system. An interface object is a structure which
contains the following information and methods:<p>


<BLOCKQUOTE>
<TABLE>
<TR><TD>Type</TD><TD>a string which identifies and differentiates interface objects</TD></TR>
<TR><TD>Region</TD><TD>a polygonal region in the window defining the context of the object</TD></TR>
<TR><TD>Visible flag</TD><TD>specifies whether the object is to be drawn or not</TD></TR>
<TR><TD>Selectable flag</TD><TD>specifies whether the object can be selected or not</TD></TR>
<TR><TD>Active flag</TD><TD>specifies whether the object is currently active or not</TD></TR>
<TR><TD>Draw method</TD><TD>how to draw the object if it is visible</TD></TR>
<TR><TD>Select method</TD><TD>how to select the object if it is selectable</TD></TR>
<TR><TD>Action method</TD><TD>what action to take when the object is active or activated</TD></TR>
<TR><TD>Parent</TD><TD>interface object of which this is a child</TD></TR>
<TR><TD>Children</TD><TD>array of child interface objects</TD></TR>
</TABLE>
</BLOCKQUOTE>

<P>

Each PG_device has a tree of interface objects associated with it. Interface
objects are created by calls to PG_make_interface_object_n. These objects
individually and by virtue of their relationship in a hierarchy allow one to
define the conventional graphical interface tools such as buttons, slider
bars, and text boxes. The mechanism is very open ended and extensible. By
defining the draw, select, and action methods and building trees of interface
objects, application developers can generate virtually any kind of graphical
interface functionality they wish.<p>

The window region defines a place on the screen to which to assign certain
interpretations of mouse events, most notably button press and motion events.
Keeping in mind such elements of a graphical interface as &#147;dialog
boxes&#148;, it is clear that an object may or may not be drawn at all times.
That is left to the interface designer to decide. So a flag and a method
control the visibility of an interface object. Similarly depending on the
context, an interface object may or may not be selectable. Selection is the
process in which mouse events are associated with the designated window
region. If an interface object is not selectable, no association is made
between a mouse event and the region of the object. The action associated
with an interface object can be very nebulous indeed. It can range from a
function call when the object is selected to a value to be assigned to an
associated variable when the object is selected. Here again, it is up to
the application designer to decide.<p>

When defining one interface object to be the child of another, the region of
the child is defined in coordinates normalized with respect to the enclosing
rectangle of the parent. This makes for a much more flexible and intuitive
design in that deeply nested objects only refer to their parents not to the
entire outer context. Changing subtrees is much easier since the children are
specified relative to the parents.<p>

In addition, interface objects have a border width, a foreground and
background color, and a pointer to which anything can be assigned (e.g.
a variable or some other structure). These items may be used in fairly
arbitrary ways by applications. PGS uses them for the pre-defined objects
as discussed in the next section.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS0149"></a>
<h3>  Event Handling and Interface Objects</h3>


The event handler embedded in PG_wind_fgets treats events from the
windowing system in following order fashion:<p>

<OL>
<LI>Checks to see whether a mouse down or key down event occurred inside
any selectable interface object.
<P>
<LI>If inside an interface object that has an action, then that action
is called.
<P>
<LI>Otherwise if there is a handler for the event type, it is called.
</OL>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS0154"></a>
<h3>  Portable User Interface Description</h3>

PGS has an ASCII representation of interface objects so that interface
designers can simply write text files, called portable user interface
files, that describe the interface and have PGS read and interpret them
at run time, modify them interactively, and write them back out again.
Thus the interfaces can be edited graphically at run time or textually
with your favorite text editor. At this writing the only option which
PGS supplies for graphical editing is moving objects around. Creation,
deletion, duplication, and resizing are planned for future releases.
However, these options can all be accomplished by editing the interface
file or by application supplied editing routines.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS0156"></a>
<h4>  Syntax for Interface Description</h4>


The syntax for describing an interface object is:<p>


type [operator(parameters)]* BND(parameter) points [{ children }]

<P>
Valid operators are:<p>


<BLOCKQUOTE>
<TABLE>
<TR><TD WIDTH="50">ACT</TD><TD>names function that does object action when active</TD></TR>
<TR><TD>BND</TD><TD>boundary specifier</TD></TR>
<TR><TD>CLR</TD><TD>specifies fore and background colors (by index)</TD></TR>
<TR><TD>DRW</TD><TD>names function that draws object when visible</TD></TR>
<TR><TD>FLG</TD><TD>state flags</TD></TR>
<TR><TD>NAME</TD><TD>object name</TD></TR>
<TR><TD>SEL</TD><TD>names function that identifies object as selected when selectable</TD></TR>
</TABLE>
</BLOCKQUOTE>

<P>
Valid BND parameters are:<p>


<BLOCKQUOTE>
<TABLE>
<TR><TD WIDTH="50">n</TD><TD>an integer number of NDC points (x, y) follow</TD></TR>
<TR><TD>RECT</TD><TD>2 NDC points specifying the lower left and upper right
limits of a boundary rectangle follow</TD></TR>
</TABLE>
</BLOCKQUOTE>

<P>
Valid FLG parameters are:<p>

<BLOCKQUOTE>
<TABLE>
<TR><TD WIDTH="50">IsVis</TD><TD>object is visible</TD></TR>
<TR><TD>IsSel</TD><TD>object is selectable</TD></TR>
<TR><TD>IsAct</TD><TD>object is active</TD></TR>
</TABLE>
</BLOCKQUOTE>

<P>
Valid CLR parameters are (using the standard color table):<p>

<BLOCKQUOTE>
<PRE>
 0	logical BLACK
 1	logical WHITE
 2	LGHT_WHITE
 3	GRAY
 4	BLUE
 5	GREEN
 6	CYAN
 7	RED
 8	MAGENTA
 9	BROWN
10	LGHT_BLUE
11	LGHT_GREEN
12	LGHT_CYAN
13	LGHT_RED
14	YELLOW
15	LGHT_MAGENTA

</PRE>
</BLOCKQUOTE>

The default for FLG parameters is IsVis and IsSel. You only need use the FLG
operator if you wish to change this setting. Each parameter specified turns
on its corresponding flag. To turn all flags off use FLG( ) (one space between
the parentheses).<p>

A line break may occur between operator specifications. The descriptions of
children of an object are delimited by curly braces.<p>

To make this text driven representation work, it is necessary to have a mechanism
to register functions and variables with PGS so that a function or variable can
be looked up by name. The functions PG_register_callback and PG_register_variable
do just this.<p>

This is a very general mechanism for assigning events to actions and generically
handling common activities on behalf of applications. PGS goes one step further
and supplies a number of specific interface objects.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS0195"></a>
<H2>PGS Interface Objects</H2>

<P>

The specific objects which PGS supplies are: CONTAINER, BUTTON, TEXT, and VARIABLE.
With these objects and some defined relationships between them it is possible to
build most of the common "widgets" found in the sets supplied with various systems.<p>

With all of the objects which PGS defines the following hold true: if an object
is not visible, then its children are not visible either; if an object is not
selectable then its children are not selectable either; if the border width is
not zero, a black border is drawn around the object; and if the background color
of an object is -1 the closest ancestor with a background color other than -1
supplies the background color.<p>

PGS interface objects have two kinds of containment associations: visual containment
in which child objects appear visually (on the screen) within the region defined by
their parents; and logical containment in which only the position in the hierarchy
defines the containment association relationship, that is children are logically
contained within their parents.<p>

The CONTAINER object is defined simply to specify logical containment of objects.
When looking up a hierarchy it is a natural breakpoint to identify groups of objects
which may have to collectively respond to events (we'll see examples of this later).
It also can be used just to ensure that a border is drawn around a group of other
objects (these would have to be children of the container).<p>

The BUTTON object is defined as a trigger. When a mouse down event happens in a
button object a chain of events is set off. This covers initiation of call back
functions, setting the value of a VARIABLE object, or moving a slider around. It
depends on what the button means (as defined by context in a hierarchy or by its
action method).<p>

A TEXT object is an encapsulation of a PG_text_box structure and the methods which
accompany it. The text box is a collection of text and editing functions. The
editing functions supply an EMACS-like text manipulation facility for any quantity
of text from one line to as much text as can be loaded into memory. Text boxes with
a single line of text have the text centered in the box and multi-line text boxes
have their text left justified.<p>

A VARIABLE object binds a variable to an interface object. The parent, siblings,
and children of the VARIABLE object define its actual behavior.<p>

PGS predefines the following methods using PG_register_callback:<p>

<PRE>

                              <B>DRW Methods</B>

draw-text	draw a text object (default for TEXT objects)

draw-variable	draw a variable object (default for VARIABLE objects)

draw-container	draw a container object (default for CONTAINER objects)

draw-button	draw a button object (default for BUTTON objects)

draw-slider	draw a slider button


                              <B>SEL Methods</B>

select-visual	select based on visual containment

select-logical	select based on logical containment


                              <B>ACT Methods</B>

slider		action function for slider buttons

toggle		action function to toggle visibility and selectability of objects

</PRE>

<P>

With these building blocks in hand we will look at some common widgets found in
many graphical user interface kits. It is very important to note that much of the
behavior of these objects derives from their relative positions in the hierarchy
of interface objects. This position relationship contributes as much as the atomic
properties of the individual objects to the functioning of these combinations. This
is a key feature in the flexibility of the PGS design.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS0217"></a>
<h3> Buttons</h3>

<p>
The intended functionality is a "button" on the screen which when selected by a
mouse click invokes a function. For example, consider an "End" button to terminate
an applications:<p>

<PRE>
Button CLR(3,2) DRW(draw-button) ACT(End) BND(RECT) (0.1,0.9) (0.2,0.95)
   {Text NAME(End) CLR(10,-1) FLG(IsVis) DRW(draw-text) BND(RECT) (0.05,0.3) (0.95,0.7)}
</PRE>

<P>
The action method of the button is the function associated with the string
&#147;End&#148; in the callback table. The text &#147;End&#148; appears in
the button and is visible but not selectable.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS0222"></a>
<h3> Enumerable Variables</h3>


Certain variables have a small number of possible values and it is convenient
and pleasing to present buttons for each possible value and have the user
select the desired one. This situation might look like this:<p>

<PRE>
Container CLR(0,0) BND(RECT) (0.1,0.82) (0.25,0.87)
  {Variable NAME(Output) CLR(10,0) BND(RECT) (0.0,0.49) (1.0,1.0)
     {Button CLR(3,2) ACT(1) BND(RECT) (0.0,-1.0) (0.5,0.0)
        {Text NAME(On) CLR(10,-1) FLG(IsVis) BND(RECT) (0.05,0.05) (0.95,0.95)}
      Button CLR(3,2) ACT(0) BND(RECT) (0.5,-1.0) (1.0,0.0)
        {Text NAME(Off) CLR(10,-1) FLG(IsVis) BND(RECT) (0.05,0.05) (0.95,0.95)}}}
</PRE>

<P>
The children of the variable registered using PG_register_variable under the
name &#147;Output&#148; are buttons which when selected set the value of the
variable to the value taken from the action of the button (1 for the
&#147;On&#148; button and 0 for the &#147;Off&#148; button). A second more
elaborate example shows this same principle:<p>

<PRE>
Container CLR(0,0) BND(RECT) (0.85,0.88) (1.0,1.0)
  {Variable NAME(Direction) CLR(10,0) BND(RECT) (0.2,0.0) (0.8,0.2)
     {Button CLR(3,2) ACT("n")
       BND(5) (0.51,3.12) (0.64,3.53) (0.51,3.88) (0.36,3.53) (0.51,3.12)
         {Text NAME(N) CLR(10,0) FLG(IsVis) BND(RECT) (0.0,1.0) (1.0,2.0)}
      Button CLR(3,2) ACT("e")
      BND(5) (0.55,3.0) (0.7,2.59) (0.9,3.0) (0.7,3.41) (0.55,3.0)
        {Text NAME(E) CLR(10,0) FLG(IsVis) BND(RECT) (1.0,0.0) (2.0,1.0)}
      Button CLR(3,2) ACT("s")
      BND(5) (0.51,2.88) (0.36,2.53) (0.51,2.12) (0.64,2.53) (0.51,2.88)
        {Text NAME(S) CLR(10,0) FLG(IsVis) BND(RECT) (0.0,-1.0) (1.0,0.0)}
      Button CLR(3,2) ACT("w")
      BND(5) (0.45,3.0) (0.3,3.41) (0.1,3.0) (0.3,2.59) (0.45,3.0)
        {Text NAME(W) CLR(10,0) FLG(IsVis) BND(RECT) (-1.0,0.0) (0.0,1.0)}}}
</PRE>

<P>
Here a variable registered under the name &#147;Direction&#148; has four
buttons (which are not rectangles), &#147;N&#148;, &#147;E&#148;,
&#147;S&#148;, and &#147;W&#148; which take values that are strings and
the text of the buttons is outside of the buttons at the points.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS0246"></a>
<h3> Nonenumerable Variables</h3>


Sometimes variables do not take a small set of discrete values. In such a
case a reasonably natural way for a graphical interface to set the variable
is to enter the text representation of the value and have it converted to
the appropriate binary form. PGS currently only supports ints, longs,
floats, doubles, and strings in this way. Here is an example of this:<p>

<PRE>
Container CLR(0,0) BND(RECT) (0.26,0.82) (0.39,0.87)
  {Variable NAME(Theta) CLR(10,0) BND(RECT) (0.0,0.49) (1.0,1.0)
     {Text NAME(30) CLR(10,-1) BND(RECT) (0.05,-0.95) (0.95,-0.05)}}
</PRE>

<P>
The text &#147;30&#148; might be the initial value for the variable registered
using PG_register_variable under the name &#147;Theta&#148;. PGS uses the
actual current value of the variable in the code for all display purposes.
To change the value, you place the cursor in the region of the text, edit
it to show the correct value, and hit a carriage return. The value of the
variable is then set to that show in the text box. Note that the VARIABLE
object must be selectable in order for the text box which actually controls
the value to be selectable.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS0252"></a>
<h3> Sliders</h3>


Another way to set numeric variables is to use a button which can move within
some limits and whose position within those bounds determines the value of the
variable. This is what we call a slider. PGS supports both one dimensional
sliders in which there is only one degree of freedom and controls one variable
and two dimensional sliders in which there are two degrees of freedom and two
variables are controlled simultaneously.<p>

An example of the one dimensional slider is:<p>

<PRE>
Container CLR(0,0) BND(RECT) (0.41,0.82) (0.54,0.88)
  {Variable NAME(Phi) CLR(10,0) BND(RECT) (0.0,0.67) (1.0,1.0)
     {Text NAME(-60) CLR(10,-1) BND(RECT) (0.05,-0.93) (0.95,-0.067)}
   Container CLR(2,2) BND(RECT) (0.0,0.0) (1.0,0.3)
     {Button CLR(0,0) DRW(draw-slider) ACT(slider) BND(RECT) (0.28,0.0) (0.43,1.0)}}
</PRE>

<P>
What makes this a one dimensional slider is that the BUTTON with the slider
action takes up the entire span in the y direction of the parent CONTAINER.
The text is not strictly necessary here. It is nice to see the value,
however, and you can also change the value by entering it in the text box.
Either mode of setting the values causes both indicators to show the
same value.<p>

An example of the two dimensional slider is:<p>

<PRE>
Container CLR(0,0) BND(RECT) (0.65,0.82) (0.8,0.97)
  {Variable NAME(Theta) CLR(10,0) BND(RECT) (0.05,0.88) (0.45,0.99)
     {Text NAME(45) CLR(10,-1) BND(RECT) (0.05,-0.95) (0.95,-0.05)}
   Variable NAME(Phi) CLR(10,0) BND(RECT) (0.55,0.88) (0.95,0.99)
     {Text NAME(0) CLR(10,-1) BND(RECT) (0.05,-0.95) (0.95,-0.05)}
   Container CLR(2,2) BND(RECT) (0.0,0.0) (1.0,0.72)
     {Button CLR(0,0) DRW(draw-slider) ACT(slider) BND(RECT) (0.0,0.0) (0.1,0.1)}}
</PRE>

<P>
This is a two dimensional slider because the BUTTON with the slider action
does not span either the entire x or y direction of the CONTAINER parent.
The first VARIABLE, &#147;Theta&#148;, associates with the x direction and
the second VARIABLE, &#147;Phi&#148;, with the y direction. The text boxes
work the same way as for the one dimensional slider.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS0270"></a>
<h3> Transients</h3>


Certain interface objects may be visible only transiently in routine use.
Pull down menus and dialog boxes are examples of such constructs. Here is
an example of an object whose visibility and selectability are toggled when
a button is selected.<p>

<PRE>
Container NAME(Menu) CLR(0,0) FLG( ) BND(RECT) (0.349,0.021) (0.551,0.121)
  {Text NAME(A) CLR(10,-1) FLG(IsVis) BND(RECT) (0.052,0.014) (0.948,0.333)
   Text NAME(B) CLR(10,-1) FLG(IsVis) BND(RECT) (0.052,0.333) (0.948,0.667)
   Text NAME(C) CLR(10,-1) FLG(IsVis) BND(RECT) (0.052,0.667) (0.948,0.986)}

Button CLR(3,2) DRW(draw-button) ACT(toggle,Menu) BND(RECT) (0.101,0.05) (0.200,0.10)
  {Text NAME(Menu) CLR(10,-1) FLG(IsVis) DRW(draw-text)
   BND(RECT) (0.053,0.286) (0.947,0.714)}
</PRE>

<P>
The container &#147;Menu&#148; forms the subtree which is to be summoned and
dismissed by actuating the button. Notice how the connection works. The action
specifies both the toggle method and an interface object whose visibility and
selectability are to be toggled. Compare this with the next example below.<p>

<PRE>
Container NAME(Rendering) CLR(0,0) FLG( ) BND(RECT) (0.399,0.051) (0.601,0.151)
  {Text NAME(Rend) CLR(10,-1) FLG(IsVis) BND(RECT) (0.052,0.014) (0.948,0.333)}

Container NAME(Axis) CLR(0,0) FLG( ) BND(RECT) (0.449,0.101) (0.651,0.201)
  {Text NAME(Ax) CLR(10,-1) FLG(IsVis) BND(RECT) (0.052,0.014) (0.948,0.333)}

Container CLR(0,0) BND(RECT) (0.101,0.100) (0.200,0.18)
  {Variable NAME(Which-Panel) CLR(0,0) ACT(toggle) BND(RECT) (0.0,0.0) (1.0,1.0)
     {Button CLR(3,2) DRW(draw-button) ACT(Rendering) BND(RECT) (0.0,0.5) (1.0,1.0)
        {Text NAME(Render) CLR(10,-1) FLG(IsVis) DRW(draw-text)
         BND(RECT) (0.05,0.3) (0.95,0.95)}
      Button CLR(3,2) DRW(draw-button) ACT(Axis) BND(RECT) (0.0,0.0) (1.0,0.5)
        {Text NAME(Axis) CLR(10,-1) FLG(IsVis) DRW(draw-text)
         BND(RECT) (0.05,0.3) (0.95,0.95)}}}
</PRE>

<P>

In this case a variable is defined with the toggle method. The value of the
variable is a string which is the name of the container to be toggled. That
is, if the &#147;Render&#148; button is pressed, the &#147;Rendering&#148;
container becomes visible and selectable. If the &#147;Axis&#148; button
is now pressed, the &#147;Rendering&#148; container becomes invisible and
unselectable while the &#147;Axis&#148; container becomes visible and
selectable. A key feature here is that the variable &#147;Which-Panel&#148;
does not and should not be registered by the application. PGS implicitly
defines and registers undefined variables like this as strings and uses them
as described above.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGSrender"></a>
<h1 ALIGN="CENTER"> Rendering Model</h1>

This section describes the model used in PGS to do various high level
renderings of data for the purposes of scientific visualization. High level
renderings refer to the notion of carrying out a large number of graphical
operations to generate a &#147;plot&#148; or &#147;rendering&#148; of a set
of data. To make this as easy as possible, PGS supplies a set of routines
which will give &#147;one picture for one call&#148;.  These routines have
the flexibility to produce plots which fit the needs of the user who can
set rendering attributes to control the output in detail. The attributes
all have a reasonable default value so that it is possible to make
reasonable plots with a single call.<p>

Visualization does not exist in a vacuum. It is strongly coupled to analysis
and to data storage. PACT, of which PGS is a part, has extensive facilities
in all of these areas. To communicate among them a common representation of
data sets is used and instances of these data sets are passed around. Data
sets are organized into two mathematically motivated structures: PM_set and
PM_mapping.  These are described in more detail in the PML Users Manual. A
third data structure called a PG_graph is used to contain and associate
rendering information with the PM_mapping which only contains the data that
a simulation or observation would yield or that an analysis program would
need. A picture of how these parts fit together is:<p>

<p>

<img src="pgs3.gif">

<p>

<B>FIGURE 1. Relationship between PG_graphs, PM_mappings, and PM_sets
</B>

<HR>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS2129"></a>
<h2> Data Structures</h2>

The data structures employed in PGS for the purposes of visualization come
from PGS itself and PML, the math library for PACT. The interested user
should consult the PML Users Manual for more complete information as
well as descriptions of routines which manipulate these structures.<p>

<CENTER><B>PM_set</B></CENTER>

The PM_set or set represents a collection of objects (primarily but not
exclusively numbers). It also describes the dimensionality of the set,
the dimensionality of the elements, and the connectivity of the elements.<p>

<CENTER><B>PM_mapping</B></CENTER>
The PM_mapping or mapping represents the relationship between two sets of
elements. In mathematics this is the generalization of a function. A mapping
consists of two sets, a domain and range set, and a rule for associating
elements of the two sets. Generally in numerical applications the rule is
based on the order of the elements of the two sets with a specification of
centering and strides through the elements.<p>

<CENTER><B>PM_mesh_topology</B></CENTER>
The PM_mesh_topology defines the connectivity of elements in a set if the
trivial logical array ordering is not to be used. The logical array ordering
is referred to as logical rectangular and is used in many simulations.
However, many applications today cannot represent their data in this way and
the means for a completely general specfication of neighbor relations is
provided by the PM_mesh_topology structure. Such relationships are referred
to as arbitrarily connected.<p>

<CENTER><B>PG_graph</B></CENTER>
To visualize data requires two ingredients: data; and rendering specifications.
The mapping described above takes care of the first part and a list of
rendering attributes does the rest.  The PG_graph contains these part in one
convenient package.<p>

<CENTER><B>PG_image</B></CENTER>
A simple but extremely useful data representation is a raster image or
cell array. PGS provides a structure to contain such information and visualize
it.<p>

<CENTER><B>PG_palette</B></CENTER>
The specification of color maps is handled with the PG_palette structure.
This information tells PGS how to match 3 dimensional RGB color space with
n dimensional data sets. The most common situation is that of a single
dimension data in a range set or image, but PGS supports multidimensional
palettes for higher dimensional ranges.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS2130"></a>
<h2> Rendering Modes</h2>

PGS currently features several rendering modes. They are listed here
along with a brief description.<p>

<CENTER><B>Line Plots</B></CENTER>

<P>
Line plots is a generic term referring to graphs with 1 dimensional domains
and ranges. When a graph has 1d domain and a 1d range PG_draw_graph will
render the data in any of the following ways depending on the value of
the PLOT-TYPE attribute.<p>

<TABLE>
<TR><TD>PLOT_CARTESIAN</TD>
<TD>	Canonical x vs y plot.</TD></TR>
<TR><TD>PLOT_POLAR</TD>
<TD>	Polar plot with r vs theta.</TD></TR>
<TR><TD>PLOT_INSEL</TD>
<TD>	Inselberg plot with parallel axes and (x, y) points represented
as lines connecting the values on the axes.</TD></TR>
<TR><TD>PLOT_HISTOGRAM</TD>
<TD>	Cartesian histogram plot with the steps starting with the LEFT 
value, the RIGHT value, or the averaged or CENTER value.</TD></TR>
<TR><TD>PLOT_SCATTER</TD>
<TD>	Scatter plot where points are a plotted with a marker character 
but not connected by line segments (Cartesian)</TD></TR>
<TR><TD>PLOT_LOGICAL</TD>
<TD>	Plot y values versus their array index.  The x values are ignored.</TD></TR>
<TR><TD>PLOT_ERROR_BAR</TD>
<TD>	Like a scatter plot but instead of marker characters being used 
error bars are drawn. Requires 2 arrays for x error and y error or 4 arrays for 
positive and negative going x error and positive or negative going y error.</TD></TR>
</TABLE>

<P><CENTER><B>Two Dimensional Plots</B></CENTER>
<P>

Two dimensional plot is a generic term referring to graphs with 2 dimensional
domains. The most common situation features a 1 dimensional range but higher
dimensions may be used. When a graph has a 2d domain PG_draw_graph will render
the data in any of the following ways depending on the value of the PLOT-TYPE
attribute.<p>

<TABLE>
<TR><TD>PLOT_CONTOUR</TD>
<TD>A traditional iso contour plot. Can be done with either 
logical or arbitrary connectivity.</TD></TR>
<TR><TD>PLOT_IMAGE</TD>
<TD>A rasterized image plot. Can only be done with logical connectivity.</TD></TR>
<TR><TD>PLOT_WIRE_MESH</TD>
<TD>The domain values are x and y and the range values are 
z in a 3 dimensional space. Line segments connect neighboring points. The 
data can be examined from any specified view angle. The algorithm is a z
buffered scan line technique. Can be done with either logical or arbitrary
connectivity.</TD></TR>
<TR><TD>PLOT_SURFACE</TD>
<TD>The domain values are x and y and the range values are 
z in a 3 dimensional space. In addition to showing the connecting line segments,
the faces bounded by the segments are shaded.  Two dimensional ranges 
are handled by taking the first component to be the z value and the second 
component as the color value. The data can be examined from any specified 
view angle. The algorithm is a z buffered scan line technique. Can be done 
with either logical or arbitrary connectivity.</TD></TR>
<TR><TD>PLOT_FILL_POLY</TD>
<TD>The facets bounded by segments connecting 
neighbors are filled with a single color determined by the range value. Can be 
done with either logical or arbitrary connectivity.</TD></TR>
</TABLE>

<P>
<CENTER><B>Vector Plots</B></CENTER>
<P>

Vector plot refers to a plot in which the range has at least two dimensions.
Currently PGS can only render 2 dimensional vectors but in the long term will
render higher dimensional vectors as well.  Also currently vector plots may
only be done with 2 dimensional domains. They are requesed by setting the
PLOT-TYPE attribute of the graph to PLOT_VECTOR.<p>

<P>
<CENTER><B>Mesh Plots</B></CENTER>
<P>

Mesh plots are graphical renderings of the connectivity of a domain set.
In PGS lines are drawn connecting neighboring points. They can be either
2 dimensional or 3 dimensional. If 3 dimensional they may be viewed from
any angle similarly to the PLOT_WIRE_MESH plots discussed above. If a NULL
range is specified only the mesh is drawn. If a range is supplied the range
values are printed at their corresponding nodes. They are requesed by
setting the PLOT-TYPE attribute of the graph to PLOT_MESH.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS0983"></a>
<h2> Rendering Attributes</h2>

By default PGS assumes a &#147;look and feel&#148; for the various renderings
that it can do. This look can be reduced to a list of characteristic or
attribute values. Realizing that applications need to control their own look
and feel, they are given a mechanism to change these attributes.<p>

Generally speaking, attributes are managed as association lists, that is
lists of key-value pairs. Attribute list are associated with graphs (PG_graph),
mappings (PM_mapping), and sets (PM_set). That is to say that some rendering
attributes belong naturally with a set (for example, the plotting limits)
and others with a graph (for example, the number of contour levels). The
PGS rendering routines query the attribute lists for specific attribute
values and use their defaults if they are not found.<p>

Modifying attribute lists is done with the following functions:
<p>

<PRE>

        <B>int PG_get_attrs_glb(int dflt, ...)</B>
        <B>int PG_get_attrs_graph(PG_graph *g, ...)</B>
        <B>int PG_get_attrs_mapping(PM_mapping *f, ...)</B>
        <B>int PG_get_attrs_set(PM_set *s, ...)</B>
        <B>int PG_get_attrs_alist(pcons *alst, ...)</B>

               These get the value of one or more attributes from the
               given object.  Each of the attribute value requests is
               in the form of a quadruple:
                  <i>attr</i>, <i>type</i>, <i>var</i>, <i>default</i>
               where
                  <i>attr</i>    = string naming the attribute (e.g. "LINE-COLOR")
                  <i>type</i>    = integer type code for the value. One of
                            G_CHAR_I, G_SHORT_I, G_INT_I,
                            G_LONG_I, G_FLOAT_I, G_DOUBLE_I, G_DOUBLE_I,
                            or G_POINTER_I
                  <i>var</i>     = the address of the variable to set
                  <i>default</i> = the default value to assign
                            if the attribute is not present
               The last argument must be NULL indicating the end of the
               list of specifications.
               The number of attributes processed is returned

        <B>int PG_set_attrs_glb(int dflt, ...)</B>
        <B>int PG_set_attrs_graph(PG_graph *g, ...)</B>
        <B>int PG_set_attrs_mapping(PM_mapping *f, ...)</B>
        <B>int PG_set_attrs_set(PM_set *s, ...)</B>
        <B>pcons *PG_set_attrs_alist(pcons *alst, ...)</B>

               These set the value of one or more attributes in the
               given object.  Each of the attribute value specifications is
               in the form of a quadruple:
                  <i>attr</i>, <i>type</i>, <i>ptr</i>, <i>val</i>
               where
                  <i>attr</i>    = string naming the attribute (e.g. "LINE-COLOR")
                  <i>type</i>    = integer type code for the value. One of
                            G_CHAR_I, G_SHORT_I, G_INT_I,
                            G_LONG_I, G_FLOAT_I, G_DOUBLE_I, G_DOUBLE_I,
                            or G_POINTER_I
                  <i>ptr</i>     = TRUE iff the attribute is an array of values
                  <i>default</i> = the value to assign to the attribute
               The last argument must be NULL indicating the end of the
               list of specifications.
               TRUE is returned iff successful


        <B>int PG_rem_attrs_graph(PG_graph *g, ...)</B>
        <B>int PG_rem_attrs_mapping(PM_mapping *f, ...)</B>
        <B>int PG_rem_attrs_set(PM_set *s, ...)</B>
        <B>pcons *PG_rem_attrs_alist(pcons *alst, ...)</B>

               These remove the value of one or more attributes from the
               given object.  Each of the attribute specifications is
               simply the attribute name:
                  <i>attr</i>
               where
                  <i>attr</i> = string naming the attribute (e.g. "LINE-COLOR")
               The last argument must be NULL indicating the end of the
               list of specifications.
               TRUE is returned iff successful


</PRE>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS2131a"></a>
<h3> Examples of Setting and Getting Attribute Values</h3>

Here are a few examples of using functions to manipulate attributes.
<p>
<pre>
    PG_graph *data;
    int mshp, nlev;
    double *levels;

                      .
                      .
                      .

    PG_set_attrs_graph(data,
		       "DRAW-MESH", G_INT_I, FALSE, mshp,
		       "N-LEVELS",  G_INT_I, FALSE, nlev,
		       "LEVELS",    G_DOUBLE_I,  TRUE,  levels,
		       NULL);
                      .
                      .
                      .
</pre>

In this example we are preparing to draw a contour plot.  So we are
setting the number of levels, N-LEVELS, and the values of the levels,
LEVELS.  We are also specifying whether or not we want the underlying
mesh to be drawn depending on whether or not <i>mshp</i> is TRUE.

<p>

<pre>
    PG_graph *data;
    int pty;
    double theta, phi;

                      .
                      .
                      .

    PG_set_attrs_mapping(data->f,
			 "PLOT-TYPE", G_INT_I, FALSE, pty,
			 "THETA",     G_DOUBLE_I,  FALSE, theta,
			 "PHI",       G_DOUBLE_I,  FALSE, phi,
			 "CHI",       G_DOUBLE_I,  FALSE, 0.0,
			 NULL);
                      .
                      .
                      .
</pre>

In this example we are preparing to render data in such a way that
a view angle specification is required (perhaps PLOT_SURFACE).  The
PLOT-TYPE is specified by <i>pty</i> and the three Euler angle are
given by THETA, PHI, and CHI.

<pre>
    pcons *tlst;
    int color;
                      .
                      .
                      .

    tlst = PG_set_attrs_alist(tlst,
			      "DRAW-AXIS",  G_INT_I, FALSE, FALSE,
			      "DRAW-LABEL", G_INT_I, FALSE, FALSE,
			      "LINE-COLOR", G_INT_I, FALSE, ++color,
			      NULL);
                      .
                      .
                      .
</pre>

In this example we are adding attributes to tlst specifying that
axes and labels are not to be drawn and the line color used is incremented
from the previous value.

<pre>
    PM_set *domain;
    PG_device *dev;
    double dbwid;
    inte dbclr, dbsty;
                      .
                      .
                      .

    PG_get_attrs_set(domain,
		     "DOMAIN-BORDER-WIDTH", G_DOUBLE_I,    &dbwid, -1.0,
		     "DOMAIN-BORDER-COLOR", G_INT_I, &dbclr, dev->WHITE,
		     "DOMAIN-BORDER-STYLE", G_INT_I, &dbsty, LINE_SOLID,
		     NULL);
                      .
                      .
                      .
</pre>

In this example we wish to get the values of the attributes from the set
<i>domain</i>.  Note the default values to supply if the named attributes
are not present in the set.
<p>

<pre>
    PM_set *domain;
                      .
                      .
                      .

    PG_rem_attrs_set(domain, "LINE-COLOR", NULL);
                      .
                      .
                      .
</pre>

In this example we remove the LINE-COLOR attribute from the set
<i>domain</i>.
<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS2131b"></a>
<h3> Attributes</h3>

This is the list of attributes currently understood by PGS.
In use these all appear as quoted strings.<p>

<p>


AXIS-TYPE
<UL>Flag specifying an axis type of CARTESIAN_2D, POLAR, or
INSEL (int).  The default is CARTESIAN_2D.
</UL>
<P>
CHI
<UL>Value specifying the chi component of the three Euler angles (double).
The default is 0.0 degrees.
</UL>
<P>
CORNER
<UL>Value indicating the node of a logical rectangle which is associated with the 
zone center (int). 1 associates lower right, 2 upper right, 3 upper left, 
and 4 lower left.  This applies to logical rectangular mappings only. The 
default is 2.
</UL>
<P>
DRAW-AXIS
<UL>If TRUE a high level rendering routine will draw a set of axes.
</UL>
<P>
DRAW-LABEL
<UL>If TRUE a high level rendering routine will draw the graph label.
</UL>
<P>
DRAW-LEGEND
<UL>If TRUE the contour plotting routine will draw the legend of contour values.
</UL>
<P>
DX-MINUS
<UL>For PLOT_ERROR_BAR type renderings this is the array of negative going 
errors on the x values (double *).  This must have the same number of 
entries as the x values.
</UL>
<P>
DX-PLUS
<UL>For PLOT_ERROR_BAR type renderings this is the array of positive going 
errors on the x values (double *).  This must have the same number of 
entries as the x values.
</UL>
<P>
DY-MINUS
<UL>For PLOT_ERROR_BAR type renderings this is the array of negative going 
errors on the y values (double *).  This must have the same number of 
entries as the y values.
</UL>
<P>
DY-PLUS
<UL>For PLOT_ERROR_BAR type renderings this is the array of positive going 
errors on the y values (double *).  This must have the same number of 
entries as the y values.
</UL>
<P>
EXISTENCE
<UL>An existence map array specifiying missing zones in a logical rectangular 
mesh (char *). This prevents plotting of information which would be 
associated with non-existent zones in a domain mesh.
</UL>
<P>
HIST-START
<UL>Flag specifying whether a PLOT_HISTOGRAM rendering starts with the 
value on the LEFT, CENTER, or RIGHT side of the bar (int).
</UL>
<P>
LEVELS
<UL>The array of contour levels which must be N-LEVELS long (double *). This is 
only used when an application wants more control over contour levels 
than the contour plotting routines in PGS already provide.
</UL>
<P>
LIMITS
<UL>Array of minimum and maximum values (one pair per dimension) specifying 
the plotting limits of a domain or range set (double *). The length must 
be twice the number of dimensions of the set.
</UL>
<P>
LINE-COLOR
<UL>The line color index (int). The default is BLUE.
</UL>
<P>
LINE-STYLE
<UL>The line style index LINE_SOLID, LINE_DOTTED, LINE_DASHED, LINE_DOTDASHED (int). The 
default is LINE_SOLID.
</UL>
<P>
LINE-WIDTH
<UL>The line width (double). The default is 0.0.
</UL>
<P>
MARKER-INDEX
<UL>The index into the marker array (int). This depends on how many markers
have been defined with PG_def_marker. The default is 0.
</UL>
<P>
MARKER-SCALE
<UL>The scale factor applied to a marker when drawn (double).
The default is 0.01.
</UL>
<P>
N-LEVELS
<UL>The number of isocontour levels used in a contour plot (int).
The default is 10.
</UL>
<P>
NORMAL-DIRECTION
<UL>A flag specifying the normal direction of surface elements in
hidden surface plot (int). The default is 1.
</UL>
<P>
PALETTE
<UL>The name of the palette to use in those plots which need color
palettes (char *). The default is the current palette of the device.
</UL>
<P>
PHI
<UL>Value specifying the phi component of the Euler angles defining the
viewing angle (double). The default is 0.0 degrees.
</UL>
<P>
PHI-LIGHT
<UL>Value specifiying the phi angle of a light source illuminating a
surface plot (double). The default is 45.0 degrees.
</UL>
<P>
PLOT-TYPE
<UL>Flag specifying the plot type PLOT_CARTESIAN, PLOT_POLAR, or PLOT_INSEL (int). The 
default is PLOT_CARTESIAN.
</UL>
<P>
RATIO
<UL>Isocontour spacing ratio (double). The default is 1.0.
</UL>
<P>
SCATTER
<UL>Flag specifying that a rendering be done as a scatter plot if
TRUE (int). The default is FALSE.
</UL>
<P>
THETA
<UL>Value specifying the theta component of the Euler angles defining
the viewing angle (double). The default is 0.0 degrees.
</UL>
<P>
THETA-LIGHT
<UL>Value specifiying the theta angle of a light source illuminating
a surface plot (double). The default is 45.0 degrees.
</UL>
<P>
VIEW-PORT
<UL>An array of values in normalized coordinates (xmin, xmax, ymin, and ymax) 
specifying a viewspace to be for a plot (double *). The default is defined 
by each rendering routine to maximize the area of the plot.
</UL>
<P>
<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS2161"></a>
<h3> Plots and associated attributes</h3>

This section tells which rendering attributes are meaningful to which renderings.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS2163"></a>
<h4> Contour Plot Attributes</h4>

<BLOCKQUOTE>
DRAW-AXIS
<P>DRAW-LABEL
<P>DRAW-LEGEND
<P>LEVELS
<P>LIMITS
<P>LINE-COLOR
<P>LINE-WIDTH
<P>LINE-STYLE
<P>N-LEVELS
<P>LEVELS
<P>RATIO
<P>VIEW-PORT
</BLOCKQUOTE>

<P>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS2171"></a>
<h4> Domain Plot Attributes</h4>

<BLOCKQUOTE>
CHI
<P>CORNER
<P>DRAW-AXIS
<P>DRAW-LABEL
<P>EXISTENCE
<P>LIMITS
<P>LINE-COLOR
<P>LINE-STYLE
<P>LINE-WIDTH
<P>MARKER-INDEX
<P>MARKER-SCALE
<P>PALETTE
<P>PHI
<P>PHI-LIGHT
<P>PLOT-TYPE
<P>THETA
<P>THETA-LIGHT
<P>SCATTER
</BLOCKQUOTE>

<P>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS2189"></a>
<h4> Line Plot Attributes</h4>

<BLOCKQUOTE>
AXIS-TYPE
<P>DX-MINUS
<P>DX-PLUS
<P>DY-MINUS
<P>DY-PLUS
<P>HIST-START
<P>LINE-COLOR
<P>LINE-STYLE
<P>LINE-WIDTH
<P>MARKER-INDEX
<P>PLOT-TYPE
<P>SCATTER
</BLOCKQUOTE>

<P>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS2203"></a>
<h4> Hidden Surface Attributes</h4>

<BLOCKQUOTE>
NORMAL-DIRECTION
</BLOCKQUOTE>

<P>
<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS2206"></a>
<h4> Image Plot Attributes</h4>

<BLOCKQUOTE>
CORNER
<P>DRAW-AXIS
<P>DRAW-LABEL
<P>EXISTENCE
<P>LIMITS
</BLOCKQUOTE>

<P>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS2211"></a>
<h4> Fill Poly Plot Attributes</h4>

<BLOCKQUOTE>
CORNER
<P>DRAW-AXIS
<P>DRAW-LABEL
<P>EXISTENCE
<P>LIMITS
<P>VIEW-PORT
</BLOCKQUOTE>

<P>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS2219"></a>
<h4> Surface Plot Attributes</h4>

<BLOCKQUOTE>
CHI
<P>CORNER
<P>DRAW-AXIS
<P>DRAW-LABEL
<P>EXISTENCE
<P>LIMITS
<P>LINE-COLOR
<P>LINE-STYLE
<P>LINE-WIDTH
<P>PHI
<P>THETA
</BLOCKQUOTE>

<P>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS2230"></a>
<h4> Vector Plot Attributes</h4>

<BLOCKQUOTE>
CORNER
<P>DRAW-AXIS
<P>DRAW-LABEL
<P>EXISTENCE
<P>LIMITS
<P>LINE-COLOR
<P>LINE-STYLE
<P>LINE-WIDTH
<P>VIEW-PORT
</BLOCKQUOTE>

<P>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGSapi"></a>
<h1 ALIGN="CENTER"> The PGS API</h1>

The application program interface (API) for PGS is presented in this section.
There are three language bindings for most functions in PGS: C; Fortran;
and SX. SX is a part of PACT as is PGS. It is an extended dialect of the
Scheme programming language. What you get using SX is like what you get
using C and loading with the PGS library. Keep in mind however that SX is
an interpreted language and lends itself to certain applications which
are not suitable for C or Fortran. For more on SX see the
SX User&#146;s Manual.<p>

Each language has its own particular features and consequently there are
differences in how the PGS functions are used. We have tried to keep
consistency between the bindings in order to help users who are familiar
with one or more of the languages involved to be able to use any of them.
Some discussion of the language differences is given below and the reader
is STRONGLY urged to READ this material before proceeding.<p>

The following short hand makes for easier explanations:<p>

<UL>
ON and TRUE mean 1 in all languages
<P>
OFF and FALSE mean 0 in all languages
</UL>

<P><CENTER><B>C</B></CENTER>
<P>

C is the language in which PGS is implemented. This means that the C bindings
traffic in the data structures and pointers of the implementation. The other
languages require various devices to obtain a functional equivalent to the C
functionality. In this sense the C bindings are fundamental and the reader
should keep this in mind at times when attempting to understand some of the
more abstruse PGS calls.<p>


<CENTER><B>Fortran</B></CENTER>
<P>

For Fortran functions the type designator REAL indicates arguments which
must be floating point numbers. Whether the actual type declaration in
the calling FORTRAN program should be real or double precision is platform
dependent. In all cases PGS expects a 64 bit quantity.<p>

Since there is no accepted standard for how C and Fortran communicate, it
is necessary for PGS (and all of PACT) to observe one rule regarding string
arguments: two variables are passed. The first is the number of meaningful
characters in the string and the second is the string itself.<p>

All functions in the FORTRAN API return TRUE (1) if PGS detects no error
(some host systems are better than others about reporting error conditions)
and FALSE (0) otherwise unless otherwise noted.<p>

<CENTER><B>SX</B></CENTER>
<P>

In SX as in all LISP dialects two features must be noted and understood.
First, values have types not variables. This means that the description of
the SX bindings don&#146;t show types associated with the formal parameters.
The types of the arguments must match those in the corresponding C calls with
the exception of numeric values (they are coerced to the needed type). Second,
there are no pointers in the C sense and the language uses pass by value
procedure calls. What this means is that nothing is returned to the caller
via the argument list. Instead a list of the return values is made and
returned. The number and order of the values in the list matches those that
are returned via the argument list in the C and Fortran calls.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS0307"></a>
<h2>Compiling and Loading</h2>

To compile your C programs you must use the following<p>

<UL><B>#include &lt;pgs.h&gt;</B></UL>

<p>

in the source files which deal with PGS graphics.<p>

FORTRAN programs have no special requirements of the sources. It is however
important to remember that the PGS FORTRAN routines all begin with
&#147;p&#148; and would be implicitly typed as real when in fact they all
return integers. You should take care to declare the routines which you use.<p>

To link your application you must use the following libraries in the
order specified.<p>


<UL><B>-lpgs [-lX11] -lpdb -lpml -lscore [-lm ...]</B></UL>

<P>

Although this is expressed as if for a UNIX linker, the order would be the
same for any system with a single pass linker. The items in [] are optional
or system dependent.<p>

Each system has different naming conventions for its libraries and the reader
is assumed to understand the appropriate naming conventions as well as knowing
how to tell the linker to find the installed PACT libraries on each system
that they use.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS0316"></a>
<h2> PGS Functionality</h2>

PGS has a wide variety of functionality. The functionality is broken down
into related groups which are listed together in sections and alphabetically
in each section.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS0318"></a>
<h3> Global State Setting Routines</h3>

These routines set state that is global in scope as opposed to device or
graph level control.<p>

<p>

<I>C Binding: </I>int PG_def_marker(int n_seg, double *x1, double *y1, double *x2, 
double *y2)
<BR><I>Fortran Binding: </I>integer pgdmrk(integer n_seg, REAL x1, REAL y1, REAL x2, REAL y2)
<BR><I>SX Binding: </I>(pg-define-marker x1 y1 x2 y2 ...)
<P>
This routine defines a new marker in terms of a set of line segments. The
arguments are the number of segments, n_seg, and arrays specifying the
x and y values of the endpoints of each segment. Each array must be n_seg
elements long. The values in the arrays must be between -1.0 and 1.0. The
marker can be scaled to any size and rotated by using the macros
PG_fset_marker_scale and PG_fset_marker_orientation. The index of the
new marker is returned and should be used as values for the MARKER-INDEX
attribute where called for.<p>

<p>

<I>C Binding: </I>int PG_fset_clear_mode(int mode)
<BR><I>Fortran Binding: </I>integer pgsclm(integer mode)
<BR><I>SX Binding: </I>
<P>
Set a global mode which the high level rendering routines use to interpret
what it means to clear the current picture. There are three interpretations
which PGS supports: 1) is to clear the entire PGS window (CLEAR_SCREEN); 2)
is to clear only the viewspace (CLEAR_VIEWSPACE) this leaves axes and labels
which have already been drawn intact; and 3) clear only the current frame
(CLEAR_FRAME) which leaves still more elements of a picture untouched. Any
other value results in no action being taken by the high level renderers to
clear anything.<p>

<p>

<I>C Binding: </I>
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>(pg-set-view-angle! theta phi chi)
<P>
Set a global default viewing angle for 3D plots. From the observer&#146;s
point of view:  phi is a clockwise rotation about the positive z axis; theta
is a clockwise rotation about the positive x axis; and chi is a
counter-clockwise rotation about the line of sight which is the same as the
z axis after the theta and phi rotations have been applied. The theta rotation
is done so that a view looking down the z axis (x, y) is turned into a view
looking down the y axis (x, z) in the most economical manner - that is with
theta equal to 90 degrees.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS0330"></a>
<h3> Global State Query Routines</h3>

<p>

<I>C Binding: </I>int64_t PG_fget_buffer_size(void)
<BR><I>Fortran Binding: </I>integer pggbsz(void)
<BR><I>SX Binding: </I>(pg-get-buffer-size)
<P>
Get the I/O buffer size for graphics files.<p>

<p>

<I>C Binding: </I>int PG_fget_clear_mode(void)
<BR><I>Fortran Binding: </I>integer pggclm(void)
<BR><I>SX Binding: </I>
<P>
Get the current value of the global mode which the high level rendering
routines use to interpret what it means to clear the current picture. See
PG_fset_clear_mode for a fuller discussion.<p>

<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS0331"></a>
<h3> Memory Management Routines</h3>

These routines allocate and initialize or release instances of PGS
data structures.<p>

<p>

<I>C Binding: </I>PG_device *PG_make_device(char *name, char *type, char *title)
<BR><I>Fortran Binding: </I>integer pgmkdv(integer ncn, char *name, integer nct, char *type, 
integer ncl, char *title)
<BR><I>SX Binding: </I>(pg-make-device name type title)
<P>
Allocate and initialize a new PG_device structure. Name specifies the kind
of device wanted (WINDOW, PS, CGM, RASTER). Type specifies whether the device
is COLOR or MONOCHROME. Title is either the text of a title bar or the name
of an output file as in the case of PS or CGM devices. In the case of PS or
CGM devices the title is used as the base of the file name and &#147;.ps&#148;
or &#147;.cgm&#148; is added as the suffix appropriately. In addition, with
PS devices the EPS conformance level can be specified as follows:<p>

<UL>
base_name [PS-level [EPS-level]]
</UL>

<P>
where PS-level and EPS-level specify the level of conformance.  PGS writes
very highly conforming files but some applications which would import them
cannot recognize standards which are higher or lower than the ones for which
they are programmed. This method lets PGS based applications target their
applications. Values of 2.0 or 3.0 are most common.<p>

<p>

<I>C Binding: </I>PG_graph *PG_make_graph_from_mapping(PM_mapping *f, char 
*info_type, void *info, int id, PG_graph *next)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>
<P>
Setup and return a PG_graph using a PM_mapping, f, and rendering information
in the alist info. The id is a character which will be used as a starting
data-id on a contour plot or as the data-id of a line plot. To chain graphs
together so that they may be plotted together next is used to point to the
next graph in a chain.<p>

<p>

<I>C Binding: </I>PG_graph *PG_make_graph_from_sets(char *label, PM_set 
*domain, PM_set *range, int centering, char *info_type, 
void *info, int id, PG_graph *next)
<BR><I>Fortran Binding: </I>integer pgmgfs(integer nl, char *label, integer domid, integer ranid, 
integer centering, integer id, integer next)
<BR><I>SX Binding: </I>(pg-make-graph domain range [centering color width style emap name])
<P>
Setup a new instance of a PG_graph and return it. The arguments are: the
domain and range sets of the mapping part of the graph; the relative of
the centering of the range and domain data; and rendering information in
the form of an alist, info, or line color, width and style and an existence
map, emap, for the mesh. The label is a string used to label the entire
plot and may be plotted in some circumstances. The id is a character which
will be used as a starting data-id on a contour plot or as the data-id of
a line plot. To chain graphs together so that they may be plotted together
next is used to point to the next graph in a chain.<p>

<p>

<I>C Binding: </I>PG_graph *PG_make_graph_r2_r1(int id, char *label, int cp, 
int kmax, int lmax, int centering, double *x, double *y, 
double *r, char *dname, char *rname)
<BR><I>Fortran Binding: </I>integer pgmg21(integer id, integer nl, char *label, integer cp, integer 
kmax, integer lmax, integer centering, REAL x, REAL y, REAL 
r, integer nd, char *dname, integer nr, char *rname)
<BR><I>SX Binding: </I>
<P>
Setup and return a specific kind of graph containing a 2d rectangular domain
from arrays x and y and a matching 1d range from array r. The size of the
arrays is kmax by lmax. If cp is TRUE the x, y, and r arrays will be copied
for the domain and range sets. It is sometimes necessary for the sets to have
dynamically allocated spaces or to have spaces which they can safely free when
they are released. The id is a character which will be used as a starting
data-id on a contour plot. The dname and rname are strings used a labels for
the domain and range sets respectively.  They are never printed on a plot but
would be written out to a data file. The label is a string used to label the
entire plot and may be plotted in some circumstances.<p>

<p>

<I>C Binding: </I>PG_graph *PG_make_graph_1d(int id, char *label, int cp, 
int n, double *x, double *y, char *xname, char *yname)
<BR><I>Fortran Binding: </I>integer pgmg11(integer id, integer nl, char *label, integer cp, integer n, 
real x, REAL y, integer nx, char *xname, integer ny, char 
*yname)
<BR><I>SX Binding: </I>
<P>
Setup and return a specific kind of graph containing a 1d domain from array
x and a matching 1d range from array y. The size of the arrays is n. If cp
is TRUE the x, y, and r arrays will be copied for the domain and range sets.
The reason for this is that it is sometimes necessary for the sets to have
dynamically allocated spaces or to have spaces which they can safely free
when they are released. The id is a character which will be used as a
data-id on a plot. The xname and yname are strings used a labels for the
domain and range sets respectively.  They are never printed on a plot but
would be written out to a data file. The label is a string used to label
the entire plot and may be plotted in some circumstances.<p>

<p>

<I>C Binding: </I>void PG_rl_graph(PG_graph *g, int rld, int rlr)
<BR><I>Fortran Binding: </I>integer pgrlgr(integer g, integer rld, integer rlr)
<BR><I>SX Binding: </I>                 automatically garbage collected
<P>
This function releases an instance of a PG_graph. If rld is TRUE the data
arrays in the domain set will be freed and if rlr is TRUE the data arrays
in the range set will be freed.<p>

<p>

<I>C Binding: </I>PG_image *PG_make_image_n(char *label, char *type,
int nd, PG_coord_sys cs, double *dbx, double *rbx, int w, int h, int bpp, PG_palette *palette)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>(pg-build-image dev data w h [name xmn xmx ymn ymx zmn zmx])
<P>
The arguments to this function are:<p>

<BLOCKQUOTE>
<TABLE>
<TR><TD>label</TD><TD>a label string for the image</TD></TR>
<TR><TD>type</TD><TD>the data type used in the image</TD></TR>
<TR><TD>nd</TD><TD>dimensionality of the domain</TD></TR>
<TR><TD>cs</TD><TD>coordinate system of dbx</TD></TR>
<TR><TD>dbx</TD><TD>domain extent - also (xmn,xmx) by (ymn,ymx)</TD></TR>
<TR><TD>(xmn, xmx)</TD><TD>the minimum and maximum extent in the x direction</TD></TR>
<TR><TD>(ymn, ymx)</TD><TD>the minimum and maximum extent in the y direction</TD></TR>
<TR><TD>(zmn, zmx)</TD><TD>rbx, the minimum and maximum extent in the image data</TD></TR>
<TR><TD>(w, h)</TD><TD>the number of pixels in the x and y direction respectively</TD></TR>
<TR><TD>bpp</TD><TD>the number of image bits per pixel (1 for MONOCHROME and typically 8 for COLOR)</TD></TR>
<TR><TD>palette</TD><TD>the palette to be used in rendering the image</TD></TR>
</TABLE>
</BLOCKQUOTE>

<P>

<I>C Binding: </I>void PG_rl_image(PG_image *im)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I> automatically garbage collected
<P>
These two functions create and release PG_image instances. The images are
k by l pixels. They have world coordinate extents from xmin to xmax and
ymin to ymax. The data ranges from zmin to zmax and is of type type. A
palette may be supplied along with a label for a plot.<p>

<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS0373"></a>
<h3> Device Control Routines</h3>

These routines provide for high level control of PGS devices. They also
permit applications to set various aspects of the state of devices.<p>

<p>

<I>C Binding: </I>void PG_clear_page(PG_device *dev, int i)
<BR><I>Fortran Binding: </I> integer pgclpg(integer devid, int i)
<BR><I>SX Binding: </I>
<P>
Clear the page for a text window such as the console. Leave the current
line at line i in the PGS window.<p>

<p>

<I>C Binding: </I>void PG_clear_region(PG_device *dev, int nd, PG_coord_sys cs,
double *bx, int pad)
<BR><I>Fortran Binding: </I>integer pgclrg(integer devid, REAL xmn, REAL xmx, REAL ymn, REAL ymx, 
integer pad)
<BR><I>SX Binding: </I>(pg-clear-region dev <xmn xmx ymn ymx> | <xmn xmx ymn ymx zmn zmx> pad)
<P>
Clear the box, specified in coordinates CS, of the
given device. The limits of the box are (bx[0],bx[2],bx[4]) to
(bx[1],bx[3],bx[5]). The pad is a number of pixels to inset the
cleared region. This facilitates clearing a region without removing
a border line around the region.<p>

<p>

<I>C Binding: </I>void PG_clear_window(PG_device *dev)
<BR><I>Fortran Binding: </I>integer pgclsc(integer devid)
<BR><I>SX Binding: </I>(pg-clear-window dev)
<P>
Clear the entire PGS window on the specified device.<p>

<p>

<I>C Binding: </I>void PG_clear_viewspace(PG_device *dev)
<BR><I>Fortran Binding: </I>integer pgclvp(integer devid)
<BR><I>SX Binding: </I>(pg-clear-viewspace dev)
<P>
Clear the current viewspace region only on the specified device.<p>

<p>

<I>C Binding: </I>void PG_close_console(void)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>
<P>
Close the console device.<p>

<p>

<I>C Binding: </I>void PG_close_device(PG_device *dev)
<BR><I>Fortran Binding: </I>integer pgclos(integer dev)
<BR><I>SX Binding: </I>(pg-close-device dev)
<P>
Close the specified device, dev.<p>

<p>

<I>C Binding: </I>void PG_finish_plot(PG_device *dev)
<BR><I>Fortran Binding: </I>integer pgfnpl(integer devid)
<BR><I>SX Binding: </I>(pg-finish-plot dev)
<P>
Finish the picture on the specified device. Once a picture is finished,
nothing more can be drawn to the device until a call to PG_clear_window
is done without serious consequences. This is especially necessary for
devices such as PS and CGM devices.<p>

<p>

<I>C Binding: </I>void PG_fget_axis_log_scale(PG_device *dev, int nd, int *iflg)
<BR><I>Fortran Binding: </I>integer pggaxl(integer devid, int nd, integer ifld)
<BR><I>SX Binding: </I>
<P>
Get the log scale flags in the specified device. The argument IFLG
contains the value of the log flag in each direction.<p>

<p>

<I>C Binding: </I>void PG_make_device_current(PG_device *dev)
<BR><I>Fortran Binding: </I>integer pgmdvc(integer devid)
<BR><I>SX Binding: </I>(pg-make-device-current dev)
<P>
Make the specified device the current device for drawing.<p>

<p>

<I>C Binding: </I>void PG_open_console(char *title, char *type, int bckgr, double xf, 
double yf, double dxf, double dyf)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>
<P>
Open a console device at the specified point (xf, yf) with the specified
width, dxf, and height, dyf (these are all in normalized coordinates). The
console window will have title in the title bar, type, type, and the
indicated background color. Type, the window type is one of &#147;COLOR&#148;
or &#147;MONOCHROME&#148;. Bckgr should be TRUE for white background and
FALSE for black background.<p>

<p>

<I>C Binding: </I>PG_device *PG_open_device(PG_device *dev, double xf, double yf, 
double dxf, double dyf)
<BR><I>Fortran Binding: </I>integer pgopen(integer devid, double xf, double yf, double dxf, double 
dyf)
<BR><I>SX Binding: </I>(pg-open-device dev xf yf dxf dyf)
<P>
Open the specified device at the specified point (xf, yf) with the specified
width, dxf, and height, dyf. These values are all normalized to the physical
device dimensions. NOTE: to make it easy to create a square window, the actual
pixel height of the PGS window is computed as dyf*display_pixel_width!<p>

<p>

<I>C Binding: </I>void PG_release_current_device(PG_device *dev)
<BR><I>Fortran Binding: </I>integer pgrdvc(integer devid)
<BR><I>SX Binding: </I>(pg-release-current-device dev)
<P>
Release the specified device as the current drawing device. (A few host
graphics systems need this functionality).<p>

<p>

<I>C Binding: </I>void PG_set_attributes(PG_device *dev, PG_dev_attributes *attr)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>
<P>
Set the collection of attributes from the PG_dev_attributes structure
attr in the specified device.<p>

<p>

<I>C Binding: </I>void PG_set_attrs_glb(int dflt, ...)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: (pg-set-attrs-global! [<name> <value>]*)</I>
<P>
Set the values of the specified global attributes.<p>

<p>

<I>C Binding: </I>void PG_set_attrs_graph(PG_graph *g,  ...)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: (pg-set-attrs-graph! g [<name> <type> <ptr> <value>]*)</I></I>
<P>
Set the values of the specified attributes in the PG_graph g.<p>

<p>

<I>C Binding: </I>void PG_set_attrs_mapping(PM_mapping *f, ...)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: (pg-set-attrs-mapping! f [<name> <type> <ptr> <value>]*)</I></I>
<P>
Set the values of the specified attributes in the PM_mapping f.<p>

<p>

<I>C Binding: </I>void PG_set_attrs_set(PM_set *s, ...)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: (pg-set-attrs-set! s [<name> <type> <ptr> <value>]*)</I></I>
<P>
Set the values of the specified attributes in the PM_set s.<p>

<p>

<I>C Binding: </I>void PG_fset_axis_log_scale(PG_device *dev, int nd, int *iflg)
<BR><I>Fortran Binding: </I>integer pgsaxl(integer devid, integer nd, integer iflg)
<BR><I>SX Binding: </I>
<P>
Set the axis log scale flags in the specified device. The argument
iflg contains the flag for each direction and causes the corresponding axis
to be plotted with a log scale if TRUE.<p>

<p>

<I>C Binding: </I>int PG_fset_border_width(PG_device *dev, int t)
<BR><I>Fortran Binding: </I>integer pgsbwd(integer devid, integer t)
<BR><I>SX Binding: </I>(pg-set-border-width dev t)
<P>
Set the width of the window border in pixels.<p>

<p>

<I>C Binding: </I>int64_t PG_set_buffer_size(int64_t sz)
<BR><I>Fortran Binding: </I>integer pgsbsz(integer sz)
<BR><I>SX Binding: </I>(pg-set-bbuffer-size! sz)
<P>
Set the I/O buffer size for graphics files.  The old value
is returned.<p>

<p>

<I>C Binding: </I>void PG_fset_clipping(PG_device *dev, int flag)
<BR><I>Fortran Binding: </I>integer pgsclp(integer devid, integer c)
<BR><I>SX Binding: </I>(pg-set-clipping! dev flag)
<P>
Turn on clipping to the current viewspace if flag is TRUE and turn off
clipping to the current viewspace if flag is FALSE on the specified device.
NOTE: moving the viewspace after turning on the clipping does NOT move the
clipping rectangle. To do this turn clipping off and back on again.<p>

<p>

<I>C Binding: </I>int PG_fset_fill_color(PG_device *dev, int color, int mapped)
<BR><I>Fortran Binding: </I>integer pgsfcl(integer devid, integer color, integer mapped)
<BR><I>SX Binding: </I>(pg-set-fill-color! dev color mapped)
<P>
Set the fill color for the device to color. The color index is mapped
through the current palette.<p>

<p>

<I>C Binding: </I>int PG_fset_finish_state(PG_device *dev, int fin)
<BR><I>Fortran Binding: </I>integer pgsfin(integer dev, integer fin)
<BR><I>SX Binding: </I>(pg-set-finish-state! dev fin)
<P>
Set the state of the flag that tells the high level rendering routines
whether or not to assume a plot is finished and issue a call to
PG_finish_plot. This is crucial when doing multiple plots or adding to
a plot after the high level renderer returns.<p>

<p>

<I>C Binding: </I>double PG_fset_marker_orientation(PG_device *dev, double theta)
<BR><I>Fortran Binding: </I>real*8 pgsmko(integer devid, real*8 theta)
<BR><I>SX Binding: </I>(pg-set-marker-orientation! dev theta)
<P>
Set the orientation angle to be applied when drawing markers. Markers can be
drawn at any angle. The angle, theta, is a uniform rotation from the positive
x axis in the counter-clockwise direction of all the segments comprising the
marker.  It is measured in degrees.<p>

<p>

<I>C Binding: </I>double PG_fset_marker_scale(PG_device *dev, double sc)
<BR><I>Fortran Binding: </I>double pgsms(integer devid, REAL sc)
<BR><I>SX Binding: </I>(pg-set-marker-scale! dev v)
<P>
Set the scale factor to be applied when drawing markers. Markers can be
drawn to any size since they are defined in normalized units (see
PG_def_marker). The scale factor sets the actual size. A reasonable
value might be 0.01.<p>

<p>

<I>C Binding: </I>double PG_fset_max_intensity(PG_device *dev, double osc)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>(pg-set-maximum-intensity! dev osc rsc gsc bsc)
<P>
To better match the characteristics of varying output devices (especially
conventional video) this function scales the overall intensity as well as
the intensity of the RGB values down from their maximum of unity. The overall,
red, green, and blue values are controlled by osc, rsc, gsc, and bsc
respectively.<p>

<p>

<I>C Binding: </I>PG_palette *PG_fset_palette(PG_device *dev, char *name)
<BR><I>Fortran Binding: </I>integer pgspal(integer devid, integer nc, char *name)
<BR><I>SX Binding: </I>(pg-set-palette! dev name)
<P>
Set the current palette to be the named one. The built-in palettes are named:
standard, spectrum, rainbow, bw, wb, rgb, cym, hc, bgy, tri, iron, thresh,
rand, reds, yellows, greens, cyans, blues, and magentas. Additional palettes
may be read in with PG_rd_palette or created with PG_make_palette. The
available palettes may be viewed with PG_show_palettes.<p>

<p>

<I>C Binding: </I>int PG_fset_res_scale_factor(PG_device *dev, int f)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>(pg-set-resolution-scale-factor! dev sf)
<P>
Set the value of the hardcopy resolution scale factor. Hardcopy devices can
be very high resolution devices which can lead to enormous image files. This
control lets the application scale down the resolution of the device to keep
image files a reasonable size. The default value is 8, that is by default
the resolution is a factor of 8 less than could be obtained for the device.
This means a factor of 64 in size for raster images.<p>

<p>

<I>C Binding: </I>void PG_fset_viewspace_pos(PG_device *dev, double x, double y)
<BR><I>Fortran Binding: </I>integer pgsvps(integer devid, REAL x, REAL y)
<BR><I>SX Binding: </I>
<P>
Set the position of the viewspace in the window. The specifications are
normalized.<p>

<p>

<I>C Binding: </I>void PG_set_viewspace_shape(PG_device *dev, double width, double 
height, double aspect)
<BR><I>Fortran Binding: </I>integer pgsvsh(integer devid, REAL width, REAL height, REAL aspect)
<BR><I>SX Binding: </I>
<P>
Set the shape of the viewspace in the window. The specifications are normalized.
The aspect ratio is used iff the height is given as 0.0.<p>

<p>

<I>C Binding: </I>void PG_turn_autodomain(PG_device *dev, int n)
<BR><I>Fortran Binding: </I>integer pgsadm(integer dev, integer dm)
<BR><I>SX Binding: </I>(pg-set-autodomain! dev n)
<P>
Determine the domain interval from the data iff n or dm is ON.<p>

<p>

<I>C Binding: </I>void PG_turn_autoplot(PG_device *dev, int n)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>(pg-set-autoplot! dev n)
<P>
Set flag to applications to automatically replot iff n is ON. This is
simply a global variable provided by PGS which applications may use to
control plotting.<p>

<p>

<I>C Binding: </I>void PG_turn_autorange(PG_device *dev, int n)
<BR><I>Fortran Binding: </I>integer pgsarn(integer dev, integer rn)
<BR><I>SX Binding: </I>(pg-set-autorange! dev n)
<P>
Determine the range interval from the data iff n or rn is ON.<p>

<p>

<I>C Binding: </I>void PG_turn_data_id(PG_device *dev, int n)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>(pg-set-data-id-flag! dev n)
<P>
Draw data identifiers on plots iff n is ON.<p>

<p>

<I>C Binding: </I>void PG_turn_grid(PG_device *dev, int n)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>(pg-set-grid-flag! dev n)
<P>
Turn the full axis grid ON or OFF.<p>

<p>

<I>C Binding: </I>void PG_turn_scatter(PG_device *dev, int n)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>(pg-set-scatter-flag! dev n)
<P>
Draw 1D data sets as scatter plots iff n is ON.<p>

<p>

<I>C Binding: </I>void PG_update_vs(PG_device *dev)
<BR><I>Fortran Binding: </I>integer pgupvs(integer devid)
<BR><I>SX Binding: </I>(pg-update-view-surface dev)
<P>
Update the view surface of the specified device. This flushes any
buffered graphics to the output medium of the device.<p>

<p>

<I>C Binding: </I>void PG_white_background(PG_device *dev, int n)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>(pg-set-white-background! dev n)
<P>
If n is TRUE use a white background otherwise use a black background.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS0525"></a>
<h3> Device Query Routines</h3>

<p>

<I>C Binding: </I>int COLOR_POSTSCRIPT_DEVICE(PG_device *dev)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>
<P>
TRUE iff dev is a color PostScript device.<p>

<p>

<I>C Binding: </I>
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>(pg-device-properties dev)
<P>
Return the name, type, and title of the device as given in the
PG_make_device call which created the device.<p>

<p>

<I>C Binding: </I>PG_dev_attributes *PG_get_attributes(PG_device *dev)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>
<P>
Collect and return the selection of attributes from the specified device
in a newly allocated PG_dev_attributes structure.<p>

<p>

<I>C Binding: </I>void PG_get_attrs_glb(int dflt, ...)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>
<P>
Get the values of the specified global attributes.<p>

<p>

<I>C Binding: </I>void PG_get_attrs_graph(PG_graph *g, ...)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>
<P>
Get the values of the specified attributes in the PG_graph g.<p>

<p>

<I>C Binding: </I>void PG_get_attrs_mapping(PM_mapping *f, ...)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>
<P>
Get the values of the specified attributes in the PM_mapping f.<p>

<p>

<I>C Binding: </I>void PG_get_attrs_set(PM_set *s, ...)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>
<P>
Get the values of the specified attributes in the PM_set s.<p>

<p>

<I>C Binding: </I>int PG_fget_clipping(PG_device *dev)
<BR><I>Fortran Binding: </I>integer pggclp(integer devid)
<BR><I>SX Binding: </I>(pg-clipping? dev)
<P>
Get the current clipping state for the specified device in flag.<p>

<p>

<I>C Binding: </I>
<BR><I>Fortran Binding: </I>integer pggfin(integer dev, integer fin)
<BR><I>SX Binding: </I>(pg-finish-state dev)
<P>
Return the state of the flag that tells the high level rendering
routines whether or not to assume a plot is finished and issue a
call to PG_finish_plot.<p>

<p>

<I>C Binding: </I>double PG_fget_marker_orientation(PG_device *dev)
<BR><I>Fortran Binding: </I>real*8 pggmko(integer dev)
<BR><I>SX Binding: </I>(pg-marker-orientation dev)
<P>

Get the current marker orientation angle of the device. Markers can be
drawn at any angle. The angle returned in v is a uniform rotation from
the positive x axis in the counter-clockwise direction of all the segments
comprising the marker.  The angle is measured in degrees.<p>

<p>

<I>C Binding: </I>double PG_fget_marker_scale(PG_device *dev)
<BR><I>Fortran Binding: </I>real*8 integer pggmks(integer devid)
<BR><I>SX Binding: </I>(pg-marker-scale dev)
<P>
Get the current marker scale factor of the device. Markers can be drawn to any
size since they are defined in normalized units (see PG_def_marker). The scale
factor returned in s controls the actual size.<p>

<p>

<I>C Binding: </I>
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>(pg-maximum-intensity dev)
<P>
Return the maximum intensity aggregate value and the individual values
for red, green, and blue colors in the specified device. The values are
normalized (0.0 to 1.0).<p>

<p>

<I>C Binding: </I>PG_palette *PG_fget_palette(PG_device *dev, char *name)
<BR><I>Fortran Binding: </I>                         not applicable
<BR><I>SX Binding: </I>(pg-palette-&gt;list dev name)
<P>
Returns a pointer to the palette specified by name.<p>

<p>

<I>C Binding: </I>
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>(pg-palettes dev)
<P>
Return a list of palettes available for the specified device.<p>

<p>

<I>C Binding: </I>int POSTSCRIPT_DEVICE(PG_device *dev)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>
<P>
TRUE iff dev is a PostScript device.<p>

<p>

<I>C Binding: </I>void PG_query_screen_n(PG_device *dev, int *dx, int *pnc)
<BR><I>Fortran Binding: </I>integer pgqdev(integer devid, integer dx, integer nc)
<BR><I>SX Binding: </I>(pg-query-device dev)
<P>
Query the device for size in pixels and color planes. The number
of colors which a device supports is 2nplanes.<p>

<p>

<I>C Binding: </I>void PG_query_window(PG_device *dev, int *pdx, int *pdy)
<BR><I>Fortran Binding: </I>integer pgqwin(integer devid, integer dx, integer dy)
<BR><I>SX Binding: </I>(pg-query-window dev)
<P>
Query the shape of the window on the device.<p>

<p>

<I>C Binding: </I>
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>(pg-show-markers)
<P>
Temporarily spawn a window to display the available marker characters on
the specified device.<p>


<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS0581"></a>
<h3> Coordinate Transformation Routines</h3>

These routines transform points from one coordinate system to another. The
three coordinate systems are: pixel coordinates referring to the integer
coordinates of pixels in a device; normalized coordinates whose values range
from 0.0 to 1.0 and are device independent; and world coordinates which are
user defined coordinates (see PG_set_window) tailored to the particular
application at hand.<p>

<p>

<I>C Binding: </I>void PtoS(PG_device *dev, int ix, int iy, double x, double y)
<BR><I>Fortran Binding: </I>integer pgptos(integer devid, integer ix, integer iy, REAL x, REAL y)
<BR><I>SX Binding: </I>(pg-pixel-&gt;normalized dev ix iy)
<P>
Converts (ix, iy) from pixel coordinates to NDC/Screen coordinates (x, y).<p>

<p>

<I>C Binding: </I>void StoP(PG_device *dev, double x, double y, int ix, int iy)
<BR><I>Fortran Binding: </I>integer pgstop(integer devid, REAL x, REAL y, integer ix, integer iy)
<BR><I>SX Binding: </I>(pg-normalized-&gt;pixel dev x y)
<P>
Converts (x, y) from NDC/Screen coordinates to pixel coordinates (ix, iy).<p>

<p>

<I>C Binding: </I>void StoW(PG_device *dev, double x, double y)
<BR><I>Fortran Binding: </I>integer pgstow(integer devid, REAL x, REAL y)
<BR><I>SX Binding: </I>(pg-normalized-&gt;world dev x y)
<P>
Converts (x, y) from screen coordinates to world coordinates.<p>

<p>

<I>C Binding: </I>void WtoS(PG_device *dev, double x, double y)
<BR><I>Fortran Binding: </I>integer pgwtos(integer devid, REAL x, REAL y)
<BR><I>SX Binding: </I>(pg-world-&gt;normalized dev x y)
<P>
Converts (x, y) from world coordinates to screen coordinates.<p>

<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS0604"></a>
<h3> Coordinate System and Viewspace Control Routines</h3>

These routines provide access to the coordinate system and viewspace
of a PGS window.<p>

<p>

<I>C Binding: </I>void PG_get_frame(PG_device *dev, double *x1, double *x2, double 
*y1, double *y2)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>(pg-frame dev)
<P>
Get the frame of the specified device. The x and y intervals are specified
in NDC by (x1, x2) and (y1, y2) respectively.<p>

<p>

<I>C Binding: </I>void PG_get_viewspace(PG_device *dev, double *x1, double *x2, double 
*y1, double *y2)
<BR><I>Fortran Binding: </I>integer pggvwp(integer devid, REAL x1, REAL x2, REAL y1, REAL 
y2)
<BR><I>SX Binding: </I>(pg-viewspace dev)
<P>
Get the viewspace of the specified device. The x and y intervals are
specified in NDC by (x1, x2) and (y1, y2) respectively.<p>

<p>

<I>C Binding: </I>void PG_get_window(PG_device *dev, double *xmn, double *xmx, 
double *ymn, double *ymx)
<BR><I>Fortran Binding: </I>integer pggwcs(integer devid, REAL x1, REAL x2, REAL y1, REAL 
y2)
<BR><I>SX Binding: </I>(pg-world-coordinate-system dev)
<P>
Get the world coordinate system defined relative to the viewspace for the
specified device. The x and y intervals are specified in WC by (x1, x2)
and (y1, y2) respectively.<p>

<p>

<I>C Binding: </I>
<BR><I>Fortran Binding: </I>integer pgrvpa(integer devid, integer n)
<BR><I>SX Binding: </I>
<P>
Restore the current viewspace, coordinate transformations, and related graphical
state. A previously saved state (see pgsvpa) is referenced by the index n.<p>

<p>

<I>C Binding: </I>
<BR><I>Fortran Binding: </I>integer pgsvpa(integer devid, integer n)
<BR><I>SX Binding: </I>
<P>
Save the current viewspace, coordinate transformations, and related graphical
state. If n &lt; 0, a new space is internally allocated. Otherwise n is
interpreted as an existing state whose space will be reused. Returns an index
in n which is to be used with a corresponding call to pgrvpa.<p>

<p>

<I>C Binding: </I>void PG_set_frame(PG_device, double x1, double x2, double y1, 
double y2)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>(pg-set-frame! dev x1 x2 y1 y2)
<P>
Set the frame of the specified device. The x and y intervals are specified
in NDC by (x1, x2) and (y1, y2) respectively.<p>

<p>

<I>C Binding: </I>void PG_set_limits_n(PG_device *dev, int nd, PG_coord_sys cs, long n,
 double **x, int type)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>
<P>
Set the coordinate system CS (defined relative to the viewspace) for
the specified device finding the limits of the N points in the supplied
data arrays. The X arrays are specified in CS. Type is the plot
type (PLOT_INSEL, PLOT_HISTOGRAM, PLOT_POLAR, PLOT_CARTESIAN).<p>

<p>

<I>C Binding: </I>void PG_set_viewspace(PG_device *dev, double x1, double x2, double 
y1, double y2)
<BR><I>Fortran Binding: </I>integer pgsvwp(integer devid, REAL x1, REAL x2, REAL y1, REAL 
y2)
<BR><I>SX Binding: </I>(pg-set-viewspace! dev x1 x2 y1 y2)
<P>
Set the viewspace of the specified device. The x and y intervals are
specified in NDC by (x1, x2) and (y1, y2) respectively.<p>

<p>

<I>C Binding: </I>void PG_set_window(PG_device *dev, double x1, double x2, double y1, 
double y2)
<BR><I>Fortran Binding: </I>integer pgswcs(integer devid, REAL x1, REAL x2, REAL y1, REAL 
y2)
<BR><I>SX Binding: </I>(pg-set-world-coordinate-system! dev x1 x2 y1 y2)
<P>
Set the world coordinate system defined relative to the viewspace for the
specified device. The x and y intervals are specified in WC by (x1, x2)
and (y1, y2) respectively.<p>

<p>

<I>C Binding: </I>void PG_fset_view_angle(PG_device *dev, int cnv, double *theta, double *phi, double *chi)
<BR><I>Fortran Binding: </I>integer pgsva(integer devid, integer cnv, REAL theta, REAL phi, REAL chi)
<BR><I>SX Binding: </I>(pg-set-view-angle! dev cnv theta phi chi)
<P>
Set the view angle for the specified device. The angles are the three Euler
angles in degrees.  The rotations are: phi about the z axis; followed by theta
about the new x axis; and finally chi about the new z axis.
<p>
If CNV is TRUE the angles are in degrees, otherwise in radians.
<p>
The device is optional in the SX binding.  If absent the angles are stored as
global state.
<p>

<I>C Binding: </I>void PG_fget_view_angle(PG_device *dev, int cnv, double *theta, double *phi, double *chi)
<BR><I>Fortran Binding: </I>integer pggva(integer devid, integer cnv, REAL theta, REAL phi, REAL chi)
<BR><I>SX Binding: </I>(pg-get-view-angle [dev] theta phi chi)
<P>
Get the view angle for the specified device. The angles are the three Euler
angles in degrees.  The rotations are: phi about the z axis; followed by theta
about the new x axis; and finally chi about the new z axis.
<p>
If CNV is TRUE the angles are in degrees, otherwise in radians.
<p>

<I>C Binding: </I>void PG_fset_light_angle(PG_device *dev, int cnv, double *theta, double *phi, double *chi)
<BR><I>Fortran Binding: </I>integer pgsva(integer devid, integer cnv, REAL theta, REAL phi, REAL chi)
<BR><I>SX Binding: </I>(pg-set-light-angle! dev cnv theta phi chi)
<P>
Set the angle of light sources for the specified device.
The angles are two Euler angles.  The angles are: phi about the z axis; and theta about the x axis.
<p>
If CNV is TRUE the angles are in degrees, otherwise in radians.
<p>

<I>C Binding: </I>void PG_fget_light_angle(PG_device *dev, int cnv, double *theta, double *phi, double *chi)
<BR><I>Fortran Binding: </I>integer pggva(integer devid, integer cnv, REAL theta, REAL phi, REAL chi)
<BR><I>SX Binding: </I>(pg-get-light-angle [dev] theta phi chi)
<P>
Get the angle of light sources for the specified device. The angles are two Euler angles.  The angles are: phi about the z axis; and theta
about the x axis.
<p>
If CNV is TRUE the angles are in degrees, otherwise in radians.
<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS0652"></a>
<h3> Line Attribute Control Routines</h3>

These routines provide control over how lines appear when drawn.<p>

<p>

<I>C Binding: </I>int PG_fget_line_color(PG_device *dev)
<BR><I>Fortran Binding: </I>integer pgglnc(integer devid)
<BR><I>SX Binding: </I>(pg-color-line dev)
<P>

<p>
<I>C Binding: </I>int PG_fget_line_style(PG_device *dev)
<BR><I>Fortran Binding: </I>integer pgglns(integer devid)
<BR><I>SX Binding: </I>(pg-line-style dev)
<P>

<p>
<I>C Binding: </I>double PG_fget_line_width(PG_device *dev)
<BR><I>Fortran Binding: </I>real*8 pgglnw(integer devid)
<BR><I>SX Binding: </I>(pg-line-width dev)
<P>

<p>
<I>C Binding: </I>int PG_fset_line_color(PG_device *dev, int lc, int mapped)
<BR><I>Fortran Binding: </I>integer pgslnc(integer devid, integer lc, integer mapped)
<BR><I>SX Binding: </I>(pg-set-line-color! dev lc mapped)
<P>

<p>
<I>C Binding: </I>int PG_fset_line_style(PG_device *dev, int ls)
<BR><I>Fortran Binding: </I>integer pgslns(integer devid, integer ls)
<BR><I>SX Binding: </I>(pg-set-line-style! dev ls)
<P>

<p>
<I>C Binding: </I>double PG_fset_line_width(PG_device *dev, double lw)
<BR><I>Fortran Binding: </I>real*8 pgslnw(integer devid, real*8 lw)
<BR><I>SX Binding: </I>(pg-set-line-width! dev lw)
<P>
These routines access the state contained in a PG_device pertaining to lines.
Specifically color, width, or style. The set routines take the device and
the new value, while the get routines take a device and the address where
the current line attribute value is to be put. Line widths go from 0.0
(thinnest) on up with 3.0 being a very thick line. Line styles are:
LINE_SOLID, LINE_DOTTED, LINE_DASHED, LINE_DOTDASHED.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS0675"></a>
<h3> Text Attribute Control Routines</h3>

The following provide control over text properties. A good deal of this
is either unsupported by many host graphics systems or is superceded by
the font based approach common in more modern host graphics systems. Users
should preferentially use routines addressing themselves to font and
type faces.<p>

<p>

<CENTER><B></B></CENTER><CENTER><B></B></CENTER><I>C Binding: </I>
<BR><I>Fortran Binding: </I>integer pgscpw(integer devid, REAL x, REAL y)
<BR><I>SX Binding: </I>(pg-set-char-path! dev x y)
<P>
This routine sets the direction along which text will be written.<p>

<p>

<I>C Binding: </I>void PG_fset_char_size_n(PG_device *dev, int nd, PG_coord_sys cs, double w, double h)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>
<P>
Set the current character size in CS coordinates.<p>

<p>

<I>C Binding: </I>double PG_fset_char_space(PG_device *dev, double s)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>
<P>
This routine sets the spacing between characters in world coordinates.<p>

<p>

<I>C Binding: </I>void PG_fset_char_up(PG_device *dev, double *x)
<BR><I>Fortran Binding: </I>void pgscuw(integer devid, REAL *x)
<BR><I>SX Binding: </I>(pg-set-char-up! dev x y)
<P>
This routine sets the direction along which characters will be oriented. This
is usually orthogonal to the direction along which characters are written.<p>

<p>

<I>C Binding: </I>void PG_fset_font(PG_device *dev, char *face, char *style, int size)
<BR><I>Fortran Binding: </I>void pgstxf(integer devid, integer ncf, char *face, integer ncs, char 
*style, integer size)
<BR><I>SX Binding: </I>(pg-set-text-font! dev face style size)
<P>
This routine sets the font in the specified device. Face refers generically
to the type face. PGS always supports helvetica, courier, and times. Style
refers to the type style and the options are: medium, bold, italic, and
bold-italic. Size refers to type size in points.<p>

<p>

<I>C Binding: </I>int PG_fset_text_color(PG_device *dev, int tc, int mapped)
<BR><I>Fortran Binding: </I>integer pgstxc(integer devid, integer tc, integer mapped)
<BR><I>SX Binding: </I>(pg-set-text-color! dev tc mapped)
<P>
Set the text color for the specified device.<p>

<p>

<CENTER><B></B></CENTER><CENTER><B></B></CENTER><I>C Binding: </I>
<BR><I>Fortran Binding: </I>integer pggcpw(integer devid, REAL x, REAL y)
<BR><I>SX Binding: </I>(pg-character-path dev)
<P>
This routine returns the direction along which text will be written.<p>

<p>

<I>C Binding: </I>
<BR><I>Fortran Binding: </I>integer pggcss(integer devid, REAL w, REAL h)
<BR><I>SX Binding: </I>(pg-character-size-ndc dev)
<P>
Return the current character size in normalized coordinates.<p>

<p>

<I>C Binding: </I>
<BR><I>Fortran Binding: </I>integer pggcsw(integer devid, REAL w, REAL h)
<BR><I>SX Binding: </I>
<P>
Get the current character size in world coordinates.<p>

<p>

<I>C Binding: </I>double PG_fget_char_space(PG_device *dev)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>
<P>
This routine returns the spacing between characters in world coordinates.<p>

<p>

<I>C Binding: </I>void PG_fget_char_up(PG_device *dev, double *x)
<BR><I>Fortran Binding: </I>void pggcuw(integer devid, REAL *x)
<BR><I>SX Binding: </I>(pg-character-up dev)
<P>
This routine returns the direction along which characters will be oriented.
This is usually orthogonal to the direction along which characters
are written.<p>

<p>

<I>C Binding: </I>void PG_fget_font(PG_device *dev, char **face, char **style, int *size)
<BR><I>Fortran Binding: </I>void pggtxf(integer devid, integer ncf, char *face, integer ncs, char 
*style, integer size)
<BR><I>SX Binding: </I>(pg-text-font dev)
<P>
This routine queries the font in the specified device. Face refers generically
to the type face. PGS always supports helvetica, courier, and times. Style
refers to the type style and the options are: medium, bold, italic, and
bold-italic. Size refers to type size in points. The FORTRAN binding has
some extra behavior. The string lengths here are both input and output
variables.  On input they contain the lengths of the string buffers, face
and style. On output they contain the number of actual characters in their
respective strings.  If the buffers are not long enough pggtxf returns FALSE
and does nothing but return the lengths of the strings. The application can
then make a second call with larger buffers.<p>

<p>

<I>C Binding: </I>int PG_fget_text_color(PG_device *dev)
<BR><I>Fortran Binding: </I>integer pggtxc(integer devid)
<BR><I>SX Binding: </I>(pg-text-color dev)
<P>
Query the text color for the specified device.<p>

<p>

<I>C Binding: </I>void PG_get_text_ext_n(PG_device *dev, int nd, PG_coord_sys cs,
 char *s, double *)
<BR><I>Fortran Binding: </I>integer pggtex(integer devid, int nd, PG_coord_sys cs, integer nc, char *s, REAL dx, REAL dy)
<BR><I>SX Binding: </I>(pg-text-extent dev nd cs s)
<P>
This routine returns the extent of the character string S in coordinate
system CS as a vector in p.<p>

<p>

<I>C Binding: </I>
<BR><I>Fortran Binding: </I>integer pggtes(integer devid, integer nc, char *s, REAL dx, REAL dy)
<BR><I>SX Binding: </I>
<P>
This routine returns the normalized coordinate extent of the character string s as a width in px and height in py.<p>

<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS0762"></a>
<h3> Graphical Text I/O Routines</h3>

These routines provide the control over terminal or file I/O in graphical
applications. The first two are directed at the special device,
PG_gs.console, which is opened with PG_open_console.<p>

<p>

<I>C Binding: </I>void PG_center_label(PG_device *dev, double sy, char *label)
<BR><I>Fortran Binding: </I>integer pgwrcl(integer dev, REAL sy, integer nc, char *label)
<BR><I>SX Binding: </I>(pg-center-label dev sy label)
<P>
This routine prints a text string, label, on the specified device and centered horizontally with a normalized vertical position specified by sy.<p>

<p>

<I>C Binding: </I>int PG_write_n(PG_device *dev, int nd, PG_coord_sys cs, double *x, char *fmt, ...)
<BR><I>Fortran Binding: </I>integer pgwrta(integer devid, REAL x, REAL y, integer nc, char *txt)
<BR><I>SX Binding: </I>(pg-draw-text dev nd, cs, x y txt)
<P>
This routine does an snprintf style print to the specified device, dev,
and at the point, in CS, specified by X.<p>

<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS0785"></a>
<h3> Point Move Routines</h3>

PGS maintains two &#147;points&#148;, a text point and a drawing point, at
which the next text and line drawing operations will start.<p>

<p>

<I>C Binding: </I>void PG_move_gr_abs_n(PG_device *dev, double *p)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>

<p>

<I>C Binding: </I>void PG_move_tx_abs_n(PG_device *dev, double *p)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>

<p>

<I>C Binding: </I>void PG_move_tx_rel_n(PG_device *dev, double *p)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>
<P>
These move the line drawing point (gr) or the text drawing point (tx) to
an absolute world coordinate point or relative to the current world
coordinate point.<p>

<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS0799"></a>
<h3> Primitive Drawing Routines</h3>

These routines are fundamental drawing routines. Some of these are primitive
in the sense that the host graphics systems all seem to supply them. The
others are trivial applications of the primitive ones. However, these are
termed fundamental in that most other PGS drawing routines are expressed
directly in terms of them.<p>

<p>

<I>C Binding: </I>void PG_draw_line_n(PG_device *dev, int nd, PG_coord_sys cs,
				      double *x1, double *x2, int clip)
<BR><I>Fortran Binding: </I>integer pgdrln(integer devid, REAL x1, REAL y1, REAL x2, REAL 
y2)
<BR><I>SX Binding: </I>(pg-draw-line dev <x1 y1 x2 y2> | <x1 y1 z1 x2 y2 xz>)
<P>
This routine draws a line between two ND dimensional points in CS
coordinates specified by X1 and X2.<p>

<p>

<I>C Binding: </I>void PG_draw_to_abs_n(PG_device *dev, double *p)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>
<P>
<p>

<I>C Binding: </I>void PG_draw_to_rel_n(PG_device *dev, double *p)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>
<P>
These draw a line segment on the specified device to the absolute world
coordinate point specified or relative to the current world coordinate point.
These both reset the current drawing point to the destination endpoint.<p>

<p>

<I>C Binding: </I>void PG_draw_polyline_n(PG_device *dev, int nd, PG_coord_sys cs,
long n, double **x, int clp)
<BR><I>Fortran Binding: </I>integer pgdpln(integer devid, REAL x, REAL y, integer n, integer clp)
<BR><I>SX Binding: </I>(pg-draw-polyline-n dev nd cs clp <x1 y1 x2 y2 ...> | <x1 y1 z1 x2 y2 z1 ...>)
<P>
This routine draws a connected line on the device, dev, starting with the
first point and ending with the last point. The n points are specified in
CS coordinates and are contained in the X arrays. CLP causes the
polyline to be clipped to the current viewspace if TRUE.<p>

<p>

<I>C Binding: </I>void PG_shade_poly(PG_device *dev, double *x, double *y, int n)
<BR><I>Fortran Binding: </I>integer pgfply(integer devid, REAL *px, REAL *py, integer n, integer 
c)
<BR><I>SX Binding: </I>(pg-fill-polygon dev c x1 y1 x2 y2 ...)
<P>
This routine draws a polygon specified by the n world coordinate points
in the x and y arrays and fills it with the current fill color
(see PG_fset_fill_color).<p>

<p>

<I>C Binding: </I>void PG_shade_poly_n(PG_device *dev, int nd, int n, double **r)
<BR><I>Fortran Binding: </I>
c)
<BR><I>SX Binding: </I>
<P>
This routine draws an nd dimensional polygon specified by the
n world coordinate points
in the arrays r and fills it with the current fill color
(see PG_fset_fill_color).<p>

<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS0830"></a>
<h3> Basic Line Drawing Routines</h3>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS0831"></a>
These routines add some creature comforts to PGS line drawing. They are
still low level routines, but they handle some fairly common situations.<p>

<p>

<I>C Binding: </I>void PG_draw_arc(PG_device *dev, double r, double a1, double a2, 
double x, double y, int unit)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>(pg-draw-arc dev r a1 a2 x y unit)
<P>
This routine draws an arc with radius, r, in world coordinates from angles
a1 to a2 centered about the point (x, y) in world coordinates. Unit controls
whether the angle is specified in degrees or radians. See the description
for PG_draw_rad below for additional details.<p>

<p>

<I>C Binding: </I>void PG_draw_box_n(PG_device *dev, int nd, PG_coord_sys cs, double *bx)
<BR><I>Fortran Binding: </I>integer pgdrbx(integer devid, REAL xmn, REAL xmx,
                                       REAL ymn, REAL ymx)
<BR><I>SX Binding: </I>(pg-draw-box dev xmn xmx ymn ymx zmn zmx)
<P>
This routine draws a box from xmn to xmx, ymn to ymx, and zmn to zmx
which are specified in world coordinates.  In C bx[0] is xmn, bx[1], is xmx,
bx[2] is ymn, bx[3] is ymx, and so on.  The argument cs is one of WORLDC,
NORMC, or PIXELC<p>

<p>

<I>C Binding: </I>void PG_draw_disjoint_polyline_n(PG_device *dev, int nd,
PG_coord_sys cs, long n, double **x, int clip)
<BR><I>Fortran Binding: </I>integer pgddpn(integer devid, REAL *x, REAL *y, REAL *z,
integer n, integer clip, integer cs)
<BR><I>SX Binding: </I>(pg-draw-disjoint-polyline-n dev clip norm x1 y1 z1 x2 y2 
z2 x3 y3 z3 x4 y4 z4 ...)
<P>
This routine draws disjoint ND dimensional line segments specified in
CS coordinates. The number of segments, N, is half the number of points.
The arrays x, y, and z define the endpoint vectors X. X[2*i] is one endpoint
of the ith segment, and X[2*i+1] is the other endpoint. The flag CLIP specifies
whether the line segments are clipped to the viewspace limits.<p>

<p>

<I>C Binding: </I>void PG_draw_markers_n(PG_device *dev, int nd, PG_coord_sys cs,
int n, double **r, int marker)
<BR><I>Fortran Binding: </I>integer pgdrmk(integer dev, integer n, REAL x, REAL y, integer marker)
<BR><I>SX Binding: </I>(pg-draw-markers nd cs dev marker x y [z])
<P>
This routine draws the marker character specified by marker at each of
the N ND points in CS defined by the R arrays on the
specified device.<p>

<p>

<I>C Binding: </I>void PG_draw_rad(PG_device *dev, double rmin, double rmax, 
double a, double x, double y, int unit)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>(pg-draw-radius dev rmin rmax a x y unit)
<P>
This routine draws a radial line from the central point (x, y) from rmin to
rmax along the direction specified by the angle a (with respect to positive
x axis increasing counter-clockwise). If unit has the value DEGREE then the
angle a is in degrees and otherwise it is in radians (there is a RADIAN
constant for symmetry).<p>

<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS0857"></a>
<h3> Axis Drawing Routines</h3>

These routines handle various aspects of drawing axes. The most basic
routine here draws a single axis with very general and controllable
characteristics.<p>

<p>

<I>C Binding: </I>void PG_axis(PG_device *dev, int axis_type)
<BR><I>Fortran Binding: </I>integer pgaxis(integer devid, integer axt)
<BR><I>SX Binding: </I>(pg-axis device type)
<P>
This routine draws a set of axes which are tied to the viewspace and world
coordinate system. The valid values for axis_type are: CARTESIAN_2D, POLAR,
and INSEL. The axes are drawn on the view boundary. See the drawing model
section above for the definition of the view boundary.<p>

<p>

<I>C Binding: </I>void PG_axis_3(PG_device *dev, double **x,
int np, double *extr)
<BR><I>Fortran Binding: </I>integer pgdax3(integer devid, REAL *x, REAL *y, REAL *z,
 integer n, integer norm)
<BR><I>SX Binding: </I>
<P>
This routine draws a simple set of 3D axes oriented at the specified angle.
The extent of the axes is determined by the n_pts points in (x[0], x[1], x[2]).
The unrotated limits of the data are specified by xmn, xmx, ymn, ymx, zmn,
and zmx.<p>

<p>

<I>C Binding: </I>PG_axis_def *PG_draw_axis_n(PG_device *dev, double *xl, 
double *xr, double *tn, double *vw, double sc, char *format, int tick_type, int 
label_type, int flag, ...)
<BR><I>Fortran Binding: </I>integer pgdrax(integer devid, REAL xl, REAL yl, REAL xr, REAL yr, 
REAL t1, REAL t2, REAL v1, REAL v2, REAL sc, 
integer nc, char *format, integer tickdef, integer 
tick_type, integer label_type)
<BR><I>SX Binding: </I>(pg-draw-axis dev xl yl xr yr t1 t2 v1 v2 sc format tick_type label_type 
tickdef)
<P>
This routine will draw a single axis and produce labels and/or ticks
depending on the arguments. The arguments are:<p>


<TABLE>
<TR><TD>(xl, yl)</TD><TD>coordinate of beginning end</TD></TR>
<TR><TD>(xr, yr)</TD><TD>coordinate of terminating end</TD></TR>
<TR><TD>(t1, t2)</TD><TD>fractional position of v1 and v2</TD></TR>
<TR><TD>(v1, v2)</TD><TD>first and last tick or label value</TD></TR>
<TR><TD>sc</TD><TD>an additional scale factor which is used, for example, when doing an 
Inselberg axis in which the range may correspond to one appropriate for the 
perpendicular dimension (set to 1.0 in most cases)</TD></TR>
<TR><TD>format</TD><TD>specifies the label format in the standard C way</TD></TR>
<TR><TD WIDTH="100">tick_type</TD><TD>types of ticks</TD></TR>
</TABLE>
<TABLE>
<TR><TD WIDTH="120"><TD>AXIS_TICK_RIGHT</TD><TD>ticks on right</TD></TR>
<TR><TD></TD><TD>AXIS_TICK_LEFT</TD><TD>ticks on left</TD></TR>
<TR><TD></TD><TD>AXIS_TICK_STRADDLE</TD><TD>ticks straddle (both)</TD></TR>
</TABLE>
<TABLE>
<TR><TD WIDTH="100">label_type</TD><TD>types of labels</TD></TR>
</TABLE>
<TABLE>
<TR><TD WIDTH="120"></TD><TD>AXIS_TICK_RIGHT</TD><TD>labels on right</TD></TR>
<TR><TD></TD><TD>AXIS_TICK_LEFT</TD><TD>labels on left</TD></TR>
<TR><TD></TD><TD>AXIS_TICK_NONE</TD><TD>no labels</TD></TR>
<TR><TD></TD><TD>AXIS_TICK_ENDS</TD><TD>labels at ends of axis</TD></TR>
</TABLE>
<TABLE>
<TR><TD WIDTH="100">tickdef</TD><TD>specifies the labels and ticks - one or more may be given and the list is 
terminated with a value of 0.</TD></TR>
</TABLE>
<TABLE>
<TR><TD WIDTH="120"></TD><TD>AXIS_TICK_MAJOR</TD><TD>major ticks</TD></TR>
<TR><TD></TD><TD>AXIS_TICK_MINOR</TD><TD>minor ticks</TD></TR>
<TR><TD></TD><TD>AXIS_TICK_LABEL</TD><TD>labels</TD></TR>
</TABLE>
<TABLE>
<TR><TD WIDTH="100">flag</TD><TD>if TRUE return a pointer to a PG_axis def structure (this is mainly for 
internal use) otherwise return NULL.</TD></TR>
</TABLE>

<P>
An axis is a directed line segment (from Xl to Xr) with ticks. The label
values as defined by v1, v2, t1, and t2; the ticks associate with the
line segment as follows:<p>

<pre>
  (xl, yl)                                 (xr, yr)<p>

   ------------------------------------------&gt;        Axis<p>
      |                               |                  Ticks<p>
   v1 = v(t1)                      v2 = v(t2)            Tick Labels (v)<p>
</pre>

<p>

<I>C Binding: </I>int PG_set_axis_attributes(PG_device *dev, ...)
<BR><I>Fortran Binding: </I>integer pgsaxa(integer devid, integer n, REAL attr, char *attrs)
<BR><I>SX Binding: </I>
<P>
This routine sets the parameters which control the look of the axes being
drawn. An arbitrary number of specifications can be made in key/value pairs.
The list is terminated with a zero key. In the Fortran Binding the key/value
pairs are placed as floating point numbers in the array attr. The value n
is the number of pairs. If the attribute is a character string, the value
is put in the character array attrs, and the number of characters is put
in attr as the attribute value following the attribute key. The character
strings are packed together with no space between the attribute values.
This also means that the order of the attributes in the array attr must
match those in attrs since only the number of characters is used to
associate the attribute with its position in the string attrs.<p>

 The control keys are:<p>


<BLOCKQUOTE>
<TABLE>

<TR>
<TD WIDTH="150">Name</TD>
<TD>Type</TD>
<TD>Value</TD>
<TD>Description</TD>
</TR>
<TR>
<TD>AXIS_LINESTYLE</TD>
<TD>integer</TD>
<TD ALIGN="CENTER">1</TD>
<TD>style of the lines</TD>
</TR>
<TR>
<TD>AXIS_LINETHICK</TD>
<TD>real</TD>
<TD ALIGN="CENTER">2</TD>
<TD>thickness of the lines</TD>
</TR>
<TR>
<TD>AXIS_LINECOLOR</TD>
<TD>integer</TD>
<TD ALIGN="CENTER">3</TD>
<TD>color of the lines</TD>
</TR>
<TR>
<TD>AXIS_LABELCOLOR</TD>
<TD>integer</TD>
<TD ALIGN="CENTER">4</TD>
<TD>color of the labels</TD>
</TR>
<TR>
<TD>AXIS_LABELFONT</TD>
<TD>char *</TD>
<TD ALIGN="CENTER">6</TD>
<TD>label font type face</TD>
</TR>
<TR>
<TD>AXIS_LABELPREC</TD>
<TD>integer</TD>
<TD ALIGN="CENTER">7</TD>
<TD>character precision</TD>
</TR>
<TR>
<TD>AXIS_X_FORMAT</TD>
<TD>char *</TD>
<TD ALIGN="CENTER">8</TD>
<TD>format of the x labels</TD>
</TR>
<TR>
<TD>AXIS_Y_FORMAT</TD>
<TD>char *</TD>
<TD ALIGN="CENTER">9</TD>
<TD>format of the y labels</TD>
</TR>
<TR>
<TD>AXIS_TICKSIZE</TD>
<TD>real</TD>
<TD ALIGN="CENTER">10</TD>
<TD>tick size in fraction of axis length</TD>
</TR>
<TR>
<TD>AXIS_GRID_ON</TD>
<TD>integer</TD>
<TD ALIGN="CENTER">11</TD>
<TD>turn on grid if TRUE</TD>
</TR>
<TR>
<TD>AXIS_SIGNIF_DIGIT</TD>
<TD>integer</TD>
<TD ALIGN="CENTER">12</TD>
<TD>number of digits in labels</TD>
</TR>
<TR>
<TD>AXIS_CHAR_ANGLE</TD>
<TD>real</TD>
<TD ALIGN="CENTER">13</TD>
<TD>orientation angle for label characters</TD>
</TR>
<TR>
<TD></TD>
<TD></TD>
<TD></TD>
<TD>(not supported on all devices)</TD>
</TR>

</TABLE>
</BLOCKQUOTE>

<P><I>C Binding: </I>double PG_fget_axis_decades(void)
<BR><I>Fortran Binding: </I>integer pggaxd(real d)
<BR><I>SX Binding: </I>
<P>
This routine returns the current maximum number of decades that log axes will
span in the argument d. This facility is primarily aimed at making the use of
logarithmic axes more flexible by defining a user controlled limit to the
number of decades plotted. In this way, potentially ill-defined logarithmic
values (such as very small positive numbers) don&#146;t interfere with the
display of otherwise fine values.<p>

<I>C Binding: </I>void PG_fset_axis_decades(double d)
<BR><I>Fortran Binding: </I>integer pgsaxd(real d)
<BR><I>SX Binding: </I>
<P>
This routine sets the current maximum number of decades that log axes will
span to the value of the argument d. This facility is primarily aimed at
making the use of logarithmic axes more flexible by defining a user
controlled limit to the number of decades plotted. In this way, potentially
ill-defined logarithmic values (such as very small positive numbers)
don&#146;t interfere with the display of otherwise fine values.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS0915"></a>
<h3> Colormap Related Routines</h3>

<p>

<I>C Binding: </I>void PG_show_colormap(PG_device *dev, int all)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>
<P>
Display the colormap for the current palette if all is FALSE and for
the entire device colormap if all is TRUE.<p>

<p>

<I>C Binding: </I>void PG_show_palettes(PG_device *dev, char *type, int wbck)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>(pg-show-palettes dev wbck)
<P>
This routine pops up a temporary new window to display the palettes available
for the specified device. The user may browse through the palettes and select
one to be the new current palette. When the selection is made the palette
window goes away. The type argument specifies the type of device to be spawned
(WINDOW, PS, or CGM), this is useful for making hardcopies of the palette set.
The wbck argument is TRUE if a white background is wanted and FALSE for a
black one.<p>

<p>

<I>C Binding: </I>PG_palette *PG_make_palette(PG_device *dev, char *name, int nc, 
int wbck)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>(pg-make-palette dev name nc wbck)
<P>
This routine pops up a temporary new window with the available colors of the
device and lets the user select from those colors to build up a new palette.
When finished the window is dismissed and the new palette becomes the current
palette of the device. Colors are selected by clicking on them with the left
mouse button. Any number of colors may be selected in this fashion, however,
colors selected after the ncth replace previously selected colors. When the
new palette is completed, clicking the right button signals acceptance and
the window goes away. The new palette is written into a file whose name is
the same as the palette name and with a &#147;.pal&#148; extension. The number
of colors requested is nc and if wbck is TRUE the temporary window has a white
background (otherwise a black one). The palette file can be read in again with
the PG_rd_palette function.<p>

<p>

<I>C Binding: </I>PG_palette *PG_rd_palette(PG_device *dev, char *fname)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>(pg-read-palette dev fname)
<P>
This function reads a palette file and makes the resulting palette the current
palette of the specified device. The format of a palette file is simple. It is
an ASCII file whose first line contains the name of the palette and the number
of colors, nc. The next nc lines contain normalized red, green, and blue
values.<p>

<p>

<I>C Binding: </I>int PG_wr_palette(PG_device *dev, PG_palette *pal, char *fname)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>(pg-write-palette dev pal fname)
<P>
This function writes a specified palette, pal, to a palette file. The format
of a palette file is simple. It is an ASCII file whose first line contains
the name of the palette and the number of colors, nc. The next nc lines
contain normalized red, green, and blue values.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS0941"></a>
<h3> Graph Control Routines</h3>

These routines control the state of graphs or act on them.<p>

These routines are at the other end of the scale from the previous low
level routines. They perform complex drawing operations which are driven
by user oriented specifications.<p>

<p>

<I>C Binding: </I>void PG_draw_graph(PG_device *dev, PG_graph *data)
<BR><I>Fortran Binding: </I>integer pgplot(integer devid, integer dataid)
<BR><I>SX Binding: </I>(pg-draw-graph dev [data]* rendering)
<P>
This routine draws the graph specified by data on the device specified by
dev. The PG_graph structure contains both data and rendering specifications.<p>

<p>

<I>C Binding: </I>void PG_domain_plot(PG_device *dev, PM_set *dom, PM_set *ran)
<BR><I>Fortran Binding: </I>integer pgdplt(integer devid, integer domid)
<BR><I>SX Binding: </I>(pg-draw-domain dev [dom]* [type extrema])
<P>
This routine draws the domain set specified by dom on the device specified
by dev. The PM_set structure contains the data. This is the generalization
of a mesh plot. A range set, ran, may optionally be provided if labels or
other information associated with the mesh points are desired. The FORTRAN
and SX bindings do not allow a range to be given at this time. The SX binding
does allow for plotting limits to be set on the domain. They are specified by
n (min, max) pairs where n is the dimensionality of the domain. The plot type
may also be directly specified as PLOT_SURFACE, PLOT_MESH, or PLOT_WIRE_MESH
instead of in the attribute list of the PM_set. The default is
PLOT_WIRE_MESH<p>

<p>

<I>C Binding: </I>int PG_fget_identifier(PG_graph *g)
<BR><I>Fortran Binding: </I>integer pgggid(integer gid)
<BR><I>SX Binding: </I>
<P>
Return the identifier character, id, of the specified graph.
<p>

<p>

<I>C Binding: </I>pcons *PG_fget_render_info(PG_graph *g)
<BR><I>Fortran Binding: </I>integer pgginf(integer gid, integer alst)
<BR><I>SX Binding: </I>
<P>
Return the attribute list, alst, of the specified graph.
<p>

<p>

<I>C Binding: </I>
<BR><I>Fortran Binding: </I>integer pgsdlm(integer grid, integer n, double v)
<BR><I>SX Binding: </I>(pg-set-domain-limits! gr v1mn v1mx ...)
<P>
Restrict the domain of the mapping contained in the graph to the values in
the array v. There are 2n values in (min, max) pairs where n is the
dimensionality of the domain.<p>

<p>

<I>C Binding: </I>int PG_set_identifier(PG_graph *g, int id)
<BR><I>Fortran Binding: </I>integer pgsgid(integer gid, integer id)
<BR><I>SX Binding: </I>
<P>
Set the identifier character, id, of the specified graph. Return
the old value.
<p>

<p>

<I>C Binding: </I>pcons *PG_fset_render_info(PG_graph *g, pcons *alst)
<BR><I>Fortran Binding: </I>integer pgsinf(integer gid, integer alst)
<BR><I>SX Binding: </I>
<P>
Set the attribute list, alst, of the specified graph.
<p>

<p>

<I>C Binding: </I>
<BR><I>Fortran Binding: </I>integer pgsrat(integer grid, integer n, char *name, integer t, char 
*type, char *val)
<BR><I>SX Binding: </I>(pg-set-graph-attribute! gr name type val)
<P>
Set a single rendering attribute in the specified graph. The name of the
attribute is in name, its type in type and its value in val. See the section
on attributes for more information on attributes and their values.<p>

<p>

<I>C Binding: </I>void PG_set_plot_type(pcons *inf, int plt, int axs)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>
<P>
Set the plot type, plt, and axis type, axs, for the specified association
list, inf. This list is usually the info part of a PG_graph.<p>

<p>

<I>C Binding: </I>
<BR><I>Fortran Binding: </I>integer pgsrlm(integer grid, integer n, double v)
<BR><I>SX Binding: </I>(pg-set-range-limits! gr v1mn v1mx ...)
<P>
Restrict the range of the mapping contained in the graph to the values in
the array v. There are 2n values in (min, max) pairs where n is the
dimensionality of the range.<p>

<p>

<I>C Binding: </I>
<BR><I>Fortran Binding: </I>integer pgsvlm(integer grid, double v)
<BR><I>SX Binding: </I>
<P>
A PG_graph may have its own viewspace limits which supercede the device
viewspace limits. This function takes an array, v, containing the limits
and attaches them to the info list of the specified graph, grid. The
limits are arranged as xmin, xmax, ymin, ymax.<p>

<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS0975"></a>
<h3> Line Plot Routines</h3>

These routines plot curves in various renderings.<p>

<p>

<I>C Binding: </I>void PG_plot_curve(PG_device *dev, double *x, double *y, int n, pcons 
*info, int l)
<BR><I>Fortran Binding: </I>integer pgplln(integer devid, REAL *px, REAL *py, integer n, integer 
mod, integer axt, integer col, REAL wid, integer sty, 
integer sca, integer mrk, integer sta, integer l)
<BR><I>SX Binding: </I>                             use pg-draw-graph
<P>
This routine is a moderately high level routine in that it will call the
other routines depending on the values in the association list info and plot
the n points in the x and y arrays. In the FORTRAN binding the following
attributes may be passed directly:<p>

<BLOCKQUOTE>
<TABLE>
<TR><TD WIDTH="40">mod</TD>
<TD>plot type (PLOT_CARTESIAN, PLOT_POLAR, PLOT_INSEL)</TD></TR>
<TR><TD>axt</TD>
<TD>axis type (CARTESIAN_2D, POLAR, INSEL)</TD></TR>
<TR><TD>col</TD>
<TD>line color</TD></TR>
<TR><TD>wid</TD>
<TD>line width</TD></TR>
<TR><TD>sty</TD>
<TD>line style</TD></TR>
<TR><TD>sca</TD>
<TD>scatter plot flag</TD></TR>
<TR><TD>mrk</TD>
<TD>marker index</TD></TR>
<TR><TD>sta</TD>
<TD>histogram starting point (LEFT, CENTER, RIGHT)</TD></TR>
<TR><TD>l</TD>
<TD>if FALSE the world coordinates are recomputed from the data and the
axes are drawn</TD></TR>
</TABLE>
</BLOCKQUOTE>

<P><I>C Binding: </I>void PG_histogram_plot(PG_device *dev, double *x, double *y, int n, 
int lncol, double lnwid, int lnsty, int scatter, int marker, 
int start, int l)
<BR><I>Fortran Binding: </I>  use pgplot
<BR><I>SX Binding: </I>   use pg-draw-graph
<P>
<p>

<I>C Binding: </I>void PG_insel_plot(PG_device *dev, double *x, double *y, int n, int 
lncol, double lnwid, int lnsty, int l)
<BR><I>Fortran Binding: </I>  use pgplot
<BR><I>SX Binding: </I>   use pg-draw-graph
<P>
<p>

<I>C Binding: </I>void PG_polar_plot(PG_device *dev, double *x, double *y, int n, int 
lncol, double lnwid, int lnsty, int scatter, int marker, int 
l)
<BR><I>Fortran Binding: </I>  use pgplot
<BR><I>SX Binding: </I>   use pg-draw-graph
<P>
<p>

<I>C Binding: </I>void PG_rect_plot(PG_device *dev, double *x, double *y, int n, int 
lncol, double lnwid, int lnsty, int scatter, int marker, int 
l)
<BR><I>Fortran Binding: </I> use pgplot
<BR><I>SX Binding: </I>  use pg-draw-graph
<P>
These routines plot the n points in the x and y arrays as a: rectangular
cartesian plot; histogram plot; Inselberg plot; or polar plot. The
qualifying arguments are:<p>

<BLOCKQUOTE>
<TABLE>
<TR><TD WIDTH="40">lncol</TD>
<TD>line color</TD></TR>
<TR><TD>lnwid</TD>
<TD>line width</TD></TR>
<TR><TD>lnsty</TD>
<TD>line style</TD></TR>
<TR><TD>scatter</TD>
<TD>TRUE for scatter plot</TD></TR>
<TR><TD>marker</TD>
<TD>index of marker character for scatter plots</TD></TR>
<TR><TD>start</TD>
<TD>LEFT, RIGHT, or CENTER start for histogram plots</TD></TR>
<TR><TD>l</TD>
<TD>if TRUE the world coordinate system is redefined by x and y data</TD></TR>
</TABLE>
</BLOCKQUOTE>

<P>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS1002"></a>
<h3> Contour Plotting Routines</h3>

<p>

<I>C Binding: </I>void PG_contour_plot(PG_device *dev, PG_graph *data)
<BR><I>Fortran Binding: </I>                                  use pgplot
<BR><I>SX Binding: </I>                             use pg-draw-graph
<P>
This routine renders the specified graph as a contour plot. PG_draw_graph dispatches
to this routine when the rendering specified in data is a contour plot.<p>

<p>

<I>C Binding: </I>
<BR><I>Fortran Binding: </I>integer pgplcn(integer devid, REAL x, REAL y, REAL z, REAL lev, 
integer k, integer l, integer nlev, integer labl, integer 
alst)
<BR><I>SX Binding: </I>                             use pg-draw-graph
<P>
Make a contour plot of the given data set on the specified device.
The arguments are:<p>

<BLOCKQUOTE>
<TABLE>
<TR><TD>(x, y)	</TD><TD>coordinate point arrays</TD></TR>
<TR><TD>z	</TD><TD>data array</TD></TR>
<TR><TD>lev	</TD><TD>contour level value array</TD></TR>
<TR><TD>(k, l)	</TD><TD>array dimensions</TD></TR>
<TR><TD>nlev	</TD><TD>number of contour levels</TD></TR>
<TR><TD>labl	</TD><TD>starting label character if non-zero</TD></TR>
<TR><TD>alst	</TD><TD>an integer attribute list identifier
(use 0 if none)</TD></TR>
</TABLE>
</BLOCKQUOTE>

<P>
<p>

<I>C Binding: </I>int PG_contour_levels(double *lev, int nlev, double fmn, double fmx, 
double ratio)
<BR><I>Fortran Binding: </I>integer pgclev(REAL lev, integer nlev, REAL fmn, REAL fmx, REAL 
ratio)
<BR><I>SX Binding: </I>                             set LEVEL attribute
<P>
Compute an array of nlev iso contour levels between fmn and fmx using
the spacing ratio. Put them in the space provided, lev. The arguments are:<p>

<BLOCKQUOTE>
<TABLE>
<TR><TD>lev		</TD><TD>the array of contour levels</TD></TR>
<TR><TD>nlev		</TD><TD>the number of contour levels</TD></TR>
<TR><TD>(fmn, fmx)	</TD><TD>the minimum and maximum values for
contour levels</TD></TR>
<TR><TD>ratio		</TD><TD>the spacing ratio between contour
levels</TD></TR>
</TABLE>
</BLOCKQUOTE>

<P>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS1019"></a>
<h3> Filled Polygon Plot Routines</h3>

<p>

<I>C Binding: </I>void PG_poly_fill_plot(PG_device *dev, PG_graph *data)
<BR><I>Fortran Binding: </I>                                    use pgplot
<BR><I>SX Binding: </I>                             use pg-draw-graph
<P>
This routine renders the specified graph as a filled polygon plot.
PG_draw_graph dispatches to this routine when the rendering specified
in data is a a filled polygon plot.<p>

<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS1026"></a>
<h3> Image Plot Routines</h3>

<p>

<I>C Binding: </I>void PG_image_plot(PG_device *dev, PG_graph *data)
<BR><I>Fortran Binding: </I>                                   use pgplot
<BR><I>SX Binding: </I>                             use pg-draw-graph
<P>
This routine renders the specified graph as a raster image plot.
PG_draw_graph dispatches to this routine when the rendering specified
in data is an image plot.<p>

<p>

<I>C Binding: </I>void PG_draw_image(PG_device *dev, PG_image *im, char *label, 
pcons *alist)
<BR><I>Fortran Binding: </I>
<BR><I>SX Binding: </I>(pg-draw-image dev im)
<P>
This routine makes an image plot from a PG_image structure. A pointer to the
image is in im and a label for the plot is in label.<p>

<p>

<I>C Binding: </I>
<BR><I>Fortran Binding: </I>integer pgplim(integer devid, integer nc, char *name, integer nct, char 
*type, REAL *pz, integer k, integer l, REAL xmn, 
REAL xmx, REAL ymn, REAL ymx, REAL zmn, 
REAL zmx, integer alst)
<BR><I>SX Binding: </I>
<P>
This routine makes an image plot from &#147;raw&#148; data.
The arguments are:<p>

<BLOCKQUOTE>
<TABLE>
<TR><TD>name</TD>
<TD>the name of the image (used as a label for the plot)</TD></TR>
<TR><TD>type</TD>
<TD>the type of the data (&#147;char&#148;, &#147;short&#148;,
&#147;int&#148;, &#147;long&#148;, &#147;float&#148;,
&#147;double&#148;)</TD></TR>
<TR><TD>z</TD>
<TD>the array of pixel values (will be scaled to the current palette
of the device)</TD></TR>
<TR><TD>(k, l)</TD>
<TD>the dimensions of the image</TD></TR>
<TR><TD>(xmn, xmx)</TD>
<TD>the minimum and maximum x values (for axis labels)</TD></TR>
<TR><TD>(ymn, ymx)</TD>
<TD>the minimum and maximum y values (for axis labels)</TD></TR>
<TR><TD>(zmn, zmx)</TD>
<TD>the nminimum and maximum data values (for palette labels)</TD></TR>
<TR><TD>alst</TD>
<TD>an integer attribute list identifier (use 0 if none)</TD></TR>
</TABLE>
</BLOCKQUOTE>

<P><I>C Binding: </I>void PG_draw_palette_n(PG_device *dev, double *dbx,
 double *rbx, double wid, int exact)
<BR><I>Fortran Binding: </I>integer pgdrpp(integer devid, REAL xmn, REAL xmx, 
REAL ymn, REAL ymx, REAL zmn, REAL zmx, REAL wid, integer exact)
<BR><I>SX Binding: </I>(pg-draw-palette dev xmn xmx ymn ymx zmn zmx wid exact)
<P>
This routine draws the specified device palette as a raster image. The palette is
drawn next to the viewspace in a rectangle along an axis specified by the points
(xmn, ymn) and (xmx, ymx). The axis is labeled by values ranging from zmn to zmx.
The width of the palette in the rectangle is specified in normalized form by wid.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS1042"></a>
<h3> Surface Plot Routines</h3>

<p>

<I>C Binding: </I>void PG_surface_plot(PG_device *dev, PG_graph *data)
<BR><I>Fortran Binding: </I>                                   use pgplot
<BR><I>SX Binding: </I>                             use pg-draw-graph
<P>
This routine renders the specified graph as a surface plot. PG_draw_graph dispatches
to this routine when the rendering specified in data is a surface plot.<p>

<p>

<I>C Binding: </I>void PG_draw_surface_n(PG_device *dev, double *a1, double *a2, 
double *aext, double **r, int nn, double *va, double width, int color, int style, 
int type, char *label, char *mesh_type, void *cnnct, 
pcons *alist)
<BR><I>Fortran Binding: </I>integer pgplsf(integer devid, REAL *px, REAL *py, REAL *pz, 
integer n, REAL xn, REAL xx, REAL yn, REAL yx, 
REAL zn, REAL zx, integer kx, integer lx, REAL th, 
REAL ph, REAL ch, integer typ, integer col, REAL wid, 
integer sty, integer nc, char *label)
<BR><I>SX Binding: </I>                             use pg-draw-graph
<P>
This routine is a medium level routine which can be called directly from applications.
The data can be rendered as a wire frame mesh or as a true shaded surface. Both forms
do hidden line and hidden surface removal. The algorithm uses a raster scan line
approach with a single Z buffer line. This choice minimizes the memory requirements of
the routine at some expense of speed.<p>

The arguments are:<p>


<BLOCKQUOTE>
<TABLE>
<TR><TD>dev</TD>
<TD>the device to which the plot is drawn</TD></TR>
<TR><TD>a1, a2</TD>
<TD>the arrays specifying height of the surface and color shading. If 
the color array is NULL the a1 array will do both height and color in a shaded 
plot</TD></TR>
<TR><TD>aext</TD>
<TD>the minimum and maximum values for a1 and a2</TD></TR>
<TR><TD>x, y</TD>
<TD>the x and y components of the positions of the nodes</TD></TR>
<TR><TD>nn</TD>
<TD>the number of points</TD></TR>
<TR><TD>xmn, xmx</TD>
<TD>the minimum and maximum values for x</TD></TR>
<TR><TD>ymn, ymx</TD>
<TD>the minimum and maximum values for y</TD></TR>
<TR><TD>theta, phi, chi</TD>
<TD>the Euler view angles</TD></TR>
<TR><TD>width</TD>
<TD>the line width to use</TD></TR>
<TR><TD>color</TD>
<TD>the line color to use</TD></TR>
<TR><TD>style</TD>
<TD>the line style to use</TD></TR>
<TR><TD>type</TD>
<TD>PLOT_SURFACE or PLOT_WIRE_MESH</TD></TR>
<TR><TD>name</TD>
<TD>a label for the plot</TD></TR>
<TR><TD>mesh_type</TD>
<TD>&#147;Logical-Rectangular&#148; (LR)
or &#147;Arbitrarily-Connected&#148; (AC)</TD></TR>
<TR><TD>cnnct</TD>
<TD>connectivity specifications: array of dimensions for LR meshes 
or a pointer to a PM_mesh_topology struct for AC meshes</TD></TR>
<TR><TD>alist</TD>
<TD>association list of plotting attributes</TD></TR>
</TABLE>
</BLOCKQUOTE>

<P>
<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS1070"></a>
<h3> Vector Plot Routines</h3>

<p>

<I>C Binding: </I>void PG_vector_plot(PG_device *dev, PG_graph *data)
<BR><I>Fortran Binding: </I>                                   use pgplot
<BR><I>SX Binding: </I>                             use pg-draw-graph
<P>
This routine renders the specified graph as a vector plot. PG_draw_graph
dispatches to this routine when the rendering specified in data is a vector
plot.<p>

<p>

<I>C Binding: </I>
<BR><I>Fortran Binding: </I>integer pgplvc(integer devid, REAL *px, REAL *py, REAL *pu, 
REAL *pv, integer n, integer alst)
<BR><I>SX Binding: </I>                             use pg-draw-graph
<P>
Make a vector plot of the given data set on the specified device.<p>

<BLOCKQUOTE>
<TABLE>
<TR><TD>devid</TD>
<TD>an integer attribute list identifier (use 0 if none)</TD></TR>
<TR><TD>(px, py)</TD>
<TD>farrays containing the x and y positions</TD></TR>
<TR><TD>(pu, pv)</TD>
<TD>farrays containing the u and v vector components</TD></TR>
<TR><TD>n</TD>
<TD>an integer number of vectors</TD></TR>
<TR><TD>alst</TD>
<TD>an integer attribute list identifier (use 0 if none)</TD></TR>
</TABLE>
</BLOCKQUOTE>

<P>
<p>

<I>C Binding: </I>void PG_set_vec_attr(PG_device *dev, ...)
<BR><I>Fortran Binding: </I>integer pgsvat(integer devid, ...)
<BR><I>SX Binding: </I>(pg-set-vector-attributes! dev ...)
<P>
This routine sets the properties of the vectors for the next vector plot. The
parameters are paired, optional, and can be in any order. For each pair, the
first value describes the option, the second, the value. The options are ints.
The values can be ints, REALs, or chars. What type the values are is determined
by the option. Most values are normalized to unity with the angle being the
exception. The list must be ended with a zero. The attributes are:<p>

<BLOCKQUOTE>
<TABLE>

<TR>
<TD>C Id</TD>
<TD>FORTRAN Id</TD>
<TD ALIGN="CENTER">Description</TD>
<TD>Default</TD>
</TR>
<TR>
<TD>VEC_SCALE</TD>
<TD ALIGN="CENTER">1</TD>
<TD>scale factor on lengths</TD>
<TD ALIGN="CENTER">1.0</TD>
</TR>
<TR>
<TD>VEC_ANGLE</TD>
<TD ALIGN="CENTER">2</TD>
<TD>angle between wings</TD>
<TD ALIGN="CENTER">22.5</TD>
</TR>
<TR>
<TD>VEC_HEADSIZE</TD>
<TD ALIGN="CENTER">3</TD>
<TD>length of the wings</TD>
<TD ALIGN="CENTER">0.05</TD>
</TR>
<TR>
<TD>VEC_FIXSIZE</TD>
<TD ALIGN="CENTER">4</TD>
<TD>a fixed vector length</TD>
<TD ALIGN="CENTER">0.0</TD>
</TR>
<TR>
<TD>VEC_MAXSIZE</TD>
<TD ALIGN="CENTER">5</TD>
<TD>a maximum vector length</TD>
<TD ALIGN="CENTER">0.0</TD>
</TR>
<TR>
<TD>VEC_LINESTYLE</TD>
<TD ALIGN="CENTER">6</TD>
<TD>line style of vectors</TD>
<TD ALIGN="CENTER">1</TD>
</TR>
<TR>
<TD>VEC_LINETHICK</TD>
<TD ALIGN="CENTER">7</TD>
<TD>line width of vectors</TD>
<TD ALIGN="CENTER">0.0</TD>
</TR>
<TR>
<TD>VEC_COLOR</TD>
<TD ALIGN="CENTER">8</TD>
<TD>line color of vectors</TD>
<TD ALIGN="CENTER">WHITE</TD>
</TR>
<TR>
<TD>VEC_FIXHEAD</TD>
<TD ALIGN="CENTER">9</TD>
<TD>a fixed head size</TD>
<TD ALIGN="CENTER">FALSE</TD>


</TABLE>
</BLOCKQUOTE>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS1096"></a>
<h3> Level Diagram Plot Routine</h3>

This routine draws a level diagram or grotrian plot.<p>


<p>

<I>C Binding: </I>void PG_grotrian_plot(PG_device *dev, PG_graph *data)
<BR><I>Fortran Binding: </I>                                   use pgplot
<BR><I>SX Binding: </I>                             use pg-draw-graph
<P>
This routine renders the specified graph as a level diagram plot.
PG_draw_graph dispatches to this routine when the rendering specified in
data is a level diagram plot. These graphs have an unusual data layout
and would not produce a very meaningful plot rendered any other way.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS1103"></a>
<h3> Graphical Interface Routines</h3>

These routines supply a capability for applications to develop portable,
user and runtime configurable graphical user interfaces.<p>

<p>

<I>C Binding: </I>void PG_draw_interface_objects(PG_device *dev)
<BR><I>Fortran Binding: </I>integer pgdrif(integer devid)
<BR><I>SX Binding: </I>
<P>
This routine draws the entire graphical interface of the specified device.<p>

<p>

<I>C Binding: </I>void PG_query_pointer(PG_device *dev, int *ir, int *pbtn, int *mod)
<BR><I>Fortran Binding: </I>integer pgqptr(integer devid, integer x, integer y, integer btn, integer 
mod)
<BR><I>SX Binding: </I>
<P>
This routine allows the application to query the state of the locator or
mouse. The mouse state is: the (x, y) position on the screen in pixel
coordinates; btn, the indicator of mouse buttons which are pressed
(MOUSE_LEFT, MOUSE_CENTER, MOUSE_RIGHT); and mod the indicator of which
keyboard modifiers are pressed (KEY_SHIFT, KEY_CNTL, KEY_ALT). These flags
can be and&#146;d to ascertain which combinations are pressed.<p>

<p>

<I>C Binding: </I>int PG_read_interface(PG_device *dev, char *fname)
<BR><I>Fortran Binding: </I>integer pgrdif(integer devid, integer n, char *fname)
<BR><I>SX Binding: </I>
<P>
This routine reads the entire graphical interface from the named file
into the specified device.<p>

<p>

<I>C Binding: </I>void PG_register_callback(char *name, PFVoid fnc)
<BR><I>Fortran Binding: </I>integer pgrgfn(integer nc, char *name, function fnc)
<BR><I>SX Binding: </I>
<P>
This routine registers a function with PGS so that it may be called by an
interface event such as the click of a button or selection of some item of
an interface. The name is used to refer to the function (via a lookup) in
such applications<p>

<I>C Binding: </I>void PG_register_variable(char *name, char *type, void *var, void 
*vmin, void *vmax)
<BR><I>Fortran Binding: </I>integer pgrgvr(integer nc, char *name, integer nt, char *type, var, 
vmin, vmax)
<BR><I>SX Binding: </I>
<P>
This routine registers a variable with PGS so that its value may be changed
by an interface event such as the selection of some item of from a menu. The
required information is the variables name, type, and address (var).
Optionally pointers to the minimum and maximum values, vmin and vmax, may
be supplied (NULL if not wanted).<p>

<p>

<I>C Binding: </I>int PG_write_interface(PG_device *dev, char *fname)
<BR><I>Fortran Binding: </I>integer pgwrif(integer devid, integer n, char *fname)
<BR><I>SX Binding: </I>
<P>
This routine writes the entire graphical interface of the specified
device to the named file.<p>

<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGSgen"><h2>PGS Functions</h2></a>


The following documentation is generated automatically from the
PGS source.  As such it is absolutely the most up to date description
of the functions and supercedes any details in the documentation above.
<p>
Note: some of the functions listed refer to a type, pboolean.  This is
currently an int but is being used as a transition to the C99 bool type.
<p>
<a href="gh-pgs.html">Function bindings</a>
<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS1135"></a>
<h2> Structures</h2>

PGS employs several structures to encapsulate information pertaining to
specific groups of functionality. C based applications can have access to
these structures (not a good idea in general because the structures may
change) and some readers may find the structures revealing of details in
the design and implementation of PGS. Some of these are also discussed in
the context of rendering. See those sections for additional details.<p>

<p>

<BR><B>PG_graph
</B><BR>
The PG_graph structure contains information specifying how data is to
be rendered. That includes specification of the rendering technique, line
attributes, and other information. A PG_graph always contains a PM_mapping
(a structure defined by the PACT library PML) pointer for the data set to
be rendered.<p>

PG_graph&#146;s can be linked together in a list and the PGS
rendering functions will render all graphs in the list.<p>

An associated type of the PG_graph is:<p>

<UL> typedef PG_graph *(*PFPPG_graph)(); </UL>
<P>

The last type is a Pointer to a Function returning a Pointer to a PG_GRAPH.
This convention is used throughout PACT. See the related documentation for
further information.<p>

<BR><B>PG_palette
</B><BR>
The PG_palette structure contains the specification of a palette of RGB
colors. The number of colors in the palette is determined by the application.
If the number of colors specified exceeds the number of colors which the
host platform can display, PGS attempts to simulate the colors with dithering
of colors which the host graphics system can display.<p>

<BR><B>PG_device
</B><BR>
The PG_device structure contains the information which the host graphics
system requires applications to provide and maintain and it keeps a set of
state variables describing such quantities as coordinate systems, palettes,
and drawing attributes.<p>

<p>

<BR><B>PG_image
</B><BR>
The PG_image structure contains the specification for cell array or
image plots. It contains data, the data type, the array dimensions, bounding
values for use in defining scales, palette information, and other appropriate
data.<p>

<p>

<BR><B>PG_dev_attributes
</B><BR>
The PG_dev_attributes structure contains a large number of the commonly
queried and set attributes found in the PG_device. The intent of this
structure is to allow applications to access and change many device
attributes quickly and efficiently. The attributes include line, text,
and fill colors, line style and width, palette, and clipping state. It
is convenient to save these all at once in a PG_dev_attributes, change
the device state, perform drawing operation, and restore the original
device state from the PG_dev_attributes.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS1154"></a>
<h2 align=center>PGS Constants</h2>

PGS defines and uses several #define&#146;d constants. These can be
used by applications and are listed here by category.<p>

<p>

<h4>General Purpose Constants</h4>
<BLOCKQUOTE>
<TABLE>
<TR><TD>PG_SPACEDM</TD><TD>3</TD><td>Maximum dimension of view space</td></TR>
<TR><TD>PG_BOXSZ</TD><TD>6</TD><td>Maximum size of view space box</td></TR>
<TR><TD>PG_NANGLES</TD><TD>3</TD><td>Number of Euler angles</td></TR>
<TR><TD>N_COLORS</TD><TD>16</TD><td>Number of colors in standard palette</td></TR>
<TR><TD>N_ANGLES</TD><TD>180</TD><td>Number of angles when drawing arcs</td></TR>
</TABLE>
</BLOCKQUOTE>

<h4>Angle Unit Constants</h4>

The following constants are defined in the PG_angle_unit enumeration:
<BLOCKQUOTE>
<TABLE>
<TR><TD>DEGREE</TD><td>Angles in degrees</td></TR>
<TR><TD>RADIAN</TD><td>Angles in radians</td></TR>
</TABLE>
</BLOCKQUOTE>

<h4>Coordinate System Constants</h4>

The following constants are defined in the PG_coord_sys enumeration:
<BLOCKQUOTE>
<TABLE>
<TR><TD>WORLDC</TD><td>World coordinates</td></TR>
<TR><TD>NORMC</TD><td>Normalized coordinates</td></TR>
<TR><TD>PIXELC</TD><td>Pixel coordinates</td></TR>
<TR><TD>BOUNDC</TD><td>Boundary coordinates</td></TR>
<TR><TD>FRAMEC</TD><td>Frame coordinates</td></TR>
</TABLE>
</BLOCKQUOTE>

<h4>Device Characterization Constants</h4>

The following constants are defined in the PG_dev_type enumeration:
<BLOCKQUOTE>
<TABLE>
<TR><TD>TEXT_WINDOW_DEVICE</TD></TR>
<TR><TD>GRAPHIC_WINDOW_DEVICE</TD></TR>
<TR><TD>PS_DEVICE</TD></TR>
<TR><TD>CGMF_DEVICE</TD></TR>
<TR><TD>PNG_DEVICE</TD></TR>
<TR><TD>JPEG_DEVICE</TD></TR>
<TR><TD>MPEG_DEVICE</TD></TR>
<TR><TD>HARD_COPY_DEVICE</TD></TR>
<TR><TD>IMAGE_DEVICE</TD></TR>
<TR><TD>RASTER_DEVICE</TD></TR>
</TABLE>
</BLOCKQUOTE>

<h4>Rendering Mode Constants</h4>

The following constants are defined in the PG_rendering enumeration:
<BLOCKQUOTE>
<TABLE>
<TR><TD WIDTH="150">PLOT_NONE</TD><TD>No plot</td><td></TD></TR>
<TR><TD >PLOT_CURVE</TD><TD>Line plots</td><td>1D-1D</TD></TR>
<TR><TD>PLOT_CONTOUR</TD><TD>Contour plots</td><td>2D-1D</TD></TR>
<TR><TD>PLOT_IMAGE</TD><TD>Cell array plots</td><td>2D-1D</TD></TR>
<TR><TD>PLOT_WIRE_MESH</TD><TD>Wire frame mesh plots</td><td>2D-1D</TD></TR>
<TR><TD>PLOT_SURFACE</TD><TD>Shaded surface plots</td><td>2D-1D, 2D-2D</td></TR>
<TR><TD>PLOT_VECTOR</TD><td>Vector plots</td><td>2D-2D</td></TR>
<TR><TD>PLOT_FILL_POLY</TD><TD>Fill polygon plots</td><td>2D-1D</TD></TR>
<TR><TD>PLOT_MESH</TD><TD>Mesh plots</td><td>2D,3D</TD></TR>
<TR><TD>PLOT_ERROR_BAR</TD><TD>Error bar plots</td><td>1D-1D</TD></TR>
<TR><TD>PLOT_SCATTER</TD><TD>Scatter plots</td><td>1D-1D</TD></TR>
<TR><TD>PLOT_DV_BND</TD><TD>Region boundary plots</td><td>2D</TD></TR>
<TR><TD>PLOT_CARTESIAN</TD><TD>Cartesian line plots</td><td>1D-1D</TD></TR>
<TR><TD>PLOT_INSEL</TD><TD>Inselberg plots</td><td>1D-1D</TD></TR>
<TR><TD>PLOT_POLAR</TD><TD>Polar plots</td><td>1D-1D</TD></TR>
<TR><TD>PLOT_HISTOGRAM</TD><TD>Histogram plots</td><td>1D-1D</TD></TR>
<TR><TD>PLOT_LOGICAL</TD><TD>Data vs index plots</td><td>1D-1D</TD></TR>
<TR><TD>PLOT_DEFAULT</TD><TD></TD><TD></TD></TR>
</TABLE>
</BLOCKQUOTE>

<h4>Axis Description Constants</h4>

The following constants are defined in the PG_axis_attr enumeration:
<BLOCKQUOTE>
<TABLE>
<TR><TD WIDTH="150">AXIS_LINESTYLE</TD><TD>Line style</TD></TR>
<TR><TD>AXIS_LINETHICK</TD><TD>Line width</TD></TR>
<TR><TD>AXIS_LINECOLOR</TD><TD>Line color</TD></TR>
<TR><TD>AXIS_LABELCOLOR</TD><TD>Text color</TD></TR>
<TR><TD>AXIS_LABELSIZE</TD><TD>Text point size</TD></TR>
<TR><TD>AXIS_LABELFONT</TD><TD>Text font</TD></TR>
<TR><TD>AXIS_X_FORMAT</TD><TD>C style format for X axis labels</TD></TR>
<TR><TD>AXIS_Y_FORMAT</TD><TD>C style format for Y axis labels</TD></TR>
<TR><TD>AXIS_TICKSIZE</TD><TD>Tick size in NDC</TD></TR>
<TR><TD>AXIS_GRID_ON</TD><TD>Show grid</TD></TR>
<TR><TD>AXIS_CHAR_ANGLE</TD><TD>Direction along with labels printed</TD></TR>
<TR><TD>AXIS_TICK_MAJOR</TD><TD>Show major ticks</TD></TR>
<TR><TD>AXIS_TICK_MINOR</TD><TD>Show minor ticks</TD></TR>
<TR><TD>AXIS_TICK_LABEL</TD><TD>Show labels</TD></TR>
<TR><TD>AXIS_TICK_RIGHT</TD><TD>Ticks to right of axis</TD></TR>
<TR><TD>AXIS_TICK_LEFT</TD><TD>Ticks to left of axis</TD></TR>
<TR><TD>AXIS_TICK_STRADDLE</TD><TD>Ticks straddle axis</TD></TR>
<TR><TD>AXIS_TICK_ENDS</TD><TD>Ticks only at ends of axis</TD></TR>
<TR><TD>AXIS_TICK_NONE</TD><TD>No ticks</TD></TR>
</TABLE>
</BLOCKQUOTE>

The following constants are defined in the PG_axis_type enumeration:
<BLOCKQUOTE>
<TABLE>
<TR><TD>CARTESIAN_2D</TD><TD>2D Cartesian axes</TD></TR>
<TR><TD>CARTESIAN_3D</TD><TD>3D Cartesian axes</TD></TR>
<TR><TD>POLAR</TD><TD>Polar axes</TD></TR>
<TR><TD>INSEL</TD><TD>Inselberg axes</TD></TR>
</TABLE>
</BLOCKQUOTE>

<h4>Grid Description Constants</h4>

The following constants are defined in the PG_grid_attr enumeration:
<BLOCKQUOTE>
<TABLE>
<TR><TD WIDTH="150">GRID_LINESTYLE</TD><TD>Line style</TD></TR>
<TR><TD>GRID_LINETHICK</TD><TD>Line width</TD></TR>
<TR><TD>GRID_LINECOLOR</TD><TD>Line color</TD></TR>
</TABLE>
</BLOCKQUOTE>

<h4>Line Attributes</h4>

The following constants are defined in the PG_line_attr enumeration:
<BLOCKQUOTE>
<TABLE>
<TR><TD>LINE_SOLID</TD><TD>Solid line</TD></TR>
<TR><TD>LINE_DASHED</TD><TD>Dashed line</TD></TR>
<TR><TD>LINE_DOTTED</TD><TD>Dotted line</TD></TR>
<TR><TD>LINE_DOTDASHED</TD><TD>Dots and dashed alternating</TD></TR>
<TR><TD>LINE_NONE</TD><TD>No line</TD></TR>
</TABLE>
</BLOCKQUOTE>

<h4>Vector Description Constants</h4>

The following constants are defined in the PG_vector_attr enumeration:
<BLOCKQUOTE>
<TABLE>
<TR><TD>VEC_SCALE</TD><TD>Set scale of vectors</TD></TR>
<TR><TD>VEC_ANGLE</TD><TD>Set angle of arrow wings</TD></TR>
<TR><TD>VEC_HEADSIZE</TD><TD>Set head size</TD></TR>
<TR><TD>VEC_FIXSIZE</TD><TD>Use fixed head size</TD></TR>
<TR><TD>VEC_MAXSIZE</TD><TD>Set maximum head size</TD></TR>
<TR><TD>VEC_LINESTYLE</TD><TD>Set vector line style</TD></TR>
<TR><TD>VEC_LINETHICK</TD><TD>Set vector line width</TD></TR>
<TR><TD>VEC_COLOR</TD><TD>Set vector line color</TD></TR>
<TR><TD>VEC_FIXHEAD</TD><TD>Use a fixed head size</TD></TR>
</TABLE>
</BLOCKQUOTE>

<h4>Mouse and Keyboard Constants</hr>

The following constants are defined in the PG_mouse_button enumeration:
<BLOCKQUOTE>
<TABLE>
<TR><TD>MOUSE_LEFT</TD><TD>Left mouse button</TD></TR>
<TR><TD>MOUSE_MIDDLE</TD><TD>Right mouse button</TD></TR>
<TR><TD>MOUSE_RIGHT</TD><TD>Middle mouse button</TD></TR>
</TABLE>
</BLOCKQUOTE>

The following constants are defined in the PG_key_modifier enumeration:
<BLOCKQUOTE>
<TABLE>
<TR><TD>KEY_SHIFT</TD><TD>Shift key</TD></TR>
<TR><TD>KEY_CNTL</TD><TD>Control key</TD></TR>
<TR><TD>KEY_ALT</TD><TD>Alt key</TD></TR>
<TR><TD>KEY_LOCK</TD><TD>Caps Lock key</TD></TR>
</TABLE>
</BLOCKQUOTE>

<h4>Interface Object Constants</hr>

The following constants are defined in the PG_iob_tag enumeration:
<BLOCKQUOTE>
<TABLE>
<TR><TD>PG_IOB_NAME</TD><TD>Name member</TD></TR>
<TR><TD>PG_IOB_CLR</TD><TD>Color member</TD></TR>
<TR><TD>PG_IOB_FLG</TD><TD>Flag member</TD></TR>
<TR><TD>PG_IOB_DRW</TD><TD>Drawable member</TD></TR>
<TR><TD>PG_IOB_ACT</TD><TD>Active member</TD></TR>
<TR><TD>PG_IOB_SEL</TD><TD>Selectable member</TD></TR>
<TR><TD>PG_IOB_OBJ</TD><TD>Object member</TD></TR>
<TR><TD>PG_IOB_END</TD><TD>End member</TD></TR>
</TABLE>
</BLOCKQUOTE>

The following constants are defined in the PG_iob_state enumeration:
<BLOCKQUOTE>
<TABLE>
<TR><TD>PG_IsVis</TD><TD>Is visible flag</TD></TR>
<TR><TD>PG_IsSel</TD><TD>Is selectable flag</TD></TR>
<TR><TD>PG_IsAct_FLG</TD><TD>Is active flag</TD></TR>
</TABLE>
</BLOCKQUOTE>

<h4>Rendering Clear Mode Constants</hr>

The following constants are defined in the PG_clear_mode enumeration:
<BLOCKQUOTE>
<TABLE>
<TR><TD>CLEAR_SCREEN</TD><TD>Clear the entire field of the device</TD></TR>
<TR><TD>CLEAR_VIEWSPACE</TD><TD>Clear the viewspace of the device</TD></TR>
<TR><TD>CLEAR_FRAME</TD><TD>Clear the current frame of the device</TD></TR>
</TABLE>
</BLOCKQUOTE>

<h4>Device Orientation Constants</hr>

The following constants are defined in the PG_orientation enumeration:
<BLOCKQUOTE>
<TABLE>
<TR><TD>HORIZONTAL</TD><TD>Display palette horizontally along the bottom</TD></TR>
<TR><TD>VERTICAL</TD><TD>Display palette vertically along the right</TD></TR>
<TR><TD>PORTRAIT_MODE</TD><TD>Portrait mode (mainly for PS)</TD></TR>
<TR><TD>LANDSCAPE_MODE</TD><TD>Landscape mode (mainly for PS)</TD></TR>
</TABLE>
</BLOCKQUOTE>

The following constants are defined in the PG_quadrant enumeration:
<BLOCKQUOTE>
<TABLE>
<TR><TD>QUAD_ONE</TD><TD>First quadrant - origin at bottom left</TD></TR>
<TR><TD>QUAD_FOUR</TD><TD>Fourth quadrant - origin at top left</TD></TR>
</TABLE>
</BLOCKQUOTE>

<h4>Text Alignment Constants</hr>

The following constants are defined in the PM_direction enumeration:
<BLOCKQUOTE>
<TABLE>
<TR><TD>DIR_LEFT</TD><TD>Left justify text</TD></TR>
<TR><TD>DIR_RIGHT</TD><TD>Right justify text</TD></TR>
<TR><TD>DIR_CENTER</TD><TD>Center text</TD></TR>
</TABLE>
</BLOCKQUOTE>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGSgloss"></a>
<h1 ALIGN="CENTER"> Glossary</h1>

Here is a list of terms which are used in this manual.<p>


<BLOCKQUOTE>
<TABLE>
<TR><TD>device</TD>
<TD>A generic name for a graphical output device such as a display window, 
a PostScript file. PGS supports WINDOW (display screen), PS (PostScript), 
and CGM devices.</TD></TR>
<TR><TD>console</TD>
<TD>A special device which mimics a UNIX shell window on platforms 
which lack that functionality (e.g. Macintosh)</TD></TR>
<TR><TD>NDC</TD>
<TD>Normalized device coordinates or screen coordinates. Values range 
between 0.0 and 1.0.</TD></TR>
<TR><TD>image</TD>
<TD>An array of pixel values also called a raster image or cell array</TD></TR>
<TR><TD>graph</TD>
<TD>Generically the collection of information needed to visualize a collection
of data. More specifically a PG_graph structure.</TD></TR>
<TR><TD>set</TD>
<TD>Generically a collection of related data. More specifically a PM_set 
structure.</TD></TR>
<TR><TD>mapping</TD>
<TD>Generically a rule of association between elements of two sets. More 
specifically a PM_mapping structure.</TD></TR>
<TR><TD>palette</TD>
<TD>An application oriented representation of a color scheme to be associated
with data in a plot.</TD></TR>
<TR><TD>colormap</TD>
<TD>A host oriented representation of a color scheme to be associated with 
colors in hardware.</TD></TR>
<TR><TD>marker</TD>
<TD>A user definable &#147;character&#148;. A collection of line segments
which can be rotated and scaled collectively.</TD></TR>
<TR><TD>connectivity</TD>
<TD>The collection of neighbor relationships between points of a
computational mesh.</TD></TR>
</TABLE>
</BLOCKQUOTE>

<P>
<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGSexample"></a>
<h1 ALIGN="CENTER">PGS By Example</h1>

Perhaps the best way to learn to use PGS is by example. Certainly it is
the easiest way to explain certain aspects of it. In this section there
are examples of some of the low level graphics primitives and the high
level rendering functions. The actual tests are:<p>

<UL>
Text Placement and Drawing
<P>Line Drawing
<P>Making Line Plots
<P>Making Contour Plots
<P>Making Image Plots
<P>Making Wire Frame Mesh Plots
<P>Making Vector Plots
</UL>

<BR>

Before showing the examples we discuss some sequences of PGS calls that are
common to most PGS applications. The hope is that this will make clear why
some of PGS functions exist and how they relate to one another.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS1267"></a>
<h2> Common Call Sequences</h2>

The PGS functions are generally not very meaningful taken one at a time. 
What is important is the way they are used together to accomplish various
graphical objectives. We are not focusing on the details of the calls here
so many details are omitted.  Concentrate on which calls are being made and
when they are performed.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS2354"></a>
<h3> Initializing a Device</h3>

The first basic job in a graphics application is to setup the devices to
be used. Most common is setting up screen windows. Also important is
initializing hardcopy devices such as a PostScript device. The sequence
is all the same, some of the parameters differ. <p>

<PRE>

                 dev = PG_make_device(...);
             

                 PG_set_viewspace_pos(dev, ...);

                 PG_set_viewspace_shape(dev, ...);

                 PG_white_background(dev, ...);
             

                 PG_open_device(dev, ...)

</PRE>

The PG_make_device call only allocates a PG_device structure and sets
default values for the state it contains. The PG_open_device call
actually consults the state of the PG_device and opens the device.
The calls in between change the state of the PG_device from the defaults. <p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS2358"></a>
<h3> Making a Picture</h3>

The next basic idea is to put together a picture.  This may be done with
low level drawing operations or high level rendering calls or both.
What is common to all is that the picture must be set up, drawn,
and finished.<p>

<PRE>

                 PG_clear_window(dev);

                       ...

                      draw

                       ...

                 PG_update_vs(dev);

                       ...

                 PG_finish_plot(dev);

</PRE>

The call to PG_clear_window must be done for hardcopy devices! It is a
pretty good idea for screen window devices too.  Sometimes in the process
of drawing a picture you want to see the results so far with the idea that
more will be drawn later. PG_update_vs makes sure that everything the has
been requested is visible. When the picture is complete and there is nothing
more to be drawn to it, PG_finish_plot is called.  This is crucial for
hardcopy devices! It is also crucial that PG_clear_window and
PG_finish_plot be called once per picture.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS2434"></a>
<h3> Setting Attributes</h3>

To gain control of the appearance of high level plots, applications must
set rendering attributes.  See the discussion of rendering attributes
before going any further here. The sequence is to use PG_fget_render_info
to obtain the attribute list from the graph, use SC_change_alist to
change or add values to the list, and use PG_fset_render_info to update
the graph&#146;s attribute list.<p>


Here are C and FORTRAN examples of setting some attributes in a graph.
In this example some attributes are set for contour plotting.<p>

<CENTER><B>C</B></CENTER>

<BLOCKQUOTE>
<PRE>
PG_graph *g;
pcons *alst;
double *clev;
int *nlev;

nlev  = CMAKE(int);
*nlev = 6;
clev  = CMAKE_N(double, *nlev);

alst = PG_fget_render_info(g);
alst = SC_change_alist(alst, "LEVELS", "double *", clev);
alst = SC_change_alist(alst, "N-LEVELS", "int *", nlev);
PG_fset_render_info(g, alst);

</PRE>
</BLOCKQUOTE>

<CENTER><B>FORTRAN</B></CENTER>

<BLOCKQUOTE>
<PRE>
integer gid, ial, nlev
real clev(10)

call pgginf(gid, ial)
call scchal(ial, 6, 'LEVELS', 6, 'double', nlev, clev)
call scchal(ial, 8, 'N-LEVELS', 7, 'integer', 1, nlev)
call pgsinf(gid, ial)
</PRE>
</BLOCKQUOTE>

<P>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS1268"></a>
<h2> Text Placement and Drawing</h2>

The following program demonstrates some of the PGS functionality for placement and drawing of text.<p>

<PRE> 
     #include "pgs.h"

     /*-----------------------------------------------------------------*/

     void main(int argc, char **argv)
        {char s[MAXLINE], *token;
         PG_device *SCR_dev, *PS_dev, *CGM_dev;
         double x1, y1, x2, y2, dx, dy;
	 double ndc[PG_BOXSZ], bx[PG_BOXSZ];
         char *face, *style;
         int size;

     /* connect the I/O functions */

         PG_open_console("PGS Test", "COLOR", TRUE, 0.1, 0.7, 0.5, 0.3);
             
         SCR_dev = PG_make_device("WINDOW", "COLOR", "PGS Test");
         PG_open_device(SCR_dev, 0.1, 0.1, 0.5, 0.6);

	 ndc[0] = 0.1;
	 ndc[1] = 0.9;
	 ndc[2] = 0.2;
	 ndc[3] = 0.8;
	 PG_set_viewspace(SCR_dev, 2, NORMC, ndc);
         PG_set_viewspace(SCR, 2, WORLDC, NULL);

         PG_box_init(2, bx, -0.02, 1.02);
         PG_draw_box_n(SCR_dev, 2, WORLDC, bx);

         CGM_dev = PG_make_device("CGM", "MONOCHROME", "gstxts");
         PG_open_device(CGM_dev, 0.1, 0.1, 0.8, 0.8);

         PS_dev = PG_make_device("PS", "MONOCHROME", "gstxts");
         PG_open_device(PS_dev, 0.1, 0.1, 0.8, 1.1);

         PG_expose_device(PG_gs.console);

         PG_clear_window(SCR_dev);
         PG_clear_window(CGM_dev);
         PG_clear_window(PS_dev);

         test_dev(SCR_dev);
         test_dev(CGM_dev);
         test_dev(PS_dev);

         PG_finish_plot(SCR_dev);
         PG_finish_plot(CGM_dev);
         PG_finish_plot(PS_dev);

         SC_pause();

         PG_close_device(SCR_dev);
         PG_close_device(CGM_dev);
         PG_close_device(PS_dev);
             
         exit(0);}

     /*-----------------------------------------------------------------*/

     /* SF_DT - set the font and draw the text */

     static void sf_dt(PG_device *dev, double x1, double y1,
		       char *face, char *style, int size)
        {double bx[PG_BOXSZ], dx[PG_SPACEDM], p[PG_SPACEDM];

	 PG_fset_font(dev, face, style, size);

         PG_get_text_ext_n(dev, 2, WORLDC, "foo", dx);

	 bx[0] = x1;
	 bx[1] = x1 + dx[0];
	 bx[2] = y1;
	 bx[3] = y1 + dx[1];

/* write some text and draw a box around it */
         p[0] = bx[0];
         p[1] = bx[2];
         PG_write_n(dev, 2, WORLDC, p, "%s", "foo");

	 PG_draw_box_n(dev, 2, WORLDC, bx);

	 return;}

     /*-----------------------------------------------------------------*/

     /* TEST_DEV - test the entire device */

     static void test_dev(PG_device *dev)
        {PG_fset_line_color(dev, dev->BLACK, TRUE);
         PG_fset_text_color(dev, dev->BLACK, TRUE);

         sf_dt(dev, .1, .9, "helvetica", "medium", 12);
         sf_dt(dev, .1, .8, "helvetica", "italic", 12);
         sf_dt(dev, .1, .7, "helvetica", "bold", 12);
         sf_dt(dev, .1, .6, "helvetica", "bold-italic", 12);

         sf_dt(dev, .1, .50, "helvetica", "medium", 10);
         sf_dt(dev, .1, .45, "helvetica", "italic", 10);
         sf_dt(dev, .1, .40, "helvetica", "bold", 10);
         sf_dt(dev, .1, .35, "helvetica", "bold-italic", 10);

         sf_dt(dev, .1, .30, "helvetica", "medium", 8);
         sf_dt(dev, .1, .25, "helvetica", "italic", 8);
         sf_dt(dev, .1, .20, "helvetica", "bold", 8);
         sf_dt(dev, .1, .15, "helvetica", "bold-italic", 8);

         sf_dt(dev, .3, .9, "times", "medium", 12);
         sf_dt(dev, .3, .8, "times", "italic", 12);
         sf_dt(dev, .3, .7, "times", "bold", 12);
         sf_dt(dev, .3, .6, "times", "bold-italic", 12);

         sf_dt(dev, .3, .50, "times", "medium", 10);
         sf_dt(dev, .3, .45, "times", "italic", 10);
         sf_dt(dev, .3, .40, "times", "bold", 10);
         sf_dt(dev, .3, .35, "times", "bold-italic", 10);

         sf_dt(dev, .3, .30, "times", "medium", 8);
         sf_dt(dev, .3, .25, "times", "italic", 8);
         sf_dt(dev, .3, .20, "times", "bold", 8);
         sf_dt(dev, .3, .15, "times", "bold-italic", 8);

         sf_dt(dev, .5, .9, "courier", "medium", 12);
         sf_dt(dev, .5, .8, "courier", "italic", 12);
         sf_dt(dev, .5, .7, "courier", "bold", 12);
         sf_dt(dev, .5, .6, "courier", "bold-italic", 12);

         sf_dt(dev, .5, .50, "courier", "medium", 10);
         sf_dt(dev, .5, .45, "courier", "italic", 10);
         sf_dt(dev, .5, .40, "courier", "bold", 10);
         sf_dt(dev, .5, .35, "courier", "bold-italic", 10);

         sf_dt(dev, .5, .30, "courier", "medium", 8);
         sf_dt(dev, .5, .25, "courier", "italic", 8);
         sf_dt(dev, .5, .20, "courier", "bold", 8);
         sf_dt(dev, .5, .15, "courier", "bold-italic", 8);
             
         PG_update_vs(dev);

         return;}

     /*-----------------------------------------------------------------*/
</PRE>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS1409"></a>
<h2> Line Drawing</h2>

This program illustrates some of the PGS calls for handling drawing
attributes and for drawing lines.<p>

<blockquote>
<PRE> 
#include "pgs.h"

char
 *color[] = {"BLACK",
             "WHITE",
             "LGHT_WHITE"
             "GRAY",
             "BLUE",
             "GREEN",
             "CYAN",
             "RED",
             "MAGENTA",
             "BROWN",
             "LGHT_BLUE",
             "LGHT_GREEN",
             "LGHT_CYAN",
             "LGHT_RED",
             "YELLOW",
             "LGHT_MAGENTA"};

/*-----------------------------------------------------------------*/

main(int argc, char **argv)
   {int i, n;
    double y, dy;
    double x1[PG_SPACEDM], x2[PG_SPACEDM], p[PG_SPACEDM];
    PG_device *SCR_dev, *SCR_dew;

    SCR_dev = PG_make_device("WINDOW", "COLOR", "PGS Test A");
    SCR_dew = PG_make_device("WINDOW", "COLOR", "PGS Test B");

    PG_set_viewspace_pos(SCR_dev, 0.0001, 0.0001);
    PG_set_viewspace_shape(SCR_dev, 0.9999, 0.0, 1.0);

    PG_set_viewspace_pos(SCR_dew, 0.0001, 0.0001);
    PG_set_viewspace_shape(SCR_dew, 0.9999, 0.0, 1.0);

    PG_white_background(SCR_dev, TRUE);
    PG_white_background(SCR_dew, FALSE);

    PG_open_device(SCR_dev, 0.1, 0.1, 0.4, 0.4);
    PG_open_device(SCR_dew, 0.5, 0.1, 0.4, 0.4);

/* connect the I/O functions */

    PG_open_console("PGS Test", "MONOCHROME", TRUE, 0.1, 0.7, 0.5, 0.3);

    PG_set_viewspace(SCR_dev, 2, NORMC, NULL);
    PG_set_viewspace(SCR, 2, WORLDC, NULL);
    PG_set_viewspace(SCR_dew, 2, NORMC, NULL);
    PG_set_viewspace(SCR, 2, WORLDC, NULL);

    x1[0] = 0.0;
    x2[0] = 0.5;

    n  = 16;
    dy = 1.0/(n + 1.0);
    y  = 0.5*dy;

    for (i = 0; i < n; i++)
        {x1[1] = y;
	 x2[1] = y;

	 p[0] = 0.6;
	 p[1] = y;

	 PG_fset_line_color(SCR_dev, i, TRUE);
	 PG_draw_line_n(SCR_dev, 2, WORLDC, x1, x2, SCR_dev->clipping);
	 PG_fset_text_color(SCR_dev, i, TRUE);
         PG_write_n(SCR_dev, 2, WORLDC, p, "%d %s", i, color[i]);

	 PG_fset_line_color(SCR_dew, i, TRUE);
	 PG_draw_line_n(SCR_dew, 2, WORLDC, x1, x2, SCR_dew->clipping);
	 PG_fset_text_color(SCR_dew, i, TRUE);
         PG_write_n(SCR_dew, 2, WORLDC, p, "%d %s", i, color[i]);

	 y += dy;};

    PG_update_vs(SCR_dew);
    PG_update_vs(SCR_dev);

    SC_pause();

    PG_close_device(SCR_dew);
    PG_close_device(SCR_dev);

    exit(0);}

/*-----------------------------------------------------------------*/
</PRE>
</blockquote>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS1499"></a>
<h2> Line Plots</h2>

This program demonstrates some of the line plot capabilities of PGS.<p>

<blockquote>
<PRE> 
#include "pgs.h"

#define N 50

/*-----------------------------------------------------------------*/

main(argc, argv)
   int argc;
   char **argv;

   {int i;
    double *x, *y;
    char *s;
    PG_graph *data;
    PG_device *SCR_dev, *SCR_dew, *PS_dev;

    s = CSTRSAVE("WINDOW");

    for (i = 1; i < argc; i++)
        if (argv[i][0] == '-')
           {switch (argv[i][1])
               {case 'h' : i++;
                           s = argv[i];
                           break;};};

    x = CMAKE_N(double, N);
    y = CMAKE_N(double, N);

    for (i = 0; i < N; i++)
        {x[i] = -(i+1)/8.0;
         y[i] = 6.022e23/x[i];};

    data = PG_make_graph_1d('A', "Test Data #1", FALSE, N,
                            x, y, "X Values", "Y Values");

    PG_open_console("GSTEST", "MONOCHROME", 1,
                    0.05, 0.7, 0.9, 0.20);

/* set up the left window */

    SCR_dev = PG_make_device(s, "COLOR", "PGS Test A");
    PG_white_background(SCR_dev, TRUE);
    PG_turn_data_id(SCR_dev, ON);
    PG_open_device(SCR_dev, 0.05, 0.2, 0.45, 0.45);

/* set up the right window */

    SCR_dew = PG_make_device(s, "COLOR", "PGS Test B");
    PG_white_background(SCR_dew, FALSE);
    PG_turn_data_id(SCR_dew, ON);
    PG_open_device(SCR_dew, 0.5, 0.2, 0.45, 0.45);

/* set up the hard copy device */

    PS_dev = PG_make_device("PS", "MONOCHROME", "PGS Test");
    PG_turn_data_id(PS_dev, ON);
    PG_open_device(PS_dev, 0.0, 0.0, 0.0, 0.0);

    PG_set_plot_type(SCR_dev, PLOT_CARTESIAN, CARTESIAN_2D);
    PG_draw_graph(SCR_dev, data);
    PG_set_plot_type(PS_dev, PLOT_CARTESIAN, CARTESIAN_2D);
    PG_draw_graph(PS_dev, data);

    SC_pause();

    PG_set_plot_type(SCR_dew, PLOT_POLAR, POLAR);
    PG_draw_graph(SCR_dew, data);
    PG_set_plot_type(PS_dev, PLOT_POLAR, POLAR);
    PG_draw_graph(PS_dev, data);

    SC_pause();

    PG_set_plot_type(SCR_dev, PLOT_INSEL, INSEL);
    PG_draw_graph(SCR_dev, data);
    PG_set_plot_type(PS_dev, PLOT_INSEL, INSEL);
    PG_draw_graph(PS_dev, data);

    SC_pause();

    for (i = 0; i < N; i++)
        {x[i] = i/8.0;
         y[i] = cos(x[i]);};

    data = PG_make_graph_1d('B', "Test Data #2", FALSE, N,
                            x, y, "X Values", "Y Values");

    PG_turn_grid(SCR_dev, ON);
    PG_turn_grid(SCR_dew, ON);
    PG_turn_grid(PS_dev, ON);

    PG_set_plot_type(SCR_dew, POLAR, POLAR);
    PG_draw_graph(SCR_dew, data);
    PG_set_plot_type(PS_dev, POLAR, POLAR);
    PG_draw_graph(PS_dev, data);

    SC_pause();

    PG_set_plot_type(SCR_dev, INSEL, INSEL);
    PG_draw_graph(SCR_dev, data);
    PG_set_plot_type(PS_dev, INSEL, INSEL);
    PG_draw_graph(PS_dev, data);

    SC_pause();
             
    PG_set_plot_type(SCR_dew, PLOT_CARTESIAN, CARTESIAN_2D);
    PG_draw_graph(SCR_dew, data);
    PG_set_plot_type(PS_dev, PLOT_CARTESIAN, CARTESIAN_2D);
    PG_draw_graph(PS_dev, data);

    SC_pause();

    PG_close_device(SCR_dev);
    PG_close_device(SCR_dew);
    PG_close_device(PS_dev);

    exit(0);}

/*-----------------------------------------------------------------*/
</PRE>
</blockquote>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS1612"></a>
<h2> Contour Plots</h2>
<p>
<h3> C language example</h3>
This C program illustrates the PGS contour plotting functionality.<p>

<blockquote>
<PRE> 
/*
 * GSSCTST.C - C PGS contour plotting example
 */

#include "cpyright.h"
#include "pgs.h"

int main(int argc, char **argv)
   {int i, k, l, kmax, lmax, kxl;
    int id, lncolor, lnstyle, centering;
    double *x, *y, *f, r, t, lnwidth;
    double xmin, xmax, ymin, ymax;
    PG_device *SCR_dev;
    PG_graph *dataset;

/* set up the graphics window */

    SCR_dev = PG_make_device("WINDOW", "COLOR", "PGS Contour Test");
    PG_open_device(SCR_dev, 0.05, 0.2, 0.45, 0.45);

    PG_set_viewspace_pos(SCR_dev, 0.25, 0.15);
    PG_set_viewspace_shape(SCR_dev, 0.5, 0.0, 0.5/0.3333);

    PG_white_background(SCR_dev, TRUE);
    PG_fset_border_width(SCR_dev, 5);

/* set up data */

    kmax      = 20;
    lmax      = 20;
    xmin      = -5.0;
    xmax      = 5.0;
    ymin      = -5.0;
    ymax      = 5.0;
    kxl       = kmax*lmax;
    x         = CMAKE_N(double, kxl);
    y         = CMAKE_N(double, kxl);
    f         = CMAKE_N(double, kxl);
    id        = 'A';
    lncolor   = SCR_dev->BLUE;
    lnwidth   = 0.0;
    lnstyle   = LINE_SOLID;
    centering = N_CENT;

    for (k = 0; k < kmax; k++)
        for (l = 0; l < lmax; l++)
            {i = l*kmax + k;
             x[i] = k/10.0 - 5.0;
             y[i] = l/10.0 - 5.0;
             r = x[i]*x[i] + y[i]*y[i];
             t = 5.0*atan(y[i]/(x[i] + SMALL));
             r = pow(r, 0.125);
             f[i] = exp(-r)*(1.0 + 0.1*cos(t));};};

    dataset = PG_make_graph_r2_r1(id, "contour", FALSE,
                                  kmax, lmax, centering, x, y, f,
                                  "xy", "f");

    PG_turn_data_id(SCR_dev, 1);

    PG_contour_plot(SCR_dev, dataset);

    SC_pause();

    PG_close_device(SCR_dev);

    exit(0);}

/*-----------------------------------------------------------------*/
</PRE>
</blockquote>

<h3> Fortran language example</h3>
This Fortran program is roughly the equivalent of the above C example.<p>

<blockquote>
<PRE>
c

c GSTEST.F - Fortran PGS contour plotting example

c

c ---------------------------------------------------------------

      program gsfctst

      parameter(KMX = 20)
      parameter(LMX = 20)
      parameter(KSZ = KMX*LMX)

c ... PGS centering options

      parameter(Z_CENT = -1)
      parameter(N_CENT = -2)
      parameter(F_CENT = -3)
      parameter(E_CENT = -4)
      parameter(U_CENT = -5)

c ... functions

      integer pgmkdv, pgmg21
      integer idev, id, cp, igraph
      integer centering
      integer kmax, lmax, kxl

      double precision x1, y1, x2, y2
      double precision xmin, xmax, ymin, ymax, r, t
      double precision x(KSZ), y(KSZ), f(KSZ)
      
      character*8 names, namep
      character*20 name2, name3
      character*12 name4

c ... set up the window

      names = 'WINDOW'
      name2 = 'COLOR'
      name3 = 'PGS Contour Test'
      idev = pgmkdv(6, names, 5, name2, 16, name3)

      x1 = 0.05
      y1 = 0.2
      x2 = 0.45
      y2 = 0.45

c ... set use pixmap flag to false

      call pgsupm(0)

c ... create the window on the screen

      call pgopen(idev, x1, y1, x2, y2)

c ... clear the screen

      call pgclsc(idev)

c ... set up the view port

      x1 = 0.1
      x2 = 0.9
      y1 = 0.1
      y2 = 0.9

      call pgsvwp(idev, x1, x2, y1, y2)

      kmax = KMX
      lmax = LMX
      kxl  = KSZ
      xmin = -5.0
      xmax = 5.0
      ymin = -5.0
      ymax = 5.0
      id   = 1
      cp   = 0

      centering = N_CENT

c... generate some data

      do 1 l = 1, lmax
         do 1 k = 1, kmax
            i = (k-1)*lmax + l
            x(i) = l/10.0 - 5.0
            y(i) = k/10.0 - 5.0
            r = x(i)*x(i) + y(i)*y(i)
            t = 5.0*atan(y(i)/x(i))
            r = r**0.125
            f(i) = exp(-r)*(1.0 + 0.1*cos(t))
 1    continue

c... turn data ids on

      call pgtdid(idev, 1);

c... make a graph for PGS to plot

      igraph = pgmg21(id, 7, 'contour', cp, kmax, lmax, centering,
     $            x, y, f, 2, 'xy', 1, 'f')

c... plot the graph

      call pgplot(idev, igraph)

      pause

c ... close the device

      call pgclos(idev)

      call exit

      end

c --------------------------------------------------------------------------
</PRE>
</blockquote>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS1701"></a>
<h2> Image Plots</h2>

PGS has facilities for manipulating cell array data and rendering such data
sets. In PGS these are referred to as image plots. The following program
generates and plots some images.<p>

<blockquote>
<PRE> 
#include "pgs.h"

/*-----------------------------------------------------------------*/

int main(int c, char **v)
   {int k, l;
    int w, h;
    double dx, dy, x, y, sf;
    char *s;
    PG_palette *pl;
    PG_image *calc_im;
    PG_device *SCR_dev, *SCR_dew, *PS_dev;
    unsigned char *bf, *p;

    w = 250;
    h = 250;

    PG_box_init(2, dbx, 0.0, 0.0);
    PG_box_init(1, rbx, 0.0, 256.0);

    calc_im = PG_make_image_n("Test Image", G_CHAR_S,
			      2, WORLDC, dbx, rbx, w, h, 4, NULL);

    if (calc_im == NULL)
       {PRINT(stdout, "CAN'T ALLOCATE IMAGE");
        exit(1);};

    PG_open_console("GSIMTS", "MONOCHROME", 1,
                    0.05, 0.7, 0.9, 0.20);

    SCR_dev = PG_make_device("WINDOW", "COLOR", "PGS Image Test");
    PG_background_white(SCR_dev, FALSE;
    PG_open_device(SCR_dev, 0.05, 0.1, 0.4, 0.4);

    SCR_dew = PG_make_device(s, "COLOR", "PGS Image Test");
    PG_background_white(SCR_dew, TRUE);
    PG_open_device(SCR_dew, 0.55, 0.1, 0.4, 0.4);

    PS_dev  = PG_make_device("PS", "COLOR", "PGS Test");
    PG_open_device(PS_dev, 0.0, 0.0, 0.0, 0.0);

    PG_set_viewspace(SCR_dev, 2, NORMC, NULL);
    PG_set_viewspace(SCR, 2, WORLDC, NULL);

    PG_set_viewspace(SCR_dew, 2, NORMC, NULL);
    PG_set_viewspace(SCR, 2, WORLDC, NULL);

/* draw the first image */

    bf = calc_im->buffer;
    pl = PG_fset_palette(SCR_dev, "bw");
    sf = pl->n_pal_colors;

    for (l = 0; l < h; l++)
        {for (k = 0; k < w; k++)
             *bf++ = sf*((double) k)*((double) (h - l - 1))/
                           ((double) h*w);};};

    PG_draw_image(SCR_dev, calc_im, "Test Data A");

    PG_fset_palette(PS_dev, "bw");
    PG_draw_image(PS_dev, calc_im, "Test Data HC");

    SC_pause();

/* draw the second image */

    dx = 2.0*PI/((double) w);
    dy = 2.0*PI/((double) h);
    bf = calc_im->buffer;
    pl = PG_fset_palette(SCR_dew, "rainbow");
    sf = pl->n_pal_colors;

    for (l = 0; l < h; l++)
        {for (k = 0; k < w; k++)
             {x = ((double) k)*dx;
              y = ((double) (h - l - 1))*dy;
             *bf++ = sf*(0.5 + 0.5*sin(x)*cos(y));};};

    PG_clear_window(SCR_dew);
    PG_draw_image(SCR_dew, calc_im, "Test Data B");

    SC_pause();

    PG_close_device(SCR_dev);
    PG_close_device(SCR_dew);
    PG_close_device(PS_dev);

    PG_rl_image(calc_im);

    exit(0);}

/*-----------------------------------------------------------------*/
</PRE>
</blockquote>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS1793"></a>
<h2> Surface Plots</h2>

PGS also has the ability to render data sets as wire-frame meshes with
hidden line removal. The following program shows how this is done.<p>

<blockquote>
<PRE> 
#include "pgs.h"

#define N_POINTS 10

static void
 draw_set(PG_device *SCR_dev, PG_device *PS_dev,
          PG_device *CGM_dev, double *rz, double *rs, double *ext,
          double *rx, double *ry, int n_pts,
          double xmn, double xmx, double ymn, double ymx,
          double theta, double phi, double width,
          int color, int style, int type, int *maxes),
 print_help(void);

/*-----------------------------------------------------------------*/

main(argc, argv)
   int argc;
   char **argv;

   {int i, k, l;
    int maxes[2], w, h, n_pts, use_color, type;
    double x, y, dx, dy, theta, phi, dp;
    char *s, t[MAXLINE], *token;
    PG_device *SCR_dev, *PS_dev, *CGM_dev;
    double *rx, *ry, *rz, *rs, *px, *py, *pz, *ps, *ext;

    w     = N_POINTS;
    h     = N_POINTS;
    use_color = FALSE;
    type      = PLOT_WIRE_MESH;
    dp        = 0.0;

    for (i = 1; i < argc; i++)
        {if (argv[i][0] == '-')
            {switch (argv[i][1])
                {case 'c' : use_color = TRUE;
                            break;
                 case 'p' : dp = ATOF(argv[++i]);
                            break;
                 case 's' : type = PLOT_SURFACE;
                            break;
                 case 'w' : type = PLOT_WIRE_MESH;
                            break;
                 case 'z' : w = h = atoi(argv[++i]);
                            break;};}

         else
            break;};

    maxes[0] = w;
    maxes[1] = h;
    n_pts    = w*h;

    PG_open_console("GSSFTS", "MONOCHROME", 1,
                    0.55, 0.1, 0.4, 0.8);

    if (use_color)
       SCR_dev = PG_make_device("WINDOW", "COLOR", "PGS Surface Test");
    else
       SCR_dev = PG_make_device("WINDOW", "MONOCHROME",
                                "PGS Surface Test");

    PG_open_device(SCR_dev, 0.1, 0.1, 0.4, 0.4);

    if (type == PLOT_SURFACE)
       PG_fset_palette(SCR_dev, "spectrum");

    rx = px = CMAKE_N(double, n_pts);
    ry = py = CMAKE_N(double, n_pts);
    rz = pz = CMAKE_N(double, n_pts);

    if (type == PLOT_SURFACE)
       rs = ps = CMAKE_N(double, n_pts);
    else
       rs = ps = pz;

    ext = CMAKE_N(double, 4);

    ext[0] = -1;
    ext[1] =  1;
    ext[2] = -1;
    ext[3] =  1;

/* draw the first image */

    dx = 2.0*PI/((double) (w - 1));
    dy = 2.0*PI/((double) (h - 1));

    for (l = 0; l < h; l++)
        {for (k = 0; k < w; k++)
             {*px++ = x = ((double) k)*dx;
              *py++ = y = ((double) l)*dy;
              *pz++ = cos(y);
              if (type == PLOT_SURFACE)
                 *ps++ = sin(x);};};

    if (dp == 0.0)
       {while (TRUE)
           {PRINT(stdout, "Viewing Angle: ");

            GETLN(t, MAXLINE, stdin);

            if ((token = strtok(t, " ,")) == NULL)
               break;

            theta = DEG_RAD*ATOF(token);

            if ((token = strtok(NULL, " ,")) == NULL)
               break;

            phi = DEG_RAD*ATOF(token);

            draw_set(SCR_dev, PS_dev, CGM_dev,
                     rz, rs, ext, rx, ry,
                     n_pts,
                     0.0, 2.0*PI, 0.0, 2.0*PI,
                     theta, phi,
                     0.0, SCR_dev->BLUE, LINE_SOLID,
                     type, maxes);};}

    else

       {for (phi = 0.0; phi <= 90.0; phi += dp)
            {draw_set(SCR_dev, PS_dev, CGM_dev,
                      rz, rs, ext, rx, ry,
                      n_pts,
                      0.0, 2.0*PI, 0.0, 2.0*PI,
                      0.0, DEG_RAD*phi,
                      0.0, SCR_dev->BLUE, LINE_SOLID,
                      type, maxes);};};

    PG_close_device(SCR_dev);

    exit(0);}

/*-----------------------------------------------------------------*/


/* DRAW_SET - draw a complete set of plots */

static void draw_set(PG_device *SCR_dev, PG_device *PS_dev,
                     PG_device *CGM_dev, double *rz, double *rs,
                     double *ext, double **r, int n_pts,
                     double *va, double width, int color,
                     int style, int type, int *maxes)
   {

    PG_draw_surface_n(SCR_dev, rz, rs, ext, r,
                      n_pts, va, width, color, style, type, "Test Data",
                      "Logical-Rectangular", maxes);

    PG_draw_surface_n(PS_dev, rz, rs, ext, r,
                      n_pts, va, width, color, style, type, "Test Data",
                      "Logical-Rectangular", maxes);

    PG_draw_surface_n(CGM_dev, rz, rs, ext, r,
                      n_pts, va, width, color, style, type, "Test Data",
                      "Logical-Rectangular", maxes);

    return;}

/*-----------------------------------------------------------------*/
</PRE>
</blockquote>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS1946"></a>
<h2> Vector Plots</h2>

PGS can render 2 dimensional vector fields. The following program demonstrates
this capability.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGS1948"></a>
<h2> FORTRAN API Example</h2>

<blockquote>
<PRE>
c
c GSTEST.F - test of PGS FORTRAN API
c

c -------------------------------------------------------------------

      program gsftst

      integer pgmkdv
      integer idev, idvp
      double precision x1, y1, x2, y2
      double precision x(5), y(5), u(4), v(4)
      character*8 names, namep
      character*10 name2, name3
      character*12 name4

c ... set up the window

      names = 'WINDOW'
      name2 = 'COLOR'
      name3 = 'PGS Test A'

      idev = pgmkdv(6, names, 5, name2, 10, name3)

      x1 = 0.05
      y1 = 0.2
      x2 = 0.45
      y2 = 0.45

      call pgopen(idev, x1, y1, x2, y2)

c ... set up the PS device

      namep = 'PS'
      name2 = 'MONOCHROME'
      name3 = 'gsftst'

      idvp = pgmkdv(2, namep, 10, name2, 6, name3)

      x1 = 0.0
      y1 = 0.0
      x2 = 0.0
      y2 = 0.0

      call pgopen(idvp, x1, y1, x2, y2)

      call pgclsc(idev)
      call pgclsc(idvp)

c ... set up the view port and world coordinate system

      x1 = 0.1
      x2 = 0.9
      y1 = 0.1
      y2 = 0.9

      call pgsvwp(idev, x1, x2, y1, y2)
      call pgsvwp(idvp, x1, x2, y1, y2)

      x1 =   0.0
      x2 =  10.0
      y1 = -15.0
      y2 =  30.0

      call pgswcs(idev, x1, x2, y1, y2)
      call pgswcs(idvp, x1, x2, y1, y2)

c ... draw a bounding box

      x1 =   0.0
      x2 =  10.0
      y1 = -15.0
      y2 =  30.0

      call pgdrbx(idev, x1, x2, y1, y2)
      call pgdrbx(idvp, x1, x2, y1, y2)

c ... write a string

      x1 = 5.0
      y1 = 0.0

      name4 = 'TEXT STRING'

 100  format(1p, e10.2)

      call pgwrta(idev, x1, y1, 11, name4)
      call pgwrta(idvp, x1, y1, 11, name4)

c ... draw a line

      x1 =  1.0
      x2 =  9.0
      y1 = -4.0
      y2 = -1.0

      call pgdrln(idev, x1, y1, x2, y2)
      call pgdrln(idvp, x1, y1, x2, y2)

c ... do a vector plot

      x(1) =  3.0
      y(1) =  4.0
      u(1) = -0.5
      v(1) = -0.25

      x(2) =  4.0
      y(2) =  4.0
      u(2) =  0.5
      v(2) = -0.25

      x(3) = 4.0
      y(3) = 5.0
      u(3) = 0.5
      v(3) = 0.0

      x(4) =  3.0
      y(4) =  5.0
      u(4) = -0.5
      v(4) =  0.5

      call pgplvc(idev, x, y, u, v, 4)
      call pgplvc(idvp, x, y, u, v, 4)

c ... draw and fill a polygon (color 4 is blue)

      x(1) = 5.0
      x(2) = 6.0
      x(3) = 6.0
      x(4) = 5.0
      x(5) = 5.0
      y(1) = 8.0
      y(2) = 8.0
      y(3) = 9.0
      y(4) = 9.0
      y(5) = 8.0

      call pgfply(idev, x, y, 5, 4)
      call pgfply(idvp, x, y, 5, 4)

      call pgfnpl(idev)
      call pgfnpl(idvp)

      pause
             
c ... close the device

      call pgclos(idev)
      call pgclos(idvp)

      call exit

      end

c ------------------------------------------------------------------
</PRE>
</blockquote>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 

<a name="PGSdocs"></a>
<h1 ALIGN="CENTER"> Other PACT Documentation</h1>

PGS depends on the SCORE and PML PACT libraries for certain key supporting
functionalities. In turn PGS structures are used in ULTRA II, SX, and PANACEA.
Some readers may find it helpful to refer to these and other PACT documents.<p>

The list of PACT Documents is:<p>

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
<p>

<br>
<hr>
For questions and comments, 
please contact the <A HREF="pactteam.html">PACT Development Team</A>.

