



/* This file was automatically generated from m4/socket1.m4, do not edit! */






































case order_FindFunction:
    a[0]._ = (uldat)sockFindFunction((byte)a[1]._, (CONST byte *)a[2].V, (byte)a[3]._, (CONST byte *)a[4].V);
    break;


case order_SyncSocket:
    a[0]._ = (uldat)sockSyncSocket();
    break;


case order_ServerSizeof:
    a[0]._ = (uldat)sockServerSizeof((byte)a[1]._);
    break;


case order_CanCompress:
    a[0]._ = (uldat)sockCanCompress();
    break;

case order_DoCompress:
    a[0]._ = (uldat)sockDoCompress((byte)a[1]._);
    break;


case order_NeedResizeDisplay:
    sockNeedResizeDisplay();
    break;


case order_AttachHW:
    sockAttachHW((uldat)a[1]._, (CONST byte *)a[2].V, (byte)a[3]._);
    break;

case order_DetachHW:
    a[0]._ = (uldat)sockDetachHW((uldat)a[1]._, (CONST byte *)a[2].V);
    break;


case order_SetFontTranslation:
    sockSetFontTranslation((CONST byte *)a[1].V);
    break;

case order_SetUniFontTranslation:
    sockSetUniFontTranslation((CONST hwfont *)a[1].V);
    break;


case order_DeleteObj:
    sockDeleteObj((obj)a[1].x);
    break;

case order_ChangeFieldObj:
    Act(ChangeField,(obj)a[1].x)((obj)a[1].x, (udat)a[2]._, (uldat)a[3]._, (uldat)a[4]._);
    break;



case order_CreateWidget:
    a[0].x = (obj)sockCreateWidget((dat)a[1]._, (dat)a[2]._, (uldat)a[3]._, (uldat)a[4]._, (dat)a[5]._, (dat)a[6]._, (hwattr)a[7]._);
    break;

case order_RecursiveDeleteWidget:
    sockRecursiveDeleteWidget((widget)a[1].x);
    break;

case order_MapWidget:
    Act(Map,(widget)a[1].x)((widget)a[1].x, (widget)a[2].x);
    break;

case order_UnMapWidget:
    Act(UnMap,(widget)a[1].x)((widget)a[1].x);
    break;

case order_SetXYWidget:
    sockSetXYWidget((widget)a[1].x, (dat)a[2]._, (dat)a[3]._);
    break;

case order_ResizeWidget:
    sockResizeWidget((widget)a[1].x, (dat)a[2]._, (dat)a[3]._);
    break;

case order_GetOwnerWidget:
    a[0].x = (obj)sockGetOwnerWidget((widget)a[1].x);
    break;

case order_ExposeWidget:
    sockExposeWidget((widget)a[1].x, (dat)a[2]._, (dat)a[3]._, (dat)a[4]._, (dat)a[5]._, (CONST byte *)a[6].V, (CONST hwfont *)a[7].V, (CONST hwattr *)a[8].V);
    break;


case order_FocusSubWidget:
    sockFocusSubWidget((widget)a[1].x);
    break;

case order_FindWidgetAtWidget:
    a[0].x = (obj)Act(FindWidgetAt,(widget)a[1].x)((widget)a[1].x, (dat)a[2]._, (dat)a[3]._);
    break;


case order_RaiseWidget:
    Act(Raise,(widget)a[1].x)((widget)a[1].x);
    break;

case order_LowerWidget:
    Act(Lower,(widget)a[1].x)((widget)a[1].x);
    break;

case order_RestackChildrenWidget:
    sockRestackChildrenWidget((widget)a[1].x, (uldat)a[2]._, (CONST widget *)a[3].V);
    break;

case order_CirculateChildrenWidget:
    sockCirculateChildrenWidget((widget)a[1].x, (byte)a[2]._);
    break;



case order_CreateGadget:
    a[0].x = (obj)sockCreateGadget((widget)a[1].x, (dat)a[2]._, (dat)a[3]._, (CONST byte *)a[4].V, (uldat)a[5]._, (uldat)a[6]._, (udat)a[7]._, (hwcol)a[8]._, (hwcol)a[9]._, (hwcol)a[10]._, (hwcol)a[11]._, (dat)a[12]._, (dat)a[13]._);
    break;


case order_CreateButtonGadget:
    a[0].x = (obj)Do(CreateButton,Gadget)(FnGadget, (widget)a[1].x, (dat)a[2]._, (dat)a[3]._, (CONST byte *)a[4].V, (uldat)a[5]._, (udat)a[6]._, (hwcol)a[7]._, (hwcol)a[8]._, (hwcol)a[9]._, (dat)a[10]._, (dat)a[11]._);
    break;


