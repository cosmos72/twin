/*
 *                    T W I N
 *
 *            a Text mode WINdow manager.
 *
 *                   Author:
 *      Massimiliano Ghilardi <max@linuz.sns.it>
 *
 *
 *  Copyright (C) 1993-2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include <signal.h>

#include "twin.h"
#include "data.h"
#include "builtin.h"
#include "methods.h"
#include "main.h"
#include "draw.h"
#include "resize.h"
#include "extreg.h"

#include "hw.h"
#include "common.h"
#include "hw_multi.h"
#include "scroller.h"
#include "util.h"
#include "remote.h"
#include "version.h"

#ifdef CONF_WM
# include "wm.h"
#endif
#ifdef CONF_SOCKET
# include "socket.h"
#endif
#ifdef CONF_TERM
# include "tterm.h"
#endif
#ifdef CONF__MODULES
# include "dl.h"
#endif

/*-------------*/

fd_set save_rfds, save_wfds;
int max_fds;
byte lenTWDisplay, *TWDisplay, *origTWDisplay, *origTERM, *origHW, *HOME;
byte **main_argv, **orig_argv;
uldat main_argv_usable_len;
byte flag_secure, flag_envrc;
byte *flag_secure_msg = "twin: cannot exec() external programs in secure mode.\n";

