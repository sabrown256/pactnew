/*
 * SCLEH.C - line editing and history
 *         - following linenoise by Salvatore Sanfilippo and Pieter Noordhuis
 */

#include "cpyright.h"

#include "score_int.h"
#include "scope_term.h"

#undef ESC

#define MAX_HIST_N   100

#define CTRL_A       0x01
#define CTRL_B       0x02
#define CTRL_C       0x03
#define CTRL_D       0x04
#define CTRL_E       0x05
#define CTRL_F       0x06
#define CTRL_G       0x07
#define CTRL_H       0x08
#define CTRL_I       0x09
#define CTRL_J       0x0a
#define CTRL_K       0x0b
#define CTRL_L       0x0c
#define CTRL_M       0x0d
#define CTRL_N       0x0e
#define CTRL_O       0x0f
#define CTRL_P       0x10
#define CTRL_Q       0x11
#define CTRL_R       0x12
#define CTRL_S       0x13
#define CTRL_T       0x14
#define CTRL_U       0x15
#define CTRL_V       0x16
#define CTRL_W       0x17
#define CTRL_X       0x18
#define CTRL_Y       0x19
#define CTRL_Z       0x1a
#define ESC          0x1b
#define BSP          0x7f

typedef struct s_lehcmp lehcmp;
typedef struct s_lehdes lehdes;
typedef struct s_lehloc lehloc;

typedef void (*PFlehcb)(const char *s, lehcmp *lc);
typedef int (*PFlehact)(lehloc *lp);

struct s_lehcmp
   {size_t len;
    char **cvec;};

struct s_lehdes
   {int fd;
    int raw;
    int exr;
    int nhx;             /* maximum number of history items */
    int nh;              /* current number of history items */
    char **hist;
    PFread read;
    PFlehact *map;
    PFlehcb cmp_cb;};

struct s_lehloc
   {int fd;
    int hind;             /* index of current history item */
    int np;               /* size of prompt text */
    int have_prompt;      /* flag for presence of prompt */
    size_t nb;            /* size of current line buffer */
    size_t pos;           /* position in current line */
    size_t len;           /* current line length (<= cols) */
    size_t cols;          /* screen line width */
    int c;
    char *bf;             /* current line buffer */
    char *prompt;};       /* prompt text */

static lehdes
 _SC_leh = { -1, FALSE, 0, MAX_HIST_N, 0, NULL,
	     read, NULL, NULL, };

static int
 trace = FALSE;

static FILE
 *fdbg = NULL;

void
 SC_leh_cmp_set_cb_cmp_cb(PFlehcb *f), 
 SC_leh_cmp_add(lehcmp *lc, char *s);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_SUP_TERMP - return TRUE iff the current TERM is supported */

