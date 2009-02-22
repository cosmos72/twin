/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#ifndef _TWIN_EXTREG_H
#define _TWIN_EXTREG_H

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
	byte (*DecodeExtension)(topaque *len, CONST byte **data, topaque *args_n, tsfield args);
	void (*MultiplexS)(uldat order, topaque args_n, tsfield args);
	tany (*MultiplexL)(uldat order, ...);
    } Socket;
    struct {
	window (*Open)(CONST byte *arg0, byte * CONST * argv);
    } Term;
};

extern exts Exts;

byte   Register_Ext(void **where, void *data);
byte UnRegister_Ext(void **where, void *data);

#define   RegisterExt(kind, member, data) \
          Register_Ext((void **)&(Exts.kind.member), (void *)data)
#define UnRegisterExt(kind, member, data) \
        UnRegister_Ext((void **)&(Exts.kind.member), (void *)data)

#define   Ext(kind,member) (Exts.kind.member)

#endif /* _TWIN_EXTREG_H */

