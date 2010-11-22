/*
 * TRLFW.C - write Lanl Link Format files
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "trllf.h"

/* File Layout:
 *    File directory
 *    [Record]*
 *
 * File directory (512 8 byte integers):
 *    header  (5 8 byte integers)
 *    [(time, MCD address)]*
 *
 * Record Layout:
 *    MCD
 *    VLD
 *    [problem VLB]
 *    variable VLB
 *    [generic VLB]*
 *
 * NOTE: the logical structure of the record has
 *         MCD -> VLD -> [VLB]*
 *       because the addresses are kept the actual order
 *       in which they are written to the disk and their
 *       actual location on the disk should not matter
 */

/* #ifdef HAVE_LLF */
#if 0

mcd_header
 headmcd;

char
 namefile[512];   /* name of current link file */

int
 lenfile,		   /* maximum length of file in 64 bit words */
 iuspop,		   /* unit specifier for pop file */
 nrecord,		   /* number of records in current file */
 lenrec,		   /* length of previous record */
 locnow,		   /* location of current record */
 locpop,		   /* location of next write */
 nvarpmc,		   /* number of mesh cell variables */
 nwpmc,			   /* number of words per mesh cell */
 nmcd,			   /* current number of mesh cell data arrays */
 kmax, 	                   /* first mesh dimension */
 lmax,			   /* second mesh dimension */
 nprobv,		   /* number of "problem" variables */
 nvlblks,		   /* current number of VLB's */
 nwdvlbt;		   /* total size of all VLB's including vl directory */

double
 dir[NWDIR],		   /* file directory */
 *blkmcd,                  /* mesh cell data block (nwpmc*lmax*kmax) */
 *varvlb,          	   /* variable VLB (17 + 5*nvarpmc) */
 *probvlb,       	   /* problem VLB  (16 + 5*nprobv) */
 *vldir;         	   /* VL directory (5 + 4*nvlblks) */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WR64ERR - handle write errors */

