
/* This file was automatically generated from m4/handy.m4, do not edit! */



/*
 *  handy_m4.h  --  public header for handy libTT functions
 *
 *  Copyright (C) 2002 by Massimiliano Ghilardi
 *
 */

#ifndef _TT_HANDY_M4_H
#define _TT_HANDY_M4_H












                                      





















/* handy ttobj methods */
            
ttopaque	TTGetId_ttobj(ttobj o);    
ttuint	TTGetRefcount_ttobj(ttobj o);    
ttuint	TTGetOflags_ttobj(ttobj o);            
ttany	TTGetUserData_ttobj(ttobj o);
void	TTSetUserData_ttobj(ttobj o, ttany user_data);    
ttopaque	TTGetEventsInprogress_ttobj(ttobj o);    
ttopaque	TTGetEventsInformed_ttobj(ttobj o);  

/* handy ttevent methods */
        
ttlistener	TTGetCallback_ttevent(ttevent o);    
ttcomponent	TTGetComponent_ttevent(ttevent o);    
ttuint	TTGetEvtype_ttevent(ttevent o);    
ttuint	TTGetEvcode_ttevent(ttevent o);    
ttuint	TTGetEvflags_ttevent(ttevent o);            
void	TTConsume_ttevent(ttevent o); 

/* handy tteventbig methods */
        
ttshort	TTGetX_tteventbig(tteventbig o);    
ttshort	TTGetY_tteventbig(tteventbig o);    
ttshort	TTGetW_tteventbig(tteventbig o);    
ttshort	TTGetH_tteventbig(tteventbig o);    
ttany	TTGetValue_tteventbig(tteventbig o);    
ttany	TTGetOldValue_tteventbig(tteventbig o);    
ttuint	TTGetLen_tteventbig(tteventbig o);    
TT_CONST ttbyte *	TTGetData_tteventbig(tteventbig o); 

/* handy ttbitmask methods */
                     

/* handy tteventmask methods */
        
ttbitmask	TTGetEvtypeMask_tteventmask(tteventmask o);    
ttbitmask	TTGetEvcodeMask_tteventmask(tteventmask o);    
ttbitmask	TTGetComponentMask_tteventmask(tteventmask o);    
ttuint	TTGetTruthTable_tteventmask(tteventmask o); 

/* handy ttcallback methods */
        
ttuint	TTGetLflags_ttcallback(ttcallback o);    
ttcomponent	TTGetComponent_ttcallback(ttcallback o);    
ttcallback	TTGetPrev_ttcallback(ttcallback o);    
ttcallback	TTGetNext_ttcallback(ttcallback o); 

/* handy ttlistener methods */
            
ttevent	TTGetEvent_ttlistener(ttlistener o);    
ttuint	TTGetNargComponent_ttlistener(ttlistener o);    
ttuint	TTGetNargEvent_ttlistener(ttlistener o);    
ttuint	TTGetNargs_ttlistener(ttlistener o);    
ttlistener_fn	TTGetFunction_ttlistener(ttlistener o);    
ttany *	TTGetArgs_ttlistener(ttlistener o);    
tteventmask	TTGetEventMask_ttlistener(ttlistener o);         

/* handy tttimer methods */
                     

/* handy ttcomponent methods */
        
ttcallback	TTGetCallbacks_ttcomponent(ttcomponent o);    
ttlistener	TTGetListeners_ttcomponent(ttcomponent o);    
tttimer	TTGetTimers_ttcomponent(ttcomponent o);    
ttdata	TTGetDatas_ttcomponent(ttcomponent o); 

/* handy ttdata methods */
            
ttcomponent	TTGetComponent_ttdata(ttdata o);    
ttbyte *	TTGetKey_ttdata(ttdata o);    
ttopaque	TTGetKeyLen_ttdata(ttdata o);    
ttany	TTGetData_ttdata(ttdata o);
void	TTSetData_ttdata(ttdata o, ttany data); 

/* handy ttvisible methods */
        
ttuint	TTGetVflags_ttvisible(ttvisible o);    
ttvisible	TTGetPrev_ttvisible(ttvisible o);    
ttvisible	TTGetNext_ttvisible(ttvisible o);    
ttvisible	TTGetParent_ttvisible(ttvisible o);    
ttvisible	TTGetChildFirst_ttvisible(ttvisible o);    
ttvisible	TTGetChildLast_ttvisible(ttvisible o);    
tttheme	TTGetTheme_ttvisible(ttvisible o);    
ttvisible_repaint_fn	TTGetRepaint_ttvisible(ttvisible o); 

