
#ifndef TW_STATTYPES_H
#define TW_STATTYPES_H

typedef union s_tsval {
  tany _;
#ifdef Tobj
  Tobj o;
#endif
  struct {
    union {
      void *V;
      const void *CV;
    };
    topaque L;
  } vec;
} tsval;

typedef struct s_tsfield {
  udat label;
  udat type;
  tsval val;
} *tsfield;

typedef struct s_tslist {
  udat N;
  udat flags;
  struct s_tsfield TSF[1];
} *tslist;

#endif /* TW_STATTYPES_H */
