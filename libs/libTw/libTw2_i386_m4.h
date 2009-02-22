



/* This file was automatically generated with m4 from m4/libTw2_i386.m4, do not edit! */

/*
 *
 *  Copyright (C) 2002 by Massimiliano Ghilardi
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 */
/*
 * hand-optimized assembler functions for i386 with gcc
 */




	.align 4
	.type	 _Tw_i386_call_1,@function
_Tw_i386_call_1:
	pushl $1
	call _Tw_EncodeCall
	popl %ecx
	popl %ecx
	ret
.L_i386_call_1:
	.size	 _Tw_i386_call_1,.L_i386_call_1-_Tw_i386_call_1














	.align 4
.globl _Tw_FindFunction
	.type	 _Tw_FindFunction,@function
_Tw_FindFunction:
	pushl $0
	jmp _Tw_i386_call_1
.L_FindFunction:
	.size	 _Tw_FindFunction,.L_FindFunction-_Tw_FindFunction




	.align 4
.globl _Tw_SyncSocket
	.type	 _Tw_SyncSocket,@function
_Tw_SyncSocket:
	pushl $1
	jmp _Tw_i386_call_1
.L_SyncSocket:
	.size	 _Tw_SyncSocket,.L_SyncSocket-_Tw_SyncSocket




	.align 4
.globl Tw_ServerSizeof
	.type	 Tw_ServerSizeof,@function
Tw_ServerSizeof:
	pushl $2
	jmp _Tw_i386_call_0
.L_ServerSizeof:
	.size	 Tw_ServerSizeof,.L_ServerSizeof-Tw_ServerSizeof




	.align 4
.globl Tw_CanCompress
	.type	 Tw_CanCompress,@function
Tw_CanCompress:
	pushl $3
	jmp _Tw_i386_call_0
.L_CanCompress:
	.size	 Tw_CanCompress,.L_CanCompress-Tw_CanCompress



	.align 4
.globl Tw_DoCompress
	.type	 Tw_DoCompress,@function
Tw_DoCompress:
	pushl $4
	jmp _Tw_i386_call_0
.L_DoCompress:
	.size	 Tw_DoCompress,.L_DoCompress-Tw_DoCompress




	.align 4
.globl Tw_NeedResizeDisplay
	.type	 Tw_NeedResizeDisplay,@function
Tw_NeedResizeDisplay:
	pushl $5
	jmp _Tw_i386_call_2
.L_NeedResizeDisplay:
	.size	 Tw_NeedResizeDisplay,.L_NeedResizeDisplay-Tw_NeedResizeDisplay




	.align 4
.globl Tw_AttachHW
	.type	 Tw_AttachHW,@function
Tw_AttachHW:
	pushl $6
	jmp _Tw_i386_call_2
.L_AttachHW:
	.size	 Tw_AttachHW,.L_AttachHW-Tw_AttachHW



	.align 4
.globl Tw_DetachHW
	.type	 Tw_DetachHW,@function
Tw_DetachHW:
	pushl $7
	jmp _Tw_i386_call_0
.L_DetachHW:
	.size	 Tw_DetachHW,.L_DetachHW-Tw_DetachHW




	.align 4
.globl Tw_SetFontTranslation
	.type	 Tw_SetFontTranslation,@function
Tw_SetFontTranslation:
	pushl $8
	jmp _Tw_i386_call_2
.L_SetFontTranslation:
	.size	 Tw_SetFontTranslation,.L_SetFontTranslation-Tw_SetFontTranslation



	.align 4
.globl Tw_SetHWFontTranslation
	.type	 Tw_SetHWFontTranslation,@function
Tw_SetHWFontTranslation:
	pushl $9
	jmp _Tw_i386_call_2
.L_SetHWFontTranslation:
	.size	 Tw_SetHWFontTranslation,.L_SetHWFontTranslation-Tw_SetHWFontTranslation




	.align 4
.globl Tw_DeleteObj
	.type	 Tw_DeleteObj,@function
Tw_DeleteObj:
	pushl $10
	jmp _Tw_i386_call_2
.L_DeleteObj:
	.size	 Tw_DeleteObj,.L_DeleteObj-Tw_DeleteObj



	.align 4
.globl Tw_ChangeFieldObj
	.type	 Tw_ChangeFieldObj,@function
