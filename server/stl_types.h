#ifndef TWIN_STL_TYPES_H
#define TWIN_STL_TYPES_H

#include <Tw/datatypes.h>
#include "stl/string.h"

typedef View<tcolor> Colors;
typedef View<trune> Runes;
typedef View<tcell> Cells;

typedef Span<tcolor> ColorSpan;
typedef Span<trune> RuneSpan;
typedef Span<tcell> CellSpan;

typedef Vector<tcolor> ColorVec;
typedef Vector<trune> RuneVec;
typedef Vector<tcell> CellVec;

#endif /* TWIN_STL_TYPES_H */
