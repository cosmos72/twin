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
    ttbyte ok;
    
    ok =
	TTCheckMagic(ttdemo_magic) &&
	TTOpen(NULL) &&
	TTCreate_ttapplication("ttdemo") &&
	(f = TTNEW(ttframe)) &&

	/*
	 * connect `askclose' event to TTExitMainLoop():
	 * 
	 * by default new listeners get called AFTER the other existing listeners
	 */
	TTCreateAskclose_ttlistener((ttcomponent)f, 0, (ttlistener_fn)TTExitMainLoop) &&
	
	(
	 TTSetWH_ttwidget((ttwidget)f, 10, 8),
	 TTSetVisible_ttvisible((ttvisible)f, TT_TRUE),
	 TTMainLoop()
	 );
	
    
    if (!ok)
	fprintf(stderr, "ttdemo: libTT error: %s%s\n", TTStrError(TTErrno), TTStrErrorDetail(TTErrno, TTErrnoDetail));
    
    TTClose();
    
    return !ok;
}

