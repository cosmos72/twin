/* contributed by Cristalle Azundris Sabon 
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
/* list msgports. */

#include <Tw/Tw.h>
#include <Tw/Twerrno.h>
#include <Tw/Twstat.h>
#include <Tw/Twstat_defs.h>

int errmsg(tdisplay td) {
    int err;
    if ((err = Tw_Errno(td))) {
	fprintf(stderr, "twlsmsgport: libTw error: %s%s\n",
		Tw_StrError(td, err), Tw_StrErrorDetail(td, err, Tw_ErrnoDetail(td)));
    }
    return !!err;
}

int main(int argc, char *argv[]) {
    tslist reply, reply2;
    uldat id;
    tdisplay td;
    int err;
    
    if (!(td = Tw_Open(NULL)) || !(id = Tw_GetAll(td)))
	return errmsg(td);

    /*
     * race! we cannot use Tw_StatL() here, since the Tw_* calls below
     * will overwrite the returned buffer. So we use Tw_CloneStatL(),
     * which copies the buffer in a private malloc:ed() area.
     */
    reply = Tw_CloneStatL(td, id, 1, TWS_all_ChildrenMsgPort_List);

    if (reply && reply->N >= 1) {
        tsfield f = reply->TSF;

        if (f->hash == TWS_all_ChildrenMsgPort_List && f->type == (TWS_vec|TWS_obj)) {
            uldat n = f->TWS_field_vecL/sizeof(tobj);
            TW_CONST tobj *data = (TW_CONST tobj*)f->TWS_field_vecV;
            int fl;
            /* data is guaranteed to be correctly aligned */

	    printf("twlsmgport: existing twin tmsgports:\n");
            while (n--) {
                fl = FALSE;
                printf("0x%x:\t", (unsigned) *data);
                reply2 = Tw_StatL(td, (*data), 1, TWS_msgport_Name);
                if (reply2 && reply2->N >= 1) {
                    f = reply2->TSF;
                    if (f && f->type == (TWS_vec|TWS_byte) && f->hash == TWS_msgport_Name) {
                        TW_CONST byte *Name = (TW_CONST byte*)f->TWS_field_vecV;
                        uldat NameLen = f->TWS_field_vecL;
                        fl = TRUE;
                        printf("%.*s\n", (int) NameLen, Name);
                    }
                }
		if (reply2)
		    Tw_DeleteStat(td, reply2);
                if (!fl)
                    putchar('\n');

                data++;
            }
            fflush(stdout);
        }
    }

    if (td && reply)
	Tw_DeleteStat(td, reply);
    
    err = errmsg(td);

    Tw_Close(td);

    return err;
}