Tw_ChangeFieldObj:
	pushl $11
	jmp _Tw_i386_call_2
.L_ChangeFieldObj:
	.size	 Tw_ChangeFieldObj,.L_ChangeFieldObj-Tw_ChangeFieldObj




	.align 4
.globl Tw_CreateWidget
	.type	 Tw_CreateWidget,@function
Tw_CreateWidget:
	pushl $12
	jmp _Tw_i386_call_0
.L_CreateWidget:
	.size	 Tw_CreateWidget,.L_CreateWidget-Tw_CreateWidget



	.align 4
.globl Tw_RecursiveDeleteWidget
	.type	 Tw_RecursiveDeleteWidget,@function
Tw_RecursiveDeleteWidget:
	pushl $13
	jmp _Tw_i386_call_2
.L_RecursiveDeleteWidget:
	.size	 Tw_RecursiveDeleteWidget,.L_RecursiveDeleteWidget-Tw_RecursiveDeleteWidget


	.align 4
.globl Tw_MapWidget
	.type	 Tw_MapWidget,@function
Tw_MapWidget:
	pushl $14
	jmp _Tw_i386_call_2
.L_MapWidget:
	.size	 Tw_MapWidget,.L_MapWidget-Tw_MapWidget


	.align 4
.globl Tw_UnMapWidget
	.type	 Tw_UnMapWidget,@function
Tw_UnMapWidget:
	pushl $15
	jmp _Tw_i386_call_2
.L_UnMapWidget:
	.size	 Tw_UnMapWidget,.L_UnMapWidget-Tw_UnMapWidget


	.align 4
.globl Tw_SetXYWidget
	.type	 Tw_SetXYWidget,@function
Tw_SetXYWidget:
	pushl $16
	jmp _Tw_i386_call_2
.L_SetXYWidget:
	.size	 Tw_SetXYWidget,.L_SetXYWidget-Tw_SetXYWidget


	.align 4
.globl Tw_ResizeWidget
	.type	 Tw_ResizeWidget,@function
Tw_ResizeWidget:
	pushl $17
	jmp _Tw_i386_call_2
.L_ResizeWidget:
	.size	 Tw_ResizeWidget,.L_ResizeWidget-Tw_ResizeWidget


	.align 4
.globl Tw_ScrollWidget
	.type	 Tw_ScrollWidget,@function
Tw_ScrollWidget:
	pushl $18
	jmp _Tw_i386_call_2
.L_ScrollWidget:
	.size	 Tw_ScrollWidget,.L_ScrollWidget-Tw_ScrollWidget


	.align 4
.globl Tw_DrawWidget
	.type	 Tw_DrawWidget,@function
Tw_DrawWidget:
	pushl $19
	jmp _Tw_i386_call_2
.L_DrawWidget:
	.size	 Tw_DrawWidget,.L_DrawWidget-Tw_DrawWidget



	.align 4
.globl Tw_SetVisibleWidget
	.type	 Tw_SetVisibleWidget,@function
Tw_SetVisibleWidget:
	pushl $20
	jmp _Tw_i386_call_2
.L_SetVisibleWidget:
	.size	 Tw_SetVisibleWidget,.L_SetVisibleWidget-Tw_SetVisibleWidget



	.align 4
.globl Tw_FocusSubWidget
	.type	 Tw_FocusSubWidget,@function
Tw_FocusSubWidget:
	pushl $21
	jmp _Tw_i386_call_2
.L_FocusSubWidget:
	.size	 Tw_FocusSubWidget,.L_FocusSubWidget-Tw_FocusSubWidget


	.align 4
.globl Tw_FindWidgetAtWidget
	.type	 Tw_FindWidgetAtWidget,@function
Tw_FindWidgetAtWidget:
	pushl $22
	jmp _Tw_i386_call_0
.L_FindWidgetAtWidget:
	.size	 Tw_FindWidgetAtWidget,.L_FindWidgetAtWidget-Tw_FindWidgetAtWidget



	.align 4
.globl Tw_RaiseWidget
	.type	 Tw_RaiseWidget,@function
Tw_RaiseWidget:
	pushl $23
	jmp _Tw_i386_call_2
.L_RaiseWidget:
	.size	 Tw_RaiseWidget,.L_RaiseWidget-Tw_RaiseWidget


	.align 4
.globl Tw_LowerWidget
	.type	 Tw_LowerWidget,@function
Tw_LowerWidget:
	pushl $24
	jmp _Tw_i386_call_2
.L_LowerWidget:
	.size	 Tw_LowerWidget,.L_LowerWidget-Tw_LowerWidget


	.align 4
.globl Tw_RestackChildrenWidget
	.type	 Tw_RestackChildrenWidget,@function
Tw_RestackChildrenWidget:
	pushl $25
	jmp _Tw_i386_call_2
.L_RestackChildrenWidget:
	.size	 Tw_RestackChildrenWidget,.L_RestackChildrenWidget-Tw_RestackChildrenWidget


	.align 4
.globl Tw_CirculateChildrenWidget
	.type	 Tw_CirculateChildrenWidget,@function
Tw_CirculateChildrenWidget:
	pushl $26
	jmp _Tw_i386_call_2
.L_CirculateChildrenWidget:
	.size	 Tw_CirculateChildrenWidget,.L_CirculateChildrenWidget-Tw_CirculateChildrenWidget




	.align 4
.globl Tw_CreateGadget
	.type	 Tw_CreateGadget,@function
Tw_CreateGadget:
	pushl $27
	jmp _Tw_i386_call_0
.L_CreateGadget:
	.size	 Tw_CreateGadget,.L_CreateGadget-Tw_CreateGadget



	.align 4
.globl Tw_CreateButtonGadget
	.type	 Tw_CreateButtonGadget,@function
Tw_CreateButtonGadget:
	pushl $28
	jmp _Tw_i386_call_0
.L_CreateButtonGadget:
	.size	 Tw_CreateButtonGadget,.L_CreateButtonGadget-Tw_CreateButtonGadget



	.align 4
.globl Tw_WriteTextsGadget
	.type	 Tw_WriteTextsGadget,@function
Tw_WriteTextsGadget:
	pushl $29
	jmp _Tw_i386_call_2
.L_WriteTextsGadget:
	.size	 Tw_WriteTextsGadget,.L_WriteTextsGadget-Tw_WriteTextsGadget


	.align 4
.globl Tw_WriteHWFontsGadget
	.type	 Tw_WriteHWFontsGadget,@function
Tw_WriteHWFontsGadget:
	pushl $30
	jmp _Tw_i386_call_2
.L_WriteHWFontsGadget:
	.size	 Tw_WriteHWFontsGadget,.L_WriteHWFontsGadget-Tw_WriteHWFontsGadget



	.align 4
.globl Tw_CreateWindow
	.type	 Tw_CreateWindow,@function
Tw_CreateWindow:
	pushl $31
	jmp _Tw_i386_call_0
.L_CreateWindow:
	.size	 Tw_CreateWindow,.L_CreateWindow-Tw_CreateWindow


	.align 4
.globl Tw_Create4MenuWindow
	.type	 Tw_Create4MenuWindow,@function
Tw_Create4MenuWindow:
	pushl $32
	jmp _Tw_i386_call_0
.L_Create4MenuWindow:
	.size	 Tw_Create4MenuWindow,.L_Create4MenuWindow-Tw_Create4MenuWindow



	.align 4
.globl Tw_WriteAsciiWindow
	.type	 Tw_WriteAsciiWindow,@function
Tw_WriteAsciiWindow:
	pushl $33
	jmp _Tw_i386_call_2
.L_WriteAsciiWindow:
	.size	 Tw_WriteAsciiWindow,.L_WriteAsciiWindow-Tw_WriteAsciiWindow


	.align 4
.globl Tw_WriteStringWindow
	.type	 Tw_WriteStringWindow,@function
Tw_WriteStringWindow:
	pushl $34
	jmp _Tw_i386_call_2
.L_WriteStringWindow:
	.size	 Tw_WriteStringWindow,.L_WriteStringWindow-Tw_WriteStringWindow


	.align 4
.globl Tw_WriteHWFontWindow
	.type	 Tw_WriteHWFontWindow,@function
Tw_WriteHWFontWindow:
	pushl $35
	jmp _Tw_i386_call_2
.L_WriteHWFontWindow:
	.size	 Tw_WriteHWFontWindow,.L_WriteHWFontWindow-Tw_WriteHWFontWindow


	.align 4
.globl Tw_WriteHWAttrWindow
	.type	 Tw_WriteHWAttrWindow,@function
Tw_WriteHWAttrWindow:
	pushl $36
	jmp _Tw_i386_call_2
.L_WriteHWAttrWindow:
	.size	 Tw_WriteHWAttrWindow,.L_WriteHWAttrWindow-Tw_WriteHWAttrWindow



	.align 4
.globl Tw_GotoXYWindow
	.type	 Tw_GotoXYWindow,@function
Tw_GotoXYWindow:
	pushl $37
	jmp _Tw_i386_call_2
.L_GotoXYWindow:
	.size	 Tw_GotoXYWindow,.L_GotoXYWindow-Tw_GotoXYWindow


	.align 4
.globl Tw_SetTitleWindow
	.type	 Tw_SetTitleWindow,@function
Tw_SetTitleWindow:
	pushl $38
	jmp _Tw_i386_call_2
.L_SetTitleWindow:
	.size	 Tw_SetTitleWindow,.L_SetTitleWindow-Tw_SetTitleWindow


	.align 4
.globl Tw_SetColTextWindow
	.type	 Tw_SetColTextWindow,@function
Tw_SetColTextWindow:
	pushl $39
	jmp _Tw_i386_call_2
.L_SetColTextWindow:
	.size	 Tw_SetColTextWindow,.L_SetColTextWindow-Tw_SetColTextWindow


	.align 4
.globl Tw_SetColorsWindow
	.type	 Tw_SetColorsWindow,@function
Tw_SetColorsWindow:
	pushl $40
	jmp _Tw_i386_call_2
.L_SetColorsWindow:
	.size	 Tw_SetColorsWindow,.L_SetColorsWindow-Tw_SetColorsWindow


	.align 4
.globl Tw_ConfigureWindow
	.type	 Tw_ConfigureWindow,@function
Tw_ConfigureWindow:
	pushl $41
	jmp _Tw_i386_call_2
.L_ConfigureWindow:
	.size	 Tw_ConfigureWindow,.L_ConfigureWindow-Tw_ConfigureWindow


	.align 4
.globl Tw_FindRowByCodeWindow
	.type	 Tw_FindRowByCodeWindow,@function
Tw_FindRowByCodeWindow:
	pushl $42
	jmp _Tw_i386_call_0
.L_FindRowByCodeWindow:
	.size	 Tw_FindRowByCodeWindow,.L_FindRowByCodeWindow-Tw_FindRowByCodeWindow



	.align 4
.globl Tw_CreateGroup
	.type	 Tw_CreateGroup,@function
Tw_CreateGroup:
	pushl $43
	jmp _Tw_i386_call_0
.L_CreateGroup:
	.size	 Tw_CreateGroup,.L_CreateGroup-Tw_CreateGroup


  
	.align 4
	.type	 _Tw_i386_call_2,@function
_Tw_i386_call_2:
	pushl $2
	call _Tw_EncodeCall
	popl %ecx
	popl %ecx
	ret
.L_i386_call_2:
	.size	 _Tw_i386_call_2,.L_i386_call_2-_Tw_i386_call_2

  
	.align 4
	.type	 _Tw_i386_call_0,@function
_Tw_i386_call_0:
	pushl $0
	call _Tw_EncodeCall
	popl %ecx
	popl %ecx
	ret
.L_i386_call_0:
	.size	 _Tw_i386_call_0,.L_i386_call_0-_Tw_i386_call_0


	.align 4
.globl Tw_InsertGadgetGroup
	.type	 Tw_InsertGadgetGroup,@function
Tw_InsertGadgetGroup:
	pushl $44
	jmp _Tw_i386_call_2
.L_InsertGadgetGroup:
	.size	 Tw_InsertGadgetGroup,.L_InsertGadgetGroup-Tw_InsertGadgetGroup


	.align 4
.globl Tw_RemoveGadgetGroup
	.type	 Tw_RemoveGadgetGroup,@function
Tw_RemoveGadgetGroup:
	pushl $45
	jmp _Tw_i386_call_2
.L_RemoveGadgetGroup:
	.size	 Tw_RemoveGadgetGroup,.L_RemoveGadgetGroup-Tw_RemoveGadgetGroup



	.align 4
.globl Tw_GetSelectedGadgetGroup
	.type	 Tw_GetSelectedGadgetGroup,@function
Tw_GetSelectedGadgetGroup:
	pushl $46
	jmp _Tw_i386_call_0
.L_GetSelectedGadgetGroup:
	.size	 Tw_GetSelectedGadgetGroup,.L_GetSelectedGadgetGroup-Tw_GetSelectedGadgetGroup


	.align 4
.globl Tw_SetSelectedGadgetGroup
	.type	 Tw_SetSelectedGadgetGroup,@function
Tw_SetSelectedGadgetGroup:
	pushl $47
	jmp _Tw_i386_call_2
.L_SetSelectedGadgetGroup:
	.size	 Tw_SetSelectedGadgetGroup,.L_SetSelectedGadgetGroup-Tw_SetSelectedGadgetGroup



	.align 4
.globl Tw_RaiseRow
	.type	 Tw_RaiseRow,@function
Tw_RaiseRow:
	pushl $48
	jmp _Tw_i386_call_2
.L_RaiseRow:
	.size	 Tw_RaiseRow,.L_RaiseRow-Tw_RaiseRow


	.align 4
.globl Tw_LowerRow
	.type	 Tw_LowerRow,@function
Tw_LowerRow:
	pushl $49
	jmp _Tw_i386_call_2
.L_LowerRow:
	.size	 Tw_LowerRow,.L_LowerRow-Tw_LowerRow


	.align 4
.globl Tw_RestackChildrenRow
	.type	 Tw_RestackChildrenRow,@function
Tw_RestackChildrenRow:
	pushl $50
	jmp _Tw_i386_call_2
.L_RestackChildrenRow:
	.size	 Tw_RestackChildrenRow,.L_RestackChildrenRow-Tw_RestackChildrenRow
 

	.align 4
.globl Tw_CirculateChildrenRow
	.type	 Tw_CirculateChildrenRow,@function
Tw_CirculateChildrenRow:
	pushl $51
	jmp _Tw_i386_call_2
.L_CirculateChildrenRow:
	.size	 Tw_CirculateChildrenRow,.L_CirculateChildrenRow-Tw_CirculateChildrenRow



	.align 4
.globl Tw_Create4MenuAny
	.type	 Tw_Create4MenuAny,@function
Tw_Create4MenuAny:
	pushl $52
	jmp _Tw_i386_call_0
.L_Create4MenuAny:
	.size	 Tw_Create4MenuAny,.L_Create4MenuAny-Tw_Create4MenuAny



	.align 4
.globl Tw_Create4MenuCommonMenuItem
	.type	 Tw_Create4MenuCommonMenuItem,@function
Tw_Create4MenuCommonMenuItem:
	pushl $53
	jmp _Tw_i386_call_0
.L_Create4MenuCommonMenuItem:
	.size	 Tw_Create4MenuCommonMenuItem,.L_Create4MenuCommonMenuItem-Tw_Create4MenuCommonMenuItem



	.align 4
.globl Tw_CreateMenu
	.type	 Tw_CreateMenu,@function
Tw_CreateMenu:
	pushl $54
	jmp _Tw_i386_call_0
.L_CreateMenu:
	.size	 Tw_CreateMenu,.L_CreateMenu-Tw_CreateMenu


	.align 4
.globl Tw_SetInfoMenu
	.type	 Tw_SetInfoMenu,@function
Tw_SetInfoMenu:
	pushl $55
	jmp _Tw_i386_call_2
.L_SetInfoMenu:
	.size	 Tw_SetInfoMenu,.L_SetInfoMenu-Tw_SetInfoMenu



	.align 4
.globl Tw_CreateMsgPort
	.type	 Tw_CreateMsgPort,@function
Tw_CreateMsgPort:
	pushl $56
	jmp _Tw_i386_call_0
.L_CreateMsgPort:
	.size	 Tw_CreateMsgPort,.L_CreateMsgPort-Tw_CreateMsgPort


	.align 4
.globl Tw_FindMsgPort
	.type	 Tw_FindMsgPort,@function
Tw_FindMsgPort:
	pushl $57
	jmp _Tw_i386_call_0
.L_FindMsgPort:
	.size	 Tw_FindMsgPort,.L_FindMsgPort-Tw_FindMsgPort



	.align 4
.globl Tw_BgImageScreen
	.type	 Tw_BgImageScreen,@function
Tw_BgImageScreen:
	pushl $58
	jmp _Tw_i386_call_2
.L_BgImageScreen:
	.size	 Tw_BgImageScreen,.L_BgImageScreen-Tw_BgImageScreen



	.align 4
.globl Tw_PrevObj
	.type	 Tw_PrevObj,@function
Tw_PrevObj:
	pushl $59
	jmp _Tw_i386_call_0
.L_PrevObj:
	.size	 Tw_PrevObj,.L_PrevObj-Tw_PrevObj


	.align 4
.globl Tw_NextObj
	.type	 Tw_NextObj,@function
Tw_NextObj:
	pushl $60
	jmp _Tw_i386_call_0
.L_NextObj:
	.size	 Tw_NextObj,.L_NextObj-Tw_NextObj


	.align 4
.globl Tw_ParentObj
	.type	 Tw_ParentObj,@function
Tw_ParentObj:
	pushl $61
	jmp _Tw_i386_call_0
.L_ParentObj:
	.size	 Tw_ParentObj,.L_ParentObj-Tw_ParentObj



	.align 4
.globl Tw_FirstScreen
	.type	 Tw_FirstScreen,@function
Tw_FirstScreen:
	pushl $62
	jmp _Tw_i386_call_0
.L_FirstScreen:
	.size	 Tw_FirstScreen,.L_FirstScreen-Tw_FirstScreen


	.align 4
.globl Tw_FirstWidget
	.type	 Tw_FirstWidget,@function
Tw_FirstWidget:
	pushl $63
	jmp _Tw_i386_call_0
.L_FirstWidget:
	.size	 Tw_FirstWidget,.L_FirstWidget-Tw_FirstWidget


	.align 4
.globl Tw_FirstMsgPort
	.type	 Tw_FirstMsgPort,@function
Tw_FirstMsgPort:
	pushl $64
	jmp _Tw_i386_call_0
.L_FirstMsgPort:
	.size	 Tw_FirstMsgPort,.L_FirstMsgPort-Tw_FirstMsgPort


	.align 4
.globl Tw_FirstMenu
	.type	 Tw_FirstMenu,@function
Tw_FirstMenu:
	pushl $65
	jmp _Tw_i386_call_0
.L_FirstMenu:
	.size	 Tw_FirstMenu,.L_FirstMenu-Tw_FirstMenu


	.align 4
.globl Tw_FirstW
	.type	 Tw_FirstW,@function
Tw_FirstW:
	pushl $66
	jmp _Tw_i386_call_0
.L_FirstW:
	.size	 Tw_FirstW,.L_FirstW-Tw_FirstW


	.align 4
.globl Tw_FirstGroup
	.type	 Tw_FirstGroup,@function
Tw_FirstGroup:
	pushl $67
	jmp _Tw_i386_call_0
.L_FirstGroup:
	.size	 Tw_FirstGroup,.L_FirstGroup-Tw_FirstGroup


	.align 4
.globl Tw_FirstMutex
	.type	 Tw_FirstMutex,@function
Tw_FirstMutex:
	pushl $68
	jmp _Tw_i386_call_0
.L_FirstMutex:
	.size	 Tw_FirstMutex,.L_FirstMutex-Tw_FirstMutex


	.align 4
.globl Tw_FirstMenuItem
	.type	 Tw_FirstMenuItem,@function
Tw_FirstMenuItem:
	pushl $69
	jmp _Tw_i386_call_0
.L_FirstMenuItem:
	.size	 Tw_FirstMenuItem,.L_FirstMenuItem-Tw_FirstMenuItem


	.align 4
.globl Tw_FirstGadget
	.type	 Tw_FirstGadget,@function
Tw_FirstGadget:
	pushl $70
	jmp _Tw_i386_call_0
.L_FirstGadget:
	.size	 Tw_FirstGadget,.L_FirstGadget-Tw_FirstGadget



	.align 4
.globl Tw_GetDisplayWidth
	.type	 Tw_GetDisplayWidth,@function
Tw_GetDisplayWidth:
	pushl $71
	jmp _Tw_i386_call_0
.L_GetDisplayWidth:
	.size	 Tw_GetDisplayWidth,.L_GetDisplayWidth-Tw_GetDisplayWidth


	.align 4
