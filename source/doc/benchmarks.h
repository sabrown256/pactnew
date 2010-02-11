TXT: PDB and HDF5 Compared
MOD: 12/19/2005

<blockquote>

<center>
<p>
<h1><font color="#ff0000">PACT Benchmarks</font></h1>
<h2><font>$TXT</font></h2>
<h3>Gregory J. Smethells</h3>
<h5><a href="Pact-Cpyright.html">PACT Copyright</a></h5>
<p>
</center>

<hr>

<p>
<ul>
<li><a href="#bench_intro">Introduction</a></li>
<li><a href="#bench_features">Feature Comparison</a>
  <ul>
  <li><a href="#bench_features-utilities">Utilities</a></li>
  <li><a href="#bench_features-datatypes">Data types</a></li>
  <li><a href="#bench_features-dataobj">Data spaces and Data sets</a></li>
  <li><a href="#bench_features-dataconv">Data Conversion and Targeting</a></li>
  <li><a href="#bench_features-fileorg">File Organization</a></li>
  </ul>
</li>
<li><a href="#bench_api">Code Comparisons</a></li>
<li><a href="#bench_results">Benchmark Results</a>
  <ul>
  <li><a href="#bench_results-setup">System Setup</a></li>
  <li><a href="#bench_results-native">Native Performance</a></li>
  <li><a href="#bench_results-indir">Native Performance with Indirections</a></li>
  <li><a href="#bench_results-cross">Cross-platform Performance</a></li>
  </ul>
</li>
<li><a href="#bench_conclusions">Conclusions</a></li>
<li><a href="#bench_ref">References</a></li>
</ul>

<hr>
<p>


<!-- ======================================================= -->

<a name="bench_intro"><h2>1 Introduction</h2></a>

<p>
Both the portable binary database (PDB) and the hierarchical data 
format revision 5 (HDF5) libraries offer an interface to binary 
data storage and retrieval in an architecture independent manner. That is,
enough meta-data is stored within the files to completely describe the format 
of the raw, user data such that the library can properly read back that 
data on any other platform. This allows these file formats to be portable and 
circumvents such hazards as endianess and machine-dependent data-type format
differences. Other features include, associating names with the user 
data stored within PDB and HDF5 files so that the data can be easily referenced, 
the ability to access some subpart of any data set, being able to associate 
attributes with the data, and mechanisms to group data sets into directory type 
structures to provide organization.  Both libraries provide a variety of APIs. 
The PDB library can be called from C, C++, and FORTRAN and has bindings in Scheme
and Python. The HDF5 library has C, C++, Java, and FORTRAN APIs and bindings in
Python, Scheme, and LabVIEW. Each library provides similar data and meta-data 
storage capabilities.
<p>

<p>
With respect to the user-level API, we argue that the PDB interface is simpler 
and has a gentler learning curve than the HDF5 APIs. More lines of code are 
required to setup and perform an operation such as a restart dump using HDF5 
than are required to do the same operation using PDBLib. In terms of performance 
characteristics, our benchmarks show that the PDB and HDF5 libraries provide 
comparable native performance; however, when data conversions are required, HDF5 
performs slightly better than PDB for similar operations. Lastly, PDB provides 
support for managing indirections (pointers) within data structures while HDF5 
currently does not.
<p>

<!-- ======================================================= -->

<a name="bench_features"><h2>2 Feature Comparison</h2></a>

<a name="bench_features-utilities"><h3>2.1 Utilities</h3></a>

