/*
 *  remote.c  -- file descriptor connections handling functions.
 *               this is the backed used by sock.c, term.c
 *               and by all code that wants to talk to the outside
 *               using file descriptors.
 *
 *               actually, sock.c needs some more functions
 *               of this kind that are directly placed in it.
 *
 *  Copyright (C) 1999-2000 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>

#include "twin.h"
#include "data.h"
#include "main.h"
#include "methods.h"
#include "hw.h"

/* variables */

void remoteKillSlot(uldat slot);
void (*KillSlot)(uldat slot) = remoteKillSlot;
void (*SocketSendMsg)(msgport *MsgPort, msg *Msg);

fdlist *FdList;
uldat FdSize, FdTop, FdBottom, FdWQueued;
#define LS	FdList[Slot]


/* functions */

static uldat FdListGrow(void) {
    uldat oldsize, size;
    fdlist *newFdList;
    
    if ((oldsize = FdSize) == MAXULDAT)
	return NOSLOT;
    
    if ((size = oldsize < 64 ? 96 : oldsize + (oldsize>>1)) < oldsize)
	size = MAXULDAT;
    
    /* use realloc(), not ReAllocMem() here */
    if (!(newFdList = (fdlist *)realloc(FdList, size*sizeof(fdlist))))
	return NOSLOT;
    
    for (FdSize = oldsize+1; FdSize<size; FdSize++)
	newFdList[FdSize].Fd = NOFD;
    
    FdList = newFdList;
    
    return oldsize;
}

INLINE uldat FdListGet(void) {
    if (FdBottom < FdSize)
	return FdBottom;
    return FdListGrow();
}

byte RemoteFlush(uldat Slot) {
    uldat chunk = 0, offset = 0;
    
    if (Slot == NOSLOT || Slot >= FdTop || LS.Fd == NOFD)
	return FALSE;

#if defined(CONF_SOCKET_GZ) || defined(CONF__MODULES)
    if (LS.PrivateFlush) {
	/* a (gzipped) paired slot:
	 * PrivateFlush() does everything:
	 * first gzip the data, then flush it */
	chunk = LS.PrivateFlush(Slot);

	if (LS.PrivateAfterFlush)
	    LS.PrivateAfterFlush(Slot);
	return (byte)chunk;
    }
#endif

    if (LS.WQlen == 0) {
	if (LS.PrivateAfterFlush)
	    LS.PrivateAfterFlush(Slot);
	return TRUE;
    }
    
    while (LS.WQlen && ((chunk = write(LS.Fd, LS.WQueue + offset, LS.WQlen)),
			chunk && chunk != (uldat)-1)) {
	offset += chunk;
	LS.WQlen -= chunk;
    }
    
    if (LS.WQlen) {
	FD_SET(LS.Fd, &save_wfds);
	if (offset)
	    MoveMem(LS.WQueue + offset, LS.WQueue, LS.WQlen);
    } else {
	FD_CLR(LS.Fd, &save_wfds);
	FdWQueued--;
    }
    
    if (LS.PrivateAfterFlush)
	LS.PrivateAfterFlush(Slot);
    return !LS.WQlen;
}

void RemoteFlushAll(void) {
    uldat Slot;
    for (Slot = 0; FdWQueued && Slot < FdTop; Slot++) {
	if (LS.Fd != NOFD && LS.WQlen)
	    (void)RemoteFlush(Slot);
    }
}

void RemoteCouldntWrite(uldat Slot) {
    if (Slot == NOSLOT || Slot >= FdTop || LS.Fd == NOFD)
	return;
    if (LS.extern_couldntwrite == FALSE) {
	LS.extern_couldntwrite = TRUE;
	FdWQueued++;
    }
    FD_SET(LS.Fd, &save_wfds);
}

void RemoteCouldWrite(uldat Slot) {
    if (Slot == NOSLOT || Slot >= FdTop || LS.Fd == NOFD)
	return;
    if (LS.extern_couldntwrite == TRUE) {
	LS.extern_couldntwrite = FALSE;
	FdWQueued--;
    }
    FD_CLR(LS.Fd, &save_wfds);
}

msgport *RemoteGetMsgPort(uldat Slot) {
    if (Slot < FdTop && LS.Fd != NOFD)
	return LS.MsgPort;
    return (msgport *)0;
}

/* Register a Fd, its HandlerIO and eventually its HandlerData arg */
/*
 * On success, return the slot number.
 * On failure, return NOSLOT (-1).
 */
