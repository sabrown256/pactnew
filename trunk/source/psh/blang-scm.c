/*
 * BLANG-SCM.C - generate SCHEME language bindings
 *
 * #include "cpyright.h"
 *
 */

typedef struct s_tns_list tns_list;
typedef struct s_smeta smeta;

struct s_tns_list
   {char cnm[BFSML];        /* C struct name, PM_set */
    char lnm[BFSML];        /* lower case version of CNM, pm_set */
    char unm[BFSML];        /* upper case version of CNM, PM_SET */
    char rnm[BFSML];        /* root struct id, SET */

    char snm[BFSML];};      /* Scheme struct name, pm-set */

struct s_smeta
   {char **topt;};          /* type with _SX_opt_xxx methods */

static int
 MODE_S = -1;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_TYPE_NAME_LIST - make canonical variations of type name TYP */

static void scheme_type_name_list(tns_list *na, tn_list *nc)
   {

    *((tn_list *) na) = *nc;

/* make the Scheme name */
    nstrncpy(na->snm, BFSML, subst(na->lnm, "_", "-", -1), -1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_GET_TYPE - get the Scheme type name for TY from text TX */

static void scheme_get_type(char *a, int nc, char *ty, char *tx)
   {char s[BFSML];
    tns_list tl;
    tn_list tc;

    if (strcmp(ty, tykind[TK_ENUM]) == 0)
       nstrncpy(a, nc, "SC_ENUM_I", -1);

    else if (strcmp(ty, tykind[TK_STRUCT]) == 0)
       {snprintf(s, BFSML, "struct s_%s", tx);
	type_name_list(s, &tc);
	scheme_type_name_list(&tl, &tc);
	snprintf(a, nc, "G_%s_I", tl.rnm);}

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

    sty = lookup_type(NULL, NULL, ty, gbd+MODE_S);
    if (IS_NULL(sty) == FALSE)
       scheme_get_type(a, nc, sty, ty);

    else if (strcmp(ty, "void *") == 0)
       nstrncpy(a, nc, "SC_VOID_I", -1);

    else if (is_func_ptr(ty, 3) == TRUE)
       nstrncpy(a, nc, "SC_POINTER_I", -1);

    else if (strchr(ty, '*') != NULL)
       {deref(t, BFLRG, ty);
	dty = lookup_type(NULL, NULL, t, gbd+MODE_S);
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

    if (is_char(ty) == B_T)
       nstrncpy(a, nc, "SS_mk_char", -1);
       
    else if (is_string(ty) == B_T)
       nstrncpy(a, nc, "SS_mk_string", -1);
       
    else if (is_bool(ty) == B_T)
       nstrncpy(a, nc, "SS_mk_boolean", -1);
       
    else if (is_fixed_point(ty) == B_T)
       nstrncpy(a, nc, "SS_mk_integer", -1);
       
    else if (is_real(ty) == B_T)
       nstrncpy(a, nc, "SS_mk_float", -1);
       
    else if (is_complex(ty) == B_T)
       nstrncpy(a, nc, "SS_mk_complex", -1);
       
    else if (is_ptr(ty) == B_T)
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
   {int i;
    char fn[BFLRG], upck[BFLRG], s[BFMG];
    char *pck, *p, **tl;
    FILE *fc, *fh, *fp;
    smeta *sm;

    pck = st->pck;
    snprintf(upck, BFLRG, pck, -1);
    upcase(upck);

/* make the Scheme metadata from the opts file */
    tl = NULL;
    fp = open_file("r", "%s.opts", pck);
    if (fp != NULL)
       {for (i = 0; TRUE; i++)
	    {p = fgets(s, BFMG, fp);
	     if (p == NULL)
	        break;
	     LAST_CHAR(p) = '\0';
	     if (blank_line(p) == TRUE)
	        continue;
	     else
	        tl = lst_add(tl, p);};

	tl = lst_add(tl, NULL);

	fclose(fp);};

    sm = MAKE(smeta);
    sm->topt = tl;
    bd->data = sm;

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

	emit_local_var_init(fc, dcl);

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
		     sty = lookup_type(NULL, NULL, ty, gbd+MODE_S);
		     if ((sty != NULL) &&
			 (strcmp(sty, tykind[TK_ENUM]) == 0))
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

/* SCHEME_ENUM_DEFS - write the SCHEME interface C enums TAG */

static void scheme_enum_defs(bindes *bd, char *tag, der_list *el, int ni)
   {char *pck;
    FILE *fc, **fpa;
    statedes *st;

    fpa = bd->fp;

    fc  = fpa[0];
    st  = bd->st;
    pck = st->pck;

/* syntax:
 *    _SS_make_ext_int(si, <Enamei>, <Evaluei>);
 */

    if (el == NULL)
       {if (strcmp(tag, "begin") == 0)
	   {csep(fc);
	    fprintf(fc, "\n");
	    fprintf(fc, "static void _SX_install_%s_consts(SS_psides *si)\n",
		    pck);
	    fprintf(fc, "   {\n");
	    fprintf(fc, "\n");}
        else if (strcmp(tag, "end") == 0)
	   {fprintf(fc, "\n");
	    fprintf(fc, "    return;}\n");
	    fprintf(fc, "\n");
	    csep(fc);};}

    else if (el->kind == TK_ENUM)
       {int i;
	long vl;
	char *vr, **ta;

	ta = el->members;
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

static void scheme_hdr_struct_def(FILE *fh, der_list *sl, char *pck)
   {int i;
    char lnm[BFSML], unm[BFSML], mnm[BFSML];
    char *p, *mbr;
    tns_list tl;
    mbrdes *md;

    scheme_type_name_list(&tl, &sl->na);

/* emit type check predicate macro */
    fprintf(fh, "#undef SX_%sP\n", tl.rnm);
    fprintf(fh, "#define SX_%sP(_o)   (SS_OBJECT_TYPE(_o) == G_%s_I)\n",
	    tl.rnm, tl.rnm);
    fprintf(fh, "\n");

/* emit member accessor macros */
    md = sl->md;
    for (i = 0; md[i].text != NULL; i++)
        {mbr = md[i].text;
	 nstrncpy(mnm, BFSML, mbr, -1);
	 p = strtok(mnm, "* \t");
	 p = strtok(NULL, "[(*)] \t");
	 nstrncpy(lnm, BFSML, p, -1);
	 nstrncpy(unm, BFSML, p, -1);
	 upcase(unm);

	 fprintf(fh, "#define %s_%s(_o)   \t(SS_GET(%s, _o)->%s)\n",
		 tl.rnm, unm, tl.cnm, lnm);};
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
    fprintf(fh, "extern object *SX_make_%s(SS_psides *si, %s *x);\n",
	    tl.lnm, tl.cnm);
    fprintf(fh, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_C_STRUCT_DEF - emitter for struct defs info belonging
 *                     - in the C file
 */

static void scheme_c_struct_def(FILE *fc, der_list *sl, char *pck)
   {tns_list tl;

    scheme_type_name_list(&tl, &sl->na);

/* emit registration with the SX VIF */
    fprintf(fc, "    nerr &= G_DEFINE_%s(SX_gs.vif);\n", tl.rnm);
    fprintf(fc, "\n");

    fprintf(fc, "    SS_install(si, \"%s?\",\n", tl.snm);
    fprintf(fc, "               \"Returns #t if the object is a %s, and #f otherwise\",\n", tl.cnm);
    fprintf(fc, "               SS_sargs,\n");
    fprintf(fc, "               _SXI_%sp, SS_PR_PROC);\n", tl.lnm);
    fprintf(fc, "\n");

    fprintf(fc, "    SS_set_type_method(G_%s_I,\n", tl.rnm);
    fprintf(fc, "                       \"C->Scheme\", SX_make_%s,\n", tl.lnm);
    fprintf(fc, "                       \"Scheme->C\", _SX_arg_%s,\n", tl.lnm);
    fprintf(fc, "                       NULL);\n");
    fprintf(fc, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_STRUCT_DEFS - write the SCHEME interface C structs TAG */

static void scheme_struct_defs(bindes *bd, char *tag, der_list *sl, int ni)
   {char *pck;
    FILE *fc, *fh, **fpa;
    statedes *st;

    fpa = bd->fp;
    st  = bd->st;
    pck = st->pck;

    fc = fpa[0];
    fh = fpa[1];

    if (sl == NULL)
       {if (strcmp(tag, "begin") == 0)
	   {csep(fc);
	    fprintf(fc, "\n");
	    fprintf(fc, "static int _SX_install_%s_derived(SS_psides *si)\n",
		    pck);
	    fprintf(fc, "   {int nerr;\n");
	    fprintf(fc, "\n");
	    fprintf(fc, "    register_%s_types();\n", pck);
	    fprintf(fc, "\n");
	    fprintf(fc, "    nerr = TRUE;\n");
	    fprintf(fc, "\n");}
        else if (strcmp(tag, "end") == 0)
	   {fprintf(fc, "    return(nerr);}\n");
	    fprintf(fc, "\n");
	    csep(fc);};}

    else if (sl->kind == TK_STRUCT)
       {scheme_hdr_struct_def(fh, sl, pck);
	scheme_c_struct_def(fc, sl, pck);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_OBJECT_DEFS - write the SCHEME interface to create
 *                    - SCHEME object from C structs
 */

static void scheme_object_defs(bindes *bd, char *tag, der_list *sl, int ni)
   {int i, ok;
    char topt[BFSML];
    char **to;
    FILE *fc, **fpa;
    tns_list tl;
    smeta *sm;

    fpa = bd->fp;
    sm  = bd->data;
    to  = sm->topt;

    fc = fpa[0];

    if (sl == NULL)
       {if (strcmp(tag, "begin") == 0)
	   csep(fc);
        else if (strcmp(tag, "end") == 0)
	   csep(fc);}

    else if (sl->kind == TK_STRUCT)
       {scheme_type_name_list(&tl, &sl->na);

	ok = FALSE;
	if (to != NULL)
	   {for (i = 0; (to[i] != NULL) && (ok == FALSE); i++)
	        ok = (strcmp(tl.cnm, to[i]) == 0);};

	if (ok == TRUE)
	   snprintf(topt, BFSML, "_SX_opt_%s", tl.cnm);
	else
	   nstrncpy(topt, BFSML, "_SX_opt_generic", -1);

        csep(fc);
        fprintf(fc, "\n");

/* emit the object write method */
	fprintf(fc, "static void _SX_wr_%s(SS_psides *si, object *o, object *fp)\n",
		tl.lnm);
	fprintf(fc, "   {%s *x;\n", tl.cnm);
	fprintf(fc, "\n");
	fprintf(fc, "    x = SS_GET(%s, o);\n", tl.cnm);
	fprintf(fc, "\n");
	fprintf(fc, "    PRINT(SS_OUTSTREAM(fp), \"<%s|%%s>\", %s(x, BIND_PRINT, NULL));\n",
		tl.rnm, topt);

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
	fprintf(fc, "    %s(x, BIND_FREE, NULL);\n", topt);
	fprintf(fc, "\n");
	fprintf(fc, "    SS_rl_object(si, o);\n");
	fprintf(fc, "\n");
	fprintf(fc, "    return;}\n");

	fprintf(fc, "\n");
        csep(fc);
        csep(fc);
	fprintf(fc, "\n");

/* emit the object instantiation method */
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
	fprintf(fc, "        %s(x, BIND_ALLOC, NULL);\n", topt);
	fprintf(fc, "        nm = %s(x, BIND_LABEL, NULL);\n", topt);
	fprintf(fc, "        rv = SS_mk_object(si, x, G_%s_I, SELF_EV, nm,\n",
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
	fprintf(fc, "    rv = %s(NULL, BIND_ARG, o);\n", topt);
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

/* BIND_DOC_SCHEME - emit Scheme binding documentation */

static void bind_doc_scheme(FILE *fp, fdecl *dcl, doc_kind dk)
   {char as[BFLRG], dcn[BFLRG];
    char *bfn, *cfn;
    extern void doc_proto_name_only(char *a, int nc, fdecl *dcl, char *dlm);

    cfn = dcl->proto.name;
    bfn = has_binding(dcl, "scheme");

    if (dk == DK_HTML)
       {if (bfn == NULL)
	   fprintf(fp, "<i>SX Binding: </i>      none\n");
	else if (dcl->bindings != NULL)
	   {map_name(dcn, BFLRG, cfn, bfn, NULL, -1, TRUE, FALSE);
	    doc_proto_name_only(as, BFLRG, dcl, NULL);
	    if (IS_NULL(as) == TRUE)
	       fprintf(fp, "<i>SX Binding: </i>      (%s)\n", dcn);
	    else
	       fprintf(fp, "<i>SX Binding: </i>      (%s %s)\n", dcn, as);};}

    else if (dk == DK_MAN)
       {if (bfn == NULL)
	   {fprintf(fp, ".B SX Binding:      none\n");
	    fprintf(fp, ".sp\n");}
	else if (dcl->bindings != NULL)
	   {map_name(dcn, BFLRG, cfn, bfn, NULL, -1, TRUE, FALSE);
	    doc_proto_name_only(as, BFLRG, dcl, NULL);
	    if (IS_NULL(as) == TRUE)
	       fprintf(fp, ".B SX Binding:      (%s)\n", dcn);
	    else
	       fprintf(fp, ".B SX Binding:      (%s %s)\n", dcn, as);

	    fprintf(fp, ".sp\n");};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CL_SCHEME - process command line arguments for Scheme binding */

static int cl_scheme(statedes *st, bindes *bd, int c, char **v)
   {int i;

    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-h") == 0)
            {printf("   Scheme options: [-nos]\n");
             printf("      nos  do not generate Scheme interfaces\n");
             printf("\n");
             return(1);}
	 else if (strcmp(v[i], "-nos") == 0)
	    st->no[MODE_S] = FALSE;};

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REGISTER_SCHEME - register SCHEME binding methods */

static int register_scheme(int fl, statedes *st)
   {int i;
    bindes *pb;

    MODE_S = nbd;

    if (fl == TRUE)
       {pb = gbd + nbd++;
	for (i = 0; i < NF; i++)
	    pb->fp[i] = NULL;

	pb->lang = "Scheme";
	pb->st   = st;
	pb->data = NULL;
	pb->cl   = cl_scheme;
	pb->init = init_scheme;
	pb->bind = bind_scheme;
	pb->doc  = bind_doc_scheme;
	pb->fin  = fin_scheme;};

    return(MODE_S);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

