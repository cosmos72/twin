/*
 *  sysmon.c  --  monitor system resources (CPU, Disk, Memory, Swap).
 *                draws in a twin window.
 *
 *  This program is placed in the public domain.
 * 
 * Uptime function added by Mohammad Bahathir Hashim <bakhtiar@softhome.net>
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/utsname.h>

#include <libTw.h>

tmsgport SysMon_MsgPort;
tmenu SysMon_Menu;
twindow SysMon_Win;

char buf[BIGBUFF];

byte InitSysMon(void) {
    struct utsname uts;
    byte *name;
    uldat len;
    
    if (uname(&uts) >= 0 && (name = malloc(11 + (len = strlen(uts.nodename))))) {
	name[0] = ' ';
	memcpy(name+1, uts.nodename, len);
	memcpy(name+1+len, " Monitor ", 9);
	len += 10;
    } else if ((name = strdup(" System Monitor ")))
	len = 16;
    else
	return FALSE;
    
    name[len] = '\0';
	
    if (TwOpen(NULL) &&
	
	(SysMon_MsgPort=TwCreateMsgPort
	 (len-2, name+1, (uldat)0, (udat)0, (byte)0)) &&
	(SysMon_Menu=TwCreateMenu
	 (SysMon_MsgPort,
	  COL(BLACK,WHITE), COL(BLACK,GREEN), COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK),
	  COL(RED,WHITE), COL(RED,GREEN), (byte)0)) &&
	TwItem4MenuCommon(SysMon_Menu) &&
	
	(SysMon_Win = TwCreateWindow
	 (len, name, NULL, SysMon_Menu, COL(HIGH|YELLOW,BLUE),
	  TW_NOCURSOR, TW_WINDOW_DRAG|TW_WINDOW_CLOSE, 0,
	  (udat)26, (udat)7, (udat )0))) {

	TwSetColorsWindow(SysMon_Win, 0x1FF,
			  (hwcol)0x3E, (hwcol)0, (hwcol)0, (hwcol)0, (hwcol)0x9F,
			  (hwcol)0x1E, (hwcol)0x3E, (hwcol)0x18, (hwcol)0x08);
	TwConfigureWindow(SysMon_Win, 0xF<<2, 0, 0, (udat)26, (udat)7, (udat)26, (udat)7);

	TwInfo4Menu(SysMon_Menu, TW_ROW_ACTIVE, (uldat)16, " System Monitor ", "pppppppppppppppp");
	TwWriteRowWindow(SysMon_Win, 26, "CPU \nDISK\nMEM \nSWAP\nUPTIME");
	TwMapWindow(SysMon_Win, TwFirstScreen());
	return TRUE;
    }
    return FALSE;
}


#define WIDTH 40

uldat HBar(hwcol Col, uldat len, uldat scale, uldat frac) { 
    static hwcol half;
    byte *s = buf;
    
    if (len) {
	TwSetColTextWindow(SysMon_Win, Col|half);
	
	len *= WIDTH;
	len += frac;
	
	if (frac * 4 >= scale && len >= scale)
	    TwWriteMem(s++, 'Þ', 1), len -= scale;
	else
	    frac = 0;
	TwWriteMem(s, 'Û', len/scale/2);
	
	TwWriteRowWindow(SysMon_Win, len/scale/2 + !!frac, buf);
	
	half = COL(0, COLFG(Col));
	frac = len % (scale*2);
    }
    return frac;
}

void Update(void) {
    static int ProcStatFd = NOFD, ProcMeminfoFd = NOFD, ProcUptimeFd = NOFD;
    static uldat CpuUser[2], CpuNice[2], CpuSystem[2], CpuIdle[2], CpuTotal;
    static uldat DiskR[2], DiskW[2], DiskTotal;
    static uldat MemUsed, MemShared, MemBuff, MemCache, MemFree, MemTotal;
    static uldat SwapUsed, SwapFree, SwapTotal;
    static byte i;
    uldat len, tmp;
    char *s, *e;
    
    if (ProcStatFd != NOFD)
	close(ProcStatFd);
    ProcStatFd = open("/proc/stat", O_RDONLY);
    
    if (ProcMeminfoFd != NOFD)
	close(ProcMeminfoFd);
    ProcMeminfoFd = open("/proc/meminfo", O_RDONLY);

    if (ProcUptimeFd != NOFD)
	close(ProcUptimeFd);
    ProcUptimeFd = open("/proc/uptime", O_RDONLY);

    if (ProcStatFd != NOFD) {
	s = buf;
	len = 0;
	while ((tmp = read(ProcStatFd, s, BIGBUFF - len)) > 0)
	    len += tmp, s += tmp;
	buf[len] = '\0';
	if ((s = strstr(buf, "cpu "))) {
	    CpuUser[i] = strtol(s+4, &e, 0) - CpuUser[!i];
	    CpuNice[i] = strtol(e, &s, 0) - CpuNice[!i];
	    CpuSystem[i] = strtol(s, &e, 0) - CpuSystem[!i];
	    CpuIdle[i] = strtol(e, &s, 0) - CpuIdle[!i];
	}
	if ((s = strstr(buf, "disk_rio "))) {
	    DiskR[i] = strtol(s+9, &e, 0);
	    DiskR[i] += strtol(e, &s, 0);
	    DiskR[i] += strtol(s, &e, 0);
	    DiskR[i] += strtol(e, &s, 0) - DiskR[!i];
	}
	if ((s = strstr(buf, "disk_wio "))) {
	    DiskW[i] = strtol(s+9, &e, 0);
	    DiskW[i] += strtol(e, &s, 0);
	    DiskW[i] += strtol(s, &e, 0);
	    DiskW[i] += strtol(e, &s, 0) - DiskW[!i];
	}
	CpuTotal = CpuUser[i] + CpuNice[i] + CpuSystem[i] + CpuIdle[i];

	if (DiskTotal) {
	    if (DiskTotal < DiskR[i] + DiskW[i])
		DiskTotal = DiskR[i] + DiskW[i];
	} else {
	    /* first cycle... cannot know bandwidth */
	    DiskR[!i] = DiskR[i];
	    DiskW[!i] = DiskW[i];
	    DiskR[i] = DiskW[i] = 0;
	    DiskTotal = 1;
	}
    }
    if (ProcMeminfoFd != NOFD) {
	s = buf;
	len = 0;
	while ((tmp = read(ProcMeminfoFd, s, BIGBUFF - len)) > 0)
	    len += tmp, s += tmp;
	buf[len] = '\0';
	if ((s = strstr(buf, "MemTotal:")))
	    MemTotal = strtol(s+9, &e, 0), s = e;
	if ((s = strstr(buf, "MemFree:")))
	    MemFree = strtol(s+8, &e, 0), s = e;
	if ((s = strstr(buf, "MemShared:")))
	    MemShared = strtol(s+10, &e, 0), s = e;
	if ((s = strstr(buf, "Buffers:")))
	    MemBuff = strtol(s+8, &e, 0), s = e;
	if ((s = strstr(buf, "Cached:")))
	    MemCache = strtol(s+8, &e, 0), s = e;

	MemUsed = MemTotal - MemFree;	
	if (MemCache > MemUsed)
	    MemCache = MemUsed;
	MemUsed -= MemCache;
	if (MemBuff > MemUsed)
	    MemBuff = MemUsed;
	MemUsed -= MemBuff;
	if (MemShared > MemUsed)
	    MemShared = MemUsed;
	MemUsed -= MemShared;

	if ((s = strstr(buf, "SwapTotal:")))
	    SwapTotal = strtol(s+10, &e, 0), s = e;
	if ((s = strstr(buf, "SwapFree:")))
	    SwapFree = strtol(s+9, &e, 0), s = e;
	SwapUsed = SwapTotal - SwapFree;
    }
    
    if (CpuTotal) {
	TwGotoXYWindow(SysMon_Win, 4, 0);
	tmp = HBar(COL(HIGH|GREEN,0), CpuUser[i],   CpuTotal, 0);
	tmp = HBar(COL(HIGH|YELLOW,0),CpuNice[i],   CpuTotal, tmp);
	tmp = HBar(COL(HIGH|RED,0),   CpuSystem[i], CpuTotal, tmp);
	(void)HBar(COL(BLUE,0),       CpuIdle[i],   CpuTotal, tmp);
    }
    if (DiskTotal) {
	TwGotoXYWindow(SysMon_Win, 4, 1);
	tmp = HBar(COL(HIGH|GREEN,0), DiskR[i], DiskTotal, 0);
	tmp = HBar(COL(HIGH|RED,0),   DiskW[i], DiskTotal, tmp);
	(void)HBar(COL(BLUE,0),       DiskTotal - DiskR[i] - DiskW[i], DiskTotal, tmp);
    }
    if (MemTotal) {
	TwGotoXYWindow(SysMon_Win, 4, 2);
	tmp = HBar(COL(HIGH|GREEN,0), MemUsed,   MemTotal, 0);
	tmp = HBar(COL(HIGH|CYAN,0),  MemShared, MemTotal, tmp);
	tmp = HBar(COL(HIGH|YELLOW,0),MemBuff,   MemTotal, tmp);
	tmp = HBar(COL(HIGH|RED,0),   MemCache,  MemTotal, tmp);
	(void)HBar(COL(BLUE,0),       MemFree,   MemTotal, tmp);
    }
    if (SwapTotal) {
	TwGotoXYWindow(SysMon_Win, 4, 3);
	tmp = HBar(COL(HIGH|GREEN,0), SwapUsed,  SwapTotal, 0);
	(void)HBar(COL(BLUE,0),       SwapFree,  SwapTotal, tmp);
    }

    TwSetColTextWindow(SysMon_Win, COL(HIGH|YELLOW,BLUE));