<p>
Each file format has utilities that support basic and specialized operations on
their files. PDB has pdbview, pdbdiff, and ultra. A UNIX like diff tool, pdbdiff does
an intelligent difference between two files and is capable of handling any file format
that it has an input spoke for including PDB and HDF5. No HDF5 tool is currently available
to do a diff on HDF5 files, though one is under development.  Ultra can be used for 
analysis and display of one-dimensional data sets and pdbview has capabilities to view, 
plot, analyze, and work with data in PDB and any other file format for which an input 
spoke is present. HDF5 is packaged with h5ls, h5repart, h5dump, among others, and has 
other specialized <a href="http://hdf.ncsa.uiuc.edu/tools5.html">tools</a> available.
H5ls lists information about file objects in an HDF5 file. H5repart can repartition an
HDF5 file or family of files. H5dump provides a means to examine and dump a HDF5 file's 
contents.
<p>

<a name="bench_features-datatypes"><h3>2.2 Data types</h3></a>

<p>
PDB and HDF5 both handle essentially the same set of data types. 
Atomic types (such as <i>int</i> and <i>double</i>) are managed through the use 
of meta-data to describe basic memory layout characteristics including the byte 
size of different primitive types, endianess, alignment, and floating point 
format standards so that data written on one machine can always be read on another. 
In addition, both file formats provide ways to manage compound data structures 
to handle more complicated data types by building upon primitive types or any 
previously defined compound data types. Lastly, arrays of fairly arbitrary 
dimensionality for any data type can be created and handled by both libraries. 
Taken as a whole, these provide a basic level of data structure capability and 
pass responsibility of any more complex, higher-level data structure management 
to the user-level code.
<p>

<p>
The main difference between the capability of these libraries to handle data 
types is in the use of indirections (pointers). PACT provides its own memory
manager, SCORE, and leveraging its added functionality, PDB provides a mechanism to 
do I/O over variables containing any arbitrary level of indirection that terminates in 
a primitive or a structure containing no indirections. HDF5 currently has no similar 
internal capability, though the user-level program may provide a mechanism to 
recursively walk indirections in its data structures and insert the proper data 
into the HDF5 file by hand. Note that the user-level program would also be 
responsible for properly allocating memory and reading the data back into the 
correct data structures when re-initializing its data from the file. 
<p>

<a name="bench_features-dataobj"><h3>2.3 Data spaces and Data sets</h3></a>

<p>
HDF5 breaks down a data set into a data space, a data type, and the raw, user data. 
The user refers to a data set by name and treats it as a single object containing both 
the data and meta-data.  The user of HDF5 is fully aware of this collection, as a data 
space and data type must be created and associated with a data set along with the 
actual user data itself when preparing to write to a file. Most of this seems 
somewhat more transparent in PDB where much of the same information is collected 
during the write call. For instance, in the basic <i>PD_write</i> call, data space and type 
information is described in the syntax of the <b>name</b> and <b>type</b> argument strings, 
respectively. Dimension descriptions are encoded as in a FORTRAN dimension statement within the 
<b>name</b>. How this space and type information is managed internally is not the 
concern of the user. PDB sticks closely to the data model already present within programming 
languages, while HDF5 steps back and abstracts these into objects within its own data model. 
<p>

<a name="bench_features-dataconv"><h3>2.4 Data Conversions and Targeting</h3></a>

<p>
Neither file format requires user data be converted to the data standards of an
architecture before the data can be read there, rather, there is always sufficient
meta-data provided within the file to do any necessary data conversions on-the-fly. 
Both HDF5 and PDB read operations automatically convert to the local host's native
formats if user data isn't already in that form.
However, targeting write operations to a destination architecture has proven to be 
a useful ability, as that prevents conversions from being necessary on subsequent 
reads of the file on a target machine where the file may be read multiple times. 
The more often reads are done non-natively on that file, the more important the benefit 
of pre-converting data becomes. PDB and HDF5 both provide mechanisms to accomplish 
pre-converting/targeting data, though the approach taken by each differs.
<p>

