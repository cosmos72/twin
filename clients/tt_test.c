/*
 *  tt_test.c  --  sample client for libTT
 *
 *  This program is placed in the public domain.
 *
 */

#include <stdio.h>

#include "Tutf/Tutf.h"
#include "Tutf/Tutf_defs.h"

#include "TT/TT.h"
#include "TT/TTerrno.h"


TT_DECL_MAGIC(ttest_magic);

static TT_CONST hwfont htest[] = {
    't', 't', 'e', 's', 't'
};

int main(int argc, char *argv[]) {
    ttbutton l;
    byte ok;
    
    ok = TTCheckMagic(ttest_magic) && TTOpen(NULL) && TTCreate_ttapplication("ttest") &&
	(l = TTCreate_ttbutton(htest, 5, 1, 5)) && (TTAddTo_ttbutton(l, TTGetFirstScreen_ttnative()), TRUE) &&
	TTMainLoop();
	
    if (!ok)
	fprintf(stderr, "ttest: libTT error: %s%s\n", TTStrError(TTErrno), TTStrErrorDetail(TTErrno, TTErrnoDetail));
    
    TTClose();
    
    return !ok;
}

