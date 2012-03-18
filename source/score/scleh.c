/*
 * SCLEH.C - line editing and history
 *         - linenoise work-alike (started from linenoise.c)
 *         - Copyright (c) 2010, Salvatore Sanfilippo <antirez at gmail dot com>
 *         - Copyright (c) 2010, Pieter Noordhuis <pcnoordhuis at gmail dot com>
 * 
 */

#include "cpyright.h"

#include "score.h"
#include "scope_term.h"

#define MAX_HIST_N   100

typedef struct s_lehcmp lehcmp;
typedef struct s_lehdes lehdes;
typedef struct s_lehloc lehloc;

typedef void(PFlehcb)(const char *s, lehcmp *lc);

struct s_lehcmp
   {size_t len;
    char **cvec;};

struct s_lehdes
   {int raw;
    int exr;
    int nhx;
    int nh;
    char **hist;
    PFlehcb *cmp_cb;
    struct termios trm;};

struct s_lehloc
   {int fd;
    int hind;
    size_t pos;
    size_t len;
    size_t cols;
    int c;
    char esa[2];
    char esb[2];
    char *bf;             /* current line buffer */
    size_t nb;            /* size of current line buffer */
    char *prompt;         /* prompt text */
    int np;};             /* size of prompt text */

static lehdes
 _SC_leh = { FALSE, 0, MAX_HIST_N, 0, NULL, NULL, };

void
 SC_leh_cmp_set_cb_cmp_cb(PFlehcb *f), 
 SC_leh_cmp_add(lehcmp *lc, char *s);

static void
 _SC_leh_done(void);

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

/* _SC_LEH_ENA_RAW - put FD into raw mode */

