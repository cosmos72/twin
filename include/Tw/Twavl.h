/*
 *  Twavl.h  --  header for AVL trees functions
 *
 *  Copyright (C) 2001 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */

#ifndef _TW_AVL_H
#define _TW_AVL_H

#include <Tw/Twtypes.h>

typedef struct s_tavl * tavl;
typedef struct s_tavl TW_CONST * tavl_c;

typedef int (*tavl_compare)(tavl_c, tavl_c);

struct s_tavl {
    tavl AVLLeft, AVLRight, AVLParent;
    uldat AVLkey; /* a quick-compare key (hash). If equal, tavl_compare is used
		   * to check for actual matching of two tavl nodes */
    byte AVLHeight;
};

#define AVLHeightOf(L) (L ? L->AVLHeight : 0)

tavl AVLFind(tavl_c key, tavl base, tavl_compare cmp);

void AVLInsert(tavl key, tavl base, tavl_compare cmp, tavl *root);
void AVLRemove(tavl node, tavl_compare cmp, tavl *root);
void AVLRebalance(tavl base, tavl_compare cmp, tavl *root);



#endif /* _TW_AVL_H */

