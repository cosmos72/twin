#ifndef _TW_METHODS_H
#define _TW_METHODS_H

extern fn Fn;
#define FnObj		(Fn.f_obj)
#define	FnArea		(Fn.f_area)
#define	FnAreaWin	(Fn.f_area_win)
#define	FnRow		(Fn.f_row)
#define	FnGadget	(Fn.f_gadget)
#define FnWindow	(Fn.f_window)
#define	FnMenuItem	(Fn.f_menuitem)
#define	FnMenu		(Fn.f_menu)
#define	FnScreen	(Fn.f_screen)
#define	FnMsg		(Fn.f_msg)
#define	FnMsgPort	(Fn.f_msgport)
#define	FnMutex		(Fn.f_mutex)
#define	FnModule	(Fn.f_module)

obj  *Clone(obj *Obj);
void  CloneList(obj *FirstFrom, obj **FirstTo, obj **LastTo);

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

#define Item4Menu(Menu, Flag, Len, Name, Window) \
	Do(Create4Menu,MenuItem)(FnMenuItem, (Menu), (Flag), (Len), (Name), (Window))

#define Item4MenuCommon(Menu) \
	Do(Create4MenuCommon,MenuItem)(FnMenuItem, (Menu))

#endif /* _TW_METHODS_H */

