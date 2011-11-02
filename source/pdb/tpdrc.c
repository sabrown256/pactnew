/*
 * TPDRC.C - test recursive casts
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
#include "pdb.h"

typedef struct S_List List;
typedef struct S_BSTRUCT BSTRUCT;
typedef struct S_ASTRUCT ASTRUCT;

struct S_ASTRUCT
   {int val;};

struct S_BSTRUCT
   {float x;
    float y;};

struct S_List
   {char *type;
    void *data;
    struct S_List *next;};

static int
 debug_mode = FALSE,
 read_only  = FALSE;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HELP - print a help message */

void print_help(void)
   {

    PRINT(STDOUT, "\nTPDRC - run PDB recursive cast tests\n\n");
    PRINT(STDOUT, "Usage: tpdrc [-d] [-h] [-r] [-v #]\n");
    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "       d - turn on debug mode to display memory maps\n");
    PRINT(STDOUT, "       h - print this help message and exit\n");
    PRINT(STDOUT, "       r - read only mode (assuming file from previous run)\n");
    PRINT(STDOUT, "       v - use specified format version (default 2)\n");
    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int c, char **v)
   {int i;
    char *fname, *aname, *bname;
    PDBfile *file; 
    defstr *dp;
    List *head, *ptr;
    ASTRUCT *aptr;
    BSTRUCT *bptr;
  
    fname     = NULL;
    read_only = FALSE;
    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'd' :
		      debug_mode  = TRUE;
		      SC_gs.mm_debug = TRUE;
		      break;
                 case 'h' :
		      print_help();
		      return(1);
                 case 'r' :
		      read_only = TRUE;
                      break;
                 case 'v' :
                      PD_set_fmt_version(SC_stoi(v[++i]));
		      break;};}
         else
	    fname = v[i];};

    file = PD_open(fname, "w"); 

    dp = PD_defstr(file,
		   "List",
		   "char *type",
		   "char *data", 
		   "List *next",
		   LAST);
    if (dp == NULL)
       PRINT(stdout, "PD_defstr List: %s\n", PD_get_error());
  
    dp = PD_defstr(file,
		   "ASTRUCT",
		   "int val",
		   LAST);
    if (dp == NULL)
       PRINT(stdout, "PD_defstr A: %s\n", PD_get_error());
    
    dp = PD_defstr(file,
		   "BSTRUCT",
		   "float x",
		   "float y",
		   LAST);
    if (dp == NULL)
       PRINT(stdout, "PD_defstr B: %s\n", PD_get_error());
  
    if (PD_cast(file, "List", "data", "type") == 0)
       PRINT(stdout, "PD_cast: %s\n", PD_get_error());

    head = CMAKE(List);
    ptr  = head;

    aname = CSTRSAVE("ASTRUCT *");
    bname = CSTRSAVE("BSTRUCT *");

    for (i = 0; i < 10; ++i)
        {if (i % 2 == 0)
	    {aptr      = CMAKE(ASTRUCT);
	     ptr->data = aptr;
	     ptr->type = aname;
	     aptr->val = i;}

	 else
	    {bptr      = CMAKE(BSTRUCT);
	     ptr->data = bptr;
	     ptr->type = bname;
	     bptr->x   = 1.2*i;
	     bptr->y   = 1.1*i;};
     
	 if (i != 9)
	    {ptr->next = CMAKE(List);
	     ptr       = ptr->next;}
	 else
	    ptr->next = NULL;};
    
    if (PD_write(file, "mylist", "List", head) == 0)
       PRINT(stdout, "PD_write: %s\n", PD_get_error());

    PD_close(file);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

