



/* This file was automatically generated from m4/call.m4, do not edit! */


/*
 *  call_m4.h  --  implementation of callbacks to libTT methods
 *
 */





















/* prototypes for public and exported methods */



/* ttobj methods */
                                
ttbyte TTGetValue_ttobj(tt_obj a1, ttuint a2, ttany * a3);
    
ttbyte TTSetValue_ttobj(tt_obj a1, ttuint a2, ttany a3);
    
ttbyte TTChangeValue_ttobj(tt_obj a1, ttuint a2, ttany a3, ttany a4);
    
void TTRef_ttobj(tt_obj a1);
    
void TTUnref_ttobj(tt_obj a1);

  
/* ttevent methods */
    
tt_obj TTCreate_ttevent(ttuint a1, ttuint a2, ttuint a3);

 
/* tteventbig methods */

    
tt_obj TTCreate_tteventbig(ttuint a1, ttuint a2, ttuint a3, ttshort a4, ttshort a5, ttshort a6, ttshort a7, ttuint a8, TT_CONST ttbyte * a9);

 
/* ttlistener methods */

    
 
/* ttcallback methods */
    
tt_obj TTCreate_ttcallback(tt_obj a1, ttuint a2, ttuint a3, ttcallback_fn a4, ttany a5);
    
tt_obj TTCreateA_ttcallback(tt_obj a1, tt_obj a2, ttuint a3, ttuint a4, ttuint a5, ttuint a6, ttcallback_fn a7, ttany TT_CONST * a8);
    
tt_obj TTCreateV_ttcallback(tt_obj a1, tt_obj a2, ttuint a3, ttuint a4, ttuint a5, ttuint a6, ttcallback_fn a7, va_list * a8);

 
/* ttcomponent methods */

 
/* ttvisible methods */
    
void TTAddTo_ttvisible(tt_obj a1, tt_obj a2);
    
void TTRemove_ttvisible(tt_obj a1);
    
void TTSetVisible_ttvisible(tt_obj a1, ttbyte a2);
    
void TTSetTheme_ttvisible(tt_obj a1, tt_obj a2);
    
void TTBuiltinRepaint_ttvisible(tt_obj a1, ttshort a2, ttshort a3, ttshort a4, ttshort a5);
    
void TTSetRepaint_ttvisible(tt_obj a1, ttvisible_repaint_fn a2);
    
void TTAdd_ttvisible(tt_obj a1, tt_obj a2);
    
void TTExpose_ttvisible(tt_obj a1, ttshort a2, ttshort a3, ttshort a4, ttshort a5);

 
/* ttnative methods */
    
tt_obj TTGetRoot_ttnative(void);
    
tt_obj TTCreate_ttnative(ttany a1);

 
/* ttwidget methods */
    
void TTSetXYWH_ttwidget(tt_obj a1, ttbyte a2, ttshort a3, ttshort a4, ttshort a5, ttshort a6);
    
void TTSetXlYl_ttwidget(tt_obj a1, ttbyte a2, ttint a3, ttint a4);
    
void TTSetXY_ttwidget(tt_obj a1, ttshort a2, ttshort a3);
    
void TTSetWH_ttwidget(tt_obj a1, ttshort a2, ttshort a3);
    
void TTSetXl_ttwidget(tt_obj a1, ttint a2);
    
void TTSetYl_ttwidget(tt_obj a1, ttint a2);

 
/* ttlabel methods */
    
ttbyte TTSetText_ttlabel(tt_obj a1, TT_CONST ttbyte * a2);
    
tt_obj TTCreate_ttlabel(TT_CONST ttbyte * a1);

 
/* ttanybutton methods */
    
ttbyte TTSetText_ttanybutton(tt_obj a1, TT_CONST ttfont * a2, ttshort a3, ttshort a4, ttshort a5);
    
tt_obj TTCreate_ttanybutton(TT_CONST ttfont * a1, ttshort a2, ttshort a3, ttshort a4);

 
/* ttbutton methods */
    
void TTSetPressed_ttbutton(tt_obj a1, ttbyte a2);
    
tt_obj TTCreate_ttbutton(TT_CONST ttfont * a1, ttshort a2, ttshort a3, ttshort a4);
    
