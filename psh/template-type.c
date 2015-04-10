/*
 * TEMPLATE-TYPE.C - generate C type manager inits
 *
 * #include "cpyright.h"
 *
 */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DEF_TYPE_MANAGER - generate type definitions for SCORE type manager */

static void def_type_manager(FILE *fp, int ne, type_desc *tl)
   {int id;
    pboolean cxx;
    char **tia;
    type_desc *td, *tp;

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
    fprintf(fp, "#define N_SPECIAL_ENUM    %d\n", type_counts[KIND_ENUM]);
    fprintf(fp, "#define N_SPECIAL_STRUCT  %d\n", type_counts[KIND_STRUCT]);
    fprintf(fp, "#define N_SPECIAL_UNION   %d\n", type_counts[KIND_UNION]);
    fprintf(fp, "\n");

    fprintf(fp, "#define N_PRIMITIVES     %d\n", N_PRIMITIVES);
    fprintf(fp, "#define N_TYPES          %d\n", N_TYPES);
    fprintf(fp, "\n");

    fprintf(fp, "#undef DEF_FUNCTION_PTR\n");
    fprintf(fp, "#ifdef __cplusplus\n");
    fprintf(fp, "# define DEF_FUNCTION_PTR(_t, _n) typedef _t (*PF##_n)(...)\n");
    fprintf(fp, "#else\n");
    fprintf(fp, "# define DEF_FUNCTION_PTR(_t, _n) typedef _t (*PF##_n)(void)\n");
    fprintf(fp, "#endif\n");
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

/* #define index and name variables - e.g. G_INT_S, G_INT_S */
    tia = NULL;
    for (id = 0; id < ne; id++)
        {td  = tl + id;
	 tia = lst_push(tia, "#define %s\t\tSC_gs.stl[%d].id\n",
			td->typ_i, id);
	 tia = lst_push(tia, "#define %s\t\tSC_gs.stl[%d].type\n",
			td->typ_s, id);};

    strings_out(fp, tia, 0, -1, B_F);
    fprintf(fp, "\n");

/* function pointer typedefs for statically defined types */
    cxx = B_F;
    tia = NULL;
    tia = lst_push(tia, "DEF_FUNCTION_PTR(void, Void);\n");
    for (id = 0; id < ne; )
        {td  = tl + id++;
         if ((td->g != KIND_OTHER) && (td->alias == NULL))
	    {if ((td->g == KIND_COMPLEX) && (cxx == B_F))
	        {cxx = B_T;
		 tia = lst_push(tia, "#ifndef __cplusplus\n");}
	     else if ((td->g == KIND_QUATERNION) && (cxx == B_T))
	        {cxx = B_F;
		 tia = lst_push(tia, "#endif\n");};
	     tia = lst_push(tia, "DEF_FUNCTION_PTR(%s, %s);\n",
			    td->type, fix_camelcase(td->type));
	     if (td->ptr == B_T)
	        {tp  = tl + id++;
		 tia = lst_push(tia, "DEF_FUNCTION_PTR(%s, %s);\n",
				tp->type, fix_camelcase(tp->type));};};};

    strings_out(fp, tia, 0, -1, B_F);
    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
