TXT: PACT FAQ
MOD: 02/03/2010

<BLOCKQUOTE>

<center>
<H1><font color="#ff0000">$TXT</font><h1>
</center>

<hr>

<H2>Welcome to the FAQ for PACT!</H2>

PACT is the environment and toolkit that we use to create portable 
applications, and to create functionality not readily available. 
PACT is our environment of choice for handling unique data,
portable computing environments, and the distributed computing 
arenas; and for converting data for the visualization application 
or algorithm of choice. We have created PACT to shield ourselves 
from new software, and hardware so we can concentrate on the 
applications at hand. PACT is beginning to emphasize design that 
takes advantage of massively parallel environments.
<P>
We would like to hear of your experiences with PACT. And, we would 
be especially pleased to try to add your mathematical, graphical, 
or other functionality to future distributions of PACT. 
<p>
<B>Absolute portability is our GOAL!</B> 
<P>
Most of the time we install PACT 
without problems, the other times take varying amounts of effort to 
track down. Usually the problems are simple, such as missing 
headers. If your experience with PACT is not absolute portability, 
we would like to hear from you so we can try to make it so. Please 
direct all portability issues to:

<center>
<a href=mailto:sabrown@llnl.gov>sabrown@llnl.gov</a> -or- <a href=mailto:minner1@llnl.gov>minner1@llnl.gov</a>
</center>

<hr>
<A NAME="F0"></a>
<H2>Frequently Asked Questions</H2>

<UL>
<L0>1) <A HREF="#F1">What is PACT?</A>
<BR>2) <A HREF="#F2">What machines does PACT run on?</A>
<BR>3) <A HREF="#F3">Where can I get PACT?</A>
<BR>4) <A HREF="#F4">How can I get in touch with PACT developers and users?</A>
<BR>5) <A HREF="#F5">What are the PACT packages?</A>
</UL>

<A NAME="F1"></a>
<H3>1) What is PACT?</H3>

PACT stands for Portable Application Code Toolkit. It is a
self-contained set of tools which support developing and
using portable programs particularly scientific and engineering
applications.  It attacks several portability problem areas:
<UL>
Binary Data
<BR>Graphics
<BR>Interprocess Communication
</UL>
PACT is composed of several (currently nine) specific packages
each of which address a specific functionality.
<br><BR><SMALL><A HREF="#F0">index</A></SMALL>

<A NAME="F2"></a>
<H3>2) What machines does PACT run on?</H3>

PACT has been tested on the following systems:
<UL>
AIX 2 and up
<BR>HP-UX
<BR>Irix 4.x
<BR>Irix 5.x
<BR>Linux 1.x
<BR>MAC OS X
<BR>OSF 1.x
<BR>SunOS 4.x
<BR>SunOS 5.x (Solaris)
<BR>Ultrix 
<BR>Unicos 7 and up
</UL>
<br><BR><SMALL><A HREF="#F0">index</A></SMALL>

<A NAME="F3"></a>
<H3>3) Where can I get PACT?</H3>

<UL>
http://pact.llnl.gov
</UL>

pact-unix.README explains how to install PACT and pactyy_mm_dd-src
is the self configuring, self installing distribution file.
<br><BR><SMALL><A HREF="#F0">index</A></SMALL>

<A NAME="F4"></a>
<H3>4) How can I get in touch with PACT developers and users?</H3>


The PACT team has setup a Majordomo based mailing list for users
of PACT.  You can subscribe to it by sending the following message
to Majordomo@lists.llnl.gov
<UL>
subscribe pact-users
</UL>
You can contact the PACT developers by sending to
<UL>
pactdev@llnl.gov
</UL>
<SMALL><A HREF="#F0">index</A></SMALL>

<A NAME="F5"></a>
<H3>5) What are the PACT packages?</H3>

<UL>
<TABLE>
<TR>
<TD><B>SCORE</B></TD>
<TD>hash functions, string handling, environment leveling</TD>
</TR>
<TR>
<TD><B>PML</B></TD>
<TD>math routines, geometry, linear solvers, etc</TD>
</TR>
<TR>
<TD><B>PDB</B></TD>
<TD>portable binary data handling</TD>
</TR>
<TR>
<TD><B>PPC</B></TD>
<TD>portable IPC, RPC, message passing</TD>
</TR>
<TR>
<TD><B>PGS</B></TD>
<TD>portable graphics and GUI tools</TD>
</TR>
<TR>
<TD><B>PANACEA</B></TD>
<TD>simulation code services</TD>
</TR>
<TR>
<TD><B>SCHEME</B></TD>
<TD>a R3RS level SCHEME interpreter library</TD>
</TR>
<TR>
<TD><B>SX</B></TD>
<TD>a SCHEME with extensions/bindings for all of PACT</TD>
</TR>
<TR>
<TD><B>ULTRA</B></TD>
<TD>1d data presentation, manipulation, analysis code</TD>
</TR>
</TABLE>
</UL>
PACT is held together by a system of C shell scripts called the
manager.  The PACT manager can also be used by other code systems
to assist with source maintenance and control.
<br><br><SMALL><A HREF="#F0">index</A></SMALL>

<hr>
For questions and comments, 
please contact the <A HREF="pactteam.html">PACT
Development Team</A>.

<hr>
Last Updated: $MOD
