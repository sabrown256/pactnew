TXT: PPC User's Manual
MOD: 09/15/2014

<CENTER>
<H1><font color="#ff0000">$TXT</font></H1>
<H2>Portable Process Control library</H2>
<H4>Stewart A. Brown
<BR>Kelly A. Barrett</H4>
</CENTER>

<BLOCKQUOTE>

<HR>

<ul>
<li><a href="#PPCintro">Introduction</a>

<li><a href="#PPCmodel">PPC Model</a>
<ul>
<li><a href="#PPCpipes">Pipes, Sockets, and Pseudo Terminals</a>
<li><a href="#PPCintrpt">Interrupt Driven and Multiplexed I/O</a>
<li><a href="#PPCfile">Remote File Access</a>
<ul>
<li><a href="#PPCio">File I/O Interface</a>
<li><a href="#PPCaccess">File Access Server</a>      
</ul>
<li><a href="#PPCcompath">Communication Paths and PPC Routines</a>
</ul>

<li><a href="#PPClib">The PPC Library</a>
<ul>
<li><a href="#PPCapi">The PPC API</a>
<li><a href="#PPCcons"> PPC Constants</a>
<li><a href="#PPCcompiling"> Compiling and Loading</a>
<li><a href="#PPCdata"> Data Structures for PPC</a>
<li><a href="#PPCexample"> Example</a>
</ul>

<li><a href="#PPCpcexec"> PCEXEC</a>
<ul>
<li><a href="#PPCusage"> Usage</a>
</ul>

<li><a href="#PPCdocs"> Related Documentation</a>
</ul>

<HR>

<h2 ALIGN="CENTER">Acknowledgements</h2>

A special acknowledgement is due to Lee Minner who helped with the socket and remote execution facility in PPC.<p>

<HR>

<a name="PPCintro"></a>
<h2 ALIGN="CENTER">Introduction</h2>

The Portable Process Control library (PPC) is a set of routines to execute and
communicate with other processes in environments which permit such operations.
The goal is to establish an easy to use interface which can be implemented on a
wide variety of platforms. The standard C library I/O interface was taken as a
model. In this view, processes are treated as much like files as is possible.
In particular, a process is opened and closed as is a file and that means that
it is forked by the parent to start with and killed at the end. While a process
is open it may be written to and read from with PPC functions which are analogous
to the standard C I/O functions fprintf and fgets. In addition, there are functions
to monitor the status of the child processes. These do not have precise analogs to
file routines, but since the basic interface has been established by the open/close
and read/write routines, these have obvious usages.<p>

PPC is one part of PACT, Portable Application Code Toolkit. See the section
Other PACT Documentation at the end for more information about PACT.<p>

<a name="PPCmodel"></a>
<h2 ALIGN="CENTER">PPC Model</h2>

This section describes the interprocess communication (IPC) model used in PPC and
gives a high level overview of some of the key features of the library. Much of
this discussion is involved with UNIX since that environment is one of the richest
(and hence most confusing) environments for IPC. We believe that the concepts are
fundamental and hence permit PPC to be ported to environments with different
semantics. Also the prevalence of the TCP/IP standard makes it a reasonable one to
study in the context of distributed IPC.<p>

<a name="PPCpipes"></a>
<h3>Pipes, Sockets, and Pseudo Terminals</h3>