int (*OverrideSelect)(int n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

static timevalue *Now;

INLINE struct timeval *CalcSleepTime(struct timeval *sleeptime, msgport Port, timevalue *now) {
    byte got = 0;
    timevalue *call = &Port->CallTime;

    if (Port->WakeUp) {
	if (CmpTime(call, now) > 0) {
	    sleeptime->tv_sec = call->Seconds - now->Seconds;
	    if (call->Fraction >= now->Fraction) {
		sleeptime->tv_usec = (call->Fraction - now->Fraction) / (1 MicroSECs);
	    } else {
		sleeptime->tv_usec = (1 FullSECs + call->Fraction - now->Fraction) / (1 MicroSECs);
		sleeptime->tv_sec--;
	    }
	    got = 1;
	} else {
	    sleeptime->tv_sec = (time_t)0;
	    sleeptime->tv_usec = 0;
	    got = 2;
	}
    }
    
    if (got != 2) while (Port) {
	if (Port->FirstMsg) {
	    sleeptime->tv_sec = (time_t)0;
	    sleeptime->tv_usec = 0;
	    got = 2;
	    break;
	}
	Port=Port->Next;
    }
    
    return got ? sleeptime : (struct timeval *)0;
}

static msgport RunMsgPort(msgport CurrPort) {
    msgport NextPort;

    if ((CurrPort->WakeUp & (TIMER_ALWAYS|TIMER_ONCE)) &&
	CmpTime(&CurrPort->CallTime, Now) <= 0)
	CurrPort->WakeUp &= ~TIMER_ONCE;
    else if (!CurrPort->FirstMsg)
	return NULL;
    
    All->RunMsgPort = CurrPort;
    
    if (CurrPort->Handler) {
	CurrPort->Handler(CurrPort);
	
	if (All->RunMsgPort == CurrPort) {
	    if (CurrPort->WakeUp & (TIMER_ALWAYS|TIMER_ONCE))
		SumTime(&CurrPort->CallTime, Now, &CurrPort->PauseDuration);
		
	    /* get ->Next *BEFORE* re-sorting!!! */
	    NextPort = CurrPort->Next;
	    SortMsgPortByCallTime(CurrPort);
	} else
	    NextPort = All->RunMsgPort;
    } else {
	DeleteList(CurrPort->FirstMsg);
	NextPort=CurrPort->Next;
    }
    return NextPort;
}

static void Usage(void) {
    fputs("Usage: twin [OPTIONS]\n"
	  "Currently known options: \n"
	  " -h, --help               display this help and exit\n"
	  " -V, --version            output version information and exit\n"
	  " --secure                 forbid starting external programs\n"
	  " --envrc                  execute .twenvrc.sh and read its output to set\n"
	  "                          environment variables (mostly useful for twdm)\n"
	  " -s, --share              start display as shared (default)\n"
	  " -x, --excl               start display as exclusive\n"
	  " --nohw                   start in background without display\n"
	  " --hw=<display>[,options] start with the given display (multiple --hw=... allowed)\n"
	  "                          (default: autoprobe all displays until one succeeds)\n"
	  "Currently known display drivers: \n"
	  "\tgfx[@<XDISPLAY>]\n"
	  "\tX[@<XDISPLAY>]\n"
	  "\ttwin[@<TWDISPLAY>]\n"
	  "\ttty[@<tty device>]\n"
	  "\tggi[@<ggi display>]\n", stdout);
}

static void ShowVersion(void) {
    fputs("twin " TWIN_VERSION_STR " with socket protocol "
	  TW_PROTOCOL_VERSION_STR "\n", stdout);
}

static byte Check4SpecialArgs(void) {
    if (main_argv[1] && !main_argv[2]) {
	if (!strcmp(main_argv[1], "-h") || !strcmp(main_argv[1], "-help")) {
	    Usage();
	    return TRUE;
	} else if (!strcmp(main_argv[1], "-V") || !strcmp(main_argv[1], "-version")) {
	    ShowVersion();
	    return TRUE;
	}
    }
    return FALSE;
}

#if !defined(CONF_WM)
static byte DieWMSo(void) {
#if defined(CONF__MODULES)
    printk("twin: fatal: failed to load the window manager: %."STR(SMALLBUFF)"s\n", ErrStr);
#else
    printk("twin: fatal: no window manager and no module loader compiled in.\n"
	   "      Where should I get the window manager from!?\n");
#endif
    flushk();
    return FALSE;
}
#endif

static byte Init(void) {
    FD_ZERO(&save_rfds);
    FD_ZERO(&save_wfds);

    OverrideSelect = select;

    CheckPrivileges();
    
    if ((origTWDisplay = getenv("TWDISPLAY")))
	origTWDisplay = strdup(origTWDisplay);
    if ((origTERM = getenv("TERM")))
	origTERM = strdup(origTERM);

    /*
     * WARNING:
     * 
     * do not touch the order of the following InitXXX() calls
     * unless you *REALLY* know what you're doing.
     * You may have twin SEGFAULT at startup or (worse) introduce subtle bugs!
     */
    
    return (   InitData()
	    && InitSignals()
	    && InitTWDisplay()
	    && (All->AtQuit = QuitTWDisplay)
	    && InitTransUser()
	    && InitTtysave()
	    && InitScroller()
	    && InitBuiltin()
	    && InitHW()
	    /*
	     * We need care here: DrawArea2(), DrawMenu(), etc. all need All->BuiltinMenu and
	     * also Video[]. The former is initialized by InitBuiltin(), the latter by InitHW().
	     * No DrawArea2() are allowed at all before InitHW() !
	     */
	    && InitDraw()   
#ifdef CONF_WM
	    && InitWM()
#elif defined(CONF__MODULES)
	    && (DlLoad(WMSo) || DieWMSo())
#else
	    && DieWMSo()
#endif
#ifdef CONF_TERM
	    && InitTerm()
#endif
#ifdef CONF_SOCKET
	    && InitSocket()
#endif
	   );
}
    
void Quit(int status) {
#if 0
    module M;
#endif
    
    RemoteFlushAll();
    
    if (All->AtQuit)
	All->AtQuit();
    
    SuspendHW(FALSE);
    /* not QuitHW() as it would fire up socket.so and maybe fork() in bg */
    
#if 0
    M = All->FirstModule;
    while (M) {
	Act(DlClose,M)(M);
	M = M->Next;
    }
#endif
    if (status < 0)
	return; /* give control back to signal handler */
    exit(status);
}

void NoOp(void) {
}

byte AlwaysTrue(void) {
    return TRUE;
}

byte AlwaysFalse(void) {
    return FALSE;
}

void *AlwaysNull(void) {
    return NULL;
}

static void MergeHyphensArgv(int argc, char **argv) {
    char *S;
    while (argc) {
	if ((S = *argv) && S[0] == '-' && S[1] == '-' && S[2] && S[3])
	    (*argv)++;
	argv++, argc--;
    }
}

#define MAXDELAY 50 MilliSECs
#define MINDELAY 10000

int main(int argc, char *argv[]) {
    msgport CurrPort;
    timevalue Old, Cut;
    fd_set read_fds, write_fds, *pwrite_fds;
    struct timeval sel_timeout, *this_timeout;
    int num_fds;
    
    DropPrivileges();

    main_argv_usable_len = ComputeUsableLenArgv(main_argv = (byte **)argv);
    
    MergeHyphensArgv(argc, argv);

#ifdef CONF__ALLOC
    /* do this as soon as possible */
    if (!InitAlloc()) {
	fputs("twin: InitAlloc() failed: internal error!\n", stderr);
	return 1;
    }
#endif
    if (Check4SpecialArgs())
	return 0;

    if (!(orig_argv = CloneStrList(main_argv+1))) {
	fputs("twin: Out of memory!\n", stderr);
	return 1;
    }

    Now = &All->Now;
    InstantNow(Now); /* needed by various InitXXX() */

    if (!Init())
	Quit(0);

    /* not needed... done by InitHW() */
    /* QueuedDrawArea2FullScreen = TRUE; */
    
    InstantNow(Now); /* read again... */
    SortAllMsgPortsByCallTime();

    /*------------------------ Main Loop -------------------------*/

    for (;;) {
	RemoteFlushAll();
	
	/*
	 * we could call InstantNow(Now) here, to get a more accurate
	 * sleep time and don't lag behing, but that doesn't make much sense
	 * since in the first place if we are lagging behing it means
	 * the system is very heavily loaded and our 'more accurate sleep time'
	 * would just further increase the load
	 */
	this_timeout = CalcSleepTime(&sel_timeout, All->FirstMsgPort, Now);

	if (ExpensiveFlushVideo) {
	    /* decide what to do... sleep a little if we can (HW_DELAY),
	     * otherwise revert to one of the other strategies */
	    if (!ChangedVideoFlagAgain || StrategyDelay >= MAXDELAY) {
		StrategyDelay = (tany)0;
		if (StrategyFlag == HW_DELAY)
		    StrategyFlag = HW_BUFFER;
	    } else {
		if ((!this_timeout && (this_timeout = &sel_timeout))
		    || this_timeout->tv_sec || this_timeout->tv_usec > MINDELAY) {
		    this_timeout->tv_sec = (tany)0;
		    this_timeout->tv_usec = MINDELAY;
		}
		StrategyFlag = HW_DELAY;
		ChangedVideoFlagAgain = FALSE;
	    }
	}
	
	do {
	    /* synchronously handle signals */
	    if (GotSignals)
		HandleSignals();
	    
	    if (NeedHW & NEEDResizeDisplay) {
		ResizeDisplay();
		QueuedDrawArea2FullScreen = TRUE;
	    }
	    
	    if (NeedHW & NEEDSelectionExport)
		SelectionExport();
	    
	    /*
	     * A display HW could Panic in its Event handlers or in its
	     * FlushVideo/FlushHW routines. So check for Panic BEFORE or AFTER
	     * FlushHW? Consider that after a HW shutdown due to Panic,
	     * all displays could get resized and thus need another FlushHW.
	     * I my opinion, it's better to keep open a paniced display HW for a while,
	     * than not to flush all remaining displays which just resized.
	     * 
	     * So first call PanicHW(), then FlushHW()
	     */
	    if (NeedHW & NEEDPanicHW)
		PanicHW();

	    if (StrategyFlag != HW_DELAY)
		FlushHW();
	    
	    if (NeedHW & NEEDPanicHW || All->FirstMsgPort->FirstMsg) {
		/*
		 * hmm... displays are rotting quickly today!
		 * we called PanicHW() just above, so don't call again,
		 * just set a zero timeout and let the above call do the work
		 * on next loop.
		 */
		sel_timeout.tv_sec = sel_timeout.tv_usec = 0;
		this_timeout = &sel_timeout;
	    }
	    
	    read_fds = save_rfds;

	    if (!FdWQueued)
		pwrite_fds = NULL;
	    else {
		write_fds = save_wfds;
		pwrite_fds = &write_fds;
	    }
	    num_fds = OverrideSelect(max_fds+1, &read_fds, pwrite_fds, NULL, this_timeout);
	    
	} while (num_fds < 0 && errno == EINTR);

	if (num_fds < 0)
	    /* ach, problem. */
	    num_fds = 0, RemoteParanoia();
	
	if (StrategyFlag == HW_DELAY) {
	    /* keep track of how much we are delaying the flush */
	    CopyMem(Now, &Old, sizeof(timevalue));
	    InstantNow(Now);
	    SubTime(&Cut, Now, &Old);
	    if (Cut.Seconds || Cut.Fraction >= MAXDELAY)
		StrategyDelay = MAXDELAY;
	    else
		StrategyDelay += Cut.Fraction;
	} else {
	    InstantNow(Now);
	    StrategyReset();
	}

	/*
	 * handle mouse, keyboard and connections to other programs
	 * (both in tty:s and Twin native connections)
	 */
	if (num_fds)
	    RemoteEvent(num_fds, &read_fds);
	    
	/*
	 * handle local events: messages arrived in msgports
	 * and timeout expires
	 */
	
	/*
	 * run WM_MsgPort first since it has to dispatch mouse/keyboard events
	 */
	(void)RunMsgPort(Ext(WM,MsgPort));
	
	/*
	 * MsgPorts are ordered: runnable ones first, then non-runnable ones.
	 * So bail out at the first non-runnable port.
	 */
	CurrPort=All->FirstMsgPort;
	
	while (CurrPort)
	    CurrPort = RunMsgPort(CurrPort);

	All->RunMsgPort=(msgport)0;
    }
    /* NOTREACHED */
    return 0;
}

