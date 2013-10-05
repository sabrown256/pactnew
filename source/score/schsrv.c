/*
 * SCHSRV.C - host-server routines
 *          - handle queries about hosts
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "score_int.h"
#include "scope_proc.h"

typedef struct s_host_server host_server;
typedef struct s_hrng hrng;

struct s_hrng
   {char **hosts;
    int nhosts;
    int current;
    int noping;};

struct s_host_server
   {PROCESS *pp;
    hasharr *table;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MAKE_RANGE - produces an array of strings for a range of node names */

static hrng *_SC_make_range(int n, char **hosts)
   {hrng *rng;

    rng = CMAKE(hrng);

    rng->hosts   = hosts;
    rng->nhosts  = n;
    rng->current = SC_random_int(0, n);
    rng->noping  = FALSE;

    return(rng);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_RANGE - produces an array of strings for a range of node names */

static hrng *_SC_range(char *base, int mn, int mx)
   {int i, ns;
    char *s, **strs;
    SC_array *arr;
    hrng *rng;

    arr = SC_STRING_ARRAY();

    for (i = mn; i <= mx; i++)
        {s = SC_dsnprintf(TRUE, "%s%d", base, i);
	 SC_array_string_add(arr, s);};

    ns   = SC_array_get_n(arr);
    strs = SC_array_done(arr);

    rng = _SC_make_range(ns, strs);

    return(rng);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LIST - tokens a string and return an array of names */

static hrng *_SC_list(char *s)
   {int ns;
    char **strs;
    hrng *rng;

    strs = SC_tokenize(s, " ()\n");
    SC_ptr_arr_len(ns, strs);

    rng = _SC_make_range(ns, strs);

    return(rng);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PING_HOST - see if HOST is really available
 *               - allow TO milliseconds for HOST to respond
 *               - return TRUE if available
 */

int _SC_ping_host(char *host, int to, int fm)
   {int fd, st;
    char hst[MAXLINE];
    char *ph;

    st = FALSE;

    if (host == NULL)
       {gethostname(hst, MAXLINE);
	ph = hst;}
    else
       ph = host;

    fd = SC_open_port(ph, 22, to, fm);

    if (fd > 0)
       {SC_close_safe(fd);
	st = TRUE;}
    else
       st = FALSE;

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_UNPINGABLE - return TRUE if the net suffix of TYPE is a token
 *                - of PNGLST
 */

static int _SC_unpingable(char *type, char *pnglst)
   {int rv;
    char t[MAXLINE], lst[MAXLINE];
    char *net, *s;

    rv = FALSE;
    if ((type != NULL) && (pnglst != NULL))
       {strcpy(t, type);
	strcpy(lst, pnglst);

/* get the net suffix */
	SC_firsttok(t, "@");
	net = SC_firsttok(t, "\n");
	if (net != NULL)
	   {while (TRUE)
	       {s = SC_firsttok(lst, ", \t\n");
		if (s == NULL)
		   break;
		else if (strcmp(s, net) == 0)
		   {rv = TRUE;
		    break;};};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_HOST_SERVER_LOOKUP - lookup the system type name
 *                        - with or without network qualification
 *                        - and return the associated host list
 */

static hrng *_SC_host_server_lookup(char *type, hasharr *tab)
   {long i;
    char *s, *net;
    hrng *h;

    if (type == NULL)
       type = SYS_Arch;

    if (strcmp(type, "all") == 0)
       {h = NULL;
	i = 0;
	SC_hasharr_next(tab, &i, NULL, NULL, (void **) &h);}

    else
       {h = (hrng *) SC_hasharr_def_lookup(tab, type);
	if (h == NULL)
	   {net = SC_hasharr_def_lookup(tab, ".net");
	    s = SC_dsnprintf(FALSE, "%s@%s", type, net);
	    h = (hrng *) SC_hasharr_def_lookup(tab, s);};};

    return(h);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_GET_NEXT_HOST - server side worker routine which maps X
 *                   - into an available host if it possibly can
 *                   - return TRUE iff successful
 */

static int _SC_get_next_host(char *out, int nc,
			     char *type, hasharr *tab, int id)
   {int ih, ic, nh, na, to, st;
    char *host, **hl;
    hrng *h;

    st = 0;
    if (type != NULL)
       {host = NULL;
	to   = 1000;
	if (tab != NULL)
	   {h = _SC_host_server_lookup(type, tab);
	    if (h != NULL)
	       {hl = h->hosts;
		nh = h->nhosts;

/* search for any host that responds */
		if (id < 0)
		   {for (ih = 0, st = FALSE; (st != TRUE) && (ih < nh); ih++)
		        {ic = h->current;
			 h->current = (ic + 1) % nh;

			 host = hl[ic];

			 if (h->noping == TRUE)
			    st = TRUE;
			 else
			    st = _SC_ping_host(host, to, FALSE);};}
	       
/* find requested host only */
		else
		   {id  %= nh;
		    host = hl[id];
		    st   = _SC_ping_host(host, to, FALSE);};};};

/* if we don't have a table the best we can do
 * is to see if TYPE is a valid host
 */
	if (host == NULL)
	   {st   = _SC_ping_host(type, to, TRUE);
	    host = (st == TRUE) ? type : "-none-";};

	na = strlen(host);
	if (na < nc)
	   snprintf(out, nc, "%s", host);};

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_GET_NHOSTS - server side worker routine which
 *                - returns the number of hosts possible for system TYPE
 */

static int _SC_get_nhosts(char *type, hasharr *tab)
   {int nh;
    hrng *h;

    nh = 1;
    if ((type != NULL) && (tab != NULL))
       {if (strcmp(type, "all") == 0)
	   nh = SC_hasharr_get_n(tab);
	else
	   {h = _SC_host_server_lookup(type, tab);
	    if (h != NULL)
	       nh = h->nhosts;};};

    return(nh);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_HOST_SERVER_REL - free element in host server table */

static int _SC_host_server_rel(haelem *hp, void *a)
   {int i, n, ok;
    char *type, **hst;
    void *v;
    hrng *rng;

    ok = SC_haelem_data(hp, NULL, &type, &v, TRUE);
    SC_ASSERT(ok == TRUE);

    if (strcmp(type, "hrng") == 0)
       {rng = (hrng *) v;
	hst = rng->hosts;
	n   = rng->nhosts;

	for (i = 0; i < n; i++)
	    CFREE(hst[i]);

	CFREE(hst);
	CFREE(v);}

    else if (strcmp(type, SC_STRING_S) == 0)
       CFREE(v);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HOST_SERVER_FIN - shutdown and cleanup the host server
 *                    - return TRUE iff successful
 */

int SC_host_server_fin(void)
   {int rv;

    rv = (_SC.hsst != NULL);
    if (rv == TRUE)
       SC_hasharr_clear(_SC.hsst, _SC_host_server_rel, NULL);

    _SC.hsst = NULL;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_READ_HOST_SERVER_DB - parse the host server database */

static void _SC_read_host_server_db(char *file)
   {int mn, mx, nb;
    char *s, *u, *type, *spec, *base;
    char *pnglst, *net;
    FILE *fp;
    hrng *rng;

    pnglst = NULL;

/* environment variable to override a .net specification in the file */
    net = getenv("HOST_SERVER_NET");

    fp = io_open(file, "r");
    if (fp != NULL)
       {_SC.hsst = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_NAME_KEY, 0);
         
        if (net != NULL)
	   {net = CSTRSAVE(net);
	    SC_hasharr_install(_SC.hsst, ".net", net, SC_STRING_S, 3, -1);};

	s  = NULL;
	nb = 0;

	while (TRUE)
	   {s = SC_dgets(s, &nb, fp);
	    if (s == NULL)
	       break;

	    if ((*s == '#') || SC_blankp(s, "#"))
	       continue;

	    else if ((strncmp(s, ".net ", 5) == 0) && (net == NULL))
	       {net = SC_strtok(s+5, " \t\n", u);
	        net = CSTRSAVE(net);
		SC_hasharr_install(_SC.hsst, ".net", net, SC_STRING_S, 3, -1);}

	    else if (strncmp(s, ".noping ", 8) == 0)
	       {pnglst = SC_strtok(s+8, " \t\n", u);
	        pnglst = CSTRSAVE(pnglst);}

	    else
	       {type = SC_strtok(s, " \t", u);
	        spec = SC_strtok(NULL, "\n\r\f", u);

		spec = SC_trim_left(spec, " \t");
		if (spec != NULL)
		   {if (*spec == '(')
		       rng = _SC_list(spec);

		    else
		       {base = SC_strtok(spec, " \t\n", u);
		        mn   = SC_stoi(SC_strtok(NULL, " \t\n", u));
			mx   = SC_stoi(SC_strtok(NULL, " \t\n", u));

			rng = _SC_range(base, mn, mx);};

		   rng->noping = _SC_unpingable(type, pnglst);

		   SC_hasharr_install(_SC.hsst, type, rng, "hrng", 3, -1);};};};

	io_close(fp);};

    CFREE(pnglst);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HOST_SERVER_INIT - parse the host-server database FILE 
 *                     - and build a hash table containing
 *                     - host names associated with system types
 *                     - return a TRUE iff successful
 */

int SC_host_server_init(char *file, int reset, int vrb)
   {int rv;
    char t[MAXLINE];
    char *r;
    hrng *rng;

    rv = FALSE;

    if ((reset == TRUE) && (_SC.hsst != NULL))
       SC_host_server_fin();

    if (file == NULL)
       file = getenv("HOST_SERVER_DB");

    if (_SC.hsst == NULL)
       {if ((file != NULL) && (SC_isfile(file)))
           _SC_read_host_server_db(file);

/* if there is no hash table yet you get a host server database with the current
 * host and system type only
 */
	if ((_SC.hsst == NULL) && (vrb == FALSE))
	   {_SC.hsst = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_NAME_KEY, 0);
	    SC_hostname(t, MAXLINE);
	    r   = SC_dsnprintf(FALSE, "(%s)", t);
	    rng = _SC_list(r);
	    SC_hasharr_install(_SC.hsst, SYS_Arch, rng, "hrng", 3, -1);};};

    rv = (_SC.hsst != NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HOST_SERVER_QUERY - given a request
 *                      - return the host info in OUT
 *                      - which is NC characters long
 *                      - returns TRUE iff successful
 */

int SC_host_server_query(char *out, int nc, char *fmt, ...)
   {int i, id, na, ns, rv;
    char s[MAXLINE], *req;
    char **strs, *ptrn, *net, *sys, *tok, *t, *p;

    if (_SC.hsst == NULL)
       rv = FALSE;

    else
       {rv = TRUE;

        SC_VDSNPRINTF(FALSE, req, fmt);

	strcpy(s, req);
	t = SC_strtok(s, "\n", p);
	if (t == NULL)
	   rv = FALSE;

/* list all types unconditionally */
	else if (strcmp(s, "-types-") == 0)
	   {strs = SC_hasharr_dump(_SC.hsst, NULL, NULL, TRUE);

	    out[0] = '\0';
	    if (strs != NULL)
	       {for (i = 0; TRUE; i++)
		    {t = strs[i];
		     if (t == NULL)
		        break;

		     else if ((*t != '#') && (strcmp(t, ".net") != 0))
		        {na = strlen(out);
			 ns = strlen(t);
			 if (na + ns + 1 < nc)
			    SC_vstrcat(out, nc, "%s ", t);
			 else
			    break;};};

		SC_LAST_CHAR(out) = '\0';
		CFREE(strs);};}

/* list all types with suffix specified in .net */
	else if (strcmp(s, "-types.net-") == 0)
	   {net  = (char *) SC_hasharr_def_lookup(_SC.hsst, ".net");
            ptrn = SC_dsnprintf(FALSE, "*@%s", net);
	    strs = SC_hasharr_dump(_SC.hsst, ptrn, NULL, TRUE);

	    out[0] = '\0';
	    if (strs != NULL)
	       {for (i = 0; TRUE; i++)
		    {t = strs[i];
		     if (t == NULL)
		        break;

		     else
		        {na = strlen(out);
			 ns = strlen(t);
			 if (na + ns + 1 < nc)
			    SC_vstrcat(out, nc, "%s ", t);
			 else
			    break;};};

		CFREE(strs);};

	    SC_LAST_CHAR(out) = '\0';}

/* list all types with specified suffix */
	else if (strncmp(s, "-types.at-", 10) == 0)
	   {SC_strtok(s, " \n", p);
	    t = SC_strtok(NULL, " \n", p);
            ptrn = SC_dsnprintf(FALSE, "*%s", t);
	    strs = SC_hasharr_dump(_SC.hsst, ptrn, NULL, TRUE);

	    if (strs != NULL)
	       {out[0] = '\0';
		for (i = 0; TRUE; i++)
		    {t = strs[i];
		     if (t == NULL)
		        break;

		     else
		        {na = strlen(out);
			 ns = strlen(t);
			 if (na + ns + 1 < nc)
			    SC_vstrcat(out, nc, "%s ", t);
			 else
			    break;};};

		SC_LAST_CHAR(out) = '\0';
		CFREE(strs);};}

	else if (strncmp(s, "-nhosts-", 8) == 0)
	   {SC_strtok(s, " \n", p);
	    t  = SC_strtok(NULL, " \n", p);
	    na = _SC_get_nhosts(t, _SC.hsst);

	    snprintf(out, nc, "%d", na);}

/* handle host queries */
	else
	   {sys = SC_strtok(s, " \t", p);
	    tok = SC_strtok(NULL, " \t", p);
	    if (tok == NULL)
	       id = -1;
	    else
	       id = SC_stoi(tok);

	    rv = _SC_get_next_host(out, nc, sys, _SC.hsst, id);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_VERIFY_HOST - check whether the current user can log into host HST
 *                - wait at most TO milliseconds
 *                - special values of TO are:
 *                -    -1 wait forever (actually INT_MAX)
 *                -    -2 use the value of the environment variable
 *                -       SC_EXEC_TIME_VERIFY if it exists or 10 seconds
 *                -
 *                - return -1 if host does not respond in time
 *                -           (presumed down)
 *                - return  0 if not at all  (SC_gs.errn set)
 *                - return  1 if so and no prompt
 *                - return  2 if so with a prompt
 */

int SC_verify_host(char *hst, int to)
   {int i, is, it, nc, np, ok, pr;
    int dt, ta, pt, done, st;
    char s[MAXLINE];
    char *ca[20], *tc, *tk, *prompt, *swd, *p;
    PROCESS *pp;
    SC_sigstate *ss;

    if (to < 0)
       {switch (to)
 	   {default :
	    case -1 :
	         to = INT_MAX;
	         break;
	    case -2 :
	         p = getenv("SC_EXEC_TIME_VERIFY");
	         if (p != NULL)
		    to = SC_stoi(p);
		 else
		    {p = SC_dsnprintf(FALSE, "SC_EXEC_TIME_VERIFY=10000");
		     SC_putenv(p);
		     to = 10000;};
	         break;};};

    i = 0;
    ca[i++] = "ssh";
    ca[i++] = "-q";
    ca[i++] = "-x";
    ca[i++] = "-o";
    ca[i++] = "BatchMode=yes";
    ca[i++] = "-o";
    ca[i++] = "StrictHostKeyChecking=no";
    ca[i++] = hst;
    ca[i++] = "echo";
    ca[i++] = "verify-host-ok";
    ca[i++] = NULL;

    prompt = "(yes/no)?";
    np     = strlen(prompt);
    swd    = "sword:";

    ok = 0;
    pr = FALSE;

    pp = SC_open(ca, NULL, "aTo", NULL);

/* if the open failed try it again using pipes instead of PTYs
 * sometimes you just cannot get a PTY
 */
    if (pp == NULL)
       pp = SC_open(ca, NULL, "apo", NULL);

    ss = SC_save_signal_handlers(1, SC_NSIG);
    if (SC_process_alive(pp))

/* choose a good sampling interval */
       {if (to < 100)
	   dt = to / 16;
	else if (to < 20000)
	   dt = 100;
	else
	   dt = 1000;

	dt = max(dt, 1);

/* allow TO seconds to receive all relevant output */
	done = FALSE;
	is   = 0;

/* NOTE: consider using SC_exec_job here because this loop
 * does not handle the case where the child finishes before
 * message are delivered for SC_gets
 * SC_exec_job handles that more correctly because of the polling
 */
	for (it = 0, ta = 0; ta < to; ta += dt, it++)
	    {if (SC_gets(s, MAXLINE, pp) != NULL)
	        {is++;

		 nc = strlen(s);
		 tc = s + 45;
		 tk = s + 43;

/* check for any kind of prompt */
		 if (((nc >= 45 + np) && (strncmp(tc, prompt, np) == 0)) ||
		     ((nc >= 43 + np) && (strncmp(tk, prompt, np) == 0)))
		    {SC_printf(pp, "yes\n");
		     pr = TRUE;}
		 else if (strstr(s, swd) != NULL)
		    {ok = -1;
		     break;}

/* check for expected message */
		 else if (strstr(s, ca[5]) != NULL)
		    {ok = 1;
		     break;};}

	     else if (done == TRUE)
	        break;

	     else
	        {st = SC_process_status(pp);
		 if (st != SC_RUNNING)
		    done = TRUE;};

	     memset(s, 0, MAXLINE);
	     SC_sleep(dt);};

/* fine tune shades of success */
	if (ok == 1)
	   ok += pr;

/* fine tune shades of failure for diagnostic purposes
 *    (reason)*10^6 + (status)*10^4 + (%ta)*10^2 + (# msg rec)
 */
	else if (ok == 0)
	   {pt = 100.0*dt/ta;
	    ok = -(100*(100*(100*pp->reason + pp->status) + pt) + is);};

	SC_close(pp);};

    SC_restore_signal_handlers(ss, TRUE);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_SYS_LENGTH_MAX - silly function
 *                       - useful in connection with formatting output
 *                       - return the character length of the
 *                       - longest string in the database
 *                       - if LOCAL is TRUE only consider hosts
 *                       - from .net
 *                       - if FULL is TRUE use fully qualified name
 *                       - otherwise without the @ qualification
 */

int SC_get_sys_length_max(int local, int full)
   {int i, ns, nc;
    char t[MAXLINE];
    char **strs, *net, *p, *ptrn;

    nc = 0;
    if (_SC.hsst == NULL)
       SC_host_server_init(_SC.hsdb, FALSE, FALSE);

    if (_SC.hsst != NULL)
       {if (local == TRUE)
	   {net  = (char *) SC_hasharr_def_lookup(_SC.hsst, ".net");
	    if (net == NULL)
	       strs = SC_hasharr_dump(_SC.hsst, NULL, NULL, TRUE);
	    else
	       {ptrn = SC_dsnprintf(FALSE, "*@%s", net);
		strs = SC_hasharr_dump(_SC.hsst, ptrn, NULL, TRUE);};}
	else
	   strs = SC_hasharr_dump(_SC.hsst, NULL, NULL, TRUE);

	if (strs != NULL)
	   {for (i = 0; TRUE; i++)
	        {if (strs[i] == NULL)
		    break;

		 strcpy(t, strs[i]);
		 if (full == FALSE)
		    {p = strchr(t, '@');
		     if (p != NULL)
		        *p = '\0';};

		 if (strcmp(t, ".net") != 0)
		    {ns = strlen(t);
		     nc = max(nc, ns);};};};};

    return(nc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_HOST_LENGTH_MAX - silly function
 *                        - useful in connection with formatting output
 *                        - return the character length of the
 *                        - longest hostname string for the current type
 *                        - if LOCAL is TRUE only consider hosts
 *                        - from .net
 *                        - if FULL is TRUE use fully qualified name
 *                        - otherwise without the @ qualification
 */

int SC_get_host_length_max(char *sys, int local, int full)
   {int ih, nh, ns, nc;
    char *host, **hl;
    hrng *h;

    nc = 0;
    if (_SC.hsst == NULL)
       SC_host_server_init(_SC.hsdb, FALSE, FALSE);

    if (_SC.hsst != NULL)
       {h = _SC_host_server_lookup(sys, _SC.hsst);
        if (h != NULL)
	   {hl = h->hosts;
	    nh = h->nhosts;
	    for (ih = 0; ih < nh; ih++)
	        {host = hl[ih];
		 ns   = strlen(host);
		 nc   = max(nc, ns);};};};

    return(nc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_HOST_NAME - given a system type SYS return an available host in HST
 *                  - return TRUE iff successful
 */

int SC_get_host_name(char *hst, int nc, char *sys)
   {int rv;

    rv = SC_host_server_init(_SC.hsdb, FALSE, FALSE);
    rv = SC_host_server_query(hst, nc, sys);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_NHOSTS - given a system type SYS return the number of
 *               - potentially available hosts
 */

int SC_get_nhosts(char *sys)
   {int rv;
    char t[MAXLINE];

    rv = SC_host_server_init(_SC.hsdb, FALSE, FALSE);
    rv = SC_host_server_query(t, MAXLINE, "-nhosts- %s", sys);
    rv = SC_stoi(t);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_HOST_TYPES - get the list of all available host types
 *                   - it is the clients responsibility to free
 *                   - array and the strings with SC_free_strings
 */

char **SC_get_host_types(int whch, char *net)
   {int rv;
    char bf[MAXLINE];
    char *spec, **strs;

    bf[0] = '\0';

    rv = SC_host_server_init(_SC.hsdb, FALSE, FALSE);
    SC_ASSERT(rv == TRUE);

    switch (whch)
       {case 1 :
	     spec = SC_dsnprintf(TRUE, "-types-");
	     break;

	case 2 :
	     spec = SC_dsnprintf(TRUE, "-types.at- %s", net);
	     break;

        case 0 :
	default :
	     spec = SC_dsnprintf(TRUE, "-types.net-");
	     break;}
	
    rv = SC_host_server_query(bf, MAXLINE, spec);
    SC_ASSERT(rv == TRUE);

    strs = SC_tokenize(bf, " \n");

    CFREE(spec);

    return(strs);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_HOST_FILE - get the list of all available host types
 *                  - from host entries in the host file F
 */

char **SC_get_host_file(char *f)
   {char s[MAXLINE];
    char *p, *u, *tok, **strs;
    SC_array *arr;
    FILE *fp;

    arr = SC_STRING_ARRAY();

    fp = io_open(f, "r");
    while (TRUE)
       {p = io_gets(s, MAXLINE, fp);
	if (p == NULL)
	   break;
	else
	   {tok = SC_strtok(s, " ", u);
	    if ((tok != NULL) && (strcmp(tok, "host") == 0))
	       {tok = SC_strtok(NULL, " ", u);
		if (tok != NULL)
		   SC_array_string_add_copy(arr, tok);};};};

    io_close(fp);

    SC_array_string_add(arr, NULL);

    strs = SC_array_done(arr);

    return(strs);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_SYSTEM_LIST - return an array of system types derived from SYS
 *                    - SYS may be one of:
 *                    -   "all"  all entries in the database
 *                    -   @<net> entries with the matching suffix
 *                    -   +<hostfile> host entries in host file
 *                    -   <type> the matching entry
 */

char **SC_get_system_list(char *sys)
   {char **tp;

    if (sys == NULL)
       sys = SYS_Arch;

    if (sys[0] == '@')
       tp = SC_get_host_types(2, sys);

    else if (sys[0] == '+')
       tp = SC_get_host_file(sys+1);

    else if (strcmp(sys, "all") == 0)
       tp = SC_get_host_types(1, NULL);

    else
       {tp = CMAKE_N(char *, 2);
	tp[0] = sys;
	tp[1] = NULL;};

    return(tp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_HOST_LIST - return an array of hosts derived from SYS
 *                  - SYS may be one of:
 *                  -   "all"  all entries in the database
 *                  -   @<net> entries with the matching suffix
 *                  -   <type> the matching entry
 *                  - if SINGLE is TRUE select only one host per type
 */

char **SC_get_host_list(char *sys, int single)
   {int i, is, n, ns, nc, st;
    char out[MAXLINE];
    char **ss, **sa;
    SC_array *arr;

    ss = SC_get_system_list(sys);
    SC_ptr_arr_len(ns, ss);

    arr = SC_STRING_ARRAY();

    for (is = 0; is < ns; is++)

/* get the number of hosts for this system type */
        {n  = SC_get_nhosts(ss[is]);
	 nc = SC_get_host_length_max(ss[is], TRUE, FALSE);
	 SC_ASSERT(nc >= 0);

	 if (single == TRUE)
	    {st = SC_host_server_query(out, MAXLINE, "%s", ss[is]);
	     SC_ASSERT(st == TRUE);
	     SC_array_string_add_copy(arr, out);}

/* check each host of this type */
	 else
	    {for (i = 0; i < n; i++)
	         {st = SC_host_server_query(out, MAXLINE, "%s %d", ss[is], i);
		  SC_ASSERT(st == TRUE);
		  SC_array_string_add_copy(arr, out);};};};

    SC_free_strings(ss);
    sa = SC_array_done(arr);

    return(sa);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HOSTNAME - return the name of the current node in S
 *             - return TRUE iff successful
 */

int SC_hostname(char *s, int nc)
   {int rv, st;
    char chst[MAXLINE];
    char *p;

    st = gethostname(chst, MAXLINE);
    if (st == 0)
       {strcpy(s, chst);
	p = strchr(s, '.');
	if (p != NULL)
	   *p = '\0';}

#if defined(HAVE_POSIX_SYS)
    else
       {struct utsname uts;

        uname(&uts);
	strncpy(s, uts.nodename, nc);
        st = 0;};
#endif

    rv = (st == 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HOSTTYPE - return the system type of the current node in S
 *             - S is NC long
 *             - with X NULL:
 *             -   return "<OS name> : <OS version>" as reported by uname
 *             - otherwise
 *             -   result of issuing as shell command X
 */

int SC_hosttype(char *s, int nc, char *x)
   {int st, rv;

    if (x != NULL)
       st = SC_execs(s, nc, NULL, -1, x);

#if defined(HAVE_POSIX_SYS)
    else
       {struct utsname uts;

	uname(&uts);
	snprintf(s, nc, "%s : %s", uts.nodename, uts.release);
        st = 0;};
#endif

    rv = (st == 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HOSTSYSTEM - return the PACT system type of the specified NODE
 *               - in the string TYPE of length NC
 *               - return TRUE iff successful
 */

int SC_hostsystem(char *type, int nc, char *node)
   {int st, rv;

    st = 1;
    if (node != NULL)
       st = SC_execs(type, nc, NULL, -1,
                     "ssh -x -q %s pact -info SYS_Arch", node);

    if (st != 0)
       {st = 0;
	SC_strncpy(type, nc, SYS_Arch, nc);};
        
    rv = (st == 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