/*
 * --- Uptime ---
 * Print to SysMon Window
 * added by Mohammad Bahathir Hashim <bakhtiar@softhome.net>
 */
    if (ProcUptimeFd) {
	unsigned long updays;
	int uphours, upminutes;

	s = buf;
	len = 0;
	while ((tmp = read(ProcUptimeFd, s, BIGBUFF - len)) > 0)
	    len += tmp, s += tmp;
	buf[len] = '\0';

	updays = strtoul(buf, NULL, 0);
	/*upseconds = updays % 60;*/
	upminutes = (updays /= 60) % 60;
	uphours = (updays /= 60) % 24;
	updays /= 24;
	
	sprintf(buf, "%d days %2d:%02d", (int)updays, uphours, upminutes);

	TwGotoXYWindow(SysMon_Win, 8, 4);
	TwWriteRowWindow(SysMon_Win, strlen(buf), buf);
    }

    CpuUser[i] += CpuUser[!i];
    CpuNice[i] += CpuNice[!i];
    CpuSystem[i] += CpuSystem[!i];
    CpuIdle[i] += CpuIdle[!i];
    DiskR[i] += DiskR[!i];
    DiskW[i] += DiskW[!i];
    
    i = !i;
}

#define Quit() \
    do { \
	if (TwErrno) { \
	    printf("%s: libTw error: %s\n", argv[0], TwStrError(TwErrno)); \
	    TwClose(); \
	    exit(1); \
	} \
	exit(0); \
    } while(0)

int main(int argc, char *argv[]) {
    tmsg Msg;
    tevent_any Event;
    struct timeval p = {0, 0};
    fd_set readfds;
    int fd;
    
    if (!InitSysMon())
	Quit();
    
    fd = TwConnectionFd();
    FD_ZERO(&readfds);

    for (;;) {
	if (p.tv_sec == 0 && p.tv_usec < 10000) {
	    Update();
	    TwFlush();
	    
	    p.tv_sec = 1;
	    p.tv_usec = 0;
	}

	while ((Msg = TwReadMsg(FALSE))) {
	    Event=&Msg->Event;
	    if (Event->EventCommon.Window == SysMon_Win) {
		if (Msg->Type==TW_MSG_WINDOW_GADGET) {
		    if (Event->EventGadget.Code == 0)
			/* 0 == Close Code */
			Quit();
		}
	    }
	}
	
	/* bail out if something goes *really* wrong */
	if (TwInPanic())
	    break;
	
	FD_SET(fd, &readfds);
	select(fd+1, &readfds, NULL, NULL, &p);
    }
    Quit();
}
