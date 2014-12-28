/*
 * GSAUX.C - auxilliary support routines for PGS
 *         - THREADSAFE
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs_int.h"

#define MAX_PARTITION 0x7FFC    /* maximum length of a CELL ARRAY partition */
#define MASK_5_BITS   0x000000000000001F
#define MASK_BYTE     0x00000000000000FF

#define CGM_ERR   -100

#define EVEN(n)      !((n) & 1)
#define ODD(n)        ((n) & 1)
#define ROUND_UP(n)   ((((n) + 1L) >> 1) << 1)

#define CGM_BF_SIZE      4096

/*--------------------------------------------------------------------------*/

/*                            CGM SUPPORT ROUTINES                          */

/*--------------------------------------------------------------------------*/

/* PG_CGM_HDR - build up a CGM element header with
 *            - the classification and identification of the command
 */

static int PG_CGM_hdr(PG_device *dev, int cat, int id, int special,
		      long ni, long nby)
   {long nb, nwrite;
    unsigned char *pout;
    unsigned char out[16];

    nb   = 0L;
    pout = out;

    if (special)
       nby = ni;

    *pout++ = (cat << 4) | (id >> 3);
    *pout++ = (id << 5) | ((nby < 31) ? nby : 31);

    nb = 2L;

/* write the buffer */
    nwrite = io_write(out, 1L, nb, dev->file);

    return((nwrite == nb) ? 2 : 0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CGM_SPECIAL - write out the special case of
 *                - METAFILE_DEFAULTS_REPLACEMENT
 */

static int PG_CGM_special(PG_device *dev, int ni)
   {long nwrite = 0;
    unsigned char *pout;
    unsigned char out[16];

    pout = out;

    if (ni >= 31)
       {*pout++ = ni >> 8;
        *pout++ = ni & MASK_BYTE;
        nwrite = io_write(out, 1L, 2L, dev->file);};

    return(nwrite);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CGM_COLOR - write out a PG_palette to a CGM
 *              - return the number of bytes written
 */

static int PG_CGM_color(PG_device *dev, PG_palette *cp, int offs)
   {int j, n, nz, k, fl;
    long nb, no, na, nw;
    unsigned char *pout, *out;
    RGB_color_map *cm;

    if (_PG.fac == 0.0)
       _PG.fac = 255.0/MAXPIX;

    fl = (dev->palettes != dev->current_palette);
    n  = cp->n_pal_colors + 2;
    cm = cp->true_colormap;
    nb = 3*n + 1;
    na = nb + 2;
    nw = nb + fl*24;

    nz = nb + 28;
    out = CMAKE_N(unsigned char, nz);
    memset(out, 0, nz);

/* GOTCHA: not prepared to handle more than 4K of color table data yet
 *         Deal with this, if and when it becomes a problem.
 */
    pout    = out;
    *pout++ = nw >> 8;
    *pout++ = nw & MASK_BYTE;
    *pout++ = offs;

    if (fl)
       {for (j = 0; j < 8; j++)
            {k = (j*255.0)/7.0;
             *pout++ = k;
             *pout++ = k;
             *pout++ = k;};

        na += 24;};

    for (j = 0; j < n; j++)
        {*pout++ = _PG.fac*cm[j].red;
         *pout++ = _PG.fac*cm[j].green;
         *pout++ = _PG.fac*cm[j].blue;};

/* write the buffer */
    no = io_write(out, 1L, na, dev->file);

    CFREE(out);

    return((no == na) ? nb : -1);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CGM_BYTE - write out the byte stream into the CGM
 *             - return the number of bytes written
 */

static int PG_CGM_byte(PG_device *dev, char *b, long nb, long accb,
		       int morec)
   {int msk;
    long i, k, n, no;
    unsigned char *pout;
    unsigned char out[CGM_BF_SIZE];
    FILE *fp;

    fp = dev->file;
    no = 0L;
    if (accb >= 31)
       {for (i = 0; i < nb; i += MAX_PARTITION)
            {k    = nb - i;
             pout = out;
             if (k < MAX_PARTITION)
                {n   = k;
                 msk = (morec ? 0x80 : 0);}

             else
                {n   = MAX_PARTITION;
                 msk = 0x80;};

             *pout++ = (n >> 8) | msk;
             *pout++ = n & MASK_BYTE;

             io_write(out, 1L, 2L, fp);

             no += io_write(&b[i], 1L, n, fp);

             if (msk && ODD(n))
                {out[0] = '\0';
                 io_write(out, 1L, 1L, fp);};};}

    else
       no = io_write(b, 1L, nb, fp);

    return((no == nb) ? nb : -1);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CGM_WORD - write NW words from W into the CGM
 *             - return the number of bytes written
 */

static int PG_CGM_word(PG_device *dev, int *w, long nw, long accb,
		       int morec)
   {int i, j, k, n, nz, mx, mxt, need_long_cmmd, msk;
    int v;
    long db, na, nb, no;
    unsigned char *pout;
    unsigned char out[CGM_BF_SIZE];
    FILE *fp;

    fp = dev->file;
    nb = nw << 1;

    nz = min(nb, CGM_BF_SIZE);
    memset(out, 0, nz);

    need_long_cmmd = (accb >= 31);

    mx = CGM_BF_SIZE - 2;
    na = 0L;
    no = 0L;
    db = accb - nb;
    for (i = accb; i > db; i -= mx)
        {k    = i - db;
         pout = out;
         if (need_long_cmmd)
            {if (k < mx)
                {n   = k;
                 msk = (morec ? 0x80 : 0);}
             else
                {n   = mx;
                 msk = 0x80;};

             *pout++ = (n >> 8) | msk;
             *pout++ = n & MASK_BYTE;

             mxt = n >> 1;
             n  += 2;}

         else
            {n   = k;
             mxt = n >> 1;};

	 for (j = 0; j < mxt; j++)
	     {v = *w++;
	      *pout++ = v >> 8;
	      *pout++ = v & MASK_BYTE;};

         no += io_write(out, 1L, (long) n, fp);
         na += n;};

    return((no == na) ? nb : -1);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CGM_double - write NW double numbers from W into the CGM
 *             - return the number of bytes written
 *             - this only needs to write zeros for now, but is
 *             - ready to be expanded to the double thing if ever needed
 */

static int PG_CGM_double(PG_device *dev, double *w, long nw, long accb,
		       int morec)
   {int i, j, k, n, nz, mx, mxt, need_long_cmmd, msk;
    long db, na, nb, no;
    unsigned char *pout;
    unsigned char out[CGM_BF_SIZE];
    FILE *fp;

    fp = dev->file;
    nb = nw << 2;

    nz = min(nb, CGM_BF_SIZE);
    memset(out, 0, nz);

    need_long_cmmd = (accb >= 31);

    mx = CGM_BF_SIZE - 2;
    na = 0L;
    no = 0L;
    db = accb - nb;
    for (i = accb; i > db; i -= mx)
        {k    = i - db;
         pout = out;
         if (need_long_cmmd)
            {if (k < mx)
                {n   = k;
                 msk = (morec ? 0x80 : 0);}
             else
                {n   = mx;
                 msk = 0x80;};

             *pout++ = (n >> 8) | msk;
             *pout++ = n & MASK_BYTE;

             mxt = n >> 2;
             n  += 2;}

         else
            {n   = k;
             mxt = n >> 2;};

	 for (j = 0; j < mxt; j++)
	     {/* *w++; */
	      *pout++ = 0;
	      *pout++ = 0;
	      *pout++ = 0;
	      *pout++ = 0;};

         no += io_write(out, 1L, (long) n, fp);
         na += n;};

    return((no == na) ? nb : -1);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CGM_STRING - write out the string into the CGM
 *               - after converting is to PASCAL form
 *               - return the number of bytes written
 */

static int PG_CGM_string(PG_device *dev, char **pb, long ns, long accb,
			 int morec, int pad)
   {int i, msk;
    char *b;
    unsigned char *pout;
    unsigned char out[MAXLINE];
    long n, lb, nb, no;

    nb = 0L;
    for (i = 0; i < ns; i++)
        nb += strlen(pb[i]);
    nb += ns;
    n   = nb;

    pout = out;
    if (accb >= 31)
       {if (nb < MAX_PARTITION)
           {msk     = (morec ? 0x80 : 0);
            *pout++ = (nb >> 8) | msk;
            *pout++ = nb & MASK_BYTE;

            n += 2;}

/* not prepared to handle strings cumulatively longer than MAX_PARTITION yet */
        else
	   return(CGM_ERR);};

    for (i = 0; i < ns; i++)
        {b  = pb[i];
         lb = strlen(b);
         PG_c_str_pascal(pout, b, lb++, pad);
         pout += lb;};

    if (pad)
       n = ROUND_UP(n);

/* write the buffer */
    no = io_write(out, 1L, n, dev->file);

    return((no == n) ? n : -1);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_C_STR_PASCAL - convert a C string to a PASCAL string
 *                 - use strlen(S) for char count if N is negative
 *                 - writes strlen(S)+1 bytes into BF
 *                 - if PAD is TRUE and strlen(S) even pad with extra char
 *                 - Note: the string must be shorter than 255 characters
 */

unsigned char *PG_c_str_pascal(unsigned char *bf, char *s, long n, int pad)
   {int nc;

    if (s != NULL)
       {nc = strlen(s);

/* compute n ignoring anything beyond 254 characters */
	n = (n >= 0) ? n : nc;
	n = min(n, 254);

	pad = (pad && EVEN(n));

/* make the PASCAL copy of the string */
	*bf++ = n;
	while (n--)
	   *bf++ = *s++;

/* force total number of characters to be even */
	if (pad)
	   *bf++ = '\0';};

    return(bf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_CGM_COMMAND - format and write out a CGM command */

int PG_CGM_command(PG_device *dev, int cat, int id, int nparams, ...)
   {int i, j, ityp;
    int offs, special, morec, nitms;
    int ni[20], iv[20];
    int *piv[20];
    long abytes, nbytes, lbytes, nb;
    char *type[20], cv[20], *pcv[20], **ppcv[20];
    double dv[20], *pdv[20];
    unsigned char out[16];
    PG_palette *cp;

    nbytes = 0L;
    cp     = NULL;
    memset(ppcv, 0, sizeof(ppcv));
    memset(pcv, 0, sizeof(pcv));
    memset(pdv, 0, sizeof(pdv));
    memset(piv, 0, sizeof(piv));

    SC_VA_START(nparams);
    special = FALSE;
    offs    = 0;
    for (i = 0; i < nparams; i++)
        {ni[i]   = SC_VA_ARG(int);
         type[i] = SC_VA_ARG(char *);
	 lbytes  = 0;

         if (type[i] == '\0')
            {special = (ni[i] > 0);
             break;}

         else
	    {ityp = SC_type_id(type[i], FALSE);

/* fixed point types */
	     if (ityp == G_INT_I)
	        {lbytes = ni[i] << 1;
		 if (ni[i] == 1)
		    iv[i] = SC_VA_ARG(int);
		 else
		    piv[i] = SC_VA_ARG(int *);}

/* floating point types */
	     else if (ityp == G_DOUBLE_I)
	        {lbytes = ni[i] << 2;
		 if (ni[i] == 1)
		    dv[i] = SC_VA_ARG(double);
		 else
		    pdv[i] = SC_VA_ARG(double *);}

	     else if (ityp == G_CHAR_I)
	        {lbytes = ni[i];
		 if (ni[i] == 1)
		    cv[i] = SC_VA_ARG(int);
		 else
		    pcv[i] = SC_VA_ARG(char *);}

	     else if (ityp == G_STRING_I)
	        {if (ni[i] == 1)
		    {pcv[i] = SC_VA_ARG(char *);
		     if (pcv[i] != NULL)
		        lbytes = strlen(pcv[i]) + 1;}
		 else
		    {ppcv[i] = SC_VA_ARG(char **);
		     lbytes  = 0L;
		     for (j = 0; j < ni[i]; j++)
		        {if (ppcv[i][j] != NULL)
			    lbytes += strlen(ppcv[i][j]) + 1L;};};}

	     else if (strcmp(type[i], "PG_palette") == 0)
	        {cp     = SC_VA_ARG(PG_palette *);
		 offs   = SC_VA_ARG(int);
		 lbytes = 3*(cp->n_pal_colors + 2) + 1;
		 lbytes = ((lbytes + 1L) >> 1) << 1;};};

         nbytes += lbytes;};

    SC_VA_END;

/* add two for the element descriptor bytes */
    abytes = nbytes;

/* if abytes is odd, pad the buffer to the next even byte */
    if (ODD(abytes))
       abytes++;

/* get the element header or partition header right */
    nitms = special ? ni[i] : 0;

    PG_CGM_hdr(dev, cat, id, special, (long) nitms, nbytes);

    for (i = 0; i < nparams; i++)
        {morec = (i != nparams - 1);
         if (type[i] == NULL)
            abytes -= PG_CGM_special(dev, ni[i]);

         else
	    {ityp = SC_type_id(type[i], FALSE);

/* fixed point types */
	     if (ityp == G_INT_I)
	        {if (ni[i] == 1)
		    abytes -= PG_CGM_word(dev, &iv[i], 1L,
					  nbytes, morec);
		 else
		    abytes -= PG_CGM_word(dev, piv[i], (long) ni[i],
					  nbytes, morec);}

/* floating point types */
	     else if (ityp == G_DOUBLE_I)
	        {if (ni[i] == 1)
		    abytes -= PG_CGM_double(dev, &dv[i], 1L,
					    nbytes, morec);
		 else
		    abytes -= PG_CGM_double(dev, pdv[i], (long) ni[i],
					    nbytes, morec);}

	     else if (ityp == G_CHAR_I)
	        {if (ni[i] == 1)
		    abytes -= PG_CGM_byte(dev, &cv[i], 1L,
					  nbytes, morec);
		 else
		    abytes -= PG_CGM_byte(dev, pcv[i], (long) ni[i],
					  nbytes, morec);}

	     else if (ityp == G_STRING_I)
	        {if (ni[i] == 1)
		    {nb =  PG_CGM_string(dev, &pcv[i], 1L,
					 nbytes, morec, TRUE);
		     if (nb == CGM_ERR)
		        return(FALSE);
		     abytes -= nb;}
		 else
		    {nb = PG_CGM_string(dev, ppcv[i], (long) ni[i],
					nbytes, morec, TRUE);
		     if (nb == CGM_ERR)
		        return(FALSE);
		     abytes -= nb;};}

	     else if ((cp != NULL) && (strcmp(type[i], "PG_palette") == 0))
	        abytes -= PG_CGM_color(dev, cp, offs);};};

/* pad out the element */
    if (abytes == 1)
       {out[0] = '\0';
        io_write(out, 1L, 1L, dev->file);
        abytes--;};

    return(abytes == 0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
