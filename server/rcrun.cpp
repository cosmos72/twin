/*
 *  rcrun.c  --  virtual machine to run byte-compiled configuration file ~/.config/twin/twinrc
 *
 *  Copyright (C) 2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "twin.h"
#include "main.h"
#include "alloc.h"
#include "builtin.h"
#include "data.h"
#include "draw.h"
#include "obj/id.h" // Obj2Id()
#include "resize.h"
#include "util.h"
#include "tterm.h" // OpenTerm()
#include "wm.h"

#include "common.h"
#include "dl.h"
#include "extreg.h"
#include "hw.h"
#include "hw_multi.h"
#include "log.h"
#include "methods.h"
#include "unaligned.h"

#include "rctypes.h"
#include "rcparse_tab.hpp"
#include "rcrun.h"

#ifdef CONF_RCPARSE
#include "rcproto.h"
#endif

#include <Tw/Twkeys.h>
#include "hotkey.h"

#include <Tutf/Tutf.h>
#include <Tutf/Tutf_defs.h>

enum {
  MAX_RUNCYCLE = 1024, /* kill a queue after this number of steps */

  MAX_RUNSTACK = 28,
};

class run {
public:
  run *next;
  uldat W;     /* the current Twidget (id) */
  uldat depth; /* index of last used stack element;
                * stack[depth] is the current instruction */
  uldat cycle;
  union {
    timevalue WakeUp;
    cstr Name;
  } SW; /* what we are waiting for: sleep timeout or Twidget map */

  wm_ctx C; /* event that generated the run queue */
  node stack[MAX_RUNSTACK];
};

byte GlobalsAreStatic;
node Globals[GLOBAL_MAX];

node *MenuBinds; /* array of pointers to nodes inside MenuList */
udat MenuBindsMax;

static run *Run;         /* list of running queues */
static run *Sleep;       /* list of sleeping queues */
static run *Wait;        /* list of waiting-for-window queues */
static run *Interactive; /* list of waiting-for-interactive-op queues */

/* shell-like wildcard pattern matching */
static byte wildcard_match(cstr p, cstr q) {
  byte c;
  if (!q) {
    q = "";
  }
  for (;;) {
    switch (c = (byte)*p++) {
    case '\0':
      return *q == '\0';
      break;
    case '\\':
      if (*q++ != *p++)
        return tfalse;
      break;
    case '?':
      if (*q++ == '\0')
        return tfalse;
      break;
    case '*':
      c = (byte)*p;
      if (c != '\\' && c != '?' && c != '*' && c != '[') {
        while ((byte)*q != c) {
          if (*q++ == '\0')
            return tfalse;
        }
      }
      do {
        if (wildcard_match(p, q)) {
          return ttrue;
        }
      } while (*q++ != '\0');
      return tfalse;
    case '[': {
      cstr endp;
      byte invert = 0, found = 0;
      byte chr;

      endp = p;
      if (*endp == '!')
        endp++;
      for (;;) {
        if (*endp == '\0') {
          goto dft; /* no matching ] */
        }
        if (*endp == '\\') {
          endp++;
        }
        if (*++endp == ']') {
          break;
        }
      }
      if (*p == '!') {
        invert++;
        p++;
      }
      chr = *q++;
      c = *p++;
      do {
        if (c == '\\') {
          c = (byte)*p++;
        }
        if (*p == '-' && p[1] != ']') {
          p++;
          if (*p == '\\') {
            p++;
          }
          if (chr >= c && chr <= (byte)*p) {
            found = 1;
          }
          p++;
        } else {
          if (chr == c) {
            found = 1;
          }
        }
      } while ((c = (byte)*p++) != ']');

      if (found == invert) {
        return tfalse;
      }
      break;
    }
    dft:
    default:
      if ((byte)*q++ != c) {
        return tfalse;
      }
      break;
    }
  }
}

node LookupNodeName(cstr name, node head) {
  node l;
  if (name)
    for (l = head; l; l = l->next) {
      if (!strcmp(name, l->name))
        return l;
    }
  return NULL;
}

static run *ReverseRunList(run *l) {
  run *base = NULL, *v;
  while (l) {
    v = l->next;
    l->next = base;
    base = l;
    l = v;
  }
  return base;
}

static ldat Pos2Ctx(udat Pos) {
  switch (Pos) {
  case POS_TITLE:
  case POS_SIDE_UP:
    return CTX_TITLE;
  case POS_SIDE_LEFT:
  case POS_SIDE_RIGHT:
  case POS_SIDE_DOWN:
    return CTX_SIDE;
  case POS_BUTTON_RESIZE:
    return CTX_CORNER;

  case POS_X_BAR_BACK:
  case POS_X_BAR_FWD:
  case POS_X_TAB:
  case POS_X_ARROW_BACK:
  case POS_X_ARROW_FWD:
  case POS_Y_BAR_BACK:
  case POS_Y_BAR_FWD:
  case POS_Y_TAB:
  case POS_Y_ARROW_BACK:
  case POS_Y_ARROW_FWD:
    return CTX_BARS;

  case POS_INSIDE:
    return CTX_INSIDE;
  case POS_MENU:
    return CTX_MENU;

  case POS_BUTTON_SCREEN:
  case POS_ROOT:
    return CTX_ROOT;
  default:
    if (Pos < BUTTON_MAX)
      return CTX_BUTTON(Pos);
  }
  return 0;
}