uldat RegisterRemote(int Fd, void *HandlerData, void (*HandlerIO)(int Fd, size_t any)) {
    uldat Slot, j;
    
    if ((Slot = FdListGet()) == NOSLOT)
	return Slot;
    
    LS.Fd = Fd;
    LS.pairSlot = NOSLOT;
    LS.HandlerData = HandlerData;
    LS.HandlerIO = HandlerIO;
    LS.MsgPort = (msgport *)0;
    LS.WQueue = LS.RQueue = (byte *)0;
    LS.WQlen = LS.WQmax = LS.RQlen = LS.RQmax = (uldat)0;
    LS.PrivateAfterFlush = LS.PrivateData = LS.PrivateFlush = NULL;
    LS.extern_couldntwrite = FALSE;
    
    if (FdTop <= Slot)
	FdTop = Slot + 1;
    for (j = FdBottom + 1; j < FdTop; j++)
	if (FdList[j].Fd == NOFD)
	    break;
    FdBottom = j;

    if (Fd >= 0) {
	FD_SET(Fd, &save_rfds);
	if (max_fds < Fd)
	    max_fds = Fd;
    }
    return Slot;
}

uldat RegisterRemoteFd(int Fd, void (*HandlerIO)(int Fd, uldat Slot)) {
    return RegisterRemote(Fd, (window *)0, (void (*)(int, size_t))HandlerIO);
}

byte RegisterWindowFdIO(window *Window, void (*HandlerIO)(int Fd, window *Window)) {
    return (Window->RemoteData.FdSlot =
	    RegisterRemote(Window->RemoteData.Fd, Window, (void (*)(int, size_t))HandlerIO))
	!= NOSLOT;
}

/* UnRegister a Fd and related stuff given a slot number */
void UnRegisterRemote(uldat Slot) {
    int i;
    uldat j;
    
    if (Slot < FdTop && LS.Fd != NOFD) {
	/*
	 * really not the case here...
	 * call RemoteFlush() manually before UnRegisterRemote() if you need
	 */
	/* RemoteFlush(Slot); */
	if (LS.WQlen) {
	    /* trow away any data still queued :( */
	    LS.WQlen = 0;
	    FdWQueued--;
	}
	if (LS.extern_couldntwrite == TRUE) {
	    FdWQueued--;
	    FD_CLR(LS.Fd, &save_wfds);
	}
	LS.RQlen = 0;
	if (LS.WQueue)
	    FreeMem(LS.WQueue);
	if (LS.RQueue)
	    FreeMem(LS.RQueue);
	
	i = LS.Fd;
	LS.Fd = NOFD;
	
	if (i >= 0) {
	    FD_CLR(i, &save_rfds);
	
	    for (i=max_fds; i>=0; i--) {
		if (FD_ISSET(i, &save_rfds))
		    break;
	    }
	    max_fds = i;
	}
	
	if (FdBottom > Slot)
	    FdBottom = Slot;
	for (j = FdTop - 1; j > FdBottom; j--)
	    if (FdList[j].Fd != NOFD)
		break;
	FdTop = (j == FdBottom) ? j : j + 1;
	
    }
}

void UnRegisterWindowFdIO(window *Window) {
    if (Window && Window->RemoteData.FdSlot < FdTop) {
	UnRegisterRemote(Window->RemoteData.FdSlot);
	Window->RemoteData.FdSlot = NOSLOT;
    }
}

void remoteKillSlot(uldat slot) {
    msgport *MsgPort;
    display_hw *D_HW;
    
    if (slot != NOSLOT) {
	if ((MsgPort = RemoteGetMsgPort(slot))) {
	    /*
	     * no infinite recursion between KillSlot and DeleteMsgPort...
	     * DeleteMsgPort doesn't kill the slot, only tries to unregister from it
	     */

	    if ((D_HW = MsgPort->AttachHW)) {
		/* avoid KillSlot <-> DeleteDisplayHW infinite recursion */
		MsgPort->AttachHW = (display_hw *)0;
		Delete(D_HW);
	    }
		
	    Delete(MsgPort); /* and all its children ! */
	}
	
	if (FdList[slot].Fd >= 0)
	    close(FdList[slot].Fd);
	UnRegisterRemote(slot);
    }
}

