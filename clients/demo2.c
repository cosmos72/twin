/*
 *  demo.c  --  sample client for libTT
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
    ttuint i, last;
    TT_CONST ttbyte *name;
    ttbyte buf[64];
    ttany data;
    
    printf("(%s) 0x%x = {\n", name = TTClassNameOf(f), (unsigned)f);
    sprintf(buf, "%s_field_last", name);
    last = TTGetValueId(buf);
    for (i = 0; i < last; i++) {
	if ((name = TTGetValueName(i)) && TTGetValue_ttobj(f, i, &data))
	    printf("\t%s\t= 0x%x;\n", name, (unsigned)(ttopaque)data);
    }
    printf("}\n");
}

/*
 * since my_printf is used from TTCreate_ttcallback(),
 * it will always receive only one argument (nargs == 1, only args[0] available).
 * 
 * args[0] will contain the object that generated the event since TTCreate_ttcallback()
 * was called with the ttcallback_lflags_arg0_component flag set.
 */
static ttany my_exit(ttany arg0) {
    printf("delete:  %x\n", (unsigned)arg0 /* this is (f) below */);
    if (TTINSTANCEOF(ttvisible, arg0)) {
	stat_ttobj((ttobj)arg0);
	/* TTExitMainLoop(); */
	my_exitmainloop = TRUE;
    }
    return 0;
}

static void my_repaint(ttvisible o, ttshort x, ttshort y, ttshort w, ttshort h) {
    printf("repaint: %x x=%d y=%d w=%d h=%d\n", (unsigned)(ttopaque)o, (int)x, (int)y, (int)w, (int)h);
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
    ttframe f;
    byte ok;
    
    ok =TTCheckMagic(ttdemo_magic) &&
	TTOpen(NULL) &&
	TTSet_ttapplication("ttdemo") &&
	(f = TTNEW(ttframe)) &&

	/*
	 * connect `askclose' event to TTDel():
	 * this is only to show how to do it, since TTClose()
	 * or exit() are already enough to shutdown libTT
	 * 
	 * set ttcallback_lflags_after flag to ensure TTDel
	 * gets called AFTER the other existing callbacks
	 * 
	 * also set ttcallback_lflags_arg0_component flag so that
	 * first argument will be (f)
	 */
	TTCreate_ttcallback((ttcomponent)f, ttevent_evtype_askclose,
			    ttcallback_lflags_after|ttcallback_lflags_arg0_component,
			    (void *)TTDel, (ttany)0) &&
	
	/*
	 * when f gets deleted (by callback above), call my_exit to exit libTT main loop.
	 * 
	 * set ttcallback_lflags_function_plain flag to tell libTT that my_printf
	 * takes a list of (ttany) parameters as arguments (only one actually, since
	 * we use TTCreate_ttcallback() and not one of TTCreate*_ttcallback() )
	 * 
	 * we could set ttcallback_lflags_arg0_component flag here too,
	 * but we do not, to show how last argument of TTCreate_ttcallback() is passed
	 * verbatim to callback.
	 */
	TTCreate_ttcallback((ttcomponent)f, ttevent_evtype_del, 
			    ttcallback_lflags_function_plain, (void *)my_exit, (ttany)f) &&
	(
	 TTSetRepaint_ttvisible((ttvisible)f, my_repaint),
	 TTSetWH_ttwidget((ttwidget)f, 10, 8),
	 TTSetVisible_ttvisible((ttvisible)f, TRUE),
	 TRUE
	 );
	
    
    if (ok) {
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

