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

#define REPLY(msg, val)                                     \
   {printf("%s:%ld\n", msg, (long) (val));                  \
    fflush(stdout);                                         \
    if (_SC_debug)                                          \
       {fprintf(_SC_diag, "%s:%ld\n", msg, (long) (val));   \
        fflush(_SC_diag);};}

typedef struct s_REMOTE_FILE REMOTE_FILE;

struct s_REMOTE_FILE
   {PROCESS *pp;
    int type;
    int fid;
    off_t file_size;
    char *buffer;
    long sb_addr;
    long size;
    off_t cf_addr;};

int
 SC_verify_writes = FALSE;

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
	SFREE(msg);};

    FLUSH_ON(fp, NULL);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SETVBUF - a dummy setvbuf for the marginal systems */

static int _SC_setvbuf(FILE *stream, char *buf, int type, size_t size)
   {int ret;

#ifdef IBMMP

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

static size_t _SC_fread(void *s, size_t bpi, size_t nitems, FILE *fp)
   {size_t nr;

    nr = SC_fread_sigsafe(s, bpi, nitems, fp);

    return(nr);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FWRITE_ATM - atomic worker for _SC_fwrite */

static size_t _SC_fwrite_atm(void *s, size_t bpi, size_t nitems, FILE *fp)
   {size_t nw;

    nw = SC_fwrite_sigsafe(s, bpi, nitems, fp);

    return(nw);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FWRITE - controlled version of C fwrite for marginal systems
 *            - marginal manifests when CPU write rate outstrips
 *            - NFS or other network bandwitdth
 */

size_t _SC_fwrite(void *s, size_t bpi, size_t nitems, FILE *fp)
   {int ok, flags, rw, fd;
    BIGINT i, nb;
    size_t nw, nr;
    off_t addr;
    char msg[MAXLINE];
    char *ps, *t;

/* turn off SIGIO handler */
    SC_catch_io_interrupts(FALSE);

    if (SC_verify_writes == TRUE)

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

	nw = _SC_fwrite_atm(s, bpi, nitems, fp);

/* go back and read it in
 * fseek will fail unless the file mode is read-write
 */
	if ((addr >= 0) && (nw > 0) && (rw == TRUE))
	   {nb = bpi*nitems;
	    t  = FMAKE_N(char, nb, "char*:_SC_FWRITE:t");

	    fseek(fp, addr, SEEK_SET);
	    nr = _SC_fread(t, bpi, nitems, fp);

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

	    SFREE(t);};}

    else
       nw = _SC_fwrite_atm(s, bpi, nitems, fp);

/* turn on SIGIO handler */
    SC_catch_io_interrupts(SC_io_interrupts_on);

    return(nw);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FGETS - do an fgets which terminates on \r or \n */

char *SC_fgets(char *s, int n, FILE *fp)
   {int i, nbr, nb;
    off_t pos;
    char *r;
    static int count = 0;

    if (fp == stdin)
       r = fgets(s, n, stdin);

    else
       {pos = ftell(fp);
	nbr = fread(s, 1, n, fp);
	if (nbr < n)
	   s[nbr] = '\0';

/* check for newlines */
	if (nbr > 0)
	   {for (i = 0; (i < n) && (s[i] != '\n') && (s[i] != '\r'); i++);
	    nb = i + 1;
	    if (nb < n)
	       s[nb] = '\0';

	    nb = min(nb, nbr);
	    fseek(fp, pos + nb, SEEK_SET);};
       
	r = (nbr > 0) ? s : NULL;};

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
	   {fid          = FMAKE(file_io_desc, "SC_FOPEN:fid");
	    fid->info    = fp;
	    fid->fopen   = SC_fopen;
	    fid->ftell   = ftell;
	    fid->lftell  = NULL;
	    fid->fseek   = fseek;
	    fid->lfseek  = NULL;
	    fid->fread   = fread;
	    fid->lfread  = NULL;
	    fid->fwrite  = _SC_fwrite;
	    fid->lfwrite = NULL;
	    fid->setvbuf = _SC_setvbuf;
	    fid->fclose  = fclose;
	    fid->fprintf = _SC_fprintf;
	    fid->fputs   = _SC_fputs;
	    fid->fgetc   = fgetc;
	    fid->ungetc  = ungetc;
	    fid->fflush  = _SC_fflush;
	    fid->feof    = feof;
	    fid->fgets   = SC_fgets;
	    fid->pointer = NULL;
	    fid->segsize = NULL;

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

    bf = FMAKE_N(char, nbe, "_SC_TRANSFER_BYTES:bf");

/* read the data looping until all expected bytes come in */
    nbt = nbe;
    for (ps = bf; TRUE; ps += nbr)
        {nbr  = SC_read_sigsafe(cfd, ps, nbt);
	 nbt -= nbr;
	 if (nbt <= 0)
	    break;};

    nbw = _SC_fwrite(bf, 1, nbe, fp);

    SFREE(bf);

    return(nbw);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_GET_CMD_RESP - wait for the response from the server */

static off_t _SC_get_cmd_resp(PROCESS *pp, char *msg)
   {int lm;
    off_t rv;
    char s[MAXLINE];

    lm = strlen(msg);

    rv = FALSE;

    while (SC_gets(s, MAXLINE, pp) != NULL)
       {if (strncmp(s, msg, lm) == 0)
           {rv = SC_STOADD(s+lm+1);
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
        fp = file[indx];

        if (_SC_debug)
           {fprintf(_SC_diag, "Doing: %c on file %d(%p)\n", code, indx, fp);
            fflush(_SC_diag);};

        switch (code)
           {case SC_FOPEN :
                 {int i, status, ok;
                  char *name, *mode;
                  off_t len;

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

		  if (ok == FALSE);
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
			   bf   = FMAKE_N(char, size,
                                  "SC_FILE_ACCESS:bf");
			   break;
		      case 2 :
			   type = _IOLBF;
			   bf   = FMAKE_N(char, size,
                                  "SC_FILE_ACCESS:bf");
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
                 {off_t addr;
                  int offset, whence;

                  addr   = SC_STOADD(SC_strtok(s+2, ",\n", t));
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
                  bf  = FMAKE_N(char, nbi*ni,
                        "SC_FILE_ACCESS:bf");

                  nir = SC_fread_sigsafe(bf, nbi, ni, fp);
                  REPLY(READ_MSG, nir);

                  nbw = SC_write_sigsafe(cfd, bf, nbi*nir);
                  REPLY(READ_MSG, nbw);

                  SFREE(bf);};

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

                  bf = FMAKE_N(char, nc+1,
                               "SC_FILE_ACCESS:bf");
                  memset(bf, '\n', nc+1);
                  ret = SC_fgets(bf, nc, fp);
                  printf("%s", bf);
                  SFREE(bf);

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

    SFREE(fp);

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
	SFREE(msg);};

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
    off_t len;
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
       _SC.hosts = SC_make_hasharr(HSZSMALL, FALSE, SC_HA_NAME_KEY);

    strcpy(host, "local");
    strcpy(fname, name);
    type = SC_LOCAL;

    fp = FMAKE(REMOTE_FILE, "SC_ROPEN:fp");
    fp->type = type;

    if (type == SC_LOCAL)
       {fp->pp = (PROCESS *) SC_fopen(name, mode);
	if (!SC_process_alive(fp->pp))
           {SFREE(fp);};}

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
               {SFREE(fp);
                return(NULL);};

            SC_block(pp);
            SC_hasharr_install(_SC.hosts, host, pp, "PROCESS", TRUE, TRUE);

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
                SFREE(fp);}
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
	    SFREE(fp);};};

    if (fp != NULL)
       {fid          = FMAKE(file_io_desc, "_SC_ROPEN:fid");
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
	fid->feof    = NULL;
	fid->fgets   = _SC_rgets;
	fid->pointer = NULL;
	fid->segsize = NULL;

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

    ok = SC_haelem_data(hp, NULL, NULL, (void **) &pp);

    ret = TRUE;

/* send the SC_FEXIT command to the server */
    if (SC_process_alive(pp))
       {SC_printf(pp, "%c%c\n", SC_FEXIT, 'A');
	SFREE(pp);};

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

static off_t _SC_lftell(FILE *fp)
   {off_t rv;

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

static int _SC_lfseek(FILE *fp, off_t offs, int whence)
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

static BIGUINT _SC_lfread(void *s, size_t bpi, BIGUINT ni, FILE *fp)
   {BIGUINT zc, n, ns, nr;
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

static BIGUINT _SC_lfwrite(void *s, size_t bpi, BIGUINT ni, FILE *fp)
   {BIGUINT zc, n, ns, nw;
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
	   {fid          = FMAKE(file_io_desc, "SC_LFOPEN:fid");
	    fid->info    = fp;
	    fid->fopen   = SC_lfopen;
	    fid->ftell   = NULL;
	    fid->lftell  = _SC_lftell;
	    fid->fseek   = NULL;
	    fid->lfseek  = _SC_lfseek;
	    fid->fread   = NULL;
	    fid->lfread  = _SC_lfread;
	    fid->fwrite  = NULL;
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
	    fid->pointer = NULL;
	    fid->segsize = NULL;

	    ret = (FILE *) fid;}
	else
	   ret = NULL;};

    return(ret);}

/*--------------------------------------------------------------------------*/

/*                               IO WRAPPERS                                */

/*--------------------------------------------------------------------------*/

/* IO_SETVBUF - file IO wrapper for SETVBUF method */

int io_setvbuf(FILE *fp, char *bf, int type, size_t sz)
   {int rv;
    file_io_desc *fid;

    rv = -1;

    if (fp != NULL)
       {if (IS_STD_IO(fp))
	   rv = _SC_setvbuf(fp, bf, type, sz);

        else
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    if (fid->setvbuf != NULL)
	       rv = (*fid->setvbuf)(fp, bf, type, sz);};};

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
	       rv = (*fid->ftell)(fp);

	    else if (fid->lftell != NULL)
	       rv = (*fid->lftell)(fp);};};

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
	       rv = (*fid->fseek)(fp, offs, whence);

	    else if (fid->lfseek != NULL)
	       rv = (*fid->lfseek)(fp, (off_t) offs, whence);};};

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
	       rv = (*fid->fread)(p, sz, ni, fp);

	    else if (fid->lfread != NULL)
	       rv = (*fid->lfread)(p, sz, (BIGUINT) ni, fp);};};

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
	       rv = (*fid->fwrite)(p, sz, ni, fp);

	    else if (fid->lfwrite != NULL)
	       rv = (*fid->lfwrite)(p, sz, (BIGUINT) ni, fp);};};

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

	    if (fid->fclose != NULL)
	       {rv = (*fid->fclose)(fp);
		fid->fclose = NULL;};

	    SFREE(fid);};};

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

	    SFREE(s);}
    
        else
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    if (fid->fprintf != NULL)
	       {SC_VA_START(fmt);
		rv = (*fid->fprintf)(fp, fmt, __a__);
		SC_VA_END;};};};

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

	    if (fid->fputs != NULL)
	       rv = (*fid->fputs)(s, fp);};};

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

	    if (fid->fgetc != NULL)
	       rv = (*fid->fgetc)(fp);};};

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

	    if (fid->ungetc != NULL)
	       rv = (*fid->ungetc)(c, fp);};};

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

	    if (fid->fflush != NULL)
	       rv = (*fid->fflush)(fp);};};

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

	    if (fid->fgets != NULL)
	       rv = (*fid->fgets)(s, n, fp);};};

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

	    if (fid->feof != NULL)
	       rv = (*fid->feof)(fp);};};

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

	    if (fid->pointer != NULL)
	       rv = (*fid->pointer)(fp);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IO_SEGSIZE - file IO wrapper for SEGSIZE method */

