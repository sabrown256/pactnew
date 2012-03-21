/*
 * SCLEH.C - line editing and history
 *         - following linenoise by Salvatore Sanfilippo and Pieter Noordhuis
 */

#include "cpyright.h"

#include "score.h"
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
    int nhx;
    int nh;
    char **hist;
    PFlehact *map;
    PFlehcb cmp_cb;};

struct s_lehloc
   {int fd;
    int hind;
    int np;               /* size of prompt text */
    size_t nb;            /* size of current line buffer */
    size_t pos;
    size_t len;
    size_t cols;
    int c;
    char *bf;             /* current line buffer */
    char *prompt;};       /* prompt text */

static lehdes
 _SC_leh = { -1, FALSE, 0, MAX_HIST_N, 0, NULL, NULL, NULL, };

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

/* _SC_LEH_REFRESH - redraw the current line */

static void _SC_leh_refresh(lehloc *lp)
   {int fd, err;
    size_t nw, len, np, pos, cols;
    char seq[64];
    char *bf, *prompt;

    fd     = lp->fd;
    bf     = lp->bf;
    len    = lp->len;
    pos    = lp->pos;
    cols   = lp->cols;
    np     = lp->np;
    prompt = lp->prompt;

    while (np+pos >= cols)
       {bf++;
        len--;
        pos--;};

    while (np+len > cols)
       len--;

    err = FALSE;

/* move cursor to left edge - 'ESC [ n G' moves cursor to column n */
    if (err == FALSE)
       {snprintf(seq, 64, "\x1b[0G");
	nw   = write(fd, seq, strlen(seq));
	err |= (nw == -1);};

/* write the prompt */
    if (err == FALSE)
       {nw   = write(fd, prompt, np);
	err |= (nw == -1);};

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

/* move cursor to original position - 'ESC [ n C' move cursor forward n chars */
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

	     nr = read(lp->fd, &c, 1);
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
   {int rv;
    size_t ie;

    rv = TRUE;

    if (_SC_leh.nh > 1)

/* update the current history entry before to
 * overwrite it with the next one
 */
       {ie = _SC_leh.nh - 1 - lp->hind;
	CFREE(_SC_leh.hist[ie]);
	_SC_leh.hist[ie] = CSTRSAVE(lp->bf);

/* display the new entry */
	lp->hind += (wh == 'A') ? 1 : -1;
	if (lp->hind < 0)
	   {lp->hind = 0;
	    rv       = FALSE;}

	else if (lp->hind >= _SC_leh.nh)
	   {lp->hind = _SC_leh.nh-1;
	    rv       = FALSE;}

	else
	   {ie = _SC_leh.nh - 1 - lp->hind;
	    SC_strncpy(lp->bf, lp->nb, _SC_leh.hist[ie], -1);

	    lp->pos = strlen(lp->bf);
	    lp->len = lp->pos;
	    _SC_leh_refresh(lp);};};

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

/* _SC_LEH_ESC - handle an escape sequence */

static int _SC_leh_esc(lehloc *lp)
   {int fd, rv;
    char esa[2], esb[2];

    rv = FALSE;

    fd = lp->fd;

    if (read(fd, esa, 2) >= 0)
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
		        {if (read(fd, esb, 2) >= 0)

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

/* _SC_LEH_PROMPT - print the PROMPT and fill BF */

static int _SC_leh_prompt(int fd, char *bf, size_t nb, const char *prompt)
   {int nr, rv;
    lehloc loc;
    PFlehact *map;

    if (_SC_leh.map == NULL)
       SC_leh_emacs_mode();

    map = _SC_leh.map;

    loc.fd     = fd;
    loc.hind   = 0;
    loc.pos    = 0;
    loc.len    = 0;
    loc.c      = 0;
    loc.cols   = _SC_leh_get_ncol();
    loc.nb     = --nb;   /* take off one for the terminating null character */
    loc.prompt = (char *) prompt;
    loc.np     = (prompt == NULL) ? 0 : strlen(prompt);
    loc.bf     = bf;
    loc.bf[0]  = '\0';

/* most recent history entry is current buffer
 * initially just an empty string
 */
    SC_leh_hist_add("");
    
    if (prompt != NULL)
       {if (write(fd, prompt, loc.np) == -1)
	   return(-1);};

    while (TRUE)
       {nr = read(fd, &loc.c, 1);
        if (nr <= 0)
	   break;

/* autocomplete when the callback is set
 * completion returns -1 on error
 * otherwise returns the next character to be handled
 */
        if ((loc.c == CTRL_I) && (_SC_leh.cmp_cb != NULL))
	   {loc.c = _SC_leh_complete(&loc);

/* exit on error */
            if (loc.c < 0)
	       break;

/* read next when null character encountered */
            else if (loc.c == 0)
	       continue;};

	rv = map[loc.c](&loc);
	if (loc.c == CTRL_M)
	   return(rv);
	else if (rv == FALSE)
	   return(-1);};

    return(loc.len);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_RAW - raw I/O from stdin */

static int _SC_leh_raw(int fd, char *bf, size_t nb, const char *prompt)
   {int rv;

    rv = -1;

    if (nb == 0)
       errno = EINVAL;

    else if (isatty(fd) == FALSE)
       {if (fgets(bf, nb, stdin) != NULL)
	   {rv = strlen(bf);
	    if ((rv > 0) && (bf[rv-1] == '\n'))
	       {rv--;
		bf[rv] = '\0';};};}

    else
       {if (_SC_leh_ena_raw(fd) == TRUE)
	   {rv = _SC_leh_prompt(fd, bf, nb, prompt);
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
   {int nr, nc;
    char bf[MAX_BFSZ];
    char *rv;
    
    _SC_leh.fd = STDIN_FILENO;

    if (_SC_leh_sup_termp() == FALSE)
       {printf("%s", prompt);
        fflush(stdout);
        if (fgets(bf, MAX_BFSZ, stdin) == NULL)
	   return(NULL);

        nc = strlen(bf);
        while ((nc > 0) && ((bf[nc-1] == '\n') || (bf[nc-1] == '\r')))
	   {nc--;
            bf[nc] = '\0';};}

    else
       {nr = _SC_leh_raw(_SC_leh.fd, bf, MAX_BFSZ, prompt);
        if (nr == -1)
	   return(NULL);};

    rv = CSTRSAVE(bf);

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
