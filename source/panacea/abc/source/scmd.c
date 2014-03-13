/*
 * SCMD.C - command processors for S
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "s.h"

hasharr
 *srctab;

PDBfile
 *pdsf;

double
 conversion_s;

char
 in_deck[MAXLINE],                                /* global input deck name */
 varname[MAXLINE];

static double
 *data;

static int
 count,
 var_sz;

/*--------------------------------------------------------------------------*/

/*                                 HANDLERS                                 */

/*--------------------------------------------------------------------------*/

/* SRC_ZARGS - handler for zero argument commands */

static void SRC_zargs(PA_command *cp)
   {

    (*(cp->proc))();

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SRC_STRARG - handler for single string argument commands */

static void SRC_strarg(PA_command *cp)
   {char *s;
    PFVString fnc;

    fnc = (PFVString) cp->proc;

    s = PA_get_field("FIELD", "SRC_STRARG", OPTL);
    (*fnc)(s);

    return;}

/*--------------------------------------------------------------------------*/

/*                              MEMORY MANAGEMENT                           */

/*--------------------------------------------------------------------------*/

/* NEXT_NAME - make a new variable name */

char *next_name(void)
   {static int ln = 0;
    char *s;

    s = CMAKE_N(char, 10);
    snprintf(s, 10, "var%d", ln++);

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MK_SOURCE_RECORD - make a new source_record and return a pointer to it */

static source_record *mk_source_record(double time, double *ldata)
   {source_record *sp;

    sp       = CMAKE(source_record);
    sp->time = time;
    sp->name = next_name();
    sp->data = ldata;

    return(sp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MK_TIME_LIST - make a time_list and return a pointer to it */

static time_list *mk_time_list(void)
   {time_list *tp;

    tp         = CMAKE(time_list);
    tp->list   = NULL;
    tp->length = 0;

    return(tp);}

/*--------------------------------------------------------------------------*/

/*                                    I/O                                   */

/*--------------------------------------------------------------------------*/

/* WRITE_VAR - write out source file variables */

static int write_var(PDBfile *lpdsf, char *name, double *tp, double **dp,
		     int sz)
   {int i, szd;
    long ind[3];
    double *vdata;
    char svname[MAXLINE], title[MAXLINE];
    static int lcount = 0;

    PA_gs.n_variables++;

    ind[0] = 0L;
    ind[2] = 1L;

/* write out the string header for this variable
 * header format :
 *           <var-name>|<num-times>|<time-array-name>
 */
    snprintf(title, MAXLINE, "src%d", lcount);
    snprintf(svname, MAXLINE, "%s|%d|st%d", name, sz, lcount);
    ind[1] = strlen(svname);
    PD_write_alt(lpdsf, title, "char", svname, 1, ind);

/* write out the data arrays one at a time */

    szd = SC_MEM_GET_N(double, *dp);
    for (i = 0; i < sz; i++)
        {snprintf(svname, MAXLINE, "%s:%d", title, i);
         vdata  = dp[i];
         ind[1] = szd - 1;
         PD_write_alt(lpdsf, svname, "double", vdata, 1, ind);};

/* now write out the time array */
    snprintf(title, MAXLINE, "st%d", lcount);
    ind[1] = sz - 1;
    PD_write_alt(lpdsf, title, "double", tp, 1, ind);

/* increment the variable counter */
    lcount++;

    return(TRUE);}

/*--------------------------------------------------------------------------*/

/*                               COMMANDS                                   */

/*--------------------------------------------------------------------------*/

/* MAKEH - write the variables out, write out the number of source
 *       - variables, and close the PDB source file
 */

static void makeh(void)
   {int i, j, l, sz, change, N_var;
    double t1, t2, *tt, **td, *pd, *time, **ldata;
    haelem **tab, *hp;
    source_record *sp;
    time_list *tp;
    pcons *pp, *pn;

/* order the times for each variable in srctab */
    sz    = srctab->size;
    tab   = srctab->table;
    N_var = 0;
    for (i = 0; i < sz; i++)
        for (hp = tab[i]; hp != NULL; hp = hp->next)
            {tp = (time_list *) (hp->def);
             l  = tp->length;

/* copy the list into arrays */
             tt = time = CMAKE_N(double, l);
             td = ldata = CMAKE_N(double *, l);
             for (pp = tp->list; pp != NULL; pp = pn)
                 {sp = (source_record *) pp->car;
                  *(tt++) = sp->time;
                  *(td++) = sp->data;
                  pn = (pcons *) pp->cdr;
                  CFREE(sp);
                  CFREE(pp);};

/* sort the arrays according to the times */
             change = TRUE;
             while (change)
                {change = FALSE;
                 for (j = 1; j < l; j++)
                     {t1 = time[j-1];
                      t2 = time[j];
                      if (t2 < t1)
                         {pd         = ldata[j-1];
                          ldata[j-1] = ldata[j];
                          ldata[j]   = pd;
                          time[j]    = t1;
                          time[j-1]  = t2;
                          change     = TRUE;};};};

/* write the variable and source_record array out */
             write_var(pdsf, hp->name, time, ldata, l);
             N_var++;

/* release the temporary storage for this variable */
             CFREE(time);
             CFREE(ldata);};

/* finish up */
    PD_write(pdsf, "n_variables", "integer", &N_var);
    PD_close(pdsf);
    pdsf = NULL;    

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RECORD_ENTRY - record a time entry in the list of times for the given
 *              - variable making an entry in srctab if needed
 *              - return the name under which the data will be installed
 *              - in the PDB source file
 */

static void record_entry(char *name, double time, double *ldata)
   {haelem *hp;
    pcons *fp;
    source_record *sp;
    time_list *tp;

    strcpy(varname, name);

/* lookup the variable and install it if this is the first time */
    hp = SC_hasharr_lookup(srctab, name);
    if (hp == NULL)
       {tp = mk_time_list();
        hp = SC_hasharr_install(srctab, name, tp, PA_gs.source, 3, -1);};

/* cons the new time onto the list of times */
    tp       = (time_list *) (hp->def);
    sp       = mk_source_record(time, ldata);
    fp       = SC_mk_pcons("source record *", (byte *) sp,
			   "time_list *", (byte *) (tp->list));
    tp->list = fp;
    (tp->length)++;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* VARH - handler for data base variable based commands
 *      - Format:
 *      -         <var> tmax size i1 ...
 */

static void varh(PA_variable *pp)
   {char *s, *pname, bf[MAXLINE];
    int first, size;
    double tmax;

    pname        = PA_VARIABLE_NAME(pp);
    conversion_s = 1.0/PA_VARIABLE_EXT_UNIT(pp);

/* get the information off the command line */
    tmax = PA_get_num_field("TMAX", "VARIABLE", REQU);
    size = PA_get_num_field("SIZE", "VARIABLE", REQU);

/* allocate storage for the data */
    data   = CMAKE_N(double, size);
    count  = 0;
    var_sz = size;

/* cat the variable name and the indexes together to make a unique label */
    first = TRUE;
    strcpy(bf, pname);
    while ((s = PA_get_field("INDEX", "VARIABLE", OPTL)) != NULL)
       {if (!first)
           SC_strcat(bf, MAXLINE, ",");
        else
           SC_strcat(bf, MAXLINE, "(");

        SC_strcat(bf, MAXLINE, s);
        first = FALSE;};

    if (!first)
       SC_strcat(bf, MAXLINE, ")");

/* record this entry */
    record_entry(bf, tmax, data);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SH - default handler for data values */

static void sh(char *u)
   {char *s;

/* save the first token which is passed in as an argument */
    count++;
    *(data++) = conversion_s*SC_stof(u);

/* read the remaining tokens from the input line */
    while ((s = PA_get_field("SPECIFICATION", "VALUE", OPTL)) != NULL)
       {count++;
        PA_ERR(!SC_numstrp(s), "BAD NUMERIC FIELD - %s", s);
        *(data++) = conversion_s*SC_stof(s);}

/* count should not be greater than the variable size */
    PA_ERR((count > var_sz),
           "TOO MANY VALUES FOR VARIABLE % s - %d vs %d",
           varname, count, var_sz);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READH - handle the read command for S */

void readh(char *str)
   {char *token, s[MAXLINE];
    PA_command *cp;
    haelem *hp;
    PA_variable *pp;
    FILE *fp;

    strcpy(in_deck, str);
    fp = fopen(str, "r");
    PA_ERR((fp == NULL), "COULDN'T OPEN FILE %s", str);

/* dispatch on commands from the deck */
    while (TRUE)
       {if (GETLN(s, MAXLINE, fp) == NULL)
           {fclose(fp);
            break;};

        if (SC_blankp(s, "c;#"))
           continue;

        token = SC_strtok(s, " \n\r\t/(", PA_gs.strtok_p);
        if (token != NULL)
           {hp = SC_hasharr_lookup(PA_gs.command_tab, token);
            if (hp != NULL)
               {conversion_s = 1.0;
                cp = (PA_command *) hp->def;
                (*(cp->handler))(cp);}

            else if ((hp = SC_hasharr_lookup(PA_gs.variable_tab, token)) != NULL)
               {pp = (PA_variable *) hp->def;
                varh(pp);}

            else if (SC_numstrp(token))
               sh(token);

            else
               PA_WARN(TRUE,"UNRECOGNIZED TOKEN %s IN FILE %s",
                       token, str);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLEAR_SYS - re-initialize the system in preparation for
 *           - a new source deck
 */

static void clear_sys(void)
   {

    if (pdsf != NULL)
       {PD_close(pdsf);
        pdsf = NULL;}

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DONEH - gracefully leave S */

static void doneh(void)
   {

    clear_sys();

    LONGJMP(SC_gs.cpu, ERR_FREE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* INST_S - install the commands for S */
 
void inst_s(void)
   {

    PA_gs.command = CSTRSAVE("command");

    PA_inst_c("end", NULL, FALSE, 0, doneh, SRC_zargs);
    PA_inst_c("clear", NULL, FALSE, 0, clear_sys, SRC_zargs);
    PA_inst_c("read", NULL, A_CHAR, 1, (PFVoid) readh, SRC_strarg);
    PA_inst_c("make", NULL, FALSE, 0, makeh, SRC_zargs);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* S_GEN - handle variable names as commands */

void S_gen(char *s)
   {PA_variable *pp;
    haelem *hp;

    hp = SC_hasharr_lookup(PA_gs.variable_tab, s);
    if (hp != NULL)
       {pp = (PA_variable *) hp->def;
        varh(pp);}

    else if (SC_numstrp(s))
       sh(s);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