<p>
All the necessary information to describe the standards PDB requires to do conversions 
are conveyed through the <i>PD_target</i> call and <b>type</b> argument string of a write 
operation. User code passes references to the target architecture's data standards and
alignment during a call to <i>PD_target</i>. From that point on, conversions are 
automatically applied to all data handled by a PDB write operation. What kinds of 
conversions are necessary are determined internally by the library by comparing the 
data standards and alignments set during the <i>PD_target</i> call with the data standards and 
alignment detected on the local host for a given type. 
<p>

<p>
A similar interface is provided by the H5T API in HDF5; however, a different 
approach is taken to describe the format of the data in memory and describe what 
format the data should have when it is written out to disk. A user initializes 
the data types of <b>each</b> of their data sets with the proper type of 
the destination machine (say, a 64 bit, big endian, floating-point: H5T_IEEE_F64BE). 
This only describes half the picture, that is, the format of the data once in the 
file. Then, as an argument to <i>HD5write</i>, the user must pass a data type 
descriptor for the format of the data resident in main memory on the local host 
(say, H5T_NATIVE_DOUBLE). PDB requires the latter as well, of course; however, when 
targeting one can apply a global setting for the 
format of data on disk using <i>PD_target</i>. In HDF5, if the user wishes to 
target all writes to the file for a specific machine, the format of each data set 
being written out to disk must be handled individually, rather than collectively. 
Overall, this approach, from the point of view of the library, is quite flexible, 
but may be seen as more cumbersome at the user-level. 
<p>

<a name="bench_features-fileorg"><h3>2.5 Data Organization</h3></a>

<p>
Both libraries provide the user with a method of hierarchically grouping variables 
and other data in a manner reminiscent of a file system's directory structure. PDB 
refers to this organization in much the same manner as one might at a UNIX command 
line interface, providing functions for changing directory (<i>PD_CD</i>), making 
directories (<i>PD_MKDIR</i>), listing members within a directory (<i>PD_LS</i>), 
and the like. HDF5 uses language that refers to directories as groups and has 
capabilities for creating groups (<i>H5Gcreate</i>) and getting a handle to an 
existing group (<i>H5Gopen</i>) at which point the user may iterate some function 
across its members (<i>H5Giterate</i>) who may be other groups or just datasets. 
Essentially, both libraries provide the same mechanisms. Though the iterator pattern
is not seen within PDB, the PDB user could loop over the variables within a directory 
in their user-level code.
<p>


<!-- ======================================================= -->

<a name="bench_api"><h2>3 Code Comparisons</h2></a>

<p>
In the general case, code to write out data is more succinct when written using 
PDBLib.  The following snippets of code show examples of using PDB 
and HDF5 to perform the same operations. The reason less code is required when 
writing PBDLib seems to be due to how much PDB hides data space and data type 
information within the parameters to its functions.  For example, storing an 
array of one-hundred doubles named f64Primitive to a PDB file would be written in C as:
<p>

<pre>
status = PD_write(tempFile, "f64Primitive(0:99)", "double", f64Primitive);
</pre>

<p>
The same user-level data space and type information would be conveyed to HDF5 as:
<p>

<pre>
dim[0] = 100;
arraySpace = H5Screate_simple(1, dim, NULL);
dataset = H5Dcreate(tempFile, "f64Primitive", H5T_NATIVE_DOUBLE, arraySpace, H5P_DEFAULT);
status = H5Dwrite(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, f64Primitive);
</pre>
<p>

<p>
Here H5S refers to the data space API and H5D refers to the data set API. As can be seen above, 
the meta-data gathered by PDB is taken in during the call to write the user
data. The name is associated with the user data and the type and space given by the parameters to the 
function call are used to construct the necessary meta-data structures on disk. The meta-data 
is spread out in the HDF5 case, into a data space (arraySpace), and data type (H5T_NATIVE_DOUBLE), 
and a data set, which groups together the user data, data space, and data type and associates a 
name with it. Once the information is gathered into a data set, the data set can be written out to disk.
<p>

<p>
When using the PDB API, the following code would suffice for writing a simple struct array 
out to disk:

