



/* This file was automatically generated with m4 from m4/proto.m4, do not edit! */

/*
 *  protom4.m4  --  check libTw functions prototypes from sockproto.m4h
 *		    against twin.h or against Tw/Tw.h include files.
 *
 *  Copyright (C) 2001,2002 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */


















































uldat  Tw_FindFunction(tdisplay, byte namelen, TW_CONST byte *name, byte protolen, TW_CONST byte *proto);

byte  Tw_SyncSocket(tdisplay );

byte  Tw_ServerSizeof(tdisplay, byte type);

byte  Tw_CanCompress(tdisplay );
byte  Tw_DoCompress(tdisplay, byte on_off);

void  Tw_NeedResizeDisplay(tdisplay );

void  Tw_AttachHW(tdisplay, uldat len, TW_CONST byte *name, byte flags);
byte  Tw_DetachHW(tdisplay, uldat len, TW_CONST byte *name);

void  Tw_SetFontTranslation(tdisplay, TW_CONST byte *trans);
void  Tw_SetUniFontTranslation(tdisplay, TW_CONST hwfont *trans);

void  Tw_DeleteObj(tdisplay, tobj O);
void  Tw_ChangeFieldObj(tdisplay, tobj O, udat field, uldat clearmask, uldat xormask);


twidget  Tw_CreateWidget(tdisplay, dat w, dat h, uldat attrib, uldat flags, dat x, dat y, hwattr fill);
void  Tw_RecursiveDeleteWidget(tdisplay, twidget W);
void  Tw_MapWidget(tdisplay, twidget W, twidget parent);
void  Tw_UnMapWidget(tdisplay, twidget W);
void  Tw_SetXYWidget(tdisplay, twidget W, dat x, dat y);
void  Tw_ResizeWidget(tdisplay, twidget W, dat w, dat h);
void  Tw_ScrollWidget(tdisplay, twidget W, ldat dxl, ldat dyl);
tmsgport  Tw_GetOwnerWidget(tdisplay, twidget W);
void  Tw_DrawWidget(tdisplay, twidget W, dat w, dat h, dat x, dat y, TW_CONST byte *text, TW_CONST hwfont *textfont, TW_CONST hwattr *textattr);

void  Tw_SetVisibleWidget(tdisplay, twidget W, byte on_off);

void  Tw_FocusSubWidget(tdisplay, twidget W);
twidget  Tw_FindWidgetAtWidget(tdisplay, twidget W, dat x, dat y);

void  Tw_RaiseWidget(tdisplay, twidget W);
void  Tw_LowerWidget(tdisplay, twidget W);
void  Tw_RestackChildrenWidget(tdisplay, twidget W, uldat n, TW_CONST twidget *children);
void  Tw_CirculateChildrenWidget(tdisplay, twidget W, byte up_down);


tgadget  Tw_CreateGadget(tdisplay, twidget parent, dat w, dat h, TW_CONST byte *text, uldat attrib, uldat flags, udat code, hwcol coltext, hwcol colselect, hwcol coldisabled, hwcol colselectdisabled, dat x, dat y);

tgadget  Tw_CreateButtonGadget(tdisplay, twidget parent, dat w, dat h, TW_CONST byte *text, uldat flags, udat code, hwcol colbg, hwcol col, hwcol coldisabled, dat x, dat y);

void  Tw_WriteTextsGadget(tdisplay, tgadget G, byte mask, dat w, dat h, TW_CONST byte *text, dat x, dat y);
void  Tw_WriteHWFontsGadget(tdisplay, tgadget G, byte mask, dat w, dat h, TW_CONST hwfont *textfont, dat x, dat y);

twindow  Tw_CreateWindow(tdisplay, dat namelen, TW_CONST byte *name, TW_CONST hwcol *colname, tmenu M, hwcol coltext, uldat cursortype, uldat attrib, uldat flags, dat w, dat h, dat hscroll);
twindow  Tw_Create4MenuWindow(tdisplay, tmenu M);

void  Tw_WriteAsciiWindow(tdisplay, twindow W, ldat len, TW_CONST byte *ascii);
void  Tw_WriteStringWindow(tdisplay, twindow W, ldat len, TW_CONST byte *string);
void  Tw_WriteHWFontWindow(tdisplay, twindow W, ldat len, TW_CONST hwfont *textfont);
void  Tw_WriteHWAttrWindow(tdisplay, twindow W, dat x, dat y, ldat len, TW_CONST hwattr *textattr);

