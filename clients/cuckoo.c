/*
 *  cuckoo.c  --  twin reimplementation of xcuckoo
 *
 *  Copyright (C) 1999-2000 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#include <Tw/Twautoconf.h>

#ifdef TW_HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef TW_HAVE_SYS_RESOURCE_H
# include <sys/resource.h>
#endif

#ifdef TW_HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif



#include <Tw/Tw.h>
#include <Tw/Twerrno.h>

#include <Tw/Twstat.h>
#include <Tw/Twstat_defs.h>

#include "version.h"


TW_DECL_MAGIC(magic);

static void Error(char *name, uldat err) {
    fprintf(stderr, "%s: libTw error: %s%s\n", name,
	    TwStrError(err), TwStrErrorDetail(err, TwErrnoDetail));
}

static void Usage(char *name) {
    fprintf(stderr, "Usage: %s [OPTIONS]\n"
	    "Currently known options: \n"
	    " -h, --help              display this help and exit\n"
	    " -V, --version           output version information and exit\n"
	    " -d, --delay=<seconds>   delay in seconds between updates\n"
	    " -w, --window=<id>       window to place our egg into\n"
	    " -e <command>            run <command> instead of `time'\n"
	    "                         (must be last option)\n", name);
}

static void ShowVersion(void) {
    fprintf(stdout, "twcuckoo " TWIN_VERSION_STR "\n");
}

static uldat ReallocBuff(byte **buff, uldat min_len) {
    uldat len;
    *buff = TwReAllocMem(*buff, len = (min_len + min_len/4 + 10));
    return len;
}


static struct timeval Now;

static void GetNow(void) {
    gettimeofday(&Now, NULL);
}
    
static uldat BuiltinTime(byte builtin_egg[10]) {
    time_t t;
    struct tm *localt;
    
    GetNow();
    t = (time_t)Now.tv_sec;
    localt = localtime(&t);
    sprintf(builtin_egg, "%02d:%02d:%02d",
	    (int)localt->tm_hour & 31,
	    (int)localt->tm_min & 63,
	    (int)localt->tm_sec & 63);
    return strlen(builtin_egg);
}

static twindow wid = TW_NOID;
static tslist wlist = NULL;
static TW_CONST byte *wtitle;
static uldat wtitle_len;

static volatile int ReceivedFatalSignal = FALSE;

static TW_RETSIGTYPE SignalChild(int n) {
    pid_t pid;
    signal(SIGCHLD, SignalChild);
    while ((pid = wait3(NULL, WNOHANG, NULL)) != 0 && pid != (pid_t)-1)
	;
    TW_RETFROMSIGNAL(0);
}

static TW_RETSIGTYPE SignalFatal(int n) {
    ReceivedFatalSignal = n;
    signal(n, SIG_DFL);
    TW_RETFROMSIGNAL(0);
}

byte InitSignals(void) {
    signal(SIGCHLD, SignalChild);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGIO,   SIG_IGN);
    signal(SIGHUP,  SignalFatal);
    signal(SIGINT,  SignalFatal);
    signal(SIGQUIT, SignalFatal);
    signal(SIGILL,  SignalFatal);
    signal(SIGABRT, SignalFatal);
    signal(SIGBUS,  SignalFatal);
    signal(SIGFPE,  SignalFatal);
    signal(SIGSEGV, SignalFatal);
    signal(SIGTERM, SignalFatal);
    signal(SIGXCPU, SignalFatal);
    signal(SIGXFSZ, SignalFatal);
# ifdef SIGPWR
    signal(SIGPWR,  SignalFatal);
# endif
    return TRUE;
}

static byte Init(void) {
    return
	InitSignals() &&
	TwCheckMagic(magic) && TwOpen(NULL);
}

static void Cleanup(void) {
    if (wlist) {
	TwSetTitleWindow(wid, wtitle_len, wtitle);
	TwDeleteStat(wlist);
    }
    TwFlush();
    if (ReceivedFatalSignal)
	raise(ReceivedFatalSignal);
}

int main(int argc, char *argv[]) {
    byte builtin_egg[10];
    byte **cmd_args = NULL, *name = argv[0], *buff = NULL;
    TW_CONST byte *egg;
    uldat egg_len, buff_len = 0, buff_capacity = 0, delay = 1, err;
    tall aid = TW_NOID;
    tsfield wfield;

    argv++;
    argc--;

    TwMergeHyphensArgv(argc, argv);
    
    while (argc) {
	if (!strcmp(*argv, "-h") || !strcmp(*argv, "-help")) {
	    Usage(name);
	    return 0;
	} else if (!strcmp(*argv, "-V") || !strcmp(*argv, "-version")) {
	    ShowVersion();
	    return 0;
	} else if (argc > 1 && !strcmp(*argv, "-d")) {
	    delay = strtol(*++argv, NULL, 0);
	    argc--;
	} else if (!strncmp(*argv, "-delay=", 7)) {
	    delay = strtol(*argv + 7, NULL, 0);
	} else if (argc > 1 && !strcmp(*argv, "-w")) {
	    wid = strtol(*++argv, NULL, 0);
	    argc--;
	} else if (!strncmp(*argv, "-window=", 8)) {
	    wid = strtol(*argv + 8, NULL, 0);
	} else if (argc > 1 && !strcmp(*argv, "-e")) {
	    cmd_args = (byte **)argv;
	    cmd_args[0] = cmd_args[1];
	    break;
	} else {
	    fprintf(stderr, "%s: argument `%s' not recognized\n"
		    "\ttry `%s --help' for usage summary.\n", name, *argv, name);
	    return 1;
	}
	argv++;
	argc--;
    }

    if (!Init() || (aid = TwGetAll()) == TW_NOID) {
	Error(name, TwErrno);
	return 1;
    }
    
    /*
     * race! we cannot use Tw_StatL() here, since the Tw_* calls below
     * will overwrite the returned buffer. So we use Tw_CloneStatL(),
     * which copies the buffer in a private malloc:ed() area.
     */
    if (wid == TW_NOID || 
	(wlist = TwCloneStatL(wid, 1, TWS_window_Name)) == NULL ||
	wlist->N < 1 || (wfield = wlist->TSF)->hash != TWS_window_Name ||
	wfield->type != (TWS_vec|TWS_byte)) {
	
	fprintf(stderr, "%s: error: no such server window: 0x%X\n", name, wid);
	return 1;
    }
    
    wtitle = wfield->TWS_field_vecV;
    wtitle_len = wfield->TWS_field_vecL;

    while (!ReceivedFatalSignal && !TwInPanic()) {
	
	/* discard any message we may get... */
	while (TwPeekMsg())
	    TwReadMsg(FALSE);

	if (cmd_args == NULL) {
	    
	    egg_len = BuiltinTime(builtin_egg);
	    egg = builtin_egg;
	    
	} else {
	    /* TODO: run external command and collect its output */

	    fprintf(stderr, "%s: external commands not yet implemented!\n", name);
	    return 1;
	}
	
	if (buff_capacity < (buff_len = wtitle_len + egg_len + 1))
	    buff_capacity = ReallocBuff(&buff, buff_len);

	memcpy(buff, wtitle, wtitle_len);
	buff[wtitle_len] = ' ';
	memcpy(buff + wtitle_len + 1, egg, egg_len);
	
	TwSetTitleWindow(wid, buff_len, buff);
	TwFlush();
	
	GetNow();
	
# ifdef TW_HAVE_USLEEP
	usleep(delay * 1000000 - Now.tv_usec);
# else
	sleep(delay);
# endif

    }
    if ((err = TwErrno))
	Error(name, err);

    Cleanup();

    return TwInPanic() ? 1 : 0;
}

