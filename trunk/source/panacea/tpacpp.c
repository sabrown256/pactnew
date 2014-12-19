/*
 * TPACPP.C - test PACPP.C functions
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "panacea_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int c, char **v)
   {int itype, itype1, i;
    int iunits[11];
    char *type, *type1, *name;
    char **s_index;

/* arrays used to automate some of the testing */
    char *type_slist[] = {
        "SC_CHAR_S",    "SC_SHORT_S",   "SC_INT_S",     "SC_LONG_S",
        "SC_FLOAT_S",   "SC_DOUBLE_S",  "SC_REAL_S",    "SC_STRING_S",
        "SC_POINTER_S", "SC_VOID_S",    "SC_SHORT_P_S",
	"SC_INT_P_S",   "SC_LONG_P_S",  "SC_FLOAT_P_S","SC_DOUBLE_P_S",
        "SC_STRUCT_S",  "SC_UNKNOWN_S",
        NULL };

    int type_ilist[] = {
        SC_CHAR_I,    SC_SHORT_I,   SC_INT_I,     SC_LONG_I,
        SC_FLOAT_I,   SC_DOUBLE_I,  SC_REAL_I,    SC_STRING_I,
        SC_POINTER_I, SC_VOID_I,    SC_SHORT_P_I,
	SC_INT_P_I,   SC_LONG_P_I,  SC_FLOAT_P_I, SC_DOUBLE_P_I,
        SC_STRUCT_I,  SC_UNKNOWN_I,
        -100 };

    _PA_internal_init();
    PA_defunc();
    PA_init_strings();
    PA_cpp_init();
    PA_cpp_default();

    for (s_index = type_slist; *s_index != NULL; s_index++)
        {name  = (char *) PA_cpp_name_to_value(*s_index);
	 itype = *(int *) PA_cpp_name_to_value(name);
	 printf(" %s\t%s\t\%d\n", *s_index, name, itype);};

    type = (char *) PA_cpp_name_to_value("SC_STRING_S");
    printf(" SC_STRING_S  - %s\n", type);

    itype = *(int *) PA_cpp_name_to_value("SC_STRING_I");
    printf(" SC_STRING_I  - %d\n", itype);

    itype  = *(int *) PA_cpp_name_to_value(SC_DOUBLE_S);
    itype1 = *(int *) PA_cpp_name_to_value("SC_DOUBLE_I");
    type   = (char *) PA_cpp_name_to_value("SC_DOUBLE_S");
    type1  = (char *) PA_cpp_value_to_name(PA_gs.cpp_type, 1);
    printf("PA_cpp_name_to_value - %d %d %s %s\n",
	   itype, itype1, type, type1);

    itype = SC_type_id(SC_DOUBLE_S, FALSE);
    printf("double is %d\n", itype);
    type = SC_type_name(6);
    printf("6 is %s\n", type);

    for (i = 0; type_ilist[i] != -100; i++)
        {name = PA_cpp_value_to_name(PA_gs.cpp_type, type_ilist[i]);
	 printf("  %d of %s is a %s\n",
		type_ilist[i], PA_gs.cpp_type, name);};
  
    iunits[0] = RAD;
    iunits[1] = STER;
    iunits[2] = MOLE;
    iunits[3] = Q;
    iunits[4] = CM;
    iunits[5] = SEC;
    iunits[6] = G;
    iunits[7] = EV;
    iunits[8] = K;
    iunits[9] = ERG;
    iunits[10] = CC;

    printf("\nUnits\n");
    for (i = 0; i < 11; i++)
        {name = PA_cpp_value_to_name(PA_gs.cpp_units, iunits[i]);
	 printf("  %d of %s is a %s\n",
		iunits[i], PA_gs.cpp_units, name);};

    name = PA_cpp_value_to_name(PA_gs.cpp_info, PA_INFO_SCOPE);
    printf("  %d of PA_gs.cpp_info is a %s\n",
	   PA_INFO_SCOPE, name);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
