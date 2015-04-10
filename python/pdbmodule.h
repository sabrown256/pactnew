/*
 * Source Version: 2.1
 * Software Release: LLNL-CODE-422942
 *
 * include cpyright.h
 */

#ifndef PY_pdb_H
#define PY_pdb_H

#include "py_int.h"

#include <patchlevel.h>

#if PY_VERSION_HEX < 0x02050000 && !defined(PY_SSIZE_T_MIN)
typedef int Py_ssize_t;
#define PY_SSIZE_T_MAX INT_MAX
#define PY_SSIZE_T_MIN INT_MIN
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

typedef struct s_PY_pdbdata *PY_pdbdatap;

typedef struct s_PY_pdbdata {
  PyObject_HEAD
  PP_HEADER(void)
} PY_pdbdata;

extern PyTypeObject PY_pdbdata_type;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/* PDBPDBDATA.C declarations */

extern PY_pdbdata
 *PY_pdbdata_newobj(PY_pdbdata *obj,
		    void *vr, char *type, long nitems,
		    dimdes *dims, defstr *dp, PP_file *fileinfo,
		    PY_defstr *dpobj, PyObject *parent);


extern void
  init_pdb(void);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif  /* !PY_pdb_H */

