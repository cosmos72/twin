#ifndef _TW_REMOTE_H
#define _TW_REMOTE_H

uldat	RegisterRemote(int Fd, void (*HandlerIO)(int Fd, uldat Slot));
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

extern uldat FdWQueued;

void   RegisterMsgPort(msgport *MsgPort, uldat Slot);
void UnRegisterMsgPort(msgport *MsgPort);

extern void (*KillSlot)(uldat slot);

#endif /* _TW_REMOTE_H */
