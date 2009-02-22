/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
/*
 *  fdlist.h  --  include for File Descriptor book-keeping
 *
 *  Copyright (C) 2001 by Massimiliano Ghilardi
 *
 */

#ifndef _TWIN_FDLIST_H
#define _TWIN_FDLIST_H

typedef void (*handler_io_s)(int, uldat);
typedef void (*handler_io_d)(int, obj);

typedef struct fdlist fdlist;	/* for compressed sockets, two fdlist's are used: */
struct fdlist {			/* the uncompressed one has                       */
    int Fd;                     /* Fd == GZFD and pairSlot == the compressed one; */
    uldat pairSlot;		/* the compressed has                             */
    obj HandlerData;		/* Fd == real fd and pairSlot == the uncompressed;*/
    union {
	handler_io_s S;
	handler_io_d D;
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
    /*
     * PrivateFlush and PrivateData are used both on low-level, compressed slot and on
     * uncompressed slot: in the compressed slot they are used to compress outgoing data,
     * while in the uncompressed one they are used to uncompress incoming data.
     */
    byte AlienMagic[9 /*TWS_highest*/];/* sizes and endianity used by slot
					* instead of native sizes and endianity */
    byte extern_couldntwrite;
};

enum Alien_magics {
	/* these are possible values of AlienXendian(slot) */
	MagicUnknown = 0, MagicNative = 1,
	MagicAlien = 2, MagicAlienXendian = 3
};

#define AlienMagic(slot)	(FdList[slot].AlienMagic)

#define AlienXendian(slot)	AlienMagic(slot)[TWS_void /*0*/]
#define AlienSizeof(type, slot) AlienMagic(slot)[TWS_##type]



#endif /* _TWIN_FDLIST_H */

