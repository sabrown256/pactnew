/*
 * PDBNET.C - routines to run over networks (special stuff)
 *
 * Routines and Usage:
 *
 *   PN_target   - returns a structure chart for the target machine
 *               - given the apropriate data_standard and data_alignment
 *               - used either to target remote machine as source or sink
 *               - (see PN_conv_in and PN_conv_out)
 *
 *   PN_open     - open a PDBfile which encapsulates a memory buffer
 *               - into and out of which all IO goes
 *
 *   PN_close    - close a PDBfile which was opened by PN_open
 *
 *   PN_defstr   - defines data structures to the two relevant structure
 *               - charts (host chart and target chart)
 *
 *   PN_conv_in  - convert the data in the input buffer which is in the
 *               - format of the remote machine (from which it has been read)
 *               - to the format of the host machine and place it in the
 *               - output buffer
 *
 *   PN_conv_out - convert the data in the output buffer which is in the
 *               - format of the host machine (from which it will be written)
 *               - to the format of the remote machine and place it in the
 *               - output buffer
 *
 *   PN_sizeof   - return the size of an item of the specified type
 *               - on the target machine (this is a macro)
 *
 *  These routines do NO network communications, they are provided so that
 *  PDBLib's data conversion and structured data handling facilities can
 *  be integrated with any network communication software.
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"

#define MEM(x)   ((x)->addr.memaddr)
#define DISK(x)  ((x)->addr.mdiskaddr)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PN_BSETVBUF - set the pseudo file buffer
 *              - return 0 on success
 */

