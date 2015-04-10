TXT: PACT Python Bindings
MOD: 3/13/2015

<CENTER>
<P>
<H1><font color="#ff0000">$TXT</font></H1>
<H4>Lee Taylor
</H4>
</CENTER>

<BLOCKQUOTE>

<P>
<HR>

<ul>
<li><a href="#pypact_intro"><H3>Introduction</H3></a>
<li><a href="#pypact_pdb"><H3>PDB</H3></a>
<li><a href="#pypact_pgs"><H3>PGS</H3></a>
</ul>
<hr>

<a name="pypact_intro"><h2>Introduction</h2></a>

PyPact provides access to PACT from Python.
This is provided as a package with two modules.
The package is <tt>pact</tt> and the modules are <tt>pdb</tt> and <tt>pgs</tt>.
The <tt>pdb</tt> module provides access to Score, PML and PDBLib.
The <tt>pgs</tt> module provides access to Score, PML, PDBLib and PGS.
Only one module should be used at a time.
No access is provided to Scheme or Panacea at this time.

<p>
TODO<BR>
If you are using a PACT distribution:<BR>
(The user needs to know how to build the shared objects.)<BR>
If you see the error message:
<p>
<tt>ImportError: No module named _pdb</tt>
<p>
then you need to build the shared objects.<BR>
END OF TODO

<p>
If you are using your own private version of PACT built from a PACT
distribution, e.g. in <em>~/pact</em>,
then you can do the following in your Python code:
<p>

<a name="pypact_pdbdata_example"><h3>Example</h3></a>
<blockquote>
<pre>
import os
import sys
site_packages = os.path.expanduser( "~/pact/python" )
if os.path.isdir( site_packages ) :
    sys.path.append( site_packages )
else :
    print "No PACT python support on this platform."
    sys.exit( -1 )
                                                                                
import pact.pdb as pdb
</pre>
</blockquote>

<p>
Putting the modules in a package helps keep the namespace clean and
avoids confusion with the <em>other</em> pdb module - the Python Debugger.

<p>
To import the library:
<blockquote>
<tt>import pact.pdb as pdb</tt><br> or <br><tt>import pact.pgs as pgs</tt>
</blockquote>
To import the PDB library AND use the Python debugger:
<blockquote>
<tt>
import pact.pdb<br>
import pdb
</tt>
</blockquote>
<p>

Recall that the PACT tools fit in the following hierarchy:

<p>
<CENTER>
<B>
SX
<BR>
PANACEA
<BR>
PGS     SCHEME
<BR>
PDB
<BR>
PML
<BR>
SCORE
</B>
</CENTER>
<br>
Score and PML are not provided as stand alone modules since 
PDB is the first level that Python users are usually interested in using.
Much of Score functionality such as memory allocation, hash tables and
association lists are accessable from Python.
However the module also depends on PDBLib's functionality to
store arbitrary data in Score's hash tables and association lists.
In the C library this is implemented by associating a <em>type</em> with each 
<tt>haelem</tt> or <tt>pcons</tt>.
In PyPact the <em>type</em> must be defined in a PDB file.
PyPact uses a virtual internal file to store the type information.
It is accessed as the module variable <tt>vif</tt>.
It is also the default <em>file</em> argument for many methods.

<p>
Many structures in PACT are represented as classes in PyPact.
Most structures have a function in the C API that returns a pointer to a new structure
and several support functions that receive the structure pointer as one of the
arguments (usually the first).
This maps directly to a <em>constructor</em> and <em>methods</em>.

<p>
The hash table functions follow the typical pattern.

<p>
C Bindings:
<dl>
<dt><tt>hasharr *SC_hash_alloc()</tt>
<dt><tt>int SC_hasharr_install(hasharr *self, char *type, void *key, void *obj)</tt>
<dt><tt>void SC_free_hasharr(hasharr *self)</tt>
</dl>

With typical usage:

<pre>
     int ierr, ival1;
     char one[] = "one";
     hasharr *ht;

     ht = SC_hash_alloc();
  
     ival1 = 1;
     iok = SC_hasharr_install(ht, "int", "one", &ival1);
     if (iok == 0) {
        errproc("Error inserting one\n";); 
     }

     SC_free_hasharr(ht);
