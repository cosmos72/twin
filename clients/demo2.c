/*
 *  demo.c  --  sample client for libTT.
 *              It mostly demonstrates some libTT functions,
 *              without being actually useful.
 *
 *  This program is placed in the public domain.
 *
 */

#include <stdio.h>
#include <errno.h>

#include <TT/TT.h>
#include <TT/TTerrno.h>

static TT_VOLATILE ttbyte my_exitmainloop;

TT_DECL_MAGIC(ttdemo_magic);

static void stat_ttobj(ttobj f) {
    ttfield i, last;
    TT_CONST ttbyte *name;
    ttbyte buf[64];
    ttarg data;
    ttclass Class;
    ttopaque type;
    ttany value;
    
    printf("(%s) 0x%x = {\n", name = TTClassNameOf(f), (unsigned)(ttopaque)f);
    sprintf(buf, "%s_field_last", name);
    last = TTGet_ttfield(buf);
    
    i = TTGet_ttfield("ttobj_field_first");
    for (; i < last; i = (ttfield)((ttopaque)i + 1)) {
	/*
	 * evil trick here:
	 * TTGet_ttfield() actually returns an integer,
	 * and we try all values for that integer from
	 * ttfield "ttobj_field_first" to "<objtype>_field_last"
	 */
	if ((name = TTGetName_ttfield(i)) && TTGetField_ttobj(f, i, &data)) {
	    value = data.value;
	    type = data.type;
	    Class = TTFromType_ttclass(type);
	    
	    /* if it's an array of bytes (i.e. a string) then print as a string */
	    if (Class == TTClass_ttbyte && TTIsArrayType_ttclass(type)) {
		if (value)
		    printf("\t%s\t= (%s)\t\"%s\";\n", name, TTGetName_ttclass(Class), (ttbyte *)(ttopaque)value);
		else
		    printf("\t%s\t= (%s)\tNULL;\n", name, TTGetName_ttclass(Class));
	    } else
		printf("\t%s\t= (%s)\t0x%lx;\n", name, TTGetName_ttclass(Class), (unsigned long)value);
	}
    }
    printf("}\n");
}

/*
 * since my_printf is used from TTCreateDel_ttlistener(),
 * it will always receive only one argument:
 * arg0 will contain the object that generated the event since TTCreateDel_ttlistener()
 * was called with the ttlistener_lflags_arg0_component flag set.
 */
static ttany my_exit(ttany arg0) {
    printf("delete:  0x%x\n", (unsigned)arg0 /* this is (f) below */);
    if (TTIS(ttvisible, arg0)) {
	/*
	 * the double cast (ttobj)(ttopaque)arg0 avoid warnings in case
	 * the integer type (ttany) of arg0 has different size than the pointer type (ttobj)
	 */
	stat_ttobj((ttobj)(ttopaque)arg0);
	/* TTExitMainLoop(); */
	my_exitmainloop = TT_TRUE;
    }
    return 0;
}

static void my_repaint(ttvisible o, ttshort x, ttshort y, ttshort w, ttshort h) {
    printf("repaint: 0x%x x=%d y=%d w=%d h=%d\n", (unsigned)(ttopaque)o, (int)x, (int)y, (int)w, (int)h);
}

static void my_resize(ttany arg0) {
    ttcomponent f;
    tteventbig e;
    static ttuint widget_w, widget_h;
    ttany value, old_value;
    ttany evcode;
    
    if (TTIS(tteventbig, arg0)) {
	e = (tteventbig)(ttopaque)arg0;
	f = TTGetComponent_ttevent((ttevent)e);

	evcode    = TTGetEvcode_ttevent((ttevent)e);
	value     = TTGetValue_tteventbig(e);
	old_value = TTGetOldValue_tteventbig(e);
	
	if (!widget_w) {
	    widget_w = TTGetEvcode_ttfield("ttwidget_w");
	    widget_h = TTGetEvcode_ttfield("ttwidget_h");
	}
	printf("resize: 0x%x %c=%d (was %d)\n", (unsigned)(ttopaque)f,
	       evcode == widget_w ? 'w' : 'h',
	       (int)value, (int)old_value);
    }
}

static void my_keydata_changed(ttany arg0) {
    tteventbig e;
    ttcomponent f;
    ttdata d;
    
    if (TTIS(tteventbig, arg0)) {
	e = (tteventbig)(ttopaque)arg0;
	d = (ttdata)TTGetComponent_ttevent((ttevent)e);
	if (TTIS(ttdata,d)) {
	    f = TTGetComponent_ttdata(d);
	    printf("key data change: 0x%x [key 0x%x, \"%s\"] data=%d (was %d)\n",
		   (unsigned)(ttopaque)f, (unsigned)(ttopaque)d, TTGetKey_ttdata(d),
		   TTGetValue_tteventbig(e), TTGetOldValue_tteventbig(e));
	}
    }
}

