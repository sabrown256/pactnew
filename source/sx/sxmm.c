/*
 * SXMM.C - constructors and destructors for SX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "sx_int.h"

/*--------------------------------------------------------------------------*/

/*                              PDB SECTION                                 */

/*--------------------------------------------------------------------------*/

/* _SX_MK_PDBDATA - make and return g_pdbdata */

g_pdbdata *_SX_mk_pdbdata(char *name, void *data, syment *ep, PDBfile *file)
   {g_pdbdata *pp;

    pp = CMAKE(g_pdbdata);

    SC_mark(data, 1);
    SC_mark(ep, 1);
    SC_mark(file, 1);

    pp->name = CSTRSAVE(name);
    pp->data = data;
    pp->ep   = ep;
    pp->file = file;

    return(pp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_RL_GPDBDATA - release g_pdbdata */

static void _SX_rl_gpdbdata(object *obj)
   {g_pdbdata *pp;

    pp = SS_GET(g_pdbdata, obj);
    if (SC_safe_to_free(pp))
       {CFREE(pp->name);

/* for now this only lowers the reference count but is NOT prepared to
 * free the objects themselves - that is for the next pass
 */
	CFREE(pp->data);
	CFREE(pp->file);
	CFREE(pp->ep);};

    CFREE(pp);

    SS_rl_object(obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_WR_GPDBDATA - print a g_pdbdata */

static void _SX_wr_gpdbdata(object *obj, object *strm)
   {

    PRINT(SS_OUTSTREAM(strm), "<PDBDATA|%s>", PDBDATA_NAME(obj));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MK_GPDBDATA - encapsulate a g_pdbdata as an object */

object *_SX_mk_gpdbdata(SS_psides *si, char *name,
			void *data, syment *ep, PDBfile *file)
   {g_pdbdata *pp;
    object *op;

    pp = _SX_mk_pdbdata(name, data, ep, file);

    op = SS_mk_object(si, pp, G_PDBDATA, SELF_EV, pp->name,
		      _SX_wr_gpdbdata, _SX_rl_gpdbdata);
    SC_mark(pp, 1);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_WR_GSYMENT - print a g_syment */

static void _SX_wr_gsyment(object *obj, object *strm)
   {

    PRINT(SS_OUTSTREAM(strm), "<SYMENT|%s>", SYMENT_TYPE(obj));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MK_GSYMENT - encapsulate a syment as an object */

object *_SX_mk_gsyment(SS_psides *si, syment *ep)
   {object *op;

    if (ep == NULL)
       return(SS_null);

    op = SS_mk_object(si, ep, G_SYMENT, SELF_EV, ep->type,
		      _SX_wr_gsyment, SS_rl_object);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_WR_GDEFSTR - print a g_defstr */

static void _SX_wr_gdefstr(object *obj, object *strm)
   {

    PRINT(SS_OUTSTREAM(strm), "<DEFSTR|%s>", DEFSTR_TYPE(obj));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MK_GDEFSTR - encapsulate a defstr as an object */

object *_SX_mk_gdefstr(SS_psides *si, defstr *dp)
   {object *op;

    if (dp == NULL)
       return(SS_null);

    op = SS_mk_object(si, dp, G_DEFSTR, SELF_EV, dp->type,
		      _SX_wr_gdefstr, SS_rl_object);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_WR_GFILE - print a g_file */

static void _SX_wr_gfile(object *obj, object *strm)
   {PDBfile *file;
     g_file *po;

    po = NULL;
    SS_args(obj,
            G_FILE, &po,
	    0);

    file = FILE_FILE(PDBfile, po);

    PRINT(SS_OUTSTREAM(strm), "<%s|%s|%s>",
	  FILE_TYPE(obj), file->tr->type, FILE_NAME(obj));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MK_FILE - make and return a g_file */

g_file *_SX_mk_file(char *name, char *type, void *file)
   {g_file *po;

    po = CMAKE(g_file);

    SC_mark(file, 1);

    po->name        = CSTRSAVE(name);
    po->type        = CSTRSAVE(type);
    po->file        = file;
    po->file_object = NULL;
    po->menu_lst    = CMAKE_ARRAY(SX_menu_item, NULL, 0);
    po->next        = SX_file_list;

    return(po);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_RL_MENU_ITEM - release a SX_menu_item */

static int _SX_rl_menu_item(void *a)
   {SX_menu_item *mi;

    if (a != NULL)
       {mi = (SX_menu_item *) a;
        CFREE(mi->vname);
        CFREE(mi->label);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_FREE_MENU - release the menus associated with a file */

void _SX_free_menu(g_file *po, int re)
   {

    SC_free_array(po->menu_lst, _SX_rl_menu_item);
    if (re == TRUE)
       po->menu_lst = CMAKE_ARRAY(SX_menu_item, NULL, 0);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_RL_FILE - release a g_file
 *             - assume the caller closed the file
 */

static void _SX_rl_file(g_file *po)
   {

    _SX_free_menu(po, FALSE);

    CFREE(po->file);
    CFREE(po->type);
    CFREE(po->name);

    po->file_object = NULL;

    CFREE(po);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_MK_GFILE - make and return type g_file */

object *SX_mk_gfile(SS_psides *si, g_file *po)
   {object *op;

    if (po == NULL)
       return(SS_null);

    SC_mark(po, 1);

    op = SS_mk_object(si, po, G_FILE, SELF_EV, po->name,
		      _SX_wr_gfile, SS_rl_object);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MK_OPEN_FILE - initialize an g_file and return a pointer
 *                  - to it
 */

g_file *_SX_mk_open_file(SS_psides *si, char *name, char *type, char *mode)
   {PDBfile *file;
    object *obj;
    g_file *po;

/* open the file */
    file = PD_open(name, mode);

/* if append or write mode open failed then try read-only mode */
    if ((file == NULL) && (strcmp(mode, "r") != 0))
       file = PD_open(name, "r");

    if (file == NULL)

/* complain about text file */
       {if (SC_isfile_text(name))
           SS_error("REQUESTED FILE IS ASCII - _SX_MK_OPEN_FILE",
                     SS_mk_string(si, name));

/* return NULL if file exists but could not be opened
 * so Scheme spoke can be tried
 */
        else if (SC_isfile(name))
	   return(NULL);

/* if file does not exist then create it */
        else
           {file = PD_open(name, "w");
            if (file == NULL)
               SS_error("CAN'T OPEN FILE - _SX_MK_OPEN_FILE", 
                        SS_mk_string(si, name));};};

    _PD_def_real(type, file);

    po  = _SX_mk_file(name, type, file);
    obj = SX_mk_gfile(si, po);
    SS_UNCOLLECT(obj);

/* GOTCHA: since po points to obj and obj points to po
 *         handle the GC by insisting that anybody who
 *         assigns to obj must take responsibility for 
 *         marking it
 */
    po->file_object = obj;

    return(po);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_REL_OPEN_FILE - GC an g_file
 *                   - include closing the file
 */

void _SX_rel_open_file(g_file *po)
   {PDBfile *file;

    file = po->file;

/* close the file */
    if (!PD_close(file))
       SS_error("CAN'T PROPERLY CLOSE FILE - _SX_REL_OPEN_FILE", 
                po->file_object);

    _SX_rl_file(po);

    return;}
    
/*--------------------------------------------------------------------------*/

/*                              PANACEA SECTION                             */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/*                                PGS SECTION                               */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/*                                PML SECTION                               */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
