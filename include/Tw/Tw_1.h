
#ifndef _TW_1_H
#define _TW_1_H


/*
 * libTw.so.1 --- single head compatibility. these are too handy to drop them.
 */
extern tdisplay Tw_DefaultD;

#define TwCheckMagic(id)		Tw_CheckMagic(id)

#define TwConfigMalloc(my_malloc, my_realloc, my_free) Tw_ConfigMalloc(my_malloc, my_realloc, my_free)

#define TwLenStr(S)			Tw_LenStr(S)
#define TwCmpStr(S1, S2)		Tw_CmpStr(S1, S2)
#define TwCopyStr(From,To)		Tw_CopyStr(From, To)

#define TwAllocMem(Size)		Tw_AllocMem(Size)
#define TwReAllocMem(Mem, Size)		Tw_ReAllocMem(Mem, Size)
#define TwFreeMem(Mem)			Tw_FreeMem(Mem)
#define TwCloneMem(Mem,Size)		Tw_CloneMem(Mem,Size)
#define TwCloneStr(S)			Tw_CloneStr(S)
#define TwCloneStr2HWFont(Mem, Size)	Tw_CloneMem2HWFont(Mem, Size)

#define TwCopyMem(From, To, Size)	Tw_CopyMem(From, To, Size)
#define TwMoveMem(From, To, Size)	Tw_MoveMem(From, To, Size)
#define TwWriteMem(Mem, Char, Size)	Tw_WriteMem(Mem, Char, Size)
#define TwCmpMem(m1, m2, Size)		Tw_CmpMem(m1, m2, Size)

#define TwDropPrivileges()		Tw_DropPrivileges()
#define TwGetPrivileges()		Tw_GetPrivileges()

#define TwFindFunction(Len, Name, ProtoLen, Proto)	Tw_FindFunction(Tw_DefaultD, Len, Name, ProtoLen, Proto)
#define TwServerSizeof(Type)		Tw_ServerSizeof(Tw_DefaultD, Type)
#define TwEnableGzip()			Tw_EnableGzip(Tw_DefaultD)
#define TwDisableGzip()			Tw_DisableGzip(Tw_DefaultD)
#define TwNeedResizeDisplay()		Tw_NeedResizeDisplay(Tw_DefaultD)
#define TwAttachHW(len, name, flags)	Tw_AttachHW(Tw_DefaultD, len, name, flags)
#define TwAttachGetReply(len)		Tw_AttachGetReply(Tw_DefaultD, len)
#define TwAttachConfirm()		Tw_AttachConfirm(Tw_DefaultD)
#define TwDetachHW(len, name)		Tw_DetachHW(Tw_DefaultD, len, name)
#define TwSetFontTranslation(trans)	Tw_SetFontTranslation(Tw_DefaultD, trans)
#define TwSetUniFontTranslation(trans)	Tw_SetUniFontTranslation(Tw_DefaultD, trans)

#define TwDeleteObj(O)			Tw_DeleteObj(Tw_DefaultD, O)

#define TwCreateWidget(XWidth, YWidth, Attrib, Flags, Left, Up, Fill) \
					Tw_CreateWidget(Tw_DefaultD, XWidth, YWidth, Attrib, Flags, Left, Up, Fill)
#define TwDeleteWidget(W)		Tw_DeleteWidget(Tw_DefaultD, W)
#define TwRecursiveDeleteWidget(W)	Tw_RecursiveDeleteWidget(Tw_DefaultD, W)
#define TwMapWidget(W, Parent)		Tw_MapWidget(Tw_DefaultD, W, Parent)
#define TwUnMapWidget(W)		Tw_UnMapWidget(Tw_DefaultD, W)
#define TwSetXYWidget(W, x, y)		Tw_SetXYWidget(Tw_DefaultD, W, x, y)
#define TwResizeWidget(W, x, y)		Tw_ResizeWidget(Tw_DefaultD, W, x, y)
#define TwScrollWidget(W, x, y)		Tw_ScrollWidget(Tw_DefaultD, W, x, y)
#define TwGetOwnerWidget(W)		Tw_GetOwnerWidget(Tw_DefaultD, W)
#define TwFindWidgetAtWidget(W, i, j)	Tw_FindWidgetAtWidget(Tw_DefaultD, W, i, j)