<pre>
PD_defstr( tempFile, "testStruct",
                 "float x",
                 "float y",
                 "float z",
                 "float t",
                 "double val",
                 LAST );

status = PD_write(tempFile, "myComp(0:99)", "testStruct", myComp);
</pre>
<p>

<p>
For HDF5 this would be:

<pre>
comp_t = H5Tcreate(H5T_COMPOUND, sizeof(testStruct));

H5Tinsert(comp_t, "x", HOFFSET(testStruct, x), H5T_NATIVE_FLOAT);
H5Tinsert(comp_t, "y", HOFFSET(testStruct, y), H5T_NATIVE_FLOAT);
H5Tinsert(comp_t, "z", HOFFSET(testStruct, z), H5T_NATIVE_FLOAT);
H5Tinsert(comp_t, "t", HOFFSET(testStruct, t), H5T_NATIVE_FLOAT);
H5Tinsert(comp_t, "val", HOFFSET(testStruct, val), H5T_NATIVE_DOUBLE);

dim[0] = 100;
arraySpace = H5Screate_simple(1, dim, NULL);
dataset = H5Dcreate(tempFile, "myComp", comp_t, arraySpace, H5P_DEFAULT);
status = H5Dwrite(dataset, comp_t, H5S_ALL, H5S_ALL, H5P_DEFAULT, myComp);
</pre>
<p>

<p>
Each of these HDF5 examples show only native data types being written out. When the data 
format of the variable in memory differs from the desired format of the variable when 
written to file, the code grows. PDB only necessitates a call to PD_target.
Assume for a moment we are targeting for an Intel cluster from a big endian system.
<p>

<pre>
PD_target(&INTELB_STD, &INTELC_ALIGNMENT);

snprintf( alphaType, BUFFER_SIZE, "double alpha(%d)", SMALL_ARRAY_SIZE );
snprintf( betaType,  BUFFER_SIZE, "double beta(%d)",  SMALL_ARRAY_SIZE );
snprintf( gammaType, BUFFER_SIZE, "double gamma(%d)", SMALL_ARRAY_SIZE );

PD_defstr( tempFile, "arrayStruct",
               "float xCoord",
               "float yCoord",
               "float zCoord",
               "float t",
               alphaType,
               betaType,
               gammaType,
               LAST );

status = PD_write(tempFile, "theStruct(0:99)", "arrayStruct", theStruct);
</pre>

<p>
And again in HDF5:
<p>

<pre>
targetFloat_t  = H5Tcopy(H5T_IEEE_F32LE);
targetDouble_t = H5Tcopy(H5T_IEEE_F64LE);

compound_t   = H5Tcreate(H5T_COMPOUND, sizeof(arrayStruct));
small_dim[0] = SMALL_ARRAY_SIZE;
array_t      = H5Tarray_create(H5T_NATIVE_DOUBLE, 1, small_dim, NULL);

H5Tinsert(compound_t, "xCoord", HOFFSET(testStruct, xCoord), H5T_NATIVE_FLOAT);
H5Tinsert(compound_t, "yCoord", HOFFSET(testStruct, yCoord), H5T_NATIVE_FLOAT);
H5Tinsert(compound_t, "zCoord", HOFFSET(testStruct, zCoord), H5T_NATIVE_FLOAT);
H5Tinsert(compound_t, "t",      HOFFSET(testStruct, t),      H5T_NATIVE_FLOAT);
H5Tinsert(compound_t, "alpha",  HOFFSET(testStruct, alpha),  array_t);
H5Tinsert(compound_t, "beta",   HOFFSET(testStruct, beta),   array_t);
H5Tinsert(compound_t, "gamma",  HOFFSET(testStruct, gamma),  array_t);

compoundTarget_t = H5Tcreate(H5T_COMPOUND,
    (H5Tget_size(targetFloat_t)*4 + H5Tget_size(targetDouble_t)*SMALL_ARRAY_SIZE*3));
