/*
 *  fdlist.h  --  include for File Descriptor book-keeping
 *
 *  Copyright (C) 2001 by Massimiliano Ghilardi
 *
 */

#ifndef _TW_FDLIST_H
#define _TW_FDLIST_H

typedef struct fdlist fdlist;	/* for compressed sockets, two fdlist's are used: */
struct fdlist {			/* the uncompressed one has                       */
    int Fd;                     /* Fd == GZFD and pairSlot == the compressed one; */
    uldat pairSlot;		/* the compressed has                             */
    obj HandlerData;		/* Fd == real fd and pairSlot == the uncompressed;*/
    union {
	void (*S)(int Fd, uldat Slot);
	void (*D)(int Fd, obj  Data);
    } HandlerIO;
    msgport MsgPort;		/* other sockets just have                        */
    byte *WQueue;		/* Fd == real fd and pairSlot == NOSLOT;          */
    byte *RQueue;
    uldat WQlen, WQmax;
    uldat RQstart, RQlen, RQmax;
    void (*PrivateAfterFlush)(uldat Slot); /* private enable/disable compression routine.
					    * it gets called after RemoteFlush() and it must
					    * remove itself if needed (e.g. call-once routines)
					    */
    byte (*PrivateFlush)(uldat Slot); /* private flush ((un)compression) routine */
    void *PrivateData;		/* used by (un)compression routines to hold private data */
    byte extern_couldntwrite;
};

#endif /* _TW_FDLIST_H */

