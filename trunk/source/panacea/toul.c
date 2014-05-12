/*
 * TOUL.C - transpose time history files to ULTRA files
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "panacea_int.h"

#undef CODE
#define CODE "TOUL"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HELP - print a help message */

static void print_help(void)
   {

    PRINT(stdout, "\nTOUL - Transpose time history files into ULTRA files.\n");
    PRINT(stdout, "\n");

    PRINT(stdout, "Usage: toul [-h] | [<execution-options>] <file-specification>\n");
    PRINT(stdout, "\n");

    PRINT(stdout, "       execution-options:  [-d] [-m] [-n <ncpf>] [-o] [-v] [-x]\n");
    PRINT(stdout, "       file-specification: -f <files> | -l <files> | [-s] <base>\n");
    PRINT(stdout, "\n");

    PRINT(stdout, "       -d  Turn on debug mode for memory info\n");

    PRINT(stdout, "       -f  Process explicitly specified files only\n");

    PRINT(stdout, "       -h  Print this help message and exit\n");

    PRINT(stdout, "       -l  Process all files linked to specified TH files\n");

    PRINT(stdout, "       -m  Merge transposed files or specified ULTRA files\n");

    PRINT(stdout, "       -n  Maximum number of curves per output file\n");

    PRINT(stdout, "       -o  Process TH files in reverse order\n");

    PRINT(stdout, "       -s  Process family of files\n");

    PRINT(stdout, "       -v  Print banner\n");

    PRINT(stdout, "       -x  Transpose TH files\n");

    PRINT(stdout, "\n");
    PRINT(stdout, "If neither -m nor -x option specifed, do transpose.\n");
    PRINT(stdout, "If transpose and merge, merge transposed files only.\n");
    PRINT(stdout, "File names must follow convention <base>.tdd or <base>.udd.\n");
    PRINT(stdout, "Files produced by merging are named m<base>.udd.\n");
    PRINT(stdout, "Each ULTRA file to be merged must contain a single domain.\n");
    PRINT(stdout, "Use the ULTRA save command for greater flexibility.\n");
    PRINT(stdout, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - read the restart dump and run the physics loop */

int main(int argc, char **argv)
   {char s[MAXLINE], t[MAXLINE], *root, **names;
    int i, ret, nn, banner, first_indx, order, ncpf;
    int do_xpose, do_merge, use_names, use_links, use_family;

    if (argc < 2)
       {print_help();
        return(1);};

    banner            = FALSE;
    first_indx        = 0;
    ncpf              = 0;
    order             = 1;
    do_xpose          = FALSE;
    do_merge          = FALSE;
    use_names         = FALSE;
    use_links         = FALSE;
    use_family        = FALSE;
    _PA.ul_print_flag = TRUE;

    for (i = 1; i < argc; i++)
        {if (argv[i][0] == '-')
            {switch (argv[i][1])
                {case 'd' : SC_gs.mm_debug = TRUE;
                            break;
                 case 'f' : use_names  = TRUE;
		            first_indx = i + 1;
                            break;
                 case 'h' : print_help();
                            return(1);
                 case 'l' : use_links = TRUE;
		            first_indx = i + 1;
                            break;
                 case 'm' : do_merge = TRUE;
                            break;
                 case 'n' : ncpf = atoi(argv[++i]);
                            break;
                 case 'o' : order = -1;
                            break;
		 case 's' : use_family = TRUE;
		            first_indx = i + 1;
		            break;
                 case 'v' : banner = TRUE;
		            break;
                 case 'x' : do_xpose = TRUE;
		            break;};}
        else
	    {if (first_indx == 0)
	        {use_family = TRUE;
		 first_indx = i;};
	     break;};};

    if ((!do_xpose) && (!do_merge))
       do_xpose = TRUE;

    SC_init("TOUL: Exit with error", NULL,
            TRUE, NULL, NULL, 0,
            TRUE, NULL, 0);

/* print the banner */
    if (banner)
       {SC_set_banner(" %s  -  %s\n\n", CODE, VERSION);
	SC_banner("");};

    if ((use_names + use_links + use_family) > 1)
       {PRINT(stdout, " Error - Multilple file-specifications.\n\n");
	return(1);};

    if (do_merge && (!do_xpose) && use_links)
       {PRINT(stdout, " Error - Cannot merge by internal links.\n\n");
	return(1);};
       
    names = argv + first_indx;
    nn    = argc - first_indx;

    if ((nn <= 0) || (first_indx == 0))
       {PRINT(stdout, " Error - Missing or illegal file-specification.\n\n");
	return(1);};

    if (do_xpose)
       {ret = 0;
	if (use_names)
	   ret = PA_th_trans_name(nn, names, order, ncpf);
        else if (use_links)
	   ret = PA_th_trans_link(nn, names, order, ncpf);
	else if (use_family)
	   ret = PA_th_trans_family(names[0], order, ncpf);

	if (ret == 0)
	   {PRINT(stdout, "%s\nNo transposable files found\n", PA_gs.err);
	    return(1);};}

    if (do_merge)
       {if (use_family)
	   root = names[0];
	else
	   {char *ls;

	    strcpy(t, names[0]);
	    root = SC_strtok(t, ".", ls);};

        snprintf(s, MAXLINE, "m%s", root);

	if (do_xpose)
	   ret = PA_merge_family(s, root, ncpf);
	else if (use_names)
	   ret = PA_merge_files(s, nn, names, ncpf);
	else if (use_family)
	   ret = PA_merge_family(s, root, ncpf);
	else
	   ret = 0;

	if (ret == 0)
	   {PRINT(stdout, "%s\nNo mergeable files found\n", PA_gs.err);
	    return(1);};}

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