small_dim[0] = SMALL_ARRAY_SIZE;
arrayTarget_t = H5Tarray_create(targetDouble_t, 1, small_dim, NULL);

H5Tinsert(compoundTarget_t, "xCoord", 0,                            targetFloat_t);
H5Tinsert(compoundTarget_t, "yCoord", H5Tget_size(targetFloat_t),   targetFloat_t);
H5Tinsert(compoundTarget_t, "zCoord", H5Tget_size(targetFloat_t)*2, targetFloat_t);
H5Tinsert(compoundTarget_t, "t",      H5Tget_size(targetFloat_t)*3, targetFloat_t);
H5Tinsert(compoundTarget_t, "alpha",  H5Tget_size(targetFloat_t)*4, arrayTarget_t);
H5Tinsert(compoundTarget_t, "beta",
    (H5Tget_size(targetFloat_t)*4 + H5Tget_size(targetDouble_t)*SMALL_ARRAY_SIZE*1),
    arrayTarget_t);
H5Tinsert(compoundTarget_t, "gamma",
    (H5Tget_size(targetFloat_t)*4 + H5Tget_size(targetDouble_t)*SMALL_ARRAY_SIZE*2),
    arrayTarget_t);

dim[0] = 100;
arraySpace = H5Screate_simple(1, dim, NULL);
dataset = H5Dcreate(tempFile, "theStruct",
                      compoundTarget_t, arraySpace, H5P_DEFAULT);

status = H5Dwrite(dataset, compound_t,
                      H5S_ALL, H5S_ALL, H5P_DEFAULT, theStruct);
</pre>

<p>
Note the use of two compound type variables. One describes the format
of the struct in the local host's memory and those descriptors with the "target" modifier
describe the format of data on the target machine as it should appear in the file.
<p>

<p>
In fairness to HDF5, we discuss three important issues. First, the code used 
in the creation and insertion into compoundTarget_t could be changed to use <i>HOFFSET</i>. 
However, in the general case, it may not be true that the size of the converted types
(those variables containing "target" in their name) and the local host's native types 
are the same on both machines. Hence, this type of code may be necessary so members within 
the struct do not overlap within the file. Second, HDF5 handles conversions on-the-fly
fairly quickly, hence, targeting to a particular architecture may
not be necessary depending on user needs and the benefits from targeting the file 
would only be seen if the file was read more often than written. 
Third, an HDF5 Lite API is under development and being distributed 
separately from the basic HDF5 library. It currently provides a C API only. This
higher-level interface allows the user to create and read datasets using a simpler
interface. However, the API does not appear to provide a means to do hyperslab
copies, nor would it simplify the number of data type descriptors necessary in the 
example above. The HDF5 Lite API is certainly a step in the right direction as far 
as simplifying user-level code, but depending on user constraints, one may still need 
to use the basic HDF5 API.
<p>


<!-- ======================================================= -->

<a name="bench_results"><h2>4 Benchmark Results</h2></a>

<a name="bench_results-setup"><h3>4.1 System Setup</h3></a>

<p>
For the benchmark tests performed, the latest version of PACT from CVS 
was used, as well as the latest version of HDF5 
(<a href="http://hdf.ncsa.uiuc.edu/HDF5/release/obtain5.html">1.4.4</a>)
available on the web. The libraries were compiled on each platform in production 
mode with default optimizations on and debugging off. The benchmarks that we will
be describing were run on the following systems:
<p>

<p>
<center>
<table cellspacing=8 border=0>

<th>System</th><th>Processors</th><th>RAM</th><th>Operating System</th>

