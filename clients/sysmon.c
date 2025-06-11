/*
 *  sysmon.c  --  monitor system resources (CPU, Disk, Memory, Swap).
 *                draws in a twin window.
 *
 *  This program is placed in the public domain.
 *
 * Uptime function added by Mohammad Bahathir Hashim <bakhtiar@softhome.net>
 */

#include "util.h"

#include <stdio.h>
#include <errno.h>

#include <Tw/Tw.h>
#include <Tw/Twerrno.h>

#ifdef TW_HAVE_STRING_H
#include <string.h>
#endif
#ifdef TW_HAVE_SYS_UTSNAME_H
#include <sys/utsname.h>
#endif
#ifdef TW_HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef TW_HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef TW_HAVE_UNISTD_H
#include <unistd.h>
#endif

static tmsgport SysMon_MsgPort;
static tmenu SysMon_Menu;
static twindow SysMon_Win;

static byte numeric = 1;

static char buf[TW_BIGBUFF + 1];

TW_DECL_MAGIC(sysmon_magic);

static byte InitSysMon(int argc, char **argv) {
  const tcolor b = TCOL(tblack, twhite);
  const tcolor color_array[16] = {b, b, b, b, b, b, b, b, b, b, b, b, b, b, b, b};
  char *name;
  ldat len;
  byte border = 1;
  byte ok;

#if defined(TW_HAVE_SYS_UTSNAME_H) && defined(TW_HAVE_UNAME)
  struct utsname uts;

  if (uname(&uts) >= 0 && (name = malloc(9 + (len = strlen(uts.nodename))))) {
    memcpy(name, uts.nodename, len);
    memcpy(name + len, " Monitor", 8);
    len += 8;
  } else
#endif
      if ((name = strdup("System Monitor"))) {
    len = 14;
  } else
    return tfalse;

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

  closeAllFds(1); // keep fd 1

  ok = TwCheckMagic(sysmon_magic) && TwOpen(NULL) &&

       (SysMon_MsgPort = TwCreateMsgPort(8, "twsysmon")) &&
       (SysMon_Menu =
            TwCreateMenu(TCOL(tblack, twhite), TCOL(tblack, tgreen), TCOL(tBLACK, twhite),
                         TCOL(tBLACK, tblack), TCOL(tred, twhite), TCOL(tred, tgreen), (byte)0)) &&
       TwItem4MenuCommon(SysMon_Menu) &&

       (SysMon_Win =
            TwCreateWindow(len, name, NULL, SysMon_Menu, TCOL(tYELLOW, tblue), TW_NOCURSOR,
                           TW_WINDOW_DRAG | TW_WINDOW_CLOSE, (border ? 0 : TW_WINDOWFL_BORDERLESS),
                           numeric ? 29 : 24, 5, 0)) &&

       (TwSetColorsWindow(SysMon_Win, 0x1FF, (tcolor)0x3F, (tcolor)0, (tcolor)0, (tcolor)0,
                          (tcolor)0x9F, (tcolor)0x17, (tcolor)0x3F, (tcolor)0x18, (tcolor)0x08),
        TwInfo4Menu(SysMon_Menu, TW_ROW_ACTIVE, 16, " System Monitor ", color_array),
        TwWriteCharsetWindow(SysMon_Win, 26, "CPU \nDISK\nMEM \nSWAP\nUPTIME"),
        TwMapWindow(SysMon_Win, TwFirstScreen()), ttrue);

  free(name);

  openDevNull();
  (void)setsid();
  ignoreSigHup();

  return ok;
}

#define WIDTH 40

static uldat HBar(tcolor Col, uldat len, uldat scale, uldat frac) {
  static tcolor half;
  char *s = buf;

  if (len) {
    TwSetColTextWindow(SysMon_Win, Col | half);

    len *= WIDTH;
    len += frac;

    if (frac * 4 >= scale && len >= scale)
      memset(s++, '\xDE', 1), len -= scale;
    else
      frac = 0;
    memset(s, '\xDB', len / scale / 2);

    TwWriteCharsetWindow(SysMon_Win, len / scale / 2 + !!frac, buf);

    half = TCOL(0, TCOLFG(Col));
    frac = len % (scale * 2);
  }
  return frac;
}

void PrintPercent(tcolor Col, uldat percent) {

  if (percent > 100)
    percent = 100;
  sprintf(buf, " %3d%%", percent);

  TwSetColTextWindow(SysMon_Win, Col);
  TwWriteCharsetWindow(SysMon_Win, 5, buf);
}