ttbyte TTIsPressed_ttbutton(tt_obj a1);

 
/* ttcheckbutton methods */

 
/* ttradiobutton methods */

 
/* ttscrollbar methods */

 
/* ttbuttongroup methods */

 
/* ttwindow methods */

 
/* ttframe methods */

 
/* ttscroller methods */

 
/* ttmenuitem methods */

 
/* ttcheckmenuitem methods */

 
/* ttradiomenuitem methods */

 
/* ttmenu methods */

 
/* ttmenubar methods */
    
tt_obj TTGetDefault_ttmenubar(void);

 
/* ttanytext methods */

 
/* tttextfield methods */

 
/* tttextarea methods */

 
/* tttheme methods */
        
tt_obj TTGetDefault_tttheme(void);

 
/* ttapplication methods */
    
tt_obj TTSet_ttapplication(TT_CONST ttbyte * a1);







/* prototypes for handy public and exported methods */


















/* prototypes for handy ttobj methods */
        
ttopaque TTGetId_ttobj(tt_obj o);
    
ttuint TTGetRefcount_ttobj(tt_obj o);
    
ttuint TTGetOflags_ttobj(tt_obj o);
            
ttany TTGetUserData_ttobj(tt_obj o);

void TTSetUserData_ttobj(tt_obj o, ttany user_data);
    
ttuint TTGetEventsInprogress_ttobj(tt_obj o);
  
/* prototypes for handy ttevent methods */
        
tt_obj TTGetCallback_ttevent(tt_obj o);
    
tt_obj TTGetComponent_ttevent(tt_obj o);
    
ttuint TTGetEvtype_ttevent(tt_obj o);
    
ttuint TTGetEvcode_ttevent(tt_obj o);
    
ttuint TTGetEvflags_ttevent(tt_obj o);
    
void TTConsume_ttevent(tt_obj o);
 
/* prototypes for handy tteventbig methods */
        
ttshort TTGetX_tteventbig(tt_obj o);
    
ttshort TTGetY_tteventbig(tt_obj o);
    
ttshort TTGetW_tteventbig(tt_obj o);
    
ttshort TTGetH_tteventbig(tt_obj o);
    
ttuint TTGetLen_tteventbig(tt_obj o);
    
TT_CONST ttbyte * TTGetData_tteventbig(tt_obj o);
 
/* prototypes for handy ttlistener methods */
        
ttuint TTGetLflags_ttlistener(tt_obj o);
    
tt_obj TTGetComponent_ttlistener(tt_obj o);
    
tt_obj TTGetPrev_ttlistener(tt_obj o);
    
tt_obj TTGetNext_ttlistener(tt_obj o);
 
/* prototypes for handy ttcallback methods */
    
tt_obj TTGetAvlLeft_ttcallback(tt_obj o);
    
tt_obj TTGetAvlRight_ttcallback(tt_obj o);
    
tt_obj TTGetAvlParent_ttcallback(tt_obj o);
            
tt_obj TTGetEvent_ttcallback(tt_obj o);
    
ttuint TTGetNargComponent_ttcallback(tt_obj o);
    
ttuint TTGetNargEvent_ttcallback(tt_obj o);
    
ttuint TTGetNargs_ttcallback(tt_obj o);
    
ttcallback_fn TTGetFunction_ttcallback(tt_obj o);
        
TT_CONST ttany * TTGetArgs_ttcallback(tt_obj o);
 
/* prototypes for handy ttcomponent methods */
        
tt_obj TTGetListeners_ttcomponent(tt_obj o);
    
tt_obj TTGetCallbacks_ttcomponent(tt_obj o);
 
/* prototypes for handy ttvisible methods */
        
ttuint TTGetVflags_ttvisible(tt_obj o);
    
tt_obj TTGetPrev_ttvisible(tt_obj o);
    
tt_obj TTGetNext_ttvisible(tt_obj o);
    
tt_obj TTGetParent_ttvisible(tt_obj o);
    
tt_obj TTGetChildFirst_ttvisible(tt_obj o);
    
tt_obj TTGetChildLast_ttvisible(tt_obj o);
    
tt_obj TTGetTheme_ttvisible(tt_obj o);
    
ttvisible_repaint_fn TTGetRepaint_ttvisible(tt_obj o);
 
