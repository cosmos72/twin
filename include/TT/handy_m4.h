
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
ttuint	TTGetEventsInprogress_ttobj(ttobj o);  

/* handy ttevent methods */
        
ttcallback	TTGetCallback_ttevent(ttevent o);    
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
ttuint	TTGetLen_tteventbig(tteventbig o);    
TT_CONST ttbyte *	TTGetData_tteventbig(tteventbig o); 

/* handy ttlistener methods */
        
ttuint	TTGetLflags_ttlistener(ttlistener o);    
ttcomponent	TTGetComponent_ttlistener(ttlistener o);    
ttlistener	TTGetPrev_ttlistener(ttlistener o);    
ttlistener	TTGetNext_ttlistener(ttlistener o); 

/* handy ttcallback methods */
    
ttcallback	TTGetAvlLeft_ttcallback(ttcallback o);    
ttcallback	TTGetAvlRight_ttcallback(ttcallback o);    
ttcallback	TTGetAvlParent_ttcallback(ttcallback o);            
ttevent	TTGetEvent_ttcallback(ttcallback o);    
ttuint	TTGetNargComponent_ttcallback(ttcallback o);    
ttuint	TTGetNargEvent_ttcallback(ttcallback o);    
ttuint	TTGetNargs_ttcallback(ttcallback o);    
ttcallback_fn	TTGetFunction_ttcallback(ttcallback o);        
TT_CONST ttany *	TTGetArgs_ttcallback(ttcallback o); 

/* handy ttcomponent methods */
        
ttlistener	TTGetListeners_ttcomponent(ttcomponent o);    
ttcallback	TTGetCallbacks_ttcomponent(ttcomponent o); 

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

/* handy ttlabel methods */
        
ttshort	TTGetTextLen_ttlabel(ttlabel o);        
TT_CONST ttfont *	TTGetText_ttlabel(ttlabel o); 

/* handy ttanybutton methods */
            
ttshort	TTGetTextWidth_ttanybutton(ttanybutton o);    
ttshort	TTGetTextHeight_ttanybutton(ttanybutton o);        
TT_CONST ttfont *	TTGetText_ttanybutton(ttanybutton o); 

/* handy ttbutton methods */
     

/* handy ttcheckbutton methods */
     

/* handy ttradiobutton methods */
     

/* handy ttscrollbar methods */
     

/* handy ttbuttongroup methods */
     

/* handy ttwindow methods */
        
ttbyte *	TTGetTitle_ttwindow(ttwindow o);    
ttmenubar	TTGetMenubar_ttwindow(ttwindow o); 

/* handy ttframe methods */
     

/* handy ttscroller methods */
        
ttscrollbar	TTGetScrollx_ttscroller(ttscroller o);    
ttscrollbar	TTGetScrolly_ttscroller(ttscroller o); 

/* handy ttmenuitem methods */
         

/* handy ttcheckmenuitem methods */
     

/* handy ttradiomenuitem methods */
     

/* handy ttmenu methods */
    
    
ttmenubar	TTGetMenubar_ttmenu(ttmenu o); 

/* handy ttmenubar methods */
     

/* handy ttanytext methods */
        
ttshort	TTGetTextLen_ttanytext(ttanytext o);        
TT_CONST ttfont *	TTGetText_ttanytext(ttanytext o); 

/* handy tttextfield methods */
     

/* handy tttextarea methods */
     

/* handy tttheme methods */
    
            
        
TT_CONST ttbyte *	TTGetName_tttheme(tttheme o); 

/* handy ttapplication methods */
        
ttbyte *	TTGetName_ttapplication(ttapplication o);







#endif /* _TT_HANDY_M4_H */
