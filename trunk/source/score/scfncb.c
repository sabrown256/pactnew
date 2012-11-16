/*
 * SCFNCB.C - some system oriented routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"
#include "scope_proc.h"

#ifdef HAVE_BFD
# include <execinfo.h>
#endif

#define SC_DBG_GDB         1
#define SC_DBG_TOTALVIEW   2
#define SC_DBG_DBX         3

#define LOAD_AVE_CMND   "(w | head -n 1)"

/*--------------------------------------------------------------------------*/

#ifdef HAVE_BFD

/*--------------------------------------------------------------------------*/

/* _SC_PUSH_LOC - push the string version of LOC onto STR */

static void _SC_push_loc(SC_array *str, int i, long ad, SC_srcloc *loc)
   {char fnc[MAXLINE], fnm[MAXLINE], t[MAXLINE];

    SC_strncpy(fnc, MAXLINE, (char *) loc->pfunc, -1);
    SC_strncpy(fnm, MAXLINE, (char *) loc->pfile, -1);

    if (fnc[0] == '\0')
       snprintf(t, MAXLINE, "#%-3d 0x%012lx %s\n",
		i, ad, fnm);
    else if (loc->line < 1)
       snprintf(t, MAXLINE, "#%-3d 0x%012lx %-24s(%s)\n",
		i, ad, fnc, fnm);
    else
       snprintf(t, MAXLINE, "#%-3d 0x%012lx %-24s(%s:%d)\n",
		i, ad, fnc, fnm, loc->line);

    SC_array_string_add_copy(str, t);

    return;}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* _SC_MEM_REAL - convert the Nth token (according to DELIM) in the string S
 *              - specifying an amount of memory to a double
 *              - the string may end in g, G, m, M, k, or K
 *              - indicating gigabytes, megabytes or kilobytes
 *              - this function is really file static but
 *              - the static declaration was left off to prevent
 *              - compiler warnings on systems where it is unused
 */

double _SC_mem_real(char *s, int n, char *delim)
   {int nc;
    char c;
    double rv;
    char sp[MAXLINE];
    char *t;

    rv = 0;
    if (s != NULL)
       {t = SC_ntok(sp, MAXLINE, s, n, delim);
	if (t != NULL)
	   {nc      = strlen(t);
	    c       = t[nc-1];
	    t[nc-1] = '\0';

	    rv = SC_stof(t);

	    switch (c)
	       {case 'g' :
		case 'G' :
		     rv *= ((double) (1 << 30));
		     break;

		case 'm' :
		case 'M' :
		     rv *= ((double) (1 << 20));
		     break;

		case 'k' :
		case 'K' :
		     rv *= ((double) (1 << 10));
		     break;};};};

/* return a minimum of 1 */
    rv = max(rv, 1);

    return(rv);}
	
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PS_TIME - convert a time string of the form [[DD-]HH:]MM:SS
 *             - to seconds
 */

int _SC_ps_time(char *time)
   {int n, day, hr, mn, sec;
    int in[3];
    char sp[MAXLINE];
    char *t, *p;

    sec = 0;
    if (time != NULL)
       {memset(in, 0, sizeof(in));

	strcpy(sp, time);
	t = strchr(sp, '-');
	if (t != NULL)
	   {*t++ = '0';
	    day  = SC_stoi(sp);}
	else
	   {t   = sp;
	    day = 0;};

	for (n = 0; t != NULL; n++, t = p)
	    {p = strchr(t, ':');
	     if (p != NULL)
	        *p++ = '\0';
	     else
	        p = NULL;

	     in[n] = SC_stoi(t);};

	if (n == 3)
	   {hr  = in[0] + 24*day;
	    mn  = in[1];
	    sec = in[2];}
	else
	   {hr  = 0;
	    mn  = in[0];
	    sec = in[1];};

	sec += 60*(60*hr + mn);};

    return(sec);}
	
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* SC_LOAD_AVE - return the load averages for the current host
 *             - the averages are returned in AV which must be at
 *             - least 3 long to received the 1, 5, and 15 minute
 *             - averages
 *             - return TRUE iff successful
 */

