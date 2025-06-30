/*
 *  rcparse.h  --  C back-end of ~/.config/twin/twinrc parser for twin
 *
 *  Copyright (C) 2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#ifndef TWIN_RCPARSE_H
#define TWIN_RCPARSE_H

/*
 * ~/.config/twin/twinrc syntax :
 *
 * AddScreen <screen name>
 *
 * AddToMenu {Default|Common} <name> (
 * "entry" <function>
 * ...
 * )
 *
 * # possible menus are:
 * # Default is the default menu
 * # Common is the common part of all menus
 *
 * AddToFunc <name> (
 * <when> <function>
 * ...
 * )
 *
 * Background <screen> <color> (
 * "string"
 * ...
 * )
 *
 * # set screen background
 *
 * Beep
 *
 * Border {Active|Inactive} <wildcard> (
 * "�ͻ"
 * "� �"
 * "�ͼ"
 * )
 *
 * Button <n> <shape> [+|-[<numeric pos>|Left|Right]]
 *
 * Center
 * # center window
 *
 * Close
 *
 * DeleteFunc <name>
 *
 * DeleteMenu {Default|Common} <itemname>
 *
 * DeleteButton <number>
 *
 * DeleteScreen <screen>
 *
 * Exec <shell command>
 *
 * ExecTty <shell command>
 *
 * Focus [On|Off|Toggle]
 *
 * GlobalFlags [[+|-]AltFont] [[+|-]CursorAlways] [[+|-]Blink] [[+|-]ScreenScroll]
 * [[+|-]TerminalsUtf8] [[+|-]MenuHide] [[+|-]MenuItems] [[+|-]Shadows] [Shadows <x> <y>]
 * [ButtonSelection <n>]
 *
 * Interactive <kind>
 * # Scroll, Move, Resize, screen, Menu
 *
 * Key "<keyname>" <context> <function>
 *
 * Kill
 *
 * Lower
 *
 * Maximize
 *
 * FullScreen
 *
 * Menu <menu name>
 * # pop-up the menu
 *
 * Mouse "<button>" <context> <function>
 *
 * Move [+|-]<x> [+|-]<y>
 * # move window
 *
 * MoveScreen [+|-]<x> [+|-]<y>
 * # move screen
 *
 * Next [function]
 * # same as Window +1 [function]
 *
 * Nop ["<string>"]
 * # void function; non-empty string inserts a linebreak in menu
 *
 * Pass
 * #also send key / mouse action to window
 *
 * Prev [function]
 * # same as Window -1 [function]
 *
 * Quit
 *
 * Read <file>
 *
 * Refresh
 *
 * Restart [<window manager name>]
 *
 * Resize [+|-]<x> [+|-]<y>
 * # resize window
 *
 * ResizeScreen [+|-]<x> [+|-]<y>
 *
 * Raise
 *
 * Roll [On|Off|Toggle]
 *
 * Scroll [+|-]<x> [+|-]<y>
 * # scrolls window
 *
 * SelStart
 * # start selection
 *
 * SelCont
 * # continue/extend selection
 *
 * SelEnd
 * # end selection
 *
 * Sleep <n>
 * # sleep <n> seconds
 *
 * Stderr "<string>"
 *
 * SendToScreen <screen>
 * # sends window to given screen
 *
 * Wait <window name>
 *
 * Window {[+|-]<n>|<window name>} [function]
 * # sets default window to operate on:
 * # Window <n> :
 * # n = 0 : re-get current focused window
 * # n > 0 : 1 = First Window, 2 = Second, etc.
 * #
 * # Window {+|-}<n>
 * # n = 0 : no-op
 * # n < 0 : -1 : Prev Window, -2 Prev Prev, etc.
 * # n > 0 : +1 : Next Window, +2 Next Next, etc.
 *
 *
 * WindowList
 */

#include "extreg.h"
#include "rcparse_tab.hpp"
#include "unaligned.h"
#include "log.h"
#include "printk.h" // printk_receive_fd()

ldat GlobalFlags[4];
ldat GlobalShadows[2];

static void yyerror(const char *s) {
  log(ERROR) << "twin: " << Chars::from_c(FILE_NAME) << ":" << LINE_NO << ": " << Chars::from_c(s)
             << "\n";
}

#define NEW() (node) my_malloc(sizeof(struct s_node))

static node ReverseList(node l) {
  node base = NULL, v;
  while (l) {
    v = l->next;
    l->next = base;
    base = l;
    l = v;
  }
  return base;
}

inline node MakeNode(str name) {
  node n = NEW();
  n->name = name; /* this is my_malloc()ed memory (done by rcparse.l) */
  return n;
}

inline node MakeBuiltinFunc(ldat id) {
  node n = NEW();
  n->id = id;
  return n;
}

/*
 * the code may try to add a NULL node to a list...
 * do nothing in that case
 * (it's a way to eat empty lines, they are coded as NULL nodes)
 */
inline node AddtoNodeList(node l, node n) {
  if (n) {
    n->next = l;
    return n;
  }
  return l;
}

static node MakeNodeBody(str name, node body, node *head) {
  node n = MakeNode(name);
  n->body = ReverseList(body);
  if (head)
    *head = AddtoNodeList(*head, n);
  return n;
}

