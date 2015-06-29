/*
 * MAKE-DEF.C - write the MAKE-DEF file
 *
 * include "cpyright.h"
 *
 */

#include "libdb.c"
#include "libpsh.c"

typedef struct s_tgdes tgdes;

struct s_tgdes
   {char *name;
    char **opt;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINTRULE - do the required rule printing with \n and \t handled
 *           - properly
 */

void printrule(FILE *fp, client *cl, char *rule, char *var)
   {

    fprintf(fp, "%s\n", rule);

    fprintf(fp, "	@[ ! -f ${LibDir}/.lock ] || { echo \"${LibDir} is locked\" ; exit 1 ; }\n");

    fprintf(fp, "\t%s\n\n", dbget(cl, FALSE, var));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start here */

int main(int c, char **v)
   {int i, rv;
    int ig, ng, it, use_acc;
    char r[BFLRG],  t[BFLRG], md[BFLRG], cc_inc[BFLRG];
    char *psy_bldobj, *psy_bldtst;
    char *psy_root, *psy_base, *psy_build, *psy_cfg, *psy_load, *psy_inst;
    char *psy_publib;
    char *hsy_os_name, *hsy_os_type, *lostd;
    char *make_strategy, *make_usegnu, *dev;
    char *fc_ld_lib, *dp_inc, *have_py, *yacc_exe;
    char *scsd, *npath, *lrpath;
    char *mod, *mk, *tnm, *date, *osrc;
    char *g, *vr, *vl, **sa, **SynSCM;
    client *cl;
    FILE *fp, *wlog;
    char *grps[] = {"Std", "Cfg", "Cfe", "Ser", "Shared"};
    char *tool_cc[] = {"Exe", "Flags", "Debug", "Optimize",
		       "Shared", "Inc", NULL};
    char *tool_fc[] = {"Exe", "Flags", "Debug", "Optimize", "Shared", NULL};
    char *tool_ld[] = {"Exe", "Flags", "Shared", "Lib", NULL};
    char *tool_ar[] = {"Exe", "Flags", "IFlag", "XFlag", NULL};
    char *tool_lx[] = {"Exe", "Flags", NULL};
    char *tool_rl[] = {"Exe", NULL};
    tgdes tools[] = {{"CC", tool_cc},
		     {"FC", tool_fc},
		     {"LD", tool_ld},
		     {"Ar", tool_ar},
		     {"Lex", tool_lx},
		     {"Ranlib", tool_rl}};

    r[0] = '\0';

/* everything up to the first token not beginning with '-' is a flag */
    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
	    {switch (v[i][1])
                {case 'f' :
                      nstrncpy(r, BFLRG, v[++i], -1);
		      break;
		 case 'h' :
                      printf("Usage: make-def [-h]\n");
                      return(1);
		 case 'l' :
                      svs.debug = TRUE;
                      break;};}
	 else
	    break;};

    rv = TRUE;

    push_path(P_PREPEND, lpath, "/sbin");
    push_path(P_PREPEND, lpath, "/bin");
    push_path(P_PREPEND, lpath, "/usr/sbin");
    push_path(P_PREPEND, lpath, "/usr/bin");
    push_path(P_PREPEND, lpath, "/usr/local/bin");
    push_path(P_PREPEND, lpath, "../scripts");

/* locate the tools needed for subshells */
    build_path(NULL,
	       "ls", "cat", "make", "perdb", "var-list",
	       NULL);

    if (IS_NULL(r) == TRUE)
       {if (cdefenv("PERDB_PATH") == TRUE)
           nstrncpy(r, BFLRG, cgetenv(TRUE, "PERDB_PATH"), -1);};

    if (IS_NULL(r) == TRUE)
       snprintf(r, BFLRG, "%s/.perdb", cgetenv(TRUE, "HOME"));

    csetenv("PERDB_PATH", r);

    cl = make_client(CLIENT, DB_PORT, FALSE, r, cl_logger, NULL);

    csetenv("CROSS_COMPILE", dbget(cl, FALSE, "CROSS_COMPILE"));

    psy_root   = dbget(cl, TRUE, "PSY_Root");
    psy_base   = dbget(cl, TRUE, "PSY_Base");
    psy_build  = dbget(cl, TRUE, "PSY_Build");
    psy_bldobj = dbget(cl, TRUE, "PSY_BldObj");
    psy_bldtst = dbget(cl, TRUE, "PSY_BldTst");
    psy_inst   = dbget(cl, TRUE, "PSY_InstRoot");
    psy_cfg    = dbget(cl, TRUE, "PSY_Cfg");
    psy_load   = dbget(cl, TRUE, "PSY_Load");

    hsy_os_name = dbget(cl, TRUE, "HSY_OS_Name");
    hsy_os_type = dbget(cl, TRUE, "HSY_OS_Type");
    if (strcmp(hsy_os_type, "USE_MSW") == 0)
       lostd = "WIN32";
    else
       lostd = "UNIX";

    nstrncpy(t, BFLRG, dbget(cl, TRUE, "USE_ACC"), -1);
    use_acc = (strcmp(t, "TRUE") == 0);
    use_acc = TRUE;

    make_strategy = dbget(cl, TRUE, "MAKE_Strategy");
    if (strcmp(make_strategy, "SpaceSaving") == 0)
       osrc = "Ar";
    else
       osrc = "O";

/* add DP_Inc to CC_Inc */
    dp_inc = dbget(cl, TRUE, "DP_Inc");
    snprintf(cc_inc, BFLRG, "%s %s", dbget(cl, TRUE, "CC_Inc"), dp_inc);

    snprintf(md, BFLRG, "%s/etc/make-def", psy_root);
    fp   = fopen_safe(md, "w");
    snprintf(md, BFLRG, "%s/log/write.make-def", psy_root);
    wlog = fopen_safe(md, "a");

    fprintf(wlog, "----- write/make-def -----\n");
    fprintf(wlog, "Write: make-def\n");
    fprintf(wlog, "\n");

    printf("   Site Dependent MAKE Configuration - make-def\n");
    fprintf(wlog, "   Site Dependent MAKE Configuration - make-def\n");
    fprintf(wlog, "\n");

    fprintf(fp, "#\n");
    fprintf(fp, "# MAKE-DEF - installation dependent definitions for %s\n",
            path_tail(psy_cfg));
    fprintf(fp, "#\n");
    fprintf(fp, "\n");
    fprintf(fp, "PSY_ID     = %s\n", dbget(cl, TRUE, "PSY_ID"));
    fprintf(fp, "PSY_CfgDir = %s\n", dbget(cl, TRUE, "PSY_CfgDir"));
    fprintf(fp, "PSY_TmpDir = %s\n", dbget(cl, TRUE, "PSY_TmpDir"));
    fprintf(fp, "\n");

    date = run(FALSE, "cat .pact-version");
    fprintf(fp, "Version    = %s\n", date);

    mk = cwhich("gmake");
    if (!file_executable(mk))
       mk = cwhich("make");
    fprintf(wlog, "Using '%s' for system make\n", mk);
    fprintf(fp, "UMake      = %s\n", mk);

    fprintf(fp, "tmpobj     = ${PSY_TmpDir}/obj\n");
    fprintf(fp, "PSY_Base   = %s\n", psy_base);
    fprintf(fp, "PSY_ScrDir = %s/scripts\n", psy_base);
    fprintf(fp, "PSY_Root   = %s\n", psy_root);
    fprintf(fp, "PSY_Build  = %s\n", psy_build);
    fprintf(fp, "PSY_BldObj = %s\n", psy_bldobj);
    fprintf(fp, "PSY_BldTst = %s\n", psy_bldtst);
    fprintf(fp, "PkgSource  = %s/${Package}\n", psy_base);
    fprintf(fp, "PkgBuild   = %s/${Package}/%s\n", psy_base, psy_bldobj);
    fprintf(fp, "PkgTest    = %s/${Package}/%s\n", psy_base, psy_bldtst);
    fprintf(fp, "ScmDir     = ${PSY_Root}/scheme\n");
    fprintf(fp, "BinDir     = ${PSY_Root}/bin\n");
    fprintf(fp, "LibDir     = ${PSY_Root}/lib\n");
    fprintf(fp, "IncDir     = ${PSY_Root}/include\n");
    fprintf(fp, "EtcDir     = ${PSY_Root}/etc\n");
    fprintf(fp, "Man1Dir    = ${PSY_Root}/man/man1\n");
    fprintf(fp, "Man3Dir    = ${PSY_Root}/man/man3\n");
    fprintf(fp, "\n");

    if (strcmp(hsy_os_name, "HP-UX") == 0)
       fprintf(fp, "TGTLib  = nolib\n");

    fprintf(fp, "TGTBin  = nobin\n");
    fprintf(fp, "TGTEtc  = noetc\n");
    fprintf(fp, "TGTInc  = noinc\n");
    fprintf(fp, "TGTScm  = noscm\n");
    fprintf(fp, "TGTScr  = noscr\n");
    fprintf(fp, "TGTScmX = noscmx\n");
    fprintf(fp, "TGTScrX = noscrx\n");
    fprintf(fp, "TGTMan3 = noman\n");
    fprintf(fp, "LibName = noname\n");
    fprintf(fp, "\n");

/* Compiler info */
    vl = run(FALSE, "../scripts/gcc-info %s", dbget(cl, TRUE, "CC_Exe"));
    sa = tokenize(vl, " \n", 0);
    if (strcmp(sa[1], "yes") == 0)
       {fprintf(fp, "#\n");
	fprintf(fp, "# GCC Info\n");
	fprintf(fp, "#\n");
	fprintf(fp, "GCCLib = %s\n", sa[7]);
	fprintf(fp, "\n");};
    lst_free(sa);

/* OS support */
    fprintf(fp, "#\n");
    fprintf(fp, "# Site Dependent OS Support\n");
    fprintf(fp, "#\n");
    if (strcmp(lostd, "UNIX") == 0)
       {fprintf(fp, "OSSrcs = ${UnxSrcs}\n");
        fprintf(fp, "OSObjs = ${%sUnxObjs}\n", osrc);}

/* cannot ever get here but it shows how the abstraction works */
    else if (strcmp(lostd, "WIN32") == 0)
       {fprintf(fp, "OSSrcs = ${W32Srcs}\n");
	fprintf(fp, "OSObjs = ${%sW32Objs}\n", osrc);}

    else
       {fprintf(fp, "OSSrcs =");
        fprintf(fp, "OSObjs =");};

    fprintf(fp, "\n");

    fprintf(fp, "ExcludeType =\n");
    fprintf(fp, "\n");

/* graphics devices */
    fprintf(fp, "#\n");
    fprintf(fp, "# Site Dependent Graphics Information\n");
    fprintf(fp, "#\n");
    fprintf(fp, "GRAPHICS_Flags   = %s\n", dbget(cl, FALSE, "GRAPHICS_Flags"));
    fprintf(fp, "MDGInc           = %s\n", dbget(cl, FALSE, "MDG_Inc"));
    fprintf(fp, "MDGLib           = %s\n", dbget(cl, FALSE, "MDG_Lib"));

    r[0] = '\0';
    t[0] = '\0';
    sa = tokenize(dbget(cl, FALSE, "GRAPHICS_Devices"), " ", 0);
    for (i = 0; sa[i] != NULL; i++)
        {dev = sa[i];
	 vstrcat(t, BFLRG, " ${S%s}", dev);
	 vstrcat(r, BFLRG, " ${%s%s}", osrc, dev);};
    lst_free(sa);
    fprintf(fp, "GRDevicesS = %s\n", t);
    fprintf(fp, "GRDevicesO = %s\n", r);

    fprintf(fp, "\n");

    fprintf(fp, "\n");
    fprintf(fp, "GRAPHICS_Windows=%s\n", dbget(cl, FALSE, "GRAPHICS_Windows"));
    fprintf(fp, "\n");

/* Syntax Modes */
    fprintf(fp, "#\n");
    fprintf(fp, "# Site Dependent Syntax Modes\n");
    fprintf(fp, "#\n");

    yacc_exe = STRSAVE(dbget(cl, FALSE, "Yacc_Exe"));

    SynSCM = NULL;
    fprintf(fp, "SyntaxModes  =");
    if (IS_NULL(yacc_exe) == FALSE)
       {scsd = run(FALSE, "../scripts/scs-meta -n");

	fprintf(wlog, "scsd = %s\n", scsd);
	push_dir("%s/scheme/syntax", psy_base);

	sa = tokenize(run(FALSE, "ls -1"), " \n", 0);
	for (i = 0; sa[i] != NULL; i++)
	    {mod = sa[i];
	     if (dir_exists(mod))
	        {if (strcmp(mod, scsd) != 0)
		    SynSCM = lst_push(SynSCM, "applications/%ssynt.scm", mod);

		 fprintf(fp, " ${%s%s}", osrc, mod);};};
	lst_free(sa);

	pop_dir();};
    fprintf(fp, "\n");

    fprintf(fp, "SyntaxScheme = %s\n", concatenate(r, BFLRG, SynSCM,
						   0, -1, " "));
    lst_free(SynSCM);

/* Parallel I/O */
    nstrncpy(r, BFLRG, dbget(cl, FALSE, "DP_METHOD"), -1);
    fprintf(fp, "MPDevicesS  = ${S%s}\n", r);
    fprintf(fp, "MPDevicesO  = ${%s%s}\n", osrc, r);

    nstrncpy(r, BFLRG, dbget(cl, FALSE, "STD_IPC"), -1);
    fprintf(fp, "IPCDevicesS = ${S%s}\n", r);
    fprintf(fp, "IPCDevicesO = ${%s%s}\n", osrc, r);

    fprintf(fp, "\n");

    fprintf(fp, "#\n");
    fprintf(fp, "# Compiler/Loader Options\n");
    fprintf(fp, "#\n");

/* emit items from the defined groups */
    ng = sizeof(grps)/sizeof(char *);
    for (ig = 0; ig < ng; ig++)
        {g = grps[ig];

	 fprintf(fp, "\n");
	 fprintf(fp, "# group %s\n", g);

	 sa = tokenize(dbget(cl, FALSE, "PCO_UseVars"), " ", 0);

	 for (i = 0; sa[i] != NULL; i++)
	     {vr = sa[i];
	      vl = dbget(cl, FALSE, "%s_%s", g, vr);
	      if (IS_NULL(vl) == TRUE)
		 vl = dbget(cl, FALSE, vr);
	      fprintf(fp, "%s_%s = %s\n", g, vr, vl);};

	 lst_free(sa);};

    fprintf(fp, "\n");

/* emit all the tool variables */
    ng = sizeof(tools)/sizeof(tgdes);
    for (ig = 0; ig < ng; ig++)
        {tnm = tools[ig].name;
	 sa  = tools[ig].opt;
	 fprintf(fp, "# tool %s\n", tnm);
	 for (it = 0; sa[it] != NULL; it++)
	     {vr = sa[it];
	      vl = dbget(cl, FALSE, "%s_%s", tnm, vr);
	      if (IS_NULL(vl) == TRUE)
		 vl = dbget(cl, FALSE, vr);
	      fprintf(fp, "%s_%s = %s\n", tnm, vr, vl);};

	 fprintf(fp, "\n");};

/* emit items from the Glb group */
    fprintf(fp, "# Global variables\n");

    sa = tokenize(dbget(cl, FALSE, "PCO_Globals"), " ", 0);
    for (i = 0; sa[i] != NULL; i++)
        {vr = sa[i];
	 fprintf(fp, "%s = %s\n", vr, cgetenv(TRUE, vr));};
    lst_free(sa);
    fprintf(fp, "\n");

    fprintf(fp, "CROSS_FE    = %s\n", dbget(cl, FALSE, "CROSS_FE"));
    fprintf(fp, "DP_BE       = %s\n", dbget(cl, FALSE, "DP_BE"));

    if (strcmp(psy_inst, "none") == 0)
       {fprintf(fp, "PSY_PubLib  = \n");
        fprintf(fp, "PSY_PubInc  = \n");}
    else
       {psy_publib = dbget(cl, TRUE, "PSY_PubLib");
	fprintf(fp, "PSY_PubRoot = %s\n", path_head(psy_publib));
	fprintf(fp, "PSY_PubLib  = -L%s\n", psy_publib);
	fprintf(fp, "PSY_PubInc  = -I%s\n", dbget(cl, FALSE, "PSY_PubInc"));};

    fprintf(fp, "\n");

    fprintf(fp, "AF_LONG_DOUBLE = %s\n", dbget(cl, FALSE, "AF_LONG_DOUBLE"));
    fprintf(fp, "\n");

/* emit the load names for the libraries - cover threaded/nonthreaded */
    fprintf(fp, "SMP_LibExt=%s\n", dbget(cl, FALSE, "SMP_LibExt"));
    fprintf(fp, "LSCORE      = -lscore\n");
    fprintf(fp, "LMM         = -lpsmm\n");
    fprintf(fp, "LPML        = -lpml\n");
    fprintf(fp, "LPDB        = -lpdb\n");
    fprintf(fp, "LPGS        = -lpgs\n");
    fprintf(fp, "LPANACEA    = -lpanacea\n");
    fprintf(fp, "LSCHEME     = -lscheme\n");
    fprintf(fp, "LSX         = -lsx\n");
    fprintf(fp, "\n");

    fprintf(fp, "#\n");
    fprintf(fp, "# System utilities\n");
    fprintf(fp, "#\n");

    r[0] = '\0';
    vstrcat(r, BFLRG, " %s", dbget(cl, FALSE, "INSTALL_Exe"));
    vstrcat(r, BFLRG, " %s", dbget(cl, FALSE, "INSTALL_Flags"));
    fprintf(fp, "INSTALL    =%s\n", r);

    r[0] = '\0';
    vstrcat(r, BFLRG, " %s", dbget(cl, FALSE, "RM_Exe"));
    vstrcat(r, BFLRG, " %s", dbget(cl, FALSE, "RM_Flags"));
    fprintf(fp, "RM         =%s\n", r);

    r[0] = '\0';
    vstrcat(r, BFLRG, " %s", dbget(cl, FALSE, "Ranlib_Exe"));
    vstrcat(r, BFLRG, " %s", dbget(cl, FALSE, "Ranlib_Flags"));
    fprintf(fp, "RANLIB     =%s\n", r);

    have_py = dbget(cl, TRUE, "HAVE_PYTHON");
    if (strstr(have_py, "TRUE") != NULL)
       {sa = tokenize(dbget(cl, FALSE, "PY_Exe"), " ", 0);

	fprintf(fp, "PY_Vers    = %s\n", dbget(cl, FALSE, "PY_Vers"));
	fprintf(fp, "PY_CC      = %s\n", dbget(cl, FALSE, "PY_CC"));
	fprintf(fp, "PY_CfgDir  = %s\n", dbget(cl, FALSE, "PY_Cfg"));
	fprintf(fp, "PY_IncDir  = %s\n", dbget(cl, FALSE, "PY_Inc"));

/* GOTCHA: what about the other python versions in the list */
	fprintf(fp, "PythonPath = %s\n", path_head(sa[0]));

	lst_free(sa);};

    fprintf(fp, "\n");
    fprintf(fp, "#\n");
    fprintf(fp, "# Space Saving Build\n");
    fprintf(fp, "#     If you want that option do the following:\n");
    fprintf(fp, "#                    LibDep = ${ArObjs}\n");
    fprintf(fp, "#                    .PRECIOUS: ${TGTLib}\n");
    fprintf(fp, "#     otherwise do:\n");
    fprintf(fp, "#                    LibDep = ${Objs}\n");
    fprintf(fp, "#\n");

/* MAKE_Strategy handler */
    fprintf(fp, "LibDep = ${%sObjs}\n", osrc);
    if (strcmp(make_strategy, "SpaceSaving") == 0)
       {fprintf(fp, "\n");
	fprintf(fp, ".PRECIOUS: ${TGTLib}\n");};

    fprintf(fp, "\n");

    fprintf(fp, "SHELL   = %s\n", dbget(cl, FALSE, "SHELL_Default"));
    fprintf(fp, "Manager = %s\n", dbget(cl, FALSE, "PSY_MngDir"));
    fprintf(fp, "DATE    = `./code-date`\n");
    fprintf(fp, "\n");

    fprintf(fp, "IncGen     =\n");
    fprintf(fp, "\n");

/* get the entries for RPATH */
    npath = run(FALSE, "analyze/rpath -o link");
    fprintf(wlog, "analyze/rpath -o link => |%s|\n", npath);
    fprintf(fp, "LDPath     = %s\n", npath);

    lrpath = run(FALSE, "analyze/rpath -o rpath");
    fprintf(wlog, "analyze/rpath -o rpath => |%s|\n", lrpath);
    fprintf(fp, "LDRPath    = %s\n", lrpath);

    fprintf(fp, "LDFLAGS    = %s ${LDRPath} ${LDPath}\n",
	    dbget(cl, FALSE, "LD_Flags"));
    fprintf(fp, "LXFLAGS    = %s\n", dbget(cl, FALSE, "Lex_Flags"));
    fprintf(fp, "MDInc      = %s %s\n",
	    dbget(cl, FALSE, "MD_Inc"),
	    cc_inc);

    fprintf(fp, "MDLib      =");
    fprintf(fp, " %s", dbget(cl, FALSE, "MD_Lib"));
    fprintf(fp, " %s", dbget(cl, FALSE, "LD_Lib"));
    fprintf(fp, " %s", dbget(cl, FALSE, "LibM_Lib"));
    fprintf(fp, "\n");
    fprintf(fp, "DPInc      = %s\n", dp_inc);
    fprintf(fp, "DPLib      = %s\n", dbget(cl, FALSE, "DP_Lib"));
    fprintf(fp, "MDI_Inc    = %s\n", dbget(cl, FALSE, "MDI_Inc"));
    fprintf(fp, "MDI_Lib    = %s\n", dbget(cl, FALSE, "MDI_Lib"));
    fprintf(fp, "MDE_Lib    = %s\n", dbget(cl, FALSE, "MDE_Lib"));

    fc_ld_lib = dbget(cl, FALSE, "FC_LD_Lib");
    if (IS_NULL(fc_ld_lib) == FALSE)
       fprintf(fp, "FLib       = %s\n", fc_ld_lib);
    else
       fprintf(fp, "FLib       = \n");
    fprintf(fp, "\n");

    fprintf(fp, "FC_MOD_FLAG   = %s\n", dbget(cl, FALSE, "FC_MOD_FLAG"));
    fprintf(fp, "Linker        = %s\n", dbget(cl, FALSE, "LD_Exe"));
    fprintf(fp, "JOBS_System   = %s\n", dbget(cl, FALSE, "JOBS_System"));
    fprintf(fp, "\n");

/* compiler control - debug/opt, show flags, ... */
    fprintf(fp, "SFL_     = Abrv\n");
    fprintf(fp, "DefaultG = DefDebug\n");
    fprintf(fp, "DefaultO = DefOptimize\n");
    fprintf(fp, "g        = Debug\n");
    fprintf(fp, "o        = Optimize\n");
    fprintf(fp, "OD       = DefaultG\n");
    fprintf(fp, "\n");

/* compiler invocations */
    if (use_acc == TRUE)
       {fprintf(fp, "CCompiler      = acc\n");
	fprintf(fp, "CCmpLdr        = acc\n");
	fprintf(fp, "CDefDebug      = %s\n",
		dbget(cl, FALSE, "ACC_Debug_Default"));
	fprintf(fp, "CDefOptimize   = %s\n",
		dbget(cl, FALSE, "ACC_Optimize_Default"));}
    else
       {fprintf(fp, "CCompiler      = %s\n", dbget(cl, FALSE, "CC_Exe"));
	fprintf(fp, "CCmpLdr        = %s\n", dbget(cl, FALSE, "CC_Linker"));
	fprintf(fp, "CDefDebug      = %s\n",
		dbget(cl, FALSE, "CC_Debug_Default"));
	fprintf(fp, "CDefOptimize   = %s\n",
		dbget(cl, FALSE, "CC_Optimize_Default"));};

    fprintf(fp, "ODC            = ${C${${OD}}}\n");
    fprintf(fp, "CFLAGS         = %s\n", dbget(cl, FALSE, "CC_Flags"));
    fprintf(fp, "CDebug         = ${CCDefDebug}\n");
    fprintf(fp, "COptimize      = ${CCDefOptimize}\n");
    fprintf(fp, "\n");

/* invocations for ACC whether it is the default or not */
    fprintf(fp, "ACDefDebug     = %s\n",
	    dbget(cl, FALSE, "ACC_Debug_Default"));
    fprintf(fp, "ACDefOptimize  = %s\n",
	    dbget(cl, FALSE, "ACC_Optimize_Default"));
    fprintf(fp, "AODC           = ${AC${${OD}}}\n");
    fprintf(fp, "ACDebug        = ${ACDefDebug}\n");
    fprintf(fp, "ACOptimize     = ${ACDefOptimize}\n");
    fprintf(fp, "\n");

    if (use_acc == TRUE)
       {fprintf(fp, "CXCompiler     = a++\n");
	fprintf(fp, "CXCmpLdr       = a++\n");
	fprintf(fp, "CXDefDebug     = %s\n",
		dbget(cl, FALSE, "ACC_Debug_Default"));
	fprintf(fp, "CXDefOptimize  = %s\n",
		dbget(cl, FALSE, "ACC_Optimize_Default"));}
    else
       {fprintf(fp, "CXCompiler     = %s\n", dbget(cl, FALSE, "CXX_Exe"));
	fprintf(fp, "CXCmpLdr       = %s\n", dbget(cl, FALSE, "CXX_Linker"));
	fprintf(fp, "CXDefDebug     = %s\n",
		dbget(cl, FALSE, "CC_Debug_Default"));
	fprintf(fp, "CXDefOptimize  = %s\n",
		dbget(cl, FALSE, "CC_Optimize_Default"));};

    fprintf(fp, "CXFLAGS        = %s\n", dbget(cl, FALSE, "CXX_Flags"));
    fprintf(fp, "CXDebug        = ${CXXDefDebug}\n");
    fprintf(fp, "CXOptimize     = ${CXXDefOptimize}\n");
    fprintf(fp, "\n");

    if (use_acc == TRUE)
       {fprintf(fp, "FCompiler      = afc\n");
	fprintf(fp, "FDefDebug      = %s\n",
		dbget(cl, FALSE, "AFC_Debug_Default"));
	fprintf(fp, "FDefOptimize   = %s\n",
		dbget(cl, FALSE, "AFC_Optimize_Default"));}
    else
       {fprintf(fp, "FCompiler      = %s\n", dbget(cl, FALSE, "FC_Exe"));
	fprintf(fp, "FDefDebug      = %s\n",
		dbget(cl, FALSE, "FC_Debug_Default"));
	fprintf(fp, "FDefOptimize   = %s\n",
		dbget(cl, FALSE, "FC_Optimize_Default"));};

    fprintf(fp, "ODF            = ${F${${OD}}}\n");
    fprintf(fp, "FFLAGS         = %s\n", dbget(cl, FALSE, "FC_Flags"));
    fprintf(fp, "FDebug         = ${FCDefDebug}\n");
    fprintf(fp, "FOptimize      = ${FCDefOptimize}\n");
    fprintf(fp, "\n");

/* invocations for AFC whether it is the default or not */
    fprintf(fp, "AFDefDebug     = %s\n",
	    dbget(cl, FALSE, "AFC_Debug_Default"));
    fprintf(fp, "AFDefOptimize  = %s\n",
	    dbget(cl, FALSE, "AFC_Optimize_Default"));
    fprintf(fp, "AFOptimize     = ${AFDefOptimize}\n");
    fprintf(fp, "AODF           = ${AF${${OD}}}\n");
    fprintf(fp, "AFDebug        = ${AFDefDebug}\n");
    fprintf(fp, "AFOptimize     = ${AFDefOptimize}\n");
    fprintf(fp, "\n");

    fprintf(fp, "CCAnnounceFull = ${CC}\n");
    fprintf(fp, "FCAnnounceFull = ${FC}\n");
    fprintf(fp, "LXAnnounceFull = ${LX}\n");
    fprintf(fp, "YCAnnounceFull = ${YC}\n");
    fprintf(fp, "\n");

    fprintf(fp, "CCAnnounceAbrv = ${CCompiler} ${ODC}\n");
    fprintf(fp, "FCAnnounceAbrv = ${FCompiler} ${ODF}\n");
    fprintf(fp, "LXAnnounceAbrv = ${CCompiler} ${ODC}\n");
    fprintf(fp, "YCAnnounceAbrv = ${CCompiler} ${ODC}\n");
    fprintf(fp, "\n");

    fprintf(fp, "CCAnnounce     = ${CCAnnounce${SFL_}}\n");
    fprintf(fp, "FCAnnounce     = ${FCAnnounce${SFL_}}\n");
    fprintf(fp, "LXAnnounce     = ${LXAnnounce${SFL_}}\n");
    fprintf(fp, "YCAnnounce     = ${YCAnnounce${SFL_}}\n");
    fprintf(fp, "\n");

    fprintf(fp, "CC  = ${CCompiler} ${ODC} ${CFLAGS} ${Shared_CC_Flags} ${IncGen} ${CcFlags}\n");
    fprintf(fp, "CXX = ${CXCompiler} ${ODC} ${CXFLAGS} ${CxFlags}\n");
    fprintf(fp, "FC  = ${FCompiler} ${ODF} ${FFLAGS} ${FcFlags}\n");
    fprintf(fp, "LX  = ${CCompiler} ${ODC} ${LXFLAGS} ${CcFlags}\n");
    fprintf(fp, "YC  = ${CCompiler} ${ODC} ${LXFLAGS} ${CcFlags}\n");
    fprintf(fp, "LD  = ${Linker} ${LdFlags}\n");
    fprintf(fp, "\n");
 
    fprintf(fp, "ACCFlags  = ${AODC} ${CFLAGS} ${Shared_CC_Flags} ${IncGen} ${CcFlags}\n");
    fprintf(fp, "ACXXFlags = ${AODC} ${CXFLAGS} ${CxFlags}\n");
    fprintf(fp, "AFCFlags  = ${AODF} ${FFLAGS} ${FcFlags}\n");
    fprintf(fp, "\n");
 
/* other invocations */
    if (use_acc == TRUE)
       {fprintf(fp, "CLD            = acc ${ODC} ${CFLAGS} ${CcFlags}\n");
	fprintf(fp, "CCCfg          = acc ${ODC} ${Cfg_CC_Flags} ${CcFlags} ${Shared_CC_Flags}\n");
	fprintf(fp, "FCCfg          = afc ${ODF} ${Cfg_FC_Flags} ${FcFlags}\n");

	fprintf(fp, "CCSer          = acc ${ODC} ${Ser_CC_Flags} ${CcFlags} ${Shared_CC_Flags}\n");
	fprintf(fp, "FCSer          = afc ${ODF} ${Ser_FC_Flags} ${FcFlags}\n");

	fprintf(fp, "CCCfe          = acc ${ODC} ${Cfe_CC_Flags} ${CcFlags} ${Shared_CC_Flags}\n");
	fprintf(fp, "FCCfe          = afc ${ODF} ${Cfe_FC_Flags} ${FcFlags}\n");
	fprintf(fp, "CCCfe_LD_Flags = ${Cfe_LD_Flags}\n");
	fprintf(fp, "CCCfe_LD_Lib   = ${Cfe_LD_Lib}\n");}
    else
       {fprintf(fp, "CLD            = ${CCmpLdr} ${ODC} ${CFLAGS} ${CcFlags}\n");
	fprintf(fp, "CCCfg          = ${Cfg_CC_Exe} ${ODC} ${Cfg_CC_Flags} ${CcFlags} ${Shared_CC_Flags}\n");
	fprintf(fp, "FCCfg          = ${Cfg_FC_Exe} ${ODF} ${Cfg_FC_Flags} ${FcFlags}\n");

	fprintf(fp, "CCSer          = ${Ser_CC_Exe} ${ODC} ${Ser_CC_Flags} ${CcFlags} ${Shared_CC_Flags}\n");
	fprintf(fp, "FCSer          = ${Ser_FC_Exe} ${ODF} ${Ser_FC_Flags} ${FcFlags}\n");

	fprintf(fp, "CCCfe          = ${Cfe_CC_Exe} ${ODC} ${Cfe_CC_Flags} ${CcFlags} ${Shared_CC_Flags}\n");
	fprintf(fp, "FCCfe          = ${Cfe_FC_Exe} ${ODF} ${Cfe_FC_Flags} ${FcFlags}\n");
	fprintf(fp, "CCCfe_LD_Flags = ${Cfe_LD_Flags}\n");
	fprintf(fp, "CCCfe_LD_Lib   = ${Cfe_LD_Lib}\n");};

    fprintf(fp, "\n");

/* other tool invocations */
    fprintf(fp, "YACC     = %s %s\n",
	    yacc_exe,
	    dbget(cl, FALSE, "Yacc_Flags"));
    fprintf(fp, "LEX      = %s\n", dbget(cl, FALSE, "Lex_Exe"));
    fprintf(fp, "AR       = %s\n", dbget(cl, FALSE, "AR_Exe"));

    fprintf(fp, "AROpt    =");
    fprintf(fp, " %s", dbget(cl, FALSE, "AR_Flags"));
    fprintf(fp, " %s", dbget(cl, FALSE, "AR_IFlag"));
    fprintf(fp, "\n");

    fprintf(fp, "ARXOpt   =");
    fprintf(fp, " %s", dbget(cl, FALSE, "AR_Flags"));
    fprintf(fp, " %s", dbget(cl, FALSE, "AR_XFlag"));
    fprintf(fp, "\n");
    fprintf(fp, "\n");

    if (strcmp(psy_load, "TRUE") == 0)
       fprintf(fp, "Load           = ${CCmpLdr}\n");
    else
       fprintf(fp, "Load           = echo\n");

    fprintf(fp, "BinAction      = \n");
    fprintf(fp, "IncAction      = \n");
    fprintf(fp, "LibAction      = \n");
    fprintf(fp, "RemoveAction   = \n");
    fprintf(fp, "CleanAction    = \n");
    fprintf(fp, "\n");

    fprintf(fp, "\n");
    fprintf(fp, ".SUFFIXES: %s\n", dbget(cl, FALSE, "MAKE_SUFFIXES"));
    fprintf(fp, "\n");

    make_usegnu = dbget(cl, FALSE, "MAKE_UseGNU");
    if (strcmp(make_usegnu, "-DUSE_GNU_MAKE") == 0)
       {fprintf(fp, "(%%): %%\n");
	fprintf(fp, "\n");};

/* write the default make rules */
    printrule(fp, cl, ".c.i:", "IRules_CCP");
    printrule(fp, cl, ".c.o:", "IRules_CCObj");
    printrule(fp, cl, ".c.a:", "IRules_CCArc");
    printrule(fp, cl, ".l.o:", "IRules_LexObj");
    printrule(fp, cl, ".l.a:", "IRules_LexArc");
    printrule(fp, cl, ".l.c:", "IRules_LexC");
    printrule(fp, cl, ".y.o:", "IRules_YaccObj");
    printrule(fp, cl, ".y.a:", "IRules_YaccArc");
    printrule(fp, cl, ".y.c:", "IRules_YaccC");
    printrule(fp, cl, ".f.o:", "IRules_FCObj");
    printrule(fp, cl, ".f.a:", "IRules_FCArc");

    fprintf(fp, "\n");

/* reverse sense for exit status */
    rv = (rv == 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
