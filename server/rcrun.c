/*
 *  rcrun.c  --  virtual machine to run byte-compiled ~/.twinrc code
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
#include "data.h"
#include "builtin.h"
#include "draw.h"
#include "resize.h"
#include "util.h"
#include "wm.h"

#include "extreg.h"
#include "methods.h"
#include "hw.h"
#include "common.h"
#include "hw_multi.h"

#include "rctypes.h"
#include "rcparse_tab.h"
#include "rcrun.h"

#ifdef CONF_WM_RC
# include "rcproto.h"
#endif
#ifdef CONF__MODULES
# include "dl.h"
#endif

#include <Tw/Twkeys.h>
#include "hotkey.h"

#ifdef CONF__UNICODE
# include <Tutf/Tutf.h>
# include <Tutf/Tutf_defs.h>
#endif




#define MAX_RUNCYCLE      1024 /* kill a queue after this number of steps */

#define MAX_RUNSTACK 28
typedef struct run run;

struct run {
    run *next;
    uldat W;     /* the current widget (id) */
    uldat depth; /* index of last used stack element;
		  * stack[depth] is the current instruction */
    uldat cycle;
    union {
	timevalue WakeUp;
	str Name;
    } SW;	 /* what we are waiting for: sleep timeout or widget map */
    
    wm_ctx C;   /* event that generated the run queue */
    node stack[MAX_RUNSTACK];
};


byte GlobalsAreStatic;
node Globals[GLOBAL_MAX];

node *MenuBinds; /* array of pointers to nodes inside MenuList */
udat MenuBindsMax;

static run *Run;   /* list of running queues */
static run *Sleep; /* list of sleeping queues */
static run *Wait;  /* list of waiting-for-window queues */
static run *Interactive;  /* list of waiting-for-interactive-op queues */


/* shell-like wildcard pattern matching */
static byte wildcard_match(str p, str q) {
    byte c;

    if (!q)
	q = "";
    
    for (;;) {
	switch (c = *p++) {
	  case '\0':
	    return !*q;
	    break;
	  case '\\':
	    if (*q++ != *p++)
		return FALSE;
	    break;
	  case '?':
	    if (*q++ == '\0')
		return FALSE;
	    break;
	  case '*':
	    c = *p;
	    if (c != '\\' && c != '?' && c != '*' && c != '[') {
		while (*q != c) {
		    if (*q++ == '\0')
			return FALSE;
		}
	    }
	    do {
		if (wildcard_match(p, q))
		    return TRUE;
	    } while (*q++ != '\0');
	    return FALSE;
	  case '[': {
	      str endp;
	      byte invert = 0, found = 0;
	      byte chr;
	      
	      endp = p;
	      if (*endp == '!')
		  endp++;
	      for (;;) {
		  if (*endp == '\0')
		      goto dft;		/* no matching ] */
		  if (*endp == '\\')
		      endp++;
		  if (*++endp == ']')
		      break;
	      }
	      if (*p == '!') {
		  invert++;
		  p++;
	      }
	      chr = *q++;
	      c = *p++;
	      do {
		  if (c == '\\')
		      c = *p++;
		  if (*p == '-' && p[1] != ']') {
		      p++;
		      if (*p == '\\')
			  p++;
		      if (chr >= c && chr <= *p)
			  found = 1;
		      p++;
		  } else {
		      if (chr == c)
			  found = 1;
		  }
	      } while ((c = *p++) != ']');
	      
	      if (found == invert)
		  return FALSE;
	      break;
	  }
	  dft:
	  default:
	    if (*q++ != c)
		return FALSE;
	    break;
	}
    }
}

node LookupNodeName(str name, node head) {
    node l;
    if (name) for (l = head; l; l = l->next) {
	if (!CmpStr(name, l->name))
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
      case POS_SIDE_UP:	return CTX_TITLE;
      case POS_SIDE_LEFT:
      case POS_SIDE_RIGHT:
      case POS_SIDE_DOWN:return CTX_SIDE;
      case POS_BUTTON_RESIZE:return CTX_CORNER;

      case POS_X_BAR_BACK:
      case POS_X_BAR_FWD:
      case POS_X_TAB:
      case POS_X_ARROW_BACK:
      case POS_X_ARROW_FWD:
      case POS_Y_BAR_BACK:
      case POS_Y_BAR_FWD:
      case POS_Y_TAB:
      case POS_Y_ARROW_BACK:
      case POS_Y_ARROW_FWD:return CTX_BARS;
	
      case POS_INSIDE:return CTX_INSIDE;
      case POS_MENU:  return CTX_MENU;

      case POS_BUTTON_SCREEN:
      case POS_ROOT:  return CTX_ROOT;
      default:
	if (Pos < BUTTON_MAX)
	    return CTX_BUTTON(Pos);
    }
    return 0;
}

