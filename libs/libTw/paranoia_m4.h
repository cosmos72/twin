



/* This file was automatically generated with m4 from m4/paranoia.m4, do not edit! */

/*
 *  paranoiam4.m4  --  check libTw functions prototypes from sockproto.h
 *			   against twin.h or against libTw.h include files.
 *
 *  Copyright (C) 2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#if defined(_TW_H)
# define CONST		TW_CONST
# define PREFIX_VOID	tdisplay
# define PREFIX		tdisplay,
#elif defined(_TWIN_H)
# define PREFIX_VOID	void
# define PREFIX
#else
# error you must #include at least one of "twin.h" or "Tw/Tw.h"
#endif




















































uldat  Tw_FindFunction(PREFIX byte a1, CONST byte *a2, byte a3, CONST byte *a4) ;

byte  Tw_SyncSocket(PREFIX_VOID ) ;

byte  Tw_ServerSizeof(PREFIX byte a1) ;

byte  Tw_CanCompress(PREFIX_VOID ) ;
byte  Tw_DoCompress(PREFIX byte a1) ;

void  Tw_NeedResizeDisplay(PREFIX_VOID ) ;

void  Tw_AttachHW(PREFIX uldat a1, CONST byte *a2, byte a3) ;
byte  Tw_DetachHW(PREFIX uldat a1, CONST byte *a2) ;

void  Tw_SetFontTranslation(PREFIX CONST byte *a1) ;
void  Tw_SetUniFontTranslation(PREFIX CONST hwfont *a1) ;

void  Tw_DeleteObj(PREFIX tobj a1) ;
void  Tw_ChangeFieldObj(PREFIX tobj a1, udat a2, uldat a3, uldat a4) ;


twidget  Tw_CreateWidget(PREFIX dat a1, dat a2, uldat a3, uldat a4, dat a5, dat a6, hwattr a7) ;
void  Tw_RecursiveDeleteWidget(PREFIX twidget a1) ;
void  Tw_MapWidget(PREFIX twidget a1, twidget a2) ;
void  Tw_UnMapWidget(PREFIX twidget a1) ;
void  Tw_SetXYWidget(PREFIX twidget a1, dat a2, dat a3) ;
void  Tw_ResizeWidget(PREFIX twidget a1, dat a2, dat a3) ;
void  Tw_ScrollWidget(PREFIX twidget a1, ldat a2, ldat a3) ;
tmsgport  Tw_GetOwnerWidget(PREFIX twidget a1) ;
void  Tw_ExposeWidget(PREFIX twidget a1, dat a2, dat a3, dat a4, dat a5, CONST byte *a6, CONST hwfont *a7, CONST hwattr *a8) ;

void  Tw_FocusSubWidget(PREFIX twidget a1) ;
twidget  Tw_FindWidgetAtWidget(PREFIX twidget a1, dat a2, dat a3) ;

void  Tw_RaiseWidget(PREFIX twidget a1) ;
void  Tw_LowerWidget(PREFIX twidget a1) ;
void  Tw_RestackChildrenWidget(PREFIX twidget a1, uldat a2, CONST twidget *a3) ;
void  Tw_CirculateChildrenWidget(PREFIX twidget a1, byte a2) ;


tgadget  Tw_CreateGadget(PREFIX twidget a1, dat a2, dat a3, CONST byte *a4, uldat a5, uldat a6, udat a7, hwcol a8, hwcol a9, hwcol a10, hwcol a11, dat a12, dat a13) ;

tgadget  Tw_CreateButtonGadget(PREFIX twidget a1, dat a2, dat a3, CONST byte *a4, uldat a5, udat a6, hwcol a7, hwcol a8, hwcol a9, dat a10, dat a11) ;

void  Tw_WriteTextsGadget(PREFIX tgadget a1, byte a2, dat a3, dat a4, CONST byte *a5, dat a6, dat a7) ;
void  Tw_WriteHWFontsGadget(PREFIX tgadget a1, byte a2, dat a3, dat a4, CONST hwfont *a5, dat a6, dat a7) ;

twindow  Tw_CreateWindow(PREFIX dat a1, CONST byte *a2, CONST hwcol *a3, tmenu a4, hwcol a5, uldat a6, uldat a7, uldat a8, dat a9, dat a10, dat a11) ;
twindow  Tw_Create4MenuWindow(PREFIX tmenu a1) ;

void  Tw_WriteAsciiWindow(PREFIX twindow a1, ldat a2, CONST byte *a3) ;
void  Tw_WriteStringWindow(PREFIX twindow a1, ldat a2, CONST byte *a3) ;
void  Tw_WriteHWFontWindow(PREFIX twindow a1, ldat a2, CONST hwfont *a3) ;
void  Tw_WriteHWAttrWindow(PREFIX twindow a1, dat a2, dat a3, ldat a4, CONST hwattr *a5) ;

void  Tw_GotoXYWindow(PREFIX twindow a1, ldat a2, ldat a3) ;
void  Tw_SetColTextWindow(PREFIX twindow a1, hwcol a2) ;
void  Tw_SetColorsWindow(PREFIX twindow a1, udat a2, hwcol a3, hwcol a4, hwcol a5, hwcol a6, hwcol a7, hwcol a8, hwcol a9, hwcol a10, hwcol a11) ;
void  Tw_ConfigureWindow(PREFIX twindow a1, byte a2, dat a3, dat a4, dat a5, dat a6, dat a7, dat a8) ;
trow  Tw_FindRowByCodeWindow(PREFIX twindow a1, dat a2) ;

tgroup  Tw_CreateGroup(PREFIX_VOID ) ;
void  Tw_InsertGadgetGroup(PREFIX tgroup a1, tgadget a2) ;
void  Tw_RemoveGadgetGroup(PREFIX tgroup a1, tgadget a2) ;

tgadget  Tw_GetSelectedGadgetGroup(PREFIX tgroup a1) ;
void  Tw_SetSelectedGadgetGroup(PREFIX tgroup a1, tgadget a2) ;

void  Tw_Create4MenuRow(PREFIX twindow a1, udat a2, byte a3, uldat a4, CONST byte *a5) ;

void  Tw_RaiseRow(PREFIX trow a1) ;
void  Tw_LowerRow(PREFIX trow a1) ;
void  Tw_RestackChildrenRow(PREFIX tobj a1, uldat a2, CONST trow *a3) ; 
void  Tw_CirculateChildrenRow(PREFIX tobj a1, byte a2) ;

tmenuitem  Tw_Create4MenuMenuItem(PREFIX tobj a1, twindow a2, byte a3, dat a4, CONST byte *a5) ;
uldat  Tw_Create4MenuCommonMenuItem(PREFIX tmenu a1) ;

tmenu  Tw_CreateMenu(PREFIX hwcol a1, hwcol a2, hwcol a3, hwcol a4, hwcol a5, hwcol a6, byte a7) ;
void  Tw_SetInfoMenu(PREFIX tmenu a1, byte a2, ldat a3, CONST byte *a4, CONST hwcol *a5) ;

tmsgport  Tw_CreateMsgPort(PREFIX byte a1, CONST byte *a2, time_t a3, frac_t a4, byte a5) ;
tmsgport  Tw_FindMsgPort(PREFIX tmsgport a1, byte a2, CONST byte *a3) ;

void  Tw_BgImageScreen(PREFIX tscreen a1, dat a2, dat a3, CONST hwattr *a4) ;

tobj  Tw_PrevObj(PREFIX tobj a1) ;
tobj  Tw_NextObj(PREFIX tobj a1) ;
tobj  Tw_ParentObj(PREFIX tobj a1) ;

tgadget    Tw_G_PrevGadget(PREFIX tgadget  a1) ;
tgadget    Tw_G_NextGadget(PREFIX tgadget  a1) ;
tgroup     Tw_GroupGadget(PREFIX tgadget  a1) ;

twidget    Tw_O_PrevWidget(PREFIX twidget  a1) ;
twidget    Tw_O_NextWidget(PREFIX twidget  a1) ;
tmsgport   Tw_OwnerWidget(PREFIX twidget  a1) ;

tscreen    Tw_FirstScreen(PREFIX_VOID ) ;
twidget    Tw_FirstWidget(PREFIX twidget  a1) ;
tmsgport   Tw_FirstMsgPort(PREFIX_VOID ) ;
tmenu      Tw_FirstMenu(PREFIX tmsgport a1) ;
twidget    Tw_FirstW(PREFIX tmsgport a1) ;
tgroup     Tw_FirstGroup(PREFIX tmsgport a1) ;
tmutex     Tw_FirstMutex(PREFIX tmsgport a1) ;
tmenuitem  Tw_FirstMenuItem(PREFIX tmenu    a1) ;
tgadget    Tw_FirstGadget(PREFIX tgroup   a1) ;

dat  Tw_GetDisplayWidth(PREFIX_VOID ) ;
dat  Tw_GetDisplayHeight(PREFIX_VOID ) ;
tall  Tw_GetAll(PREFIX_VOID ) ;

byte  Tw_SendToMsgPort(PREFIX tmsgport a1, udat a2, CONST byte *a3) ;
void  Tw_BlindSendToMsgPort(PREFIX tmsgport a1, udat a2, CONST byte *a3) ;

tobj  Tw_GetOwnerSelection(PREFIX_VOID ) ;
void  Tw_SetOwnerSelection(PREFIX time_t a1, frac_t a2) ;
void  Tw_RequestSelection(PREFIX tobj a1, uldat a2) ;
void  Tw_NotifySelection(PREFIX tobj a1, uldat a2, uldat a3, CONST byte *a4, uldat a5, CONST byte *a6) ;

byte  Tw_SetServerUid(PREFIX uldat a1, byte a2) ;



