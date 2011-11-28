/*
 * PDSYMT.C - routines for delayed symbol table reading
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_ADD_ENTRYP - return TRUE iff ACC and REJ determine that
 *                - entry NAME should be added to the symbol table
 */

int _PD_add_entryp(char *name, char *acc, char *rej)
   {int ok;

    ok = ((rej == NULL) || (SC_regx_match(name, rej) == FALSE));
    if (ok == FALSE)
       ok = ((acc != NULL) && (SC_regx_match(name, acc) == TRUE));

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_SYMT_DELAY_RULES - set the standard accept and reject rules for
 *                      - the various delay modes
 *                      - return TRUE iff any rules are set
 *                      - if WHEN is
 *                      -    0 set rules for opening file
 *                      -    1 set rules for changing directory
 */

int _PD_symt_delay_rules(PDBfile *file, int when, char **pa, char **pr)
   {int rv;
    char *acc, *rej;

    rv = FALSE;

    acc = NULL;
    rej = NULL;

    switch (when)

/* for open file */
       {case 0 :
	     if (file->delay_sym == PD_DELAY_PTRS)
	        {rej = SC_dsnprintf(TRUE, "%s*#/*", file->ptr_base);
		 rv  = TRUE;}
	     else if (file->delay_sym == PD_DELAY_ALL)
	        {rej = SC_dsnprintf(TRUE, "*");
		 acc = SC_dsnprintf(TRUE, "*/");
		 rv  = TRUE;};
	     break;

/* for change directory */
	case 1 :
	     if (file->delay_sym == PD_DELAY_PTRS)
	        {acc = SC_dsnprintf(TRUE, "%s*#%s",
				    file->ptr_base, file->current_prefix);
		 rv  = TRUE;}
	     else if (file->delay_sym == PD_DELAY_ALL)
	        {acc = SC_dsnprintf(TRUE, "*%s*", file->current_prefix);
		 rv  = TRUE;};
	     break;};

    if (pa != NULL)
       *pa = acc;
    else
       CFREE(acc);

    if (pr != NULL)
       *pr = rej;
    else
       CFREE(rej);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_SYMT_SET_DELAY_MODE - set the file delay mode
 *                         - return the old value
 */

PD_delay_mode _PD_symt_set_delay_mode(PDBfile *file, char *mode)
   {PD_delay_mode ov;

    ov = PD_DELAY_NONE;

    if (file != NULL)
       {ov = file->delay_sym;

/* if file mode is "rp" use delay pointer mode */
	if (strchr(mode, 'p') != NULL)
	   file->delay_sym = PD_DELAY_PTRS;

/* if file mode is "rs" use delay all mode */
	else if (strchr(mode, 's') != NULL)
	   file->delay_sym = PD_DELAY_ALL;};

    return(ov);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PARE_SYMT - remove symbol table entries added in the last cd
 *               - return the number of entries removed
 */

int _PD_pare_symt(PDBfile *file)
   {int i, nr;
    char s[MAXLINE];
    char **ent;

    nr = 0;

    if ((file->delay_sym != PD_DELAY_NONE) &&
	(strcmp(file->current_prefix, "/") != 0))
       {snprintf(s, MAXLINE, "*%s*", file->current_prefix);
	ent = SC_hasharr_dump(file->symtab, s, NULL, FALSE);

	for (i = 0; ent[i] != NULL; i++)
	    {if (SC_LAST_CHAR(ent[i]) != '/')
	        {SC_hasharr_remove(file->symtab, ent[i]);
	         nr++;};};

        SC_free_strings(ent);};

    return(nr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RD_SYMT - read entries from the symbol table
 *             - entries are checked first against REJ
 *             - if they match they are not entered in the symbol table
 *             - if not rejected they are checked against ACC
 *             - if they match they are entered in the symbol table
 *             - if REJ is NULL everything goes to the ACC filter
 *             - if ACC is NULL everything passing the REJ filter is accepted
 *             - regular expressions are interpreted as follows:
 *             -   <base>*#/a/b* matches all of:
 *             -      <base>*#/a/b
 *             -      <base>*#/a/b/c
 *             -      <base>*#/a/b/c/
 *             -      <base>*#/a/b/c/d
 *             - where <base> is the file->ptr_base string
 *             - return TRUE iff successful
 */

int _PD_rd_symt(PDBfile *file, char *acc, char *rej)
   {int rv;

    rv = (*file->rd_symt)(file, acc, rej);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
