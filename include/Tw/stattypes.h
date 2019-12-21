
#ifndef _TW_STATTYPES_H
#define _TW_STATTYPES_H

typedef union s_tsval {
  tany _;
#ifdef obj
  obj o;
#endif
  struct {
    union {
      void *V;
      TW_CONST void *CV;
    };
    topaque L;
  } vec;
} tsval;

typedef struct s_tsfield {
  udat hash;
  udat type;
  tsval val;
} * tsfield;

typedef struct s_tslist {
  udat N;
  udat flags;
  struct s_tsfield TSF[1];
} * tslist;

#endif /* _TW_STATTYPES_H */