static node RCFindKeyBind(ldat label, ldat shiftflags) {
  node l = KeyList;

  shiftflags &=
      ~(KBD_CAPS_LOCK | KBD_NUM_LOCK); /* ignore CapsLock and NumLock when looking for a keybind! */

  for (; l; l = l->next) {
    if (label == l->id && shiftflags == l->x.ctx)
      return l->body;
  }
  return NULL;
}

static node RCFindMouseBind(ldat code, ldat ctx) {
  node l = MouseList;
  ldat hc = code & (PRESS_ | RELEASE_);
  code &= HOLD_ANY;

  for (; l; l = l->next) {
    /* triple-inclusive match here:
     * (l->id & code)   : match buttons
     * (l->id & hc)     : match hold/click
     * (l->x.ctx & ctx) : match release context
     */
    if ((l->id & code) && (l->id & hc) && (l->x.ctx & ctx))
      return l;
  }
  return NULL;
}

trune *RCFindBorderPattern(Twindow w, byte border) {
  node l;

  if (!w) {
    return NULL;
  }
  for (l = BorderList; l; l = l->next) {
    if ((l->x.f.flag == FL_INACTIVE) == border) {
      String &winTitle = w->Name;
      if (winTitle.make_c_str() && wildcard_match(l->name, winTitle.data())) {
        break;
      }
    }
  }
  return w->BorderPattern[border] = (l ? l->runes : NULL);
}

inline void RCRemove(run **p) {
  run *r = *p;
  *p = r->next;
  r->next = NULL;
}

#define RCAddFirst(r, head)                                                                        \
  do {                                                                                             \
    (r)->next = (head);                                                                            \
    (head) = (r);                                                                                  \
  } while (0)

static void RCKillAll(void) {
  run *list[4] = {
      Run,
      Sleep,
      Wait,
      Interactive,
  };
  udat i;

  for (i = 0; i < 4; i++) {
    run *s = list[i], *r;
    while ((r = s)) {
      s = r->next;
      FreeMem(r);
    }
  }
  Run = Sleep = Wait = Interactive = (run *)0;
}

inline void RCKill(run **p) {
  run *r;
  if ((r = *p)) {
    *p = r->next;
    FreeMem(r);
  }
}

static run *RCNew(node l) {
  run *r;

  if ((r = (run *)AllocMem(sizeof(run)))) {
    memset(r, 0, sizeof(run));
    r->cycle = 0;
    r->stack[r->depth = 0] = l;
    RCAddFirst(r, Run);
  }
  return r;
}

static ldat applyflagx(node n) {
  ldat x = 0;
  switch (n->x.f.plus_minus) {
  case 0:
  case '+':
  case FL_ON:
    x++;
    break;
  case '-':
  case FL_OFF:
    x--;
    break;
  default:
    break;
  }
  return x * n->x.f.a;
}

static ldat applyflagy(node n) {
  ldat y = 0;
  switch (n->x.f.flag) {
  case 0:
  case '+':
  case FL_ON:
    y++;
    break;
  case '-':
  case FL_OFF:
    y--;
    break;
  default:
    break;
  }
  return y * n->x.f.b;
}

static Twindow RCFindWindowName(cstr name) {
  size_t len = name ? strlen(name) : 0;
  Tscreen screen = All->Screens.First;

  while (screen) {
    /* search among mapped windows */
    Twidget w = screen->Widgets.First;
    Twindow win;
    while (w) {
      if (IS_WINDOW(w) && (win = (Twindow)w)->Name == Chars(name, len)) {
        return win;
      }
      w = w->Next;
    }
    screen = screen->NextScreen();
  }
  return NULL;
}

static Tscreen RCFindScreenName(cstr name) {
  uldat len = name ? strlen(name) : 0;
  Tscreen screen = All->Screens.First;
  while (screen) {
    if (screen->NameLen >= 0 && (udat)screen->NameLen == len && !memcmp(screen->Name, name, len)) {
      break;
    }
  }
  return screen;
}

inline Twidget RCCheck4WidgetId(run *r) {
  Twidget w;
  if (!(w = (Twidget)Id2Obj(Twidget_class_byte, r->W)) || !w->Parent || !IS_SCREEN(w->Parent))

    r->W = NOID;
  return w;
}

enum {
  Snext = 0,
  Sfunc = 1,
  Sbody = 2,
  Ssleep = 3,
  Swait = 4,
  Sinter = 5,
  Serr = 6,
};

inline Twidget ForwardWindow(Twidget w) {
  while (w) {
    if (IS_WINDOW(w))
      return w;
    w = w->Next;
  }
  return w;
}

