/*
 *  alloc.c  --  wrappers around malloc() / realloc() / free()
 *
 *  Copyright (C) 1999-2000,2016 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "twin.h"
#include "alloc.h"
#include "util.h"

#include <Tutf/Tutf.h> // Tutf_CP437_to_UTF_32[]

#ifdef TW_HAVE_SIGNAL_H
#include <signal.h>
#endif

udat ErrNo;
CONST char *ErrStr;
byte Error(udat Code_Error) {
  switch ((ErrNo = Code_Error)) {
  case NOMEMORY:
    ErrStr = "Out of memory!";
    break;
  case NOTABLES:
    ErrStr = "Internal tables full!";
    break;
  case SYSCALLERROR:
    ErrStr = strerror(errno);
    break;
  default:
    break;
  }
  return tfalse;
}

void *AllocMem(size_t Size) {
  void *res = NULL;
  if (Size) {
    if (!(res = malloc(Size)))
      Error(NOMEMORY);
  }
  return res;
}

void *ReAllocMem(void *Mem, size_t Size) {
  void *res;
  if (Mem) {
    if (Size) {
      res = realloc(Mem, Size);
      /* cannot use AllocMem() + CopyMem() here: we don't know Mem current size */
    } else {
      FreeMem(Mem);
      res = NULL;
    }
  } else
    res = AllocMem(Size);
  return res;
}

void *AllocMem0(size_t ElementSize, size_t Count) {
  void *res = NULL;
  if (ElementSize || Count) {
    if (!(res = calloc(Count, ElementSize)))
      Error(NOMEMORY);
  }
  return res;
}

void *ReAllocMem0(void *Mem, size_t ElementSize, size_t OldCount, size_t NewCount) {
  void *res;
  if (Mem) {
    if (ElementSize && NewCount) {
      if ((res = realloc(Mem, ElementSize * NewCount))) {
        if (NewCount > OldCount)
          memset((byte *)res + ElementSize * OldCount, '\0', ElementSize * (NewCount - OldCount));
      } else if ((res = AllocMem0(ElementSize, NewCount))) {
        size_t MinCount = OldCount < NewCount ? OldCount : NewCount;
        CopyMem(Mem, res, ElementSize * MinCount);
        FreeMem(Mem);
      }
    } else {
      FreeMem(Mem);
      res = NULL;
    }
  } else
    res = AllocMem0(ElementSize, NewCount);
  return res;
}

void *CloneMem(CONST void *From, uldat Size) {
  void *temp;
  if (From && Size && (temp = AllocMem(Size)))
    return CopyMem(From, temp, Size);
  return NULL;
}

char *CloneStr(CONST char *s) {
  char *q;
  uldat len;

  if (s) {
    len = 1 + strlen(s);
    if ((q = (char *)AllocMem(len)))
      CopyMem(s, q, len);
    return q;
  }
  return NULL;
}

char *CloneStrL(CONST char *s, uldat len) {
  char *q;

  if (s) {
    if ((q = (char *)AllocMem(len + 1))) {
      if (len)
        CopyMem(s, q, len);
      q[len] = '\0';
    }
    return q;
  }
  return NULL;
}

char **CloneStrList(char **s) {
  uldat n = 1;
  char **t = s, **v;

  if (t) {
    while (*t) {
      t++;
      n++;
    }
    t = (char **)AllocMem(n * sizeof(char *));
  }

  if ((v = t)) {
    for (; *s && n; v++, s++, n--) {
      if (!(*v = CloneStr(*s)))
        break;
    }

    if (*s && n) {
      /* failed... clean up */
      for (; t < v; t++)
        FreeMem(*t);
      FreeMem(t);
      t = NULL;
    } else
      *v = NULL;
  }
  return t;
}

trune *CloneStr2TRune(CONST char *s, uldat len) {
  trune *temp, *save;

  if (s) {
    if ((temp = save = (trune *)AllocMem((len + 1) * sizeof(trune)))) {
      while (len--) {
        *temp++ = Tutf_CP437_to_UTF_32[(byte)*s++];
      }
      *temp = '\0';
    }
    return save;
  }
  return NULL;
}

#if defined(TW_HAVE_ALARM) && defined(TW_HAVE_SIGACTION)
volatile int AlarmReceived = 0;

static TW_RETSIGTYPE AlarmHandler(int sig) {
  AlarmReceived = 1;
  TW_RETFROMSIGNAL(0);
}
void SetAlarm(unsigned seconds) {
  struct sigaction act;
  if (seconds != 0) {
    act.sa_handler = AlarmHandler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_RESETHAND;

    if (sigaction(SIGALRM, &act, NULL) == 0) {
      AlarmReceived = 0;
      alarm(seconds);
    }
  } else {
    alarm(0);

    act.sa_handler = SIG_DFL;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGALRM, &act, NULL);

    AlarmReceived = 0;
  }
}
#endif
