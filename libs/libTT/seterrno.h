
#ifndef _TT_SETERRNO_H
#define _TT_SETERRNO_H

/* error handling */
TT_INLINE void SetErrno(ttuint E, ttuint S) {
    s_tt_errno *L = GetErrnoLocation();
    L->E = E;
    L->S = S;
}

#endif /* _TT_SETERRNO_H */

