#ifndef _TW_WM_H
#define _TW_WM_H

typedef struct wm_ctx wm_ctx;

struct wm_ctx {
    window *W;
    screen *Screen;
    menu *Menu;
    menuitem *Item;
    gadget *G;
    udat Type; /* Msg->Type: MSG_* */
    byte Pos; /* POS_* */
    byte ByMouse;
    udat Code; /* keyboard key / mouse buttons pressed */
    udat ShiftFlags;
    dat i, j;  /* mouse coordinates */
    ldat Left, Up, Rgt, Dwn; /* window (W) corners */
};


byte InitWM(void);

void Check4Resize(window *W);
void AskCloseWindow(window *W);
void MaximizeWindow(window *W, byte full_screen);
void ShowWinList(wm_ctx *C);

void FocusCtx(wm_ctx *C);
void ActivateCtx(wm_ctx *C, byte State);

void ForceRelease(CONST wm_ctx *C);

#endif /* _TW_WM_H */
