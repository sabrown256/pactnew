/*
 * Source Version: 2.1
 * Software Release: LLNL-CODE-422942
 *
 * include cpyright.h
 */

#ifndef PY_pgs_H
#define PY_pgs_H

#include "py_int.h"

#define PY_ARRAY_UNIQUE_SYMBOL PP_
#ifdef HAVE_PY_NUMERIC
# include <Numeric/arrayobject.h>
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
  init_pgs(void);

#ifdef __cplusplus
}
#endif
#endif  /* !PY_pgs_H */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
