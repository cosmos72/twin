/*
 *  avl.c  --  implementation of AVL balanced trees functions
 *
 *  Copyright (C) 2001 by Massimiliano Ghilardi
 *
 * AVL (Adelson-Velskii and Landis) tree to speed up listener search
 * from O(n) to O(log n) based on code from linux kernel mm subsystem,
 * written by Bruno Haible <haible@ma2s2.mathematik.uni-karlsruhe.de>.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 */

#include <Tw/Tw.h>
#include <Tw/Twavl.h>


/*
 * Searching a node in a linear list is horribly slow.
 * Use an AVL (Adelson-Velskii and Landis) tree to speed up this search
 * from O(n) to O(log n), where n is the number of nodes. n is typically zero,
 * but may reach a few thousands if clients use listeners heavily.
 * Written by Bruno Haible <haible@ma2s2.mathematik.uni-karlsruhe.de> for the
 * linux kernel mm subsystem, then adapted to libTw by Massimiliano Ghilardi.
 */

tavl AVLFind(tavl_c key, tavl base, tavl_compare cmp) {
    uldat kkey = key->AVLkey;
    int _cmp;
    
    while (base) {
	if (kkey < base->AVLkey)
	    base = base->AVLLeft;
	else if (kkey > base->AVLkey)
	    base = base->AVLRight;
	else {
	    _cmp = cmp(key, base);
	    if (_cmp < 0)
		base = base->AVLLeft;
	    else if (_cmp > 0)
		base = base->AVLRight;
	    else
		break;
	}
    }
    return base;
}

TW_INLINE void AVL_Insert(tavl node, tavl Parent, tavl old, tavl *root) {
    if (Parent) {
	if (Parent->AVLLeft == old)
	    Parent->AVLLeft = node;
	else
	    Parent->AVLRight = node;
    } else if (root)
	*root = node;
    if (node) node->AVLParent = Parent;
}

#ifndef AVLHeightOf
# define AVLHeightOf(node) (node ? node->AVLHeight : 0)
#endif

void AVLRebalance(tavl P /*base*/, tavl_compare cmp, tavl *root) {
    tavl L, R, PP;
    byte HL, HR;
    
    while (P) {
	L = P->AVLLeft;
	HL = AVLHeightOf(L);
	R = P->AVLRight;
	HR = AVLHeightOf(R);
	
	if (HL > HR + 1) {
	    /*                                                      */
	    /*                           (P)                        */
	    /*                          /   \                       */
	    /*                    (L)n+2     n(R)                   */
	    /*                                                      */
	    tavl LL = L->AVLLeft, LR = L->AVLRight;
	    byte HLR = AVLHeightOf(LR);
	    if (AVLHeightOf(LL) >= HLR) {
		/*                                                        */
		/*               (P)                (L)n+2|n+3            */
		/*              /   \                  /    \             */
		/*        (L)n+2     n(R)   -->  (LL)n+1  n+1|n+2(P)      */
		/*           / \                           /    \         */
		/*     (LL)n+1 n|n+1(LR)             (LR)n|n+1   n(R)     */
		/*                                                        */
		if ((P->AVLLeft = LR)) LR->AVLParent = P;
		L->AVLRight = P;
		AVL_Insert(L, (PP = P->AVLParent), P, root);
		P->AVLParent = L;
		L->AVLHeight = 1 + (P->AVLHeight = 1 + HLR);
		P = PP;
	    } else {
		/*                                                        */
		/*               (P)                 (LR)n+2              */
		/*              /   \                 /     \             */
		/*        (L)n+2     n(R)   -->  (L)n+1     n+1(P)        */
		/*           / \                    / \     / \           */
		/*      (LL)n  n+1(LR)         (LL)n   X   Y   n(R)       */
		/*             / \                                        */
		/*            X   Y                                       */
		/*                                                        */
		if ((P->AVLLeft = LR->AVLRight)) LR->AVLRight->AVLParent = P;
		LR->AVLRight = P;
		AVL_Insert(LR, (PP = P->AVLParent), P, root);
		P->AVLParent = LR;
		if ((L->AVLRight = LR->AVLLeft)) LR->AVLLeft->AVLParent = L;
		LR->AVLLeft = L; L->AVLParent = LR;
		L->AVLHeight = P->AVLHeight = HLR;
		LR->AVLHeight = HL;
		P = PP;
	    }
	} else if (HL + 1 < HR) {
	    /* similar to the above, just swap Left <--> Right sides */
	    tavl RR = R->AVLRight, RL = R->AVLLeft;
	    byte HRL = AVLHeightOf(RL);
	    if (AVLHeightOf(RR) >= HRL) {
		if ((P->AVLRight = RL)) RL->AVLParent = P;
		R->AVLLeft = P;
		AVL_Insert(R, (PP = P->AVLParent), P, root);
		P->AVLParent = R;
		R->AVLHeight = 1 + (P->AVLHeight = 1 + HRL);
		P = PP;
	    } else {
		if ((P->AVLRight = RL->AVLLeft)) RL->AVLLeft->AVLParent = P;
		RL->AVLLeft = P;
		AVL_Insert(RL, (PP = P->AVLParent), P, root);
		P->AVLParent = RL;
		if ((R->AVLLeft = RL->AVLRight)) RL->AVLRight->AVLParent = R;
		RL->AVLRight = R; R->AVLParent = RL;
		R->AVLHeight = P->AVLHeight = HRL;
		RL->AVLHeight = HR;
		P = PP;
	    }
	} else {
	    HL = 1 + (HL > HR ? HL : HR);
	    if (P->AVLHeight != HL) {
		P->AVLHeight = HL;
		P = P->AVLParent;
	    } else
		break;
	}
    }
}