<tr>
<td>IBM SP</td><td>(16) 375 MHz Power 3</td><td>16 GB</td><td>AIX 5.1</td>
</tr>
<tr>
<td>Compaq ES45</td><td>(4) 1 GHz EV6.8 Alpha</td><td>4 GB</td><td>Tru64 Unix 5.1</td>
</tr>
<tr>
<td>Generic Intel</td><td>(2) 2.2 GHz Pentium 4 Xeon</td><td>2 GB</td><td>CHAOS 1.0 (based on RH 7.3)</td>
</tr>

</table>
</center>
<p>

<p>
The benchmarks were setup with a flexible interface to allow the user to
flag several options including the number of variables, data size, and what machine, 
if any, to target the file for. The benchmark simulated performing a restart dump
for a scientific application by doing I/O over a range of variable types
using both PDB and HDF5, and then timing the operation. Data types 
included large arrays of: int, long, float, double, simple structs, nested structs, 
and (if flagged) structs containing pointers. For the latter, code 
was written to follow the indirections on behalf of HDF5 and write out the data
by hand, and similarly when reading the data back, code was written to 
handle allocating memory and restoring the original state of the data structure. 
All benchmarks were run serially, on a single node, reading and writing 
to the local disk. The benchmarks were not run in single-user mode on a 
quiet job queue, hence some timing error must be assumed due to the process waiting 
in the run queue to be rescheduled on the processor. Several iterations of the 
same restart dump simulation were performed to determine a mean duration. All 
data written out was compared to that read back in as a correctness check.
<p>

<p>
We classify our benchmarks into three different cases. For <i>Native benchmarks</i>, 
all data was written to disk over the course of several PDB or HDF5 calls, and 
then the file was closed. Several iterations of this data dump were done and a mean
duration was taken. Then the same file was opened and either PDB or HDF5 calls were made
to read all data back into their respective variables. Again several iterations were 
used during this restart to determine a mean duration. The number of variables and 
the data size were varied to exhibit a range of I/O conditions. 
<i>Native benchmarks with indirections</i> consisted of the same methodology as 
<i>Native benchmarks</i> with the addition of an extra variable, a struct containing 
pointers to other structures.  Lastly, <i>Cross-platform benchmarks</i> were run by 
targeting data formats in the file to a particular architecture, writing the file to 
the local disk, then moving that file across the network to the target machine's 
local disk, where the file was finally read back in. Each of these scenarios will 
be discussed in turn.
<p>

<a name="bench_results-native"><h3>4.2 Native Performance</h3></a>

<p>
For these graphs, the number of variables was held constant as the amount of  
data per variable was varied. The datasize on the x-axis represents the sum of the byte 
size of all user variables in the restart dump. Time on the y-axis is either the sum of the
time to write the restart dump file (Dump) or the time to read back all data from the restart
dump file (Restart). The first benchmark graphs shown were performed on the Intel Xeon node. In the 
general case, the native performance exhibited by both libraries is fairly similar. PDB appears 
to scale more smoothly to larger I/O sizes in these first two graphs, taking less time than HDF5, 
though the percent difference is fairly small and may be due to noise introduced by other jobs 
on the system.
<p>

<center>
<table cellspacing=8>

<tr>
<td><center><img src="benchmarks1.gif"></center></td>
<td><center><img src="benchmarks2.gif"></center></td>
</tr>

<tr>
<td> <center> Graph I -- Performed on the Intel Xeon node.  </center> </td>
<td> <center> Graph II -- Performed on the Intel Xeon node.  </center> </td>
</tr>

<tr>
<td colspan=2>
<br>
<p>
The following graphs show native performance on the IBM SP node. The graphs again
indicate very similar performance characteristics though the results show the
opposite case with HDF5 scaling better. These four graphs are representative
of the performance shown in other cases tried on both machines. More fine-tuned
investigation would be necessary to determine how the results are affected by
process scheduling noise and I/O architecture differences. The high end of the
datasize scale is more sensitive to scheduling noise as many more I/O syscalls are
placed and more time is spent within the system giving more opportunity
for other processes to affect the timing of the restart dump simulation. 
Overall, the native performance appears to be fairly similar.
<p>
<br>
</td>
</tr>


