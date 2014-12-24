/*
 * TEMPLATE-VARARG.C - generate primitive type vararg routines
 *
 * #include "cpyright.h"
 *
 */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_VA_ARG_CLAUSE - write the variable arg clause for type ID */

static void write_va_arg_clause(FILE *fp, type_desc *td)
   {char *ty, *tp;

    ty = td->type;
    if ((td->promo != NULL) && (td->g != KIND_POINTER))
       tp = td->promo;
    else
       tp = ty;

    fprintf(fp, "       case %d:   \t\t\\\n", td->id);
    fprintf(fp, "            {%s *_pv = (%s *) (_d);   \t\t\\\n", ty, ty);
    fprintf(fp, "             _pv[_n] = va_arg(__a__, %s);};   \t\t\\\n", tp);
    fprintf(fp, "            break;    \t\t\\\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DEF_VARARG_MACROS - generate the variable arg type macros */

static void def_vararg_macros(FILE *fp, int ne, type_desc *tl)
   {int i;
    type_desc *td;

/* variable arg macros */
    fprintf(fp, "#ifndef NO_VA_ARG_ID\n");

/* if va_arg supports complex types */
    fprintf(fp, "#ifdef HAVE_COMPLEX_VA_ARG\n");

    fprintf(fp, "#define SC_VA_ARG_ID(_id, _d, _n)                    \\\n");
    fprintf(fp, "   {int _lid;                                        \\\n");
    fprintf(fp, "    if (_id == SC_STRING_I)                          \\\n");
    fprintf(fp, "       _lid = _id;                                   \\\n");
    fprintf(fp, "    else if (SC_is_type_ptr(_id) == TRUE)            \\\n");
    fprintf(fp, "       _lid = SC_POINTER_I;                          \\\n");
    fprintf(fp, "    else                                             \\\n");
    fprintf(fp, "       _lid = _id;                                   \\\n");
    fprintf(fp, "    switch (_lid) {                                  \\\n");

    for (i = 0; tl[i].type != NULL; i++)
        {td = tl + i;
	 if (td->alias == NULL)
	    {switch (td->g)
	        {case KIND_BOOL       :
		 case KIND_CHAR       :
		 case KIND_INT        :
		 case KIND_ENUM       :
		 case KIND_FLOAT      :
		 case KIND_COMPLEX    :
		 case KIND_QUATERNION :
		      write_va_arg_clause(fp, td);
		      break;
		 case KIND_POINTER :
		      if ((strcmp(td->type, "char *") == 0) ||
			  (strcmp(td->type, "void *") == 0))
			 write_va_arg_clause(fp, td);
		      break;
		 default :
		      break;};};};

    fprintf(fp, "       }                                             \\\n");
    fprintf(fp, "   }\n");

/* if va_arg does NOT support complex type */
    fprintf(fp, "#else\n");

    fprintf(fp, "#define SC_VA_ARG_ID(_id, _d, _n)                    \\\n");
    fprintf(fp, "   {int _lid;                                        \\\n");
    fprintf(fp, "    if (_id == SC_STRING_I)                          \\\n");
    fprintf(fp, "       _lid = _id;                                   \\\n");
    fprintf(fp, "    else if (SC_is_type_ptr(_id) == TRUE)            \\\n");
    fprintf(fp, "       _lid = SC_POINTER_I;                          \\\n");
    fprintf(fp, "    else                                             \\\n");
    fprintf(fp, "       _lid = _id;                                   \\\n");
    fprintf(fp, "    switch (_lid) {                                  \\\n");

    for (i = 0; tl[i].type != NULL; i++)
        {td = tl + i;
	 if (td->alias == NULL)
	    {switch (td->g)
	        {case KIND_BOOL       :
		 case KIND_CHAR       :
		 case KIND_INT        :
		 case KIND_ENUM       :
		 case KIND_FLOAT      :
		 case KIND_QUATERNION :
		      write_va_arg_clause(fp, td);
		      break;
		 case KIND_POINTER :
		      if ((strcmp(td->type, "char *") == 0) ||
			  (strcmp(td->type, "void *") == 0))
			 write_va_arg_clause(fp, td);
		      break;
		 default :
		      break;};};};

    fprintf(fp, "       }                                             \\\n");
    fprintf(fp, "   }\n");

    fprintf(fp, "#endif\n");
    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_VA_ARG - write the va arg routines */

static void write_va_arg(FILE *fp, int ne, type_desc *tl)
   {

/* variable arg macros */
    def_vararg_macros(fp, ne, tl);

    fprintf(fp, "#endif\n");
    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