static void PrintAbsoluteK(tcolor Col, unsigned long nK) {
  unsigned long G = nK >> 20;
  unsigned long M = (nK >> 10) & 0x3FF;
  unsigned long k = nK & 0x3FF;
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
    sprintf(buf, " %1lu.%1lu%c", hi, lo / 103, label);
  else
    sprintf(buf, "    0");

  TwSetColTextWindow(SysMon_Win, Col);
  TwWriteCharsetWindow(SysMon_Win, 5, buf);
}

// read at most maxlenPlus1-1 bytes, and append '\0'
static uldat FullReadZ(int fd, char *dst, uldat maxlenPlus1) {
  ssize_t got;
  const uldat maxlen = maxlenPlus1 - 1;
  uldat left = maxlen;
  while (left) {
    while ((got = read(fd, dst, left)) == -1 && errno == EINTR) {
    }
    if (got <= 0) {
      break;
    } else if (got > (ssize_t)maxlen) {
      got = maxlen;
    }
    left -= got;
    dst += got;
  }
  *dst = '\0';
  return maxlen - left;
}

typedef struct s_cpu_usage {
  unsigned long User, Nice, System, Idle, Wait, HardInt, SoftInt, Total;
} cpu_usage;

typedef struct s_mem_usage {
  unsigned long Used, Shared, Buff, Cache, Free, Total;
} mem_usage;

typedef struct s_swap_usage {
  unsigned long Used, Free, Total;
} swap_usage;

typedef struct s_disk_usage {
  unsigned long Read, Write, Max;
} disk_usage;

typedef struct s_usage {
  cpu_usage Cpu;
  mem_usage Mem;
  swap_usage Swap;
  disk_usage Disk;
} usage;

