/*
 * BLANG-PY.C - generate Python language bindings
 *
 * #include "cpyright.h"
 *
 */

/*
#define METHOD_DEF_VAR
*/

typedef struct s_tnp_list tnp_list;

struct s_tnp_list
   {char cnm[BFSML];        /* C struct name, PM_set */
    char pnm[BFSML];        /* Python struct name, PY_PM_set */
    char tnm[BFSML];        /* Python type name, PY_PM_set_type */
    char rnm[BFSML];        /* root struct id, SET */
    char inm[BFSML];        /* default instance id, set */
    char lnm[BFSML];        /* lower case version of CNM, pm_set */
    char unm[BFSML];};      /* upper case version of CNM, PM_SET */

static char
 **_py_bound_types = NULL;

static int
 MODE_P = -1;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_TYPE_NAME_LIST - make canonical variations of type name TYP */

static void python_type_name_list(char *typ, tnp_list *na)
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

/* make the Python name */
    snprintf(na->pnm, BFSML, "PY_%s", p);

/* make the Python type name */
    snprintf(na->tnm, BFSML, "PY_%s_type", p);

/* get root struct name */
    if (p[2] == '_')
       nstrncpy(na->rnm, BFSML, p+3, -1);
    else
       nstrncpy(na->rnm, BFSML, p, -1);
    upcase(na->rnm);

/* get default instance name */
    nstrncpy(na->inm, BFSML, na->rnm, -1);
    downcase(na->inm);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_PARSE_MEMBER - parse out MBR for python */

