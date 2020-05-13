



/* This file was automatically generated with m4 from m4/proto.m4, do not edit! */

/*
 *  proto_m4.h  --  check libTw functions prototypes from Tw_sockproto.m4h
 *                    against twin.h or against Tw/Tw.h include files.
 *
 *  Copyright (C) 2001,2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */














/** return server-side id of given libTw function name with given prototype */
uldat  Tw_FindFunction(tdisplay TwD, byte namelen, TW_CONST char *name, byte protolen, TW_CONST char *proto);


/** sync libTw socket: flush output buffer and wait for server replies */
byte  Tw_SyncSocket(tdisplay TwD);


/** return server idea of sizeof(type) */
byte  Tw_ServerSizeof(tdisplay TwD, byte type);


/** return 1 if server supports compression (using zlib) */
byte  Tw_CanCompress(tdisplay TwD);

/** used internally by libTw to enable/disable compression only on server side;
 * DO NOT USE THIS, use Tw_EnableGzip() and Tw_DisableGzip() instead */
byte  Tw_DoCompress(tdisplay TwD, byte on_off);


/** force a server display resize; used by twdisplay */
void  Tw_NeedResizeDisplay(tdisplay TwD);


/** tell server to open a new display; used by twattach/twdisplay */
void  Tw_AttachHW(tdisplay TwD, uldat len, TW_CONST char *name, byte flags);

/** tell server to close given display */
byte  Tw_DetachHW(tdisplay TwD, uldat len, TW_CONST char *name);


/** set server global charset translation */
void  Tw_SetFontTranslation(tdisplay TwD, TW_CONST byte *trans);

/** set server global charset translation */
void  Tw_SetTRuneTranslation(tdisplay TwD, TW_CONST trune *trans);


/** delete given object */
void  Tw_DeleteObj(tdisplay TwD, tobj O);

/** change given field of object; new_value = (old_value & ~clearmask) ^ xormask */
void  Tw_ChangeField(tdisplay TwD, tobj O, udat field, uldat clearmask, uldat xormask);


twidget  Tw_CreateWidget(tdisplay TwD, dat w, dat h, uldat attrib, uldat flags, dat x, dat y, tcell fill);

/** delete given widget and all its children */
void  Tw_RecursiveDeleteWidget(tdisplay TwD, twidget W);
void  Tw_MapWidget(tdisplay TwD, twidget W, twidget parent);
void  Tw_UnMapWidget(tdisplay TwD, twidget W);
void  Tw_SetXYWidget(tdisplay TwD, twidget W, dat x, dat y);
void  Tw_ResizeWidget(tdisplay TwD, twidget W, dat w, dat h);
void  Tw_ScrollWidget(tdisplay TwD, twidget W, ldat dxl, ldat dyl);
void  Tw_DrawWidget(tdisplay TwD, twidget W, dat w, dat h, dat x, dat y, TW_CONST char *text, TW_CONST trune *textfont, TW_CONST tcell *textattr);

void  Tw_SetVisibleWidget(tdisplay TwD, twidget W, byte on_off);

void  Tw_FocusSubWidget(tdisplay TwD, twidget W);
twidget  Tw_FindWidgetAtWidget(tdisplay TwD, twidget W, dat x, dat y);

void  Tw_RaiseWidget(tdisplay TwD, twidget W);
void  Tw_LowerWidget(tdisplay TwD, twidget W);
void  Tw_RestackChildrenWidget(tdisplay TwD, twidget W, uldat n, TW_CONST twidget *children);
void  Tw_CirculateChildrenWidget(tdisplay TwD, twidget W, byte up_down);


tgadget  Tw_CreateGadget(tdisplay TwD, twidget parent, dat w, dat h, TW_CONST char *text, uldat attrib, uldat flags, udat code, tcolor coltext, tcolor colselect, tcolor coldisabled, tcolor colselectdisabled, dat x, dat y);

tgadget  Tw_CreateButtonGadget(tdisplay TwD, twidget parent, dat w, dat h, TW_CONST char *text, uldat flags, udat code, tcolor colbg, tcolor col, tcolor coldisabled, dat x, dat y);

void  Tw_WriteTextsGadget(tdisplay TwD, tgadget G, byte mask, dat w, dat h, TW_CONST char *text, dat x, dat y);
void  Tw_WriteTRunesGadget(tdisplay TwD, tgadget G, byte mask, dat w, dat h, TW_CONST trune *textfont, dat x, dat y);

twindow  Tw_CreateWindow(tdisplay TwD, dat titlelen, TW_CONST char *title, TW_CONST tcolor *coltitle, tmenu M, tcolor coltext, uldat cursortype, uldat attrib, uldat flags, dat w, dat h, dat hscroll);
twindow  Tw_Create4MenuWindow(tdisplay TwD, tmenu M);

void  Tw_WriteAsciiWindow(tdisplay TwD, twindow W, uldat len, TW_CONST char *ascii);
void  Tw_WriteStringWindow(tdisplay TwD, twindow W, uldat len, TW_CONST char *string);
void  Tw_WriteTRuneWindow(tdisplay TwD, twindow W, uldat len, TW_CONST trune *textfont);
void  Tw_WriteTCellWindow(tdisplay TwD, twindow W, dat x, dat y, uldat len, TW_CONST tcell *textattr);