static byte ImmAddScreen(str name) {
  MakeNodeBody(name, NULL, &ScreenList);
  return ttrue;
}

static void MergeNodeLists(node n, node l) {
  node ol = n->body;

  if (!ol)
    n->body = l;
  else if (l) {
    while (ol->next)
      ol = ol->next;
    ol->next = l;
  }
}

static byte MergeMenu(str name, node l) {
  node n;

  if ((n = LookupNodeName(name, MenuList)))
    MergeNodeLists(n, ReverseList(l));
  else
    MakeNodeBody(name, l, &MenuList);
  return ttrue;
}

static byte MergeFunc(str name, node l) {
  node n;

  if ((n = LookupNodeName(name, FuncList)))
    MergeNodeLists(n, ReverseList(l));
  else
    MakeNodeBody(name, l, &FuncList);
  return ttrue;
}

static byte ImmBackground(str name, tcell color, node shape) {
  node n;

  /* automagically create screen "1" if needed */
  if ((n = LookupNodeName(name, ScreenList)) ||
      (!strcmp(name, "1") && (n = MakeNodeBody(name, NULL, &ScreenList)))) {

    n->body = ReverseList(shape);
    n->x.color = color;
    return ttrue;
  }
  return tfalse;
}

static void UnwindBorderShape(node n) {
  cstr s = NULL;
  str d = n->bytes = (str)my_malloc(n->x.ctx = 10);
  node shape = n->body;

  while (shape) {
    s = shape->name;
    while (*s && (d - n->bytes) < 9)
      *d++ = *s++;
    if (!*s)
      shape = shape->next;
  }
  *d = '\0';
}

static byte ImmBorder(str wildcard, ldat flag, node shape) {
  node n;
  if (shape) {
    n = MakeNodeBody(wildcard, shape, &BorderList);
    n->x.f.flag = flag;
    UnwindBorderShape(n);
    n->body = NULL;
    return ttrue;
  }
  return tfalse;
}

static ldat FreeButtonPos(ldat n, ldat lr) {
  ldat i;
  ldat freepos = lr == FL_LEFT ? 0 : -2;

  for (i = 0; i < BUTTON_MAX; i++)
    if (All->ButtonVec[i].exists) {
      if (All->ButtonVec[i].pos >= 0 && freepos >= 0 && freepos < All->ButtonVec[i].pos + 2)
        freepos = All->ButtonVec[i].pos + 2;
      else if (All->ButtonVec[i].pos < 0 && freepos < 0 && freepos > All->ButtonVec[i].pos - 2)
        freepos = All->ButtonVec[i].pos - 2;
    }
  return freepos;
}

static byte ImmButton(ldat n, str shape, ldat lr, ldat flag, ldat pos) {
  if (n >= 0 && n < BUTTON_MAX && strlen(shape) >= 2) {
    All->ButtonVec[n].shape[0] = Tutf_CP437_to_UTF_32[(byte)shape[0]];
    All->ButtonVec[n].shape[1] = Tutf_CP437_to_UTF_32[(byte)shape[1]];
    if (lr == FL_RIGHT)
      pos = -pos;
    if (flag == '+' || flag == '-')
      /* relative position */
      pos += FreeButtonPos(n, lr);
    else {
      /* absolute position */
      if (lr == FL_RIGHT)
        pos -= 2;
      /* -2 is the position corresponding to `Right 0' */
    }
    All->ButtonVec[n].pos = pos;
    All->ButtonVec[n].exists = All->ButtonVec[n].changed = ttrue;
    return ttrue;
  }
  return tfalse;
}

static void DeleteNodeName(str name, node *l) {
  for (; *l; l = &((*l)->next)) {
    if (!strcmp(name, (*l)->name)) {
      *l = (*l)->next;
      return;
    }
  }
}

static byte ImmDeleteFunc(str name) {
  DeleteNodeName(name, &FuncList);
  return ttrue;
}

static byte ImmDeleteMenu(str name) {
  DeleteNodeName(name, &MenuList);
  return ttrue;
}

static byte ImmDeleteButton(ldat n) {
  if (n >= 0 && n < BUTTON_MAX)
    All->ButtonVec[n].exists = tfalse;
  return ttrue;
}

static byte ImmDeleteScreen(str name) {
  DeleteNodeName(name, &ScreenList);
  return ttrue;
}

