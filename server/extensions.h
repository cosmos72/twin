
#ifndef _TW_EXTENSIONS_H
#define _TW_EXTENSIONS_H

typedef struct exts exts;

struct exts {
    struct {
	msgport MsgPort;
    } WM;
    struct {
	void (*KillSlot)(uldat slot);
    } Remote;
    struct {
	void (*SendMsg)(msgport MsgPort, msg Msg);
    } Socket;
    struct {
	void (*SendMsg)(msgport MsgPort, msg Msg);
    } SocketAlien;
    struct {
	window (*Open)(CONST byte *arg0, byte * CONST * argv);
    } Term;
};

extern exts Exts;

byte   RegisterExt(void **where, void *data);
byte UnRegisterExt(void **where, void *data);

#define   RegisterExtension(kind, member, data) \
          RegisterExt((void **)&(Exts.kind.member), (void *)data)
#define UnRegisterExtension(kind, member, data) \
        UnRegisterExt((void **)&(Exts.kind.member), (void *)data)

#define   Ext(kind,member) (Exts.kind.member)

#endif /* _TW_EXTENSIONS_H */