static node RCFindKeyBind(ldat label, ldat shiftflags) {
    node l = KeyList;
    for (; l; l = l->next) {
	if (label == l->id && shiftflags == l->x.ctx)
	    return l->body;
    }
    return NULL;
}

static node RCFindMouseBind(ldat code, ldat ctx) {
    node l = MouseList;
    ldat hc = code & (PRESS_|RELEASE_);
    code &= HOLD_ANY;
    
    for (; l; l = l->next) {
	/* triple-inclusive match here:
	 * (l->id & code)   : match buttons
	 * (l->id & hc)     : match hold/click
	 * (l->x.ctx & ctx): match release context
	 */
	if ((l->id & code) && (l->id & hc) && (l->x.ctx & ctx))
	    return l;
    }
    return NULL;
}

hwfont *RCFindBorderPattern(window W, byte Border) {
    node l;
    
    if (!W)
	return NULL;
    
    for (l = BorderList; l; l=l->next) {
	if ((l->x.f.flag == FL_INACTIVE) == Border && wildcard_match(l->name, W->Name))
	    break;
    }
    return W->BorderPattern[Border] = l ? (hwfont *)l->data : NULL;
}

INLINE void RCRemove(run **p) {
    run *r = *p;
    *p = r->next;
    r->next = NULL;
}

#define RCAddFirst(r, head) do { (r)->next = (head); (head) = (r); } while (0)

static void RCKillAll(void) {
    run *r, *s, **l, *list[4];

    list[0] = Run;  list[1] = Sleep;
    list[2] = Wait; list[3] = Interactive;
    for (s = *(l = list); l < list + 4; s = *++l) {
	while ((r = s)) {
	    s = r->next;
	    FreeMem(r);
	}
    }
    Run = Sleep = Wait = Interactive = (run *)0;
}

INLINE void RCKill(run **p) {
    run *r;
    if ((r = *p)) {
	*p = r->next;
	FreeMem(r);
    }
}

