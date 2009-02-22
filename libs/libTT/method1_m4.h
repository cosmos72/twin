



/* This file was automatically generated from m4/method1.m4, do not edit! */

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
 *  method1_m4.h  --  implementation of TT*_ttmethod functions,
 *                    reflective ttmethods for all existing objects
 *
 */

/*  Copyright (C) 2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */
























/* prototypes (return type and parameter type arrays) of existing methods */

#define S(t) TT_CAT(str_order_,t)

static TT_CONST ttbyte * TT_CONST method_proto[] = {
    /* WARNING: we never return a ttclass... if prototype says so, we return a ttclass */

    /* generic methods */
    
   S(ttuint),
   S(void),
   S(ttobject) S(ttclass),
   S(void) S(ttobject),
   S(ttclass) S(ttobj),
   S(ttclass) S(ttobj),
   S(ttbyte) S(ttclass) S(ttobj),
   S(ttbyte) S(ttobj),

 
    /* ttobj methods */
                        
   S(ttbyte) S(ttobj) S(ttfield) S(ttarg),
   S(ttbyte) S(ttobj) S(ttfield) S(ttarg),
   S(ttbyte) S(ttobj) S(ttfield) S(ttany) S(ttany), 
    /* ttclass methods */
    
   S(ttbyte) S(ttclass),
   S(ttopaque) S(ttclass),
   S(ttclass) S(ttclass),
   S(ttbyte) S(ttclass) S(ttclass),
   S(ttclass) S(ttopaque),
   S(ttbyte) S(ttopaque),
   S(ttbyte) S(ttopaque), 
    /* ttobject methods */
                    
   S(void) S(ttobject),
   S(void) S(ttobject),
   S(ttbyte) S(ttobject) S(ttany), 
    /* ttvector methods */
        
   S(ttany) S(ttvector) S(ttopaque),    
   S(ttbyte) S(ttvector) S(ttopaque) S(ttany),
   S(ttvector) S(ttany),
   S(ttvector) S(ttopaque) S(ttarg),
   S(ttvector) S(ttvector),
   S(ttvector) S(ttopaque) S(va_list),
   S(ttvector) S(ttopaque) S(ttany),
   S(ttbyte) S(ttvector) S(ttany),
   S(ttbyte) S(ttvector) S(ttopaque) S(ttany),
   S(ttbyte) S(ttvector) S(ttopaque) S(ttopaque) S(ttarg),
   S(ttbyte) S(ttvector) S(ttopaque) S(ttvector),
   S(ttbyte) S(ttvector) S(ttopaque) S(ttopaque) S(va_list),    
   S(ttbyte) S(ttvector) S(ttopaque) S(ttopaque) S(ttany),    
   S(ttopaque) S(ttvector) S(ttopaque) S(ttany),
   S(ttbyte) S(ttvector) S(ttopaque),    
   S(ttbyte) S(ttvector) S(ttopaque) S(ttopaque), 
    /* ttfield methods */
    
   S(ttfield) S(ttbyte),
   S(ttbyte) S(ttfield) S(ttobject) S(ttarg),
   S(ttbyte) S(ttfield) S(ttobject) S(ttarg),
   S(ttbyte) S(ttfield) S(ttobject) S(ttany) S(ttany),
   S(ttopaque) S(ttfield),
   S(ttfield) S(ttopaque),
   S(ttopaque) S(ttbyte), 
    /* ttmethod methods */
    
   S(ttmethod) S(ttbyte),
   S(ttmethod) S(ttfunction_fn),
   S(ttopaque) S(ttmethod) S(ttopaque) S(ttopaque) S(ttarg),
   S(ttbyte) S(ttmethod) S(ttopaque) S(ttarg),
   S(ttbyte) S(ttmethod) S(ttarg) S(ttopaque) S(ttarg),
   S(ttbyte) S(ttmethod) S(ttarg) S(ttvector),
   S(ttbyte) S(ttmethod) S(ttarg) S(ttopaque) S(va_list),
   S(ttbyte) S(ttmethod) S(ttarg) S(ttopaque) S(ttany), 
    /* ttevent methods */
    
   S(ttevent) S(ttuint) S(ttuint) S(ttuint),
   S(void) S(ttevent) S(ttcomponent), 
    /* tteventbig methods */
    
   S(tteventbig) S(ttuint) S(ttuint) S(ttuint) S(ttshort) S(ttshort) S(ttshort) S(ttshort) S(ttany) S(ttany) S(ttopaque) S(ttbyte),
   S(tteventbig) S(ttuint) S(ttuint) S(ttuint) S(ttshort) S(ttshort) S(ttshort) S(ttshort),
   S(tteventbig) S(ttuint) S(ttuint) S(ttuint) S(ttany) S(ttany) S(ttopaque) S(ttbyte), 
    /* ttbitmask methods */
    
   S(ttbitmask) S(ttany),
   S(ttbitmask) S(ttbitmask),
   S(ttbitmask) S(ttvector),
   S(ttbitmask) S(ttvector),
   S(void) S(ttbitmask),
   S(void) S(ttbitmask),
   S(void) S(ttbitmask),
   S(void) S(ttbitmask) S(ttuint),
   S(ttbyte) S(ttbitmask) S(ttany),
   S(ttbyte) S(ttbitmask) S(ttany),
   S(ttbyte) S(ttbitmask) S(ttbitmask),
   S(ttbyte) S(ttbitmask) S(ttvector),
   S(ttbyte) S(ttbitmask) S(ttany),
   S(ttbyte) S(ttbitmask) S(ttbitmask),
   S(ttbyte) S(ttbitmask) S(ttvector),
   S(ttbyte) S(ttbitmask) S(ttany) S(ttuint),
   S(ttbyte) S(ttbitmask) S(ttvector) S(ttuint),
   S(ttbyte) S(ttbitmask) S(ttbitmask) S(ttuint), 
    /* tteventmask methods */
    
   S(tteventmask) S(ttbitmask) S(ttbitmask) S(ttbitmask),
   S(tteventmask) S(ttbitmask) S(ttbitmask) S(ttbitmask),
   S(ttbyte) S(tteventmask) S(ttbitmask),
   S(ttbyte) S(tteventmask) S(ttbitmask),
   S(ttbyte) S(tteventmask) S(ttbitmask),
   S(ttbyte) S(tteventmask) S(ttuint), 
    /* ttcallback methods */
         
    /* ttlistener methods */
    
   S(ttlistener) S(ttcomponent) S(ttuint) S(ttuint) S(ttfunction_fn) S(ttany),
   S(ttlistener) S(ttcomponent) S(ttevent) S(ttuint) S(ttfunction_fn) S(ttany),
   S(ttlistener) S(ttcomponent) S(ttevent) S(ttuint) S(ttfunction_fn) S(ttopaque) S(ttopaque) S(ttvector),
   S(ttlistener) S(ttcomponent) S(ttuint) S(ttfunction_fn),
   S(ttlistener) S(ttcomponent) S(ttuint) S(ttfunction_fn),
   S(ttlistener) S(ttcomponent) S(ttuint) S(ttuint) S(ttuint) S(ttfunction_fn),
   S(ttlistener) S(ttcomponent) S(ttuint) S(ttuint) S(ttuint) S(ttfunction_fn),
   S(ttlistener) S(ttcomponent) S(ttuint) S(ttfunction_fn),
   S(ttlistener) S(ttcomponent) S(ttuint) S(ttuint) S(ttfunction_fn),
   S(ttlistener) S(ttcomponent) S(ttbyte) S(ttuint) S(ttfunction_fn),
   S(ttlistener) S(ttcomponent) S(ttuint) S(ttfunction_fn),    
   S(void) S(ttlistener) S(ttcomponent),    
   S(void) S(ttlistener),
   S(ttbyte) S(ttlistener) S(ttvector),
   S(ttbyte) S(ttlistener) S(tteventmask), 
    /* tttimer methods */
    
   S(tttimer) S(ttcomponent) S(ttuint) S(ttfunction_fn) S(ttopaque) S(ttopaque) S(ttvector) S(ttany) S(ttany),
   S(tttimer) S(ttcomponent) S(ttuint) S(ttfunction_fn) S(ttany) S(ttany) S(ttany),
   S(void) S(tttimer) S(ttany) S(ttany),    
   S(void) S(tttimer) S(ttbyte),    
   S(ttbyte) S(tttimer), 
    /* ttcomponent methods */
    
   S(void) S(ttcomponent) S(ttlistener),
   S(void) S(ttcomponent) S(ttlistener),
   S(ttdata) S(ttcomponent) S(ttbyte) S(ttany),
   S(ttany) S(ttcomponent) S(ttbyte),
   S(void) S(ttcomponent) S(ttbyte),
   S(ttdata) S(ttcomponent) S(ttbyte), 
    /* ttdata methods */
            
   S(ttbyte) S(ttdata) S(ttany), 
    /* ttvisible methods */
        
   S(void) S(ttvisible) S(ttvisible) S(ttany),    
   S(void) S(ttvisible),    
   S(void) S(ttvisible),    
   S(void) S(ttvisible),    
   S(void) S(ttvisible) S(ttbyte),    
   S(ttbyte) S(ttvisible) S(tttheme),    
   S(void) S(ttvisible) S(ttshort) S(ttshort) S(ttshort) S(ttshort) S(ttshort) S(ttbyte) S(ttfont) S(ttattr),    
   S(void) S(ttvisible) S(ttshort) S(ttshort) S(ttshort) S(ttshort),

   S(ttbyte) S(ttvisible) S(ttfunction_fn),
   S(void) S(ttvisible) S(ttshort) S(ttshort) S(ttshort) S(ttshort),
   S(void) S(ttvisible) S(ttvisible) S(ttany), 
    /* ttlayout methods */
             
    /* ttboxlayout methods */
    
   S(ttboxlayout) S(ttany), 
    /* ttborderlayout methods */
     
    /* ttnative methods */
        
   S(ttnative),    
   S(ttshort) S(ttnative),    
   S(ttshort) S(ttnative),
   S(ttnative) S(ttany), 
    /* ttwidget methods */
        
   S(void) S(ttwidget) S(ttlayout),    
   S(ttbyte) S(ttwidget) S(ttshort) S(ttshort),    
   S(ttbyte) S(ttwidget) S(ttshort) S(ttshort),    
   S(ttbyte) S(ttwidget) S(ttint),    
   S(ttbyte) S(ttwidget) S(ttint),    
   S(ttbyte) S(ttwidget) S(ttint),    
   S(ttbyte) S(ttwidget) S(ttint),    
   S(ttbyte) S(ttwidget) S(tttooltip),
   S(ttbyte) S(ttwidget) S(ttuint) S(ttint) S(ttint),
   S(ttbyte) S(ttwidget) S(ttuint) S(ttint) S(ttint),
   S(ttbyte) S(ttwidget) S(ttuint) S(ttshort) S(ttshort) S(ttshort) S(ttshort),
   S(ttbyte) S(ttwidget) S(ttshort),
   S(ttbyte) S(ttwidget) S(ttshort),
   S(ttbyte) S(ttwidget) S(ttshort),
   S(ttbyte) S(ttwidget) S(ttshort), 
    /* ttlabel methods */
        
   S(void) S(ttlabel) S(ttopaque) S(ttfont),
   S(ttbyte) S(ttlabel) S(ttfont),
   S(ttbyte) S(ttlabel) S(ttbyte),
   S(ttlabel) S(ttbyte), 
    /* tttooltip methods */
        
   S(ttbyte) S(tttooltip) S(ttwidget), 
    /* ttbuttongroup methods */
        
   S(void) S(ttbuttongroup) S(ttradiobutton),    
   S(void) S(ttbuttongroup) S(ttradiobutton),    
   S(void) S(ttbuttongroup) S(ttradiobutton), 
    /* ttanybutton methods */
            
   S(void) S(ttanybutton) S(ttshort) S(ttshort) S(ttshort) S(ttattr),
   S(ttbyte) S(ttanybutton) S(ttshort) S(ttshort) S(ttshort) S(ttattr),
   S(ttbyte) S(ttanybutton) S(ttshort) S(ttshort) S(ttshort) S(ttfont),
   S(ttbyte) S(ttanybutton) S(ttshort) S(ttshort) S(ttshort) S(ttbyte),
   S(ttanybutton) S(ttshort) S(ttshort) S(ttshort) S(ttbyte), 
    /* ttbutton methods */
        
   S(void) S(ttbutton) S(ttbyte),
   S(ttbyte) S(ttbutton),
   S(ttbutton) S(ttshort) S(ttshort) S(ttshort) S(ttbyte), 
    /* ttcheckbutton methods */
        
   S(void) S(ttcheckbutton) S(ttbyte),
   S(ttbyte) S(ttcheckbutton),
   S(ttcheckbutton) S(ttshort) S(ttshort) S(ttshort) S(ttbyte), 
    /* ttradiobutton methods */
    
   S(void) S(ttradiobutton) S(ttbuttongroup),
   S(void) S(ttradiobutton),
   S(ttradiobutton) S(ttshort) S(ttshort) S(ttshort) S(ttbyte), 
    /* ttanyscroll methods */
                
   S(ttbyte) S(ttanyscroll) S(ttbyte),    
   S(ttbyte) S(ttanyscroll) S(ttint),    
   S(ttbyte) S(ttanyscroll) S(ttint),    
   S(ttbyte) S(ttanyscroll) S(ttint),    
   S(ttbyte) S(ttanyscroll) S(ttint), 
    /* ttscrollbar methods */
    
   S(ttscrollbar) S(ttbyte),
   S(ttscrollbar) S(ttbyte) S(ttint) S(ttint) S(ttint), 
    /* ttslider methods */
        
   S(ttbyte) S(ttslider) S(ttint),    
   S(ttbyte) S(ttslider) S(ttint),    
   S(ttbyte) S(ttslider) S(ttint),
   S(ttslider) S(ttbyte) S(ttint), 
    /* ttprogressbar methods */
    
   S(ttprogressbar) S(ttbyte), 
    /* ttscrollpane methods */
    
   S(ttscrollpane) S(ttshort) S(ttshort),    
   S(ttbyte) S(ttscrollpane) S(ttscrollbar),    
   S(ttbyte) S(ttscrollpane) S(ttscrollbar), 
    /* ttwindow methods */
        
   S(ttbyte) S(ttwindow) S(ttbyte), 
    /* ttframe methods */
        
   S(ttbyte) S(ttframe) S(ttmenubar),
   S(ttframe) S(ttbyte), 
    /* ttscroller methods */
    
   S(ttscroller) S(ttshort) S(ttshort), 
    /* ttmenuitem methods */
     
    /* ttcheckmenuitem methods */
     
    /* ttradiomenuitem methods */
     
    /* ttmenu methods */
     
    /* ttmenubar methods */
    
   S(ttmenubar), 
    /* ttanytext methods */
     
    /* tttextfield methods */
     
    /* tttextarea methods */
     
    /* tttheme methods */
    
   S(tttheme), 
    /* ttapplication methods */
    
   S(ttapplication) S(ttbyte),
   S(ttapplication),


    /* TTSet*_* methods omitted, they are listed above! */
#if 0
        
#endif
    /* TTGet*_* methods */




 
    /* TTGet*_ttobj methods */         
    /* TTGet*_ttclass methods */             
    /* TTGet*_ttobject methods */    
   S(ttopaque) S(ttobject),    
   S(ttuint) S(ttobject),            
   S(ttany) S(ttobject),    
   S(ttopaque) S(ttobject),    
   S(ttopaque) S(ttobject), 
    /* TTGet*_ttvector methods */    
   S(ttopaque) S(ttvector),    
   S(ttopaque) S(ttvector),    
   S(ttany) S(ttvector), 
    /* TTGet*_ttfield methods */    
   S(ttopaque) S(ttfield),    
   S(ttbyte) S(ttfield),    
   S(ttopaque) S(ttfield), 
    /* TTGet*_ttmethod methods */    
   S(ttopaque) S(ttmethod),    
   S(ttbyte) S(ttmethod),    
   S(ttfunction_fn) S(ttmethod),    
   S(ttopaque) S(ttmethod),    
   S(ttvector) S(ttmethod), 
    /* TTGet*_ttevent methods */    
   S(ttcomponent) S(ttevent),    
   S(ttuint) S(ttevent),    
   S(ttuint) S(ttevent),    
   S(ttuint) S(ttevent),    
   S(ttevent) S(ttevent),    
   S(ttevent) S(ttevent), 
    /* TTGet*_tteventbig methods */    
   S(ttshort) S(tteventbig),    
   S(ttshort) S(tteventbig),    
   S(ttshort) S(tteventbig),    
   S(ttshort) S(tteventbig),    
   S(ttany) S(tteventbig),    
   S(ttany) S(tteventbig),    
   S(ttopaque) S(tteventbig),    
   S(ttbyte) S(tteventbig), 
    /* TTGet*_ttbitmask methods */                 
    /* TTGet*_tteventmask methods */    
   S(ttbitmask) S(tteventmask),    
   S(ttbitmask) S(tteventmask),    
   S(ttbitmask) S(tteventmask),    
   S(ttuint) S(tteventmask), 
    /* TTGet*_ttcallback methods */    
   S(ttuint) S(ttcallback),    
   S(ttcomponent) S(ttcallback),    
   S(ttcallback) S(ttcallback),    
   S(ttcallback) S(ttcallback), 
    /* TTGet*_ttlistener methods */        
   S(ttevent) S(ttlistener),    
   S(ttfunction_fn) S(ttlistener),    
   S(ttopaque) S(ttlistener),    
   S(ttopaque) S(ttlistener),    
   S(ttvector) S(ttlistener),    
   S(tteventmask) S(ttlistener),         
    /* TTGet*_tttimer methods */    
   S(ttany) S(tttimer),    
   S(ttany) S(tttimer),    
   S(tttimer) S(tttimer),    
   S(tttimer) S(tttimer), 
    /* TTGet*_ttcomponent methods */    
   S(ttcallback) S(ttcomponent),    
   S(ttlistener) S(ttcomponent),    
   S(tttimer) S(ttcomponent),    
   S(ttdata) S(ttcomponent), 
    /* TTGet*_ttdata methods */        
   S(ttcomponent) S(ttdata),    
   S(ttopaque) S(ttdata),    
   S(ttbyte) S(ttdata),    
   S(ttany) S(ttdata), 
    /* TTGet*_ttvisible methods */    
   S(ttuint) S(ttvisible),    
   S(ttvisible) S(ttvisible),    
   S(ttvisible) S(ttvisible),    
   S(ttvisible) S(ttvisible),    
   S(ttvisible) S(ttvisible),    
   S(ttvisible) S(ttvisible),    
   S(ttany) S(ttvisible),    
   S(tttheme) S(ttvisible),    
   S(ttfunction_fn) S(ttvisible), 
    /* TTGet*_ttlayout methods */    
   S(ttwidget) S(ttlayout), 
    /* TTGet*_ttboxlayout methods */    
   S(ttuint) S(ttboxlayout), 
    /* TTGet*_ttborderlayout methods */ 
    /* TTGet*_ttnative methods */ 
    /* TTGet*_ttwidget methods */    
   S(ttshort) S(ttwidget),    
   S(ttshort) S(ttwidget),    
   S(ttshort) S(ttwidget),    
   S(ttshort) S(ttwidget),    
   S(ttlayout) S(ttwidget),    
   S(ttint) S(ttwidget),    
   S(ttint) S(ttwidget),    
   S(ttint) S(ttwidget),    
   S(ttint) S(ttwidget),    
   S(ttcol) S(ttwidget),    
   S(tttooltip) S(ttwidget), 
    /* TTGet*_ttlabel methods */    
   S(ttopaque) S(ttlabel),    
   S(ttfont) S(ttlabel), 
    /* TTGet*_tttooltip methods */        
   S(ttwidget) S(tttooltip), 
    /* TTGet*_ttbuttongroup methods */    
   S(ttradiobutton) S(ttbuttongroup),    
   S(ttradiobutton) S(ttbuttongroup),    
   S(ttradiobutton) S(ttbuttongroup), 
    /* TTGet*_ttanybutton methods */    
   S(ttshort) S(ttanybutton),    
   S(ttshort) S(ttanybutton),    
   S(ttattr) S(ttanybutton),     
    /* TTGet*_ttbutton methods */ 
    /* TTGet*_ttcheckbutton methods */ 
    /* TTGet*_ttradiobutton methods */    
   S(ttbuttongroup) S(ttradiobutton),    
   S(ttradiobutton) S(ttradiobutton),    
   S(ttradiobutton) S(ttradiobutton), 
    /* TTGet*_ttanyscroll methods */    
   S(ttbyte) S(ttanyscroll),    
   S(ttint) S(ttanyscroll),    
   S(ttint) S(ttanyscroll),    
   S(ttint) S(ttanyscroll),        
   S(ttint) S(ttanyscroll),    
   S(ttint) S(ttanyscroll),    
   S(ttuint) S(ttanyscroll), 
    /* TTGet*_ttscrollbar methods */     
    /* TTGet*_ttslider methods */    
   S(ttint) S(ttslider),    
   S(ttint) S(ttslider),    
   S(ttint) S(ttslider), 
    /* TTGet*_ttprogressbar methods */         
    /* TTGet*_ttscrollpane methods */    
   S(ttscrollbar) S(ttscrollpane),    
   S(ttscrollbar) S(ttscrollpane), 
    /* TTGet*_ttwindow methods */    
   S(ttopaque) S(ttwindow),    
   S(ttbyte) S(ttwindow), 
    /* TTGet*_ttframe methods */    
   S(ttmenubar) S(ttframe), 
    /* TTGet*_ttscroller methods */    
   S(ttscrollpane) S(ttscroller), 
    /* TTGet*_ttmenuitem methods */    
   S(ttopaque) S(ttmenuitem),    
   S(ttbyte) S(ttmenuitem), 
    /* TTGet*_ttcheckmenuitem methods */ 
    /* TTGet*_ttradiomenuitem methods */ 
    /* TTGet*_ttmenu methods */    
   S(ttmenubar) S(ttmenu), 
    /* TTGet*_ttmenubar methods */ 
    /* TTGet*_ttanytext methods */    
   S(ttopaque) S(ttanytext),    
   S(ttattr) S(ttanytext), 
    /* TTGet*_tttextfield methods */ 
    /* TTGet*_tttextarea methods */ 
    /* TTGet*_tttheme methods */    
   S(ttopaque) S(tttheme),    
   S(ttbyte) S(tttheme),                 
    /* TTGet*_ttapplication methods */    
   S(ttopaque) S(ttapplication),    
   S(ttbyte) S(ttapplication),




};

