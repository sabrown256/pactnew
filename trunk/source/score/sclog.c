/*
 * SCLOG.C - score logging routines 
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"
#include "scope_proc.h"

#define TMP_DIR          "/tmp"
#define LOGNAME_PREFIX   "pact-log"
#define HOSTNAME_MAX     64

SC_thread_lock
 _SC_log_lock = SC_LOCK_INIT_STATE;

/*--------------------------------------------------------------------------*/

/*                         PUBLIC INTERFACE                                 */

/*--------------------------------------------------------------------------*/

/* SC_LOG - Log the given args using format to the tmp log file, 
 *          prefixed with a PID, entry number, and timestamp.
 *
 *          It is assumed that each file descriptor to a log is unique, 
 *          hence no locking mechanisms are used within this function.
 *
 *          Returns -1 on error, otherwise 0.
 */

int SC_log(SC_logfile lg, char *format, ...)
   {int status;
    va_list p_arg;
    char timestamp[MAXLINE];
    char *pt;
    time_t curr_time;

    status = 0;

/* grab a string version of the current time */
    time(&curr_time);
    pt = SC_ctime(curr_time, timestamp, MAXLINE);
    if (pt != NULL)

/* remove that pesky terminating newline char */
       {SC_LAST_CHAR(pt) = '\0';

/* log to the tmp file */
	fprintf(lg.file, "[%d:%d | %s]: ", (int)(lg.pid), lg.entry, pt);

	va_start(p_arg, format);
	vfprintf(lg.file, format, p_arg);
	va_end(p_arg);

	fprintf(lg.file, "\n");};

    return(status);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_OPEN_LOG - Open a tmp log file in a tmp dir, write out some initial
 *               information, and return the handle to the user.
 *
 *               A different log file is opened for each SC_open_log request,
 *               hence locks are used to ensure that within a process on a host,
 *               threaded or not, each new log opened has a different entry num.
 *               Each log file is automatically unlinked when SC_close_log 
 *               is called, after its contents are appended to the global log.
 *            
 *               Returns NULL on error, otherwise an SC_logfile.
 */

SC_logfile SC_open_log(void)
   {char hostnm[HOSTNAME_MAX], unm[MAXLINE];
    char *uf;
    SC_logfile lg;

    lg.file  = NULL;
    lg.entry = 0;

/* get a unique name for this log file: prefix-hostname-pid-num */
    lg.pid = getpid();
    gethostname(hostnm, HOSTNAME_MAX);

/* make sure everyone who gets a tmp log file, gets their own unique file
 * so we do not have to worry about locking during SC_log() calls
 */
    SC_LOCKON(_SC_log_lock);

    uf = SC_dsnprintf(FALSE, "%s/%s-%s-%d-%d",
		      TMP_DIR, LOGNAME_PREFIX, hostnm, (int) lg.pid,
		      _SC.nlog);
  
/* truncate this file and open it for writing/reading */ 
    lg.file = SC_fopen_safe(uf, "w+");

/* if successful: print initial log and setup tmp log to delete on close */
    if (lg.file != NULL)
       {SC_get_uname(unm, MAXLINE, -1);
        lg.entry = _SC.nlog;
        _SC.nlog++; 

        fprintf(lg.file, "\n");
        SC_log(lg, "opened by '%s' on %s", unm, hostnm, _SC.nlog); 

/* this just makes the link counter on the inode zero
 * the file will not actually be deleted until it is closed
 */
        SC_remove(uf);};

    SC_LOCKOFF(_SC_log_lock);

    return(lg);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_CLOSE_LOG - Atomically append the data in the tmp log file to the global log
 *                and close both log files.
 *
 *                The global log file is available to any serial, threaded,
 *                or MPI parallel process that can see the NFS mount containing
 *                the global log file, hence locks are used at the grainulatity
 *                of the global log's inode while appending a temp log entry.
 *                fcntl is used to implement locks since locking will be  
 *                done over NFS and flock only manages inode locks locally.
 *                Note, this means the nfsd will need to be lock aware, that
 *                is, it must accept RPC calls from rpc.lockd of remote hosts.
 *
 *                Returns -1 on error, otherwise 0.
 */

int SC_close_log(SC_logfile lg)
   {int status, global_log;
    char bf[MAXLINE];
    char *fname;
    size_t nbr;
    ssize_t nbw;
    struct flock lock; 

    status = 0;

/* ignore requests to close non-existant tmp logs */
    if (lg.file == NULL) 
       return(-1);

/* get a handle to the global log */
    fname = getenv("SC_GLOBAL_LOG_FILE");
    if (fname == NULL)
       return(-1);

    global_log = SC_open_safe(fname, O_WRONLY | O_APPEND | O_CREAT, 0644);

    if (global_log == -1)
       {status = -1;}
    else 
       {SC_log(lg, "closed");

/* get an exclusive write lock for the global log */
        lock.l_type   = F_WRLCK;
        lock.l_start  = 0;
        lock.l_whence = SEEK_SET;  /* Lock entire file */
        lock.l_len    = 0;
        lock.l_pid    = getpid();

/* BLOCK waiting to LOCK the global log */
        status = fcntl(global_log, F_SETLKW, &lock); 

/* interrupt sent while waiting for lock: global log NOT locked, return */
        if (status == -1)
	   {SC_close_safe(global_log);
	    return(-1);};

/* LOCK HELD: No returns allowed until lock released */

/* coax buffers bound for the tmp log to disk */
        SC_fflush_safe(lg.file);  
        sync(); 

/* append tmp log to global log */
        rewind(lg.file);
        nbr = SC_fread_sigsafe(bf, (size_t) 1, (size_t) MAXLINE, lg.file);
            
/* while we've read bytes and didn't get an error: continue copy */
        while ((nbr > 0) && (ferror(lg.file) == 0))
           {nbw = SC_write_sigsafe(global_log, bf, nbr);
  
/* if we have a write failure: punt */ 
	    if (nbw == -1) 
	       {perror("write"); 
		nbr = -1;}
	    else
	       nbr = SC_fread_sigsafe(bf, 1, MAXLINE, lg.file);};
        
        if (ferror(lg.file))
           perror("fread");
    
/* coax the buffers bound for the global log to disk */
        fsync(global_log);
     
/* UNLOCK the global_log */
        lock.l_type = F_UNLCK;
        status = fcntl(global_log, F_SETLKW, &lock); 

        if (status == -1)
           perror("fcntl");

/* LOCK RELEASED */
 
/* release our handle to the global log file resource */
        SC_close_safe(global_log);};

/* and finally, close the tmp log, deleting it in the process */
    if (lg.file != NULL)
       SC_fclose_safe(lg.file);

    return(status);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
