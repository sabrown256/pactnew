/*
 * SCFIO.C - I/O hooks suitable for mapped files and remote file access
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scope_proc.h"

#define MAX_FILES 100

#define OPEN_MSG     "SC_FOPEN"
#define SETVBUF_MSG  "SC_FSETVBUF"
#define CLOSE_MSG    "SC_FCLOSE"
#define FLUSH_MSG    "SC_FFLUSH"
#define TELL_MSG     "SC_FTELL"
#define SEEK_MSG     "SC_FSEEK"
#define READ_MSG     "SC_FREAD"
#define WRITE_MSG    "SC_FWRITE"
#define PUTS_MSG     "SC_PUTS"
#define GETS_MSG     "SC_FGETS"
#define GETC_MSG     "SC_FGETC"
#define UNGETC_MSG   "SC_FUNGETC"
#define EXIT_MSG     "SC_FEXIT"

#define REPLY(msg, val)                                                         \
   {printf("%s:%ld\n", msg, (long) (val));                                      \
    fflush(stdout);                                                             \
    if (_SC_debug)                                                              \
       {fprintf(_SC_diag, "%s:%ld\n", msg, (long) (val));                       \
        fflush(_SC_diag);};}

#define IO_OPER_START_TIME(_f)                                                  \
   {double _to;                                                                 \
    if (fid->gather == TRUE)                                                    \
       _to = SC_wall_clock_time()

#define IO_OPER_ACCUM_TIME(_f, _o)                                              \
    if (fid->gather == TRUE)                                                    \
       (_f)->nsec[_o] += (SC_wall_clock_time() - _to);                          \
    (_f)->nhits[_o]++;}

typedef struct s_REMOTE_FILE REMOTE_FILE;

struct s_REMOTE_FILE
   {PROCESS *pp;
    int type;
    int fid;
    int64_t file_size;
    char *buffer;
    long sb_addr;
    long size;
    int64_t cf_addr;};

PFfprintf
 _SC_putln = io_printf;

PFfgets
 _SC_getln = io_gets;

PFfputs
 _SC_putstr = io_puts;


/* declare the IO hooks */

#ifdef SC_C_STD_BUFFERED_IO

PFfopen
 io_open_hook = SC_fopen,
 lio_open_hook = SC_lfopen;

#else

PFfopen
 io_open_hook = SC_bopen,
 lio_open_hook = SC_lbopen;

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MAKE_IO_DESC - allocate, initialize, and return a file_io_desc */