static byte ImmGlobalFlags(node l) {
  ldat i, j;

  for (; l; l = l->next) {
    switch (l->id) {
    case ALTFONT: /*ignored for compatibility*/
      return ttrue;
    case BLINK: /*ignored for compatibility*/
      continue;
    case CURSOR_ALWAYS:
      i = setup_cursor_always;
      break;
    case SCREEN_SCROLL:
      i = setup_screen_scroll;
      break;
    case MENU_HIDE:
      i = setup_menu_hide;
      break;
    case MENU_INFO:
      i = setup_menu_info;
      break;
    case MENU_RELAX:
      i = setup_menu_relax;
      break;
    case SHADOWS:
      i = setup_shadows;
      break;
    case TERMINALS_UTF8:
      i = setup_terminals_utf8;
      break;
    case BUTTON_PASTE:
      i = 0;
      break;
    case BUTTON_SELECTION:
      i = -1;
      break;
    default:
      return tfalse;
    }
    if (i > 0) {
      switch (l->x.f.flag) {
      case FL_ON:
      case '+':
      case 0:
        GlobalFlags[0] |= i;  /* OR */
        GlobalFlags[1] &= ~i; /* XOR */
        break;
      case FL_OFF:
      case '-':
        GlobalFlags[0] |= i; /* OR */
        GlobalFlags[1] |= i; /* XOR */
        /* (x OR 1) XOR 1 == 0 */
        break;
      case FL_TOGGLE:
        GlobalFlags[0] &= ~i; /* OR */
        GlobalFlags[1] |= i;  /* XOR */
        /* x XOR 1 == ~x */
        break;
      case -(ldat)'+':
        if (l->id == SHADOWS) {
          GlobalShadows[0] = l->x.f.a;
          GlobalShadows[1] = l->x.f.b;
        }
        break;
      default:
        return tfalse;
      }
    } else {
      /* ButtonSelection or ButtonPaste */

      j = l->x.f.flag;
      if (j >= 1 && j <= BUTTON_N_MAX) {
        GlobalFlags[i + 3] = HOLD_CODE(j - 1);
      } else {
        GlobalFlags[i + 3] = HOLD_LEFT;
      }
    }
  }
  return ttrue;
}

static ldat BitmapCtx(str _ctx) {
  ldat res = 0;
  byte c;
  while ((c = *_ctx++))
    switch (c) {
    case 'a':
    case 'A':
      res |= CTX_ANY;
      break;
    case 'b':
    case 'B':
      res |= CTX_BARS;
      break;
    case 'c':
    case 'C':
      res |= CTX_CORNER;
      break;
    case 'i':
    case 'I':
      res |= CTX_INSIDE;
      break;
    case 'm':
    case 'M':
      res |= CTX_MENU;
      break;
    case 'r':
    case 'R':
      res |= CTX_ROOT;
      break;
    case 's':
    case 'S':
      res |= CTX_SIDE;
      break;
    case 't':
    case 'T':
      res |= CTX_TITLE;
      break;
    case 'w':
    case 'W':
      res |= CTX_WIN;
      break;
    default:
      if (c >= '0' && c <= '9')
        res |= 1 << (c - '0');
      break;
    }
  return res;
}

static node LookupBind(ldat label, ldat ctx, node l) {
  for (; l; l = l->next) {
    /* strict match here: (l->x.ctx == ctx) */
    if (label == l->id && l->x.ctx == ctx)
      return l;
  }
  return NULL;
}

static str toString(ldat i) {
  str s = (str)my_malloc(2 + 3 * sizeof(ldat));
  sprintf(s, "%d", (int)i);
  return s;
}

static ldat FindTwKey(str name) {
  uldat i;
  for (i = 0; TW_KeyList[i].name; i++) {
    if (!strcmp(name, TW_KeyList[i].name))
      return (ldat)i;
  }
  return -1;
}

static byte BindKey(ldat shiftflags, str label, node func) {
  ldat key;
  node n;

  switch (strlen(label)) {
  case 0:
    return tfalse;
  case 1:
    key = (byte)*label;
    break;
  default:
    if ((key = FindTwKey(label)) == -1)
      return tfalse;
    key = TW_KeyList[key].key;
    break;
  }

  if (!(n = LookupBind(key, shiftflags, KeyList))) {
    n = MakeBuiltinFunc(key);
    n->name = label;
    n->x.ctx = shiftflags;
    KeyList = AddtoNodeList(KeyList, n);
  }
  n->body = ReverseList(func);
  return ttrue;
}

static byte BindMouse(str buttons, str _ctx, node func) {
  ldat ctx = BitmapCtx(_ctx);
  udat buttonmask = 0;
  node n;
  byte c;

  while ((c = *buttons++)) {
    if (c >= '1' && c <= '1' + BUTTON_N_MAX)
      buttonmask |= HOLD_CODE(c - '1');
    else if (c == 'H' || c == 'h')
      buttonmask |= PRESS_;
    else if (c == 'C' || c == 'c')
      buttonmask |= RELEASE_;
  }
  if (buttonmask) {
    if (!(buttonmask & (PRESS_ | RELEASE_)))
      /* default is 'C' */
      buttonmask |= RELEASE_;

    if (!(n = LookupBind(buttonmask, ctx, MouseList))) {
      n = MakeBuiltinFunc(buttonmask);
      n->x.ctx = ctx;
      n->name = _ctx;
      MouseList = AddtoNodeList(MouseList, n);
    }
    n->body = ReverseList(func);
    return ttrue;
  }
  return tfalse;
}

static node MakeFlagNode(ldat id, ldat flag) {
  node n = MakeBuiltinFunc(id);
  n->x.f.flag = flag;
  return n;
}

static node MakeModuleNode(str label, ldat flag) {
  node n;
  ldat len = strlen(label);
  /* remove .so suffix */
  if (len > 3 && !memcmp(label + len - 3, ".so", 3))
    label[len - 3] = '\0';
  n = MakeNode(label);
  n->id = MODULE;
  n->x.f.flag = flag;
  n->x.f.a = -1; /* code for this module will be asked when needed */
  return n;
}

static node MakeShadowsNode(ldat x, ldat y) {
  node n = MakeBuiltinFunc(SHADOWS);
  n->x.f.flag = -(ldat)'+';
  n->x.f.a = x;
  n->x.f.b = y;
  return n;
}

static node MakeUserFunc(str name) {
  node n = MakeNode(name);
  n->id = USERFUNC;
  return n;
}

static void MakeArgVecReverse(node n, node l) {
  uldat len = 1;
  node base = NULL, v;
  cstr *s;

  if (!l)
    return;

  while (l) {
    v = l->next;
    l->next = base;
    base = l;
    l = v;
    len++;
  }
  l = base;

  n->x.v.argc = len;
  n->x.v.argv = s = (cstr *)my_malloc(len * sizeof(str));

  while (l) {
    *s++ = l->name;
    l = l->next;
  }
  *s = NULL;
}

static node MakeExec(node l) {
  node n = MakeBuiltinFunc(EXEC);
  MakeArgVecReverse(n, l);
  return n;
}

static node MakeExecTty(node l) {
  node n = MakeBuiltinFunc(EXECTTY);
  MakeArgVecReverse(n, l);
  return n;
}

static node MakeMoveResizeScroll(ldat id, ldat flag_x, ldat x, ldat flag_y, ldat y) {
  node n = MakeBuiltinFunc(id);
  n->x.f.plus_minus = flag_x;
  n->x.f.flag = flag_y;
  n->x.f.a = x;
  n->x.f.b = y;
  return n;
}

static node MakeRestartWM(str name) {
  node n = MakeNode(name);
  n->id = RESTART;
  return n;
}

static node MakeStderr(node l) {
  node n = MakeBuiltinFunc(STDERR);
  MakeArgVecReverse(n, l);
  return n;
}

static node MakeSendToScreen(str name) {
  node n = MakeNode(name);
  n->id = SENDTOSCREEN;
  return n;
}

static node MakeSyntheticKey(ldat shiftflags, str label) {
  ldat key;
  const char *seq;
  char buf[4];
  node n;

  switch (strlen(label)) {
  case 0:
    return NULL;

  case 1:
    key = (byte)*label;
    seq = label;
    break;

  default:
    if ((key = FindTwKey(label)) == -1)
      return NULL;
    seq = TW_KeyList[key].seq;
    key = TW_KeyList[key].key;
    break;
  }

  if (strlen(seq) == 1) {
    if (shiftflags & KBD_ALT_FL) {
      buf[0] = '\x1B';
      buf[1] = *seq;
      buf[2] = '\0';
      seq = buf;
    } else if (shiftflags & KBD_CTRL_FL) {
      buf[0] = *seq & 0x1F;
      buf[1] = '\0';
      seq = buf;
    }
  }

  n = NEW();
  n->id = SYNTHETICKEY;
  n->x.f.flag = shiftflags;
  n->x.f.a = key;
  n->name = my_strdup(seq);
  return n;
}

static node MakeSleep(ldat t) {
  node n = NEW();
  n->id = SLEEP;
  n->x.f.a = t;
  return n;
}

static node MakeWait(str name) {
  node n = MakeNode(name);
  n->id = WAIT;
  return n;
}

static node MakeWindowNumber(ldat flag, ldat x) {
  node n = MakeBuiltinFunc(WINDOW);
  n->x.f.plus_minus = flag;
  n->x.f.a = x;
  return n;
}

static node MakeWindow(str name) {
  node n = MakeBuiltinFunc(WINDOW);
  n->name = name;
  return n;
}

static node AddtoStringList(node l, str string) {
  return AddtoNodeList(l, MakeNode(string));
}

#if defined(DEBUG_YACC) || defined(DEBUG_RC)

static str TokenName(ldat id) {
  switch (id) {
  case tblack:
    return "tblack";
  case tblue:
    return "tblue";
  case tgreen:
    return "tgreen";

  case '+':
    return "+";
  case '-':
    return "-";
  case ADDSCREEN:
    return "AddScreen";
  case ADDTOMENU:
    return "AddToMenu";
  case ADDTOFUNC:
    return "AddToFunc";
  case BACKGROUND:
    return "Background";
  case BORDER:
    return "Border";
  case BUTTON:
    return "Button";
  case DELETEFUNC:
    return "DeleteFunc";
  case DELETEMENU:
    return "DeleteMenu";
  case DELETEBUTTON:
    return "DeleteButton";
  case DELETESCREEN:
    return "DeleteScreen";
  case EXEC:
    return "Exec";
  case EXECTTY:
    return "ExecTty";
  case GLOBALFLAGS:
    return "GlobalFlags";
  case INTERACTIVE:
    return "Interactive";
  case KEY:
    return "Key";
  case MENU:
    return "Menu";
  case MOUSE:
    return "Mouse";
  case MOVE:
    return "Move";
  case MOVESCREEN:
    return "MoveScreen";
  case NEXT:
    return "Next";
  case NOP:
    return "Nop";
  case PREV:
    return "Prev";
  case READ:
    return "Read";
  case RESTART:
    return "Restart";
  case RESIZE:
    return "Resize";
  case RESIZESCREEN:
    return "ResizeScreen";
  case SCREEN:
    return "screen";
  case SCROLL:
    return "Scroll";
  case SENDTOSCREEN:
    return "SendToScreen";
  case SLEEP:
    return "Sleep";
  case STDERR:
    return "Stderr";
  case SYNTHETICKEY:
    return "SyntheticKey";
  case WAIT:
    return "Wait";
  case WINDOW:
    return "Window";
  case BEEP:
    return "Beep";
  case CENTER:
    return "Center";
  case CLOSE:
    return "Close";
  case COPY:
    return "Copy";
  case KILL:
    return "Kill";
  case PASTE:
    return "Paste";
  case QUIT:
    return "Quit";
  case REFRESH:
    return "Refresh";
  case WINDOWLIST:
    return "WindowList";
  case FOCUS:
    return "Focus";
  case LOWER:
    return "Lower";
  case MAXIMIZE:
    return "Maximize";
  case FULLSCREEN:
    return "FullScreen";
  case RAISE:
    return "Raise";
  case RAISELOWER:
    return "RaiseLower";
  case ROLL:
    return "Roll";
  case USERFUNC:
    return "";
  case FL_ON:
    return "On";
  case FL_OFF:
    return "Off";
  case FL_TOGGLE:
    return "Toggle";
  case FL_ACTIVE:
    return "Active";
  case FL_INACTIVE:
    return "Inactive";
  case FL_LEFT:
    return "Left";
  case FL_RIGHT:
    return "Right";
  case ALTFONT:
    return "AltFont";
  case BLINK:
    return "Blink";
  case CURSOR_ALWAYS:
    return "CursorAlways";
  case MENU_HIDE:
    return "MenuHide";
  case MENU_INFO:
    return "MenuInfo";
  case MENU_RELAX:
    return "MenuRelax";
  case SCREEN_SCROLL:
    return "ScreenScroll";
  case SHADOWS:
    return "Shadows";
  case BUTTON_PASTE:
    return "ButtonPaste";
  case BUTTON_SELECTION:
    return "ButtonSelection";
  default:
    break;
  }
  return "(unknown)";
}
#endif

#ifdef DEBUG_RC

static str ColorName(tcolor col) {
  switch (col) {
  case tblack:
    return "Black";
  case tblue:
    return "Blue";
  case tgreen:
    return "Green";
  case tcyan:
    return "Cyan";
  case tred:
    return "Red";
  case tmagenta:
    return "Magenta";
  case tyellow:
    return "Yellow";
  case twhite:
    return "White";
  case thigh:
    return "High";
  default:
    break;
  }
  return "(unknown)";
}

static void DumpColorName(tcolor col) {
  tcolor fg = TCOLFG(col), bg = TCOLBG(col);

  if (fg & thigh)
    fprintf(stderr, "%s ", ColorName(thigh));
  fprintf(stderr, "%s %s ", ColorName(fg & ~thigh), TokenName(FL_ON));
  if (bg & thigh)
    fprintf(stderr, "%s ", ColorName(thigh));
  fprintf(stderr, "%s ", ColorName(bg & ~thigh));
}

static void DumpNameList(node l, byte nl) {
  for (; l; l = l->next) {
    if (l->name)
      fprintf(stderr, "\"%s\"%c", l->name, nl ? '\n' : ' ');
  }
}

static void DumpGenericNode(node n) {
  ldat f, x;

  if (n) {
    if (n->id)
      fprintf(stderr, "%s ", TokenName(n->id));
    if (n->name)
      fprintf(stderr, "\"%s\" ", n->name);
    if ((f = n->x.f.plus_minus))
      fprintf(stderr, "%s", TokenName(f));
    if ((x = n->x.f.a) || f == '+' || f == '-')
      fprintf(stderr, "%d ", x);
    if ((f = n->x.f.flag))
      fprintf(stderr, "%s", TokenName(f));
    if ((x = n->x.f.b) || f == '+' || f == '-')
      fprintf(stderr, "%d ", x);
    if (n->body)
      DumpGenericNode(n->body);
  }
}

static void DumpPlusList(node l) {
  for (; l; l = l->next) {
    if (l->name)
      fprintf(stderr, "\"%s\" ", l->name);
    DumpGenericNode(l->body);
    fprintf(stderr, "\n");
  }
}

static void DumpFuncNode(node n) {
  if (!n)
    return;

  fprintf(stderr, "%s \"%s\" (\n", TokenName(ADDTOFUNC), n->name);
  DumpPlusList(n->body);
  fprintf(stderr, ")\n");
}

static void DumpMenuNode(node n) {
  if (!n)
    return;

  fprintf(stderr, "%s \"%s\" (\n", TokenName(ADDTOMENU), n->name);
  DumpPlusList(n->body);
  fprintf(stderr, ")\n");
}

static void DumpScreenNode(node n) {
  if (!n)
    return;

  fprintf(stderr, "%s \"%s\"\n%s \"%s\" ", TokenName(ADDSCREEN), n->name, TokenName(BACKGROUND),
          n->name);
  DumpColorName(n->x.color);
  fprintf(stderr, "(\n");
  DumpNameList(n->body, ttrue);
  fprintf(stderr, ")\n");
}

static void DumpBorderNode(node n) {
  if (!n)
    return;

  fprintf(stderr,
          "%s \"%s\" %s (\n"
          "\"%.3s\"\n"
          "\"%.3s\"\n"
          "\"%.3s\"\n"
          ")\n",
          TokenName(BORDER), n->name, TokenName(n->x.f.flag), n->data, n->data + 3, n->data + 6);
}

static void DumpKeyNode(node n) {
  if (!n)
    return;
  fprintf(stderr, "%s %s ", TokenName(KEY), n->name);
  DumpGenericNode(n->body);
  fprintf(stderr, "\n");
}

static void DumpMouseNode(node n) {
  byte buttons[BUTTON_N_MAX + 1], *b = buttons;
  udat i;

  if (!n)
    return;

  for (i = 0; i < BUTTON_N_MAX; i++) {
    if (n->id & HOLD_N(i))
      *b++ = '1' + i;
  }
  memset(b, '\0', BUTTON_N_MAX + 1 - (b - buttons));

  fprintf(stderr, "%s %s %s ", TokenName(MOUSE), buttons, n->name);
  DumpGenericNode(n->body);
  fprintf(stderr, "\n");
}

static void DumpGlobals(void) {
  node l;

  for (l = FuncList; l; l = l->next)
    DumpFuncNode(l);

  for (l = MenuList; l; l = l->next)
    DumpMenuNode(l);

  for (l = ScreenList; l; l = l->next)
    DumpScreenNode(l);

  for (l = BorderList; l; l = l->next)
    DumpBorderNode(l);

  for (l = KeyList; l; l = l->next)
    DumpKeyNode(l);

  for (l = MouseList; l; l = l->next)
    DumpMouseNode(l);

  for (l = CallList; l; l = l->next) {
    DumpGenericNode(l);
    fprintf(stderr, "\n");
  }
}

#endif /* DEBUG_RC */

/*
 * clear all global pointers before parsing ~/.config/twin/twinrc
 * (this happens in the child process)
 */
static void ClearGlobals(void) {
  memset(Globals, '\0', GLOBAL_MAX * sizeof(node));
  memset(All->ButtonVec, '\0', BUTTON_MAX * sizeof(button_vec));
  MenuBinds = NULL;
  MenuBindsMax = 0;
}

/*
 * copy all global pointers at the beginning of shm pool
 * to pass them to parent
 * (this happens in the child process)
 */
static void WriteGlobals(void) {
  node *g = Globals;
  void **M = (void **)shm_getbase();

  while (g < Globals + GLOBAL_MAX)
    *M++ = ReverseList(*g++);

  byte *m = reinterpret_cast<byte *>(M);

  CopyMem(All->ButtonVec, m, sizeof(All->ButtonVec));
  m += sizeof(All->ButtonVec);
  CopyMem(GlobalFlags, m, sizeof(GlobalFlags));
  m += sizeof(GlobalFlags);
  CopyMem(GlobalShadows, m, sizeof(GlobalShadows));
  m += sizeof(GlobalShadows);
}

static Tscreen FindNameInScreens(dat len, const char *name, Tscreen screen) {
  while (screen) {
    if (len == screen->NameLen && !memcmp(name, screen->Name, len)) {
      return screen;
    }
    screen = screen->NextScreen();
  }
  return NULL;
}

static node FindNameInList(uldat len, const char *name, node list) {
  while (list) {
    if (list->name && strlen(list->name) == len && !memcmp(name, list->name, len))
      return list;
    list = list->next;
  }
  return NULL;
}

static void DeleteScreens(Tscreen first) {
  Tscreen s = first, next;
  while (s) {
    next = s->NextScreen();
    s->Delete();
    s = next;
  }
}

/*
 * create new screens as needed or fail with no side effects
 */
static byte CreateNeededScreens(node list, Tscreen *res_Screens) {
  node body;
  Tscreen s, prev = (Tscreen)0, top = (Tscreen)0;
  cstr n;
  tcell *attr, *r;
  trune f;
  tcolor c;
  uldat w, h, len, _len;

  while (list) {
    w = h = 0;
    for (body = list->body; body; body = body->next) {
      len = body->name ? strlen(body->name) : 0;
      if (w < len)
        w = len;
      h++;
    }
    if (!w && !h)
      continue;

    if ((attr = (tcell *)AllocMem(w * h * sizeof(tcell)))) {
      h = 0;
      for (body = list->body; body; body = body->next) {
        if (body->name) {
          n = body->name;
          _len = len = strlen(n);
          c = list->x.color;
          r = attr + w * h;
          while (len--) {
            f = Tutf_CP437_to_UTF_32[(byte)*n++];
            *r++ = TCELL(c, f);
          }
          while (_len++ < w)
            *r++ = TCELL(c, ' ');
        }
        h++;
      }
      s = New(screen)(strlen(list->name), list->name, w, h, attr);

      FreeMem(attr);
    }
    if (!attr || !s) {
      DeleteScreens(top);
      return tfalse;
    }
    if (prev) {
      prev->SetNextScreen(s);
    }
    prev = s;
    if (!top) {
      top = s;
    }
    list = list->next;
  }
  *res_Screens = top;
  return ttrue;
}

/*
 * make screens in new_Screens visible if they don't exist,
 * otherwise copy their background then delete them.
 */
static void UpdateVisibleScreens(Tscreen new_Screens) {
  Tscreen screen, next, orig;
  for (screen = new_Screens; screen; screen = next) {
    next = screen->NextScreen();
    screen->SetNextScreen((Tscreen)0);

    if ((orig = FindNameInScreens(screen->NameLen, screen->Name, All->Screens.First))) {
      orig->USE.B.BgWidth = screen->USE.B.BgWidth;
      orig->USE.B.BgHeight = screen->USE.B.BgHeight;
      if (orig->USE.B.Bg) {
        FreeMem(orig->USE.B.Bg);
      }
      orig->USE.B.Bg = screen->USE.B.Bg;
      screen->USE.B.Bg = NULL;
      screen->Delete();
    } else {
      InsertLast(Screens, screen, All);
    }
    screen = next;
  }
}

/*
 * Delete no-longer needed screens (all except "1" and ones in list)
 */
static void DeleteUnneededScreens(node list) {
  Tscreen screen, next;
  for (screen = All->Screens.First; screen; screen = next) {
    next = screen->NextScreen();
    if ((screen->NameLen != 1 || screen->Name[0] != '1') &&
        !FindNameInList(screen->NameLen, screen->Name, list)) {
      screen->Delete();
    }
  }
}

static void NewCommonMenu_Overflow(void) {
  log(ERROR) << "twin: RC parser: user-defined menu is too big! (max is "
             << (int)(TW_MAXUDAT - COD_RESERVED + 1) << " entries)\n";
}

/*
 * create new CommonMenu and MenuBinds from MenuList
 * or fail with no side effects
 */
static byte NewCommonMenu(void *const *shm_M, Tmenu *res_CommonMenu, node **res_MenuBinds,
                          uldat *res_MenuBindsMax) {
  node new_MenuList;
  node *new_MenuBinds = (node *)0;
  uldat new_MenuBindsMax;

  Tmenu Menu = (Tmenu)0;
  Tmenuitem item;
  Twindow w;
  Trow Row;
  node N, M;
  uldat maxlen, l;
  str Line;

  /* extract needed length for new_MenuBinds[] */
  new_MenuBindsMax = 0;
  new_MenuList = (node)(*(shm_M + (&MenuList - Globals)));
  for (M = new_MenuList; M; M = M->next) {
    for (N = M->body; N; N = N->next) {
      if (N->body && N->body->id != NOP)
        new_MenuBindsMax++;
    }
  }

  if (new_MenuBindsMax > TW_MAXUDAT - COD_RESERVED) {
    NewCommonMenu_Overflow();
    return tfalse;
  }

  if (new_MenuBindsMax && !(new_MenuBinds = (node *)AllocMem(new_MenuBindsMax * sizeof(node))))
    return tfalse;

  new_MenuBindsMax = 0;
  new_MenuList = (node)(*(shm_M + (&MenuList - Globals)));

  if (!(Menu = New(menu)(Ext(WM, MsgPort), (tcolor)0, (tcolor)0, (tcolor)0, (tcolor)0, (tcolor)0,
                         (tcolor)0, ttrue)))
    return tfalse;

  /* ok, now create the CommonMenu. Fill new_MenuBinds[] as we proceed */

  for (M = new_MenuList; M; M = M->next) {
    if ((w = Win4Menu(Menu)) && (item = Item4Menu(Menu, w, ttrue, strlen(M->name), M->name))) {

      if (!item->PrevItem()) {
        item->Left = 0; /* remove padding */
      }
      maxlen = 0;

      for (N = M->body; N; N = N->next) {
        if (N->body && N->body->id != NOP) {
          l = strlen(N->name);
          maxlen = Max2(maxlen, l);
        }
      }

      if (!(Line = (str)AllocMem(maxlen + 1)))
        break;

      memset(Line, 0xC4, maxlen);
      Line[maxlen] = '\0'; /* not strictly necessary */

      for (N = M->body; N; N = N->next) {
        if (N->body && N->body->id != NOP) {
          if ((Row = Row4Menu(w, 0, ROW_ACTIVE, strlen(N->name), N->name))) {

            Row->Code = (udat)(new_MenuBindsMax + COD_RESERVED);
            new_MenuBinds[new_MenuBindsMax++] = N->body;
          }
        } else if (N->name && N->name[0]) {
          Row = Row4Menu(w, 0, ROW_INACTIVE, strlen(N->name), N->name);
        } else {
          Row = Row4Menu(w, 0, ROW_IGNORE, maxlen, Line);
        }
        if (!Row)
          break;
      }
      FreeMem(Line);

      if (N) /* out of memory! */
        break;
    }
  }
  if (!M) {
    *res_CommonMenu = Menu;
    *res_MenuBinds = new_MenuBinds;
    *res_MenuBindsMax = new_MenuBindsMax;
    return ttrue;
  }

  /* out of memory! */
  FreeMem(new_MenuBinds);
  Menu->Delete();
  return tfalse;
}

/*
 * get all global pointers from the beginning of shm pool, create
 * new Screens, Common Menu and MenuBinds or fail with no side effect.
 */
