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

TT_DECL_MAGIC(ttdemo_magic);

int main(int argc, char *argv[]) {
    ttframe f;
    byte ok;
    
    ok =TTCheckMagic(ttdemo_magic) &&
	TTOpen(NULL) &&
	TTSet_ttapplication("ttdemo") &&
	(f = TTNEW(ttframe)) &&

	/*
	 * connect `askclose' event to TTExitMainLoop():
	 * 
	 * set ttcallback_lflags_after flag to ensure TTExitMainLoop
	 * gets called AFTER the other existing callbacks
	 */
	TTCreate_ttcallback((ttcomponent)f, ttevent_evtype_askclose,
			    ttcallback_lflags_after,
			    (void *)TTExitMainLoop, (ttany)0) &&
	(
	 TTSetWH_ttwidget((ttwidget)f, 10, 8),
	 TTSetVisible_ttvisible((ttvisible)f, TRUE),
	 TTMainLoop()
	 );
	
    
    if (!ok)
	fprintf(stderr, "ttdemo: libTT error: %s%s\n", TTStrError(TTErrno), TTStrErrorDetail(TTErrno, TTErrnoDetail));
    
    TTClose();
    
    return !ok;
}

