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
    /*
     * PrivateFlush and PrivateData are used both on low-level, compressed slot and on
     * uncompressed slot: in the compressed slot they are used to compress outgoing data,
     * while in the uncompressed one they are used to uncompress incoming data.
     */
    byte AlienMagic[8];			/* sizes and endianity used by slot
					 * instead of native sizes and endianity */
    byte extern_couldntwrite;
};

enum Alien_types {
	Alien_xendian = 0,
	/* the following are hardcoded in socket.c, libTw.c and libTw ABI, don't change! */
	Alien_byte = 1, Alien_num = 1,
	Alien_dat  = 2, Alien_udat = 2,
	Alien_ldat = 3, Alien_uldat = 3,
	Alien_hwcol = 4,
	Alien_time_t = 5, Alien_frac_t = 6,
	/* this is internal to socketalien.c */
	Alien_hwattr = 7
};

enum Alien_magics {
	/* these are possible values of AlienMagic[Alien_xendian]  */
	MagicUnknown = 0, MagicNative = 1,
	MagicAlien = 2, MagicAlienXendian = 3
};

#define AlienMagic(slot)	(FdList[slot].AlienMagic)

#define AlienXendian(slot)	AlienMagic(slot)[Alien_xendian]
#define AlienSizeof(type, slot) AlienMagic(slot)[Alien_##type]



#endif /* _TW_FDLIST_H */