inline Twidget BackwardWindow(Twidget w) {
  while (w) {
    if (IS_WINDOW(w))
      return w;
    w = w->Prev;
  }
  return w;
}

inline Tscreen ScreenOf(Twidget w) {
  Twidget P;
  return w && (P = w->Parent) && IS_SCREEN(P) ? (Tscreen)P : (Tscreen)0;
}

/* run the specified queue */
static byte RCSteps(run *r) {
  Twidget w, SkipW;
  Tscreen screen;
  wm_ctx *C;
  const cstr *argv;
  node n, f;
  ldat flag;
  int nfd;
  byte state = Snext;
  bool ret = false;

  w = RCCheck4WidgetId(r);
  screen = ScreenOf(w);
  C = &r->C;
  n = r->stack[r->depth];

  while (r->cycle++ < MAX_RUNCYCLE) {
    state = Snext;

    if (n)
      switch (n->id) {

      case EXEC:
        if (flag_secure) {
          log(ERROR) << flag_secure_msg;
        } else {
          switch (fork()) {
          case -1: /* error */
            break;
          case 0: /* child */
            nfd = open("/dev/null", O_RDWR);
            close(0);
            close(1);
            close(2);
            if (nfd >= 0) {
              dup2(nfd, 0);
              dup2(nfd, 1);
              dup2(nfd, 2);
            }
            execvp(n->x.v.argv[0], (char *const *)RemoveConst(n->x.v.argv));
            exit(1);
            break;
          default: /* parent */
            break;
          }
        }
        break;

      case EXECTTY:
        /*
         * luckily, this does not immediately Map() the window...
         * it goes in a msg_map queued in the WM_MsgPort,
         * so we have the time to reach the corresponding WAIT here
         */
        OpenTerm(n->x.v.argv[0], n->x.v.argv);
        break;

      case INTERACTIVE:
        C->W = w;
        if (!C->Screen && !(C->Screen = screen))
          C->Screen = All->Screens.First;

        ret = false;
        switch (n->x.f.flag) {
        case MENU:
          ret = ActivateCtx(C, state_menu);
          break;
        case SCROLL:
          ret = ActivateCtx(C, state_scroll);
          break;
        case MOVE:
          ret = ActivateCtx(C, state_drag);
          break;
        case RESIZE:
          ret = ActivateCtx(C, state_resize);
          break;
        case SCREEN:
          ret = ActivateCtx(C, state_screen);
          break;
        default:
          state = Serr;
          break;
        }
        if (ret) {
          state = Sinter;
        }
        break;
      case MENU:
        /* this is just like INTERACTIVE MENU ... but does not wait! */
        ActivateCtx(C, state_menu);
        break;
      case MODULE:
        if (n->x.f.a == -1) {
          n->x.f.a = DlName2Id(Chars::from_c(n->name));
        }
        if (n->x.f.flag == FL_ON) {
          DlLoad((IdSo)n->x.f.a);
        } else {
          DlUnload((IdSo)n->x.f.a);
        }
        break;
      case MOVE:
        if (w && IS_WINDOW(w))
          DragWindow((Twindow)w, applyflagx(n), applyflagy(n));
        break;
      case MOVESCREEN:
        if (screen && screen != All->Screens.First)
          screen->Focus();
        DragFirstScreen(applyflagx(n), applyflagy(n));
        break;
      case NOP:
        break;
      case RESTART:
        SendControlMsg(Ext(WM, MsgPort), MSG_CONTROL_RESTART, 1 + strlen(n->name), n->name);
        return Serr;
        break;
      case RESIZE:
        if (w && IS_WINDOW(w)) {
          dat x = applyflagx(n), y = applyflagy(n);
          if (n->x.f.plus_minus == 0)
            x = n->x.f.a - w->XWidth + 2 * !(w->Flags & WINDOWFL_BORDERLESS);
          if (n->x.f.flag == 0)
            y = n->x.f.b - w->YWidth + 2 * !(w->Flags & WINDOWFL_BORDERLESS);
          ResizeRelWindow((Twindow)w, x, y);
          Check4Resize((Twindow)w); /* send MSG_WINDOW_CHANGE and realloc(w->Contents) */
        }
        break;
      case RESIZESCREEN:
        if (screen && screen != All->Screens.First)
          screen->Focus();
        ResizeFirstScreen(applyflagx(n));
        break;
      case SCROLL:
        if (w && IS_WINDOW(w))
          ScrollWindow((Twindow)w, applyflagx(n), applyflagy(n));
        break;
      case SENDTOSCREEN:
        if (w && IS_WINDOW(w) && screen && n->name) {
          Tscreen s = RCFindScreenName(n->name);
          if (s != screen) {
            w->UnMap();
            w->Map(s);
          }
        }
        break;
      case SLEEP:
        r->SW.WakeUp.Seconds = n->x.f.a / 1000;
        r->SW.WakeUp.Fraction = n->x.f.a % 1000;
        IncrTime(&r->SW.WakeUp, &All->Now);
        state = Ssleep;
        break;
      case STDERR:
        argv = n->x.v.argv;
        while (*argv) {
          log(INFO) << Chars::from_c(*argv++) << " ";
        }
        log(INFO) << "\n";
        break;
      case SYNTHETICKEY:
        if (w)
          SyntheticKey(w, n->x.f.a, n->x.f.flag, strlen(n->name), n->name);
        break;
      case WAIT:
        /* remember the window name we are waiting for */
        r->SW.Name = n->name;
        state = Swait;
        break;
      case WINDOW:
        if (n->name)
          w = RCFindWindowName(n->name);
        else {
          ldat i = applyflagx(n);
          flag = n->x.f.plus_minus;

          /*
           * WINDOW <n> :
           * n = 0 : re-get current focused window
           * n > 0 : 1 = First Window, 2 = Second, etc.
           *
           * WINDOW {+|-}<n>
           * n = 0 : no-op
           * n < 0 : -1 : Prev Window, -2 Prev Prev, etc.
           * n > 0 : +1 : Next Window, +2 Next Next, etc.
           */

          if (flag == 0) {
            if (i == 0) {
              w = All->Screens.First->FocusW();
              if (w && !IS_WINDOW(w)) {
                w = (Twidget)0;
              }
            } else {
              if (i > 0)
                i--;
              w = ForwardWindow(All->Screens.First->Widgets.First);
            }
          }

          if (w) {
            while (i > 0 && w) {
              i--;
              if ((SkipW = ForwardWindow(w->Next))) {
                w = SkipW;
              } else {
                screen = ScreenOf(w);
                w = NULL;
                if (screen)
                  while ((screen = screen->NextScreen()) &&
                         !(w = ForwardWindow(screen->Widgets.First)))
                    ;
              }
            }
            while (i < 0 && w) {
              i++;
              if ((SkipW = BackwardWindow(w->Prev))) {
                w = SkipW;
              } else {
                screen = ScreenOf(w);
                w = NULL;
                if (screen) {
                  while ((screen = screen->PrevScreen()) &&
                         !(w = BackwardWindow(screen->Widgets.Last))) {
                  }
                }
              }
            }
          }
        }
        r->W = w ? w->Id : NOID;
        screen = ScreenOf(w);
        if (n->body) /* enter function */
          state = Sbody;
        break;
      case BEEP:
        BeepHW();
        break;
      case CENTER:
        if (w && IS_WINDOW(w))
          CenterWindow((Twindow)w);
        break;
      case CLOSE:
        if (w)
          AskCloseWidget(w);
        break;
      case COPY:
        All->Clipboard->dup(All->Selection);
        break;
      case KILL:
        /* BRUTAL! */
        if (w) {
          Tmsgport M = w->Owner;
          /*
           * try not to exagerate with brutality:
           * allow deleting remote clients msgports only
           */
          if (M->RemoteData.FdSlot != NOSLOT) {
            Ext(Remote, KillSlot)(M->RemoteData.FdSlot);
            w = NULL;
            screen = NULL;
            r->W = NOID;
          } else
            AskCloseWidget(w);
        }
        break;
      case PASTE:
        All->Clipboard->paste();
        break;
      case QUIT:
        Quit(0);
        break;
      case REFRESH:
        RefreshVideo();
        break;
      case WINDOWLIST:
        ShowWinList(C);
        break;
      case FOCUS:
        if (w && screen) {
          flag = n->x.f.flag;
          if (flag == FL_TOGGLE) {
            flag = (screen->FocusW() == (Twidget)w) ? FL_OFF : FL_ON;
          }
          if (flag == FL_ON && screen != All->Screens.First) {
            screen->Focus();
          }
          if (flag == FL_ON) {
            w->Focus();
          } else {
            Swindow::Focus(NULL);
          }
        }
        break;
      case MAXIMIZE:
      case FULLSCREEN:
        if (w && IS_WINDOW(w) && screen)
          MaximizeWindow((Twindow)w, n->id == FULLSCREEN);
        break;
      case LOWER:
        if (w && screen)
          LowerWidget(w, false);
        break;
      case RAISE:
        if (w && screen) {
          RaiseWidget(w, false);
        }
        break;
      case RAISELOWER:
        if (w && screen) {
          if ((Twidget)w == screen->Widgets.First) {
            LowerWidget(w, true);
          } else {
            RaiseWidget(w, true);
          }
        }
        break;
      case ROLL:
        if (w && IS_WINDOW(w)) {
          flag = n->x.f.flag;
          if (flag == FL_TOGGLE) {
            flag = w->Attr & WINDOW_ROLLED_UP ? FL_OFF : FL_ON;
          }
          RollUpWindow((Twindow)w, flag == FL_ON);
        }
        break;
      case USERFUNC:
        if (n->x.func || ((f = LookupNodeName(n->name, FuncList)) && (n->x.func = f->body))) {
          /* stored into n->f.func to speedup future references */
          state = Sfunc;
        } else {
          /* user function not found. abort. */
          state = Serr;
        }
        break;
      default:
        /* unknown command. abort. */
        state = Serr;
        break;
      }

    if (state == Snext) {
      /* next instruction */
      if (!n || !(n = n->next)) {
        /* end of function. pop the stack. */
        if (r->depth) {
          n = r->stack[--r->depth];
        } else {
          break; /* stack empty. exit. */
        }
      }
    } else if (state == Sfunc || state == Sbody) {
      /* enter f or n->body */
      if (r->depth < MAX_RUNSTACK) {
        r->stack[r->depth++] = n->next;
        n = (state == Sfunc) ? n->x.func : n->body;
      } else {
        break; /* stack overflow */
      }
    } else if (state == Ssleep || state == Swait || state == Sinter) {
      /* prepare to resume from next instruction after sleeping */
      r->stack[r->depth] = n->next;
      break;
    } else
      break;
  }
  return state;
}

