
#ifndef _TW_STATTYPES_H
#define _TW_STATTYPES_H

typedef union s_tsval {
    tany _;
#ifdef obj
    obj    o;
#endif
    struct {
	TW_CONST void *V;
	topaque L;
    } vec;
    struct {
	void *V;
	topaque L;
    } vecV;
} tsval;

typedef struct s_tsfield {
    udat hash;
    udat type;
    tsval val;
} *tsfield;

typedef struct s_tslist {
    udat N;
    udat flags;
    struct s_tsfield TSF[1];
} *tslist;

#endif /* _TW_STATTYPES_H */

