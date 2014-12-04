/*
 * SXHOOK.C - various function hooks for SX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "sx_int.h"

typedef struct s_SX_iodes SX_iodes;

struct s_SX_iodes
   {int fd;
    PROCESS *pp;
    object *fnc;};

static SX_iodes
 _SX_callbacks[100];

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_INS_MEM - replace member number IMEM with MEMBER
 *             - IMEM is 0 based
 */

static void _SX_ins_mem(SS_psides *si, defstr *dp,
			char *member, int imem, PDBfile *file)
   {memdes *lst, *prev, *desc;
    int i;

    for (prev = NULL, lst = dp->members,  i = 0;
         lst != NULL;
         lst = lst->next, i++)
        {if (i == imem)
            {desc = _PD_mk_descriptor(member, file->default_offset);
	     desc->member_offs = lst->member_offs;
             if (prev == NULL)
                dp->members = desc;
             else
                prev->next = desc;

             desc->next = lst->next;
             _PD_rl_descriptor(lst);

             return;};

         prev = lst;};

    SS_error(si, "HASK_HOOK FAILED - _SX_INS_MEM", SS_null);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_HASH_HOOK - write hook for pdblib to handle hash tables
 *               - PD_gs.write
 */

memdes *_SX_hash_hook(PDBfile *file, char *vr, defstr *dp)
   {char *type;
    int c;
    char new_mem[MAXLINE];
    memdes *md;
    SS_psides *si;

    si = SC_GET_CONTEXT(_SX_hash_hook);

    type = dp->type;

    if (strcmp(type, "object") == 0)
       {c = SC_arrtype(vr, 0);
        if (c == SC_INT_I)
	   SC_strncpy(new_mem, MAXLINE, "long_long *val", -1);

	else if (c == SC_FLOAT_I)
	   SC_strncpy(new_mem, MAXLINE, "double *val", -1);

	else if (c == SC_STRING_I)
	   SC_strncpy(new_mem, MAXLINE, "SS_string *val", -1);

	else if (c == G_SS_CONS_I)
	   SC_strncpy(new_mem, MAXLINE, "SS_cons *val", -1);

	else if (c == G_SS_VARIABLE_I)
	   SC_strncpy(new_mem, MAXLINE, "SS_variable *val", -1);

	else if ((c == SC_BOOL_I) ||
		 (c == SS_EOF_I) ||
		 (c == SS_NULL_I))
	   SC_strncpy(new_mem, MAXLINE, "SS_boolean *val", -1);

	else if (c == G_SS_VECTOR_I)
	   SC_strncpy(new_mem, MAXLINE, "SS_vector *val", -1);

	else
	   SC_strncpy(new_mem, MAXLINE, "unknown val", -1);

        _SX_ins_mem(si, dp, new_mem, 1, file);};

    md = dp->members;

    return(md);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_INIT_HASH_OBJECT - initialize pdb_write_hook for scheme hash objects
 *                      - add scheme types into a hash table.
 */

void _SX_init_hash_objects(SS_psides *si, PDBfile *file)
   {int err;

    if (file != NULL)

/* hash table types */
       {PD_def_attr_str(file);

/* PML types */
	PD_def_mapping(file);

/* PDBLib types */
	PD_def_pdb_types(file);

	err = TRUE;

	err &= (G_SS_EVAL_MODE_E(file) != NULL);
	err &= G_OBJECT_D(file);
	err &= G_SS_STRING_D(file);
	err &= G_SS_CONS_D(file);
	err &= G_SS_VARIABLE_D(file);
	err &= G_SS_BOOLEAN_D(file);
	err &= G_SS_VECTOR_D(file);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_FPRINTF - variable arg version of fPRINTf for the library
 *            - combines both PG_fprintf and SS_printf
 *            - the one for graphical printing options and the other
 *            - for transcript logging
 */

int SX_fprintf(FILE *fp, char *fmt, ...)
   {int rv;
    char *bf;
    FILE *hp;
    SS_psides *si;
    extern int _PG_display_page(PG_device *dev, FILE *fp, char *s);

    si = SC_GET_CONTEXT(SX_fprintf);

    rv = FALSE;

    if (fp != NULL)
       {SC_VDSNPRINTF(FALSE, bf, fmt);

	hp = SS_OUTSTREAM(si->histdev);
	if ((si->hist_flag != NO_LOG) && (fp != hp))
	   io_printf(hp, "%s", bf);

/* the ifs are nested to get the right behavior wrt the print_flag
 * LEAVE THEM ALONE !!!!
 */
	if ((PG_gs.console != NULL) && ((fp == stdout) || (fp == PG_gs.stdscr)))
	   rv = (PG_gs.console->gprint_flag) ?
	         _PG_display_page(PG_gs.console, fp, bf) :
		 FALSE;
	else
	   rv = io_puts(bf, fp);};

    SC_ASSERT(rv == TRUE);

    return(FALSE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_FPUTS - version of fputs for the library
 *          - for graphical printing options and
 *          - for transcript logging
 */

int SX_fputs(const char *s, FILE *fp)
   {int rv;
    FILE *hp;
    SS_psides *si;
    extern int _PG_display_page(PG_device *dev, FILE *fp, char *s);

    si = SC_GET_CONTEXT(SX_fputs);

    rv = FALSE;

    if (fp != NULL)
       {hp = SS_OUTSTREAM(si->histdev);
	if ((si->hist_flag != NO_LOG) && (fp != hp))
	   rv = SS_puts(s, hp, io_puts);

/* the ifs are nested to get the right behavior wrt the print_flag
 * LEAVE THEM ALONE !!!!
 */
	if ((PG_gs.console != NULL) && ((fp == stdout) || (fp == PG_gs.stdscr)))
	   {if (PG_gs.console->gprint_flag)
	       {if (SS_get_display_flag() == TRUE)
		   _PG_display_page(PG_gs.console, fp, (char *) s);
		else
		   {_PG_display_page(PG_gs.console, fp, "\"");
		    _PG_display_page(PG_gs.console, fp, (char *) s);
		    _PG_display_page(PG_gs.console, fp, "\"");};};}
	else
	   rv = SS_puts(s, fp, io_puts);}

    SC_ASSERT(rv == TRUE);

    return(FALSE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_PROC_CALLBACK - handle I/O callbacks to Scheme level functions */

static void _SS_proc_callback(int fd, int mask, void *a)
   {char *nm;
    PROCESS *pp;
    SX_iodes *pi;
    SS_psides *si;

    pi = _SX_callbacks + fd;
    si = SS_get_current_scheme(-1);

    if (SS_procedurep(pi->fnc))
       {nm = SS_PROCEDURE_NAME(pi->fnc);
	pp = pi->pp;

	SS_call_scheme(si, nm,
		       SS_PROCESS_I, pp,
		       0);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_PROCESS_IO_CALLBACK - setup I/O callbacks for poll */

void _SX_process_io_callback(PROCESS *pp, object *frd, object *fwr)
   {int fd;
    SX_iodes *pi;

    fd = pp->io[0];
    PG_loop_callback(SC_INTEGER_I, &fd,
		     _SS_proc_callback, NULL, pp->id);

    pi = _SX_callbacks + fd;
    pi->fd  = fd;
    pi->pp  = pp;
    pi->fnc = frd;

    fd = pp->io[1];
    PG_loop_callback(SC_INTEGER_I, &fd,
		     _SS_proc_callback, NULL, pp->id);

    pi = _SX_callbacks + fd;
    pi->fd  = fd;
    pi->pp  = pp;
    pi->fnc = fwr;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_PROCESS_REMOVE_CALLBACK - remove callbacks for poll */

void _SX_process_remove_callback(PROCESS *pp)
   {int fd;
    SX_iodes *pi;

    fd = pp->io[0];
    PG_remove_callback(&fd);

    pi = _SX_callbacks + fd;
    pi->fd  = 0;
    pi->pp  = NULL;
    pi->fnc = NULL;

    fd = pp->io[1];
    PG_remove_callback(&fd);

    pi = _SX_callbacks + fd;
    pi->fd  = 0;
    pi->pp  = NULL;
    pi->fnc = NULL;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

