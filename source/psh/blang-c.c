/*
 * BLANG-C.C - generate C language bindings
 *
 * #include "cpyright.h"
 *
 */

typedef struct s_cmeta cmeta;

struct s_cmeta
   {char **hdrs;
    char **enums;
    char **structs;
    char **unions;};

int
 MODE_C = -1;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BIND_DOC_C - emit C binding documentation */

static void bind_doc_c(FILE *fp, fdecl *dcl, doc_kind dk)
   {char pr[BFLRG];

    c_proto(pr, BFLRG, dcl);

    if (dk == DK_HTML)
       fprintf(fp, "<i>C Binding: </i>       %s\n", pr);

    else if (dk == DK_MAN)
       {fprintf(fp, ".B C Binding:       %s\n", pr);
        fprintf(fp, ".sp\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CL_C - process command line arguments for C binding */

static int cl_c(statedes *st, bindes *bd, int c, char **v)
   {int i, rv;
    char *cpr, *cdv, **sdv, **scp;

    bd->iparam = MAKE_N(int, 2);

    rv  = 1;
    cpr = "";
    cdv = "";

    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-c") == 0)
	    cpr = v[++i];
	 else if (strcmp(v[i], "-dv") == 0)
	    cdv = v[++i];
	 else if (strcmp(v[i], "-h") == 0)
            {printf("   C specifications: -c <c-proto> -dv <c-der>\n");
             printf("      c    file containing C prototypes\n");
             printf("      dv   file containing C enum,struct,union defs\n");
             printf("\n");
	     cpr = "";
	     break;};};

    if (IS_NULL(cpr) == FALSE)
       {sdv = file_text(FALSE, cdv);
	scp = file_text(FALSE, cpr);

	st->cdv = sdv;
	st->cpr = scp;

	bd->iparam[0] = lst_length(sdv);
	bd->iparam[1] = lst_length(scp);

	rv = 0;}

    else
       printf("No prototypes found for '%s'\n", st->pck);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* C_EMIT_TYPES_DEF - emit type related definitions into source file */

static void c_emit_types_def(FILE *fc, der_list *sl, char *pck,
			     int ni, int fl)
   {

    if (fl == TRUE)
       {fprintf(fc, "char *G_%s_names[%d];\n", pck, 2*ni);
	fprintf(fc, "\n");}

    else
       fprintf(fc, "int %s = -1;\n", sl->na.inm);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* C_EMIT_TYPES_HDR - emit type related declarations and macros */

static void c_emit_types_hdr(FILE *fh, der_list *sl, char *pck,
			     int ni, int fl)
   {int i, nc;
    char a[BFLRG];
    char *mbr, *p;
    mbrdes *md;
    tn_list *tl;

    if (fl == TRUE)
       {fprintf(fh, "extern char *G_%s_names[];\n", pck);
	fprintf(fh, "\n");}

    else
       {md = sl->md;
	tl = &sl->na;

/* emit macro to define type to PDB file */
	fprintf(fh, "#define %s(_f)\t\\\n", tl->dnm);
	fprintf(fh, "   (PD_defstr_i(_f, \"%s\", \t\\\n", tl->cnm);

	a[0] = '\0';
	for (i = 0; md[i].text != NULL; i++)
	    {mbr = md[i].text;
	     if (md[i].cast != CAST_NONE)
		{p  = strstr(mbr, "MBR(");
		 *p = '\0';};

	     if (md[i].is_fnc_ptr == B_T)
	        fprintf(fh, "\t\t\"function %s\",\t\\\n", md[i].name);
	     else
	        fprintf(fh, "\t\t\"%s\",\t\\\n", trim(mbr, BOTH, " \t"));
	     
	     if (md[i].cast == CAST_TYPE)
	        vstrcat(a, BFLRG, 
			"    PD_cast(_f, \"%s\", \"%s\", \"%s\") &&\t\\\n",
			tl->cnm, md[i].name, md[i].cast_mbr);
	     else if (md[i].cast == CAST_LENGTH)
	        vstrcat(a, BFLRG, 
			"    PD_size_from(_f, \"%s\", \"%s\", \"%s\") &&\t\\\n",
			tl->cnm, md[i].name, md[i].cast_mbr);};

	if (IS_NULL(a) == FALSE)
	   {fprintf(fh, "\t\tLAST) &&\t\\\n");
	    nc = strlen(a);
	    nstrncpy(a+nc-6, 3, ")\n", 2);
	    fputs(a, fh);}
	else
	   fprintf(fh, "\t\tLAST))\n");

	fprintf(fh, "\n");

/* emit declaration of SCORE type index */
	fprintf(fh, "#define G_%s_S   G_%s_names[%d]\n",
		tl->unm, pck, 2*ni);
	fprintf(fh, "#define G_%s_P_S G_%s_names[%d]\n",
		tl->unm, pck, 2*ni+1);

	fprintf(fh, "extern int %s;\n", tl->inm);
	fprintf(fh, "\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* C_ENUM_DEFS - generate coding to define enums to PDBLib */

static void c_enum_defs(bindes *bd, char *tag, der_list *el, int ie, int ni)
   {FILE *fh, **fpa;
    tn_list tl;

    fpa = bd->fp;

    fh = fpa[1];

    if (el == NULL)
       {if (strcmp(tag, "begin") == 0)
	   {}
        else if (strcmp(tag, "end") == 0)
	   {};}

    else if (el->kind == TK_ENUM)
       {tl = el->na;

/* emit macro to define enum to PDB file */
	fprintf(fh, "#define %s(_f)\t", tl.enm);
	fprintf(fh, "PD_defenum(_f, \"%s\")\n", tl.cnm);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* C_OBJECT_DEFS - generate coding to define SCORE type manager index
 *               - and PDBLib PD_defstr call
 */

static void c_object_defs(bindes *bd, char *tag, der_list *sl, int is, int ni)
   {char *pck;
    FILE *fc, *fh, **fpa;

    fpa = bd->fp;
    pck = bd->st->pck;

    fc = fpa[0];
    fh = fpa[1];

    if (sl == NULL)
       {if (strcmp(tag, "begin") == 0)
	   {c_emit_types_def(fc, sl, pck, ni, TRUE);
	    c_emit_types_hdr(fh, sl, pck, ni, TRUE);}
        else if (strcmp(tag, "end") == 0)
	   {};}

    else if (sl->kind == TK_STRUCT)
       {c_emit_types_def(fc, sl, pck, is, FALSE);
        c_emit_types_hdr(fh, sl, pck, is, FALSE);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* C_TYPE_REG - generate coding to register C types with SCORE type manager */

static void c_type_reg(bindes *bd, char *tag, der_list *sl, int is, int ni)
   {FILE *fc, **fpa;
    tn_list *tl;

    fpa = bd->fp;
    tl  = &sl->na;

    fc = fpa[0];

    if (sl == NULL)
       {if (strcmp(tag, "begin") == 0)
	   {}
        else if (strcmp(tag, "end") == 0)
	   {};}

    else if (sl->kind == TK_STRUCT)

/* initialize G_*_I */
       {fprintf(fc, "       %s   = SC_type_register(\"%s\", KIND_STRUCT,\n",
		tl->inm, tl->cnm);
	fprintf(fc, "\t\t\t\tTRUE, sizeof(%s), 0);\n",
		tl->cnm);

/* initialize G_*_S and G_*_P_S */
	fprintf(fc, "       %s   = CSTRSAVE(\"%s\");\n",
		tl->snm, tl->cnm);
	fprintf(fc, "       G_%s_P_S = CSTRSAVE(\"%s *\");\n",
		tl->unm, tl->cnm);

	fprintf(fc, "\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BIND_C - generate C bindings from CPR and SBI
 *        - return TRUE iff successful
 */

static int bind_c(bindes *bd)
   {int rv;
    FILE *fc, *fh;
    statedes *st;

    rv = TRUE;
    fc = bd->fp[0];
    fh = bd->fp[1];
    st = bd->st;

/* make the list of enum objects */
    foreach_enum_defs(bd, c_enum_defs, TRUE);
    fprintf(fh, "\n");
    fprintf(fh, "\n");

/* make the list of struct objects */
    foreach_struct_defs(bd, c_object_defs, TRUE);

    fprintf(fh, "extern void G_register_%s_types(void);\n", st->pck);
    fprintf(fh, "\n");

    fprintf(fc, "\n");
    csep(fc);
    csep(fc);
    fprintf(fc, "\n");

    fprintf(fc, "void G_register_%s_types(void)\n", st->pck);
    fprintf(fc, "   {\n");
    fprintf(fc, "\n");
    fprintf(fc, "    ONCE_SAFE(TRUE, NULL)\n");
    foreach_struct_defs(bd, c_type_reg, TRUE);
    fprintf(fc, "    END_SAFE;\n");
    fprintf(fc, "\n");

    fprintf(fc, "    return;}\n");

    fprintf(fc, "\n");
    csep(fc);
    csep(fc);
    fprintf(fc, "\n");

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_C - initialize C files */

static void init_c(statedes *st, bindes *bd)
   {int i;
    char fn[BFLRG], upck[BFLRG], s[BFMG];
    char *p, *pck, **el, **sl, **ul, **hl;
    FILE *fc, *fh, *fp;
    cmeta *cm;

    pck = st->pck;
    snprintf(upck, BFLRG, pck, -1);
    upcase(upck);

/* make the C metadata from the derivedc file */
    hl = NULL;
    el = NULL;
    sl = NULL;
    ul = NULL;
    fp = open_file("r", "%s.derivedc", pck);
    if (fp != NULL)
       {for (i = 0; TRUE; i++)
	    {p = fgets(s, BFMG, fp);
	     if (p == NULL)
	        break;
	     LAST_CHAR(p) = '\0';
	     if (blank_line(p) == TRUE)
	        continue;
	     else if (strncmp(p, "include = ", 10) == 0)
	        hl = tokenize(p+10, " \t", 0);
	     else if (strncmp(p, "enum e_", 7) == 0)
	        el = lst_add(el, p);
	     else if (strncmp(p, "struct s_", 9) == 0)
	        sl = lst_add(sl, p);
	     else if (strncmp(p, "union u_", 8) == 0)
	        ul = lst_add(ul, p);};

	el = lst_add(el, NULL);
	sl = lst_add(sl, NULL);
	ul = lst_add(ul, NULL);

	fclose(fp);};

    cm = MAKE(cmeta);
    cm->hdrs    = hl;
    cm->enums   = el;
    cm->structs = sl;
    cm->unions  = ul;
    bd->data    = cm;

/* open C file */
    if ((st->path == NULL) || (strcmp(st->path, ".") == 0))
       snprintf(fn, BFLRG, "gc-%s.c", pck);
    else
       snprintf(fn, BFLRG, "%s/gc-%s.c", st->path, pck);

    fc = open_file("w", fn);
    bd->fp[0] = fc;

    fprintf(fc, "/*\n");
    fprintf(fc, " * GC-%s.C - generated support routines for %s\n",
	    upck, upck);
    fprintf(fc, " *\n");
    fprintf(fc, " */\n");
    fprintf(fc, "\n");

    fprintf(fc, "#include \"cpyright.h\"\n");
    if (hl != NULL)
       {for (i = 0; hl[i] != NULL; i++)
	    fprintf(fc, "#include \"%s\"\n", hl[i]);};
/*    fprintf(fc, "#include \"%s_int.h\"\n", pck); */
    fprintf(fc, "#include \"%s_gen.h\"\n", pck);
    fprintf(fc, "\n");

/* open header file */
    if ((st->path == NULL) || (strcmp(st->path, ".") == 0))
       snprintf(fn, BFLRG, "gc-%s.h", pck);
    else
       snprintf(fn, BFLRG, "%s/gc-%s.h", st->path, pck);

    fh = open_file("w", fn);
    bd->fp[1] = fh;

    fprintf(fh, "/*\n");
    fprintf(fh, " * GC-%s.H - header containing generated support for %s\n",
	    upck, upck);
    fprintf(fh, " *\n");
    fprintf(fh, " */\n");
    fprintf(fh, "\n");

    fprintf(fh, "#include \"cpyright.h\"\n");
    fprintf(fh, "\n");
    fprintf(fh, "#ifndef GEN_%s_H\n", upck);
    fprintf(fh, "#define GEN_%s_H\n", upck);
    fprintf(fh, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIN_C - finalize C file */

static void fin_c(bindes *bd)
   {int i;
    FILE *fh, *fp;
    cmeta *cm;

    cm = bd->data;
    FREE(cm->enums);
    FREE(cm->structs);
    FREE(cm->unions);
    FREE(cm);

/* finish off the C file */

/* finish off the header file */
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

/* REGISTER_C - register C binding methods
 *            - this is a dummy - crucial but empty
 */

static int register_c(int fl, statedes *st)
   {int i;
    bindes *pb;

    MODE_C = nbd;

    if (fl == TRUE)
       {pb = gbd + nbd++;
	for (i = 0; i < NF; i++)
	    pb->fp[i] = NULL;

	pb->lang = "C";
	pb->st   = st;
	pb->data = NULL;
	pb->cl   = cl_c;
	pb->init = init_c;
	pb->bind = bind_c;
	pb->doc  = bind_doc_c;
	pb->fin  = fin_c;};

    return(MODE_C);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

