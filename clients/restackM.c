/*
 *  restackM - reverse stacking order of all menuitems in BuiltinMenu
 *
 *  This program is placed in the public domain.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <Tw/Tw.h>
#include <Tw/Twerrno.h>
#include <Tw/Twstat.h>
#include <Tw/Twstat_defs.h>

#include "version.h"

TW_DECL_MAGIC(lsobj_magic);

static void reverse_list(uldat n, tobj *v) {
    tobj tmp;
    uldat i, j;
    
    for (i = 0, j = n-1; i<j; i++, j--) {
	tmp = v[i];
	v[i] = v[j];
	v[j] = tmp;
    }
}


int main(int argc, char *argv[]) {
    uldat err;
    tobj a, id, *v;
    tslist l;
    tsfield f;
    
    if (TwCheckMagic(lsobj_magic) && TwOpen(NULL)) {
	err = 0;
	a = TwGetAll();
	id = TwStat(a, TWS_all_BuiltinMenu);
	printf("All = 0x%lx\n -> BuiltinMenu = 0x%lx\n", (long)a, (long)id);
	
	if ((l = TwStatL(id, 1, TWS_menu_ChildrenI_List))) {
	    if ((f = TwFindStat(l, TWS_menu_ChildrenI_List)) &&
		f->type == (TWS_vec|TWS_tobj)) {
		
		err = f->TWS_field_vecL / sizeof(tobj);
		v = (tobj *)f->TWS_field_vecV;
		reverse_list(err, v);
		
		TwRestackRows(id, err, v);
		TwFlush();
	    }
	    TwDeleteStat(l);
	}
    }
    if ((err = TwErrno)) {
	fprintf(stderr, "%s: libTw error: %s%s\n", argv[0],
		TwStrError(err), TwStrErrorDetail(err, TwErrnoDetail));
	return 1;
    }
    return 0;
}