int SC_load_ave(double *av)
   {int rv;

#if defined(HAVE_POSIX_SYS)

    int i, st;
    char sp[MAXLINE];
    char **res, *t;

    st = SC_exec(&res, LOAD_AVE_CMND, NULL, -1);
    if (st == 0)
       {for (i = 0; i < 3; i++)
	    {t     = SC_ntok(sp, MAXLINE, res[0], 10+i, " ,\t");
	     av[i] = SC_stof(t);};};

    SC_free_strings(res);

    rv = (st == 0);

#else

    rv = FALSE;

#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* SC_FREE_MEM - return the free memory on the current host
 *             - the results are returned in MEM which is NI long
 *             - depending on NI
 *             -    MEM[0] = the total system memory
 *             -    MEM[1] = the free memory
 *             -    MEM[2] = the total swap memory
 *             -    MEM[3] = the free swap memory
 *             - return TRUE iff successful
 *
 * #bind SC_free_mem fortran() scheme() python()
 */

int SC_free_mem(int ni, double *mem ARG([*,*,*,*],out))
   {int i, rv;

#if defined(HAVE_POSIX_SYS)

    int st;
    char **res;

    st  = 1;
    res = NULL;

    for (i = 0; i < ni; i++)
        mem[i] = 1.0;

#if defined(LINUX) || defined(CYGWIN)
    {int l;
     char *t, *v, *p, **sa;

     st = 0;
     sa = SC_file_strings("/proc/meminfo");

     for (i = 0, l = 0; (sa[i] != NULL) && (l < ni); i++)
         {t = SC_strtok(sa[i], ": \t\n", p);
	  if (t == NULL)
	     continue;
	  if (strcmp(t, "Mem") == 0)
	     {v      = SC_strtok(NULL, " \t\n", p);
	      mem[0] = SC_stol(v);
	      l++;
	      SC_strtok(NULL, " \t\n", p);
	      if (ni > 1)
	         {v      = SC_strtok(NULL, " \t\n", p);
		  mem[1] = SC_stol(v);
		  l++;};}
	  else if (strcmp(t, "MemTotal") == 0)
	     {v      = SC_strtok(NULL, " \t\n", p);
	      mem[0] = SC_stol(v);
	      l++;}
	  else if ((ni > 1) && (strcmp(t, "SwapTotal") == 0))
	     {v      = SC_strtok(NULL, " \t\n", p);
	      mem[2] = SC_stol(v);
	      l++;}
	  else if ((ni > 1) && (strcmp(t, "SwapFree") == 0))
	     {v      = SC_strtok(NULL, " \t\n", p);
	      mem[3] = SC_stol(v);
	      l++;};};

     SC_free_strings(sa);}

#elif defined(MACOSX)
    int ok;
    char path[PATH_MAX];
    char *cmd;

    ok = SC_full_path("pcexec", path, PATH_MAX);
    if (ok == 0)
       {cmd = "(pcexec -c 1 top | head -n 10 | awk '($1 ~ /PhysMem/) { print }')";
        st  = SC_exec(&res, cmd, NULL, -1);
        if (st == 0)
	   {double act, fr;
	    act = _SC_mem_real(res[0], 8, " \t\n\r");    /* active */
	    fr  = _SC_mem_real(res[0], 10, " \t\n\r");   /* free */
	    if (ni > 0)
	       mem[0]  = act + fr;
	    if (ni > 1)
	       mem[1]  = fr;};};

#elif defined(FREEBSD)
    char *cmd;

    cmd = "(top -b -n 1 | head -n 10 | awk '($1 ~ /Mem/) { print } ($1 ~ /Swap/) { print }')";
    st  = SC_exec(&res, cmd, NULL, -1);
    if (st == 0)
       {double act, fr;
	act = _SC_mem_real(res[0], 2, " \t\n\r");    /* active */
	fr  = _SC_mem_real(res[0], 12, " \t\n\r");   /* free */
	if (ni > 0)
	   mem[0]  = act + fr;
        if (ni > 1)
	   mem[1]  = fr;
        if (ni > 2)
	   mem[2]  = _SC_mem_real(res[0], 15, " \t\n\r");
        if (ni > 3)
	   mem[3]  = _SC_mem_real(res[0], 17, " \t\n\r");};

#elif defined(SOLARIS)
    char *cmd;

    cmd = "(top -b -n 1 | head -n 10 | awk '($1 ~ /Memory/) { print }')";
    st  = SC_exec(&res, cmd, NULL, -1);
    if (st == 0)
       {if (ni > 0)
	   mem[0] = _SC_mem_real(res[0], 2, " \t\n\r");
        if (ni > 1)
	   mem[1] = _SC_mem_real(res[0], 5, " \t\n\r");
        if (ni > 2)
	   mem[2] = _SC_mem_real(res[0], 8, " \t\n\r");
        if (ni > 3)
	   mem[3] = _SC_mem_real(res[0], 11, " \t\n\r");};

#elif defined(AIX)
    char *cmd;

    cmd = "(vmstat -vs | awk '/free pages/ {print $1} /memory pages/ {print $1}')";
    st  = SC_exec(&res, cmd, NULL, -1);
    if (st == 0)
       {if (ni > 0)
	   mem[0] = 4096.0*SC_stof(res[0]);
        if (ni > 1)
	   mem[1] = 4096.0*SC_stof(res[1]);};

#else
    char *cmd;

    cmd = "(top -b -n 1 | head -n 10 | awk '($1 ~ /Memory/) { print }')";
    st  = SC_exec(&res, cmd, NULL, -1);
    if (st == 0)
       {if (ni > 0)
	   mem[0] = _SC_mem_real(res[0], 2, " \t\n\r");
        if (ni > 1)
	   mem[1] = _SC_mem_real(res[0], 6, " \t\n\r");};

#endif

    SC_free_strings(res);

    rv = (st == 0);

    for (i = 0; i < 2; i++)
        mem[i] = max(mem[i], 1.0);

#else

    rv = FALSE;

    for (i = 0; i < 2; i++)
        mem[i] = 1.0;

#endif

/* convert to bytes */
     for (i = 0; i < ni; i++)
         mem[i] *= 1024.0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_NCPU - return the number of CPUs on the current host */

int SC_get_ncpu(void)
   {int np;

    np = 0;

#if defined(LINUX) || defined(CYGWIN)

    {int method;
     char s[MAXLINE+1];
     char *ps;
     FILE *fp;

     fp = SC_fopen_safe("/proc/cpuinfo", "r");
     if (fp != NULL)
        {np     = 0;
         method = 0;
	 while (SC_fgets(s, MAXLINE, fp) != NULL)
	    {s[MAXLINE] = '\0';
             ps = strtok(s, " :\t");
	     if (ps == NULL)
	        continue;

	     else if (strcmp(ps, "processor") == 0)
	        {ps     = strtok(NULL, " :\t\n\r");
		 np     = SC_stoi(ps);
	         method = 1;}

	     else if (strcmp(ps, "cpus") == 0)
	        {ps = strtok(NULL, " :\t\n");
	         if ((ps != NULL) && (strcmp(ps, "active") == 0))
		    {ps     = strtok(NULL, " :\t\n\r");
		     np     = SC_stoi(ps);
		     method = 0;};};};
	 fclose(fp);

	 np += method;};}

#elif defined(AIX)

    {int st;

# include <sys/sysconfig.h>
# include <sys/var.h>

     struct var v;

     st = sysconfig(SYS_GETPARMS, &v, sizeof(struct var));
     if (st == 0)
        {
/*         np = v.v_ncpus_cfg; */
	 np = v.v_ncpus;}
     else
        np = 1;}

/* since OpenMP has a standard function use this in preference
 * to other options when it works right (AIX does not - it spawns
 * NCPU threads)
 */

#elif defined(PTHREAD_OMP)

   {extern int omp_get_num_procs(void);

    np = omp_get_num_procs();}

#elif defined(MACOSX)

   {int st;
    char *s, *t, *p, *cmd, **out;

    np  = 1;
    cmd = "(system_profiler SPHardwareDataType | grep 'Number Of Cores:')";
    st  = SC_exec(&out, cmd, NULL, -1);
    if (st == 0)
       {s = out[0];
	t = SC_strtok(s, ":\t\n", p);
	t = SC_strtok(NULL, " \t\n", p);
	np = SC_stoi(t);};

    SC_free_strings(out);}

#elif defined(SGI)

/* number of processors configured */
/*     np = sysmp(MP_NPROCS); */

/* number of processors available */
     np = sysmp(MP_NAPROCS);

#elif defined(SOLARIS)

/* number of processors configured */
/*    np = sysconf(_SC_NPROCESSORS_CONF); */

/* number of processors available */
    np = sysconf(_SC_NPROCESSORS_ONLN);

#elif defined(HPUX)

# include <sys/param.h>
# include <sys/pstat.h>

    struct pst_dynamic psd;

    pstat_getdynamic(&psd, sizeof(psd), (size_t) 1);
    np = psd.psd_proc_cnt;

#elif defined(OSF)

# include <sys/sysinfo.h>
# include <machine/hal_sysinfo.h>

   np = -1;
   getsysinfo(GSI_CPUS_IN_BOX, &np, sizeof(np),
              NULL, NULL, NULL);

#else

    np = 1;

#endif

    return(np);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_PNAME - return the full path name of the program running
 *              - as process id PID in PATH which is NC long
 *              - if PID < 0 use the current process id
 *              - return:
 *              -    0 on success
 *              -   -1 NAME is not on your path
 *              -    n NAME is on your path but NC is too few characters
 *              -      n is the number of characters needed to contain
 *              -      the full path
 */

int SC_get_pname(char *path, int nc, int pid)
   {int rv, st;
    char *p, *t;
    SC_rusedes ru;

    if (pid < 0)
       pid = getpid();

     rv = -1;

#if 1
    {char *s;

     st = SC_resource_usage(&ru, pid);
     if (st == 1)
        {s  = CSTRSAVE(ru.cmd);
	 t  = SC_strtok(s, " \t\n", p);
	 if (t != NULL)
	    rv = SC_full_path(t, path, nc);
	 CFREE(s);};};
#else
    {int i, tid;
     char *cmd, *s, **out;

#if defined(MACOSX)
     cmd = "ps -eo pid,command";
#else
     cmd = "ps -eo pid,args";
#endif

     st = SC_exec(&out, cmd, NULL, -1);
     if (st == 0)
        {for (i = 0; TRUE; i++)
	     {s = out[i];
	      if (s == NULL)
	         break;

	      t   = SC_strtok(s, " \t\n", p);
	      tid = SC_stoi(t);
	      if (pid == tid)
	         {t  = SC_strtok(NULL, " \t\n", p);
		  rv = SC_full_path(t, path, nc);
		  break;};};};

     SC_free_strings(out);};
#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_GET_CYGWIN_PROCESSES - get process info for a CYGWIN process
 *                          - cygwin "pid,ppid,pgid,winpid,tty, uid,stime,args"
 *                          - unix   "pid,ppid,pgid,uid,nice,vsz,time,etime,stime,args"
 */

SC_proc_info *_SC_get_cygwin_processes(char *cmd, int uid)
   {int i, j, m, n, nc, st;
    int in[5];
    char *p, *s, *t, **out;
    char *tstm;
    SC_proc_info *pi, *pc;
    extern char *strptime(const char *s, const char *format, struct tm *tm);

    pi = NULL;
    st = SC_exec(&out, cmd, NULL, -1);
    if (st == 0)
       {SC_ptr_arr_len(n, out);

	pi = CMAKE_N(SC_proc_info, n+1);
	m  = 0;

	for (i = 1; i < n; i++)
	    {s = out[i];

             for (j = 0; j < 4; j++, s = NULL)
	         {t     = SC_strtok(s, " \t\n", p);
		  in[j] = SC_stoi(t);

/* skip winpid and tty */
                  if (j == 2)
		     {t = SC_strtok(s, " \t\n", p);
		      t = SC_strtok(s, " \t\n", p);};};

	     if ((uid != -1) && (in[3] != uid))
	        continue;

	     pc = pi + m++;

	     pc->pid      = in[0];
	     pc->ppid     = in[1];
	     pc->pgid     = in[2];
	     pc->uid      = in[3];
	     pc->priority = 0;

	     tstm = SC_strtok(s, " \t\n", p);

	     t = SC_strtok(s, "\n", p);
	     if (t != NULL)
	        {nc = strlen(t);
		 SC_strncpy(pc->command, MAXLINE, t, nc);};

	     pc->memory = 0.0;
	     pc->time   = 0.0;
	     pc->etime  = 0.0;
	     pc->tstart = _SC_ps_time(tstm);};

	pc      = pi + m++;
	pc->pid = -1;
	CREMAKE(pi, SC_proc_info, m);};

    SC_free_strings(out);

    return(pi);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_GET_UNIX_PROCESSES - get process info for a UNIX process */

SC_proc_info *_SC_get_unix_processes(char *cmd, int uid)
   {int i, j, m, n, nc, st;
    int in[5];
    char *p, *s, *t, **out;
    char *tmem, *tstm, *tetm, *ttst;
    SC_proc_info *pi, *pc;
    extern char *strptime(const char *s, const char *format, struct tm *tm);

    pi = NULL;
    st = SC_exec(&out, cmd, NULL, -1);
    if (st == 0)
       {SC_ptr_arr_len(n, out);

	pi = CMAKE_N(SC_proc_info, n+1);
	m  = 0;

	for (i = 1; i < n; i++)
	    {s = out[i];
             for (j = 0; j < 5; j++, s = NULL)
	         {t     = SC_strtok(s, " \t\n", p);
		  in[j] = SC_stoi(t);};

	     if ((uid != -1) && (in[3] != uid))
	        continue;

	     pc = pi + m++;

	     pc->pid      = in[0];
	     pc->ppid     = in[1];
	     pc->pgid     = in[2];
	     pc->uid      = in[3];
	     pc->priority = in[4];

	     tmem = SC_strtok(s, " \t\n", p);
	     tstm = SC_strtok(s, " \t\n", p);
	     tetm = SC_strtok(s, " \t\n", p);
	     ttst = SC_strtok(s, " \t\n", p);

	     t  = SC_strtok(s, "\n", p);
	     if (t != NULL)
	        {nc = strlen(t);
		 SC_strncpy(pc->command, MAXLINE, t, nc);};

	     pc->memory = _SC_mem_real(tmem, 1, " ");
	     pc->time   = _SC_ps_time(tstm);
	     pc->etime  = _SC_ps_time(tetm);
	     pc->tstart = _SC_ps_time(ttst);};

	pc      = pi + m++;
	pc->pid = -1;
	CREMAKE(pi, SC_proc_info, m);};

    SC_free_strings(out);

    return(pi);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_PROCESSES - process info on selected processes */

SC_proc_info *SC_get_processes(int uid)
   {char *cmd;
    SC_proc_info *pi;

#if defined(CYGWIN) || defined(MSW)
    cmd = "ps -W";
    pi  = _SC_get_cygwin_processes(cmd, uid);

# elif defined(BEOS)
    cmd = "ps";
    pi  = NULL;

#else

# if defined(SGI) || defined(AIX) || defined(OSF) || defined(SOLARIS)
    cmd = "ps -eo pid,ppid,pgid,uid,nice,vsz,time,etime,stime,args";
# elif defined(LINUX)
    cmd = "ps -w -eo pid,ppid,pgid,uid,nice,vsz,time,etime,stime,args";
# elif defined(MACOSX)
    cmd = "ps -axeo pid,ppid,pgid,uid,nice,vsz,time,time,stime,command";
# elif defined(FREEBSD)
    cmd = "ps -axeo pid,ppid,pgid,uid,nice,vsz,time,time,start,command";
# endif

    pi = _SC_get_unix_processes(cmd, uid);

# endif

    return(pi);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_NFS_MONITOR - return some NFS statistics in the array ST
 *                - if NC = 1 return
 *                -    ST[0] = # RPC calls
 *                - if NC = 2 return
 *                -    ST[0] = # RPC calls
 *                -    ST[1] = # NFS operations
 *                - if NC > 2 return NC (upto 8) of the values:
 *                -    ST[0] = # RPC calls
 *                -    ST[1] = # RPC retrans
 *                -    ST[2] = # NFS operations
 *                -    ST[3] = # NFS reads
 *                -    ST[4] = # NFS writes
 *                -    ST[5] = # NFS lookups
 *                -    ST[6] = # NFS getattrs
 *                -    ST[7] = # NFS setattrs
 *                - return TRUE iff successful
 *                - NOTE: if FALSE is returned check path for
 *                - PACT script NFSMON
 */

int SC_nfs_monitor(int *st, int nc)
   {int i, rv;
    char out[MAXLINE];
    char *s, *t, *p;

    p  = NULL;
    rv = SC_execs(out, MAXLINE, NULL, 10000, "nfsmon -q -r");
    if (rv == 0)
       {s = out;
	t = SC_strtok(s, " \t\n", p);
	s = NULL;

	st[0] = SC_stol(t);

	if (nc == 2)
	   {t     = SC_strtok(s, " \t\n", p);
	    t     = SC_strtok(s, " \t\n", p);
	    st[1] = SC_stol(t);}

	else
	   {for (i = 1; i < nc; i++)
	        {t     = SC_strtok(s, " \t\n", p);
		 st[i] = SC_stol(t);};};
	rv = TRUE;}

    else
       rv = FALSE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SLEEP - sleep for TO milliseconds */

void SC_sleep(int to)
   {int n;


#if 1
    
    struct timespec req, rem;
    double t, ts, tn;

    t  = 1.0e-3*to;
    ts = floor(t);
    tn = t - ts;
    req.tv_sec  = (time_t) ts;
    req.tv_nsec = 1.0e9*tn;

    while (TRUE)
       {n = nanosleep(&req, &rem);
	if ((n == -1) && (errno == EINTR))
	   req = rem;
	else
	   break;};

#else

    n = SC_poll(NULL, 0, to);

#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_ENV - return a pointer to the NULL terminated environment array
 *            - if not available return NULL
 */

char **SC_get_env(void)
   {char **s;

#if defined(MACOSX)
    extern char ***_NSGetEnviron(void);

    s = *_NSGetEnviron();

#else
    extern char **environ;

    s = environ;
#endif

    return(s);}

/*--------------------------------------------------------------------------*/

/*                        DEBUGGER/RETRACE ROUTINES                         */

/*--------------------------------------------------------------------------*/

/* _SC_WHICH_DBG - decide which debugger to use
 *               - return the full path to the debugger in the buffer PATH
 *               - which is NC long
 *               - use PREFER or the SC_DEBUGGER environment variable
 *               - for an expressed debugger preference
 *               - return the debugger id code
 *               - or -1 if no debugger found
 */
#if 0
static int _SC_which_dbg(char *prefer, char *path, int nc)
   {int rv, id;

    if (prefer == NULL)
       prefer = getenv("SC_DEBUGGER");

    if (prefer == NULL)
       {rv = SC_full_path("gdb", path, MAXLINE);
	if (rv != 0)
	   {rv = SC_full_path("totalview", path, MAXLINE);
	    if (rv != 0)
	       rv = SC_full_path("dbx", path, MAXLINE);};}
    else
       rv = SC_full_path(prefer, path, MAXLINE);

    if (rv == 0)
       {if (strstr(path, "gdb") != NULL)
	   id = SC_DBG_GDB;
	else if (strstr(path, "totalview") != NULL)
	   id = SC_DBG_TOTALVIEW;
	else if (strstr(path, "dbx") != NULL)
	   id = SC_DBG_DBX;}
    else
       id = -1;

    return(id);}
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ATTACH_DBG - attach a debugger to the process PID
 *               - if PID < 0 use the current process id
 *               - return TRUE iff successful
 */

int SC_attach_dbg(int pid)
   {int rv;
    char path[PATH_MAX];
    char *cmd;

    if (pid < 0)
       pid = getpid();

    rv = SC_get_pname(path, PATH_MAX, pid);
    if (rv == 0)
       {cmd = SC_dsnprintf(TRUE, "atdbg -l log.atdbg -p %d -e %s", pid, path);
	rv  = system(cmd);
	CFREE(cmd);}
    else
       rv = -1;

#if 0
    if (rv == 0)
       {id = _SC_which_dbg(NULL, dbg, MAXLINE);
	switch (id)
	   {case SC_DBG_GDB :
	         cmd = SC_dsnprintf(TRUE, "xterm -sb -e %s %s %d", dbg, path, pid);
		 system(cmd);
		 rv = TRUE;
		 break;
	    case SC_DBG_TOTALVIEW :
		 cmd = SC_dsnprintf(TRUE, "%s %s -pid %d", dbg, path, pid);
		 system(cmd);
		 rv = TRUE;
		 break;
	    case SC_DBG_DBX :
		 cmd = SC_dsnprintf(TRUE, "%s %s -pid %d", dbg, path, pid);
		 system(cmd);
		 rv = TRUE;
		 break;
	    default :
		 rv = FALSE;
		 break;};

	CFREE(cmd);};
#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BACKTRACE_EXE - get the actual backtrace of PID
 *                   - if PID == -1 use the current process
 *                   - return an array of strings with text of backtrace
 */

static char **_SC_backtrace_exe(int pid, int to)
   {int i, epid, ok, rv;
    char path[PATH_MAX];
    char *cmd, **t;

    t = NULL;

    epid = (pid < 0) ? getpid() : pid;
    rv   = SC_get_pname(path, PATH_MAX, epid);

/* current process and executable access active
 * might as well save the fork of a child process
 */
    ok = FALSE;

#ifdef HAVE_BFD
    if ((pid < 0) && (_SC.exe.open != NULL))
       {int n;
	long ad;
	void **bf;
	char s[MAXLINE];
	char **out, **ta;
	SC_storloc sl;
	SC_srcloc *loc;
	SC_array *str;
	exedes *st;

	bf  = CMAKE_N(void *, 100);
	n   = backtrace(bf, 100);
	out = backtrace_symbols(bf, n);

	if (n > 0)
	   {str = SC_STRING_ARRAY();

	    st = SC_exe_open(path, NULL, NULL, TRUE, TRUE, TRUE, TRUE);
	    if (st != NULL)
	       {loc = &sl.loc;
		n--;
		for (i = 0; i < n; i++)
		    {SC_strncpy(s, MAXLINE, out[i], -1);
		     ta = SC_tokenize(s, " \t[]");
		     if (ta != NULL)
		        {if (ta[1] != NULL)
			    {ad = SC_strtol(ta[1], NULL, 16);

			     SC_exe_map_addr(&sl, st, out[i]);

			     _SC_push_loc(str, i, ad, loc);}
			 else
			    {snprintf(s, MAXLINE, "#%-4d[%s]\n", i, ta[0]);
			     SC_array_string_add_copy(str, s);};

			 SC_free_strings(ta);};};

		SC_exe_close(st);

		ok = TRUE;};

	    t = _SC_array_string_join(&str);};

	CFREE(bf);
	free(out);};
#endif

/* process other than current or no executable access */
    if ((ok == FALSE) && (rv == 0))
       {cmd = SC_dsnprintf(TRUE, "atdbg -r -p %d -e %s", epid, path);
	rv = SC_exec(&t, cmd, NULL, to);
	CFREE(cmd);};

/* strip off newlines */
    if (t != NULL)
       {for (i = 0; t[i] != NULL; i++)
	    SC_trim_right(t[i], "\n");};

    return(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_RETRACE_EXE - attach a debugger to the process PID and return the
 *                - call stack in POUT
 *                - POUT sets a char ** pointer in the caller
 *                - the array of strings is terminated by a NULL entry
 *                - it is the caller's responsibility to free the
 *                - string array
 *                - if PID < 0 use the current process id
 *                - wait no more than TO milliseconds if TO > 0
 *                - if TO == -1 wait as long as it takes
 */

int SC_retrace_exe(char ***pout, int pid, int to)
   {int i, rv;
    char **t;

    rv = FALSE;

    t = _SC_backtrace_exe(pid, to);

/* print the strings if the caller does not want them returned */
    if (t != NULL)
       {if (pout == NULL)
	   {for (i = 0; t[i] != NULL; i++)
	        {io_printf(stdout, "%s\n", t[i]);};
	    SC_free_strings(t);}

/* otherwise return the strings */
	else
	   *pout = t;

	 rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_RETRACE_DBG - attach a debugger to the process PID and return the
 *                - call stack in POUT
 *                - POUT sets a char ** pointer in the caller
 *                - the array of strings is terminated by a NULL entry
 *                - it is the caller's responsibility to free the
 *                - string array
 *                - if PID < 0 use the current process id
 */

int SC_retrace_dbg(char ***pout, int pid)
   {int rv;

    rv = SC_retrace_exe(pout, pid, -1);

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                             LATENCY ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* _SC_TIME_FILE_OPEN - find an average time to open the file S */

static void _SC_time_file_open(char *s)
   {int fd;

    fd = open(s, O_CREAT, 0600);
    close(fd);
    SC_remove(s);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_TIME_PING - issue an _SC_ping_host for timing purposes */

static void _SC_time_ping(char *s)
   {

    _SC_ping_host(NULL, 100, TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_TIME_EXEC - issue an SC_EXECS for timing purposes */

static void _SC_time_exec(char *s)
   {char t[MAXLINE];

    SC_execs(t, MAXLINE, NULL, -1, "date");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_TIME_COMMAND - find an average time to issue the command FNC
 *                  - which takes a single string argument S
 */

static double _SC_time_command(void (*fnc)(char *s), char *s, int n)
   {int i;
    double t0, dt, et;
    char *fn;

    dt = 0.0;

    for (i = 0; i < n; i++)
        {fn = SC_dsnprintf(TRUE, "%s.%d", s, i);

	 t0 = SC_wall_clock_time();

	 (*fnc)(fn);

	 CFREE(fn);

	 et = SC_wall_clock_time() - t0;

	 SC_sleep(10);

	 dt += et;}

    dt /= ((double) n);

    return(dt);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_LATENCIES - compute latencies:
 *                  -   TMP  time to create a file in /tmp
 *                  -        (presumed direct attached disk)
 *                  -   HOME time to create a file in $HOME
 *                  -        (presumed network attached disk - NFS)
 *                  -   NET  time to connect a socket
 *                  -   PROC time to launch a process
 */

void SC_get_latencies(double *ptmp, double *phm, double *pnet, double *pprc)
   {int pid;
    char s[MAXLINE], hst[MAXLINE];

    gethostname(hst, MAXLINE);
    pid = getpid();

    if (ptmp != NULL)
       {snprintf(s, MAXLINE, "/tmp/.latency-%s-%d", hst, pid);
	*ptmp = _SC_time_command(_SC_time_file_open, s, 32);};

    if (phm != NULL)
       {snprintf(s, MAXLINE, "%s/.latency-%s-%d", getenv("HOME"), hst, pid);
	*phm = _SC_time_command(_SC_time_file_open, s, 32);};

    if (pnet != NULL)
       *pnet = _SC_time_command(_SC_time_ping, s, 16);

    if (pprc != NULL)
       *pprc = _SC_time_command(_SC_time_exec, s, 8);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_UNAME - return the NAME of the user UID
 *              - NAME is NC long
 */

char *SC_get_uname(char *name, int nc, int uid)
   {char *rv;

    rv = NULL;

    if (uid == -1)
       uid = getuid();

    name[0] = '\0';

#if defined(HAVE_GETPWUID)

    {struct passwd *pw;

     pw = getpwuid(uid);
     if (pw != NULL)
        {snprintf(name, nc, "%10s", pw->pw_name);
	 rv = name;};}

#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_YIELD - wrapper for function to yield the processor */

int SC_yield(void)
   {int rv;

#if 1
    rv = sched_yield();
#else
    SC_sleep(10);
    rv = TRUE;
#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