static byte Update(byte i, usage Usage[2]) {
  usage *U = &Usage[i];
  usage *V = &Usage[!i];
  char *s, *e, *e2, *e3;
  uldat tmp;
  int fd;

  if ((fd = open("/proc/stat", O_RDONLY)) != TW_NOFD) {
    (void)FullReadZ(fd, buf, sizeof(buf));
    if ((s = strstr(buf, "cpu "))) {
      U->Cpu.User = strtoul(s + 4, &e, 0) - V->Cpu.User;
      U->Cpu.Nice = strtoul(e, &s, 0) - V->Cpu.Nice;
      U->Cpu.System = strtoul(s, &e, 0) - V->Cpu.System;
      U->Cpu.Idle = strtoul(e, &s, 0) - V->Cpu.Idle;
      /* linux kernel 2.6 cpu stats: */
      U->Cpu.Wait = strtoul(s, &e, 0) - V->Cpu.Wait;
      U->Cpu.HardInt = strtoul(e, &s, 0) - V->Cpu.HardInt;
      U->Cpu.SoftInt = strtoul(s, &e, 0) - V->Cpu.SoftInt;
    }
    U->Cpu.Total = U->Cpu.User + U->Cpu.Nice + U->Cpu.System + U->Cpu.Idle + U->Cpu.Wait +
                   U->Cpu.HardInt + U->Cpu.SoftInt;

    /* linux kernel 2.2 disk stats: */
    if ((s = strstr(buf, "disk_rio "))) {
      U->Disk.Read = strtoul(s + 9, &e, 0);
      U->Disk.Read += strtoul(e, &s, 0);
      U->Disk.Read += strtoul(s, &e, 0);
      U->Disk.Read += strtoul(e, &s, 0) - V->Disk.Read;
    }
    if ((s = strstr(buf, "disk_wio "))) {
      U->Disk.Write = strtoul(s + 9, &e, 0);
      U->Disk.Write += strtoul(e, &s, 0);
      U->Disk.Write += strtoul(s, &e, 0);
      U->Disk.Write += strtoul(e, &s, 0) - V->Disk.Write;
    }
    if ((s = strstr(buf, "disk_io: "))) {
      /* linux kernel 2.4 disk stats: */
      s += 10;
      U->Disk.Read = U->Disk.Write = 0;
      while (s && (s = strstr(s, "):("))) {
        if ((s = strchr(s + 3, ',')) && (s = strchr(s + 1, ','))) {
          U->Disk.Read += strtoul(s + 1, &e, 0);
          if (*e == ',' && (e = strchr(e + 1, ',')))
            U->Disk.Write += strtoul(e + 1, &s, 0);
        }
      }
      U->Disk.Read -= V->Disk.Read;
      U->Disk.Write -= V->Disk.Write;
    }

    close(fd);
  }

  if ((fd = open("/proc/diskstats", O_RDONLY)) != TW_NOFD) {
    /* linux kernel 2.6 disk stats: */

    (void)FullReadZ(fd, buf, sizeof(buf));
    s = buf;

    U->Disk.Read = U->Disk.Write = 0;

    while ((e = strstr(s, " hd")), (e2 = strstr(s, " sd")), (e3 = strstr(s, " nvme")),
           e || e2 || e3) {
      if (e && (!e2 || e < e2) && (!e3 || e < e3))
        s = e + 3;
      else if (e2 && (!e3 || e2 < e3))
        s = e2 + 3;
      else {
        s = e3 + 5;
        if (s[0] != ' ' && s[1] == 'n') {
          s += 2;
        }
      }

      if (s[0] != ' ' && s[1] == ' ') {
        /* match hd? sd? nvme?n? */
        strtoul(s + 2, &s, 0);              /* skip  'reads'         */
        strtoul(s, &s, 0);                  /* skip  'read_merges'   */
        U->Disk.Read += strtoul(s, &s, 0);  /* parse 'read_sectors'  */
        strtoul(s, &s, 0);                  /* skip  'read_ticks'    */
        strtoul(s, &s, 0);                  /* skip  'writes'        */
        strtoul(s, &s, 0);                  /* skip  'write_merges'  */
        U->Disk.Write += strtoul(s, &s, 0); /* parse 'write_sectors' */
      }
    }

    U->Disk.Read -= V->Disk.Read;
    U->Disk.Write -= V->Disk.Write;

    close(fd);
  }

  if (U->Disk.Max) {
    if (U->Disk.Max < U->Disk.Read + U->Disk.Write) {
      U->Disk.Max = U->Disk.Read + U->Disk.Write;
    }
  } else {
    /* first cycle... cannot know bandwidth */
    V->Disk.Read = U->Disk.Read;
    V->Disk.Write = U->Disk.Write;
    U->Disk.Read = U->Disk.Write = 0;
    U->Disk.Max = 1;
  }
  V->Disk.Max = U->Disk.Max;

  if ((fd = open("/proc/meminfo", O_RDONLY)) != TW_NOFD) {
    (void)FullReadZ(fd, buf, sizeof(buf));
    if ((s = strstr(buf, "MemTotal:")))
      U->Mem.Total = strtoul(s + 9, &e, 0), s = e;
    if ((s = strstr(buf, "MemFree:")))
      U->Mem.Free = strtoul(s + 8, &e, 0), s = e;
    if ((s = strstr(buf, "MemShared:")))
      U->Mem.Shared = strtoul(s + 10, &e, 0), s = e;
    if ((s = strstr(buf, "Buffers:")))
      U->Mem.Buff = strtoul(s + 8, &e, 0), s = e;
    if ((s = strstr(buf, "Cached:")))
      U->Mem.Cache = strtoul(s + 8, &e, 0), s = e;

    U->Mem.Used = U->Mem.Total - U->Mem.Free;
    if (U->Mem.Cache > U->Mem.Used)
      U->Mem.Cache = U->Mem.Used;
    U->Mem.Used -= U->Mem.Cache;
    if (U->Mem.Buff > U->Mem.Used)
      U->Mem.Buff = U->Mem.Used;
    U->Mem.Used -= U->Mem.Buff;
    if (U->Mem.Shared > U->Mem.Used)
      U->Mem.Shared = U->Mem.Used;
    U->Mem.Used -= U->Mem.Shared;

    if ((s = strstr(buf, "SwapTotal:")))
      U->Swap.Total = strtoul(s + 10, &e, 0), s = e;
    if ((s = strstr(buf, "SwapFree:")))
      U->Swap.Free = strtoul(s + 9, &e, 0), s = e;
    U->Swap.Used = U->Swap.Total - U->Swap.Free;

    close(fd);
  }

  if (U->Cpu.Total) {
    TwGotoXYWindow(SysMon_Win, 4, 0);
    if (numeric)
      PrintPercent(TCOL(tYELLOW, tblue),
                   100 * (U->Cpu.Total - U->Cpu.Idle - U->Cpu.Wait) / U->Cpu.Total);
    tmp = HBar(TCOL(tGREEN, 0), U->Cpu.User, U->Cpu.Total, 0);
    tmp = HBar(TCOL(tYELLOW, 0), U->Cpu.Nice, U->Cpu.Total, tmp);
    tmp = HBar(TCOL(tRED, 0), U->Cpu.System, U->Cpu.Total, tmp);
    tmp = HBar(TCOL(tMAGENTA, 0), U->Cpu.HardInt, U->Cpu.Total, tmp);
    tmp = HBar(TCOL(tCYAN, 0), U->Cpu.SoftInt, U->Cpu.Total, tmp);
    tmp = HBar(TCOL(tBLUE, 0), U->Cpu.Wait, U->Cpu.Total, tmp);
    (void)HBar(TCOL(tblue, 0), U->Cpu.Idle, U->Cpu.Total, tmp);
  }
  if (U->Disk.Max) {
    TwGotoXYWindow(SysMon_Win, 4, 1);
    if (numeric)
      PrintAbsoluteK(TCOL(tYELLOW, tblue), (U->Disk.Read + U->Disk.Write) >> 1);
    tmp = HBar(TCOL(tGREEN, 0), U->Disk.Read, U->Disk.Max, 0);
    tmp = HBar(TCOL(tRED, 0), U->Disk.Write, U->Disk.Max, tmp);
    (void)HBar(TCOL(tblue, 0), U->Disk.Max - U->Disk.Read - U->Disk.Write, U->Disk.Max, tmp);
  }
  if (U->Mem.Total) {
    TwGotoXYWindow(SysMon_Win, 4, 2);
    if (numeric)
      PrintAbsoluteK(TCOL(tYELLOW, tblue), (U->Mem.Total - U->Mem.Free));
    tmp = HBar(TCOL(tGREEN, 0), U->Mem.Used, U->Mem.Total, 0);
    tmp = HBar(TCOL(tCYAN, 0), U->Mem.Shared, U->Mem.Total, tmp);
    tmp = HBar(TCOL(tYELLOW, 0), U->Mem.Buff, U->Mem.Total, tmp);
    tmp = HBar(TCOL(tRED, 0), U->Mem.Cache, U->Mem.Total, tmp);
    (void)HBar(TCOL(tblue, 0), U->Mem.Free, U->Mem.Total, tmp);
  }
  if (U->Swap.Total) {
    TwGotoXYWindow(SysMon_Win, 4, 3);
    if (numeric)
      PrintAbsoluteK(TCOL(tYELLOW, tblue), U->Swap.Used);
    tmp = HBar(TCOL(tGREEN, 0), U->Swap.Used, U->Swap.Total, 0);
    (void)HBar(TCOL(tblue, 0), U->Swap.Free, U->Swap.Total, tmp);
  }

  TwSetColTextWindow(SysMon_Win, TCOL(twhite, tblue));

  /*
   * --- Uptime ---
   * Print to SysMon Window
   * added by Mohammad Bahathir Hashim <bakhtiar@softhome.net>
   */
  if ((fd = open("/proc/uptime", O_RDONLY)) != TW_NOFD) {

    unsigned long updays;
    int uphours, upminutes;

    (void)FullReadZ(fd, buf, sizeof(buf));

    updays = strtoul(buf, NULL, 0);
    /*upseconds = updays % 60;*/
    upminutes = (updays /= 60) % 60;
    uphours = (updays /= 60) % 24;
    updays /= 24;

    sprintf(buf, "%lu days %2d:%02d", updays, uphours, upminutes);

    TwGotoXYWindow(SysMon_Win, 8, 4);
    TwWriteCharsetWindow(SysMon_Win, strlen(buf), buf);

    close(fd);
  }

  U->Cpu.User += V->Cpu.User;
  U->Cpu.Nice += V->Cpu.Nice;
  U->Cpu.System += V->Cpu.System;
  U->Cpu.Idle += V->Cpu.Idle;
  U->Cpu.Wait += V->Cpu.Wait;
  U->Cpu.HardInt += V->Cpu.HardInt;
  U->Cpu.SoftInt += V->Cpu.SoftInt;

  U->Disk.Read += V->Disk.Read;
  U->Disk.Write += V->Disk.Write;

  return !i;
}

