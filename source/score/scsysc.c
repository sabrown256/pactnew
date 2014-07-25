/*
 * SCSYSC.C - SCORE system comand equivalent
 *          - and support routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"
#include "scope_proc.h"

/* generic state activities */

#define STATE_FREE_TASKS      300
#define STATE_FREE_LOG        301
#define STATE_PRINTF          302
#define HANDLE_ECHO           303
#define HANDLE_SLEEP          304
#define HANDLE_CD             305
#define MANAGE_LAUNCHED_JOB   306
#define COMMAND_EXEC          307
#define RUN_NEXT_TASK         308
#define TASK_DONE             309
#define START_JOB             310
#define FIN_JOB               311
#define CLOSE_JOB             312
#define REPORT_EXIT           313
#define SERVER_PRINTF         314

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPRSUBTASK - print the subtask T */

void dprsubtask(subtask *t)
   {int i;

    if (t != NULL)
       {printf("Task: %s\n", t->command);
        printf("   Tokens: %d\n", t->nt);
	printf("   Argv:\n");
	for (i = 0; t->argf[i] != NULL; i++)
	    printf("      (%d) %s\n", i, t->argf[i]);

	if (t->shell != NULL)
	   printf("   Shell: %s\n", t->shell);
	if (t->ios != NULL)
	   printf("   Redirections: %s\n", t->ios);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SET_RUN_TASK_STATE - set the thread parstate to STATE */

void _SC_set_run_task_state(parstate *state)
   {SC_thread_proc *ps;

    ps = _SC_get_thr_processes(-1);
    ps->task = state;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_CHG_DIR - change to directory DIR
 *             - then copy the current directory name into JOB
 *             - if DIR is NULL and the JOB has a directory use it
 *             - return 0 iff successful
 */

int _SC_chg_dir(char *dir, char **pndir)
   {int st, dot;
    char *ndr, *cwd;

    ndr = (pndir == NULL) ? NULL : *pndir;
    st  = 0;

/* determine whether dir state or implies staying in the current directory */
    dot = ((dir == NULL) || (strcmp(dir, ".") == 0));
    if ((dot == TRUE) && (ndr != NULL))
       {dir = ndr;
	dot = ((dir == NULL) || (strcmp(dir, ".") == 0));};

    if (dot == FALSE)
       st = chdir(dir);

    if ((st == 0) && (pndir != NULL))
       {cwd = SC_getcwd();
	if (cwd != NULL)
	   {if ((ndr == NULL) || (strcmp(cwd, ndr) != 0))
	       {CFREE(ndr);
		*pndir = CSTRSAVE(cwd);};
	    CFREE(cwd);};};

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_SHELL - given a string SHELL return an explicit shell */

char *SC_get_shell(char *shell)
   {

    if (shell == NULL)
       shell = getenv("SHELL");

    if (shell == NULL)
       shell = SHELL_Default;

    return(shell);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_STATE_LOG - log messages for STATE
 *               - this can be used on a client or server
 */

static int _SC_state_log(parstate *state, char *fmt, ...)
   {int rv, ifl;
    char chst[MAXLINE];
    char *msg;

    rv = FALSE;

    ifl = (strcmp(fmt, "server> init(%d)") == 0);
    if (ifl == TRUE)
       {gethostname(chst, MAXLINE);

	msg = SC_dsnprintf(TRUE, "Host: %s\n", chst);
	SC_array_string_add(state->log, msg);

	msg = SC_dsnprintf(TRUE, "Date: %s\n", SC_datef());
	SC_array_string_add(state->log, msg);

	msg = SC_dsnprintf(TRUE,
			   "---------------------------------------------------\n");
	SC_array_string_add(state->log, msg);};

    SC_VDSNPRINTF(TRUE, msg, fmt);

    SC_array_string_add(state->log, msg);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_READ_FILTER - parse the filter file FNAME */

fspec *_SC_read_filter(char *fname)
   {int n;
    char s[MAXLINE];
    char *tok, *txt, *p;
    FILE *fp;
    fspec *filter;

    if (fname == NULL)
       fp = NULL;
    else
       fp = SC_fopen_safe(fname, "r");

    if (fp != NULL)
       {filter = CMAKE_N(fspec, 1000);

	n  = 0;
	while (SC_fgets(s, MAXLINE, fp) != NULL)
	   {tok = SC_strtok(s, " \t\n\r", p);
	    txt = SC_strtok(NULL, "\n\r", p);

	    if ((tok != NULL) && (txt != NULL) && (*tok != '#'))
	       {filter[n].itok = SC_stoi(tok);

/* whack leading white space off of txt */
		for (; strchr(" \t", *txt) != NULL; txt++);

		SC_strncpy(filter[n].text, MAXLINE, txt, -1);
		n++;};};

	filter[n].text[0] = '\0';
	filter[n].itok    = -1;
	n++;

	SC_fclose_safe(fp);}

    else
       filter = NULL;

    return(filter);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FREE_FILTER - free the space FILTER */

void _SC_free_filter(fspec *filter)
   {

    if (filter != NULL)
       {CFREE(filter);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MATCH - return TRUE iff T matches a specification in FILTER */

static int _SC_match(char *t, fspec *filter)
   {int i, j, id, ok;
    char *f, *ps, *tok, *s, *p;

    ok = FALSE;
    if (filter != NULL)
       {for (i = 0; !ok; i++)
	    {id = filter[i].itok;
	     f  = filter[i].text;
	     if (f[0] == '\0')
	        break;

	     else if (id == 0)
	        ok = (strstr(t, f) != NULL);

	     else
	        {s  = CSTRSAVE(t);
		 ps = s;

		 tok = NULL;
		 for (j = 1; j <= id; j++)
		     {tok = SC_strtok(ps, " \t", p);
		      ps  = NULL;};

		 if (tok != NULL)
		    ok = (strstr(tok, f) != NULL);

		 CFREE(s);};};};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FREE_TASKLST - free up the space for a task list */

static void _SC_free_tasklst(tasklst *tl)
   {int it, nt;
    subtask *sub;

    if (tl->tasks != NULL)
       {nt = tl->nt;
	for (it = 0; it < nt; it++)
	    {sub = tl->tasks + it;

	     CFREE(sub->shell);
	     CFREE(sub->command);
	     CFREE(sub->ios);

	     SC_free_strings(sub->argf);};

	CFREE(tl->tasks);};

    CFREE(tl);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FREE_TASKDESC - free a taskdesc instance JOB */

static int _SC_free_taskdesc(void *a)
   {jobinfo *inf;
    taskdesc *job;

    if (a != NULL)
       {job = *(taskdesc **) a;
	if (job != NULL)
	   {inf = &job->inf;

	    SC_free_array(inf->out, SC_array_free_n);

	    _SC_free_tasklst(job->command);

	    CFREE(inf->full);
	    CFREE(inf->directory);
	    CFREE(inf->shell);
	    CFREE(job->host);
	    CFREE(job);

	    *(taskdesc **) a = NULL;};};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EXEC_FREE_TASKS - free the task of the given STATE */

static void _SC_exec_free_tasks(parstate *state)
   {

    SC_START_ACTIVITY(state, STATE_FREE_TASKS);

    SC_free_array(state->tasks, _SC_free_taskdesc);
    state->tasks = NULL;

    SC_END_ACTIVITY(state);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EXEC_FREE_LOGS - free the logs of the given STATE */

static void _SC_exec_free_logs(parstate *state)
   {

    SC_START_ACTIVITY(state, STATE_FREE_LOG);

    SC_free_array(state->log, SC_array_free_n);
    state->log = NULL;

    SC_END_ACTIVITY(state);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EXEC_SETUP_STATE - setup a parstate instance for a member of
 *                      - the SC_exec family of functions 
 */

void _SC_exec_setup_state(parstate *state, char *shell, char **env,
			  int na, int show, int tag, int srv,
			  int ign, fspec *flt, SC_evlpdes *pe,
			  PFFileCallback acc, PFFileCallback rej,
			  int (*finish)(taskdesc *job, char *msg))
   {int dtw, to;
    char *s;

    s = getenv("SC_EXEC_SERVER_WAIT");
    if (s == NULL)
       dtw = 300;
    else
       dtw = SC_stoi(s);

    s = getenv("SC_EXEC_OPEN_RETRY");
    if (s == NULL)
       to = 100;
    else
       to = SC_stoi(s);

    state->shell        = shell;
    state->env          = env;
    state->na           = na;
    state->heartbeat_dt = DEFAULT_HEARTBEAT;
    state->wait_ref     = 0;
    state->n_running    = 0;
    state->n_complete   = 0;
    state->doing        = 0;
    state->show         = show;
    state->tagio        = tag;
    state->server       = srv;
    state->ignore       = ign;
    state->done         = FALSE;
    state->filter       = flt;
    state->loop         = pe;

    state->wait_dt      = dtw;
    state->open_retry   = to;

    state->directory    = NULL;
    _SC_chg_dir(".", &state->directory);

    state->tasks = CMAKE_ARRAY(taskdesc *, NULL, 0);
    state->log   = SC_STRING_ARRAY();

    state->acc        = acc;
    state->rej        = rej;
    state->finish     = finish;
    state->print      = _SC_state_log;
    state->free_logs  = _SC_exec_free_logs;
    state->free_tasks = _SC_exec_free_tasks;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EXEC_FREE_STATE - cleanup a parstate */

void _SC_exec_free_state(parstate *state, int flt)
   {

    _SC_chg_dir(state->directory, NULL);
    CFREE(state->directory);

    if (flt == TRUE)
       _SC_free_filter(state->filter);

    SC_free_event_loop(state->loop);

    state->free_logs(state);
    state->free_tasks(state);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SERVER_PRINTF - print to both the STATE log
 *                   - and if STATE is a server state to the parent
 *                   - and AS log if it exists
 *                   - this version is for situations when there is no JOB
 */

int _SC_server_printf(asyncstate *as, parstate *state, char *tag,
		      char *fmt, ...)
   {int rv;
    char *msg;

    SC_START_ACTIVITY(state, SERVER_PRINTF);

    rv = TRUE;

    SC_VDSNPRINTF(TRUE, msg, fmt);

    state->print(state, "%s %s", tag, msg);

    _SC_exec_printf(as, "%s %s %s", _SC_EXEC_SRV_ID, tag, msg);

    CFREE(msg);

    SC_END_ACTIVITY(state);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DSTATELOG - help with generic debug logging */

void dstatelog(char *fmt, ...)
   {char tag[MAXLINE];
    char *tm, *tms, *msg;
    parstate *state;

    SC_VDSNPRINTF(TRUE, msg, fmt);

    GET_TASK_STATE(state);

    if (state != NULL)
       {tm  = SC_datef();
	tms = tm + 11;

	snprintf(tag, MAXLINE, "[Job  -/- %s]", tms);

	CFREE(tm);

	state->print(state, "%s (%d) %s", tag, state->doing, msg);};

    CFREE(msg);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PRINT_FILTERED - print each entry in MSG that is not rejects by
 *                    - a match with a specification in FILTER
 *                    - label each line with JID iff not equal -1
 */

void _SC_print_filtered(asyncstate *as, char **msg,
			fspec *filter, int jid, char *host)
   {int i, nm;
    char fmt[MAXLINE];
    char *t;

    SC_ptr_arr_len(nm, msg);

    for (i = 0; i < nm; i++)
        {t = msg[i];
	 if (!_SC_match(t, filter))
	    {if (host != NULL)
	        {if (as->nchar_max > 0)
		    snprintf(fmt, MAXLINE, "%%-%ds| %%s", as->nchar_max);
		 else
		    snprintf(fmt, MAXLINE, "%%s| %%s");
		 _SC_exec_printf(as, fmt, host, t);}

	     else if (jid == -1)
	        _SC_exec_printf(as, "%s", t);

	     else
	        _SC_exec_printf(as, "[Job %2d out]: %s", jid, t);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SHOW_COMMAND - decide whether to show commands
 *                  - if so print the relevant part of S using AS
 */

char *_SC_show_command(asyncstate *as, char *s, int show)
   {int c;

    c = s[0];
    if ((show == 2) || ((show == 1) && (c != '@')))
       _SC_exec_printf(as, "%s\n", s);

    if (c == '@')
       s++;

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PUT_COMMAND - decide whether to show commands
 *                 - if so put the relevant part of S into OUT
 */

char *_SC_put_command(SC_array *out, char *s, int show)
   {int c;
    char *bf;

    c = s[0];
    if ((show == 2) || ((show == 1) && (c != '@')))
       {bf = SC_dsnprintf(TRUE, "%s\n", s);
	SC_array_string_add(out, bf);};

    if (c == '@')
       s++;

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_DECIDE_RETRY - decide whether or not a retry is neccessary
 *                  - if a retry will be done this routine sleeps
 *                  - until it is time to retry
 *                  - return
 *                  -    0  if successful
 *                  -    1  if failed and retry
 *                  -   -1  if failed finally
 */

int _SC_decide_retry(asyncstate *as, jobinfo *inf, tasklst *tl, int st)
   {int rv, ts, warn;
    int ia, na, it;
    char *cmd, *ltm, *stmsg, *cmmsg;
    subtask *sub;

    ia = inf->ia;
    na = inf->na;

    warn = FALSE;

    if (st != 0)
       {ltm = SC_datef();

	rv = -1;
	if (ia < na)
	   {warn = TRUE;
	    rv   = 1;
	    if (ia < 2)
	       ts = SC_random_int(100, 1000);
	    else
	       ts = SC_random_int(30000, 40000);

	    stmsg = SC_dsnprintf(TRUE,
				 "***> failed (%d) [%s] - attempt %d in %.2f seconds\n",
				 st, ltm, ia + 1, ts/1000.0);

	    SC_sleep(ts);}

        else if (na > 1)
	   {warn  = TRUE;
	    stmsg = SC_dsnprintf(TRUE,
				 "***> failed (%d) [%s] - quitting after %d attempts\n",
				 st, ltm, na);};

	CFREE(ltm);}

    else
       {if ((ia > 1) && (na > 1))
	   {warn  = TRUE;
	    ltm   = SC_datef();
	    stmsg = SC_dsnprintf(TRUE,
				 "***> succeeded [%s] - on attempt %d\n",
				 ltm, ia);

	    CFREE(ltm);};

	rv = 0;};

    if (warn == TRUE)
       {SC_array_string_add(inf->out, stmsg);

	cmd   = inf->full;
	cmmsg = SC_dsnprintf(TRUE, "***>       %s\n", cmd);
	SC_array_string_add(inf->out, cmmsg);

	if ((st != 0) && (tl != NULL))
	   {it  = _SC_get_command(tl, 1);
	    sub = tl->tasks + it;
	    cmd = sub->command;

	    if ((ia < na) && (na > 1))
	       {cmmsg = SC_dsnprintf(TRUE,
				     "***> retry '%s' - task %d\n", cmd, it+1);
		SC_array_string_add(inf->out, cmmsg);};};};

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                           TASK PARSING                                   */

/*--------------------------------------------------------------------------*/

/* _SC_PREP_COMMAND - prepare the command IN
 *                  - transform ( ... ) to ' ... ' and place it in OUT
 */

static char *_SC_prep_command(char *in, int check)
   {int nc;
    char *out;

    out = NULL;
    if (in != NULL)
       {SC_trim_right(in, " ");

/* transform a command of the form ( ... ) to ' ... '
 * this will save a process being created by the shell SH
 */
	nc = strlen(in) - 1;
	nc = max(nc, 0);
	if ((in[0] == '(') && (in[nc] == ')'))
	   {out = CSTRSAVE(in+1);
	    SC_LAST_CHAR(out) = '\0';}

/* copy in the command if not of the form ( ... ) */
	else
	   out = CSTRSAVE(in);};

    return(out);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SUBST_TASK_ENV - substitute recognized environment variables values */

static void _SC_subst_task_env(int na, char **ta)
   {int i, c, nb, nc;
    char *r, *t, *ps, *pe, *val;
    char var[MAXLINE];

    for (i = 0; i < na; i++)
        {t = ta[i];
	 c = *t;
	 if (strchr("\'({[", c) != NULL)
	    continue;

	 r = NULL;

	 while (TRUE)
	    {ps = strchr(t, '$');
	     if (ps == NULL)
	        break;
	     else
	        {for (pe = ps+1; TRUE; pe++)
		     {c = *pe;
		      if (c == '{')
                         continue;
		      else if (!SC_isalnum(c) && (c != '_') && (c != '}'))
			 break;};
		 nb = ps - t;
		 ps++;
		 nc = pe - ps;

/* handle ${VAR} forms */
		 if (*ps == '{')
		    SC_strncpy(var, MAXLINE, ps+1, nc-2);

/* handle $VAR forms */
		 else
		    SC_strncpy(var, MAXLINE, ps, nc);

		 val = getenv(var);
		 if (val == NULL)
		    break;
		 else
		    {r = SC_dstrcat(r, t);
		     r[nb] = '\0';
		     r = SC_dstrcat(r, val);
		     r = SC_dstrcat(r, pe);

		     CFREE(t);
		     t = r;};};};

	 ta[i] = t;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_IO_SPLICE_OUT - splice out the Ith thru Jth tokens out
 *                   - of PS->ARGF
 */

static int _SC_io_splice_out(subtask *ps, int i, int j)
   {int n, di;
    char **ta;

    n  = ps->nt;
    ta = ps->argf;

    di = j - i + 1;
    n -= di;
    for ( ; i <= j; j--)
        {CFREE(ta[j]);};

    for (j = i; j < n; j++)
        ta[j] = ta[j+di];

    ps->nt = n;

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PARSE_REDIRECT - parse tokens until
 *                    - a redirection of the form <src><oper><dst>
 *                    - is obtained
 *                    -
 *                    - output redirections (covers sh and csh families)
 *                    -   [src]>[&] dst     src to dst, create name, error if it exists
 *                    -   [src]>![&] dst    src to dst, create name, truncate if it exists
 *                    -   [src]>>[&] dst    src to dst, append
 *                    -
 *                    -   dst := <filename> | <fd>
 *                    -   src := <fd> | &
 *                    -   fd  := [digits]+  (defaults to 1, stdout, if absent)
 */

static int _SC_parse_redirect(SC_iodes *pio, subtask *ps, int i, char *p,
			      char *src)
   {int j, fid, gid, so;
    int c, nc;
    char file[MAXLINE];
    char *t, *fn, **ta;

    ta = ps->argf;
    j  = i;

    if (pio != NULL)
       {t = ta[j];
	
	gid = -1;
	fid = -1;
	fn  = NULL;
	so  = FALSE;

/* save the original specification */
	pio->raw = CSTRSAVE(p);

	nc = p - t;
	strncpy(src, t, nc);
	src[nc] = '\0';
	
/* msh syntax */
	if (t[0] == SC_PROCESS_DELIM)
	   {fn = t + 3;
	    SC_strncpy(file, MAXLINE, t+3, -1);}

/* conventional UNIX shell syntax */
	else
	   {for (nc = 0; TRUE; nc++)
	        {c = *p++;
		 if ((c == '\0') || (strchr("<>!&", c) == NULL))
		    {p--;
		     break;};};

	    file[0] = '\0';
	    nc      = strlen(p);
	    if (nc == 0)
	       {t = ta[++j];
		if (t != NULL)
		   {SC_strncpy(file, MAXLINE, t, -1);
		    fn = file;};
		so = TRUE;}
	    else
	       {SC_strncpy(file, MAXLINE, p, nc);
		file[nc] = '\0';
	        fn = file;
		so = TRUE;};};

	pio->gid = gid;
	pio->fid = fid;
	if (fn != NULL)
	   pio->file = CSTRSAVE(fn);

/* parse out the specification - results in PIO */
	_SC_io_kind(pio, NULL);

/* splice out the redirect related tokens array */
	if (so == TRUE)
	   {_SC_io_splice_out(ps, i, j);
	    j = i - 1;};};

    return(j);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_REDIRECT_FD - determine the I/O redirections in PS->ARGF
 *                 - and fill in PS->FD from them
 */

static int _SC_redirect_fd(subtask *ps, int i, char *p)
   {int rv;
    char src[MAXLINE];
    SC_iodes io;

    memset(&io, 0, sizeof(io));

/* parse out the redirect related specifications */
    rv = _SC_parse_redirect(&io, ps, i, p, src);

/* add the redirect specifications to the filedes */
    switch (io.knd)
       {case SC_IO_STD_IN :
	     _SC_redir_filedes(ps->fd, SC_N_IO_CH, 0, &io);
	     break;

        case SC_IO_STD_OUT :
        case SC_IO_STD_ERR :
        case SC_IO_STD_BOND :
	     {int fd;

	      switch (src[0])
	         {case '&' :
		       _SC_redir_filedes(ps->fd, SC_N_IO_CH, 2, &io);
		       fd = 1;
		       break;
		  case '\0' :
		       fd = 1;
		       break;
		  default :
		       fd = SC_stoi(src);
		       break;};
	      _SC_redir_filedes(ps->fd, SC_N_IO_CH, fd, &io);};
	     break;

	default :
	     break;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_REDIRECT_SUBTASK - look through the tokens of the subtask PS
 *                      - for I/O redirection specifications and
 *                      - move any such specifications to the
 *                      - subtasks file descriptors
 */

static void _SC_redirect_subtask(subtask *ps)
   {int i, n;
    char **ta, *t, *p;

    n  = ps->nt;
    ta = ps->argf;
    for (i = 0; i < n; i++)
        {t = ta[i];

/* do not look at a token that begins with any kind of quote
 * it is a string
 */
	 if (strchr("'\"", *t) == NULL)

/* look for tokens containing redirection specifications */
	    {p = strpbrk(t, "<>");
	     if (p != NULL)
	        _SC_redirect_fd(ps, i--, p);
/*	     else if ((t[0] == SC_PROCESS_DELIM) && (t[2] == 'f')) */
	     else if (t[0] == SC_PROCESS_DELIM)
	        _SC_redirect_fd(ps, i--, t);

	     n = ps->nt;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_UNQUOTE_SUBTASK - look through the tokens of the subtask PS
 *                     - and remove enclosing quote marks
 *                     - unless this an 'echo' command
 *                     - this mimics a shell having done its processing
 */

static void _SC_unquote_subtask(subtask *ps)
   {int i, n;
    char **ta, *t, *p;

    n  = ps->nt;
    ta = ps->argf;
    if ((ta != NULL) && (strcmp(ta[0], "echo") != 0))
       {for (i = 0; i < n; i++)
	    {t = ta[i];
	     if (strchr("'\"", *t) != NULL)
	        {p = CSTRSAVE(t+1);
		 SC_LAST_CHAR(p) = '\0';
		 CFREE(t);
		 ta[i] = p;};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PUSH_SUBTASK - fill the ITth task in SUB from the
 *                  - parts SF, TF and DOSH
 */

static void _SC_push_subtask(subtask *sub, int it, char *shell,
			     SC_array *tf, int dosh, char *ios)
   {int nt;
    char **sa, *sc;
    subtask *ps;

    ps = sub + it;

    nt = SC_array_get_n(tf);
    sa = SC_array_done(tf);

    sc = SC_dconcatenate(sa, 0, nt, " ");

    ps->kind    = TASK_COMPOUND;
    ps->need    = dosh;
    ps->pipe    = (ios != NULL);
    ps->shell   = CSTRSAVE(shell);
    ps->nt      = nt;
    ps->argf    = sa;
    ps->command = sc;
    ps->ios     = CSTRSAVE(ios);

    _SC_init_filedes(ps->fd);

/* process out I/O redirections if we don't need a shell to do the command */
    if (dosh == FALSE)
       _SC_redirect_subtask(ps);

/* remove a layer of quotes if we don't need a shell to do the command */
    if (dosh == FALSE)
       _SC_unquote_subtask(ps);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PUSH_TOKEN - push token T onto array TF */

static void _SC_push_token(SC_array *tf, char *t)
   {char *s;

    s = CSTRSAVE(t);
    SC_LAST_CHAR(s) = '\0';
		 
    SC_array_string_add(tf, s);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_INIT_SUBTASKS - initialize the subtask array SUB
 *                   - from the NA tokens in TA
 *                   - return the real number of subtasks
 */

static int _SC_init_subtasks(subtask *sub, char *shell, char **ta, int na)
   {int it, n, dosh, doif, term;
    char *t, *ios;
    SC_array *tf;
    SC_thread_proc *ps;

    ps = _SC_get_thr_processes(-1);

    it    = 0;
    doif  = FALSE;
    dosh  = FALSE;
    term  = FALSE;
    ios   = NULL;

    tf = SC_STRING_ARRAY();
    SC_array_resize(tf, na, -1.0);

    for (n = 0; n < na; n++)
        {t = ta[n];

	 if ((t[0] == SC_PROCESS_DELIM) && (ps->msh_syntax == TRUE))
	    ios = t;

/* NOTE: *?[] are essential regular expression syntax elements
 * which must be handled by the shell - cannot leave any of these off
 */
	 else if (strpbrk(t, "[]()@$*?`") != NULL)
	    dosh = TRUE;

	 else if (strncmp(t, "if", 2) == 0)
	    {doif = TRUE;
	     dosh = TRUE;}

/* complete an entry at a terminal 'fi' */
	 else if ((strcmp(t, "fi\n") == 0) || (strcmp(t, "fi;\n") == 0))
	    {_SC_push_token(tf, t);
	     doif = FALSE;
	     term = TRUE;}

/* complete an entry at a terminal 'endif' */
	 else if (strcmp(t, "endif\n") == 0)
	    {_SC_push_token(tf, t);
	     doif = FALSE;
	     term = TRUE;}

/* complete an entry at a terminal ';' */
	 else if ((strcmp(t, ";\n") == 0) && (doif == FALSE))
	    term = TRUE;

	 else if (strcmp(t, SC_PIPE_DELIM) == 0)
	    ios = t;

	 if (term == TRUE)
	    {if (tf->n > 0)
	        {_SC_push_subtask(sub, it++, shell, tf, dosh, ios);
		 tf = SC_STRING_ARRAY();
		 SC_array_resize(tf, na, -1.0);};
	     ios  = NULL;
	     term = FALSE;
	     dosh = FALSE;}

/* build up an entry from non-terminal tokens */
	 else
	    _SC_push_token(tf, t);};

    if ((n >= na) && (tf->n > 0))
       _SC_push_subtask(sub, it++, shell, tf, dosh, ios);

    return(it);}

/*--------------------------------------------------------------------------*/

/*                              TASK EXECUTION                              */

/*--------------------------------------------------------------------------*/

/* _SC_INCR_TASK_COUNT - increment the task count of JOB
 *                     - usually called after a task has been
 *                     - launched
 */

static void _SC_incr_task_count(taskdesc *job, int ni, int done)
   {jobinfo *inf;
    tasklst *tl;

    if (job != NULL)
       {inf = &job->inf;
	tl  = job->command;
	if (ni < 0)
	   ni = tl->nt;

	tl->nl += ni;

	inf->ia++;

	if (done == TRUE)
	   {tl->nc++;
	    inf->ia = 0;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_HANDLE_ECHO - handle an echo command internally
 *                 - return TRUE iff successful
 */

static int _SC_handle_echo(taskdesc *job, asyncstate *as, subtask *sub)
   {int c, n, m, sc, nc, na, newl, rv;
    char *s, *t, **ta;
    jobinfo *inf;
    SC_iodes *fd;
    parstate *state;

    rv = TRUE;

    if (job != NULL)
       {state = job->context;

	SC_START_ACTIVITY(state, HANDLE_ECHO);

	inf = &job->inf;
	na  = sub->nt;
	ta  = sub->argf;

	m    = 1;
	newl = TRUE;
	if ((m < na) && (strcmp(ta[m], "-n") == 0))
	   {newl = FALSE;
	    m++;};

/* using T as a temporary concatenate the strings in TA to S */
	s = NULL;
	t = NULL;
	for (n = m; n < na; n++)
	    {t = SC_dstrcat(t, ta[n]);

	     c  = t[0];
	     sc = 0;
	     nc = strlen(t);
	     switch (c)
	        {case '\"' :
		 case '\'' :
		      sc++;
		      nc--;

		 default:
		      if (0 <= nc)
			 {t[nc++] = ' ';
			  t[nc++] = '\0';};
		      s = SC_dstrcat(s, t+sc);
		      break;};

	     t[0] = '\0';};

	CFREE(t);

/* handle the dispostion of S */
	if (n != -1)
	   {if (s == NULL)
	       s = CSTRSAVE("");
	    else
	       SC_LAST_CHAR(s) = '\0';

	    fd = sub->fd;
	    if ((fd[1].file == NULL) && (fd[2].file == NULL))
	       {job->print(job, as, "echo '%s'\n", s);

		if (newl == TRUE)
		   s = SC_dstrcat(s, "\n");

		SC_array_string_add(inf->out, s);}

	    else
	       {job->print(job, as, "%s\n", sub->command);
		job->redir(job, as, fd, s, newl);
		CFREE(s);};

	    inf->status = 0;
	    inf->signal = SC_EXITED;};

	SC_END_ACTIVITY(state);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_HANDLE_SLEEP - handle a command of the form
 *                  -     sleep <n>
 *                  - internally
 *                  - return TRUE iff successful
 */

static int _SC_handle_sleep(taskdesc *job, asyncstate *as, subtask *sub)
   {int ns, rv;
    double ds;
    char *t, **ta;
    jobinfo *inf;
    parstate *state;

    rv = TRUE;

    if (job != NULL)
       {state = job->context;

	SC_START_ACTIVITY(state, HANDLE_SLEEP);

	inf = &job->inf;
	ta  = sub->argf;

	t = ta[1];
	if (t != NULL)
	   {ds = SC_stof(t);
	    ns = 1000*ds;
	    if (ds > 0.0)
	       ns = max(ns, 1);
	    else
	       ns = 0;

	    job->print(job, as, "sleep %d msec\n", ns);

	    SC_sleep(ns);

	    job->print(job, as, "awake after %d msec\n", ns);

	    inf->status = 0;
	    inf->signal = SC_EXITED;};

	SC_END_ACTIVITY(state);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_HANDLE_CD - handle a command of the form
 *               -     cd <path>
 *               - internally
 *               - return TRUE iff successful
 */

static int _SC_handle_cd(taskdesc *job, asyncstate *as, subtask *sub)
   {int st, rv, ok;
    char *dir, **ta;
    jobinfo *inf;
    tasklst *tl;
    parstate *state;

    rv = FALSE;

    if (job != NULL)
       {state = job->context;

	SC_START_ACTIVITY(state, HANDLE_CD);

	inf = &job->inf;
	tl  = job->command;
	ta  = sub->argf;

	dir = ta[1];
	if (dir != NULL)
	   {job->print(job, as, "cdir '%s'\n", dir);

	    for (ok = TRUE; ok; )
	        {inf->ia++;

/* in order to handle retries properly advance the launch count
 * temporarily to refer to the correct job
 * each retry backs off the count to do the task again
 */
		 tl->nl++;

		 st = _SC_chg_dir(dir, &inf->directory);
		 if (st == 0)
		    {job->print(job, as,
				"new  '%s' (%d)\n", inf->directory, st);
		     rv = TRUE;
		     ok = FALSE;}

		 else
		    ok = job->retryp(job, sub, st, 0, TRUE);};

/* restore the launch count so that 
 * _SC_incr_task_count can do its job properly
 */
	    tl->nl--;

	    inf->status = st;
	    inf->signal = SC_EXITED;};

	SC_END_ACTIVITY(state);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_GET_COMMAND - get the command for the current task - OFF */

int _SC_get_command(tasklst *tl, int off)
   {int it, ntm;

    ntm = tl->nt - 1;
    it  = tl->nl - off;

    it = min(it, ntm);
    it = max(it, 0);

    return(it);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PROCESS_LOST - return TRUE if the process has been idle too long */

static int _SC_process_lost(PROCESS *pp)
   {int idle;
    double t, ifh, ifs, ift;
    SC_process_rusedes *pru;
    static double ft = -1.0, fit = -1.0, fis = -1.0, fih = -1.0;

    idle = FALSE;

    if (pp != NULL)
       {pru = pp->pru;
	if (pru != NULL)
	   {t   = pru->wct - pru->wcr;
	    t   = max(t, SMALL);
	    ift = pru->idlet / t;
	    ifs = pru->since / t;
	    ifh = pru->since / ((double) DEFAULT_HEARTBEAT);

	    idle = TRUE;

/* do not worry until the job has run more than a 60 seconds */
	    if (ft < 0.0)
	       {ft = SC_stof(getenv("SC_EXEC_LOST_TIME"));
		if (ft == 0.0)
		   ft = 60.0;};
	    idle &= (t > ft);

/* do not kill off shell scripts which are mostly idle
 * it must have been idle more than 90% of its lifetime to be considered lost
 */
	    if (fit < 0.0)
	       {fit = SC_stof(getenv("SC_EXEC_LOST_IDLE"));
		if (fit == 0.0)
		   fit = 0.9;};
	    idle &= (ift < fit);

/* kill off process that has been idle for the last 50% of its life */
	    if (fis < 0.0)
	       {fis = SC_stof(getenv("SC_EXEC_LOST_SINCE"));
		if (fis == 0.0)
		   fis = 0.5;};
	    idle &= (ifs > fis);

/* do not kill off servers unless they have lost at least 5 heartbeats */
	    if (fih < 0.0)
	       {fih = SC_stof(getenv("SC_EXEC_LOST_BEATS"));
		if (fih == 0.0)
		   fih = 5.0;};
	    idle &= (ifh > fih);};};

/* log lost processes which can be extremely hard to find without this info */
    if (idle == TRUE)
       _SC_exec_printf(NULL,
		       "***> idle process %d  sec(%.2e) idle(%.0f%%) since(%.0f%%) beats(%.2f)\n",
		       pp->id, t, 100.0*ift, 100.0*ifs, ifh);

    return(idle);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MANAGE_LAUNCHED_JOB - enter the newly opened PROCESS PP into
 *                         - the taskdesc JOB
 *                         - return TRUE iff succesfully entered
 */

static int _SC_manage_launched_job(taskdesc *job, asyncstate *as, PROCESS *pp)
   {int idp, ifd, it, pi, rv;
    char *cmd;
    tasklst *tl;
    subtask *sub;
    jobinfo *inf;
    SC_evlpdes *pe;
    SC_iodes *fd;
    parstate *state;

    rv = FALSE;

    if (job != NULL)
       {state = job->context;

	SC_START_ACTIVITY(state, MANAGE_LAUNCHED_JOB);

	pe  = state->loop;
	inf = &job->inf;

	tl  = job->command;
	it  = _SC_get_command(tl, 0);
	sub = tl->tasks + it;

	if (SC_process_alive(pp))
	   {cmd = sub->command;
	    idp = pp->id;

	    job->print(job, as,
		       "exec '%s' (pid %d)\n",
		       cmd, idp);

	    if (SC_status(pp) == SC_RUNNING)
	       {SC_unblock(pp);
		SC_set_attr(pp, SC_LINE, TRUE);

		pi = SC_register_event_loop_callback(pe, SC_PROCESS_I, pp,
						     state->acc, state->rej,
						     -1);
		SC_ASSERT(pi >= 0);};

	    job->pp = pp;

	    state->n_running++;

	    rv = TRUE;}

	else
	   {job->pp = NULL;

	    fd  = sub->fd;
	    ifd = _SC_redir_fail(fd);
	    switch (ifd)
	       {case 0 :
		     job->print(job, as,
				"no such file %s\n",
				fd[ifd].file);
		     break;
		case 1 :
		case 2 :
		     job->print(job, as,
				"%s already exists\n",
				fd[ifd].file);
		     break;

		default :
		     job->print(job, as, "exec failed\n");
		     break;};};

/* GOTCHA: what if process is already finished? */
	inf->tstart   = SC_wall_clock_time();
	inf->status   = NOT_FINISHED;
	inf->signal   = NOT_FINISHED;
	job->finished = FALSE;

	SC_END_ACTIVITY(state);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_CMND_EXEC - execute a command for _SC_handle_cmnd
 *               - return TRUE if successfully launched
 *               - otherwise return FALSE
 */

static int _SC_cmnd_exec(taskdesc *job, asyncstate *as, subtask *sub)
   {int st, to, nc;
    char *t, **ca, *icmnd[6];
    jobinfo *inf;
    PROCESS *pp;
    SC_iodes *fd;
    parstate *state;

    st = FALSE;

    if (job != NULL)
       {state = job->context;

	SC_START_ACTIVITY(state, COMMAND_EXEC);

	inf = &job->inf;
	fd  = sub->fd;

	if (sub->need == FALSE)
	   ca = sub->argf;

	else
	   {nc = 0;
	    t  =_SC_shell_no_rc_cmd(sub->shell);
	    icmnd[nc++] = sub->shell;
	    if (t[0] == '-')
	       icmnd[nc++] = t;
	    icmnd[nc++] = "-c";
	    icmnd[nc++] = sub->command;
	    icmnd[nc++] = NULL;

	    ca = icmnd;

	    job->print(job, as, "shell %s\n", sub->shell);};
    
	to = state->open_retry;

	st = _SC_chg_dir(NULL, &inf->directory);

	pp = SC_open(ca, state->env, "apvo",
#if 0
                     "IO-DES", fd, SC_N_IO_CH,
#else
		     "STDIN",  fd[0].file, fd[0].flag,
		     "STDOUT", fd[1].file, fd[1].flag,
		     "STDERR", fd[2].file, fd[2].flag,
#endif
		     "OPEN-RETRY-TIME", (int) to,
		     "RING-EXP", (int) 18,
		     NULL);

	if (pp == NULL)
	   job->print(job, as, "exec failed %d\n", SC_gs.errn);
	else
	   pp->lost = _SC_process_lost;

	st = _SC_manage_launched_job(job, as, pp);

	SC_END_ACTIVITY(state);};

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_DO_COMMAND - do a command that is guaranteed to be simple
 *                - return the job status this will be
 *                -    NOT_FINISHED if the command was launched
 *                -    0            if the command completes and succeeds
 *                -    n            if the command failed
 *                - launched may or may not mean completed
 */

static int _SC_do_command(taskdesc *job, asyncstate *as, int it)
   {int rv, st, done, fl;
    char *t, **ta;
    tasklst *tl;
    subtask *sub;

    st = TRUE;

    if (job != NULL)
       {tl  = job->command;
	sub = tl->tasks + it;
	ta  = sub->argf;

/* initialize the zero read count for each new task */
	job->nzip = 0;

	t  = ta[0];
	fl = ((sub->need == FALSE) && (sub->pipe == FALSE));

/* handle echo */
	if ((strcmp(t, "echo") == 0) && (fl == TRUE))
	   done = _SC_handle_echo(job, as, sub);

/* handle sleep */
	else if ((strcmp(t, "sleep") == 0) && (fl == TRUE))
	   done = _SC_handle_sleep(job, as, sub);

/* handle cd */
	else if ((strcmp(t, "cd") == 0) && (fl == TRUE))
	   done = _SC_handle_cd(job, as, sub);

/* anything else is a command that must be exec'd one way or the other */
	else
	   {rv = _SC_cmnd_exec(job, as, sub);
	    SC_ASSERT(rv == TRUE);
	    done = FALSE;};

	_SC_incr_task_count(job, 1, done);

	st = job->inf.status;};

    return(st);}

/*--------------------------------------------------------------------------*/

/*                         TASKLIST MANAGEMENT                              */

/*--------------------------------------------------------------------------*/

/* _SC_MAKE_TASKLST - initialize a task list from a command string CMD */

static tasklst *_SC_make_tasklst(char *shell, char *cmd)
   {int n, na, nt;
    char **ta, *s;
    subtask *sub;
    tasklst *tl;

    tl = CMAKE(tasklst);

/* convert the command from ( ... ) form to just ... form */
    s = _SC_prep_command(cmd, FALSE);

/* tokenize the command to facilitate looking for simple commands */

#if 0

/* this one takes the delimiter off */
    ta = SC_tokenizef(s, " \t\n\r", ADD_DELIMITER | EXPAND_ESC);
    SC_ptr_arr_len(na, ta);
    for (n = 0; n < na; n++)
        SC_LAST_CHAR(ta[n]) = '\n';

#else

/* this one leaves the delimiter on which is needed to distinguish
 * the newlines from other delimiters
 */
    ta = SC_tokenize_literal(s, " \t\n\r", TRUE, 3);

#endif

    SC_ptr_arr_len(na, ta);

    _SC_subst_task_env(na, ta);

    CFREE(s);

/* count the apparent subtasks
 * constructs such as (cd .. ; ) would look like 2 subtasks
 */
    for (nt = na, n = 0; n < na; n++)
        nt += PS_strcnts(ta[n], ";\n", FALSE);

    sub = CMAKE_N(subtask, 2*nt);

/* initialize and get the real number of subtasks
 * constructs such as (cd .. ; ) would actually have 1 subtask
 */
    nt = _SC_init_subtasks(sub, shell, ta, na);

    SC_free_strings(ta);

    tl->nl     = 0;
    tl->nc     = 0;
    tl->nt     = nt;
    tl->status = 0;
    tl->tasks  = sub;

    return(tl);}

/*--------------------------------------------------------------------------*/

/*                                 JOB MANAGEMENT                           */

/*--------------------------------------------------------------------------*/

/* _SC_RUN_NEXT_TASK - execute the next task on the task list of JOB
 *                   - if called from SIGCHLD handler SIGP is TRUE
 *                   - return FALSE if there are no more jobs to launch
 *                   - otherwise return TRUE
 */

static int _SC_run_next_task(taskdesc *job, int sigp)
   {int rtry, st, rv;
    tasklst *tl;
    asyncstate *as;
    parstate *state;

    state = job->context;

    SC_START_ACTIVITY(state, RUN_NEXT_TASK);

    _SC_set_run_task_state(state);

    as = NULL;
    tl = job->command;
    st = 0;

    job->print(job, as,
	       "next task fin(%d) cmp(%d) lch(%d) n(%d) st(%d)\n",
	       job->finished, tl->nc, tl->nl, tl->nt, tl->status);

    if (job->finished == FALSE)
       {while (tl->nl < tl->nt)
	   {rtry = job->close(job, TRUE);
	    if (job->finished == TRUE)
	       break;

	    st = _SC_do_command(job, as, tl->nl);
	    if (st != 0)
	       break;};}

    rv = !job->finished;

    if ((tl->nl >= tl->nt) && (st != NOT_FINISHED))
       {if (state->finish != NULL)
	   rtry = state->finish(job, _SC_EXEC_COMPLETE);
	else
	   rtry = job->close(job, TRUE);

	if (rtry == TRUE)
	   {rv            = TRUE;
	    job->finished = FALSE;
	    job->exec(job, sigp);}
	else
	   rv = FALSE;};

    SC_END_ACTIVITY(state);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_TASK_RETRY_P - look at task status and decide whether to retry
 *                  - return TRUE iff retry is to be done
 */

static int _SC_task_retry_p(taskdesc *job, subtask *sub,
			    int sts, int sgn, int setst)
   {int it, ok, rv;
    char *cmd;
    tasklst *tl;
    jobinfo *inf;
    parstate *state;
    SC_process_rusedes *pru;

    rv = FALSE;

    if (job != NULL)
       {state = job->context;
	inf   = &job->inf;
	tl    = job->command;
	it    = _SC_get_command(tl, 1);
	cmd   = sub->command;

	ok = _SC_decide_retry(NULL, inf, tl, sts);

/* if retrying - report and backup the tasklist to make the
 * failed one be the next one launched in job->exec
 */
	if (ok == 1)
	   {if (inf->na > 1)
	       {job->print(job, NULL, "retry '%s' attempt %d\n", cmd, it);
		tl->nl = it;};

	    rv = TRUE;}

/* if done wrap up for this task */
	else
	   {job->print(job, NULL, "done (%d/%d) '%s'\n", sts, sgn, cmd);

	    if (job->pp != NULL)
	       {pru = job->pp->pru;
		job->print(job, NULL,
			   "used wc(%.2f) usr(%.2f) sys(%.2f) idle(%.2f) tail(%.2f) mem(%.2f)\n",
			   pru->wct, pru->usrt, pru->syst,
			   pru->idlet, pru->since, pru->mem);};

	    if ((sts != 0) && (state->ignore == FALSE))
	       {tl->nl        = tl->nt;
		tl->nc        = tl->nt;
		job->finished = TRUE;}
	    else
	       tl->nc++;

	    inf->ia = 0;

	    if (setst == TRUE)
	       {inf->status = sts;
		inf->signal = sgn;};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_TASK_DONE - process a task that is finished
 *               - all tasks that are successfully launched
 *               - should come through here after they exit
 *               - return TRUE iff retry is to be done
 */

static int _SC_task_done(taskdesc *job, int setst)
   {int it, rv, sts, sgn;
    PROCESS *pp;
    tasklst *tl;
    subtask *sub;
    PFFileCallback acc;
    parstate *state;

    rv = FALSE;

    if (job != NULL)
       {state = job->context;

	SC_START_ACTIVITY(state, TASK_DONE);

	tl = job->command;

/* if the current task is already done then the
 * number launched, nl, will be equal to number complete, nc
 */
	if (tl->nl > tl->nc)
	   {pp  = job->pp;

	    it  = _SC_get_command(tl, 1);
	    sub = tl->tasks + it;

	    sts = pp->reason;
	    sgn = pp->status;

	    acc = state->acc;
	    if (acc != NULL)
	       (*acc)(pp->io[0], 0, state);

	    if (job->nzip > 3)
	       job->print(job, NULL,
			  "%d empty reads during execution\n", job->nzip);

	    rv = job->retryp(job, sub, sts, sgn, setst);};

	SC_END_ACTIVITY(state);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PRINT_REDIR - write the string S to the appropriate
 *                 - file descriptors of FD
 */

static void _SC_print_redir(taskdesc *job, asyncstate *as,
			    SC_iodes *fd, char *s, int newl)
   {int ffe, ffo, df, nc;
    char *fne, *fno;
    mode_t p;

    if (s != NULL)
       {nc = strlen(s);

	fno = fd[1].file;
	ffo = fd[1].flag;
	fne = fd[2].file;
	ffe = fd[2].flag;

	p = SC_get_perm(FALSE);

/* if the files have the same name only write stdout */
	if ((fno != NULL) && (fne != NULL))
	   {if (strcmp(fno, fne) == 0)
	       {ffe = 0;
		fne = NULL;};};

/* write stdout */
	if (fno != NULL)
	   {df = SC_open_safe(fno, ffo, p);
	    if (df != -1)
	       {SC_write_sigsafe(df, s, nc);
		if (newl == TRUE)
		   SC_write_sigsafe(df, "\n", 1);
		SC_close_safe(df);}
	    else
	       job->print(job, as,
			  "error opening file '%s' mode 0%o - %d\n",
			  fno, ffo, df);};

/* write stderr */
	if (fne != NULL)
	   {df = SC_open_safe(fne, ffe, p);
	    if (df != -1)
	       {SC_write_sigsafe(df, s, nc);
		if (newl == TRUE)
		   SC_write_sigsafe(df, "\n", 1);
		SC_close_safe(df);}
	    else
	       job->print(job, as,
			  "error opening file '%s' mode 0o%o - %d\n",
			  fne, ffe, df);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_STATE_CHECK_PROCESS - examine the status of
 *                         - the current process in JOB
 */

static void _SC_state_check_process(taskdesc *job, asyncstate *as,
				    int *pnr, int *pnc)
   {int id, more, sgn, sts;
    int nr, nc;
    PROCESS *pp;
    jobinfo *inf;
    parstate *state;

    if (job != NULL)
       {state = job->context;
	pp    = job->pp;
	inf   = &job->inf;
	id    = inf->id;
	sgn   = inf->signal;
	sts   = inf->status;

	nr = *pnr;
	nc = *pnc;

	if (pp != NULL)
	   pp->lost = _SC_process_lost;

	if (SC_process_status(pp) != SC_RUNNING)
	   {if ((sgn != NOT_FINISHED) && ((sgn & SC_EXITED) == 0))
	       _SC_server_printf(as, state, _SC_EXEC_INFO,
				 "a task in job %d died abnormally %d/%d\n",
				 id, sts, sgn);

	    if ((sgn != NOT_FINISHED) && ((sgn & SC_LOST) == 0))
	       _SC_server_printf(as, state, _SC_EXEC_INFO,
				 "a task in job %d was lost %d/%d\n",
				 id, sts, sgn);

	    more = job->exec(job, FALSE);
	    if (more == FALSE)
	       nc++;
	    else
	       nr++;}

	else
	   nr++;

	*pnr = nr;
	*pnc = nc;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LAUNCH_JOB - exec the JOB command
 *                - return TRUE iff successful
 *                - this is a server side routine
 */

static int _SC_launch_job(taskdesc *job, asyncstate *as)
   {int rv, more;
    char *cm, *dr, *hs, *cmc;
    jobinfo *inf;
    PROCESS *pp;

    rv = FALSE;

    if (job != NULL)
       {hs  = job->host;
	inf = &job->inf;

	dr = inf->directory;
	cm = inf->full;

	_SC_setup_output(inf, "_SC_LAUNCH_JOB");

/* launch job on remote host
 * NOTE: by doing SC_open_remote we do a SC_verify_host
 * this means two SSHes and the consequent extra time
 * on the other hand a simple _SC_ping_host has resulted in
 * a hang
 */
	if (hs != NULL)
	   {pp = SC_open_remote(hs, inf->full, NULL, NULL, "ap", NULL);
	    if (pp == NULL)
	       job->print(job, as,
			  "remote exec on %s failed %d\n",
			  hs, SC_gs.errn);
	    else
	       pp->lost = _SC_process_lost;

	    rv = _SC_manage_launched_job(job, as, pp);

	    _SC_incr_task_count(job, -1, FALSE);}

/* launch job on current host */
	else
	   {rv  = TRUE;
	    cmc = CSTRSAVE(cm);

	    _SC_chg_dir(dr, &inf->directory);

	    more = job->exec(job, FALSE);
	    if (more == FALSE)
	       {inf->status = 0;
		inf->signal = SC_EXITED;};

	    CFREE(cmc);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FIN_JOB - when a JOB is finished
 *             - handle the termination, output, and retry logic
 *             - return TRUE iff the JOB is to be retried
 */

static int _SC_fin_job(taskdesc *job, asyncstate *as, int srv)
   {int pid, rtry;
    char *p, **out;
    jobinfo *inf;
    parstate *state;

    rtry = FALSE;

    if (job != NULL)
       {state = job->context;

	SC_START_ACTIVITY(state, FIN_JOB);

	inf = &job->inf;
	if (inf == NULL)
	   job->print(job, as,
		      "***> no job info - _SC_FIN_JOB\n");

	pid = (state->tagio == TRUE) ? inf->id : -1;

	rtry = job->close(job, TRUE);
	if (rtry == FALSE)
	   {if (srv == FALSE)
	       {p = _SC_show_command(as, inf->full, state->show);
		SC_ASSERT(p != NULL);};

/* print job output thru filter */
	    if (SC_array_get_n(inf->out) == 0L)
	       job->print(job, as,
			  "***> no output for job - _SC_FIN_JOB\n");

	    else
	       {out = _SC_array_string_join(&inf->out);

		_SC_print_filtered(as, out, job->filter, pid, job->host);

		SC_free_strings(out);

		inf->out = CMAKE_ARRAY(char *, NULL, 0);};

	    state->n_complete++;};

	SC_END_ACTIVITY(state);};

    return(rtry);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_START_JOB - execute command CM using SHELL
 *               - upto NA times before reporting failure
 *               - output is passed through FILTER before printing
 *               - SHOW controls command echoing
 *               - this is a server side routine
 */

static int _SC_start_job(taskdesc *job, asyncstate *as, int launch)
   {int ok;
    jobinfo *inf;
    parstate *state;

    ok = FALSE;

    if (job != NULL)
       {state = job->context;

	SC_START_ACTIVITY(state, START_JOB);

	inf = &job->inf;

	if (launch == TRUE)
	   {ok = job->launch(job, as);
	    if (ok == FALSE)
	       CFREE(job);}
	else
	   {_SC_setup_output(inf, "_SC_START_JOB");
	    ok = TRUE;};

	SC_END_ACTIVITY(state);};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_CLOSE_JOB_PROCESS - close the current process of the JOB properly */

static int _SC_close_job_process(taskdesc *job, int setst)
   {int rtry, ok;
    char *p;
    PROCESS *pp;
    jobinfo *inf;
    parstate *state;

    rtry = FALSE;

    if (job != NULL)
       {state = job->context;

	if (state != NULL)
	   {SC_START_ACTIVITY(state, CLOSE_JOB);

	    inf = &job->inf;

	    p = SC_datef();
	    if (p != NULL)
	       {SC_strncpy(inf->stop_time, 32, p, -1);
		CFREE(p);}
	    else
	       SC_strncpy(inf->stop_time, 32, "0", -1);

	    pp = job->pp;
	    if (SC_process_alive(pp))
	       {if (pp->stop_time != NULL)
		   SC_strncpy(inf->stop_time, 32, pp->stop_time, -1);

		rtry = job->done(job, setst);

		if (state != NULL)
		   SC_remove_event_loop_callback(state->loop, SC_PROCESS_I, pp);

		ok = SC_close(pp);
		SC_ASSERT(ok == TRUE);};

	    job->pp = NULL;

	    SC_END_ACTIVITY(state);};};

    return(rtry);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_JOB_TAG - return the tag with JOB id, task, and time */

static void _SC_job_tag(taskdesc *job, char *tag, int nc, char *tm)
   {int id, ic, il, it, rl;
    char *tms;

    rl = FALSE;
    if (tm == NULL)
       {tm = SC_datef();
	rl = TRUE;};

    tms = tm + 11;

    if (job != NULL)
       {id = job->inf.id;
	il = job->command->nl;
	ic = job->command->nc;
	it = min(il, ic);

	snprintf(tag, nc, "[Job %2d/%d %s]", id, it, tms);}
    else
       snprintf(tag, nc, "[Job --/- %s]", tms);

    if (rl)
       CFREE(tm);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_JOB_PRINT - print to both the STATE log
 *               - and if STATE is a server state to the parent
 *               - and AS log if it exists
 */

int _SC_job_print(taskdesc *job, asyncstate *as, char *fmt, ...)
   {int rv;
    char tag[MAXLINE];
    char *msg;
    parstate *state;

    rv = FALSE;

    if (job != NULL)
       {state = job->context;

	SC_START_ACTIVITY(state, STATE_PRINTF);

	rv = TRUE;

	SC_VDSNPRINTF(TRUE, msg, fmt);

	job->tag(job, tag, MAXLINE, NULL);

	state->print(state, "%s %s", tag, msg);

	if (state->server == TRUE)
	   _SC_exec_printf(as, "%s %s %s", _SC_EXEC_SRV_ID, tag, msg);

	CFREE(msg);

	SC_END_ACTIVITY(state);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_REPORT_JOB_EXIT - log the fact that the process of JOB has exited */

static void _SC_report_job_exit(taskdesc *job, char *where)
   {int it, idp;
    char *cmd, *hnd;
    PROCESS *pp;
    tasklst *tl;
    subtask *sub;
    SC_contextdes sc;
    parstate *state;

    state = job->context;

    SC_START_ACTIVITY(state, REPORT_EXIT);

    tl  = job->command;
    pp  = job->pp;

    it  = _SC_get_command(tl, 1);
    sub = tl->tasks + it;

    cmd = sub->command;
    idp = pp->id;

    sc = SC_which_signal_handler(SIGCHLD);
    if (sc.f == SIG_IGN)
       hnd = "off";
    else
       hnd = "on";

    job->print(job, NULL,
	       "term %s %s '%s' (pid %d)\n",
	       where, hnd, cmd, idp);

    SC_END_ACTIVITY(state);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ADD_JOB - add a JOB to an array of jobs TA */

static void _SC_add_job(taskdesc *job)
   {int i, n;
    taskdesc *tsk;
    SC_array *ta;

    ta = job->context->tasks;
    n  = SC_array_get_n(ta);
    for (i = 0; i < n; i++)
        {tsk = *(taskdesc **) SC_array_get(ta, i);
	 if (tsk == NULL)
	    {SC_array_set(ta, i, &job);
	     break;};};

    if (i >= n)
       SC_array_push(ta, &job);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_REMOVE_JOB - remove a JOB from an array of jobs TA
 *                - return TRUE if JOB was freed
 */

static int _SC_remove_job(taskdesc *job)
   {int i, n, ok;
    taskdesc *tsk;
    SC_array *ta;

    ok = FALSE;
    ta = job->context->tasks;
    n  = SC_array_get_n(ta);
    for (i = 0; i < n; i++)
        {tsk = *(taskdesc **) SC_array_get(ta, i);
	 if (tsk == job)
	    {SC_array_set(ta, i, NULL);
	     CFREE(job);
	     SC_array_set(ta, i, &job);
	     ok = TRUE;
	     break;};};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MAKE_TASKDESC - allocate and initialize a taskdesc */

taskdesc *SC_make_taskdesc(parstate *state, int jid,
			   char *host, char *shell, char *dir, char *cmd)
   {taskdesc *job;
    jobinfo *inf;

    job = CMAKE(taskdesc);
    if (job != NULL)
       {inf = &job->inf;

	shell = SC_get_shell(shell);

	if ((dir == NULL) || (strcmp(dir, ".") == 0))
	   dir = state->directory;

	if (dir != NULL)
	   dir = CSTRSAVE(dir);

	inf->id        = jid;
	inf->ia        = 0;
	inf->na        = state->na;
	inf->shell     = CSTRSAVE(shell);
	inf->directory = dir;
	inf->full      = CSTRSAVE(cmd);
	inf->tstart    = 0.0;

	job->host     = CSTRSAVE(host);
	job->arch     = NULL;
	job->command  = _SC_make_tasklst(shell, cmd);
	job->finished = FALSE;
	job->pp       = NULL;
	job->filter   = state->filter;
	job->context  = state;
	job->nzip     = 0;

	job->start    = _SC_start_job;
	job->launch   = _SC_launch_job;
	job->done     = _SC_task_done;
	job->retryp   = _SC_task_retry_p;
	job->check    = _SC_state_check_process;
	job->finish   = _SC_fin_job;
	job->close    = _SC_close_job_process;
	job->redir    = _SC_print_redir;
	job->tag      = _SC_job_tag;
	job->print    = _SC_job_print;
	job->report   = _SC_report_job_exit;
	job->exec     = _SC_run_next_task;

	job->add      = _SC_add_job;
	job->remove   = _SC_remove_job;};

    return(job);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
