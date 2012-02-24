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
#include <sys/stat.h>

#include <Tw/Twautoconf.h>

#ifdef TW_HAVE_SYS_UTSNAME_H
# include <sys/utsname.h>
#endif

#include <Tw/Tw.h>
#include <Tw/Twerrno.h>

tmsgport SysMon_MsgPort;
tmenu SysMon_Menu;
twindow SysMon_Win;

byte numeric = 1;

char buf[TW_BIGBUFF];

TW_DECL_MAGIC(sysmon_magic);

byte InitSysMon(int argc, char ** argv) {
    byte *name;
    ldat len;
    byte border = 1;
    
#if defined(TW_HAVE_SYS_UTSNAME_H) && defined(TW_HAVE_UNAME)
    struct utsname uts;
    
    if (uname(&uts) >= 0 && (name = malloc(9 + (len = strlen(uts.nodename))))) {
	memcpy(name, uts.nodename, len);
	memcpy(name+len, " Monitor", 8);
	len += 8;
    } else
#endif
    if ((name = strdup("System Monitor")))
	len = 14;
    else
	return FALSE;
    
    name[len] = '\0';

    while (--argc) {
	if (!strcmp(*++argv, "+numeric"))
	    numeric = 1;
	else if (!strcmp(*argv, "-numeric"))
	    numeric = 0;
	else if (!strcmp(*argv, "+border"))
	    border = 1;
	else if (!strcmp(*argv, "-border"))
	    border = 0;
    }
    
    return
	TwCheckMagic(sysmon_magic) && TwOpen(NULL) &&
	
	(SysMon_MsgPort=TwCreateMsgPort(8, "twsysmon")) &&
	(SysMon_Menu=TwCreateMenu
	 (COL(BLACK,WHITE), COL(BLACK,GREEN), COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK),
	  COL(RED,WHITE), COL(RED,GREEN), (byte)0)) &&
	TwItem4MenuCommon(SysMon_Menu) &&
	
	(SysMon_Win = TwCreateWindow
	 (len, name, NULL, SysMon_Menu, COL(HIGH|YELLOW,BLUE),
	  TW_NOCURSOR, TW_WINDOW_DRAG|TW_WINDOW_CLOSE, (border ? 0 : TW_WINDOWFL_BORDERLESS),
	  numeric ? 29 : 24, 5, 0)) &&

	(TwSetColorsWindow(SysMon_Win, 0x1FF,
			   (hwcol)0x3F, (hwcol)0, (hwcol)0, (hwcol)0, (hwcol)0x9F,
			   (hwcol)0x17, (hwcol)0x3F, (hwcol)0x18, (hwcol)0x08),
	 TwInfo4Menu(SysMon_Menu, TW_ROW_ACTIVE, 16, " System Monitor ", "pppppppppppppppp"),
	 TwWriteAsciiWindow(SysMon_Win, 26, "CPU \nDISK\nMEM \nSWAP\nUPTIME"),
	 TwMapWindow(SysMon_Win, TwFirstScreen()),
	 TRUE);
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
	
	TwWriteAsciiWindow(SysMon_Win, len/scale/2 + !!frac, buf);
	
	half = COL(0, COLFG(Col));
	frac = len % (scale*2);
    }
    return frac;
}

void PrintPercent(hwcol Col, uldat percent) { 
    
    if (percent > 100)
	percent = 100;
    sprintf(buf, " %3d%%", percent);
    
    TwSetColTextWindow(SysMon_Win, Col);
    TwWriteAsciiWindow(SysMon_Win, 5, buf);
}