static void wr64err(int ierr)
   {

    printf("error from wr64 = %6d\n", ierr);
    exit(1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _LB_WR64 - write */

void _LB_wr64(int fd, double *vlb, int nwdvlb, int locpop)
   {int ierr;

    wr64(fd, vlb, nwdvlb, locpop, ierr);
    if (ierr != 0)
       wr64err(ierr);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LB_ADDVLB - add the vlb to the file and enter it into the vl directory
 *           -   VLB     - vlb to add
 *           -   NWDVLB  - size of vlb in 64 bit words
 */

void LB_addvlb(double *vlb, int nwdvlb)
   {int l;

    _LB_wr64(iuspop, vlb, nwdvlb, locpop);

    l = 5 + 4*nvlblks;
    nvlblks++;
    change("vldir", 0);

    vldir[l]   = vlb[0];
    vldir[l+1] = locpop;   /* this is converted to an offset in _LB_write_vld */
    vldir[l+2] = vlb[2];
    vldir[l+3] = vlb[3];

    locpop  += nwdvlb;
    nwdvlbt += nwdvlb;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LB_FILE_INIT - initialize pop file
 *              -    NF     - name of first pop file
 *              -    LENFIL - maximum length of file (64 bit words)
 */

void LB_file_init(char *nf, int lenfil)
   {

    strcpy(namefile, nf);
    lenfile  = lenfil;
    lenrec   = 0;

    allot("dir", 0);
    allot("headmcd", 0);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LB_MCDH - begin a pop time record
 *         -   TIME     - problem time (sh)
 *         -   NCYC     - problem cycle number
 *         -   NMESH    - number of variables per mesh cell
 *         -   KMX      - first mesh dimension
 *         -   LMX      - second mesh dimension
 *         -   CODENAME - code name
 *         -   CODEVERS - code version
 *         -   CODEDATE - code load date "mm/dd/yy"
 *         -   CODETIME - code load time "hh.mm.ss"
 *         -   DUMPDATE - dump date "mm/dd/yy"
 *         -   DUMPTIME - dump time "hh.mm.ss"
 *         -   PROBNAME - problem name
 */

void LB_mcdh(double time, int ncyc, int nmesh, int kmx, int lmx,
	     char *codename, char *codevers, char *codedate,
	     char *codetime, char *dumpdate, char *dumptime,
	     char *probname)
   {int ierr;
    fdir_entry *pe;

    nvarpmc = nmesh;
    nwpmc   = nvarpmc;             /* no data compression at this time */
    nmcd    = 0;
    kmax    = kmx;
    lmax    = lmx;
    nprobv  = 0;
    nvlblks = 0;
    nwdvlbt = 0;

/* set up mesh cell data header */
    headmcd.time        = time;
    headmcd.words_block = NWMCDH;
    headmcd.words_point = nwpmc;
    headmcd.vars_cell   = nvarpmc;

    strncpy(headmcd.consistency, "yes", 3);
    headmcd.n_dimensions = 2;
    headmcd.n_des_words  = 4;

    strncpy(headmcd.dim1, "kmax", 4);
    headmcd.dim1_max   = kmax;
    headmcd.dim1_first = 1;
    headmcd.dim1_last  = kmax;

    strncpy(headmcd.dim2, "lmax", 4);
    headmcd.dim2_max   = lmax;
    headmcd.dim2_first = 1;
    headmcd.dim2_last  = lmax;

    strncpy(headmcd.code_name,    codename, 8);
    strncpy(headmcd.code_version, codevers, 8);
    strncpy(headmcd.code_date,    codedate, 8);
    strncpy(headmcd.code_time,    codetime, 8);
    strncpy(headmcd.dump_date,    dumpdate, 8);
    strncpy(headmcd.dump_time,    dumptime, 8);

    strncpy(headmcd.problem_name, probname, 32);

    headmcd.cycle = ncyc;

/* create space for mesh cell data */
    if (nvarpmc != 0)
       {allot("blkmcd", 0);
	allot("varvlb", 0);};

/* start a new file if necessary */
    if ((lenrec == 0) || (2*(nrecord+1)+4 > NWDIR) ||
        (locpop+lenrec > lenfile))
       {if (lenrec != 0)
	   {rd64cls(iuspop, ierr);
	    if (ierr != 0)
	       wr64err(ierr);
	    basnxtsq(namefile, namefile);};

	freeus(iuspop);
	wr64opn(iuspop, namefile, ierr);
        if (ierr != 0)
	   wr64err(ierr);

        nrecord = 0;
        locpop  = NWDIR;};

    locnow  = locpop;
    locpop += (NWMCDH + nwpmc*lmax*kmax);

    pe = (fdir_entry *) &dir[4 + 2*nrecord];
    pe->time    = time;
    pe->address = locnow;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LB_MCD - setup and reserve space for a mesh cell data array
 *        -    ORGNAME        - orginating code name for variable
 *        -    STDNAME        - standard name for variable
 *        -    ORGUNITS       - orginating code units
 *        -    SCALEFAC       - factor to convert to standard units
 *        -    INTARRAY(k, l) - data array (of any type)
 *        -    TYPE           - "integer", "flt pt", "ascii"
 *        -    IPOINT         - [0, 1] for [zone, point] centering
 */

typedef union u_number number;
union u_number
   {int i[2];
    double r;};

void LB_mcd(char *orgname, char *stdname, char *orgunits,
	    double scalefac, int *intarray, char *type,
	    int ipoint)
   {int i, k, l, kk, ll, itype;
    vlb_var_desc *pd;
    number val;
    double value;
    double *array;

    if (nmcd >= nvarpmc)
       printf("Too many calls to LB_mcd\n");

    l = 17 + 5*nmcd;
    nmcd++;

/* set up the packed word attribute */
    itype = 2;
    if (strcmp(type, "integer") == 0)
       itype = 1;

    if (strcmp(type, "flt pt") == 0)
       itype = 0;

    val.i[0] = ((itype << 10) | (ipoint << 4) | (nmcd >> 5));
    val.i[1] = ((nmcd << 27) | (1 << 18) | (64 << 9) | 15);

#if (WORDSIZE == 64)
    val.i[0] = ((val.i[0] << 32) | val.i[1]);
#endif

/* make entry in "variable" VLB */
    pd = (vlb_var_desc *) &varvlb[l];
    strncpy(pd->codename, orgname, 8);
    strncpy(pd->stdname,  stdname, 8);
    strncpy(pd->units,    orgunits, 8);
    pd->value = val.r;
    pd->scale = scalefac;

/* move mesh cell data array into blkmcd (transpose and convert to 64 bits) */
    array = (double *) intarray;

    for (k = 0; k < kmax; k++)
        {kk = min(kmax, k+1-ipoint);
	 for (l = 0; l < lmax; l++)
	     {ll = min(lmax, l+1-ipoint);
	      i  = ll*kmax + kk;

#if (WORDSIZE == 64)
	      value = array[i];
#endif
	      if (strncmp(type, "integer", 7) == 0)
		 value = intarray[i];
	      else if (strncmp(type, "flt pt", 6) == 0)
		 value = array[i];
	      else
		 value = array[i];};

	 if ((ipoint == 0) && ((k == kmax) || (l == lmax)))
	    value = 0.0;

/*	 blkmcd(nmcd, l, k) = value; */};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LB_PROB - setup problem (scaler real8) variable
 *         -    ORGNAME   - orginating code name for variable
 *         -    STDNAME   - standard name for variable
 *         -    ORGUNITS  - orginating code units
 *         -    SCALEFAC  - factor to convert to standard units
 *         -    VALUE     - data word
 */

void LB_prob(char *orgname, char *stdname, char *orgunits,
	     double scalefac, double value)
   {int l;
    vlb_prb_desc *pd;

    l = 16 + 5*nprobv;
    nprobv++;
    change("probvlb", 0);

    pd = (vlb_prb_desc *) &probvlb[l];

    strncpy(pd->codename, orgname,  8);
    strncpy(pd->stdname,  stdname,  8);
    strncpy(pd->units,    orgunits, 8);
    pd->scale = scalefac;
    pd->value = value;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LB_VLB - write variable length data block containing one array
 *        -    ORGNAME       - orginating code name for variable
 *        -    STDNAME       - standard name for variable
 *        -    ORGUNITS      - orginating code units
 *        -    SCALEFAC      - factor to convert to standard units
 *        -    INTARRAY(*)   - data array (of any type and dimension)
 *        -    TYPE          - "integer", "flt pt", "ascii"
 *        -    NDIM          - number of dimensions in intarray
 *        -    ORGNAMD(ndim) - orginating code name for dimension
 *        -    STDNAMD(ndim) - standard name for dimension
 *        -    IDIMS(ndim)   - starting index for dimension
 *        -    IDIME(ndim)   - ending index for dimension
 */

void LB_vlb(char *orgname, char *stdname, char *orgunits,
	    double scalefac, int *intarray, char *type, int ndim,
	    char *orgnamd, char *stdnamd, int *idims, int *idime)
   {int i, l, ix, idim, len;
    int ndwdpa, nwdvlbh, nwdpdim, nwdvlb;
    int *maxdim, *lendim, *jdim;
    double *buf, *array;
    vlb_header *ph;
    vlb_dim *pd;
    vlb_desc *pv;

/* compute length and get buffer */
    maxdim = FMAKE_N(int, ndim, "LB_VLB:maxdim");
    lendim = FMAKE_N(int, ndim, "LB_VLB:lendim");
    jdim   = FMAKE_N(int, ndim, "LB_VLB:jdim");

    len = 1;
    for (idim = 0; idim < ndim; idim++)
        {maxdim[idim] = max(0, idime[idim]-idims[idim]+1);
	 lendim[idim] = len;
	 len          = len*maxdim[idim];
	 jdim[idim]   = 1;};

    ndwdpa  = 2;
    nwdvlbh = 8+ndwdpa;
    nwdpdim = 6;
    nwdvlb  = nwdvlbh + nwdpdim*ndim + (2+ndwdpa+len);

    buf = MAKE_N(double, nwdvlb);

/* setup header, etc. */
    ph = (vlb_header *) buf;
    strncpy(ph->name, stdname, 8);
    strncpy(ph->type, type, 8);
    strncpy(ph->units, "org unit", 8);
    strncpy(ph->scale, "scal fac", 8);
    ph->header_size = nwdvlbh;
    ph->n_words     = nwdvlb;
    ph->n_arrays    = 1;
    ph->n_dims      = ndim;
    ph->n_wpd_dims  = nwdpdim;
    ph->n_wpd_array = ndwdpa;

    l = nwdvlbh;

/* setup dimensions */
    for (idim = 0; idim < ndim; idim++, l += 6)
        {pd = (vlb_dim *) &buf[l];

	 strncpy(pd->codename, orgnamd + 8*idim, 8);
	 strncpy(pd->stdname,  stdnamd + 8*idim, 8);
	 strncpy(pd->desc,     "no",             2);

	 pd->max   = maxdim[idim];
	 pd->start = idims[idim];
	 pd->stop  = idime[idim];};

/* setup descriptor */
    pv = (vlb_desc *) &buf[l];
    strncpy(pv->codename, orgname,  8);
    strncpy(pv->stdname,  stdname,  8);
    strncpy(pv->units,    orgunits, 8);
    pv->scale = scalefac;

    l += (2 + ndwdpa);

/* move array into buffer (transpose & convert to 64 bit cray binary) */
    array = (double *) intarray;

    ix = 1;
    for (i = 0; i < len; i++)
        {l++;

#if (WORDSIZE == 64)
	 buf[l] = array[ix];
#else
	 if (strncmp(type, "integer", 7) == 0)
	    buf[l] = intarray[ix];
	 else if (strncmp(type, "flt pt", 6) == 0)
	    buf[l] = array[ix];
	 else
	    buf[l] = array[ix];
#endif

	 for (idim = ndim-1 ; idim >= 0; idim--)
	     {jdim[idim]++;
	      ix += lendim[idim];
	      if (jdim[idim] <= maxdim[idim])
		 break;

	      jdim[idim] = 1;
	      ix -= lendim[idim]*maxdim[idim];};};

    LB_addvlb(buf, nwdvlb);

    SFREE(maxdim);
    SFREE(lendim);
    SFREE(jdim);
    SFREE(buf);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _LB_WRITE_PROBLEM - write out the special VLB "problem" */

static void _LB_write_problem(void)
   {int l, len, ndwdpa, nwdvlbh, nwdpdim, nwdvlb;
    vlb_header *ph;
    vlb_dim *pd;

    ndwdpa  = 2;
    nwdvlbh = 8 + ndwdpa;
    nwdpdim = 6;
    len     = 1;
    nwdvlb  = nwdvlbh + nwdpdim + nprobv*(2+ndwdpa+len);

/* fill the header */
    ph = (vlb_header *) probvlb;

    strncpy(ph->name, "problem", 7);
    strncpy(ph->type, "flt pt",  6);
    strncpy(ph->units, "org unit", 8);
    strncpy(ph->scale, "scal fac", 8);

    ph->header_size = nwdvlbh;
    ph->n_words     = nwdvlb;
    ph->n_arrays    = nprobv;
    ph->n_dims      = 1;
    ph->n_wpd_dims  = nwdpdim;
    ph->n_wpd_array = ndwdpa;

    l = nwdvlbh;

/* fill the dimension descriptor */
    pd = (vlb_dim *) &probvlb[l];

    strncpy(pd->codename, "        ", 8);
    strncpy(pd->stdname,  "        ", 8);
    strncpy(pd->desc,     "no",       2);

    pd->max   = 1;
    pd->start = 1;
    pd->stop  = 1;

    LB_addvlb(probvlb, nwdvlb);

    return;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _LB_WRITE_VARIABLE - write out the special VLB "variable" */

static void _LB_write_variable(void)
   {int l, nwdvlbh, nwdpdim, ndim1, nwdvlb, nwlabls;
    char *ps;
    vlb_header *ph;
    vlb_dim *pd;

    if (nvarpmc != 0)
       {if (nmcd < nvarpmc)
	   printf("Not enough calls to LB_mcd\n");
	exit(0);};

    nwdvlbh = 8;
    nwdpdim = 6;
    nwlabls = 3;
    ndim1   = 3;
    nwdvlb  = nwdvlbh + nwdpdim + nwlabls + nvarpmc*(2+ndim1);

/* fill the header */
    ph = (vlb_header *) varvlb;

    strncpy(ph->name,  "variable", 8);
    strncpy(ph->type,  "mixed",    5);

    ph->header_size = nwdvlbh;
    ph->n_words     = nwdvlb;
    ph->n_arrays    = nvarpmc;
    ph->n_dims      = 1;
    ph->n_wpd_dims  = nwdpdim;
    ph->n_wpd_array = 0;

    l = nwdvlbh;

/* fill the dimension descriptor */
    pd = (vlb_dim *) &varvlb[l];

    strncpy(pd->codename, "        ", 8);
    strncpy(pd->stdname,  "        ", 8);
    strncpy(pd->desc,     "yes",      3);

    pd->max   = ndim1;
    pd->start = 1;
    pd->stop  = ndim1;

    l += nwdpdim;

    ps = (char *) &varvlb[l];
    strncpy(ps,      "wd attrb", 8);
    strncpy(ps + 8,  "org unit", 8);
    strncpy(ps + 16, "scal fac", 8);

    l += nwlabls;

    LB_addvlb(varvlb, nwdvlb);

    _LB_wr64(iuspop, blkmcd, nwpmc*lmax*kmax, locnow+NWMCDH);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _LB_WRITE_VLD - write out the variable length directory for this record */

static void _LB_write_vld(int *plocvldir)
   {int i, l, nwdvldh, nwdvldir, locvldir;
    vld_header *ph;
    vld_entry *pe;

    nwdvldh  = 5;
    nwdvldir = nwdvldh + 4*nvlblks;

    ph = (vld_header *) vldir;
    strncpy(ph->name, "vl dir", 6);
    ph->header_size = nwdvldh;
    ph->dir_size    = nwdvldir;
    ph->n_vlb       = nvlblks;
    ph->n_words     = nwdvlbt;

    l = 5;

    locvldir = locpop;
    for (i = 0; i < nvlblks; i++, l += 4)
        {pe = (vld_entry *) &vldir[l];
	 pe->address -= locvldir;};

    _LB_wr64(iuspop, vldir, nwdvldir, locvldir);

    locpop += nwdvldir;

    *plocvldir = locvldir;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _LB_WRITE_MCD - write the MCD header */

static void _LB_write_mcd(int locvldir)
   {

    if (locpop - locnow != lenrec)
       strncpy(headmcd.consistency, "no", 2);

    lenrec           = locpop - locnow;
    headmcd.vld_addr = locvldir;

    _LB_wr64(iuspop, (double *) &headmcd, sizeof(headmcd), locnow);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _LB_UPDATE_FDIR - update the file directory */

static void _LB_update_fdir(void)
   {fdir_header *ph;

    nrecord++;
    ph = (fdir_header *) dir;

    strncpy(ph->name, "link", 4);
    ph->n_word   = NWDIR;
    ph->n_record = nrecord;
    ph->address  = locpop-1;

    _LB_wr64(iuspop, dir, NWDIR, 0);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LB_WRITE - finish this pop record and write it out */

void LB_write(void)
   {int locvldir;

/* write out "problem" vlb */
    if (nprobv != 0)
       _LB_write_problem();

/* write out "variable" VLB & mesh cell data block */
    _LB_write_variable();

/* write out VL directory (must be done after all VLBs are written) */
    _LB_write_vld(&locvldir);

/* write out the mesh cell data header */
    _LB_write_mcd(locvldir);

/* update the file directory */
    _LB_update_fdir();

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif
