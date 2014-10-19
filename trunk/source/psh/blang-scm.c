/*
 * BLANG-SCM.C - generate SCHEME language bindings
 *
 * #include "cpyright.h"
 *
 */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CS_TYPE - return "Scheme" type corresponding to C type TY
 *         - to make SS_args call
 */

static void cs_type(char *a, int nc, farg *al, int drf)
   {char t[BFLRG], ty[BFLRG];
    char *sty, *dty;

    if (drf == TRUE)
       deref(ty, BFLRG, al->type);
    else
       nstrncpy(ty, BFLRG, al->type, -1);

    sty = lookup_type(NULL, ty, MODE_C, MODE_S);
    if (IS_NULL(sty) == FALSE)
       nstrncpy(a, nc, sty, -1);

    else if (strcmp(ty, "void *") == 0)
       nstrncpy(a, nc, "SC_VOID_I", -1);

    else if (is_func_ptr(ty, 3) == TRUE)
       nstrncpy(a, nc, "SC_POINTER_I", -1);

    else if (strchr(ty, '*') != NULL)
       {deref(t, BFLRG, ty);
	dty = lookup_type(NULL, t, MODE_C, MODE_S);
	if (dty != NULL)
	   nstrncpy(a, nc, dty, -1);
	else
	   nstrncpy(a, nc, "SC_POINTER_I", -1);}

    else
       nstrncpy(a, nc, "SC_UNKNOWN_I", -1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SO_TYPE - return the Scheme object constructor for C type TY */

static fparam so_type(char *a, int nc, char *ty)
   {fparam rv;

    rv = FP_SCALAR;

    if (strcmp(ty, "char") == 0)
       nstrncpy(a, nc, "SS_mk_char", -1);
       
    else if (strcmp(ty, "char *") == 0)
       nstrncpy(a, nc, "SS_mk_string", -1);
       
    else if (strcmp(ty, "bool") == 0)
       nstrncpy(a, nc, "SS_mk_boolean", -1);
       
    else if ((strcmp(ty, "short") == 0) ||
	     (strcmp(ty, "int") == 0) ||
	     (strcmp(ty, "long") == 0) ||
	     (strcmp(ty, "long long") == 0) ||
	     (strcmp(ty, "int16_t") == 0) ||
	     (strcmp(ty, "int32_t") == 0) ||
	     (strcmp(ty, "int64_t") == 0))
       nstrncpy(a, nc, "SS_mk_integer", -1);
       
    else if ((strcmp(ty, "float") == 0) ||
	     (strcmp(ty, "double") == 0) ||
	     (strcmp(ty, "long double") == 0))
       nstrncpy(a, nc, "SS_mk_float", -1);
       
    else if ((strcmp(ty, "float _Complex") == 0) ||
	     (strcmp(ty, "double _Complex") == 0) ||
	     (strcmp(ty, "long double _Complex") == 0))
       nstrncpy(a, nc, "SS_mk_complex", -1);
       
    else if (is_ptr(ty) == TRUE)
       {rv = FP_ARRAY;
	nstrncpy(a, nc, "SX_mk_C_array", -1);}

/* handle unknown types with a mostly graceful failure */
    else
       {rv = FP_ANY;
	nstrncpy(a, nc, "none", -1);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_SCHEME - initialize Scheme file */

static void init_scheme(statedes *st, bindes *bd)
   {char fn[BFLRG];
    char *pck;
    FILE *fp;

    pck = st->pck;

    if ((st->path == NULL) || (strcmp(st->path, ".") == 0))
       snprintf(fn, BFLRG, "gs-%s.c", pck);
    else
       snprintf(fn, BFLRG, "%s/gs-%s.c", st->path, pck);

    fp = open_file("w", fn);

    fprintf(fp, "\n");
    fprintf(fp, "#include \"cpyright.h\"\n");
    fprintf(fp, "#include \"sx_int.h\"\n");
    fprintf(fp, "#include \"%s_int.h\"\n", pck);
    fprintf(fp, "\n");

    csep(fp);

    bd->fp = fp;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_WRAP_DECL - function declaration */

static void scheme_wrap_decl(FILE *fp, fdecl *dcl)
   {int na;
    char dcn[BFLRG];

    na = dcl->na;

    nstrncpy(dcn, BFLRG, dcl->proto.name, -1);
    downcase(dcn);

    fprintf(fp, "\n");
    fprintf(fp, "/* WRAP |%s| */\n", dcl->proto.arg);
    fprintf(fp, "\n");

    fprintf(fp, "static object *");
    fprintf(fp, "_SXI_%s", dcn);
    if (na == 0)
       fprintf(fp, "(SS_psides *si)");
    else
       fprintf(fp, "(SS_psides *si, object *argl)");

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_WRAP_LOCAL_DECL - local variable declarations */

static void scheme_wrap_local_decl(FILE *fp, fdecl *dcl,
				   fparam knd, char *so)
   {int i, na, nv, voida, voidf;
    char t[BFLRG];
    char *rty;
    farg *al;
    idecl *ip;

    na    = dcl->na;
    al    = dcl->al;
    voida = dcl->voida;
    voidf = dcl->voidf;
    rty   = dcl->proto.type;
	
    nv = 0;
    for (i = 0; i <= na; i++)
        {if ((voida == TRUE) && (i == 0))
	    continue;

	 ip = &al[i].interp;

	 if (nv == 0)
	    snprintf(t, BFLRG, "   {");
	 else
	    snprintf(t, BFLRG, "    ");

/* variable for return value */
	 if (i == na)
	    {if (voidf == FALSE)
	        {vstrcat(t, BFLRG, "%s _rv;\n    ", rty);
		 if (knd == FP_ARRAY)
		    {nstrcat(t, BFLRG, "long _sz;\n");
		     nstrcat(t, BFLRG, "    C_array *_arr;\n    ");};};

	     nstrcat(t, BFLRG, "object *_lo;\n");}

/* local vars */
	 else if (IS_NULL(ip->decl) == FALSE)
           {nstrcat(t, BFLRG, ip->decl);
            nv++;};

	 if (IS_NULL(t) == FALSE)
	    fputs(subst(t, "* ", "*", -1), fp);};

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_WRAP_LOCAL_ASSN_DEF - assign default values to local variable AL */

static void scheme_wrap_local_assn_def(FILE *fp, farg *al)
   {char *defa;
    idecl *ip;

    ip   = &al->interp;
    defa = ip->defa;

    fputs(defa, fp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_WRAP_LOCAL_ASSN_ARG - add AL to SS_args call argument list */

static void scheme_wrap_local_assn_arg(char *a, int nc, farg *al)
   {char *arg;
    idecl *ip;

    ip  = &al->interp;
    arg = ip->argi;

    nstrcat(a, nc, arg);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_WRAP_LOCAL_ASSN - local variable assignments */

static void scheme_wrap_local_assn(FILE *fp, fdecl *dcl)
   {int i, na, voida;
    char a[BFLRG];
    farg *al;

    voida = dcl->voida;

    if (voida == FALSE)
       {na = dcl->na;
	al = dcl->al;

/* set the default values */
	for (i = 0; i < na; i++)
	    scheme_wrap_local_assn_def(fp, al+i);

/* make the SS_args call */
	a[0] = '\0';
	for (i = 0; i < na; i++)
	    scheme_wrap_local_assn_arg(a, BFLRG, al+i);

	fprintf(fp, "    SS_args(si, argl,\n");
	fprintf(fp, "%s", a);
	fprintf(fp, "            0);\n");
	fprintf(fp, "\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_WRAP_LOCAL_CALL - function call */

static void scheme_wrap_local_call(FILE *fp, fdecl *dcl)
   {char a[BFLRG], t[BFLRG];
    char *ty, *nm;

    ty = dcl->proto.type;
    nm = dcl->proto.name;

    cf_call_list(a, BFLRG, dcl, TRUE);

    if (strcmp(ty, "void") == 0)
       snprintf(t, BFLRG, "    %s(%s);\n", nm, a);
    else
       snprintf(t, BFLRG, "    _rv = %s(%s);\n", nm, a);

    fputs(t, fp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_ARRAY_RETURN - compute possible array return from DCL */

static void scheme_array_return(char *t, int nc, fdecl *dcl)
   {int i, iv, na, nr, nvl;
    fdir dir;
    char a[BFLRG], dty[BFLRG];
    char *nm;
    farg *al;

    t[0] = '\0';
    a[0] = '\0';

    nr = dcl->nr;

/* make up the list arguments */
    if (nr > 0)
       {na = dcl->na;
	al = dcl->al;
        for (i = 0; i < na; i++)
	    {nvl = al[i].nv;
	     dir = al[i].dir;
	     if ((nvl > 0) && ((dir == FD_OUT) || (dir == FD_IN_OUT)))
	        {nm = al[i].name;
		 cs_type(dty, BFLRG, al+i, TRUE);
	     
		 if (nvl == 1)
		    vstrcat(a, BFLRG,
			    "                       %s, &_l%s,\n",
			    dty, nm);
		 else
		    {for (iv = 0; iv < nvl; iv++)
			 vstrcat(a, BFLRG,
				 "                       %s, &_l%s[%d],\n",
				 dty, nm, iv);};};};};

/* if the list argument are non empty make up the call */
    if (IS_NULL(a) == FALSE)
       {snprintf(t, nc, "    _lo = SS_make_list(si,\n");
	nstrcat(t, nc, a);
	vstrcat(t, nc, "                       0);\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_SCALAR_RETURN - compute scalar return from DCL */

static void scheme_scalar_return(char *t, int nc,
				 fdecl *dcl, fparam knd, char *so)
   {char dty[BFLRG];
    char *ty, *sty;

    t[0] = '\0';

    ty = dcl->proto.type;

    if (strcmp(ty, "void") == 0)
       snprintf(t, nc, "    _lo = SS_f;\n");
    else
       {if (IS_NULL(so) == FALSE)
	   {switch (knd)
	       {case FP_ANY :
		     sty = lookup_type(NULL, ty, MODE_C, MODE_S);
		     if ((sty != NULL) && (strcmp(sty, "SC_ENUM_I") == 0))
		        snprintf(t, nc, "    _lo = SS_mk_integer(si, _rv);\n");
		     else
		        {snprintf(t, nc,
				  "\n/* no way to return '%s' */\n", ty);
			 nstrcat(t, nc, "    _lo = SS_null;\n");};
		     break;

	        case FP_ARRAY :
		     deref(dty, BFLRG, ty);
		     if (strcmp(dty, "void") == 0)
		        {snprintf(t, nc,
				  "    _sz = SC_arrlen(_rv);\n");
			 vstrcat(t, nc,
				 "    _arr = PM_make_array(\"char\", _sz, _rv);\n");}
		     else
		        {snprintf(t, nc,
				  "    _sz = SC_arrlen(_rv)/sizeof(%s);\n",
				  dty);
			 vstrcat(t, nc,
				 "    _arr = PM_make_array(\"%s\", _sz, _rv);\n",
				 dty);};
		     vstrcat(t, nc, "    _lo  = %s(si, _arr);\n", so);
		     break;
	        default :
                     if (strcmp(ty, "bool") == 0)
		        snprintf(t, nc, "    _lo = %s(si, \"g\", (int) _rv);\n",
				 so);
		     else
		        snprintf(t, nc, "    _lo = %s(si, _rv);\n", so);
		     break;};}
	else
	   snprintf(t, nc, "    _lo = SS_null;\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_WRAP_LOCAL_RETURN - function return */

static void scheme_wrap_local_return(FILE *fp, fdecl *dcl,
				     fparam knd, char *so)
   {char t[BFLRG];

    scheme_array_return(t, BFLRG, dcl);
    if (IS_NULL(t) == TRUE)
       scheme_scalar_return(t, BFLRG, dcl, knd, so);

    fputs(t, fp);

    fprintf(fp, "\n");

    fprintf(fp, "    return(_lo);}\n");
    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_ENUM_DEFS - write the SCHEME interface C enums DV */

static void scheme_enum_defs(FILE *fp, char *dv, char **ta, char *pck)
   {

/* syntax:
 *    _SS_make_ext_int(si, <Enamei>, <Evaluei>);
 */

    if (ta == NULL)
       {if (strcmp(dv, "begin") == 0)
	   {}
        else if (strcmp(dv, "end") == 0)
	   fprintf(fp, "\n");}

    else if (strcmp(ta[0], "enum") == 0)
       {int i;
	long vl;
	char *vr;

	vl = 0;
	for (i = 2; ta[i] != NULL; )
            {vr = strtok(ta[i++], "{,;}");
	     if ((ta[i] != NULL) && (ta[i][0] == '='))
	        {i++;
	         vl = stoi(ta[i++]);}
	     else
	        vl++;

	     fprintf(fp, "    _SS_make_ext_int(si, \"%s\", %ld);\n",
	             vr, vl);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_STRUCT_DEFS - write the SCHEME interface C structs DV */

static void scheme_struct_defs(FILE *fp, char *dv, char **ta, char *pck)
   {

/* syntax:
 *    PD_defstr(SX_vif, <Sname>,
 *              "<Mtype1> <Mname1>",
 *                    ...
 *              "<Mtypen> <Mnamen>",
 *              LAST);
 */

    if (ta == NULL)
       {if (strcmp(dv, "begin") == 0)
	   {}
        else if (strcmp(dv, "end") == 0)
	   fprintf(fp, "\n");}

    else if (strcmp(ta[0], "struct") == 0)
       {int i;
	char *p, *mbr;

	p = trim(ta[0]+9, BOTH, " \t");
	fprintf(fp, "    PD_defstr(SX_vif, \"%s\",\n", p);

	for (i = 2; ta[i] != NULL; i++)
	    {mbr = trim(ta[i], BOTH, " \t");

/* function pointer */
	     p = strstr(mbr, "(*");
	     if (p != NULL)
	        fprintf(fp, "              \"function %s\",\n",
			strtok(p+2, ")"));
	     else
	        fprintf(fp, "              \"%s\",\n", mbr);};

	fprintf(fp, "              LAST);\n");
	fprintf(fp, "\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_WRAP_INSTALL - add the installation of the function */

static char **scheme_wrap_install(char **fl, fdecl *dcl, char *sfn,
				  char **com)
   {int voida;
    char a[BFLRG], t[BFLRG], dcn[BFLRG], ifn[BFLRG];
    char *pi;

    voida = dcl->voida;

    nstrncpy(dcn, BFLRG, dcl->proto.name, -1);
    downcase(dcn);

/* make the scheme interpreter name */
    pi = (strcmp(sfn, "yes") == 0) ? dcl->proto.name : sfn;
    map_name(ifn, BFLRG, pi, sfn, NULL, -1, TRUE, FALSE);

/* prepare the function inline documenation */
    t[0] = '\0';
    if (com != NULL)
       concatenate(t, BFLRG, com, 0, -1, " ");

    if (IS_NULL(t) == TRUE)
       {if_call_list(a, BFLRG, dcl, NULL);
	if (voida == FALSE)
	   snprintf(t, BFLRG, "Procedure: %s\\n     Usage: (%s %s)",
		    ifn, ifn, a);
	else
	   snprintf(t, BFLRG, "Procedure: %s\\n     Usage: (%s)",
		    ifn, ifn);};

/* add the installation of the function */
    if (voida == FALSE)
       snprintf(a, BFLRG, 
		"    SS_install(si, \"%s\",\n               \"%s\",\n               SS_nargs,\n               _SXI_%s, SS_PR_PROC);\n\n",
		ifn, t, dcn);
    else
       snprintf(a, BFLRG, 
		"    SS_install(si, \"%s\",\n               \"%s\",\n               SS_zargs,\n               _SXI_%s, SS_PR_PROC);\n\n",
		ifn, t, dcn);

    fl = lst_add(fl, a);

    return(fl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_WRAP - wrap C function DCL in SCHEME callable function
 *             - using name FFN
 */

static char **scheme_wrap(FILE *fp, char **fl, fdecl *dcl,
			  char *sfn, char **com)
   {fparam knd;
    char so[BFLRG];

    if ((is_var_arg(dcl->proto.arg) == FALSE) && (strcmp(sfn, "none") != 0))
       {csep(fp);

	knd = so_type(so, BFLRG, dcl->proto.type);
	
/* function declaration */
	scheme_wrap_decl(fp, dcl);

/* local variable declarations */
	scheme_wrap_local_decl(fp, dcl, knd, so);

/* local variable assignments */
	scheme_wrap_local_assn(fp, dcl);

/* function call */
	scheme_wrap_local_call(fp, dcl);

/* function return */
	scheme_wrap_local_return(fp, dcl, knd, so);

	csep(fp);

/* add the installation of the function */
	fl = scheme_wrap_install(fl, dcl, sfn, com);};

    return(fl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_INSTALL - write the routine to install the bindings */

static void scheme_install(bindes *bd, char **fl)
   {int i;
    FILE *fp;
    char *pck;
    statedes *st;

    fp  = bd->fp;
    st  = bd->st;
    pck = st->pck;

    csep(fp);

    fprintf(fp, "\n");
    fprintf(fp, "void SX_install_%s_bindings(SS_psides *si)\n", pck);
    fprintf(fp, "   {\n");
    fprintf(fp, "\n");

/* make the list of enum constants to install */
    emit_enum_defs(bd, scheme_enum_defs);

/* make the list of structs to install */
    emit_struct_defs(bd, scheme_struct_defs);

    if (fl != NULL)
       {for (i = 0; fl[i] != NULL; i++)
	    fputs(fl[i], fp);

	free_strings(fl);};

    fprintf(fp, "   return;}\n");
    fprintf(fp, "\n");
    csep(fp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BIND_SCHEME - generate Scheme bindings from CPR and SBI
 *             - return TRUE iff successful
 */

static int bind_scheme(bindes *bd)
   {int ib, ndcl, rv;
    char *sfn, **fl;
    fdecl *dcl, *dcls;
    statedes *st;
    FILE *fp;

    fp   = bd->fp;
    st   = bd->st;
    dcls = st->dcl;
    ndcl = st->ndcl;

    rv = TRUE;
    fl = NULL;

/* make the list of function wrappers to install */
    for (ib = 0; ib < ndcl; ib++)
        {dcl = dcls + ib;
	 sfn = has_binding(dcl, "scheme");
	 if (sfn != NULL)
	    fl = scheme_wrap(fp, fl, dcl, sfn, NULL);};

/* write the routine to install the bindings */
    scheme_install(bd, fl);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIN_SCHEME - finalize Scheme file */

static void fin_scheme(bindes *bd)
   {FILE *fp;

    fp = bd->fp;

    csep(fp);
    fclose_safe(fp);

    bd->fp = NULL;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REGISTER_SCHEME - register SCHEME binding methods */

static int register_scheme(int fl, statedes *st)
   {int nb;
    bindes *pb;

    if (fl == TRUE)
       {nb = nbd;

	pb = gbd + nbd++;
	pb->st   = st;
	pb->fp   = NULL;
	pb->init = init_scheme;
	pb->bind = bind_scheme;
	pb->fin  = fin_scheme;};

    return(nb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