/* run all the runnable queues */
static void RCRun(void) {
  run *r, **p;

  /* ensure correct execution order */
  Run = ReverseRunList(Run);
  p = &Run;

  while ((r = *p))
    switch (RCSteps(r)) {
    case Ssleep:
      /* go to sleep */
      RCRemove(p);
      RCAddFirst(r, Sleep);
      break;
    case Swait:
      /* wait 4 window */
      RCRemove(p);
      RCAddFirst(r, Wait);
      break;
    case Sinter:
      /* wait 4 interactive op */
      RCRemove(p);
      RCAddFirst(r, Interactive);
      break;
    default:
      /* exit, runaway or error */
      RCKill(p);
      break;
    }
}

/*
 * let's see how much we can sleep:
 * return false if no limit to sleep time;
 * else _t will be filled with max time to sleep.
 */
static bool RCSleep(timevalue *_t) {
  timevalue *t = _t;
  run *r = Sleep;

  t->Seconds = TW_MAXTANY;
  t->Fraction = (tany)0; /* not TW_MAXTANY as Normalize() would overflow */

  while (r) {
    if (CmpTime(&r->SW.WakeUp, t) < 0) {
      t = &r->SW.WakeUp;
    }
    r = r->next;
  }

  if (t != _t) {
    if (CmpTime(t, &All->Now) > 0) {
      SubTime(_t, t, &All->Now);
    } else {
      /* avoid busy looping */
      _t->Seconds = 0;
      _t->Fraction = 10 MilliSECs;
    }
    return true;
  }
  return false;
}