#define TwDrawWidget(W, XWidth, YWidth, Left, Up, Text, Font, Attr) \
		Tw_DrawWidget(Tw_DefaultD, W, XWidth, YWidth, Left, Up, Text, Font, Attr)

#define TwDraw2Widget(W, XWidth, YWidth, Left, Up, Pitch, Text, Font, Attr) \
		Tw_Draw2Widget(Tw_DefaultD, W, XWidth, YWidth, Left, Up, Pitch, Text, Font, Attr)

#define TwDrawTextWidget(W, XWidth, YWidth, Left, Up, Pitch, Text) \
		Tw_DrawTextWidget(Tw_DefaultD, W, XWidth, YWidth, Left, Up, Pitch, Text)
#define TwDrawHWFontWidget(W, XWidth, YWidth, Left, Up, Pitch, Font) \
		Tw_DrawHWFontWidget(Tw_DefaultD, W, XWidth, YWidth, Left, Up, Pitch, Font)
#define TwDrawHWAttrWidget(W, XWidth, YWidth, Left, Up, Pitch, Attr) \
		Tw_DrawHWAttrWidget(Tw_DefaultD, W, XWidth, YWidth, Left, Up, Pitch, Attr)

#define TwSetVisibleWidget(W, on_off)	Tw_SetVisibleWidget(Tw_DefaultD, W, on_off)
#define TwFocusSubWidget(W)		Tw_FocusSubWidget(Tw_DefaultD, W)

#define TwLowerWidget(W)		Tw_LowerWidget(Tw_DefaultD, W)
#define TwRaiseWidget(W)		Tw_RaiseWidget(Tw_DefaultD, W)
#define TwRestackChildrenWidget(W, N, arrayW) Tw_RestackChildrenWidget(Tw_DefaultD, W, N, arrayW)
#define TwRestackWidgets		TwRestackChildrenWidget
#define TwCirculateChildrenWidget(W, up_or_down) Tw_CirculateChildrenWidget(Tw_DefaultD, W, up_or_down)
#define TwCirculateWidgets		TwCirculateChildrenWidget


#define TwRestackChildrenRow(O, N, arrayR) Tw_RestackChildrenRow(Tw_DefaultD, O, N, arrayR)
#define TwRestackRows			TwRestackChildrenRow
#define TwCirculateChildrenRow(P, up_or_down) Tw_CirculateChildrenRow(Tw_DefaultD, P, up_or_down)
#define TwCirculateRows			TwCirculateChildrenRow

#define TwCreateGroup()			Tw_CreateGroup(Tw_DefaultD)
#define TwInsertGadgetGroup(Group, G)	Tw_InsertGadgetGroup(Tw_DefaultD, Group, G)
#define TwRemoveGadgetGroup(Group, G)	Tw_RemoveGadgetGroup(Tw_DefaultD, Group, G)
#define TwGetSelectedGadgetGroup(Group) Tw_GetSelectedGadgetGroup(Tw_DefaultD, Group)
#define TwSetSelectedGadgetGroup(Group, G) Tw_SetSelectedGadgetGroup(Tw_DefaultD, Group, G)

#define TwCreateGadget(      Parent, XWidth, YWidth, TextNormal, Attrib, Flags, Code, \
     ColText, ColTextSelect, ColTextDisabled, ColTextSelectDisabled, Left, Up) \
Tw_CreateGadget(Tw_DefaultD, Parent, XWidth, YWidth, TextNormal, Attrib, Flags, Code, \
     ColText, ColTextSelect, ColTextDisabled, ColTextSelectDisabled, Left, Up)
#define TwDeleteGadget			TwDeleteObj
#define TwRecursiveDeleteGadget		TwRecursiveDeleteWidget
#define TwMapGadget			TwMapWidget
#define TwUnMapGadget			TwUnMapWidget
#define TwSetXYGadget			TwSetXYWidget
#define TwResizeGadget			TwResizeWidget
#define TwScrollGadget			TwScrollWidget
#define TwGetOwnerGadget		TwGetOwnerWidget
#define TwDrawGadget			TwDrawWidget
#define TwDraw2Gadget			TwDraw2Widget
#define TwDrawTextGadget		TwDrawTextWidget
#define TwDrawHWFontGadget		TwDrawHWFontWidget
#define TwDrawHWAttrGadget		TwDrawHWAttrWidget

