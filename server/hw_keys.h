
/*
 *             W A R N I N G  :
 *
 * the ORDER of the IS() macros in this file is CAREFULLY chosen
 * so that the corresponding XK_* KeySym (from <X11/keysymdef.h>)
 * have continuously GROWING values (i.e. are sorted).
 *
 * If you need to add more entries in this file,
 * look at the include file <X11/keysymdef.h> and put new entries
 * at the exact position to preserve sorting.
 *
 * If you mess up this file, compiling `twin' with X11 support
 * will result in a non-working executable.
 *
 */

IS(BackSpace,	1, "\x7F")
IS(Tab,		1, "\x09")
IS(Linefeed,	1, "\x0A")
IS(Clear,	1, "\x0B")
IS(Return,	1, "\x0D")
IS(Pause,	3, "\x1B[P")
IS(Scroll_Lock,	0, "")
#ifdef XK_Sys_Req
IS(Sys_Req,	0, "")
#endif
IS(Escape,	1, "\x1B")


IS(Home,	4, "\x1B[1~")
IS(Left,	3, "\x1B[D")
IS(Up,		3, "\x1B[A")
IS(Right,	3, "\x1B[C")
IS(Down,	3, "\x1B[B")
IS(Prior,	4, "\x1B[5~")
IS(Next,	4, "\x1B[6~")
IS(End,		4, "\x1B[4~")
IS(Begin,	4, "\x1B[1~") /* as Home */
IS(Select,	0, "")
IS(Print,	0, "")
IS(Execute,	0, "")

IS(Insert,	4, "\x1B[2~")
IS(Undo,	0, "")
IS(Redo,	0, "")
IS(Menu,	0, "")
IS(Find,	0, "")
IS(Cancel,	0, "")
IS(Help,	0, "")
IS(Break,	0, "")
/*IS(Mode_switch,0, "")*/     /* I don't want to receive this, it's AltGr */
IS(Num_Lock,	3, "\x1BOP")

IS(KP_Space,	1, " ")
IS(KP_Tab,	1, "\x09")
IS(KP_Enter,	3, "\x1BOM")

IS(KP_F1,	4, "\x1B[[A") /* as F1 */
IS(KP_F2,	4, "\x1B[[B") /* as F2 */
IS(KP_F3,	4, "\x1B[[C") /* as F3 */
IS(KP_F4,	4, "\x1B[[D") /* as F4 */
/* these are duplicates of KP_[0..9] */
#if 0
IS(KP_Home,	3, "\x1BOw")
IS(KP_Left,	3, "\x1BOt")
IS(KP_Up,	3, "\x1BOx")
IS(KP_Right,	3, "\x1BOv")
IS(KP_Down,	3, "\x1BOr")
IS(KP_Prior,	3, "\x1BOy")
IS(KP_Next,	3, "\x1BOs")
IS(KP_End,	3, "\x1BOq")
IS(KP_Begin,	3, "\x1BOw") /* as KP_Home */
IS(KP_Insert,	3, "\x1BOp")
#endif

#ifdef XK_KP_Delete
IS(KP_Delete,	3, "\x1BOn")
#endif
IS(KP_Multiply,	3, "\x1BOR")
IS(KP_Add,	3, "\x1BOl")
IS(KP_Separator,3, "\x1BOm")
IS(KP_Subtract,	3, "\x1BOS")
IS(KP_Decimal,	3, "\x1BOn") /* as KP_Delete */
IS(KP_Divide,	3, "\x1BOQ")

IS(KP_0,	3, "\x1BOp")
IS(KP_1,	3, "\x1BOq")
IS(KP_2,	3, "\x1BOr")
IS(KP_3,	3, "\x1BOs")
IS(KP_4,	3, "\x1BOt")
IS(KP_5,	3, "\x1BOu")
IS(KP_6,	3, "\x1BOv")
IS(KP_7,	3, "\x1BOw")
IS(KP_8,	3, "\x1BOx")
IS(KP_9,	3, "\x1BOy")

IS(KP_Equal,	3, "\x1BOo")

IS(F1,		4, "\x1B[[A")
IS(F2,		4, "\x1B[[B")
IS(F3,		4, "\x1B[[C")
IS(F4,		4, "\x1B[[D")
IS(F5,		4, "\x1B[[E")
IS(F6,		5, "\x1B[17~")
IS(F7,		5, "\x1B[18~")
IS(F8,		5, "\x1B[19~")
IS(F9,		5, "\x1B[20~")
IS(F10,		5, "\x1B[21~")
IS(F11,		5, "\x1B[23~")
IS(F12,		5, "\x1B[24~")

IS(Delete,	4, "\x1B[3~")

