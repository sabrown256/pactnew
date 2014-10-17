/*
 * BLANG-PY.C - generate Python language bindings
 *
 * #include "cpyright.h"
 *
 */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PY_FORMAT - convert <itype>, <var> pairs to Python parse format string */

static void py_format(char *fmt, int nc, char *spec, char *name)
   {int i;
    char t[BFLRG];
    char *ty, *pf, **ta;

    memset(fmt, 0, nc);

    nstrncpy(t, BFLRG, spec, -1);
    ta = tokenize(t, ", \t\n", 0);
    if (ta != NULL)
       {pf = fmt;
        for (i = 0; ta[i] != NULL; i += 2)
	    {ty = ta[i];
	     if ((strcmp(ty, "SC_CHAR_I") == 0) ||
		 (strcmp(ty, "char") == 0))
	        *pf++ = 'b';
	     else if ((strcmp(ty, "SC_SHORT_I") == 0) ||
		      (strcmp(ty, "short") == 0))
	        *pf++ = 'h';
	     else if ((strcmp(ty, "SC_INT_I") == 0) ||
		      (strcmp(ty, "int") == 0))
	        *pf++ = 'i';
	     else if ((strcmp(ty, "SC_LONG_I") == 0) ||
		      (strcmp(ty, "long") == 0))
	        *pf++ = 'l';
	     else if ((strcmp(ty, "SC_LONG_LONG_I") == 0) ||
		      (strcmp(ty, "long long") == 0))
	        *pf++ = 'L';
	     else if ((strcmp(ty, "SC_FLOAT_I") == 0) ||
		      (strcmp(ty, "float") == 0))
	        *pf++ = 'f';
	     else if ((strcmp(ty, "SC_DOUBLE_I") == 0) ||
		      (strcmp(ty, "double") == 0))
	        *pf++ = 'd';
	     else if ((strcmp(ty, "SC_STRING_I") == 0) ||
		      (strcmp(ty, "char *") == 0))
	        *pf++ = 's';
	     else if (strcmp(ty, "SC_ENUM_I") == 0)
	        *pf++ = 'i';
	     else
	        *pf++ = 'O';};

	if (name != NULL)
	   vstrcat(fmt, nc, ":%s", name);

	free_strings(ta);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PY_ARG - convert <itype>, <var> pairs to Python call arg list string */

static void py_arg(char *arg, int nc, char *spec)
   {int i, n;
    char t[BFLRG];
    char **ta;

    if (arg != NULL)
       {arg[0] = '\0';

	nstrncpy(t, BFLRG, spec, -1);
	ta = tokenize(t, ", \t\n", 0);
	if (ta != NULL)
	   {for (i = 1; ta[i] != NULL; i += 2)
	        vstrcat(arg, nc, "%s, ", ta[i]);

	    n = strlen(arg);
	    arg[n-2] = '\0';

	    free_strings(ta);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_ENUM_DEFS - write the Python interface C enums DV */

static void python_enum_defs(FILE *fp, char *dv, char **ta, char *pck)
   {

/* syntax:
 *  To summarize adding enums to python, blang generates
 *  a function that adds the names to a dictionary.
 *  This function is then called by the module initialization instead
 *  of using the #include method that is being used with methods.
 *  
 * int blang_generated(PyObject *d)
 *    {if (PyDict_SetItemString(d, "enum", PyInt_FromLong(ENUM)) < 0)
 *        return(-1);
 *      ...
 *     return(0);}
 *
 */

    if ((ta != NULL) && (strcmp(ta[0], "enum") == 0))
       {};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_STRUCT_DEFS - write the Python interface C structs DV */

static void python_struct_defs(FILE *fp, char *dv, char **ta, char *pck)
   {

/* syntax:
 */

    if (ta != NULL)
       {};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_DEF_STRUCTS - define local version of struct definitions */

static void python_def_structs(FILE *fp, statedes *st)
   {int ib, nbi;
    char t[BFLRG];
    char *sb, *cty, *pty, **sbi, **sa;

    sbi = st->sbi;
    nbi = st->nbi;

/* make local struct definitions */
    for (ib = 0; ib < nbi; ib++)
        {sb = sbi[ib];
	 if (blank_line(sb) == FALSE)
	    {nstrncpy(t, BFLRG, sb, -1);
	     sa = tokenize(t, " \t", 0);
	     if ((sa != NULL) && (strcmp(sa[0], "derived") == 0))
	        {cty = sa[1];
		 pty = sa[4];
		 if ((IS_NULL(pty) == FALSE) &&
		     (strcmp(pty, "SC_ENUM_I") != 0) &&
		     (strcmp(pty, "PyObject") != 0))
		    {fprintf(fp, "typedef struct s_%s *%sp;\n", pty, pty);
		     fprintf(fp, "typedef struct s_%s %s;\n", pty, pty);
		     fprintf(fp, "\n");
		     fprintf(fp, "struct s_%s\n", pty);
		     fprintf(fp, "   {PyObject_HEAD\n");
		     fprintf(fp, "    %s *pobj;};\n", cty);
		     fprintf(fp, "\n");};};

	     free_strings(sa);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_PYTHON - initialize Python file */

static void init_python(statedes *st, bindes *bd)
   {char fn[BFLRG];
    char *pck;
    FILE *fp;

    pck = st->pck;

    if ((st->path == NULL) || (strcmp(st->path, ".") == 0))
       snprintf(fn, BFLRG, "gp-%s.c", pck);
    else
       snprintf(fn, BFLRG, "%s/gp-%s.c", st->path, pck);

    fp = open_file("w", fn);

    fprintf(fp, "\n");
    fprintf(fp, "#include <Python.h>\n");
    fprintf(fp, "#include \"%s_int.h\"\n", pck);
    fprintf(fp, "#include \"sx_int.h\"\n");
    fprintf(fp, "\n");

    python_def_structs(fp, st);

    csep(fp);

    bd->fp = fp;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_MAKE_DECL - make wrapper function declaration */

static void python_make_decl(char *t, int nc, fdecl *dcl)
   {int i, na, n;
    char dty[BFLRG], s[BFLRG];
    char *cfn, *ty, *lty, *pty;
    farg *al;

    na  = dcl->na;
    cfn = dcl->proto.name;

    snprintf(t, nc, " *_PY_%s", cfn);
    n = strlen(t);
    memset(s, ' ', n);
    s[n] = '\0';

    pty = "PyObject";
    for (i = 0; i < na; i++)
        {al = dcl->al + i;
	 ty = al->type;
	 deref(dty, BFLRG, ty);
	 lty = lookup_type(NULL, dty, MODE_C, MODE_P);
	 if ((lty != NULL) && (strcmp(lty, "SC_ENUM_I") != 0))
	    {pty = lty;
	     break;};};

    vstrcat(t, nc, "(%s *self, PyObject *args, PyObject *kwds)", pty);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_WRAP_DECL - function declaration */

static void python_wrap_decl(FILE *fp, fdecl *dcl)
   {char t[BFLRG];

    fprintf(fp, "\n");
    fprintf(fp, "/* WRAP |%s| */\n", dcl->proto.arg);
    fprintf(fp, "\n");

    python_make_decl(t, BFLRG, dcl);

    fprintf(fp, "PyObject%s\n", t);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_KW_LIST - compute the keyword list */

static void python_kw_list(char *kw, int nc, fdecl *dcl)
   {int i, na;
    farg *al;
    idecl *ip;

    na = dcl->na;
    al = dcl->al;
	
    kw[0] = '\0';

    for (i = 0; i < na; i++)
        {if (al[i].cls == FALSE)
	    {ip = &al[i].interp;
	     nstrcat(kw, BFLRG, ip->argn);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_CLASS_SELF - emit the self object assignment */

static void python_class_self(FILE *fp, fdecl *dcl)
   {int i, na;
    char *nm;
    farg *al;

    na = dcl->na;
    al = dcl->al;
	
    for (i = 0; i < na; i++)
        {if (al[i].cls == TRUE)
	    {nm = al[i].name;
	     fprintf(fp, "    _l%s = self->pobj;\n\n", nm);
	     break;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_WRAP_LOCAL_DECL - local variable declarations */

static void python_wrap_local_decl(FILE *fp, fdecl *dcl, char *kw)
   {int i, na, voida, voidf;
    char t[BFLRG];
    char *rty;
    farg *al;
    idecl *ip;

    na    = dcl->na;
    al    = dcl->al;
    voida = dcl->voida;
    voidf = dcl->voidf;
    rty   = dcl->proto.type;
	
    if ((voida == FALSE) && (IS_NULL(kw) == FALSE))
       {fprintf(fp, "   {int ok;\n");
	fprintf(fp, "    PyObject *_lo;\n");}
    else
       fprintf(fp, "   {PyObject *_lo;\n");

    for (i = 0; i <= na; i++)
        {if ((voida == TRUE) && (i == 0))
	    continue;

	 ip = &al[i].interp;

	 snprintf(t, BFLRG, "    ");

/* variable for return value */
	 if (i == na)
	    {if (voidf == FALSE)
	        vstrcat(t, BFLRG, "%s _rv;\n    ", rty);

	     if ((voida == FALSE) && (IS_NULL(kw) == FALSE))
	        vstrcat(t, BFLRG, "char *kw_list[] = {%sNULL};\n", kw);}

/* local vars */
	 else if (IS_NULL(ip->decl) == FALSE)
	    nstrcat(t, BFLRG, ip->decl);

	 if (IS_NULL(t) == FALSE)
	    fputs(subst(t, "* ", "*", -1), fp);};

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_WRAP_LOCAL_ASSN_ARG - add AL to PyArg_ParseTupleAndKeywords call
 *                            - argument list
 */

static void python_wrap_local_assn_arg(char *a, int nc, farg *al)
   {char *arg;
    idecl *ip;

    if (al->cls == FALSE)
       {ip  = &al->interp;
        arg = ip->argi;
	nstrcat(a, nc, arg);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_WRAP_LOCAL_ASSN - local variable assignments */

static void python_wrap_local_assn(FILE *fp, fdecl *dcl, char *pfn, char *kw)
   {int i, na, voida;
    char a[BFLRG], dcn[BFLRG], fmt[BFLRG], arg[BFLRG];
    char *cfn;
    farg *al;

    cfn   = dcl->proto.name;
    na    = dcl->na;
    al    = dcl->al;
    voida = dcl->voida;

    python_class_self(fp, dcl);

    if ((voida == FALSE) && (IS_NULL(kw) == FALSE))
       {map_name(dcn, BFLRG, cfn, pfn, "p", -1, FALSE, TRUE);

/* make the PyArg_ParseTupleAndKeywords call */
	a[0] = '\0';
	for (i = 0; i < na; i++)
	    python_wrap_local_assn_arg(a, BFLRG, al+i);

	py_format(fmt, BFLRG, a, dcn);
	fprintf(fp, "    ok = PyArg_ParseTupleAndKeywords(args, kwds,\n");
	fprintf(fp, "                                     \"%s\",\n", fmt);
	fprintf(fp, "                                     kw_list,\n");

	py_arg(arg, BFLRG, a);
	fprintf(fp, "                                     %s);\n", arg);

	fprintf(fp, "    if (ok == FALSE)\n");
	fprintf(fp, "       return(NULL);\n");
	fprintf(fp, "\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_WRAP_LOCAL_CALL - function call */

static void python_wrap_local_call(FILE *fp, fdecl *dcl)
   {int voidf;
    char a[BFLRG], t[BFLRG];
    char *nm;

    voidf = dcl->voidf;
    nm    = dcl->proto.name;

    cf_call_list(a, BFLRG, dcl, TRUE);

    if (voidf == TRUE)
       snprintf(t, BFLRG, "    %s(%s);\n", nm, a);
    else
       snprintf(t, BFLRG, "    _rv = %s(%s);\n", nm, a);

    fputs(t, fp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_VALUE_RETURN - compute return value from DCL */

static void python_value_return(char *t, int nc, fdecl *dcl)
   {int i, iv, na, nr, nvl, voidf;
    fdir dir;
    char a[BFLRG], dty[BFLRG], fmt[BFLRG], arg[BFLRG];
    char *nm;
    farg *al;

    t[0] = '\0';
    a[0] = '\0';

    nr    = dcl->nr;
    voidf = dcl->voidf;

    if (voidf == FALSE)
       {cs_type(dty, BFLRG, &dcl->proto, TRUE);
	vstrcat(a, BFLRG, "\t\t\t%s, &_rv,\n", dty);};

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
		    vstrcat(a, BFLRG, "\t\t\t%s, &_l%s,\n", dty, nm);
		 else
		    {for (iv = 0; iv < nvl; iv++)
			 vstrcat(a, BFLRG, "\t\t\t%s, &_l%s[%d],\n",
				 dty, nm, iv);};};};};

/* if the list argument are non empty make up the call */
    if (IS_NULL(a) == FALSE)
       {py_format(fmt, BFLRG, a, NULL);
	py_arg(arg, BFLRG, a);
	snprintf(t, nc, "    _lo = Py_BuildValue(\"%s\",\n", fmt);
	vstrcat(t, nc,  "                        %s);\n", arg);}
    else
       {snprintf(t, nc, "\n");
	nstrcat(t, nc, "    Py_INCREF(Py_None);\n");
	nstrcat(t, nc, "    _lo = Py_None;\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_WRAP_LOCAL_RETURN - function return */

static void python_wrap_local_return(FILE *fp, fdecl *dcl)
   {char t[BFLRG];

    python_value_return(t, BFLRG, dcl);
    if (IS_NULL(t) == TRUE)
       fprintf(fp, "    _lo = Py_None;\n");
    else
       fputs(t, fp);
    fprintf(fp, "\n");

    fprintf(fp, "    return(_lo);}\n");
    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_METHOD_DEF - method definition wrapper */

static void python_method_def(char *dfn, int nc, fdecl *dcl,
			      char *pfn, int def)
   {int n, voida;
    char a[BFLRG], s[BFLRG], t[BFLRG];
    char fn[BFLRG];
    char *cfn, *nm;
    farg *alc;

    cfn   = dcl->proto.name;
    voida = dcl->voida;

    if (def == TRUE)
       {map_name(fn, BFLRG, cfn, pfn, NULL, -1, FALSE, TRUE);

	alc = get_class_arg(dcl);

/* prepare the function inline documenation */
	if_call_list(a, BFLRG, dcl, ",");
	if (alc != NULL)
	   {nm = alc->name;
	    if (voida == FALSE)
	       snprintf(t, BFLRG, "Method: %s\\n     Usage: %s.%s(%s)",
			fn, nm, fn, a);
	    else
	       snprintf(t, BFLRG, "Method: %s\\n     Usage: %s.%s()",
			fn, nm, fn);}
	else
	   {if (voida == FALSE)
	       snprintf(t, BFLRG, "Procedure: %s\\n     Usage: %s(%s)",
			fn, fn, a);
	    else
	       snprintf(t, BFLRG, "Procedure: %s\\n     Usage: %s()",
			fn, fn);};

/* emit the method definition */
	snprintf(s, BFLRG, " _PYD_%s = ", cfn);
	n = strlen(s) + 1;

	snprintf(dfn, nc, "{\"%s\", (PyCFunction) _PY_%s, METH_KEYWORDS,\n",
		 fn, cfn);

	memset(s, ' ', n);
	s[n] = '\0';
	vstrcat(dfn, nc, "%s\"%s\"}", s, t);}

/* emit the method declaration */
    else
       snprintf(dfn, nc, " _PYD_%s", cfn);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_WRAP - wrap C function DCL in PYTHON callable function
 *             - using name PFN
 */

static void python_wrap(FILE *fp, fdecl *dcl, char *pfn)
   {char upn[BFLRG], kw[BFLRG];

    if ((is_var_arg(dcl->proto.arg) == FALSE) && (strcmp(pfn, "none") != 0))
       {nstrncpy(upn, BFLRG, pfn, -1);
	upcase(upn);

	csep(fp);

	python_kw_list(kw, BFLRG, dcl);

/* function declaration */
	python_wrap_decl(fp, dcl);

/* local variable declarations */
	python_wrap_local_decl(fp, dcl, kw);

/* local variable assignments */
	python_wrap_local_assn(fp, dcl, pfn, kw);

/* function call */
	python_wrap_local_call(fp, dcl);

/* function return */
	python_wrap_local_return(fp, dcl);

	csep(fp);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_HEADER - write the header with declarations for Python wrappers */

static void python_header(bindes *bd)
   {int ib, ndcl;
    char name[BFLRG], upk[BFLRG], dfn[BFLRG], t[BFLRG];
    char *pck, *cfn, *pfn;
    fdecl *dcl, *dcls;
    statedes *st;
    FILE *fh;

    st   = bd->st;
    pck  = st->pck;
    dcls = st->dcl;
    ndcl = st->ndcl;

    nstrncpy(upk, BFLRG, pck, -1);
    upcase(upk);

    if ((st->path == NULL) || (strcmp(st->path, ".") == 0))
       snprintf(name, BFLRG, "py-%s.h", pck);
    else
       snprintf(name, BFLRG, "%s/py-%s.h", st->path, pck);

    fh = fopen_safe(name, "w");
    if (fh == NULL)
       return;

    fprintf(fh, "/*\n");
    fprintf(fh, " * PY-%s.H - generated header for %s bindings\n", upk, pck);
    fprintf(fh, " *\n");
    fprintf(fh, " */\n");
    fprintf(fh, "\n");

/* extern declarations for wrappers */
    fprintf(fh, "extern PyObject\n");

    for (ib = 0; ib < ndcl; ib++)
        {dcl = dcls + ib;
	 pfn = has_binding(dcl, "python");
	 if ((pfn != NULL) && (strcmp(pfn, "none") != 0) &&
	     (is_var_arg(dcl->proto.arg) == FALSE))
	    {python_make_decl(t, BFLRG, dcl);
	     fprintf(fh, "%s,\n", t);};};

    fprintf(fh, " _PY_%s_null;\n", pck);
    fprintf(fh, "\n");

/* method definition macros */
    for (ib = 0; ib < ndcl; ib++)
        {dcl = dcls + ib;
	 cfn = dcl->proto.name;
	 pfn = has_binding(dcl, "python");
	 if ((pfn != NULL) && (strcmp(pfn, "none") != 0) &&
	     (is_var_arg(dcl->proto.arg) == FALSE))
	    {python_method_def(dfn, BFLRG, dcl, pfn, TRUE);
	     fprintf(fh, "#define _PYD_%s %s\n",
		     cfn, subst(dfn, "\n", "\\\n", -1));};};

    fprintf(fh, "\n");

    fclose_safe(fh);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_INSTALL - write the routine to install the bindings */

static void python_install(bindes *bd)
   {int ib, ndcl;
    char dfn[BFLRG];
    char *pck, *cfn, *pfn;
    fdecl *dcl, *dcls;
    statedes *st;
    FILE *fp;

    fp   = bd->fp;
    st   = bd->st;
    pck  = st->pck;
    dcls = st->dcl;
    ndcl = st->ndcl;

    csep(fp);

/* make the list of enum constants to install */
    emit_enum_defs(bd, python_enum_defs);

/* make the list of struct constants to install */
    emit_struct_defs(bd, python_struct_defs);

    csep(fp);

    fprintf(fp, "\n");
    fprintf(fp, "PyMethodDef\n");

/* method definition wrapper */
    for (ib = 0; ib < ndcl; ib++)
        {dcl = dcls + ib;
	 cfn = dcl->proto.name;
	 pfn = has_binding(dcl, "python");
	 if ((pfn != NULL) && (strcmp(pfn, "none") != 0) &&
	     (is_var_arg(dcl->proto.arg) == FALSE))
	    {python_method_def(dfn, BFLRG, dcl, pfn, TRUE);
	     fprintf(fp, " _PYD_%s = %s,\n", cfn, dfn);};};

    fprintf(fp, " _PYD_%s_null;\n", pck);
    fprintf(fp, "\n");
    csep(fp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BIND_PYTHON - generate Python bindings from CPR and SBI
 *             - return TRUE iff successful
 */

static int bind_python(bindes *bd)
   {int ib, ndcl, rv;
    char *pfn;
    fdecl *dcl, *dcls;
    statedes *st;
    FILE *fp;

    fp   = bd->fp;
    st   = bd->st;
    dcls = st->dcl;
    ndcl = st->ndcl;

    rv = TRUE;

    for (ib = 0; ib < ndcl; ib++)
        {dcl = dcls + ib;
	 pfn = has_binding(dcl, "python");
	 if (pfn != NULL)
	    python_wrap(fp, dcl, pfn);};

/* write the method definitions */
    python_install(bd);

/* write the method declarations */
    python_header(bd);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIN_PYTHON - finalize Python file */

static void fin_python(bindes *bd)
   {FILE *fp;

    fp = bd->fp;

    csep(fp);
    fclose_safe(fp);

    bd->fp = NULL;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REGISTER_PYTHON - register PYTHON binding methods */

static int register_python(int fl, statedes *st)
   {int nb;
    bindes *pb;

    if (fl == TRUE)
       {nb = nbd;

	pb = gbd + nbd++;
	pb->st   = st;
	pb->fp   = NULL;
	pb->init = init_python;
	pb->bind = bind_python;
	pb->fin  = fin_python;};

    return(nb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