#define TwLowerGadget			TwLowerWidget
#define TwRaiseGadget			TwRaiseWidget
#define TwRestackChildrenGadget		TwRestackChildrenWidget
#define TwRestackGadgets		TwRestackWidgets
#define TwCirculateChildrenGadget	TwCirculateChildrenWidget
#define TwCirculateGadgets		TwCirculateWidgets

#define TwCreateButtonGadget(Window, BgCol, Col, ColDisabled, Code, Flags, \
			     Left, Up, XWidth, YWidth, Text) \
		Tw_CreateButtonGadget(Tw_DefaultD, \
			     Window, BgCol, Col, ColDisabled, Code, Flags, \
			     Left, Up, XWidth, YWidth, Text)

#define TwSetPressedGadget(Gadget, on)	Tw_SetPressedGadget(Tw_DefaultD, Gadget, on)
#define TwIsPressedGadget(Gadget)	Tw_IsPressedGadget(Tw_DefaultD, Gadget)
#define TwSetToggleGadget(Gadget, on)	Tw_SetToggleGadget(Tw_DefaultD, Gadget, on)
#define TwIsToggleGadget(Gadget)	Tw_IsToggleGadget(Tw_DefaultD, Gadget)
#define TwWriteTextGadget(Gadget, X, Y, Text, Left, Up) \
					Tw_WriteTextGadget(Tw_DefaultD, Gadget, X, Y, Text, Left, Up)
#define TwWriteTextsGadget(Gadget, bitmap, X, Y, Text, Left, Up) \
					Tw_WriteTextsGadget(Tw_DefaultD, Gadget, bitmap, X, Y, Text, Left, Up)
#define TwSetTextGadget(Gadget, X, Y, Text, Left, Up) \
					Tw_SetTextGadget(Tw_DefaultD, Gadget, X, Y, Text, Left, Up)
#define TwSetTextsGadget(Gadget, bitmap, X, Y, Text, Left, Up) \
					Tw_SetTextsGadget(Tw_DefaultD, Gadget, bitmap, X, Y, Text, Left, Up)

#define TwWriteHWFontGadget(Gadget, X, Y, HWFont, Left, Up) \
					Tw_WriteHWFontGadget(Tw_DefaultD, Gadget, X, Y, HWFont, Left, Up)
#define TwWriteHWFontsGadget(Gadget, bitmap, X, Y, HWFont, Left, Up) \
					Tw_WriteHWFontsGadget(Tw_DefaultD, Gadget, bitmap, X, Y, HWFont, Left, Up)
#define TwSetHWFontGadget(Gadget, X, Y, HWFont, Left, Up) \
					Tw_SetHWFontGadget(Tw_DefaultD, Gadget, X, Y, HWFont, Left, Up)
#define TwSetHWFontsGadget(Gadget, bitmap, X, Y, HWFont, Left, Up) \
					Tw_SetHWFontsGadget(Tw_DefaultD, Gadget, bitmap, X, Y, HWFont, Left, Up)

/* backward compatibility. will be removed */
#define TwCreate4MenuRow(Window, Code, FlagActive, TextLen, Text) \
		Tw_Create4MenuRow(Tw_DefaultD, Window, Code, FlagActive, TextLen, Text)
#define TwCreate4Menu2Row(Window, Code, FlagActive, TextLen, Text) \
		Tw_Create4Menu2Row(Tw_DefaultD, Window, Code, FlagActive, TextLen, Text)
#define TwRow4Menu			TwCreate4Menu2Row
#define TwCreateWindow(NameLen, Name, ColName, Menu, ColText, cursorType, \
		       Attrib, Flags, XWidth, YWidth, ScrollBackLines) \
		Tw_CreateWindow(Tw_DefaultD, NameLen, Name, ColName, Menu, ColText, cursorType, \
		       Attrib, Flags, XWidth, YWidth, ScrollBackLines)

#define TwDeleteWindow			TwDeleteObj
#define TwRecursiveDeleteWindow		TwRecursiveDeleteWidget
#define TwMapWindow			TwMapWidget
#define TwUnMapWindow			TwUnMapWidget
#define TwSetXYWindow			TwSetXYWidget
#define TwResizeWindow			TwResizeWidget
#define TwScrollWindow			TwScrollWidget
#define TwGetOwnerWindow		TwGetOwnerWidget
#define TwDrawWindow			TwDrawWidget
#define TwDraw2Window			TwDraw2Widget
#define TwDrawTextWindow		TwDrawTextWidget
#define TwDrawHWFontWindow		TwDrawHWFontWidget
#define TwDrawHWAttrWindow		TwDrawHWAttrWidget