<tr>
<td><center><img src="benchmarks3.gif"></center></td>
<td><center><img src="benchmarks4.gif"></center></td>
</tr>

<tr>
<td> <center> Graph III -- Performed on the IBM SP node.  </center> </td>
<td> <center> Graph IV -- Performed on the IBM SP node.  </center> </td>
</tr>


<tr>
<td colspan=2>
<br>
<p>
These last graphs were performed on the Compaq ES45 node. The datasize per
variable is based off of the system pagesize and datasizes have doubled due to 
the pagesize on Tru64 UNIX being twice that of the other systems. Here the I/O 
performance is again comparable assuming some error due to scheduling
or other noise within the system. 
<p>
<br>
</td>
</tr>


<tr>
<td><center><img src="benchmarks5.gif"></center></td>
<td><center><img src="benchmarks6.gif"></center></td>
</tr>

<tr>
<td> <center> Graph V -- Performed on the Compaq ES45 node.  </center> </td>
<td> <center> Graph VI -- Performed on the Compaq ES45 node.  </center> </td>
</tr>

</table>
</center>


<a name="bench_results-indir"><h3>4.3 Native Performance with Indirections</h3></a>

<center>
<table cellspacing=8>

<tr>
<td colspan=2>
<p>
These benchmarks focus on performance when an extra array of structs containing
pointer members are added to the variable set. The inefficiencies that can be seen
on HDF5's behalf are likely due to one of, or a combination of, two issues. The 
methodology of the HDF5 indirection handling code was to loop over the array of 
structs containing indirections, dereferencing and writing out each pointer as a 
separate data set.  This greatly increases the number of datasets within the HDF5 
file and it is feasible that HDF5 does not scale well with respect to the number of 
variables to be managed. The other possibility is that due to the large increase in 
the number of <i>H5Dwrite</i> calls, the overhead due to many more function calls 
and many, smaller writes causes the decrease in performance. More efficient code may 
be feasible than that used during the benchmarks, but the most straightforward code 
would also likely be the first approximation of user code that would be written to handle 
such a data structure by any given scientific application code base. 
<p>
<br>
</td>
</tr>

<tr>
<td><center><img src="benchmarks7.gif"></center></td>
<td><center><img src="benchmarks8.gif"></center></td>
</tr>

<tr>
<td><center>Graph VII -- Performed on the Intel Xeon node</center></td>
<td><center>Graph VIII -- Performed on the Compaq ES45 node</center></td>
</tr>

<tr>
<td colspan=2>
<br>
<p>
Further investigation shows that HDF5 does not scale as well as PDB when the
number of data sets to be managed increases to very large values. The reasons 
behind the lack of efficiency for HDF5 in this case are likely due
to B-tree data structures that must be maintained as more data sets are added
to the file.  Graph VIII is a native benchmark without any indirection variables 
where the datasize per variable is held constant at an eighth of the system pagesize
while the number of user variables varies. This suggests the likely cause of the 
decreased performance seen in the native indirections benchmarks are at least partially 
the result of this situation. Approaches that could continue to store the data associated 
with the indirect structs in the previous benchmarks while maintaining fewer data sets 
would likely increase the efficiency of the HDF5 code, however it would remain that the 
user-level code would be left responsible for creating this efficient code base in order 
to handle any situation where the structure members were not statically allocated. 
<p>
</td>
</tr>

</table>
</center>


<a name="bench_results-cross"><h3>4.4 Cross-platform Performance</h3></a>

<p>
On the Intel Xeon node, benchmarks were run targeting the format of the 
restart dump data for the Power 3 on the IBM SP node.
The differences of note between the two architectures is that the Intel chip
is little endian and the Power 3 is big endian hence byte-order reversals will be
performed. No alignment nor primitive datatype byte size differences exist.
<p>

