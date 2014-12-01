/*
 * BLANG-BA.C - generate Basis language bindings
 *
 * #include "cpyright.h"
 *
 */

static int
 MODE_B = -1;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_BASIS - initialize Basis file */

static void init_basis(statedes *st, bindes *bd)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BIND_BASIS - generate Basis bindings from CPR and SBI
 *            - return TRUE iff successful
 */

static int bind_basis(bindes *bd)
   {int rv;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIN_BASIS - finalize Basis file */

static void fin_basis(bindes *bd)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BIND_DOC_BASIS - emit Basis binding documentation */

static void bind_doc_basis(FILE *fp, fdecl *dcl, doc_kind dk)
   {

    if (dk == DK_HTML)
       {}

    else if (dk == DK_MAN)
       {};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CL_BASIS - process command line arguments for Basis binding */

static int cl_basis(statedes *st, bindes *bd, int c, char **v)
   {

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REGISTER_BASIS - register BASIS binding methods */

static int register_basis(int fl, statedes *st)
   {int i;
    bindes *pb;

    MODE_B = nbd;

    if (fl == TRUE)
       {pb = gbd + nbd++;
	for (i = 0; i < NF; i++)
	    pb->fp[i] = NULL;

	pb->lang = "basis";
	pb->st   = st;
	pb->data = NULL;
	pb->cl   = cl_basis;
	pb->init = init_basis;
	pb->bind = bind_basis;
	pb->doc  = bind_doc_basis;
	pb->fin  = fin_basis;};

    return(MODE_B);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

