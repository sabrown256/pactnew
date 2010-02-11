TXT: PACT: Latest Changes
MOD: 02/03/2010

<center>
<H1><font color="#ff0000">$TXT</font></H1>
</center>

<HR>

<P>
<H3>PACT Release Announcement </H3>
<P> 

<!------------------------------------------------------------------------>
<!------------------------------------------------------------------------>

Release 2_3_10 of PACT

<P>
<PRE>

Wed Feb 3 07:53:22 PST 2010

The changes consist of:

* General
   Two major source reorganizations:

   1) Split the headers into two.  One part contains the external
      interfaces used by applications.  The other contains internal
      interfaces used by PACT itself.  For example:  pdb.h was
      split into pdb.h which contains the external API and pdb_int.h
      which contains the internal API.  The "internal" headers
      are all named xxx_int.h and the "external" headers retain the
      original name.

      The "external" API functions are to be documented and the
      "internal" ones are not.  The "external" API contains no
      identifiers beginning with _XX_ only ones with XX_ where
      XX is one of the PACT package designations, e.g. PD for
      PDBLib.  The intention is to clarify which declarations
      belong to the documented, stable user API and which belong
      to the declarations that can evolve as needed.  The latter
      may be used by applications with the understanding that
      they can change without notice.

   2) Related groups of declarations have been grouped into
      there own header.  Conventionally, the headers are named
      scope_xxx.h.  Current "scope" are:

         score/scope_array.h  
         score/scope_hash.h   
         score/scope_io.h     
         score/scope_lex.h	  
         score/scope_make.h   
         score/scope_mem.h	  
         score/scope_mmap.h   
         score/scope_mpi.h	
         score/scope_proc.h	
         score/scope_term.h
         score/scope_thread.h
         score/scope_typeh.h

         pml/scope_complex.h  
         pml/scope_math.h     

         pgs/scope_cgm.h      
         pgs/scope_mpeg.h     
         pgs/scope_quickdraw.h
         pgs/scope_raster.h   
         pgs/scope_x11.h      

     This only helps to find and identify groups of related
     declarations.  Most of these are automatically included
     by the "external" headers as needed.

   Several buffer overflow errors have been identified and fixed.

   PACT can now build with Bison as well as Yacc.

* SCORE

  The SC_array structure was added to replace all prior incarnations
  of the SC_dynamic_array.  This provides a superior smart array
  capability in PACT.

  The HASHTAB struct and functions have been deprecated in favor
  of the new hasharr struct and functions.  The hasharr combines
  a hash table and a smart array in one struct to provide access
  to elements by index or by name.

  A major error in thread locking causing some long standing
  memory manager errors in threaded applications was fixed.

* PDB

  PDB was reorganized to handle translation spokes formerly
  available only via PDBView.  New access spokes were provided
  so that PDBLib can access (via PD_open, PD_read, ...):

     SQLite databases
     mySQL databases
     some XML files
     more HDF5 files

  PDBLib can also now read and write a plain text representation
  of any data.

* SCHEME
  Add missing min and max to Scheme.

* PDBView
  Redo PDBView copy commands into cp ala UNIX.

* ULTRA
  Added "fill" command to fill area under curves or interior
  of closed curves.

  By loading mouse.scm ULTRA can zoom and pan under mouse
  control in addition to the use of RAN and DOM commands.

* Python
  The PACT python bindings are fully integrated.  They
  are build and installed automatically when PACT is
  configured with the same compiler that was used to
  build the python version being supported and when
  the python numeric package is present.

</PRE>
<hr>

<!------------------------------------------------------------------------>
<!------------------------------------------------------------------------>

Release 7_18_07 of PACT

<P>
<PRE>

Wed Jul 18 14:12:01 PDT 2007

The changes consist of:


* General
   Use SC_strcat in place of strcat and SC_strncpy in place of
   strncpy for protection against buffer overruns.

* PSH
   Add routines to provide shell-like functionality and
   convert do-net and atdbg to simple C programs.

* SCRIPTS
   Added new shell scripts for extra functionality.

* SCORE
   Improvements and increase POSIX compliance of PACT dmake
   utility.
   Fix SCORE memory manager bug which caused erroneous memory
   corruption report.

* PGS
   Fixed floating point sensitivity in some algorithms.
   Fixed memory leaks.
   Fixed bugs with interface object handling.

* SCHEME
   Add missing min and max to Scheme.
   Add strict-c flag to SCHEME and handle xor/pow strictly or not.

* PDBView
   Fix define-file-variable for arrays.
   Fix conversion bug effecting certain classes of floating
   point conversions.
   Fix handling of special pointer case in comparisions.
   Fix interpolation errors binary mapping handler.

* ULTRA
   Add '^' operator syntax for pow in ULTRA and handle the
   overload on the C parser
   Fix problem in BC handler when selecting points.

</PRE>
<hr>

<!------------------------------------------------------------------------>
<!------------------------------------------------------------------------>

Release 2_13_04 of PACT

<P> 

<PRE>

Fri Feb 13 08:51:03 PST 2004


The changes consist of:

* Fix PDB bug relating to directories

* Add syntax error checking to SCHEME (affects ULTRA and PDBView)

* Extended C syntax mode in ULTRA and PDBView.  The interpreter
  in ULTRA and PDBView now interprets C code including most
  preprocessing directives.

* Fix bug in init of hashed charts so that casts always work properly

* Improvements to the autotools build system

* Make scheme continuation stack grow as necessary to eliminate
recursion limit

* Fix desc command bug in pdbview

* Fix deadlock issue in use of MPI-IO in parallel PDB 

* Add ability to insert checksums into and verify checksums in PDB files

* New configure/make build system also available for building PACT

* Add ability to zero out denormalized floating point values

* HDF5 input plugin for pdbview and pdbdiff uses libhdf5

* Fix portview dimension changing bug in pdbdiff

* Additional cleaner interface for score hash tables added, as well as a
hash iterator interface

* Fix bug that causes ultra to hang in select during print statements

* Documentation additions to reflect the above changes

* Several miscellaneous minor bug fixes

* Other minor bug fixes and cosmetic changes

</PRE>

<hr>
For questions and comments, please contact the
<A HREF="pactteam.html">PACT Development Team</A>.

<hr>
Last Updated: $MOD