static byte ReadGlobals(void) {
  node *g;
  void *const *M = (void *const *)shm_getbase();
  Tmenu new_CommonMenu;
  node *new_MenuBinds = (node *)0;
  uldat new_MenuBindsMax;
  Tscreen new_Screens = (Tscreen)0;

  if (!CreateNeededScreens((node)M[ScreenIndex], &new_Screens))
    return tfalse;

  if (!NewCommonMenu(M, &new_CommonMenu, &new_MenuBinds, &new_MenuBindsMax)) {
    DeleteScreens(new_Screens);
    return tfalse;
  }

  /* ok, this is the no-return point. we must succeed now */

  UpdateVisibleScreens(new_Screens);
  DeleteUnneededScreens((node)M[ScreenIndex]);

  if (!GlobalsAreStatic)
    FreeMem(MenuBinds);
  GlobalsAreStatic = tfalse;

  if (All->CommonMenu) {
    All->CommonMenu->Delete();
  }
  All->CommonMenu = new_CommonMenu;
  MenuBinds = new_MenuBinds;
  MenuBindsMax = new_MenuBindsMax;

  g = Globals;
  while (g < Globals + GLOBAL_MAX)
    *g++ = reinterpret_cast<node>(*M++);

  const byte *m = reinterpret_cast<const byte *>(M);

  CopyMem(m, All->ButtonVec, sizeof(All->ButtonVec));
  m += sizeof(All->ButtonVec);
  CopyMem(m, GlobalFlags, sizeof(GlobalFlags));
  m += sizeof(GlobalFlags);
  CopyMem(m, GlobalShadows, sizeof(GlobalShadows));
  m += sizeof(GlobalShadows);

#ifdef DEBUG_RC
  DumpGlobals();
#endif

  All->SetUp->Flags |= GlobalFlags[0];
  All->SetUp->Flags ^= GlobalFlags[1];
  All->SetUp->ButtonSelection = GlobalFlags[2];
  All->SetUp->ButtonPaste = GlobalFlags[3];
  All->SetUp->DeltaXShade = GlobalShadows[0];
  All->SetUp->DeltaYShade = GlobalShadows[1];

  shm_TSR();

  QueuedDrawArea2FullScreen = true;

  return ttrue;
}

static byte rcparse(cstr path);

static bool rcload(Tdisplay hw) {
  str path;
  uldat len;
#ifndef DEBUG_FORK
  int fdm[2];
  int fdl[2];
#endif
  bool c = false;

  if (!(path = FindConfigFile("twinrc", &len)))
    return c;

  /*
   * try to guess a reasonable size:
   * assume a failsafe avg of a node every 4 bytes
   */
  len = Min2(len, TW_MAXULDAT / sizeof(node));
  len = Max2(len, TW_BIGBUFF) * sizeof(node) / 4;

  if (!shm_init(len)) {
    FreeMem(path);
    return c;
  }

#ifdef DEBUG_FORK
  /*
   * This works, but it would be too complex to add proper error recovery to it:
   * one would need to check that all shm_malloc() calls succeed, or otherwise
   * correctly cleanup and cleanly abort yacc/lex parsing.
   *
   * It is thus much easier and cleaner to do the work in a child process, assuming
   * in the rest of the code that shm_malloc() and yacc/lex parsing never fail,
   * and brutally exit() from shm_malloc() or yacc/lex parsing in case of errors.
   *
   * In this way the original process deals with an atomic fork()/rcparse() combo,
   * that either succeeds or fails with no side effect at all, except for the
   * shm area, which anyway must always be cleaned up, even in case of success.
   *
   * On the other hand, it is much easier to debug the code if there are no fork()s,
   * so this gets used in case of debugging.
   */
  if (rcparse(path)) {
    WriteGlobals();
    c = ReadGlobals();
  }
  if (!c) {
    ClearGlobals();
    shm_abort();
  }
  return c;
#else
  if (socketpair(AF_UNIX, SOCK_STREAM, 0, fdm) == 0) {
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fdl) == 0) {
      switch (fork()) {
      case -1: /* error */
        close(fdm[0]);
        close(fdm[1]);
        close(fdl[0]);
        close(fdl[1]);
        break;
      case 0: /* child */
        QuitSignals();
        close(fdm[0]);
        close(fdl[0]);
        if (fdl[1] != 2) {
          close(2);
          dup2(fdl[1], 2);
          close(fdl[1]);
        }
        ClearGlobals();
        if (rcparse(path)) {
          WriteGlobals();
          shm_send(fdm[1]);
        }
        exit(0);
        break;
      default: /* parent */
        FreeMem(path);
        close(fdm[1]);
        close(fdl[1]);
        printk_receive_fd(fdl[0]);

        if (shm_receive(fdm[0]) && shm_shrink())
          c = ReadGlobals();
        close(fdm[0]);
        close(fdl[0]);
        break;
      }
    } else {
      close(fdm[0]);
      close(fdm[1]);
    }
  }
  if (!c)
    shm_abort();
  return c;
#endif
}

EXTERN_C byte InitModule(Tmodule Module) {
  Module->DoInit = rcload;
  return ttrue;
}

EXTERN_C void QuitModule(Tmodule Module) {
}

#endif /* TWIN_RCPARSE_H */