static int _SC_leh_sup_termp(void)
   {int i, rv;
    char *term;
    static char *unsup[] = {"dumb", "cons25", NULL};

    rv = TRUE;

    term = getenv("TERM");
    if (term != NULL)
       {for (i = 0; unsup[i]; i++)
	    if (SC_str_icmp(term, unsup[i]) == 0)
	       {rv = FALSE;
		break;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_FREE_HIST - free the history list */

static void _SC_leh_free_hist(void)
   {int i;

    if (_SC_leh.hist != NULL)
       {for (i = 0; i < _SC_leh.nh; i++)
            CFREE(_SC_leh.hist[i]);

        CFREE(_SC_leh.hist);};

   return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_DISA_RAW - put FD into cooked mode */

static void _SC_leh_disa_raw(int fd)
   {int rv;

    if (_SC_leh.raw == TRUE)
       {rv = SC_set_cooked_state(fd, FALSE);
	if (rv == TRUE)
	   _SC_leh.raw = FALSE;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_DONE - at exit try to restore the terminal
 *              - to its initial conditions
 */

static void _SC_leh_done(void)
   {int fd;

    fd = _SC_leh.fd;
    if (fd != -1)
       {_SC_leh_disa_raw(fd);

	_SC_leh_free_hist();

	CFREE(_SC_leh.map);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_ENA_RAW - put FD into raw mode */

static int _SC_leh_ena_raw(int fd)
   {int rv;

    rv = FALSE;

    if (isatty(fd) == TRUE)
       {if (_SC_leh.exr == FALSE)
	   {atexit(_SC_leh_done);
	    _SC_leh.exr = TRUE;};

	rv = SC_set_io_attrs(fd,
			     BRKINT,  SC_TERM_INPUT,     FALSE,
			     ICRNL,   SC_TERM_INPUT,     FALSE,
			     INPCK,   SC_TERM_INPUT,     FALSE,
			     ISTRIP,  SC_TERM_INPUT,     FALSE,
			     IXON,    SC_TERM_INPUT,     FALSE,
			     OPOST,   SC_TERM_OUTPUT,    FALSE,
			     ICANON,  SC_TERM_LOCAL,     FALSE,
			     ECHO,    SC_TERM_LOCAL,     FALSE,
			     IEXTEN,  SC_TERM_LOCAL,     FALSE,
			     ISIG,    SC_TERM_LOCAL,     FALSE,
			     CS8,     SC_TERM_CONTROL,   TRUE,
			     VMIN,    SC_TERM_CHAR,      1,
			     VTIME,   SC_TERM_CHAR,      0,
			     0);};

    if (rv == TRUE)
       _SC_leh.raw = TRUE;

    else
       errno = ENOTTY;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_GET_NCOL - return the number of columns */

static int _SC_leh_get_ncol(void)
   {int rv;
    struct winsize ws;

    rv = 80;
    if (ioctl(1, TIOCGWINSZ, &ws) != -1)
       rv = ws.ws_col;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_PUT_PROMPT - handle all logic for writing the prompt
 *                    - return TRUE iff successful
 */

static int _SC_leh_put_prompt(lehloc *lp)
   {int fd, rv, nb;
    size_t np;
    char *prompt;
    static char *linsp = "\f\r\n";

    fd = lp->fd;

/* if we have no prompt check for a saved prompt value */
    if (lp->prompt == NULL)
       {lp->prompt = _SC.elbf;
	lp->have_prompt = TRUE;}
    else
       lp->have_prompt = FALSE;

    if (lp->prompt == NULL)
       lp->np = 0;

/* handle case of prompt string with newlines, and so forth */
    else if (strchr(linsp, lp->prompt[0]) != NULL)
       {np = strspn(lp->prompt, linsp);
	lp->prompt = (char *) (lp->prompt + np);
	lp->np     = strlen(lp->prompt);

        nb = write(fd, "\n", 1);
        rv = (nb == 1);}

    else
       lp->np = strlen(lp->prompt);

    np     = lp->np;
    prompt = lp->prompt;

    rv = TRUE;
    if ((prompt != NULL) && (np > 0) && (lp->have_prompt == FALSE))
       rv = (write(fd, prompt, np) != -1);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_REFRESH - redraw the current line */

static void _SC_leh_refresh(lehloc *lp)
   {int fd, dp, err;
    size_t nw, len, np, pos, cols;
    char seq[64];
    char *bf;

    fd   = lp->fd;
    bf   = lp->bf;
    len  = lp->len;
    pos  = lp->pos;
    cols = lp->cols;
    np   = lp->np;

    dp = np + pos - cols + 1;
    if (dp > 0)
       {bf  += dp;
	len -= dp;
	pos -= dp;};

    dp = np + len - cols;
    if (dp > 0)
       len -= dp;

    err = FALSE;

/* move cursor to left edge - 'ESC [ n G' moves cursor to column n */
    if (err == FALSE)
       {snprintf(seq, 64, "\x1b[0G");
	nw   = write(fd, seq, strlen(seq));
	err |= (nw == -1);};

/* write the prompt */
    if (err == FALSE)
       err |= (_SC_leh_put_prompt(lp) == FALSE);

/* write the current buffer content */
    if (err == FALSE)
       {nw   = write(fd, bf, len);
	err |= (nw == -1);};

/* erase to right - 'ESC [ n K'
 *   if n is 0 or missing, clear from cursor to EOL
 *   if n is 1, clear from BOL line to cursor
 *   if n is 2, clear entire line
 */
    if (err == FALSE)
       {snprintf(seq, 64, "\x1b[0K");
	nw   = write(fd, seq, strlen(seq));
	err |= (nw == -1);};

/* move cursor to original position
 * 'ESC [ n C' move cursor forward n chars
 */
    if (err == FALSE)
       {snprintf(seq, 64, "\x1b[0G\x1b[%dC", (int) (pos + np));
	nw   = write(fd, seq, strlen(seq));
	err |= (nw == -1);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_BEEP - sound a terminal beep */

static void _SC_leh_beep(void)
   {

    fprintf(stderr, "\x7");
    fflush(stderr);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_FREE_CMP - free the completions LC */

static void _SC_leh_free_cmp(lehcmp *lc)
   {size_t i;

    if ((lc != NULL) && (lc->cvec != NULL))
       {for (i = 0; i < lc->len; i++)
	    CFREE(lc->cvec[i]);

	CFREE(lc->cvec);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_COMPLETE - complete the line BF
 *                  - return last read character
 */

static int _SC_leh_complete(lehloc *lp)
   {int nr, nw, ok;
    size_t i;
    char c;
    lehcmp lc = { 0, NULL };

    c = 0;

    _SC_leh.cmp_cb(lp->bf, &lc);
    if (lc.len == 0)
       _SC_leh_beep();

    else
       {ok = FALSE;
	for (i = 0; ok == FALSE; )

/* show completion or original buffer */
	    {if (i < lc.len)
	        {size_t clen;
		 lehloc loc;

		 clen = strlen(lc.cvec[i]);

		 loc     = *lp;
		 loc.bf  = lc.cvec[i];
		 loc.pos = clen;
		 loc.len = clen;

                 _SC_leh_refresh(&loc);}

	     else
	        _SC_leh_refresh(lp);

	     nr = _SC_leh.read(lp->fd, &c, 1);
	     if (nr <= 0)
	        {_SC_leh_free_cmp(&lc);
                 return(-1);};

	     switch(c)

/* tab */
	        {case CTRL_I :
                      i = (i+1) % (lc.len+1);
                      if (i == lc.len)
		         _SC_leh_beep();
		      break;

/* escape - re-display original buffer */
                 case ESC :
                      if (i < lc.len)
                         _SC_leh_refresh(lp);
		      ok = TRUE;
		      break;

/* update buffer and return */
                 default :
                      if (i < lc.len)
		         {nw   = snprintf(lp->bf, lp->nb, "%s", lc.cvec[i]);
			  lp->len = nw;
			  lp->pos = nw;};
		      ok = TRUE;
		      break;};};};

    _SC_leh_free_cmp(&lc);

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_LEH_CLEAR - clear the screen
 *              -    ESC [ H ESC [ 2 J
 *              - this is really two sequences
 *              -    ESC [ H      moves the cursor to upper left corner
 *              -    ESC [ 2 J    clear the whole screen
 */

void SC_leh_clear(int fd)
   {int nr;

    nr = write(fd, "\x1b[H\x1b[2J", 7);

    SC_ASSERT(nr == 7);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_LEFT - move left in the current line */

static int _SC_leh_left(lehloc *lp)
   {int rv;

    rv = TRUE;

    if (lp->pos > 0)
       {lp->pos--;
	_SC_leh_refresh(lp);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_RIGHT - move right in the current line */

static int _SC_leh_right(lehloc *lp)
   {int rv;

    rv = TRUE;

    if (lp->pos != lp->len)
       {lp->pos++;
	_SC_leh_refresh(lp);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_UP_DOWN - move up/down through history */

static int _SC_leh_up_down(lehloc *lp, int wh)
   {int ih, nh, rv;
    size_t ie;

    rv = TRUE;

    if (_SC_leh.nh > 1)
       {ih = lp->hind;
	nh = _SC_leh.nh;

/* update the current history entry before
 * overwriting it with the next one
 */
	ie = nh - 1 - ih;
	CFREE(_SC_leh.hist[ie]);
	_SC_leh.hist[ie] = CSTRSAVE(lp->bf);

/* display the new entry */
	ih += (wh == 'A') ? 1 : -1;

	if (ih < 0)
	   ih = 0;

	else if (ih >= nh)
	   ih = nh - 1;

	else
	   {ie = nh - 1 - ih;
	    SC_strncpy(lp->bf, lp->nb, _SC_leh.hist[ie], -1);

	    lp->pos  = strlen(lp->bf);
	    lp->len  = lp->pos;
	    lp->hind = ih;
	    _SC_leh_refresh(lp);

	    if (fdbg != NULL)
	       {fprintf(fdbg, ": <up/down> %d %d", ih, nh);
		fflush(fdbg);};};

	lp->hind = ih;};

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                             EDITING ACTIONS                              */

/*--------------------------------------------------------------------------*/

/* _SC_LEH_ANY - any character not otherwise trapped */

static int _SC_leh_any(lehloc *lp)
   {int c, fd, rv;
    size_t pos, len, cols, nb;
    char *bf;

    c    = lp->c;
    fd   = lp->fd;
    len  = lp->len;
    pos  = lp->pos;
    cols = lp->cols;
    nb   = lp->nb;

    rv = TRUE;

    if (len < nb)
       {bf = lp->bf;
	if (len == pos)
	   {bf[pos] = c;
	    lp->pos++;
	    lp->len++;
	    bf[lp->len] = '\0';

/* avoid a full update of the line in the trivial case */
	    if (lp->np+lp->len < cols)
	       {if (write(fd, &c, 1) == -1)
		   rv = FALSE;}
	    else
	       _SC_leh_refresh(lp);}

        else
	   {memmove(bf+pos+1, bf+pos, len-pos);
	    bf[pos] = c;
	    lp->len++;
	    lp->pos++;
	    bf[lp->len] = '\0';
	    _SC_leh_refresh(lp);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_DEL - delete escape sequence */

static int _SC_leh_del(lehloc *lp)
   {size_t pos, len;
    char *bf;

    pos = lp->pos;
    len = lp->len;

    if ((0 < len) && (pos < len))
       {bf = lp->bf;
	memmove(bf+pos, bf+pos+1, len-pos-1);
	lp->len--;
	bf[lp->len] = '\0';
	_SC_leh_refresh(lp);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_DEL_BACK - delete the previous character (Ctrl-H) */

static int _SC_leh_del_back(lehloc *lp)
   {size_t pos, len;
    char *bf;

    pos = lp->pos;
    len = lp->len;
    if ((0 < pos) && (0 < len))
       {bf = lp->bf;
	memmove(bf+pos-1, bf+pos, len-pos);
	lp->pos--;
	lp->len--;
	bf[lp->len] = '\0';
	_SC_leh_refresh(lp);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_DEL_CURR - delete the current character (Ctrl-D) */

static int _SC_leh_del_curr(lehloc *lp)
   {int rv;
    size_t pos, len;
    char *bf;

    rv = TRUE;

    pos = lp->pos;
    len = lp->len;
    if ((1 < len) && (pos < len-1))
       {bf = lp->bf;
	memmove(bf+pos, bf+pos+1, len-pos);
	lp->len--;
	bf[lp->len] = '\0';
	_SC_leh_refresh(lp);}

    else if (len == 0)
       {_SC_leh.nh--;
	CFREE(_SC_leh.hist[_SC_leh.nh]);
	rv = FALSE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_EXCH_CHAR - exchange the current and
 *                   - previous characters (Ctrl-T)
 */

static int _SC_leh_exch_char(lehloc *lp)
   {size_t pos, len;
    char *bf;

    pos = lp->pos;
    len = lp->len;

    if ((pos > 0) && (pos < len))
       {bf = lp->bf;

	SC_SWAP_VALUE(int, bf[pos], bf[pos-1]);

	if (pos != len-1)
	   lp->pos++;

	_SC_leh_refresh(lp);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_NEXT_LINE - goto the next line (Ctrl-N) */

static int _SC_leh_next_line(lehloc *lp)
   {int rv;

    rv = _SC_leh_up_down(lp, 'B');

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_PREV_LINE - goto the previous line (Ctrl-P) */

static int _SC_leh_prev_line(lehloc *lp)
   {int rv;

    rv = _SC_leh_up_down(lp, 'A');

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_KILL_LINE - kill the entire line (Ctrl-U) */

static int _SC_leh_kill_line(lehloc *lp)
   {

    lp->bf[0] = '\0';
    lp->pos = 0;
    lp->len = 0;
    _SC_leh_refresh(lp);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_KILL_EOL - kill to the end of the line (Ctrl-K) */

static int _SC_leh_kill_eol(lehloc *lp)
   {

    lp->bf[lp->pos] = '\0';
    lp->len = lp->pos;
    _SC_leh_refresh(lp);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_GOTO_BOL - got the beginning of the line (Ctrl-A) */

static int _SC_leh_goto_bol(lehloc *lp)
   {

    lp->pos = 0;
    _SC_leh_refresh(lp);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_GOTO_EOL - got the end of the line (Ctrl-E) */

static int _SC_leh_goto_eol(lehloc *lp)
   {

    lp->pos = lp->len;
    _SC_leh_refresh(lp);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_CLEAR_SCREEN - clear the screen (Ctrl-L) */

static int _SC_leh_clear_screen(lehloc *lp)
   {

    SC_leh_clear(lp->fd);
    _SC_leh_refresh(lp);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_INTR - interrupt (Ctrl-C) */

static int _SC_leh_intr(lehloc *lp)
   {

    _SC_leh_disa_raw(STDIN_FILENO);

    kill(0, SIGINT);

    errno = EAGAIN;

    return(FALSE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_CR - handle carriage return (Ctrl-M) */

static int _SC_leh_cr(lehloc *lp)
   {int rv;

    _SC_leh.nh--;
    CFREE(_SC_leh.hist[_SC_leh.nh]);
    rv = lp->len;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_RESUME - resume foreground execution */

static void _SC_leh_resume(int sig)
   {

/* return to raw mode */
    _SC_leh_ena_raw(STDIN_FILENO);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_SUSPEND - suspend the process group */

static int _SC_leh_suspend(lehloc *lp)
   {

/* switch to cooked mode for terminal */
    _SC_leh_disa_raw(STDIN_FILENO);

/* put handler on SIGCONT */
    SC_signal_n(SIGCONT, _SC_leh_resume, NULL);

/* send ourselves a STOP */
    kill(getpid(), SIGSTOP);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_ESC - handle an escape sequence */

static int _SC_leh_esc(lehloc *lp)
   {int fd, rv;
    char esa[2], esb[2];

    rv = FALSE;

    fd = lp->fd;

    if (_SC_leh.read(fd, esa, 2) >= 0)
       {if (esa[0] == '[')
	   {switch (esa[1])

/* left arrow */
	       {case 'D' :
		     rv = _SC_leh_left(lp);
		     break;

/* right arrow */
                case 'C' :
		     rv = _SC_leh_right(lp);
		     break;

/* up arrow */
		case 'A' :
		     rv = _SC_leh_up_down(lp, 'A');
		     break;

/* down arrow */
		case 'B' :
		     rv = _SC_leh_up_down(lp, 'B');
		     break;

/* extended escape sequences */
	        default :
		     if (('0' < esa[1]) && (esa[1] < '7'))
		        {if (_SC_leh.read(fd, esb, 2) >= 0)

/* DEL */
			    {if ((esa[1] == '3') && (esb[0] == '~'))
			        rv = _SC_leh_del(lp);};};
		     break;};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_LEH_EMACS_MODE - setup emacs mode */

void SC_leh_emacs_mode(void)
   {int i;
    PFlehact *map;

    map = CMAKE_N(PFlehact, 256);
    if (map != NULL)
       {_SC_leh.map = map;

	for (i = 0; i < 256; i++)
	    map[i] = _SC_leh_any;

        map[CTRL_A] = _SC_leh_goto_bol;
        map[CTRL_B] = _SC_leh_left;
        map[CTRL_C] = _SC_leh_intr;
        map[CTRL_D] = _SC_leh_del_curr;
        map[CTRL_E] = _SC_leh_goto_eol;
        map[CTRL_F] = _SC_leh_right;
        map[CTRL_H] = _SC_leh_del_back;
        map[CTRL_K] = _SC_leh_kill_eol;
        map[CTRL_L] = _SC_leh_clear_screen;
        map[CTRL_M] = _SC_leh_cr;
        map[CTRL_N] = _SC_leh_next_line;
        map[CTRL_P] = _SC_leh_prev_line;
        map[CTRL_T] = _SC_leh_exch_char;
        map[CTRL_U] = _SC_leh_kill_line;
        map[CTRL_Z] = _SC_leh_suspend;
        map[BSP]    = _SC_leh_del_back;
        map[ESC]    = _SC_leh_esc;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_LEH_VI_MODE - setup vi mode */

void SC_leh_vi_mode(void)
   {int i;
    PFlehact *map;

    map = CMAKE_N(PFlehact, 256);
    if (map != NULL)
       {_SC_leh.map = map;

	for (i = 0; i < 256; i++)
	    map[i] = _SC_leh_any;
/*
 * ESC          - Switch to command mode
 * i            - Insert before cursor
 * a            - Insert after cursor
 * I            - Insert at the beginning of line
 * A            - Insert at the end of line
 * s            - Delete a single character under the cursor and enter input
 *              - mode (equivalent to c[SPACE])
 * h            - Move one character right
 * l            - Move one character left
 * 0            - Move to the beginning of line
 * $            - Move to the end of line
 * x            - Delete a single character under the cursor
 * X            - Delete a character before the cursor
 * D            - Delete to the end of the line (equivalent to d$)
 * dd           - Delete current line (equivalent to 0d$)
 * /string or   - Search history backward for a command matching string
 * CTRL-r       -
 * ?string or   - Search history forward for a command matching string
 * CTRL-s       - (Note that on most machines Ctrl-s STOPS the terminal
 *              - output, change it with `stty' (Ctrl-q to resume))
 * CTRL-r       - Transpose (exchange) two characters
 */
       };

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_GETS - print the PROMPT and fill BF in LP
 *              - return NULL on error
 */

static char *_SC_leh_gets(lehloc *lp)
   {int nr, nc, fd, ok;
    char bf[2];
    char *rv;
    PFlehact *map;

    if ((trace == TRUE) && (fdbg == NULL))
       fdbg = fopen("log.keys", "w");

    if (_SC_leh.map == NULL)
       SC_leh_emacs_mode();

    map = _SC_leh.map;

    fd = lp->fd;

/* most recent history entry is current buffer
 * initially just an empty string
 */
    SC_leh_hist_add("");
    
    rv = NULL;
    ok = TRUE;

    ok &= _SC_leh_put_prompt(lp);

    while (ok == TRUE)
       {nr = _SC_leh.read(fd, bf, 1);
        if (nr <= 0)
	   break;

	lp->c = bf[0];

        if (fdbg != NULL)
	   {fprintf(fdbg, "> '%c'  %d %d  (%ld %ld)  ",
		    lp->c, lp->hind, _SC_leh.nh,
		    (long) lp->pos, (long) lp->len);
	    fflush(fdbg);};

/* autocomplete when the callback is set
 * completion returns -1 on error
 * otherwise returns the next character to be handled
 */
        if ((lp->c == CTRL_I) && (_SC_leh.cmp_cb != NULL))
	   {lp->c = _SC_leh_complete(lp);

/* exit on error */
            if (lp->c < 0)
	       break;

/* read next when null character encountered */
            else if (lp->c == 0)
	       continue;};

	nc = map[lp->c](lp);

        if (fdbg != NULL)
	   {fprintf(fdbg, ": %d %p (%ld %d)\n",
		    nc, map[lp->c],
		    (long) lp->pos, _SC_leh.nh);
	    fflush(fdbg);};

	if (lp->c == CTRL_M)
	   {rv = lp->bf;
	    ok = FALSE;}
	else if (nc == FALSE)
	   {rv = NULL;
	    ok = FALSE;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_RAW - raw I/O from stdin
 *             - return NULL on failure
 */

static char *_SC_leh_raw(lehloc *lp)
   {int nc, fd;
    char *bf;
    size_t nb;
    char *rv;

    fd = lp->fd;
    bf = lp->bf;
    nb = lp->nb;

    nc = -1;
    rv = bf;

    if (nb == 0)
       {errno = EINVAL;
	rv    = NULL;}

    else if (isatty(fd) == FALSE)
       {rv = fgets(bf, nb, stdin);
	if (rv != NULL)
	   {nc = strlen(rv);
	    if ((nc > 0) && (rv[nc-1] == '\n'))
	       {nc--;
		rv[nc] = '\0';};};}

    else
       {if (_SC_leh_ena_raw(fd) == TRUE)
	   {rv = _SC_leh_gets(lp);
	    _SC_leh_disa_raw(fd);
	    printf("\n");};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_LEH - the main readline workalike routine
 *        - following linenoise:
 *        - do everything with just three sequences
 *        - there may have some flickering with some slow terminals
 *        - but fewer sequences makes for easier portability
 */

char *SC_leh(const char *prompt)
   {int nc;
    char bf[MAX_BFSZ+1];
    char *rv;
    lehloc loc;

    memset(&loc, 0, sizeof(loc));

    loc.fd     = STDIN_FILENO;
    loc.cols   = _SC_leh_get_ncol();
    loc.nb     = MAX_BFSZ;
    loc.bf     = bf;
    loc.bf[0]  = '\0';
    loc.prompt = (char *) prompt;

    rv = NULL;
    _SC_leh.fd = STDIN_FILENO;

    if (_SC_leh_sup_termp() == FALSE)
       {_SC_leh_put_prompt(&loc);

	fflush(stdout);
	rv = fgets(bf, MAX_BFSZ, stdin);
        if (rv != NULL)
	   {nc = strlen(rv);
	    while ((nc > 0) && ((rv[nc-1] == '\n') || (rv[nc-1] == '\r')))
	       {nc--;
		rv[nc] = '\0';};};}

    else
       rv = _SC_leh_raw(&loc);

    if (rv != NULL)
       rv = CSTRSAVE(rv);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_LEH_CMP_SET_CB - register a callback function F
 *                   - for tab-completion
 */

void SC_leh_cmp_set_cb(PFlehcb f)
   {

    _SC_leh.cmp_cb = f;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_LEH_CMP_ADD - add S to the list of completions LC */

void SC_leh_cmp_add(lehcmp *lc, char *s)
   {

    CREMAKE(lc->cvec, char *, lc->len+1);
    lc->cvec[lc->len++] = CSTRSAVE(s);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_LEH_HIST_ADD - add S to the history ring */

int SC_leh_hist_add(const char *s)
   {int rv, nb;
    char *cpy;

    rv = FALSE;

    if (_SC_leh.nhx != 0)
       {if (_SC_leh.hist == NULL)
	   {_SC_leh.hist = CMAKE_N(char *, _SC_leh.nhx);
	    if (_SC_leh.hist == NULL)
	       return(rv);
            nb = _SC_leh.nhx*sizeof(char *);
	    memset(_SC_leh.hist, 0, nb);};

	cpy = CSTRSAVE((char *) s);
	if (cpy != NULL)
	   {if (_SC_leh.nh == _SC_leh.nhx)
	       {CFREE(_SC_leh.hist[0]);
                nb = (_SC_leh.nhx - 1)*sizeof(char *);
		memmove(_SC_leh.hist, _SC_leh.hist+1, nb);
		_SC_leh.nh--;};

	     _SC_leh.hist[_SC_leh.nh] = cpy;
	     _SC_leh.nh++;

	     rv = TRUE;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_LEH_HIST_SET_N - set the size of the history ring to N */

int SC_leh_hist_set_n(int n)
   {int rv, nh, nb;
    char **sa;

    rv = FALSE;

    if ((n > 0) && (_SC_leh.hist != NULL))
       {nh = _SC_leh.nh;

        sa = CMAKE_N(char *, n);
        if (sa != NULL)
	   {if (n < nh)
	       nh = n;

	    nb = sizeof(char*)*nh;
	    memcpy(sa, _SC_leh.hist+(_SC_leh.nhx-nh), nb);
	    CFREE(_SC_leh.hist);

	    _SC_leh.hist = sa;

	    _SC_leh.nhx = n;
	    if (_SC_leh.nh > _SC_leh.nhx)
	       _SC_leh.nh = _SC_leh.nhx;

	    rv = TRUE;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_LEH_HIST_SAVE - save the history to FNAME
 *                  - return TRUE iff successful
 */

int SC_leh_hist_save(char *fname)
   {int rv, i;
    FILE *fp;

    rv = FALSE;

    fp = fopen(fname, "w");
    if (fp != NULL)
       {for (i = 0; i < _SC_leh.nh; i++)
	    fprintf(fp, "%s\n", _SC_leh.hist[i]);

	rv = TRUE;

	fclose(fp);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_LEH_HIST_LOAD - load the history from FNAME
 *                  - return TRUE iff successful
 */

int SC_leh_hist_load(char *fname)
   {int rv;
    char t[MAX_BFSZ];
    char *p;
    FILE *fp;

    rv = FALSE;

    fp = fopen(fname, "r");
    if (fp != NULL)
       {while (fgets(t, MAX_BFSZ, fp) != NULL)
	   {p = strchr(t, '\r');
	    if (p == NULL)
	       p = strchr(t, '\n');

	    if (p != NULL)
	       *p = '\0';

	    SC_leh_hist_add(t);};

	 rv = TRUE;

	 fclose(fp);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_LEH_SET_READ - set the read method for LEH
 *                 - return the old function
 */

PFread SC_leh_set_read(PFread f)
   {PFread ov;

    ov = _SC_leh.read;
    _SC_leh.read = f;

    return(ov);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

