/*
 * PADEFT.C - test the functionality of file PADEF.C
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "panace.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main()
{
  int len1, len2, len3, len4;
  PA_dimens *vdims;
  pcons *att_alist, *unit_alist;
  PA_variable *pp;
  int foo3_data;

  /* initialize some stuff */
  _PA_internal_init();
  PA_defunc();
  PA_init_strings();
  PA_cpp_init();
  PA_cpp_default();
  PA_def_var_init();

/* ------- test units --------------- */
  PA_def_var_units("1/cm", PER, CM, UNITS);
  unit_alist = (pcons *) SC_hasharr_def_lookup(PA_var_unit_tab, "1/cm", "1/cm");
  printf("1/cm -\n");
  PA_print_alist(unit_alist);

/* ------- test attributes --------------- */
  PA_def_var_attribute("foo_att", PA_INFO_ALLOCATION, STATIC,
		       PA_INFO_PERSISTENCE, CACHE_F, PA_INFO_CENTER, Z_CENT, 0);
  att_alist = (pcons *) SC_hasharr_def_lookup(PA_var_att_tab, "foo_att", "foo_att");
  printf("foo_att -\n");
  PA_print_alist(att_alist);

/* ------- test dimensions --------------- */
  PA_def_var_dimension("dim1", &len1, LAST);
  vdims = (PA_dimens *) SC_hasharr_def_lookup(PA_var_dim_tab, "dim1", "dim1");
  printf("\ndim1 - ");
  PA_print_var_dim(vdims);

  len1 = 10;
  printf("\ndim1 - ");
  PA_print_var_dim(vdims);

  len2 = 4;
  len3 = 40;
  len4 = 50;

  vdims = PA_def_var_dimension("dim2", PA_DON, &len1, &len2, LAST);
  printf("\ndim2 - ");
  PA_print_var_dim(vdims);

  vdims = PA_def_var_dimension("dim3", PA_DUL, &len3, &len4, LAST);
  printf("\ndim3 - ");
  PA_print_var_dim(vdims);

  PA_def_var_dimension("dim4", &len1, &len2, &len3, 
		       PA_DON, &len1, &len2, PA_DUL, &len3, &len4, LAST);
  vdims = (PA_dimens *) SC_hasharr_def_lookup(PA_var_dim_tab, "dim4", "dim4");
  printf("\ndim4 - ");
  PA_print_var_dim(vdims);

  printf("\n");
/* ------- test variables --------------- */

  printf("define static allocation default\n");
  pp = PA_def_var_default("std_default", PA_INFO_ALLOCATION, STATIC, 0);

  printf("define dynamic allocation default\n");
  pp = PA_def_var_default("std_default1", PA_INFO_ALLOCATION, DYNAMIC, 0);

  pp = PA_def_variable("foo", LAST_TAG);
  PA_print_variable(pp);

  printf("\nAll defaults\n");
  pp = PA_def_variable("foo1", PA_INFO_SCOPE, SCRATCH, 0);
  PA_print_variable(pp);

  printf("\nshare=foo1, persist=cache_r\n");
  pp = PA_def_variable("foo2", PA_INFO_SHARE, "foo1", PA_INFO_PERSISTENCE, CACHE_R, 0);
  PA_print_variable(pp);

  printf("att_name=foo_att\n");
  pp = PA_def_variable("foo3", PA_INFO_ATT_NAME, "foo_att", 
		       PA_INFO_DATA_PTR, &foo3_data, 0);
  PA_print_variable(pp);

  printf("share=foo3, dim=len1\n");
  pp = PA_def_variable("foo4", PA_INFO_SHARE, "foo3", PA_INFO_DIMS, &len1, LAST, 0);
  PA_print_variable(pp);

  printf("share=foo3, dim=len2\n");
  pp = PA_def_variable("foo5", PA_INFO_SHARE, "foo3", PA_INFO_DIMS, &len2, LAST, 0);
  PA_print_variable(pp);

  printf("share=foo3, dim_name=dim3\n");
  pp = PA_def_variable("foo6", PA_INFO_SHARE, "foo3", PA_INFO_DIM_NAME, "dim3", 0);
  PA_print_variable(pp);

  printf("share=foo6, persist=cache_f\n");
  pp = PA_def_variable("foo7", PA_INFO_SHARE, "foo6", PA_INFO_PERSISTENCE, CACHE_F, 0);
  PA_print_variable(pp);

  printf("with units PER 1/cm\n");
  pp = PA_def_variable("foo8", PA_INFO_UNITS_NAME, "1/cm", 0);
  PA_print_variable(pp);

  printf("with units MOLE Q PER CC K\n");
  pp = PA_def_variable("foo9", PA_INFO_UNITS, MOLE, Q, PER, CC, K, UNITS, 0);
  PA_print_variable(pp);

  printf("with units RAD\n");
  pp = PA_def_variable("foo10", PA_INFO_UNITS, RAD, UNITS, 0);
  PA_print_variable(pp);

  pp = PA_def_variable("foo11", PA_INFO_UNITS, MOLE, Q, PER, CC, K, UNITS);
  pp = PA_def_variable("foo11", -100, 20, 0);
  pp = PA_def_variable("foo11", PA_INFO_ALLOCATION, RUNTIME, 0);

  pp = PA_def_variable("foo12", PA_INFO_ATT_NAME, "bad_add",
		       PA_INFO_DIM_NAME, "bad_dim",
		       PA_INFO_UNITS_NAME, "bad_units",
		       0);
