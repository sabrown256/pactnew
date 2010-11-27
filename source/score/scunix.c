/*
 * SCUNIX.C - UNIX specific routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
#include "score_int.h" 

extern void
 _SC_posix_process(PROCESS *pp);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PATH_DELIMITER_UNIX - copy the PATH delimiter, ';' to DELIM */

static void _SC_path_delimiter_unix(char *delim)
   {
    
    strcpy(delim, ":");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FILE_LENGTH_UNIX - return the length of the file PATH */

static BIGINT _SC_file_length_unix(char *path)
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

static int _SC_query_mode_unix(char *name, char *mode)
   {int ret;
    struct stat sb;

    ret = FALSE;

    if (stat(name, &sb) == 0)

/* if not a regular file forget it (what about other types: S_IFLNK,
 * S_IFSOCK, S_IFIFO, S_IFBLK, or S_IFCHR?)
 */
       {if (sb.st_mode & S_IFREG)
	   {if (mode == NULL)
	       ret = sb.st_mode & S_IREAD;
	    else if ((strcmp(mode, "r") == 0) || (strcmp(mode, "rb") == 0))
	       ret = sb.st_mode & S_IREAD;
	    else
	       ret = sb.st_mode & S_IWRITE;

	    if (ret != 0)
	       ret = TRUE;};};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_QUERY_EXEC_UNIX - return TRUE if the file PATH is executable */

static int _SC_query_exec_unix(char *path)
   {int muid, mgid, fuid, fgid, only, ret;
    int usrx, grpx, othx, file;
    int uex, gex, oex;
    struct stat bf;

    ret  = FALSE;
    only = FALSE;
    muid = getuid();
    if (only == TRUE)
       mgid = getgid();

    if (stat(path, &bf) == 0)
       {int ig, ng;
	gid_t gl[NGROUPS_MAX+1];

	fuid = bf.st_uid;
	fgid = bf.st_gid;
	file = bf.st_mode & S_IFREG;

	uex = ((bf.st_mode & S_IXUSR) != 0);
	gex = ((bf.st_mode & S_IXGRP) != 0);
	oex = ((bf.st_mode & S_IXOTH) != 0);

/* determine whether we have user permissions */
	usrx = ((muid == fuid) && (uex == TRUE));

/* determine whether we have group permissions */
	if (only)
	   grpx = ((mgid == fgid) && (gex == TRUE));
	else
	   {ng   = getgroups(NGROUPS_MAX, gl);
	    grpx = 0;
	    for (ig = 0; (grpx == 0) && (ig < ng); ig++)
	        grpx |= (gl[ig] == fgid);
	    grpx &= (gex == TRUE);};

/* determine whether we have world permissions */
	othx = oex;
	ret  = ((file && (usrx || grpx || othx)) != 0);};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

SC_oscapdes
  _SC_os = { _SC_path_delimiter_unix,
	     _SC_file_length_unix,
	     _SC_query_mode_unix,
	     _SC_query_exec_unix,
	     _SC_posix_process };


