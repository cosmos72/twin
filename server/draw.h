#ifndef _TW_DRAW_H
#define _TW_DRAW_H

void SearchFontMenuItem(menu *Menu, menuitem *MenuItem, dat i, byte Select, byte *PtrFont, byte *PtrColor);
void SearchFontInfo(menu *Menu, dat i, byte Select, byte *PtrFont, byte *PtrColor);
widget *SearchWidget(widget *Parent, dat X, dat Y);

void DrawSelfGadget(draw_ctx *D);
void DrawSelfWindow(draw_ctx *D);
void DrawSelfScreen(draw_ctx *D);

void DrawDesktop(screen *Screen, dat Xstart, dat Ystart, dat Xend, dat Yend, byte Shaded);

void DrawArea2(screen *FirstScreen, widget *Top, widget *OnlyW,
	       dat X1, dat Y1, dat X2, dat Y2, byte Shaded);
#define FULL_SCREEN (screen *)0, (widget *)0, (widget *)0, (dat)0, (dat)0, MAXDAT, MAXDAT, FALSE

void DrawWidget(widget *W, dat Xstart, dat Ystart, dat Xend, dat Yend, byte Shaded);
void DrawScreen(screen *Screen);

void DrawBorderWindow(window *Window, byte Flags);
#define BORDER_LEFT	(0x1)
#define BORDER_UP	(0x2)
#define BORDER_RIGHT	(0x4)
#define BORDER_DOWN	(0x8)
#define BORDER_ANY	(0xF)

void DrawShadeWindow(window *Window, dat Xstart, dat Ystart, dat Xend, dat Yend, byte Internal);
void DrawAreaShadeWindow(screen *Screen, window *Window, dat Xstart, dat Ystart, dat Xend, dat Yend,
			 ldat shLeft, ldat shUp, ldat shRgt, ldat shDwn, byte Internal);
void DrawAreaWindow2(window *Window);
/* like DrawAreaWindow2, but does not draw the shadow: */
void DrawFullWindow2(window *Window);

void DrawPartialWindow2(window *W, dat X1, dat Y1, dat X2, dat Y2);
void DrawLogicWindow2(window *Window, ldat X1, ldat Y1, ldat X2, ldat Y2);

void ReDrawRolledUpAreaWindow(window *Window, byte Shaded);

void ClearHilight(window *Window);
void StartHilight(window *Window, ldat XSel, ldat YSel);
void ExtendHilight(window *Window, ldat XSel, ldat YSel);

screen *ScreenParent(widget *W);
widget *TopParent(widget *W);

#endif /* _TW_DRAW_H */
