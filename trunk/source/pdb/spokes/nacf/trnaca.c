/*
 * TRNACA.C - command processors for NACF spoke in TRANSL
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "trnacf.h"

char
 *NACFILE_S = "NACfile";

static int
 n_structs = 0;

static char
 *NAC_types[] = {"double", "float", "double",
                 NULL, NULL, NULL, NULL, NULL,
                 "long", "int", "short", "long", 
                 NULL, NULL, NULL, NULL,
                 "char", "char",
                 "char", NULL, NULL, NULL, NULL, NULL,
                 "char", "char", NULL, NULL,
                 NULL, NULL, NULL, NULL, NULL};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _NAC_UNPACK_TABLE - unpack a table
 *                   - FT  - the field table
 *                   - RT  - the raw table data
 *                   - DT  - the packed data table (byte stream)
 *                   - N   - the number of CRAY words in RT
 *                   - NDT - the number of CRAY words in DT
 */

static long *_NAC_unpack_table(PDBfile *file, char *rt, char *dt,
			       int nrt, int ndt, int *pnum)
   {int i, j, n, bit_off, n_bits, count, first;
    char *nb, *pdt;
    long *out, *prt, *ft, *pout, *pft;
    long v, nwpe, nbpe, nitems;
    int *ord;
    static int size_max = SC_BITS_BYTE*sizeof(long);

    n  = nrt/5;
    ft = CMAKE_N(long, nrt);
    _PD_conv_in(file, ft, rt, SC_LONG_S, (long) nrt);

/* compute the number of words per entry (nwpe) */
    nwpe = 0L;
    for (i = 0; i < n; i++)
        {v    = ft[5*i];
         nwpe = max(nwpe, v);};

/* compute the number of entries in the table, dt */
    nitems = ndt/nwpe;
    nbpe   = CRAY_BYTES_WORD*nwpe;

/* compute the byte order */
    ord = CMAKE_N(int, nbpe);
    for (i = 0; i < nbpe; i++)
        ord[i] = i+1;

/* compute the number of bits in each mask */
    nb = CMAKE_N(char, n);
    for (i = 0; i < n; i++)
        {prt   = (long *) (rt + i*40 + 16);
         switch (sizeof(long))
            {case 4 :
	          count  = SC_bit_count(*prt++, sizeof(long));
		  count += SC_bit_count(*prt++, sizeof(long));
		  break;
             case 8 :
	          count = SC_bit_count(*prt++, CRAY_BYTES_WORD);
		  break;
             default :
	          PD_error("DIDN'T EXPECT THIS - _NAC_UNPACK_TABLE",
			   PD_OPEN);};
         nb[i] = count;};

    out = CMAKE_N(long, nitems*n);

    first = TRUE;
    for (i = 0; i < nitems; i++)
        {pout = out + i*n;
         pdt  = dt  + i*nbpe;
         for (j = 0; j < n; j++)
             {pft     = ft + j*5;
              n_bits  = nb[j];
              bit_off = pft[0]*64 - n_bits - pft[1];
              if (n_bits >= size_max)
                 {if (first)
                     {PRINT(stdout, "WARNING: TRUNCATED A FIELD - _NAC_UNPACK_TABLE\n");
                      first = FALSE;};
                  bit_off += (n_bits - size_max);
                  n_bits   = size_max;};

              pout[j] = SC_extract_field(pdt, bit_off, n_bits,
					 nbpe, ord);};};
                                          
    CFREE(ord);
    CFREE(ft);
    CFREE(nb);

    *pnum = n;

    return(out);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _NAC_FIND_STR - if the given memdes matches an existing one
 *               - return a pointer to it otherwise return NULL
 */

static defstr *_NAC_find_str(PDBfile *file, memdes *dp)
   {int differ;
    defstr *ndp;
    memdes *ndsc, *odsc;
    haelem *hp;
    hasharr *chrt;

    chrt = file->chart;

    for (hp = *(chrt->table); hp != NULL; hp = hp->next)
        {ndp = (defstr *) hp->def;

         differ = FALSE;
         ndsc   = ndp->members;
         odsc   = dp;
         while (TRUE)
            {if ((ndsc == NULL) || (odsc == NULL))
                break;

             if (strcmp(ndsc->member, odsc->member) != 0)
                {differ = TRUE;
                 break;};

             ndsc = ndsc->next;
             odsc = odsc->next;};

         if ((ndsc != NULL) || (odsc != NULL))
            continue;

         if (!differ)
            break;};

    return((hp == NULL) ? NULL : ndp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _NAC_PROC_STR - process NAC record structure defining defstr's as
 *               - it goes
 */

static defstr *_NAC_proc_str(PDBfile *file, char *names,
			     long *ofd, long *ofa, long *ofm,
			     int n, int numdir, int numatt, int numdim)
   {char memb[MAXLINE], mname[MAXLINE], mdims[MAXLINE], type_name[MAXLINE];
    char *mtype;
    int doffs, ndims, skip;
    long next, itype, mini, maxi, width, numb, mult;
    long *pd, *pa, *pm;
    defstr *dp;
    memdes *desc, *lst, *prev;

    prev  = NULL;
    lst   = NULL;
    doffs = file->default_offset;

    pd = ofd + (n - 1)*numdir;

/* start with the first child */
    next = pd[8];

    while (next)
       {skip = 0;

	pd = ofd + (next - 1)*numdir;
        pa = ofa + (pd[5] - 1)*numatt;

/* get the member name */
        strncpy(mname, names + pd[3], pd[4]);
        mname[pd[4]] = '\0';

/* if the member is a struct recurse down */
        snprintf(mdims, MAXLINE, "(");
        ndims = 0;
        width = pa[2];
        numb  = pa[1];
        itype = pa[0];
        mtype = NAC_types[itype];
        switch (itype)
           {case NAC_ASCII :
	         mult = CRAY_BYTES_WORD*((width + 7)/CRAY_BYTES_WORD);
		 if (mult > 1)
		    {SC_vstrcat(mdims, MAXLINE, "%ld", mult);
		     ndims++;};
		 break;

            case NAC_CHARS :
	         if (width > 1)
		    {SC_vstrcat(mdims, MAXLINE, "%ld", width);
		     ndims++;};
		 break;

            case NAC_LJBF :
	         mult = CRAY_BYTES_WORD*((width + 63)/64);
		 if (mult > 1)
		    {SC_vstrcat(mdims, MAXLINE, "%ld", mult);
		     ndims++;};
		 break;

            case NAC_BIT :
	         numb  = (numb + 7)/CRAY_BYTES_WORD;
		 pd[6] = 0;
		 break;

            case NAC_REC :
	         dp = _NAC_proc_str(file, names, ofd, ofa, ofm,
				    next,
				    numdir, numatt, numdim);
		 if (dp == NULL)
		    return(NULL);

		 mtype = dp->type;
		 break;};

        if (mtype == NULL)
           return(NULL);

        next = pd[7];

/* get the member dimensions dimensions */
        if (pd[6] == 0)
           {if (ndims == 1)
               SC_strcat(mdims, MAXLINE, ",");
            SC_vstrcat(mdims, MAXLINE, "%ld)", numb);
            ndims++;}

        else
           {if (ndims == 1)
               SC_strcat(mdims, MAXLINE, ",");

            pm = ofm + (pd[6] - 1)*numdim;
            while (TRUE)

/* check to see whether minimum dimension index is stored */
               {if (pm[0] == 0)
                   mini = 1L;
                else
                   mini = pm[2] ? pm[1] : -pm[1];

/* check to see whether maximum dimension index is stored */
                if (pm[5] == 0)
                   maxi = pa[1];
                else
                   maxi = pm[7] ? pm[6] : -pm[6];

		if (mini > maxi)
		   {skip = 1;
		    break;};

                if (mini == 1)
                   SC_vstrcat(mdims, MAXLINE, "%ld", maxi);
                else
                   SC_vstrcat(mdims, MAXLINE, "%ld:%ld", mini, maxi);
                ndims++;

                if (pm[13] != 0)
                   {pm = ofm + (pm[13] - 1)*numdim;
                    SC_strcat(mdims, MAXLINE, ",");}
                else
                   {SC_strcat(mdims, MAXLINE, ")");
                    break;};};};
	
/* skip members that have dimensions of length less than one */
	if (skip)
	   continue;

        if ((ndims == 1) && (numb == 1))
           mdims[0] = '\0';

        snprintf(memb, MAXLINE, "%s %s%s", mtype, mname, mdims);
        desc = _PD_mk_descriptor(memb, doffs);

        if (lst == NULL)
           lst = desc;
        else
           prev->next = desc;
        prev = desc;};

/* make the type name */
    snprintf(type_name, MAXLINE, "str-%d", n_structs);

    dp = _NAC_find_str(file, lst);
    if (dp == NULL)
       {n_structs++;

/* install the type in both charts */
        dp = _PD_defstr_inst(file, type_name, STRUCT_KIND, lst,
			     NO_ORDER, NULL, NULL, PD_CHART_HOST);}
    else
       for (desc = lst; desc != NULL; desc = lst)
	   {lst = desc->next;
	    _PD_rl_descriptor(desc);};

    return(dp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _NAC_BUILD_NAME_TABLE - build the NAC name table (ala symbol table) */

static void _NAC_build_name_table(PDBfile *file, char *names,
				  long *ofd, long *ofa, long *ofm,
				  int n, int numdir, int numatt, 
				  int numdim)
   {long next, itype, mult;
    long *pd, *pa, *pm;
    long addr, numb, mini, leng, width;
    char name[MAXLINE], *type;
    hasharr *nt;
    syment *ep;
    dimdes *dims, *nxt, *prev;
    defstr *dp;

    n_structs = 0;

    nt = file->symtab;

    next = n;
    while (next)
       {pd = ofd + (next - 1)*numdir;
        pa = ofa + (pd[5] - 1)*numatt;

        strncpy(name, names + pd[3], pd[4]);
        name[pd[4]] = '\0';

        dims  = NULL;
        addr  = pd[0]*CRAY_BYTES_WORD;
        width = pa[2];
        numb  = pa[1];
        itype = pa[0];
        type  = NAC_types[itype];
        switch (itype)
           {case NAC_ASCII :
	         mult  = CRAY_BYTES_WORD*((width + 7)/CRAY_BYTES_WORD);
		 numb *= mult;
		 dims  = _PD_mk_dimensions(1L, mult);
		 break;

            case NAC_CHARS :
	         mult  = width;
		 numb *= mult;
		 dims  = _PD_mk_dimensions(1L, mult);
		 break;

            case NAC_LJBF :
	         mult  = CRAY_BYTES_WORD*((width + 63)/64);
		 numb *= mult;
		 dims  = _PD_mk_dimensions(1L, mult);
		 break;

            case NAC_BIT :
	         numb = (numb + 7)/CRAY_BYTES_WORD;
		 dims = _PD_mk_dimensions(1L, numb);
		 break;

            case NAC_REC :
	         dp = _NAC_proc_str(file, names,
				    ofd, ofa, ofm, next,
				    numdir, numatt, numdim);
		 if (dp == NULL)
		    {PRINT(stdout,
			   "WARNING: VARIABLE %s HAS BAD STRUCTURE - _NAC_BUILD_NAME_TABLE\n", name);
		     next = pd[7];
		     continue;};

		 type = dp->type;

		 break;};

        next = pd[7];
        prev = dims;
        if (pd[6] == 0)
           {leng = numb;
	    dims = _PD_mk_dimensions(1L, leng);}

        else
           {pm = ofm + (pd[6] - 1)*numdim;
            while (TRUE)

/* check to see whether minimum dimension index is stored */
               {if (pm[0] == 0)
                   mini = 1L;
                else
                   mini = pm[2] ? pm[1] : -pm[1];

/* check to see whether maximum dimension index is stored */
                if (pm[5] == 0)
                   leng = numb;
                else
                   {leng  = pm[7] ? pm[6] : -pm[6];
                    leng -= (mini - 1L);};

/* skip variables with dimensions of length less than one */
		if (leng <= 0)
		   break;

                nxt = _PD_mk_dimensions(mini, leng);
                if (dims == NULL)
                   dims = nxt;
                else
                   prev->next = nxt;
                prev = nxt;

                if (pm[13] != 0)
                   pm = ofm + (pm[13] - 1)*numdim;
                else
                   break;};};

/* skip variables with dimensions of length less than one */
	if (leng <= 0)
	   continue;

        if (type == NULL)
           {PRINT(stdout, "UNKNOWN TYPE %d - _NAC_BUILD_NAME_TABLE\n", itype);
            continue;};

        ep = _PD_mk_syment(type, numb, addr, 0L, dims);

        SC_hasharr_install(nt, name, ep, SYMENT_S, TRUE, TRUE);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _NAC_FILEP - return TRUE iff TYPE is NAC file */

static int _NAC_filep(char *type)
   {int rv;

    rv = (strcmp(type, NACFILE_S) == 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _NAC_CLOSE - close an NAC file */

static int _NAC_close(PDBfile *file)
   {FILE *fp;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    switch (SETJMP(pa->close_err))
       {case ABORT :
	     return(FALSE);
        case ERR_FREE :
	     return(TRUE);
        default :
	     memset(pa->err, 0, MAXLINE);
	     break;};

    fp = file->stream;
    if (lio_close(fp) != 0)
       PD_error("CAN'T CLOSE FILE - _NAC_CLOSE", PD_CLOSE);

/* free the space */
    _PD_rl_pdb(file);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _NAC_OPEN - open an existing NAC File */

static PDBfile *_NAC_open(tr_layer *tr, SC_udl *pu, char *name, char *mode)
   {int numdir, numatt, numdim;
    long tptlen, tptaddr, dftaddr, dftlen, ntaddr, ntlen;
    long dtaddr, dtlen, aftaddr, aftlen, ataddr, atlen;
    long dmftaddr, dmftlen, dmaddr, dmlen;
    long SS_params[10], *tp, *ofd, *ofa, *ofm;
    unsigned char str[MAXLINE];
    char id[MAXLINE], *idmf, *ifdm;
    char *idf, *ifd, *iaf, *ifa;
    char *names, *bf;
    PDBfile *file;
    FILE *fp;

    fp   = pu->stream;
    file = NULL;

    _PD_set_io_buffer(pu);

/* attempt to read an ASCII header */
    if (lio_seek(fp, 0L, SEEK_SET))
       PD_error("FSEEK FAILED TO FIND ORIGIN - _NAC_OPEN", PD_OPEN);

    if (lio_read(str, CRAY_BYTES_WORD, 10, fp) != 10)
       PD_error("CAN'T READ THE FILE HEADER - _NAC_OPEN", PD_OPEN);

/* grab the ASCII id */
    strncpy(id, (char *) &str[8], CRAY_BYTES_WORD);
    id[CRAY_BYTES_WORD] = '\0';

    if ((str[0] == 0x6E) && (str[1] == 0x91) &&
        (strcmp(id, "nac-file") == 0))
       {file = _PD_mk_pdb(pu, NULL, BINARY_MODE_RPLUS, TRUE, NULL, tr);
	if (file == NULL)
	   PD_error("CAN'T ALLOCATE NACFILE - _NAC_OPEN", PD_OPEN);

	file->default_offset = 1;
	file->major_order    = COLUMN_MAJOR_ORDER;
	file->type           = CSTRSAVE(NACFILE_S);
	if (*mode == 'a')
	   file->mode = PD_APPEND;
	else
	   file->mode = PD_OPEN;

	_PD_set_standard(file, &CRAY_STD, &WORD8_ALIGNMENT);

	_PD_init_chrt(file, TRUE);

	_PD_conv_in(file, SS_params, &str[16], SC_LONG_S, 8L);

/* get the table pointer table */
	tptlen  = SS_params[4];
	tptaddr = SS_params[5]*CRAY_BYTES_WORD;
	bf = CMAKE_N(char, tptlen*CRAY_BYTES_WORD);
	tp = CMAKE_N(long, tptlen);

	if (lio_seek(fp, tptaddr, SEEK_SET))
	   PD_error("FAILED TO FIND TABLE POINTER TABLE - _NAC_OPEN", PD_OPEN);

	if (lio_read(bf, CRAY_BYTES_WORD, tptlen, fp) != tptlen)
	   PD_error("FAILED TO READ TABLE POINTER TABLE - _NAC_OPEN", PD_OPEN);

	_PD_conv_in(file, tp, bf, SC_LONG_S, (long) tptlen);
	CFREE(bf);

/* get the directory field table */
	dftaddr = tp[19]*CRAY_BYTES_WORD;
	dftlen  = tp[20];
	idf = CMAKE_N(char, dftlen*CRAY_BYTES_WORD);

	if (lio_seek(fp, dftaddr, SEEK_SET))
	   PD_error("FAILED TO FIND DIRECTORY FIELD TABLE - _NAC_OPEN", PD_OPEN);

	if (lio_read(idf, CRAY_BYTES_WORD, dftlen, fp) != dftlen)
	   PD_error("FAILED TO READ DIRECTORY FIELD TABLE - _NAC_OPEN", PD_OPEN);

/* get the directory table */
	dtaddr = tp[3]*CRAY_BYTES_WORD;
	dtlen  = tp[4];
	ifd = CMAKE_N(char, dtlen*CRAY_BYTES_WORD);

	if (lio_seek(fp, dtaddr, SEEK_SET))
	   PD_error("FAILED TO FIND DIRECTORY TABLE - _NAC_OPEN", PD_OPEN);

	if (lio_read(ifd, CRAY_BYTES_WORD, dtlen, fp) != dtlen)
	   PD_error("FAILED TO READ DIRECTORY TABLE - _NAC_OPEN", PD_OPEN);

/* get the attribute field table */
	aftaddr = tp[22]*CRAY_BYTES_WORD;
	aftlen  = tp[23];
	iaf = CMAKE_N(char, aftlen*CRAY_BYTES_WORD);

	if (lio_seek(fp, aftaddr, SEEK_SET))
	   PD_error("FAILED TO FIND ATTRIBUTE FIELD TABLE - _NAC_OPEN", PD_OPEN);

	if (lio_read(iaf, CRAY_BYTES_WORD, aftlen, fp) != aftlen)
	   PD_error("FAILED TO READ ATTRIBUTE FIELD TABLE - _NAC_OPEN", PD_OPEN);

/* get the attribute table */
	ataddr = tp[7]*CRAY_BYTES_WORD;
	atlen  = tp[8];
	ifa = CMAKE_N(char, atlen*CRAY_BYTES_WORD);

	if (lio_seek(fp, ataddr, SEEK_SET))
	   PD_error("FAILED TO FIND ATTRIBUTE TABLE - _NAC_OPEN", PD_OPEN);

	if (lio_read(ifa, CRAY_BYTES_WORD, atlen, fp) != atlen)
	   PD_error("FAILED TO READ ATTRIBUTE TABLE - _NAC_OPEN", PD_OPEN);

	ofd = _NAC_unpack_table(file, idf, ifd, dftlen, dtlen, &numdir);
	ofa = _NAC_unpack_table(file, iaf, ifa, aftlen, atlen, &numatt);

/* get the dimensionality field table */
	dmftaddr = tp[25]*CRAY_BYTES_WORD;
	dmftlen  = tp[26];
	idmf = CMAKE_N(char, dmftlen*CRAY_BYTES_WORD);

	if (lio_seek(fp, dmftaddr, SEEK_SET))
	   PD_error("FAILED TO FIND DIMENSIONALITY FIELD TABLE - _NAC_OPEN", PD_OPEN);

	if (lio_read(idmf, CRAY_BYTES_WORD, dmftlen, fp) != dmftlen)
	   PD_error("FAILED TO READ DIMENSIONALITY FIELD TABLE - _NAC_OPEN", PD_OPEN);

/* get the dimensionality table if there is one */
	dmaddr = tp[10]*CRAY_BYTES_WORD;
	dmlen  = tp[11];
	ofm    = NULL;
	if (dmlen > 0)
	   {ifdm = CMAKE_N(char, dmlen*CRAY_BYTES_WORD);
	    if (lio_seek(fp, dmaddr, SEEK_SET))
	       PD_error("FAILED TO FIND DIMENSIONALITY TABLE - _NAC_OPEN", PD_OPEN);

	    if (lio_read(ifdm, CRAY_BYTES_WORD, dmlen, fp) != dmlen)
	       PD_error("FAILED TO READ DIMENSIONALITY TABLE - _NAC_OPEN", PD_OPEN);
	    ofm = _NAC_unpack_table(file, idmf, ifdm, dmftlen, dmlen, &numdim);};

/* get the name table */
	ntaddr = tp[13]*CRAY_BYTES_WORD;
	ntlen  = tp[14];
	names  = CMAKE_N(char, ntlen);

	if (lio_seek(fp, ntaddr, SEEK_SET))
	   PD_error("FAILED TO FIND NAME TABLE - _NAC_OPEN", PD_OPEN);

	if (lio_read(names, 1, ntlen, fp) != ntlen)
	   PD_error("FAILED TO READ NAME TABLE - _NAC_OPEN", PD_OPEN);

	_NAC_build_name_table(file, names, ofd, ofa, ofm, tp[6],
			      numdir, numatt, numdim);

	CFREE(idf);
	CFREE(ifd);
	CFREE(iaf);
	CFREE(ifa);
	CFREE(ofd);
	CFREE(ofa);
	CFREE(names);};

    return(file);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PD_REGISTER_NACF - install the NACF extensions to PDBLib */
 
int PD_register_nacf(void)
   {static int n = -1;

    ONCE_SAFE(TRUE, NULL)
       n = PD_REGISTER(NACFILE_S, "nacf", _NAC_filep,
		       NULL, _NAC_open, _NAC_close, NULL, NULL);
       SC_ASSERT(n >= 0);
    END_SAFE;

    return(n);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