#define TwLowerWindow			TwLowerWidget
#define TwRaiseWindow			TwRaiseWidget
#define TwRestackChildrenWindow		TwRestackChildrenWidget
#define TwRestackWindows		TwRestackWidgets
#define TwCirculateChildrenWindow	TwCirculateChildrenWidget
#define TwCirculateWindows		TwCirculateWidgets

#define TwCreate4MenuWindow(Menu)	Tw_Create4MenuWindow(Tw_DefaultD, Menu)
#define TwWin4Menu			TwCreate4MenuWindow
#define TwWriteAsciiWindow(Window, Len, AsciiSeq) \
		Tw_WriteAsciiWindow(Tw_DefaultD, Window, Len, AsciiSeq)
#define TwWriteStringWindow(Window, Len, String) \
		Tw_WriteStringWindow(Tw_DefaultD, Window, Len, String)
#define TwWriteHWFontWindow(Window, Len, HWFont) \
		Tw_WriteHWFontWindow(Tw_DefaultD, Window, Len, HWFont)
#define TwWriteHWAttrWindow(Window, x, y, Len, Attr) \
		Tw_WriteHWAttrWindow(Tw_DefaultD, Window, x, y, Len, Attr)

#define TwSetColTextWindow(Window, ColText) Tw_SetColTextWindow(Tw_DefaultD, Window, ColText)
#define TwSetColorsWindow(Window, Bitmap, ColGadgets, ColArrows, ColBars, ColTabs, \
			  ColBorder, ColText, ColSelect, ColDisabled, ColSelectDisabled) \
		Tw_SetColorsWindow(Tw_DefaultD, Window, Bitmap, ColGadgets, ColArrows, ColBars, ColTabs, \
			  ColBorder, ColText, ColSelect, ColDisabled, ColSelectDisabled)
#define TwConfigureWindow(Window, Bitmap, Left, Up, MinXWidth, MinYWidth, MaxXWidth, MaxYWidth) \
	Tw_ConfigureWindow(Tw_DefaultD, Window, Bitmap, Left, Up, MinXWidth, MinYWidth, MaxXWidth, MaxYWidth)
#define TwGotoXYWindow(Window, X, Y)	Tw_GotoXYWindow(Tw_DefaultD, Window, X, Y)
#define TwFindRowByCodeWindow(Window, Code)	Tw_FindRowByCodeWindow(Tw_DefaultD, Window, Code)

/* you cannot Create/Delete screens from a client */
#define TwLowerScreen			TwLowerWidget
#define TwRaiseScreen			TwRaiseWidget
#define TwRestackChildrenScreen		TwRestackChildrenWidget
#define TwRestackScreens		TwRestackWidgets
#define TwCirculateChildrenScreen	TwCirculateChildrenWidget
#define TwCirculateScreens		TwCirculateWidgets

#define TwBgImageScreen(Screen, BgWidth, BgHeight, BgImage) \
		Tw_BgImageScreen(Tw_DefaultD, Screen, BgWidth, BgHeight, BgImage)


#define TwCreate4MenuMenuItem(Parent, Window, Flags, NameLen, Name) \
		Tw_Create4MenuMenuItem(Tw_DefaultD, Parent, Window, Flags, NameLen, Name)
#define TwItem4Menu			TwCreate4MenuMenuItem
#define TwCreate4MenuAny(Parent, Window, Code, Flags, Len, Name) \
		Tw_Create4MenuAny(Tw_DefaultD, Parent, Window, Code, Flags, Len, Name);



#define TwCreate4MenuCommonMenuItem(Menu) Tw_Create4MenuCommonMenuItem(Tw_DefaultD, Menu)
#define TwItem4MenuCommon		TwCreate4MenuCommonMenuItem
#define TwDeleteMenuItem(MenuItem)	Tw_DeleteMenuItem(Tw_DefaultD, MenuItem)
#define TwCreateMenu(ColItem, ColSelect, ColDisabled, \
		     ColSelectDisabled, ColShtCut, ColSelShtCut, FlagDefColInfo) \
		Tw_CreateMenu(Tw_DefaultD, ColItem, ColSelect, ColDisabled, \
		     ColSelectDisabled, ColShtCut, ColSelShtCut, FlagDefColInfo)

