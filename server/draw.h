#ifndef _TW_DRAW_H
#define _TW_DRAW_H

void SearchFontMenuItem(menu *Menu, menuitem *MenuItem, dat i, byte Select, byte *PtrFont, byte *PtrColor);
void SearchFontInfo(menu *Menu, dat i, byte Select, byte *PtrFont, byte *PtrColor);
void DrawWindow(window *Window, gadget *FirstGadget, gadget *OnlyThisGadget, dat Xstart, dat Ystart, dat Xend, dat Yend, byte Shaded);
void DrawDesktop(screen *Screen, dat Xstart, dat Ystart, dat Xend, dat Yend, byte Shaded);
void DrawMenuBar(screen *Screen, dat Xstart, dat Xend);

void DrawArea(screen *FirstScreen, window *FirstWindow, window *OnlyThisWindow, gadget *FirstGadget, gadget *OnlyThisGadget, dat Xstart, dat Ystart, dat Xend, dat Yend, byte Shaded);
#define FULLSCREEN (screen *)0, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)0, (dat)0, MAXDAT, MAXDAT, FALSE
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
void DrawUnShadeWindow(window *Window, dat Xstart, dat Ystart, dat Xend, dat Yend);
void DrawAreaWindow(window *Window, byte Shaded);
void ReDrawRolledUpAreaWindow(window *Window, byte Shaded);
void DrawAbsoluteWindow(window *Window, udat Xstart, udat Ystart, udat Xend, udat Yend);
void DrawTextWindow(window *Window, uldat Xstart, uldat Xend, uldat NumRowStart, uldat NumRowEnd);

void ClearHilight(window *Window);
void StartHilight(window *Window, uldat XSel, uldat YSel);
void ExtendHilight(window *Window, uldat XSel, uldat YSel);

#endif /* _TW_DRAW_H */
