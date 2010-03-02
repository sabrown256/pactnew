/*
 * TRLLF.C - Lanl Link Format spoke
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

/* IMPLEMENTATION NOTES:
 * (1) All LLF files are written with 8 bytes per word. 
 *
 */

#include "cpyright.h"
 
#include "trllf.h"

char
 *LLFILE_S = "LLfile";

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* LLF_PARSE_MCD - parse the Mesh Cell Data block headers */
 
static mcd_header *LLF_parse_mcd(PDBfile *file, fdir_header filehdr,
				 fdir_entry *fileentry)
   {int i;
    size_t ni;
    mcd_header* mcdhdr, mhtemp;

    mcdhdr = FMAKE_N(mcd_header, filehdr.n_record, "LLF_PARSE_MCD:mcdhdr");

    for (i = 0; i < filehdr.n_record; i++)
        {if (lio_seek(file->stream, (8L * (long)fileentry[i].address), SEEK_SET))
	    PD_error("FSEEK FAILED TO FIND A MCD HEADER - LLF_PARSE_MCD", PD_OPEN);

	 ni = lio_read(&mcdhdr[i], sizeof(mcd_header), 1, file->stream);
	 if (ni != sizeof(mcd_header))
	    PD_error("CAN'T READ A MCD HEADER - LLF_PARSE_MCD", PD_OPEN);

	 mhtemp = mcdhdr[i];
	 _PD_conv_in(file, &mcdhdr[i], &mhtemp, "mcd_header", 1);};
  
   return(mcdhdr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* LLF_PARSE_VLD - parse the Variable Length Directory headers */
 
static vld_header *LLF_parse_vld(PDBfile *file, fdir_header filehdr,
				 mcd_header *mcdhdr)
   {int i;
    size_t ni;
    vld_header* vldhdr, vhtemp;
 
    vldhdr = FMAKE_N(vld_header, filehdr.n_record, "LLF_PARSE_VLD:vldhdr");

    for (i = 0; i < filehdr.n_record; i++)
        {if (lio_seek(file->stream, (8L * (long)mcdhdr[i].vld_addr), SEEK_SET))
	    PD_error("FSEEK FAILED TO FIND A VLD HEADER - LLF_OPEN", PD_OPEN);

	 ni = lio_read(&vldhdr[i], sizeof(vld_header), 1, file->stream);
	 if (ni != sizeof(vld_header))
	    PD_error("CAN'T READ A VLD HEADER - LLF_PARSE_VFD", PD_OPEN);

	 vhtemp = vldhdr[i];
	 _PD_conv_in(file, &vldhdr[i], &vhtemp, "vld_header", 1);};

   return(vldhdr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _LLF_FILEP - return TRUE iff TYPE is an LLF file */

static int _LLF_filep(char *type)
   {int rv;

    rv = (strcmp(type, LLFILE_S) == 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _LLF_CLOSE - close an LLF file */

static int _LLF_close(PDBfile *file)
   {FILE *fp;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    switch (SETJMP(pa->close_err))
       {case ABORT :
	     return(FALSE);
        case ERR_FREE :
	     return(TRUE);
        default :
	     memset(pa->err, 0, MAXLINE);
	     break;};

    fp = file->stream;
    if (lio_close(fp) != 0)
       PD_error("CAN'T CLOSE FILE - LLF_CLOSE", PD_CLOSE);

/* free the space */
    _PD_rl_pdb(file);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _LLF_OPEN - open an existing LLFile */

static PDBfile *_LLF_open(tr_layer *tr, SC_udl *pu, char *name, char *mode)
   {int i;
    size_t ni;
    unsigned char signature[5] = "link";
    FILE *fp;
    fdir_header filehdr, fhtemp;
    fdir_entry *fileentry, fetemp;
    mcd_header *mcdhdr;
    vld_header *vldhdr;
    defstr *dp;
    PDBfile *file;

    fp   = pu->stream;
    file = NULL;

    if (PD_buffer_size != -1)
       if (setvbuf(fp, NULL, _IOFBF, (size_t) PD_buffer_size*BUFSIZ))
          PD_error("CAN'T SET FILE BUFFER - _LLF_OPEN", PD_OPEN);

/* attempt to read an ASCII header: 4 words
 *                 +--------+
 *      signature: |link    | => "link    "  (also accept "linkieee")
 *      # words:   |01234567| => words in dir (first MCD header block)
 *      # records: |01234567| => number of dumps or records
 *      last addr: |01234567| => last word address in the file (end marker)
 *                 +--------+
 */
    if (lio_seek(fp, 0L, SEEK_SET))
       PD_error("FSEEK FAILED TO FIND ORIGIN - _LLF_OPEN", PD_OPEN);

    if (lio_read(&filehdr, CRAY_BYTES_WORD, 4, fp) != 4)
       PD_error("CAN'T READ THE FILE HEADER - _LLF_OPEN", PD_OPEN);

/* GOTCHA: does the "ieee" *mean* anything? */

/* ignore all but the first 4 chars of the file signature */
    filehdr.name[4] = '\0';

/* start creating a PDB file representation */
    if (strncmp((char *) filehdr.name, (char *) signature, 4) == 0)
       {file = _PD_mk_pdb(pu, NULL, BINARY_MODE_RPLUS, TRUE, NULL, tr);
	if (file == NULL)
	   PD_error("CAN'T ALLOCATE LFFILE - _LLF_OPEN", PD_OPEN);

	file->default_offset = 1;
	file->major_order    = COLUMN_MAJOR_ORDER;
	file->type           = SC_strsavef(LLFILE_S, "char*:_LLF_OPEN:type");

	if (*mode == 'a')
	   file->mode = PD_APPEND;
	else
	   file->mode = PD_OPEN;

	_PD_set_standard(file, &CRAY_STD, &UNICOS_ALIGNMENT);

	_PD_init_chrt(file);


/* define to both file and host charts a 64-bit signed integer type */
	dp = _PD_defstr(file, FALSE, "int64_t", INT_KIND,
			NULL, 8, 8, file->std->int_order, TRUE,
			NULL, NULL, FALSE, FALSE);
	dp = _PD_defstr(file, TRUE, "int64_t", INT_KIND,
			NULL, 8, 8, file->host_std->int_order, TRUE,
			NULL, NULL, FALSE, FALSE);

/* define the structs from the LLF header */
	PD_DEFINE_FDIR_HEADER; 
	PD_DEFINE_FDIR_ENTRY; 
	PD_DEFINE_MCD_HEADER; 
	PD_DEFINE_VLD_HEADER; 
	PD_DEFINE_VLD_ENTRY; 
	PD_DEFINE_VLB_HEADER; 
	PD_DEFINE_VLB_DIM; 
	PD_DEFINE_VLB_DESC; 
	PD_DEFINE_VLB_PRB_DESC; 
	PD_DEFINE_VLB_VAR_DESC; 
                    
/* convert the file header */
	fhtemp = filehdr;
	_PD_conv_in(file, &filehdr, &fhtemp, "fdir_header", 1);

	fileentry = FMAKE_N(fdir_entry, filehdr.n_record, "_LLF_OPEN:fileentry");

/* read the file directory */
	ni = lio_read(&fileentry[0], sizeof(fdir_entry), filehdr.n_record, file->stream);
	if (ni != sizeof(fdir_entry) * filehdr.n_record)
	   PD_error("CAN'T READ THE FILE HEADER - _LLF_OPEN", PD_OPEN);

/* convert the file dir entries */
	for (i = 0; i < filehdr.n_record ; i++)
	    {fetemp = fileentry[i];
	     _PD_conv_in(file, &fileentry[i], &fetemp, "fdir_entry", 1);};

/* read the MCDs */
	mcdhdr = LLF_parse_mcd(file, filehdr, fileentry);

/* read the VLDs */
	vldhdr = LLF_parse_vld(file, filehdr, mcdhdr);

/* read the VLBs */
	SFREE(fileentry);
	SFREE(mcdhdr);
	SFREE(vldhdr);};

    return(file);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PD_REGISTER_LLF - install the LLF extensions to PDBLib */
 
void PD_register_llf(void)
   {int n;

    n = PD_REGISTER(LLFILE_S, "llf", _LLF_filep,
		    NULL, _LLF_open, _LLF_close, NULL, NULL);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