#undef S


#define M(m) TT_CAT(ttmethod_,m)


/* attributes of array parameters of existing methods */




static TT_CONST ttany method_type[][3] = {

    /* generic methods */
    
    { M(ClassNameOf), 0, /*TT_ARG_READ TT_ARG_ARRAY_Z*/ 0|e_R |e_Z, },

 
    /* ttobj methods */
                         
    /* ttclass methods */
    
    { M(GetName_ttclass), 0, /*TT_ARG_READ TT_ARG_ARRAY_Z*/ 0|e_R |e_Z, }, 
    /* ttobject methods */
                     
    /* ttvector methods */
                         
    /* ttfield methods */
    
    { M(Get_ttfield), 1, /*TT_ARG_READ TT_ARG_ARRAY_Z*/ 0|e_R |e_Z, }, 
    /* ttmethod methods */
    
    { M(Get_ttmethod), 1, /*TT_ARG_READ TT_ARG_ARRAY_Z*/ 0|e_R |e_Z, }, 
    /* ttevent methods */
     
    /* tteventbig methods */
     
    /* ttbitmask methods */
     
    /* tteventmask methods */
     
    /* ttcallback methods */
         
    /* ttlistener methods */
             
    /* tttimer methods */
             
    /* ttcomponent methods */
     
    /* ttdata methods */
             
    /* ttvisible methods */
                                    
 
    /* ttlayout methods */
             
    /* ttboxlayout methods */
     
    /* ttborderlayout methods */
     
    /* ttnative methods */
                 
    /* ttwidget methods */
                                     
    /* ttlabel methods */
         
    /* tttooltip methods */
         
    /* ttbuttongroup methods */
                 
    /* ttanybutton methods */
             
    /* ttbutton methods */
         
    /* ttcheckbutton methods */
         
    /* ttradiobutton methods */
     
    /* ttanyscroll methods */
                                 
    /* ttscrollbar methods */
     
    /* ttslider methods */
                 
    /* ttprogressbar methods */
     
    /* ttscrollpane methods */
             
    /* ttwindow methods */
         
    /* ttframe methods */
         
    /* ttscroller methods */
     
    /* ttmenuitem methods */
     
    /* ttcheckmenuitem methods */
     
    /* ttradiomenuitem methods */
     
    /* ttmenu methods */
     
    /* ttmenubar methods */
     
    /* ttanytext methods */
     
    /* tttextfield methods */
     
    /* tttextarea methods */
     
    /* tttheme methods */
     
    /* ttapplication methods */
    


    /* TTSet*_* methods omitted, they are listed above! */
#if 0
        
#endif
    /* TTGet*_* methods */




 
    /* TTGet*_ttobj methods */         
    /* TTGet*_ttclass methods */             
    /* TTGet*_ttobject methods */                             
    /* TTGet*_ttvector methods */            
    { M(GetArray_ttvector), 0, /*TT_ARG_ARRAY((_F(array_n)))*/ 0|e_A(e_F(ttvector, array_n)|e_OP(e_SET)), }, 
    /* TTGet*_ttfield methods */        
    { M(GetName_ttfield), 0, /* TT_ARG_ARRAY_Z*/ 0|e_Z, },     
    /* TTGet*_ttmethod methods */        
    { M(GetName_ttmethod), 0, /* TT_ARG_ARRAY((_F(name_len)))*/ 0|e_A(e_F(ttmethod, name_len)|e_OP(e_SET)), },             
    /* TTGet*_ttevent methods */                         
    /* TTGet*_tteventbig methods */                                
    { M(GetData_tteventbig), 0, /* TT_ARG_ARRAY((_F(data_len)))*/ 0|e_A(e_F(tteventbig, data_len)|e_OP(e_SET)), }, 
    /* TTGet*_ttbitmask methods */                 
    /* TTGet*_tteventmask methods */                 
    /* TTGet*_ttcallback methods */                 
    /* TTGet*_ttlistener methods */                                     
    /* TTGet*_tttimer methods */                 
    /* TTGet*_ttcomponent methods */                 
    /* TTGet*_ttdata methods */                
    { M(GetKey_ttdata), 0, /*TT_ARG_ARRAY((_F(key_len)))*/ 0|e_A(e_F(ttdata, key_len)|e_OP(e_SET)), },     
    /* TTGet*_ttvisible methods */                                     
    /* TTGet*_ttlayout methods */     
    /* TTGet*_ttboxlayout methods */     
    /* TTGet*_ttborderlayout methods */ 
    /* TTGet*_ttnative methods */ 
    /* TTGet*_ttwidget methods */                                             
    /* TTGet*_ttlabel methods */        
    { M(GetText_ttlabel), 0, /*TT_ARG_ARRAY((_F(text_len)))*/ 0|e_A(e_F(ttlabel, text_len)|e_OP(e_SET)), }, 
    /* TTGet*_tttooltip methods */         
    /* TTGet*_ttbuttongroup methods */             
    /* TTGet*_ttanybutton methods */            
    { M(GetText_ttanybutton), 0, /*TT_ARG_ARRAY((_F(text_width),*,_F(text_height)))*/ 0|e_A(e_F(ttanybutton, text_width)|(e_F(ttanybutton, text_height))<<e_WFP|e_OP(e_MUL)), },     
    /* TTGet*_ttbutton methods */ 
    /* TTGet*_ttcheckbutton methods */ 
    /* TTGet*_ttradiobutton methods */             
    /* TTGet*_ttanyscroll methods */                                 
    /* TTGet*_ttscrollbar methods */     
    /* TTGet*_ttslider methods */             
    /* TTGet*_ttprogressbar methods */         
    /* TTGet*_ttscrollpane methods */         
    /* TTGet*_ttwindow methods */        
    { M(GetTitle_ttwindow), 0, /*TT_ARG_ARRAY((_F(title_len)))*/ 0|e_A(e_F(ttwindow, title_len)|e_OP(e_SET)), }, 
    /* TTGet*_ttframe methods */     
    /* TTGet*_ttscroller methods */     
    /* TTGet*_ttmenuitem methods */        
    { M(GetName_ttmenuitem), 0, /*TT_ARG_ARRAY((_F(name_len)))*/ 0|e_A(e_F(ttmenuitem, name_len)|e_OP(e_SET)), }, 
    /* TTGet*_ttcheckmenuitem methods */ 
    /* TTGet*_ttradiomenuitem methods */ 
    /* TTGet*_ttmenu methods */     
    /* TTGet*_ttmenubar methods */ 
    /* TTGet*_ttanytext methods */        
    { M(GetText_ttanytext), 0, /*TT_ARG_ARRAY((_F(text_len)))*/ 0|e_A(e_F(ttanytext, text_len)|e_OP(e_SET)), }, 
    /* TTGet*_tttextfield methods */ 
    /* TTGet*_tttextarea methods */ 
    /* TTGet*_tttheme methods */        
    { M(GetName_tttheme), 0, /*TT_ARG_ARRAY((_F(name_len)))*/ 0|e_A(e_F(tttheme, name_len)|e_OP(e_SET)), },                 
    /* TTGet*_ttapplication methods */        
    { M(GetName_ttapplication), 0, /*TT_ARG_ARRAY((_F(name_len)))*/ 0|e_A(e_F(ttapplication, name_len)|e_OP(e_SET)), },




};