/* prototypes for handy ttnative methods */
     
/* prototypes for handy ttwidget methods */
        
ttshort TTGetX_ttwidget(tt_obj o);
    
ttshort TTGetY_ttwidget(tt_obj o);
    
ttshort TTGetW_ttwidget(tt_obj o);
    
ttshort TTGetH_ttwidget(tt_obj o);
    
ttint TTGetXl_ttwidget(tt_obj o);
    
ttint TTGetYl_ttwidget(tt_obj o);
 
/* prototypes for handy ttlabel methods */
        
ttshort TTGetTextLen_ttlabel(tt_obj o);
        
TT_CONST ttfont * TTGetText_ttlabel(tt_obj o);
 
/* prototypes for handy ttanybutton methods */
            
ttshort TTGetTextWidth_ttanybutton(tt_obj o);
    
ttshort TTGetTextHeight_ttanybutton(tt_obj o);
        
TT_CONST ttfont * TTGetText_ttanybutton(tt_obj o);
 
/* prototypes for handy ttbutton methods */
     
/* prototypes for handy ttcheckbutton methods */
     
/* prototypes for handy ttradiobutton methods */
     
/* prototypes for handy ttscrollbar methods */
     
/* prototypes for handy ttbuttongroup methods */
     
/* prototypes for handy ttwindow methods */
         
/* prototypes for handy ttframe methods */
     
/* prototypes for handy ttscroller methods */
        
tt_obj TTGetScrollx_ttscroller(tt_obj o);
    
tt_obj TTGetScrolly_ttscroller(tt_obj o);
 
/* prototypes for handy ttmenuitem methods */
         
/* prototypes for handy ttcheckmenuitem methods */
     
/* prototypes for handy ttradiomenuitem methods */
     
/* prototypes for handy ttmenu methods */
    
    
tt_obj TTGetMenubar_ttmenu(tt_obj o);
 
/* prototypes for handy ttmenubar methods */
     
/* prototypes for handy ttanytext methods */
        
ttshort TTGetTextLen_ttanytext(tt_obj o);
        
TT_CONST ttfont * TTGetText_ttanytext(tt_obj o);
 
/* prototypes for handy tttextfield methods */
     
/* prototypes for handy tttextarea methods */
     
/* prototypes for handy tttheme methods */
    
            
        
TT_CONST ttbyte * TTGetName_tttheme(tt_obj o);
 
/* prototypes for handy ttapplication methods */
        
ttbyte * TTGetName_ttapplication(tt_obj o);








/* enum for method callbacks */

