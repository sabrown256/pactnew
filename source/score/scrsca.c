/*
 * SCRSCA.C - routines involving resource usage
 *          - highly conditionalized
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"
#include "scope_mem.h"

#ifdef HAVE_RESOURCE_USAGE
# include <sys/resource.h>
#endif

/* make sure HZ is defined */
#ifndef HZ
# if defined(CYGWIN)
#  include <sys/param.h>
# elif defined(LINUX)
#  include <asm/param.h>
# endif
#endif

#if defined(LINUX) || defined(CYGWIN)

# define LINUX_PROC_PPID     0      /* PID of the parent */
# define LINUX_PROC_PGID     1      /* process group ID of the process */
# define LINUX_PROC_SESS     2      /* session ID of the process */
# define LINUX_PROC_TTY      3      /* tty the process uses */
# define LINUX_PROC_TGID     4      /* process group ID the terminal process */
# define LINUX_PROC_FLAGS    5      /* flags of the process */
# define LINUX_PROC_MNFLT    6      /* number of minor faults */
# define LINUX_PROC_CMNFLT   7      /* number of minor faults children */
# define LINUX_PROC_MJFLT    8      /* number of major faults */
# define LINUX_PROC_CMJFLT   9      /* number of major faults children */
# define LINUX_PROC_UTIME   10      /* user mode jiffies */
# define LINUX_PROC_STIME   11      /* kernel mode jiffies */
# define LINUX_PROC_CUTIME  12      /* user mode jiffies children */
# define LINUX_PROC_CSTIME  13      /* kernel mode jiffies children */
# define LINUX_PROC_PRIOR   14      /* nice value + fifteen */
# define LINUX_PROC_NICE    15      /* nice value -19 < 19 */
# define LINUX_PROC_0       16      /* hard coded to 0 as a placeholder */
# define LINUX_PROC_TIMER   17      /* jiffies before the next SIGALRM is sent */
# define LINUX_PROC_TSTART  18      /* jiffies process started after system boot */
# define LINUX_PROC_VSIZE   19      /* virtual memory size in bytes */
# define LINUX_PROC_RSS     20      /* Resident Set Size - pages the process has */
# define LINUX_PROC_RLIM    21      /* current limit in bytes of the RSS */
# define LINUX_PROC_ASTART  22      /* address above which program text can run */
# define LINUX_PROC_AEND    23      /* address below which program text can run */
# define LINUX_PROC_ASTACK  24      /* address of the start of the stack */
# define LINUX_PROC_ESP     25      /* current value of esp (stack pointer) */
# define LINUX_PROC_EIP     26      /* current EIP (instruction pointer) */
# define LINUX_PROC_SIGPND  27      /* bitmap of pending signals (usually 0) */
# define LINUX_PROC_SIGBLK  28      /* bitmap of blocked signals (usually 0, 2 for shells) */
# define LINUX_PROC_SIGIGN  29      /* bitmap of ignored signals */
# define LINUX_PROC_SIGCGT  30      /* bitmap of caught signals */
# define LINUX_PROC_WCHAN   31      /* "channel" in which the process is waiting */
# define LINUX_PROC_NSWAP   32      /* pages swapped - not maintained */
# define LINUX_PROC_CNSWAP  33      /* cumulative nswap for child processes */
# define LINUX_PROC_EXIT    34      /* signal to be sent to parent */
# define LINUX_PROC_PROC    35      /* CPU number last executed on */
# define LINUX_N_PROC       36

#elif defined(AIX)
# include <sys/procfs.h>

#elif defined(SOLARIS)
# define _STRUCTURED_PROC	1
# define _LP64
# include <sys/procfs.h>

#elif defined(MACOSX)
# include <sys/sysctl.h>
# include <mach/task.h>
# include <mach/mach_init.h>
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SHOW_RESOURCE_USAGE - display contents of RU */