#define method_type_n (sizeof(method_type)/sizeof(method_type[0]))




#define method_return_type_n (sizeof(method_return_type)/sizeof(method_return_type[0]))



#undef M




/* names of existing methods */


static TT_CONST ttbyte * TT_CONST method_name[] = {

    /* generic methods */
    
    "TTLibraryVersion",
    "TTExitMainLoop",
    "TTNew",
    "TTDel",
    "TTClassOf",
    "TTSuperClassOf",
    "TTInstanceOf",
    "TTClassNameOf",

 
    /* ttobj methods */
                        
    "TTGetField_ttobj",
    "TTSetField_ttobj",
    "TTChangeField_ttobj", 
    /* ttclass methods */
    
    "TTGetName_ttclass",
    "TTGetSize_ttclass",
    "TTGetSuper_ttclass",
    "TTIsInstance_ttclass",
    "TTFromType_ttclass",
    "TTIsArrayType_ttclass",
    "TTIsPrimitiveType_ttclass", 
    /* ttobject methods */
                    
    "TTRef_ttobject",
    "TTUnref_ttobject",
    "TTSetUserData_ttobject", 
    /* ttvector methods */
        
    "TTGetAt_ttvector",    
    "TTSetAt_ttvector",
    "TTCreate1_ttvector",
    "TTCreateA_ttvector",
    "TTCreateR_ttvector",
    "TTCreateV_ttvector",
    "TTCreateY_ttvector",
    "TTAppend_ttvector",
    "TTAdd_ttvector",
    "TTAddA_ttvector",
    "TTAddR_ttvector",
    "TTAddV_ttvector",    
    "TTAddY_ttvector",    
    "TTContainsValue_ttvector",
    "TTRemoveAt_ttvector",    
    "TTRemoveRange_ttvector", 
    /* ttfield methods */
    
    "TTGet_ttfield",
    "TTGetValue_ttfield",
    "TTSetValue_ttfield",
    "TTChangeValue_ttfield",
    "TTToEvcode_ttfield",
    "TTFromEvcode_ttfield",
    "TTGetEvcode_ttfield", 
    /* ttmethod methods */
    
    "TTGet_ttmethod",
    "TTGetByAddress_ttmethod",
    "TTGetArraySizeA_ttmethod",
    "TTCheckArraySizesA_ttmethod",
    "TTCallA_ttmethod",
    "TTCallR_ttmethod",
    "TTCallV_ttmethod",
    "TTCallY_ttmethod", 
    /* ttevent methods */
    
    "TTCreate_ttevent",
    "TTFire_ttevent", 
    /* tteventbig methods */
    
    "TTCreate8_tteventbig",
    "TTCreate4_tteventbig",
    "TTCreate4s4_tteventbig", 
    /* ttbitmask methods */
    
    "TTCreate_ttbitmask",
    "TTCreateB2_ttbitmask",
    "TTCreateR_ttbitmask",
    "TTCreateR2_ttbitmask",
    "TTClear_ttbitmask",
    "TTFill_ttbitmask",
    "TTNegate_ttbitmask",
    "TTChange_ttbitmask",
    "TTContains_ttbitmask",
    "TTAdd_ttbitmask",
    "TTAddB_ttbitmask",
    "TTAddR_ttbitmask",
    "TTRemove_ttbitmask",
    "TTRemoveB_ttbitmask",
    "TTRemoveR_ttbitmask",
    "TTCombine_ttbitmask",
    "TTCombineR_ttbitmask",
    "TTCombineB_ttbitmask", 
    /* tteventmask methods */
    
    "TTCreateB_tteventmask",
    "TTCreateB2_tteventmask",
    "TTSetEvtypeMask_tteventmask",
    "TTSetEvcodeMask_tteventmask",
    "TTSetComponentMask_tteventmask",
    "TTSetTruthTable_tteventmask", 
    /* ttcallback methods */
         
    /* ttlistener methods */
    
    "TTCreate_ttlistener",
    "TTCreateE_ttlistener",
    "TTCreateR_ttlistener",
    "TTCreateDel_ttlistener",
    "TTCreateAskclose_ttlistener",
    "TTCreateKey_ttlistener",
    "TTCreateMouse_ttlistener",
    "TTCreateActivate_ttlistener",
    "TTCreateChange_ttlistener",
    "TTCreateKeyData_ttlistener",
    "TTCreateExpose_ttlistener",    
    "TTAddTo_ttlistener",    
    "TTRemove_ttlistener",
    "TTSetArgsR_ttlistener",
    "TTSetEventMask_ttlistener", 
    /* tttimer methods */
    
    "TTCreateR_tttimer",
    "TTCreate_tttimer",
    "TTSetDelay_tttimer",    
    "TTSetEnabled_tttimer",    
    "TTIsEnabled_tttimer", 
    /* ttcomponent methods */
    
    "TTAddListener_ttcomponent",
    "TTRemoveListener_ttcomponent",
    "TTSetKeyData_ttcomponent",
    "TTGetKeyData_ttcomponent",
    "TTUnsetKeyData_ttcomponent",
    "TTGetData_ttcomponent", 
    /* ttdata methods */
            
    "TTSetData_ttdata", 
    /* ttvisible methods */
        
    "TTAddTo_ttvisible",    
    "TTValidate_ttvisible",    
    "TTRemove_ttvisible",    
    "TTInvalidate_ttvisible",    
    "TTSetVisible_ttvisible",    
    "TTSetTheme_ttvisible",    
    "TTDraw_ttvisible",    
    "TTBuiltinRepaint_ttvisible",

    "TTSetRepaint_ttvisible",
    "TTExpose_ttvisible",
    "TTAdd_ttvisible", 
    /* ttlayout methods */
             
    /* ttboxlayout methods */
    
    "TTCreate_ttboxlayout", 
    /* ttborderlayout methods */
     
    /* ttnative methods */
        
    "TTGetRoot_ttnative",    
    "TTGetW_ttnative",    
    "TTGetH_ttnative",
    "TTCreate_ttnative", 
    /* ttwidget methods */
        
    "TTSetLayout_ttwidget",    
    "TTSetXY_ttwidget",    
    "TTSetWH_ttwidget",    
    "TTSetXl_ttwidget",    
    "TTSetYl_ttwidget",    
    "TTSetWl_ttwidget",    
    "TTSetHl_ttwidget",    
    "TTSetTooltip_ttwidget",
    "TTSetXlYl_ttwidget",
    "TTSetWlHl_ttwidget",
    "TTSetXYWH_ttwidget",
    "TTSetX_ttwidget",
    "TTSetY_ttwidget",
    "TTSetW_ttwidget",
    "TTSetH_ttwidget", 
    /* ttlabel methods */
        
    "TTSetFontD_ttlabel",
    "TTSetFont_ttlabel",
    "TTSetText_ttlabel",
    "TTCreate_ttlabel", 
    /* tttooltip methods */
        
    "TTSetWidget_tttooltip", 
    /* ttbuttongroup methods */
        
    "TTAdd_ttbuttongroup",    
    "TTRemove_ttbuttongroup",    
    "TTSetChecked_ttbuttongroup", 
    /* ttanybutton methods */
            
    "TTSetAttrD_ttanybutton",
    "TTSetAttr_ttanybutton",
    "TTSetFont_ttanybutton",
    "TTSetText_ttanybutton",
    "TTCreate_ttanybutton", 
    /* ttbutton methods */
        
    "TTSetPressed_ttbutton",
    "TTIsPressed_ttbutton",
    "TTCreate_ttbutton", 
    /* ttcheckbutton methods */
        
    "TTSetChecked_ttcheckbutton",
    "TTIsChecked_ttcheckbutton",
    "TTCreate_ttcheckbutton", 
    /* ttradiobutton methods */
    
    "TTAddToGroup_ttradiobutton",
    "TTRemoveFromGroup_ttradiobutton",
    "TTCreate_ttradiobutton", 
    /* ttanyscroll methods */
                
    "TTSetOrientation_ttanyscroll",    
    "TTSetSize_ttanyscroll",    
    "TTSetRealSize_ttanyscroll",    
    "TTSetViewSize_ttanyscroll",    
    "TTSetPosition_ttanyscroll", 
    /* ttscrollbar methods */
    
    "TTCreate1_ttscrollbar",
    "TTCreate4_ttscrollbar", 
    /* ttslider methods */
        
    "TTSetSlideMin_ttslider",    
    "TTSetSlideMax_ttslider",    
    "TTSetSlideValue_ttslider",
    "TTCreate_ttslider", 
    /* ttprogressbar methods */
    
    "TTCreate_ttprogressbar", 
    /* ttscrollpane methods */
    
    "TTCreate_ttscrollpane",    
    "TTSetBarX_ttscrollpane",    
    "TTSetBarY_ttscrollpane", 
    /* ttwindow methods */
        
    "TTSetTitle_ttwindow", 
    /* ttframe methods */
        
    "TTSetMenubar_ttframe",
    "TTCreate_ttframe", 
    /* ttscroller methods */
    
    "TTCreate_ttscroller", 
    /* ttmenuitem methods */
     
    /* ttcheckmenuitem methods */
     
    /* ttradiomenuitem methods */
     
    /* ttmenu methods */
     
    /* ttmenubar methods */
    
    "TTGetDefault_ttmenubar", 
    /* ttanytext methods */
     
    /* tttextfield methods */
     
    /* tttextarea methods */
     
    /* tttheme methods */
    
    "TTGetDefault_tttheme", 
    /* ttapplication methods */
    
    "TTCreate_ttapplication",
    "TTGet_ttapplication",


    /* TTSet*_* methods omitted, they are listed above! */
#if 0
        
#endif
    /* TTGet*_* methods */




 
    /* TTGet*_ttobj methods */         
    /* TTGet*_ttclass methods */             
    /* TTGet*_ttobject methods */    
    "TTGetRefcount_ttobject",    
    "TTGetOflags_ttobject",            
    "TTGetUserData_ttobject",    
    "TTGetEventsInprogress_ttobject",    
    "TTGetEventsInformed_ttobject", 
    /* TTGet*_ttvector methods */    
    "TTGetArraySize_ttvector",    
    "TTGetArrayN_ttvector",    
    "TTGetArray_ttvector", 
    /* TTGet*_ttfield methods */    
    "TTGetNameLen_ttfield",    
    "TTGetName_ttfield",    
    "TTGetType_ttfield", 
    /* TTGet*_ttmethod methods */    
    "TTGetNameLen_ttmethod",    
    "TTGetName_ttmethod",    
    "TTGetAddress_ttmethod",    
    "TTGetReturnType_ttmethod",    
    "TTGetParameterType_ttmethod", 
    /* TTGet*_ttevent methods */    
    "TTGetComponent_ttevent",    
    "TTGetEvtype_ttevent",    
    "TTGetEvcode_ttevent",    
    "TTGetEvflags_ttevent",    
    "TTGetPrev_ttevent",    
    "TTGetNext_ttevent", 
    /* TTGet*_tteventbig methods */    
    "TTGetX_tteventbig",    
    "TTGetY_tteventbig",    
    "TTGetW_tteventbig",    
    "TTGetH_tteventbig",    
    "TTGetValue_tteventbig",    
    "TTGetOldValue_tteventbig",    
    "TTGetDataLen_tteventbig",    
    "TTGetData_tteventbig", 
    /* TTGet*_ttbitmask methods */                 
    /* TTGet*_tteventmask methods */    
    "TTGetEvtypeMask_tteventmask",    
    "TTGetEvcodeMask_tteventmask",    
    "TTGetComponentMask_tteventmask",    
    "TTGetTruthTable_tteventmask", 
    /* TTGet*_ttcallback methods */    
    "TTGetLflags_ttcallback",    
    "TTGetComponent_ttcallback",    
    "TTGetPrev_ttcallback",    
    "TTGetNext_ttcallback", 
    /* TTGet*_ttlistener methods */        
    "TTGetEvent_ttlistener",    
    "TTGetFunction_ttlistener",    
    "TTGetArgComponentN_ttlistener",    
    "TTGetArgEventN_ttlistener",    
    "TTGetArgs_ttlistener",    
    "TTGetEventMask_ttlistener",         
    /* TTGet*_tttimer methods */    
    "TTGetDelayT_tttimer",    
    "TTGetDelayF_tttimer",    
    "TTGetTimerPrev_tttimer",    
    "TTGetTimerNext_tttimer", 
    /* TTGet*_ttcomponent methods */    
    "TTGetCallbacks_ttcomponent",    
    "TTGetListeners_ttcomponent",    
    "TTGetTimers_ttcomponent",    
    "TTGetDatas_ttcomponent", 
    /* TTGet*_ttdata methods */        
    "TTGetComponent_ttdata",    
    "TTGetKeyLen_ttdata",    
    "TTGetKey_ttdata",    
    "TTGetData_ttdata", 
    /* TTGet*_ttvisible methods */    
    "TTGetVflags_ttvisible",    
    "TTGetPrev_ttvisible",    
    "TTGetNext_ttvisible",    
    "TTGetParent_ttvisible",    
    "TTGetChildFirst_ttvisible",    
    "TTGetChildLast_ttvisible",    
    "TTGetConstraint_ttvisible",    
    "TTGetTheme_ttvisible",    
    "TTGetRepaint_ttvisible", 
    /* TTGet*_ttlayout methods */    
    "TTGetWidget_ttlayout", 
    /* TTGet*_ttboxlayout methods */    
    "TTGetOrientation_ttboxlayout", 
    /* TTGet*_ttborderlayout methods */ 
    /* TTGet*_ttnative methods */ 
    /* TTGet*_ttwidget methods */    
    "TTGetX_ttwidget",    
    "TTGetY_ttwidget",    
    "TTGetW_ttwidget",    
    "TTGetH_ttwidget",    
    "TTGetLayout_ttwidget",    
    "TTGetXl_ttwidget",    
    "TTGetYl_ttwidget",    
    "TTGetWl_ttwidget",    
    "TTGetHl_ttwidget",    
    "TTGetCol_ttwidget",    
    "TTGetTooltip_ttwidget", 
    /* TTGet*_ttlabel methods */    
    "TTGetTextLen_ttlabel",    
    "TTGetText_ttlabel", 
    /* TTGet*_tttooltip methods */        
    "TTGetWidget_tttooltip", 
    /* TTGet*_ttbuttongroup methods */    
    "TTGetGroupFirst_ttbuttongroup",    
    "TTGetGroupLast_ttbuttongroup",    
    "TTGetChecked_ttbuttongroup", 
    /* TTGet*_ttanybutton methods */    
    "TTGetTextWidth_ttanybutton",    
    "TTGetTextHeight_ttanybutton",    
    "TTGetText_ttanybutton",     
    /* TTGet*_ttbutton methods */ 
    /* TTGet*_ttcheckbutton methods */ 
    /* TTGet*_ttradiobutton methods */    
    "TTGetGroup_ttradiobutton",    
    "TTGetGroupPrev_ttradiobutton",    
    "TTGetGroupNext_ttradiobutton", 
    /* TTGet*_ttanyscroll methods */    
    "TTGetOrientation_ttanyscroll",    
    "TTGetSize_ttanyscroll",    
    "TTGetRealSize_ttanyscroll",    
    "TTGetViewSize_ttanyscroll",        
    "TTGetPosition_ttanyscroll",    
    "TTGetRealPosition_ttanyscroll",    
    "TTGetState_ttanyscroll", 
    /* TTGet*_ttscrollbar methods */     
    /* TTGet*_ttslider methods */    
    "TTGetSlideMin_ttslider",    
    "TTGetSlideMax_ttslider",    
    "TTGetSlideValue_ttslider", 
    /* TTGet*_ttprogressbar methods */         
    /* TTGet*_ttscrollpane methods */    
    "TTGetBarX_ttscrollpane",    
    "TTGetBarY_ttscrollpane", 
    /* TTGet*_ttwindow methods */    
    "TTGetTitleLen_ttwindow",    
    "TTGetTitle_ttwindow", 
    /* TTGet*_ttframe methods */    
    "TTGetMenubar_ttframe", 
    /* TTGet*_ttscroller methods */    
    "TTGetScrollpane_ttscroller", 
    /* TTGet*_ttmenuitem methods */    
    "TTGetNameLen_ttmenuitem",    
    "TTGetName_ttmenuitem", 
    /* TTGet*_ttcheckmenuitem methods */ 
    /* TTGet*_ttradiomenuitem methods */ 
    /* TTGet*_ttmenu methods */    
    "TTGetMenubar_ttmenu", 
    /* TTGet*_ttmenubar methods */ 
    /* TTGet*_ttanytext methods */    
    "TTGetTextLen_ttanytext",    
    "TTGetText_ttanytext", 
    /* TTGet*_tttextfield methods */ 
    /* TTGet*_tttextarea methods */ 
    /* TTGet*_tttheme methods */    
    "TTGetNameLen_tttheme",    
    "TTGetName_tttheme",                 
    /* TTGet*_ttapplication methods */    
    "TTGetNameLen_ttapplication",    
    "TTGetName_ttapplication",




};