static int _SC_leh_ena_raw(int fd)
   {int rv;
    struct termios raw;

    rv = 0;

    if (isatty(STDIN_FILENO) == FALSE)
       goto fatal;

    if (_SC_leh.exr == FALSE)
       {atexit(_SC_leh_done);
        _SC_leh.exr = TRUE;};

    if (tcgetattr(fd, &_SC_leh.trm) == -1)
       goto fatal;

/* modify the original mode */
    raw = _SC_leh.trm;

/* input modes: no break, no CR to NL, no parity check, no strip char, 
 * no start/stop output control
 */
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

/* output modes - disable post processing */
    raw.c_oflag &= ~(OPOST);

/* control modes - set 8 bit chars */
    raw.c_cflag |= (CS8);

/* local modes - choing off, canonical off, no extended functions, 
 * no signal chars (^Z, ^C)
 */
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

/* control chars - set return condition: min number of bytes and timer
 * We want read to return every single byte, without timeout
 */
    raw.c_cc[VMIN]  = 1;
    raw.c_cc[VTIME] = 0;              /* 1 byte, no timer */

/* put terminal in raw mode after flushing */
    if (tcsetattr(fd, TCSAFLUSH, &raw) < 0)
       goto fatal;

    _SC_leh.raw = TRUE;

    return(rv);

fatal:
    errno = ENOTTY;
    return(-1);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_DISA_RAW - put FD into cooked mode */

static void _SC_leh_disa_raw(int fd)
   {

    if ((_SC_leh.raw == TRUE) &&
	(tcsetattr(fd, TCSAFLUSH, &_SC_leh.trm) != -1))
       _SC_leh.raw = FALSE;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_DONE - at exit try to restore the terminal
 *              - to its initial conditions
 */

static void _SC_leh_done(void)
   {

    _SC_leh_disa_raw(STDIN_FILENO);
    _SC_leh_free_hist();

    return;}

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

    while ((np+pos) >= cols)
       {bf++;
        len--;
        pos--;};

    while (np+len > cols)
       len--;

    err = FALSE;

/* move cursor to left edge */
    if (err == FALSE)
       {snprintf(seq, 64, "\x1b[0G");
	nw   = write(fd, seq, strlen(seq));
	err |= (nw == -1);};

/* write the prompt and the current buffer content */
    if (err == FALSE)
       {nw   = write(fd, prompt, np);
	err |= (nw == -1);};

    if (err == FALSE)
       {nw   = write(fd, bf, len);
	err |= (nw == -1);};

/* erase to right */
    if (err == FALSE)
       {snprintf(seq, 64, "\x1b[0K");
	nw   = write(fd, seq, strlen(seq));
	err |= (nw == -1);};

/* move cursor to original position */
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
    char c;
    lehcmp lc = { 0, NULL };

    c = 0;

    _SC_leh.cmp_cb(lp->bf, &lc);
    if (lc.len == 0)
       _SC_leh_beep();

    else
       {size_t i;

	ok = FALSE;
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
	        {case 9 :
                      i = (i+1) % (lc.len+1);
                      if (i == lc.len)
		         _SC_leh_beep();
		      break;

/* escape - re-display original buffer */
                 case 27 :
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

/* SC_LEH_CLEAR - clear the screen */

void SC_leh_clear(void)
   {int nr;

    nr = write(STDIN_FILENO, "\x1b[H\x1b[2J", 7);

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

static int _SC_leh_up_down(lehloc *lp)
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
	lp->hind += (lp->esa[1] == 65) ? 1 : -1;
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

/* _SC_LEH_DEL_BACK - delete the previous character (Ctrl-H) */

int _SC_leh_del_back(lehloc *lp)
   {size_t pos, len;
    char *bf;

    pos = lp->pos;
    len = lp->len;
    if ((pos > 0) && (len > 0))
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

int _SC_leh_del_curr(lehloc *lp)
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

int _SC_leh_exch_char(lehloc *lp)
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

/* _SC_LEH_KILL_LINE - kill the entire line (Ctrl-U) */

int _SC_leh_kill_line(lehloc *lp)
   {

    lp->bf[0] = '\0';
    lp->pos = 0;
    lp->len = 0;
    _SC_leh_refresh(lp);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_KILL_EOL - kill to the end of the line (Ctrl-K) */

int _SC_leh_kill_eol(lehloc *lp)
   {

    lp->bf[lp->pos] = '\0';
    lp->len = lp->pos;
    _SC_leh_refresh(lp);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_GOTO_BOL - got the beginning of the line (Ctrl-A) */

int _SC_leh_goto_bol(lehloc *lp)
   {

    lp->pos = 0;
    _SC_leh_refresh(lp);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_GOTO_EOL - got the end of the line (Ctrl-E) */

int _SC_leh_goto_eol(lehloc *lp)
   {

    lp->pos = lp->len;
    _SC_leh_refresh(lp);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_CLEAR_SCREEN - clear the screen (Ctrl-L) */

int _SC_leh_clear_screen(lehloc *lp)
   {

    SC_leh_clear();
    _SC_leh_refresh(lp);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_PROMPT - print the PROMPT and fill BF */

static int _SC_leh_prompt(int fd, char *bf, size_t nb, const char *prompt)
   {int nr, rv;
    lehloc loc;

    size_t len;
    len  = 0;

    loc.fd     = fd;
    loc.hind   = 0;
    loc.pos    = 0;
    loc.len    = 0;
    loc.cols   = _SC_leh_get_ncol();
    loc.bf     = bf;
    loc.nb     = --nb;   /* take off one for the terminating null character */
    loc.prompt = (char *) prompt;
    loc.np     = (prompt == NULL) ? 0 : strlen(prompt);

    loc.bf[0] = '\0';

/* the latest history entry is always our current buffer, that
 * initially is just an empty string
 */
    SC_leh_hist_add("");
    
    if (prompt != NULL)
       {if (write(fd, prompt, loc.np) == -1)
	   return(-1);};

    while (TRUE)
       {nr = read(fd, &loc.c, 1);
        if (nr <= 0)
	   return(loc.len);

/* only autocomplete when the callback is set. It returns < 0 when
 * there was an error reading from fd. Otherwise it will return the
 * character that should be handled next
 */
        if ((loc.c == 9) && (_SC_leh.cmp_cb != NULL))
	   {loc.c = _SC_leh_complete(&loc);

/* return on errors */
            if (loc.c < 0)
	       return(loc.len);

/* read next character when 0 */
            if (loc.c == 0)
	       continue;};

        switch (loc.c)

/* ctrl-m */
	   {case 13 :
	         _SC_leh.nh--;
		 CFREE(_SC_leh.hist[_SC_leh.nh]);
		 return(loc.len);

/* ctrl-c */
	    case 3 :
	         errno = EAGAIN;
		 return(-1);

/* backspace and ctrl-h */
	   case 127 :
	   case 8 :
	        rv = _SC_leh_del_back(&loc);
		break;

/* ctrl-d, remove char at right of cursor */
	   case 4 :
	        rv = _SC_leh_del_curr(&loc);
		if (rv == FALSE)
		   return(-1);
		break;

/* ctrl-t */
	   case 20 :
	        rv = _SC_leh_exch_char(&loc);
		break;

/* ctrl-b */
	   case 2 :
	        _SC_leh_left(&loc);
		break;

/* ctrl-f */
	   case 6 :
	        _SC_leh_right(&loc);
                break;

/* ctrl-p */
	   case 16 :
	        loc.esa[1] = 65;
		goto up_down_arrow;

/* ctrl-n */
	   case 14 :
	        loc.esa[1] = 66;
		goto up_down_arrow;
		break;

/* escape sequence */
	   case 27 :
	        if (read(fd, loc.esa, 2) == -1)
		   break;

/* left arrow */
		if ((loc.esa[0] == 91) && (loc.esa[1] == 68))
		   _SC_leh_left(&loc);

		else if ((loc.esa[0] == 91) && (loc.esa[1] == 67))
		   _SC_leh_right(&loc);

		else if ((loc.esa[0] == 91) &&
			 ((loc.esa[1] == 65) || (loc.esa[1] == 66)))

/* up and down arrow: history */
up_down_arrow:
		   {
#if 0
                    rv = _SC_leh_up_down(&loc);
#else
                    rv = TRUE;
		    if (_SC_leh.nh > 1)
		       {size_t ie;

/* update the current history entry before to
 * overwrite it with tne next one
 */
			ie = _SC_leh.nh-1-loc.hind;
			CFREE(_SC_leh.hist[ie]);
			_SC_leh.hist[ie] = CSTRSAVE(loc.bf);

/* display the new entry */
			loc.hind += (loc.esa[1] == 65) ? 1 : -1;
			if (loc.hind < 0)
			   {loc.hind = 0;
			    rv = FALSE;}
		        else if (loc.hind >= _SC_leh.nh)
			   {loc.hind = _SC_leh.nh-1;
			    rv = FALSE;}

			else
			   {ie = _SC_leh.nh-1-loc.hind;
			    SC_strncpy(loc.bf, loc.nb, _SC_leh.hist[ie], -1);

			    loc.len = strlen(loc.bf);
			    loc.pos = loc.len;
			    _SC_leh_refresh(&loc);};};
#endif

		    if (rv == FALSE)
		       break;}

		else if ((loc.esa[0] == 91) && (loc.esa[1] > 48) &&
			 (loc.esa[1] < 55))
		   {

/* extended escape */
		    if (read(fd, loc.esb, 2) == -1)
		       break;

/* delete */
		    if ((loc.esa[1] == 51) && (loc.esb[0] == 126))
		       {if ((loc.len > 0) && (loc.pos < loc.len))
			   {memmove(bf+loc.pos, bf+loc.pos+1, loc.len-loc.pos-1);
			    loc.len--;
			    bf[loc.len] = '\0';
			    _SC_leh_refresh(&loc);};};};
		break;

/* ctrl-u, delete the whole line */
	   case 21 :
	        rv = _SC_leh_kill_line(&loc);
		break;

/* ctrl-k, delete from current to end of line */
	   case 11 :
	        rv = _SC_leh_kill_eol(&loc);
		break;

/* ctrl-a, go to the start of the line */
	   case 1 :
	        rv = _SC_leh_goto_bol(&loc);
		break;

/* ctrl-e, go to the end of the line */
	   case 5 :
	        rv = _SC_leh_goto_eol(&loc);
		break;

/* ctrl-l, clear screen */
	   case 12 :
	        rv = _SC_leh_clear_screen(&loc);
		break;

	   default :
	        if (loc.len < loc.nb)
		   {if (loc.len == loc.pos)
		       {bf[loc.pos] = loc.c;
			loc.pos++;
                        loc.len++;
			bf[loc.len] = '\0';
			if (loc.np+loc.len < loc.cols)

/* avoid a full update of the line in the trivial case */
			   {if (write(fd, &loc.c, 1) == -1)
			       return(-1);}
		        else
			   _SC_leh_refresh(&loc);}

		    else
		        {memmove(bf+loc.pos+1, bf+loc.pos, loc.len-loc.pos);
			 bf[loc.pos] = loc.c;
			 loc.len++;
			 loc.pos++;
			 bf[loc.len] = '\0';
			 _SC_leh_refresh(&loc);};};
		break;};};

    return(loc.len);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEH_RAW - raw I/O from stdin */

static int _SC_leh_raw(char *bf, size_t nb, const char *prompt)
   {int fd, rv;

    rv = -1;

    fd = STDIN_FILENO;

    if (nb == 0)
       errno = EINVAL;

    else if (isatty(STDIN_FILENO) == FALSE)
       {if (fgets(bf, nb, stdin) != NULL)
	   {rv = strlen(bf);
	    if ((rv > 0) && (bf[rv-1] == '\n'))
	       {rv--;
		bf[rv] = '\0';};};}

    else
       {if (_SC_leh_ena_raw(fd) != -1)
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
 *        -
 *        - Move cursor horizontal absolute
 *        -    ESC [ n G    moves cursor to column n
 *        -
 *        - Erase line
 *        -    ESC [ n K    if n is 0 or missing, clear from cursor to EOL
 *        -                 if n is 1, clear from BOL line to cursor
 *        -                 if n is 2, clear entire line
 *        -
 *        - Move cursor forward
 *        -    ESC [ n C    moves cursor forward of n chars
 *        -
 *        - Clear Screen
 *        -    ESC [ H ESC [ 2 J    (this is really two sequences)
 *        -    ESC [ H      moves the cursor to upper left corner
 *        -    ESC [ 2 J    clear the whole screen
 */

char *SC_leh(const char *prompt)
   {char *rv;

    int count, nc;
    char bf[MAX_BFSZ];
    
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
       {count = _SC_leh_raw(bf, MAX_BFSZ, prompt);

        if (count == -1)
	   return(NULL);};

    rv = CSTRSAVE(bf);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_LEH_CMP_SET_CB - register a callback function F
 *                   - for tab-completion
 */

void SC_leh_cmp_set_cb(PFlehcb *f)
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

