/*
 * SCHSRU.C - stand alone host-server
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "scope_proc.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EXEC_CMD - execute the given command CMD on all hosts of type SYS */

static int _SC_exec_cmd(char **cmd, char *sys,
			int exc, int vrb, int to, int notme)
   {int i, n, rv;
    char hst[MAXLINE];
    char *cm, *flt, *ca[5], *dr[5], **sa;

#ifdef MAKE_FILTER_FILE
    flt = MAKE_FILTER_FILE;
#else
    flt = NULL;
#endif

    cm = NULL;
    for (n = 0; cmd[n] != NULL; n++)
        {cm = SC_dstrcat(cm, cmd[n]);
	 cm = SC_dstrcat(cm, " ");};

    SC_LAST_CHAR(cm) = '\0';

    if (vrb == TRUE)
       {io_printf(stdout, "\n");
        switch (exc)
	   {case 1 :
	         if (sys[0] == '@')
		    io_printf(stdout, "Executing '%s' on all hosts %s:\n", cm, sys);
	         else if (sys[0] == '+')
		    io_printf(stdout, "Executing '%s' on all hosts from %s:\n",
			      cm, sys+1);
		 else if (strcmp(sys, "all") == 0)
		    io_printf(stdout, "Executing '%s' on all known hosts:\n", cm);
		 else
		    io_printf(stdout, "Executing '%s' on hosts of type %s:\n",
			      cm, sys);
		 break;
	    case 2 :
	         if (sys[0] == '@')
		    io_printf(stdout, "Executing '%s' on all types %s:\n", cm, sys);
	         else if (sys[0] == '+')
		    io_printf(stdout, "Executing '%s' on all types from %s:\n",
			      cm, sys+1);
		 else if (strcmp(sys, "all") == 0)
		    io_printf(stdout, "Executing '%s' on all known types:\n", cm);
		 else
		    io_printf(stdout, "Executing '%s' on type(s) %s:\n",
			      cm, sys);
		 break;};};

    rv = 0;

/* make the host list */
    sa = SC_get_host_list(sys, exc-1);

    if (notme == TRUE)
       {gethostname(hst, MAXLINE);
	SC_ptr_arr_len(n, sa);
	for (i = 0; i < n; i++)
	    {if (strcmp(sa[i], hst) == 0)
	        {SFREE(sa[i]);
		 sa[i] = sa[n-1];
		 sa[--n] = NULL;
		 break;};};};

/* make the command list */
    ca[0] = cm;
    ca[1] = NULL;

/* make the directory list */
    dr[0] = "~";
    dr[1] = NULL;

    SC_exec_async_h(NULL, NULL, sa, ca, dr, -1, flt, 1, 0, 1);

    SC_free_strings(sa);

    SFREE(cm);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LIST - list the hosts associated with SYS */

static int _SC_list(char *sys, int vrb)
   {int i, n, st;
    char out[MAXLINE];
    char **sa;

    st = -1;

    if (vrb == TRUE)
       {io_printf(stdout, "\n");
	io_printf(stdout, "Listing available hosts of type %s\n", sys);};

    if ((sys[0] == '@') || (sys[0] == '+') || (strcmp(sys, "all") == 0))

/* make the host list */
       {sa = SC_get_host_list(sys, 0);
	SC_ptr_arr_len(n, sa);
	for (i = 0; i < n; i++)
	    io_printf(stdout, "   %s\n", sa[i]);
	SFREE(sa);}

    else

/* get the number of hosts for this system type */
       {n = SC_get_nhosts(sys);

/* check each host of this type */
	for (i = 0; i < n; i++)
	    {st = SC_host_server_query(out, MAXLINE, "%s %d", sys, i);
	     io_printf(stdout, "   %s\n", out);};};
	 
    if (vrb == TRUE)
       io_printf(stdout, "\n");

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_NUMBER - report the number of nodes for the specified type */

static int _SC_number(char *sys, int vrb)
   {int nhst;

    if (vrb == TRUE)
       {io_printf(stdout, "\n");
	io_printf(stdout, "# of available hosts of type %s:\n", sys);};

    nhst = SC_get_nhosts(sys);

    io_printf(stdout, "   %d\n", nhst);

    if (vrb == TRUE)
       io_printf(stdout, "\n");

    return(nhst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_TYPES - list the available system types */

static int _SC_types(int vrb)
   {int i, n;
    char *s, **tp;

    if (vrb == TRUE)
       {io_printf(stdout, "\n");
	io_printf(stdout, "Available host types:\n");};

    tp = SC_get_host_types(1, NULL);
    SC_ptr_arr_len(n, tp);

/* check each host of this type */
    for (i = 0; i < n; i++)
        {s = tp[i];
	 io_printf(stdout, "   %-16s\n", s);};

    if (vrb == TRUE)
       io_printf(stdout, "\n");

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_VERIFY - verify that the hosts can be successfully logged into
 *            - take no more than TO milliseconds for any given host
 */

static int _SC_verify(char *sys, char *host, int vrb, int to)
   {int i, nh, st;
    double t0, dt;
    char *hst, **sh;

    st = -1;

    if (host == NULL)
       {if (vrb == TRUE)
	   {io_printf(stdout, "\n");
	    if (sys[0] == '@')
	       io_printf(stdout, "Verifying available hosts %s (%.3f sec):\n",
			 sys, (to/1000.0));
	    else if (strcmp(sys, "all") == 0)
	       io_printf(stdout, "Verifying available hosts (%.3f sec):\n",
			 (to/1000.0));
	    else
	       io_printf(stdout, "Verifying available hosts of type %s (%.3f sec):\n",
			 sys, (to/1000.0));};

	sh = SC_get_host_list(sys, FALSE);
	SC_ptr_arr_len(nh, sh);

	for (i = 0; i < nh; i++)
	    {hst = sh[i];

	     t0 = SC_wall_clock_time();
	     st = SC_verify_host(hst, to);
	     dt = SC_wall_clock_time() - t0;

	     if ((st < -1) || (st == 0))
	        io_printf(stdout, "   %-16s denied  (%.2f)\n", hst, dt);
	     else if (st == -1)
	        io_printf(stdout, "   %-16s down    (%.2f)\n", hst, dt);
	     else if (st == 1)
	        io_printf(stdout, "   %-16s ok      (%.2f)\n", hst, dt);
	     else if (st == 2)
	        io_printf(stdout, "   %-16s added   (%.2f)\n", hst, dt);};

	SC_free_strings(sh);}

    else
       {t0 = SC_wall_clock_time();
	st = SC_verify_host(host, to);
	dt = SC_wall_clock_time() - t0;

	if (st == -1)
	   io_printf(stdout, "   %-16s down    (%.2f)\n", host, dt);
	else if (st == 0)
	   io_printf(stdout, "   %-16s denied  (%.2f)\n", host, dt);
	else if (st == 1)
	   io_printf(stdout, "   %-16s ok      (%.2f)\n", host, dt);
	else if (st == 2)
	   io_printf(stdout, "   %-16s added   (%.2f)\n", host, dt);};

    if (vrb == TRUE)
       io_printf(stdout, "\n");

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MATCH - given a host name or a system type specification in NAME
 *           - print the name of an accessible host
 *           - iff one responds in TO milliseconds
 *           - otherwise print "-none-"
 */

static int _SC_match(char *name, int to)
   {int i, n, st;
    char out[MAXLINE];

/* loop over hosts here because SC_host_server_query does not do full
 * authentication and can therefore succeed even though SC_verify_host
 * which does authenticate might subsequently fail
 */
    n = SC_get_nhosts(name);
    for (i = 0, st = 0; (i < n) && (st < 1); i++)
        {st = SC_host_server_query(out, MAXLINE, "%s %d", name, -1);
	 if (st == TRUE)
	    st = SC_verify_host(out, to);};

    if (st <= 0)
       {st = -1;
	io_printf(stdout, "-none-\n");}
    else
       io_printf(stdout, "%s\n", out);

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SYS_HOST - given a comma delimited list of system types LST
 *              - find a host for each system type
 *              - the list "-" means all hosts in .net
 */

static int _SC_sys_host(char *lst, int vrb)
   {int i, n, rv, st;
    char s[MAXLINE], out[MAXLINE];
    char *ps, *t, *p, *sys[100], **tp;

    if (lst[0] == '-')
       {tp = SC_get_host_types(0, NULL);

/* check each host of this type */
	for (n = 0; TRUE; n++)
	    {ps = tp[n];
	     if (ps == NULL)
	        break;
	     else
	        {t = strchr(ps, '@');
		 if (t != NULL)
		    *t = '\0';
		 sys[n] = ps;};};}

    else
       {ps = s;
	strcpy(s, lst);
	for (n = 0; TRUE; n++)
	    {t = SC_strtok(ps, ",", p);
	     if (t == NULL)
	        break;
	     sys[n] = t;
	     ps     = NULL;};};

    if (vrb == TRUE)
       {io_printf(stdout, "\n");
	io_printf(stdout, "Representative host for system types:\n");};

    rv = TRUE;
    for (i = 0; i < n; i++)
        {ps = sys[i];
	 st = SC_host_server_query(out, MAXLINE, ps);
         if (st == TRUE)
	    io_printf(stdout, "   %-16s   %s\n", ps, out);};

    if (vrb == TRUE)
       io_printf(stdout, "\n");

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_QUERY - runs infinite read/eval/print loop
 *           - expects system type or host name for input
 *           - returns name of host that can be sent work
 */

static int _SC_query(void)
   {char s[MAXLINE], out[MAXLINE];
    char *t;

    while (TRUE)
       {io_gets(s, MAXLINE, stdin);
        t = strtok(s, "\n");

	if (strcmp(s, "quit") == 0)
           break;

	else
	   {SC_host_server_query(out, MAXLINE, s);
	    io_printf(stdout, "%s\n", out);};};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* USAGE - show the usage and options */

static void usage(void)
   {

    io_printf(stdout, "\n");
    io_printf(stdout, "Usage: hserve [-dt #] [-exh <command>] [-ext <command>] [-f <host-db>] [-h] [-lst]\n");
    io_printf(stdout, "              [-m <name>] [-me] [-n] [-q] [-sys <list>] [-t <type>] [-typ] [-vfy]\n");
    io_printf(stdout, "\n");
    io_printf(stdout, "   dt   - number of milliseconds to wait for hosts (default 5000)\n");
    io_printf(stdout, "   exh  - exec by host - all hosts of the specified type(s)\n");
    io_printf(stdout, "   ext  - exec by type - one host of each specified type(s)\n");
    io_printf(stdout, "   f    - use specified file for host database\n");
    io_printf(stdout, "   h    - this help message\n");
    io_printf(stdout, "   lst  - list all hosts of the specified type\n");
    io_printf(stdout, "   m    - report a host that matches a host or type specification\n");
    io_printf(stdout, "   me   - exclude the current host when executing commands\n");
    io_printf(stdout, "   n    - report number of hosts of the specified type\n");
    io_printf(stdout, "   q    - quiet mode, just the facts\n");
    io_printf(stdout, "   sys  - list a host for each system type in comma delimited <list>\n");
    io_printf(stdout, "   t    - perform queries for specified system type (defaults to current host type)\n");
    io_printf(stdout, "   typ  - list known system types\n");
    io_printf(stdout, "   vfy  - verify that you can login to all hosts of the specified type\n");
    io_printf(stdout, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start here */

int main(int c, char **v)
   {int i, st, to, exc, lst, num, typ, vfy, vrb, notme;
    char *s, *p, *file, *host, *name, *sys, *sysl;

    p = getenv("SC_EXEC_TIME_VERIFY");
    if (p == NULL)
       to = 5000;
    else
       to = SC_stoi(p);

    st    = 1;
    exc   = FALSE;
    lst   = FALSE;
    num   = FALSE;
    typ   = FALSE;
    vfy   = FALSE;
    vrb   = TRUE;
    notme = FALSE;
    file  = NULL;
    host  = NULL;
    name  = NULL;
    sys   = SYSTEM_ID;
    sysl  = NULL;
    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-dt") == 0)
	    to = SC_stoi(v[++i]);

         else if ((strcmp(v[i], "-exec") == 0) ||
		  (strcmp(v[i], "-exh") == 0))
	    {exc = 1;
	     lst = FALSE;
	     num = FALSE;
	     typ = FALSE;
	     vfy = FALSE;
	     i++;
	     break;}

         else if (strcmp(v[i], "-ext") == 0)
	    {exc = 2;
	     lst = FALSE;
	     num = FALSE;
	     typ = FALSE;
	     vfy = FALSE;
	     i++;
	     break;}

	 else if (strcmp(v[i], "-f") == 0)
	    file = v[++i];

	 else if (strcmp(v[i], "-h") == 0)
	    {usage();
	     return(1);}

	 else if (strcmp(v[i], "-sys") == 0)
	    sysl = v[++i];

         else if (strcmp(v[i], "-lst") == 0)
	    {exc = FALSE;
	     lst = TRUE;
	     num = FALSE;
	     typ = FALSE;
	     vfy = FALSE;}

         else if (strcmp(v[i], "-m") == 0)
	    name = v[++i];

         else if (strcmp(v[i], "-me") == 0)
	    notme = TRUE;

         else if (strcmp(v[i], "-n") == 0)
	    {exc = FALSE;
	     lst = FALSE;
	     num = TRUE;
	     typ = FALSE;
	     vfy = FALSE;}

	 else if (strcmp(v[i], "-q") == 0)
	    vrb = FALSE;

         else if (strcmp(v[i], "-typ") == 0)
	    {exc = FALSE;
	     lst = FALSE;
	     num = FALSE;
	     typ = TRUE;
	     vfy = FALSE;}

	 else if (strcmp(v[i], "-t") == 0)
	    sys = v[++i];

	 else if (strcmp(v[i], "-vfy") == 0)
	    {exc = FALSE;
	     lst = FALSE;
	     num = FALSE;
	     typ = FALSE;
	     vfy = TRUE;}

	 else if (v[i][0] == '-')
	    {io_printf(stdout, "Unknown option: %s\n", v[i]);
	     return(1);}

	 else
	    host = v[i];};

/* push the timeout time out into the environment variable */
    s = SC_dsnprintf(FALSE, "SC_EXEC_TIME_VERIFY=%d", to);
    SC_putenv(s);

    if (file == NULL)
       file = getenv("HOST_SERVER_DB");

    st = SC_host_server_init(file, FALSE, vrb);
    if (st == FALSE)
       {if (file == NULL)
           io_printf(stdout, "No HOST_SERVER_DB specified - exiting\n");
        else
           io_printf(stdout, "Unable to use %s for HOST_SERVER_DB - exiting\n",
		     file);
        return(1);};

    if (name != NULL)
       st = _SC_match(name, to);
    else if (sysl != NULL)
       st = _SC_sys_host(sysl, vrb);
    else if (exc != FALSE)
       st = _SC_exec_cmd(v+i, sys, exc, vrb, to, notme);
    else if (lst == TRUE)
       st = _SC_list(sys, vrb);
    else if (num == TRUE)
       st = _SC_number(sys, vrb);
    else if (typ == TRUE)
       st = _SC_types(vrb);
    else if (vfy == TRUE)
       st = _SC_verify(sys, host, vrb, to);
    else
       st = _SC_query();

    SC_host_server_fin();

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