<center>
<table cellspacing=8>

<tr>
<td><center><img src="benchmarks9.gif"></center></td>
<td><center><img src="benchmarks10.gif"></center></td>
</tr>

<tr>
<td> <center>Graph IX -- Target files to Power3 architecture</center> </td>
<td> <center>Graph X -- Create files without targeting</center> </td>
</tr>

</table>
</center>

<p>
All restart dump files are written on the Intel Xeon system, then transfered to and read by 
the IBM SP node. The first graph shows the performance when the file's data formats are 
targeted to the IBM SP platform, while the second graph illustrates performance when no 
targeting is applied. Comparing the restart times of the two graphs, we see both libraries 
were better off taking the extra time to write a file meant for a Power 3 processor. We also see 
that PDB and HDF5 restart performance is comparable in Graph IX where both libraries target their 
files though a little extra cost is taken during the creation of the PDB file to obtain similar 
restart times. It appears that PDB is somewhat less efficient here and this cost must be 
taken at either the time of creation of the file (Graph IX) or when the file is read (Graph X), 
if it is the case that the file is transfered between machines of a different architecture. 
Otherwise, if there is no change in platform, the native benchmarks are the ones that apply. 
<p>

<!-- ======================================================= -->

<a name="bench_conclusions"><h2>5 Conclusions</h2></a>

<p>
We have discussed the features provided by both libraries and have found that, in general,
they provide a very similar interface to binary data storage and retrieval. The major 
exception being PDB's ability to handle data structures containing indirections.
HDF5 is unable to handle this situation unless the user takes the responsibility 
upon themselves to write the code necessary to traverse the data structures. Otherwise, 
available data types, attributes, file organization features, as well as conversion 
abilities are nearly identical. 
<p>

<p>
Native performance exhibited by both codes was very comparable. The mean duration
to do large restart dump simulations was similar across a large spectrum of I/O conditions.
If the user's needs keep the file usage local, then the performance will be fairly
similar, though if the user's needs require that the file be moved across non-homogeneous
systems, then the performance will favor HDF5 slightly. Also, when scaling the number of 
variables to larger values it was shown that PDB was more efficient than HDF5. This may be 
due to the internal data structures chosen to manage the meta-data within the file. 
<p>

<p>
PDB code was found to be more succinct than HDF5 for similar operations, though
HDF5 Lite has taken on the task of developing a simpler interface to HDF5. The basic
HDF5 interface requires the user to manipulate many more handles than PDB and
perhaps some of this could be hidden away. PDB manages much of the data-type and
data-space complexity by allowing the user to embed the information in the read or
write call itself. PDB handles reads and writes as essentially added functionality
to the operating system level read and write calls. HDF5 handles the same situation by 
breaking out each component of their I/O model into its own API.
<p>

<p>
The features provided by PDB and HDF5 are very similar; however, upon closer inspection
we have shown that some differences do exist. The main similarities appear in the native
performance and basic library features, while the main differences show themselves in 
cross-platform performance and the handling of data types with indirections. Overall,
the functionality provided by both libraries is well-developed and either would be a good
choice for a project's binary storage requirements.
<p>

<!-- ======================================================= -->

<a name="bench_ref"><h2>References</h2></a>

<ul>
<li>PACT Development Team. "<a href="index.html">Portable Application Code Toolkit</a>." 
<i>PACT</i>. April 11th, 2000. Lawrence Livermore National Laboratory. November 1st, 2002. </li>
<li>HDF5 Development Team. "<a href="http://hdf.ncsa.uiuc.edu/HDF5/">Hierarchical Data Format 5</a>."
<i>HDF5</i>. October 2nd, 2002. National Center for Supercomputing Applications. November 1st, 2002. </li>
</ul>

<hr>

For questions and comments, 
please contact the <A HREF="pactteam.html">PACT
Development Team</A>.
<hr>
Last Updated: $MOD