In UNIX environments there are three ways for processes to communicate that seem
suitable for PPC. They are pipes, sockets, and pseudo terminals (PTY&#146;s).
Although one might argue that for local processes there is no real advantage to
using pipes over sockets or vice versa, PPC supports them both to guard against
platforms where one or the other has some non-standard feature. By contrast,
PTY&#146;s have a crucial significance in that some programs with which a code
may wish to communicate behave differently when talking to a terminal than they do
when talking to a socket or pipe. FTP is a classic example of this phenomenon.
Only sockets are available for remote processes.<p>

When an application opens a child process it must specify the IPC medium. In this
way the application developer has control over how the parent and child communicate.<p>

<a name="PPCintrpt"></a>
<h3>Interrupt Driven and Multiplexed I/O</h3>

When communicating with one or more child processes, an application is often in
the position of having or wanting to get input from the terminal or one or more
child processes. There are three ways that are commonly used to do this:<p>

<BLOCKQUOTE>
unblocking the input channels and polling in the application
<P>using a multiplex I/O system call (e.g. select or poll)
<P>using interrupts on input channels
</BLOCKQUOTE>

PPC supports all three of these options.<p>

<a name="PPCfile"></a>
<h3> Remote File Access</h3>

Sometimes applications need to access files on remote systems which cannot be
mounted via some standard mechanism such as NFS or AFS. Using the IPC machinery
inherent in its design, PPC also supplies a facility to do I/O on remote files.
The model here is in two parts.<p>

<a name="PPCio"></a>
<b>File I/O Interface</b>

These give a call compatible interface to the major portion of the standard
C file I/O library. It also provides a simple way for an application to supply
its own functions to make variations on the functionality. In particular, PPC
supplies a set of functions to access files on remote hosts. The function
SC_io_connect toggles between the default set of functions and the remote
access versions.<p>

<a name="PPCaccess"></a>
<b> File Access Server</b>

The remote file access functions mentioned in the last section depend on a server
running on the remote host. The utililty pcexec is that server.<p>

When a request to open a file with a name using the syntax discussed in the section
PCEXEC indicates a file on a remote host, pcexec is started on that host in server
mode. It then handles all file access requests made by the other calls in the previous
section. Any number of files can be managed per host per user. When the last file that
the remote pcexec session knows about is closed, pcexec exits. NOTE: if your
application crashes, there will most likely be an orphaned pcexec running on the
remote host and you will have to kill it yourself.<p>

<a name="PPCcompath"></a>
<h3>Communication Paths and PPC Routines</h3>

Use PPC routines PC_printf and PC_gets to communicate with child processes.
Use C routines fgets and fprintf to communicate with parent processes.

<img src="ppc1.gif"><p>

<BR>

<a name="PPClib"></a>
<h2 ALIGN="CENTER"> The PPC Library</h2>

This section describes the PPC library and related information necessary to use
PPC in your applications.<p>

<a name="PPCapi"></a>
<h3>The PPC API</h3>

These routines form the interface between applications programs and the low level
operating system dependent process control and communication calls. As such they may
lack some of the flexibility which the low level routines afford, but they are much
easier to use and for most applications they do everything that is necessary.<p>

Most of these routines put an error message into a global variable called PC_err.
The message contains the name of the function in which the error occurred thus
eliminating the need for a cross reference document on error codes. In this way
applications programs can check for error conditions themselves and decide in what
manner to use the PPC error messages instead of having error messages printed by the
system routines. Error messages are not stacked and must be processed by the
application before any other PPC calls are made in order to avoid potential
overwrites.

<P>
<BR><B>int PC_block(PROCESS *pp)
</B><BR>
<DL><DD>Set the PROCESS pp to be blocked (wait for messages).<p>
Input:	pp, a pointer to a PROCESS.
<P>Output:	TRUE, if successful, FALSE otherwise.
</DL>

<P>
<BR><B>int PC_block_fd(int fd)
</B><BR>
<DL><DD>Set the file descriptor to be blocked (wait for messages).<p>
Input:	fd, an integer file descriptor.
<P>Output:	TRUE, if successful, FALSE otherwise.
</DL>

<P>
<BR><B>int PC_block_file(FILE *fp)
</B><BR>
<DL><DD>Set the FILE to be blocked (wait for messages).<p>
Input:	fp, a pointer to a FILE.
<P>Output:	TRUE, if successful, FALSE otherwise.
</DL>

<P>
<BR><B>int PC_close(PROCESS *pp)
</B><BR>
<DL><DD>Kill the process specified by pp. This is used to terminate and remove
a process when it is not needed (even if the executable process has terminated).<p>
Input:	pp, a pointer to a PROCESS.
<P>Output:	TRUE, if successful, FALSE otherwise.
</DL>

<P>
<BR><B>int PC_echo_off_fd(int fd)
</B><BR>
<DL><DD>Set the file descriptor input to be unechoed.<p>
Input:	fd, an integer file descriptor.
<P>Output:	TRUE, if successful, FALSE otherwise.
</DL>

<P>
<BR><B>int PC_echo_off_file(FILE *fp)
</B><BR>
<DL><DD>Set the FILE input to be unechoed.<p>
Input:	fp, a pointer to a FILE.
<P>Output:	TRUE, if successful, FALSE otherwise.
</DL>

<P>
<BR><B>int PC_echo_on_fd(int fd)
</B><BR>
<DL><DD>Set the file descriptor input to be echoed.<p>
Input:	fd, an integer file descriptor.
<P>Output:	TRUE, if successful, FALSE otherwise.
</DL>

<P>
<BR><B>int PC_echo_on_file(FILE *fp)
</B><BR>
<DL><DD>Set the FILE input to be echoed.<p>
Input:	fp, a pointer to a FILE.
<P>Output:	TRUE, if successful, FALSE otherwise.
</DL>

<P>
<BR><B>int PC_flush(PROCESS *pp)
</B><BR>
<DL><DD>Flush the input and output streams for the given process.<p>
Input:	pp, a pointer to a PROCESS.
<P>Output:	TRUE, if successful, FALSE otherwise.
</DL>

<P>
<BR><B>int PC_gets(char *bf, int len, PROCESS *pp)
</B><BR>
<DL><DD>Read a string from a process into the buffer provided. This behaves
exactly like fgets except that if there is no input available, NULL is
returned without waiting.
<TABLE>
<TR><TD>Input:	</TD><TD>bf   </TD><TD>a pointer to an ASCII string.</TD></TR>
<TR><TD>        </TD><TD>len  </TD><TD>an integer length of the buffer bf.</TD></TR>
<TR><TD>        </TD><TD>pp   </TD><TD>a pointer to the PROCESS from which to
read the input.</TD></TR>
</TABLE>
<P>Output:	a pointer to bf if successful or NULL if nothing is available to be read.
</DL>

<P>
<BR><B>int PC_io_callback_fd(int fd, PFVoid fnc)
</B><BR>
<DL><DD>Change the state of the specified file descriptor so that the specified
function will be called when there is input available (interrupt driven or
multiplexed).
<TABLE>
<TR><TD>Input:	</TD><TD>fd   </TD><TD>an integer file descriptor.</TD></TR>
<TR><TD>        </TD><TD>fnc  </TD><TD>a pointer to a function returning nothing
which will handle the input.</TD></TR>
</TABLE>
<P>Output:	TRUE, if successful, FALSE otherwise.
</DL>

<P>
<BR><B>int PC_io_callback_file(FILE *fp, PFVoid fnc)
</B><BR>
<DL><DD>Change the state of the specified FILE so that the specified function
will be called when there is input available (interrupt driven or multiplexed).
<TABLE>
<TR><TD>Input:	</TD><TD>fp   </TD><TD>a pointer to a FILE.</TD></TR>
<TR><TD>        </TD><TD>fnc  </TD><TD>a pointer to a function returning nothing
which will handle the input.</TD></TR>
</TABLE>
<P>Output:	TRUE, if successful, FALSE otherwise.
</DL>

<P>
<BR><B>int SC_io_connect(int flag)
</B><BR>
<DL><DD>If flag is PC_REMOTE set the I/O hooks (see File I/O Interface section)
to the functions for remote file access and if flag is PC_LOCAL set them to the
standard C library calls for local access.<p>
Input:  flag, an integer flag.
<P>Output:  TRUE, if successful, FALSE otherwise.
</DL>

<P>
<BR><B>PROCESS *SC_open(char **argv, char **envp, char *mode, ...)
</B><BR>
Execute a process with command line arguments from argv in some
(as yet unspecified) mode. The arguments are handled exactly like those of C
programs where argv[0] is the name of the process/program and the remaining
entries in argv are null terminated strings each corresponding to a command
line argument.<p>
The legal modes are expressed similarly to those for the standard C fopen call:<p>
<CENTER> ( &#146;r&#146; | &#146;w&#146; | &#146;a&#146;)[&#146;p&#146; 
| &#146;s&#146; | &#146;t&#146; | &#146;T&#146;][&#146;b&#146;][&#146;o&#146;][&#146;v&#146;]
</CENTER>

<P>
where

<BLOCKQUOTE>
<TABLE>
<TR><TD WIDTH="40">r	</TD><TD>read only (child stdout only connected to parent)</TD></TR>
<TR><TD>w	</TD><TD>write only (child stdin only connected to parent)</TD></TR>
<TR><TD>a	</TD><TD>append or read/write (child stdin and stdout connnected to parent)</TD></TR>
<TR><TD>p	</TD><TD>communicate via pipe</TD></TR>
<TR><TD>s	</TD><TD>communicate via socket</TD></TR>
<TR><TD>t	</TD><TD>communicate via PTY (preserve original terminal state)</TD></TR>
<TR><TD>T	</TD><TD>communicate via PTY (do not preserve original terminal state)</TD></TR>
<TR><TD>b	</TD><TD>binary data exchanged</TD></TR>
<TR><TD>o	</TD><TD>set OMP_NUM_THREADS to 1 in the parent</TD></TR>
<TR><TD>v	</TD><TD>process variable argument list of key, value pairs</TD></TR>
</TABLE>
</BLOCKQUOTE>

The legal keys for the 'v' mode are:<p>

<pre>
 "INIT"             Use supplied function to create PROCESS structs
                    for the parent and child.  The value is a function
                    pointer with prototype:
                       PROCESS *(f*)(char **argv, char *mode, int type)
                    Do not use this unless you really, really understand
                    the internals - the default initializer is just fine.

 "OPEN-RETRY-TIME"  The time in milliseconds to wait before retrying to
                    open files for redirection.  This helps with NFS
                    file systems under load.  The value is an int.

 "RING-EXP"         The power of 2 used to size the ring buffer between
                    parent and child I/O streams. The default is 17 for
                    a buffer size of 128 KBytes.  The value is an int.

 "EXIT"             Call supplied function with supplied argument when
                    process exits.  This takes two values.  The first
                    if a function pointer with prototype:
                       void (*f)(PROCESS *pp, void *a)
                    The second is a 'void *' pointer which is passed to
                    the function.  The default is NULL and NULL.
 "STDIN"            Use the file specified for stdin of the child process.
                    This takes two values: the name of the file to
                    open and the mode flags as used by the 'open' system
                    call.  The name is a 'char *' and the mode flags is
                    an int.
 "STDOUT"           Use the file specified for stdout of the child process.
                    This takes two values: the name of the file to
                    open and the mode flags as used by the 'open' system
                    call.  The name is a 'char *' and the mode flags is
                    an int.
 "STDERR"           Use the file specified for stderr of the child process.
                    This takes two values: the name of the file to
                    open and the mode flags as used by the 'open' system
                    call.  The name is a 'char *' and the mode flags is
                    an int.
 LAST               A predefined pointer value used to indicate the end
                    of the arguments to the 'v' option.  This is required
                    and is not a string.
</pre>
<p>

For example, for bidirectional communication with a local child via a pseudo TTY in plain ASCII mode use "at". At this point read only and write only are not fully implemented.<p>
The default is "as" for remote processes and "ap" for local processes.
<P>

<DL><DD>
<TABLE>
<TR><TD>Input:	</TD><TD>argv   </TD><TD>an array of pointers to the arguments.</TD></TR>
<TR><TD>        </TD><TD>envp   </TD><TD>an array of pointers to the environment
strings.</TD></TR>
<TR><TD>        </TD><TD>mode   </TD><TD>an ASCII string indicating the IPC mode.</TD></TR>
</TABLE>
Output:	a pointer to a PROCESS.
</DL>

<P>
<BR><B>int PC_printf(PROCESS *pp, char *fmt, ...)
</B><BR>
<DL><DD>Write the arguments to the PROCESS pp according to the format fmt.
<TABLE>
<TR><TD>Input:	</TD><TD>pp   </TD><TD>a pointer to a PROCESS.</TD></TR>
<TR><TD>  </TD><TD>fmt  </TD><TD>an ASCII string which specifies the output format.</TD></TR>
<TR><TD>  </TD><TD>...  </TD><TD>the arguments specified in the format.</TD></TR>
</TABLE>
Output:	TRUE, if successful and FALSE otherwise.
</DL>

<P>
<BR><B>int PC_read(void *bf, char *type, size_t ni, PROCESS *pp)
</B><BR>
<DL><DD>Do a binary read of ni items of type type from the PROCESS pp into the buffer bf.<p>
<TABLE>
<TR><TD>Input:	</TD><TD>bf  </TD><TD>a pointer to memory into which the data is to
be read.</TD></TR>
<TR><TD>  </TD><TD>type  </TD><TD>an ASCII string which specifies the data type of items to be read.</TD></TR>
<TR><TD>  </TD><TD>ni    </TD><TD>an integer (size_t) number of items to be read.</TD></TR>
<TR><TD>  </TD><TD>pp    </TD><TD>a pointer to a PROCESS.</TD></TR>
</TABLE>
Output:	The number of items successfully read.
</DL>

<P>
<BR><B>int PC_set_attr(PROCESS *pp, int i, int state)
</B><BR>
<DL><DD>Set the status flags for the specified PROCESS. The flags which can be set are:<p>
<BLOCKQUOTE>
<TABLE>
<TR><TD>PC_LINE	</TD><TD>line at a time input</TD></TR>
<TR><TD>PC_NDELAY	</TD><TD>non-blocking I/O</TD></TR>
<TR><TD>PC_APPEND	</TD><TD>append (writes guaranteed at the end)</TD></TR>
<TR><TD>PC_SYNC		</TD><TD>synchronous write option</TD></TR>
<TR><TD>PC_ASYNC	</TD><TD>interrupt-driven I/O for sockets</TD></TR>
</TABLE>
</BLOCKQUOTE>
<P>
<TABLE>
<TR><TD>Input:	</TD><TD>pp   </TD><TD>a pointer to a PROCESS.</TD></TR>
<TR><TD>  </TD><TD>i  </TD><TD>an integer value containing a bit pattern indicating
attribute settings.</TD></TR>
<TR><TD>  </TD><TD>state   </TD><TD>an integer value indicating to set or reset.</TD></TR>
</TABLE>
Output:	i if successful and -1 otherwise.
</DL>

<P>
<BR><B>int PC_set_fd_attr(int fd, int i, int state)
</B><BR>
<DL><DD>Set the file status flags for a specified file descriptor.
The flag which can be set are:<p>
<BLOCKQUOTE>
<TABLE>
<TR><TD>PC_LINE </TD><TD>line at a time input</TD></TR>
<TR><TD>PC_NDELAY       </TD><TD>non-blocking I/O</TD></TR>
<TR><TD>PC_APPEND       </TD><TD>append (writes guaranteed at the end)</TD></TR>
<TR><TD>PC_SYNC         </TD><TD>synchronous write option</TD></TR>
<TR><TD>PC_ASYNC        </TD><TD>interrupt-driven I/O for sockets</TD></TR>
</TABLE>
</BLOCKQUOTE>
<P>
<TABLE>
<TR><TD>Input:  </TD><TD>fd   </TD><TD>an integer file descriptor.</TD></TR>
<TR><TD>  </TD><TD>i  </TD><TD>an integer value containing a bit pattern indicating
attribute settings.</TD></TR>
<TR><TD>  </TD><TD>state   </TD><TD>an integer value indicating to set or reset.</TD></TR>
</TABLE>
Output: i if successful and -1 otherwise.
</DL>


<P>
<BR><B>void PC_signal_handler(int signo)
</B><BR>
<DL><DD>On receipt of a signal that a child process status has changed, loop
forever asking about children with changed status until the system says there
are no more.<p>
Input:	signo, an integer signal number.
<P>Output:	None.
</DL>

<P>
<BR><B>int PC_status(PROCESS *pp)
</B><BR>
<DL><DD>Return the execution status of the PROCESS pp.<p>
Input:	pp, a pointer to a PROCESS.
<P>Output:	an integer value, one of RUNNING, STOPPED, EXITED, COREDUMPED, 
or SIGNALED.
</DL>

<P><BR><B>int PC_unblock(PROCESS *pp)
</B><BR>
<DL><DD>Set the PROCESS to be unblocked (do not wait for messages).<p>
Input:	pp, a pointer to a PROCESS.
<P>Output:	TRUE, if successful, FALSE otherwise.
</DL>

<P><BR><B>int PC_unblock_fd(int fd)
</B><BR>
<DL><DD>Set the file descriptor to be unblocked (do not wait for messages).<p>
Input:	fd, an integer file descriptor.
<P>Output:	TRUE, if successful, FALSE otherwise.
</DL>

<P><BR><B>int PC_unblock_file(FILE *fp)
</B><BR>
<DL><DD>Set the FILE to be unblocked (do not wait for messages).<p>
Input:	fp, a pointer to a FILE.
<P>Output:	TRUE, if successful, FALSE otherwise.
</DL>

<P>
<BR><B>int PC_write(void *bf, char *type, size_t ni, PROCESS *pp)
</B><BR>
Do a binary write of ni items of type type to the PROCESS pp from the buffer bf.<p>
<TABLE>
<TR><TD>Input:	</TD><TD>bf  </TD><TD>a pointer to memory containing the data
to be written.</TD></TR>
<TR><TD> </TD><TD>type </TD><TD>an ASCII string which specifies the data type
of items to be written.</TD></TR>
<TR><TD> </TD><TD>ni   </TD><TD>an integer (size_t) number of items to be written.</TD></TR>
<TR><TD> </TD><TD>pp  </TD><TD>a pointer to a PROCESS.</TD></TR>
</TABLE>
Output:	The number of items successfully written.



<P>
<a name="PPCcons"></a>
<h3> PPC Constants</h3>

The following #define&#146;d constants should be used in the contexts indicated:<p>


<BLOCKQUOTE>
<TABLE>
<TR><TD>RUNNING		</TD>
<TD>0	</TD><TD>return value of PC_status indicating process running</TD></TR>
<TR><TD>STOPPED		</TD>
<TD>1	</TD><TD>return value of PC_status indicating process stopped</TD></TR>
<TR><TD>CHANGED		</TD>
<TD>2	</TD><TD>return value of PC_status indicating process status changed</TD></TR>
<TR><TD>EXITED		</TD>
<TD>4	</TD><TD>return value of PC_status indicating process exited</TD></TR>
<TR><TD>COREDUMPED	</TD>
<TD>8	</TD><TD>return value of PC_status indicating process crashed</TD></TR>
<TR><TD>SIGNALED	</TD>
<TD>16	</TD><TD>return value of PC_status indicating process signalled</TD></TR>
<TD><BR></TD><TD></TD></TR>
<TR><TD>PC_LOCAL	</TD>
<TD>102	</TD><TD>value indicating process or file on current CPU</TD></TR>
<TR><TD>PC_REMOTE	</TD>
<TD>103	</TD><TD>value indicating process or file on remote host</TD></TR>
<TD><BR></TD><TD></TD></TR>
<TR><TD>USE_PTYS	</TD>
<TD>50	</TD><TD>value indicating IPC medium is a pseudo terminal</TD></TR>
<TR><TD>USE_SOCKETS	</TD>
<TD>51	</TD><TD>value indicating IPC medium is a socket</TD></TR>
<TR><TD>USE_PIPES	</TD>
<TD>52	</TD><TD>value indicating IPC medium is a pipe</TD></TR>
<TD><BR></TD><TD></TD></TR>
<TR><TD>PC_NDELAY	</TD>
<TD></TD><TD>used with PC_set_attr to set non-blocking reads</TD></TR>
<TR><TD>PC_APPEND	</TD>
<TD></TD><TD>used with PC_set_attr to specify writes at end</TD></TR>
<TR><TD>PC_SYNC		</TD>
<TD></TD><TD>used with PC_set_attr to specify synchronous writes</TD></TR>
<TR><TD>PC_LINE		</TD>
<TD></TD><TD>used with PC_set_attr to specify line-at-a-time input</TD></TR>
</TABLE>
</BLOCKQUOTE>


<P>
<a name="PPCcompiling"></a>
<h3> Compiling and Loading</h3>

To compile your C programs you must use the following<p>

<BLOCKQUOTE>
#include &lt;ppc.h&gt;
</BLOCKQUOTE>

in the source files which deal with the library routines.<p>

To link your application you must use the following libraries in the order specified.<p>

<BLOCKQUOTE>
<B>
-lppc -lpdb -lpml -lscore [ -lm ...]
</B>
</BLOCKQUOTE>

<P>
Although this is expressed as if for a UNIX linker, the order would be the same for
any system with a single pass linker. The items in [] are optional or system
dependent.<p>

Each system has different naming conventions for its libraries and the reader is
assumed to understand the appropriate naming conventions as well as knowing how to
tell the linker to find the installed PACT libraries on each system that they use.<p>

<a name="PPCdata"></a>
<h3> Data Structures for PPC</h3>

<B>PROCESS
</B><BR>
The data structure which underlies PPC is the PROCESS. It is analogous in
purpose to the FILE structure used for file I/O. PROCESS structures contain
the information necessary for PPC routines to monitor and communicate with
child processes. They are passed to PPC routines the way the FILE structure
is passed in the standard C file I/O routines.<p>

<a name="PPCexample"></a>
<h3> Example</h3>

The following example is a basic test of PPC in which a small polling loop
gets messages from the controlling terminal and passes them to a child process
while polling the child process for messages and sending them to the controlling
terminal. This program should be entirely transparent to the application.<p>

<p>

<BLOCKQUOTE>
<PRE> #include "ppc.h"

 main(argc, argv, envp)
    int argc;
    char **argv, **envp;

    {PROCESS *pp;
     char s[BIGLINE];

 /* open the process */

     if ((pp = SC_open(argv+1, envp, "w")) == NULL)
        {printf("\nFailed to open: %s\n\n", argv[1]);
         exit(1);};

     printf("\nRunning process: %s\n\n", argv[1]);

 /* unblock stdin and turn stdout buffering off */

     PC_unblock_file(stdin);
     setbuf(stdout, NULL);

     while (TRUE)
        {PC_err[0] = '\0';
         while (PC_gets(s, BIGLINE, pp) != NULL)
             printf("%s", s);

 /* check the status of the process */

         if (PC_status(pp) != RUNNING)
            {printf("\nProcess %s terminated (%d %d)\n\n",
                    argv[1], pp->status, pp->reason);
             break;};

 /* get any messages from tty, if available */

         if (fgets(s, BIGLINE, stdin) != NULL)
            PC_printf(pp, "%s", s);

         if (PC_err[0] != '\0')
            {printf("\nERROR: %s\n\n", PC_err);
             break;};};

 /* close the process */

     PC_close(pp);

     printf("\nProcess test %s ended\n\n", argv[1]);

 /* turn on blocking for stdin (very important) */

     PC_block_file(stdin);

     exit(0);}
</PRE>
</BLOCKQUOTE>


<a name="PPCpcexec"></a>
<h2 ALIGN="CENTER"> PCEXEC</h2>

PPC includes an application program called pcexec. It began as a test code
for PPC but it has expanded to the point where it has utility in its own right.
In fact, pcexec does two jobs. First, it can be used to run other programs in
any of the modes discussed in the section The PPC Model. Second, it acts as a
file access server for the remote file access capability in PACT.<p>

<a name="PPCusage"></a>
<h3> Usage</h3>

Run prog as a child process:<p>

<BLOCKQUOTE>
pcexec [-p | -s | -t] [-i] [-q] prog [arg1 ...]
</BLOCKQUOTE>

Act as a file access server on host (triggered by -f):<p>

<BLOCKQUOTE>
pcexec -f [-l] host
</BLOCKQUOTE>

The forms for prog are:<p>

<BLOCKQUOTE>
<TABLE>
<TR><TD>name		</TD><TD>run name on local host</TD></TR>
<TR><TD>host:name	</TD><TD>run name on remote host</TD></TR>
<TR><TD>CPU@name	</TD><TD>run name on processor CPU</TD></TR>
<TR><TD>host:CPU@name	</TD><TD>run name on processor CPU on remote host</TD></TR>
</TABLE>
</BLOCKQUOTE>

The last two are not yet completed.
<p>


The full syntax for host is one of the following:<p>

<BLOCKQUOTE>
hostname
<BR>hostname,username
<BR>hostname,username,passwd
<P>
NOTE: whitespace is NOT allowed<p>
</BLOCKQUOTE>

Options:<p>

<BLOCKQUOTE>
<TABLE>
<TR><TD>i  </TD><TD>Poll explicitly instead of using system call</TD></TR>
<TR><TD>l  </TD><TD>When acting as file server, log transactions to log file in your home directory</TD></TR>
<TR><TD>q  </TD><TD>Print only messages from the child</TD></TR>
<TR><TD>p  </TD><TD>Use pipes for communications (default for local processes)</TD></TR>
<TR><TD>s  </TD><TD>Use sockets for communications (only mode for remote processes)</TD></TR>
<TR><TD>t  </TD><TD>Use pseudo terminals for communications</TD></TR>
</TABLE>
</BLOCKQUOTE>

<P>
All three modes (pipes, sockets, pseudo terminals) are available for
local processes.<p>

<br>
<a name="PPCdocs"></a>
<h2 ALIGN="CENTER"> Related Documentation</h2>


PPC uses functions from the PACT SCORE and PDB libraries. The interested
reader is referred to the other PACT documentation:<p>


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

