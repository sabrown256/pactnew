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

/* _SX_MK_PDBDATA - make and return SX_pdbdata */

SX_pdbdata *_SX_mk_pdbdata(const char *name, void *data,
			   syment *ep, PDBfile *file)
   {SX_pdbdata *pp;

    pp = CMAKE(SX_pdbdata);

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

/* _SX_RL_GPDBDATA - release SX_pdbdata */

static void _SX_rl_gpdbdata(SS_psides *si, object *obj)
   {SX_pdbdata *pp;

    pp = SS_GET(SX_pdbdata, obj);
    if (SC_safe_to_free(pp))
       {CFREE(pp->name);

/* for now this only lowers the reference count but is NOT prepared to
 * free the objects themselves - that is for the next pass
 */
	CFREE(pp->data);
	CFREE(pp->file);
	CFREE(pp->ep);};

    CFREE(pp);

    SS_rl_object(si, obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_WR_GPDBDATA - print a SX_pdbdata */

static void _SX_wr_gpdbdata(SS_psides *si, object *obj, object *strm)
   {

    PRINT(SS_OUTSTREAM(strm), "<PDBDATA|%s>", PDBDATA_NAME(obj));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MK_GPDBDATA - encapsulate a SX_pdbdata as an object */

object *_SX_mk_gpdbdata(SS_psides *si, const char *name,
			void *data, syment *ep, PDBfile *file)
   {SX_pdbdata *pp;
    object *op;

    pp = _SX_mk_pdbdata(name, data, ep, file);

    op = SS_mk_object(si, pp, G_SX_PDBDATA_I, SELF_EV, pp->name,
		      _SX_wr_gpdbdata, _SX_rl_gpdbdata);
    SC_mark(pp, 1);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_WR_GFILE - print a SX_file */

static void _SX_wr_gfile(SS_psides *si, object *obj, object *strm)
   {char *tyw, *nm;
    const char *tya;
    PDBfile *file;
    SX_file *po;

    po  = NULL;
    tyw = "none";
    tya = "none";
    nm  = "none";
    SS_args(si, obj,
            G_SX_FILE_I, &po,
	    0);

    if (po != NULL)
       {if (po->type != NULL)
	   tyw = po->type;
	if (po->name != NULL)
	   nm  = po->name;};

    file = FILE_FILE(PDBfile, po);
    if ((file != NULL) && (file->tr != NULL))
	tya = file->tr->type;

    PRINT(SS_OUTSTREAM(strm), "<%s|%s|%s>", tyw, tya, nm);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MK_FILE - make and return a SX_file */

SX_file *_SX_mk_file(const char *name, const char *type, void *file)
   {SX_file *po;

    po = CMAKE(SX_file);

    SC_mark(file, 1);

    po->name        = CSTRSAVE(name);
    po->type        = CSTRSAVE(type);
    po->file        = file;
    po->file_object = NULL;
    po->menu_lst    = CMAKE_ARRAY(SX_menu_item, NULL, 0);
    po->next        = SX_gs.file_list;

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

void _SX_free_menu(SX_file *po, int re)
   {

    SC_free_array(po->menu_lst, _SX_rl_menu_item);
    if (re == TRUE)
       po->menu_lst = CMAKE_ARRAY(SX_menu_item, NULL, 0);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_RL_FILE - release a SX_file
 *             - assume the caller closed the file
 */

static void _SX_rl_file(SX_file *po)
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

/* SX_MK_GFILE - make and return type SX_file */

object *SX_mk_gfile(SS_psides *si, SX_file *po)
   {object *op;

    if (po == NULL)
       return(SS_null);

    SC_mark(po, 1);

    op = SS_mk_object(si, po, G_SX_FILE_I, SELF_EV, po->name,
		      _SX_wr_gfile, SS_rl_object);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MK_OPEN_FILE - initialize an SX_file and return a pointer
 *                  - to it
 */

SX_file *_SX_mk_open_file(SS_psides *si, const char *name,
			  const char *type, const char *mode)
   {char nm[MAXLINE];
    const char *pnm;
    PDBfile *file;
    object *obj;
    SX_file *po;
    tr_layer *tr;

    pnm = name;
    if (*mode == 'w')
       {tr = _PD_lookup_fn(name);
	if (tr != NULL)
	   {snprintf(nm, MAXLINE, "%s,fmt=%s", name, tr->fmt);
	    pnm = nm;};};

/* open the file */
    file = PD_open(pnm, mode);

/* if append or write mode open failed then try read-only mode */
    if ((file == NULL) && (strcmp(mode, "r") != 0))
       file = PD_open(pnm, "r");

    if (file == NULL)

/* complain about text file */
       {if (SC_isfile_text(name))
           SS_error(si, "REQUESTED FILE IS ASCII - _SX_MK_OPEN_FILE",
                     SS_mk_string(si, name));

/* return NULL if file exists but could not be opened
 * so Scheme spoke can be tried
 */
        else if (SC_isfile(name))
	   return(NULL);

/* if file does not exist then create it */
        else
           {file = PD_open(pnm, "w");
            if (file == NULL)
               SS_error(si, "CAN'T OPEN FILE - _SX_MK_OPEN_FILE", 
                        SS_mk_string(si, name));};};

    _PD_def_real(file, type);

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

/* _SX_REL_OPEN_FILE - GC an SX_file
 *                   - include closing the file
 */

void _SX_rel_open_file(SS_psides *si, SX_file *po)
   {PDBfile *file;

    if (po != NULL)
       {file = po->file;

/* close the file */
	if (file != NULL)
	   {if (!PD_close(file))
	       SS_error(si, "CAN'T PROPERLY CLOSE FILE - _SX_REL_OPEN_FILE", 
			po->file_object);};

	po->file = NULL;

	_SX_rl_file(po);};

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
