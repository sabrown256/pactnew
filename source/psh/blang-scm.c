/*
 * BLANG-SCM.C - generate SCHEME language bindings
 *
 * #include "cpyright.h"
 *
 */

typedef struct s_tns_list tns_list;

struct s_tns_list
   {char cnm[BFSML];        /* C struct name, PM_set */
    char snm[BFSML];        /* Scheme struct name, pm-set */
    char rnm[BFSML];        /* root struct id, SET */
    char lnm[BFSML];        /* lower case version of CNM, pm_set */
    char unm[BFSML];};      /* upper case version of CNM, PM_SET */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_TYPE_NAME_LIST - make canonical variations of type name TYP */

static void scheme_type_name_list(char *typ, tns_list *na)
   {char *p;

/* get C struct name */
    p = trim(typ, BOTH, " \t");
    nstrncpy(na->cnm, BFSML, p, -1);

/* upper case C name */
    nstrncpy(na->unm, BFSML, p, -1);
    upcase(na->unm);

/* lower case C name */
    nstrncpy(na->lnm, BFSML, p, -1);
    downcase(na->lnm);

/* make the Scheme name */
    nstrncpy(na->snm, BFSML, subst(na->lnm, "_", "-", -1), -1);

/* get root struct name */
    if (p[2] == '_')
       nstrncpy(na->rnm, BFSML, p+3, -1);
    else
       nstrncpy(na->rnm, BFSML, p, -1);
    upcase(na->rnm);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_GET_TYPE - get the Scheme type name for TY from text TX */

static void scheme_get_type(char *a, int nc, char *ty, char *tx)
   {tns_list tl;

    if (strcmp(ty, tykind[TK_ENUM]) == 0)
       nstrncpy(a, nc, "SC_ENUM_I", -1);

    else if (strcmp(ty, tykind[TK_STRUCT]) == 0)
       {scheme_type_name_list(tx, &tl);
	snprintf(a, nc, "SX_%s_I", tl.rnm);}

    else
       nstrncpy(a, nc, ty, -1);

    return;}

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
       scheme_get_type(a, nc, sty, ty);

    else if (strcmp(ty, "void *") == 0)
       nstrncpy(a, nc, "SC_VOID_I", -1);

    else if (is_func_ptr(ty, 3) == TRUE)
       nstrncpy(a, nc, "SC_POINTER_I", -1);

    else if (strchr(ty, '*') != NULL)
       {deref(t, BFLRG, ty);
	dty = lookup_type(NULL, t, MODE_C, MODE_S);
	if (dty != NULL)
	   scheme_get_type(a, nc, dty, t);
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
	nstrncpy(a, nc, "SX_make_c_array", -1);}

/* handle unknown types with a mostly graceful failure */
    else
       {rv = FP_ANY;
	nstrncpy(a, nc, "none", -1);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_SCHEME - initialize Scheme file */

static void init_scheme(statedes *st, bindes *bd)
   {char fn[BFLRG], upck[BFLRG];
    char *pck;
    FILE *fc, *fh;

    pck = st->pck;
    snprintf(upck, BFLRG, pck, -1);
    upcase(upck);

/* open C file */
    if ((st->path == NULL) || (strcmp(st->path, ".") == 0))
       snprintf(fn, BFLRG, "gs-%s.c", pck);
    else
       snprintf(fn, BFLRG, "%s/gs-%s.c", st->path, pck);

    fc = open_file("w", fn);
    bd->fp[0] = fc;

    fprintf(fc, "/*\n");
    fprintf(fc, " * GS-%s.C - generated support routines for %s\n",
	    upck, upck);
    fprintf(fc, " *\n");
    fprintf(fc, " */\n");
    fprintf(fc, "\n");

    fprintf(fc, "#include \"cpyright.h\"\n");
    fprintf(fc, "#include \"sx_int.h\"\n");
    fprintf(fc, "#include \"%s_int.h\"\n", pck);
    fprintf(fc, "#include \"gs-%s.h\"\n", pck);
    fprintf(fc, "\n");

    csep(fc);

/* open header file */
    if ((st->path == NULL) || (strcmp(st->path, ".") == 0))
       snprintf(fn, BFLRG, "gs-%s.h", pck);
    else
       snprintf(fn, BFLRG, "%s/gs-%s.h", st->path, pck);

    fh = open_file("w", fn);
    bd->fp[1] = fh;

    fprintf(fh, "/*\n");
    fprintf(fh, " * GS-%s.H - header containing generated support for %s\n",
	    upck, upck);
    fprintf(fh, " *\n");
    fprintf(fh, " */\n");
    fprintf(fh, "\n");

    fprintf(fh, "#include \"cpyright.h\"\n");
    fprintf(fh, "\n");
    fprintf(fh, "#ifndef GEN_SX_%s_H\n", upck);
    fprintf(fh, "#define GEN_SX_%s_H\n", upck);
    fprintf(fh, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_WRAP_DECL - function declaration */

static void scheme_wrap_decl(FILE *fc, fdecl *dcl)
   {int na;
    char dcn[BFLRG];

    na = dcl->na;

    nstrncpy(dcn, BFLRG, dcl->proto.name, -1);
    downcase(dcn);

    fprintf(fc, "\n");
    fprintf(fc, "/* WRAP |%s| */\n", dcl->proto.arg);
    fprintf(fc, "\n");

    fprintf(fc, "static object *");
    fprintf(fc, "_SXI_%s", dcn);
    if (na == 0)
       fprintf(fc, "(SS_psides *si)");
    else
       fprintf(fc, "(SS_psides *si, object *argl)");

    fprintf(fc, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_WRAP_LOCAL_DECL - local variable declarations */

static void scheme_wrap_local_decl(FILE *fc, fdecl *dcl,
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
	    fputs(subst(t, "* ", "*", -1), fc);};

    fprintf(fc, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_WRAP_LOCAL_ASSN_DEF - assign default values to local variable AL */

static void scheme_wrap_local_assn_def(FILE *fc, farg *al)
   {char *defa;
    idecl *ip;

    ip   = &al->interp;
    defa = ip->defa;

    fputs(defa, fc);

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

static void scheme_wrap_local_assn(FILE *fc, fdecl *dcl)
   {int i, na, voida;
    char a[BFLRG];
    farg *al;

    voida = dcl->voida;

    if (voida == FALSE)
       {na = dcl->na;
	al = dcl->al;

/* set the default values */
	for (i = 0; i < na; i++)
	    scheme_wrap_local_assn_def(fc, al+i);

/* make the SS_args call */
	a[0] = '\0';
	for (i = 0; i < na; i++)
	    scheme_wrap_local_assn_arg(a, BFLRG, al+i);

	fprintf(fc, "    SS_args(si, argl,\n");
	fprintf(fc, "%s", a);
	fprintf(fc, "            0);\n");
	fprintf(fc, "\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_WRAP_LOCAL_CALL - function call */

static void scheme_wrap_local_call(FILE *fc, fdecl *dcl)
   {char a[BFLRG], t[BFLRG];
    char *ty, *nm;

    ty = dcl->proto.type;
    nm = dcl->proto.name;

    cf_call_list(a, BFLRG, dcl, TRUE);

    if (strcmp(ty, "void") == 0)
       snprintf(t, BFLRG, "    %s(%s);\n", nm, a);
    else
       snprintf(t, BFLRG, "    _rv = %s(%s);\n", nm, a);

    fputs(t, fc);

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

static void scheme_wrap_local_return(FILE *fc, fdecl *dcl,
				     fparam knd, char *so)
   {char t[BFLRG];

    scheme_array_return(t, BFLRG, dcl);
    if (IS_NULL(t) == TRUE)
       scheme_scalar_return(t, BFLRG, dcl, knd, so);

    fputs(t, fc);

    fprintf(fc, "\n");

    fprintf(fc, "    return(_lo);}\n");
    fprintf(fc, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_ENUM_DEFS - write the SCHEME interface C enums DV */

static void scheme_enum_defs(FILE **fpa, char *dv, char **ta, char *pck)
   {FILE *fc;

    fc = fpa[0];

/* syntax:
 *    _SS_make_ext_int(si, <Enamei>, <Evaluei>);
 */

    if (ta == NULL)
       {if (strcmp(dv, "begin") == 0)
	   {csep(fc);
	    fprintf(fc, "\n");
	    fprintf(fc, "static void _SX_install_%s_consts(SS_psides *si)\n",
		    pck);
	    fprintf(fc, "   {\n");
	    fprintf(fc, "\n");}
        else if (strcmp(dv, "end") == 0)
	   {fprintf(fc, "\n");
	    fprintf(fc, "    return;}\n");
	    fprintf(fc, "\n");
	    csep(fc);};}

    else if (strcmp(ta[0], tykind[TK_ENUM]) == 0)
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

	     fprintf(fc, "    _SS_make_ext_int(si, \"%s\", %ld);\n",
	             vr, vl);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_HDR_STRUCT_DEF - emitter for struct defs info belonging
 *                       - in the header file
 */

static void scheme_hdr_struct_def(FILE *fh, char *dv, char **ta, char *pck)
   {int i;
    char lnm[BFSML], unm[BFSML], mnm[BFSML];
    char *p, *mbr;
    tns_list tl;

    scheme_type_name_list(ta[0]+9, &tl);

/* emit type check predicate macro */
    fprintf(fh, "#undef SX_%sP\n", tl.rnm);
    fprintf(fh, "#define SX_%sP(_o)   (SS_OBJECT_TYPE(_o) == SX_%s_I)\n",
	    tl.rnm, tl.rnm);
    fprintf(fh, "\n");

#if 0
    fprintf(fh, "#define %s(_o)   \t(SS_GET(%s, _o))\n",
	    tl.rnm, tl.cnm);
#endif

/* emit member accessor macros */
    for (i = 1; ta[i] != NULL; i++)
        {mbr = trim(ta[i], BOTH, " \t");
	 if (IS_NULL(mbr) == FALSE)
	    {nstrncpy(mnm, BFSML, mbr, -1);
	     p = strtok(mnm, "* \t");
	     p = strtok(NULL, "[(*)] \t");
	     nstrncpy(lnm, BFSML, p, -1);
	     nstrncpy(unm, BFSML, p, -1);
	     upcase(unm);

	     fprintf(fh, "#define %s_%s(_o)   \t(SS_GET(%s, _o)->%s)\n",
		     tl.rnm, unm, tl.cnm, lnm);};};
    fprintf(fh, "\n");

/* emit list extractor */
    fprintf(fh, "#define SX_GET_%s_FROM_LIST(_si, _v, _a, _s)   \\\n", tl.rnm);
    fprintf(fh, "   {obj = SS_car(_si, _a);   \\\n");
    fprintf(fh, "    _a = SS_cdr(_si, _a);   \\\n");
    fprintf(fh, "    _v = _SX_opt_%s(NULL, BIND_ARG, obj);   \\\n", tl.cnm);
    fprintf(fh, "    if (((char *) _v) == _SX.unresolved)   \\\n");
    fprintf(fh, "       SS_error(_si, _s, obj);}\n");
    fprintf(fh, "\n");

/* emit external declarations */
    fprintf(fh, "extern int SX_%s_I;\n", tl.rnm);
    fprintf(fh, "\n");

    fprintf(fh, "extern object *SX_make_%s(SS_psides *si, %s *x);\n",
	    tl.lnm, tl.cnm);
    fprintf(fh, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_C_STRUCT_DEF - emitter for struct defs info belonging
 *                     - in the C file
 */

static void scheme_c_struct_def(FILE *fc, char *dv, char **ta, char *pck)
   {int i;
    char *p, *mbr;
    tns_list tl;

    scheme_type_name_list(ta[0]+9, &tl);

/* emit registration with the SX VIF */
    fprintf(fc, "    dp = PD_defstr(SX_gs.vif, \"%s\",\n", tl.cnm);

    for (i = 2; ta[i] != NULL; i++)
        {mbr = trim(ta[i], BOTH, " \t");
	 if (IS_NULL(mbr) == FALSE)

/* function pointer */
	    {p = strstr(mbr, "(*");
	     if (p != NULL)
	        fprintf(fc, "                   \"function %s\",\n",
			strtok(p+2, ")"));
	     else
	        fprintf(fc, "                   \"%s\",\n", mbr);};};

    fprintf(fc, "                   LAST);\n");
    fprintf(fc, "    nerr += (dp == NULL);\n");
    fprintf(fc, "\n");

    fprintf(fc, "    SS_install(si, \"%s?\",\n", tl.snm);
    fprintf(fc, "               \"Returns #t if the object is a %s, and #f otherwise\",\n", tl.cnm);
    fprintf(fc, "               SS_sargs,\n");
    fprintf(fc, "               _SXI_%sp, SS_PR_PROC);\n", tl.lnm);
    fprintf(fc, "\n");

/* emit registration with the SCORE type system */
    fprintf(fc, "    SX_%s_I = SC_type_register(\"%s\", KIND_STRUCT, sizeof(%s),\n", tl.rnm, tl.cnm, tl.cnm);
    fprintf(fc, "              SC_TYPE_FREE, _SX_rl_%s,\n", tl.lnm);
    fprintf(fc, "              0);\n");
    fprintf(fc, "\n");

    fprintf(fc, "    SS_set_type_method(SX_%s_I,\n", tl.rnm);
    fprintf(fc, "		        \"C->Scheme\", SX_make_%s,\n", tl.lnm);
    fprintf(fc, "		        \"Scheme->C\", _SX_arg_%s,\n", tl.lnm);
    fprintf(fc, "		        NULL);\n");

#if 0
    fprintf(fc, "    ty    = _SC_get_type_id(SX_%s_I);\n", tl.rnm);
    fprintf(fc, "    alst  = ty->a;\n");
    fprintf(fc, "    alst  = SC_add_alist(alst, \"C->Scheme\", \"%s\", SX_make_%s);\n",
	    tl.unm, tl.lnm);
    fprintf(fc, "    alst  = SC_add_alist(alst, \"Scheme->C\", \"%s\", _SX_arg_%s);\n",
	    tl.unm, tl.lnm);
    fprintf(fc, "    ty->a = alst;\n");
#endif
    fprintf(fc, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_STRUCT_DEFS - write the SCHEME interface C structs DV */

static void scheme_struct_defs(FILE **fpa, char *dv, char **ta, char *pck)
   {FILE *fc, *fh;

    fc = fpa[0];
    fh = fpa[1];

/* syntax:
 *    PD_defstr(SX_vif, <Sname>,
 *              "<Mtype1> <Mname1>",
 *                    ...
 *              "<Mtypen> <Mnamen>",
 *              LAST);
 */

    if (ta == NULL)
       {if (strcmp(dv, "begin") == 0)
	   {csep(fc);
	    fprintf(fc, "\n");
	    fprintf(fc, "static int _SX_install_%s_derived(SS_psides *si)\n",
		    pck);
	    fprintf(fc, "   {int nerr;\n");
	    fprintf(fc, "    defstr *dp;\n");
#if 0
	    fprintf(fc, "    pcons *alst;\n");
	    fprintf(fc, "    SC_type *ty;\n");
#endif
	    fprintf(fc, "\n");
	    fprintf(fc, "    nerr = 0;\n");
	    fprintf(fc, "\n");}
        else if (strcmp(dv, "end") == 0)
	   {fprintf(fc, "    return(nerr);}\n");
	    fprintf(fc, "\n");
	    csep(fc);};}

    else if (strncmp(ta[0], "struct s_", 9) == 0)
       {scheme_hdr_struct_def(fh, dv, ta, pck);
	scheme_c_struct_def(fc, dv, ta, pck);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_OBJECT_DEFS - write the SCHEME interface to create
 *                    - SCHEME object from C structs
 */

static void scheme_object_defs(FILE **fpa, char *dv, char **ta, char *pck)
   {FILE *fc;
    tns_list tl;

    fc = fpa[0];

    if (ta == NULL)
       {if (strcmp(dv, "begin") == 0)
	   csep(fc);
        else if (strcmp(dv, "end") == 0)
	   csep(fc);}

    else if (strncmp(ta[0], "struct s_", 9) == 0)
       {scheme_type_name_list(ta[0]+9, &tl);

        csep(fc);
        fprintf(fc, "\n");

/* emit the object write method */
	fprintf(fc, "static void _SX_wr_%s(SS_psides *si, object *o, object *fp)\n",
		tl.lnm);
	fprintf(fc, "   {%s *x;\n", tl.cnm);
	fprintf(fc, "\n");
	fprintf(fc, "    x = SS_GET(%s, o);\n", tl.cnm);
	fprintf(fc, "\n");
	fprintf(fc, "    PRINT(SS_OUTSTREAM(fp), \"<%s|%%s>\", _SX_opt_%s(x, BIND_PRINT, NULL));\n",
		tl.rnm, tl.cnm);

	fprintf(fc, "\n");
	fprintf(fc, "    return;}\n");

        fprintf(fc, "\n");
        csep(fc);
        csep(fc);
        fprintf(fc, "\n");

/* emit the object release method */
	fprintf(fc, "static void _SX_rl_%s(SS_psides *si, object *o)\n",
		tl.lnm);
	fprintf(fc, "   {%s *x;\n", tl.cnm);
	fprintf(fc, "\n");
	fprintf(fc, "    x = SS_GET(%s, o);\n", tl.cnm);
	fprintf(fc, "\n");
	fprintf(fc, "    _SX_opt_%s(x, BIND_FREE, NULL);\n", tl.cnm);
	fprintf(fc, "\n");
	fprintf(fc, "    SS_rl_object(si, o);\n");
	fprintf(fc, "\n");
	fprintf(fc, "    return;}\n");

	fprintf(fc, "\n");
        csep(fc);
        csep(fc);
	fprintf(fc, "\n");

/* emit the object instantiation method */
	fprintf(fc, "int SX_%s_I;\n", tl.rnm);

	fprintf(fc, "\n");
	fprintf(fc, "object *SX_make_%s(SS_psides *si, %s *x)\n",
		tl.lnm, tl.cnm);
	fprintf(fc, "   {object *rv;\n");
	fprintf(fc, "\n");

	fprintf(fc, "    if (x == NULL)\n");
	fprintf(fc, "       rv = SS_null;\n");
	fprintf(fc, "    else\n");

/* use the BIND member to determine the print name */
	fprintf(fc, "       {char *nm;\n");
	fprintf(fc, "\n");
	fprintf(fc, "        _SX_opt_%s(x, BIND_ALLOC, NULL);\n",
		tl.cnm);
	fprintf(fc, "        nm = _SX_opt_%s(x, BIND_LABEL, NULL);\n",
		tl.cnm);
	fprintf(fc, "        rv = SS_mk_object(si, x, SX_%s_I, SELF_EV, nm,\n",
		tl.rnm);
	fprintf(fc, "                          _SX_wr_%s, _SX_rl_%s);}\n",
		tl.lnm, tl.lnm);

	fprintf(fc, "\n");
	fprintf(fc, "    return(rv);}\n");

	fprintf(fc, "\n");
        csep(fc);
        csep(fc);
	fprintf(fc, "\n");

	fprintf(fc, "void *_SX_arg_%s(SS_psides *si, object *o)\n", tl.lnm);
	fprintf(fc, "   {void *rv;\n");
	fprintf(fc, "\n");
	fprintf(fc, "    rv = _SX_opt_%s(NULL, BIND_ARG, o);\n", tl.cnm);
	fprintf(fc, "\n");
	fprintf(fc, "    if (rv == _SX.unresolved)\n");
	fprintf(fc, "       SS_error(si, \"OBJECT NOT %s - _SX_ARG_%s\", o);\n",
		tl.unm, tl.unm);
	fprintf(fc, "\n");
	fprintf(fc, "    return(rv);}\n");

	fprintf(fc, "\n");
        csep(fc);
        csep(fc);
	fprintf(fc, "\n");

	fprintf(fc, "static object *_SXI_%sp(SS_psides *si, object *o)\n",
		tl.lnm);
	fprintf(fc, "   {object *rv;\n");
	fprintf(fc, "\n");
	fprintf(fc, "    rv = SX_%sP(o) ? SS_t : SS_f;\n", tl.rnm);
	fprintf(fc, "\n");
	fprintf(fc, "    return(rv);}\n");

	fprintf(fc, "\n");
        csep(fc);};

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

static char **scheme_wrap(FILE *fc, char **fl, fdecl *dcl,
			  char *sfn, char **com)
   {fparam knd;
    char so[BFLRG];

    if ((is_var_arg(dcl->proto.arg) == FALSE) && (strcmp(sfn, "none") != 0))
       {csep(fc);

	knd = so_type(so, BFLRG, dcl->proto.type);
	
/* function declaration */
	scheme_wrap_decl(fc, dcl);

/* local variable declarations */
	scheme_wrap_local_decl(fc, dcl, knd, so);

/* local variable assignments */
	scheme_wrap_local_assn(fc, dcl);

/* function call */
	scheme_wrap_local_call(fc, dcl);

/* function return */
	scheme_wrap_local_return(fc, dcl, knd, so);

	csep(fc);

/* add the installation of the function */
	fl = scheme_wrap_install(fl, dcl, sfn, com);};

    return(fl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_INSTALL - write the routine to install the bindings */

static void scheme_install(bindes *bd, char **fl)
   {int i;
    FILE *fc;
    char *pck;
    statedes *st;

    fc  = bd->fp[0];
    st  = bd->st;
    pck = st->pck;

/* make the list of enum constants to install */
    emit_enum_defs(bd, scheme_enum_defs);

/* make the list of struct objects */
    emit_struct_defs(bd, scheme_object_defs);

/* make the list of structs to install */
    emit_struct_defs(bd, scheme_struct_defs);

    csep(fc);

    fprintf(fc, "\n");
    fprintf(fc, "void SX_install_%s_bindings(SS_psides *si)\n", pck);
    fprintf(fc, "   {\n");
    fprintf(fc, "\n");

/* make the list of enum constants to install */
    fprintf(fc, "    _SX_install_%s_consts(si);\n", pck);

/* make the list of structs to install */
    fprintf(fc, "    _SX_install_%s_derived(si);\n", pck);

    fprintf(fc, "\n");

    if (fl != NULL)
       {for (i = 0; fl[i] != NULL; i++)
	    fputs(fl[i], fc);

	free_strings(fl);};

    fprintf(fc, "   return;}\n");
    fprintf(fc, "\n");
    csep(fc);

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
    FILE *fc;

    fc   = bd->fp[0];
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
	    fl = scheme_wrap(fc, fl, dcl, sfn, NULL);};

/* write the routine to install the bindings */
    scheme_install(bd, fl);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIN_SCHEME - finalize Scheme file */

static void fin_scheme(bindes *bd)
   {int i;
    FILE *fc, *fh, *fp;

/* finish of the C file */
    fc = bd->fp[0];
    csep(fc);

/* finish of the header file */
    fh = bd->fp[1];
    fprintf(fh, "#endif\n");
    fprintf(fh, "\n");

    for (i = 0; i < NF; i++)
        {fp = bd->fp[i];
	 if (fp != NULL)
	    {fclose_safe(fp);
	     bd->fp[i] = NULL;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REGISTER_SCHEME - register SCHEME binding methods */

static int register_scheme(int fl, statedes *st)
   {int i, nb;
    bindes *pb;

    if (fl == TRUE)
       {nb = nbd;

	pb = gbd + nbd++;
	for (i = 0; i < NF; i++)
	    pb->fp[i] = NULL;

	pb->st   = st;
	pb->init = init_scheme;
	pb->bind = bind_scheme;
	pb->fin  = fin_scheme;};

    return(nb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