void SC_show_resource_usage(SC_rusedes *ru)
   {

    io_printf(stdout, "   process id                                  = %d\n", ru->pid);
    io_printf(stdout, "   process parent id                           = %d\n", ru->ppid);
    io_printf(stdout, "   process user id                             = %d\n", ru->uid);
    io_printf(stdout, "   process scheduling priority                 = %d\n", ru->priority);
    io_printf(stdout, "   total user time used                        = %.3e\n", ru->ut);
    io_printf(stdout, "   total system time used                      = %.3e\n", ru->st);
    io_printf(stdout, "   maximum resident set size (in kilobytes)    = %.3e\n", ru->maxrss);
    io_printf(stdout, "   data segment memory used (kilobyte-seconds) = %.3e\n", ru->idrss);
    io_printf(stdout, "   stack memory used (kilobyte-seconds)        = %.3e\n", ru->isrss);
    io_printf(stdout, "   soft page faults - reclaims                 = %.0f\n", ru->minflt);
    io_printf(stdout, "   hard page faults - requiring I/O            = %.0f\n", ru->majflt);
    io_printf(stdout, "   times a process was swapped out of memory   = %.0f\n", ru->nswap);
    io_printf(stdout, "   block input operations via the file system  = %.0f\n", ru->inblock);
    io_printf(stdout, "   block output operations via the file system = %.0f\n", ru->outblock);
    io_printf(stdout, "   IPC messages sent                           = %.0f\n", ru->msgsnd);
    io_printf(stdout, "   IPC messages received                       = %.0f\n", ru->msgrcv);
    io_printf(stdout, "   signals delivered                           = %.0f\n", ru->nsignals);
    io_printf(stdout, "   voluntary context switches                  = %.0f\n", ru->nvcsw);
    io_printf(stdout, "   involuntary context switches                = %.0f\n", ru->nivcsw);
    io_printf(stdout, "   system calls                                = %.0f\n", ru->nsysc);
    io_printf(stdout, "   command line                                = %s\n", ru->cmd);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_RES_USAGE_SELF - fill RU with current resource usages
 *                    - measured from the first call
 *                    - return TRUE iff useful data was available
 *                    - zero out RU otherwise
 *                    - if WHCH is FALSE do process only
 *                    - if WHCH is TRUE do children too
 */

static int _SC_res_usage_self(SC_rusedes *ru, int whch)
   {int rv;

    rv = FALSE;

    SC_mem_trim(0);

    ru->pid      = getpid();
    ru->ppid     = getppid();
    ru->uid      = 0;
    ru->priority = 0;
    ru->ut       = 0.0;
    ru->st       = 0.0;
    ru->maxrss   = 0.0;
    ru->idrss    = 0.0;
    ru->isrss    = 0.0;
    ru->minflt   = 0.0;
    ru->majflt   = 0.0;
    ru->nswap    = 0.0;
    ru->inblock  = 0.0;
    ru->outblock = 0.0;
    ru->msgsnd   = 0.0;
    ru->msgrcv   = 0.0;
    ru->nsignals = 0.0;
    ru->nvcsw    = 0.0;
    ru->nivcsw   = 0.0;
    ru->nsysc    = 0.0;
    ru->cmd[0]   = '\0';

#ifdef HAVE_RESOURCE_USAGE

    {struct rusage r, *pr;
     static struct rusage rs, rc;

     if (whch == FALSE)
        pr = &rs;
     else
        pr = &rc;

     rv = getrusage(RUSAGE_SELF, &r);
     if (rv == 0)
        {if (whch == FALSE)
	    {ONCE_SAFE(TRUE, NULL)
	        rs = r;
	     END_SAFE;}
         else
	    {ONCE_SAFE(TRUE, NULL)
	        rc = r;
	     END_SAFE;};

	 ru->ut       = (r.ru_utime.tv_sec  - pr->ru_utime.tv_sec) +
                        (r.ru_utime.tv_usec - pr->ru_utime.tv_usec)*1.0e-6;
	 ru->st       = (r.ru_stime.tv_sec  - pr->ru_stime.tv_sec) +
                        (r.ru_stime.tv_usec - pr->ru_stime.tv_usec)*1.0e-6;
#ifdef MACOSX
/* the OS X man page incorrectly states that the units are in kBytes
 * they are in fact in bytes
 * see http://lists.apple.com/archives/darwin-kernel/2009/Mar/msg00005.html
 */
	 ru->maxrss   = (r.ru_maxrss   - pr->ru_maxrss)/1024;
#else
	 ru->maxrss   = r.ru_maxrss   - pr->ru_maxrss;
#endif
	 ru->idrss    = r.ru_idrss    - pr->ru_idrss;
	 ru->isrss    = r.ru_isrss    - pr->ru_isrss;
	 ru->minflt   = r.ru_minflt   - pr->ru_minflt;
	 ru->majflt   = r.ru_majflt   - pr->ru_majflt;
	 ru->nswap    = r.ru_nswap    - pr->ru_nswap;
	 ru->inblock  = r.ru_inblock  - pr->ru_inblock;
	 ru->outblock = r.ru_oublock  - pr->ru_oublock;
	 ru->msgsnd   = r.ru_msgsnd   - pr->ru_msgsnd;
	 ru->msgrcv   = r.ru_msgrcv   - pr->ru_msgrcv;
	 ru->nsignals = r.ru_nsignals - pr->ru_nsignals;
	 ru->nvcsw    = r.ru_nvcsw    - pr->ru_nvcsw;
	 ru->nivcsw   = r.ru_nivcsw   - pr->ru_nivcsw;};

     rv = (rv == 0);};

#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_RESOURCE_USAGE - fill RU with current resource usages
 *                   - return TRUE iff useful data was available
 *                   - zero out RU otherwise
 */

int SC_resource_usage(SC_rusedes *ru, int pid)
   {int rv;

    rv = FALSE;

    if (ru == NULL)
       return(rv);

    SC_MEM_INIT(SC_rusedes, ru);

    if (pid == -1)
       rv = _SC_res_usage_self(ru, FALSE);

    else if (pid == -2)
       rv = _SC_res_usage_self(ru, TRUE);

    else

#if defined(LINUX) || defined(CYGWIN)

       {int i, fd, nc;
	double val[LINUX_N_PROC], cnv;
	char s[MAXLINE];
	char *ps, *tok, *u, *fname;
	struct stat bf;
	static double mby = -1.0;

	SC_mem_trim(0);

	if (mby == -1.0)
	   mby = getpagesize()/(1024.0*1024.0);

	fname = SC_dsnprintf(TRUE, "/proc/%d/stat", pid);
	fd    = open(fname, O_RDONLY);
	if (fd > 0)
	   {nc = read(fd, s, MAXLINE);
	    close(fd);

	    if ((0 < nc) && (nc < MAXLINE))
	       {if (stat(fname, &bf) == 0)
		   ru->uid = bf.st_uid;

		s[nc] = '\0';
		ps    = strchr(s, ')');
		if ((ps != NULL) && (strlen(ps) > 3))
		   ps += 3;

		for (i = 0; i < LINUX_N_PROC; ps = NULL)
		    {tok = SC_strtok(ps, " \t\n", u);
		     val[i++] = SC_stoi(tok);};

		cnv = HZ*1.0e-4;

		ru->ppid   = val[LINUX_PROC_PPID];
		ru->ut     = cnv*val[LINUX_PROC_UTIME];
		ru->st     = cnv*val[LINUX_PROC_STIME];
		ru->maxrss = mby*val[LINUX_PROC_RSS];
		ru->minflt = val[LINUX_PROC_MNFLT];
		ru->majflt = val[LINUX_PROC_MJFLT];
		ru->nswap  = val[LINUX_PROC_NSWAP];};};
	CFREE(fname);

	fname = SC_dsnprintf(TRUE, "/proc/%d/cmdline", pid);
	fd    = open(fname, O_RDONLY);
	if (fd > 0)
	   {nc = read(fd, s, MAXLINE);
	    close(fd);
	    if (nc > 0)
	       SC_strncpy(ru->cmd, MAXLINE, s, -1);};
	CFREE(fname);

	rv = TRUE;};

#elif defined(AIX)

       {int fd, nb, nc;
	char *fname;
	double ut, st;
	pstatus_t sp;
	psinfo_t si;
	pr_timestruc64_t *us, *sy;
	struct stat bf;
	static double mby = 1.0/(1024.0*1024.0);

	fname = SC_dsnprintf(TRUE, "/proc/%d/status", pid);
	fd    = open(fname, O_RDONLY);
	if (fd > 0)
	   {nc = sizeof(pstatus_t);
	    nb = read(fd, &sp, nc);
	    close(fd);

	    if (stat(fname, &bf) == 0)
	       ru->uid = bf.st_uid;

	    if (nb == nc)
	       {us = &sp.pr_utime;
		sy = &sp.pr_stime;

		ut = us->tv_sec + us->tv_nsec*1.0e-9;
		st = sy->tv_sec + sy->tv_nsec*1.0e-9;

		ru->ppid   = sp.pr_ppid;
		ru->ut     = ut;
		ru->st     = st;
		ru->maxrss = mby*sp.pr_brksize;};};
	CFREE(fname);

	fname = SC_dsnprintf(TRUE, "/proc/%d/psinfo", pid);
	fd    = open(fname, O_RDONLY);
	if (fd > 0)
	   {nc = sizeof(psinfo_t);
	    nb = read(fd, &si, nc);
	    close(fd);

	    if (nb == nc)
	       SC_strncpy(ru->cmd, MAXLINE, si.pr_psargs, -1);};
	CFREE(fname);

	rv = TRUE;};

#elif defined(SOLARIS)

       {int i, fd, nb, nc;
	char *fname;
	double ut, st;
	pstatus_t sp;
	psinfo_t si;
        prusage_t su;
	timestruc_t *us, *sy;
	struct stat bf;
	static double mby = 1.0/(1024.0*1024.0);

	fname = SC_dsnprintf(TRUE, "/proc/%d/status", pid);
	fd    = open(fname, O_RDONLY);
	if (fd > 0)
	   {nc = sizeof(pstatus_t);
	    nb = read(fd, &sp, nc);
	    close(fd);

	    if (stat(fname, &bf) == 0)
	       ru->uid = bf.st_uid;

	    if (nb == nc)
	       {us = &sp.pr_utime;
		sy = &sp.pr_stime;

		ut = us->tv_sec + us->tv_nsec*1.0e-9;
		st = sy->tv_sec + sy->tv_nsec*1.0e-9;

		ru->ppid   = sp.pr_ppid;
		ru->ut     = ut;
		ru->st     = st;
		ru->maxrss = mby*sp.pr_brksize;};};
	CFREE(fname);

	fname = SC_dsnprintf(TRUE, "/proc/%d/psinfo", pid);
	fd    = open(fname, O_RDONLY);
	if (fd > 0)
	   {nc = sizeof(psinfo_t);
	    nb = read(fd, &si, nc);
	    close(fd);

	    if (nb == nc)
	       SC_strncpy(ru->cmd, MAXLINE, si.pr_psargs, -1);};
	CFREE(fname);

	fname = SC_dsnprintf(TRUE, "/proc/%d/usage", pid);
	fd    = open(fname, O_RDONLY);
	if (fd > 0)
	   {nc = sizeof(prusage_t);
	    nb = read(fd, &su, sizeof(prusage_t));
	    close(fd);

	    if (nb == nc)
	       {ru->minflt   = su.pr_minf;
		ru->majflt   = su.pr_majf;
		ru->nswap    = su.pr_nswap;    /* times a process was swapped out of memory */
		ru->inblock  = su.pr_inblk;    /* block input operations via the file system */
		ru->outblock = su.pr_oublk;    /* block output operations via the file system */
		ru->msgsnd   = su.pr_msnd;     /* IPC messages sent */
		ru->msgrcv   = su.pr_mrcv;     /* IPC messages received */
		ru->nsignals = su.pr_sigs;     /* signals delivered */
		ru->nvcsw    = su.pr_vctx;     /* voluntary context switches */
		ru->nivcsw   = su.pr_ictx;     /* involuntary context switches */
		ru->nsysc    = su.pr_sysc;};}; /* system calls */
	CFREE(fname);

	rv = TRUE;};

#elif defined(MACOSX)

       {int i, ne, na, nr, nc;
	int mib[4];
	size_t sz;
	char *pc, *ps, *pd, *pe;
	double ut, st;
	double flt, pft, scm, scu, msnd, mrcv;
	time_value_t *us, *sy;
	task_t job;
	struct kinfo_proc *kp;
	struct task_basic_info tbi;
	struct task_thread_times_info tti;
	struct task_events_info tei;
	mach_msg_type_number_t tic;
	mach_port_name_t ct;
	static double mby = 1.0/(1024.0*1024.0);

/* get pid and parent pid info */
	mib[0] = CTL_KERN;
	mib[1] = KERN_PROC;
	mib[2] = KERN_PROC_PID;
	mib[3] = pid;

	if (sysctl(mib, 4, NULL, &sz, NULL, 0) == 0)
	   {ne = sz/sizeof(struct kinfo_proc);
	    kp = CMAKE_N(struct kinfo_proc, ne);

	    if (sysctl(mib, 4, kp, &sz, NULL, 0) == 0)
	       {ru->uid  = kp->kp_eproc.e_pcred.p_ruid;
		ru->ppid = kp->kp_eproc.e_ppid;};

	    CFREE(kp);};

/* get command line */
	mib[0] = CTL_KERN;
	mib[1] = KERN_ARGMAX;

	sz = sizeof(na);

	if (sysctl(mib, 2, &na, &sz, NULL, 0) == 0)
	   {if (na > MAX_BFSZ)
	       {pc = CMAKE_N(char,  na);

		mib[0] = CTL_KERN;
		mib[1] = KERN_PROCARGS;
		mib[2] = pid;

		sz = na;
		if (sysctl(mib, 3, pc, &sz, NULL, 0) == 0)
		   {ps = pc;
		    pe = pc + sz;
		    pd = ru->cmd;
		    nr = sizeof(ru->cmd);
		    *pd = '\0';
		    for (i = 0; (ps < pe); i++)
		        {nc = strlen(ps);

			 if ((i != 1) || (strncmp(ps, pd, nc) != 0))
			    SC_vstrcat(pd, nr, "%s ", ps);

		         ps += nc;
			 while ((ps - pe < 0) && (*ps == '\0'))
			    ps++;};};
			     
		CFREE(pc);};};

/* get timing and other run related info */
	job = MACH_PORT_NULL;
	ct  = mach_task_self();
	if (task_for_pid(ct, pid, &job) == KERN_SUCCESS)
	   {tic = TASK_BASIC_INFO_COUNT;
	    task_info(job, TASK_BASIC_INFO, (task_info_t) &tbi, &tic);

	    tic = TASK_THREAD_TIMES_INFO_COUNT;
	    task_info(job, TASK_THREAD_TIMES_INFO, (task_info_t) &tti, &tic);

	    tic = TASK_EVENTS_INFO_COUNT;
	    task_info(job, TASK_EVENTS_INFO, (task_info_t) &tei, &tic);

	    us = &tti.user_time;
	    sy = &tti.system_time;

	    ut = us->seconds + us->microseconds*1.0e-6;
	    st = sy->seconds + sy->microseconds*1.0e-6;

	    ru->ut     = ut;
	    ru->st     = st;
	    ru->maxrss = tbi.resident_size*mby;

	    flt  = tei.faults;
	    pft  = tei.pageins;
	    scm  = tei.syscalls_mach;
	    scu  = tei.syscalls_unix;
	    msnd = tei.messages_sent;
	    mrcv = tei.messages_received;

	    ru->minflt = flt - pft;    /* minor faults */
	    ru->majflt = pft;          /* major faults */
	    ru->msgsnd = msnd;         /* IPC messages sent */
	    ru->msgrcv = mrcv;         /* IPC messages received */
	    ru->nvcsw  = tei.csw;      /* voluntary context switches */
	    ru->nivcsw = tei.csw;      /* involuntary context switches */
	    ru->nsysc  = scm + scu;};  /* system calls */

	rv = TRUE;};

#else

       {int st;
	char **res, *pt, *prss, *puid, *pcmd, *u, *cmd;
        static double mby = 1.0/1024.0;

	cmd = SC_dsnprintf(TRUE, "ps -o uid,time,rss,command -p %d", pid);
	st  = SC_exec(&res, cmd, NULL, -1);
	if (st == 0)
	   {puid = SC_strtok(res[1], " \t\n", u);
	    pt   = SC_strtok(NULL, " \t\n", u);
	    prss = SC_strtok(NULL, " \t\n", u);
	    pcmd = SC_strtok(NULL, "\n", u);

	    ru->uid    = SC_stoi(puid);
	    ru->ut     = SC_str_time(pt);
	    ru->st     = 0.0;
	    ru->maxrss = mby*SC_stoi(prss);  /* reported in kbytes */
	    SC_strncpy(ru->cmd, MAXLINE, pcmd, -1);

	    rv = TRUE;};
	CFREE(cmd);

	SC_free_strings(res);};

#endif

/* if we got other information successfully then fill in the rest */
    if (rv == TRUE)
       {ru->pid      = pid;
	ru->priority = getpriority(PRIO_PROCESS, pid);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_STATR - return memory usage statistics including system resources
 *
 * #bind SC_mem_statr fortran() scheme(memory-rusage) python()
 */

void SC_mem_statr(int64_t *al ARG([*],out), int64_t *fr ARG([*],out),
		  int64_t *df ARG([*],out), int64_t *mx ARG([*],out),
		  int64_t *rs ARG([*],out), int64_t *ov ARG([*],out))
   {int64_t nb;
    SC_heap_des *ph;
    SC_rusedes ru;

    SC_resource_usage(&ru, -1);

    ph = _SC_tid_mm();

    if (al != NULL)
       *al = ph->sp_alloc;

    if (fr != NULL)
       *fr = ph->sp_free;

    if (df != NULL)
       *df = ph->sp_diff;

    if (mx != NULL)
       *mx = ph->sp_max;

    if (rs != NULL)
       *rs = 1.0e3*ru.maxrss;

    if (ov != NULL)
       {nb  = _SC_n_blocks(ph, 7);
	*ov = nb*sizeof(mem_header);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPRSTATR - print memory stats including overhead */

void dprstatr(int ifmt)
   {int64_t al, fr, df, mx, rs, ov;

    SC_mem_statr(&al, &fr, &df, &mx, &rs, &ov);

    if (ifmt == TRUE)
       {printf("Allocated: %ld\n", (long) al);
	printf("Freed:     %ld\n", (long) fr);
	printf("Diff:      %ld\n", (long) df);
	printf("Max Diff:  %ld\n", (long) mx);
	printf("RSS:       %ld\n", (long) rs);
	printf("Overhead:  %ld\n", (long) ov);}
    else
       {printf("Allocated: %10.3e\n", (double) al);
	printf("Freed:     %10.3e\n", (double) fr);
	printf("Diff:      %10.3e\n", (double) df);
	printf("Max Diff:  %10.3e\n", (double) mx);
	printf("RSS:       %10.3e\n", (double) rs);
	printf("Overhead:  %10.3e\n", (double) ov);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SET_RESOURCE_LIMITS - set limits on resource usage
 *                        - return TRUE iff successful
 */

int SC_set_resource_limits(int64_t mem, int64_t cpu, int64_t fsz,
			   int nfd, int nprc)
   {int rv;
    struct rlimit rl;

    rv = 0;

/* set virtual memory usage limit */
    if (mem == -1)
       mem = SC_stoi(getenv("SC_EXEC_RLIMIT_AS"));
    if (mem > 0)
       {rv |= getrlimit(RLIMIT_AS, &rl);
	if (rv == 0)
	   rl.rlim_cur = mem;
	rv |= setrlimit(RLIMIT_AS, &rl);};

/* set CPU time limit */
    if (cpu == -1)
       cpu = SC_stoi(getenv("SC_EXEC_RLIMIT_CPU"));
    if (cpu > 0)
       {rv |= getrlimit(RLIMIT_CPU, &rl);
	if (rv == 0)
	   rl.rlim_cur = cpu;
	rv |= setrlimit(RLIMIT_CPU, &rl);};

/* set file size limit */
    if (fsz == -1)
       fsz = SC_stoi(getenv("SC_EXEC_RLIMIT_FSIZE"));
    if (fsz > 0)
       {rv |= getrlimit(RLIMIT_FSIZE, &rl);
	if (rv == 0)
	   rl.rlim_cur = fsz;
	rv |= setrlimit(RLIMIT_FSIZE, &rl);};

/* set file descriptor limit */
    if (nfd == -1)
       nfd = SC_stoi(getenv("SC_EXEC_RLIMIT_NOFILE"));
    if (nfd > 0)
       {rv |= getrlimit(RLIMIT_NOFILE, &rl);
	if (rv == 0)
	   rl.rlim_cur = nfd;
	rv |= setrlimit(RLIMIT_NOFILE, &rl);};

/* set number of processes limit */
#if !defined(AIX) && !defined(CYGWIN)
    if (nprc == -1)
       nprc = SC_stoi(getenv("SC_EXEC_RLIMIT_NPROC"));
    if (nprc > 0)
       {rv |= getrlimit(RLIMIT_NPROC, &rl);
	if (rv == 0)
	   rl.rlim_cur = nprc;
	rv |= setrlimit(RLIMIT_NPROC, &rl);};
#endif

/* invert the sense */
    rv = (rv == 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_RESOURCE_LIMITS - get limits on resource usage
 *                        - return TRUE iff successful
 */

int SC_get_resource_limits(int64_t *pmem, int64_t *pcpu, int64_t *pfsz,
			   int *pnfd, int *pnprc)
   {int rv;
    struct rlimit rl;

    rv = 0;

/* get virtual memory usage limit */
    if (pmem != NULL)
       {rv |= getrlimit(RLIMIT_AS, &rl);
	if (rv == 0)
	   *pmem = rl.rlim_cur;
	else
	   *pmem = -1;};

/* get CPU time limit */
    if (pcpu != NULL)
       {rv |= getrlimit(RLIMIT_CPU, &rl);
	if (rv == 0)
	   *pcpu = rl.rlim_cur;
	else
	   *pcpu = -1;};

/* get file size limit */
    if (pfsz != NULL)
       {rv |= getrlimit(RLIMIT_FSIZE, &rl);
	if (rv == 0)
	   *pfsz = rl.rlim_cur;
	else
	   *pfsz = -1;};

/* get file descriptor limit */
    if (pnfd != NULL)
       {rv |= getrlimit(RLIMIT_NOFILE, &rl);
	if (rv == 0)
	   *pnfd = rl.rlim_cur;
	else
	   *pnfd = -1;};

/* get number of processes limit */
#if !defined(AIX) && !defined(CYGWIN)
    if (pnprc != NULL)
       {rv |= getrlimit(RLIMIT_NPROC, &rl);
	if (rv == 0)
	   *pnprc = rl.rlim_cur;
	else
	   *pnprc = -1;};
#else
    if (pnprc != NULL)
       *pnprc = -1;
#endif

/* invert the sense */
    rv = (rv == 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
