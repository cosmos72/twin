#ifndef _TW_METHODS_H
#define _TW_METHODS_H

byte FindInfo(menu Menu, dat i);
window FakeOpenTerm(CONST byte *arg0, byte * CONST * argv);
window FakeKbdFocus(window W);
void FakeWriteAscii(window Window, ldat Len, CONST byte *Text);
void FakeWriteHWAttr(window Window, dat x, dat y, ldat Len, CONST hwattr *Attr);
byte FakeFindBorderWindow(window W, dat u, dat v, byte Border, byte *PtrChar, hwcol *PtrColor);

extern fn Fn;
#define FnObj		(Fn.f_obj)
#define FnGroup		(Fn.f_group)
#define	FnWidget	(Fn.f_widget)
#define	FnGadget	(Fn.f_gadget)
#define FnWindow	(Fn.f_window)
#define	FnScreen	(Fn.f_screen)
#define	FnRow		(Fn.f_row)
#define	FnMenuItem	(Fn.f_menuitem)
#define	FnMenu		(Fn.f_menu)
#define	FnMsgPort	(Fn.f_msgport)
#define	FnMutex		(Fn.f_mutex)
#define	FnMsg		(Fn.f_msg)
#define	FnModule	(Fn.f_module)
#define	FnDisplayHW	(Fn.f_display_hw)

#define Do(Command, ObjName)	(Fn##ObjName->Command)

#define Act(Command, Obj)	((Obj)->Fn->Command)

#define Delete(Obj)		Act(Delete, Obj)(Obj)

#define DeleteList(First)	while (First) Delete(First)

#define InsertOnly(ObjName, Obj, Parent) \
	Act(Insert, (Obj))((Obj), (Parent), NULL, NULL)

#define InsertFirst(ObjName, Obj, Parent) \
	Act(Insert, (Obj))((Obj), (Parent), NULL, (Parent)->First##ObjName)

#define InsertMiddle(ObjName, Obj, Parent, ObjPrev, ObjNext) \
	Act(Insert, (Obj))((Obj), (Parent), (ObjPrev), (ObjNext))

#define InsertLast(ObjName, Obj, Parent) \
	Act(Insert, (Obj))((Obj), (Parent), (Parent)->Last##ObjName, NULL)

#define Remove(Obj) \
	Act(Remove, (Obj))(Obj)

#define MoveFirst(ObjName, Parent, Obj) \
	(Remove(Obj), InsertFirst(ObjName, (Obj), (Parent)))

#define MoveLast(ObjName, Parent, Obj) \
	(Remove(Obj), InsertLast(ObjName, (Obj), (Parent)))

#define SendMsg(MsgPort, WhichMsg) \
	(InsertLast(Msg, (WhichMsg), (MsgPort)))

#define Info4Menu(Menu, Flags, Len, Text, ColText) \
	Act(SetInfo,(Menu))((Menu), (Flags), (Len), (Text), (ColText))

#define Win4Menu(Menu) \
	Do(Create4Menu,Window)(FnWindow, (Menu))

#define Row4Menu(Window, Code, Flags, Len, Text) \
	Do(Create4Menu,Row)(FnRow, (Window), (Code), (Flags), (Len), (Text))

#define Item4Menu(Menu, Window, Flag, Len, Name) \
	Do(Create4Menu,MenuItem)(FnMenuItem, (Menu), (Window), (Flag), (Len), (Name))

#define Item4MenuCommon(Menu) \
	Do(Create4MenuCommon,MenuItem)(FnMenuItem, (Menu))

void *OverrideMth(void **where, void *OldMethod, void *NewMethod);

#define OverrideMethod(ObjName, Command, ExpectedMethod, NewMethod) \
          OverrideMth((void **)&(Fn##ObjName->Command), (void *)ExpectedMethod, (void *)NewMethod)


#endif /* _TW_METHODS_H */

