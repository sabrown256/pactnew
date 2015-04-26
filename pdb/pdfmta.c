/*
 * PDFMTA.C - PDB I format methods
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"

/* PDBFile I Layout:
 *       
 *       Header (ASCII)
 *         Identifying token
 *         Numeric Data Standard/Alignment ID
 *         Structure Chart Address
 *         Symbol Table Address
 *         Reserved Space
 *       Data
 *       Structure Chart
 *       Symbol Table
 *       EOF
 */

enum e_PD_data_std
   {IEEE_32_64 = 1,           /* IEEE standard 32 bit float : 64 bit double */
    INTEL_X86,                                /* Intel 80x86 class machines */
    CRAY_64,                                         /* CRAY class machines */
    VAX_11,                                           /* VAX class machines */
    IEEE_32_96};              /* IEEE standard 32 bit float : 96 bit double */

typedef enum e_PD_data_std PD_data_std;

extern int
 _PD_rd_chrt_ii(PDBfile *file),
 _PD_rd_ext_ii(PDBfile *file),
 _PD_rd_symt_ii(PDBfile *file, char *acc, char *rej);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_OPEN_I - open an existing PDB I file
 *            - extract the symbol table and structure chart,
 *            - and return a pointer to the PDB file
 *            - if successful else NULL
 */

static int _PD_open_i(PDBfile *file, const char *mode)
   {char str[MAXLINE];
    char *token, *s;
    FILE *fp;
    PD_data_std istd;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

/* the second token is the machine type that wrote the file
 * set the file->std for machine type
 * for PD_open the file->std is always the PDBfile standard
 * alignment issues are not properly handled before PDB_SYSTEM_VERSION 3
 * but do the best that we can
 */
    token = SC_strtok(NULL, " ", s);
    if (token == NULL)
       PD_error("INCOMPLETE HEADER - PD_OPEN", PD_OPEN);

    istd = (PD_data_std) ((token != NULL) ? atoi(token) : 0);

    switch (istd)
       {case IEEE_32_64 :
	     file->std   = _PD_copy_standard(&PPC32_STD);
	     file->align = _PD_copy_alignment(&WORD2_ALIGNMENT);
	     break;
        case IEEE_32_96 :
	     file->std   = _PD_copy_standard(&M68X_STD);
	     file->align = _PD_copy_alignment(&WORD2_ALIGNMENT);
	     break;
        case INTEL_X86 :
	     file->std   = _PD_copy_standard(&I386_STD);
	     file->align = _PD_copy_alignment(&WORD2_ALIGNMENT);
	     break;
	case CRAY_64 :
	     file->std   = _PD_copy_standard(&CRAY_STD);
	     file->align = _PD_copy_alignment(&WORD8_ALIGNMENT);
	     break;
        case VAX_11 :
	     file->std   = _PD_copy_standard(&VAX_STD);
	     file->align = _PD_copy_alignment(&WORD4_ALIGNMENT);
	     break;
        default :
	     file->std   = _PD_copy_standard(&X86_64_STD);
	     file->align = _PD_copy_alignment(&GNU4_X86_64_ALIGNMENT);
	     break;};

/* to correctly handle the situation in which many PDBfiles are open
 * at the same time always try to latch on to the file->host_std
 * alignment issues are not properly handled before PDB_SYSTEM_VERSION 3
 * but do the best that we can
 */
    if (_PD_compare_std(file->host_std, file->std,
			file->host_align, file->align))
       {_PD_rl_standard(file->std);
	file->std   = _PD_copy_standard(file->host_std);
	_PD_rl_alignment(file->align);
	file->align = _PD_copy_alignment(file->host_align);};

    fp = pa->ofp;

/* record the current file position as the location of the symbol table
 * address and sequentially the chart address
 */
    file->headaddr = lio_tell(fp);
    if (file->headaddr == -1)
       PD_error("CAN'T FIND HEADER ADDRESS - PD_OPEN", PD_OPEN);

/* read the address of the symbol table and structure chart */
    if (_PD_rfgets(str, MAXLINE, fp) == NULL)
       PD_error("CAN'T READ SYMBOL TABLE ADDRESS - PD_OPEN", PD_OPEN);

    token = SC_strtok(str, "\001", s);
    if (token == NULL)
       PD_error("BAD STRUCTURE CHART ADDRESS - PD_OPEN", PD_OPEN);
    file->chrtaddr = SC_stol(token);

    token = SC_strtok(NULL, "\001", s);
    if (token == NULL)
       PD_error("BAD SYMBOL TABLE ADDRESS - PD_OPEN", PD_OPEN);
    file->symtaddr = SC_stol(token);

/* read the symbol table first so that the file pointer is positioned
 * to the "extra" information, then read the "extra's" to get the
 * alignment data, and finish with the structure chart which needs
 * the alignment data
 */

/* read the symbol table */
    if (lio_seek(fp, file->symtaddr, SEEK_SET))
       PD_error("FSEEK FAILED SYMBOL TABLE - PD_OPEN", PD_OPEN);
    if (!_PD_rd_symt_ii(file, "*", NULL))
       PD_error("CAN'T READ SYMBOL TABLE - PD_OPEN", PD_OPEN);

/* read the miscellaneous data */
    if (!_PD_rd_ext_ii(file))
       PD_error("CAN'T READ MISCELLANEOUS DATA - PD_OPEN", PD_OPEN);

/* initialize the pdb system defs and structure chart */
    _PD_init_chrt(file, TRUE);

/* read the structure chart */
    if (lio_seek(fp, file->chrtaddr, SEEK_SET))
       PD_error("FSEEK FAILED STRUCTURE CHART - PD_OPEN", PD_OPEN);

    if (!_PD_rd_chrt_ii(file))
       PD_error("CAN'T READ STRUCTURE CHART - PD_OPEN", PD_OPEN);

    _PD_read_attrtab(file);

/* position the file pointer to the location of the structure chart */
    if (lio_seek(fp, file->chrtaddr, SEEK_SET))
       PD_error("FSEEK FAILED CHART - PD_OPEN", PD_OPEN);

/* initialize the starting address for writing */
    _PD_SET_ADDRESS(file, file->chrtaddr);

/* if this file already contains a valid checksum, default file checksums to on */
    if (PD_verify(file) == TRUE)
       PD_activate_cksum(file, PD_MD5_FILE);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CREATE_I - return FALSE
 *              - no new format I files will be created
 */

static int _PD_create_i(PDBfile *file, const char *mode, int mst)
   {

    return(FALSE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_FLUSH_I - return FALSE
 *             - no format I files will be written
 */

static int _PD_flush_i(PDBfile *file)
   {

    return(FALSE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_WR_FMT_I - noop
 *              - no format I files will be written
 */

static int _PD_wr_fmt_i(PDBfile *file)
   {

    return(FALSE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_WRITE_META_I - noop
 *                  - no format I files will be written
 */

static int _PD_write_meta_i(PDBfile *file, FILE *out, int fh)
   {

    return(FALSE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_SET_FORMAT_I - set the methods for a format version I file */

int _PD_set_format_i(PDBfile *file)
   {

    _PD_set_format_ii(file);

    file->open   = _PD_open_i;
    file->create = _PD_create_i;
    file->flush  = _PD_flush_i;

    file->wr_meta = _PD_write_meta_i;
    file->wr_fmt  = _PD_wr_fmt_i;

    file->format_version = 1;
    file->use_itags      = TRUE;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

 