</pre>

In PyPact <tt>SC_make_hasharr</tt> is replaced by the <tt>hasharr</tt>
class constructor and <tt>SC_hasharr_install</tt> becomes a method of the instance.
<tt>SC_hash_free</tt> is called by Python's garbage collector when there are 
no more references to the instance.

<pre>
    import pact.pdb as pdb
    ht = pdb.hasharr()
    ht.install("one", 1)
</pre>

PACT errors are trapped by raising Python exceptions.

<p>
Where natural, types also use standard protocols.  For example,
both hash tables and association lists use the mapping protocal.

<pre>
    ht = pdb.hasharr()
    ht["one"] = 1
</pre>


<!-- ------------------------------------------------------------>
<hr>

<a name="pypact_pdb"><h2>PDB</h2></a>

<p>
This section focuses on how to use the PDB module.

<a name="pypact_pdbdata"><h3>The pdbdata Object</h3></a>

The <tt>pdbdata</tt> object is the only object in PyPact that does not 
map directly to a structure in PACT.
It is used to fully describe a piece of memory and acts as a middle-man 
between the C and Python type systems.
It consists of a pointer to memory and sufficient information to describe
the memory.
This includes the type and dimensions.
This also includes a pointer to a PDBfile that is used to look up the type.
With this information Python is able to access the memory as the user
expects including accessing members of a derived type as attributes of
an object.

Python provides int and float objects.
The Python implementation uses a C <tt>long</tt> for int and 
and C <tt>double</tt> for float.
The pdbdata constructor allows the default types to be overridden.

<tt>pdbdata(1, 'int')</tt>

Since Python does not support pointers directly,
a sequence of Python objects will match several C declarations.
<tt>[ 1, 2, 3, 4, 5]</tt> can be both <tt>int[5]</tt> and <tt>int *</tt>.
In one case, it is clear that there are 5 integers (<tt>int[5]</tt>).
In the other, we only know that it points to an int.
By default, a pdbdata object will be explicit about lengths
and use <tt>int[5]</tt>.
However, if it's necessary to match a specific type, then <tt>int *</tt>
can be specified.
The pdbdata type requires that pointers be allocated by Pact.
This allows it to find out how much memory is allocated
which allows a pdbdata to be converted back into Python objects.

