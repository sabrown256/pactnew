/*
 * BLANG-BA.C - generate Basis language bindings
 *
 * #include "cpyright.h"
 *
 */

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

/* REGISTER_BASIS - register BASIS binding methods */

static int register_basis(int fl, statedes *st)
   {int nb;
    bindes *pb;

    if (fl == TRUE)
       {nb = nbd;

	pb = gbd + nbd++;
	pb->st   = st;
	pb->fp   = NULL;
	pb->init = init_basis;
	pb->bind = bind_basis;
	pb->fin  = fin_basis;};

    return(nb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