void RemoteEvent(int FdCount, fd_set *FdSet) {
    uldat Slot;
    int fd;
    for (Slot=0; Slot<FdTop && FdCount; Slot++) {
	if ((fd = LS.Fd) >= 0) {
	    if (FD_ISSET(fd, FdSet)) {
		FdCount--;
		if (LS.HandlerData)
		    (*(void (*)(int, void *))LS.HandlerIO) (fd, LS.HandlerData);
		else
		    (*(void (*)(int, uldat))LS.HandlerIO) (fd, Slot);
	    }
	}
    }
}

void RemoteParanoia(void) {
    struct timeval zero;
    fd_set sel_fds;
    int safe, unsafe, test;
    uldat Slot;
    
    /* rebuild max_fds and save_rfds */
    FD_ZERO(&save_rfds);    
    for (Slot=0; Slot<FdTop; Slot++) {
	if ((test = LS.Fd) >= 0) {
	    FD_SET(test, &save_rfds);
	    max_fds = test;
	}
    }

    sel_fds = save_rfds;
    zero.tv_sec = zero.tv_usec = 0;
    if (select(max_fds+1, &sel_fds, NULL, NULL, &zero) >= 0)
	return;

    
    if (errno != EBADF && errno != EINVAL) {
	/* transient error ? */
	for (test = 10; test > 0; test--) {
	    sel_fds = save_rfds;
	    zero.tv_sec = 0;
	    zero.tv_usec = 100000;
	    if (select(max_fds+1, &sel_fds, NULL, NULL, &zero) >= 0)
		break;
	}
	if (test > 0)
	    /* solved ? */
	    return;
    }
    
    /*
     * Assume we must thrash some fd with permanent errors.
     * Find the first one by binary seach.
     */
    safe = 0;
    unsafe = max_fds+1;
    
    while (safe + 1 < unsafe) {
	sel_fds = save_rfds;
	zero.tv_sec = zero.tv_usec = 0;
	
	test = (unsafe + safe) / 2;
	if (select(test, &sel_fds, NULL, NULL, &zero) >= 0)
	    safe = test;
	else
	    unsafe = test;
    }
    /* ok, let's trow away 'unsafe-1' fd. */
	
    for (Slot=0; Slot<FdTop; Slot++) {
	if (safe == LS.Fd) {
	    UnRegisterRemote(Slot);
	    
	    /* let the Handler realize this fd is dead */
	    if (LS.HandlerData)
		(*(void (*)(int, window *))(LS.HandlerIO)) (safe, LS.HandlerData);
	    else
		(*(void (*)(int, uldat))(LS.HandlerIO)) (safe, Slot);
	    
	    return;
	}
    }
    
    /* Paranoia: still here? */
    FD_CLR(safe, &save_rfds);
}

/*
 * Add data[0...len-1] bytes to Write queue.
 * If data is NULL, don't copy data... but alloc (len) bytes and mark them as busy.
 *
 * return len if succeeded, 0 if failed.
 */
uldat RemoteWriteQueue(uldat Slot, uldat len, void *data) {
    uldat nmax;
    byte *tmp;
    
    if (len == 0 || Slot == NOSLOT || LS.Fd == NOFD)
	return 0;
    
    /* append to queue */
    if (LS.WQlen + len > LS.WQmax) {
	tmp = (byte *)ReAllocMem(LS.WQueue, nmax = (LS.WQmax+len+40)*5/4);
	if (!tmp)
	    return 0;
	LS.WQueue = tmp;
	LS.WQmax = nmax;
    }
    
    if (!LS.WQlen)
	FdWQueued++;
    if (data)
	CopyMem(data, LS.WQueue + LS.WQlen, len);
    LS.WQlen += len;
    return len;
}

void RegisterMsgPort(msgport *MsgPort, uldat Slot) {
    if (MsgPort && MsgPort->RemoteData.FdSlot == NOSLOT &&
	Slot < FdTop && LS.Fd != NOFD && !LS.MsgPort) {
	
	LS.MsgPort = MsgPort;
	MsgPort->RemoteData.FdSlot = Slot;
	MsgPort->RemoteData.Fd = LS.Fd;
    }
}

void UnRegisterMsgPort(msgport *MsgPort) {
    uldat Slot;
    if (MsgPort && (Slot = MsgPort->RemoteData.FdSlot) < FdTop &&
	LS.Fd != NOFD && LS.MsgPort == MsgPort) {
	
	MsgPort->RemoteData.FdSlot = NOSLOT;
	LS.MsgPort = (msgport *)0;
    }
}