static int python_parse_member(char *mbr, char *mnm, char *mty, char *mdm,
			       char *bty, int nc)
   {int nind, nb;
    char *lty, *pb;

    parse_member(mbr, mnm, mty, mdm, BFSML);

    nstrncpy(bty, nc, mty, -1);

    lty = lookup_type(NULL, mty, MODE_C, gbd+MODE_P);
    if ((lty != NULL) && (strcmp(lty, tykind[TK_ENUM]) == 0))
       nstrncpy(mty, nc, tykind[TK_ENUM], -1);

    nb = strlen(bty) - 1;
    pb = bty + nb;
    for (nind = 0; (*pb == '*') && (nind < nb); pb--, nind++)
        *pb = '\0';

    trim(bty, BACK, " \t");

    return(nind);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_ADD_BOUND_TYPE - install TY to the list of
 *                       - types that have bindings
 */

void python_add_bound_type(char *ty)
   {

    _py_bound_types = lst_push(_py_bound_types, ty);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_LOOKUP_BOUND_TYPE - lookup member type MTY to see if it
 *                          - is a type that has bindings
 */

int python_lookup_bound_type(char *mty)
   {int i, nc, ok;
    char *t;

    ok = FALSE;
    for (i = 0; (_py_bound_types[i] != NULL) && (ok == FALSE); i++)
        {t  = _py_bound_types[i];
	 nc = strlen(t);
	 ok = (strncmp(mty, t, nc) == 0);};

    return(ok);}

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
	     if ((strcmp(ty, "SC_SHORT_I") == 0) ||
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

/* GOTHCA: only appear to get this in context of 'char *'
 * e.g. PD_pwd
 */
	     else if ((strcmp(ty, "SC_CHAR_I") == 0) ||
		 (strcmp(ty, "char") == 0))
	        *pf++ = 's';
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

static void python_enum_defs(FILE **fpa, char *dv, char **ta,
			     char *pck, int ni)
   {FILE *fc;

    fc = fpa[0];

    if (ta == NULL)
       {if (strcmp(dv, "begin") == 0)
	   {fprintf(fc, "\n");
	    fprintf(fc, "int PY_add_%s_enum(PyObject *m)\n", pck);
	    fprintf(fc, "   {int nerr;\n");
	    fprintf(fc, "\n");
	    fprintf(fc, "    nerr = 0;\n");
	    fprintf(fc, "\n");}

        else if (strcmp(dv, "end") == 0)
	   {fprintf(fc, "    return(nerr);}\n");
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

	     fprintf(fc, "    nerr += (PyModule_AddIntConstant(m, \"%s\", %ld) < 0);\n",
		     vr, vl);
	     fprintf(fc, "\n");};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_HDR_STRUCT_DEF - emitter for struct defs info belonging
 *                       - in the header file
 */

static void python_hdr_struct_def(FILE *fh, char *dv, char **ta, char *pck)
   {tnp_list tl;

    python_type_name_list(ta[0]+9, &tl);

    python_add_bound_type(tl.cnm);

    csep(fh);
    fprintf(fh, "\n");

    fprintf(fh, "/* %s binding */\n", tl.cnm);
    fprintf(fh, "\n");

    fprintf(fh, "typedef struct s_%s *%sp;\n", tl.pnm, tl.pnm);
    fprintf(fh, "typedef struct s_%s %s;\n", tl.pnm, tl.pnm);
    fprintf(fh, "\n");
    fprintf(fh, "struct s_%s\n", tl.pnm);
    fprintf(fh, "   {PyObject_HEAD\n");
    fprintf(fh, "#ifdef PY_EXT_METHOD_%s\n", tl.rnm);
    fprintf(fh, "    PY_EXT_METHOD_%s\n", tl.rnm);
    fprintf(fh, "#endif\n");
    fprintf(fh, "    %s *pyo;};\n", tl.cnm);
    fprintf(fh, "\n");

    fprintf(fh, "extern PyTypeObject\n");
    fprintf(fh, " %s_type;\n", tl.pnm);
    fprintf(fh, "\n");

    fprintf(fh, "extern PyGetSetDef\n");
    fprintf(fh, " %s_getset[];\n", tl.pnm);
    fprintf(fh, "\n");

    fprintf(fh, "extern int\n");
    fprintf(fh, " %s_extractor(PyObject *obj, void *arg),\n", tl.pnm);
    fprintf(fh, " %s_check(PyObject *op);\n", tl.pnm);
    fprintf(fh, "\n");

    fprintf(fh, "extern PyObject\n");
    fprintf(fh, " *%s_from_ptr(%s *x);\n", tl.pnm, tl.cnm);
    fprintf(fh, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_UNKNOWN_MEMBER - handle unknown action case
 *                       - FL is 1 for get action
 *                       - and 2 for set action
 */

static void python_unknown_member(FILE *fc, char *pm, char *mty, int fl)
   {

/* for get methods */
    if (fl == 1)
       {fprintf(fc, "/* unknown get '%s' member '%s' */\n",mty, pm);
        fprintf(fc, "    {PyErr_SetString(PyExc_NotImplementedError, \"%s\");\n",
		pm);
	fprintf(fc, "     rv = NULL;};\n");}

/* for set methods */
    else
       {fprintf(fc, "/* unknown set '%s' member '%s' */\n", mty, pm);
	fprintf(fc, "    PyErr_SetString(PyExc_NotImplementedError, \"%s\");\n",
		pm);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_C_STRUCT_DEF - emitter for struct defs info belonging
 *                     - in the C file
 */

static void python_c_struct_def(FILE *fc, char *dv, char **ta, char *pck)
   {int im, hs, nind;
    char mnm[BFSML], mty[BFSML], mdm[BFSML], pty[BFSML], bty[BFSML];
    char *pm;
    tnp_list tl;

    python_type_name_list(ta[0]+9, &tl);

    csep(fc);
    fprintf(fc, "\n");
    fprintf(fc, "/*                     %s ROUTINES               */\n",
	    tl.unm);
    fprintf(fc, "\n");
    csep(fc);
    fprintf(fc, "\n");

/* object check routine */
    fprintf(fc, "int %s_check(PyObject *op)\n", tl.pnm);
    fprintf(fc, "   {int rv;\n");
    fprintf(fc, "\n");
    fprintf(fc, "    rv = PyObject_TypeCheck(op, &%s);\n", tl.tnm);
    fprintf(fc, "\n");
    fprintf(fc, "    return(rv);}\n");

    fprintf(fc, "\n");
    csep(fc);
    csep(fc);
    fprintf(fc, "\n");

/* object from pointer routine */
    fprintf(fc, "PyObject *%s_from_ptr(%s *x)\n", tl.pnm, tl.cnm);
    fprintf(fc, "   {%s *self;\n", tl.pnm);
    fprintf(fc, "    PyObject *rv;\n");
    fprintf(fc, "\n");
    fprintf(fc, "    rv = NULL;\n");
    fprintf(fc, "\n");
    fprintf(fc, "    self = PyObject_NEW(%s, &%s);\n", tl.pnm, tl.tnm);
    fprintf(fc, "    if (self != NULL)\n");
    fprintf(fc, "       {self->pyo = x;\n");
    fprintf(fc, "        rv = (PyObject *) self;};\n");
    fprintf(fc, "\n");
    fprintf(fc, "    return(rv);}\n");

    fprintf(fc, "\n");
    csep(fc);
    csep(fc);
    fprintf(fc, "\n");

/* object get routine */
    fprintf(fc, "static PyObject *%s_get(%s *self, void *context)\n",
	    tl.pnm, tl.pnm);
    fprintf(fc, "   {PyObject *rv;\n");
    fprintf(fc, "\n");
    fprintf(fc, "    rv = %s_from_ptr(self->pyo);\n", tl.pnm);
    fprintf(fc, "\n");
    fprintf(fc, "    return(rv);}\n");

    fprintf(fc, "\n");
    csep(fc);
    csep(fc);
    fprintf(fc, "\n");

/* getter - member accessor methods */
    for (im = 1; ta[im] != NULL; im++)
        {pm = trim(ta[im], BOTH, " \t");
	 if (IS_NULL(pm) == TRUE)
	    continue;

	 nind = python_parse_member(pm, mnm, mty, mdm, bty, BFSML);

	 fprintf(fc, "static PyObject *%s_get_%s(%s *self, void *context)\n",
		 tl.pnm, mnm, tl.pnm);
	 fprintf(fc, "   {PyObject *rv;\n");
	 fprintf(fc, "\n");

/* if member is a function pointer */
	 if (strstr(pm, "(*") != NULL)
	    python_unknown_member(fc, pm, mty, 1);

/* if member is 'char *' */
	 else if (strcmp(mty, "char *") == 0)
	    fprintf(fc, "    rv = Py_BuildValue(\"s\", self->pyo->%s);\n",
		    mnm);

/* if member is fixed point type */
	 else if ((strcmp(mty, "int") == 0) ||
		  (strcmp(mty, "int64_t") == 0) ||
		  (strcmp(mty, "long") == 0) ||
		  (strcmp(mty, tykind[TK_ENUM]) == 0))
	    {if (IS_NULL(mdm) == TRUE)
	        fprintf(fc, "    rv = PY_INT_LONG(self->pyo->%s);\n", mnm);
	     else
                python_unknown_member(fc, pm, mty, 1);}

/* if member is floating point type */
	 else if ((strcmp(mty, "float") == 0) ||
		  (strcmp(mty, "double") == 0))
	    {if (IS_NULL(mdm) == TRUE)
	        fprintf(fc, "    rv = PyFloat_FromDouble(self->pyo->%s);\n", mnm);
	     else
                python_unknown_member(fc, pm, mty, 1);}

/* if member is pointer to a known bound type */
         else if (python_lookup_bound_type(bty) == TRUE)
	    {snprintf(pty, BFSML, "PY_%s", bty);
	     if (nind > 0)
	        fprintf(fc, "    rv = %s_from_ptr(self->pyo->%s);\n",
			trim(pty, BOTH, " *"), mnm);
	     else
	        fprintf(fc, "    rv = %s_from_ptr(&self->pyo->%s);\n",
			trim(pty, BOTH, " *"), mnm);}

/* if member is pointer */
         else if (nind > 0)
	    fprintf(fc, "    rv = PY_COBJ_VOID_PTR(self->pyo->%s, NULL);\n",
		    mnm);

/* unknown member action */
	 else
	    python_unknown_member(fc, pm, mty, 1);

	 fprintf(fc, "\n");
	 fprintf(fc, "    return(rv);}\n");

	 fprintf(fc, "\n");
	 csep(fc);
	 csep(fc);
	 fprintf(fc, "\n");};

/* setter - member accessor methods */
    for (im = 1; ta[im] != NULL; im++)
        {pm = trim(ta[im], BOTH, " \t");
	 if (IS_NULL(pm) == TRUE)
	    continue;

	 nind = python_parse_member(pm, mnm, mty, mdm, bty, BFSML);

	 fprintf(fc, "static int %s_set_%s(%s *self,\n",
		 tl.pnm, mnm, tl.pnm);
         fprintf(fc, "                   PyObject *value, void *context)\n");
	 fprintf(fc, "   {int rv;\n");
	 fprintf(fc, "\n");
         fprintf(fc, "    rv = -1;\n");
	 fprintf(fc, "\n");

         fprintf(fc, "    if (value == NULL)\n");
         fprintf(fc, "       PyErr_SetString(PyExc_TypeError,\n");
         fprintf(fc, "                       \"attribute deletion is not supported\");\n");
         fprintf(fc, "\n");
         fprintf(fc, "    else\n");

/* if member is 'char *' */
	 if (strcmp(mty, "char *") == 0)
	    {fprintf(fc, "       {int ok;\n");
	     fprintf(fc, "\n");
	     fprintf(fc, "        ok = PyArg_Parse(value, \"s\", &self->pyo->%s);\n", mnm);
	     fprintf(fc, "        if (ok == TRUE)\n");
	     fprintf(fc, "           rv = 0;};\n");}

/* if member is fixed point type */
	 else if ((strcmp(mty, "int") == 0) ||
		  (strcmp(mty, "int64_t") == 0) ||
		  (strcmp(mty, "long") == 0) ||
		  (strcmp(mty, tykind[TK_ENUM]) == 0))
	    {if (IS_NULL(mdm) == FALSE)
                python_unknown_member(fc, pm, mty, 2);
	     else
	        {fprintf(fc, "       {int ok;\n");
		 fprintf(fc, "        long lv;\n");
		 fprintf(fc, "\n");
		 fprintf(fc, "        ok = PyArg_Parse(value, \"l\", &lv);\n");
		 fprintf(fc, "        if (ok == TRUE)\n");
		 fprintf(fc, "           {self->pyo->%s = lv;\n", mnm);
		 fprintf(fc, "            rv = 0;};};\n");};}

/* if member is floating point type */
	 else if ((strcmp(mty, "float") == 0) ||
		  (strcmp(mty, "double") == 0))
	    {if (IS_NULL(mdm) == FALSE)
                python_unknown_member(fc, pm, mty, 2);
	     else
	        {fprintf(fc, "       {int ok;\n");
	         fprintf(fc, "        double dv;\n");
		 fprintf(fc, "\n");
		 fprintf(fc, "        ok = PyArg_Parse(value, \"d\", &dv);\n");
		 fprintf(fc, "        if (ok == TRUE)\n");
		 fprintf(fc, "           {self->pyo->%s = dv;\n", mnm);
		 fprintf(fc, "            rv = 0;};};\n");};}

/* if member is pointer to a known bound type */
         else if (python_lookup_bound_type(bty) == TRUE)
	    {if (nind > 0)
	        {fprintf(fc, "       {int ok;\n");
		 fprintf(fc, "\n");
		 fprintf(fc, "        ok = PY_%s_extractor(value, self->pyo->%s);\n",
			 bty, mnm);
		 fprintf(fc, "        if (ok == TRUE)\n");
		 fprintf(fc, "           {_PY_opt_%s(self->pyo->%s, BIND_ALLOC, NULL);\n",
			 bty, mnm);
		 fprintf(fc, "            rv = 0;};};\n");}
	     else
	        fprintf(fc, "       rv = -1;     /* non pointer struct '%s' */\n",
			pm);}

/* if member is pointer */
         else if (nind > 0)
	    python_unknown_member(fc, pm, mty, 2);

/* unknown member action */
	 else
	    python_unknown_member(fc, pm, mty, 2);

         fprintf(fc, "\n");

	 fprintf(fc, "    return(rv);}\n");

	 fprintf(fc, "\n");
	 csep(fc);
	 csep(fc);
	 fprintf(fc, "\n");};

/* tp_init method */

    fprintf(fc, "static int %s_tp_init(%s *self, PyObject *args, PyObject *kwds)\n",
	    tl.pnm, tl.pnm);
    fprintf(fc, "   {int rv;\n");
    fprintf(fc, "\n");
    fprintf(fc, "    rv = -1;\n");
    fprintf(fc, "\n");
#if 0
    fprintf(fc, "    int ok;\n");
    fprintf(fc, "    char *kw_list[] = { <member-names>, NULL};\n");
    fprintf(fc, "    ok = PyArg_ParseTupleAndKeywords(args, kwds,\n");
    fprintf(fc, "				     "ssiliiO&zO!O&szszszO!:make_set",\n");
    fprintf(fc, "				     kw_list,\n");
    fprintf(fc, "                                     &<members>);\n");
    fprintf(fc, "    if (ok)\n");
    fprintf(fc, "       {rv        = 0;\n");
    fprintf(fc, "	 self->pyo = PM_mk_set(<args>);};\n");
    fprintf(fc, "\n");
#endif
    fprintf(fc, "    return(rv);}\n");

    fprintf(fc, "\n");
    csep(fc);
    csep(fc);
    fprintf(fc, "\n");

/* doc array */
    fprintf(fc, "static char\n");

    for (im = 1; ta[im] != NULL; im++)
        {pm = trim(ta[im], BOTH, " \t");
	 if (IS_NULL(pm) == TRUE)
	    continue;
	 parse_member(pm, mnm, mty, NULL, BFSML);
         fprintf(fc, " %s_doc_%s[] = \"\",\n", tl.pnm, mnm);};

    fprintf(fc, " %s_doc[] = \"\";\n", tl.pnm);

    fprintf(fc, "\n");
    csep(fc);
    csep(fc);
    fprintf(fc, "\n");

/* getset array */
    fprintf(fc, "PyGetSetDef %s_getset[] = {\n", tl.pnm);
    fprintf(fc, "    {\"%s\", (getter) %s_get, NULL, %s_doc, NULL},\n",
	    tl.inm, tl.pnm, tl.pnm);

    for (im = 1; ta[im] != NULL; im++)
        {pm = trim(ta[im], BOTH, " \t");
	 if (IS_NULL(pm) == TRUE)
	    continue;
	 parse_member(pm, mnm, mty, NULL, BFSML);

/* GOTCHA: when do we have a setter? */
	 hs = FALSE;
	 hs = TRUE;

         if (hs == TRUE)
	    fprintf(fc, "    {\"%s\", (getter) %s_get_%s, (setter) %s_set_%s, %s_doc_%s, NULL},\n",
		    mnm, tl.pnm, mnm, tl.pnm, mnm, tl.pnm, mnm);
	 else
	    fprintf(fc, "    {\"%s\", (getter) %s_get_%s, NULL, %s_doc_%s, NULL},\n",
		    mnm, tl.pnm, mnm, tl.pnm, mnm);};

    fprintf(fc, "#ifdef PY_EXT_GETSET_%s\n", tl.rnm);
    fprintf(fc, "    PY_EXT_GETSET_%s\n", tl.rnm);
    fprintf(fc, "#endif\n");

    fprintf(fc, "    {NULL}};\n");
    fprintf(fc, "\n");

    fprintf(fc, "PyTypeObject %s_type;\n", tl.pnm);
    fprintf(fc, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_STRUCT_DEFS - write the Python interface C structs DV */

static void python_struct_defs(FILE **fpa, char *dv, char **ta,
			       char *pck, int ni)
   {FILE *fc, *fh;

    fc = fpa[0];
    fh = fpa[1];

    if (ta == NULL)
       {if (strcmp(dv, "begin") == 0)
	   {}
        else if (strcmp(dv, "end") == 0)
	   {};}

    else if (strncmp(ta[0], "struct s_", 9) == 0)
       {python_hdr_struct_def(fh, dv, ta, pck);
	python_c_struct_def(fc, dv, ta, pck);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_OBJECT_DEFS - define local version of struct definitions */

static void python_object_defs(FILE **fpa, char *dv, char **ta,
			       char *pck, int ni)
   {tnp_list tl;
    FILE *fc;

    fc = fpa[0];

    if (ta == NULL)
       {if (strcmp(dv, "begin") == 0)
	   {csep(fc);
	    fprintf(fc, "\n");
	    fprintf(fc, "int PY_init_%s(PyObject *m, PyObject *d)\n", pck);
	    fprintf(fc, "   {int nerr;\n");
	    fprintf(fc, "\n");
	    fprintf(fc, "    nerr = 0;\n");
	    fprintf(fc, "\n");}

        else if (strcmp(dv, "end") == 0)
	   {fprintf(fc, "    return(nerr);}\n");
	    fprintf(fc, "\n");};}

    else if (strncmp(ta[0], "struct s_", 9) == 0)
       {python_type_name_list(ta[0]+9, &tl);

	fprintf(fc, "    %s_type.tp_new   = PyType_GenericNew;\n",
		tl.pnm);
	fprintf(fc, "    %s_type.tp_alloc = PyType_GenericAlloc;\n",
		tl.pnm);
	fprintf(fc, "    nerr += (PyType_Ready(&%s_type) < 0);\n",
		tl.pnm);
	fprintf(fc, "    nerr += (PyDict_SetItemString(d, \"%s\", (PyObject *) &%s_type) < 0);\n",
		tl.inm, tl.pnm);
	fprintf(fc, "\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_PYTHON - initialize Python file */

static void init_python(statedes *st, bindes *bd)
   {char fn[BFLRG], upck[BFLRG];
    char *pck;
    FILE *fc, *fh;

    pck = st->pck;
    snprintf(upck, BFLRG, pck, -1);
    upcase(upck);

/* open C file */
    if ((st->path == NULL) || (strcmp(st->path, ".") == 0))
       snprintf(fn, BFLRG, "gp-%s.c", pck);
    else
       snprintf(fn, BFLRG, "%s/gp-%s.c", st->path, pck);

    fc = open_file("w", fn);
    bd->fp[0] = fc;

    fprintf(fc, "/*\n");
    fprintf(fc, " * GP-%s.C - generated support routines for %s\n",
	    upck, upck);
    fprintf(fc, " *\n");
    fprintf(fc, " */\n");
    fprintf(fc, "\n");

    fprintf(fc, "\n");
    fprintf(fc, "#include \"cpyright.h\"\n");
    fprintf(fc, "#include \"py_int.h\"\n");
    fprintf(fc, "#include \"%s_int.h\"\n", pck);
    fprintf(fc, "#include \"gp-%s.h\"\n", pck);
    fprintf(fc, "\n");

    csep(fc);

/* open header file */
    if ((st->path == NULL) || (strcmp(st->path, ".") == 0))
       snprintf(fn, BFLRG, "gp-%s.h", pck);
    else
       snprintf(fn, BFLRG, "%s/gp-%s.h", st->path, pck);

    fh = open_file("w", fn);
    bd->fp[1] = fh;

    fprintf(fh, "/*\n");
    fprintf(fh, " * GP-%s.H - header containing generated support for %s\n",
	    upck, upck);
    fprintf(fh, " *\n");
    fprintf(fh, " */\n");
    fprintf(fh, "\n");

    fprintf(fh, "#include \"cpyright.h\"\n");
    fprintf(fh, "\n");
    fprintf(fh, "#ifndef GEN_PY_%s_H\n", upck);
    fprintf(fh, "#define GEN_PY_%s_H\n", upck);
    fprintf(fh, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_MAKE_DECL - make wrapper function declaration */

static void python_make_decl(char *t, int nc, fdecl *dcl)
   {int i, na;
    char dty[BFLRG], p[BFSML];
    char *cfn, *ty, *lty, *pty;
    farg *al;

    na  = dcl->na;
    cfn = dcl->proto.name;

    snprintf(t, nc, " *_PY_%s", cfn);

    pty = "PyObject";
    for (i = 0; i < na; i++)
        {al = dcl->al + i;
	 ty = al->type;
	 deref(dty, BFLRG, ty);
	 lty = lookup_type(NULL, dty, MODE_C, gbd+MODE_P);
	 if ((lty != NULL) && (strcmp(lty, tykind[TK_STRUCT]) == 0))
	    {snprintf(p, BFSML, "PY_%s", dty);
	     pty = p;
	     break;};};

    vstrcat(t, nc, "(%s *self, PyObject *args, PyObject *kwds)", pty);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_WRAP_DECL - function declaration */

static void python_wrap_decl(FILE *fc, fdecl *dcl)
   {char t[BFLRG];

    fprintf(fc, "\n");
    fprintf(fc, "/* WRAP |%s| */\n", dcl->proto.arg);
    fprintf(fc, "\n");

    python_make_decl(t, BFLRG, dcl);

    fprintf(fc, "PyObject%s\n", t);

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

static void python_class_self(FILE *fc, fdecl *dcl)
   {int i, na;
    char *nm;
    farg *al;

    na = dcl->na;
    al = dcl->al;
	
    for (i = 0; i < na; i++)
        {if (al[i].cls == TRUE)
	    {nm = al[i].name;
	     fprintf(fc, "    _l%s = self->pyo;\n\n", nm);
	     break;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_WRAP_LOCAL_DECL - local variable declarations */

static void python_wrap_local_decl(FILE *fc, fdecl *dcl, char *kw)
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
       {fprintf(fc, "   {int ok;\n");
	fprintf(fc, "    PyObject *_lo;\n");}
    else
       fprintf(fc, "   {PyObject *_lo;\n");

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
	    fputs(subst(t, "* ", "*", -1), fc);};

    fprintf(fc, "\n");

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

static void python_wrap_local_assn(FILE *fc, fdecl *dcl, char *pfn, char *kw)
   {int i, na, voida;
    char a[BFLRG], dcn[BFLRG], fmt[BFLRG], arg[BFLRG];
    char *cfn;
    farg *al;

    cfn   = dcl->proto.name;
    na    = dcl->na;
    al    = dcl->al;
    voida = dcl->voida;

    python_class_self(fc, dcl);

    if ((voida == FALSE) && (IS_NULL(kw) == FALSE))
       {map_name(dcn, BFLRG, cfn, pfn, "p", -1, FALSE, TRUE);

/* make the PyArg_ParseTupleAndKeywords call */
	a[0] = '\0';
	for (i = 0; i < na; i++)
	    python_wrap_local_assn_arg(a, BFLRG, al+i);

	py_format(fmt, BFLRG, a, dcn);
	fprintf(fc, "    ok = PyArg_ParseTupleAndKeywords(args, kwds,\n");
	fprintf(fc, "                                     \"%s\",\n", fmt);
	fprintf(fc, "                                     kw_list,\n");

	py_arg(arg, BFLRG, a);
	fprintf(fc, "                                     %s);\n", arg);

	fprintf(fc, "    if (ok == FALSE)\n");
	fprintf(fc, "       return(NULL);\n");
	fprintf(fc, "\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_WRAP_LOCAL_CALL - function call */

static void python_wrap_local_call(FILE *fc, fdecl *dcl)
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

    fputs(t, fc);

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

    t[0]   = '\0';
    a[0]   = '\0';
    dty[0] = '\0';

    nr    = dcl->nr;
    voidf = dcl->voidf;

    if (voidf == FALSE)
       {cs_type(dty, BFLRG, &dcl->proto, TRUE);
	vstrcat(a, BFLRG, "\t\t\t%s, _rv,\n", dty);};

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
		    vstrcat(a, BFLRG, "\t\t\t%s, _l%s,\n", dty, nm);
		 else
		    {for (iv = 0; iv < nvl; iv++)
			 vstrcat(a, BFLRG, "\t\t\t%s, _l%s[%d],\n",
				 dty, nm, iv);};};};};

/* if the list argument are non empty make up the call */
    if (IS_NULL(a) == FALSE)
       {if (strcmp(dty, "SC_STRING_I") == 0)
	   snprintf(t, nc, "    _lo = PY_strings_tuple(_rv, -1, TRUE);\n");
        else
	   {py_format(fmt, BFLRG, a, NULL);
	    py_arg(arg, BFLRG, a);
	    snprintf(t, nc, "    _lo = Py_BuildValue(\"%s\",\n", fmt);
	    vstrcat(t, nc,  "                        %s);\n", arg);};}
    else
       {snprintf(t, nc, "\n");
	nstrcat(t, nc, "    Py_INCREF(Py_None);\n");
	nstrcat(t, nc, "    _lo = Py_None;\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_WRAP_LOCAL_RETURN - function return */

static void python_wrap_local_return(FILE *fc, fdecl *dcl)
   {char t[BFLRG];

    python_value_return(t, BFLRG, dcl);
    if (IS_NULL(t) == TRUE)
       fprintf(fc, "    _lo = Py_None;\n");
    else
       fputs(t, fc);
    fprintf(fc, "\n");

    fprintf(fc, "    return(_lo);}\n");
    fprintf(fc, "\n");

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

	snprintf(dfn, nc, "{\"%s\", (PyCFunction) _PY_%s, PY_ARG_KEY,\n",
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

static void python_wrap(FILE *fc, fdecl *dcl, char *pfn)
   {char upn[BFLRG], kw[BFLRG];

    if ((is_var_arg(dcl->proto.arg) == FALSE) && (strcmp(pfn, "none") != 0))
       {nstrncpy(upn, BFLRG, pfn, -1);
	upcase(upn);

	csep(fc);

	python_kw_list(kw, BFLRG, dcl);

/* function declaration */
	python_wrap_decl(fc, dcl);

/* local variable declarations */
	python_wrap_local_decl(fc, dcl, kw);

/* local variable assignments */
	python_wrap_local_assn(fc, dcl, pfn, kw);

/* function call */
	python_wrap_local_call(fc, dcl);

/* function return */
	python_wrap_local_return(fc, dcl);

	csep(fc);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_HEADER - write the header with declarations for Python wrappers */

static void python_header(bindes *bd)
   {int ib, ndcl;
    char upk[BFLRG], t[BFLRG];
    char *pck, *pfn;
    fdecl *dcl, *dcls;
    statedes *st;
    FILE *fh;

    st   = bd->st;
    pck  = st->pck;
    dcls = st->dcl;
    ndcl = st->ndcl;

    nstrncpy(upk, BFLRG, pck, -1);
    upcase(upk);

    fh = bd->fp[1];
    if (fh == NULL)
       return;

    csep(fh);
    csep(fh);
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

/* emit dummy to end the list of extern PyObjects */
    fprintf(fh, " _PY_%s_null;\n", pck);
    fprintf(fh, "\n");

#ifdef METHOD_DEF_VAR
/* method declarations */
    char dfn[BFLRG];
    char *cfn;
    fprintf(fh, "extern PyMethodDef\n");
    for (ib = 0; ib < ndcl; ib++)
        {dcl = dcls + ib;
	 cfn = dcl->proto.name;
	 pfn = has_binding(dcl, "python");
	 if ((pfn != NULL) && (strcmp(pfn, "none") != 0) &&
	     (is_var_arg(dcl->proto.arg) == FALSE))
	    {python_method_def(dfn, BFLRG, dcl, pfn, TRUE);
	     fprintf(fh, " _PYD_%s,\n", cfn);};}
    fprintf(fh, " _PYD_%s_null;\n", pck);
    fprintf(fh, "\n");
#else
/* GOTCHA: this is emitted in the C file as a PyMethodDef variable
 * so the #define should not be needed
 * also the identical name for the variable and the macro is a problem
 */
/* method definition macros */
    char dfn[BFLRG];
    char *cfn;
    for (ib = 0; ib < ndcl; ib++)
        {dcl = dcls + ib;
	 cfn = dcl->proto.name;
	 pfn = has_binding(dcl, "python");
	 if ((pfn != NULL) && (strcmp(pfn, "none") != 0) &&
	     (is_var_arg(dcl->proto.arg) == FALSE))
	    {python_method_def(dfn, BFLRG, dcl, pfn, TRUE);
	     fprintf(fh, "#define _PYD_%s %s\n",
		     cfn, subst(dfn, "\n", "\\\n", -1));
	     fprintf(fh, "\n");};};
#endif

    fprintf(fh, "extern int\n");
    fprintf(fh, " PY_init_%s(PyObject *m, PyObject *d);\n", pck);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_INSTALL - write the routine to install the bindings */

static void python_install(bindes *bd)
   {FILE *fc;

    fc = bd->fp[0];

    csep(fc);

/* make the list of enum constants to install */
    emit_enum_defs(bd, python_enum_defs);

/* make the list of struct objects */
    emit_struct_defs(bd, python_object_defs);

/* make the list of struct constants to install */
    emit_struct_defs(bd, python_struct_defs);

    csep(fc);

#ifdef METHOD_DEF_VAR
    int ib, ndcl;
    char dfn[BFLRG];
    char *pck, *cfn, *pfn;
    fdecl *dcl, *dcls;
    statedes *st;

    st   = bd->st;
    pck  = st->pck;
    ndcl = st->ndcl;
    dcls = st->dcl;

    csep(fc);
    fprintf(fc, "\n");

    fprintf(fc, "PyMethodDef\n");

/* method definition wrapper */
    for (ib = 0; ib < ndcl; ib++)
        {dcl = dcls + ib;
	 cfn = dcl->proto.name;
	 pfn = has_binding(dcl, "python");
	 if ((pfn != NULL) && (strcmp(pfn, "none") != 0) &&
	     (is_var_arg(dcl->proto.arg) == FALSE))
	    {python_method_def(dfn, BFLRG, dcl, pfn, TRUE);
	     fprintf(fc, " _PYD_%s = %s,\n", cfn, dfn);};};

    fprintf(fc, " _PYD_%s_null;\n", pck);
    fprintf(fc, "\n");
    csep(fc);
#endif

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
    FILE *fc;

    fc   = bd->fp[0];
    st   = bd->st;
    dcls = st->dcl;
    ndcl = st->ndcl;

    rv = TRUE;

    for (ib = 0; ib < ndcl; ib++)
        {dcl = dcls + ib;
	 pfn = has_binding(dcl, "python");
	 if (pfn != NULL)
	    python_wrap(fc, dcl, pfn);};

/* write the method definitions */
    python_install(bd);

/* write the method declarations */
    python_header(bd);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIN_PYTHON - finalize Python file */

static void fin_python(bindes *bd)
   {int i;
    FILE *fc, *fh, *fp;

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

/* DOC_PROTO_PYTHON - render the arg list of DCL into A for the
 *                  - Python callable C wrapper
 */

static void doc_proto_python(char *a, int nc, char *dcn, fdecl *dcl)
   {char t[BFLRG];
    farg *alc;

    alc = get_class_arg(dcl);

    a[0] = '\0';

    if (alc != NULL)
       vstrcat(a, nc, "%s.%s", alc->name, dcn);
    else
       vstrcat(a, nc, "%s", dcn);

    if_call_list(t, BFLRG, dcl, ",");
    vstrcat(a, nc, "(%s)", t);

    memmove(a, trim(a, BOTH, " "), nc);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BIND_DOC_PYTHON - emit Python binding documentation */

static void bind_doc_python(FILE *fp, fdecl *dcl, doc_kind dk)
   {char ap[BFLRG], dcn[BFLRG];
    char *bfn, *cfn;

    cfn = dcl->proto.name;
    bfn = has_binding(dcl, "python");

    if (dk == DK_HTML)
       {if (bfn == NULL)
	   fprintf(fp, "<i>Python Binding: </i>  none\n");
	else if (dcl->bindings != NULL)
	   {map_name(dcn, BFLRG, cfn, bfn, NULL, -1, FALSE, TRUE);
	    doc_proto_python(ap, BFLRG, dcn, dcl);
	    fprintf(fp, "<i>Python Binding: </i>  %s\n", ap);};}

    else if (dk == DK_MAN)
       {if (bfn == NULL)
	   {fprintf(fp, ".B Python Binding:  none\n");
	    fprintf(fp, ".sp\n");}
	else if (dcl->bindings != NULL)
	   {map_name(dcn, BFLRG, cfn, bfn, NULL, -1, FALSE, TRUE);
	    doc_proto_python(ap, BFLRG, dcn, dcl);
	    fprintf(fp, ".B Python Binding:  %s\n", ap);
	    fprintf(fp, ".sp\n");};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CL_PYTHON - process command line arguments for Python binding */

static int cl_python(statedes *st, bindes *bd, int c, char **v)
   {int i;

    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-h") == 0)
            {printf("   Python options: [-nop]\n");
             printf("      nop  do not generate python interfaces\n");
             printf("\n");
             return(1);}
	 else if (strcmp(v[i], "-nop") == 0)
	    st->no[MODE_P] = FALSE;};

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REGISTER_PYTHON - register PYTHON binding methods */

static int register_python(int fl, statedes *st)
   {int i;
    bindes *pb;

    MODE_P = nbd;

    if (fl == TRUE)
       {pb = gbd + nbd++;
	for (i = 0; i < NF; i++)
	    pb->fp[i] = NULL;

	pb->st   = st;
	pb->cl   = cl_python;
	pb->init = init_python;
	pb->bind = bind_python;
	pb->doc  = bind_doc_python;
	pb->fin  = fin_python;};

    return(MODE_P);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

