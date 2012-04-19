/*
 * PDBTFR.H - test framework routines for PDB
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PDB_TEST_FRAME
#define PDB_TEST_FRAME

#include "cpyright.h"

#include "pdb.h"

#define BALLOC  0
#define BFREE   1

typedef struct tframe_s tframe;

struct tframe_s
   {int dbg;
    int cs;
    int64_t memb[2];  /* mem stats before - alloc and free */
    int64_t mema[2];  /* mem stats after - alloc and free */
    char msg[MAXLINE];
    double time;};

int
 read_only = FALSE;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_TARGET - set up the target for the data file */

static void test_target(char *tgt, char *base, int n,
		        char *fname, char *datfile)
   {int rv;

    if (tgt != NULL)
       {rv = PD_target_platform(tgt);
	SC_ASSERT(rv == TRUE);

        sprintf(fname, "%s-%s.rs%d", base, tgt, n);
        sprintf(datfile, "%s-%s.db%d", base, tgt, n);}

    else
       {sprintf(fname, "%s-nat.rs%d", base, n);
        sprintf(datfile, "%s-nat.db%d", base, n);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DUMP_TEST_SYMBOL_TABLE - dump the symbol table */

void dump_test_symbol_table(FILE *fp, hasharr *tab, int n)
   {int i, ne;
    char **names;

    PRINT(fp, "\nTest %d Symbol table:\n", n);

    ne    = SC_hasharr_get_n(tab);
    names = SC_hasharr_dump(tab, NULL, NULL, FALSE);
    for (i = 0; i < ne; i++)
        PRINT(fp, "%s\n", names[i]);
    SC_free_strings(names);

    PRINT(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ERROR - get out on an error */

void error(int n, FILE *fp, char *fmt, ...)
   {char t[MAXLINE];
        
    SC_VA_START(fmt);
    SC_VSNPRINTF(t, MAXLINE, fmt);
    SC_VA_END;

    io_printf(fp, "%s", t);

    exit(n);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRE_TEST - setup memory and timing info */

static void pre_test(tframe *tf, int debug)
   {

    if (debug)
       tf->dbg = 2;
    else
       tf->dbg = 0;

/* NOTE: under the debugger set tf->dbg to 1 or 2 for additional
 *       memory leak monitoring
 */
    tf->cs = SC_mem_monitor(-1, tf->dbg, "B", tf->msg);

    SC_mem_stats(&tf->memb[BALLOC], &tf->memb[BFREE], NULL, NULL);

    tf->time = SC_wall_clock_time();

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_HEADER - print column headings */

static void test_header(void)
   {

    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "\t\t                      Memory                   Time\n");
    PRINT(STDOUT, "\t\t                     (bytes)                  (secs)\n");
    PRINT(STDOUT, "\t\t     Test   Allocated      Freed      Diff\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* POST_TEST - report memory and timing */

static void post_test(tframe *tf, int n)
   {long da, df, dd;
    double dt;

    dt = SC_wall_clock_time() - tf->time;

    SC_mem_stats(&tf->mema[BALLOC], &tf->mema[BFREE], NULL, NULL);

    da = tf->mema[BALLOC] - tf->memb[BALLOC];
    df = tf->mema[BFREE]  - tf->memb[BFREE];
    dd = da - df;

    tf->cs = SC_mem_monitor(tf->cs, tf->dbg, "B", tf->msg);

    PRINT(STDOUT,
          "\t\t     %3d    %9ld  %9ld   %7ld     %.2g\n",
	  n, da, df, dd, dt);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif
