



/* This file was automatically generated from m4/call.m4, do not edit! */


/*
 *  call_m4.h  --  implementation of listeners to libTT methods
 *
 */



























/* enum for method listeners */

typedef enum e_order_methods {
    /* generic functions */
    order_ExitMainLoop = 1,
    order_New,
    order_Del,




    /* ttobj methods */                                
    order_Ref_ttobj,
    order_Unref_ttobj,  
    /* ttevent methods */
    order_Create_ttevent,
    order_Fire_ttevent, 
    /* tteventbig methods */

    order_Create8_tteventbig,
    order_Create4_tteventbig,
    order_Create4s4_tteventbig, 
    /* ttbitmask methods */
    order_Clone_ttbitmask,
    order_Create_ttbitmask,
    order_CreateA_ttbitmask,
    order_Clear_ttbitmask,
    order_Fill_ttbitmask,
    order_Negate_ttbitmask,
    order_Change_ttbitmask,
    order_IsMember_ttbitmask,
    order_Add_ttbitmask,
    order_AddA_ttbitmask,
    order_Remove_ttbitmask,
    order_RemoveA_ttbitmask,
    order_Combine_ttbitmask,
    order_CombineA_ttbitmask,
    order_CombineB_ttbitmask, 
    /* tteventmask methods */
    order_Create_tteventmask,
    order_SetEvtypeMask_tteventmask,
    order_SetEvcodeMask_tteventmask,
    order_SetComponentMask_tteventmask,
    order_SetTruthTable_tteventmask, 
    /* ttcallback methods */     
    /* ttlistener methods */
    order_Create_ttlistener,
    order_CreateA_ttlistener,
    order_CreateV_ttlistener,

    order_CreateDel_ttlistener,
    order_CreateAskclose_ttlistener,
    order_CreateKey_ttlistener,
    order_CreateMouse_ttlistener,
    order_CreateActivate_ttlistener,
    order_CreateChange_ttlistener,
    order_CreateKeyData_ttlistener,
    order_CreateExpose_ttlistener,
    
    order_AddTo_ttlistener,    
    order_Remove_ttlistener,
    order_SetArgs_ttlistener,
    order_SetEventMask_ttlistener, 
    /* tttimer methods */
    order_CreateA_tttimer,
    order_Create_tttimer,
    order_SetDelay_tttimer,    
    order_SetEnabled_tttimer,    
    order_IsEnabled_tttimer, 
    /* ttcomponent methods */
    order_AddListener_ttcomponent,
    order_RemoveListener_ttcomponent,
    order_SetKeyData_ttcomponent,
    order_GetKeyData_ttcomponent,
    order_UnsetKeyData_ttcomponent,
    order_GetData_ttcomponent, 
    /* ttdata methods */         
    /* ttvisible methods */    
    order_AddTo_ttvisible,    
    order_Remove_ttvisible,    
    order_Invalidate_ttvisible,    
    order_SetVisible_ttvisible,    
    order_SetTheme_ttvisible,    
    order_Draw_ttvisible,    
    order_BuiltinRepaint_ttvisible,
    order_SetRepaint_ttvisible,
    order_Add_ttvisible,
    order_Expose_ttvisible, 
    /* ttnative methods */    
    order_GetRoot_ttnative,    
    order_GetW_ttnative,    
    order_GetH_ttnative,
    order_Create_ttnative, 
    /* ttwidget methods */    
    order_SetXY_ttwidget,    
    order_SetWH_ttwidget,    
    order_SetXl_ttwidget,    
    order_SetYl_ttwidget,    
    order_SetWl_ttwidget,    
    order_SetHl_ttwidget,    
    order_SetTooltip_ttwidget,
    order_SetXlYl_ttwidget,
    order_SetWlHl_ttwidget,
    order_SetXYWH_ttwidget,
    order_SetX_ttwidget,
    order_SetY_ttwidget,
    order_SetW_ttwidget,
    order_SetH_ttwidget, 
    /* ttlabel methods */    
    order_DoSetFont_ttlabel,
    order_SetFont_ttlabel,
    order_SetText_ttlabel,
    order_Create_ttlabel, 
    /* tttooltip methods */    
    order_SetWidget_tttooltip, 
    /* ttbuttongroup methods */    
    order_Add_ttbuttongroup,    
    order_Remove_ttbuttongroup,    
    order_SetChecked_ttbuttongroup, 
    /* ttanybutton methods */        
    order_DoSetAttr_ttanybutton,
    order_SetAttr_ttanybutton,
    order_SetFont_ttanybutton,
    order_SetText_ttanybutton,
    order_Create_ttanybutton, 
    /* ttbutton methods */    
    order_SetPressed_ttbutton,
    order_IsPressed_ttbutton,
    order_Create_ttbutton, 
    /* ttcheckbutton methods */    
    order_SetChecked_ttcheckbutton,
    order_IsChecked_ttcheckbutton,
    order_Create_ttcheckbutton, 
    /* ttradiobutton methods */
    order_AddToGroup_ttradiobutton,
    order_RemoveFromGroup_ttradiobutton,
    order_Create_ttradiobutton, 
    /* ttanyscroll methods */            
    order_SetOrientation_ttanyscroll,    
    order_SetSize_ttanyscroll,    
    order_SetRealSize_ttanyscroll,    
    order_SetViewSize_ttanyscroll,    
    order_SetPosition_ttanyscroll, 
    /* ttscrollbar methods */
    order_Create1_ttscrollbar,
    order_Create4_ttscrollbar, 
    /* ttslider methods */    
    order_SetSlideMin_ttslider,    
    order_SetSlideMax_ttslider,    
    order_SetSlideValue_ttslider,
    order_Create_ttslider, 
    /* ttprogressbar methods */
    order_Create_ttprogressbar, 
    /* ttscrollpane methods */
    order_Create_ttscrollpane,    
    order_SetBarX_ttscrollpane,    
    order_SetBarY_ttscrollpane, 
    /* ttwindow methods */    
    order_SetTitle_ttwindow, 
    /* ttframe methods */    
    order_SetMenubar_ttframe,
    order_Create_ttframe, 
    /* ttscroller methods */
    order_Create_ttscroller, 
    /* ttmenuitem methods */ 
    /* ttcheckmenuitem methods */ 
    /* ttradiomenuitem methods */ 
    /* ttmenu methods */ 
    /* ttmenubar methods */
    order_GetDefault_ttmenubar, 
    /* ttanytext methods */ 
    /* tttextfield methods */ 
    /* tttextarea methods */ 
    /* tttheme methods */
    order_GetDefault_tttheme, 
    /* ttapplication methods */
    order_Set_ttapplication,
    order_Get_ttapplication,





    order_FN_handy,







    /* handy ttobj methods */            
    order_GetId_ttobj,    
    order_GetRefcount_ttobj,    
    order_GetOflags_ttobj,            
    order_GetUserData_ttobj,
    order_SetUserData_ttobj,    
    order_GetEventsInprogress_ttobj,    
    order_GetEventsInformed_ttobj,  
    /* handy ttevent methods */        
    order_GetCallback_ttevent,    
    order_GetComponent_ttevent,    
    order_GetEvtype_ttevent,    
    order_GetEvcode_ttevent,    
    order_GetEvflags_ttevent,            
    order_Consume_ttevent, 
    /* handy tteventbig methods */        
    order_GetX_tteventbig,    
    order_GetY_tteventbig,    
    order_GetW_tteventbig,    
    order_GetH_tteventbig,    
    order_GetValue_tteventbig,    
    order_GetOldValue_tteventbig,    
    order_GetLen_tteventbig,    
    order_GetData_tteventbig, 
    /* handy ttbitmask methods */                     
    /* handy tteventmask methods */        
    order_GetEvtypeMask_tteventmask,    
    order_GetEvcodeMask_tteventmask,    
    order_GetComponentMask_tteventmask,    
    order_GetTruthTable_tteventmask, 
    /* handy ttcallback methods */        
    order_GetLflags_ttcallback,    
    order_GetComponent_ttcallback,    
    order_GetPrev_ttcallback,    
    order_GetNext_ttcallback, 
    /* handy ttlistener methods */            
    order_GetEvent_ttlistener,    
    order_GetNargComponent_ttlistener,    
    order_GetNargEvent_ttlistener,    
    order_GetNargs_ttlistener,    
    order_GetFunction_ttlistener,    
    order_GetArgs_ttlistener,    
    order_GetEventMask_ttlistener,         
    /* handy tttimer methods */                     
    /* handy ttcomponent methods */        
    order_GetCallbacks_ttcomponent,    
    order_GetListeners_ttcomponent,    
    order_GetTimers_ttcomponent,    
    order_GetDatas_ttcomponent, 
    /* handy ttdata methods */            
    order_GetComponent_ttdata,    
    order_GetKey_ttdata,    
    order_GetKeyLen_ttdata,    
    order_GetData_ttdata,
    order_SetData_ttdata, 
    /* handy ttvisible methods */        
    order_GetVflags_ttvisible,    
    order_GetPrev_ttvisible,    
    order_GetNext_ttvisible,    
    order_GetParent_ttvisible,    
    order_GetChildFirst_ttvisible,    
    order_GetChildLast_ttvisible,    
    order_GetTheme_ttvisible,    
    order_GetRepaint_ttvisible, 
    /* handy ttnative methods */     
    /* handy ttwidget methods */        
    order_GetX_ttwidget,    
    order_GetY_ttwidget,    
    order_GetW_ttwidget,    
    order_GetH_ttwidget,    
    order_GetXl_ttwidget,    
    order_GetYl_ttwidget,    
    order_GetWl_ttwidget,    
    order_GetHl_ttwidget,        
    order_GetTooltip_ttwidget, 
    /* handy ttlabel methods */        
    order_GetTextLen_ttlabel,    
    order_GetText_ttlabel, 
    /* handy tttooltip methods */            
    order_GetWidget_tttooltip,     
    /* handy ttbuttongroup methods */        
    order_GetGroupFirst_ttbuttongroup,    
    order_GetGroupLast_ttbuttongroup,    
    order_GetChecked_ttbuttongroup, 
    /* handy ttanybutton methods */        
    order_GetText_ttanybutton,    
    order_GetTextWidth_ttanybutton,    
    order_GetTextHeight_ttanybutton,     
    /* handy ttbutton methods */     
    /* handy ttcheckbutton methods */     
    /* handy ttradiobutton methods */        
    order_GetGroup_ttradiobutton,    
    order_GetGroupPrev_ttradiobutton,    
    order_GetGroupNext_ttradiobutton, 
    /* handy ttanyscroll methods */        
    order_GetOrientation_ttanyscroll,    
    order_GetSize_ttanyscroll,    
    order_GetRealSize_ttanyscroll,    
    order_GetViewSize_ttanyscroll,        
    order_GetPosition_ttanyscroll,    
    order_GetRealPosition_ttanyscroll,     
    /* handy ttscrollbar methods */         
    /* handy ttslider methods */        
    order_GetSlideMin_ttslider,    
    order_GetSlideMax_ttslider,    
    order_GetSlideValue_ttslider, 
    /* handy ttprogressbar methods */             
    /* handy ttscrollpane methods */        
    order_GetBarX_ttscrollpane,    
    order_GetBarY_ttscrollpane, 
    /* handy ttwindow methods */        
    order_GetTitleLen_ttwindow,    
    order_GetTitle_ttwindow, 
    /* handy ttframe methods */        
    order_GetMenubar_ttframe, 
    /* handy ttscroller methods */        
    order_GetScrollpane_ttscroller, 
    /* handy ttmenuitem methods */        
    order_GetName_ttmenuitem, 
    /* handy ttcheckmenuitem methods */     
    /* handy ttradiomenuitem methods */     
    /* handy ttmenu methods */    
    
    order_GetMenubar_ttmenu, 
    /* handy ttmenubar methods */     
    /* handy ttanytext methods */        
    order_GetTextLen_ttanytext,    
    order_GetText_ttanytext, 
    /* handy tttextfield methods */     
    /* handy tttextarea methods */     
    /* handy tttheme methods */    
    
    order_GetName_tttheme,                 
    /* handy ttapplication methods */        
    order_GetName_ttapplication,






    order_FN_last
} e_order_methods;


typedef struct s_ttmethod s_ttmethod;
typedef struct s_ttmethod *ttmethod;
struct s_ttmethod {
    opaque mtho;
    void *mth;
};

/* array for method listeners */

static struct s_ttmethod method_array[] = {
    /* generic functions */
    { order_ExitMainLoop, (void *)TTExitMainLoop, },
    { order_New, (void *)TTNew, },
    { order_Del, (void *)TTDel, }, 




    /* ttobj methods */                                
    { order_Ref_ttobj, (void *)TTRef_ttobj, },
    { order_Unref_ttobj, (void *)TTUnref_ttobj, },  
    /* ttevent methods */
    { order_Create_ttevent, (void *)TTCreate_ttevent, },
    { order_Fire_ttevent, (void *)TTFire_ttevent, }, 
    /* tteventbig methods */

    { order_Create8_tteventbig, (void *)TTCreate8_tteventbig, },
    { order_Create4_tteventbig, (void *)TTCreate4_tteventbig, },
    { order_Create4s4_tteventbig, (void *)TTCreate4s4_tteventbig, }, 
    /* ttbitmask methods */
    { order_Clone_ttbitmask, (void *)TTClone_ttbitmask, },
    { order_Create_ttbitmask, (void *)TTCreate_ttbitmask, },
    { order_CreateA_ttbitmask, (void *)TTCreateA_ttbitmask, },
    { order_Clear_ttbitmask, (void *)TTClear_ttbitmask, },
    { order_Fill_ttbitmask, (void *)TTFill_ttbitmask, },
    { order_Negate_ttbitmask, (void *)TTNegate_ttbitmask, },
    { order_Change_ttbitmask, (void *)TTChange_ttbitmask, },
    { order_IsMember_ttbitmask, (void *)TTIsMember_ttbitmask, },
    { order_Add_ttbitmask, (void *)TTAdd_ttbitmask, },
    { order_AddA_ttbitmask, (void *)TTAddA_ttbitmask, },
    { order_Remove_ttbitmask, (void *)TTRemove_ttbitmask, },
    { order_RemoveA_ttbitmask, (void *)TTRemoveA_ttbitmask, },
    { order_Combine_ttbitmask, (void *)TTCombine_ttbitmask, },
    { order_CombineA_ttbitmask, (void *)TTCombineA_ttbitmask, },
    { order_CombineB_ttbitmask, (void *)TTCombineB_ttbitmask, }, 
    /* tteventmask methods */
    { order_Create_tteventmask, (void *)TTCreate_tteventmask, },
    { order_SetEvtypeMask_tteventmask, (void *)TTSetEvtypeMask_tteventmask, },
    { order_SetEvcodeMask_tteventmask, (void *)TTSetEvcodeMask_tteventmask, },
    { order_SetComponentMask_tteventmask, (void *)TTSetComponentMask_tteventmask, },
    { order_SetTruthTable_tteventmask, (void *)TTSetTruthTable_tteventmask, }, 
    /* ttcallback methods */     
    /* ttlistener methods */
    { order_Create_ttlistener, (void *)TTCreate_ttlistener, },
    { order_CreateA_ttlistener, (void *)TTCreateA_ttlistener, },
    { order_CreateV_ttlistener, (void *)TTCreateV_ttlistener, },

    { order_CreateDel_ttlistener, (void *)TTCreateDel_ttlistener, },
    { order_CreateAskclose_ttlistener, (void *)TTCreateAskclose_ttlistener, },
    { order_CreateKey_ttlistener, (void *)TTCreateKey_ttlistener, },
    { order_CreateMouse_ttlistener, (void *)TTCreateMouse_ttlistener, },
    { order_CreateActivate_ttlistener, (void *)TTCreateActivate_ttlistener, },
    { order_CreateChange_ttlistener, (void *)TTCreateChange_ttlistener, },
    { order_CreateKeyData_ttlistener, (void *)TTCreateKeyData_ttlistener, },
    { order_CreateExpose_ttlistener, (void *)TTCreateExpose_ttlistener, },
    
    { order_AddTo_ttlistener, (void *)TTAddTo_ttlistener, },    
    { order_Remove_ttlistener, (void *)TTRemove_ttlistener, },
    { order_SetArgs_ttlistener, (void *)TTSetArgs_ttlistener, },
    { order_SetEventMask_ttlistener, (void *)TTSetEventMask_ttlistener, }, 
    /* tttimer methods */
    { order_CreateA_tttimer, (void *)TTCreateA_tttimer, },
    { order_Create_tttimer, (void *)TTCreate_tttimer, },
    { order_SetDelay_tttimer, (void *)TTSetDelay_tttimer, },    
    { order_SetEnabled_tttimer, (void *)TTSetEnabled_tttimer, },    
    { order_IsEnabled_tttimer, (void *)TTIsEnabled_tttimer, }, 
    /* ttcomponent methods */
    { order_AddListener_ttcomponent, (void *)TTAddListener_ttcomponent, },
    { order_RemoveListener_ttcomponent, (void *)TTRemoveListener_ttcomponent, },
    { order_SetKeyData_ttcomponent, (void *)TTSetKeyData_ttcomponent, },
    { order_GetKeyData_ttcomponent, (void *)TTGetKeyData_ttcomponent, },
    { order_UnsetKeyData_ttcomponent, (void *)TTUnsetKeyData_ttcomponent, },
    { order_GetData_ttcomponent, (void *)TTGetData_ttcomponent, }, 
    /* ttdata methods */         
    /* ttvisible methods */    
    { order_AddTo_ttvisible, (void *)TTAddTo_ttvisible, },    
    { order_Remove_ttvisible, (void *)TTRemove_ttvisible, },    
    { order_Invalidate_ttvisible, (void *)TTInvalidate_ttvisible, },    
    { order_SetVisible_ttvisible, (void *)TTSetVisible_ttvisible, },    
    { order_SetTheme_ttvisible, (void *)TTSetTheme_ttvisible, },    
    { order_Draw_ttvisible, (void *)TTDraw_ttvisible, },    
    { order_BuiltinRepaint_ttvisible, (void *)TTBuiltinRepaint_ttvisible, },
    { order_SetRepaint_ttvisible, (void *)TTSetRepaint_ttvisible, },
    { order_Add_ttvisible, (void *)TTAdd_ttvisible, },
    { order_Expose_ttvisible, (void *)TTExpose_ttvisible, }, 
    /* ttnative methods */    
    { order_GetRoot_ttnative, (void *)TTGetRoot_ttnative, },    
    { order_GetW_ttnative, (void *)TTGetW_ttnative, },    
    { order_GetH_ttnative, (void *)TTGetH_ttnative, },
    { order_Create_ttnative, (void *)TTCreate_ttnative, }, 
    /* ttwidget methods */    
    { order_SetXY_ttwidget, (void *)TTSetXY_ttwidget, },    
    { order_SetWH_ttwidget, (void *)TTSetWH_ttwidget, },    
    { order_SetXl_ttwidget, (void *)TTSetXl_ttwidget, },    
    { order_SetYl_ttwidget, (void *)TTSetYl_ttwidget, },    
    { order_SetWl_ttwidget, (void *)TTSetWl_ttwidget, },    
    { order_SetHl_ttwidget, (void *)TTSetHl_ttwidget, },    
    { order_SetTooltip_ttwidget, (void *)TTSetTooltip_ttwidget, },
    { order_SetXlYl_ttwidget, (void *)TTSetXlYl_ttwidget, },
    { order_SetWlHl_ttwidget, (void *)TTSetWlHl_ttwidget, },
    { order_SetXYWH_ttwidget, (void *)TTSetXYWH_ttwidget, },
    { order_SetX_ttwidget, (void *)TTSetX_ttwidget, },
    { order_SetY_ttwidget, (void *)TTSetY_ttwidget, },
    { order_SetW_ttwidget, (void *)TTSetW_ttwidget, },
    { order_SetH_ttwidget, (void *)TTSetH_ttwidget, }, 
    /* ttlabel methods */    
    { order_DoSetFont_ttlabel, (void *)TTDoSetFont_ttlabel, },
    { order_SetFont_ttlabel, (void *)TTSetFont_ttlabel, },
    { order_SetText_ttlabel, (void *)TTSetText_ttlabel, },
    { order_Create_ttlabel, (void *)TTCreate_ttlabel, }, 
    /* tttooltip methods */    
    { order_SetWidget_tttooltip, (void *)TTSetWidget_tttooltip, }, 
    /* ttbuttongroup methods */    
    { order_Add_ttbuttongroup, (void *)TTAdd_ttbuttongroup, },    
    { order_Remove_ttbuttongroup, (void *)TTRemove_ttbuttongroup, },    
    { order_SetChecked_ttbuttongroup, (void *)TTSetChecked_ttbuttongroup, }, 
    /* ttanybutton methods */        
    { order_DoSetAttr_ttanybutton, (void *)TTDoSetAttr_ttanybutton, },
    { order_SetAttr_ttanybutton, (void *)TTSetAttr_ttanybutton, },
    { order_SetFont_ttanybutton, (void *)TTSetFont_ttanybutton, },
    { order_SetText_ttanybutton, (void *)TTSetText_ttanybutton, },
    { order_Create_ttanybutton, (void *)TTCreate_ttanybutton, }, 
    /* ttbutton methods */    
    { order_SetPressed_ttbutton, (void *)TTSetPressed_ttbutton, },
    { order_IsPressed_ttbutton, (void *)TTIsPressed_ttbutton, },
    { order_Create_ttbutton, (void *)TTCreate_ttbutton, }, 
    /* ttcheckbutton methods */    
    { order_SetChecked_ttcheckbutton, (void *)TTSetChecked_ttcheckbutton, },
    { order_IsChecked_ttcheckbutton, (void *)TTIsChecked_ttcheckbutton, },
    { order_Create_ttcheckbutton, (void *)TTCreate_ttcheckbutton, }, 
    /* ttradiobutton methods */
    { order_AddToGroup_ttradiobutton, (void *)TTAddToGroup_ttradiobutton, },
    { order_RemoveFromGroup_ttradiobutton, (void *)TTRemoveFromGroup_ttradiobutton, },
    { order_Create_ttradiobutton, (void *)TTCreate_ttradiobutton, }, 
    /* ttanyscroll methods */            
    { order_SetOrientation_ttanyscroll, (void *)TTSetOrientation_ttanyscroll, },    
    { order_SetSize_ttanyscroll, (void *)TTSetSize_ttanyscroll, },    
    { order_SetRealSize_ttanyscroll, (void *)TTSetRealSize_ttanyscroll, },    
    { order_SetViewSize_ttanyscroll, (void *)TTSetViewSize_ttanyscroll, },    
    { order_SetPosition_ttanyscroll, (void *)TTSetPosition_ttanyscroll, }, 
    /* ttscrollbar methods */
    { order_Create1_ttscrollbar, (void *)TTCreate1_ttscrollbar, },
    { order_Create4_ttscrollbar, (void *)TTCreate4_ttscrollbar, }, 
    /* ttslider methods */    
    { order_SetSlideMin_ttslider, (void *)TTSetSlideMin_ttslider, },    
    { order_SetSlideMax_ttslider, (void *)TTSetSlideMax_ttslider, },    
    { order_SetSlideValue_ttslider, (void *)TTSetSlideValue_ttslider, },
    { order_Create_ttslider, (void *)TTCreate_ttslider, }, 
    /* ttprogressbar methods */
    { order_Create_ttprogressbar, (void *)TTCreate_ttprogressbar, }, 
    /* ttscrollpane methods */
    { order_Create_ttscrollpane, (void *)TTCreate_ttscrollpane, },    
    { order_SetBarX_ttscrollpane, (void *)TTSetBarX_ttscrollpane, },    
    { order_SetBarY_ttscrollpane, (void *)TTSetBarY_ttscrollpane, }, 
    /* ttwindow methods */    
    { order_SetTitle_ttwindow, (void *)TTSetTitle_ttwindow, }, 
    /* ttframe methods */    
    { order_SetMenubar_ttframe, (void *)TTSetMenubar_ttframe, },
    { order_Create_ttframe, (void *)TTCreate_ttframe, }, 
    /* ttscroller methods */
    { order_Create_ttscroller, (void *)TTCreate_ttscroller, }, 
    /* ttmenuitem methods */ 
    /* ttcheckmenuitem methods */ 
    /* ttradiomenuitem methods */ 
    /* ttmenu methods */ 
    /* ttmenubar methods */
    { order_GetDefault_ttmenubar, (void *)TTGetDefault_ttmenubar, }, 
    /* ttanytext methods */ 
    /* tttextfield methods */ 
    /* tttextarea methods */ 
    /* tttheme methods */
    { order_GetDefault_tttheme, (void *)TTGetDefault_tttheme, }, 
    /* ttapplication methods */
    { order_Set_ttapplication, (void *)TTSet_ttapplication, },
    { order_Get_ttapplication, (void *)TTGet_ttapplication, },











    /* handy ttobj methods */            
    { order_GetId_ttobj, (void *)TTGetId_ttobj, },    
    { order_GetRefcount_ttobj, (void *)TTGetRefcount_ttobj, },    
    { order_GetOflags_ttobj, (void *)TTGetOflags_ttobj, },            
    { order_GetUserData_ttobj, (void *)TTGetUserData_ttobj, },
    { order_SetUserData_ttobj, (void *)TTSetUserData_ttobj, },    
    { order_GetEventsInprogress_ttobj, (void *)TTGetEventsInprogress_ttobj, },    
    { order_GetEventsInformed_ttobj, (void *)TTGetEventsInformed_ttobj, },  
    /* handy ttevent methods */        
    { order_GetCallback_ttevent, (void *)TTGetCallback_ttevent, },    
    { order_GetComponent_ttevent, (void *)TTGetComponent_ttevent, },    
    { order_GetEvtype_ttevent, (void *)TTGetEvtype_ttevent, },    
    { order_GetEvcode_ttevent, (void *)TTGetEvcode_ttevent, },    
    { order_GetEvflags_ttevent, (void *)TTGetEvflags_ttevent, },            
    { order_Consume_ttevent, (void *)TTConsume_ttevent, }, 
    /* handy tteventbig methods */        
    { order_GetX_tteventbig, (void *)TTGetX_tteventbig, },    
    { order_GetY_tteventbig, (void *)TTGetY_tteventbig, },    
    { order_GetW_tteventbig, (void *)TTGetW_tteventbig, },    
    { order_GetH_tteventbig, (void *)TTGetH_tteventbig, },    
    { order_GetValue_tteventbig, (void *)TTGetValue_tteventbig, },    
    { order_GetOldValue_tteventbig, (void *)TTGetOldValue_tteventbig, },    
    { order_GetLen_tteventbig, (void *)TTGetLen_tteventbig, },    
    { order_GetData_tteventbig, (void *)TTGetData_tteventbig, }, 
    /* handy ttbitmask methods */                     
    /* handy tteventmask methods */        
    { order_GetEvtypeMask_tteventmask, (void *)TTGetEvtypeMask_tteventmask, },    
    { order_GetEvcodeMask_tteventmask, (void *)TTGetEvcodeMask_tteventmask, },    
    { order_GetComponentMask_tteventmask, (void *)TTGetComponentMask_tteventmask, },    
    { order_GetTruthTable_tteventmask, (void *)TTGetTruthTable_tteventmask, }, 
    /* handy ttcallback methods */        
    { order_GetLflags_ttcallback, (void *)TTGetLflags_ttcallback, },    
    { order_GetComponent_ttcallback, (void *)TTGetComponent_ttcallback, },    
    { order_GetPrev_ttcallback, (void *)TTGetPrev_ttcallback, },    
    { order_GetNext_ttcallback, (void *)TTGetNext_ttcallback, }, 
    /* handy ttlistener methods */            
    { order_GetEvent_ttlistener, (void *)TTGetEvent_ttlistener, },    
    { order_GetNargComponent_ttlistener, (void *)TTGetNargComponent_ttlistener, },    
    { order_GetNargEvent_ttlistener, (void *)TTGetNargEvent_ttlistener, },    
    { order_GetNargs_ttlistener, (void *)TTGetNargs_ttlistener, },    
    { order_GetFunction_ttlistener, (void *)TTGetFunction_ttlistener, },    
    { order_GetArgs_ttlistener, (void *)TTGetArgs_ttlistener, },    
    { order_GetEventMask_ttlistener, (void *)TTGetEventMask_ttlistener, },         
    /* handy tttimer methods */                     
    /* handy ttcomponent methods */        
    { order_GetCallbacks_ttcomponent, (void *)TTGetCallbacks_ttcomponent, },    
    { order_GetListeners_ttcomponent, (void *)TTGetListeners_ttcomponent, },    
    { order_GetTimers_ttcomponent, (void *)TTGetTimers_ttcomponent, },    
    { order_GetDatas_ttcomponent, (void *)TTGetDatas_ttcomponent, }, 
    /* handy ttdata methods */            
    { order_GetComponent_ttdata, (void *)TTGetComponent_ttdata, },    
    { order_GetKey_ttdata, (void *)TTGetKey_ttdata, },    
    { order_GetKeyLen_ttdata, (void *)TTGetKeyLen_ttdata, },    
    { order_GetData_ttdata, (void *)TTGetData_ttdata, },
    { order_SetData_ttdata, (void *)TTSetData_ttdata, }, 
    /* handy ttvisible methods */        
    { order_GetVflags_ttvisible, (void *)TTGetVflags_ttvisible, },    
    { order_GetPrev_ttvisible, (void *)TTGetPrev_ttvisible, },    
    { order_GetNext_ttvisible, (void *)TTGetNext_ttvisible, },    
    { order_GetParent_ttvisible, (void *)TTGetParent_ttvisible, },    
    { order_GetChildFirst_ttvisible, (void *)TTGetChildFirst_ttvisible, },    
    { order_GetChildLast_ttvisible, (void *)TTGetChildLast_ttvisible, },    
    { order_GetTheme_ttvisible, (void *)TTGetTheme_ttvisible, },    
    { order_GetRepaint_ttvisible, (void *)TTGetRepaint_ttvisible, }, 
    /* handy ttnative methods */     
    /* handy ttwidget methods */        
    { order_GetX_ttwidget, (void *)TTGetX_ttwidget, },    
    { order_GetY_ttwidget, (void *)TTGetY_ttwidget, },    
    { order_GetW_ttwidget, (void *)TTGetW_ttwidget, },    
    { order_GetH_ttwidget, (void *)TTGetH_ttwidget, },    
    { order_GetXl_ttwidget, (void *)TTGetXl_ttwidget, },    
    { order_GetYl_ttwidget, (void *)TTGetYl_ttwidget, },    
    { order_GetWl_ttwidget, (void *)TTGetWl_ttwidget, },    
    { order_GetHl_ttwidget, (void *)TTGetHl_ttwidget, },        
    { order_GetTooltip_ttwidget, (void *)TTGetTooltip_ttwidget, }, 
    /* handy ttlabel methods */        
    { order_GetTextLen_ttlabel, (void *)TTGetTextLen_ttlabel, },    
    { order_GetText_ttlabel, (void *)TTGetText_ttlabel, }, 
    /* handy tttooltip methods */            
    { order_GetWidget_tttooltip, (void *)TTGetWidget_tttooltip, },     
    /* handy ttbuttongroup methods */        
    { order_GetGroupFirst_ttbuttongroup, (void *)TTGetGroupFirst_ttbuttongroup, },    
    { order_GetGroupLast_ttbuttongroup, (void *)TTGetGroupLast_ttbuttongroup, },    
    { order_GetChecked_ttbuttongroup, (void *)TTGetChecked_ttbuttongroup, }, 
    /* handy ttanybutton methods */        
    { order_GetText_ttanybutton, (void *)TTGetText_ttanybutton, },    
    { order_GetTextWidth_ttanybutton, (void *)TTGetTextWidth_ttanybutton, },    
    { order_GetTextHeight_ttanybutton, (void *)TTGetTextHeight_ttanybutton, },     
    /* handy ttbutton methods */     
    /* handy ttcheckbutton methods */     
    /* handy ttradiobutton methods */        
    { order_GetGroup_ttradiobutton, (void *)TTGetGroup_ttradiobutton, },    
    { order_GetGroupPrev_ttradiobutton, (void *)TTGetGroupPrev_ttradiobutton, },    
    { order_GetGroupNext_ttradiobutton, (void *)TTGetGroupNext_ttradiobutton, }, 
    /* handy ttanyscroll methods */        
    { order_GetOrientation_ttanyscroll, (void *)TTGetOrientation_ttanyscroll, },    
    { order_GetSize_ttanyscroll, (void *)TTGetSize_ttanyscroll, },    
    { order_GetRealSize_ttanyscroll, (void *)TTGetRealSize_ttanyscroll, },    
    { order_GetViewSize_ttanyscroll, (void *)TTGetViewSize_ttanyscroll, },        
    { order_GetPosition_ttanyscroll, (void *)TTGetPosition_ttanyscroll, },    
    { order_GetRealPosition_ttanyscroll, (void *)TTGetRealPosition_ttanyscroll, },     
    /* handy ttscrollbar methods */         
    /* handy ttslider methods */        
    { order_GetSlideMin_ttslider, (void *)TTGetSlideMin_ttslider, },    
    { order_GetSlideMax_ttslider, (void *)TTGetSlideMax_ttslider, },    
    { order_GetSlideValue_ttslider, (void *)TTGetSlideValue_ttslider, }, 
    /* handy ttprogressbar methods */             
    /* handy ttscrollpane methods */        
    { order_GetBarX_ttscrollpane, (void *)TTGetBarX_ttscrollpane, },    
    { order_GetBarY_ttscrollpane, (void *)TTGetBarY_ttscrollpane, }, 
    /* handy ttwindow methods */        
    { order_GetTitleLen_ttwindow, (void *)TTGetTitleLen_ttwindow, },    
    { order_GetTitle_ttwindow, (void *)TTGetTitle_ttwindow, }, 
    /* handy ttframe methods */        
    { order_GetMenubar_ttframe, (void *)TTGetMenubar_ttframe, }, 
    /* handy ttscroller methods */        
    { order_GetScrollpane_ttscroller, (void *)TTGetScrollpane_ttscroller, }, 
    /* handy ttmenuitem methods */        
    { order_GetName_ttmenuitem, (void *)TTGetName_ttmenuitem, }, 
    /* handy ttcheckmenuitem methods */     
    /* handy ttradiomenuitem methods */     
    /* handy ttmenu methods */    
    
    { order_GetMenubar_ttmenu, (void *)TTGetMenubar_ttmenu, }, 
    /* handy ttmenubar methods */     
    /* handy ttanytext methods */        
    { order_GetTextLen_ttanytext, (void *)TTGetTextLen_ttanytext, },    
    { order_GetText_ttanytext, (void *)TTGetText_ttanytext, }, 
    /* handy tttextfield methods */     
    /* handy tttextarea methods */     
    /* handy tttheme methods */    
    
    { order_GetName_tttheme, (void *)TTGetName_tttheme, },                 
    /* handy ttapplication methods */        
    { order_GetName_ttapplication, (void *)TTGetName_ttapplication, },






};

#define order_FN_n (sizeof(method_array)/sizeof(method_array[0]))

static ttbyte method_needsort = TRUE;

static int CompareMethods(TT_CONST ttmethod m1, TT_CONST ttmethod m2) {
    return (int)((byte *)m1->mth - (byte *)m2->mth);
}

/*
 * use binary search for efficiency
 */
static opaque Method2Order(void *method) {
    s_ttmethod key, *m;
    
    if (method_needsort) {
	method_needsort = FALSE;
	qsort(method_array, order_FN_n, sizeof(struct s_ttmethod),
	      (int (*)(TT_CONST void *, TT_CONST void *))CompareMethods);
    }
    
    key.mth = method;
    
    if ((m = bsearch(&key, method_array, order_FN_n, sizeof(struct s_ttmethod),
	             (int (*)(TT_CONST void *, TT_CONST void *))CompareMethods))) {
	return m->mtho;
    }
    return (opaque)0;
}







/*
 * here and below, we do consider TTGet*() methods specially:
 * they have no side effects, and if called from within a callback they
 * cannot return a useful return value, so we can do without calling them.
 */
static void CallMethod(ttuint order, ttuint nargs, TT_CONST ttany *a) {

    switch (order) {
    
    /* generic functions */
    
      case order_ExitMainLoop:
	if (nargs >= 0)
	    TTExitMainLoop();
	break;
      case order_New:
	if (nargs >= 1)
	    TTNew((tt_fn)(opaque)a[0]);
	break;
      case order_Del:
	if (nargs >= 1)
	    TTDel((tt_obj)(opaque)a[0]);
	break;






    /* ttobj methods */
                                    
      case order_Ref_ttobj:
	if (nargs >= 1)
	    TTRef_ttobj((tt_obj)(opaque)a[0]);
	break;
      case order_Unref_ttobj:
	if (nargs >= 1)
	    TTUnref_ttobj((tt_obj)(opaque)a[0]);
	break;
  
    /* ttevent methods */
    
      case order_Create_ttevent:
	if (nargs >= 3)
	    TTCreate_ttevent((ttuint)a[0], (ttuint)a[1], (ttuint)a[2]);
	break;
      case order_Fire_ttevent:
	if (nargs >= 2)
	    TTFire_ttevent((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;
 
    /* tteventbig methods */
    

      case order_Create8_tteventbig:
	if (nargs >= 11)
	    TTCreate8_tteventbig((ttuint)a[0], (ttuint)a[1], (ttuint)a[2], (ttshort)a[3], (ttshort)a[4], (ttshort)a[5], (ttshort)a[6], (ttany)a[7], (ttany)a[8], (ttuint)a[9], (TT_CONST ttbyte *)(opaque)a[10]);
	break;
      case order_Create4_tteventbig:
	if (nargs >= 7)
	    TTCreate4_tteventbig((ttuint)a[0], (ttuint)a[1], (ttuint)a[2], (ttshort)a[3], (ttshort)a[4], (ttshort)a[5], (ttshort)a[6]);
	break;
      case order_Create4s4_tteventbig:
	if (nargs >= 7)
	    TTCreate4s4_tteventbig((ttuint)a[0], (ttuint)a[1], (ttuint)a[2], (ttany)a[3], (ttany)a[4], (ttuint)a[5], (TT_CONST ttbyte *)(opaque)a[6]);
	break;
 
    /* ttbitmask methods */
    
      case order_Clone_ttbitmask:
	if (nargs >= 1)
	    TTClone_ttbitmask((tt_obj)(opaque)a[0]);
	break;
      case order_Create_ttbitmask:
	if (nargs >= 1)
	    TTCreate_ttbitmask((ttany)a[0]);
	break;
      case order_CreateA_ttbitmask:
	if (nargs >= 2)
	    TTCreateA_ttbitmask((ttopaque)a[0], (TT_ARG_READ ttany *)(opaque)a[1]);
	break;
      case order_Clear_ttbitmask:
	if (nargs >= 1)
	    TTClear_ttbitmask((tt_obj)(opaque)a[0]);
	break;
      case order_Fill_ttbitmask:
	if (nargs >= 1)
	    TTFill_ttbitmask((tt_obj)(opaque)a[0]);
	break;
      case order_Negate_ttbitmask:
	if (nargs >= 1)
	    TTNegate_ttbitmask((tt_obj)(opaque)a[0]);
	break;
      case order_Change_ttbitmask:
	if (nargs >= 2)
	    TTChange_ttbitmask((tt_obj)(opaque)a[0], (ttuint)a[1]);
	break;
      case order_IsMember_ttbitmask:
	if (nargs >= 2)
	    TTIsMember_ttbitmask((tt_obj)(opaque)a[0], (ttany)a[1]);
	break;
      case order_Add_ttbitmask:
	if (nargs >= 2)
	    TTAdd_ttbitmask((tt_obj)(opaque)a[0], (ttany)a[1]);
	break;
      case order_AddA_ttbitmask:
	if (nargs >= 3)
	    TTAddA_ttbitmask((tt_obj)(opaque)a[0], (ttopaque)a[1], (TT_ARG_READ ttany *)(opaque)a[2]);
	break;
      case order_Remove_ttbitmask:
	if (nargs >= 2)
	    TTRemove_ttbitmask((tt_obj)(opaque)a[0], (ttany)a[1]);
	break;
      case order_RemoveA_ttbitmask:
	if (nargs >= 3)
	    TTRemoveA_ttbitmask((tt_obj)(opaque)a[0], (ttopaque)a[1], (TT_ARG_READ ttany *)(opaque)a[2]);
	break;
      case order_Combine_ttbitmask:
	if (nargs >= 3)
	    TTCombine_ttbitmask((tt_obj)(opaque)a[0], (ttany)a[1], (ttuint)a[2]);
	break;
      case order_CombineA_ttbitmask:
	if (nargs >= 4)
	    TTCombineA_ttbitmask((tt_obj)(opaque)a[0], (ttopaque)a[1], (TT_ARG_READ ttany *)(opaque)a[2], (ttuint)a[3]);
	break;
      case order_CombineB_ttbitmask:
	if (nargs >= 3)
	    TTCombineB_ttbitmask((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1], (ttuint)a[2]);
	break;
 
    /* tteventmask methods */
    
      case order_Create_tteventmask:
	if (nargs >= 3)
	    TTCreate_tteventmask((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1], (tt_obj)(opaque)a[2]);
	break;
      case order_SetEvtypeMask_tteventmask:
	if (nargs >= 2)
	    TTSetEvtypeMask_tteventmask((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;
      case order_SetEvcodeMask_tteventmask:
	if (nargs >= 2)
	    TTSetEvcodeMask_tteventmask((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;
      case order_SetComponentMask_tteventmask:
	if (nargs >= 2)
	    TTSetComponentMask_tteventmask((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;
      case order_SetTruthTable_tteventmask:
	if (nargs >= 2)
	    TTSetTruthTable_tteventmask((tt_obj)(opaque)a[0], (ttuint)a[1]);
	break;
 
    /* ttcallback methods */
        
 
    /* ttlistener methods */
    
      case order_Create_ttlistener:
	if (nargs >= 5)
	    TTCreate_ttlistener((tt_obj)(opaque)a[0], (ttuint)a[1], (ttuint)a[2], (ttlistener_fn)(opaque)a[3], (ttany)a[4]);
	break;
      case order_CreateA_ttlistener:
	if (nargs >= 8)
	    TTCreateA_ttlistener((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1], (ttuint)a[2], (ttuint)a[3], (ttuint)a[4], (ttuint)a[5], (ttlistener_fn)(opaque)a[6], (TT_ARG_READ ttany *)(opaque)a[7]);
	break;
      case order_CreateV_ttlistener:
	if (nargs >= 8)
	    TTCreateV_ttlistener((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1], (ttuint)a[2], (ttuint)a[3], (ttuint)a[4], (ttuint)a[5], (ttlistener_fn)(opaque)a[6], (va_list *)(opaque)a[7]);
	break;

      case order_CreateDel_ttlistener:
	if (nargs >= 3)
	    TTCreateDel_ttlistener((tt_obj)(opaque)a[0], (ttuint)a[1], (ttlistener_fn)(opaque)a[2]);
	break;
      case order_CreateAskclose_ttlistener:
	if (nargs >= 3)
	    TTCreateAskclose_ttlistener((tt_obj)(opaque)a[0], (ttuint)a[1], (ttlistener_fn)(opaque)a[2]);
	break;
      case order_CreateKey_ttlistener:
	if (nargs >= 5)
	    TTCreateKey_ttlistener((tt_obj)(opaque)a[0], (ttuint)a[1], (ttuint)a[2], (ttuint)a[3], (ttlistener_fn)(opaque)a[4]);
	break;
      case order_CreateMouse_ttlistener:
	if (nargs >= 5)
	    TTCreateMouse_ttlistener((tt_obj)(opaque)a[0], (ttuint)a[1], (ttuint)a[2], (ttuint)a[3], (ttlistener_fn)(opaque)a[4]);
	break;
      case order_CreateActivate_ttlistener:
	if (nargs >= 3)
	    TTCreateActivate_ttlistener((tt_obj)(opaque)a[0], (ttuint)a[1], (ttlistener_fn)(opaque)a[2]);
	break;
      case order_CreateChange_ttlistener:
	if (nargs >= 4)
	    TTCreateChange_ttlistener((tt_obj)(opaque)a[0], (ttuint)a[1], (ttuint)a[2], (ttlistener_fn)(opaque)a[3]);
	break;
      case order_CreateKeyData_ttlistener:
	if (nargs >= 4)
	    TTCreateKeyData_ttlistener((tt_obj)(opaque)a[0], (TT_ARG_READ ttbyte *)(opaque)a[1], (ttuint)a[2], (ttlistener_fn)(opaque)a[3]);
	break;
      case order_CreateExpose_ttlistener:
	if (nargs >= 3)
	    TTCreateExpose_ttlistener((tt_obj)(opaque)a[0], (ttuint)a[1], (ttlistener_fn)(opaque)a[2]);
	break;
    
      case order_AddTo_ttlistener:
	if (nargs >= 2)
	    TTAddTo_ttlistener((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;    
      case order_Remove_ttlistener:
	if (nargs >= 1)
	    TTRemove_ttlistener((tt_obj)(opaque)a[0]);
	break;
      case order_SetArgs_ttlistener:
	if (nargs >= 4)
	    TTSetArgs_ttlistener((tt_obj)(opaque)a[0], (ttuint)a[1], (ttuint)a[2], (TT_ARG_READ ttany *)(opaque)a[3]);
	break;
      case order_SetEventMask_ttlistener:
	if (nargs >= 2)
	    TTSetEventMask_ttlistener((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;
 
    /* tttimer methods */
    
      case order_CreateA_tttimer:
	if (nargs >= 9)
	    TTCreateA_tttimer((tt_obj)(opaque)a[0], (ttuint)a[1], (ttuint)a[2], (ttuint)a[3], (ttuint)a[4], (ttlistener_fn)(opaque)a[5], (TT_ARG_READ ttany *)(opaque)a[6], (ttuint)a[7], (ttuint)a[8]);
	break;
      case order_Create_tttimer:
	if (nargs >= 6)
	    TTCreate_tttimer((tt_obj)(opaque)a[0], (ttuint)a[1], (ttlistener_fn)(opaque)a[2], (ttany)a[3], (ttuint)a[4], (ttuint)a[5]);
	break;
      case order_SetDelay_tttimer:
	if (nargs >= 3)
	    TTSetDelay_tttimer((tt_obj)(opaque)a[0], (ttuint)a[1], (ttuint)a[2]);
	break;    
      case order_SetEnabled_tttimer:
	if (nargs >= 2)
	    TTSetEnabled_tttimer((tt_obj)(opaque)a[0], (ttbyte)a[1]);
	break;    
      case order_IsEnabled_tttimer:
	if (nargs >= 1)
	    TTIsEnabled_tttimer((tt_obj)(opaque)a[0]);
	break;
 
    /* ttcomponent methods */
    
      case order_AddListener_ttcomponent:
	if (nargs >= 2)
	    TTAddListener_ttcomponent((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;
      case order_RemoveListener_ttcomponent:
	if (nargs >= 2)
	    TTRemoveListener_ttcomponent((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;
      case order_SetKeyData_ttcomponent:
	if (nargs >= 3)
	    TTSetKeyData_ttcomponent((tt_obj)(opaque)a[0], (TT_ARG_READ ttbyte *)(opaque)a[1], (ttany)a[2]);
	break;
      case order_GetKeyData_ttcomponent:
	if (nargs >= 2)
	    TTGetKeyData_ttcomponent((tt_obj)(opaque)a[0], (TT_ARG_READ ttbyte *)(opaque)a[1]);
	break;
      case order_UnsetKeyData_ttcomponent:
	if (nargs >= 2)
	    TTUnsetKeyData_ttcomponent((tt_obj)(opaque)a[0], (TT_ARG_READ ttbyte *)(opaque)a[1]);
	break;
      case order_GetData_ttcomponent:
	if (nargs >= 2)
	    TTGetData_ttcomponent((tt_obj)(opaque)a[0], (TT_ARG_READ ttbyte *)(opaque)a[1]);
	break;
 
    /* ttdata methods */
            
 
    /* ttvisible methods */
        
      case order_AddTo_ttvisible:
	if (nargs >= 2)
	    TTAddTo_ttvisible((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;    
      case order_Remove_ttvisible:
	if (nargs >= 1)
	    TTRemove_ttvisible((tt_obj)(opaque)a[0]);
	break;    
      case order_Invalidate_ttvisible:
	if (nargs >= 1)
	    TTInvalidate_ttvisible((tt_obj)(opaque)a[0]);
	break;    
      case order_SetVisible_ttvisible:
	if (nargs >= 2)
	    TTSetVisible_ttvisible((tt_obj)(opaque)a[0], (ttbyte)a[1]);
	break;    
      case order_SetTheme_ttvisible:
	if (nargs >= 2)
	    TTSetTheme_ttvisible((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;    
      case order_Draw_ttvisible:
	if (nargs >= 9)
	    TTDraw_ttvisible((tt_obj)(opaque)a[0], (ttshort)a[1], (ttshort)a[2], (ttshort)a[3], (ttshort)a[4], (ttshort)a[5], (TT_ARG_READ ttbyte *)(opaque)a[6], (TT_ARG_READ ttfont *)(opaque)a[7], (TT_ARG_READ ttattr *)(opaque)a[8]);
	break;    
      case order_BuiltinRepaint_ttvisible:
	if (nargs >= 5)
	    TTBuiltinRepaint_ttvisible((tt_obj)(opaque)a[0], (ttshort)a[1], (ttshort)a[2], (ttshort)a[3], (ttshort)a[4]);
	break;
      case order_SetRepaint_ttvisible:
	if (nargs >= 2)
	    TTSetRepaint_ttvisible((tt_obj)(opaque)a[0], (ttvisible_repaint_fn)(opaque)a[1]);
	break;
      case order_Add_ttvisible:
	if (nargs >= 2)
	    TTAdd_ttvisible((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;
      case order_Expose_ttvisible:
	if (nargs >= 5)
	    TTExpose_ttvisible((tt_obj)(opaque)a[0], (ttshort)a[1], (ttshort)a[2], (ttshort)a[3], (ttshort)a[4]);
	break;
 
    /* ttnative methods */
        
      case order_GetRoot_ttnative:
	if (nargs >= 0)
	    TTGetRoot_ttnative();
	break;    
      case order_GetW_ttnative:
	if (nargs >= 1)
	    TTGetW_ttnative((tt_obj)(opaque)a[0]);
	break;    
      case order_GetH_ttnative:
	if (nargs >= 1)
	    TTGetH_ttnative((tt_obj)(opaque)a[0]);
	break;
      case order_Create_ttnative:
	if (nargs >= 1)
	    TTCreate_ttnative((ttany)a[0]);
	break;
 
    /* ttwidget methods */
        
      case order_SetXY_ttwidget:
	if (nargs >= 3)
	    TTSetXY_ttwidget((tt_obj)(opaque)a[0], (ttshort)a[1], (ttshort)a[2]);
	break;    
      case order_SetWH_ttwidget:
	if (nargs >= 3)
	    TTSetWH_ttwidget((tt_obj)(opaque)a[0], (ttshort)a[1], (ttshort)a[2]);
	break;    
      case order_SetXl_ttwidget:
	if (nargs >= 2)
	    TTSetXl_ttwidget((tt_obj)(opaque)a[0], (ttint)a[1]);
	break;    
      case order_SetYl_ttwidget:
	if (nargs >= 2)
	    TTSetYl_ttwidget((tt_obj)(opaque)a[0], (ttint)a[1]);
	break;    
      case order_SetWl_ttwidget:
	if (nargs >= 2)
	    TTSetWl_ttwidget((tt_obj)(opaque)a[0], (ttint)a[1]);
	break;    
      case order_SetHl_ttwidget:
	if (nargs >= 2)
	    TTSetHl_ttwidget((tt_obj)(opaque)a[0], (ttint)a[1]);
	break;    
      case order_SetTooltip_ttwidget:
	if (nargs >= 2)
	    TTSetTooltip_ttwidget((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;
      case order_SetXlYl_ttwidget:
	if (nargs >= 4)
	    TTSetXlYl_ttwidget((tt_obj)(opaque)a[0], (ttbyte)a[1], (ttint)a[2], (ttint)a[3]);
	break;
      case order_SetWlHl_ttwidget:
	if (nargs >= 4)
	    TTSetWlHl_ttwidget((tt_obj)(opaque)a[0], (ttbyte)a[1], (ttint)a[2], (ttint)a[3]);
	break;
      case order_SetXYWH_ttwidget:
	if (nargs >= 6)
	    TTSetXYWH_ttwidget((tt_obj)(opaque)a[0], (ttbyte)a[1], (ttshort)a[2], (ttshort)a[3], (ttshort)a[4], (ttshort)a[5]);
	break;
      case order_SetX_ttwidget:
	if (nargs >= 2)
	    TTSetX_ttwidget((tt_obj)(opaque)a[0], (ttshort)a[1]);
	break;
      case order_SetY_ttwidget:
	if (nargs >= 2)
	    TTSetY_ttwidget((tt_obj)(opaque)a[0], (ttshort)a[1]);
	break;
      case order_SetW_ttwidget:
	if (nargs >= 2)
	    TTSetW_ttwidget((tt_obj)(opaque)a[0], (ttshort)a[1]);
	break;
      case order_SetH_ttwidget:
	if (nargs >= 2)
	    TTSetH_ttwidget((tt_obj)(opaque)a[0], (ttshort)a[1]);
	break;
 
    /* ttlabel methods */
        
      case order_DoSetFont_ttlabel:
	if (nargs >= 3)
	    TTDoSetFont_ttlabel((tt_obj)(opaque)a[0], (TT_ARG_DIE ttfont *)(opaque)a[1], (ttopaque)a[2]);
	break;
      case order_SetFont_ttlabel:
	if (nargs >= 2)
	    TTSetFont_ttlabel((tt_obj)(opaque)a[0], (TT_ARG_READ ttfont *)(opaque)a[1]);
	break;
      case order_SetText_ttlabel:
	if (nargs >= 2)
	    TTSetText_ttlabel((tt_obj)(opaque)a[0], (TT_ARG_READ ttbyte *)(opaque)a[1]);
	break;
      case order_Create_ttlabel:
	if (nargs >= 1)
	    TTCreate_ttlabel((TT_ARG_READ ttbyte *)(opaque)a[0]);
	break;
 
    /* tttooltip methods */
        
      case order_SetWidget_tttooltip:
	if (nargs >= 2)
	    TTSetWidget_tttooltip((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;
 
    /* ttbuttongroup methods */
        
      case order_Add_ttbuttongroup:
	if (nargs >= 2)
	    TTAdd_ttbuttongroup((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;    
      case order_Remove_ttbuttongroup:
	if (nargs >= 2)
	    TTRemove_ttbuttongroup((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;    
      case order_SetChecked_ttbuttongroup:
	if (nargs >= 2)
	    TTSetChecked_ttbuttongroup((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;
 
    /* ttanybutton methods */
            
      case order_DoSetAttr_ttanybutton:
	if (nargs >= 5)
	    TTDoSetAttr_ttanybutton((tt_obj)(opaque)a[0], (TT_ARG_DIE ttattr *)(opaque)a[1], (ttshort)a[2], (ttshort)a[3], (ttshort)a[4]);
	break;
      case order_SetAttr_ttanybutton:
	if (nargs >= 5)
	    TTSetAttr_ttanybutton((tt_obj)(opaque)a[0], (TT_ARG_READ ttattr *)(opaque)a[1], (ttshort)a[2], (ttshort)a[3], (ttshort)a[4]);
	break;
      case order_SetFont_ttanybutton:
	if (nargs >= 5)
	    TTSetFont_ttanybutton((tt_obj)(opaque)a[0], (TT_ARG_READ ttfont *)(opaque)a[1], (ttshort)a[2], (ttshort)a[3], (ttshort)a[4]);
	break;
      case order_SetText_ttanybutton:
	if (nargs >= 5)
	    TTSetText_ttanybutton((tt_obj)(opaque)a[0], (TT_ARG_READ ttbyte *)(opaque)a[1], (ttshort)a[2], (ttshort)a[3], (ttshort)a[4]);
	break;
      case order_Create_ttanybutton:
	if (nargs >= 4)
	    TTCreate_ttanybutton((TT_ARG_READ ttbyte *)(opaque)a[0], (ttshort)a[1], (ttshort)a[2], (ttshort)a[3]);
	break;
 
    /* ttbutton methods */
        
      case order_SetPressed_ttbutton:
	if (nargs >= 2)
	    TTSetPressed_ttbutton((tt_obj)(opaque)a[0], (ttbyte)a[1]);
	break;
      case order_IsPressed_ttbutton:
	if (nargs >= 1)
	    TTIsPressed_ttbutton((tt_obj)(opaque)a[0]);
	break;
      case order_Create_ttbutton:
	if (nargs >= 4)
	    TTCreate_ttbutton((TT_ARG_READ ttbyte *)(opaque)a[0], (ttshort)a[1], (ttshort)a[2], (ttshort)a[3]);
	break;
 
    /* ttcheckbutton methods */
        
      case order_SetChecked_ttcheckbutton:
	if (nargs >= 2)
	    TTSetChecked_ttcheckbutton((tt_obj)(opaque)a[0], (ttbyte)a[1]);
	break;
      case order_IsChecked_ttcheckbutton:
	if (nargs >= 1)
	    TTIsChecked_ttcheckbutton((tt_obj)(opaque)a[0]);
	break;
      case order_Create_ttcheckbutton:
	if (nargs >= 4)
	    TTCreate_ttcheckbutton((TT_ARG_READ ttbyte *)(opaque)a[0], (ttshort)a[1], (ttshort)a[2], (ttshort)a[3]);
	break;
 
    /* ttradiobutton methods */
    
      case order_AddToGroup_ttradiobutton:
	if (nargs >= 2)
	    TTAddToGroup_ttradiobutton((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;
      case order_RemoveFromGroup_ttradiobutton:
	if (nargs >= 1)
	    TTRemoveFromGroup_ttradiobutton((tt_obj)(opaque)a[0]);
	break;
      case order_Create_ttradiobutton:
	if (nargs >= 4)
	    TTCreate_ttradiobutton((TT_ARG_READ ttbyte *)(opaque)a[0], (ttshort)a[1], (ttshort)a[2], (ttshort)a[3]);
	break;
 
    /* ttanyscroll methods */
                
      case order_SetOrientation_ttanyscroll:
	if (nargs >= 2)
	    TTSetOrientation_ttanyscroll((tt_obj)(opaque)a[0], (ttbyte)a[1]);
	break;    
      case order_SetSize_ttanyscroll:
	if (nargs >= 2)
	    TTSetSize_ttanyscroll((tt_obj)(opaque)a[0], (ttint)a[1]);
	break;    
      case order_SetRealSize_ttanyscroll:
	if (nargs >= 2)
	    TTSetRealSize_ttanyscroll((tt_obj)(opaque)a[0], (ttint)a[1]);
	break;    
      case order_SetViewSize_ttanyscroll:
	if (nargs >= 2)
	    TTSetViewSize_ttanyscroll((tt_obj)(opaque)a[0], (ttint)a[1]);
	break;    
      case order_SetPosition_ttanyscroll:
	if (nargs >= 2)
	    TTSetPosition_ttanyscroll((tt_obj)(opaque)a[0], (ttint)a[1]);
	break;
 
    /* ttscrollbar methods */
    
      case order_Create1_ttscrollbar:
	if (nargs >= 1)
	    TTCreate1_ttscrollbar((ttbyte)a[0]);
	break;
      case order_Create4_ttscrollbar:
	if (nargs >= 4)
	    TTCreate4_ttscrollbar((ttbyte)a[0], (ttint)a[1], (ttint)a[2], (ttint)a[3]);
	break;
 
    /* ttslider methods */
        
      case order_SetSlideMin_ttslider:
	if (nargs >= 2)
	    TTSetSlideMin_ttslider((tt_obj)(opaque)a[0], (ttint)a[1]);
	break;    
      case order_SetSlideMax_ttslider:
	if (nargs >= 2)
	    TTSetSlideMax_ttslider((tt_obj)(opaque)a[0], (ttint)a[1]);
	break;    
      case order_SetSlideValue_ttslider:
	if (nargs >= 2)
	    TTSetSlideValue_ttslider((tt_obj)(opaque)a[0], (ttint)a[1]);
	break;
      case order_Create_ttslider:
	if (nargs >= 2)
	    TTCreate_ttslider((ttbyte)a[0], (ttint)a[1]);
	break;
 
    /* ttprogressbar methods */
    
      case order_Create_ttprogressbar:
	if (nargs >= 1)
	    TTCreate_ttprogressbar((ttbyte)a[0]);
	break;
 
    /* ttscrollpane methods */
    
      case order_Create_ttscrollpane:
	if (nargs >= 2)
	    TTCreate_ttscrollpane((ttshort)a[0], (ttshort)a[1]);
	break;    
      case order_SetBarX_ttscrollpane:
	if (nargs >= 2)
	    TTSetBarX_ttscrollpane((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;    
      case order_SetBarY_ttscrollpane:
	if (nargs >= 2)
	    TTSetBarY_ttscrollpane((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;
 
    /* ttwindow methods */
        
      case order_SetTitle_ttwindow:
	if (nargs >= 2)
	    TTSetTitle_ttwindow((tt_obj)(opaque)a[0], (TT_ARG_READ ttbyte *)(opaque)a[1]);
	break;
 
    /* ttframe methods */
        
      case order_SetMenubar_ttframe:
	if (nargs >= 2)
	    TTSetMenubar_ttframe((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;
      case order_Create_ttframe:
	if (nargs >= 1)
	    TTCreate_ttframe((TT_ARG_READ ttbyte *)(opaque)a[0]);
	break;
 
    /* ttscroller methods */
    
      case order_Create_ttscroller:
	if (nargs >= 2)
	    TTCreate_ttscroller((ttshort)a[0], (ttshort)a[1]);
	break;
 
    /* ttmenuitem methods */
    
 
    /* ttcheckmenuitem methods */
    
 
    /* ttradiomenuitem methods */
    
 
    /* ttmenu methods */
    
 
    /* ttmenubar methods */
    
      case order_GetDefault_ttmenubar:
	if (nargs >= 0)
	    TTGetDefault_ttmenubar();
	break;
 
    /* ttanytext methods */
    
 
    /* tttextfield methods */
    
 
    /* tttextarea methods */
    
 
    /* tttheme methods */
    
      case order_GetDefault_tttheme:
	if (nargs >= 0)
	    TTGetDefault_tttheme();
	break;
 
    /* ttapplication methods */
    
      case order_Set_ttapplication:
	if (nargs >= 1)
	    TTSet_ttapplication((TT_ARG_READ ttbyte *)(opaque)a[0]);
	break;
      case order_Get_ttapplication:
	if (nargs >= 0)
	    TTGet_ttapplication();
	break;












    /* handy ttobj methods */                                
      case order_SetUserData_ttobj:
	if (nargs >= 2)
	    TTSetUserData_ttobj((tt_obj)(opaque)a[0], (ttany)a[1]);
	break;          
    /* handy ttevent methods */                                    
      case order_Consume_ttevent:
	if (nargs >= 1)
	    TTConsume_ttevent((tt_obj)(opaque)a[0]);
	break; 
    /* handy tteventbig methods */                                     
    /* handy ttbitmask methods */                     
    /* handy tteventmask methods */                     
    /* handy ttcallback methods */                     
    /* handy ttlistener methods */                                             
    /* handy tttimer methods */                     
    /* handy ttcomponent methods */                     
    /* handy ttdata methods */                        
      case order_SetData_ttdata:
	if (nargs >= 2)
	    TTSetData_ttdata((tt_obj)(opaque)a[0], (ttany)a[1]);
	break; 
    /* handy ttvisible methods */                                     
    /* handy ttnative methods */     
    /* handy ttwidget methods */                                             
    /* handy ttlabel methods */             
    /* handy tttooltip methods */                 
    /* handy ttbuttongroup methods */                 
    /* handy ttanybutton methods */                     
    /* handy ttbutton methods */     
    /* handy ttcheckbutton methods */     
    /* handy ttradiobutton methods */                 
    /* handy ttanyscroll methods */                                     
    /* handy ttscrollbar methods */         
    /* handy ttslider methods */                 
    /* handy ttprogressbar methods */             
    /* handy ttscrollpane methods */             
    /* handy ttwindow methods */             
    /* handy ttframe methods */         
    /* handy ttscroller methods */         
    /* handy ttmenuitem methods */         
    /* handy ttcheckmenuitem methods */     
    /* handy ttradiomenuitem methods */     
    /* handy ttmenu methods */    
     
    /* handy ttmenubar methods */     
    /* handy ttanytext methods */             
    /* handy tttextfield methods */     
    /* handy tttextarea methods */     
    /* handy tttheme methods */    
                     
    /* handy ttapplication methods */        






      default:
	break;
    }
}