typedef enum e_order_methods {
    /* generic functions */
    order_BAD = -1,
    order_ExitMainLoop,
    order_New,
    order_Del,



    /* ttobj methods */                                
    order_GetValue_ttobj,    
    order_SetValue_ttobj,    
    order_ChangeValue_ttobj,    
    order_Ref_ttobj,    
    order_Unref_ttobj,  
    /* ttevent methods */    
    order_Create_ttevent, 
    /* tteventbig methods */
    
    order_Create_tteventbig, 
    /* ttlistener methods */
     
    /* ttcallback methods */    
    order_Create_ttcallback,    
    order_CreateA_ttcallback,    
    order_CreateV_ttcallback, 
    /* ttcomponent methods */ 
    /* ttvisible methods */    
    order_AddTo_ttvisible,    
    order_Remove_ttvisible,    
    order_SetVisible_ttvisible,    
    order_SetTheme_ttvisible,    
    order_BuiltinRepaint_ttvisible,    
    order_SetRepaint_ttvisible,    
    order_Add_ttvisible,    
    order_Expose_ttvisible, 
    /* ttnative methods */    
    order_GetRoot_ttnative,    
    order_Create_ttnative, 
    /* ttwidget methods */    
    order_SetXYWH_ttwidget,    
    order_SetXlYl_ttwidget,    
    order_SetXY_ttwidget,    
    order_SetWH_ttwidget,    
    order_SetXl_ttwidget,    
    order_SetYl_ttwidget, 
    /* ttlabel methods */    
    order_SetText_ttlabel,    
    order_Create_ttlabel, 
    /* ttanybutton methods */    
    order_SetText_ttanybutton,    
    order_Create_ttanybutton, 
    /* ttbutton methods */    
    order_SetPressed_ttbutton,    
    order_Create_ttbutton,    
    order_IsPressed_ttbutton, 
    /* ttcheckbutton methods */ 
    /* ttradiobutton methods */ 
    /* ttscrollbar methods */ 
    /* ttbuttongroup methods */ 
    /* ttwindow methods */ 
    /* ttframe methods */ 
    /* ttscroller methods */ 
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




    order_FN_handy,







    /* handy ttobj methods */        
    order_GetId_ttobj,    
    order_GetRefcount_ttobj,    
    order_GetOflags_ttobj,            
    order_GetUserData_ttobj,
    order_SetUserData_ttobj,    
    order_GetEventsInprogress_ttobj,  
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
    order_GetLen_tteventbig,    
    order_GetData_tteventbig, 
    /* handy ttlistener methods */        
    order_GetLflags_ttlistener,    
    order_GetComponent_ttlistener,    
    order_GetPrev_ttlistener,    
    order_GetNext_ttlistener, 
    /* handy ttcallback methods */    
    order_GetAvlLeft_ttcallback,    
    order_GetAvlRight_ttcallback,    
    order_GetAvlParent_ttcallback,            
    order_GetEvent_ttcallback,    
    order_GetNargComponent_ttcallback,    
    order_GetNargEvent_ttcallback,    
    order_GetNargs_ttcallback,    
    order_GetFunction_ttcallback,        
    order_GetArgs_ttcallback, 
    /* handy ttcomponent methods */        
    order_GetListeners_ttcomponent,    
    order_GetCallbacks_ttcomponent, 
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
    /* handy ttlabel methods */        
    order_GetTextLen_ttlabel,        
    order_GetText_ttlabel, 
    /* handy ttanybutton methods */            
    order_GetTextWidth_ttanybutton,    
    order_GetTextHeight_ttanybutton,        
    order_GetText_ttanybutton, 
    /* handy ttbutton methods */     
    /* handy ttcheckbutton methods */     
    /* handy ttradiobutton methods */     
    /* handy ttscrollbar methods */     
    /* handy ttbuttongroup methods */     
    /* handy ttwindow methods */         
    /* handy ttframe methods */     
    /* handy ttscroller methods */        
    order_GetScrollx_ttscroller,    
    order_GetScrolly_ttscroller, 
    /* handy ttmenuitem methods */         
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






    order_FN_n
} e_order_methods;


typedef struct s_ttmethod s_ttmethod;
typedef struct s_ttmethod *ttmethod;
struct s_ttmethod {
    opaque mtho;
    void *mth;
};

/* array for method callbacks */

static struct s_ttmethod method_array[] = {
    /* generic functions */
    { order_ExitMainLoop, (void *)TTExitMainLoop, },
    { order_New, (void *)TTNew, },
    { order_Del, (void *)TTDel, }, 



    /* ttobj methods */                                
    { order_GetValue_ttobj, (void *)TTGetValue_ttobj, },    
    { order_SetValue_ttobj, (void *)TTSetValue_ttobj, },    
    { order_ChangeValue_ttobj, (void *)TTChangeValue_ttobj, },    
    { order_Ref_ttobj, (void *)TTRef_ttobj, },    
    { order_Unref_ttobj, (void *)TTUnref_ttobj, },  
    /* ttevent methods */    
    { order_Create_ttevent, (void *)TTCreate_ttevent, }, 
    /* tteventbig methods */
    
    { order_Create_tteventbig, (void *)TTCreate_tteventbig, }, 
    /* ttlistener methods */
     