/*
 * this is the real Initialization / Restart function.
 *
 * kill the queues, reload ~/.config/twin/twinrc and restart queues
 */
static void RCReload(void) {
  Tmodule m;
  bool (*mod_rcload)(Tdisplay) = NULL;
  byte success;

  if ((m = DlLoad(RCParseSo))) {
    mod_rcload = m->DoInit;
  }
#if 0
  else { /* this would garble -hw=tty display */
    log(ERROR) << "twin: failed to load the RC parser:\n      " << Errstr << "\n";
  }
#endif

  success = mod_rcload && mod_rcload(NULL);

  if (m)
    DlUnload(RCParseSo);

  if (success) {
    QueuedDrawArea2FullScreen = true;

    ResetBorderPattern();
    RCKillAll();
    if (CallList)
      RCNew(CallList);

    FillButtonWin();
    UpdateOptionWin();
    HideMenu(!!(All->SetUp->Flags & setup_menu_hide));
  }
}

/* wake up queues when slept enough or when finished interactive op */
static void RCWake(void) {
  run **p = &Sleep, *r;

  while ((r = *p)) {
    if (CmpTime(&r->SW.WakeUp, &All->Now) <= 0) {
      RCRemove(p); /* p does not change but *p is now the next run */
      RCAddFirst(r, Run);
    } else
      p = &r->next;
  }

  if ((All->State & state_any) == state_default && Interactive) {
    p = &Interactive;
    while ((r = *p)) {
      RCRemove(p); /* p does not change but *p is now the next run */
      RCAddFirst(r, Run);
    }
  }
}

/* wake up queues when the wanted window appears */
static void RCWake4Window(Twindow w) {
  const String &name = w->Name;
  run **p = &Wait, *r;

  if (name.size() > 0)
    while ((r = *p)) {
      if (name == Chars::from_c(r->SW.Name)) {
        r->W = w->Id;
        RCRemove(p); /* p does not change but *p is now the next run */
        RCAddFirst(r, Run);
      } else {
        p = &r->next;
      }
    }
}

static byte MouseClickReleaseSameCtx(uldat W1, uldat W2, ldat clickCtx, ldat relCtx, ldat ctx) {
  if ((ctx & clickCtx) && (ctx & relCtx)) {
    if ((clickCtx & CTX_ANY_WIN) != (relCtx & CTX_ANY_WIN)) {
      return tfalse;
    }
    if ((clickCtx & CTX_ANY_WIN) && (relCtx & CTX_ANY_WIN)) {
      return W1 == W2;
    }
    return ttrue;
  }
  return tfalse;
}

