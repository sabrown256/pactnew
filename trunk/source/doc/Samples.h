TXT: PACT: Application Samples
MOD: 05/19/2011

<blockquote>

<H1 ALIGN=CENTER><font color="#ff0000">$TXT</font></H1>

<hr>

The following applications are a part of
the PACT sources.  They form our test suite, but they also illustrate
a good deal of the PACT functionality.  The first part is the name of
an executable file; followed by a brief description of the test
including which API it focuses on; and finally how to build it or,
in the case of the interpreters, which files to look at.

<P>

<B>SCORE</B>
<UL>
<li>scatst
; Hash table and association list test (C API)
; pact ha</li>
<li>schtst
; Hash table and association list test (Fortran API)
; pact fha</li>
<li>tscctl, tscctla
; Test some of the string handling functions (C API)
; pact compre
</li>
</UL>

<P>

<B>PML</B>
<UL>
<li>
mlmtrt
; Tests of the matrix handling routines (C API)
; pact matrix
</li>
<li>
mlacts
; Tests aspects of mesh topology structures (C API)
; pact lrac
</li>
<li>
mlstst
; Tests topological sort routine (C API)
; pact topsort
</li>
<li>
mlicts, mlicst
; Test ICCG solvers (C API)
; pact icsolv
</li>
</UL>

<P>

<B>PDB</B>
<UL>
<li>
pdbtst
; Main PDB test excersizing most of PDB (C API)
; pact Cstd
</li>
<li>
pdatst
; Test PDB attribute handling (C API)
; pact Cstd
</li>
<li>
pdftst
; Main PDB test excersizing most of PDB (Fortran API)
; pact Fstd
</li>
<li>
pdtime
; PDB timing test (C API)
; pact time
</li>
</UL>

<P>

<B>PPC</B>
<UL>
<li>
pcbtst
; Test exchange of binary data with process (C API)
; pact Cstd
</li>
<li>
pcmtst
; Multiple child test (C API)
; pact Cstd
</li>
<li>
pcexec
; Utility to execute child via pipes, sockets, or PTY (C API)
; pact Cstd
</li>
</UL>

<P>

<B>PGS</B>
<UL>
<li>
gsevts
; Interactive event handling test (C API)
; pact event
</li>
<li>
gsftst
; Fortran API test
; pact Fstd
</li>
<li>
gsclts
; Line drawing test (C API)
; pact line
</li>
<li>
gsmlts
; Poly segment test (C API)
; pact polyseg
</li>
<li>
gstxts
; Text handling test (C API)
; pact text
</li>
<li>
gsltst
; Curve drawing test (C API)
; pact curve
</li>
<li>
gssfts
; Shaded surface and wire frame rendering test (C API)
; pact surface
</li>
<li>
gsfpts
; Fill polygon rendering test (C API)
; pact fillpoly
</li>
<li>
gsctst
; Contour plot test (C API)
; pact contour
</li>
<li>
gsvtst
; Vector plot test (C API)
; pact vector
</li>
<li>
gsimts
; Image plot test (C API)
; pact image
</li>
<li>
gsshcm
; Utility to show colormaps in a variety of ways (C API)
; pact colormap
</li>
<li>
gspsts
; PostScript device test (C API)
; pact ps
</li>
<li>
gscgmt
; CGM device test (C API)
; pact cgm
gslaxt
; Log axis test (C API)
; pact logaxis
</li>
</UL>

<P>

<B>PANACEA</B>
<UL>
<li>
pathts
; Time history tests (C API)
; pact Cth
</li>
</UL>

<P>

<B>SCHEME</B>
<UL>
<li>
shtest
; C shell script to run SCHEME interpreter tests
; (see mmtst.scm, shproc.scm)
</li>
</UL>

<P>

<B>SX</B>
<UL>
<li>
sxtest
; C shell script to run SX interpreter tests
; <BR>(see sxpdb.scm, sxpdba.scm, sxhash.scm, and sxpgs.scm)
</li>
</UL>

<P>

<B>ULTRA</B>
<UL>
<li>
ultest
; C shell script to run ULTRA tests
; (see ulfft.scm, ulmmt.scm, and ultest.scm)
</li>
</UL>

<hr>
For questions and comments, 
please contact the <A HREF="pactteam.html">PACT
Development Team</A>.

<hr>
Last Updated: $MOD
