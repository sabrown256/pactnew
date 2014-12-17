/*
 * TEMPLATE-COERCE.C - generate primitive type converters
 *
 * #include "cpyright.h"
 *
 */

#ifdef SC_FAST_TRUNC
#define CONVERT  _SC_write_n_to_n_fast
#define C_TO_C   _SC_write_n_to_n_fast
#else
#define CONVERT  _SC_write_n_to_n_safe
#define C_TO_C   _SC_write_c_to_c_safe
#endif

#define Separator(_f)  fprintf(_f, "/*--------------------------------------------------------------------------*/\n\n")

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_COERCE_PROTO - write the prototype definition for
 *                    - a conversion routine
 */

static void write_coerce_proto(FILE *fp, typdes *td, typdes *ts)
   {

    fprintf(fp, "long _SC_%s_%s(void *d, long od, long ds, void *s, long os, long ss, long n)\n",
	    td->fncp, ts->fncp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WRITE_N_TO_N_FAST - number to number conversions */

static void _SC_write_n_to_n_fast(FILE *fp, typdes *td, typdes *ts)
   {

    if ((td->g == KIND_OTHER) || (ts->g == KIND_OTHER))
       fprintf(stderr, "ERROR: bad n_to_n type\n");

    write_coerce_proto(fp, td, ts);

    fprintf(fp, "   {long i;\n");
    fprintf(fp, "    %s *ps = (%s *) s;\n", ts->type, ts->type);
    fprintf(fp, "    %s *pd = (%s *) d;\n", td->type, td->type);

    fprintf(fp, "    ps += os;\n");
    fprintf(fp, "    pd += od;\n");

    fprintf(fp, "    for (i = 0; i < n; i++, pd += ds, ps += ss)\n");
    fprintf(fp, "        *pd = *ps;\n");

    fprintf(fp, "    return(i);}\n");

    fprintf(fp, " \n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WRITE_N_TO_N_SAFE - number to number conversions */

static void _SC_write_n_to_n_safe(FILE *fp, typdes *td, typdes *ts)
   {int ok;

    if ((td->g == KIND_OTHER) || (ts->g == KIND_OTHER))
       fprintf(stderr, "ERROR: bad n_to_n type\n");

    ok = ((td->bpi < ts->bpi) &&
	  ((td->g == KIND_CHAR) ||
	   (td->g == KIND_INT) ||
	   (td->g == KIND_FLOAT)));

    write_coerce_proto(fp, td, ts);

    fprintf(fp, "   {long i;\n");
    if (ok == TRUE)
       fprintf(fp, "    %s sm, smn, smx;\n", ts->type);
    fprintf(fp, "    %s *ps = (%s *) s;\n", ts->type, ts->type);
    fprintf(fp, "    %s *pd = (%s *) d;\n", td->type, td->type);

    fprintf(fp, "    ps += os;\n");
    fprintf(fp, "    pd += od;\n");

    if (ok == TRUE)
       {fprintf(fp, "    smn = (%s) %s;\n", ts->type, td->mn);
	fprintf(fp, "    smx = (%s) %s;\n", ts->type, td->mx);
	fprintf(fp, "    for (i = 0; i < n; i++, pd += ds, ps += ss)\n");
	fprintf(fp, "        {sm  = *ps;\n");
	fprintf(fp, "         sm  = min(sm, smx);\n");
	fprintf(fp, "         sm  = max(sm, smn);\n");
	fprintf(fp, "         *pd = sm;};\n");}
    else
       {fprintf(fp, "    for (i = 0; i < n; i++, pd += ds, ps += ss)\n");
	fprintf(fp, "        *pd = *ps;\n");};

    fprintf(fp, "    return(i);}\n");

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WRITE_C_TO_C_SAFE - complex to complex conversions */

static void _SC_write_c_to_c_safe(FILE *fp, typdes *td, typdes *ts)
   {int eid;

    if ((td->fncp == NULL) || (ts->fncp == NULL))
       fprintf(stderr, "ERROR: bad c_to_c type\n");

    eid = 2;

    write_coerce_proto(fp, td, ts);

    fprintf(fp, "   {long i;\n");
    fprintf(fp, "    %s *ps = (%s *) s;\n", ts->type, ts->type);
    fprintf(fp, "    %s *pd = (%s *) d;\n", td->type, td->type);

    if (td->id < ts->id)
       {fprintf(fp, "    long double sr, si, smn, smx;\n");
	fprintf(fp, "    long double _Complex zs;\n");
	fprintf(fp, "    smn = %s;\n", td->mn);
	fprintf(fp, "    smx = %s;\n", td->mx);};

    fprintf(fp, "    ps += os;\n");
    fprintf(fp, "    pd += od;\n");

    if (td->id < ts->id)
       {fprintf(fp, "    for (i = 0; i < n; i++, pd += ds, ps += ss)\n");
	fprintf(fp, "        {zs = *ps;\n");
	fprintf(fp, "         sr = %s(zs);\n", realp[eid]);
	fprintf(fp, "         si = %s(zs);\n", imagp[eid]);
	fprintf(fp, "         sr = min(sr, smx);\n");
	fprintf(fp, "         sr = max(sr, smn);\n");
	fprintf(fp, "         si = min(si, smx);\n");
	fprintf(fp, "         si = max(si, smn);\n");
	fprintf(fp, "         *pd = sr + si*I;};\n");}
    else
       {fprintf(fp, "    for (i = 0; i < n; i++, pd += ds, ps += ss)\n");
	fprintf(fp, "        *pd = *ps;\n");};

    fprintf(fp, "    return(i);}\n");

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WRITE_C_TO_R - complex to real conversions */

static void _SC_write_c_to_r(FILE *fp, typdes *td, typdes *ts)
   {int eid;

    if ((td->fncp == NULL) || (ts->fncp == NULL))
       fprintf(stderr, "ERROR: bad c_to_r type\n");

    eid = 2;

    write_coerce_proto(fp, td, ts);

    fprintf(fp, "   {long i;\n");
    fprintf(fp, "    %s *ps = (%s *) s;\n", ts->type, ts->type);
    fprintf(fp, "    %s *pd = (%s *) d;\n", td->type, td->type);

    fprintf(fp, "    ps += os;\n");
    fprintf(fp, "    pd += od;\n");

/* this crashes GCC 4.5.1 - internal compiler error
    fprintf(fp, "    for (i = 0; i < n; i++, *pd += ds = *ps++);\n");
*/
    fprintf(fp, "    for (i = 0; i < n; i++, pd += ds, ps += ss)\n");
    fprintf(fp, "        *pd = %s(*ps);\n", realp[eid]);

    fprintf(fp, "    return(i);}\n");

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WRITE_Q_TO_R - quaternion to real conversions */

static void _SC_write_q_to_r(FILE *fp, typdes *td, typdes *ts)
   {

    if ((td->fncp == NULL) || (ts->fncp == NULL))
       fprintf(stderr, "ERROR: bad q_to_r type\n");

    write_coerce_proto(fp, td, ts);

    fprintf(fp, "   {long i;\n");
    fprintf(fp, "    quaternion *ps = (quaternion *) s;\n");
    fprintf(fp, "    %s *pd = (%s *) d;\n", td->type, td->type);

    fprintf(fp, "    ps += os;\n");
    fprintf(fp, "    pd += od;\n");

    fprintf(fp, "    for (i = 0; i < n; i++, pd += ds, ps += ss)\n");
    fprintf(fp, "        *pd = ps->s;\n");

    fprintf(fp, "    return(i);}\n");

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WRITE_Q_TO_C - quaternion to complex conversions */

static void _SC_write_q_to_c(FILE *fp, typdes *td, typdes *ts)
   {

    if ((td->fncp == NULL) || (ts->fncp == NULL))
       fprintf(stderr, "ERROR: bad q_to_c type\n");

    write_coerce_proto(fp, td, ts);

    fprintf(fp, "   {long i;\n");
    fprintf(fp, "    quaternion *ps = (quaternion *) s;\n");
    fprintf(fp, "    %s *pd = (%s *) d;\n", td->type, td->type);
    fprintf(fp, "    %s z;\n", td->type);
    fprintf(fp, "    quaternion q;\n");

    fprintf(fp, "    ps += os;\n");
    fprintf(fp, "    pd += od;\n");

    fprintf(fp, "    for (i = 0; i < n; i++, pd += ds, ps += ss)\n");
    fprintf(fp, "        {q   = *ps;\n");
    fprintf(fp, "         z   = q.s + q.i*I;\n");
    fprintf(fp, "         *pd = z;};\n");

    fprintf(fp, "    return(i);}\n");

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WRITE_R_TO_Q - real to quaternion conversions */

static void _SC_write_r_to_q(FILE *fp, typdes *td, typdes *ts)
   {

    if ((td->fncp == NULL) || (ts->fncp == NULL))
       fprintf(stderr, "ERROR: bad r_to_q type\n");

    write_coerce_proto(fp, td, ts);

    fprintf(fp, "   {long i;\n");
    fprintf(fp, "    %s *ps = (%s *) s;\n", ts->type, ts->type);
    fprintf(fp, "    quaternion q = {0.0, 0.0, 0.0, 0.0};\n");
    fprintf(fp, "    quaternion *pd = d;\n");

    fprintf(fp, "    ps += os;\n");
    fprintf(fp, "    pd += od;\n");

    fprintf(fp, "    for (i = 0; i < n; i++, pd += ds, ps += ss)\n");
    fprintf(fp, "        {q.s = *ps;\n");
    fprintf(fp, "         *pd = q;};\n");

    fprintf(fp, "    return(i);}\n");

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WRITE_C_TO_Q - complex to quaternion conversions */

static void _SC_write_c_to_q(FILE *fp, typdes *td, typdes *ts)
   {int eid;

    if ((td->fncp == NULL) || (ts->fncp == NULL))
       fprintf(stderr, "ERROR: bad c_to_q type\n");

    eid = 2;

    write_coerce_proto(fp, td, ts);

    fprintf(fp, "   {long i;\n");
    fprintf(fp, "    %s *ps = (%s *) s;\n", ts->type, ts->type);
    fprintf(fp, "    long double _Complex z;\n");
    fprintf(fp, "    quaternion q = {0.0, 0.0, 0.0, 0.0};\n");
    fprintf(fp, "    quaternion *pd = d;\n");

    fprintf(fp, "    ps += os;\n");
    fprintf(fp, "    pd += od;\n");

    fprintf(fp, "    for (i = 0; i < n; i++, pd += ds, ps += ss)\n");
    fprintf(fp, "        {z   = *ps;\n");
    fprintf(fp, "         q.s = %s(z);\n", realp[eid]);
    fprintf(fp, "         q.i = %s(z);\n", imagp[eid]);
    fprintf(fp, "         *pd = q;};\n");

    fprintf(fp, "    return(i);}\n");

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WRITE_P_TO_N - pointer to number conversions */

static void _SC_write_p_to_n(FILE *fp, typdes *td, typdes *ts)
   {

    if (td->fncp == NULL)
       fprintf(stderr, "ERROR: bad p_to_n type\n");

    write_coerce_proto(fp, td, ts);

    fprintf(fp, "   {long i;\n");
    fprintf(fp, "    SC_address ad;\n");
    fprintf(fp, "    void **ps = (void **) s;\n");
    fprintf(fp, "    %s *pd = (%s *) d;\n", td->type, td->type);

    fprintf(fp, "    ps += os;\n");
    fprintf(fp, "    pd += od;\n");

    fprintf(fp, "    for (i = 0; i < n; i++, pd += ds, ps += ss)\n");
    fprintf(fp, "        {ad.memaddr = *ps;\n");
    fprintf(fp, "         *pd        = ad.diskaddr;};\n");

    fprintf(fp, "    return(i);}\n");

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WRITE_P_TO_Q - pointer to quaternion conversions */

static void _SC_write_p_to_q(FILE *fp, typdes *td, typdes *ts)
   {

    if (td->fncp == NULL)
       fprintf(stderr, "ERROR: bad p_to_q type\n");

    write_coerce_proto(fp, td, ts);

    fprintf(fp, "   {long i;\n");
    fprintf(fp, "    SC_address ad;\n");
    fprintf(fp, "    quaternion q = {0.0, 0.0, 0.0, 0.0};\n");
    fprintf(fp, "    quaternion *pd = d;\n");
    fprintf(fp, "    %s *ps = (%s *) s;\n", ts->type, ts->type);

    fprintf(fp, "    ps += os;\n");
    fprintf(fp, "    pd += od;\n");

    fprintf(fp, "    for (i = 0; i < n; i++, pd += ds, ps += ss)\n");
    fprintf(fp, "        {ad.memaddr = *ps;\n");
    fprintf(fp, "         q.s        = ad.diskaddr;\n");
    fprintf(fp, "         *pd        = q;};\n");

    fprintf(fp, "    return(i);}\n");

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WRITE_N_TO_P - number to pointer conversions */

static void _SC_write_n_to_p(FILE *fp, typdes *td, typdes *ts)
   {

    if (ts->fncp == NULL)
       fprintf(stderr, "ERROR: bad n_to_p type\n");

    write_coerce_proto(fp, td, ts);

    fprintf(fp, "   {long i;\n");
    fprintf(fp, "    SC_address ad;\n");
    fprintf(fp, "    %s *ps = (%s *) s;\n", ts->type, ts->type);
    fprintf(fp, "    void **pd = (void **) d;\n");

    fprintf(fp, "    ps += os;\n");
    fprintf(fp, "    pd += od;\n");

    fprintf(fp, "    for (i = 0; i < n; i++, pd += ds, ps += ss)\n");
    fprintf(fp, "        {ad.diskaddr = *ps;\n");
    fprintf(fp, "         *pd         = ad.memaddr;};\n");

    fprintf(fp, "    return(i);}\n");

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WRITE_Q_TO_P - quaternion to pointer conversions */

static void _SC_write_q_to_p(FILE *fp, typdes *td, typdes *ts)
   {

    if (ts->fncp == NULL)
       fprintf(stderr, "ERROR: bad q_to_p type\n");

    write_coerce_proto(fp, td, ts);

    fprintf(fp, "   {long i;\n");
    fprintf(fp, "    SC_address ad;\n");
    fprintf(fp, "    quaternion q = {0.0, 0.0, 0.0, 0.0};\n");
    fprintf(fp, "    quaternion *ps = (quaternion *) s;\n");
    fprintf(fp, "    void **pd = (void **) d;\n");

    fprintf(fp, "    ps += os;\n");
    fprintf(fp, "    pd += od;\n");

    fprintf(fp, "    for (i = 0; i < n; i++, pd += ds, ps += ss)\n");
    fprintf(fp, "        {q           = *ps;\n");
    fprintf(fp, "         ad.diskaddr = q.s;\n");
    fprintf(fp, "         *pd         = ad.memaddr;};\n");

    fprintf(fp, "    return(i);}\n");

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_CONV_DEF - write the definition of the conversion array */

static void write_conv_def(FILE *fp, int ne, typdes *tl, int src)
   {int id, is;
    typdes *td, *ts;

    if (src == TRUE)
       {fprintf(fp, "PFConv\n");
	fprintf(fp, " _SC_convf[N_PRIMITIVES][N_PRIMITIVES] = {\n");

	for (id = 0; id < ne; id++)
	    {td = tl + id;
	     if (IS_PRIMITIVE_TYPE(td) == B_F)
	        continue;

	     fprintf(fp, "      {\n");
	     for (is = 0; is < ne; is++)
	         {ts = tl + is;
		  if (IS_PRIMITIVE_TYPE(ts) == B_F)
		     continue;

		  fprintf(fp, "        _SC_%s_%s,\n", td->fncp, ts->fncp);};

	     fprintf(fp, "       },\n");};

	fprintf(fp, "};\n");}
    else
       {fprintf(fp, "typedef long (*PFConv)(void *d, long od, long ds, void *s, long os, long ss, long n);\n");
	fprintf(fp, "\n");

	fprintf(fp, "extern PFConv\n");
	fprintf(fp, " _SC_convf[][N_PRIMITIVES];\n");};


    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_FROM_C - write conversions from complex */

static void write_from_c(FILE *fp, typdes *td, typdes *ts)
   {

    switch (td->g)
       {case KIND_BOOL :
	case KIND_CHAR :
	case KIND_INT  :
	     CONVERT(fp, td, ts);
             break;
	case KIND_FLOAT :
	     _SC_write_c_to_r(fp, td, ts);
	     break;
	case KIND_COMPLEX :
	     C_TO_C(fp, td, ts);
	     break;
	case KIND_QUATERNION :
	     _SC_write_c_to_q(fp, td, ts);
	     break;
	case KIND_POINTER :
	     _SC_write_n_to_p(fp, td, ts);
	     break;
        default :
             break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_FROM_Q - write conversions from quaternion */

static void write_from_q(FILE *fp, typdes *td, typdes *ts)
   {

    switch (td->g)
       {case KIND_BOOL :
	case KIND_CHAR :
	case KIND_INT  :
	case KIND_FLOAT :
	     _SC_write_q_to_r(fp, td, ts);
	     break;
	case KIND_COMPLEX :
	     _SC_write_q_to_c(fp, td, ts);
	     break;
	case KIND_POINTER :
	     _SC_write_q_to_p(fp, td, ts);
	     break;
        default :
             break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_TO_Q - write conversions to quaternion */

static void write_to_q(FILE *fp, typdes *td, typdes *ts)
   {

    switch (ts->g)
       {case KIND_BOOL :
	case KIND_CHAR :
	case KIND_INT  :
	case KIND_FLOAT :
	     _SC_write_r_to_q(fp, td, ts);
	     break;
	case KIND_COMPLEX :
	     _SC_write_c_to_q(fp, td, ts);
	     break;
	case KIND_POINTER :
	     _SC_write_p_to_q(fp, td, ts);
	     break;
        default :
             break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_COERCE - write the conversion routines */

static void write_coerce(FILE *fp, int ne, typdes *tl, int src)
   {int id, is;
    typdes *td, *ts;

    if (src == TRUE)
       {fprintf(fp, "#include \"score_int.h\"\n");
	fprintf(fp, " \n");

	fprintf(fp, "#undef CONVERT\n");
	fprintf(fp, " \n");

	fprintf(fp, "#define BOOL_MIN    0\n");
	fprintf(fp, "#define BOOL_MAX    1\n");
	fprintf(fp, " \n");

	Separator(fp);
	fprintf(fp, "/*                           TYPE CONVERSION                                */\n\n");
	Separator(fp);

	for (id = 0; id < ne; id++)
	    {td = tl + id;
	     if (IS_PRIMITIVE_TYPE(td) == B_F)
	        continue;

	     for (is = 0; is < ne; is++)
	         {ts = tl + is;
		  if (IS_PRIMITIVE_TYPE(ts) == B_F)
		     continue;

/* direct copy */
		  if ((id == is) && (td->g != KIND_POINTER))
		     _SC_write_n_to_n_fast(fp, td, ts);

/* complex to anything conversions */
		  else if (ts->g == KIND_COMPLEX)
		     write_from_c(fp, td, ts);

/* quaternion to anything conversions */
		  else if (ts->g == KIND_QUATERNION)
		     write_from_q(fp, td, ts);

/* anything to quaternion conversions */
		  else if (td->g == KIND_QUATERNION)
		     write_to_q(fp, td, ts);

		  else if ((td->g != KIND_POINTER) &&
			   (ts->g == KIND_POINTER))
		     _SC_write_p_to_n(fp, td, ts);

		  else if ((td->g == KIND_POINTER) &&
			   (ts->g != KIND_POINTER))
		     _SC_write_n_to_p(fp, td, ts);

		  else if ((td->knd == TK_PRIMITIVE) &&
			    (ts->knd == TK_PRIMITIVE))
		     CONVERT(fp, td, ts);};};

	fprintf(fp, "\n");};

    write_conv_def(fp, ne, tl, src);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
