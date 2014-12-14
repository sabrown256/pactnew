/*
 * TEMPLATE-TYPE.C - generate C type manager inits
 *
 * #include "cpyright.h"
 *
 */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DEF_TYPE_MANAGER - generate type definitions for SCORE type manager */

static void def_type_manager(FILE *fp, int ne, typdes *tl)
   {int id, nv;
    char **tia, **tma;
    typdes *td, *tp;

    fprintf(fp, "#define REAL double\n");
    fprintf(fp, "#define HUGE_REAL 1.0e100\n");
    fprintf(fp, "\n");

    fprintf(fp, "#ifndef HAVE_FIXED_WIDTH_FLOAT\n");
    fprintf(fp, "typedef float       float32_t;\n");
    fprintf(fp, "typedef double      float64_t;\n");
    fprintf(fp, "typedef long double float128_t;\n");
    fprintf(fp, "#endif\n");
    fprintf(fp, "\n");

    fprintf(fp, "/*--------------------------------------------------------------------------*/\n");
    fprintf(fp, "\n");
    fprintf(fp, "/*                           DEFINED CONSTANTS                              */\n");
    fprintf(fp, "\n");
    fprintf(fp, "/*--------------------------------------------------------------------------*/\n");
    fprintf(fp, "\n");

    fprintf(fp, "#define N_PRIMITIVE_CHAR  %d\n", type_counts[KIND_CHAR]);
    fprintf(fp, "#define N_PRIMITIVE_FIX   %d\n", type_counts[KIND_INT]);
    fprintf(fp, "#define N_PRIMITIVE_FP    %d\n", type_counts[KIND_FLOAT]);
    fprintf(fp, "#define N_PRIMITIVE_CPX   %d\n", type_counts[KIND_COMPLEX]);
    fprintf(fp, "#define N_PRIMITIVE_QUT   %d\n", type_counts[KIND_QUATERNION]);
    fprintf(fp, "\n");

    fprintf(fp, "#define N_PRIMITIVES     %d\n", N_PRIMITIVES);
    fprintf(fp, "#define N_TYPES          %d\n", N_TYPES);
    fprintf(fp, "\n");

/* define the default static type set macro */
    fprintf(fp, "#define _SC_DEF_STATIC_TYPE_LIST_ {                                             \\\n");

    for (id = 0; id < ne; id++)
        {td = tl + id;
         fprintf(fp, "   { %4d, ",
		 td->id);

/* print bpi field */
         if (td->g == KIND_OTHER)
            fprintf(fp, "%4d, ", -1);
         else if (td->alias != NULL)
            fprintf(fp, "sizeof(%s), ", td->alias);
	 else if (td->bpi > 0)
	    fprintf(fp, "%4d, ", td->bpi);
	 else
            fprintf(fp, "sizeof(%s), ", td->type);

	 fprintf(fp, "\"%s\", \"%s\", \"%s\", \"%s\", \"%s\", ",
		 td->type, td->stype, td->utype, td->ftype, td->fncp);
	 TYPE_FIELD(fp, td->alias);

	 fprintf(fp, "%d, ",
		 td->ptr);
	 fprintf(fp, "%s, %s, ",
		 tyknde[td->knd], tygrpe[td->g]);

	 TYPE_FIELD(fp, td->mn);
	 TYPE_FIELD(fp, td->mx);
	 TYPE_FIELD(fp, td->defv);
	 TYPE_FIELD(fp, td->f90);
	 TYPE_FIELD(fp, td->promo);
	 TYPE_FIELD(fp, td->comp);

	 fprintf(fp, "\"%s\", \"%s\", ",
		 td->typ_i, td->typ_s);

	 fprintf(fp, "NULL, NULL, NULL }, \t\\\n");};

    fprintf(fp, "   { -1, -1, NULL, NULL, NULL, NULL, NULL, NULL, 0, TK_META, KIND_OTHER, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL } \t\\\n");

    fprintf(fp, "   }\n");
    fprintf(fp, "\n");

/* #define index and name variables - e.g. SC_INT_S, SC_INT_S */
    tia = NULL;
    tma = NULL;
    nv = 0;
    for (id = 0; id < ne; )
        {td = tl + id++;
	 tia = lst_push(tia, "#define %s\t\tSC_gs.ltyp[%d].i\n",
			td->typ_i, nv);
	 tia = lst_push(tia, "#define %s\t\tSC_gs.ltyp[%d].s\n",
			td->typ_s, nv);

	 if (td->ptr == B_T)
	    {tp = tl + id++;
	     tia = lst_push(tia, "#define %s\t\tSC_gs.ltyp[%d].p_i\n",
			    tp->typ_i, nv);
	     tia = lst_push(tia, "#define %s\t\tSC_gs.ltyp[%d].p_s\n",
			    tp->typ_s, nv);

	     tma = lst_push(tma, "   { %3d, \"%s\",\t%3d, \"%s\" },   \t\t\\\n",
			    td->id, td->type, tp->id, tp->type);}
	 else
	    tma = lst_push(tma, "   { %3d, \"%s\", },   \t\t\\\n",
			   td->id, td->type);

	 nv++;};

    fprintf(fp, "#define SC_TYP_N\t\t%d\n", nv);
    strings_out(fp, tia, 0, -1, B_F);
    fprintf(fp, "\n");

/* define the default type initializer macro */
    fprintf(fp, "/* these must have the same sequence/values as the dynamic values */\n");
    fprintf(fp, "\n");
    fprintf(fp, "#define _SC_DEF_TYP_ {                                                       \\\n");
    strings_out(fp, tma, 0, -1, B_F);
    fprintf(fp, "   }\n");
    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
