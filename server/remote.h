/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef _TWIN_REMOTE_H
#define _TWIN_REMOTE_H

uldat	RegisterRemoteFd(int Fd, void (*HandlerIO)(int Fd, uldat Slot));
uldat	RegisterRemote(int Fd, obj HandlerData, void *HandlerIO); /* (void (*HandlerIO))(int Fd, obj HandlerData) */
void  UnRegisterRemote(uldat Slot);
byte	RegisterWindowFdIO(window Window, void (*HandlerIO)(int Fd, window Window));
void  UnRegisterWindowFdIO(window Window);
uldat	RemoteWriteQueue(uldat Slot, uldat len, CONST void *data);
#define	RemoteWindowWriteQueue(Window, len, data) RemoteWriteQueue((Window)->RemoteData.FdSlot, (len), (data))
byte	RemoteFlush(uldat Slot);
#define	RemoteWindowFlush(Window) RemoteFlush((Window)->RemoteData.FdSlot)

msgport RemoteGetMsgPort(uldat Slot);

void RemoteFlushAll(void);
void RemoteEvent(int FdNum, fd_set *FdSet);
void RemoteParanoia(void);

/*
 * call this if you couldn't write all pending data to a slot
 * which DOES NOT use the RemoteWriteQueue mechanism
 * (i.e. the slot of a display or something like that)
 */
void RemoteCouldntWrite(uldat Slot);
/*
 * if you called RemoteCouldntWrite(),
 * you *MUST* call RemoteCouldWrite() after you succeed
 * in writing all pending data
 */
void RemoteCouldWrite(uldat Slot);

extern uldat FdWQueued;

void   RegisterMsgPort(msgport MsgPort, uldat Slot);
void UnRegisterMsgPort(msgport MsgPort);

void remoteKillSlot(uldat slot);

#endif /* _TWIN_REMOTE_H */