/* handle incoming messages */
bool RC_VMQueue(const wm_ctx *C) {
  uldat ClickWinId = All->Screens.First->ClickWindow ? All->Screens.First->ClickWindow->Id : NOID;
  Twidget w;
  ldat ctx;
  node n;
  run *r;
  udat Code;
  bool used = false;
  /* from wm.c : */
  extern byte ClickWindowPos;

  switch (C->Type) {
  case msg_key:
  case msg_mouse:

    n = (node)0;

    if (C->Type == msg_mouse) {

      ctx = Pos2Ctx(C->Pos);

      if (isSINGLE_PRESS(C->Code)) {
        Code = HOLD_CODE(PRESS_N(C->Code)) | PRESS_;
        n = RCFindMouseBind((ldat)Code, ctx);
        if (n) {
          ClickWinId = C->W ? C->W->Id : NOID;
          n = n->body;
        }
      } else if (isSINGLE_RELEASE(C->Code)) {
        Code = HOLD_CODE(RELEASE_N(C->Code)) | RELEASE_;
        n = RCFindMouseBind((ldat)Code, ctx);
        if (n && MouseClickReleaseSameCtx(ClickWinId, C->W ? C->W->Id : NOID,
                                          Pos2Ctx(ClickWindowPos), ctx, n->x.ctx)) {
          n = n->body;
        } else {
          n = NULL;
        }
      }
    } else {
      n = RCFindKeyBind((ldat)C->Code, (ldat)C->ShiftFlags);
    }
    if (n && (r = RCNew(n))) {
      used = true;
      CopyMem(C, &r->C, sizeof(wm_ctx));
      r->W = ClickWinId;
      /* to preserve execution orded, run it right now ! */
      RCRun();
    }
    break;
  case msg_map:
    used = true;
    RCWake4Window((Twindow)C->W);
    break;
  case msg_control:
    if (C->Code == MSG_CONTROL_OPEN) {
      used = true;
      if (All->State != state_default) {
        /*
         * return to state_default, and rely on RCReload()
         * to set QueuedDrawArea2FullScreen = true
         */
        ForceRelease(C);
      }
      RCReload();
    }
    break;
  case msg_menu_row:
    if (C->Code >= COD_RESERVED && C->Code < MenuBindsMax + COD_RESERVED) {
      n = MenuBinds[C->Code - COD_RESERVED];
      if (n && (r = RCNew(n))) {
        used = true;
        CopyMem(C, &r->C, sizeof(wm_ctx));
        w = All->Screens.First->FocusW();
        r->W = w ? w->Id : NOID;
        r->C.ByMouse = false;
        /* to preserve execution orded, run it right now ! */
        RCRun();
      }
    }
    break;
  default:
    break;
  }
  return used;
}

/* virtual machine main loop */
bool RC_VM(timevalue *t) {

  /* do the real thing: run the queues */
  RCWake();
  RCRun();
  return RCSleep(t);
}

void QuitRC(void) {
  ResetBorderPattern();
  RCKillAll();
  shm_quit();
}

enum {
  COD_COMMON_DRAG = COD_RESERVED,
  COD_COMMON_RESIZE = COD_RESERVED + 1,
  COD_COMMON_SCROLL = COD_RESERVED + 2,
  COD_COMMON_CENTER = COD_RESERVED + 3,
  COD_COMMON_MAXIMIZE = COD_RESERVED + 4,
  COD_COMMON_FULLSCREEN = COD_RESERVED + 5,
  COD_COMMON_ROLLTOGGLE = COD_RESERVED + 6,
  COD_COMMON_RAISELOWER = COD_RESERVED + 7,
  COD_COMMON_UNFOCUS = COD_RESERVED + 8,
  COD_COMMON_NEXT = COD_RESERVED + 9,
  COD_COMMON_WINDOWLIST = COD_RESERVED + 10,
  COD_COMMON_REFRESH = COD_RESERVED + 11,
  COD_COMMON_HOTKEY = COD_RESERVED + 12,
  COD_COMMON_CLOSE = COD_RESERVED + 13,

  COD_COMMON_FIRST = COD_COMMON_DRAG,
  COD_COMMON_LAST = COD_COMMON_CLOSE,
};

