/*
 * PADEF.C - new definition routines for PANACEA
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "panacea_int.h"

#define PA_info_name(tag)  PA_cpp_value_to_name(PA_gs.cpp_info, tag)

int
 PA_def_error;

char
 *PA_INFO_TYPE_S,
 *PA_INFO_N_DIMS_S,
 *PA_INFO_DIMS_S,
 *PA_INFO_SCOPE_S,
 *PA_INFO_CLASS_S,
 *PA_INFO_CENTER_S,
 *PA_INFO_PERSISTENCE_S,
 *PA_INFO_ALLOCATION_S,
 *PA_INFO_FILE_NAME_S,
 *PA_INFO_INIT_VAL_S,
 *PA_INFO_INIT_FNC_S,
 *PA_INFO_CONV_S,   /* ext_unit */
 *PA_INFO_UNIT_S,   /* int_unit */
 *PA_INFO_KEY_S,
 *PA_INFO_ATTRIBUTE_S,
 *PA_INFO_UNITS_S,
 *PA_INFO_DATA_PTR_S,
 *PA_INFO_UNIT_NUMER_S,
 *PA_INFO_UNIT_DENOM_S,
 *PA_INFO_APPL_ATTR_S,
 *PA_INFO_DEFAULT_S,
 *PA_INFO_SHARE_S,
 *PA_INFO_ATT_NAME_S,
 *PA_INFO_DIM_NAME_S,
 *PA_INFO_UNITS_NAME_S,
 *PA_INFO_DOMAIN_NAME_S,
 *PA_INFO_MAP_DOMAIN_S,
 *PA_INFO_BUILD_DOMAIN_S;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_DEF_VARIABLE - define a variable
 *                 - process the arguments then install into the
 *                 - variable table
 */

