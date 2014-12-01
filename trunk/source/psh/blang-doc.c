/*
 * BLANG-DOC.C - generate documentation
 *
 * #include "cpyright.h"
 *
 */

static int
 MODE_DOC = -1;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_DOC - initialize Doc file */

static void init_doc(statedes *st, bindes *bd)
   {char fn[BFLRG];
    char *pck;
    FILE *fp;

    pck = st->pck;

    if ((st->path == NULL) || (strcmp(st->path, ".") == 0))
       snprintf(fn, BFLRG, "gh-%s.html", pck);
    else
       snprintf(fn, BFLRG, "%s/gh-%s.html", st->path, pck);

    fp = open_file("w", fn);

    fprintf(fp, "\n");

    hsep(fp);

    bd->fp[0] = fp;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DOC_PROTO_NAME_ONLY - render the arg list of DCL into A using
 *                     - variable names only
 */

void doc_proto_name_only(char *a, int nc, fdecl *dcl, char *dlm)
   {int i, na;
    farg *al;

    na = dcl->na;
    al = dcl->al;

    a[0] = '\0';
    if (na != 0)
       {if (dlm == NULL)
	   {for (i = 0; i < na; i++)
	        vstrcat(a, BFLRG, " %s", al[i].name);}
	else
	   {for (i = 0; i < na; i++)
	        vstrcat(a, BFLRG, " %s%s", al[i].name, dlm);
	    a[strlen(a)-strlen(dlm)] = '\0';};};

    memmove(a, trim(a, BOTH, " "), nc);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIND_COMMENT - return a pointer to the part of CDC which
 *              - documents CFN
 */

static char **find_comment(char *cfn, int ndc, char **cdc)
   {int i;
    char *s, **com;

    com = NULL;
    for (i = 0; (i < ndc) && (com == NULL); i++)
        {s = cdc[i];
	 if ((strstr(s, "#bind") != NULL) &&
	     (strstr(s, cfn) != NULL))
	    {for ( ; i >= 0; i--)
	         {s = cdc[i];
                  if (strncmp(s, "/*", 2) == 0)
		     {com = cdc + i;
		      break;};};};};

    return(com);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PROCESS_DOC - process the comments into a string */

static void process_doc(char *t, int nc, char **com)
   {int i;
    char *s, *p;

    t[0] = '\0';
    if (com != NULL)
       {for (i = 0; IS_NULL(com[i]) == FALSE; i++)
	    {s = com[i] + 3;
	     if (strstr(s, "#bind ") == NULL)
	        {p = strchr(s, '-');
		 if (p != NULL)
		    vstrcat(t, nc, "%s\n", p+2);
		 else
		    nstrcat(t, nc, "\n");};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HTML_WRAP - wrap C function DCL in HTML form */

static void html_wrap(FILE *fp, fdecl *dcl, char *sb, int ndc, char **cdc)
   {int ib;
    char upn[BFLRG], lfn[BFLRG];
    char fty[BFLRG], t[BFLRG];
    char *cfn, **com;
    bindes *pb;

    cfn = dcl->proto.name;

    com = find_comment(cfn, ndc, cdc);

    nstrncpy(upn, BFLRG, cfn, -1);
    upcase(upn);

    nstrncpy(lfn, BFLRG, cfn, -1);
    downcase(lfn);

    cf_type(fty, BFLRG, dcl->proto.type);

    hsep(fp);
    fprintf(fp, "\n");

    fprintf(fp, "<a name=\"%s\"><h2>%s</h2></a>\n", lfn, upn);
    fprintf(fp, "\n");
    fprintf(fp, "<p>\n");
    fprintf(fp, "<pre>\n");

/* emit the contributions from the various languages */
    for (pb = gbd, ib = 0; ib < nbd; ib++, pb++)
        {if (pb->doc != NULL)
	    pb->doc(fp, dcl, DK_HTML);};

    fprintf(fp, "</pre>\n");
    fprintf(fp, "<p>\n");

    process_doc(t, BFLRG, com);
    if (IS_NULL(t) == FALSE)
       {fprintf(fp, "<pre>\n");
	fputs(t, fp);
	fprintf(fp, "</pre>\n");};

    fprintf(fp, "<p>\n");
    fprintf(fp, "\n");
    hsep(fp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAN_WRAP - wrap C function DCL in MAN form */

static void man_wrap(statedes *st, fdecl *dcl,
		     char *sb, char *pck, int ndc, char **cdc)
   {int ib, voidf;
    char fname[BFLRG], upk[BFLRG];
    char upn[BFLRG], lfn[BFLRG];
    char fty[BFLRG], t[BFLRG];
    char *cfn, **com;
    FILE *fp;
    bindes *pb;

    cfn   = dcl->proto.name;
    voidf = dcl->voidf;

    com = find_comment(cfn, ndc, cdc);

    nstrncpy(upk, BFLRG, pck, -1);
    upcase(upk);

    nstrncpy(upn, BFLRG, cfn, -1);
    upcase(upn);

    nstrncpy(lfn, BFLRG, cfn, -1);
    downcase(lfn);

    cf_type(fty, BFLRG, dcl->proto.type);

    if ((st->path == NULL) || (strcmp(st->path, ".") == 0))
       snprintf(fname, BFLRG, "%s.3", cfn);
    else
       snprintf(fname, BFLRG, "%s/%s.3", st->path, cfn);

    fp = fopen_safe(fname, "w");
    if (fp == NULL)
       return;

    fprintf(fp, ".\\\"\n");
    fprintf(fp, ".\\\" See the terms of include/cpyright.h\n");
    fprintf(fp, ".\\\"\n");
    fprintf(fp, ".TH %s 3  2011-06-21 \"%s\" \"%s Documentation\"\n",
	    upn, upk, upk);
    fprintf(fp, ".SH NAME\n");
    fprintf(fp, "%s \\- \n", cfn);
    fprintf(fp, ".SH SYNOPSIS\n");
    fprintf(fp, ".nf\n");

    fprintf(fp, ".B #include <%s.h>\n", pck);
    fprintf(fp, ".sp\n");

/* emit the contributions from the various languages */
    for (pb = gbd, ib = 0; ib < nbd; ib++, pb++)
        {if (pb->doc != NULL)
	    pb->doc(fp, dcl, DK_MAN);};

    process_doc(t, BFLRG, com);
    if (IS_NULL(t) == FALSE)
       {fprintf(fp, ".fi\n");
	fprintf(fp, ".SH DESCRIPTION\n");
	fprintf(fp, "%s\n", t);};

    if (voidf == TRUE)
       {fprintf(fp, ".SH RETURN VALUE\n");
	fprintf(fp, "none\n");
	fprintf(fp, ".sp\n");};

    fprintf(fp, ".SH COPYRIGHT\n");
    fprintf(fp, "include/cpyright.h\n");
    fprintf(fp, ".sp\n");

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BIND_DOC - generate Doc bindings from CPR and SBI
 *          - return TRUE iff successful
 */

static int bind_doc(bindes *bd)
   {int ib, ndc, ndcl, rv;
    char *pck, **cdc;
    fdecl *dcl, *dcls;
    statedes *st;
    FILE *fp;

    fp   = bd->fp[0];
    st   = bd->st;
    ndc  = bd->iparam[0];
    cdc  = st->cdc;
    pck  = st->pck;
    dcls = st->dcl;
    ndcl = st->ndcl;

    rv = TRUE;

    for (ib = 0; ib < ndcl; ib++)
        {dcl = dcls + ib;
	 if (dcl->error == FALSE)
	    {html_wrap(fp, dcl, NULL, ndc, cdc);
	      man_wrap(st, dcl, NULL, pck, ndc, cdc);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIN_DOC - finalize Doc file */

static void fin_doc(bindes *bd)
   {int i;
    FILE *fp;

    fp = bd->fp[0];
    hsep(fp);

    for (i = 0; i < NF; i++)
        {fp = bd->fp[i];
	 if (fp != NULL)
	    {fclose_safe(fp);
	     bd->fp[i] = NULL;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CL_DOC - process command line arguments for doc binding */

static int cl_doc(statedes *st, bindes *bd, int c, char **v)
   {int i;
    char *cdc, **sdc;

    bd->iparam = MAKE_N(int, 1);

    cdc = "";

    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-d") == 0)
	    cdc = v[++i];
	 else if (strcmp(v[i], "-h") == 0)
	    {printf("   Documentation options: [-d <doc>] [-nod]\n");
             printf("      d    file containing documentation comments\n");
             printf("      nod  do not generate documentation\n");
             printf("\n");
             return(1);}
	 else if (strcmp(v[i], "-nod") == 0)
	    st->no[MODE_DOC] = FALSE;};

    sdc = file_text(FALSE, cdc);

    st->cdc = sdc;

    bd->iparam[0] = lst_length(sdc);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REGISTER_DOC - register documentation methods */

static int register_doc(int fl, statedes *st)
   {int i;
    bindes *pb;

    MODE_DOC = nbd;

    if (fl == TRUE)
       {pb = gbd + nbd++;
	for (i = 0; i < NF; i++)
	    pb->fp[i] = NULL;

	pb->lang = "doc";
	pb->st   = st;
	pb->data = NULL;
	pb->cl   = cl_doc;
	pb->init = init_doc;
	pb->bind = bind_doc;
	pb->doc  = NULL;
	pb->fin  = fin_doc;};

    return(MODE_DOC);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

