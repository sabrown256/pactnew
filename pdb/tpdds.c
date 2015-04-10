/*
 * TPDDS.C - test PDB delayed symbol machinery
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include <cpyright.h>
#include "pdb.h"

static int
 is = 1,
 ia[2] = {10, 11},
 *ip = NULL;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int var_check(PDBfile *pf)
   {int i, n, rv;
    syment *ep;
    static char *names[] = { "/is", "/ia", "/ip",
			     "/ints/is", "/ints/ia", "/ints/ip",
			     "/ints/d/is", "/ints/d/ia", "/ints/d/ip",
			     "/floats/is", "/floats/ia", "/floats/ip",
			     "/floats/d/is", "/floats/d/ia", "/floats/d/ip" };

    rv = TRUE;

    n = sizeof(names)/sizeof(char *);

/* known */
    printf("Known:\n");
    for (i = 0; i < n; i++)
        {ep = PD_inquire_entry(pf, names[i], TRUE, NULL);
	 if (ep != NULL)
            printf("   %s\n", names[i]);};
    printf("\n");

/* unknown */
    printf("Unknown:\n");
    for (i = 0; i < n; i++)
        {ep = PD_inquire_entry(pf, names[i], TRUE, NULL);
	 if (ep == NULL)
            printf("   %s\n", names[i]);};
    printf("\n");

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static void show_vars(PDBfile *pf, char *dir)
   {int i, ns;
    char **sa;

    PD_cd(pf, dir);

    sa = PD_ls(pf, "/", NULL, &ns);

    printf("Variables:\n");
    for (i = 0; i < ns; i++)
        printf("   %s\n", sa[i]);
    printf("\n");

    SC_free_strings(sa);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int check_file(char *file, char *mode)
   {int cs, rv;
    char msg[MAXLINE];
    PDBfile *pf;

    rv = TRUE;
    cs = SC_mem_monitor(-1, 2, "Mode", msg);

    pf = PD_open(file, mode);
    PD_set_track_pointers(pf, FALSE);

/* these should not work with delayed symbols */
    printf("---------------------------------------\n");
    printf("---------------------------------------\n");
    printf("   / directory, delay %s mode\n", mode);
    printf("\n");

    show_vars(pf, "/");
    var_check(pf);
    printf("\n");

/* these should work in all cases */
    printf("---------------------------------------\n");
    printf("   /ints directory, delay %s mode\n", mode);
    printf("\n");

    show_vars(pf, "/ints");
    var_check(pf);
    printf("\n");

    PD_print_entry(pf, "/ints/is", &is, NULL);
    PD_print_entry(pf, "/ints/ia", ia, NULL);
    PD_print_entry(pf, "/ints/ip", &ip, NULL);
    printf("\n");

/* these should work in all cases */
    printf("---------------------------------------\n");
    printf("   /floats directory, delay %s mode\n", mode);
    printf("\n");

    show_vars(pf, "/floats");
    printf("\n");

    var_check(pf);
    printf("\n");

    PD_print_entry(pf, "/floats/is", &is, NULL);
    PD_print_entry(pf, "/floats/ia", ia, NULL);
    PD_print_entry(pf, "/floats/ip", &ip, NULL);
    printf("\n");

    PD_cd(pf, "/");
    PD_close(pf);

    cs = SC_mem_monitor(cs, 2, "Mode", msg);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAKE_DATA -  create data file */

static int make_data(void)
   {int i;
    PDBfile *pf;

    pf = PD_open("pdptr.pdb", "w");
    PD_set_track_pointers(pf, FALSE);

    is = 1;
    ip = CMAKE_N(int, 4);
    for (i = 0; i < 4; i++)
        ip[i] = i+1;

    PD_write(pf, "is", "int", &is);
    PD_write(pf, "ia[2]", "int", ia);
    PD_write(pf, "ip", "int *", &ip);

    PD_mkdir(pf, "/ints");
    PD_cd(pf, "/ints");

    PD_write(pf, "is", "int", &is);
    PD_write(pf, "ia[2]", "int", ia);
    PD_write(pf, "ip", "int *", &ip);

    PD_mkdir(pf, "d");
    PD_cd(pf, "d");

    PD_write(pf, "is", "int", &is);
    PD_write(pf, "ia[2]", "int", ia);
    PD_write(pf, "ip", "int *", &ip);

    PD_cd(pf, "/");

    PD_mkdir(pf, "/floats");
    PD_cd(pf, "/floats");

    PD_write(pf, "is", "int", &is);
    PD_write(pf, "ia[2]", "int", ia);
    PD_write(pf, "ip", "int *", &ip);

    PD_mkdir(pf, "d");
    PD_cd(pf, "d");

    PD_write(pf, "is", "int", &is);
    PD_write(pf, "ia[2]", "int", ia);
    PD_write(pf, "ip", "int *", &ip);

    PD_cd(pf, "/");

    PD_close(pf);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int c, char **v)
   {int rv;

    make_data();

    rv  = TRUE;
    rv &= check_file("pdptr.pdb", "rsa");
    rv &= check_file("pdptr.pdb", "rsc");
    rv &= check_file("pdptr.pdb", "rsd");
    rv &= check_file("pdptr.pdb", "rp");

    CFREE(ip);

    rv = (rv != TRUE);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