void PrintAbsoluteK(hwcol Col, unsigned long nK) { 
    unsigned long G =  nK >> 20;
    unsigned long M = (nK >> 10) & 0x3FF;
    unsigned long k =  nK & 0x3FF;
    unsigned long hi, lo;
    char label;

    if (G)
	hi = G, lo = M, label = 'G';
    else if (M)
	hi = M, lo = k, label = 'M';
    else
	hi = k, lo = 0, label = 'k';
    
	
    if (hi >= 10)
	sprintf(buf, " %3lu%c", hi, label);
    else if (hi || lo)
	sprintf(buf, " %1lu.%1lu%c", hi, lo/103, label);
    else
	sprintf(buf, "    0");
	
    TwSetColTextWindow(SysMon_Win, Col);
    TwWriteAsciiWindow(SysMon_Win, 5, buf);
}

void Update(void) {
    static int Fd;
    static unsigned long CpuUser[2], CpuNice[2], CpuSystem[2], CpuIdle[2],
	CpuWait[2], CpuHardInt[2], CpuSoftInt[2], CpuTotal;
    static unsigned long DiskR[2], DiskW[2], DiskMax;
    static unsigned long MemUsed, MemShared, MemBuff, MemCache, MemFree, MemTotal;
    static unsigned long SwapUsed, SwapFree, SwapTotal;
    static byte i;
    uldat len, tmp;
    char *s, *e, *e2;
    
    if ((Fd = open("/proc/stat", O_RDONLY)) != TW_NOFD) {
	s = buf;
	len = 0;
	while ((tmp = read(Fd, s, TW_BIGBUFF - len)) > 0)
	    len += tmp, s += tmp;
	buf[len] = '\0';
	if ((s = strstr(buf, "cpu "))) {
	    CpuUser[i] = strtoul(s+4, &e, 0) - CpuUser[!i];
	    CpuNice[i] = strtoul(e, &s, 0) - CpuNice[!i];
	    CpuSystem[i] = strtoul(s, &e, 0) - CpuSystem[!i];
	    CpuIdle[i] = strtoul(e, &s, 0) - CpuIdle[!i];
	    /* linux kernel 2.6 cpu stats: */
	    CpuWait[i] = strtoul(s, &e, 0) - CpuWait[!i];
	    CpuHardInt[i] = strtoul(e, &s, 0) - CpuHardInt[!i];
	    CpuSoftInt[i] = strtoul(s, &e, 0) - CpuSoftInt[!i];
	}
	CpuTotal = CpuUser[i] + CpuNice[i] + CpuSystem[i] + CpuIdle[i]
	    + CpuWait[i] + CpuHardInt[i] + CpuSoftInt[i];

	/* linux kernel 2.2 disk stats: */
	if ((s = strstr(buf, "disk_rio "))) {
	    DiskR[i] = strtoul(s+9, &e, 0);
	    DiskR[i] += strtoul(e, &s, 0);
	    DiskR[i] += strtoul(s, &e, 0);
	    DiskR[i] += strtoul(e, &s, 0) - DiskR[!i];
	}
	if ((s = strstr(buf, "disk_wio "))) {
	    DiskW[i] = strtoul(s+9, &e, 0);
	    DiskW[i] += strtoul(e, &s, 0);
	    DiskW[i] += strtoul(s, &e, 0);
	    DiskW[i] += strtoul(e, &s, 0) - DiskW[!i];
	}
	if ((s = strstr(buf, "disk_io: "))) {
	    /* linux kernel 2.4 disk stats: */
	    s += 10;
	    DiskR[i] = DiskW[i] = 0;
	    while (s && (s = strstr(s, "):("))) {
		if ((s = strchr(s+3, ',')) && (s = strchr(s+1, ','))) {
		    DiskR[i] += strtoul(s+1, &e, 0);
		    if (*e == ',' && (e = strchr(e+1, ',')))
			DiskW[i] += strtoul(e+1, &s, 0);
		}
	    }
	    DiskR[i] -= DiskR[!i];
	    DiskW[i] -= DiskW[!i];
	}

	close(Fd);
    }

    if ((Fd = open("/proc/diskstats", O_RDONLY)) != TW_NOFD) {
	/* linux kernel 2.6 disk stats: */

	s = buf;
	len = 0;
	while ((tmp = read(Fd, s, TW_BIGBUFF - len)) > 0)
	    len += tmp, s += tmp;
	buf[len] = '\0';
	s = buf;

	DiskR[i] = DiskW[i] = 0;

	while ((e = strstr(s, " hd")), (e2 = strstr(s, " sd")), e || e2) {
            if (e && (!e2 || e < e2))
                s = e+3;
            else
                s = e2+3;
	    
	    if (s[0] != ' ' && s[1] == ' ') {
		/* match hd? and sd? */
		strtoul(s+2, &s, 0);           /* skip  'reads'         */
		strtoul(s, &s, 0);             /* skip  'read_merges'   */
		DiskR[i] += strtoul(s, &s, 0); /* parse 'read_sectors'  */
		strtoul(s, &s, 0);             /* skip  'read_ticks'    */
		strtoul(s, &s, 0);             /* skip  'writes'        */
		strtoul(s, &s, 0);             /* skip  'write_merges'  */
		DiskW[i] += strtoul(s, &s, 0); /* parse 'write_sectors' */
	    }
	}

	DiskR[i] -= DiskR[!i];
	DiskW[i] -= DiskW[!i];
	
	close(Fd);
    }

    if (DiskMax) {
	if (DiskMax < DiskR[i] + DiskW[i])
	    DiskMax = DiskR[i] + DiskW[i];
    } else {
	/* first cycle... cannot know bandwidth */
	DiskR[!i] = DiskR[i];
	DiskW[!i] = DiskW[i];
	DiskR[i] = DiskW[i] = 0;
	DiskMax = 1;
    }

    if ((Fd = open("/proc/meminfo", O_RDONLY)) != TW_NOFD) {
	s = buf;
	len = 0;
	while ((tmp = read(Fd, s, TW_BIGBUFF - len)) > 0)
	    len += tmp, s += tmp;
	buf[len] = '\0';
	if ((s = strstr(buf, "MemTotal:")))
	    MemTotal = strtoul(s+9, &e, 0), s = e;
	if ((s = strstr(buf, "MemFree:")))
	    MemFree = strtoul(s+8, &e, 0), s = e;
	if ((s = strstr(buf, "MemShared:")))
	    MemShared = strtoul(s+10, &e, 0), s = e;
	if ((s = strstr(buf, "Buffers:")))
	    MemBuff = strtoul(s+8, &e, 0), s = e;
	if ((s = strstr(buf, "Cached:")))
	    MemCache = strtoul(s+8, &e, 0), s = e;

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
	    SwapTotal = strtoul(s+10, &e, 0), s = e;
	if ((s = strstr(buf, "SwapFree:")))
	    SwapFree = strtoul(s+9, &e, 0), s = e;
	SwapUsed = SwapTotal - SwapFree;

	close(Fd);
    }

    if (CpuTotal) {
	TwGotoXYWindow(SysMon_Win, 4, 0);
	if (numeric)
	    PrintPercent(COL(HIGH|YELLOW,BLUE), 100 * (CpuTotal - CpuIdle[i] - CpuWait[i]) / CpuTotal);
	tmp = HBar(COL(HIGH|GREEN,0), CpuUser[i],   CpuTotal, 0);
	tmp = HBar(COL(HIGH|YELLOW,0),CpuNice[i],   CpuTotal, tmp);
	tmp = HBar(COL(HIGH|RED,0),   CpuSystem[i], CpuTotal, tmp);
	tmp = HBar(COL(HIGH|MAGENTA,0),CpuHardInt[i],CpuTotal, tmp);
	tmp = HBar(COL(HIGH|CYAN,0),  CpuSoftInt[i],CpuTotal, tmp);
	tmp = HBar(COL(HIGH|BLUE,0),  CpuWait[i],   CpuTotal, tmp);
	(void)HBar(COL(BLUE,0),       CpuIdle[i],   CpuTotal, tmp);
    }
    if (DiskMax) {
	TwGotoXYWindow(SysMon_Win, 4, 1);
	if (numeric)
	    PrintAbsoluteK(COL(HIGH|YELLOW,BLUE), (DiskR[i]+DiskW[i])>>1);
	tmp = HBar(COL(HIGH|GREEN,0), DiskR[i], DiskMax, 0);
	tmp = HBar(COL(HIGH|RED,0),   DiskW[i], DiskMax, tmp);
	(void)HBar(COL(BLUE,0),       DiskMax - DiskR[i] - DiskW[i], DiskMax, tmp);
    }
    if (MemTotal) {
	TwGotoXYWindow(SysMon_Win, 4, 2);
	if (numeric)
	    PrintAbsoluteK(COL(HIGH|YELLOW,BLUE), (MemTotal-MemFree));
	tmp = HBar(COL(HIGH|GREEN,0), MemUsed,   MemTotal, 0);
	tmp = HBar(COL(HIGH|CYAN,0),  MemShared, MemTotal, tmp);
	tmp = HBar(COL(HIGH|YELLOW,0),MemBuff,   MemTotal, tmp);
	tmp = HBar(COL(HIGH|RED,0),   MemCache,  MemTotal, tmp);
	(void)HBar(COL(BLUE,0),       MemFree,   MemTotal, tmp);
    }
    if (SwapTotal) {
	TwGotoXYWindow(SysMon_Win, 4, 3);
	if (numeric)
	    PrintAbsoluteK(COL(HIGH|YELLOW,BLUE), SwapUsed);
	tmp = HBar(COL(HIGH|GREEN,0), SwapUsed,  SwapTotal, 0);
	(void)HBar(COL(BLUE,0),       SwapFree,  SwapTotal, tmp);
    }

    TwSetColTextWindow(SysMon_Win, COL(WHITE,BLUE));

    /*
     * --- Uptime ---
     * Print to SysMon Window
     * added by Mohammad Bahathir Hashim <bakhtiar@softhome.net>
     */
    if ((Fd = open("/proc/uptime", O_RDONLY)) != TW_NOFD) {

	unsigned long updays;
	int uphours, upminutes;

	s = buf;
	len = 0;
	while ((tmp = read(Fd, s, TW_BIGBUFF - len)) > 0)
	    len += tmp, s += tmp;
	buf[len] = '\0';

	updays = strtoul(buf, NULL, 0);
	/*upseconds = updays % 60;*/
	upminutes = (updays /= 60) % 60;
	uphours = (updays /= 60) % 24;
	updays /= 24;
	
	sprintf(buf, "%d days %2d:%02d", (int)updays, uphours, upminutes);

	TwGotoXYWindow(SysMon_Win, 8, 4);
	TwWriteAsciiWindow(SysMon_Win, strlen(buf), buf);

	close(Fd);
    }

    CpuUser[i] += CpuUser[!i];
    CpuNice[i] += CpuNice[!i];
    CpuSystem[i] += CpuSystem[!i];
    CpuIdle[i] += CpuIdle[!i];
    CpuWait[i] += CpuWait[!i];
    CpuHardInt[i] += CpuHardInt[!i];
    CpuSoftInt[i] += CpuSoftInt[!i];
    
    DiskR[i] += DiskR[!i];
    DiskW[i] += DiskW[!i];
    
    i = !i;
}

#define Quit() \
    do { \
	if ((err = TwErrno)) { \
	    printf("%s: libTw error: %s%s\n", argv[0], \
		       TwStrError(err), TwStrErrorDetail(err, TwErrnoDetail)); \
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
    uldat err;
    
    if (!InitSysMon(argc, argv))
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
	    if (Event->EventCommon.W == SysMon_Win) {
		if (Msg->Type==TW_MSG_WIDGET_GADGET) {
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
