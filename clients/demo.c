/*
 *  demo.c  --  sample client for libTT
 *
 *  This program is placed in the public domain.
 *
 */

#include <stdio.h>

#include <TT/TT.h>
#include <TT/TTerrno.h>


TT_DECL_MAGIC(ttdemo_magic);

/*
 * since my_printf is used from TTCreate_ttcallback(),
 * it will always receive only one argument (nargs == 1, only args[0] available).
 * 
 * args[0] will contain the object that generated the event since TTCreateSimple_ttcallback()
 * was called with the ttcallback_lflags_arg0_component flag set.
 */
static ttany my_exit(ttany arg0) {
    printf("delete:  %x\n", (unsigned)arg0 /* this is (f) below */);
    if (TTINSTANCEOF(ttvisible, arg0))
	TTExitMainLoop();
    return 0;
}

static void my_repaint(ttvisible o, ttshort x, ttshort y, ttshort w, ttshort h) {
    printf("repaint: %x x=%d y=%d w=%d h=%d\n", (unsigned)o, (int)x, (int)y, (int)w, (int)h);
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
	 * takes a set of (ttany) parameters as arguments (only one actually, since
	 * we use TTCreate_ttcallback() and not one of TTCreate*_ttcallback() )
	 * 
	 * we could set ttcallback_lflags_arg0_component flag here too,
	 * but we do not to show how last argument of TTCreate_ttcallback() is passed
	 * verbatim to callback.
	 */
	TTCreate_ttcallback((ttcomponent)f, ttevent_evtype_del, 
			    ttcallback_lflags_function_plain, (void *)my_exit, (ttany)f) &&
	(
	 TTSetRepaint_ttvisible((ttvisible)f, my_repaint),
	 TTSetVisible_ttvisible((ttvisible)f, TRUE),
	 TTMainLoop()
	 );
	
    if (!ok)
	fprintf(stderr, "ttdemo: libTT error: %s%s\n", TTStrError(TTErrno), TTStrErrorDetail(TTErrno, TTErrnoDetail));
    
    TTClose();
    
    return !ok;
}