case order_WriteTextsGadget:
    Act(WriteTexts,(gadget)a[1].x)((gadget)a[1].x, (byte)a[2]._, (dat)a[3]._, (dat)a[4]._, (CONST byte *)a[5].V, (dat)a[6]._, (dat)a[7]._);
    break;

case order_WriteHWFontsGadget:
    Act(WriteHWFonts,(gadget)a[1].x)((gadget)a[1].x, (byte)a[2]._, (dat)a[3]._, (dat)a[4]._, (CONST hwfont *)a[5].V, (dat)a[6]._, (dat)a[7]._);
    break;


case order_CreateWindow:
    a[0].x = (obj)Do(Create,Window)(FnWindow, (dat)a[1]._, (CONST byte *)a[2].V, (CONST hwcol *)a[3].V, (menu)a[4].x, (hwcol)a[5]._, (uldat)a[6]._, (uldat)a[7]._, (uldat)a[8]._, (dat)a[9]._, (dat)a[10]._, (dat)a[11]._);
    break;

case order_Create4MenuWindow:
    a[0].x = (obj)Do(Create4Menu,Window)(FnWindow, (menu)a[1].x);
    break;


case order_WriteAsciiWindow:
    sockWriteAsciiWindow((window)a[1].x, (ldat)a[2]._, (CONST byte *)a[3].V);
    break;

case order_WriteStringWindow:
    sockWriteStringWindow((window)a[1].x, (ldat)a[2]._, (CONST byte *)a[3].V);
    break;

case order_WriteHWFontWindow:
    sockWriteHWFontWindow((window)a[1].x, (ldat)a[2]._, (CONST hwfont *)a[3].V);
    break;

case order_WriteHWAttrWindow:
    sockWriteHWAttrWindow((window)a[1].x, (dat)a[2]._, (dat)a[3]._, (ldat)a[4]._, (CONST hwattr *)a[5].V);
    break;


case order_GotoXYWindow:
    Act(GotoXY,(window)a[1].x)((window)a[1].x, (ldat)a[2]._, (ldat)a[3]._);
    break;

case order_SetColTextWindow:
    Act(SetColText,(window)a[1].x)((window)a[1].x, (hwcol)a[2]._);
    break;

case order_SetColorsWindow:
    Act(SetColors,(window)a[1].x)((window)a[1].x, (udat)a[2]._, (hwcol)a[3]._, (hwcol)a[4]._, (hwcol)a[5]._, (hwcol)a[6]._, (hwcol)a[7]._, (hwcol)a[8]._, (hwcol)a[9]._, (hwcol)a[10]._, (hwcol)a[11]._);
    break;

case order_ConfigureWindow:
    Act(Configure,(window)a[1].x)((window)a[1].x, (byte)a[2]._, (dat)a[3]._, (dat)a[4]._, (dat)a[5]._, (dat)a[6]._, (dat)a[7]._, (dat)a[8]._);
    break;

case order_FindRowByCodeWindow:
    a[0].x = (obj)sockFindRowByCodeWindow((window)a[1].x, (dat)a[2]._);
    break;


case order_CreateGroup:
    a[0].x = (obj)sockCreateGroup();
    break;

case order_InsertGadgetGroup:
    Act(InsertGadget,(group)a[1].x)((group)a[1].x, (gadget)a[2].x);
    break;

case order_RemoveGadgetGroup:
    Act(RemoveGadget,(group)a[1].x)((group)a[1].x, (gadget)a[2].x);
    break;


case order_GetSelectedGadgetGroup:
    a[0].x = (obj)Act(GetSelectedGadget,(group)a[1].x)((group)a[1].x);
    break;

case order_SetSelectedGadgetGroup:
    Act(SetSelectedGadget,(group)a[1].x)((group)a[1].x, (gadget)a[2].x);
    break;


case order_Create4MenuRow:
    sockCreate4MenuRow((window)a[1].x, (udat)a[2]._, (byte)a[3]._, (uldat)a[4]._, (CONST byte *)a[5].V);
    break;


case order_RaiseRow:
    Act(Raise,(row)a[1].x)((row)a[1].x);
    break;

case order_LowerRow:
    Act(Lower,(row)a[1].x)((row)a[1].x);
    break;

case order_RestackChildrenRow:
    sockRestackChildrenRow((obj)a[1].x, (uldat)a[2]._, (CONST row *)a[3].V);
    break;
 
case order_CirculateChildrenRow:
    sockCirculateChildrenRow((obj)a[1].x, (byte)a[2]._);
    break;


case order_Create4MenuMenuItem:
    a[0].x = (obj)sockCreate4MenuMenuItem((obj)a[1].x, (window)a[2].x, (byte)a[3]._, (dat)a[4]._, (CONST byte *)a[5].V);
    break;

