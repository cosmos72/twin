#ifndef TWIN_STL_TYPES_H
#define TWIN_STL_TYPES_H

#include <Tw/datatypes.h>
#include "stl/string.h"

typedef View<tcolor> TColors;
typedef View<trune> TRunes;
typedef View<tcell> TCells;

typedef Span<tcolor> TColorSpan;
typedef Span<trune> TRuneSpan;
typedef Span<tcell> TCellSpan;

typedef Vector<tcolor> TColorVec;
typedef Vector<trune> TRuneVec;
typedef Vector<tcell> TCellVec;

#endif /* TWIN_STL_TYPES_H */
