
#ifndef _TWIN_EXTENSIONS_H
#define _TWIN_EXTENSIONS_H

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
	byte (*InitAuth)(void);
    } Socket;
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

#endif /* _TWIN_EXTENSIONS_H */

