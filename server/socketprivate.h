#ifndef _TW_SOCKET_PRIVATE_H
#define _TW_SOCKET_PRIVATE_H

byte *RemoteReadGrowQueue(uldat Slot, uldat len);
uldat RemoteReadShrinkQueue(uldat Slot, uldat len);
byte *RemoteReadGetQueue(uldat Slot, uldat *len);
uldat RemoteReadDeQueue(uldat Slot, uldat len);
byte *RemoteWriteGetQueue(uldat Slot, uldat *len);

byte RemoteGzip(uldat Slot);
byte RemoteGunzip(uldat Slot);

void SocketAlienPop(CONST byte **src, uldat alien_len, byte *dst, uldat len);
void SocketAlienPush(CONST byte *src, uldat len, byte **dst, uldat alien_len);

#define AlienPop(s,type,lval)		SocketAlienPop((CONST byte **)&(s),AlienSizeof(type,Slot),(byte *)&(lval),sizeof(type))
#define AlienPush(s,type,lval)		SocketAlienPush((CONST byte *)&(lval),sizeof(type),&(s),AlienSizeof(type,Slot))

struct s_socket_f {
    uldat (*FindFunction)(byte Len, CONST byte *name);
    byte (*SyncSocket)(void);
    byte (*CanCompress)(void);
    byte (*DoCompress)(byte on_off);
    
    void (*NeedResizeDisplay)(void);    
    void (*AttachHW)(uldat len, CONST byte *arg, byte flags);
    byte (*DetachHW)(uldat len, CONST byte *arg);
    void (*SetFontTranslation)(CONST byte trans[0x80]);
    
    void (*DeleteObj)(void *V);
    
    widget (*CreateWidget)(dat XWidth, dat YWidth, hwattr Fill, dat Left, dat Up);    
    void (*RecursiveDeleteWidget)(void *V);
    msgport (*GetOwnerWidget)(widget W);
    
    gadget (*CreateGadget)
	(widget Parent, dat XWidth, dat YWidth, CONST byte *TextNormal, udat Code, udat Flags,
	 hwcol ColText, hwcol ColTextSelect, hwcol ColTextDisabled, hwcol ColTextSelectDisabled,
	 dat Left, dat Up,       CONST byte *TextSelect, CONST byte *TextDisabled, CONST byte *TextSelectDisabled,
	 CONST hwcol *ColNormal, CONST hwcol *ColSelect, CONST hwcol *ColDisabled, CONST hwcol *ColSelectDisabled);
    void (*SetPressedGadget)(gadget G, byte on_off);
    byte (*IsPressedGadget)(gadget G);
    void (*SetToggleGadget)(gadget G, byte on_off);
    byte (*IsToggleGadget)(gadget G);
    
    void (*ResizeWindow)(window Window, dat X, dat Y);
    
    msgport (*CreateMsgPort)(byte LenTitle, CONST byte *Title, time_t WakeUp, frac_t WakeUpFrac, byte Flags);
    msgport (*FindMsgPort)(msgport Prev, byte LenTitle, CONST byte *Title);
    
    screen (*FirstScreen)(void);
    widget (*FirstWidget)(widget W);
    msgport (*FirstMsgPort)(void);
    menu (*FirstMenu)(msgport MsgPort);
    menuitem (*FirstMenuItem)(menu Menu);

    obj (*PrevObj)(obj Obj);
    obj (*NextObj)(obj Obj);
    obj (*ParentObj)(obj Obj);
    
    dat (*GetDisplayWidth)(void);
    dat (*GetDisplayHeight)(void);

    byte (*SendToMsgPort)(msgport MsgPort, udat Len, CONST byte *Data);
    void (*BlindSendToMsgPort)(msgport MsgPort, udat Len, CONST byte *Data);
    
    obj  (*GetOwnerSelection)(void);
    void (*SetOwnerSelection)(time_t Time, frac_t Frac);
    void (*NotifySelection)(obj Requestor, uldat ReqPrivate,
			       uldat Magic, CONST byte MIME[MAX_MIMELEN], uldat Len, CONST byte *Data);
    void (*RequestSelection)(obj Owner, uldat ReqPrivate);
};

typedef struct s_socket_f socket_f;

extern socket_f SocketF;

typedef void (*handler_io_s)(int, uldat);

#endif /* _TW_SOCKET_PRIVATE_H */