/* addresses of existing methods */

#define A(m) (ttfunction_fn)TT_CAT(TT,m)


static TT_CONST ttfunction_fn method_addr[] = {

    /* generic methods */
    
    A(LibraryVersion),
    A(ExitMainLoop),
    A(New),
    A(Del),
    A(ClassOf),
    A(SuperClassOf),
    A(InstanceOf),
    A(ClassNameOf),

 
    /* ttobj methods */
                        
    A(GetField_ttobj),
    A(SetField_ttobj),
    A(ChangeField_ttobj), 
    /* ttclass methods */
    
    A(GetName_ttclass),
    A(GetSize_ttclass),
    A(GetSuper_ttclass),
    A(IsInstance_ttclass),
    A(FromType_ttclass),
    A(IsArrayType_ttclass),
    A(IsPrimitiveType_ttclass), 
    /* ttobject methods */
                    
    A(Ref_ttobject),
    A(Unref_ttobject),
    A(SetUserData_ttobject), 
    /* ttvector methods */
        
    A(GetAt_ttvector),    
    A(SetAt_ttvector),
    A(Create1_ttvector),
    A(CreateA_ttvector),
    A(CreateR_ttvector),
    A(CreateV_ttvector),
    A(CreateY_ttvector),
    A(Append_ttvector),
    A(Add_ttvector),
    A(AddA_ttvector),
    A(AddR_ttvector),
    A(AddV_ttvector),    
    A(AddY_ttvector),    
    A(ContainsValue_ttvector),
    A(RemoveAt_ttvector),    
    A(RemoveRange_ttvector), 
    /* ttfield methods */
    
    A(Get_ttfield),
    A(GetValue_ttfield),
    A(SetValue_ttfield),
    A(ChangeValue_ttfield),
    A(ToEvcode_ttfield),
    A(FromEvcode_ttfield),
    A(GetEvcode_ttfield), 
    /* ttmethod methods */
    
    A(Get_ttmethod),
    A(GetByAddress_ttmethod),
    A(GetArraySizeA_ttmethod),
    A(CheckArraySizesA_ttmethod),
    A(CallA_ttmethod),
    A(CallR_ttmethod),
    A(CallV_ttmethod),
    A(CallY_ttmethod), 
    /* ttevent methods */
    
    A(Create_ttevent),
    A(Fire_ttevent), 
    /* tteventbig methods */
    
    A(Create8_tteventbig),
    A(Create4_tteventbig),
    A(Create4s4_tteventbig), 
    /* ttbitmask methods */
    
    A(Create_ttbitmask),
    A(CreateB2_ttbitmask),
    A(CreateR_ttbitmask),
    A(CreateR2_ttbitmask),
    A(Clear_ttbitmask),
    A(Fill_ttbitmask),
    A(Negate_ttbitmask),
    A(Change_ttbitmask),
    A(Contains_ttbitmask),
    A(Add_ttbitmask),
    A(AddB_ttbitmask),
    A(AddR_ttbitmask),
    A(Remove_ttbitmask),
    A(RemoveB_ttbitmask),
    A(RemoveR_ttbitmask),
    A(Combine_ttbitmask),
    A(CombineR_ttbitmask),
    A(CombineB_ttbitmask), 
    /* tteventmask methods */
    
    A(CreateB_tteventmask),
    A(CreateB2_tteventmask),
    A(SetEvtypeMask_tteventmask),
    A(SetEvcodeMask_tteventmask),
    A(SetComponentMask_tteventmask),
    A(SetTruthTable_tteventmask), 
    /* ttcallback methods */
         
    /* ttlistener methods */
    
    A(Create_ttlistener),
    A(CreateE_ttlistener),
    A(CreateR_ttlistener),
    A(CreateDel_ttlistener),
    A(CreateAskclose_ttlistener),
    A(CreateKey_ttlistener),
    A(CreateMouse_ttlistener),
    A(CreateActivate_ttlistener),
    A(CreateChange_ttlistener),
    A(CreateKeyData_ttlistener),
    A(CreateExpose_ttlistener),    
    A(AddTo_ttlistener),    
    A(Remove_ttlistener),
    A(SetArgsR_ttlistener),
    A(SetEventMask_ttlistener), 
    /* tttimer methods */
    
    A(CreateR_tttimer),
    A(Create_tttimer),
    A(SetDelay_tttimer),    
    A(SetEnabled_tttimer),    
    A(IsEnabled_tttimer), 
    /* ttcomponent methods */
    
    A(AddListener_ttcomponent),
    A(RemoveListener_ttcomponent),
    A(SetKeyData_ttcomponent),
    A(GetKeyData_ttcomponent),
    A(UnsetKeyData_ttcomponent),
    A(GetData_ttcomponent), 
    /* ttdata methods */
            
    A(SetData_ttdata), 
    /* ttvisible methods */
        
    A(AddTo_ttvisible),    
    A(Validate_ttvisible),    
    A(Remove_ttvisible),    
    A(Invalidate_ttvisible),    
    A(SetVisible_ttvisible),    
    A(SetTheme_ttvisible),    
    A(Draw_ttvisible),    
    A(BuiltinRepaint_ttvisible),

    A(SetRepaint_ttvisible),
    A(Expose_ttvisible),
    A(Add_ttvisible), 
    /* ttlayout methods */
             
    /* ttboxlayout methods */
    
    A(Create_ttboxlayout), 
    /* ttborderlayout methods */
     
    /* ttnative methods */
        
    A(GetRoot_ttnative),    
    A(GetW_ttnative),    
    A(GetH_ttnative),
    A(Create_ttnative), 
    /* ttwidget methods */
        
    A(SetLayout_ttwidget),    
    A(SetXY_ttwidget),    
    A(SetWH_ttwidget),    
    A(SetXl_ttwidget),    
    A(SetYl_ttwidget),    
    A(SetWl_ttwidget),    
    A(SetHl_ttwidget),    
    A(SetTooltip_ttwidget),
    A(SetXlYl_ttwidget),
    A(SetWlHl_ttwidget),
    A(SetXYWH_ttwidget),
    A(SetX_ttwidget),
    A(SetY_ttwidget),
    A(SetW_ttwidget),
    A(SetH_ttwidget), 
    /* ttlabel methods */
        
    A(SetFontD_ttlabel),
    A(SetFont_ttlabel),
    A(SetText_ttlabel),
    A(Create_ttlabel), 
    /* tttooltip methods */
        
    A(SetWidget_tttooltip), 
    /* ttbuttongroup methods */
        
    A(Add_ttbuttongroup),    
    A(Remove_ttbuttongroup),    
    A(SetChecked_ttbuttongroup), 
    /* ttanybutton methods */
            
    A(SetAttrD_ttanybutton),
    A(SetAttr_ttanybutton),
    A(SetFont_ttanybutton),
    A(SetText_ttanybutton),
    A(Create_ttanybutton), 
    /* ttbutton methods */
        
    A(SetPressed_ttbutton),
    A(IsPressed_ttbutton),
    A(Create_ttbutton), 
    /* ttcheckbutton methods */
        
    A(SetChecked_ttcheckbutton),
    A(IsChecked_ttcheckbutton),
    A(Create_ttcheckbutton), 
    /* ttradiobutton methods */
    
    A(AddToGroup_ttradiobutton),
    A(RemoveFromGroup_ttradiobutton),
    A(Create_ttradiobutton), 
    /* ttanyscroll methods */
                
    A(SetOrientation_ttanyscroll),    
    A(SetSize_ttanyscroll),    
    A(SetRealSize_ttanyscroll),    
    A(SetViewSize_ttanyscroll),    
    A(SetPosition_ttanyscroll), 
    /* ttscrollbar methods */
    
    A(Create1_ttscrollbar),
    A(Create4_ttscrollbar), 
    /* ttslider methods */
        
    A(SetSlideMin_ttslider),    
    A(SetSlideMax_ttslider),    
    A(SetSlideValue_ttslider),
    A(Create_ttslider), 
    /* ttprogressbar methods */
    
    A(Create_ttprogressbar), 
    /* ttscrollpane methods */
    
    A(Create_ttscrollpane),    
    A(SetBarX_ttscrollpane),    
    A(SetBarY_ttscrollpane), 
    /* ttwindow methods */
        
    A(SetTitle_ttwindow), 
    /* ttframe methods */
        
    A(SetMenubar_ttframe),
    A(Create_ttframe), 
    /* ttscroller methods */
    
    A(Create_ttscroller), 
    /* ttmenuitem methods */
     
    /* ttcheckmenuitem methods */
     
    /* ttradiomenuitem methods */
     
    /* ttmenu methods */
     
    /* ttmenubar methods */
    
    A(GetDefault_ttmenubar), 
    /* ttanytext methods */
     
    /* tttextfield methods */
     
    /* tttextarea methods */
     
    /* tttheme methods */
    
    A(GetDefault_tttheme), 
    /* ttapplication methods */
    
    A(Create_ttapplication),
    A(Get_ttapplication),


    /* TTSet*_* methods omitted, they are listed above! */
#if 0
        
#endif
    /* TTGet*_* methods */




 
    /* TTGet*_ttobj methods */         
    /* TTGet*_ttclass methods */             
    /* TTGet*_ttobject methods */    
    A(GetRefcount_ttobject),    
    A(GetOflags_ttobject),            
    A(GetUserData_ttobject),    
    A(GetEventsInprogress_ttobject),    
    A(GetEventsInformed_ttobject), 
    /* TTGet*_ttvector methods */    
    A(GetArraySize_ttvector),    
    A(GetArrayN_ttvector),    
    A(GetArray_ttvector), 
    /* TTGet*_ttfield methods */    
    A(GetNameLen_ttfield),    
    A(GetName_ttfield),    
    A(GetType_ttfield), 
    /* TTGet*_ttmethod methods */    
    A(GetNameLen_ttmethod),    
    A(GetName_ttmethod),    
    A(GetAddress_ttmethod),    
    A(GetReturnType_ttmethod),    
    A(GetParameterType_ttmethod), 
    /* TTGet*_ttevent methods */    
    A(GetComponent_ttevent),    
    A(GetEvtype_ttevent),    
    A(GetEvcode_ttevent),    
    A(GetEvflags_ttevent),    
    A(GetPrev_ttevent),    
    A(GetNext_ttevent), 
    /* TTGet*_tteventbig methods */    
    A(GetX_tteventbig),    
    A(GetY_tteventbig),    
    A(GetW_tteventbig),    
    A(GetH_tteventbig),    
    A(GetValue_tteventbig),    
    A(GetOldValue_tteventbig),    
    A(GetDataLen_tteventbig),    
    A(GetData_tteventbig), 
    /* TTGet*_ttbitmask methods */                 
    /* TTGet*_tteventmask methods */    
    A(GetEvtypeMask_tteventmask),    
    A(GetEvcodeMask_tteventmask),    
    A(GetComponentMask_tteventmask),    
    A(GetTruthTable_tteventmask), 
    /* TTGet*_ttcallback methods */    
    A(GetLflags_ttcallback),    
    A(GetComponent_ttcallback),    
    A(GetPrev_ttcallback),    
    A(GetNext_ttcallback), 
    /* TTGet*_ttlistener methods */        
    A(GetEvent_ttlistener),    
    A(GetFunction_ttlistener),    
    A(GetArgComponentN_ttlistener),    
    A(GetArgEventN_ttlistener),    
    A(GetArgs_ttlistener),    
    A(GetEventMask_ttlistener),         
    /* TTGet*_tttimer methods */    
    A(GetDelayT_tttimer),    
    A(GetDelayF_tttimer),    
    A(GetTimerPrev_tttimer),    
    A(GetTimerNext_tttimer), 
    /* TTGet*_ttcomponent methods */    
    A(GetCallbacks_ttcomponent),    
    A(GetListeners_ttcomponent),    
    A(GetTimers_ttcomponent),    
    A(GetDatas_ttcomponent), 
    /* TTGet*_ttdata methods */        
    A(GetComponent_ttdata),    
    A(GetKeyLen_ttdata),    
    A(GetKey_ttdata),    
    A(GetData_ttdata), 
    /* TTGet*_ttvisible methods */    
    A(GetVflags_ttvisible),    
    A(GetPrev_ttvisible),    
    A(GetNext_ttvisible),    
    A(GetParent_ttvisible),    
    A(GetChildFirst_ttvisible),    
    A(GetChildLast_ttvisible),    
    A(GetConstraint_ttvisible),    
    A(GetTheme_ttvisible),    
    A(GetRepaint_ttvisible), 
    /* TTGet*_ttlayout methods */    
    A(GetWidget_ttlayout), 
    /* TTGet*_ttboxlayout methods */    
    A(GetOrientation_ttboxlayout), 
    /* TTGet*_ttborderlayout methods */ 
    /* TTGet*_ttnative methods */ 
    /* TTGet*_ttwidget methods */    
    A(GetX_ttwidget),    
    A(GetY_ttwidget),    
    A(GetW_ttwidget),    
    A(GetH_ttwidget),    
    A(GetLayout_ttwidget),    
    A(GetXl_ttwidget),    
    A(GetYl_ttwidget),    
    A(GetWl_ttwidget),    
    A(GetHl_ttwidget),    
    A(GetCol_ttwidget),    
    A(GetTooltip_ttwidget), 
    /* TTGet*_ttlabel methods */    
    A(GetTextLen_ttlabel),    
    A(GetText_ttlabel), 
    /* TTGet*_tttooltip methods */        
    A(GetWidget_tttooltip), 
    /* TTGet*_ttbuttongroup methods */    
    A(GetGroupFirst_ttbuttongroup),    
    A(GetGroupLast_ttbuttongroup),    
    A(GetChecked_ttbuttongroup), 
    /* TTGet*_ttanybutton methods */    
    A(GetTextWidth_ttanybutton),    
    A(GetTextHeight_ttanybutton),    
    A(GetText_ttanybutton),     
    /* TTGet*_ttbutton methods */ 
    /* TTGet*_ttcheckbutton methods */ 
    /* TTGet*_ttradiobutton methods */    
    A(GetGroup_ttradiobutton),    
    A(GetGroupPrev_ttradiobutton),    
    A(GetGroupNext_ttradiobutton), 
    /* TTGet*_ttanyscroll methods */    
    A(GetOrientation_ttanyscroll),    
    A(GetSize_ttanyscroll),    
    A(GetRealSize_ttanyscroll),    
    A(GetViewSize_ttanyscroll),        
    A(GetPosition_ttanyscroll),    
    A(GetRealPosition_ttanyscroll),    
    A(GetState_ttanyscroll), 
    /* TTGet*_ttscrollbar methods */     
    /* TTGet*_ttslider methods */    
    A(GetSlideMin_ttslider),    
    A(GetSlideMax_ttslider),    
    A(GetSlideValue_ttslider), 
    /* TTGet*_ttprogressbar methods */         
    /* TTGet*_ttscrollpane methods */    
    A(GetBarX_ttscrollpane),    
    A(GetBarY_ttscrollpane), 
    /* TTGet*_ttwindow methods */    
    A(GetTitleLen_ttwindow),    
    A(GetTitle_ttwindow), 
    /* TTGet*_ttframe methods */    
    A(GetMenubar_ttframe), 
    /* TTGet*_ttscroller methods */    
    A(GetScrollpane_ttscroller), 
    /* TTGet*_ttmenuitem methods */    
    A(GetNameLen_ttmenuitem),    
    A(GetName_ttmenuitem), 
    /* TTGet*_ttcheckmenuitem methods */ 
    /* TTGet*_ttradiomenuitem methods */ 
    /* TTGet*_ttmenu methods */    
    A(GetMenubar_ttmenu), 
    /* TTGet*_ttmenubar methods */ 
    /* TTGet*_ttanytext methods */    
    A(GetTextLen_ttanytext),    
    A(GetText_ttanytext), 
    /* TTGet*_tttextfield methods */ 
    /* TTGet*_tttextarea methods */ 
    /* TTGet*_tttheme methods */    
    A(GetNameLen_tttheme),    
    A(GetName_tttheme),                 
    /* TTGet*_ttapplication methods */    
    A(GetNameLen_ttapplication),    
    A(GetName_ttapplication),




};

#undef A

#define method_array_n (sizeof(method_name)/sizeof(method_name[0]))


