/*
 * SXHOOK.C - write hook for SX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "sx_int.h"

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
	   strcpy(new_mem, "long_long *val");

	else if (c == SC_FLOAT_I)
	   strcpy(new_mem, "double *val");

	else if (c == SC_STRING_I)
	   strcpy(new_mem, "string *val");

	else if (c == SS_CONS_I)
	   strcpy(new_mem, "cons *val");

	else if (c == SS_VARIABLE_I)
	   strcpy(new_mem, "variable *val");

	else if ((c == SC_BOOL_I) ||
		 (c == SS_EOF_I) ||
		 (c == SS_NULL_I))
	   strcpy(new_mem, "boolean *val");

	else if (c == SS_VECTOR_I)
	   strcpy(new_mem, "vector *val");

	else
	   strcpy(new_mem, "unknown val");

        _SX_ins_mem(si, dp, new_mem, 1, file);};

    md = dp->members;

    return(md);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_INIT_HASH_OBJECT - initialize pdb_write_hook for scheme hash objects
 *                      - add scheme types into a hash table.
 */

void _SX_init_hash_objects(SS_psides *si, PDBfile *file)
   {

    if (file != NULL)

/* hash table types */
       {PD_def_attr_str(file);

	SS_DEFINE_OBJECT(si);

/* PML types */
	PD_def_mapping(file);

/* SCHEME object types */
	SS_DEFINE_STRING;
	SS_DEFINE_CONS;
	SS_DEFINE_VARIABLE;
	SS_DEFINE_BOOLEAN;
	SS_DEFINE_VECTOR;

/* PDBLib types */
	PD_def_pdb_types(file);};

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
	if ((PG_console_device != NULL) && ((fp == stdout) || (fp == stdscr)))
	   rv = (PG_console_device->gprint_flag) ?
	         _PG_display_page(PG_console_device, fp, bf) :
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
	if ((PG_console_device != NULL) && ((fp == stdout) || (fp == stdscr)))
	   {if (PG_console_device->gprint_flag)
	       {if (SS_get_display_flag() == TRUE)
		   _PG_display_page(PG_console_device, fp, (char *) s);
		else
		   {_PG_display_page(PG_console_device, fp, "\"");
		    _PG_display_page(PG_console_device, fp, (char *) s);
		    _PG_display_page(PG_console_device, fp, "\"");};};}
	else
	   rv = SS_puts(s, fp, io_puts);}

    SC_ASSERT(rv == TRUE);

    return(FALSE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

