/*
 *                    T W I N
 *
 *            a Text mode WINdow manager.
 *
 *                   Author:
 *      Massimiliano Ghilardi <max@linuz.sns.it>
 *
 *
 *  Copyright (C) 1993-2000 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

#include "twin.h"
#include "methods.h"
#include "data.h"
#include "main.h"

#include "resize.h"
#include "draw.h"
#include "hw.h"
#include "common.h"
#include "hw_multi.h"
#include "builtin.h"
#include "wm.h"
#include "scroller.h"
#include "util.h"
#include "remote.h"

#ifdef CONF_SOCKET
#include "socket.h"
#endif

#ifdef CONF_TERM
#include "term.h"
#endif

/*-------------*/

fd_set save_rfds, save_wfds;
int max_fds;
byte lenTWDisplay, *TWDisplay, *origTWDisplay, *origTERM, *origHW;
byte **main_argv, **orig_argv;
byte ctty_InUse;

int (*OverrideSelect)(int n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

static timevalue *Now;

INLINE struct timeval *CalcSleepTime(struct timeval *sleeptime, msgport *Port, timevalue *now) {
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

static msgport *RunMsgPort(msgport *CurrPort) {
    msgport *NextPort;

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

static void CheckPrivileges(void) {
    char c;
    
    c = GetPrivileges() < 0;
    DropPrivileges();
    
    if (c) {
	fprintf(stderr, "twin: not running setuid root.\n"
			"      might be unable to start the terminal emulator.\n"
			"      hit RETURN to continue, or CTRL-C to quit.\n");
	fflush(stdout);
	read(0, &c, 1);
    }
}

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
     * You may introduce subtle bugs or plainly
     * have twin segfault at startup.
     */
    
    return (
	    InitSignals() &&
	    SetTWDisplay() &&
	    (All->AtQuit = UnSetTWDisplay) &&
	    InitData() &&
	    InitTransUser() &&
	    InitTtysave() &&
	    InitHW() && 
	    InitScroller() &&
	    InitWM() &&
#ifdef CONF_TERM
	    InitTerm() &&
#endif
#ifdef CONF_SOCKET
	    InitSocket() &&
#endif
	    InitBuiltin()
	    
#if 0
	    &&
	    InitTwEdit() &&
	    InitClock() &&
	    InitEmpty() &&
	    (!(All->SetUp->Flags & NEW_FONT) || SetNewFont()) &&
	    (!(All->SetUp->Flags & NEW_PALETTE) || SetPalette())
#endif
	    );
}
    
void Quit(int status) {
#if 0
    module *M;
#endif
    
    if (All->AtQuit)
	All->AtQuit();
    
    SuspendHW(FALSE);
    /* not QuitHW() as it would fire up socket.so and prepare to run with no HW */
    
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

#define MAXDELAY 50 MilliSECs
#define MINDELAY 10000

int main(int argc, char *argv[]) {
    msgport *CurrPort;
    timevalue Old, Cut;
    fd_set read_fds, write_fds, *pwrite_fds;
    struct timeval sel_timeout, *this_timeout;
    int num_fds;
    
    DropPrivileges();
    
    main_argv = (byte **)argv;
    
#ifdef CONF__ALLOC
    if (!InitAlloc()) {
	fputs("twin: InitAlloc() failed: internal error!\n", stderr);
	return 1;
    }
#endif
    if (!(orig_argv = CloneStrList(main_argv+1))) {
	fputs("twin: Out of memory!\n", stderr);
	return 1;
    }
    
    if (!Init())
	Quit(0);

    DrawArea(FULLSCREEN);
    
    Now = &All->Now;
    InstantNow(Now);
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
		StrategyDelay = (frac_t)0;
		if (StrategyFlag == HW_DELAY)
		    StrategyFlag = HW_BUFFER;
	    } else {
		if ((!this_timeout && (this_timeout = &sel_timeout))
		    || this_timeout->tv_sec || this_timeout->tv_usec > MINDELAY) {
		    this_timeout->tv_sec = (time_t)0;
		    this_timeout->tv_usec = MINDELAY;
		}
		StrategyFlag = HW_DELAY;
		ChangedVideoFlagAgain = FALSE;
	    }
	}
	
	do {
	    if (NeedHW & NEEDResizeDisplay) {
		ResizeDisplay();
		DrawArea(FULLSCREEN);
		UpdateCursor();
	    }
	    
	    if (NeedHW & NEEDSelectionExport)
		SelectionExport();
	    
	    /*
	     * A display HW could Panic in its Event handlers or in its
	     * FlushVideo/FlushHW routines. So check for Panic BEFORE or AFTER
	     * FlushHW? Consider that after a Panic, all displays could get resized
	     * and thus need a FlushHW.
	     * I my opinion, it's better to keep open a paniced display HW for a while,
	     * than not to flush all remaining displays which just resized.
	     * 
	     * So first call PanicHW(), then FlushHW()
	     */
	    if (NeedHW & NEEDPanicHW)
		PanicHW();

	    if (StrategyFlag != HW_DELAY)
		FlushHW();
	    
	    if (NeedHW & NEEDPanicHW) {
		/* hmm... displays are rotting quickly today */
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
	(void)RunMsgPort(WM_MsgPort);
	
	/*
	 * MsgPorts are ordered: runnable ones first, then non-runnable ones.
	 * So bail out at the first non-runnable port.
	 */
	CurrPort=All->FirstMsgPort;
	
	while (CurrPort)
	    CurrPort = RunMsgPort(CurrPort);

	All->RunMsgPort=(msgport *)0;
    }
    /* NOTREACHED */
    return 0;
}