.globl Tw_GetDisplayHeight
	.type	 Tw_GetDisplayHeight,@function
Tw_GetDisplayHeight:
	pushl $72
	jmp _Tw_i386_call_0
.L_GetDisplayHeight:
	.size	 Tw_GetDisplayHeight,.L_GetDisplayHeight-Tw_GetDisplayHeight


	.align 4
.globl Tw_GetAll
	.type	 Tw_GetAll,@function
Tw_GetAll:
	pushl $73
	jmp _Tw_i386_call_0
.L_GetAll:
	.size	 Tw_GetAll,.L_GetAll-Tw_GetAll



	.align 4
.globl Tw_SendToMsgPort
	.type	 Tw_SendToMsgPort,@function
Tw_SendToMsgPort:
	pushl $74
	jmp _Tw_i386_call_0
.L_SendToMsgPort:
	.size	 Tw_SendToMsgPort,.L_SendToMsgPort-Tw_SendToMsgPort


	.align 4
.globl Tw_BlindSendToMsgPort
	.type	 Tw_BlindSendToMsgPort,@function
Tw_BlindSendToMsgPort:
	pushl $75
	jmp _Tw_i386_call_2
.L_BlindSendToMsgPort:
	.size	 Tw_BlindSendToMsgPort,.L_BlindSendToMsgPort-Tw_BlindSendToMsgPort



	.align 4
.globl Tw_GetOwnerSelection
	.type	 Tw_GetOwnerSelection,@function
Tw_GetOwnerSelection:
	pushl $76
	jmp _Tw_i386_call_0
.L_GetOwnerSelection:
	.size	 Tw_GetOwnerSelection,.L_GetOwnerSelection-Tw_GetOwnerSelection


	.align 4
.globl Tw_SetOwnerSelection
	.type	 Tw_SetOwnerSelection,@function
Tw_SetOwnerSelection:
	pushl $77
	jmp _Tw_i386_call_2
.L_SetOwnerSelection:
	.size	 Tw_SetOwnerSelection,.L_SetOwnerSelection-Tw_SetOwnerSelection


	.align 4
.globl Tw_RequestSelection
	.type	 Tw_RequestSelection,@function
Tw_RequestSelection:
	pushl $78
	jmp _Tw_i386_call_2
.L_RequestSelection:
	.size	 Tw_RequestSelection,.L_RequestSelection-Tw_RequestSelection


	.align 4
.globl Tw_NotifySelection
	.type	 Tw_NotifySelection,@function
Tw_NotifySelection:
	pushl $79
	jmp _Tw_i386_call_2
.L_NotifySelection:
	.size	 Tw_NotifySelection,.L_NotifySelection-Tw_NotifySelection



	.align 4
.globl Tw_SetServerUid
	.type	 Tw_SetServerUid,@function
Tw_SetServerUid:
	pushl $80
	jmp _Tw_i386_call_0
.L_SetServerUid:
	.size	 Tw_SetServerUid,.L_SetServerUid-Tw_SetServerUid



	.align 4
.globl Tw_OpenExtension
	.type	 Tw_OpenExtension,@function
Tw_OpenExtension:
	pushl $81
	jmp _Tw_i386_call_0
.L_OpenExtension:
	.size	 Tw_OpenExtension,.L_OpenExtension-Tw_OpenExtension


	.align 4
.globl Tw_CallBExtension
	.type	 Tw_CallBExtension,@function
Tw_CallBExtension:
	pushl $82
	jmp _Tw_i386_call_0
.L_CallBExtension:
	.size	 Tw_CallBExtension,.L_CallBExtension-Tw_CallBExtension


	.align 4
.globl Tw_CloseExtension
	.type	 Tw_CloseExtension,@function
Tw_CloseExtension:
	pushl $83
	jmp _Tw_i386_call_2
.L_CloseExtension:
	.size	 Tw_CloseExtension,.L_CloseExtension-Tw_CloseExtension





	.align 4
	.type	 _Tw_i386_call_3,@function
_Tw_i386_call_3:
	pushl $3
	call _Tw_EncodeCall
	popl %ecx
	popl %ecx
	ret
.L_i386_call_3:
	.size	 _Tw_i386_call_3,.L_i386_call_3-_Tw_i386_call_3


