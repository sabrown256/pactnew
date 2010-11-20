/*
 * PASTR.C - various string and structure manipulation functions supporting
 *         - PANACEA
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "panacea_int.h"

char 
 *PA_token_delimiters = " \t\r\n";

/*--------------------------------------------------------------------------*/

/*                            STRUCTURE FUNCTIONS                           */

/*--------------------------------------------------------------------------*/

/* PA_INT_INSTANCE - intern an instance of
 *                 - a type defined in the virtual internal file
 */

syment *PA_int_instance(char *name, char *type, long n, void *data)
   {syment *ep;
    SC_address addr;

    addr.memaddr = data;

    ep = _PD_mk_syment(type, n, addr.diskaddr, NULL, NULL);
    _PD_e_install(PA_vif, name, ep, TRUE);

    return(ep);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_MK_INSTANCE - allocate and return a pointer to an instance of
 *                - a type defined in the virtual internal file
 */

void *PA_mk_instance(char *name, char *type, long n)
   {void *data;
    syment *ep;
    SC_address addr;

    data = _PD_alloc_entry(PA_vif, type, n);

    addr.memaddr = data;

    ep = _PD_mk_syment(type, n, addr.diskaddr, NULL, NULL);
    _PD_e_install(PA_vif, name, ep, TRUE);

    return(data);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_RL_INSTANCE - release an instance of a type defined
 *                - in the virtual internal file
 */

void PA_rl_instance(char *name)
   {syment *ep;
    SC_address addr;

    ep = PD_inquire_entry(PA_vif, name, FALSE, NULL);
    if (ep == NULL)
       return;

    addr.diskaddr = PD_entry_address(ep);
    SFREE(addr.memaddr);

    _PD_rl_syment_d(ep);
    SC_hasharr_remove(PA_vif->symtab, name);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_SET_MEMBER - set the member of an internal structure to
 *               - point to the given space
 */

void PA_set_member(char *name, void *data, char *member)
   {int id;
    int *pi;
    short *ps;
    long *pl;
    float *pf;
    double *pd;
    char s[MAXLINE], **ppc, *pc, *type;
    syment *ep;
    SC_address addr;

    snprintf(s, MAXLINE, "%s.%s", name, member);
    ep = _PD_effective_ep(PA_vif, s, FALSE, NULL);

    addr.diskaddr = PD_entry_address(ep);

    type = PD_entry_type(ep);
    id   = SC_type_id(type, FALSE);

    if (_PD_indirection(type))
       {ppc  = (char **) addr.memaddr;
        *ppc = (char *) data;}

/* floating point types */
    else if (id == SC_DOUBLE_I)
       {pd  = (double *) addr.memaddr;
        *pd = *(double *) data;}

    else if (id == SC_FLOAT_I)
       {pf  = (float *) addr.memaddr;
        *pf = *(float *) data;}

/* fixed point types */
    else if (id == SC_INT_I)
       {pi  = (int *) addr.memaddr;
        *pi = *(int *) data;}

    else if (id == SC_LONG_I)
       {pl  = (long *) addr.memaddr;
        *pl = *(long *) data;}

    else if (id == SC_SHORT_I)
       {ps  = (short *) addr.memaddr;
        *ps = *(short *) data;}

    else if (id == SC_CHAR_I)
       {pc  = (char *) addr.memaddr;
        *pc = *(char *) data;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_GET_MEMBER - get the member of an internal structure
 *               - and return a pointer to it
 */

void *PA_get_member(char *name, char *member)
   {char s[MAXLINE];
    void *rv;
    syment *ep;
    SC_address addr;

    snprintf(s, MAXLINE, "%s.%s", name, member);
    ep = _PD_effective_ep(PA_vif, s, FALSE, NULL);

    addr.diskaddr = PD_entry_address(ep);
    rv            = addr.memaddr;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
