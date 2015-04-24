/*
 * SCUNIX.C - UNIX specific routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
#include "score_int.h" 

#if defined(UNIX)

extern void
 _SC_posix_process(PROCESS *pp);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PATH_DELIMITER_UNIX - copy the PATH delimiter, ';' to DELIM */

static void _SC_path_delimiter_unix(char *delim)
   {
    
    SC_strncpy(delim, 2, ":", -1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FILE_LENGTH_UNIX - return the length of the file PATH */

static int64_t _SC_file_length_unix(const char *path)
   {int st;
    int64_t ln;
    struct stat sb;

    ln = 0;

    st = stat(path, &sb);
    if (st == 0)
       ln = sb.st_size;

    return(ln);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_QUERY_MODE_UNIX - return TRUE if the file NAME can be opened
 *                     - in MODE
 */

static int _SC_query_mode_unix(const char *name, const char *mode)
   {int ret;
    struct stat sb;

    ret = FALSE;

    if ((name != NULL) && (stat(name, &sb) == 0))

/* if not a regular file forget it (what about other types: S_IFLNK,
 * S_IFSOCK, S_IFIFO, S_IFBLK, or S_IFCHR?)
 */
       {if (sb.st_mode & S_IFREG)
	   {if (mode == NULL)
	       ret = sb.st_mode & S_IRUSR;
	    else if ((strcmp(mode, "r") == 0) || (strcmp(mode, "rb") == 0))
	       ret = sb.st_mode & S_IRUSR;
	    else
	       ret = sb.st_mode & S_IWUSR;

	    if (ret != 0)
	       ret = TRUE;};};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_QUERY_EXEC_UNIX - return TRUE if the file PATH is executable */

static int _SC_query_exec_unix(const char *path)
   {int ret;

    ret = PS_file_kind(S_IFREG, 0111, path);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* system capability API - PACT level */

SC_oscapdes
  _SC_os = { _SC_path_delimiter_unix,
	     _SC_file_length_unix,
	     _SC_query_mode_unix,
	     _SC_query_exec_unix,
	     _SC_posix_process };

#if 0
/* system call API - POSIX level compliant */

syscall_api
  _SC_osapi = { getpwuid, getuid, getgid, 
		getpid, getppid, getpgrp, tcgetpgrp, 
		setsid, fork, waitpid, kill, sched_yield,
		fsync,
		ioctl,         /* Solaris differs in protoype */
		fcntl, 
		poll, nanosleep, 
		setenv, unsetenv, 
		gethostname,   /* Solaris differs in protoype */
		ttyname,
		srandom,       /* FreeBSD differs in protoype */
		random };
#endif

#endif
