/*
 * BLANG-C.C - generate C language bindings
 *
 * #include "cpyright.h"
 *
 */

typedef struct s_tnc_list tnc_list;

struct s_tnc_list
   {char cnm[BFSML];        /* C struct name, PM_set */
    char rnm[BFSML];        /* root struct id, SET */
    char lnm[BFSML];        /* lower case version of CNM, pm_set */
    char unm[BFSML];};      /* upper case version of CNM, PM_SET */

int
 MODE_C = -1;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* C_TYPE_NAME_LIST - make canonical variations of type name TYP */

static void c_type_name_list(char *typ, tnc_list *na)
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

/* get root struct name */
    if (p[2] == '_')
       nstrncpy(na->rnm, BFSML, p+3, -1);
    else
       nstrncpy(na->rnm, BFSML, p, -1);
    upcase(na->rnm);

    return;}

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

static void c_emit_types_def(FILE *fc, char **ta, tnc_list *tl)
   {

/* examples:
 *
 *  int SC_HAELEM_I;
 */

    fprintf(fc, "int G_%s_I;\n", tl->rnm);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* C_EMIT_TYPES_HDR - emit type related declarations and macros */

static void c_emit_types_hdr(FILE *fh, char **ta, tnc_list *tl)
   {int i;
    char nm[BFSML], ty[BFSML], dm[BFSML];
    char *mbr;

/* examples:
 *
 * typedef struct s_haelem haelem;
 * 
 * struct s_haelem                 
 *    {long iht;
 *     long iar;
 *     char *name;
 *     char *type;
 *     void *def;
 *     int free;
 *     haelem *next;};
 * 
 * #define PD_DEFINE_HAELEM(_f)                                       \
 *    {PD_defstr(_f, "haelem",                                        \
 * 	      "long iht",                                          \
 * 	      "long iar",                                          \
 * 	      "char *name",                                        \
 * 	      "char *type",                                        \
 * 	      "char *def",                                         \
 * 	      "int free",                                          \
 * 	      "haelem *next",                                      \
 * 	      LAST);                                               \
 *     PD_cast(_f, "haelem", "def", "type");}
 * 
 *  extern int SC_HAELEM_I;
 *
 */

/* emit macro to define type to PDB file */
    fprintf(fh, "#define G_DEFINE_%s(_f)\t\\\n", tl->rnm);
    fprintf(fh, "   PD_defstr(_f, \"%s\", \t\\\n", tl->cnm);

    for (i = 1; ta[i] != NULL; i++)
        {mbr = trim(ta[i], BOTH, " \t");
         if (IS_NULL(mbr) == FALSE)
	    {parse_member(mbr, nm, ty, dm, BFSML);
	     if (is_func_ptr(mbr, 1) == B_T)
	        fprintf(fh, "\t\t\"function %s\",\t\\\n", nm);
	     else
	        fprintf(fh, "\t\t\"%s\",\t\\\n", mbr);};};

    fprintf(fh, "\t\tLAST)\n");
    fprintf(fh, "\n");

/* emit declaration of SCORE type index */
    fprintf(fh, "extern int G_%s_I;\n", tl->rnm);
    fprintf(fh, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* C_OBJECT_DEFS - generate coding to define SCORE type manager index
 *               - and PDBLib PD_defstr call
 */

static void c_object_defs(FILE **fpa, char *dv, char **ta,
			  char *pck, int ni)
   {FILE *fc, *fh;
    tnc_list tl;

    fc = fpa[0];
    fh = fpa[1];

    if (ta == NULL)
       {if (strcmp(dv, "begin") == 0)
	   {}
        else if (strcmp(dv, "end") == 0)
	   {};}

    else if (strncmp(ta[0], "struct s_", 9) == 0)
       {c_type_name_list(ta[0]+9, &tl);
	c_emit_types_def(fc, ta, &tl);
        c_emit_types_hdr(fh, ta, &tl);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* C_TYPE_REG - generate coding to register C types with SCORE type manager */

static void c_type_reg(FILE **fpa, char *dv, char **ta,
		       char *pck, int ni)
   {FILE *fc;
    tnc_list tl;

    fc = fpa[0];

    if (ta == NULL)
       {if (strcmp(dv, "begin") == 0)
	   {}
        else if (strcmp(dv, "end") == 0)
	   {};}

    else if (strncmp(ta[0], "struct s_", 9) == 0)
/*  SC_HAELEM_I = SC_type_register(SC_HAELEM_S, KIND_STRUCT, sizeof(haelem), 0);
 */
       {c_type_name_list(ta[0]+9, &tl);

	fprintf(fc, "       G_%s_I = SC_type_register(\"%s\", KIND_STRUCT,\n",
		tl.rnm, tl.cnm);
	fprintf(fc, "\t\t\t\tsizeof(%s), 0);\n",
		tl.cnm);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BIND_C - generate C bindings from CPR and SBI
 *        - return TRUE iff successful
 */

static int bind_c(bindes *bd)
   {int rv;
    FILE *fc;
    statedes *st;

    rv = TRUE;
    fc = bd->fp[0];
    st = bd->st;

/* make the list of struct objects */
    emit_struct_defs(bd, c_object_defs);

    fprintf(fc, "\n");
    csep(fc);
    csep(fc);
    fprintf(fc, "\n");

    fprintf(fc, "void register_%s_types(void)\n", st->pck);
    fprintf(fc, "   {\n");
    fprintf(fc, "\n");
    fprintf(fc, "    ONCE_SAFE(TRUE, NULL)\n");
    emit_struct_defs(bd, c_type_reg);
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
   {char fn[BFLRG], upck[BFLRG];
    char *pck;
    FILE *fc, *fh;

    pck = st->pck;
    snprintf(upck, BFLRG, pck, -1);
    upcase(upck);

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
    fprintf(fc, "#include \"%s_int.h\"\n", pck);
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

	pb->st   = st;
	pb->cl   = cl_c;
	pb->init = init_c;
	pb->bind = bind_c;
	pb->doc  = bind_doc_c;
	pb->fin  = fin_c;};

    return(MODE_C);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