PA_variable *PA_def_variable(char *name, ...)
   {PA_variable *pp;

    SC_VA_START(name);
    pp = _PA_process_def_var(name, &SC_VA_VAR);
    SC_VA_END;
  
    if (PA_VARIABLE_ALLOCATION(pp) == STATIC)
       {PA_WARN((PA_VARIABLE_DATA(pp) == NULL),
		"VARIABLE %s IS STATIC AND PA_INFO_DATA_PTR IS NOT DEFINED",
		PA_VARIABLE_NAME(pp));};

    PA_gs.variable_tab = PA_install_table(name, pp,
					  PA_gs.variable,
					  PA_gs.variable_tab);

    return(pp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_DEF_VAR_DEFAULT - define a variable default template
 *                    - it's just like a variable, except it's not in the
 *                    - variable table, it's in PA_va_def_tab
 *                    - maybe this this be changed someday to use
 *                    - package scoping instead of a different table
 */

PA_variable *PA_def_var_default(char *name, ...)
   {PA_variable *pp;

    SC_VA_START(name);
    pp = _PA_process_def_var(name, &SC_VA_VAR);
    SC_VA_END;

    PA_gs.var_def_tab = PA_install_table(name, pp,
					 PA_gs.variable,
					 PA_gs.var_def_tab);

    return(pp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_DEF_VAR_ATTRIBUTE - take a bunch of variable attributes, put them
 *                      - on an alist and store into a table
 *                      - it can be looked up later,
 *                      - by _PA_process_def_var and used to define
 *                      - attributes for variable
 */

pcons *PA_def_var_attribute(char *name, ...)
   {int enough;
    int tag;
    pcons *att_alist = NULL;

    att_alist = NULL;

    SC_VA_START(name);

    enough = FALSE;
    while (!enough)
       {tag = SC_VA_ARG(int);
	switch (tag)
	   {case PA_INFO_ALLOCATION :
            case PA_INFO_CENTER :
            case PA_INFO_CLASS :
            case PA_INFO_PERSISTENCE :
            case PA_INFO_SCOPE :
                 att_alist = _PA_process_att(tag, &SC_VA_VAR, att_alist);
		 break;

            case LAST_TAG:
                 enough = TRUE;
		 break;

            default:
                 PA_def_error = 1;
	         PA_WARN(PA_def_error,
			 "ILLEGAL TAG IN PA_DEF_VAR_ATTRIBUTE %s (%d = %s)",
			 name, tag, PA_info_name(tag));};};

    SC_VA_END;

/* check if att_alist is null, no fields */

/* store in table */
    PA_gs.var_att_tab = PA_install_table(name, att_alist,
					 PA_gs.attribute,
					 PA_gs.var_att_tab);

    return(att_alist);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_DEF_VAR_DIMENSION - define variable dimensions */

PA_dimens *PA_def_var_dimension(char *name, ...)
   {PA_dimens *vdims;

    SC_VA_START(name);
    vdims = _PA_process_dimension(&SC_VA_VAR);
    SC_VA_END;

/* store in table */
    PA_gs.var_dim_tab = PA_install_table(name, vdims,
					 PA_gs.dimension,
					 PA_gs.var_dim_tab);

    return(vdims);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_DEF_VAR_UNIT - define variable PA_gs.unitss */

pcons *PA_def_var_units(char *name, ...)
   {pcons *unit_alist;

    unit_alist = NULL;
    SC_VA_START(name);
    unit_alist = _PA_process_units(&SC_VA_VAR, unit_alist);
    SC_VA_END;
    
/* store in table */
    PA_gs.var_unit_tab = PA_install_table(name, unit_alist,
					  PA_gs.unit,
					  PA_gs.var_unit_tab);

    return(unit_alist);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_DEF_VAR_DOMAIN - define a variable domain */

void PA_def_var_domain(char *name, ...)
  {

   SC_VA_START(name);

/* PA_var_dom_tab = PA_install_table(name, pp, PA_gs.domain, PA_var_dom_tab); */

   SC_VA_END;

   return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_DEF_VAR_INIT - define a variable initializer */

void PA_def_var_init(void)
   {int vattr[N_ATTRIBUTES];
    double conv_fact, unit_fact;
    char *vtype;
    pcons *nu, *du, *alist;
    PA_dimens *vdims;
    void *iv;
    PFVarInit ifn;
    PA_variable *pp;
    
/* setup defaults */
    if (_PA.default_variable == NULL)
       {vattr[0]  = RUNTIME;
	vattr[1]  = OPTL;
	vattr[2]  = REL;
	vattr[3]  = U_CENT;
	vattr[4]  = DYNAMIC;
	vdims     = NULL;
	iv        = NULL;
	ifn       = NULL;
	vtype     = SC_DOUBLE_S;
	conv_fact = 1.0;
	unit_fact = 1.0;
	nu        = NULL;
	du        = NULL;
	alist     = NULL;

	pp = _PA_mk_variable("default-variable", vdims, iv, ifn,
			     vattr, vtype,
			     conv_fact, unit_fact, nu, du, alist);

	PA_gs.var_def_tab = PA_install_table("default-variable", pp,
					     PA_gs.variable,
					     PA_gs.var_def_tab);

	_PA.default_variable = pp;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_PROCESS_DEF_VAR - two tag alists are maintained to correspond to
 *                     - the two levels of precedence
 *                     - as each tag is read off of the input, interpret
 *                     - it and add the fields that it represents onto the
 *                     - correct alist
 *                     - the level of precedence from low to high is:
 *                     -    alist2
 *                     -    alist1
 *                     - look up the name given in the correct table
 *                     - and add the values defined in the corresponding
 *                     - PA_def_var_xxx call onto alist2
 *                     - for all the rest add the value onto alist1
 *                     -
 *                     - after all input has been added onto an alist
 *                     - search the lists in inverse order of precedence
 *                     - this will find the highest precedence last
 */

PA_variable *_PA_process_def_var(char *vname, va_list *lst)
   {int i;
    int enough;
    int tag;
    int *pival;
    int vattr[N_ATTRIBUTES];
    char *attr_name[N_ATTRIBUTES];
    char *name;
    char *vtype;
    char *tag_name;
    pcons *nu, *du, *alist;
    PA_variable *pp;
    PA_dimens *vdims;
    void *iv;
    void *data_ptr;
    PFVarInit ifn;
    pcons *alist1, *alist2;
    pcons *att_alist, *unit_alist;
  
    attr_name[0] = PA_INFO_SCOPE_S;
    attr_name[1] = PA_INFO_CLASS_S;
    attr_name[2] = PA_INFO_PERSISTENCE_S;
    attr_name[3] = PA_INFO_CENTER_S;
    attr_name[4] = PA_INFO_ALLOCATION_S;

    vattr[0]  = PA_VARIABLE_SCOPE(_PA.default_variable);
    vattr[1]  = PA_VARIABLE_CLASS(_PA.default_variable);
    vattr[2]  = PA_VARIABLE_PERSISTENCE(_PA.default_variable);
    vattr[3]  = PA_VARIABLE_CENTERING(_PA.default_variable);
    vattr[4]  = PA_VARIABLE_ALLOCATION(_PA.default_variable);
    vdims     = PA_VARIABLE_DIMS(_PA.default_variable);
    iv        = PA_VARIABLE_INIT_VAL(_PA.default_variable);
    ifn       = PA_VARIABLE_INIT_FUNC(_PA.default_variable);
    vtype     = PA_VARIABLE_TYPE_S(_PA.default_variable);
    nu        = PA_VARIABLE_UNIT_NUMER(_PA.default_variable);
    du        = PA_VARIABLE_UNIT_DENOM(_PA.default_variable);
    alist     = PA_VARIABLE_ATTR(_PA.default_variable);
    data_ptr  = NULL;

/* make two alists, one for each precedence level */
    alist1 = NULL;  /* fields   PA_INFO_SCOPE =        */
    alist2 = NULL;  /* named groups PA_INFO_ATT_NAME = */

    SC_VA_SAVE(lst);

    att_alist = NULL;
    name      = NULL;
    enough    = FALSE;
    while (!enough)
       {tag = SC_VA_ARG(int);
	tag_name = PA_info_name(tag);
	switch (tag)

/* reset defaults */
           {case PA_INFO_SHARE :
	    case PA_INFO_DEFAULT :
	         name = SC_VA_ARG(char *);
		 if (tag == PA_INFO_DEFAULT)
		    pp = (PA_variable *) SC_hasharr_def_lookup(PA_gs.var_def_tab,
							       name);
		 else
		    pp = PA_inquire_variable(name);

/* override defaults */
		 vattr[0]  = PA_VARIABLE_SCOPE(pp);
		 vattr[1]  = PA_VARIABLE_CLASS(pp);
		 vattr[2]  = PA_VARIABLE_PERSISTENCE(pp);
		 vattr[3]  = PA_VARIABLE_CENTERING(pp);
		 vattr[4]  = PA_VARIABLE_ALLOCATION(pp);
		 vdims     = PA_VARIABLE_DIMS(pp);
		 iv        = PA_VARIABLE_INIT_VAL(pp);
		 ifn       = PA_VARIABLE_INIT_FUNC(pp);
		 vtype     = PA_VARIABLE_TYPE_S(pp);
		 nu        = PA_VARIABLE_UNIT_NUMER(pp);
		 du        = PA_VARIABLE_UNIT_DENOM(pp);
		 alist     = PA_VARIABLE_ATTR(pp);
		 break;
		 
	   case PA_INFO_INIT_VAL :
	        alist1 = SC_add_alist(alist1,
				      tag_name,
				      SC_POINTER_S,
				      SC_VA_ARG(void *));
		 break;

	    case PA_INFO_INIT_FNC :
	         alist1 = SC_add_alist(alist1,
				       tag_name,
				       SC_POINTER_S,
				       SC_VA_ARG(PFByte *));
		 break;

	    case PA_INFO_TYPE :
   	         alist1 = SC_add_alist(alist1,
				       tag_name,
				       SC_STRING_S,
				       SC_VA_ARG(char *));
		 break;

	    case PA_INFO_ALLOCATION :
	    case PA_INFO_CENTER :
	    case PA_INFO_CLASS :
	    case PA_INFO_PERSISTENCE :
	    case PA_INFO_SCOPE :
	         alist1 = _PA_process_att(tag, &SC_VA_VAR, alist1);
		 break;

	    case PA_INFO_DIMS :
	         vdims = _PA_process_dimension(&SC_VA_VAR);
		 alist1 = SC_add_alist(alist1,
				       tag_name,
				       PA_gs.dimension,
				       vdims);
		 break;

	    case PA_INFO_UNITS:
		 alist1 = _PA_process_units(&SC_VA_VAR, alist1);
		 break;

	    case PA_INFO_DATA_PTR:
		 data_ptr = SC_VA_ARG(void *);
		 break;
      
	    case PA_INFO_ATT_NAME :
	         name = SC_VA_ARG(char *);
		 att_alist = (pcons *) SC_hasharr_def_lookup(PA_gs.var_att_tab, name);
		 if (att_alist == NULL)
		    {PA_def_error = 1;
		     PA_WARN(PA_def_error,
			     "UNDEFINED ATTR - PA_PROCESS_DEF_VAR\n%s\t%s = %s",
			     vname, tag_name, name);}
		 else
		    alist2 = SC_append_alist(alist2, att_alist);

		 break;

	    case PA_INFO_DIM_NAME :
	         name = SC_VA_ARG(char *);
		 vdims = (PA_dimens *) SC_hasharr_def_lookup(PA_gs.var_dim_tab, name);
		 if (vdims == NULL)
		    {PA_def_error = 1;
		     PA_WARN(PA_def_error,
			     "UNDEFINED DIM - PA_PROCESS_DEF_VAR\n%s\t%s = %s",
			     vname, tag_name, name);}
		 else
		    alist2 = SC_add_alist(alist2,
					  PA_info_name(PA_INFO_DIMS),
					  PA_gs.dimension,
					  vdims);
		 break;

	    case PA_INFO_DOMAIN_NAME :
	         if (att_alist == NULL)
		    {PA_def_error = 1;
		     PA_WARN(PA_def_error,
			     "UNDEFINED DOMAIN - PA_PROCESS_DEF_VAR\n%s\t%s = %s",
			     vname, tag_name, name);};
		 break;

	    case PA_INFO_UNITS_NAME:
		 name = SC_VA_ARG(char *);
		 unit_alist = (pcons *) SC_hasharr_def_lookup(PA_gs.var_unit_tab, name);
		 if (unit_alist == NULL)
		    {PA_def_error = 1;
		     PA_WARN(PA_def_error,
			     "UNDEFINED UNIT - PA_PROCESS_DEF_VAR\n%s\t%s = %s",
			     vname, tag_name, name);}
		 else
		    alist2 = SC_append_alist(alist2, unit_alist);
		 break;

	    case LAST_TAG :
	         enough = 1;
		 break;
      
	    default:
		 {PFUserAttr fnc;

		  fnc = PA_GET_FUNCTION(PFUserAttr, "user_defined_attribute");
		  if (fnc == NULL)
		     {PA_def_error = 1;
		      PA_WARN(PA_def_error,
			      "UNKNOWN TAG - PA_PROCESS_DEF_VAR\n%s (%d)",
			      vname, tag);

/* since we don't recognise this tag, we don't know what should follow it
 * so don't process anymore
 */
		      enough = 1;}

		  else
		     alist2 = (*fnc)(tag, &SC_VA_VAR, alist2);};

		 break;};};

    SC_VA_RESTORE(lst);

/* alist2 is the lower precedence list - search it first */

/* now search thru the tags and find current values */
    for (i = 0; i < 5; i++)
        {pival = (int *) SC_assoc(alist2, attr_name[i]);
	 if (pival)
 	    vattr[i] = *pival;};

    SC_assoc_info_alt(alist2,
		      PA_INFO_DIMS_S,         &vdims,
		      PA_INFO_INIT_VAL_S,     &iv,
		      PA_INFO_INIT_FNC_S,     &ifn,
		      PA_INFO_TYPE_S,         &vtype,
		      PA_INFO_UNIT_NUMER_S,   &nu,
		      PA_INFO_UNIT_DENOM_S,   &du,
		      PA_INFO_APPL_ATTR_S,    &alist,
		      NULL);
    
/* alist1 is the higher precedence list - search it last */

    for (i = 0; i < 5; i++)
        {pival = (int *) SC_assoc(alist1, attr_name[i]);
	 if (pival)
	    vattr[i] = *pival;};

    SC_assoc_info_alt(alist1,
		      PA_INFO_DIMS_S,         &vdims,
		      PA_INFO_INIT_VAL_S,     &iv,
		      PA_INFO_INIT_FNC_S,     &ifn,
		      PA_INFO_TYPE_S,         &vtype,
		      PA_INFO_UNIT_NUMER_S,   &nu,
		      PA_INFO_UNIT_DENOM_S,   &du,
		      PA_INFO_APPL_ATTR_S,    &alist,
		      NULL);
    
    pp = _PA_mk_variable(vname, vdims, iv, ifn, vattr, vtype,
			 1.0, 1.0, nu, du, alist);
    
    PA_VARIABLE_DATA(pp) = data_ptr;
    
/* clean up all this mess */
    if (alist1)
       SC_free_alist(alist1, 3);

    if (alist2)
       SC_free_alist(alist2, 3);

    return(pp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_PROCESS_ATT - process the attribute tag */

pcons *_PA_process_att(int tag, va_list *list, pcons *alist)
    {int ival, *pival;
     char *tag_name, *value_name, *group;

     switch (tag)
        {case PA_INFO_ALLOCATION:
	      group = PA_gs.cpp_allocation;
	      break;

 	 case PA_INFO_CENTER:
	      group = PA_gs.cpp_center;
	      break;

         case PA_INFO_CLASS:
	      group = PA_gs.cpp_class;
	      break;

	 case PA_INFO_PERSISTENCE:
	      group = PA_gs.cpp_persistence;
	      break;

         case PA_INFO_SCOPE:
	      group = PA_gs.cpp_scope;
	      break;

	 default:
	      group = NULL;
	      break;};

    if (group != NULL)

/* pull off value */
       {SC_VA_SAVE(list);
	ival = SC_VA_ARG(int);
	SC_VA_RESTORE(list);

	tag_name   = PA_cpp_value_to_name(PA_gs.cpp_info, tag);
	value_name = PA_cpp_value_to_name(group, ival);

	if (value_name != NULL)
	   {pival = CMAKE(int);
	    *pival = ival;
	    alist = SC_add_alist(alist,
				 tag_name,
				 SC_INT_P_S,
				 pival);}
	else
	   {alist = NULL;
	    PA_def_error = 1;

	    PA_WARN(PA_def_error,
		    "ILLEGAL TAG PAIR IN _PA_PROCESS_ATT\n\t %s=%d",
		    tag_name, ival);};};

    return(alist);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_PROCESS_DIMENSION - process the dimension list */

PA_dimens *_PA_process_dimension(va_list *list)
   {int *mini, *maxi, meth;
    PA_dimens *vdims, *next, *prev;

    SC_VA_SAVE(list);

/* get the dimensions */
    vdims = NULL;
    while ((maxi = SC_VA_ARG(int *)) != LAST)
       {if (maxi == PA_gs.dul)
	  {mini = SC_VA_ARG(int *);
	   maxi = SC_VA_ARG(int *);
	   meth = *PA_gs.dul;}
	
       else if (maxi == PA_gs.don)
	  {mini = SC_VA_ARG(int *);
	   maxi = SC_VA_ARG(int *);
	   meth = *PA_gs.don;}
	
       else
	  {mini = &Zero_I;
	   meth = *PA_gs.don;};

	next = _PA_mk_dimens(mini, maxi, meth);
	if (vdims == NULL)
	   vdims = next;
	else
 	   prev->next = next;
	prev = next;};
    
    SC_VA_RESTORE(list);

    return(vdims);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_PROCESS_UNITS - from the variable argument list defined by list
 *                   - process arguments to build up the numerator and
 *                   - denominator of the units
 */

pcons *_PA_process_units(va_list *list, pcons *alist)
   {int dm, *pv;
    pcons *nu, *du, *next, *prev;

    SC_VA_SAVE(list);

/* get the units */
    nu = NULL;
    while (TRUE)
       {dm = SC_VA_ARG(int);
        if ((dm == PER) || (dm == UNITS))
           break;
        pv  = CMAKE(int);
        *pv = dm;
        next = SC_mk_pcons(SC_INT_P_S, pv, SC_PCONS_P_S, NULL);
        if (nu == NULL)
           nu = next;
        else
           prev->cdr = (void *) next;
        prev = next;};

    du = NULL;
    if (dm != UNITS)
       while ((dm = SC_VA_ARG(int)) != UNITS)
          {pv  = CMAKE(int);
           *pv = dm;
	   next = SC_mk_pcons(SC_INT_P_S, pv, SC_PCONS_P_S, NULL);
           if (du == NULL)
              du = next;
           else
              prev->cdr = (void *) next;
           prev = next;};

    SC_VA_RESTORE(list);

    alist = SC_add_alist(alist,
			 PA_info_name(PA_INFO_UNIT_NUMER),
			 SC_PCONS_P_S,
			 nu);
    alist = SC_add_alist(alist,
			 PA_info_name(PA_INFO_UNIT_DENOM),
			 SC_PCONS_P_S,
			 du);
    return(alist);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_INSTALL_TABLE - allocate hash table if it is NULL
 *                  - return a pointer to the hash table
 */

hasharr *PA_install_table(char *s, void *vr, char *type, hasharr *tab)
   {

    if (tab == NULL)
       tab = SC_make_hasharr(HSZLARGE, NODOC, SC_HA_NAME_KEY, 0);

    SC_hasharr_install(tab, s, vr, type, 3, -1);

    return(tab);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/



