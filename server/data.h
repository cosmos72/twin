#ifndef _TW_DATA_H
#define _TW_DATA_H

byte InitData(void);

extern all *All;

extern gadget GadgetFlag, GadgetSwitch;

extern byte GadgetClose[2][2],
GadgetBack[2][2],
GadgetResize[2][2],
ScrollBarX[2][3],
ScrollBarY[2][3],
TabX[2],
TabY[2],
StdBorder[2][2][3][3];

extern hwcol DEFAULT_ColGadgets, DEFAULT_ColArrows, DEFAULT_ColBars, DEFAULT_ColTabs,
	DEFAULT_ColBorder, DEFAULT_ColDisabled, DEFAULT_ColSelectDisabled;

#if 0
extern font NewFont8[], NewFont14[], NewFont16[];
#endif

#define ExecK(n,k) ExecKey[n][(k) > '~' ? (k) - '~' + ' ' : (k)]
#define ExecM(n,k) ExecMouse[n][k]

#define FnK(p,k) (p)[(k) > '~' ? (k) - '~' + ' ' : (k)]
#define FnM(p,k) (p)[k]

#endif /* _TW_DATA_H */
