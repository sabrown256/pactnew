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

typedef struct s_PP_pdbdataObject *PP_pdbdataObjectp;

typedef struct s_PP_pdbdataObject {
  PyObject_HEAD
  PP_HEADER(void)
} PP_pdbdataObject;

extern PyTypeObject PP_pdbdata_Type;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/* PDBPDBDATA.C declarations */

extern PP_pdbdataObject
 *PP_pdbdata_newobj(PP_pdbdataObject *obj,
		    void *vr, char *type, long nitems,
		    dimdes *dims, defstr *dp, PP_file *fileinfo,
		    PY_defstr *dpobj, PyObject *parent);


extern void
  init_pdb(void);

#ifdef __cplusplus
}
#endif
#endif  /* !PY_pdb_H */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