/* handy ttnative methods */
     

/* handy ttwidget methods */
        
ttshort	TTGetX_ttwidget(ttwidget o);    
ttshort	TTGetY_ttwidget(ttwidget o);    
ttshort	TTGetW_ttwidget(ttwidget o);    
ttshort	TTGetH_ttwidget(ttwidget o);    
ttint	TTGetXl_ttwidget(ttwidget o);    
ttint	TTGetYl_ttwidget(ttwidget o);    
ttint	TTGetWl_ttwidget(ttwidget o);    
ttint	TTGetHl_ttwidget(ttwidget o);        
tttooltip	TTGetTooltip_ttwidget(ttwidget o); 

/* handy ttlabel methods */
        
ttopaque	TTGetTextLen_ttlabel(ttlabel o);    
ttfont *	TTGetText_ttlabel(ttlabel o); 

/* handy tttooltip methods */
            
ttwidget	TTGetWidget_tttooltip(tttooltip o);     

/* handy ttbuttongroup methods */
        
ttradiobutton	TTGetGroupFirst_ttbuttongroup(ttbuttongroup o);    
ttradiobutton	TTGetGroupLast_ttbuttongroup(ttbuttongroup o);    
ttradiobutton	TTGetChecked_ttbuttongroup(ttbuttongroup o); 

/* handy ttanybutton methods */
        
ttattr *	TTGetText_ttanybutton(ttanybutton o);    
ttshort	TTGetTextWidth_ttanybutton(ttanybutton o);    
ttshort	TTGetTextHeight_ttanybutton(ttanybutton o);     

/* handy ttbutton methods */
     

/* handy ttcheckbutton methods */
     

/* handy ttradiobutton methods */
        
ttbuttongroup	TTGetGroup_ttradiobutton(ttradiobutton o);    
ttradiobutton	TTGetGroupPrev_ttradiobutton(ttradiobutton o);    
ttradiobutton	TTGetGroupNext_ttradiobutton(ttradiobutton o); 

/* handy ttanyscroll methods */
        
ttbyte	TTGetOrientation_ttanyscroll(ttanyscroll o);    
ttint	TTGetSize_ttanyscroll(ttanyscroll o);    
ttint	TTGetRealSize_ttanyscroll(ttanyscroll o);    
ttint	TTGetViewSize_ttanyscroll(ttanyscroll o);        
ttint	TTGetPosition_ttanyscroll(ttanyscroll o);    
ttint	TTGetRealPosition_ttanyscroll(ttanyscroll o);     

/* handy ttscrollbar methods */
         

/* handy ttslider methods */
        
ttint	TTGetSlideMin_ttslider(ttslider o);    
ttint	TTGetSlideMax_ttslider(ttslider o);    
ttint	TTGetSlideValue_ttslider(ttslider o); 

/* handy ttprogressbar methods */
             

/* handy ttscrollpane methods */
        
ttscrollbar	TTGetBarX_ttscrollpane(ttscrollpane o);    
ttscrollbar	TTGetBarY_ttscrollpane(ttscrollpane o); 

/* handy ttwindow methods */
        
ttopaque	TTGetTitleLen_ttwindow(ttwindow o);    
ttbyte *	TTGetTitle_ttwindow(ttwindow o); 

/* handy ttframe methods */
        
ttmenubar	TTGetMenubar_ttframe(ttframe o); 

/* handy ttscroller methods */
        
ttscrollpane	TTGetScrollpane_ttscroller(ttscroller o); 

/* handy ttmenuitem methods */
        
ttbyte *	TTGetName_ttmenuitem(ttmenuitem o); 

/* handy ttcheckmenuitem methods */
     

/* handy ttradiomenuitem methods */
     

/* handy ttmenu methods */
    
    
ttmenubar	TTGetMenubar_ttmenu(ttmenu o); 

/* handy ttmenubar methods */
     

/* handy ttanytext methods */
        
ttopaque	TTGetTextLen_ttanytext(ttanytext o);    
ttattr *	TTGetText_ttanytext(ttanytext o); 

/* handy tttextfield methods */
     

/* handy tttextarea methods */
     

/* handy tttheme methods */
    
    
ttbyte *	TTGetName_tttheme(tttheme o);                 

/* handy ttapplication methods */
        
ttbyte *	TTGetName_ttapplication(ttapplication o);







#endif /* _TT_HANDY_M4_H */
