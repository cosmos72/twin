/*
 *  demo3.c  --  sample client for libTT
 *
 *  This program is placed in the public domain.
 *
 */

#include <stdio.h>
#include <errno.h>

#include <TT/TT.h>
#include <TT/TTerrno.h>

TT_DECL_MAGIC(ttdemo_magic);

static ttslider s1, s2;
static ttlabel l1, l2;

static void print_error(void) {
    fprintf(stderr, "ttdemo3: libTT error: %s%s\n", TTStrError(TTErrno), TTStrErrorDetail(TTErrno, TTErrnoDetail));
}

static void show_slider(ttany arg0) {
    tteventbig e = (tteventbig)(ttopaque)arg0;
    ttslider s;
    byte buf[8];
    int val;
    
    if (TTINSTANCEOF(tteventbig, e)) {
	s = (ttslider)TTGetComponent_ttevent((ttevent)e);
	val = TTGetValue_tteventbig(e) & 0x3F; /* ensure not to overrun buf[] */
	sprintf(buf, "%c=%2d", s == s1 ? 'x' : 'y', val);
	if (s == s1)
	    TTSetText_ttlabel(l1, buf);
	else if (s == s2)
	    TTSetText_ttlabel(l2, buf);
    }
}

static ttbyte init(void) {
    ttframe f;
    ttbutton b;
    ttcheckbutton c;
    ttradiobutton r1, r2;
    ttbuttongroup g;
    ttscroller s;
    ttscrollpane p;
    ttlabel l3[6];
    ttint i;
    ttbyte ok = TT_FALSE;
    
    if (TTCheckMagic(ttdemo_magic) &&
	TTOpen(NULL) &&
	TTCreate_ttapplication("ttdemo3") &&

	(f = TTCreate_ttframe("ttdemo3")) &&
	
	/*
	 * connect `askclose' event for f to TTExitMainLoop():
	 * 
	 * by default (unless you set ttlistener_lflags_before flag)
	 * the listener gets called AFTER the other existing listeners
	 */
	TTCreateAskclose_ttlistener((ttcomponent)f, 0, (ttlistener_fn)TTExitMainLoop) &&
	
	(b  = TTCreate_ttbutton(6, 1, 6, "button")) &&
	(c  = TTCreate_ttcheckbutton(11, 1, 11, "checkbutton")) &&
	(r1 = TTCreate_ttradiobutton(12, 1, 12, "radiobutton1")) &&
	(r2 = TTCreate_ttradiobutton(12, 1, 12, "radiobutton2")) &&
	(g  = TTNEW(ttbuttongroup)) &&
	
	(s1 = TTCreate_ttslider(ttanyscroll_orientation_x, 12)) &&
	(s2 = TTCreate_ttslider(ttanyscroll_orientation_y, 9)) &&

	(s  = TTCreate_ttscroller(22, 6)) &&
	
	(l1 = TTCreate_ttlabel("x= 0")) &&
	(l2 = TTCreate_ttlabel("y= 0")) &&
	(l3[0] = TTCreate_ttlabel("this is a scroll test.")) &&
	(l3[1] = TTCreate_ttlabel("you should be able to scroll")) &&
	(l3[2] = TTCreate_ttlabel("this text with the scrollbars")) &&
	(l3[3] = TTCreate_ttlabel("at the right and at the bottom")) &&
	(l3[4] = TTCreate_ttlabel("of the text itself. Please report")) &&
	(l3[5] = TTCreate_ttlabel("any incorrect behaviour you find.")) &&

	TTCreateChange_ttlistener((ttcomponent)s1, TTGetEvcode_ttfield("ttslider_slide_value"),
				  0, (ttlistener_fn)show_slider) &&
	
	TTCreateChange_ttlistener((ttcomponent)s2, TTGetEvcode_ttfield("ttslider_slide_value"),
				  0, (ttlistener_fn)show_slider) &&
	
	(ok = TT_TRUE)  ) {
	
	
	TTSetXY_ttwidget((ttwidget)b,  0, 0);
	TTSetXY_ttwidget((ttwidget)c,  0, 2);
	TTSetXY_ttwidget((ttwidget)r1, 0, 4);
	TTSetXY_ttwidget((ttwidget)r2, 0, 6);
	TTSetXY_ttwidget((ttwidget)s1, 0, 8);
	TTSetXY_ttwidget((ttwidget)s2,17, 0);
	TTSetXY_ttwidget((ttwidget)l1,19, 2);
	TTSetXY_ttwidget((ttwidget)l2,19, 4);
	TTSetXY_ttwidget((ttwidget)s,  1,10);
	
	TTAdd_ttvisible((ttvisible)f, (ttvisible)b , ttlayout_constraint_root);
	TTAdd_ttvisible((ttvisible)f, (ttvisible)c , ttlayout_constraint_root);
	TTAdd_ttvisible((ttvisible)f, (ttvisible)r1, ttlayout_constraint_root);
	TTAdd_ttvisible((ttvisible)f, (ttvisible)r2, ttlayout_constraint_root);
	TTAdd_ttvisible((ttvisible)f, (ttvisible)s1, ttlayout_constraint_root);
	TTAdd_ttvisible((ttvisible)f, (ttvisible)s2, ttlayout_constraint_root);
	TTAdd_ttvisible((ttvisible)f, (ttvisible)l1, ttlayout_constraint_root);
	TTAdd_ttvisible((ttvisible)f, (ttvisible)l2, ttlayout_constraint_root);
	TTAdd_ttvisible((ttvisible)f, (ttvisible)s , ttlayout_constraint_root);

	p = TTGetScrollpane_ttscroller(s);
	TTSetWlHl_ttwidget((ttwidget)p, 0x3, 35, 7);
	
	for (i = 0; i < sizeof(l3)/sizeof(l3[0]); i++) {
	    TTSetXY_ttwidget((ttwidget)l3[i], 0, i);
	    TTAdd_ttvisible((ttvisible)p, (ttvisible)l3[i], ttlayout_constraint_root);
	}
	
	TTAdd_ttbuttongroup(g, r1);
	TTAdd_ttbuttongroup(g, r2);
	
	TTSetWH_ttwidget((ttwidget)f, 24,16);
	
	TTSetVisible_ttvisible((ttvisible)f, TT_TRUE);
    }

    return ok;
}

static ttbyte mainloop(void) {
    return TTMainLoop();
}

static void quit(void) {
    if (TTErrno)
	print_error();
    
    TTClose();
}

int main(int argc, char *argv[]) {
    ttbyte ret;
    
    ret = init() && mainloop();
    
    quit();
    
    return !ret;
}