void  Tw_GotoXYWindow(tdisplay TwD, twindow W, ldat x, ldat y);
void  Tw_SetTitleWindow(tdisplay TwD, twindow W, dat titlelen, TW_CONST char *title);
void  Tw_SetColTextWindow(tdisplay TwD, twindow W, tcolor coltext);
void  Tw_SetColorsWindow(tdisplay TwD, twindow W, udat mask, tcolor colgadgets, tcolor colarrows, tcolor colbars, tcolor coltabs, tcolor colborder, tcolor coltext, tcolor colselect, tcolor coldisabled, tcolor colselectdisabled);
void  Tw_ConfigureWindow(tdisplay TwD, twindow W, byte mask, dat x, dat y, dat minw, dat minh, dat maxw, dat maxh);
trow  Tw_FindRowByCodeWindow(tdisplay TwD, twindow W, dat code);

tgroup  Tw_CreateGroup(tdisplay TwD);
void  Tw_InsertGadgetGroup(tdisplay TwD, tgroup g, tgadget G);
void  Tw_RemoveGadgetGroup(tdisplay TwD, tgroup g, tgadget G);

tgadget  Tw_GetSelectedGadgetGroup(tdisplay TwD, tgroup g);
void  Tw_SetSelectedGadgetGroup(tdisplay TwD, tgroup g, tgadget G);

void  Tw_RaiseRow(tdisplay TwD, trow R);
void  Tw_LowerRow(tdisplay TwD, trow R);
void  Tw_RestackChildrenRow(tdisplay TwD, tobj O, uldat n, TW_CONST trow *children);
void  Tw_CirculateChildrenRow(tdisplay TwD, tobj O, byte up_down);

trow  Tw_Create4MenuAny(tdisplay TwD, tobj parent, twindow W, udat code, byte flags, ldat len, TW_CONST char *text);

uldat  Tw_Create4MenuCommonMenuItem(tdisplay TwD, tmenu M);

tmenu  Tw_CreateMenu(tdisplay TwD, tcolor colitem, tcolor colselect, tcolor coldisabled, tcolor colselectdisabled, tcolor colshortcut, tcolor colshortcutselect, byte flags);
void  Tw_SetInfoMenu(tdisplay TwD, tmenu M, byte flags, ldat len, TW_CONST char *text, TW_CONST tcolor *coltext);

tmsgport  Tw_CreateMsgPort(tdisplay TwD, byte len, TW_CONST char *name);
tmsgport  Tw_FindMsgPort(tdisplay TwD, tmsgport prev, byte len, TW_CONST char *name);

void  Tw_BgImageScreen(tdisplay TwD, tscreen S, dat w, dat h, TW_CONST tcell *textattr);

tobj  Tw_PrevObj(tdisplay TwD, tobj O);
tobj  Tw_NextObj(tdisplay TwD, tobj O);
tobj  Tw_ParentObj(tdisplay TwD, tobj O);

tscreen    Tw_FirstScreen(tdisplay TwD);
twidget    Tw_FirstWidget(tdisplay TwD, twidget  W);
tmsgport   Tw_FirstMsgPort(tdisplay TwD);
tmenu      Tw_FirstMenu(tdisplay TwD, tmsgport P);
twidget    Tw_FirstW(tdisplay TwD, tmsgport P);
tgroup     Tw_FirstGroup(tdisplay TwD, tmsgport P);
tmutex     Tw_FirstMutex(tdisplay TwD, tmsgport P);
tmenuitem  Tw_FirstMenuItem(tdisplay TwD, tmenu    M);
tgadget    Tw_FirstGadget(tdisplay TwD, tgroup   g);

dat  Tw_GetDisplayWidth(tdisplay TwD);
dat  Tw_GetDisplayHeight(tdisplay TwD);
tall  Tw_GetAll(tdisplay TwD);

byte  Tw_SendToMsgPort(tdisplay TwD, tmsgport P, udat len, TW_CONST byte *data);
void  Tw_BlindSendToMsgPort(tdisplay TwD, tmsgport P, udat len, TW_CONST byte *data);

tobj  Tw_GetOwnerSelection(tdisplay TwD);
void  Tw_SetOwnerSelection(tdisplay TwD, tany secnow, tany fracnow);
void  Tw_RequestSelection(tdisplay TwD, tobj owner, uldat reqprivate);
void  Tw_NotifySelection(tdisplay TwD, tobj requestor, uldat reqprivate, uldat magic, TW_CONST char *mime, uldat len, TW_CONST char *data);

byte  Tw_SetServerUid(tdisplay TwD, uldat uid, byte privileges);

textension  Tw_OpenExtension(tdisplay TwD, byte namelen, TW_CONST char *name);
tany  Tw_CallBExtension(tdisplay TwD, textension id, topaque len, TW_CONST byte *data, TW_CONST byte *return_type);
void  Tw_CloseExtension(tdisplay TwD, textension id);