static ttbyte my_init(void) {
    ttframe f;
    ttlistener l1, l2;
    
    if (TTCheckMagic(ttdemo_magic) &&
	TTOpen(NULL) &&
	TTCreate_ttapplication("ttdemo") &&
	(f = TTNEW(ttframe)) &&

	/*
	 * connect `askclose' event to TTDel():
	 * this is only to show how to do it, since TTClose()
	 * or exit() are already enough to shutdown libTT
	 * 
	 * by default new listeners get called AFTER the other existing listeners
	 * 
	 * set ttlistener_lflags_arg0_component flag so that
	 * first argument will be (f)
	 */
	TTCreateAskclose_ttlistener((ttcomponent)f, ttlistener_lflags_arg0_component,
				    (void *)TTDel) &&
	
	/*
	 * when f gets deleted (by callback above), call my_exit to exit libTT main loop.
	 */
	TTCreateDel_ttlistener((ttcomponent)f, ttlistener_lflags_arg0_component,
			       (void *)my_exit) &&
	
	/*
	 * react to f resizes.
	 */
	(l1 = TTCreateChange_ttlistener((ttcomponent)f, TTGetEvcode_ttfield("ttwidget_w"),
					0, (void *)my_resize)) &&
	(l2 = TTCreateChange_ttlistener((ttcomponent)f, TTGetEvcode_ttfield("ttwidget_h"),
					0, (void *)my_resize)) &&

	/*
	 * the following two function show how to use KeyData in ttcomponents
	 * and watch for changes in it.
	 */
	TTSetKeyData_ttcomponent((ttcomponent)f, "somestring", 19) &&
	TTCreateKeyData_ttlistener((ttcomponent)f, "somestring",
				   0, (void *)my_keydata_changed) &&
	TTSetKeyData_ttcomponent((ttcomponent)f, "somestring", 123456789) &&
	
	TT_TRUE)
    {
	/*
	 * play with blocked events
	 */
	ttuint evtypes[] = {
	    0x2000, 0x0100, 0x1000
	};
	ttopaque n = sizeof(evtypes)/sizeof(evtypes[0]);
	
	TTSetEventMask_ttlistener
	    (l1, TTCreateB_tteventmask
	     (TTCreateR_ttbitmask
	      (TTCreateY_ttvector
	       (n, evtypes)), NULL, NULL));
	/*
	 * now we cannot use the created ttvectors, ttbitmasks and tteventmasks anymore:
	 * TTSetEventMask_ttlistener(), TTCreate_tteventmask() and TTCreateV_ttbitmask()
	 * swallow them.
	 */
	TTSetEventMask_ttlistener
	    (l2, TTCreateB_tteventmask
	     (TTCreateR_ttbitmask
	      (TTCreateY_ttvector
	       (n, evtypes)), NULL, NULL));
	/*
	 * idem as above
	 */

	
	    
	TTSetRepaint_ttvisible((ttvisible)f, my_repaint);
	TTSetWH_ttwidget((ttwidget)f, 10, 8);
	TTSetVisible_ttvisible((ttvisible)f, TT_TRUE);

	return TT_TRUE;
    }
    
    return TT_FALSE;
}

static ttbyte my_mainloop(void) {
    int fd, ret;
    fd_set rfds;

    if ((fd = TTConnectionFd()) < 0)
	return FALSE;
    
    FD_ZERO(&rfds);
    for (; !my_exitmainloop; ) {
	do {
	    /* important: we must TTFlush() before select() ! */
	    TTFlush();
	    FD_SET(fd, &rfds);
	    /* we could also add other fds to select(), or set a timeout */
	    ret = select(fd+1, &rfds, NULL, NULL, NULL);
	} while (ret < 0 && errno == EINTR && !my_exitmainloop);
	if (FD_ISSET(fd, &rfds))
	    /*
	     * here `FALSE' is the `wait' parameter:
	     * if events are immediately available TTMainLoopOnce(wait) dispatches them,
	     * else if `wait' is set waits for at least one event and dispatches it,
	     * else just returns
	     */
	    if (!TTMainLoopOnce(FALSE))
		break;
    }
    return my_exitmainloop;
}

int main(int argc, char *argv[]) {
    byte ok;
    
    if ((ok = my_init())) {
	/*
	 * ok = TTMainLoop();
	 */
	/* do it the hard way instead: */
	ok = my_mainloop();
    }
    
    if (!ok)
	fprintf(stderr, "ttdemo: libTT error: %s%s\n", TTStrError(TTErrno), TTStrErrorDetail(TTErrno, TTErrnoDetail));
    
    TTClose();
    
    return !ok;
}