BIGUINT io_segsize(void *a, BIGINT n)
   {BIGUINT rv;
    file_io_desc *fid;
    FILE *fp;

    fp = (FILE *) a;
    rv = 0;
    if (fp != NULL)
       {if (!IS_STD_IO(fp))
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    if (fid->segsize != NULL)
	       rv = (*fid->segsize)(fp, n);};};

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                               LIO WRAPPERS                               */

/*--------------------------------------------------------------------------*/

/* LIO_SETVBUF - large file IO wrapper for SETVBUF method */

int lio_setvbuf(FILE *fp, char *bf, int type, size_t sz)
   {int rv;
    file_io_desc *fid;

    rv = -1;
    if (fp != NULL)
       {if (IS_STD_IO(fp))
	   rv = _SC_setvbuf(fp, bf, type, sz);

        else
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    if (fid->setvbuf != NULL)
	       rv = (*fid->setvbuf)(fp, bf, type, sz);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LIO_TELL - large file IO wrapper for FTELL method */

off_t lio_tell(FILE *fp)
   {off_t rv;
    file_io_desc *fid;

    rv = -1;
    if (fp != NULL)
       {if (IS_STD_IO(fp))
	   rv = _SC_lftell(fp);
    
        else
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    if (fid->lftell != NULL)
	       rv = (*fid->lftell)(fp);

	    else if (fid->ftell != NULL)
	       rv = (*fid->ftell)(fp);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LIO_SEEK - large file IO wrapper for FSEEK method */

int lio_seek(FILE *fp, off_t offs, int whence)
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
	       rv = (*fid->lfseek)(fp, offs, whence);

	    else if (fid->fseek != NULL)
	       rv = (*fid->fseek)(fp, (long) offs, whence);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LIO_READ - large file IO wrapper for FREAD method */

BIGUINT lio_read(void *p, size_t sz, BIGUINT ni, FILE *fp)
   {BIGUINT rv;
    file_io_desc *fid;

    rv = -1;
    if (fp != NULL)
       {if (IS_STD_IO(fp))
	   rv = SC_fread_sigsafe(p, sz, ni, fp);
    
        else
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    if (fid->lfread != NULL)
	       rv = (*fid->lfread)(p, sz, ni, fp);

	    else if (fid->fread != NULL)
	       rv = (*fid->fread)(p, sz, (size_t) ni, fp);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LIO_WRITE - large file IO wrapper for FWRITE method */

BIGUINT lio_write(void *p, size_t sz, BIGUINT ni, FILE *fp)
   {BIGUINT rv;
    file_io_desc *fid;

    rv = -1;
    if (fp != NULL)
       {if (IS_STD_IO(fp))
	   rv = _SC_fwrite_atm(p, sz, ni, fp);
    
        else
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    if (fid->lfwrite != NULL)
	       rv = (*fid->lfwrite)(p, sz, ni, fp);

	    else if (fid->fwrite != NULL)
	       rv = (*fid->fwrite)(p, sz, (size_t) ni, fp);};};

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

	    if (fid->fclose != NULL)
	       {rv = (*fid->fclose)(fp);
		fid->fclose = NULL;};

	    SFREE(fid);};};

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

	    SFREE(s);}
    
        else
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    if (fid->fprintf != NULL)
	       {SC_VA_START(fmt);
		rv = (*fid->fprintf)(fp, fmt, __a__);
		SC_VA_END;};};};

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

	    if (fid->fputs != NULL)
	       rv = (*fid->fputs)(s, fp);};};

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

	    if (fid->fgetc != NULL)
	       rv = (*fid->fgetc)(fp);};};

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

	    if (fid->ungetc != NULL)
	       rv = (*fid->ungetc)(c, fp);};};

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

	    if (fid->fflush != NULL)
	       rv = (*fid->fflush)(fp);};};

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

	    if (fid->fgets != NULL)
	       rv = (*fid->fgets)(s, n, fp);};};

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

	    if (fid->feof != NULL)
	       rv = (*fid->feof)(fp);};};

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

	    if (fid->pointer != NULL)
	       rv = (*fid->pointer)(fp);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LIO_SEGSIZE - large file IO wrapper for SEGSIZE method */

