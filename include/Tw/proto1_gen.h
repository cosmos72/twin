
/* This file was automatically generated with scheme from `twin/include/scheme/sockproto_inc.scm', do not edit! */

/**
 *  Copyright (C) 2001-2025 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 */
/** return server-side id of given libtw function name with given prototype */
#define TwFindFunction(namelen, name, protolen, proto)	Tw_FindFunction(Tw_DefaultD, namelen, name, protolen, proto)
/** sync libtw socket: flush output buffer and wait for server replies */
#define TwSyncSocket()	Tw_SyncSocket(Tw_DefaultD)
/** return server idea of sizeof(type) */
#define TwServerSizeof(type)	Tw_ServerSizeof(Tw_DefaultD, type)
/** return 1 if server supports compression (using zlib) */
#define TwCanCompress()	Tw_CanCompress(Tw_DefaultD)
/** used internally by libtw to enable/disable compression only on server side;
 * DO NOT USE THIS, use Tw_EnableGzip() and Tw_DisableGzip() instead */
#define TwDoCompress(on_off)	Tw_DoCompress(Tw_DefaultD, on_off)
/** force a server display resize; used by twdisplay */
#define TwNeedResizeDisplay()	Tw_NeedResizeDisplay(Tw_DefaultD)
/** tell server to open a new display; used by twattach/twdisplay */
#define TwAttachHW(len, name, flags)	Tw_AttachHW(Tw_DefaultD, len, name, flags)
/** tell server to close given display */
#define TwDetachHW(len, name)	Tw_DetachHW(Tw_DefaultD, len, name)
/** set server global charset translation */
#define TwSetFontTranslation(trans)	Tw_SetFontTranslation(Tw_DefaultD, trans)
/** set server global charset translation */
#define TwSetTRuneTranslation(trans)	Tw_SetTRuneTranslation(Tw_DefaultD, trans)
/** delete given object */
#define TwDeleteObj(O)	Tw_DeleteObj(Tw_DefaultD, O)
/** change given field of object; new_value = (old_value & ~clearmask) ^ xormask */
#define TwChangeField(O, field, clearmask, xormask)	Tw_ChangeField(Tw_DefaultD, O, field, clearmask, xormask)
#define TwCreateWidget(w, h, attrib, flags, x, y, fill)	Tw_CreateWidget(Tw_DefaultD, w, h, attrib, flags, x, y, fill)
/** delete given widget and all its children */
#define TwRecursiveDeleteWidget(W)	Tw_RecursiveDeleteWidget(Tw_DefaultD, W)
#define TwMapWidget(W, parent)	Tw_MapWidget(Tw_DefaultD, W, parent)
#define TwUnMapWidget(W)	Tw_UnMapWidget(Tw_DefaultD, W)
#define TwSetXYWidget(W, x, y)	Tw_SetXYWidget(Tw_DefaultD, W, x, y)
#define TwResizeWidget(W, w, h)	Tw_ResizeWidget(Tw_DefaultD, W, w, h)
#define TwScrollWidget(W, dxl, dyl)	Tw_ScrollWidget(Tw_DefaultD, W, dxl, dyl)
#define TwDrawWidget(W, w, h, x, y, chars, runes, cells)	Tw_DrawWidget(Tw_DefaultD, W, w, h, x, y, chars, runes, cells)
#define TwSetVisibleWidget(W, on_off)	Tw_SetVisibleWidget(Tw_DefaultD, W, on_off)
#define TwFocusSubWidget(W)	Tw_FocusSubWidget(Tw_DefaultD, W)
#define TwFindWidgetAtWidget(W, x, y)	Tw_FindWidgetAtWidget(Tw_DefaultD, W, x, y)
#define TwRaiseWidget(W)	Tw_RaiseWidget(Tw_DefaultD, W)
#define TwLowerWidget(W)	Tw_LowerWidget(Tw_DefaultD, W)
#define TwRestackChildrenWidget(W, n, children)	Tw_RestackChildrenWidget(Tw_DefaultD, W, n, children)
#define TwCirculateChildrenWidget(W, up_down)	Tw_CirculateChildrenWidget(Tw_DefaultD, W, up_down)
#define TwCreateGadget(parent, w, h, chars, attrib, flags, code, coltext, colselect, coldisabled, colselectdisabled, x, y)	Tw_CreateGadget(Tw_DefaultD, parent, w, h, chars, attrib, flags, code, coltext, colselect, coldisabled, colselectdisabled, x, y)
#define TwCreateButtonGadget(parent, w, h, chars, flags, code, colbg, col, coldisabled, x, y)	Tw_CreateButtonGadget(Tw_DefaultD, parent, w, h, chars, flags, code, colbg, col, coldisabled, x, y)
#define TwWriteTextsGadget(G, mask, w, h, chars, x, y)	Tw_WriteTextsGadget(Tw_DefaultD, G, mask, w, h, chars, x, y)
#define TwWriteTRunesGadget(G, mask, w, h, runes, x, y)	Tw_WriteTRunesGadget(Tw_DefaultD, G, mask, w, h, runes, x, y)
#define TwCreateWindow(titlelen, title, coltitle, M, coltext, cursortype, attrib, flags, w, h, hscroll)	Tw_CreateWindow(Tw_DefaultD, titlelen, title, coltitle, M, coltext, cursortype, attrib, flags, w, h, hscroll)
#define TwCreate4MenuWindow(M)	Tw_Create4MenuWindow(Tw_DefaultD, M)
#define TwWriteCharsetWindow(W, len, charset_bytes)	Tw_WriteCharsetWindow(Tw_DefaultD, W, len, charset_bytes)
#define TwWriteUtf8Window(W, len, utf8_bytes)	Tw_WriteUtf8Window(Tw_DefaultD, W, len, utf8_bytes)
#define TwWriteTRuneWindow(W, len, runes)	Tw_WriteTRuneWindow(Tw_DefaultD, W, len, runes)
#define TwWriteTCellWindow(W, x, y, len, cells)	Tw_WriteTCellWindow(Tw_DefaultD, W, x, y, len, cells)
#define TwGotoXYWindow(W, x, y)	Tw_GotoXYWindow(Tw_DefaultD, W, x, y)
#define TwSetTitleWindow(W, titlelen, title)	Tw_SetTitleWindow(Tw_DefaultD, W, titlelen, title)
#define TwSetColTextWindow(W, coltext)	Tw_SetColTextWindow(Tw_DefaultD, W, coltext)
#define TwSetColorsWindow(W, mask, colgadgets, colarrows, colbars, coltabs, colborder, coltext, colselect, coldisabled, colselectdisabled)	Tw_SetColorsWindow(Tw_DefaultD, W, mask, colgadgets, colarrows, colbars, coltabs, colborder, coltext, colselect, coldisabled, colselectdisabled)
#define TwConfigureWindow(W, mask, x, y, minw, minh, maxw, maxh)	Tw_ConfigureWindow(Tw_DefaultD, W, mask, x, y, minw, minh, maxw, maxh)
#define TwFindRowByCodeWindow(W, code)	Tw_FindRowByCodeWindow(Tw_DefaultD, W, code)
#define TwCreateGroup()	Tw_CreateGroup(Tw_DefaultD)
#define TwInsertGadgetGroup(g, G)	Tw_InsertGadgetGroup(Tw_DefaultD, g, G)
#define TwRemoveGadgetGroup(g, G)	Tw_RemoveGadgetGroup(Tw_DefaultD, g, G)
#define TwGetSelectedGadgetGroup(g)	Tw_GetSelectedGadgetGroup(Tw_DefaultD, g)
#define TwSetSelectedGadgetGroup(g, G)	Tw_SetSelectedGadgetGroup(Tw_DefaultD, g, G)
#define TwRaiseRow(R)	Tw_RaiseRow(Tw_DefaultD, R)
#define TwLowerRow(R)	Tw_LowerRow(Tw_DefaultD, R)
#define TwRestackChildrenRow(O, n, children)	Tw_RestackChildrenRow(Tw_DefaultD, O, n, children)
#define TwCirculateChildrenRow(O, up_down)	Tw_CirculateChildrenRow(Tw_DefaultD, O, up_down)
#define TwCreate4MenuAny(parent, W, code, flags, len, chars)	Tw_Create4MenuAny(Tw_DefaultD, parent, W, code, flags, len, chars)
#define TwCreate4MenuCommonMenuItem(M)	Tw_Create4MenuCommonMenuItem(Tw_DefaultD, M)
#define TwCreateMenu(colitem, colselect, coldisabled, colselectdisabled, colshortcut, colshortcutselect, flags)	Tw_CreateMenu(Tw_DefaultD, colitem, colselect, coldisabled, colselectdisabled, colshortcut, colshortcutselect, flags)
#define TwSetInfoMenu(M, flags, len, chars, coltext)	Tw_SetInfoMenu(Tw_DefaultD, M, flags, len, chars, coltext)
#define TwCreateMsgPort(len, name)	Tw_CreateMsgPort(Tw_DefaultD, len, name)
#define TwFindMsgPort(prev, len, name)	Tw_FindMsgPort(Tw_DefaultD, prev, len, name)
#define TwBgImageScreen(S, w, h, textattr)	Tw_BgImageScreen(Tw_DefaultD, S, w, h, textattr)
#define TwPrevObj(O)	Tw_PrevObj(Tw_DefaultD, O)
#define TwNextObj(O)	Tw_NextObj(Tw_DefaultD, O)
#define TwParentObj(O)	Tw_ParentObj(Tw_DefaultD, O)
#define TwFirstScreen()	Tw_FirstScreen(Tw_DefaultD)
#define TwFirstWidget(W)	Tw_FirstWidget(Tw_DefaultD, W)
#define TwFirstMsgPort()	Tw_FirstMsgPort(Tw_DefaultD)
#define TwFirstMenu(P)	Tw_FirstMenu(Tw_DefaultD, P)
#define TwFirstW(P)	Tw_FirstW(Tw_DefaultD, P)
#define TwFirstGroup(P)	Tw_FirstGroup(Tw_DefaultD, P)
#define TwFirstMutex(P)	Tw_FirstMutex(Tw_DefaultD, P)
#define TwFirstMenuItem(M)	Tw_FirstMenuItem(Tw_DefaultD, M)
#define TwFirstGadget(g)	Tw_FirstGadget(Tw_DefaultD, g)
#define TwGetDisplayWidth()	Tw_GetDisplayWidth(Tw_DefaultD)
#define TwGetDisplayHeight()	Tw_GetDisplayHeight(Tw_DefaultD)
#define TwGetAll()	Tw_GetAll(Tw_DefaultD)
#define TwSendToMsgPort(P, len, data)	Tw_SendToMsgPort(Tw_DefaultD, P, len, data)
#define TwBlindSendToMsgPort(P, len, data)	Tw_BlindSendToMsgPort(Tw_DefaultD, P, len, data)
#define TwGetOwnerSelection()	Tw_GetOwnerSelection(Tw_DefaultD)
#define TwSetOwnerSelection(secnow, fracnow)	Tw_SetOwnerSelection(Tw_DefaultD, secnow, fracnow)
#define TwRequestSelection(owner, reqprivate)	Tw_RequestSelection(Tw_DefaultD, owner, reqprivate)
#define TwNotifySelection(requestor, reqprivate, magic, mime, len, data)	Tw_NotifySelection(Tw_DefaultD, requestor, reqprivate, magic, mime, len, data)
#define TwSetServerUid(uid, privileges)	Tw_SetServerUid(Tw_DefaultD, uid, privileges)