case order_Create4MenuCommonMenuItem:
    a[0]._ = (uldat)Do(Create4MenuCommon,MenuItem)(FnMenuItem, (menu)a[1].x);
    break;


case order_CreateMenu:
    a[0].x = (obj)sockCreateMenu((hwcol)a[1]._, (hwcol)a[2]._, (hwcol)a[3]._, (hwcol)a[4]._, (hwcol)a[5]._, (hwcol)a[6]._, (byte)a[7]._);
    break;

case order_SetInfoMenu:
    Act(SetInfo,(menu)a[1].x)((menu)a[1].x, (byte)a[2]._, (ldat)a[3]._, (CONST byte *)a[4].V, (CONST hwcol *)a[5].V);
    break;


case order_CreateMsgPort:
    a[0].x = (obj)sockCreateMsgPort((byte)a[1]._, (CONST byte *)a[2].V, (time_t)a[3]._, (frac_t)a[4]._, (byte)a[5]._);
    break;

case order_FindMsgPort:
    a[0].x = (obj)sockFindMsgPort((msgport)a[1].x, (byte)a[2]._, (CONST byte *)a[3].V);
    break;


case order_BgImageScreen:
    Act(BgImage,(screen)a[1].x)((screen)a[1].x, (dat)a[2]._, (dat)a[3]._, (CONST hwattr *)a[4].V);
    break;


case order_PrevObj:
    a[0].x = (obj)sockPrevObj((obj)a[1].x);
    break;

case order_NextObj:
    a[0].x = (obj)sockNextObj((obj)a[1].x);
    break;

case order_ParentObj:
    a[0].x = (obj)sockParentObj((obj)a[1].x);
    break;


case order_G_PrevGadget:
    a[0].x = (obj)sockG_PrevGadget((gadget )a[1].x);
    break;

case order_G_NextGadget:
    a[0].x = (obj)sockG_NextGadget((gadget )a[1].x);
    break;

case order_GroupGadget:
    a[0].x = (obj)sockGroupGadget((gadget )a[1].x);
    break;


case order_O_PrevWidget:
    a[0].x = (obj)sockO_PrevWidget((widget )a[1].x);
    break;

case order_O_NextWidget:
    a[0].x = (obj)sockO_NextWidget((widget )a[1].x);
    break;

case order_OwnerWidget:
    a[0].x = (obj)sockOwnerWidget((widget )a[1].x);
    break;


case order_FirstScreen:
    a[0].x = (obj)sockFirstScreen();
    break;

case order_FirstWidget:
    a[0].x = (obj)sockFirstWidget((widget )a[1].x);
    break;

case order_FirstMsgPort:
    a[0].x = (obj)sockFirstMsgPort();
    break;

case order_FirstMenu:
    a[0].x = (obj)sockFirstMenu((msgport)a[1].x);
    break;

case order_FirstW:
    a[0].x = (obj)sockFirstW((msgport)a[1].x);
    break;

case order_FirstGroup:
    a[0].x = (obj)sockFirstGroup((msgport)a[1].x);
    break;

case order_FirstMutex:
    a[0].x = (obj)sockFirstMutex((msgport)a[1].x);
    break;

case order_FirstMenuItem:
    a[0].x = (obj)sockFirstMenuItem((menu   )a[1].x);
    break;

case order_FirstGadget:
    a[0].x = (obj)sockFirstGadget((group  )a[1].x);
    break;


case order_GetDisplayWidth:
    a[0]._ = (uldat)sockGetDisplayWidth();
    break;

case order_GetDisplayHeight:
    a[0]._ = (uldat)sockGetDisplayHeight();
    break;

case order_GetAll:
    a[0].x = (obj)sockGetAll();
    break;


case order_SendToMsgPort:
    a[0]._ = (uldat)sockSendToMsgPort((msgport)a[1].x, (udat)a[2]._, (CONST byte *)a[3].V);
    break;

case order_BlindSendToMsgPort:
    sockBlindSendToMsgPort((msgport)a[1].x, (udat)a[2]._, (CONST byte *)a[3].V);
    break;


case order_GetOwnerSelection:
    a[0].x = (obj)sockGetOwnerSelection();
    break;

case order_SetOwnerSelection:
    sockSetOwnerSelection((time_t)a[1]._, (frac_t)a[2]._);
    break;

case order_RequestSelection:
    sockRequestSelection((obj)a[1].x, (uldat)a[2]._);
    break;

case order_NotifySelection:
    sockNotifySelection((obj)a[1].x, (uldat)a[2]._, (uldat)a[3]._, (CONST byte *)a[4].V, (uldat)a[5]._, (CONST byte *)a[6].V);
    break;


case order_SetServerUid:
    a[0]._ = (uldat)sockSetServerUid((uldat)a[1]._, (byte)a[2]._);
    break;