#define TwSetInfoMenu(Menu, Flags, Len, Text, ColText) \
		Tw_SetInfoMenu(Tw_DefaultD, Menu, Flags, Len, Text, ColText)
#define TwInfo4Menu			TwSetInfoMenu
#define TwDeleteMenu(Menu)		Tw_DeleteMenu(Tw_DefaultD, Menu)
     
#define TwCreateMsgPort(NameLen, ProgramName, PauseSec, PauseFraction, WakeUp) \
		Tw_CreateMsgPort(Tw_DefaultD, NameLen, ProgramName, PauseSec, PauseFraction, WakeUp)
#define TwDeleteMsgPort(MsgPort)	Tw_DeleteMsgPort(Tw_DefaultD, MsgPort)
#define TwFindMsgPort(Prev, NameLen, ProgramName) \
		Tw_FindMsgPort(Tw_DefaultD, Prev, NameLen, ProgramName)
     

#define TwPrevObj(Obj)			Tw_PrevObj(Tw_DefaultD, Obj)
#define TwNextObj(Obj)			Tw_NextObj(Tw_DefaultD, Obj)
#define TwParentObj(Obj)		Tw_ParentObj(Tw_DefaultD, Obj)
#define TwO_PrevWidget(Widget)		Tw_O_PrevWidget(Tw_DefaultD, Widget)
#define TwO_NextWidget(Widget)		Tw_O_NextWidget(Tw_DefaultD, Widget)
#define TwOwnerWidget(Widget)		Tw_OwnerWidget(Tw_DefaultD, Widget)
#define TwG_PrevGadget(Gadget)		Tw_G_PrevGadget(Tw_DefaultD, Gadget)
#define TwG_NextGadget(Gadget)		Tw_G_NextGadget(Tw_DefaultD, Gadget)
#define TwGroupGadget(Gadget)		Tw_GroupGadget(Tw_DefaultD, Gadget)

#define TwFirstScreen()			Tw_FirstScreen(Tw_DefaultD)
#define TwFirstWidget(Parent)		Tw_FirstWidget(Tw_DefaultD, Parent)
#define TwFirstMsgPort()		Tw_FirstMsgPort(Tw_DefaultD)
#define TwFirstMenu(MsgPort)		Tw_FirstMenu(Tw_DefaultD, MsgPort)
#define TwFirstW(MsgPort)		Tw_FirstW(Tw_DefaultD, MsgPort)
#define TwFirstGroup(MsgPort)		Tw_FirstGroup(Tw_DefaultD, MsgPort)
#define TwFirstMutex(MsgPort)		Tw_FirstMutex(Tw_DefaultD, MsgPort)
#define TwFirstMenuItem(Menu)		Tw_FirstMenuItem(Tw_DefaultD, Menu)
#define TwFirstGadget(Group)		Tw_FirstMenu(Tw_DefaultD, Group)

#define TwGetDisplayWidth()		Tw_GetDisplayWidth(Tw_DefaultD)
#define TwGetDisplayHeight()		Tw_GetDisplayHeight(Tw_DefaultD)
#define TwGetAll()			Tw_GetAll(Tw_DefaultD)

#define TwCreateMsg(Type, Len)		Tw_CreateMsg(Tw_DefaultD, Type, Len)
#define TwDeleteMsg(Msg)		Tw_DeleteMsg(Tw_DefaultD, Msg)
#define TwSendMsg(MsgPort, Msg)		Tw_SendMsg(Tw_DefaultD, MsgPort, Msg)
#define TwBlindSendMsg(MsgPort, Msg)	Tw_BlindSendMsg(Tw_DefaultD, MsgPort, Msg)

#define TwGetOwnerSelection()		Tw_GetOwnerSelection(Tw_DefaultD)
#define TwSetOwnerSelection(Time, Frac) Tw_SetOwnerSelection(Tw_DefaultD, Time, Frac)
#define TwRequestSelection(Owner, ReqPrivate)	Tw_RequestSelection(Tw_DefaultD, Owner, ReqPrivate)
#define TwNotifySelection(Requestor, ReqPrivate, Magic, MIME, Len, Data) \
					Tw_NotifySelection(Tw_DefaultD, Requestor, ReqPrivate, \
							   Magic, MIME, Len, Data)