static void Quit(const char *argv0) {
  uldat err;
  if ((err = TwErrno)) {
    printf("%s: libtw error: %s%s\n", argv0, TwStrError(err), TwStrErrorDetail(err, TwErrnoDetail));
    TwClose();
    exit(1);
  }
  exit(0);
}

int main(int argc, char *argv[]) {
  usage Usage[2];
  tmsg msg;
  tevent_any Event;
  struct timeval p = {0, 0};
  fd_set readfds;
  int fd, which = 0;

  if (!InitSysMon(argc, argv))
    Quit(argv[0]);

  fd = TwConnectionFd();
  FD_ZERO(&readfds);

  memset(Usage, 0, sizeof(Usage));

  for (;;) {
    if (p.tv_sec == 0 && p.tv_usec < 10000) {
      which = Update(which, Usage);
      TwFlush();

      p.tv_sec = 1;
      p.tv_usec = 0;
    }

    while ((msg = TwReadMsg(tfalse))) {
      Event = &msg->Event;
      if (Event->EventCommon.W == SysMon_Win) {
        if (msg->Type == TW_MSG_WIDGET_GADGET) {
          if (Event->EventGadget.Code == 0)
            /* 0 == Close Code */
            Quit(argv[0]);
        }
      }
    }

    /* bail out if something goes *really* wrong */
    if (TwInPanic())
      break;

    FD_SET(fd, &readfds);
    select(fd + 1, &readfds, NULL, NULL, &p);
  }
  Quit(argv[0]);
}