static byte USEDefaultCommonMenu(void) {
  Tmenu Menu;
  Tmenuitem item;
  Twindow w;
  Trow Row;

  if (!(Menu = Smenu::Create(Ext(WM, MsgPort), TCOL0, TCOL0, TCOL0, TCOL0, TCOL0, TCOL0, ttrue)))
    return tfalse;

  if ((w = Swindow::Create4Menu(Menu)) &&
      (item = Smenuitem::Create4Menu(Menu, w, 0, ttrue, 8, " Window ")) &&

      /* we cannot create rows with codes >= COD_RESERVED... */
      (Row = Row4Menu(w, 0, ROW_ACTIVE, 13, " Move        ")) && (Row->Code = COD_COMMON_DRAG) &&
      (Row = Row4Menu(w, 0, ROW_ACTIVE, 13, " Resize      ")) && (Row->Code = COD_COMMON_RESIZE) &&
      (Row = Row4Menu(w, 0, ROW_ACTIVE, 13, " Scroll      ")) && (Row->Code = COD_COMMON_SCROLL) &&
      (Row = Row4Menu(w, 0, ROW_ACTIVE, 13, " Center      ")) && (Row->Code = COD_COMMON_CENTER) &&
      (Row = Row4Menu(w, 0, ROW_ACTIVE, 13, " Maximize    ")) &&
      (Row->Code = COD_COMMON_MAXIMIZE) &&
      (Row = Row4Menu(w, 0, ROW_ACTIVE, 13, " Full screen ")) &&
      (Row->Code = COD_COMMON_FULLSCREEN) &&
      (Row = Row4Menu(w, 0, ROW_ACTIVE, 13, " Roll Up     ")) &&
      (Row->Code = COD_COMMON_ROLLTOGGLE) &&
      Row4Menu(w, 0, ROW_IGNORE, 13, "\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4") &&
      (Row = Row4Menu(w, 0, ROW_ACTIVE, 13, " Raise/Lower ")) &&
      (Row->Code = COD_COMMON_RAISELOWER) &&
      (Row = Row4Menu(w, 0, ROW_ACTIVE, 13, " UnFocus     ")) && (Row->Code = COD_COMMON_UNFOCUS) &&
      (Row = Row4Menu(w, 0, ROW_ACTIVE, 13, " Next        ")) && (Row->Code = COD_COMMON_NEXT) &&
      (Row = Row4Menu(w, 0, ROW_ACTIVE, 13, " List...     ")) &&
      (Row->Code = COD_COMMON_WINDOWLIST) &&
      Row4Menu(w, 0, ROW_IGNORE, 13, "\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4") &&
      (Row = Row4Menu(w, 0, ROW_ACTIVE, 13, " Refresh     ")) && (Row->Code = COD_COMMON_REFRESH) &&
      (Row = Row4Menu(w, 0, ROW_ACTIVE, 13, " Send HotKey ")) && (Row->Code = COD_COMMON_HOTKEY) &&
      Row4Menu(w, 0, ROW_IGNORE, 13, "\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4") &&
      (Row = Row4Menu(w, 0, ROW_ACTIVE, 13, " Close       ")) && (Row->Code = COD_COMMON_CLOSE)) {

    /* success */

    item->Left = 0; /* remove padding */

    if (All->CommonMenu) {
      All->CommonMenu->Delete();
    }
    All->CommonMenu = Menu;
    return ttrue;
  }

  /* out of memory */
  Menu->Delete();
  return tfalse;
}