    /* ttcallback methods */    
    { order_Create_ttcallback, (void *)TTCreate_ttcallback, },    
    { order_CreateA_ttcallback, (void *)TTCreateA_ttcallback, },    
    { order_CreateV_ttcallback, (void *)TTCreateV_ttcallback, }, 
    /* ttcomponent methods */ 
    /* ttvisible methods */    
    { order_AddTo_ttvisible, (void *)TTAddTo_ttvisible, },    
    { order_Remove_ttvisible, (void *)TTRemove_ttvisible, },    
    { order_SetVisible_ttvisible, (void *)TTSetVisible_ttvisible, },    
    { order_SetTheme_ttvisible, (void *)TTSetTheme_ttvisible, },    
    { order_BuiltinRepaint_ttvisible, (void *)TTBuiltinRepaint_ttvisible, },    
    { order_SetRepaint_ttvisible, (void *)TTSetRepaint_ttvisible, },    
    { order_Add_ttvisible, (void *)TTAdd_ttvisible, },    
    { order_Expose_ttvisible, (void *)TTExpose_ttvisible, }, 
    /* ttnative methods */    
    { order_GetRoot_ttnative, (void *)TTGetRoot_ttnative, },    
    { order_Create_ttnative, (void *)TTCreate_ttnative, }, 
    /* ttwidget methods */    
    { order_SetXYWH_ttwidget, (void *)TTSetXYWH_ttwidget, },    
    { order_SetXlYl_ttwidget, (void *)TTSetXlYl_ttwidget, },    
    { order_SetXY_ttwidget, (void *)TTSetXY_ttwidget, },    
    { order_SetWH_ttwidget, (void *)TTSetWH_ttwidget, },    
    { order_SetXl_ttwidget, (void *)TTSetXl_ttwidget, },    
    { order_SetYl_ttwidget, (void *)TTSetYl_ttwidget, }, 
    /* ttlabel methods */    
    { order_SetText_ttlabel, (void *)TTSetText_ttlabel, },    
    { order_Create_ttlabel, (void *)TTCreate_ttlabel, }, 
    /* ttanybutton methods */    
    { order_SetText_ttanybutton, (void *)TTSetText_ttanybutton, },    
    { order_Create_ttanybutton, (void *)TTCreate_ttanybutton, }, 
    /* ttbutton methods */    
    { order_SetPressed_ttbutton, (void *)TTSetPressed_ttbutton, },    
    { order_Create_ttbutton, (void *)TTCreate_ttbutton, },    
    { order_IsPressed_ttbutton, (void *)TTIsPressed_ttbutton, }, 
    /* ttcheckbutton methods */ 
    /* ttradiobutton methods */ 
    /* ttscrollbar methods */ 
    /* ttbuttongroup methods */ 
    /* ttwindow methods */ 
    /* ttframe methods */ 
    /* ttscroller methods */ 
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










    /* handy ttobj methods */        
    { order_GetId_ttobj, (void *)TTGetId_ttobj, },    
    { order_GetRefcount_ttobj, (void *)TTGetRefcount_ttobj, },    
    { order_GetOflags_ttobj, (void *)TTGetOflags_ttobj, },            
    { order_GetUserData_ttobj, (void *)TTGetUserData_ttobj, },
    { order_SetUserData_ttobj, (void *)TTSetUserData_ttobj, },    
    { order_GetEventsInprogress_ttobj, (void *)TTGetEventsInprogress_ttobj, },  
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
    { order_GetLen_tteventbig, (void *)TTGetLen_tteventbig, },    
    { order_GetData_tteventbig, (void *)TTGetData_tteventbig, }, 
    /* handy ttlistener methods */        
    { order_GetLflags_ttlistener, (void *)TTGetLflags_ttlistener, },    
    { order_GetComponent_ttlistener, (void *)TTGetComponent_ttlistener, },    
    { order_GetPrev_ttlistener, (void *)TTGetPrev_ttlistener, },    
    { order_GetNext_ttlistener, (void *)TTGetNext_ttlistener, }, 
    /* handy ttcallback methods */    
    { order_GetAvlLeft_ttcallback, (void *)TTGetAvlLeft_ttcallback, },    
    { order_GetAvlRight_ttcallback, (void *)TTGetAvlRight_ttcallback, },    
    { order_GetAvlParent_ttcallback, (void *)TTGetAvlParent_ttcallback, },            
    { order_GetEvent_ttcallback, (void *)TTGetEvent_ttcallback, },    
    { order_GetNargComponent_ttcallback, (void *)TTGetNargComponent_ttcallback, },    
    { order_GetNargEvent_ttcallback, (void *)TTGetNargEvent_ttcallback, },    
    { order_GetNargs_ttcallback, (void *)TTGetNargs_ttcallback, },    
    { order_GetFunction_ttcallback, (void *)TTGetFunction_ttcallback, },        
    { order_GetArgs_ttcallback, (void *)TTGetArgs_ttcallback, }, 
    /* handy ttcomponent methods */        
    { order_GetListeners_ttcomponent, (void *)TTGetListeners_ttcomponent, },    
    { order_GetCallbacks_ttcomponent, (void *)TTGetCallbacks_ttcomponent, }, 
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
    /* handy ttlabel methods */        
    { order_GetTextLen_ttlabel, (void *)TTGetTextLen_ttlabel, },        
    { order_GetText_ttlabel, (void *)TTGetText_ttlabel, }, 
    /* handy ttanybutton methods */            
    { order_GetTextWidth_ttanybutton, (void *)TTGetTextWidth_ttanybutton, },    
    { order_GetTextHeight_ttanybutton, (void *)TTGetTextHeight_ttanybutton, },        
    { order_GetText_ttanybutton, (void *)TTGetText_ttanybutton, }, 
    /* handy ttbutton methods */     
    /* handy ttcheckbutton methods */     
    /* handy ttradiobutton methods */     
    /* handy ttscrollbar methods */     
    /* handy ttbuttongroup methods */     
    /* handy ttwindow methods */         
    /* handy ttframe methods */     
    /* handy ttscroller methods */        
    { order_GetScrollx_ttscroller, (void *)TTGetScrollx_ttscroller, },    
    { order_GetScrolly_ttscroller, (void *)TTGetScrolly_ttscroller, }, 
    /* handy ttmenuitem methods */         
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
    return (opaque)order_BAD;
}







