#ifndef _TW_DATA_H
#define _TW_DATA_H

byte InitData(void);

extern CONST byte *conf_destdir_lib_twin;
extern CONST byte *conf_destdir_lib_twin_modules_;

extern palette Palette[MAXCOL+1], defaultPalette[MAXCOL+1];

extern all *All;

typedef struct keylist keylist;
struct keylist {
    byte *name;
    udat key;
    byte len;
    byte *seq;
};

extern keylist TW_KeyList[];

extern gadget GadgetFlag, GadgetSwitch;

extern byte GadgetResize[2][2],
    ScrollBarX[2][3],
    ScrollBarY[2][3],
    TabX[2],
    TabY[2],
    StdBorder[2][2][9],
    Screen_Back[2];

extern hwcol DEFAULT_ColGadgets, DEFAULT_ColArrows, DEFAULT_ColBars, DEFAULT_ColTabs,
	DEFAULT_ColBorder, DEFAULT_ColDisabled, DEFAULT_ColSelectDisabled;

#if 0
extern font NewFont8[], NewFont14[], NewFont16[];
#endif

#endif /* _TW_DATA_H */