file_io_desc *SC_make_io_desc(void *fp)
   {file_io_desc *fid;

    fid = CMAKE(file_io_desc);

    SC_MEM_INIT(file_io_desc, fid);

    fid->info = fp;

    if (SC_collect_io_info(-1) == TRUE)
       fid->gather = TRUE;

    return(fid);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_IO_INFO - return the nhits and timing info for FP */

void SC_get_io_info(FILE *fp, int **pnhits, double **pnsec)
   {int *nh;
    double *ns;
    file_io_desc *fid;

    nh = NULL;
    ns = NULL;
    if (fp != NULL)
       {if (!IS_STD_IO(fp))
	   {fid = (file_io_desc *) fp;
	    nh  = fid->nhits;
	    ns  = fid->nsec;};};

    if (pnhits != NULL)
       *pnhits = nh;

    if (pnsec != NULL)
       *pnsec = ns;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_COLLECT_IO_INFO - depending on WH collect I/O operation stats
 *                    - on all files
 *                    - return the old value
 *                    - WH  -1 return old value
 *                    -      0 turn off stats gathering
 *                    -      1 turn on stats gathering
 */

int SC_collect_io_info(int wh)
   {int rv;
    static int gather = FALSE;

    rv = gather;
    if (wh >= 0)
       gather = wh;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GATHER_IO_INFO - depending on WH gather I/O operation stats on FP
 *                   - return the old value
 *                   - WH  -1 return old value
 *                   -      0 turn off stats gathering
 *                   -      1 turn on stats gathering
 */

int SC_gather_io_info(FILE *fp, int wh)
   {int rv;
    file_io_desc *fid;

    rv = FALSE;
    if (fp != NULL)
       {if (!IS_STD_IO(fp))
	   {fid = (file_io_desc *) fp;
	    rv  = fid->gather;
	    if (wh >= 0)
	       fid->gather = wh;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SET_BUFFER_SIZE - set the default I/O buffer size to SZ
 *                    - return the old value
 *                    - -1 turns off default buffering optimization
 *                    - which happens on file open
 *                    - see _SC_bio_read_opt
 */

int64_t SC_set_buffer_size(int64_t sz)
   {int64_t rv;
    
    rv = _SC.buffer_size;
    
    _SC.buffer_size = sz;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_BUFFER_SIZE - get the default I/O buffer size */

int64_t SC_get_buffer_size(void)
   {int64_t rv;
    
    rv = _SC.buffer_size;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_PROMPT - fprintf PROMPT on stdout and fgets S on stdin */ 

char *SC_prompt(char *prompt, char *s, int n)
   {int nc, ok;
    char *rv;

    ok = FALSE;
    rv = NULL;

/* reserve the last character for a '\0' terminator */
    nc = n - 1;
    if (nc > 0)
       {

#ifdef HAVE_READLINE

	if (SC_isblocked_file(stdin) == TRUE)
	   {char *t;

/* if we have no prompt assume that we are working with SC_exec_job
 * and the last line received from the child is the prompt
 */
	    if (prompt == NULL)
	       {prompt = _SC.elbf;
		rl_already_prompted = TRUE;}
	    else
	       rl_already_prompted = FALSE;

	    rl_save_prompt();
	    t = readline(prompt);
	    if (t != NULL)
	       {snprintf(s, nc, "%s\n", t);
		if (strlen(t) > 0)
		   add_history(t);
		rv = s;};

	    rl_restore_prompt();
	    rl_clear_message();
 	    
	    ok = TRUE;};

#endif

	if (ok == FALSE)
	   {if (prompt != NULL)
	       printf("%s", prompt);
	    rv = fgets(s, nc, stdin);};

/* always set last character to '\0' to avoid a buffer overrun */
	s[nc] = '\0';};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_RL_FGETS - use SC_prompt for fgets on stdin */ 

char *_SC_rl_fgets(char *s, int n)
   {char *rv;

#if 0

    rv = SC_prompt(NULL, s, n);

#else

    int nc;

    rv = NULL;

/* reserve the last character for a '\0' terminator */
    nc = n - 1;
    if (nc > 0)
       {rv = fgets(s, nc, stdin);
	s[n] = '\0';};

#endif

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                              LOCAL FILE I/O                              */

/*--------------------------------------------------------------------------*/

/* _SC_FFLUSH - wrapper for fflush for handling null file pointers */

static int _SC_fflush(FILE *fp)
   {int ret;

    ret = FALSE;

    if (fp != NULL)
       ret = fflush(fp);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FPUTS - wrapper for fputs for ensuring that everything is sent
 *           - this is for persistence and fault tolerance
 *           - NOTE: under high load fputs has been known to fail, on fast
 *           - processors such as the Opteron, i.e. not sending everything
 *           - return the number of characters sent which is consistent
 *           - with fputs return value
 */

static int _SC_fputs(const char *s, FILE *fp)
   {int i, nb, nc, ns, nz;
    char *p;

    nb = 0;

    if ((fp != NULL) && (s != NULL))
       {p  = (char *) s;
	nz = 0;
	for (i = 0, nc = strlen(s); nc > 0; nc -= ns, i++)
	    {ns = SC_fwrite_sigsafe(p, 1, nc, fp);

/* count the number of consecutive zero writes */
	     if (ns <= 0)
	        nz++;
	     else
	        nz = 0;

/* if we are not getting anywhere bail out and
 * return minus the number of chars written to signify an error
 * this is to be consistent with fputs error return
 */
	     if (nz > 3)
	        {nb = -nb;
		 nb = min(nb, -1);
		 break;};

/* wait a tenth of a second if the whole thing did not get sent
 * presumably because the system is clogged
 */
	     if (ns < nc)
	        SC_sleep(100);

	     nb += ns;
	     p  += ns;};};

    return(nb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FPRINTF - worker supporting io_printf */

static int _SC_fprintf(FILE *fp, char *fmt, va_list a)
   {int ret;
    char *msg;

    ret = 0;

    if (fp != NULL)
       {msg = SC_vdsnprintf(TRUE, fmt, a);
	ret = _SC_fputs(msg, fp);
	CFREE(msg);};

    FLUSH_ON(fp, NULL);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SETVBUF - a dummy setvbuf for the marginal systems */

static int _SC_setvbuf(FILE *stream, char *buf, int type, size_t size)
   {int ret;

#ifdef IBMMP

    ret = 0;
    SC_ibm_non_ANSI_compliant = TRUE;

#else

    ret = setvbuf(stream, buf, type, size);

#endif

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SETBUF - controlled version of C setbuf for marginal systems */

void SC_setbuf(FILE *fp, char *bf)
   {

#ifdef IBMMP

    SC_ibm_non_ANSI_compliant = TRUE;

#else
    if (bf == NULL)
       io_setvbuf(fp, NULL, _IONBF, 0);
    else
       io_setvbuf(fp, bf, _IOFBF, SC_arrlen(bf));
#endif

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FREAD - atomic worker ala _SC_fwrite */

static size_t _SC_fread(void *s, size_t bpi, size_t ni, FILE *fp)
   {size_t nr;

    nr = SC_fread_sigsafe(s, bpi, ni, fp);

    return(nr);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FWRITE_ATM - atomic worker for _SC_fwrite */

static size_t _SC_fwrite_atm(void *s, size_t bpi, size_t ni, FILE *fp)
   {size_t nw;

    nw = SC_fwrite_sigsafe(s, bpi, ni, fp);

    return(nw);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FWRITE - controlled version of C fwrite for marginal systems
 *            - marginal manifests when CPU write rate outstrips
 *            - NFS or other network bandwitdth
 */

size_t _SC_fwrite(void *s, size_t bpi, size_t ni, FILE *fp)
   {int ok, flags, rw, fd;
    int64_t i, nb;
    size_t nw, nr;
    int64_t addr;
    char msg[MAXLINE];
    char *ps, *t;

/* turn off SIGIO handler */
    SC_catch_io_interrupts(FALSE);

    if (SC_gs.verify_writes == TRUE)

/* eliminate stdin, stdout, and stderr */
       {fd = fileno(fp);
	rw = (fd > 2);

/* eliminate read-only and write-only streams */
	flags = SC_fd_flags(fd);
	rw   &= ((flags & O_RDWR) != 0);

/* remember where we are and write it out */
	addr = ftell(fp);
	if (addr < 0)
	   SC_strerror(errno, msg, MAXLINE);

	nw = _SC_fwrite_atm(s, bpi, ni, fp);

/* go back and read it in
 * fseek will fail unless the file mode is read-write
 */
	if ((addr >= 0) && (nw > 0) && (rw == TRUE))
	   {nb = bpi*ni;
	    t  = CMAKE_N(char, nb);

	    fseek(fp, addr, SEEK_SET);
	    nr = _SC_fread(t, bpi, ni, fp);

/* compare */
	    ok = TRUE;
	    if (nr != nw)
	       ok = FALSE;
	    else
	       {ps = (char *) s;
		for (i = 0; (ok == TRUE) && (i < nb); i++)
		    ok = (ps[i] == t[i]);};

/* if they disagree move back to the start and return 0 */
	    if (ok == FALSE)
	       {nw = 0;
		fseek(fp, addr, SEEK_SET);};

	    CFREE(t);};}

    else
       nw = _SC_fwrite_atm(s, bpi, ni, fp);

/* turn on SIGIO handler */
    SC_catch_io_interrupts(SC_gs.io_interrupts_on);

    return(nw);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FGETS - do an fgets which terminates on \r or \n */

char *SC_fgets(char *s, int n, FILE *fp)
   {int i, nbr, nb, nc;
    int64_t pos;
    char *r;
    static int count = 0;

    r = NULL;

/* reserve the last character for a '\0' terminator */
    nc = n - 1;
    if (nc > 0)
       {if (fp == stdin)
	   r = _SC_rl_fgets(s, nc);

        else
	   {pos = ftell(fp);
	    nbr = fread(s, 1, nc, fp);
	    if (nbr < nc)
	       s[nbr] = '\0';

/* check for newlines */
	    if (nbr > 0)
	       {for (i = 0; (i < nc) && (s[i] != '\n') && (s[i] != '\r'); i++);
		nb = i + 1;
		if (nb < nc)
		   s[nb] = '\0';

		nb = min(nb, nbr);
		fseek(fp, pos + nb, SEEK_SET);};
       
	    r = (nbr > 0) ? s : NULL;};

/* always set last character to '\0' to avoid a buffer overrun */
	s[nc] = '\0';};

    count++;

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FOPEN - wrapper for fopen for handling null file names */

FILE *SC_fopen(char *name, char *mode)
   {FILE *ret, *fp;
    file_io_desc *fid;

    ret = NULL;

    if (name != NULL)
       {fp = fopen(name, mode);
	if (fp != NULL)
	   {fid          = SC_make_io_desc(fp);
	    fid->info    = fp;
	    fid->fopen   = SC_fopen;
	    fid->ftell   = ftell;
	    fid->fseek   = fseek;
	    fid->fread   = fread;
	    fid->fwrite  = _SC_fwrite;
	    fid->setvbuf = _SC_setvbuf;
	    fid->fclose  = fclose;
	    fid->fprintf = _SC_fprintf;
	    fid->fputs   = _SC_fputs;
	    fid->fgetc   = fgetc;
	    fid->ungetc  = ungetc;
	    fid->fflush  = _SC_fflush;
	    fid->feof    = feof;
	    fid->fgets   = SC_fgets;

	    ret = (FILE *) fid;}
	else
	   ret = NULL;};

    return(ret);}

/*--------------------------------------------------------------------------*/

/*                             REMOTE FILE I/O                              */

/*--------------------------------------------------------------------------*/

/* _SC_TRANSFER_BYTES - transfer bytes from the client to the file
 *                    - CFD is the client data socket
 *                    - NBE is the expected number of bytes from the client
 *                    - FP is the output file
 *                    - return the number of bytes successfully written
 */

static long _SC_transfer_bytes(int cfd, long nbe, FILE *fp)
   {char *bf, *ps;
    long nbw, nbt, nbr;

    bf = CMAKE_N(char, nbe);

/* read the data looping until all expected bytes come in */
    nbt = nbe;
    for (ps = bf; TRUE; ps += nbr)
        {nbr  = SC_read_sigsafe(cfd, ps, nbt);
	 nbt -= nbr;
	 if (nbt <= 0)
	    break;};

    nbw = _SC_fwrite(bf, 1, nbe, fp);

    CFREE(bf);

    return(nbw);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_GET_CMD_RESP - wait for the response from the server */

static int64_t _SC_get_cmd_resp(PROCESS *pp, char *msg)
   {int lm;
    int64_t rv;
    char s[MAXLINE];

    lm = strlen(msg);

    rv = FALSE;

    while (SC_gets(s, MAXLINE, pp) != NULL)
       {if (strncmp(s, msg, lm) == 0)
           {rv = SC_stol(s+lm+1);
	    break;};};

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                           SERVER SIDE ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* SC_FILE_ACCESS - carry out file access commands on the named host
 *                - this is the server end
 *                - the command codes are:
 *                -   SC_FOPEN     'o'
 *                -   SC_FCLOSE    'c'
 *                -   SC_FFLUSH    'f'
 *                -   SC_FTELL     't'
 *                -   SC_FSEEK     's'
 *                -   SC_FREAD     'r'
 *                -   SC_FWRITE    'w'
 *                -   SC_FPUTS     'p'
 *                -   SC_FGETS     'g'
 *                -   SC_FGETC     'h'
 *                -   SC_FUNGETC   'u'
 *                -   SC_FEXIT     'x'
 */

int SC_file_access(int log)
   {int code, cfd, indx, ret;
    char s[MAXLINE], *bf, *t;
    FILE *file[MAX_FILES], *fp;

    SC_MEM_INIT_N(FILE *, file, MAX_FILES);

    _SC_debug = log;

    if (_SC_debug)
       {_SC_diag = fopen("SC_fs.log", "w");
	if (_SC_diag == NULL)
	   return(-1);};

    cfd = -1;
    while (TRUE)
       {if (_SC_debug)
           {fprintf(_SC_diag, "\nWaiting for command ... ");
            fflush(_SC_diag);};

        if (SC_fgets(s, MAXLINE, stdin) == NULL)
           continue;

        if (_SC_debug)
           {fprintf(_SC_diag, "received: %s", s);
            fflush(_SC_diag);};

        code = s[0];
        indx = s[1];
        if (indx < 'A')
           continue;

        indx -= 'A';
	indx  = max(indx, 0);
        fp    = file[indx];

        if (_SC_debug)
           {fprintf(_SC_diag, "Doing: %c on file %d(%p)\n", code, indx, fp);
            fflush(_SC_diag);};

        switch (code)
           {case SC_FOPEN :
                 {int i, status, ok;
                  char *name, *mode;
                  int64_t len;

                  ok = TRUE;
		  ONCE_SAFE(TRUE, NULL)
                     {int port;

                      port = SC_init_server(SC_GET_PORT, TRUE);
                      if (port != -1)
	 	         {REPLY("USE_PORT", port);

			  cfd = SC_init_server(SC_GET_CONNECTION, TRUE);}
		      else
		         ok = FALSE;}
		  END_SAFE;

		  if (ok == FALSE)
		     return(-1);

                  name = SC_strtok(s+2, ",\n", t);
                  mode = SC_strtok(NULL, ",\n", t);
		  if ((name != NULL) && (mode != NULL))
		     {if (_SC_debug)
			 {fprintf(_SC_diag, "   Open: %s, %s ... ", name, mode);
			  fflush(_SC_diag);};

		      fp = fopen(name, mode);}
		  else
		     fp = NULL;

                  status = -1;
                  if (fp != NULL)
                     {for (i = 0; i < MAX_FILES; i++)
                          {if (file[i] == NULL)
                              {file[i] = fp;
                               break;};};

                      status = (i < MAX_FILES) ? i : -1;};

                  if (_SC_debug)
                     {fprintf(_SC_diag, "status(%d)\n", status);
                      fflush(_SC_diag);};

                  REPLY(OPEN_MSG, status);

/* report the file length */
		  len = 0;
		  if (fp != NULL)
		     len = SC_file_size(fp);

                  REPLY(TELL_MSG, len);};

                 break;

            case SC_FSETVBUF :
                 {int ptype, type = -1, size;
                  char *bf = NULL;

                  ptype = SC_stol(SC_strtok(s+2, ",\n", t));
                  size  = SC_stol(SC_strtok(NULL, ",\n", t));

		  switch (ptype)
		     {case 1 :
		 	   type = _IOFBF;
			   bf   = CMAKE_N(char, size);
			   break;
		      case 2 :
			   type = _IOLBF;
			   bf   = CMAKE_N(char, size);
			   break;
		      case 3 :
			   type = _IONBF;
			   bf   = NULL;
			   break;
		      default :
			   
                           break;};

		  REPLY(SETVBUF_MSG, _SC_setvbuf(fp, bf, type, size));};

                 break;

            case SC_FCLOSE :
                 file[indx] = NULL;
                 if (_SC_debug)
                    {fprintf(_SC_diag, "   Close: %d(%p) ... ", indx, fp);
                     fflush(_SC_diag);};

                 REPLY(CLOSE_MSG, fclose(fp));
                 break;

            case SC_FFLUSH :
                 REPLY(FLUSH_MSG, fflush(fp));
                 break;

            case SC_FTELL :
                 REPLY(TELL_MSG, ftell(fp));
                 break;

            case SC_FSEEK :
                 {int64_t addr;
                  int offset, whence;

                  addr   = SC_stol(SC_strtok(s+2, ",\n", t));
                  offset = SC_stol(SC_strtok(NULL, ",\n", t));

		  switch (offset)
		     {default :
		      case 0  :
		 	   whence = SEEK_SET;
			   break;
		      case 1 :
		 	   whence = SEEK_CUR;
			   break;
		      case 2 :
		 	   whence = SEEK_END;
                           break;};

                  ret = fseek(fp, addr, whence);
                  REPLY(SEEK_MSG, ret);};

                 break;

            case SC_FREAD :
                 {long nir, nbw;
                  size_t nbi, ni;

                  nbi = SC_stol(SC_strtok(s+2, ",\n", t));
                  ni  = SC_stol(SC_strtok(NULL, ",\n", t));
                  bf  = CMAKE_N(char, nbi*ni);

                  nir = SC_fread_sigsafe(bf, nbi, ni, fp);
                  REPLY(READ_MSG, nir);

                  nbw = SC_write_sigsafe(cfd, bf, nbi*nir);
                  REPLY(READ_MSG, nbw);

                  CFREE(bf);};

                 break;

            case SC_FWRITE :
                 {long nbe, nbw;
                  size_t nbi, ni;

                  nbi = SC_stol(SC_strtok(s+2, ",\n", t));
                  ni  = SC_stol(SC_strtok(NULL, ",\n", t));
                  nbe = nbi*ni;
                  nbw = _SC_transfer_bytes(cfd, nbe, fp);

                  REPLY(WRITE_MSG, nbw);};

                 break;

            case SC_FPUTS :
                 {long nce, nct;

                  nce = SC_stol(SC_strtok(s+2, ",\n", t));
                  nct = _SC_transfer_bytes(cfd, nce, fp);

                  REPLY(PUTS_MSG, nct);};

                 break;

            case SC_FGETS :
                 {char *ret;
                  int nc;

                  nc = SC_stol(SC_strtok(s+2, ",\n", t));

                  bf = CMAKE_N(char, nc+1);
                  memset(bf, '\n', nc);
		  bf[nc] = '\0';
                  ret = SC_fgets(bf, nc, fp);
                  printf("%s", bf);
                  CFREE(bf);

                  REPLY(GETS_MSG, ((ret == NULL) ? -1 : 1));};

                 break;

            case SC_FGETC :
                 REPLY(GETC_MSG, fgetc(fp));
                 break;

            case SC_FUNGETC :
                 {int c;

                  c = SC_stol(SC_strtok(s+2, ",\n", t));
                  REPLY(UNGETC_MSG, ungetc(c, fp));};

                 break;

            case SC_FEXIT :
                 {int i;

                  ret = TRUE;
		  for (i = 0; i < MAX_FILES; i++)
		      {if (file[i] == NULL)
			  ret &= fclose(file[i]);};

/* close the data socket */
                  close(cfd);

                  REPLY(EXIT_MSG, ret);};

                 return(ret);

            default :
                 break;};};}

/*--------------------------------------------------------------------------*/

/*                             CLIENT SIDE ROUTINES                         */

/*--------------------------------------------------------------------------*/

/* _SC_RSETVBUF - set the file buffer
 *              - corresponds to SC_FSETVBUF message
 */

static int _SC_rsetvbuf(FILE *stream, char *bf, int type, size_t size)
   {REMOTE_FILE *fp;
    PROCESS *pp;
    int ret, ptype;

    fp = (REMOTE_FILE *) stream;
    if (fp == NULL)
       return(FALSE);

    if (fp->type == SC_LOCAL)
       ret = io_setvbuf((FILE *) fp->pp, bf, type, size);

    else
       {pp = fp->pp;

        switch (type)
           {case _IOFBF :
	         ptype = 1;
                 fp->buffer = bf;
                 fp->size   = size;
                 break;

            case _IOLBF :
	         ptype = 2;
                 fp->buffer = bf;
                 fp->size   = size;
                 break;

            case _IONBF :
	         ptype = 3;
                 fp->buffer = NULL;
                 fp->size   = 0L;
                 break;

            default :
	         return(-1);};

/* send the SC_FSETVBUF command to the server */
        SC_printf(pp, "%c%c,%d,%d\n",
		  SC_FSETVBUF, fp->fid, ptype, (int) size);

/* wait for the response */
        ret = _SC_get_cmd_resp(pp, SETVBUF_MSG);};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_RCLOSE - close the file wherever it is
 *            - corresponds to SC_FCLOSE message
 */

static int _SC_rclose(FILE *stream)
   {REMOTE_FILE *fp;
    PROCESS *pp;
    int ret;

    fp = (REMOTE_FILE *) stream;
    if (fp == NULL)
       return(FALSE);

    if (fp->type == SC_LOCAL)
       ret = io_close((FILE *) fp->pp);

    else
       {pp = fp->pp;

/* send the SC_FCLOSE command to the server */
        SC_printf(pp, "%c%c\n", SC_FCLOSE, fp->fid);

/* wait for the response */
        ret = _SC_get_cmd_resp(pp, CLOSE_MSG);};

    CFREE(fp);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_RFLUSH - do an fflush on the file wherever it is
 *            - corresponds to SC_FFLUSH message
 */

static int _SC_rflush(FILE *stream)
   {int rv;
    REMOTE_FILE *fp;
    PROCESS *pp;

    fp = (REMOTE_FILE *) stream;
    if (fp == NULL)
       rv = FALSE;

    else if (fp->type == SC_LOCAL)
       rv = io_flush((FILE *) fp->pp);

    else
       {pp = fp->pp;

/* send the SC_FFLUSH command to the server */
        SC_printf(pp, "%c%c\n", SC_FFLUSH, fp->fid);

/* wait for the response */
        rv = _SC_get_cmd_resp(pp, FLUSH_MSG);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_RTELL - do an ftell on the file wherever it is
 *           - corresponds to SC_FTELL message
 */

static long _SC_rtell(FILE *stream)
   {REMOTE_FILE *fp;
    PROCESS *pp;
    long addr;

    fp = (REMOTE_FILE *) stream;
    if (fp == NULL)
       addr = -1L;

    else if (fp->type == SC_LOCAL)
       addr = io_tell((FILE *) fp->pp);

    else
       {pp = fp->pp;

/* send the SC_FTELL command to the server */
        SC_printf(pp, "%c%c\n", SC_FTELL, fp->fid);

/* wait for the response */
        addr = _SC_get_cmd_resp(pp, TELL_MSG);

/* sync the local and remote addresses */
        if (addr != fp->cf_addr)
           fp->cf_addr = addr;};

    return(addr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_RSEEK - do an fseek on the file wherever it is
 *           - corresponds to SC_FSEEK message
 */

static int _SC_rseek(FILE *stream, long addr, int offset)
   {REMOTE_FILE *fp;
    PROCESS *pp;
    int poffs, rv;

    fp = (REMOTE_FILE *) stream;
    if (fp == NULL)
       rv = -1;

    else if (fp->type == SC_LOCAL)
       rv = io_seek((FILE *) fp->pp, addr, offset);

    else
       {pp = fp->pp;

        switch (offset)
           {case SEEK_SET :
	         poffs = 0;
		 fp->cf_addr = addr;
                 break;

            case SEEK_CUR :
	         poffs = 1;
		 fp->cf_addr += addr;
                 break;

            case SEEK_END :
	         poffs = 2;
		 fp->cf_addr = fp->file_size + addr;
                 break;

            default :
	         return(-1);};

/* send the SC_FSEEK command to the server */
        SC_printf(pp, "%c%c%ld,%d\n", SC_FSEEK, fp->fid, (long) addr, poffs);

/* wait for the response */
        rv = _SC_get_cmd_resp(pp, SEEK_MSG);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_RREAD - do an fread on the file wherever it is
 *           - corresponds to SC_FREAD message
 */

static size_t _SC_rread(void *s, size_t nbi, size_t ni, FILE *stream)
   {size_t rv;
    REMOTE_FILE *fp;
    PROCESS *pp;

    fp = (REMOTE_FILE *) stream;
    if (fp == NULL)
       rv = 0;

    else if (fp->type == SC_LOCAL)
       rv = io_read(s, nbi, ni, (FILE *) fp->pp);

    else
       {int nir, nbw, nbr, nbt;
        char *ps;

        pp = fp->pp;

/* send the SC_FREAD command to the server */
        SC_printf(pp, "%c%c%ld,%ld\n", SC_FREAD, fp->fid, nbi, ni);

/* wait for the response */
        nir = _SC_get_cmd_resp(pp, READ_MSG);

/* read the data looping until all expected bytes come in */
        nbt = nbi*nir;
        for (ps = s; TRUE; ps += nbr)
            {nbr  = SC_read_sigsafe(pp->data, ps, nbt);
             nbt -= nbr;
             if (nbt <= 0)
                break;};

/* wait for the response */
        nbw = _SC_get_cmd_resp(pp, READ_MSG);

        if (nbi*nir != nbw)
           nir = 0;

/* adjust the local idea of the current disk address */
        fp->cf_addr += nbw;

/* there is some checking to do here */
        rv = (nir < 0) ? 0 : (size_t) nir;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_RWRITE - do an fwrite on the file wherever it is
 *            - corresponds to SC_FWRITE message
 */

static size_t _SC_rwrite(void *s, size_t nbi, size_t ni, FILE *stream)
   {size_t niw;
    REMOTE_FILE *fp;
    PROCESS *pp;
    long nbw;

    fp = (REMOTE_FILE *) stream;
    if ((fp == NULL) || (nbi*ni == 0))
       niw = 0;

    else if (fp->type == SC_LOCAL)
       niw = io_write(s, nbi, ni, (FILE *) fp->pp);

    else
       {pp = fp->pp;

/* send the SC_FWRITE command to the server */
        SC_printf(pp, "%c%c%ld,%ld\n", SC_FWRITE, fp->fid, nbi, ni);

/* write the data */
        SC_write_sigsafe(pp->data, s, nbi*ni);

        nbw = _SC_get_cmd_resp(pp, WRITE_MSG);

/* adjust the local idea of the current disk address */
        fp->cf_addr += nbw;

/* must return items even though easier to deal with bytes to this point */
	niw = nbw/nbi;};

    return(niw);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_RPRINTF - do an fprintf style write to the given file
 *             - wherever it is
 */

static int _SC_rprintf(FILE *stream, char *fmt, va_list a)
   {int nw;
    size_t ni;
    REMOTE_FILE *fp;
    char *msg;

    fp = (REMOTE_FILE *) stream;
    if (fp == NULL)
       nw = 0;

    else if (fp != NULL)
       {msg = SC_vdsnprintf(TRUE, fmt, a);
	ni  = strlen(msg);
	nw  = io_write((void *) msg, (size_t) 1, ni, (FILE *) fp->pp);
	CFREE(msg);};

    return(nw);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_RPUTS - do an fputs on the file
 *           - corresponds to SC_FPUTS message
 */

static int _SC_rputs(const char *s, FILE *stream)
   {int nc, rv;
    long nbw;
    PROCESS *pp;
    REMOTE_FILE *fp;

    fp = (REMOTE_FILE *) stream;
    if (fp == NULL)
       rv = 0;

    else if (fp->type == SC_LOCAL)
       rv = io_puts(s, (FILE *) fp->pp);

    else
       {pp = fp->pp;

        nc = strlen(s);

/* send the SC_FPUTS command to the server */
        SC_printf(pp, "%c%c%d\n", SC_FPUTS, fp->fid, nc);

/* write the data */
        SC_write_sigsafe(pp->data, (char *) s, nc);

        nbw = _SC_get_cmd_resp(pp, PUTS_MSG);

/* adjust the local idea of the current disk address */
        fp->cf_addr += nbw;

/* wait for the response */
        rv = (nbw != nc) ? EOF : 0;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_RGETS - do an fgets on the file wherever it is
 *           - corresponds to SC_FGETS message
 */

static char *_SC_rgets(char *s, int nc, FILE *stream)
   {int ret;
    char *rv;
    REMOTE_FILE *fp;
    PROCESS *pp;

    fp = (REMOTE_FILE *) stream;
    if (fp == NULL)
       rv = NULL;

    else if (fp->type == SC_LOCAL)
       rv = io_gets(s, nc, (FILE *) fp->pp);

    else
       {pp = fp->pp;

/* send the SC_FGETS command to the server */
        SC_printf(pp, "%c%c%d\n", SC_FGETS, fp->fid, nc);

/* get the string */
        SC_gets(s, nc, pp);

/* adjust the local idea of the current disk address */
        fp->cf_addr += strlen(s);

/* wait for the response */
        ret = _SC_get_cmd_resp(pp, GETS_MSG);

        rv = (ret == -1) ? NULL : s;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_RGETC - do an fgetc on the file wherever it is
 *           - corresponds to SC_FGETC message
 */

static int _SC_rgetc(FILE *stream)
   {int rv;
    REMOTE_FILE *fp;
    PROCESS *pp;

    fp = (REMOTE_FILE *) stream;
    if (fp == NULL)
       rv = EOF;

    else if (fp->type == SC_LOCAL)
       rv = io_getc((FILE *) fp->pp);

    else
       {pp = fp->pp;

/* send the SC_FGETC command to the server */
        SC_printf(pp, "%c%c\n", SC_FGETC, fp->fid);

/* adjust the local idea of the current disk address */
        fp->cf_addr++;

/* wait for the response */
        rv = _SC_get_cmd_resp(pp, GETC_MSG);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_RUNGETC - do an fungetc on the file wherever it is
 *             - corresponds to SC_FUNGETC message
 */

static int _SC_rungetc(int c, FILE *stream)
   {int rv;
    REMOTE_FILE *fp;
    PROCESS *pp;

    fp = (REMOTE_FILE *) stream;
    if (fp == NULL)
       rv = 0;

    else if (fp->type == SC_LOCAL)
       rv = io_ungetc(c, (FILE *) fp->pp);

    else
       {pp = fp->pp;

/* send the SC_FUNGETC command to the server */
        SC_printf(pp, "%c%c%c\n", SC_FUNGETC, fp->fid, c);

/* adjust the local idea of the current disk address */
        fp->cf_addr--;

/* wait for the response */
        rv = _SC_get_cmd_resp(pp, UNGETC_MSG);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ROPEN - open the file via server on specified host
 *           - corresponds to SC_FOPEN message
 */

static FILE *_SC_ropen(char *name, char *mode)
   {char s[MAXLINE], host[MAXLINE], fname[MAXLINE], *t;
    int rsp, type, get_data_line, log;
    int64_t len;
    PROCESS *pp;
    REMOTE_FILE *fp;
    file_io_desc *fid;
    FILE *ret;
    static int data = -1;

    if (name == NULL)
       return(NULL);

    ret = NULL;
    log = FALSE;

/* if no host table initialize it */
    if (_SC.hosts == NULL)
       _SC.hosts = SC_make_hasharr(HSZSMALL, FALSE, SC_HA_NAME_KEY, 0);

    strcpy(host, "local");
    strcpy(fname, name);
    type = SC_LOCAL;

    fp = CMAKE(REMOTE_FILE);
    fp->type = type;

    if (type == SC_LOCAL)
       {fp->pp = (PROCESS *) SC_fopen(name, mode);
	if (!SC_process_alive(fp->pp))
           {CFREE(fp);};}

    else
       {get_data_line = FALSE;

/* find the relevant process */
        pp = (PROCESS *) SC_hasharr_def_lookup(_SC.hosts, host);
	if (!SC_process_alive(pp))
           {char *argv[4];
            
            snprintf(s, MAXLINE, "%s:pcexec", host);
            argv[0] = s;
            argv[1] = "-f";

/* NOTE: log should be turned on under the debugger to see the
 * transaction file from the server end
 */
	    if (log)
               {argv[2] = "-l";
                argv[3] = NULL;}
	    else
               argv[2] = NULL;

            pp = SC_open(argv, NULL, mode, NULL);
	    if (!SC_process_alive(pp))
               {CFREE(fp);
                return(NULL);};

            SC_block(pp);
            SC_hasharr_install(_SC.hosts, host, pp, "PROCESS", 3, -1);

            get_data_line = TRUE;};

/* send the SC_FOPEN command to the server */
        SC_printf(pp, "%c%c%s,%s\n", SC_FOPEN, 'A', fname, mode);

        if (get_data_line)
           {int port;

            port = _SC_get_cmd_resp(pp, "USE_PORT");
            if (port < 5000)
               {SC_close(pp);
                return(NULL);};

            data = SC_init_client(SC_strtok(s, ",:", t), port);
            if (data == -1)
               {SC_close(pp);
                CFREE(fp);}
	    else
	       {SC_block_fd(data);
		pp->data = data;};};

/* wait for the status */
        rsp = _SC_get_cmd_resp(pp, OPEN_MSG);

/* wait for the file length */
        len = _SC_get_cmd_resp(pp, TELL_MSG);

        if ((rsp != -1) && (pp->data != -1))
           {fp->pp        = pp;
            fp->fid       = rsp + 'A';
            fp->file_size = len;}
        else
	   {SC_close(pp);
	    CFREE(fp);};};

    if (fp != NULL)
       {fid          = SC_make_io_desc(fp);
	fid->info    = fp;
	fid->fopen   = _SC_ropen;
	fid->ftell   = _SC_rtell;
	fid->fseek   = _SC_rseek;
	fid->fread   = _SC_rread;
	fid->fwrite  = _SC_rwrite;
	fid->setvbuf = _SC_rsetvbuf;
	fid->fclose  = _SC_rclose;
	fid->fprintf = _SC_rprintf;
	fid->fputs   = _SC_rputs;
	fid->fgetc   = _SC_rgetc;
	fid->ungetc  = _SC_rungetc;
	fid->fflush  = _SC_rflush;
	fid->fgets   = _SC_rgets;

	ret = (FILE *) fid;}
    else
       ret = NULL;

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_REXIT - close a remote file server
 *           - corresponds to SC_FEXIT message
 */

static int _SC_rexit(haelem *hp, void *a)
   {int ret, ok;
    PROCESS *pp;

    ok = SC_haelem_data(hp, NULL, NULL, (void **) &pp, FALSE);
    SC_ASSERT(ok == TRUE);

    ret = TRUE;

/* send the SC_FEXIT command to the server */
    if (SC_process_alive(pp))
       {SC_printf(pp, "%c%c\n", SC_FEXIT, 'A');
	CFREE(pp);};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_EXIT_ALL - close all remote file servers with the SC_FEXIT message */

int SC_exit_all(void)
   {int ret;

    ret = TRUE;

    if (_SC.hosts != NULL)

/* send FEXIT to all servers in the host table */
       {SC_free_hasharr(_SC.hosts, _SC_rexit, NULL);
        _SC.hosts = NULL;}

    return(ret);}

/*--------------------------------------------------------------------------*/

/*                              FILE I/O API                                */

/*--------------------------------------------------------------------------*/

/* _SC_LFTELL - large file IO wrapper for FTELL method */

static int64_t _SC_lftell(FILE *fp)
   {int64_t rv;

#ifdef _LARGE_FILES
    extern off_t ftello(FILE *fp);

    rv = ftello(fp);

#else

    rv = ftell(fp);

#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LFSEEK - large file IO wrapper for FSEEK method */

static int _SC_lfseek(FILE *fp, int64_t offs, int whence)
   {int rv;

#ifdef _LARGE_FILES
    extern int fseeko(FILE *fp, off_t offs, int whence);

    rv = fseeko(fp, offs, whence);

#else

    rv = fseek(fp, offs, whence);

#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LFREAD - large file IO wrapper for FREAD method */

static uint64_t _SC_lfread(void *s, size_t bpi, uint64_t ni, FILE *fp)
   {uint64_t zc, n, ns, nr;
    char *ps;

    zc = 0;
    ns = ni;
    nr = 0;
    ps = (char *) s;
    while ((ns > 0) && (zc < 10))
       {n = SC_fread_sigsafe(ps, bpi, (size_t) ns, fp);

	zc = (n == 0) ? zc + 1 : 0;

	ps += bpi*n;
	ns -= n;
        nr += n;};

    return(nr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LFWRITE - large file IO wrapper for FWRITE method */

static uint64_t _SC_lfwrite(void *s, size_t bpi, uint64_t ni, FILE *fp)
   {uint64_t zc, n, ns, nw;
    char *ps;

    zc = 0;
    ns = ni;
    nw = 0;
    ps = (char *) s;
    while ((ns > 0) && (zc < 10))
       {n = SC_fwrite_sigsafe(ps, bpi, ns, fp);

	zc = (n == 0) ? zc + 1 : 0;

        if (n < ns)
           fflush(fp);

	ps += bpi*n;
	ns -= n;
        nw += n;};

    return(nw);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_LFOPEN - large file wrapper for fopen for handling null file names */

FILE *SC_lfopen(char *name, char *mode)
   {FILE *ret, *fp;
    file_io_desc *fid;

    ret = NULL;

    if (name != NULL)
       {fp = fopen(name, mode);
	if (fp != NULL)
	   {fid          = SC_make_io_desc(fp);
	    fid->fopen   = SC_lfopen;
	    fid->lftell  = _SC_lftell;
	    fid->lfseek  = _SC_lfseek;
	    fid->lfread  = _SC_lfread;
	    fid->lfwrite = _SC_lfwrite;
	    fid->setvbuf = _SC_setvbuf;
	    fid->fclose  = fclose;
	    fid->fprintf = _SC_fprintf;
	    fid->fputs   = _SC_fputs;
	    fid->fgetc   = fgetc;
	    fid->ungetc  = ungetc;
	    fid->fflush  = _SC_fflush;
	    fid->feof    = feof;
	    fid->fgets   = SC_fgets;

	    ret = (FILE *) fid;}
	else
	   ret = NULL;};

    return(ret);}

/*--------------------------------------------------------------------------*/

/*                               IO WRAPPERS                                */

/*--------------------------------------------------------------------------*/

/* IO_SETVBUF - file IO wrapper for SETVBUF method
 *            - return 0 on success
 */

int io_setvbuf(FILE *fp, char *bf, int type, size_t sz)
   {int rv;
    file_io_desc *fid;

    rv = 0;

    if (fp != NULL)
       {if (IS_STD_IO(fp))
	   rv = _SC_setvbuf(fp, bf, type, sz);

        else
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

            IO_OPER_START_TIME(fid);

	    if (fid->setvbuf != NULL)
	       rv = (*fid->setvbuf)(fp, bf, type, sz);

            IO_OPER_ACCUM_TIME(fid, IO_OPER_SETVBUF);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IO_TELL - file IO wrapper for FTELL method */

long io_tell(FILE *fp)
   {long rv;
    file_io_desc *fid;

    rv = -1;
    if (fp != NULL)
       {if (IS_STD_IO(fp))
	   rv = ftell(fp);
    
        else
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    if (fid->ftell != NULL)
	       {IO_OPER_START_TIME(fid);
		rv = (*fid->ftell)(fp);
		IO_OPER_ACCUM_TIME(fid, IO_OPER_FTELL);}

	    else if (fid->lftell != NULL)
	       {IO_OPER_START_TIME(fid);
		rv = (*fid->lftell)(fp);
		IO_OPER_ACCUM_TIME(fid, IO_OPER_LFTELL);};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IO_SEEK - file IO wrapper for FSEEK method */

int io_seek(FILE *fp, long offs, int whence)
   {int rv;
    file_io_desc *fid;

    rv = -1;
    if (fp != NULL)
       {if (IS_STD_IO(fp))
	   rv = fseek(fp, offs, whence);
    
        else
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    if (fid->fseek != NULL)
	       {IO_OPER_START_TIME(fid);
		rv = (*fid->fseek)(fp, offs, whence);
		IO_OPER_ACCUM_TIME(fid, IO_OPER_FSEEK);}

	    else if (fid->lfseek != NULL)
	       {IO_OPER_START_TIME(fid);
		rv = (*fid->lfseek)(fp, (int64_t) offs, whence);
		IO_OPER_ACCUM_TIME(fid, IO_OPER_LFSEEK);};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IO_READ - file IO wrapper for FREAD method */

size_t io_read(void *p, size_t sz, size_t ni, FILE *fp)
   {size_t rv;
    file_io_desc *fid;

    rv = -1;
    if (fp != NULL)
       {if (IS_STD_IO(fp))
	   rv = SC_fread_sigsafe(p, sz, ni, fp);
    
        else
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    if (fid->fread != NULL)
	       {IO_OPER_START_TIME(fid);
		rv = (*fid->fread)(p, sz, ni, fp);
		IO_OPER_ACCUM_TIME(fid, IO_OPER_FREAD);}

	    else if (fid->lfread != NULL)
	       {IO_OPER_START_TIME(fid);
		rv = (*fid->lfread)(p, sz, (uint64_t) ni, fp);
		IO_OPER_ACCUM_TIME(fid, IO_OPER_LFREAD);};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IO_WRITE - file IO wrapper for FWRITE method */

size_t io_write(void *p, size_t sz, size_t ni, FILE *fp)
   {size_t rv;
    file_io_desc *fid;

    rv = -1;
    if (fp != NULL)
       {if (IS_STD_IO(fp))
	   rv = _SC_fwrite_atm(p, sz, ni, fp);
    
        else
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    if (fid->fwrite != NULL)
	       {IO_OPER_START_TIME(fid);
		rv = (*fid->fwrite)(p, sz, ni, fp);
		IO_OPER_ACCUM_TIME(fid, IO_OPER_FWRITE);}

	    else if (fid->lfwrite != NULL)
	       {IO_OPER_START_TIME(fid);
		rv = (*fid->lfwrite)(p, sz, (uint64_t) ni, fp);
		IO_OPER_ACCUM_TIME(fid, IO_OPER_LFWRITE);};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IO_CLOSE - file IO wrapper for FCLOSE method */

int io_close(FILE *fp)
   {int rv;
    file_io_desc *fid;

    rv = EOF;
    if (fp != NULL)
       {if (IS_STD_IO(fp))
	   rv = fclose(fp);
    
        else
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    IO_OPER_START_TIME(fid);

	    if (fid->fclose != NULL)
	       {rv = (*fid->fclose)(fp);
		fid->fclose = NULL;};

	    IO_OPER_ACCUM_TIME(fid, IO_OPER_FCLOSE);

	    CFREE(fid);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IO_PRINTF - wrapper for fprintf method of file FP */

int io_printf(FILE *fp, char *fmt, ...)
   {int rv;
    char *s;
    file_io_desc *fid;

    rv = 0;
    if (fp != NULL)
       {if (IS_STD_IO(fp))
           {SC_VDSNPRINTF(TRUE, s, fmt);

	    rv = strlen(s);

	    SC_mpi_fputs(s, fp);

	    CFREE(s);}
    
        else
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    IO_OPER_START_TIME(fid);

	    if (fid->fprintf != NULL)
	       {SC_VA_START(fmt);
		rv = (*fid->fprintf)(fp, fmt, __a__);
		SC_VA_END;};

	    IO_OPER_ACCUM_TIME(fid, IO_OPER_FPRINTF);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IO_PUTS - file IO wrapper for FPUTS method */

int io_puts(const char *s, FILE *fp)
   {int rv;
    file_io_desc *fid;

    rv = EOF;
    if (s == NULL)
       rv = 0;

    else if (fp != NULL)
       {if (IS_STD_IO(fp))
	   rv = SC_mpi_fputs(s, fp);
    
        else
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    IO_OPER_START_TIME(fid);

	    if (fid->fputs != NULL)
	       rv = (*fid->fputs)(s, fp);

	    IO_OPER_ACCUM_TIME(fid, IO_OPER_FPUTS);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IO_GETC - file IO wrapper for FGETC method */

int io_getc(FILE *fp)
   {int rv;
    file_io_desc *fid;

    rv = EOF;
    if (fp != NULL)
       {if (IS_STD_IO(fp))
	   rv = fgetc(fp);
    
        else
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    IO_OPER_START_TIME(fid);

	    if (fid->fgetc != NULL)
	       rv = (*fid->fgetc)(fp);

	    IO_OPER_ACCUM_TIME(fid, IO_OPER_FGETC);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IO_UNGETC - file IO wrapper for UNGETC method */

int io_ungetc(int c, FILE *fp)
   {int rv;
    file_io_desc *fid;

    rv = EOF;
    if (fp != NULL)
       {if (IS_STD_IO(fp))
	   rv = ungetc(c, fp);
    
        else
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    IO_OPER_START_TIME(fid);

	    if (fid->ungetc != NULL)
	       rv = (*fid->ungetc)(c, fp);

	    IO_OPER_ACCUM_TIME(fid, IO_OPER_UNGETC);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IO_FLUSH - file IO wrapper for FFLUSH method */

int io_flush(FILE *fp)
   {int rv;
    file_io_desc *fid;

    rv = EOF;
    if (fp != NULL)
       {if (IS_STD_IO(fp))
	   rv = fflush(fp);
    
        else
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    IO_OPER_START_TIME(fid);

	    if (fid->fflush != NULL)
	       rv = (*fid->fflush)(fp);

	    IO_OPER_ACCUM_TIME(fid, IO_OPER_FFLUSH);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IO_GETS - file IO wrapper for FGETS method */

char *io_gets(char *s, int n, FILE *fp)
   {char *rv;
    file_io_desc *fid;

    rv = NULL;
    if (fp != NULL)
       {if (IS_STD_IO(fp))
	   rv = SC_fgets(s, n, fp);
    
        else
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    IO_OPER_START_TIME(fid);

	    if (fid->fgets != NULL)
	       rv = (*fid->fgets)(s, n, fp);

	    IO_OPER_ACCUM_TIME(fid, IO_OPER_FGETS);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IO_EOF - file IO wrapper for FEOF method */

int io_eof(FILE *fp)
   {int rv;
    file_io_desc *fid;

    rv = 0;
    if (fp != NULL)
       {if (IS_STD_IO(fp))
	   rv = feof(fp);
    
        else
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    IO_OPER_START_TIME(fid);

	    if (fid->feof != NULL)
	       rv = (*fid->feof)(fp);

	    IO_OPER_ACCUM_TIME(fid, IO_OPER_FEOF);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IO_POINTER - file IO wrapper for POINTER method
 *            - this applies only to memory mapped files
 */

char *io_pointer(void *a)
   {char *rv;
    file_io_desc *fid;
    FILE *fp;

    fp = (FILE *) a;
    rv = NULL;
    if (fp != NULL)
       {if (!IS_STD_IO(fp))
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    IO_OPER_START_TIME(fid);

	    if (fid->pointer != NULL)
	       rv = (*fid->pointer)(fp);

	    IO_OPER_ACCUM_TIME(fid, IO_OPER_POINTER);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IO_SEGSIZE - file IO wrapper for SEGSIZE method */

uint64_t io_segsize(void *a, int64_t n)
   {uint64_t rv;
    file_io_desc *fid;
    FILE *fp;

    fp = (FILE *) a;
    rv = 0;
    if (fp != NULL)
       {if (!IS_STD_IO(fp))
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    IO_OPER_START_TIME(fid);

	    if (fid->segsize != NULL)
	       rv = (*fid->segsize)(fp, n);

	    IO_OPER_ACCUM_TIME(fid, IO_OPER_SEGSIZE);};};

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                               LIO WRAPPERS                               */

/*--------------------------------------------------------------------------*/

/* LIO_SETVBUF - large file IO wrapper for SETVBUF method
 *             - return 0 on success
 */

int lio_setvbuf(FILE *fp, char *bf, int type, size_t sz)
   {int rv;
    file_io_desc *fid;

    rv = 0;

    if (fp != NULL)
       {if (IS_STD_IO(fp))
	   rv = _SC_setvbuf(fp, bf, type, sz);

        else
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

            IO_OPER_START_TIME(fid);

	    if (fid->setvbuf != NULL)
	       rv = (*fid->setvbuf)(fp, bf, type, sz);

            IO_OPER_ACCUM_TIME(fid, IO_OPER_SETVBUF);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LIO_TELL - large file IO wrapper for FTELL method */

int64_t lio_tell(FILE *fp)
   {int64_t rv;
    file_io_desc *fid;

    rv = -1;
    if (fp != NULL)
       {if (IS_STD_IO(fp))
	   rv = _SC_lftell(fp);
    
        else
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    if (fid->lftell != NULL)
	       {IO_OPER_START_TIME(fid);
		rv = (*fid->lftell)(fp);
		IO_OPER_ACCUM_TIME(fid, IO_OPER_LFTELL);}

	    else if (fid->ftell != NULL)
	       {IO_OPER_START_TIME(fid);
		rv = (*fid->ftell)(fp);
		IO_OPER_ACCUM_TIME(fid, IO_OPER_FTELL);};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LIO_SEEK - large file IO wrapper for FSEEK method */

int lio_seek(FILE *fp, int64_t offs, int whence)
   {int rv;
    file_io_desc *fid;

    rv = -1;
    if (fp != NULL)
       {if (IS_STD_IO(fp))
	   rv = _SC_lfseek(fp, offs, whence);
    
        else
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    if (fid->lfseek != NULL)
	       {IO_OPER_START_TIME(fid);
		rv = (*fid->lfseek)(fp, (int64_t) offs, whence);
		IO_OPER_ACCUM_TIME(fid, IO_OPER_LFSEEK);}

	    else if (fid->fseek != NULL)
	       {IO_OPER_START_TIME(fid);
		rv = (*fid->fseek)(fp, offs, whence);
		IO_OPER_ACCUM_TIME(fid, IO_OPER_FSEEK);};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LIO_READ - large file IO wrapper for FREAD method */

uint64_t lio_read(void *p, size_t sz, uint64_t ni, FILE *fp)
   {uint64_t rv;
    file_io_desc *fid;

    rv = -1;
    if (fp != NULL)
       {if (IS_STD_IO(fp))
	   rv = SC_fread_sigsafe(p, sz, ni, fp);
    
        else
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    if (fid->lfread != NULL)
	       {IO_OPER_START_TIME(fid);
		rv = (*fid->lfread)(p, sz, ni, fp);
		IO_OPER_ACCUM_TIME(fid, IO_OPER_LFREAD);}

	    else if (fid->fread != NULL)
	       {IO_OPER_START_TIME(fid);
		rv = (*fid->fread)(p, sz, (size_t) ni, fp);
		IO_OPER_ACCUM_TIME(fid, IO_OPER_FREAD);};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LIO_WRITE - large file IO wrapper for FWRITE method */

uint64_t lio_write(void *p, size_t sz, uint64_t ni, FILE *fp)
   {uint64_t rv;
    file_io_desc *fid;

    rv = -1;
    if (fp != NULL)
       {if (IS_STD_IO(fp))
	   rv = _SC_fwrite_atm(p, sz, ni, fp);
    
        else
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    if (fid->lfwrite != NULL)
	       {IO_OPER_START_TIME(fid);
		rv = (*fid->lfwrite)(p, sz, ni, fp);
		IO_OPER_ACCUM_TIME(fid, IO_OPER_LFWRITE);}

	    else if (fid->fwrite != NULL)
	       {IO_OPER_START_TIME(fid);
		rv = (*fid->fwrite)(p, sz, (size_t) ni, fp);
		IO_OPER_ACCUM_TIME(fid, IO_OPER_FWRITE);};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LIO_CLOSE - large file IO wrapper for FCLOSE method */

int lio_close(FILE *fp)
   {int rv;
    file_io_desc *fid;

    rv = EOF;
    if (fp != NULL)
       {if (IS_STD_IO(fp))
	   rv = fclose(fp);
    
        else
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    IO_OPER_START_TIME(fid);

	    if (fid->fclose != NULL)
	       {rv = (*fid->fclose)(fp);
		fid->fclose = NULL;};

	    IO_OPER_ACCUM_TIME(fid, IO_OPER_FCLOSE);

	    CFREE(fid);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LIO_PRINTF - wrapper for fprintf method of file FP
 *            - we cannot use a macro as we do for the other methods
 *            - because of the variable arg list
 */

int lio_printf(FILE *fp, char *fmt, ...)
   {int rv;
    char *s;
    file_io_desc *fid;

    rv = 0;
    if (fp != NULL)
       {if (IS_STD_IO(fp))
           {SC_VDSNPRINTF(TRUE, s, fmt);

	    rv = strlen(s);

	    SAFE_FPUTS(s, fp);

	    CFREE(s);}
    
        else
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    IO_OPER_START_TIME(fid);

	    if (fid->fprintf != NULL)
	       {SC_VA_START(fmt);
		rv = (*fid->fprintf)(fp, fmt, __a__);
		SC_VA_END;};

	    IO_OPER_ACCUM_TIME(fid, IO_OPER_FPRINTF);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LIO_PUTS - large file IO wrapper for FPUTS method */

int lio_puts(const char *s, FILE *fp)
   {int rv;
    file_io_desc *fid;

    rv = EOF;
    if (fp != NULL)
       {if (IS_STD_IO(fp))
	   rv = SAFE_FPUTS(s, fp);
    
        else
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    IO_OPER_START_TIME(fid);

	    if (fid->fputs != NULL)
	       rv = (*fid->fputs)(s, fp);

	    IO_OPER_ACCUM_TIME(fid, IO_OPER_FPUTS);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LIO_GETC - large file IO wrapper for FGETC method */

int lio_getc(FILE *fp)
   {int rv;
    file_io_desc *fid;

    rv = EOF;
    if (fp != NULL)
       {if (IS_STD_IO(fp))
	   rv = fgetc(fp);
    
        else
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    IO_OPER_START_TIME(fid);

	    if (fid->fgetc != NULL)
	       rv = (*fid->fgetc)(fp);

	    IO_OPER_ACCUM_TIME(fid, IO_OPER_FGETC);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LIO_UNGETC - large file IO wrapper for UNGETC method */

int lio_ungetc(int c, FILE *fp)
   {int rv;
    file_io_desc *fid;

    rv = EOF;
    if (fp != NULL)
       {if (IS_STD_IO(fp))
	   rv = ungetc(c, fp);
    
        else
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    IO_OPER_START_TIME(fid);

	    if (fid->ungetc != NULL)
	       rv = (*fid->ungetc)(c, fp);

	    IO_OPER_ACCUM_TIME(fid, IO_OPER_UNGETC);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LIO_FLUSH - large file IO wrapper for FFLUSH method */

int lio_flush(FILE *fp)
   {int rv;
    file_io_desc *fid;

    rv = EOF;
    if (fp != NULL)
       {if (IS_STD_IO(fp))
	   rv = fflush(fp);
    
        else
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    IO_OPER_START_TIME(fid);

	    if (fid->fflush != NULL)
	       rv = (*fid->fflush)(fp);

	    IO_OPER_ACCUM_TIME(fid, IO_OPER_FFLUSH);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LIO_GETS - large file IO wrapper for FGETS method */

char *lio_gets(char *s, int n, FILE *fp)
   {char *rv;
    file_io_desc *fid;

    rv = NULL;
    if (fp != NULL)
       {if (IS_STD_IO(fp))
	   rv = SC_fgets(s, n, fp);
    
        else
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    IO_OPER_START_TIME(fid);

	    if (fid->fgets != NULL)
	       rv = (*fid->fgets)(s, n, fp);

	    IO_OPER_ACCUM_TIME(fid, IO_OPER_FGETS);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LIO_EOF - large file IO wrapper for FEOF method */

int lio_eof(FILE *fp)
   {int rv;
    file_io_desc *fid;

    rv = TRUE;
    if (fp != NULL)
       {if (IS_STD_IO(fp))
	   rv = feof(fp);
    
        else
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    IO_OPER_START_TIME(fid);

	    if (fid->feof != NULL)
	       rv = (*fid->feof)(fp);

	    IO_OPER_ACCUM_TIME(fid, IO_OPER_FEOF);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LIO_POINTER - large file IO wrapper for POINTER method
 *             - this applies only to memory mapped files
 */

char *lio_pointer(void *a)
   {char *rv;
    file_io_desc *fid;
    FILE *fp;

    fp = (FILE *) a;
    rv = NULL;
    if (fp != NULL)
       {if (!IS_STD_IO(fp))
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    IO_OPER_START_TIME(fid);

	    if (fid->pointer != NULL)
	       rv = (*fid->pointer)(fp);

	    IO_OPER_ACCUM_TIME(fid, IO_OPER_POINTER);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LIO_SEGSIZE - large file IO wrapper for SEGSIZE method */

uint64_t lio_segsize(void *a, int64_t n)
   {uint64_t rv;
    file_io_desc *fid;
    FILE *fp;

    fp = (FILE *) a;
    rv = 0;
    if (fp != NULL)
       {if (!IS_STD_IO(fp))
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    IO_OPER_START_TIME(fid);

	    if (fid->segsize != NULL)
	       rv = (*fid->segsize)(fp, n);

	    IO_OPER_ACCUM_TIME(fid, IO_OPER_SEGSIZE);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_STD_FILE - given the specific file emulation data P
 *             - return a file_io_desc cast as a FILE for
 *             - use by the IO_ functions
 */

FILE *SC_std_file(void *p)
   {file_io_desc *fid;
    FILE *fp;

    fid          = SC_make_io_desc(p);
    fid->fopen   = SC_fopen;
    fid->ftell   = ftell;
    fid->fseek   = fseek;
    fid->fread   = fread;
    fid->fwrite  = _SC_fwrite;
    fid->setvbuf = _SC_setvbuf;
    fid->fclose  = fclose;
    fid->fprintf = _SC_fprintf;
    fid->fputs   = _SC_fputs;
    fid->fgetc   = fgetc;
    fid->ungetc  = ungetc;
    fid->fflush  = _SC_fflush;
    fid->feof    = feof;
    fid->fgets   = SC_fgets;

    fp = (FILE *) fid;

    return(fp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_IO_CONNECT - connect the PDB io hooks for remote access */

int SC_io_connect(int flag)
   {int ret;

    ret = TRUE;

#ifndef HAVE_PROCESS_CONTROL
    if (flag == SC_REMOTE)
       flag = SC_LOCAL;
#endif

    switch (flag)
       {case SC_REMOTE :
	     io_open_hook = _SC_ropen;
	     break;

        case SC_BUFFERED :
	     io_open_hook = SC_bopen;
	     break;

        case SC_LOCAL :
	     io_open_hook = SC_fopen;
	     break;

	default :
	     ret = FALSE;
	     break;};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FILE_LENGTH - return the byte length of the named file
 *                - return -1 if the file does not exist
 *
 * #bind SC_file_length fortran() scheme(file-length) python()
 */

int64_t SC_file_length(char *name)
   {int64_t ln;
    char path[PATH_MAX];

    ln = -1L;
    if ((name != NULL) &&
	(SC_file_path(name, path, PATH_MAX, TRUE) == 0))
       ln = SC_FILE_LENGTH(path);

    return(ln);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FILE_SIZE - return the byte length of the FILE fp
 *              - NOTE: this is used by the hooks and must use the
 *              - raw C library functions
 *              - compare with SC_filelen
 */

int64_t SC_file_size(FILE *fp)
   {int64_t ad, ln;

/* remember where we are */
    ad = ftell(fp);

    fseek(fp, 0, SEEK_END);
    ln = ftell(fp);

/* return to where we started */
    fseek(fp, ad, SEEK_SET);

    return(ln);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FILELEN - return the length of the given file
 *            - NOTE: this uses the hooks not the
 *            - raw C library functions
 *            - compare with SC_file_size
 */

int64_t SC_filelen(FILE *fp)
   {int64_t caddr, flen;

    caddr = lio_tell(fp);
    lio_seek(fp, 0, SEEK_END);

    flen = lio_tell(fp);
    lio_seek(fp, caddr, SEEK_SET);

    return(flen);}
 
/*--------------------------------------------------------------------------*/

/*                            SAFE I/O ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* SC_READ_SIGSAFE - make read safe for signals
 *                 - necessitated by broken AIX behavior
 *                 - wrt sigaction
 */

ssize_t SC_read_sigsafe(int fd, void *bf, size_t n)
   {int blk;
    long nbo, nbr;
    ssize_t rv;
    char *pbf;

#ifdef AIX
    PFSignal_handler oh;

/* turn off SIGIO handler */
    if (SC_gs.io_interrupts_on == TRUE)
       oh = SC_signal_action_n(SC_SIGIO, SIG_IGN, NULL, 0, -1);

#endif

    blk = SC_isblocked_fd(fd);

/* non-blocking read or terminal - take what you get */
    if ((blk == FALSE) || (isatty(fd) == TRUE))
       rv = read(fd, bf, n);

/* blocking read - insist on the specified number of bytes or an error */
    else
       {nbo = n;
	nbr = -1;
	pbf = bf;
	rv  = 0;

	while ((nbo > 0) && (nbr != 0))
	   {nbr = read(fd, pbf, nbo);
	    if (nbr < 0)
	       {rv = nbr;
		break;};
	    pbf += nbr;
	    nbo -= nbr;
	    rv  += nbr;};};

#ifdef AIX

/* turn on SIGIO handler */
    if (SC_gs.io_interrupts_on == TRUE)
       SC_signal_action_n(SC_SIGIO, oh, NULL, 0, BLOCK_WITH_SIGIO, -1);
 
#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_WRITE_SIGSAFE - make write safe for signals
 *                  - and to ensure that the requested number of bytes
 *                  - is written if at all possible
 *                  - return number of bytes written or -1
 */

ssize_t SC_write_sigsafe(int fd, void *bf, size_t n)
   {long nbo, nbw, zc;
    ssize_t rv;
    char *pbf;

    nbo = n;
    nbw = 0;
    pbf = bf;
    zc  = 0;

    while ((nbo > 0) && (zc < 10))
       {nbw = write(fd, pbf, nbo);
	if (nbw < 0)

/* if EAGAIN/EWOULDBLOCK try sleeping 10 ms to let the system catch up
 * limit the number of attempts to 10
 */
	   {if (errno == EAGAIN)
               {SC_sleep(10);
                zc++;
		continue;}
	    else
	       break;}
	else
	   zc = 0;

	pbf += nbw;
	nbo -= nbw;};

    rv = (nbw < 0) ? nbw : n;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FREAD_SAFE - atomic worker for SC_FREAD_SIGSAFE */

static size_t _SC_fread_safe(void *s, size_t bpi, size_t ni, FILE *fp)
   {size_t zc, n, ns, nr;
    char *ps;

    zc = 0;
    ns = ni;
    nr = 0;
    ps = (char *) s;
    while ((ns > 0) && (zc < 10))
       {n = fread(ps, bpi, ns, fp);

	zc = (n == 0) ? zc + 1 : 0;

	ps += bpi*n;
	ns -= n;
        nr += n;};

    return(nr);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FREAD_SIGSAFE - make fread safe for signals
 *                  - and to ensure that the requested number of bytes
 */

size_t SC_fread_sigsafe(void *s, size_t bpi, size_t ni, FILE *fp)
   {size_t rv;

#ifdef AIX
    PFSignal_handler oh;

/* turn off SIGIO handler */
    if (SC_gs.io_interrupts_on == TRUE)
       oh = SC_signal_action_n(SC_SIGIO, SIG_IGN, NULL, 0, -1);

    rv = _SC_fread_safe(s, bpi, ni, fp);

/* turn on SIGIO handler */
    if (SC_gs.io_interrupts_on == TRUE)
       SC_signal_action_n(SC_SIGIO, oh, NULL, 0, BLOCK_WITH_SIGIO, -1);
 
#else

    rv = _SC_fread_safe(s, bpi, ni, fp);

#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FWRITE_SIGSAFE - make fwrite safe for signals
 *                   - and to ensure that the requested number of bytes
 *                   - is written if at all possible
 *                   - return number of bytes written or -1
 */

size_t SC_fwrite_sigsafe(void *s, size_t bpi, size_t ni, FILE *fp)
   {size_t zc, n, ns, nw;
    char *ps;

    zc = 0;
    ns = ni;
    nw = 0;
    ps = (char *) s;
    while ((ns > 0) && (zc < 10))
       {n = fwrite(ps, bpi, ns, fp);

	if (n <= 0)
           {zc++;
            if (ferror(fp) != 0)
	       {clearerr(fp);
		SC_sleep(10);};}
        else
	   zc = 0;

        if (n < ns)
           fflush(fp);

	ps += bpi*n;
	ns -= n;
        nw += n;};

    return(nw);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DGETS - read arbitrarily long lines */

char *SC_dgets(char *bf, int *pnc, FILE *fp)
   {int ok, zsp;
    long i, no, nn, nbr, nb;
    off_t ad;
    char *pbf;

    if (fp == NULL)
       {nn = 0;
	bf = NULL;}

    else
       {no = *pnc;

	zsp = SC_zero_space_n(2, -1);

	if (bf == NULL)
	   {if (no == 0)
	       no = MAX_BFSZ;

	    bf = CMAKE_N(char, no);};

	ad  = io_tell(fp);
	nb  = 0;
	nbr = 0;
	for (pbf = bf, nn = no, ok = TRUE; ok == TRUE; )
	    {nbr += io_read(pbf, 1, no, fp);
	     if (nbr < nn)
	        bf[nbr] = '\0';

	     if (nbr == 0)
	        ok = FALSE;

/* check for newlines */
	     else if (nbr > 0)
	        {for (i = 0; (i < no) && (pbf[i] != '\n') && (pbf[i] != '\r'); i++);
		 nb = nn - no + i + 1;
		 if (nb < nn)
		    {bf[nb] = '\0';
		     ok     = FALSE;}
		 else
		    {no  = nn;
		     nn  = (no << 1);
		     CREMAKE(bf, char, nn);
		     pbf = bf + no;};};};

	if (nbr == 0)
	   {CFREE(bf);
	    nn = 0;};

	io_seek(fp, ad + nb, SEEK_SET);

	zsp = SC_zero_space_n(zsp, -1);};

    *pnc = nn;

    return(bf);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

