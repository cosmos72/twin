/*
 *  threadtest.c  --  test libTw thread-safety
 *
 */

#ifndef _REENTRANT
# define _REENTRANT /* for thread-safety */
#endif

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include <pthread.h>

#include "Tw/Tw.h"
#include "Tw/Twerrno.h"

tmsgport Thrd_MsgPort;
tmenu Thrd_Menu;
twindow Thrd_Win1, Thrd_Win2;

pthread_t t1, t2;

TW_DECL_MAGIC(threadtest_magic);

byte InitThrd(void) {
    if (TwCheckMagic(threadtest_magic) &&
	TwOpen(NULL) &&
	
	(Thrd_MsgPort=TwCreateMsgPort
	 (10, "threadtest", 0, 0, 0)) &&
	(Thrd_Menu=TwCreateMenu(
	  COL(BLACK,WHITE), COL(BLACK,GREEN), COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK),
	  COL(RED,WHITE), COL(RED,GREEN), (byte)0)) &&
	TwItem4MenuCommon(Thrd_Menu)) {
	
	TwInfo4Menu(Thrd_Menu, TW_ROW_ACTIVE, 13, " Thread Test ", "ppppppppppppp");
	return TRUE;
    }
    return FALSE;
}


static void Quit(void) {
    uldat err, detail;
    if ((err = TwErrno)) {
	detail = TwErrnoDetail;
	fprintf(stderr, "threadtest: libTw error: %s%s\n",
		TwStrError(err), TwStrErrorDetail(err, detail));
	TwClose();
	exit(1);
    }
    exit(0);
}

long seed;
int fd;
pthread_mutex_t T = PTHREAD_MUTEX_INITIALIZER;

void mainloop(twindow *Win) {
    tmsg Msg;
    tevent_any Event;
    /*struct timeval p = {0, 0};*/
    fd_set readfds;
    long x, y;

    FD_ZERO(&readfds);
    srand48(++seed);

    if (!(*Win = TwCreateWindow
	(11, Win == &Thrd_Win1 ? "threadtest1" : "threadtest2", NULL, Thrd_Menu, COL(HIGH|YELLOW,BLUE),
	 TW_NOCURSOR, TW_WINDOW_DRAG|TW_WINDOW_CLOSE|TW_WINDOW_RESIZE, TW_WINDOWFL_USECONTENTS,
	 31 + sizeof(long)/sizeof(hwattr), 16, 0)))
	Quit();

    TwMapWindow(*Win, TwFirstScreen());

    for (;;) {
	for (y = 256; y; y--) {
	    x = lrand48();
	    TwWriteHWAttrWindow(*Win, lrand48() >> 26, lrand48() >> 27,
				sizeof(long)/sizeof(hwattr), (hwattr *)&x);
	}
	
	/* bail out if something goes *really* wrong */
	if (!TwFlush())
	    Quit();
	
	/*
	 * use TwCloneReadMsg() instead of TwReadMsg()
	 * as the other thread is calling Tw* functions...
	 * they invalidate the static buffer used by TwReadMsg()
	 */
	while ((Msg = TwCloneReadMsg(FALSE))) {
	    Event=&Msg->Event;
	    if (Msg->Type==TW_MSG_WIDGET_GADGET) {
		if (Event->EventGadget.Code == 0)
		    /* 0 == Close Code */
		    Quit();
	    }
	    TwFreeMem(Msg);
	}
	/*
	FD_SET(fd, &readfds);
	p.tv_sec = 0;
	p.tv_usec = 10000;
	select(fd+1, &readfds, NULL, NULL, &p);
	 */
    }
}

int main(int argc, char *argv[]) {
    if (!InitThrd())
	Quit();

    fd = TwConnectionFd();

    seed = time(NULL);
    
    pthread_create(&t1, NULL, (void *)mainloop, &Thrd_Win2);
    usleep(1);
    mainloop(&Thrd_Win1);
    
    Quit();
    return 0;
}