void  Tw_GotoXYWindow(tdisplay, twindow W, ldat x, ldat y);
void  Tw_SetColTextWindow(tdisplay, twindow W, hwcol coltext);
void  Tw_SetColorsWindow(tdisplay, twindow W, udat mask, hwcol colgadgets, hwcol colarrows, hwcol colbars, hwcol coltabs, hwcol colborder, hwcol coltext, hwcol colselect, hwcol coldisabled, hwcol colselectdisabled);
void  Tw_ConfigureWindow(tdisplay, twindow W, byte mask, dat x, dat y, dat minw, dat minh, dat maxw, dat maxh);
trow  Tw_FindRowByCodeWindow(tdisplay, twindow W, dat code);

tgroup  Tw_CreateGroup(tdisplay );
void  Tw_InsertGadgetGroup(tdisplay, tgroup g, tgadget G);
void  Tw_RemoveGadgetGroup(tdisplay, tgroup g, tgadget G);

tgadget  Tw_GetSelectedGadgetGroup(tdisplay, tgroup g);
void  Tw_SetSelectedGadgetGroup(tdisplay, tgroup g, tgadget G);

void  Tw_Create4MenuRow(tdisplay, twindow W, udat code, byte flags, ldat len, TW_CONST byte *text);

void  Tw_RaiseRow(tdisplay, trow R);
void  Tw_LowerRow(tdisplay, trow R);
void  Tw_RestackChildrenRow(tdisplay, tobj O, uldat n, TW_CONST trow *children); 
void  Tw_CirculateChildrenRow(tdisplay, tobj O, byte up_down);

tmenuitem  Tw_Create4MenuMenuItem(tdisplay, tobj parent, twindow W, byte flags, dat len, TW_CONST byte *text);
trow  Tw_Create4MenuAny(tdisplay, tobj parent, twindow W, udat code, byte flags, ldat len, TW_CONST byte *text);

uldat  Tw_Create4MenuCommonMenuItem(tdisplay, tmenu M);

tmenu  Tw_CreateMenu(tdisplay, hwcol colitem, hwcol colselect, hwcol coldisabled, hwcol colselectdisabled, hwcol colshortcut, hwcol colshortcutselect, byte flags);
void  Tw_SetInfoMenu(tdisplay, tmenu M, byte flags, ldat len, TW_CONST byte *text, TW_CONST hwcol *coltext);

tmsgport  Tw_CreateMsgPort(tdisplay, byte len, TW_CONST byte *name, time_t secpause, frac_t fracpause, byte flags);
tmsgport  Tw_FindMsgPort(tdisplay, tmsgport prev, byte len, TW_CONST byte *name);

void  Tw_BgImageScreen(tdisplay, tscreen S, dat w, dat h, TW_CONST hwattr *textattr);

tobj  Tw_PrevObj(tdisplay, tobj O);
tobj  Tw_NextObj(tdisplay, tobj O);
tobj  Tw_ParentObj(tdisplay, tobj O);

tgadget    Tw_G_PrevGadget(tdisplay, tgadget  G);
tgadget    Tw_G_NextGadget(tdisplay, tgadget  G);
tgroup     Tw_GroupGadget(tdisplay, tgadget  G);

twidget    Tw_O_PrevWidget(tdisplay, twidget  W);
twidget    Tw_O_NextWidget(tdisplay, twidget  W);
tmsgport   Tw_OwnerWidget(tdisplay, twidget  W);

tscreen    Tw_FirstScreen(tdisplay );
twidget    Tw_FirstWidget(tdisplay, twidget  W);
tmsgport   Tw_FirstMsgPort(tdisplay );
tmenu      Tw_FirstMenu(tdisplay, tmsgport P);
twidget    Tw_FirstW(tdisplay, tmsgport P);
tgroup     Tw_FirstGroup(tdisplay, tmsgport P);
tmutex     Tw_FirstMutex(tdisplay, tmsgport P);
tmenuitem  Tw_FirstMenuItem(tdisplay, tmenu    M);
tgadget    Tw_FirstGadget(tdisplay, tgroup   g);

dat  Tw_GetDisplayWidth(tdisplay );
dat  Tw_GetDisplayHeight(tdisplay );
tall  Tw_GetAll(tdisplay );

byte  Tw_SendToMsgPort(tdisplay, tmsgport P, udat len, TW_CONST byte *data);
void  Tw_BlindSendToMsgPort(tdisplay, tmsgport P, udat len, TW_CONST byte *data);

tobj  Tw_GetOwnerSelection(tdisplay );
void  Tw_SetOwnerSelection(tdisplay, time_t secnow, frac_t fracnow);
void  Tw_RequestSelection(tdisplay, tobj owner, uldat reqprivate);
void  Tw_NotifySelection(tdisplay, tobj requestor, uldat reqprivate, uldat magic, TW_CONST byte *mine, uldat len, TW_CONST byte *data);

byte  Tw_SetServerUid(tdisplay, uldat uid, byte privileges);



