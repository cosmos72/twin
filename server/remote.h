#ifndef _TW_REMOTE_H
#define _TW_REMOTE_H

uldat	RegisterRemoteFd(int Fd, void (*HandlerIO)(int Fd, uldat Slot));
uldat	RegisterRemote(int Fd, void *Private, void (*HandlerIO)(int Fd, void *Private));
void  UnRegisterRemote(uldat Slot);
byte	RegisterWindowFdIO(window *Window, void (*HandlerIO)(int Fd, window *Window));
void  UnRegisterWindowFdIO(window *Window);
uldat	RemoteWriteQueue(uldat Slot, uldat len, void *data);
#define	RemoteWindowWriteQueue(Window, len, data) RemoteWriteQueue((Window)->RemoteData.FdSlot, (len), (data))
byte	RemoteFlush(uldat Slot);
#define	RemoteWindowFlush(Window) RemoteFlush((Window)->RemoteData.FdSlot)

msgport *RemoteGetMsgPort(uldat Slot);

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

void   RegisterMsgPort(msgport *MsgPort, uldat Slot);
void UnRegisterMsgPort(msgport *MsgPort);

extern void (*KillSlot)(uldat slot);
extern void (*SocketSendMsg)(msgport *MsgPort, msg *Msg);

#endif /* _TW_REMOTE_H */