BIGUINT lio_segsize(void *a, BIGINT n)
   {BIGUINT rv;
    file_io_desc *fid;
    FILE *fp;

    fp = (FILE *) a;
    rv = 0;
    if (fp != NULL)
       {if (!IS_STD_IO(fp))
	   {fid = (file_io_desc *) fp;
	    fp  = fid->info;

	    if (fid->segsize != NULL)
	       rv = (*fid->segsize)(fp, n);};};

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

    fid          = FMAKE(file_io_desc, "SC_STD_FILE:fid");
    fid->info    = p;
    fid->fopen   = SC_fopen;

    fid->ftell   = ftell;
    fid->lftell  = NULL;
    fid->fseek   = fseek;
    fid->lfseek  = NULL;
    fid->fread   = fread;
    fid->lfread  = NULL;
    fid->fwrite  = _SC_fwrite;
    fid->lfwrite = NULL;

    fid->setvbuf = _SC_setvbuf;
    fid->fclose  = fclose;
    fid->fprintf = _SC_fprintf;
    fid->fputs   = _SC_fputs;
    fid->fgetc   = fgetc;
    fid->ungetc  = ungetc;
    fid->fflush  = _SC_fflush;
    fid->feof    = feof;
    fid->fgets   = SC_fgets;
    fid->pointer = NULL;
    fid->segsize = NULL;

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
 */

BIGINT SC_file_length(char *name)
   {BIGINT ln;
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

BIGINT SC_file_size(FILE *fp)
   {BIGINT ad, ln;

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

off_t SC_filelen(FILE *fp)
   {off_t caddr, flen;

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
   {ssize_t rv;

#ifdef AIX
    PFSignal_handler oh;

/* turn off SIGIO handler */
    if (SC_io_interrupts_on == TRUE)
       oh = SC_signal_action(SC_SIGIO, SIG_IGN, 0, -1);

    rv = read(fd, bf, n);

/* turn on SIGIO handler */
    if (SC_io_interrupts_on == TRUE)
       SC_signal_action(SC_SIGIO, oh, 0, BLOCK_WITH_SIGIO, -1);
 
#else

    rv = read(fd, bf, n);

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

static size_t _SC_fread_safe(void *s, size_t bpi, size_t nitems, FILE *fp)
   {size_t zc, n, ns, nr;
    char *ps;

    zc = 0;
    ns = nitems;
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

size_t SC_fread_sigsafe(void *s, size_t bpi, size_t nitems, FILE *fp)
   {size_t rv;

#ifdef AIX
    PFSignal_handler oh;

/* turn off SIGIO handler */
    if (SC_io_interrupts_on == TRUE)
       oh = SC_signal_action(SC_SIGIO, SIG_IGN, 0, -1);

    rv = _SC_fread_safe(s, bpi, nitems, fp);

/* turn on SIGIO handler */
    if (SC_io_interrupts_on == TRUE)
       SC_signal_action(SC_SIGIO, oh, 0, BLOCK_WITH_SIGIO, -1);
 
#else

    rv = _SC_fread_safe(s, bpi, nitems, fp);

#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FWRITE_SIGSAFE - make fwrite safe for signals
 *                   - and to ensure that the requested number of bytes
 *                   - is written if at all possible
 *                   - return number of bytes written or -1
 */

size_t SC_fwrite_sigsafe(void *s, size_t bpi, size_t nitems, FILE *fp)
   {size_t zc, n, ns, nw;
    char *ps;

    zc = 0;
    ns = nitems;
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

