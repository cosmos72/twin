/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_REMOTE_H
#define TWIN_REMOTE_H

typedef void (*handler_io)(int Fd, uldat Slot);
typedef void (*handler_obj)(int Fd, obj HandlerObj);
typedef void (*handler_window)(int Fd, window Window);

uldat RegisterRemoteFd(int Fd, handler_io HandlerIO);
uldat RegisterRemote(int Fd, obj HandlerData, handler_obj HandlerObj);
void UnRegisterRemote(uldat Slot);
byte RegisterWindowFdIO(window Window, handler_window HandlerWindow);
void UnRegisterWindowFdIO(window Window);
uldat RemoteWriteQueue(uldat Slot, uldat len, const void *data);
#define RemoteWindowWriteQueue(Window, len, data)                                                  \
  RemoteWriteQueue((Window)->RemoteData.FdSlot, (len), (data))
byte RemoteFlush(uldat Slot);
#define RemoteWindowFlush(Window) RemoteFlush((Window)->RemoteData.FdSlot)

Tmsgport RemoteGetMsgPort(uldat Slot);

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

void RegisterMsgPort(Tmsgport MsgPort, uldat Slot);
void UnRegisterMsgPort(Tmsgport MsgPort);

void remoteKillSlot(uldat slot);

#endif /* TWIN_REMOTE_H */