void AVLInsert(tavl L /*key*/, tavl P /*base*/, tavl_compare cmp, tavl *root) {
    uldat Lkey = L->AVLkey;
    uldat Ckey;
    tavl C = P;
    
    while (C) {
	P = C;
	Ckey = C->AVLkey;
	if (Lkey < Ckey || (Lkey == Ckey && cmp(L, P) < 0))
	    C = P->AVLLeft;
	else /* key >= Ckey */
	    C = P->AVLRight;
    }
    /* enter AVLRebalance() in a known state */
    L->AVLLeft = L->AVLRight = NULL;
    L->AVLParent = P;
    L->AVLHeight = 1;
    if (P) {
	if (Lkey < Ckey || (Lkey == Ckey && cmp(L, P) < 0))
	    P->AVLLeft = L;
	else
	    P->AVLRight = L;
	AVLRebalance(P, cmp, root);
    } else if (root)
	*root = L;
}

/* FIXME: this is not completely tested */
void AVLRemove(tavl P /*node*/, tavl_compare cmp, tavl *root) {
    tavl L, R, LC, RC, Y;
    byte HL, HR;
    
    L = LC = P->AVLLeft;
    R = RC = P->AVLRight;
    if (L && R) {
	while (LC && RC) {
	    L = LC; LC = L->AVLRight;
	    R = RC; RC = R->AVLLeft;
	}
	if (!LC) {
	    /*                                                        */
	    /*               P                      L                 */
	    /*             /   \                  /   \               */
	    /*            X     R                X     R              */
	    /*             .                      .                   */
	    /*              .         -->          .                  */
	    /*               L                      LC                */
	    /*              /                                         */
	    /*            LC                                          */
	    /*                                                        */
	    LC = L->AVLLeft;
	    if ((Y = L->AVLParent) == P)
		Y->AVLLeft = LC;
	    else
		Y->AVLRight = LC;
	    if (LC) LC->AVLParent = Y;
	    AVL_Insert(L, P->AVLParent, P, root);
	    if ((L->AVLRight = P->AVLRight)) L->AVLRight->AVLParent = L;
	    if ((L->AVLLeft = P->AVLLeft)) L->AVLLeft->AVLParent = L;
	    HL = AVLHeightOf(L->AVLLeft);
	    HR = AVLHeightOf(L->AVLRight);
	    L->AVLHeight = 1 + (HL > HR ? HL : HR);
	} else {
	    /* similar to the above, just swap Left <--> Right sides */
	    RC = R->AVLRight;
	    Y = R->AVLParent;
	    if ((Y = L->AVLParent) == P)
		Y->AVLRight = RC;
	    else
		Y->AVLLeft = RC;
	    if (RC) RC->AVLParent = Y;
	    AVL_Insert(R, P->AVLParent, P, root);
	    if ((R->AVLLeft = P->AVLLeft)) R->AVLLeft->AVLParent = R;
	    if ((R->AVLRight = P->AVLRight)) R->AVLRight->AVLParent = R;
	    HR = AVLHeightOf(R->AVLRight);
	    HL = AVLHeightOf(R->AVLLeft);
	    R->AVLHeight = 1 + (HR > HL ? HR : HL);
	}
    } else
	AVL_Insert(R ? R : L, Y = P->AVLParent, P, root);

    AVLRebalance(Y, cmp, root);
    P->AVLParent = P->AVLLeft = P->AVLRight = NULL;
}

