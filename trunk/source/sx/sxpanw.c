/*
 * SXPANW.C - write out PANACEA data for SX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "sx_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_DISPLAY_PAN_OBJECT - display a PANACEA object */

object *_SXI_display_pan_object(SS_psides *si, object *obj)
   {int id, ni;
    char *name;
    syment *ep;
    PA_variable *pp;
    PA_package *pck;
    double conv;

    if (SX_PACKAGEP(obj))
       {pck = SS_GET(PA_package, obj);
        PRINT(stdout, "PACKAGE: %s\n", pck->name);
        PRINT(stdout, "  Time in package: %f\n", pck->time);
        PRINT(stdout, "  Memory required: %f\n", pck->space);
        PRINT(stdout, "  Number of switches: %d\n", pck->n_swtch);
        PRINT(stdout, "  Number of parameters: %d\n", pck->n_param);
        PRINT(stdout, "  Number of names: %d\n\n", pck->n_ascii);}

    else if (SX_PANVARP(obj))
       {pp = SS_GET(PA_variable, obj);
        if (pp->data != NULL)
           {ep = pp->desc;
	    id = SC_type_id(PD_entry_type(ep), FALSE);
            if (id == SC_DOUBLE_I)
               {ni   = PD_entry_number(ep);
                conv = pp->conv/pp->unit;
                PA_scale_array(pp->data, ni, conv);
                PD_write_entry(stdout, SX_vif, pp->name, pp->data, ep,
			       0, NULL);
                conv = 1.0/conv;
                PA_scale_array(pp->data, ni, conv);}
            else
               PD_write_entry(stdout, SX_vif, pp->name, pp->data, ep,
			      0, NULL);}
        else
           PRINT(stdout, "\n%s not connected to any data\n\n", pp->name);}

    else
       {name = SS_get_string(obj);
        pp   = PA_INQUIRE_VARIABLE(name);
        if (pp == NULL)
           SS_error_n(si, "VARIABLE NOT IN DATA BASE - _SXI_DISPLAY_PAN_OBJECT",
		      obj);

        else if (pp->data != NULL)
           {ep = pp->desc;
	    id = SC_type_id(PD_entry_type(ep), FALSE);
            if (id == SC_DOUBLE_I)
               {ni   = PD_entry_number(ep);
                conv = pp->conv/pp->unit;
                PA_scale_array(pp->data, ni, conv);
                PD_write_entry(stdout, SX_vif, pp->name, pp->data, ep,
			       0, NULL);
                conv   = 1.0/conv;
                PA_scale_array(pp->data, ni, conv);}
            else
               PD_write_entry(stdout, SX_vif, pp->name, pp->data, ep,
			      0, NULL);}
        else
           PRINT(stdout, "\n%s not connected to any data\n\n", pp->name);};

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_DESC_PAN_VARIABLE - display the description of a PANACEA variable */

static object *_SX_desc_pan_variable(object *obj)
   {PA_variable *pp;
    PA_dimens *pvd;
    syment *ep;

    pp = NULL;
    SS_args(obj,
            G_PANVAR, &pp,
            0);

    if (pp == NULL)
       SS_error("BAD PANACEA VARIABLE - _SX_DESC_PAN_VARIABLE", obj);

    else
 
/* print variable name with dimensions */
       {pvd = pp->dims;
	if (pvd == NULL)
	   PRINT(stdout, "\nVariable: %s\n", pp->name);
	else
	   {PRINT(stdout, "\nVariable: %s(", pp->name);
	    while (TRUE)
	       {PRINT(stdout, "%d:%d", *pvd->index_min, *pvd->index_max);
		pvd = pvd->next;
		if (pvd != NULL)
		   PRINT(stdout, ", ");
		else
		   break;};
	    PRINT(stdout, ")\n");};

/* print variable type */
	ep = pp->desc;
	PRINT(stdout, "Variable Type: %s\n", ep->type);

/* print variable attributes of one kind or another */
	PRINT(stdout, "Number of Elements: %ld\n", pp->size);
	PRINT(stdout, "External Unit Conversion Factor: %11.3e\n", pp->conv);
	PRINT(stdout, "Internal Unit Conversion Factor: %11.3e\n", pp->unit);
	PRINT(stdout, "Variable Attributes:\n");

	PRINT(stdout, "  Scope       - ");
	switch (pp->scope)
	   {case DEFN :
	         PRINT(stdout, "Definition\n");
	         break;
	    case RESTART :
	         PRINT(stdout, "Restart\n");
	         break;
	    case DMND :
	         PRINT(stdout, "Demand\n");
	         break;
	    case RUNTIME :
	         PRINT(stdout, "Run-time\n");
	         break;
	    case EDIT :
	         PRINT(stdout, "Edit\n");
	         break;};

	PRINT(stdout, "  Class       - ");
	switch (pp->option)
	   {case REQU :
	         PRINT(stdout, "Required\n");
	         break;
	    case OPTL :
	         PRINT(stdout, "Optional\n");
	         break;
	    case PSEUDO :
	         PRINT(stdout, "Pseudo\n");
	         break;};

	PRINT(stdout, "  Persistence - ");
	switch (pp->persistence)
 	   {case REL :
	         PRINT(stdout, "Release\n");
	         break;
 	    case KEEP :
	         PRINT(stdout, "Keep\n");
                 break;
            case CACHE_F :
	         PRINT(stdout, "Cache (Fixed)\n");
                 break;
            case CACHE_R :
	         PRINT(stdout, "Cache (Resizable)\n");
                 break;};

	PRINT(stdout, "  Allocation  - ");
	switch (pp->allocation)
	   {case STATIC :
	         PRINT(stdout, "Static\n");
	         break;
	    case DYNAMIC :
	         PRINT(stdout, "Dynamic\n");
	         break;};

	PRINT(stdout, "  Centering   - ");
	switch (pp->centering)
	   {case Z_CENT :
	         PRINT(stdout, "Zone Centered\n");
	         break;
	    case N_CENT :
	         PRINT(stdout, "Node Centered\n");
	         break;
	    case F_CENT :
	         PRINT(stdout, "Face Centered\n");
	         break;
	    case E_CENT :
	         PRINT(stdout, "Edge Centered\n");
	         break;
	    case U_CENT :
	         PRINT(stdout, "Uncentered\n");
	         break;};

	if (pp->data != NULL)
	   {SC_address addr;

	    addr.memaddr = pp->data;
	    PRINT(stdout, "Data Location: %lx\n", addr.diskaddr);};};

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_DESC_PAN_PACKAGE - display the description of a PANACEA package */

static object *_SX_desc_pan_package(object *obj)
   {PA_package *pck;

    pck = NULL;
    SS_args(obj,
            G_PACKAGE, &pck,
            0);

    if (pck == NULL)
       SS_error("BAD PANACEA PACKAGE - _SX_DESC_PAN_PACKAGE", obj);

    else

/* print package name with dimensions */
       {PRINT(stdout, "\nName: %s\n", pck->name);

	PRINT(stdout, "Time: %g\n", pck->p_t);
	PRINT(stdout, "Time Step: %g\n", pck->p_dt);
	PRINT(stdout, "Cycle: %d\n", pck->p_cycle);

	PRINT(stdout, "CPU Time Used: %g\n", pck->time);
	PRINT(stdout, "Memory Used: %g (kbytes)\n", pck->space);
	PRINT(stdout, "Time Step Vote: %g\n", pck->dt);
	PRINT(stdout, "Time Step Controlling Zone: %d\n", pck->dt_zone);

	PRINT(stdout, "Number of switches: %d\n", pck->n_swtch);
	PRINT(stdout, "Number of parameters: %d\n", pck->n_param);
	PRINT(stdout, "Number of names: %d\n\n", pck->n_ascii);

	if (pck->db_file != NULL)
	   PRINT(stdout, "Dictionary File: %s\n", pck->db_file);

	PRINT(stdout, "\n");};

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_DESC_PAN - display the description of a PANACEA entity */

object *_SXI_desc_pan(SS_psides *si, object *obj)
   {char *s;
    haelem *hp;

    if (SX_PACKAGEP(obj))
       _SX_desc_pan_package(obj);

    else if (SX_PANVARP(obj))
       _SX_desc_pan_variable(obj);

    else
       {s  = SS_get_string(obj);
        hp = SC_hasharr_lookup(PA_variable_tab, s);
        if (hp == NULL)
           SS_error_n(si, "BAD ENTITY - _SXI_DESC_PAN", obj);

	else if (hp->type[3] == 'p')
           _SX_desc_pan_package(obj);

        else
           _SX_desc_pan_variable(obj);};

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
