/*
 * SCRSCA.C - routines involving resource usage
 *          - highly conditionalized
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"

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

/* _SC_RES_USAGE_SELF - fill RU with current resource usages
 *                    - measured from the first call
 *                    - return TRUE iff useful data was available
 *                    - zero out RU otherwise
 *                    - if WHCH is FALSE do process only
 *                    - if WHCH is TRUE do children too
 */

static int _SC_res_usage_self(SC_rusedes *ru, int whch)
   {int rv;

#ifdef HAVE_RESOURCE_USAGE

    int who;
    struct rusage r, *pr;
    static struct rusage rs, rc;

    if (whch == FALSE)
       {who = RUSAGE_SELF;
	pr  = &rs;}
    else
       {who = RUSAGE_CHILDREN;
	pr  = &rc;};

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
	ru->maxrss   = r.ru_maxrss   - pr->ru_maxrss;
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
	ru->nivcsw   = r.ru_nivcsw   - pr->ru_nivcsw;}

    else
       {ru->ut       = 0.0;
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
	ru->nivcsw   = 0.0;};

    rv = (rv == 0);

#else

    rv = FALSE;

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

    memset(ru, 0, sizeof(SC_rusedes));

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
	SFREE(fname);

	fname = SC_dsnprintf(TRUE, "/proc/%d/cmdline", pid);
	fd    = open(fname, O_RDONLY);
	if (fd > 0)
	   {nc = read(fd, s, MAXLINE);
	    close(fd);
	    if (nc > 0)
	       SC_strncpy(ru->cmd, MAXLINE, s, -1);};
	SFREE(fname);

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
	SFREE(fname);

	fname = SC_dsnprintf(TRUE, "/proc/%d/psinfo", pid);
	fd    = open(fname, O_RDONLY);
	if (fd > 0)
	   {nc = sizeof(psinfo_t);
	    nb = read(fd, &si, nc);
	    close(fd);

	    if (nb == nc)
	       SC_strncpy(ru->cmd, MAXLINE, si.pr_psargs, -1);};
	SFREE(fname);

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
	SFREE(fname);

	fname = SC_dsnprintf(TRUE, "/proc/%d/psinfo", pid);
	fd    = open(fname, O_RDONLY);
	if (fd > 0)
	   {nc = sizeof(psinfo_t);
	    nb = read(fd, &si, nc);
	    close(fd);

	    if (nb == nc)
	       SC_strncpy(ru->cmd, MAXLINE, si.pr_psargs, -1);};
	SFREE(fname);

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
	SFREE(fname);

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
	    kp = FMAKE_N(struct kinfo_proc, ne, "SC_RESOURCE_USAGE:kpa");

	    if (sysctl(mib, 4, kp, &sz, NULL, 0) == 0)
	       {ru->uid  = kp->kp_eproc.e_pcred.p_ruid;
		ru->ppid = kp->kp_eproc.e_ppid;};

	    SFREE(kp);};

/* get command line */
	mib[0] = CTL_KERN;
	mib[1] = KERN_ARGMAX;

	sz = sizeof(na);

	if (sysctl(mib, 2, &na, &sz, NULL, 0) == 0)
	   {if (na > MAX_BFSZ)
	       {pc = FMAKE_N(char,  na, "SC_RESOURCE_USAGE:pc");

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
			 while ((ps != pe) && (*ps == '\0'))
			    ps++;};};
			     
		SFREE(pc);};};

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
	SFREE(cmd);

	SC_free_strings(res);};

#endif

/* if we got other information successfully then fill in the rest */
    if (rv == TRUE)
       {ru->pid      = pid;
	ru->priority = getpriority(PRIO_PROCESS, pid);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
