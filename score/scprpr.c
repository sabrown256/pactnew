/*
 * SCPRPR.C - monitor process progress and performance
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"

#define DISP_BYNAME   1
#define DISP_BYUID    2
#define DISP_ALL      3
#define DISP_TIME     4

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* SHOW_TIME - display times */

static void show_time(char *s, int nc, SC_rusedes *ru, int tmod, int label)
   {char ut[MAXLINE], st[MAXLINE];
    double vut, vst, vct;

    if (label == TRUE)
       {switch (tmod)
	   {case 1:
	         snprintf(s, nc, "  UserTime");
	         break;
	    case 2:
	         snprintf(s, nc, "   SysTime");
	         break;
	    case 3:
	         snprintf(s, nc, "      Time");
	         break;
	    case 4:
	         snprintf(s, nc, "  UserTime    SysTime");
	         break;};}
    else
       {vut = ru->ut;
	vst = ru->st;
	vct = vut + vst;

	switch (tmod)
	   {case 1:
	         SC_sec_str(vut, ut, MAXLINE);
	         snprintf(s, nc, "%10s", ut);
	         break;
	    case 2:
		 SC_sec_str(vst, st, MAXLINE);
	         snprintf(s, nc, "%10s", st);
	         break;
	    case 3:
		 SC_sec_str(vct, ut, MAXLINE);
	         snprintf(s, nc, "%10s", ut);
	         break;
	    case 4:
	         SC_sec_str(vut, ut, MAXLINE);
		 SC_sec_str(vst, st, MAXLINE);
	         snprintf(s, nc, "%10s %10s", ut, st);
	         break;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* SHOW_UNAME - display uid by name or by number */

void show_uname(char *s, int nc, SC_rusedes *ru, int umod)
   {

    if (umod == 1)
       snprintf(s, nc, "  %8d", ru->uid);

    else
       SC_get_uname(s, nc, ru->uid);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* SHOW_BYNAME - display jobs by name */

int show_byname(SC_rusedes *ru, int tmod)
   {int err;
    char t[MAXLINE], u[MAXLINE];

    ONCE_SAFE(TRUE, NULL)
       show_time(t, MAXLINE, ru, tmod, TRUE);
       io_printf(stdout,
		 "       USER      PID %s  Pr Mem(MBy)  Command\n", t);
    END_SAFE;

    err = 0;
    show_time(t, MAXLINE, ru, tmod, FALSE);
    show_uname(u, MAXLINE, ru, 2);

    io_printf(stdout, " %s %8d %s %3d %8.1f  %s\n",
	      u,
	      ru->pid,
	      t,
	      ru->priority,
	      ru->maxrss,
	      ru->cmd);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* SHOW_BYUID - display jobs by UID */

int show_byuid(SC_rusedes *ru, int tmod)
   {int err;
    char t[MAXLINE], u[MAXLINE];

    ONCE_SAFE(TRUE, NULL)
       show_time(t, MAXLINE, ru, tmod, TRUE);
       io_printf(stdout,
		 "        UID      PID %s  Pr Mem(Mby)  Command\n", t);
    END_SAFE;

    err = 0;
    show_time(t, MAXLINE, ru, tmod, FALSE);
    show_uname(u, MAXLINE, ru, 1);

    io_printf(stdout, " %s %8d %s %3d %8.1f  %s\n",
	      u,
	      ru->pid,
	      t,
	      ru->priority,
	      ru->maxrss,
	      ru->cmd);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* SHOW_ALL_INFO - display all job info */

int show_all_info(SC_rusedes *ru, int tmod)
   {int err;
    char t[MAXLINE], u[MAXLINE];

    ONCE_SAFE(TRUE, NULL)
       show_time(t, MAXLINE, ru, tmod, TRUE);
       io_printf(stdout, "        UID      PID     PPID %s  Pr Mem(Mby)", t);
       io_printf(stdout, "   MinFlt   MajFlt");
       io_printf(stdout, "     NSys     NSig     NSwp");
       io_printf(stdout, "     NSnd     NRcv");
       io_printf(stdout, "     NCSV     NCSI");
       io_printf(stdout, "     NBin     NBout");
       io_printf(stdout, "\n");
    END_SAFE;

    err = 0;
    show_time(t, MAXLINE, ru, tmod, FALSE);
    show_uname(u, MAXLINE, ru, 2);

    io_printf(stdout, " %s %8d %8d %s %3d %8.1f",
	      u,
	      ru->pid,
	      ru->ppid,
	      t,
	      ru->priority,
	      ru->maxrss);

    io_printf(stdout, " %8.0f %8.0f",
	      ru->minflt,
	      ru->majflt);

    io_printf(stdout, " %8.0f %8.0f %8.0f",
	      ru->nsysc,
	      ru->nsignals,
	      ru->nswap);

    io_printf(stdout, " %8.0f %8.0f",
	      ru->msgsnd,
	      ru->msgrcv);

    io_printf(stdout, " %8.0f %8.0f",
	      ru->nvcsw,
	      ru->nivcsw);

    io_printf(stdout, " %8.0f %8.0f",
	      ru->inblock,
	      ru->outblock);

    io_printf(stdout, "\n");

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* SHOW_TIME_INFO - display time job info */

int show_time_info(SC_rusedes *ru, int tmod, int pid)
   {int err;
    double tc, dt, dut, dst, dmem, idt;
    char t[MAXLINE], u[MAXLINE];
    static int count = 0;
    static double wcr, wct, usrt, syst, mem, since, idlet;

    if (count == 0)
       {if (pid != -1)
	   {wcr   = SC_wall_clock_time();
	    wct   = 0.0;
	    usrt  = 0.0;
	    syst  = 0.0;
	    mem   = 0.0;
	    since = 0.0;
	    idlet = 0.0;};

	show_time(t, MAXLINE, ru, tmod, TRUE);
	io_printf(stdout, "     UNAME");
	io_printf(stdout, "     Wall     Idle    Since");
	io_printf(stdout, "     User   dUser");
	io_printf(stdout, "      Sys    dSys");
	io_printf(stdout, "      Mem    dMem");
	io_printf(stdout, "\n");};

    err = 0;
    show_uname(u, MAXLINE, ru, 2);

    tc = SC_wall_clock_time() - wcr;
    dt = tc - wct;

/* compute the rates */
    if ((dt != 0.0) && (pid != -1) && (count != 0))
       {idt  = 1.0/dt;
	dut  = (ru->ut - usrt)*idt;
	dst  = (ru->st - syst)*idt;
	dmem = (ru->maxrss - mem)*idt;}
    else
       {dut  = 0.0;
	dst  = 0.0;
	dmem = 0.0;};

    wct  = tc;
    usrt = ru->ut;
    syst = ru->st;
    mem  = ru->maxrss;
    if ((dut != 0.0) || (dst != 0.0))
       since = 0.0;
    else
       {since += dt;
	idlet += dt;};

    io_printf(stdout, "%s", u);

    io_printf(stdout, " %8.1f %8.1f %8.2f",
	      wct, idlet, since);

    io_printf(stdout, " %8.1f %7.2f",
	      usrt, dut);

    io_printf(stdout, " %8.1f %7.2f",
	      syst, dst);

    io_printf(stdout, " %8.1f %7.2f",
	      mem, dmem);

    io_printf(stdout, "\n");

    count++;

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PS - run a ps type of monitoring */

int ps(char *ptrn, int pid, int tmod, int all, int mode, int nc, int dt)
   {int i, ic, err, mid, uid;
    int match, nm, np;
    int *ids;
    SC_rusedes ru;

    uid = getuid();
    mid = getpid();

    err = 0;

    if (nc == 1)
       dt = 0;
    else if (nc == -1)
       nc = INT_MAX;

    for (ic = 0; ic < nc; ic++)
        {ids = SC_process_ids();
	 if (ids == NULL)
	    continue;

	 nm = 0;
	 np = 0;
	 for (i = 0; ids[i] != -1; i++)
	     {np++;
	      err = SC_resource_usage(&ru, ids[i]);

	      match = FALSE;
	      if (pid == -1)
		 {if (ptrn != NULL)
		     match = ((SC_regx_match(ru.cmd, ptrn) == TRUE) ||
			      (SC_strstr(ru.cmd, ptrn) != NULL));
		  else
		     match = ((all == TRUE) || (uid == ru.uid));};

	      match |= (pid == ids[i]);
	      match &= (mid != ids[i]);

	      if (match == TRUE)
		 {nm++;
		  switch (mode)
		     {case DISP_BYNAME:
			   show_byname(&ru, tmod);
			   break;
		      case DISP_BYUID:
			   show_byuid(&ru, tmod);
			   break;
		      case DISP_ALL:
			   show_all_info(&ru, tmod);
			   break;
		      case DISP_TIME:
			   show_time_info(&ru, tmod, pid);
			   break;};};};
        
	 CFREE(ids);

/* if no jobs matched the constraints do not repeat */
	 if (nm == 0)
	    break;

	 SC_sleep(dt);};

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SHOW_HELP - show the usage and options */

static void show_help(void)
   {

    io_printf(stdout, "Usage: propr [-a] [-byuid] [-c #] [-dt <msec>] [-ia] [-h]\n");
    io_printf(stdout, "             [-p <pid>] [-t <mode>] [<patrn>]\n\n");

    io_printf(stdout, "   a      list all jobs\n");
    io_printf(stdout, "   byuid  list jobs by user id instead of by name\n");
    io_printf(stdout, "   c      list # times (default 1)\n");
    io_printf(stdout, "   dt     sleep <msec> between lists (default 0)\n");
    io_printf(stdout, "   ia     display all job info\n");
    io_printf(stdout, "   it     display job timing info\n");
    io_printf(stdout, "   h      this help message\n");
    io_printf(stdout, "   p      list only process <pid>\n");
    io_printf(stdout, "   t      show times as follows:\n");
    io_printf(stdout, "             1 just user time\n");
    io_printf(stdout, "             2 just system time\n");
    io_printf(stdout, "             3 single user+system time\n");
    io_printf(stdout, "             4 both user and system time\n");
    io_printf(stdout, "  <patrn> list processes whose command line matches\n");
    io_printf(stdout, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* MAIN - start here */

int main(int c, char **v)
   {int i, err;
    int all, mode, pid, tmod;
    int dt, count;
    char *patrn;

    all   = FALSE;
    mode  = DISP_BYNAME;
    pid   = -1;
    tmod  = 3;
    count = 1;
    dt    = 1000;
    patrn = NULL;
    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'a':
                      all = TRUE;
                      break;
                 case 'b':
                      if (strcmp(v[i], "-byuid") == 0)
 			 mode = DISP_BYUID;
                      break;
                 case 'c':
		      count = SC_stoi(v[++i]);
                      break;
                 case 'd':
		      dt = SC_stoi(v[++i]);
                      break;
                 case 'h':
                      show_help();
                      return(1);
                 case 'i':
		      if (strcmp(v[i], "-ia") == 0)
			 mode = DISP_ALL;
		      else if (strcmp(v[i], "-it") == 0)
			 {mode = DISP_TIME;
			  tmod = 4;};
                      break;
		 case 'p':
                      pid = SC_stoi(v[++i]);
                      break;
		 case 't':
                      tmod = SC_stoi(v[++i]);
                      break;
		 default:
		      io_printf(stdout, "Unknown option: %s\n", v[i]);
		      return(1);};}
         else
	    {patrn = v[i];
	     break;};};

    err = ps(patrn, pid, tmod, all, mode, count, dt);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