/*  PA_print_var_tab(PA_variable_tab); */

}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void PA_print_var_tab(hasharr *tab)
   {int i;
    char **dump, **work;
    PA_variable *pp;

    dump = SC_hasharr_dump(tab, NULL, NULL, FALSE);
    if (dump != NULL)
       {for (work=dump; *work != NULL; work++)
	    {pp = PA_inquire_variable(*work);
	     PA_print_variable(pp);};

	 SFREE(dump);};

   return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PA_print_variable(PA_variable *pp)
{
  printf("Name   : %s\n", PA_VARIABLE_NAME(pp));
  printf("Type   : %s\n", PA_VARIABLE_TYPE_S(pp));
  printf("Scope  : %s\n",
	 PA_cpp_value_to_name(PA_CPP_SCOPE, PA_VARIABLE_SCOPE(pp)));
  printf("Class  : %s\n",
	 PA_cpp_value_to_name(PA_CPP_CLASS, PA_VARIABLE_CLASS(pp)));
  printf("Persist: %s\n",
	 PA_cpp_value_to_name(PA_CPP_PERSISTENCE, PA_VARIABLE_PERSISTENCE(pp)));
  printf("Center : %s\n",
	 PA_cpp_value_to_name(PA_CPP_CENTER, PA_VARIABLE_CENTERING(pp)));
  printf("Alloc  : %s\n",
	 PA_cpp_value_to_name(PA_CPP_ALLOCATION, PA_VARIABLE_ALLOCATION(pp)));

  PA_print_var_dim(PA_VARIABLE_DIMS(pp));
  PA_print_var_units(pp);

  printf("\n");
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PA_print_var_dim(PA_dimens *vdims)
   {int min_index = 0;
    int max_index = 0;
    int *mini, *maxi;
    int ixx, ixn;
    PA_dimens *pvd;
    char *dim;
    char s[MAXLINE];

    printf("dim    : ");
    if (vdims == NULL)
       printf("NULL\n");

    else
       {strcpy(s, "[");
	for (pvd = vdims; pvd != NULL; pvd = pvd->next)
	    {if (pvd->index_max != NULL)
	        maxi = pvd->index_max;
	     else
	        maxi = &max_index;

	     if (pvd->index_min != NULL)
	        mini = pvd->index_min;
	     else
	        mini = &min_index;

	     ixn = 0;
	     ixx = -1;
	     switch (pvd->method)
	        {case PA_UPPER_LOWER :
		      ixx = *maxi;
		      ixn = *mini;
		      ixx = max(ixn, ixx);
		      break;
      
		 case PA_OFFSET_NUMBER :
		      ixn = *mini;
		      ixx = *mini + *maxi - 1L;
		      ixx = max(ixn, ixx);
		      break;};
	     SC_vstrcat(s, MAXLINE, "%d:%d,", ixn, ixx);};

	SC_LAST_CHAR(s) = ']';
	printf("%s\n", s);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PA_print_var_units(PA_variable *pp)
{
  int id;
  pcons *pc, *pd, *pn;

  pn = PA_VARIABLE_UNIT_NUMER(pp);
  pd = PA_VARIABLE_UNIT_DENOM(pp);

  printf("unit   :");

  if (pn == NULL && pd == NULL) {
    printf(" NULL\n");
  } else {
    if (pn != NULL) {
      for (pc = pn; pc != NULL; pc = (pcons *) pc->cdr) {
	id = PA_LST_VAL(pc);
	printf(" %s", PA_cpp_value_to_name(PA_CPP_UNITS, id));
      }
    }

    if (pd != NULL) {
      printf(" PER");
      for (pc = pd; pc != NULL; pc = (pcons *) pc->cdr) {
	id = PA_LST_VAL(pc);
	printf(" %s", PA_cpp_value_to_name(PA_CPP_UNITS, id));
      }
    }
    printf("\n");
  }

  return;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_PRINT_ALIST -  */

PA_print_alist(pcons *alist)
{
  pcons *pa, *c;

  printf("---\n");
  for (pa = alist; pa != NULL; pa = (pcons *) pa->cdr) {

    _PA_print_alist_node(pa->car_type, pa->car);

  }

}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

_PA_print_alist_node(char *type, void *data)
{
  int itype;
  int i;
  char *s;
  pcons *pp;

  itype = convert_type_s_i(type);

  if (data == NULL) {
    printf("NULL\t(%s)", type);
  } else {
    switch (itype) {
    case SC_INTEGER_I:
    case SC_INTEGER_P_I:
      i = *(int *) data;
      printf("%d\t(%s)", i, type);
      break;
    case SC_STRING_I:
      s = (char *) data;
      printf("%s\t(%s)", s, type);
      break;
    case SC_PCONS_I:
    case SC_PCONS_P_I:
      pp = (pcons *) data; 
      printf("%x (%s)\n", data, type);
      _PA_print_alist_node(pp->car_type, pp->car);
      printf("\t");
      _PA_print_alist_node(pp->cdr_type, pp->cdr);
      _PA_print_info_sym(pp->car, pp->cdr);
      printf("\n");
      break;
    default:
      printf("%x", data);
    }
  }

}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

_PA_print_info_sym(char *field, int *data)
{
  char *name;

  if (data != NULL) {
    if (strcmp(field, PA_INFO_CENTER_S) == 0) {
      name = PA_cpp_value_to_name(PA_CPP_CENTER, *data);
      printf("\t(%s)", name);
    } else if (strcmp(field, PA_INFO_ALLOCATION_S) == 0) {
      name = PA_cpp_value_to_name(PA_CPP_ALLOCATION, *data);
      printf("\t(%s)", name);
    } else if (strcmp(field, PA_INFO_SCOPE_S) == 0) {
      name = PA_cpp_value_to_name(PA_CPP_SCOPE, *data);
      printf("\t(%s)", name);
    } else if (strcmp(field, PA_INFO_PERSISTENCE_S) == 0) {
      name = PA_cpp_value_to_name(PA_CPP_PERSISTENCE, *data);
      printf("\t(%s)", name);
    } else if (strcmp(field, PA_INFO_UNITS_S) == 0) {
      name = PA_cpp_value_to_name(PA_CPP_UNITS, *data);
      printf("\t(%s)", name);
    } else if (strcmp(field, PA_INFO_UNIT_DENOM_S) == 0) {
      name = PA_cpp_value_to_name(PA_CPP_UNITS, *data);
      printf("\t(%s)", name);
    } else if (strcmp(field, PA_INFO_UNIT_NUMER_S) == 0) {
      name = PA_cpp_value_to_name(PA_CPP_UNITS, *data);
      printf("\t(%s)", name);
    } else if (strcmp(field, PA_INFO_TYPE_S) == 0) {
      name = PA_cpp_value_to_name(PA_CPP_TYPE, *data);
      printf("\t(%s)", name);
    }
  } 
    
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