static run *RCNew(node l) {
    run *r;
    
    if ((r = (run *)AllocMem(sizeof(run)))) {
	r->cycle = 0;
	r->stack[ r->depth = 0 ] = l;
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

static window RCFindWindowName(str name) {
    uldat len = strlen(name);
    window W;
    screen S = All->FirstScreen;
	
    while (S) {
	/* search among mapped windows */
	W = (window)S->FirstW;
	while (W) {
	    if (IS_WINDOW(W) && W->NameLen == len && !CmpMem(W->Name, name, len))
		return W;
	    W = (window)W->Next;
	}
	S = S->Next;
    }
    return NULL;
}
	
static screen RCFindScreenName(str name) {
    uldat len = strlen(name);
    screen S = All->FirstScreen;
    while (S) {
	if (S->NameLen == len && !CmpMem(S->Name, name, len))
	    break;
    }
    return S;
}

INLINE widget RCCheck4WidgetId(run *r) {
    widget W;
    if (!(W = (widget)Id2Obj(widget_magic_id, r->W))
	|| !W->Parent || !IS_SCREEN(W->Parent))
	
	r->W = NOID;
    return W;
}

#define Snext 0
#define Sfunc 1
#define Sbody 2
#define Ssleep 3
#define Swait 4
#define Sinter 5
#define Serr  6


INLINE widget ForwardWindow(widget W) {
    while (W) {
	if (IS_WINDOW(W))
	    return W;
	W = W->Next;
    }
    return W;
}

INLINE widget BackwardWindow(widget W) {
    while (W) {
	if (IS_WINDOW(W))
	    return W;
	W = W->Prev;
    }
    return W;
}

INLINE screen ScreenOf(widget W) {
    widget P;
    return W && (P=W->Parent) && IS_SCREEN(P) ? (screen)P : (screen)0;
}

/* run the specified queue */
static byte RCSteps(run *r) {
    widget W, SkipW;
    screen S;
    wm_ctx *C;
    node n, f;
    ldat flag;
    str *argv;
    byte state, ret;
    int nfd;
    
    W = RCCheck4WidgetId(r);
    S = ScreenOf(W);
    C = &r->C;
    n = r->stack[r->depth];
    
    while (r->cycle++ < MAX_RUNCYCLE) {
	state = Snext;
	
	if (n) switch (n->id) {
	    
	  case EXEC:
	    if (flag_secure)
		printk(flag_secure_msg);
	    else switch (fork()) {
	      case -1: /* error */
		break;
	      case 0:  /* child */
		nfd = open("/dev/null", O_RDWR);
		close(0); close(1); close(2);
		if (nfd >= 0) {
		    dup2(nfd, 0); dup2(nfd, 1); dup2(nfd, 2);
		}
		execvp((char *)n->x.v.argv[0], (char **)n->x.v.argv);
		exit(1);
		break;
	      default: /* parent */
		break;
	    }
	    break;
	    
	  case EXECTTY:
	    /*
	     * luckily, this does not immediately Map() the window...
	     * it goes in a MSG_MAP queued in the WM_MsgPort,
	     * so we have the time to reach the corresponding WAIT here
	     */
	    Ext(Term,Open)(n->x.v.argv[0], n->x.v.argv);
	    break;
	    
	  case INTERACTIVE:
	    C->W = W;
	    if (!C->Screen && !(C->Screen=S))
		C->Screen = All->FirstScreen;
	    
	    ret = FALSE;
	    switch (n->x.f.flag) {
	      case MENU:   ret = ActivateCtx(C, STATE_MENU);   break;
	      case SCROLL: ret = ActivateCtx(C, STATE_SCROLL); break;
	      case MOVE:   ret = ActivateCtx(C, STATE_DRAG);   break;
	      case RESIZE: ret = ActivateCtx(C, STATE_RESIZE); break;
	      case SCREEN: ret = ActivateCtx(C, STATE_SCREEN); break;
	      default:     state = Serr;		       break;
	    }
	    if (ret)
		state = Sinter;
	    break;
	  case MENU:
	    /* this is just like INTERACTIVE MENU ... but does not wait! */
	    ActivateCtx(C, STATE_MENU);
	    break;
	  case MODULE:
#ifdef CONF__MODULES
	    if (n->x.f.a == -1)
		n->x.f.a = DlName2Code(n->name);
	    if (n->x.f.flag == FL_ON)
		DlLoad(n->x.f.a);
	    else
		DlUnLoad(n->x.f.a);
#endif
	    break;
	  case MOVE:
	    if (W && IS_WINDOW(W))
		DragWindow((window)W, applyflagx(n), applyflagy(n));
	    break;
	  case MOVESCREEN:
	    if (S && S != All->FirstScreen)
		Act(Focus,S)(S);
	    DragFirstScreen(applyflagx(n), applyflagy(n));
	    break;
	  case NOP:
	    break;
	  case RESTART:
	    SendControlMsg(Ext(WM,MsgPort), MSG_CONTROL_RESTART, 1 + strlen(n->name), n->name);
	    return Serr;
	    break;
	  case RESIZE:
	    if (W && IS_WINDOW(W)) {
		dat x = applyflagx(n), y = applyflagy(n);
		if (n->x.f.plus_minus == 0)
		    x = n->x.f.a - W->XWidth + 2*!(W->Flags & WINDOWFL_BORDERLESS);
		if (n->x.f.flag == 0)
		    y = n->x.f.b - W->YWidth + 2*!(W->Flags & WINDOWFL_BORDERLESS);
		ResizeRelWindow((window)W, x, y);
		Check4Resize((window)W); /* send MSG_WINDOW_CHANGE and realloc(W->Contents) */
	    }
	    break;
	  case RESIZESCREEN:
	    if (S && S != All->FirstScreen)
		Act(Focus,S)(S);
	    ResizeFirstScreen(applyflagx(n));
	    break;
	  case SCROLL:
	    if (W && IS_WINDOW(W))
		ScrollWindow((window)W, applyflagx(n), applyflagy(n));
	    break;
	  case SENDTOSCREEN:
	    if (W && IS_WINDOW(W) && S && n->name) {
		screen Screen = RCFindScreenName(n->name);
		if (S != Screen) {
		    Act(UnMap,W)(W);
		    Act(Map,W)(W, (widget)Screen);
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
	    while (*argv)
		printk("%."STR(SMALLBUFF)"s ", *argv++);
	    printk("\n");
	    break;
	  case SYNTHETICKEY:
	    if (W)
		SyntheticKey(W, n->x.f.a, n->x.f.flag, strlen(n->name), n->name);
	    break;
	  case WAIT:
	    /* remember the window name we are waiting for */
	    r->SW.Name = n->name;
	    state = Swait;
	    break;
	  case WINDOW:
	    if (n->name)
		W = (widget)RCFindWindowName(n->name);
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
			W = All->FirstScreen->FocusW;
			if (W && !IS_WINDOW(W))
			    W = (widget)0;
		    } else {
			if (i > 0) i--;
			W = ForwardWindow(All->FirstScreen->FirstW);
		    }
		}
		
		if (W) {
		    while (i > 0 && W) {
			i--;
			if ((SkipW = ForwardWindow(W->Next)))
			    W = SkipW;
			else {
			    S = ScreenOf(W);
			    W = NULL;
			    if (S) while ((S = S->Next) && !(W = ForwardWindow(S->FirstW)))
				;
			}
		    }
		    while (i < 0 && W) {
			i++;
			if ((SkipW = BackwardWindow(W->Prev)))
			    W = SkipW;
			else {
			    S = ScreenOf(W);
			    W = NULL;
			    if (S) while ((S = S->Prev) && !(W = BackwardWindow(S->LastW)))
				;
			}
		    }
		}
	    }
	    r->W = W ? W->Id : NOID;
	    S = ScreenOf(W);
	    if (n->body) /* enter function */
		state = Sbody;
	    break;
	  case BEEP:
	    BeepHW();
	    break;
	  case CENTER:
	    if (W && IS_WINDOW(W))
		CenterWindow((window)W);
	    break;
	  case CLOSE:
	    if (W)
		AskCloseWidget(W);
	    break;
	  case KILL:
	    /* BRUTAL! */
	    if (W) {
		msgport M = W->Owner;
		/*
		 * try not to exagerate with brutality:
		 * allow deleting remote clients msgports only
		 */
		if (M->RemoteData.FdSlot != NOSLOT) {
		    Ext(Remote,KillSlot)(M->RemoteData.FdSlot);
		    W = NULL;
		    S = NULL;
		    r->W = NOID;
		} else
		    AskCloseWidget(W);
	    }
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
	    if (W && S) {
		flag = n->x.f.flag;
		if (flag == FL_TOGGLE)
		    flag = (S->FocusW == (widget)W) ? FL_OFF : FL_ON;
		
		if (flag == FL_ON && S != All->FirstScreen)
		    Act(Focus,S)(S);
		
		Act(Focus,W)(flag == FL_ON ? W : (widget)0);
	    }
	    break;
	  case MAXIMIZE:
	  case FULLSCREEN:
	    if (W && IS_WINDOW(W) && S)
		MaximizeWindow((window)W, n->id == FULLSCREEN);
	    break;
	  case LOWER:
	    if (W && S)
		LowerWidget(W, FALSE);
	    break;
	  case RAISE:
	    if (W && S)
		RaiseWidget(W, FALSE);
	    break;
	  case RAISELOWER:
	    if (W && S) {
		if ((widget)W == S->FirstW)
		    LowerWidget(W, TRUE);
		else
		    RaiseWidget(W, TRUE);
	    }
	    break;
	  case ROLL:
	    if (W && IS_WINDOW(W)) {
		flag = n->x.f.flag;
		if (flag == FL_TOGGLE)
		    flag = W->Attrib & WINDOW_ROLLED_UP ? FL_OFF : FL_ON;
		RollUpWindow((window)W, flag == FL_ON);
	    }
	    break;
	  case USERFUNC:
	    if (n->x.func || ((f = LookupNodeName(n->name, FuncList))
			      && (n->x.func = f->body)))
		/* stored into n->f.func to speedup future references */
		state = Sfunc;
	    else /* user function not found. abort. */
		state = Serr;
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
		if (r->depth)
		    n = r->stack[--r->depth];
		else /* stack empty. exit. */
		    break;
	    }
	} else if (state == Sfunc || state == Sbody) {
	    /* enter f or n->body */
	    if (r->depth < MAX_RUNSTACK) {
		r->stack[r->depth++] = n->next;
		n = (state == Sfunc) ? n->x.func : n->body;
	    } else /* stack overflow */
		break;
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
    
    while ((r = *p)) switch (RCSteps(r)) { 
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
 * return FALSE if no limit to sleep time;
 * else _t will be filled with max time to sleep.
 */
static byte RCSleep(timevalue *_t) {
    timevalue *t = _t;
    run *r = Sleep;

    t->Seconds = MAXTANY;
    t->Fraction = (tany)0; /* not MAXTANY as Normalize() would overflow */

    while (r) {
	if (CmpTime(&r->SW.WakeUp, t) < 0)
	    t = &r->SW.WakeUp;
	r = r->next;
    }
    
    if (t != _t) {
	if (CmpTime(t, &All->Now) > 0)
	    SubTime(_t, t, &All->Now);
	else {
	    /* avoid busy looping */
	    _t->Seconds = 0;
	    _t->Fraction = 10 MilliSECs;
	}
	return TRUE;
    }
    return FALSE;
}




/*
 * this is the real Initialization / Restart function.
 * 
 * kill the queues, reload .twinrc and restart queues
 */
static void RCReload(void) {
    byte success;
#if !defined(CONF_WM_RC) && defined(CONF__MODULES)
    module M;
    byte (*mod_rcload)(void) = (byte (*)(void))0;

    if ((M = DlLoad(RCParseSo)))
	mod_rcload = M->Private;
# if 0
    /* this would garble -hw=tty display */
    else
	printk("twin: failed to load the RC parser:\n"
		"      %."STR(SMALLBUFF)"s\n", ErrStr);
# endif
#endif
    

    success =
#if defined(CONF_WM_RC)
	rcload()
#elif defined(CONF__MODULES)
	(mod_rcload && mod_rcload())
#else
	0
#endif
	;
    
#if !defined(CONF_WM_RC) && defined(CONF__MODULES)
    if (M)
	DlUnLoad(RCParseSo);
#endif
    
    if (success) {
	QueuedDrawArea2FullScreen = TRUE;
	
	ResetBorderPattern();
	RCKillAll();
	if (CallList)
	    RCNew(CallList);

	FillButtonWin();
	UpdateOptionWin();
	HideMenu(!!(All->SetUp->Flags & SETUP_MENU_HIDE));
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
    
    if ((All->State & STATE_ANY) == STATE_DEFAULT && Interactive) {
	p = &Interactive;
	while ((r = *p)) {
	    RCRemove(p); /* p does not change but *p is now the next run */
	    RCAddFirst(r, Run);
	}
    }
}

/* wake up queues when the wanted window appears */
static void RCWake4Window(window W) {
    str Name = W->Name;
    run **p = &Wait, *r;

    if (Name) while ((r = *p)) {
	if (!CmpStr(r->SW.Name, Name)) {
	    r->W = W->Id;
	    RCRemove(p); /* p does not change but *p is now the next run */
	    RCAddFirst(r, Run);
	} else
	    p = &r->next;
    }
}

static byte MouseClickReleaseSameCtx(uldat W1, uldat W2, ldat clickCtx, ldat relCtx, ldat ctx) {
    if ((ctx & clickCtx) && (ctx & relCtx)) {
	if ((clickCtx & CTX_ANY_WIN) != (relCtx & CTX_ANY_WIN))
	    return FALSE;
	if ((clickCtx & CTX_ANY_WIN) && (relCtx & CTX_ANY_WIN))
	    return W1 == W2;
	return TRUE;
    }
    return FALSE;
}


/* handle incoming messages */
byte RC_VMQueue(CONST wm_ctx *C) {
    uldat ClickWinId = All->FirstScreen->ClickWindow
	? All->FirstScreen->ClickWindow->Id : NOID;
    widget W;
    ldat ctx;
    node n;
    run *r;
    udat Code;
    byte used = FALSE;
    /* from wm.c : */
    extern byte ClickWindowPos;

	
    switch (C->Type) {
      case MSG_KEY:	
      case MSG_MOUSE:
	
	n = (node)0;
	
	if (C->Type == MSG_MOUSE) {
	    
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
						  Pos2Ctx(ClickWindowPos), ctx, n->x.ctx))
		    n = n->body;
		else
		    n = NULL;
	    }
	} else
	    n = RCFindKeyBind((ldat)C->Code, (ldat)C->ShiftFlags);
	    
	if (n && (r = RCNew(n))) {
	    used = TRUE, CopyMem(C, &r->C, sizeof(wm_ctx));
	    r->W = ClickWinId;
	    /* to preserve execution orded, run it right now ! */
	    RCRun();
	}
	break;
      case MSG_MAP:
	used = TRUE, RCWake4Window((window)C->W);
	break;
      case MSG_CONTROL:
	if (C->Code == MSG_CONTROL_OPEN) {
	    used = TRUE;
	    if (All->State != STATE_DEFAULT)
		/*
		 * return to STATE_DEFAULT, and rely on RCReload()
		 * to set QueuedDrawArea2FullScreen = TRUE
		 */
		ForceRelease(C);
	    
	    RCReload();
	}
	break;
      case MSG_MENU_ROW:
	if (C->Code >= COD_RESERVED && C->Code < MenuBindsMax + COD_RESERVED) {
	    n = MenuBinds[C->Code - COD_RESERVED];
	    if (n && (r = RCNew(n))) {
		used = TRUE;
		CopyMem(C, &r->C, sizeof(wm_ctx));
		W = All->FirstScreen->FocusW;
		r->W = W ? W->Id : NOID;
		r->C.ByMouse = FALSE;
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
byte RC_VM(timevalue *t) {

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










#define COD_COMMON_FIRST	COD_COMMON_DRAG

#define COD_COMMON_DRAG         (COD_RESERVED)
#define COD_COMMON_RESIZE       (COD_RESERVED + 1)
#define COD_COMMON_SCROLL       (COD_RESERVED + 2)
#define COD_COMMON_CENTER	(COD_RESERVED + 3)
#define COD_COMMON_MAXIMIZE	(COD_RESERVED + 4)
#define COD_COMMON_FULLSCREEN	(COD_RESERVED + 5)
#define COD_COMMON_ROLLTOGGLE   (COD_RESERVED + 6)
#define COD_COMMON_RAISELOWER	(COD_RESERVED + 7)
#define COD_COMMON_UNFOCUS      (COD_RESERVED + 8)
#define COD_COMMON_NEXT         (COD_RESERVED + 9)
#define COD_COMMON_WINDOWLIST	(COD_RESERVED + 10)
#define COD_COMMON_REFRESH	(COD_RESERVED + 11)
#define COD_COMMON_HOTKEY       (COD_RESERVED + 12)
#define COD_COMMON_CLOSE	(COD_RESERVED + 13)

#define COD_COMMON_LAST		COD_COMMON_CLOSE

static byte USEDefaultCommonMenu(void) {
    menu Menu;
    menuitem Item;
    window W;
    row Row;
    
    if (!(Menu = Do(Create,Menu)(FnMenu, Ext(WM,MsgPort), (hwcol)0, (hwcol)0, (hwcol)0,
				 (hwcol)0, (hwcol)0, (hwcol)0, TRUE)))
	return FALSE;
    
    if ((W=Win4Menu(Menu)) &&
	(Item = Item4Menu(Menu, W, TRUE, 8, " Window ")) &&

	/* we cannot create rows with codes >= COD_RESERVED... */
	(Row = Row4Menu(W, 0, ROW_ACTIVE, 13," Move        ")) && (Row->Code = COD_COMMON_DRAG) &&
	(Row = Row4Menu(W, 0, ROW_ACTIVE, 13," Resize      ")) && (Row->Code = COD_COMMON_RESIZE) &&
	(Row = Row4Menu(W, 0, ROW_ACTIVE, 13," Scroll      ")) && (Row->Code = COD_COMMON_SCROLL) &&
	(Row = Row4Menu(W, 0, ROW_ACTIVE, 13," Center      ")) && (Row->Code = COD_COMMON_CENTER) &&
	(Row = Row4Menu(W, 0, ROW_ACTIVE, 13," Maximize    ")) && (Row->Code = COD_COMMON_MAXIMIZE) &&
	(Row = Row4Menu(W, 0, ROW_ACTIVE, 13," Full Screen ")) && (Row->Code = COD_COMMON_FULLSCREEN) &&
	(Row = Row4Menu(W, 0, ROW_ACTIVE, 13," Roll Up     ")) && (Row->Code = COD_COMMON_ROLLTOGGLE) &&
	       Row4Menu(W, 0, ROW_IGNORE, 13,"컴컴컴컴컴컴�") &&
	(Row = Row4Menu(W, 0, ROW_ACTIVE, 13," Raise/Lower ")) && (Row->Code = COD_COMMON_RAISELOWER) &&
	(Row = Row4Menu(W, 0, ROW_ACTIVE, 13," UnFocus     ")) && (Row->Code = COD_COMMON_UNFOCUS) &&
	(Row = Row4Menu(W, 0, ROW_ACTIVE, 13," Next        ")) && (Row->Code = COD_COMMON_NEXT) &&
	(Row = Row4Menu(W, 0, ROW_ACTIVE, 13," List...     ")) && (Row->Code = COD_COMMON_WINDOWLIST) &&
	Row4Menu(W, 0, ROW_IGNORE, 13,"컴컴컴컴컴컴�") &&
	(Row = Row4Menu(W, 0, ROW_ACTIVE, 13," Refresh     ")) && (Row->Code = COD_COMMON_REFRESH) &&
	(Row = Row4Menu(W, 0, ROW_ACTIVE, 13," Send HotKey ")) && (Row->Code = COD_COMMON_HOTKEY) &&
	Row4Menu(W, 0, ROW_IGNORE, 13,"컴컴컴컴컴컴�") &&
	(Row = Row4Menu(W, 0, ROW_ACTIVE, 13," Close       ")) && (Row->Code = COD_COMMON_CLOSE)) {

	/* success */
	
	Item->Left = 0; /* remove padding */
	
	if (All->CommonMenu)
	    Delete(All->CommonMenu);
	All->CommonMenu = Menu;
	return TRUE;
    }
    
    /* out of memory */
    Delete(Menu);
    return FALSE;
}

byte InitRC(void) {
    byte InitRCOptions(void);
    
    static struct node N[] = {
	{ INTERACTIVE, NULL, NULL, NULL, NULL, { { 0, MOVE, }, } },	/* COD_COMMON_DRAG       */
	{ INTERACTIVE, NULL, NULL, NULL, NULL, { { 0, RESIZE, }, } },	/* COD_COMMON_RESIZE     */
	{ INTERACTIVE, NULL, NULL, NULL, NULL, { { 0, SCROLL, }, } },	/* COD_COMMON_SCROLL     */
	{ CENTER, },							/* COD_COMMON_CENTER     */
	{ MAXIMIZE, },							/* COD_COMMON_MAXIMIZE   */
	{ FULLSCREEN, },						/* COD_COMMON_FULLSCREEN */    
	{ ROLL,        NULL, NULL, NULL, NULL, { { 0, FL_TOGGLE, }, } },/* COD_COMMON_ROLLTOGGLE */
	{ RAISELOWER, },						/* COD_COMMON_RAISELOWER */
	{ FOCUS,       NULL, NULL, NULL, NULL, { { 0, FL_OFF, }, } },	/* COD_COMMON_UNFOCUS    */
	{ WINDOW,      NULL, NULL, NULL, NULL, { { '+', 0, 1, }, } },	/* COD_COMMON_NEXT       */
	{ WINDOWLIST, },						/* COD_COMMON_WINDOWLIST */
	{ REFRESH, },							/* COD_COMMON_REFRESH    */
	{ SYNTHETICKEY, },						/* COD_COMMON_HOTKEY     */
	{ CLOSE, }							/* COD_COMMON_CLOSE      */
    };
    static struct node *pN[] = {
	N, N+1, N+2, N+3, N+4, N+5, N+6, N+7, N+8, N+9, N+10, N+11, N+12, N+13
    };
    
    /* now user functions list */
    static struct node F[] = {
	/* AddToFunc _UnFocus ( Window 0; Focus Off;) */
	{ 0,      "_UnFocus", NULL, F+1, },
	{ WINDOW, NULL, N+8, NULL, NULL, { { 0, 0, } } }
    };
    
    /* now keyboard binds */
    static struct node K[] = {
	/* Key HOT_KEY Interactive Menu */
	{ 0, NULL, NULL, K+1, },
	{ INTERACTIVE, NULL, NULL, NULL, NULL, { { 0, MENU, }, } }
    };
    
    /* now mouse binds */
    static struct node M[] = {
	/* Mouse   1   0  Close */
	{ HOLD_LEFT|RELEASE_,  "0", M+1, N+13,NULL, { { CTX_0, } } },
	/* Mouse  123  1  RaiseLower */
	{ HOLD_ANY |RELEASE_,  "1", M+2, N+7, NULL, { { CTX_1, } } },
	/* Mouse  123  3  Roll Toggle */
	{ HOLD_ANY |RELEASE_,  "2", M+3, N+6, NULL, { { CTX_2, } } },
	/* Mouse  H1   TS Interactive Move */
	{ HOLD_LEFT|PRESS_,   "TS", M+4, N+0, NULL, { { CTX_TITLE|CTX_SIDE, } } },
	/* Mouse  H1   C  Interactive Resize */
	{ HOLD_LEFT|PRESS_,    "C", M+5, N+1, NULL, { { CTX_CORNER, } } },
	/* Mouse  H1   C  Interactive Scroll */
	{ HOLD_LEFT|PRESS_,    "B", M+6, N+2, NULL, { { CTX_BARS, } } },
	/* Mouse  H1   R _UnFocus */
	{ HOLD_LEFT|PRESS_,    "R", M+8, M+7, NULL, { { CTX_ROOT, } } },
	{ USERFUNC, "_UnFocus", },
	/* Mouse  H2  R  WindowList */
	{ HOLD_MIDDLE|PRESS_,  "R", M+9, N+10,NULL, { { CTX_ROOT, } } },
	/* Mouse  H3  A  Interactive Menu */
	{ HOLD_RIGHT|PRESS_,   "A", M+10,K+1, NULL, { { CTX_ANY, } } },
	/* Mouse  H1  M  Interactive Screen */
	{ HOLD_LEFT|PRESS_,    "M", NULL,M+11,NULL, { { CTX_MENU, } } },
	{ INTERACTIVE, NULL, NULL, NULL, NULL, { { 0, SCREEN, }, } }
    };
#ifdef CONF__UNICODE
# define UD_ARROW T_UTF_16_UP_DOWN_ARROW
#else
# define UD_ARROW '\x12'
#endif
    static button_vec V[] = {
	{ {'[',      ']'     },  0, TRUE, FALSE },
	{ {UD_ARROW, UD_ARROW}, -2, TRUE, FALSE },
	{ {'>',      '<'     }, -4, TRUE, FALSE }
    };

    byte *Seq = "";
    /*
     * this is really heavy on the compiler...
     * but it should be able to optimize it
     * to just the line with the correct TW_xxx key
     */
#define IS(key,len,seq) if (HOT_KEY == TW_##key) Seq = seq;
# include "hw_keys.h"
#undef  IS
    K[0].name = N[COD_COMMON_HOTKEY - COD_COMMON_FIRST].name = Seq;
    K[0].id   = N[COD_COMMON_HOTKEY - COD_COMMON_FIRST].x.f.a = HOT_KEY;

    WriteMem(Globals, 0, sizeof(Globals));
    FuncList  = F;
    KeyList   = K;
    MouseList = M;
    
    MenuBinds = pN;
    MenuBindsMax = COD_COMMON_LAST - COD_COMMON_FIRST + 1;
    GlobalsAreStatic = TRUE;

    WriteMem(All->ButtonVec, 0, sizeof(All->ButtonVec));
    CopyMem(V, All->ButtonVec, sizeof(V));
    
    All->SetUp->ButtonSelection = HOLD_LEFT;
    All->SetUp->ButtonPaste = HOLD_MIDDLE;
    All->SetUp->DeltaXShade = 3;
    All->SetUp->DeltaXShade = 2;

    if (USEDefaultCommonMenu()) {
	
	InitRCOptions();
	UpdateOptionWin();
	FillButtonWin();
	HideMenu(!!(All->SetUp->Flags & SETUP_MENU_HIDE));
	Act(DrawMenu,All->FirstScreen)(All->FirstScreen, 0, MAXDAT);
	
	return TRUE;
    }
    return FALSE;    
}