byte InitRC(void) {
  byte InitRCOptions(void);

  static struct s_node N[] = {
      {INTERACTIVE,
       NULL,
       NULL,
       NULL,
       {NULL},
       {
           {
               0,
               MOVE,
           },
       }}, /* COD_COMMON_DRAG       */
      {INTERACTIVE,
       NULL,
       NULL,
       NULL,
       {NULL},
       {
           {
               0,
               RESIZE,
           },
       }}, /* COD_COMMON_RESIZE     */
      {INTERACTIVE,
       NULL,
       NULL,
       NULL,
       {NULL},
       {
           {
               0,
               SCROLL,
           },
       }}, /* COD_COMMON_SCROLL     */
      {
          CENTER,
      }, /* COD_COMMON_CENTER     */
      {
          MAXIMIZE,
      }, /* COD_COMMON_MAXIMIZE   */
      {
          FULLSCREEN,
      }, /* COD_COMMON_FULLSCREEN */
      {ROLL,
       NULL,
       NULL,
       NULL,
       {NULL},
       {
           {
               0,
               FL_TOGGLE,
           },
       }}, /* COD_COMMON_ROLLTOGGLE */
      {
          RAISELOWER,
      }, /* COD_COMMON_RAISELOWER */
      {FOCUS,
       NULL,
       NULL,
       NULL,
       {NULL},
       {
           {
               0,
               FL_OFF,
           },
       }}, /* COD_COMMON_UNFOCUS    */
      {WINDOW,
       NULL,
       NULL,
       NULL,
       {NULL},
       {
           {
               '+',
               0,
               1,
           },
       }}, /* COD_COMMON_NEXT       */
      {
          WINDOWLIST,
      }, /* COD_COMMON_WINDOWLIST */
      {
          REFRESH,
      }, /* COD_COMMON_REFRESH    */
      {
          SYNTHETICKEY,
      }, /* COD_COMMON_HOTKEY     */
      {
          CLOSE,
      } /* COD_COMMON_CLOSE      */
  };
  static struct s_node *pN[] = {N,     N + 1, N + 2, N + 3,  N + 4,  N + 5,  N + 6,
                                N + 7, N + 8, N + 9, N + 10, N + 11, N + 12, N + 13};

  /* now user functions list */
  static struct s_node F[] = {/* AddToFunc _UnFocus ( Window 0; Focus Off;) */
                              {
                                  0,
                                  "_UnFocus",
                                  NULL,
                                  F + 1,
                              },
                              {WINDOW,
                               NULL,
                               N + 8,
                               NULL,
                               {NULL},
                               {{
                                   0,
                                   0,
                               }}}};

  /* now keyboard binds */
  static struct s_node K[] = {/* Key HOT_KEY Interactive Menu */
                              {
                                  0,
                                  NULL,
                                  NULL,
                                  K + 1,
                              },
                              {INTERACTIVE,
                               NULL,
                               NULL,
                               NULL,
                               {NULL},
                               {
                                   {
                                       0,
                                       MENU,
                                   },
                               }}};

  /* now mouse binds */
  static struct s_node M[] = {/* Mouse   1   0  Close */
                              {HOLD_LEFT | RELEASE_,
                               "0",
                               M + 1,
                               N + 13,
                               {NULL},
                               {{
                                   CTX_0,
                               }}},
                              /* Mouse  123  1  RaiseLower */
                              {HOLD_ANY | RELEASE_,
                               "1",
                               M + 2,
                               N + 7,
                               {NULL},
                               {{
                                   CTX_1,
                               }}},
                              /* Mouse  123  3  Roll Toggle */
                              {HOLD_ANY | RELEASE_,
                               "2",
                               M + 3,
                               N + 6,
                               {NULL},
                               {{
                                   CTX_2,
                               }}},
                              /* Mouse  H1   TS Interactive Move */
                              {HOLD_LEFT | PRESS_,
                               "TS",
                               M + 4,
                               N + 0,
                               {NULL},
                               {{
                                   CTX_TITLE | CTX_SIDE,
                               }}},
                              /* Mouse  H1   C  Interactive Resize */
                              {HOLD_LEFT | PRESS_,
                               "C",
                               M + 5,
                               N + 1,
                               {NULL},
                               {{
                                   CTX_CORNER,
                               }}},
                              /* Mouse  H1   C  Interactive Scroll */
                              {HOLD_LEFT | PRESS_,
                               "B",
                               M + 6,
                               N + 2,
                               {NULL},
                               {{
                                   CTX_BARS,
                               }}},
                              /* Mouse  H1   R _UnFocus */
                              {HOLD_LEFT | PRESS_,
                               "R",
                               M + 8,
                               M + 7,
                               {NULL},
                               {{
                                   CTX_ROOT,
                               }}},
                              {
                                  USERFUNC,
                                  "_UnFocus",
                              },
                              /* Mouse  H2  R  WindowList */
                              {HOLD_MIDDLE | PRESS_,
                               "R",
                               M + 9,
                               N + 10,
                               {NULL},
                               {{
                                   CTX_ROOT,
                               }}},
                              /* Mouse  H3  A  Interactive Menu */
                              {HOLD_RIGHT | PRESS_,
                               "A",
                               M + 10,
                               K + 1,
                               {NULL},
                               {{
                                   CTX_ANY,
                               }}},
                              /* Mouse  H1  M  Interactive screen */
                              {HOLD_LEFT | PRESS_,
                               "M",
                               NULL,
                               M + 11,
                               {NULL},
                               {{
                                   CTX_MENU,
                               }}},
                              {INTERACTIVE,
                               NULL,
                               NULL,
                               NULL,
                               {NULL},
                               {
                                   {
                                       0,
                                       SCREEN,
                                   },
                               }}};
#define UD_ARROW T_UTF_32_UP_DOWN_ARROW
  static button_vec V[] = {{{'[', ']'}, 0, ttrue, tfalse},
                           {{UD_ARROW, UD_ARROW}, -2, ttrue, tfalse},
                           {{'>', '<'}, -4, ttrue, tfalse}};

  const char *Seq = "";
  /*
   * this is really heavy on the compiler...
   * but it should be able to optimize it
   * to just the line with the correct TW_xxx key
   */
#define IS(key, len, seq)                                                                          \
  if (HOT_KEY == TW_##key)                                                                         \
    Seq = seq;
#include "hw_keys.h"
#undef IS
  K[0].name = N[COD_COMMON_HOTKEY - COD_COMMON_FIRST].name = Seq;
  K[0].id = N[COD_COMMON_HOTKEY - COD_COMMON_FIRST].x.f.a = HOT_KEY;

  memset(Globals, 0, sizeof(Globals));
  FuncList = F;
  KeyList = K;
  MouseList = M;

  MenuBinds = pN;
  MenuBindsMax = COD_COMMON_LAST - COD_COMMON_FIRST + 1;
  GlobalsAreStatic = ttrue;

  memset(All->ButtonVec, 0, sizeof(All->ButtonVec));
  CopyMem(V, All->ButtonVec, sizeof(V));

  All->SetUp->ButtonSelection = HOLD_LEFT;
  All->SetUp->ButtonPaste = HOLD_MIDDLE;
  All->SetUp->DeltaXShade = 3;
  All->SetUp->DeltaXShade = 2;

  if (USEDefaultCommonMenu()) {

    InitRCOptions();
    UpdateOptionWin();
    FillButtonWin();
    HideMenu(!!(All->SetUp->Flags & setup_menu_hide));
    All->Screens.First->DrawMenu(0, TW_MAXDAT);

    return ttrue;
  }
  return tfalse;
}
