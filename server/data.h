#ifndef _TW_DATA_H
#define _TW_DATA_H

byte InitData(void);

extern palette Palette[MAXCOL+1], defaultPalette[MAXCOL+1];

extern all *All;

extern gadget GadgetFlag, GadgetSwitch;

extern byte GadgetResize[2][2],
    ScrollBarX[2][3],
    ScrollBarY[2][3],
    TabX[2],
    TabY[2],
    StdBorder[2][2][3][3];

#define MAX_BUTTONS 10
extern byte Button_N;
extern num  Button_Close, Button_Back;
extern byte Button_Fn[MAX_BUTTONS];
extern byte Button_Shape[MAX_BUTTONS][2][2];
extern num  Button_Pos[MAX_BUTTONS];
extern byte Button_Delta_Left, Button_Delta_Right;

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
