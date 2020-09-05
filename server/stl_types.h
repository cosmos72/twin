#ifndef TWIN_STL_TYPES_H
#define TWIN_STL_TYPES_H

#include <Tw/datatypes.h>
#include "stl/string.h"

typedef View<char> Chars;
typedef View<tcolor> Colors;
typedef View<trune> Runes;
typedef View<tcell> Cells;

typedef Span<char> CharSpan;
typedef Span<tcolor> ColorSpan;
typedef Span<trune> RuneSpan;
typedef Span<tcell> CellSpan;

typedef Array<char> CharArray;
typedef Array<tcolor> ColorArray;
typedef Array<trune> RuneArray;
typedef Array<tcell> CellArray;

#endif /* TWIN_STL_TYPES_H */