/*
 * here and below, we do consider TTGet*() methods specially:
 * they have no side effects, and if called from within a callback they
 * cannot return a useful return value, so we can do without calling them.
 */
static void CallMethod(ttuint order, ttuint nargs, TT_CONST ttany *a) {

    switch (order) {
    
    /* generic functions */
    
      case order_New:
	if (nargs >= 1)
	    TTNew((tt_fn)(opaque)a[0]);
	break;
      case order_Del:
	if (nargs >= 1)
	    TTDel((tt_obj)(opaque)a[0]);
	break;
      case order_ExitMainLoop:
	if (nargs >= 0)
	    TTExitMainLoop();
	break;





    /* ttobj methods */
                                    
      case order_GetValue_ttobj:
	if (nargs >= 3)
	    TTGetValue_ttobj((tt_obj)(opaque)a[0], (ttuint)a[1], (ttany *)(opaque)a[2]);
	break;    
      case order_SetValue_ttobj:
	if (nargs >= 3)
	    TTSetValue_ttobj((tt_obj)(opaque)a[0], (ttuint)a[1], (ttany)a[2]);
	break;    
      case order_ChangeValue_ttobj:
	if (nargs >= 4)
	    TTChangeValue_ttobj((tt_obj)(opaque)a[0], (ttuint)a[1], (ttany)a[2], (ttany)a[3]);
	break;    
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
 
    /* tteventbig methods */
    
    
      case order_Create_tteventbig:
	if (nargs >= 9)
	    TTCreate_tteventbig((ttuint)a[0], (ttuint)a[1], (ttuint)a[2], (ttshort)a[3], (ttshort)a[4], (ttshort)a[5], (ttshort)a[6], (ttuint)a[7], (TT_CONST ttbyte *)(opaque)a[8]);
	break;
 
    /* ttlistener methods */
    
    
 
    /* ttcallback methods */
        
      case order_Create_ttcallback:
	if (nargs >= 5)
	    TTCreate_ttcallback((tt_obj)(opaque)a[0], (ttuint)a[1], (ttuint)a[2], (ttcallback_fn)(opaque)a[3], (ttany)a[4]);
	break;    
      case order_CreateA_ttcallback:
	if (nargs >= 8)
	    TTCreateA_ttcallback((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1], (ttuint)a[2], (ttuint)a[3], (ttuint)a[4], (ttuint)a[5], (ttcallback_fn)(opaque)a[6], (ttany TT_CONST *)(opaque)a[7]);
	break;    
      case order_CreateV_ttcallback:
	if (nargs >= 8)
	    TTCreateV_ttcallback((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1], (ttuint)a[2], (ttuint)a[3], (ttuint)a[4], (ttuint)a[5], (ttcallback_fn)(opaque)a[6], (va_list *)(opaque)a[7]);
	break;
 
    /* ttcomponent methods */
    
 
    /* ttvisible methods */
        
      case order_AddTo_ttvisible:
	if (nargs >= 2)
	    TTAddTo_ttvisible((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;    
      case order_Remove_ttvisible:
	if (nargs >= 1)
	    TTRemove_ttvisible((tt_obj)(opaque)a[0]);
	break;    
      case order_SetVisible_ttvisible:
	if (nargs >= 2)
	    TTSetVisible_ttvisible((tt_obj)(opaque)a[0], (ttbyte)a[1]);
	break;    
      case order_SetTheme_ttvisible:
	if (nargs >= 2)
	    TTSetTheme_ttvisible((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
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
      case order_Create_ttnative:
	if (nargs >= 1)
	    TTCreate_ttnative((ttany)a[0]);
	break;
 
    /* ttwidget methods */
        
      case order_SetXYWH_ttwidget:
	if (nargs >= 6)
	    TTSetXYWH_ttwidget((tt_obj)(opaque)a[0], (ttbyte)a[1], (ttshort)a[2], (ttshort)a[3], (ttshort)a[4], (ttshort)a[5]);
	break;    
      case order_SetXlYl_ttwidget:
	if (nargs >= 4)
	    TTSetXlYl_ttwidget((tt_obj)(opaque)a[0], (ttbyte)a[1], (ttint)a[2], (ttint)a[3]);
	break;    
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
 
    /* ttlabel methods */
        
      case order_SetText_ttlabel:
	if (nargs >= 2)
	    TTSetText_ttlabel((tt_obj)(opaque)a[0], (TT_CONST ttbyte *)(opaque)a[1]);
	break;    
      case order_Create_ttlabel:
	if (nargs >= 1)
	    TTCreate_ttlabel((TT_CONST ttbyte *)(opaque)a[0]);
	break;
 
    /* ttanybutton methods */
        
      case order_SetText_ttanybutton:
	if (nargs >= 5)
	    TTSetText_ttanybutton((tt_obj)(opaque)a[0], (TT_CONST ttfont *)(opaque)a[1], (ttshort)a[2], (ttshort)a[3], (ttshort)a[4]);
	break;    
      case order_Create_ttanybutton:
	if (nargs >= 4)
	    TTCreate_ttanybutton((TT_CONST ttfont *)(opaque)a[0], (ttshort)a[1], (ttshort)a[2], (ttshort)a[3]);
	break;
 
    /* ttbutton methods */
        
      case order_SetPressed_ttbutton:
	if (nargs >= 2)
	    TTSetPressed_ttbutton((tt_obj)(opaque)a[0], (ttbyte)a[1]);
	break;    
      case order_Create_ttbutton:
	if (nargs >= 4)
	    TTCreate_ttbutton((TT_CONST ttfont *)(opaque)a[0], (ttshort)a[1], (ttshort)a[2], (ttshort)a[3]);
	break;    
      case order_IsPressed_ttbutton:
	if (nargs >= 1)
	    TTIsPressed_ttbutton((tt_obj)(opaque)a[0]);
	break;
 
    /* ttcheckbutton methods */
    
 
    /* ttradiobutton methods */
    
 
    /* ttscrollbar methods */
    
 
    /* ttbuttongroup methods */
    
 
    /* ttwindow methods */
    
 
    /* ttframe methods */
    
 
    /* ttscroller methods */
    
 
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
	    TTSet_ttapplication((TT_CONST ttbyte *)(opaque)a[0]);
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
    /* handy ttlistener methods */                     
    /* handy ttcallback methods */                                                 
    /* handy ttcomponent methods */             
    /* handy ttvisible methods */                                     
    /* handy ttnative methods */     
    /* handy ttwidget methods */                             
    /* handy ttlabel methods */                 
    /* handy ttanybutton methods */                         
    /* handy ttbutton methods */     
    /* handy ttcheckbutton methods */     
    /* handy ttradiobutton methods */     
    /* handy ttscrollbar methods */     
    /* handy ttbuttongroup methods */     
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