Nested sequences are represented as multiply dimensioned types.
<tt>([[1,2,3],[4,5,6]]</tt> is <tt>long[2][3]</tt>.

If the array is not regular, then it is treated as an array of pointers.
<tt>[[1,2,3],[4,5]]</tt> is <tt>long *[2]</tt>.

It is also possible to convert Python class instances into pdbdata object.
See the register_class method of PDBfile object below.

<tt>None</tt> is treated as <tt>NULL</tt> and results in a pointer type.
No type at all defaults to <tt>int</tt>
<tt>[None] </tt> is <tt>int *[1]</tt>

<a name="pypact_pdbdata_cons"><h4>Constructor</h4></a>

<pre>pdbdata(type, data, file)
</pre>
<em>data</em> -- data of type <i>type</i> to be encapsulated.<br>
<em>type</em> -- string name of the data type of the <tt>pdbdata</tt>.<br>
<em>file</em> -- define the types to this PDBfile.
(This argument is optional and the virtual internal file is used by default.)<br>

<p>

<a name="pypact_pdb_methods"><h4>Module Methods</h4></a>

<p>
These module methods take a <tt>pdbdata</tt> object and return the individual fields.

<dl>
<dt><tt>getdefstr(obj)</tt> - returns a <tt>defstr</tt> object.
<dt><tt>gettype(obj)</tt> - returns a string.
<dt><tt>getfile(obj)</tt> - returns a <tt>PDBfile</tt> object.
<dt><tt>getdata(obj)</tt> - returns a <tt>CObject</tt> object.
<!-- <dt><tt>getmember(obj)</tt> - returns None. -->
</dl>

<p>
These modules methods can be used to print the contents of objects.
These replace the output from the <tt>print</tt> statement in
earlier version.  Python 3 changed the way objects are printed
requiring the addition of these functions.

<dl>
<dt><tt>printdata(obj [, file])</tt> - print a <tt>pdbdata</tt> object to stdout.
<dt><tt>printdefstr(obj [, file])</tt> - print a <tt>defstr</tt> object to stdout.
</dl>

In addition to using the constructor,
a pdbdata can be returned by any method that returns user data; for example, 
<tt>pdbfile.read</tt>.

<p>
It is controlled by the <tt>setform</tt> method.

<pre>
setform(array, struct, scalar)
</pre>

With no arguments the current values of setform are returned as a tuple.
<em>array</em>, <em>struct</em> and <em>scalar</em> should be one
of the following values, and reflect how reads and writes handle
arrays, structs, and scalar data, respectively.

<dl>
<dt><tt>AS_NONE</tt> - Keep existing value.
<dt><tt>AS_PDBDATA</tt> - return as a pdbdata
<dt><tt>AS_OBJECT</tt> - return as object
<dt><tt>AS_TUPLE</tt> - return as a tuple
<dt><tt>AS_LIST</tt> - return as a list
<dt><tt>AS_DICT</tt> - return as a dictionary
<dt><tt>AS_ARRAY</tt> - return as a numpy array
</dl>

Not all constants are valid for all forms.

<table>
<tr>
<td>type</TD>
<td>array</td>
<td>struct</td>
<td>scalar</td>
</tr><tr>
<td><tt>AS_PDBDATA</tt></TD>
<td align="center">X</td>
<td align="center">X</td>
<td align="center">X</td>
</tr><tr>
<td><tt>AS_OBJECT</tt></TD>
<td align="center"></td>
<td align="center"></td>
<td align="center">X</td>
</tr><tr>
<td><tt>AS_TUPLE</tt></TD>
<td align="center">X</td>
<td align="center">X</td>
<td align="center"></td>
</tr><tr>
<td><tt>AS_LIST</tt></TD>
<td align="center">X</td>
<td align="center"></td>
<td align="center"></td>
</tr><tr>
<td><tt>AS_DICT</tt></TD>
<td align="center"></td>
<td align="center">X</td>
<td align="center"></td>
</tr><tr>
<td><tt>AS_ARRAY</tt></TD>
<td align="center">X</td>
<td align="center"></td>
<td align="center"></td>
</tr>
</table>

<!--
  reorient table
<table>
<tr>
<td>type</TD>
<td><tt>AS_PDBDATA</tt></TD>
<td><tt>AS_OBJECT</tt></TD>
<td><tt>AS_TUPLE</tt></TD>
<td><tt>AS_LIST</tt></TD>
<td><tt>AS_DICT</tt></TD>
<td><tt>AS_ARRAY</tt></TD>
</tr><tr>
<td>array</td>
<td align="center">X</td>
<td align="center"></td>
<td align="center">X</td>
<td align="center">X</td>
<td align="center"></td>
<td align="center">X</td>
</tr><tr>
<td>struct</td>
<td align="center">X</td>
<td align="center"></td>
<td align="center">X</td>
<td align="center"></td>
<td align="center">X</td>
<td align="center"></td>
</tr><tr>
<td>scalar</td>
<td align="center">X</td>
<td align="center">X</td>
<td align="center"></td>
<td align="center"></td>
<td align="center"></td>
<td align="center"></td>
</tr>
</table>
-->

<p>
<pre>
unpack(data, array, struct, scalar)
</pre>
Unpack a <tt>pdbdata</tt> object into more Python friendly types.
<p>
If <tt>pdbdata</tt> represents a structure then the members can be
accessed as attributes of object.
<p>
If <tt>pdbdata</tt> represents an array then the sequence protocol is 
supported.
<p>
<a name="pypact_pdbdata_example"><h3>Examples</h3></a>

<blockquote>
<pre>
>>> import pact.pdb as pdb

>>> d = pdb.pdbdata(4.0, 'double')
>>> d
data =   4.0000000000000000e+00

>>> r = pdb.unpack(d)
>>> type(r)
&lt;type 'float'&gt;
>>> r
4.0

>>> d = pdb.pdbdata((4.0, 5.0), 'double[2]')
>>> d
data(0) =   4.0000000000000000e+00
data(1) =   5.0000000000000000e+00

>>> r = pdb.unpack(d)
>>> type(r)
&lt;type 'list'&gt;
>>> r
[4.0, 5.0]
>>> pdb.setform(array=pdb.AS_TUPLE)
(3, 3, 2)
>>> r = pdb.unpack(d)
>>> type(r)
&lt;type 'tuple'&gt;
>>> r
(4.0, 5.0)


>>> d = pdb.pdbdata([None, [4., 5.]], 'double **')
>>> d
data(0) = (nil)
data(1)(0) =   4.0000000000000000e+00
data(1)(1) =   5.0000000000000000e+00

>>> r = pdb.unpack(d)
>>> r
[None, [4.0, 5.0]]
>>> print r[0]
None
>>> print r[1]
[4.0, 5.0]

</pre>
</blockquote>

<!--
<a name="pypact_pdbdata"><h2>PDB data Object</h2></a>

Created when a structure is read.
-->

<a name="pypact_file"><h3>File Object</h3></a>

There are a few module variables that are used to access files.
<tt>files</tt> is a dictionary of opened <tt>PDBfile</tt> instances
   indexed by file name.  It is used to store references to files
to avoid garbage collection
until the <tt>close</tt> method is explicitly called.
<tt>vif</tt> is the virtual internal file.  It is used as the default
  file for many other methods.
<p>


<tt>PDBfile(name[, mode])</tt>
<em>name</em> is the name of the file to open/create.
<em>mode</em> is the file mode -
  <tt>r</tt> read,
  <tt>w</tt> write,
  <tt>a</tt> append
The default is <tt>r</tt>.
<tt>open</tt> is an alias for <tt>PDBfile</tt>.

<a name="pypact_file_attrs"><h4>Attributes</h4></a>

<dl>
<dt><tt>object</tt>
<dt><tt>name</tt>
<dt><tt>type</tt>
<dt><tt>symtab</tt>
<dt><tt>chart</tt>
<dt><tt>host_chart</tt>
<dt><tt>attrtab</tt>
<dt><tt>previous_file</tt>
<dt><tt>date</tt>
<dt><tt>mode</tt>
<dt><tt>default_offset</tt>
<dt><tt>virtual_internal</tt>
<dt><tt>system_version</tt>
<dt><tt>major_order</tt>
</dl>

<a name="pypact_file_methods"><h4>Methods</h4></a>

The <em>ind</em> argument is used to index arrays.
If it is a scalar, it must be the number of items.
If it is a sequence, then each member applies to a dimension.
If a scalar, then it is the number of items.
If length is 1, it is interpreted as (upper, ).
If length is 2, it is interpreted as (lower, upper).
If length is 3, it is interpreted as (lower, upper, stride).
<p>
The following examples use f90 array notation.
<table>
<tr>
<td>f90
<td><em>ind</em>
</tr><tr>
<td>(4)
<td>4
</tr><tr>
<td>(4)
<td>[4]
</tr><tr>
<td>(0:5)
<td>[(0,5)]
</tr><tr>
<td>(1:5:2)
<td>[(1,5,2)]
</tr><tr>
<td>(4, 5)
<td>[4, 5]
</tr><tr>
<td>(0:5, 0:6)
<td>[(0,5), (0,6)]
</tr><tr>
<td>(7, 0:10)
<td>[7, (0, 10)]
<tr>
</table>

When writing data, PyPact will try to determine the type of the data
if <em>outtype</em> is not specified.  Python reals map to <tt>doubles</tt>.
strings map to <tt>char *</tt>
<p>
<!-- A note about pointers and arrays -->
<p>

<dl>
<dt><tt>flush()</tt>
<dt><tt>close()</tt>
<dt><tt>write(name, var[, outtype, ind])</tt>
  <dl>
  <dt><em>name </em> is the name of the variable to write.
  <dt><em>var </em> is the variable object to write.
  <dt><em>outtype </em> optional, is the type of variable.
  <dt><em>ind </em> optional, indexing information.
  </dl>
<dt><tt>write_raw(name, var, type[, ind])</tt>
  <dl>
  <dt><em>name </em> is the name of the variable to write.
  <dt><em>var </em> is the variable object to write.
  It must support the buffer interface.
  <dt><em>type </em> is the type of variable.
  <dt><em>ind </em> optional, indexing information.
  </dl>
<dt><tt>read(name[, intype, ind])</tt>
  <dl>
  <dt><em>name </em> is the name of the variable to read.
  <dt><em>intype </em> optional, type of data desired.
  <dt><em>ind </em> optional, indexing information.
  </dl>
<dt><tt>defent(name, type)</tt>
  <dl>
  <dt><em>name </em> name of variable.
  <dt><em>type </em> type of data in file.
  </dl>
<dt><tt>defstr(name, members)</tt>
  <dl>
  <dt><em>name </em> name of new type
  <dt><em>members </em> optional, sequence of types.
       If members are defined a new type is created.
       Otherwise <em>name</em> is looked up and returned.
  </dl>
<dt><tt>cd(dirname)</tt>
  <dl>
  <dt><em>dirname </em> Name of directory.
  </dl>
<dt><tt>mkdir(dirname)</tt>
  <dl>
  <dt><em>dirname </em> Name of directory.
  </dl>
<dt><tt>ln(var, link)</tt>
  <dl>
  <dt><em>var </em>
  <dt><em>link </em>
  </dl>
<dt><tt>ls([path, type])</tt>
  <dl>
  <dt><em>path </em> optional
  <dt><em>type </em> optional
  </dl>
<dt><tt>pwd()</tt>
<dt><tt>register(cls, type [, ctor])</tt>
  <dl>
  <dt><em>cls </em> The Python Class object
  <dt><em>type </em> The name of the pdblib defstr.
  <dt><em>ctor </em> This function is used during read.  It accepts a
  dictionary of structure members and returns an object of class cls.
  </dl>
</dl>

<a name="pypact_file_examples"><h4>Examples</h4></a>

Open a file.
<blockquote>
<pre>
<!--
        import pact.pdb as pdb
        fp = pdb.PDBfile("file1", "w")
        fp.close()
        fp = pdb.open("file2")
        fp.close()
-->
>>> import pact.pdb as pdb
>>> fp = pdb.open("xxxx", "w")
>>> type(fp)
&lt;type 'PDBfile'&gt;
>>> pdb.files
{'xxxx': &lt;PDBfile object at 0xb3f7bb40&gt;}
>>> fp.close()
>>> pdb.files
{}
>>>  ctrl-D
</pre>
</blockquote>


Write four doubles to a files as a 2-d double array and as
a 1-d float array
<!-- File.test2c -->
<blockquote>
<pre>
%python
>>> import pact.pdb as pdb
>>> fp = pdb.open("xxxx", "w")
>>> ref = [2.0, 3.0, 4.0, 5.0]
>>> fp.write("d2", ref, ind=[2,2])
>>> fp.write("d3", ref, "float")
>>> fp.close()
>>>  ctrl-D
%pdbview xxxx

	PDBView 2.0 - 11.22.04

-> ls

d2   d3                                                                         
-> d2

/d2(0,0) =   2.0000000000000000e+00
/d2(0,1) =   3.0000000000000000e+00
/d2(1,0) =   4.0000000000000000e+00
/d2(1,1) =   5.0000000000000000e+00

-> d3

/d3(0) =   2.0000000e+00
/d3(1) =   3.0000000e+00
/d3(2) =   4.0000000e+00
/d3(3) =   5.0000000e+00

-> desc d2

Name: d2
Type: double
Dimensions: (0:1, 0:1)
Length: 4
Address: 108

-> desc d3

Name: d3
Type: float
Dimensions: (0:3)
Length: 4
Address: 140

-> quit
</pre>
</blockquote>

Write a class instance
<blockquote>
<pre>
%cat user.py
class User:
    def __init__(self, a, b, c):
        self.a = a
        self.b = b
        self.c = c
    def __repr__(self):
        return 'User(%(a)d, %(b)d, %(c)f)' % self.__dict__

def makeUser(dict):
    return User(dict['a'], dict['b'], dict['c'])

fp = pdb.open("user.pdb", "w")
fp.defstr( "user", ("int a", "int b", "float c"))
fp.register_class(User, "user", makeUser)

v1 = User(1,2,3)
fp.write("var1", v1)
v2 = fp.read("var1")
fp.close()
print "v1 =", v1
print "v2 =", v2

%python user.py
v1 = User(1, 2, 3.000000)
v2 = User(1, 2, 3.000000)

%pdbview user.pdb
 
-> var1
 
/var1.a =          1
/var1.b =          2
/var1.c =  3.0000000e+00


</blockquote>
</pre>



<a name="pypact_defstr"><h3>Defstr Object</h3></a>

A <tt>defstr</tt> type is used to define and create structures.

<tt>defstr(name, members[, file])</tt>
<dl>
<dt><em>name </em>name for defstr.
<dt><em>members </em>sequence of type names.
<dt><em>file </em> PDBfile instance. optional defaults to <tt>vif</tt>
</dl>

The returned instance is also a constructor for instances of a defstr.

<a name="pypact_defstr_attrs"><h4>Attributes</h4></a>

<dl>
<dt><tt>dp</tt>
<dt><tt>type</tt>
<dt><tt>size_bits</tt>
<dt><tt>size</tt>
<dt><tt>alignment</tt>
<dt><tt>n_indirects</tt>
<dt><tt>is_indirect</tt>
<dt><tt>convert</tt>
<dt><tt>onescmp</tt>
<dt><tt>unsgned</tt>
<dt><tt>order_flag</tt>
</dl>

<a name="pypact_defstr_methods"><h4>Methods</h4></a>

A <tt>defstr</tt> supports the mapping protocol plus 
some methods usually associated with mappings.

<dl>
<dt><tt>has_key</tt> Not Implemented
<dt><tt>items</tt> Not Implemented
<dt><tt>keys</tt>
<dt><tt>values</tt> Not Implemented
<dt><tt>get</tt> Not Implemented
</dl>

<a name="pypact_defstr_examples"><h4>Examples</h4></a>

<pre>

>>> import pact.pdb as pdb
>>> d = pdb.defstr('struct', ('int i', 'float j'))
>>> type(d)
<type 'defstr'>
>>> print d
Type: struct
Alignment: 4
Members: {int i;
          float j;}
Size in bytes: 8

>>> a = d((3,4))
>>> a
data.i =           3
data.j =   4.0000000e+00

>>> a.i
3
>>> a.j
4.0
>>> a.i = 5
>>> a.j = 6
>>> a
data.i =           5
data.j =   6.0000000e+00

-----------------

>>> import pact.pdb as pdb
>>> d = pdb.defstr('struct', ('int i', 'float j'))
>>> a = d((3,4))
>>> fp = pdb.open('yyyy', 'w')
>>> fp.defstr('struct', d)
Type: struct
Alignment: 4
Members: {int i;
          float j;}
Size in bytes: 8

>>> fp.write('aaa', a)
>>> fp.close()
>>>  ctrl-D
% pdbview yyyy

	PDBView 2.0 - 11.22.04

-> ls

aaa                                                                             
-> aaa

/aaa(1).i =           3
/aaa(1).j =   4.0000000e+00

-> desc aaa

Name: aaa
Type: struct
Dimensions: (1:1)
Length: 1
Address: 108

-> struct struct

Type: struct
Alignment: 4
Members: {int i;
          float j;}
Size in bytes: 8
</pre>

<a name="pypact_memdes"><h3>Memdes Object</h3></a>

<a name="pypact_memdes_attrs"><h4>Attributes</h4></a>

<dl>
<dt><tt>desc</tt>
<dt><tt>member</tt>
<dt><tt>cast_memb</tt>
<dt><tt>cast_offs</tt>
<dt><tt>is_indirect</tt>
<dt><tt>type</tt>
<dt><tt>base_type</tt>
<dt><tt>name</tt>
<dt><tt>number</tt>
</dl>

<a name="pypact_memdes_methods"><h4>Methods</h4></a>

None

<a name="pypact_memdes_examples"><h4>Examples</h4></a>
<pre>
</pre>

<!-- ------------------------------------------------------------>
<a name="pypact_score"><h2>SCORE</h2></a>

<tt>hasharr</tt> and <tt>assoc</tt> both seem very similar at the 
Python level. Both implement the mapping protocol.
The chief difference is in the data structures they build in memory.
This allows the user to build the type of data structure required by PACT.
For example, many graphics routines take or return 
an association list to describe plotting options.
The application can treat the association list
in a Pythonic manner by treating it as a dictionary.
<p>
<a name="pypact_memory"><h3>Memory Allocation</h3></a>

PyPact provides a way to allocate and check memory using
PACT's memory allocation routines.
The <tt>void *</tt> pointer is contained in a <tt>CObject</tt>.
There is no PyPact specific type/class for memory.
All methods are module methods, not class methods.
The <tt>CObject</tt> is a convient way to pass C pointers around
and is generally only useful to methods that know 
what to do with it.
<p>

<a name="pypact_memory_attrs"><h4>Attributes</h4></a>

None

<a name="pypact_memory_methods"><h4>Methods</h4></a>

<dl>
<dt><tt>zero_space(flag)</tt>
<dt><tt>alloc(nitems, bytepitem, name)</tt>
<dt><tt>realloc(p, nitems, bytepitem)</tt>
<dt><tt>cfree(p)</tt>
<dt><tt>mem_print(p)</tt>
<dt><tt>mem_trace()</tt>
<dt><tt>reg_mem(p, length, name)</tt>
<dt><tt>dereg_mem(p)</tt>
<dt><tt>mem_lookup(p)</tt>
<dt><tt>mem_monitor(old, lev, id)</tt>
<dt><tt>mem_chk(type)</tt>
<dt><tt>is_score_ptr(p)</tt>
<dt><tt>arrlen(p)</tt>
<dt><tt>mark(p, n)</tt>
<dt><tt>ref_count(p)</tt>
<dt><tt>set_count(p, n)</tt>
<dt><tt>permanent(p)</tt>
<dt><tt>arrtype(p, type)</tt>
</dl>


<a name="pypact_memory_examples"><h4>Examples</h4></a>
<blockquote>
<pre>
>>> p = pdb.alloc(2, 8, "array1")
>>> type(p)
&lt;type 'PyCObject'&gt;
>>> pdb.arrlen(p)
16
</pre>
</blockquote>

<a name="pypact_hasharr"><h3>Hash Table</h3></a>
<a name="pypact_hasharr_attrs"><h4>Attributes</h4></a>

None

<a name="pypact_hasharr_methods"><h4>Methods</h4></a>

<dl>
<dt><tt>install(key, obj, type)</tt>
<dt><tt>def_lookup(key)</tt>
<dt><tt>clear()</tt>

<dt><tt>has_key(key)</tt>
<dt><tt>items</tt> - not implemented
<dt><tt>keys()</tt>
<dt><tt>update(dict)</tt>
<dt><tt>values</tt> - not implemented
<dt><tt>get</tt> - not implemented
</dl>

<a name="pypact_hasharr_examples"><h4>Examples</h4></a>

<blockquote>
<pre>
>>> ht = pdb.hasharr()
>>> ht["one"] = 1
>>> ht.keys()
('one',)
>>> ht["one"]
1

>>> pdb.vif.chart.keys()
('defstr', 'syment', 'symindir', 'symblock', 'memdes', 'dimdes',
'hasharr', 'haelem', 'PM_mapping', 'PM_mesh_topology', 'PM_set',
'PG_image', 'pcons', 'SC_array', 'Directory', 'function',
'REAL', 'double', 'float', 'u_long_long', 'long_long', 'u_long',
'long', 'u_integer', 'integer', 'u_int', 'int', 'u_short', 'short',
'u_char', 'char', '*')
>>> pdb.vif.symtab.keys()
('/', '/&ptrs/')

</pre>
</blockquote>

<a name="pypact_alist"><h3>Association List</h3></a>
<a name="pypact_alist_attrs"><h4>Attributes</h4></a>

None

<a name="pypact_alist_methods"><h4>Methods</h4></a>

<dl>
<dt><tt>clear</tt> - not implemented
<dt><tt>has_key(key)</tt> 
<dt><tt>items()</tt>
<dt><tt>keys()</tt>
<dt><tt>update(dict)</tt>
<dt><tt>values</tt> - not implemented
<dt><tt>get</tt> - not implemented
</dl>

<a name="pypact_alist_examples"><h4>Examples</h4></a>
<pre>
</pre>

<!-- ------------------------------------------------------------>
<a name="pypact_pml"><h2>PML</h2></a>

TODO

<a name="pypact_mapping"><h3>Mapping Object</h3></a>
<a name="pypact_mapping_attrs"><h4>Attributes</h4></a>
<a name="pypact_mapping_methods"><h4>Methods</h4></a>
<a name="pypact_mapping_examples"><h4>Examples</h4></a>
<pre>
</pre>
<a name="pypact_set"><h3>Set Object</h3></a>
<a name="pypact_set_attrs"><h4>Attributes</h4></a>
<a name="pypact_set_methods"><h4>Methods</h4></a>
<a name="pypact_set_examples"><h4>Examples</h4></a>
<pre>
</pre>
<a name="pypact_field"><h3>Field Object</h3></a>
<a name="pypact_field_attrs"><h4>Attributes</h4></a>
<a name="pypact_field_methods"><h4>Methods</h4></a>
<a name="pypact_field_examples"><h4>Examples</h4></a>
<pre>
</pre>
<a name="pypact_topology"><h3>Mesh Topology Object</h3></a>
<a name="pypact_topology_attrs"><h4>Attributes</h4></a>
<a name="pypact_topology_methods"><h4>Methods</h4></a>
<a name="pypact_topology_examples"><h4>Examples</h4></a>
<pre>
</pre>


<!-- ------------------------------------------------------------>
<hr>

<a name="pypact_pgs"><h2>PGS</h2></a>

<!-- ------------------------------------------------------------>
<a name="pypact_api"><h2>API</h2></a>

PyPact has some user callable routines to allow the developer
to define their own types to PyPact.  This will allow PyPact to
return an instance of the correct class when reading from a file.
<p>

<pre>
typedef int (*PP_pack_func) (void *vr, PyObject *obj, long nitems, int tc)
typedef PyObject *(*PP_unpack_func) (void *vr, long nitems)
typedef PP_descr *(*PP_get_descr)(PP_file *fileinfo, PyObject *obj)

PP_descr *PP_make_descr(
    PP_types      typecode,
    char         *type,
    long          bpi
    )
PP_type_map *PP_make_type_entry(
    PP_types       typecode,
    int            sequence,
    PP_descr      *descr,
    PyTypeObject  *ob_type,
    PP_pack_func   pack,
    PP_unpack_func unpack,
    PP_get_descr   get_descr
    )
void PP_register_type(PP_file *fileinfo, PP_type_entry *entry)
void PP_register_object(PP_file *fileinfo, PP_type_entry *entry)
</pre>

<!-- ------------------------------------------------------------>
<a name="pypact_developer"><h2>Developer Notes</h2></a>

This section focuses on areas that developers using PyPact 
and developers of PyPact might be interested in.

<a name="pypact_gen"><h3>Generating Source</h3></a>

Much of the source code for PyPact is generated using the
modulator tool from Basis.
This tool generates the boiler plate that the Python API
requires to connect functions and data structures together into
an extension module.
This tool generates new style classes.
The input consists of an IDL file (interface definition file).

It also reads any previously generated code to preserve changes made
to certain sections of the generated source.
These sections are contained between 
<tt>DO-NOT-DELETE splicer.begin</tt> and
<tt>DO-NOT-DELETE splicer.end</tt> comments.
<p>
Each generated file starts with the comment
<tt>This is generated code</tt>.
<p>

<a name="pypact_install"><h3>Installation</h3></a>

PyPact will be installed by dsys if shared libraries are defined.
PACT's autoconf/automake system will also build the modules.
Finally, a <tt>setup.py</tt> script is provided to build the module.

<p>
The default method of building the extension will load the PACT libraries
into the extension.  This allows things to work as expected when
importing PyPact from the Python executable.
If, instead, Python is imbedded into an application which already has
the PACT libraries loaded, then importing PyPact will result in two
copies of PACT being in memory.  This is not a Good Thing.
In this case, the application developers will have to rebuild
PyPact without loading the PACT libraries.  One way to accomplish this is by
editing the <tt>setup.py</tt> script to remove the <tt>libraries</tt>
argument from the <tt>Extension</tt> constructor.
<p>

<a name="pypact_user_defined"><h3>User Defined Classes</h3></a>

TODO

<!-- How to get user defined C or C++ types to read/write properly -->

<br><br>
<hr>
For questions and comments, 
please contact the <A HREF="pactteam.html">PACT Development Team</A>.