#define TwSetServerUid(uid, privileges)	Tw_SetServerUid(Tw_DefaultD, uid, privileges)

#define TwOpen(d)			(!!(Tw_DefaultD = Tw_Open(d)))
#define TwClose()			(Tw_Close(Tw_DefaultD), Tw_DefaultD = (tdisplay)0)
#define TwConnectionFd()		Tw_ConnectionFd(Tw_DefaultD)
#define TwLibraryVersion()		Tw_LibraryVersion(Tw_DefaultD)
#define TwServerVersion()		Tw_ServerVersion(Tw_DefaultD)
#define TwFlush()			Tw_Flush(Tw_DefaultD)
#define TwTimidFlush()			Tw_TimidFlush(Tw_DefaultD)
#define TwSync()			Tw_Sync(Tw_DefaultD)
#define TwInPanic()			Tw_InPanic(Tw_DefaultD)
#define TwPendingMsg()			Tw_PendingMsg(Tw_DefaultD)
#define TwPeekMsg()			Tw_PeekMsg(Tw_DefaultD)
#define TwReadMsg(Wait)			Tw_ReadMsg(Tw_DefaultD, Wait)
#define TwCloneReadMsg(Wait)		Tw_CloneReadMsg(Tw_DefaultD, Wait)

#define TwAddKeyboardListener(               Widget, Key,  ShiftFlags, Listener, Arg) \
       Tw_AddKeyboardListener(Tw_DefaultD,   Widget, Key,  ShiftFlags, Listener, Arg)

#define TwAddMouseListener(             Widget, Code, ShiftFlags, Listener, Arg) \
       Tw_AddMouseListener(Tw_DefaultD, Widget, Code, ShiftFlags, Listener, Arg)

#define TwAddControlListener(             Widget, Code, Listener, Arg) \
       Tw_AddControlListener(Tw_DefaultD, Widget, Code, Listener, Arg)

#define TwAddClientMsgListener(             Widget, Code, Listener, Arg) \
       Tw_AddClientMsgListener(Tw_DefaultD, Widget, Code, Listener, Arg)

#define TwAddDisplayListener(             Code, Listener, Arg) \
       Tw_AddDisplayListener(Tw_DefaultD, Code, Listener, Arg)

#define TwAddWidgetListener(             Widget, Code, Listener, Arg) \
       Tw_AddWidgetListener(Tw_DefaultD, Widget, Code, Listener, Arg)

#define TwAddGadgetListener(             Widget, Code, Listener, Arg) \
       Tw_AddGadgetListener(Tw_DefaultD, Widget, Code, Listener, Arg)

#define TwAddMenuListener(             Widget, Menu, Code, Listener, Arg) \
       Tw_AddMenuListener(Tw_DefaultD, Widget, Menu, Code, Listener, Arg)

#define TwAddSelectionListener(             Widget, Listener, Arg) \
       Tw_AddSelectionListener(Tw_DefaultD, Widget, Listener, Arg)
       
#define TwAddSelectionNotifyListener(             Listener, Arg) \
       Tw_AddSelectionNotifyListener(Tw_DefaultD, Listener, Arg)
       
#define TwAddSelectionRequestListener(             Listener, Arg) \
       Tw_AddSelectionRequestListener(Tw_DefaultD, Listener, Arg)

#define TwInsertListener(L)		Tw_InsertListener(Tw_DefaultD, L)
#define TwRemoveListener(L)		Tw_RemoveListener(Tw_DefaultD, L)
#define TwCreateListener(Type, E, L, Arg) Tw_CreateListener(Tw_DefaultD, Type, E, L, Arg)
#define TwSetTEListener(L, Type, E)	Tw_SetTEListener(Tw_DefaultD, L, Type, E)
#define TwDeleteListener(L)		Tw_DeleteListener(Tw_DefaultD, L)

#define TwSetDefaultListener(L, arg)	Tw_SetDefaultListener(Tw_DefaultD, L, arg)

#define TwDispatchMsg(Msg)		Tw_DispatchMsg(Tw_DefaultD, Msg)
#define TwMainLoop()			Tw_MainLoop(Tw_DefaultD)
#define TwExitMainLoop()		Tw_ExitMainLoop(Tw_DefaultD)

#endif /* _TW_1_H */

