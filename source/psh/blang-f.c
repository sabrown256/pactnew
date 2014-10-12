/*
 * BLANG-F.C - generate Fortran language bindings
 *
 * #include "cpyright.h"
 *
 */

/* #define FORTRAN_BIND_C_ALL */

/* #define RETURN_INTEGER */

#ifdef RETURN_INTEGER
#define C_PTR_RETURN "integer(isizea)"
#else
#define C_PTR_RETURN "type(C_PTR)"
#endif

static char
 *ind = "";
/* *ind = "      "; */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FEMIT - emit lines broken apropriately for Fortran */

static void femit(FILE *fp, char *t, char *trm)
   {int i, c, n, nc;
    char *pt;

    nc = strlen(t);

    for (pt = t; nc > 0; )
        {n = (pt == t) ? 73 : 51;
	 if (n > nc)
	    {if (pt == t)
		fprintf(fp, "%s %s\n", pt, trm);
	     else
		fprintf(fp, "                      %s %s\n", pt, trm);
	     break;}
	 else
	    {n = min(n, nc);
	     for (i = n-1; i >= 0; i--)
	         {if (strchr(", \t", pt[i]) != NULL)
		     {c = pt[i+1];
		      pt[i+1] = '\0';
		      if (pt == t)
			 fprintf(fp, "%s &\n", pt);
		      else
			 fprintf(fp, "                      %s &\n", pt);
		      pt[i+1] = c;
		      i++;
		      pt += i;
		      nc -= i;
		      break;};};
	     if (i < 0)
	        break;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CF_TYPE - return Fortran type corresponding to C type TY */

static void cf_type(char *a, int nc, char *ty)
   {char *fty;

    if (strcmp(ty, "char *") == 0)
       nstrncpy(a, nc, "string", -1);

    else if (is_ptr(ty) == TRUE)
       nstrncpy(a, nc, "void *", -1);

    else
       {fty = lookup_type(NULL, ty, MODE_C, MODE_F);
	if (fty != NULL)
	   nstrncpy(a, nc, fty, -1);
	else
	   nstrncpy(a, nc, "unknown", -1);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FC_TYPE - return C type corresponding to FORTRAN type TY in WTY
 *         - return the kind of FORTRAN argument TY is
 */

static void fc_type(char *wty, int nc, farg *al, int afl, int mode)
   {fparam knd;
    char lty[BFLRG];
    char *pty, *arg, *ty;

    if (al != NULL)
       {knd = FP_ANY;

	if (wty == NULL)
	   {wty = lty;
	    nc  = BFLRG;};

	arg = al->arg;
	ty  = al->type;

	pty = lookup_type(NULL, ty, MODE_C, mode);

/* handle variable arg list */
	if (is_var_arg(ty) == TRUE)
	   {nstrncpy(wty, nc, "char *", -1);
	    knd = FP_VARARG;}

/* handle function pointer */
	else if ((afl == TRUE) && (strstr(arg, "(*") != NULL))
	   {nstrncpy(wty, nc, arg, -1);
	    knd = FP_FNC;}

/* follow the PACT function pointer PF convention */
	else if (is_func_ptr(ty, 2) == TRUE)
	   {nstrncpy(wty, nc, ty, -1);
	    knd = FP_FNC;}

	else if (pty != NULL)
	   {if (LAST_CHAR(ty) == '*')
	       {knd = FP_ARRAY;

/* for C dereference the type so that we have an array of type */
		if (mode == MODE_C)
		   deref(wty, BFLRG, pty);

/* for Fortran account for the -A convention used in the type registry */
		else if (mode == MODE_F)
		   {nstrncpy(wty, nc, pty, -1);
		    wty[strlen(wty)-2] = '\0';}

/* do not yet know to do anything but take the type for other bindings */
		else
		   nstrncpy(wty, nc, pty, -1);}
	    else
	       {knd = FP_SCALAR;
		nstrncpy(wty, nc, pty, -1);};}

/* unknown type */
	else
	   {nstrncpy(wty, nc, ty, -1);

	    if (LAST_CHAR(ty) == '*')
	       knd = FP_INDIRECT;
	    else
	       berr("Unknown type '%s'", ty);};

	al->knd = knd;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FC_DECL_LIST - render the arg list of DCL into A for the
 *              - Fortran callable C wrapper
 */

static void fc_decl_list(char *a, int nc, fdecl *dcl)
   {int i, na, nca, voida;
    fparam knd;
    char *ty, *nm, *arg, **cargs;
    farg *al;

    na    = dcl->na;
    al    = dcl->al;
    voida = dcl->voida;

    a[0] = '\0';
    if ((na == 0) || (voida == TRUE))
       nstrcat(a, BFLRG, "void");
    else
       {for (i = 0; i < na; i++)
	    {nm  = al[i].name;
	     ty  = al[i].type;
             arg = al[i].arg;
	     knd = al[i].knd;

	     switch (knd)
	        {case FP_FNC :
		      vstrcat(a, BFLRG, "%s, ", arg);
 		      break;
		 case FP_ARRAY :
		      if ((dcl->nc == 0) && (strcmp(ty, "char *") == 0))
			 dcl->cargs = lst_push(dcl->cargs, nm);
		      vstrcat(a, BFLRG, "%s%s, ", ty, nm);
		      break;
		 case FP_SCALAR :
		      vstrcat(a, BFLRG, "%s *%s, ", ty, nm);
		      break;
		 case FP_INDIRECT :
		      vstrcat(a, BFLRG, "%s *%s, ", ty, nm);
		      break;
		 default :
		      printf("Unknown type: %s\n", ty);
		      break;};

	   al[i].knd = knd;};

	cargs = dcl->cargs;
	nca   = lst_length(cargs);
        for (i = 0; i < nca; i++)
	    vstrcat(a, BFLRG, "%s snc%s, ", istrl, cargs[i]);
	if (dcl->nc == 0)
	   dcl->nc = nca;

        a[strlen(a) - 2] = '\0';};

    nstrncpy(a, nc, subst(a, "* ", "*", -1), -1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_FORTRAN - initialize Fortran wrapper file */

static void init_fortran(statedes *st, bindes *bd)
   {int nc, cfl;
    char fn[BFLRG], hf[BFLRG], ufn[BFLRG], fill[BFLRG];
    char *pck;
    FILE *fp;

    fp  = NULL;
    cfl = st->cfl;
    pck = st->pck;

    if (cfl & 1)
       {if ((st->path == NULL) || (strcmp(st->path, ".") == 0))
	   snprintf(fn, BFLRG, "gf-%s.c", pck);
	 else
	   snprintf(fn, BFLRG, "%s/gf-%s.c", st->path, pck);
	nstrncpy(ufn, BFLRG, fn, -1);
	upcase(ufn);

	nc = strlen(fn);
	memset(fill, ' ', nc);
	fill[nc] = '\0';

	fp = open_file("w", fn);

	fprintf(fp, "/*\n");
	fprintf(fp, " * %s - F90 wrappers for %s\n", ufn, pck);
	fprintf(fp, " * %s - NOTE: this file was automatically generated\n", fill);
	fprintf(fp, " * %s - any manual changes will not be effective\n", fill);
	fprintf(fp, " *\n");
	fprintf(fp, " */\n");
	fprintf(fp, "\n");

#if 1
	fprintf(fp, "#include \"cpyright.h\"\n");

	snprintf(hf, BFLRG, "%s_int.h", pck);
	fprintf(fp, "#include \"sx_int.h\"\n");
	if (file_exists(hf) == TRUE)
	   fprintf(fp, "#include \"%s\"\n", hf);
#else
	fprintf(fp, "#include \"cpyright.h\"\n");
	fprintf(fp, "#include \"sx_int.h\"\n");
	fprintf(fp, "#include \"%s_int.h\"\n", pck);
#endif

	fprintf(fp, "\n");
	csep(fp);};

    bd->fp = fp;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FORTRAN_WRAP_DECL - function declaration */

static void fortran_wrap_decl(FILE *fp, fdecl *dcl,
			      fparam knd, char *rt, char *cfn, char *ffn)
   {char ucn[BFLRG], dcn[BFLRG], a[BFLRG], t[BFLRG];

    map_name(dcn, BFLRG, cfn, ffn, "f", -1, FALSE, FALSE);

    nstrncpy(ucn, BFLRG, dcn, -1);
    upcase(ucn);

    fc_decl_list(a, BFLRG, dcl);

    fprintf(fp, "\n");
    fprintf(fp, "/* WRAP |%s| */\n", dcl->proto.arg);
    fprintf(fp, "\n");

#if defined(FORTRAN_BIND_C_ALL)
    switch (knd)
       {case FP_INDIRECT :

#ifdef RETURN_INTEGER
	     snprintf(t, BFLRG, "FIXNUM %s(%s)\n",
		      dcn, a);
#else
	     snprintf(t, BFLRG, "void *%s(%s)\n",
		      dcn, a);
#endif
	     break;
        case FP_ARRAY :
	     snprintf(t, BFLRG, "%s *%s(%s)\n",
		      rt, dcn, a);
	     break;
        case FP_SCALAR :
	     snprintf(t, BFLRG, "%s %s(%s)\n",
		      rt, dcn, a);
	     break;
        default :
	     snprintf(t, BFLRG, "%s %s(%s)\n",
		      rt, dcn, a);
	     break;};

#else

    switch (knd)
       {case FP_INDIRECT :

#ifdef RETURN_INTEGER
	     snprintf(t, BFLRG, "FIXNUM FF_ID(%s, %s)(%s)\n",
		      dcn, ucn, a);
#else
	     snprintf(t, BFLRG, "void *FF_ID(%s, %s)(%s)\n",
		      dcn, ucn, a);
#endif
	     break;
        case FP_ARRAY :
	     snprintf(t, BFLRG, "%s *FF_ID(%s, %s)(%s)\n",
		      rt, dcn, ucn, a);
	     break;
        case FP_SCALAR :
	     snprintf(t, BFLRG, "%s FF_ID(%s, %s)(%s)\n",
		      rt, dcn, ucn, a);
	     break;
        default :
	     snprintf(t, BFLRG, "%s FF_ID(%s, %s)(%s)\n",
		      rt, dcn, ucn, a);
	     break;};
#endif

    fputs(subst(t, "* ", "*", -1), fp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FORTRAN_WRAP_LOCAL_DECL - local variable declarations */

static void fortran_wrap_local_decl(FILE *fp, fdecl *dcl,
				    fparam knd, char *rt)
   {int i, na, nv, voida, voidf;
    char t[BFLRG];
    char *nm, *ty;
    farg *al;

    na    = dcl->na;
    al    = dcl->al;
    voida = dcl->voida;
    voidf = dcl->voidf;

    nv = 0;
    for (i = 0; i <= na; i++)
        {nm = al[i].name;
	 if ((voida == TRUE) && (i == 0))
	    continue;

	 if (nv == 0)
	    snprintf(t, BFLRG, "   {");
	 else
	    snprintf(t, BFLRG, "    ");

/* variable for return value */
	 if (i == na)
	    {if (voidf == FALSE)
		{switch (knd)
		    {case FP_INDIRECT :
#ifdef RETURN_INTEGER
		          nstrcat(t, BFLRG, "FIXNUM _rv;\n");
			  vstrcat(t, BFLRG, "    SC_address _ad%s;\n",
				  dcl->proto.name);
#else
		          nstrcat(t, BFLRG, "void *_rv;\n");
#endif
			  break;
		     case FP_ARRAY :
			  vstrcat(t, BFLRG, "%s *_rv;\n", rt);
			  break;
		     case FP_SCALAR :
		          vstrcat(t, BFLRG, "%s _rv;\n", rt);
			  break;
		     default :
		          vstrcat(t, BFLRG, "%s _rv;\n", rt);
			  break;};};}

/* local vars */
	 else if (nm[0] != '\0')
	    {ty = al[i].type;
	     switch (al[i].knd)
		{case FP_FNC :
                      if (nv > 0)
			 t[0] = '\0';
		      else
			 nstrcat(t, BFLRG, "\n");
		      break;
		 case FP_INDIRECT :
		      vstrcat(t, BFLRG, "%s _l%s;\n", ty, nm);
		      break;
		 case FP_ARRAY :
		      if (strcmp(ty, "char *") == 0)
			 vstrcat(t, BFLRG, "char _l%s[MAXLINE];\n", nm);
		      else
			 vstrcat(t, BFLRG, "%s_l%s;\n", ty, nm);
		      break;
		 case FP_SCALAR :
		      vstrcat(t, BFLRG, "%s _l%s;\n", ty, nm);
		      break;
		 default :
		      vstrcat(t, BFLRG, "%s _l%s;\n", ty, nm);
		      break;};
	     nv++;};

	 if (IS_NULL(t) == FALSE)
	    fputs(subst(t, "* ", "*", -1), fp);};

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FORTRAN_WRAP_LOCAL_ASSN - local variable assignments */

static void fortran_wrap_local_assn(FILE *fp, fdecl *dcl)
   {int i, na, nv;
    fparam knd;
    char t[BFLRG];
    char *nm, *ty;
    farg *al;

    na = dcl->na;
    al = dcl->al;

    nv = 0;
    for (i = 0; i < na; i++)
        {nm = al[i].name;
	 if (nm[0] != '\0')
	    {ty  = al[i].type;
	     knd = al[i].knd;
	     switch (knd)
	        {case FP_FNC :
		      t[0] = '\0';
		      break;
		 case FP_INDIRECT :
		      snprintf(t, BFLRG, "    _l%-8s = *(%s*) %s;\n",
			       nm, ty, nm);
		      break;
		 case FP_ARRAY :
		      if (strcmp(ty, "char *") == 0)
			 snprintf(t, BFLRG,
				  "    SC_FORTRAN_STR_C(_l%s, %s, snc%s);\n",
				  nm, nm, nm);
		      else
			 snprintf(t, BFLRG, "    _l%-8s = (%s) %s;\n",
				  nm, ty, nm);
		      break;
		 case FP_SCALAR :
		      snprintf(t, BFLRG, "    _l%-8s = (%s) *%s;\n",
			       nm, ty, nm);
		      break;
		 default :
		      snprintf(t, BFLRG, "    _l%-8s = (%s) *%s;\n",
			       nm, ty, nm);
		      break;};

	     if (IS_NULL(t) == FALSE)
	        {nv++;
		 fputs(t, fp);};};};

    if (nv > 0)
       fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FORTRAN_WRAP_LOCAL_CALL - function call */

static void fortran_wrap_local_call(FILE *fp, fdecl *dcl,
				    fparam knd, char *rt)
   {int voidf;
    char a[BFLRG];
    char *nm;

    cf_call_list(a, BFLRG, dcl, FALSE);

    nm    = dcl->proto.name;
    voidf = dcl->voidf;

    if (voidf == FALSE)
       {switch (knd)
	   {case FP_INDIRECT :
#ifdef RETURN_INTEGER
	         fprintf(fp, "    _ad%s.memaddr = (void *) %s(%s);\n",
			 nm, nm, a);
#else
	         fprintf(fp, "    _rv = (void *) %s(%s);\n",
			 nm, a);
#endif
		 break;
	    default :
	         fprintf(fp, "    _rv = %s(%s);\n", nm, a);
		 break;};}
    else
       fprintf(fp, "    %s(%s);\n", nm, a);

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FORTRAN_WRAP_LOCAL_RETURN - emit the return */

static void fortran_wrap_local_return(FILE *fp, fdecl *dcl, fparam knd)
   {int voidf;

    voidf = dcl->voidf;

    if (voidf == FALSE)
       {switch (knd)
	   {case FP_INDIRECT :
#ifdef RETURN_INTEGER
	         fprintf(fp, "    _rv = _ad%s.diskaddr;\n\n", dcl->proto.name);
#endif
	         fprintf(fp, "    return(_rv);}\n");
		 break;
	    default :
	         fprintf(fp, "    return(_rv);}\n");
		 break;};}
    else
       fprintf(fp, "    return;}\n");

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FORTRAN_WRAP - wrap C function PRO in FORTRAN callable function
 *              - using name FFN
 */

static void fortran_wrap(FILE *fp, fdecl *dcl, char *ffn)
   {fparam knd;
    char rt[BFLRG];
    char *cfn, *fn;

    cfn = dcl->proto.name;

    if (is_var_arg(dcl->proto.arg) == TRUE)
       {if ((strcmp(ffn, "yes") == 0) || (strcmp(ffn, "none") == 0))
	   fn = cfn;
        else
	   fn = ffn;
	berr("%s is not interoperable - variable args", fn);}

    else if (strcmp(ffn, "none") != 0)
       {fc_type(rt, BFLRG, &dcl->proto, FALSE, MODE_C);

	knd = dcl->proto.knd;

	csep(fp);

/* function declaration */
	fortran_wrap_decl(fp, dcl, knd, rt, cfn, ffn);

/* local variable declarations */
        fortran_wrap_local_decl(fp, dcl, knd, rt);

/* local variable assignments */
	fortran_wrap_local_assn(fp, dcl);

/* function call */
	fortran_wrap_local_call(fp, dcl, knd, rt);

/* return */
        fortran_wrap_local_return(fp, dcl, knd);

	csep(fp);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BIND_FORTRAN - generate Fortran bindings from CPR and SBI
 *              - return TRUE iff successful
 */

static int bind_fortran(bindes *bd)
   {int ib, rv, ndcl;
    char *ffn;
    fdecl *dcl, *dcls;
    statedes *st;
    FILE *fp;

    rv = TRUE;
    st = bd->st;

    if (st->cfl & 1)
       {fp = bd->fp;

	dcls = st->dcl;
	ndcl = st->ndcl;
	for (ib = 0; ib < ndcl; ib++)
	    {dcl = dcls + ib;
	     ffn = has_binding(dcl, "fortran");
	     if (ffn != NULL)
	        fortran_wrap(fp, dcl, ffn);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIN_FORTRAN - finalize Fortran file */

static void fin_fortran(bindes *bd)
   {FILE *fp;

    fp = bd->fp;

    csep(fp);
    fclose_safe(fp);

    bd->fp = NULL;

    return;}

/*--------------------------------------------------------------------------*/

/*                         FORTRAN MODULE ROUTINES                          */

/*--------------------------------------------------------------------------*/

/* MC_TYPE - return FORTRAN type, FTY, C cognate type, CTY,
 *         - corresponding to C type T
 *
 *    Type	Named constant	        C type or types
 *    INTEGER
 *    	C_INT			int, signed int
 *      C_SHORT			short int, signed short int
 *      C_LONG			long int, signed long int
 *      C_LONG_LONG		long long int, signed long long int
 *      C_SIGNED_CHAR		signed char, unsigned char
 *      C_SIZE_T		size_t
 *    	C_INT_LEAST8_T		int_least8_t
 *    	C_INT_LEAST16_T		int_least16_t
 *    	C_INT_LEAST32_T		int_least32_t
 *    	C_INT_LEAST64_T		int_least64_t
 *    	C_INT_FAST8_T		int_fast8_t
 *    	C_INT_FAST16_T		int_fast16_t
 *    	C_INT_FAST32_T		int_fast32_t
 *    	C_INT_FAST64_T		int_fast64_t
 *    	C_INTMAX_T		c intmax_t
 *    
 *    REAL
 *    	C_FLOAT			float, float _Imaginary
 *    	C_DOUBLE		double, double _Imaginary
 *    
 *    COMPLEX
 *    	C_LONG_DOUBLE		long double, long double _Imaginary
 *    	C_COMPLEX		_Complex
 *    	C_FLOAT_COMPLEX		float _Complex
 *    	C_DOUBLE_COMPLEX	double _Complex
 *    	C_LONG_DOUBLE_COMPLEX  	long double _Complex
 *    
 *    LOGICAL
 *    	C_BOOL			_Bool
 *    
 *    CHARACTER
 *    	C_CHAR			char
 */

static void mc_type(int nc, char *fty, char *cty,
		    char *wty, farg *arg)
   {char lfty[BFLRG], lcty[BFLRG], lwty[BFLRG];
    char *ty;

    if (arg != NULL)
       {fc_type(lwty, nc, arg, FALSE, MODE_F);
	memmove(lwty, subst(lwty, "FIXNUM", "integer", -1), nc);

	ty = arg->type;
	if (is_var_arg(ty) == TRUE)
	   {nstrncpy(lfty, BFLRG, "error", -1);
	    nstrncpy(lcty, BFLRG, "error", -1);}

/* follow the PACT function pointer PF convention */
	else if (is_func_ptr(ty, 3) == TRUE)
	   {nstrncpy(lfty, BFLRG, "type", -1);
	    nstrncpy(lcty, BFLRG, "C_FUNPTR", -1);}

	else if (is_ptr(ty) == TRUE)
	   {nstrncpy(lfty, BFLRG, "type", -1);
	    nstrncpy(lcty, BFLRG, "C_PTR", -1);}

	else if (strstr(ty, "long long") != NULL)
	   {nstrncpy(lfty, BFLRG, "integer", -1);
	    nstrncpy(lcty, BFLRG, "C_LONG_LONG", -1);}

	else if (strstr(ty, "long") != NULL)
	   {nstrncpy(lfty, BFLRG, "integer", -1);
	    nstrncpy(lcty, BFLRG, "C_LONG", -1);}

	else if (strstr(ty, "short") != NULL)
	   {nstrncpy(lfty, BFLRG, "integer", -1);
	    nstrncpy(lcty, BFLRG, "C_SHORT", -1);}

	else if (strstr(ty, "int") != NULL)
	   {nstrncpy(lfty, BFLRG, "integer", -1);
	    nstrncpy(lcty, BFLRG, "C_INT", -1);}

	else if (strstr(ty, "signed char") != NULL)
	   {nstrncpy(lfty, BFLRG, "integer", -1);
	    nstrncpy(lcty, BFLRG, "C_SIGNED_CHAR", -1);}

	else if (strstr(ty, "size_t") != NULL)
	   {nstrncpy(lfty, BFLRG, "integer", -1);
	    nstrncpy(lcty, BFLRG, "C_SIZE_T", -1);}

	else if (strstr(ty, "long double _Complex") != NULL)
	   {nstrncpy(lfty, BFLRG, "complex", -1);
	    nstrncpy(lcty, BFLRG, "C_LONG_DOUBLE_COMPLEX", -1);}

	else if (strstr(ty, "double _Complex") != NULL)
	   {nstrncpy(lfty, BFLRG, "complex", -1);
	    nstrncpy(lcty, BFLRG, "C_DOUBLE_COMPLEX", -1);}

	else if (strstr(ty, "float _Complex") != NULL)
	   {nstrncpy(lfty, BFLRG, "complex", -1);
	    nstrncpy(lcty, BFLRG, "C_FLOAT_COMPLEX", -1);}

	else if (strstr(ty, "long double") != NULL)
	   {nstrncpy(lfty, BFLRG, "real", -1);
	    nstrncpy(lcty, BFLRG, "C_LONG_DOUBLE", -1);}

	else if (strstr(ty, "double") != NULL)
	   {nstrncpy(lfty, BFLRG, "real", -1);
	    nstrncpy(lcty, BFLRG, "C_DOUBLE", -1);}

	else if (strstr(ty, "float") != NULL)
	   {nstrncpy(lfty, BFLRG, "real", -1);
	    nstrncpy(lcty, BFLRG, "C_FLOAT", -1);}

	else if (strstr(ty, "_Bool") != NULL)
	   {nstrncpy(lfty, BFLRG, "logical", -1);
	    nstrncpy(lcty, BFLRG, "C_BOOL", -1);}

	else if (strcmp(ty, "void") == 0)
	   {nstrncpy(lfty, BFLRG, "subroutine", -1);
	    nstrncpy(lcty, BFLRG, "C_VOID", -1);}

/* take unknown types to be integer - covers enums */
	else
	   {nstrncpy(lfty, BFLRG, "integer", -1);
	    nstrncpy(lcty, BFLRG, "C_INT", -1);};

	if (fty != NULL)
	   nstrncpy(fty, nc, lfty, -1);

	if (cty != NULL)
	   nstrncpy(cty, nc, lcty, -1);

	if (wty != NULL)
	   nstrncpy(wty, nc, lwty, -1);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MC_PROTO_LIST - return list of tokens for type and name of argument
 *               - caller formats and frees list
 */

static char **mc_proto_list(fdecl *dcl)
   {int i, na, voida;
    fparam knd;
    char wty[BFLRG];
    char **lst, *nm;
    farg *al;

    lst = NULL;

    na    = dcl->na;
    al    = dcl->al;
    voida = dcl->voida;

    if (al != NULL)

/* function/return type */
       {mc_type(BFLRG, NULL, NULL, wty, &dcl->proto);
	nm  = dcl->proto.name;
	knd = dcl->proto.knd;
	switch (knd)
	   {case FP_FNC      :
	    case FP_INDIRECT :
	         lst = lst_push(lst, C_PTR_RETURN);
		 lst = lst_push(lst, nm);
		 break;
	    case FP_ARRAY :
	         if (strcmp(wty, "character") == 0)
		    {lst = lst_push(lst, "character(*)");
		     lst = lst_push(lst, nm);}
		 else if (strcmp(wty, "C_PTR") == 0)
		    {lst = lst_push(lst, C_PTR_RETURN);
		     lst = lst_push(lst, nm);}
		 else
		    {lst = lst_push(lst, wty);
		     lst = lst_push(lst, "%s(*)", nm);};
		 break;
	    case FP_SCALAR :
	    default        :
	         lst = lst_push(lst, wty);
		 lst = lst_push(lst, nm);
		 break;};

/* arguments */
	for (i = 0; i < na; i++)
	    {if ((voida == TRUE) && (i == 0))
		continue;

	      mc_type(BFLRG, NULL, NULL, wty, al+i);
	      nm  = al[i].name;
	      knd = al[i].knd;
	      switch (knd)
		 {case FP_FNC      :
		  case FP_INDIRECT :
		       lst = lst_push(lst, C_PTR_RETURN);
		       lst = lst_push(lst, nm);
		       break;
		  case FP_ARRAY :
		       if (strcmp(wty, "character") == 0)
			  {lst = lst_push(lst, "character(*)");
			   lst = lst_push(lst, nm);}
		       else if (strcmp(wty, "C_PTR") == 0)
			  {lst = lst_push(lst, C_PTR_RETURN);
			   lst = lst_push(lst, nm);}
		       else
			  {lst = lst_push(lst, wty);
			   lst = lst_push(lst, "%s(*)", nm);};
		       break;
		  case FP_SCALAR :
		  default        :
		       lst = lst_push(lst, wty);
		       lst = lst_push(lst, nm);
		       break;};};

	dcl->tfproto = lst;
	dcl->ntf     = lst_length(lst);};

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MC_NEED_PTR - return TRUE if the package types module will be needed */

static int mc_need_ptr(fdecl *dcl)
   {int i, na, ok, voida;
    char cty[BFLRG];
    farg *al;

    na    = dcl->na;
    al    = dcl->al;
    voida = dcl->voida;

    ok = FALSE;

    mc_type(BFLRG, NULL, cty, NULL, &dcl->proto);
    if (dcl->proto.knd != FP_VARARG)
       ok |= ((strcmp(cty, "C_PTR") == 0) ||
	      (strcmp(cty, "C_FUNPTR") == 0));

    if ((na > 0) && (al != NULL) && (voida == FALSE))
       {for (i = 0; i < na; i++)
	    {mc_type(BFLRG, NULL, cty, NULL, al+i);
	     if (al[i].knd != FP_VARARG)
	        ok |= ((strcmp(cty, "C_PTR") == 0) ||
		       (strcmp(cty, "C_FUNPTR") == 0));};};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MC_DECL_LIST - render the arg list of DCL into A for the
 *              - Fortran/C interoperability module interface
 */

static void mc_decl_list(char *a, int nc, fdecl *dcl)
   {int i, na, voida;
    char *nm;
    farg *al;

    na    = dcl->na;
    al    = dcl->al;
    voida = dcl->voida;

    a[0] = '\0';
    if ((na > 0) && (voida == FALSE))
       {for (i = 0; i < na; i++)
	    {nm = al[i].name;

	     vstrcat(a, BFLRG, "%s, ", nm);};

        a[strlen(a) - 2] = '\0';};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_MODULE - initialize Fortran/C interoperatbility module file */

static void init_module(statedes *st, bindes *bd)
   {int nc, cfl;
    char fn[BFLRG], ufn[BFLRG], fill[BFLRG];
    char *pck;
    FILE *fp;

    fp = NULL;

    cfl = st->cfl;
    pck = st->pck;

    if (cfl & 2)
       {if ((st->path == NULL) || (strcmp(st->path, ".") == 0))
	   snprintf(fn, BFLRG, "gm-%s.f", pck);
	 else
	   snprintf(fn, BFLRG, "%s/gm-%s.f", st->path, pck);
	nstrncpy(ufn, BFLRG, fn, -1);
	upcase(ufn);

	nc = strlen(fn);
	memset(fill, ' ', nc);
	fill[nc] = '\0';

	fp = open_file("w", fn);

	fprintf(fp, "!\n");
	fprintf(fp, "! %s - F90 interfaces for %s\n", ufn, pck);
	fprintf(fp, "! %s - NOTE: this file was automatically generated\n", fill);
	fprintf(fp, "! %s - any manual changes will not be effective\n", fill);
	fprintf(fp, "!\n");
	fprintf(fp, "\n");

/* if there are no interfaces there is no need for the types module */
	fprintf(fp, "%smodule types_%s\n", ind, pck);
	fprintf(fp, "%s   integer, parameter :: isizea = %d\n", ind,
		(int) sizeof(char *));
	fprintf(fp, "%send module types_%s\n", ind, pck);
	fprintf(fp, "\n");

	fprintf(fp, "%smodule pact_%s\n", ind, pck);
	fprintf(fp, "%s   use iso_c_binding\n", ind);
	fprintf(fp, "\n");};

    bd->fp = fp;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MODULE_NATIVE_F - write interface for native Fortran function */

static char **module_native_f(FILE *fp, fdecl *dcl, char **sa, char *pck)
   {int i, voidf;
    char *oper, *p;

/* skip past any leading blank lines */
    for ( ; sa != NULL; sa++)
        {p = *sa;
	 if (p == NULL)
	    return(NULL);
	 else if (IS_NULL(p) == FALSE)
	    break;};

    oper = NULL;

    for (i = 0 ; sa != NULL; sa++, i++)
        {p = *sa;

	 if (IS_NULL(p) == TRUE)
	    {sa++;
	     break;};

	 if (i == 0)
	    {fprintf(fp, "%s%s\n", ind, p);
	     voidf = (strstr(p, "subroutine") != NULL);
	     oper  = (voidf == TRUE) ? "subroutine" : "function";

	     fprintf(fp, "%s         use iso_c_binding\n", ind);
	     fprintf(fp, "%s         implicit none\n", ind);}
	 else
	    fprintf(fp, "%s   %s\n", ind, p);};

    if (oper != NULL)
       fprintf(fp, "%s      end %s\n", ind, oper);

    fprintf(fp, "\n");

    return(sa);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MODULE_PRE_WRAPPABLE - return TRUE iff the function described by PR
 *                      - can be fully wrapped as opposed to being declared
 *                      - external
 */

static int module_pre_wrappable(char *pr)
   {int rv;

    rv = ((is_var_arg(pr) == FALSE) &&
	  (strstr(pr, "integer(isizea)") == NULL) &&
	  (strstr(pr, "type(C_PTR)") == NULL) &&
	  (strstr(pr, "external") == NULL));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MODULE_PRE_WRAP_EXT - write the Fortran interface for hand written
 *                     - prototype PR
 *                     - for functions that can only be declared external
 */

static void module_pre_wrap_ext(FILE *fp, char *pr, char **ta, char *pck)
   {

    if ((ta != NULL) && (module_pre_wrappable(pr) == FALSE))
       {if (strcmp(ta[0], "void") == 0)
	   fprintf(fp, "%s   external :: %s\n", ind, ta[1]);
	else
	   fprintf(fp, "%s   %s, external :: %s\n", ind, ta[0], ta[1]);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MODULE_ENUM_DECL - write the Fortran interface C enums DV */

static void module_enum_decl(FILE *fp, char *dv, char **ta, char *pck)
   {int i;
    char s[BFLRG];

/* syntax:
 *    enum, bind(C)
 *       ENUMERATOR :: <e1> [ = <v1>], ...
 *    end enum
 */

    if ((ta != NULL) && (strcmp(ta[0], "enum") == 0))
       {fprintf(fp, "   enum, bind(C)\n");

	s[0] = '\0';
	for (i = 2; ta[i] != NULL; i++)
	    vstrcat(s, BFLRG, "%s ", ta[i]);

	fprintf(fp, "      ENUMERATOR :: %s\n", strtok(s, "{};"));
	fprintf(fp, "   end enum\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MODULE_STRUCT_DECL - write the Fortran interface C structs DV */

static void module_struct_decl(FILE *fp, char *dv, char **ta, char *pck)
   {

    if (ta != NULL)
       {};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MODULE_PRE_WRAP_FULL - write the Fortran interface for hand written
 *                      - prototype PR
 *                      - for functions that can only be fully declared
 */

static void module_pre_wrap_full(FILE *fp, char *pr, char **ta, char *pck)
   {int i, nt, voidf;
    char a[BFLRG], t[BFLRG];
    char *rty, *oper, *dcn;

    if ((ta != NULL) && (module_pre_wrappable(pr) == TRUE))
       {nt = lst_length(ta);

	rty   = ta[0];
	dcn   = ta[1];
	voidf = (strcmp(rty, "void") == 0);
	oper  = (voidf == TRUE) ? "subroutine" : "function";

	a[0] = '\0';
        for (i = 3; i < nt; i += 2)
	    vstrcat(a, BFLRG, "%s, ", ta[i]);
	if (nt > 2)
	   a[strlen(a) - 2] = '\0';

	snprintf(t, BFLRG, "%s      %s %s(%s)", ind, oper, dcn, a);
	femit(fp, t, "");

	fprintf(fp, "%s         use iso_c_binding\n", ind);
	fprintf(fp, "%s         use types_%s\n", ind, pck);
	fprintf(fp, "%s         implicit none\n", ind);
	if (voidf == FALSE)
	   fprintf(fp, "%s         %-12s :: %s\n", ind, rty, dcn);

	for (i = 2; i < nt; i += 2)
	    {if (strcmp(ta[i], "character") == 0)
		fprintf(fp, "%s         %s(*) :: %s\n", ind, ta[i], ta[i+1]);
             else if (strcmp(ta[i], "integer-A") == 0)
                fprintf(fp, "%s         %-12s :: %s(*)\n", ind, "integer", ta[i+1]);
             else if (strcmp(ta[i], "real8-A") == 0)
                fprintf(fp, "%s         %-12s :: %s(*)\n", ind, "real*8", ta[i+1]);
             else
                fprintf(fp, "%s         %-12s :: %s\n", ind, ta[i], ta[i+1]);};

	fprintf(fp, "%s      end %s %s\n", ind, oper, dcn);
	fprintf(fp, "\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MODULE_ITF_WRAPPABLE - return TRUE iff the function described by DCL
 *                      - can be fully wrapped as opposed to being declared
 *                      - external
 */

static int module_itf_wrappable(fdecl *dcl)
   {int rv;
    char *pr;

    pr = dcl->proto.arg;
    rv = ((is_var_arg(pr) == FALSE) && (strstr(pr, "void *") == NULL));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MODULE_ITF_WRAP_EXT - write the interface for a simple extern */

static void module_itf_wrap_ext(FILE *fp, fdecl *dcl, char *pck, char *ffn)
   {char dcn[BFLRG], fty[BFLRG], cty[BFLRG];
    char *cfn, *rty;
    static int first = TRUE;

    cfn = dcl->proto.name;

/* declare the incomplete ones as external */
    if ((module_itf_wrappable(dcl) == FALSE) && (strcmp(ffn, "none") != 0))
       {map_name(dcn, BFLRG, cfn, ffn, "f", -1, FALSE, FALSE);

	if (mc_need_ptr(dcl) == TRUE)
	   {if (first == TRUE)
	       {first = FALSE;
		fprintf(fp, "%s   use types_%s\n", ind, pck);
		fprintf(fp, "\n");};};

	rty = dcl->proto.type;
	mc_type(BFLRG, fty, cty, NULL, &dcl->proto);
	if (strcmp(rty, "void") == 0)
	   fprintf(fp, "%s   external :: %s\n", ind, dcn);
        else
	  {if ((strcmp(cty, "C_PTR") == 0) || (strcmp(cty, "C_FUNPTR") == 0))
	      fprintf(fp, "%s   %s, external :: %s\n",
		      ind, C_PTR_RETURN, dcn);
	   else
	     fprintf(fp, "%s   %s, external :: %s\n", ind, fty, dcn);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MODULE_ITF_WRAP_FULL - write the Fortran interface for
 *                      - C function CFN
 */

static void module_itf_wrap_full(FILE *fp, fdecl *dcl, char *pck, char *ffn)
   {int i, ns, voidf;
    char dcn[BFLRG], a[BFLRG], t[BFLRG];
    char fty[BFLRG], cty[BFLRG];
    char *cfn, *oper, **args;

    cfn = dcl->proto.name;

/* emit complete declarations */
    if ((module_itf_wrappable(dcl) == TRUE) && (strcmp(ffn, "none") != 0))
       {map_name(dcn, BFLRG, cfn, ffn, "f", -1, FALSE, FALSE);

	voidf = dcl->voidf;
	oper  = (voidf == TRUE) ? "subroutine" : "function";

	mc_type(BFLRG, fty, cty, NULL, &dcl->proto);
	mc_decl_list(a, BFLRG, dcl);

#if defined(FORTRAN_BIND_C_ALL)
	snprintf(t, BFLRG, "%s      %s %s(%s)", ind, oper, dcn, a);
	femit(fp, t, "&");

	snprintf(t, BFLRG, "bind(c)");
	fprintf(fp, "%s                %s\n", ind, t);
#else
	snprintf(t, BFLRG, "%s      %s %s(%s)", ind, oper, dcn, a);
	femit(fp, t, "");
#endif
	fprintf(fp, "%s         use iso_c_binding\n", ind);
	if (mc_need_ptr(dcl) == TRUE)
	   fprintf(fp, "%s         use types_%s\n", ind, pck);

	fprintf(fp, "%s         implicit none\n", ind);

/* return value declaration */
	if (voidf == FALSE)
	   {if ((strcmp(cty, "C_PTR") == 0) ||
		(strcmp(cty, "C_FUNPTR") == 0))
	       snprintf(t, BFLRG, "%s         %-15s :: %s\n",
			ind, C_PTR_RETURN, dcn);
	    else
	       snprintf(t, BFLRG, "%s         %-15s :: %s\n", ind, fty, dcn);

	    fputs(t, fp);};

/* argument declarations */
	args = dcl->tfproto;
	ns   = dcl->ntf;
	for (i = 2; i < ns; i += 2)
	    {snprintf(t, BFLRG, "%s         %-15s :: %s\n",
		      ind, args[i], args[i+1]);
	     fputs(t, fp);};

	fprintf(fp, "%s      end %s %s\n", ind, oper, dcn);

	fprintf(fp, "\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MODULE_INTEROP_WRAP - generate FORTRAN/C interoperability interface for
 *                     - C function CFN
 */

static void module_interop_wrap(FILE *fp, fdecl *dcl, char *ffn)
   {int i, na, voidf, voida;
    char dcn[BFLRG], a[BFLRG];
    char fty[BFLRG], cty[BFLRG];
    char cd[BFLRG], cb[BFLRG];
    char *cfn, *nm, *oper;
    farg *al;

    cfn = dcl->proto.name;

    if (is_var_arg(dcl->proto.arg) == TRUE)
       berr("%s is not interoperable - variable args", cfn);

    else
       {map_name(dcn, BFLRG, cfn, ffn, "i", -1, FALSE, FALSE);

	na    = dcl->na;
	al    = dcl->al;
	voida = dcl->voida;
	voidf = dcl->voidf;
	oper  = (voidf == TRUE) ? "subroutine" : "function";

	mc_type(BFLRG, fty, cty, NULL, &dcl->proto);
	mc_decl_list(a, BFLRG, dcl);

	snprintf(cd, BFLRG, "%s      %s %s(%s)", ind, oper, dcn, a);
	femit(fp, cd, "&");

	snprintf(cb, BFLRG, "bind(c, name='%s')", cfn);
	fprintf(fp, "%s                %s\n", ind, cb);

	fprintf(fp, "%s         use iso_c_binding\n", ind);
	fprintf(fp, "%s         implicit none\n", ind);
	if (voidf == FALSE)
	   fprintf(fp, "%s         %s(%s) :: %s\n", ind, fty, cty, dcn);

/* argument declarations */
	for (i = 0; i < na; i++)
	    {if ((voida == TRUE) && (i == 0))
	        continue;

	     mc_type(BFLRG, fty, cty, NULL, al+i);
	     nm = al[i].name;
	     fprintf(fp, "%s         %s(%s), value :: %s\n",
		     ind, fty, cty, nm);};

	fprintf(fp, "%s      end %s %s\n", ind, oper, dcn);
	fprintf(fp, "\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BIND_MODULE - generate Fortran/C interoperability interface
 *             - from CPR and SBI
 *             - return TRUE iff successful
 */

static int bind_module(bindes *bd)
   {int ib, id, iw, rv, ndcl;
    char t[BFLRG];
    char *ffn, *sb, *pck;
    char **fpr, **fwr, **cdv, **sa, **ta;;
    fdecl *dcl, *dcls;
    statedes *st;
    FILE *fp;

    rv = TRUE;
    st = bd->st;

    if (st->cfl & 2)
       {fp   = bd->fp;
	pck  = st->pck;
	fpr  = st->fpr;
	fwr  = st->fwr;
	dcls = st->dcl;
	cdv  = st->cdv;
	ndcl = st->ndcl;

	dcl = NULL;

/* make simple external declaration for variable argument functions */
	fprintf(fp, "! ... external declarations for generated wrappers\n");
	for (ib = 0; ib < ndcl; ib++)
	    {dcl = dcls + ib;
	     ffn = has_binding(dcl, "fortran");
	     if (ffn != NULL)
	        module_itf_wrap_ext(fp, dcl, pck, ffn);};

	fprintf(fp, "\n");

/* make declarations for enums */
	if (cdv != NULL)
	   {int nbi;
	    char ps[BFLRG];
	    char *te, **sbi;

	    sbi = st->sbi;
	    nbi = st->nbi;

	    fprintf(fp, "! ... C enum declarations\n");
            for (ib = 0; ib < nbi; ib++)
	        {nstrncpy(ps, BFLRG, sbi[ib], -1);
		 if ((strncmp(ps, "derived", 7) == 0) &&
		     (strstr(ps, "SC_ENUM_I") != NULL))
		    {te = strtok(ps, " ");
		     te = strtok(NULL, " ");
		     for (id = 0; cdv[id] != NULL; id++)
		         {sb = cdv[id];
			  if (blank_line(sb) == FALSE)
			     {nstrncpy(t, BFLRG, sb, -1);
			      ta = tokenize(t, " \t", 0);
			      if (strcmp(ta[1]+2, te) == 0)
				 module_enum_decl(fp, sb, ta, pck);
			      free_strings(ta);};};};};
	    fprintf(fp, "\n");

	    fprintf(fp, "! ... C struct declarations\n");
            for (ib = 0; ib < nbi; ib++)
	        {nstrncpy(ps, BFLRG, sbi[ib], -1);
		 if ((strncmp(ps, "derived", 7) == 0) &&
		     (strstr(ps, "SC_ENUM_I") == NULL))
		    {te = strtok(ps, " ");
		     te = strtok(NULL, " ");
		     for (id = 0; cdv[id] != NULL; id++)
		         {sb = cdv[id];
			  if (blank_line(sb) == FALSE)
			     {nstrncpy(t, BFLRG, sb, -1);
			      ta = tokenize(t, " \t", 0);
			      if (strcmp(ta[1]+2, te) == 0)
				 module_struct_decl(fp, sb, ta, pck);
			      free_strings(ta);};};};};
	    fprintf(fp, "\n");};

/* make external declarations for variable argument pre-wrapped functions */
	if (fwr != NULL)
	   {fprintf(fp, "! ... external declarations for old wrappers\n");
	    for (iw = 0; fwr[iw] != NULL; iw++)
	        {sb = fwr[iw];
		 if (blank_line(sb) == FALSE)
		    {nstrncpy(t, BFLRG, sb, -1);
		     ta = tokenize(t, " \t", 0);
		     module_pre_wrap_ext(fp, sb, ta, pck);
		     free_strings(ta);};};};

	fprintf(fp, "\n");

/* start the interface */
	fprintf(fp, "%s   interface\n", ind);
	fprintf(fp, "\n");

/* make interface for native Fortran functions */
	if (fpr != NULL)
	   {fprintf(fp, "! ... declarations for native Fortran functions\n");
	    for (iw = 0, sa = fpr; TRUE; iw++)
	        {sa = module_native_f(fp, dcl, sa, pck);
		 if (sa == NULL)
		    break;};};

/* make full interface for non-variable argument functions */
	fprintf(fp, "! ... declarations for generated wrappers\n");
	for (ib = 0; ib < ndcl; ib++)
	    {dcl = dcls + ib;
	     ffn = has_binding(dcl, "fortran");
	     if (ffn != NULL)
	        module_itf_wrap_full(fp, dcl, pck, ffn);};

/* make full interface for non-variable argument pre-wrapped functions */
	if (fwr != NULL)
	   {fprintf(fp, "! ... declarations for old wrappers\n");
	    for (iw = 0; fwr[iw] != NULL; iw++)
	        {sb = fwr[iw];
		 if (blank_line(sb) == FALSE)
		    {nstrncpy(t, BFLRG, sb, -1);
		     ta = tokenize(t, " \t", 0);
		     module_pre_wrap_full(fp, sb, ta, pck);
		     free_strings(ta);};};};

/* make full interface for non-variable argument functions */
	fprintf(fp, "! ... declarations for interoperability\n");
	for (ib = 0; ib < ndcl; ib++)
	    {dcl = dcls + ib;
	     ffn = has_binding(dcl, "fortran");
	     if (ffn != NULL)
	        module_interop_wrap(fp, dcl, ffn);};

/* finish the interface */
	fprintf(fp, "%s   end interface\n", ind);
	fprintf(fp, "\n");};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIN_MODULE - finalize Fortran/C interoperability interface module file */

static void fin_module(bindes *bd)
   {char *pck;
    statedes *st;
    FILE *fp;

    fp  = bd->fp;
    st  = bd->st;
    pck = st->pck;

    fprintf(fp, "%send module pact_%s\n", ind, pck);
    fprintf(fp, "\n");

    fclose_safe(fp);

    bd->fp = NULL;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REGISTER_FORTRAN - register FORTRAN binding methods */

static int register_fortran(int fl, statedes *st)
   {int nb;
    bindes *pb;

    if (fl == TRUE)
       {nb = nbd;

	pb = gbd + nbd++;
	pb->st   = st;
	pb->fp   = NULL;
	pb->init = init_fortran;
	pb->bind = bind_fortran;
	pb->fin  = fin_fortran;

	pb = gbd + nbd++;
	pb->st   = st;
	pb->fp   = NULL;
	pb->init = init_module;
	pb->bind = bind_module;
	pb->fin  = fin_module;};

    return(nb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

