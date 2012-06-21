/*
 * SCWIN32.C - MS Windows win32 specific routines
 *           - would cover DOS too
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
#include "score_int.h" 

#if defined(MSW)

extern void
 _SC_win32_process(PROCESS *pp);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	    LPSTR lpCmdLine, int iCmdShow)
   {
    
    _PG_hInstance = hInstance;
    _PG_iCmdShow  = iCmdShow;
    
    main(1, NULL);

    return(_PG_msg.wParam);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PATH_DELIMITER_WIN32 - copy the PATH delimiter, ';' to DELIM */

static void _SC_path_delimiter_win32(char *delim)
   {
    
    strcpy(delim, ";");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FILE_LENGTH_WIN32 - return the length of the file PATH */

static int64_t _SC_file_length_win32(char *path)
   {int64_t ln;
    FILE *fp;

    ln = 0L;
    fp = fopen(path, "r");
    if (fp != NULL)
       {ln = SC_file_size(fp);
	io_close(fp);};

    return(ln);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_QUERY_MODE_WIN32 - return TRUE if the file NAME can be opened
 *                      - in MODE
 */

static int _SC_query_mode_win32(char *name, char *mode)
   {int ret;
    FILE *fp;

    if (mode == NULL)
       fp = io_open(name, "r");
    else
       fp = io_open(name, mode);

    ret = (fp != NULL);
    if (fp != NULL)
       io_close(fp);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_QUERY_EXEC_WIN32 - return TRUE if the file PATH is executable */

static int _SC_query_exec_win32(char *path)
   {int ret;
    char s[MAXLINE];
    char *base, *sfx;

    strcpy(s, path);
    SC_str_lower(s);

    base = SC_strtok(s, ".", u);
    sfx  = SC_strtok(NULL, "\n", u);
    if (sfx != NULL)
       ret = ((strcmp(sfx, "exe") == 0) || (strcmp(sfx, "bat") == 0))

    return(ret);}

/*--------------------------------------------------------------------------*/

/*                               OS API CALLS                               */

/*--------------------------------------------------------------------------*/

/* GETPWUID_WIN - getpwuid for MSW */

static uid_t getpwuid_win(void)
   {uid_t rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GETUID_WIN - getuid for MSW */

static uid_t getuid_win(void)
   {uid_t rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GETGID_WIN - getgid for MSW */

static gid_t getgid_win(void)
   {gid_t rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GETPID_WIN - getpid for MSW */

static pid_t getpid_win(void)
   {pid_t rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GETPPID_WIN - getppid for MSW */

static pid_t getppid_win(void)
   {pid_t rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GETPGRP_WIN - getpgrp for MSW */

static pid_t getpgrp_win(void)
   {pid_t rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TCGETPGRP_WIN - tcgetpgrp for MSW */

static pid_t tcgetpgrp_win(int fd)
   {pid_t rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SETSID_WIN - setsid for MSW */

static pid_t setsid_win(void)
   {pid_t rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FORK_WIN - fork for MSW */

static pid_t fork_win(void)
   {pid_t rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* KILL_WIN - kill for MSW */

static int kill_win(pid_t pid, int sig)
   {int rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHED_YIELD_WIN - SCHED_yield for MSW */

static int sched_yield_win(void)
   {int rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FSYNC_WIN - fsync for MSW */

static int fsync_win(int fd)
   {int rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IOCTL_WIN - ioctl for MSW */

static int ioctl_win(int d, int request, ...)
   {int rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FCNTL_WIN - fcntl for MSW */

static int fcntl_win(int fd, int cmd, ...)
   {int rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* POLL_WIN - poll for MSW */

static int poll_win(struct pollfd *fds, nfds_t nfds, int timeout)
   {int rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NANOSLEEP_WIN - nanosleep for MSW */

static int nanosleep_win(const struct timespec *req, struct timespec *rem)
   {int rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SETENV_WIN - setenv for MSW */

static int setenv_win(const char *name, const char *value, int overwrite)
   {int rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UNSETENV_WIN - unsetenv for MSW */

static int unsetenv_win(const char *name)
   {int rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GETHOSTNAME_WIN - gethostname for MSW */

static int gethostname_win(char *name, size_t len)
   {int rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TTYNAME_WIN - ttyname for MSW */

static char *ttyname_win(int fd)
   {char *rv;

    rv = NULL;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SRANDOM_WIN - srandom for MSW */

static void srandom_win(unsigned int seed)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RANDOM_WIN - random for MSW */

static long random_win(void)
   {long rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* system capability API */

SC_oscapdes
  _SC_os = { _SC_path_delimiter_win32,
	     _SC_file_length_win32,
	     _SC_query_mode_win32,
	     _SC_query_exec_win32,
	     _SC_win32_process };

/* system call API */

syscall_api
  _SC_osapi = { getpwuid_win, getuid_win, getgid_win, 
		getpid_win, getppid_win, getpgrp_win, tcgetpgrp_win, 
		setsid_win, fork_win, kill_win, sched_yield_win,
		fsync_win, ioctl_win, fcntl_win, 
		poll_win, nanosleep_win, 
		setenv_win, unsetenv_win, 
		gethostname_win, ttyname_win, srandom_win, random_win };

#endif
