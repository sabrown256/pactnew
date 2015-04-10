/*
 * FASCB.C - FORTRAN interface routines for SCORE
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "fpact.h"

/*--------------------------------------------------------------------------*/

/*                           FORTRAN API ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* SC_SET_HASH_DUMP - set up a hash dump for a FORTRAN application
 *                  - some other routine will have to make this call
 *                  - to supply the hash array
 */

int SC_set_hash_dump(hasharr *ha, int (*fun)(haelem **v, int n))
   {int i, sz, n_entries;
    haelem *np, **tb, **entries;

    SC_free_hash_dump();

/* if no function is supplied dump and sort by name alphabetically */
    if (fun == NULL)
       {_SC.hash_entries = SC_hasharr_dump(ha, NULL, NULL, FALSE);
        _SC.ne = SC_MEM_GET_N(char *, _SC.hash_entries) - 1;

	if (_SC.hash_entries != NULL)
	   {for (i = 0; i < _SC.ne; i++)
	        _SC.hash_entries[i] = CSTRSAVE(_SC.hash_entries[i]);};}

    else
       {entries = CMAKE_N(haelem *, ha->ne);
        if (entries == NULL)
           return(-1);

/* fill in the list of pointers to names in the hash array */
        sz = ha->size;
        tb = ha->table;
        n_entries = 0;
        for (i = 0; i < sz; i++)
            for (np = tb[i]; np != NULL; np = np->next)
                entries[n_entries++] = np;

/* sort the array of haelem's */
        n_entries = (*fun)(entries, n_entries);

        _SC.hash_entries = CMAKE_N(char *, n_entries);
        if (_SC.hash_entries == NULL)
           return(-1);

        for (i = 0; i < n_entries; i++)
            _SC.hash_entries[i] = CSTRSAVE(entries[i]->name);

        _SC.ne = n_entries;

        CFREE(entries);};

    return(_SC.ne);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FREE_HASH_DUMP - free space allocated by call to SC_set_hash_dump */

int SC_free_hash_dump(void)
   {int i;

    if (_SC.hash_entries != NULL)
       {for (i = 0; i < _SC.ne; i++)
            {CFREE(_SC.hash_entries[i]);};
        CFREE(_SC.hash_entries);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_ENTRY - return the name associated with the nth entry in the
 *              - sorted list of names in the last sorted hash array done
 *              - by SC_set_hash_dump
 */

char *SC_get_entry(int n)
   {char *rv;

    if ((n < 0) || (_SC.ne <= n))
       rv = NULL;
    else
       rv = _SC.hash_entries[n];

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCMKHT - make a hash array */

FIXNUM FF_ID(scmkht, SCMKHT)(FIXNUM *ssz, FIXNUM *sdoc, FIXNUM *sflgs)
   {int sz, doc, flags;
    hasharr *ha;
    FIXNUM rv;

    sz    = *ssz;
    doc   = *sdoc;
    flags = *sflgs;

    ha = SC_make_hasharr(sz, doc, SC_HA_NAME_KEY, flags);
    if (ha == NULL)
       rv = 0;
    else
       rv = SC_ADD_POINTER(ha);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCRLHT - release a hash array */

FIXNUM FF_ID(scrlht, SCRLHT)(FIXNUM *sha)
   {hasharr *ha;
    FIXNUM rv;

    ha = SC_DEL_POINTER(hasharr, *sha);

    SC_free_hasharr(ha, NULL, NULL);

    *sha = 0;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHCLR - clear a hash array */

FIXNUM FF_ID(schclr, SCHCLR)(FIXNUM *sha)
   {hasharr *ha;
    FIXNUM rv;

    ha = SC_GET_POINTER(hasharr, *sha);

    SC_hasharr_clear(ha, NULL, NULL);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHINS - install an object in a hash array
 *        - if cp is TRUE make a copy of the object pointed to by pointer
 *        - in this case the type had better be right because it is the
 *        - only info install has to go on
 */

FIXNUM FF_ID(schins, SCHINS)(FIXNUM *sha, FIXNUM *sncn, char *pname,
			     void *ptr, FIXNUM *snct, char *ptype,
			     FIXNUM *scp)
   {int cp;
    char name[MAXLINE], type[MAXLINE];
    char *t;
    hasharr *ha;
    haelem *hp;
    FIXNUM rv;

    cp = *scp;
    ha = SC_GET_POINTER(hasharr, *sha);

    SC_FORTRAN_STR_C(name, pname, *sncn);
    SC_FORTRAN_STR_C(type, ptype, *snct);

    if (cp)
       {long n;
	void *s;

        n = SIZEOF(type);
        s = CMAKE_N(char, n);
        memcpy(s, ptr, n);

        ptr = s;};

    t  = CSTRSAVE(type),
    hp = SC_hasharr_install(ha, name, ptr, t, 2, -1);

    rv = _SC_to_number(hp);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHLKP - look up an object in a hash array
 *        - return the object thru P
 */

FIXNUM FF_ID(schlkp, SCHLKP)(FIXNUM *sha, void *p,
			     FIXNUM *sncn, char *pname)
   {int n;
    FIXNUM rv;
    char name[MAXLINE];
    haelem *hp;
    hasharr *ha;

    ha = SC_GET_POINTER(hasharr, *sha);

    SC_FORTRAN_STR_C(name, pname, *sncn);

    hp = SC_hasharr_lookup(ha, name);
    if (hp == NULL)
       rv = FALSE;

    else
       {n = SIZEOF(hp->type);
	memcpy(p, hp->def, n);

	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHLKU - look up an object in a hash array
 *        - return a pointer to the object thru P
 */

FIXNUM FF_ID(schlku, SCHLKU)(FIXNUM *sha, void **p,
			     FIXNUM *sncn, char *pname)
   {FIXNUM rv;
    char name[MAXLINE];
    haelem *hp;
    hasharr *ha;

    ha = SC_GET_POINTER(hasharr, *sha);

    SC_FORTRAN_STR_C(name, pname, *sncn);

    hp = SC_hasharr_lookup(ha, name);
    if (hp == NULL)
       rv = FALSE;
    else
       {*p = (void *) hp->def;
	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHREM - remove an object from a hash array */

FIXNUM FF_ID(schrem, SCHREM)(FIXNUM *sha, FIXNUM *sncn, char *pname)
   {FIXNUM rv;
    char name[MAXLINE];
    hasharr *ha;

    ha = SC_GET_POINTER(hasharr, *sha);

    SC_FORTRAN_STR_C(name, pname, *sncn);

    rv = SC_hasharr_remove(ha, name);

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                             THREAD ROUTINES                              */

/*--------------------------------------------------------------------------*/

FIXNUM FF_ID(scinth, SCINTH)(FIXNUM *snt, PFTid tid)
   {int nt;
    FIXNUM rv;

    nt = *snt;

    SC_init_threads(nt, tid);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

