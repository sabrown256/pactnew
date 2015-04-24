/*
 * TEMPLATE-RENDER.C - generate primitive type rendering routines
 *
 * #include "cpyright.h"
 *
 */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_STR_PROTO - write the prototype definition for
 *                 - a rendering routine
 */

static void write_str_proto(FILE *fp, type_desc *td)
   {

    fprintf(fp, "char *_SC_str_%s(char *t, int nc, const void *s, long n, int mode)\n",
            td->fncp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WRITE_BOOL - write the rendering function for KIND_BOOL */

static void _SC_write_bool(FILE *fp, type_desc *td)
   {

    write_str_proto(fp, td);

    fprintf(fp, "   {int nb;\n");
    fprintf(fp, "    char *fmt;\n");
    fprintf(fp, "    %s *pv = (%s *) s;\n", td->type, td->type);

    fprintf(fp, "    fmt = (mode == 1) ? _SC.types.formats[%d] : _SC.types.formata[%d];\n",
            td->id, td->id);

    fprintf(fp, "    nb = -1;\n");
    fprintf(fp, "    if (strchr(fmt, 's') != NULL)\n");
    fprintf(fp, "       nb = snprintf(t, nc, fmt, pv[n] ? \"T\" : \"F\");\n");
    fprintf(fp, "    else if (strchr(fmt, 'd') != NULL)\n");
    fprintf(fp, "       nb = snprintf(t, nc, fmt, (int) pv[n]);\n");

    fprintf(fp, "    if (nb < 0)\n");
    fprintf(fp, "       t = NULL;\n");
    fprintf(fp, "    return(t);}\n");
    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WRITE_COMPLEX - write the rendering function for KIND_COMPLEX */

static void _SC_write_complex(FILE *fp, type_desc *td)
   {int eid;
    type_desc *tc;
    static int flid = -1;

    tc = lookup_type_info(td->comp);
    if (flid == -1)
       flid = tc->id;

    eid = (tc->id - flid) >> 1;

    write_str_proto(fp, td);

    fprintf(fp, "   {int nb;\n");
    fprintf(fp, "    char *fmt;\n");
    fprintf(fp, "    %s _Complex z;\n", td->comp);
    fprintf(fp, "    %s *pv = (%s *) s;\n", td->type, td->type);

    fprintf(fp, "    fmt = (mode == 1) ? _SC.types.formats[%d] : _SC.types.formata[%d];\n",
            td->id, td->id);

#if (AF_LONG_DOUBLE_IO == 1)
    fprintf(fp, "\n");
    fprintf(fp, "    {char *p;\n");
    fprintf(fp, "     for (p = fmt; *p != \'\\0\'; p++)\n");
    fprintf(fp, "         if (*p == \'L\')\n");
    fprintf(fp, "            *p++ = \'l\';};\n");
    fprintf(fp, "\n");

    eid = min(eid, 1);
#endif

    fprintf(fp, "    {%s pr, pi;\n", td->comp);
    fprintf(fp, "     z  = pv[n];\n");
    fprintf(fp, "     pr = %s(z);\n", realp[eid]);
    fprintf(fp, "     pi = %s(z);\n", imagp[eid]);
    fprintf(fp, "     nb = snprintf(t, nc, fmt, pr, pi);};\n");

    fprintf(fp, "    if (nb < 0)\n");
    fprintf(fp, "       t = NULL;\n");

    fprintf(fp, "    return(t);}\n");
    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WRITE_QUATERNION - write the rendering function
 *                      - for KIND_QUATERNION
 */

static void _SC_write_quaternion(FILE *fp, type_desc *td)
   {

    write_str_proto(fp, td);

    fprintf(fp, "   {int nb;\n");
    fprintf(fp, "    char *fmt;\n");
    fprintf(fp, "    quaternion q;\n");
    fprintf(fp, "    %s *pv = (%s *) s;\n", td->type, td->type);

    fprintf(fp, "    fmt = (mode == 1) ? _SC.types.formats[%d] : _SC.types.formata[%d];\n",
            td->id, td->id);

    fprintf(fp, "    q  = pv[n];\n");
    fprintf(fp, "    nb = snprintf(t, nc, fmt, q.s, q.i, q.j, q.k);\n");

    fprintf(fp, "    if (nb < 0)\n");
    fprintf(fp, "       t = NULL;\n");

    fprintf(fp, "    return(t);}\n");
    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WRITE_NTOS - write the rendering function for type TD */

static void _SC_write_ntos(FILE *fp, type_desc *td)
   {

    write_str_proto(fp, td);

    fprintf(fp, "   {int nb;\n");
    fprintf(fp, "    char *fmt;\n");
    fprintf(fp, "    static int nld = 0;\n");
    fprintf(fp, "    %s *pv = (%s *) s;\n", td->type, td->type);

    fprintf(fp, "    fmt = (mode == 1) ? _SC.types.formats[%d] : _SC.types.formata[%d];\n",
            td->id, td->id);

/* non-standard long double I/O */
#if (AF_LONG_DOUBLE_IO == 1)
    fprintf(fp, "\n");
    fprintf(fp, "    {char *lp;\n");
    fprintf(fp, "     for (lp = fmt; *lp != \'\\0\'; lp++)\n");
    fprintf(fp, "         if (*lp == \'L\')\n");
    fprintf(fp, "            *lp++ = \'l\';};\n");
    fprintf(fp, "\n");

/* no long double I/O */
#elif (AF_LONG_DOUBLE_IO == 0)
    fprintf(fp, "\n");
    fprintf(fp, "    {char lc, *ls, *ld;\n");
    fprintf(fp, "     for (ls = fmt, ld = fmt; *ls != \'\\0\'; ls++)\n");
    fprintf(fp, "         {lc = *ls;\n");
    fprintf(fp, "          if (lc != \'L\')\n");
    fprintf(fp, "             *ld++ = lc;\n");
    fprintf(fp, "          else\n");
    fprintf(fp, "             nld = 1;};\n");
    fprintf(fp, "     *ld = \'\\0\';};\n");
    fprintf(fp, "\n");
#endif

    fprintf(fp, "    if (nld == 1)\n");
    fprintf(fp, "       nb  = snprintf(t, nc, fmt, (double) pv[n]);\n");
    fprintf(fp, "    else\n");
    fprintf(fp, "       nb  = snprintf(t, nc, fmt, pv[n]);\n");

    fprintf(fp, "    if (nb < 0)\n");
    fprintf(fp, "       t = NULL;\n");

    fprintf(fp, "    return(t);}\n");
    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WRITE_PTR - write the rendering function for pointer type TD */

static void _SC_write_ptr(FILE *fp, type_desc *td)
   {

    write_str_proto(fp, td);

    fprintf(fp, "   {int nld, nb;\n");
    fprintf(fp, "    char *fmt;\n");
    fprintf(fp, "    %s *pv = (%s *) s;\n", td->type, td->type);

    fprintf(fp, "    nld = 0;\n");
    fprintf(fp, "    fmt = (mode == 1) ? _SC.types.formats[%d] : _SC.types.formata[%d];\n",
            td->id, td->id);

/* non-standard long double I/O */
#if (AF_LONG_DOUBLE_IO == 1)
    fprintf(fp, "\n");
    fprintf(fp, "    {char *lp;\n");
    fprintf(fp, "     for (lp = fmt; *lp != \'\\0\'; lp++)\n");
    fprintf(fp, "         if (*lp == \'L\')\n");
    fprintf(fp, "            *lp++ = \'l\';};\n");
    fprintf(fp, "\n");

/* no long double I/O */
#elif (AF_LONG_DOUBLE_IO == 0)
    fprintf(fp, "\n");
    fprintf(fp, "    {char lc, *ls, *ld;\n");
    fprintf(fp, "     for (ls = fmt, ld = fmt; *ls != \'\\0\'; ls++)\n");
    fprintf(fp, "         {lc = *ls;\n");
    fprintf(fp, "          if (lc != \'L\')\n");
    fprintf(fp, "             *ld++ = lc;\n");
    fprintf(fp, "          else\n");
    fprintf(fp, "             nld = 1;};\n");
    fprintf(fp, "     *ld = \'\\0\';};\n");
    fprintf(fp, "\n");
#endif

    fprintf(fp, "    if (nld == 1)\n");
    fprintf(fp, "       nb  = snprintf(t, nc, fmt, *(double *) pv[n]);\n");
    fprintf(fp, "    else\n");
    fprintf(fp, "       nb  = snprintf(t, nc, fmt, pv[n]);\n");

    fprintf(fp, "    if (nb < 0)\n");
    fprintf(fp, "       t = NULL;\n");

    fprintf(fp, "    return(t);}\n");
    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_STR_DECL - write the declaration of the rendering array */

static void write_str_decl(FILE *fp, int ne, type_desc *tl, int src)
   {int i;
    type_desc *td;

    if (src == TRUE)
       {fprintf(fp, "PFStrv\n");
	fprintf(fp, " _SC_strf[] = {\n");

	for (i = 0; i < ne; i++)
	    {td = tl + i;
	     if (IS_PRIMITIVE_TYPE(td) == B_F)
	        fprintf(fp, "                NULL,\n");
	     else
	        fprintf(fp, "                _SC_str_%s,\n", td->fncp);};

	fprintf(fp, "   };\n");}
    else
       {fprintf(fp, "typedef char *(*PFStrv)(char *t, int nc, const void *s, long n, int mode);\n");
	fprintf(fp, "\n");

	fprintf(fp, "extern PFStrv\n");
	fprintf(fp, " _SC_strf[];\n");};

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_STR - write the rendering routines */

static void write_str(FILE *fp, int ne, type_desc *tl, int src)
   {int i;
    type_desc *td;

    if (src == TRUE)
       {Separator(fp);
        fprintf(fp, "/*                          NUMBER RENDERING                                */\n\n");
        Separator(fp);

        for (i = 0; i < ne; i++)
            {td = tl + i;
             if (IS_PRIMITIVE_TYPE(td) == B_F)
                continue;

             switch (td->g)
                {case KIND_BOOL :
                      _SC_write_bool(fp, td);
                      break;
                 case KIND_COMPLEX :
                      _SC_write_complex(fp, td);
                      break;
                 case KIND_QUATERNION :
                      _SC_write_quaternion(fp, td);
                      break;
                 case KIND_POINTER :
                      _SC_write_ptr(fp, td);
                      break;
                 default :
                      _SC_write_ntos(fp, td);
                      break;};};};

    write_str_decl(fp, ne, tl, src);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