static int _PN_bsetvbuf(FILE *stream, char *bf, int type, size_t size)
   {int ret;
    BF_FILE *fb;

    fb = _PD_GET_FILE_PTR(stream);
    SC_ASSERT(fb != NULL);

    ret = 0;

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PN_BCLOSE - close the pseudo file */

static int _PN_bclose(FILE *stream)
   {int ret;
    BF_FILE *fb;
static int count=0;
count++;

    fb = _PD_GET_FILE_PTR(stream);

    ret = 0;
    if (fb == NULL)
       ret = EOF;

    _PD_REMOVE_FILE(stream);

    CFREE(fb);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PN_BFLUSH - do an fflush on the pseudo file */

static int _PN_bflush(FILE *stream)
   {int ret;
    BF_FILE *fb;

    fb = _PD_GET_FILE_PTR(stream);

    ret = FALSE;
    if (fb == NULL)
       ret = TRUE;

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PN_BTELL - do an ftell on the pseudo file */

static int64_t _PN_btell(FILE *stream)
   {BF_FILE *fb;
    int64_t addr;

    fb = _PD_GET_FILE_PTR(stream);

    addr = -1;
    if (fb == NULL)
       addr = -1;

    else
       addr = DISK(fb);

    return(addr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PN_BSEEK - do an fseek on the pseudo file */

static int _PN_bseek(FILE *stream, int64_t addr, int offset)
   {int ret;
    BF_FILE *fb;

    fb = _PD_GET_FILE_PTR(stream);

    ret = 0;
    if (fb == NULL)
       ret = -1;

    else
       {switch (offset)
           {case SEEK_SET :
		 DISK(fb) = addr;
                 break;

            case SEEK_CUR :
		 DISK(fb) += addr;
                 break;

            case SEEK_END :
		 DISK(fb) = fb->bf.mdiskaddr + fb->length + addr;
                 break;

            default :
	         ret = -1;
	         break;};};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PN_BREAD - do an fread on the pseudo file */

static u_int64_t _PN_bread(void *s, size_t nbi, u_int64_t ni, FILE *stream)
   {BF_FILE *fb;
    size_t nbw, nr;

    fb = _PD_GET_FILE_PTR(stream);

    nbw = nbi*ni;
    if (fb == NULL)
       nbw = 0;

    else
       {nr  = fb->length - (fb->addr.memaddr - fb->bf.memaddr);
	nbw = min(nbw, nr);

	memcpy(s, MEM(fb), nbw);

/* adjust the current address */
        DISK(fb) += nbw;};

    return(ni);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PN_BWRITE - do an fwrite on the pseudo file */

static u_int64_t _PN_bwrite(const void *s, size_t nbi, u_int64_t ni,
			    FILE *stream)
   {long nbw;
    void *d;
    BF_FILE *fb;

    fb = _PD_GET_FILE_PTR(stream);

    nbw = nbi*ni;
    if ((fb == NULL) || (nbi*ni == 0))
       nbw = 0;

    else
       {d = MEM(fb);
	memcpy(d, s, nbw);

/* adjust the current address */
        DISK(fb) += nbw;};

    return(ni);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PN_BPRINTF - do an fprintf on the pseudo file */

static int _PN_bprintf(FILE *fp, const char *fmt, va_list a)
   {inti ni, nw;
    char *bf;

    nw = 0;

    if (fp != NULL)
       {bf = SC_vdsnprintf(FALSE, fmt, a);
	ni = strlen(bf);
	nw = _PN_bwrite(bf, 1, ni, fp);};

    return((int) nw);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PN_BPUTS - do an fputs on the pseudo file */

static int _PN_bputs(const char *s, FILE *stream)
   {int nc;
    long nbw;
    BF_FILE *fb;

    fb = _PD_GET_FILE_PTR(stream);

    if (fb == NULL)
       nbw = 0;

    else
       {nc  = strlen(s);
	nbw = _PN_bwrite((char *) s, 1, nc, stream);};

    return(nbw);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PN_BGETS - do an fgets on the pseudo file */

static char *_PN_bgets(char *s, int nc, FILE *stream)
   {int ns;
    char *pb, *ps;
    BF_FILE *fb;

    fb = _PD_GET_FILE_PTR(stream);

    ps = NULL;
    if (fb == NULL)
       ps = NULL;

    else
       {pb = strchr(MEM(fb), '\n');
	if (pb != NULL)
	   {ns = pb - MEM(fb) + 1;
	    ns = min(nc, ns);
	    memcpy(s, MEM(fb), ns);
	    s[ns] = '\0';

/* adjust the local idea of the current disk address */
	    DISK(fb) += ns;};};

    return(ps);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PN_BGETC - do an fgetc on the pseudo file */

static int _PN_bgetc(FILE *stream)
   {int c;
    BF_FILE *fb;

    fb = _PD_GET_FILE_PTR(stream);

    if (fb == NULL)
       c = EOF;

    else
       c = *MEM(fb)++;

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PN_BUNGETC - do an fungetc on the pseudo file */

static int _PN_bungetc(int c, FILE *stream)
   {BF_FILE *fb;

    fb = _PD_GET_FILE_PTR(stream);

    if (fb == NULL)
       c = 0;

    else
       MEM(fb)--;

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PN_BOPEN - prepare the pseudo file */

static FILE *_PN_bopen(const char *name, const char *mode)
   {char *nm;
    BF_FILE *fb;
    FILE *rf, *fp;
    file_io_desc *fid;
    extern FILE *_PD_popen(const char *name, const char *mode);

    fb = CMAKE(BF_FILE);

    nm = (char *) name;

    fb->length       = SC_arrlen(name);
    fb->bf.memaddr   = nm;
    fb->addr.memaddr = nm;

    SC_mark(nm, 1);

    rf = _PD_SETUP_PSEUDO_FILE(fb);

    fid          = SC_make_io_desc(rf);
    fid->fopen   = _PN_bopen;

    fid->lftell  = _PN_btell;
    fid->lfseek  = _PN_bseek;
    fid->lfread  = _PN_bread;
    fid->lfwrite = _PN_bwrite;

    fid->setvbuf = _PN_bsetvbuf;
    fid->fclose  = _PN_bclose;
    fid->fprintf = _PN_bprintf;
    fid->fputs   = _PN_bputs;
    fid->fgetc   = _PN_bgetc;
    fid->ungetc  = _PN_bungetc;
    fid->fflush  = _PN_bflush;
    fid->fgets   = _PN_bgets;

    fp = (FILE *) fid;

    return(fp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PN_SAVE_HOOKS - save the current pio_hooks */

static void _PN_save_hooks(void)
   {PD_smp_state *pa;

    pa = _PD_get_state(-1);

    pa->pn_open_save = PD_gs.par.open_hook;

    PD_gs.par.open_hook = (PFfopen) _PN_bopen;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PN_RESTORE_HOOKS - restore the pio_hooks */

static void _PN_restore_hooks(void)
   {PD_smp_state *pa;

    pa = _PD_get_state(-1);

    PD_gs.par.open_hook = pa->pn_open_save;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PN_SIZEOF - sizeof operator for PDBNet */

static int _PN_sizeof(const char *s)
   {int rv;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    if (pa->host_chart != NULL)
       rv = _PD_lookup_size(s, pa->host_chart);

    else
       rv = SC_sizeof(s);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PN_CONV_IN - Convert the data IN after input from a remote host and
 *            - place the results in OUT.
 *            - The space OUT must be large enough to receive that converted
 *            - data.
 *            - The data IN has NI items of type, TYPE as defined by
 *            - the structure chart CHI.
 *
 * #bind PN_conv_in fortran() scheme() python()
 */

void PN_conv_in(void *out, void *in, const char *type, int64_t ni,
		hasharr *chi)
   {data_standard *istd;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    switch (SETJMP(pa->trace_err))
       {case ABORT :
	     return;
        case ERR_FREE :
	     return;
        default :
	     memset(pa->err, 0, MAXLINE);
	     break;};

    istd = (data_standard *) SC_hasharr_def_lookup(chi, "standard");

    PD_convert((char **) &out, (char **) &in, type, type, ni,
               istd, pa->host_std, pa->host_std,
               chi, pa->host_chart, 0, PD_TRACE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PN_CONV_OUT - Convert the data IN before output to a remote host
 *             - with different architecture and place the results in OUT.
 *             - The space OUT must be large enough to receive that converted
 *             - data.
 *             - The data IN has NI of type, TYPE, as defined by
 *             - the structure chart CHO.
 *
 * #bind PN_conv_out fortran() scheme() python()
 */

void PN_conv_out(void *out, void *in, const char *type, int64_t ni,
		 hasharr *cho)
   {data_standard *ostd;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    switch (SETJMP(pa->trace_err))
       {case ABORT :
	     return;
        case ERR_FREE :
	     return;
        default :
	     memset(pa->err, 0, MAXLINE);
	     break;};

    ostd = (data_standard *) SC_hasharr_def_lookup(cho, "standard");

    PD_convert((char **) &out, (char **) &in, type, type, ni,
               pa->host_std, ostd, pa->host_std,
               pa->host_chart, cho, 0, PD_TRACE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PN_TARGET - Make a new structure chart for the associated data
 *           - standard, STD, and data alignment, ALIGN, to be used
 *           - by PDBLib for network rather than file I/O.
 *           - Return the new chart if successful and return NULL otherwise.
 *
 * #bind PN_target fortran() scheme() python()
 */

hasharr *PN_target(data_standard *std, data_alignment *align)
   {hasharr *chart;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

/* one time initializations */
    _PD_init_consts();

/* initialize the host chart the first time only */
    if (pa->host_std == NULL)
       {pa->host_std   = _PD_copy_standard(pa->int_std);
	pa->host_align = _PD_copy_alignment(pa->int_align);
        pa->host_chart = PN_target(pa->host_std, pa->host_align);};

/* initialize the chart */
    chart  = SC_make_hasharr(1, NODOC, SC_HA_NAME_KEY, 0);

    _PD_setup_chart(chart, std, pa->host_std, align, pa->host_align,
		    PD_CHART_FILE, TRUE);

/* special hack for jollies and to save typing */
    SC_hasharr_install(chart, "standard", std, G_DATA_STANDARD_S, 3, -1);
    SC_hasharr_install(chart, "alignment", align, G_DATA_ALIGNMENT_S, 3, -1);

    return(chart);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PN_DEFSTR - Define an new derived type NAME for structure chart, CHART.
 *           - Use the data alignment, ALIGN, and the default offset, DEFOFF
 *           - to completely define the new type.
 *           - The remaining arguments are strings each of which
 *           - specifies a member of the struct.
 *           - The list is terminated by the special token LAST.
 *           - Unknown member types result in failure to define a new type.
 *           - Return the new defstr if successful otherwise return NULL.
 *           - The syntax of a member is the same as that of PD_defstr.
 *
 * #bind PN_defstr fortran() scheme() python()
 */

defstr *PN_defstr(hasharr *chart, const char *name, data_alignment *align,
                  int defoff, ...)
   {char *nxt, *ptype, *type;
    defstr *dp;
    memdes *desc, *lst, *prev;
    PDBfile *file;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    SC_gs.size = _PN_sizeof;

    SC_VA_START(defoff);

    prev   = NULL;
    lst    = NULL;
    for (nxt = SC_VA_ARG(char *); (int) *nxt != 0;
         nxt = SC_VA_ARG(char *))
        {desc  = _PD_mk_descriptor(nxt, defoff);
         type  = CSTRSAVE(nxt);
         ptype = SC_firsttok(type, " \n");
         if (SC_hasharr_lookup(chart, ptype) == NULL)
            if ((ptype == NULL) ||
		(strcmp(ptype, name) != 0) ||
		(_PD_indirection(nxt) == FALSE))
               {snprintf(pa->err, MAXLINE,
                         "ERROR: %s BAD MEMBER TYPE - PN_DEFSTR\n",
                         nxt);
                return(NULL);};
         CFREE(type);
         if (lst == NULL)
            lst = desc;
         else
            prev->next = desc;
         prev = desc;};

    SC_VA_END;

/* create a tmp PDBfile */
    file = CMAKE(PDBfile);
    file->chart      = chart;
    file->host_chart = pa->host_chart;
    file->align      = align;
    file->host_align = pa->host_align;

/* install the type in both charts */
    dp = _PD_defstr_inst(file, name, KIND_STRUCT, lst,
			 NO_ORDER, NULL, NULL, PD_CHART_HOST);

/* free tmp PDBfile */
    CFREE(file);
 
    if (dp == NULL)
       PD_error("CAN'T HANDLE PRIMITIVE TYPE - PN_DEFSTR", PD_GENERIC);

    return(dp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PN_OPEN - Create a special pseudo PDBfile FM that is to be used
 *         - for data transmission across networks rather than storage
 *         - in a file.
 *         - BF is a buffer used for data conversion in I/O operations
 *         - using the pseudo PDBfile and must be at least as large as
 *         - the largest data tree to be read or written.
 *         - The io hooks are set appropriately for network I/O.
 *         - Return the PDBfile if successful and return NULL otherwise.
 *
 * #bind PN_open fortran() scheme() python()
 */

PDBfile *PN_open(PDBfile *fm, char *bf)
   {SC_udl *pu;
    PDBfile *file;
    PD_smp_state *pa;
    FILE *fp;

    pa = _PD_get_state(-1);

/* NOTE: shuts up compilers who complain about variables being used before
 * being set - although it cannot really happen
 */
    fp   = NULL;
    file = NULL;

    if (!_PD_IS_SMP_INIT)
       PD_init_threads(0, NULL);

    _PN_save_hooks();

    switch (SETJMP(pa->create_err))
       {case ABORT :
	     lio_close(fp);
	     _PN_restore_hooks();
	     return(NULL);

        case ERR_FREE :
	     return(file);

        default :
	     memset(pa->err, 0, MAXLINE);
	     break;};

    pu = _PD_pio_open(bf, "net");

    if (pu == NULL)
       PD_error("CAN'T CREATE FILE - PN_OPEN", PD_CREATE);

    fp = pu->stream;

/* make the PDBfile */
    file = CMAKE(PDBfile);
    if (file == NULL)
       PD_error("CAN'T ALLOCATE PDBFILE - PN_OPEN", PD_CREATE);

    else

/* copy over stuff from the template file */
       {*file = *fm;

        pu->stream = NULL;

	_SC_rel_udl(pu);

	file->name             = CSTRSAVE("pseudo");
	file->stream           = fp;
	file->mode             = PD_APPEND;
	file->file_mode        = CSTRSAVE(BINARY_MODE_WPLUS);
	file->chrtaddr         = _PD_get_current_address(file, PD_CREATE);
	file->date             = SC_date();
	file->virtual_internal = FALSE;
	file->current_prefix   = NULL;
	file->symtab           = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_NAME_KEY, 0);

	_PD_ptr_init_apl(file);

/* register the file with the parallel file manager */
	_PD_ADD_FILE(file, (int64_t) 0);

/* create a directory to hold the symbol table entries
 * generated for pointees.
 */
	_PD_SET_ADDRESS(file, file->chrtaddr);};

    return(file);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PN_CLOSE - Close the pseudo PDBfile FILE and reset all the io hooks.
 *          - Return TRUE if successful and FALSE otherwise.
 *
 * #bind PN_close fortran() scheme() python()
 */

int PN_close(PDBfile *file)
   {int ret;
    FILE *fp;

    fp  = file->stream;
    ret = TRUE;

    if (lio_close(fp) != 0)
       PD_error("CAN'T CLOSE FILE - PD_CLOSE", PD_CLOSE);

    _PN_restore_hooks();

    _PD_ptr_free_apl(file);

    _PD_clr_table(file->symtab, _PD_ha_rl_syment);

    CFREE(file->current_prefix);
    CFREE(file->date);
    CFREE(file->file_mode);
    CFREE(file->name);
    CFREE(file);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PN_WRITE - Write the data VR of type, TYPE, and number of items NI
 *          - to the pseudo PDBfile FILE.
 *          - Return TRUE if successful and FALSE otherwise.
 *          -
 *          - NOTE: VR must be a pointer to an object with the type
 *          - given by TYPE!!!!
 *
 * #bind PN_write fortran() scheme() python()
 */

int PN_write(PDBfile *file, const char *type, int64_t ni, void *vr)
   {int ret;
    char bf[MAXLINE];

    PD_reset_ptr_list(file);

    snprintf(bf, MAXLINE, "s[%s]", SC_itos(NULL, 0, ni, NULL));

    ret = PD_write(file, bf, type, vr);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PN_READ - Read the data VR of type, TYPE, and number of items NI
 *         - from the pseudo PDBfile FILE.
 *         - Return the number of items successfully read.
 *         -
 *         - NOTE: VR must be a pointer to an object with the type
 *         - given by TYPE (PDBLib will allocated space if necessary)!
 *
 * #bind PN_read fortran() scheme() python()
 */

int PN_read(PDBfile *file, const char *type, int64_t ni, void *vr)
   {int ret;
    int64_t addr;
    char bf[MAXLINE];
    syment *ep;

    PD_reset_ptr_list(file);

    snprintf(bf, MAXLINE, "s");

    addr = _PD_get_current_address(file, PD_GENERIC);
    ep   = _PD_mk_syment(type, ni, addr, NULL, NULL);
    _PD_e_install(file, bf, ep, TRUE);

    ret = PD_read(file, bf, vr);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

