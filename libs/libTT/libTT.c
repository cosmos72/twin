/*
 *  libTT.c  --  'The Text Toolkit' library implementation
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA
 *
 */


#ifdef CONF_SOCKET_PTHREADS
# include <pthread.h>
#endif

#include <Tutf/Tutf_defs.h>
#include <Tw/Tw_defs.h>

/* include our internal copy of TT.h instead of the public one */
#include "TT.h"
#include <TT/TTerrno.h>

#include "mutex.h"

#include "TTextern.h"
#include "TTassert.h"

#include "dummytheme.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#ifdef CONF__MODULES
# include <dlfcn.h>
#endif

#ifdef CONF_SOCKET_PTHREADS
TH_R_MUTEX_HELPER_DEFS(static);
#endif

ttfns _TT_null_InitHW(tthw *HW);
static ttfns module_InitHW(tthw *HW);
ttfns _TT_twin_InitHW(tthw *HW);
ttfns _TT_gtk_InitHW(tthw *HW);
ttfns _TT_xml_InitHW(tthw *HW);

static ttobj FindNative(ttany id);

static void IdListInit(void);
static void DropId(ttobj Obj);

static void DispatchSimpleEvent(ttcomponent o, ttuint evtype);
static void DispatchEvent(ttcomponent o, ttevent ev, ttbyte dflags);

static opaque Method2Order(void *method);

static ttcallback AddTo_ttcallback(ttcallback c, ttcomponent o);
static void Remove_ttcallback(ttcallback c);

static void DelAll_ttobj(ttbyte quick);

static void Expose_ttvisible(ttvisible o, ttshort x, ttshort y, ttshort w, ttshort h);

#if TW_SIZEOFHWFONT == 1
# define CloneStr2TTFont TTCloneMem
#else
static ttfont *CloneStr2TTFont(TT_CONST ttbyte * s, size_t len);
#endif

#include "inlines.h"


void *(*TTAllocMem)(size_t) = malloc;
void *(*TTReAllocMem)(void *, size_t) = realloc;
void  (*TTFreeMem)(void *) = free;

/**
 * creates a copy of a chunk of memory
 */
void *TTCloneMem(TT_CONST void *S, size_t len) {
    void *T;
    if (S && (T = TTAllocMem(len)))
	return TTCopyMem(S, T, len);
  return NULL;
}
/**
 * creates a copy of a null-terminated string string
 */
ttbyte *TTCloneStr(TT_CONST ttbyte *S) {
    size_t len;
    ttbyte *T;
    if (S) {
	len = 1 + TTLenStr(S);
	if ((T = TTAllocMem(len)))
	    return TTCopyMem(S, T, len);
    }
    return NULL;
}

/**
 * sets the function to call to allocate/realloc/free memory;
 * can be called only if no connections to server are open.
 */
void TTConfigMalloc(void *(*my_malloc)(size_t),
		     void *(*my_realloc)(void *, size_t),
		     void  (*my_free)(void *)) {
    
    LOCK;
    if (!TTD.OpenFlag) {
	if (my_malloc && my_realloc && my_free) {
	    TTAllocMem = my_malloc;
	    TTReAllocMem = my_realloc;
	    TTFreeMem = my_free;
	} else {
	    TTAllocMem = malloc;
	    TTReAllocMem = realloc;
	    TTFreeMem = free;
	}
    }
    UNLK;
}




#ifdef CONF_SOCKET_PTHREADS

TT_INLINE ttbyte GrowErrnoLocation(void) {
    s_tt_errno *vec;
    ttuint newmax = rErrno.max <= 8 ? 16 : (rErrno.max<<1);
    
    if ((vec = (s_tt_errno *)TTReAllocMem(rErrno.vec, newmax * sizeof(s_tt_errno)))) {

	/* assume (th_self)-1 is _NOT_ a valid thread identifier */
	TTWriteMem(vec + rErrno.max, '\xFF', (newmax-rErrno.max) * sizeof(s_tt_errno));
	rErrno.vec = vec;
	rErrno.max = newmax;
	
	return TRUE;
    }
    /* out of memory! */
    return FALSE;
}

static s_tt_errno *GetErrnoLocation(void) {
    s_tt_errno *r = &rCommonErrno;
    th_self self;
    ttuint i;

    do if (TTD.OpenFlag) {
	self = th_self_get();
	
	/* we cache last thread that called GetErrnoLocation() */
	i = rErrno.last;
	if (i < rErrno.max && rErrno.vec[i].T == self) {
	    r = &rErrno.vec[i];
	    break;
	}
	
	for (i=0; i<rErrno.max; i++) {
	    if (rErrno.vec[i].T == self)
		break;
	    if (rErrno.vec[i].T == (th_self)-1) {
		/* empty slot, initialize it */
		rErrno.vec[i].T = self;
		rErrno.vec[i].E = rErrno.vec[i].S = 0;
		break;
	    }
	}
	if (i < rErrno.max) {
	    rErrno.last = i;
	    r = &rErrno.vec[i];
	    break;
	}
	if (GrowErrnoLocation()) {
	    rErrno.vec[i].E = rErrno.vec[i].S = 0;
	    rErrno.vec[i].T = self;
	    rErrno.last = i; /* i is previous rErrno.max */
	    r = &rErrno.vec[i];
	    break;
	}
    } while (0);
    
    return r;
}

static void FreeErrnoLocation(void) {
    if (rErrno.vec)
	TTFreeMem(rErrno.vec);
}


#else /* !CONF_SOCKET_PTHREADS */

# define GetErrnoLocation()	(TTD.OpenFlag ? &TTD.rErrno_ : &TTD.rCommonErrno_)
# define FreeErrnoLocation()	do { } while (0)

#endif /* CONF_SOCKET_PTHREADS */


#define Errno (GetErrnoLocation()->E)
#define ErrnoDetail (GetErrnoLocation()->S)



void TTAssertFail (char *assertion, char *file, unsigned int line, char *function) {
    fprintf(stderr, "TT-CRITICAL: %s:%u: %s: assertion `%s' failed\n",
	    file, line, function, assertion);
}


ttbyte TTInPanic(void) {
    /* maybe LOCK here too? */
    return TTD.PanicFlag;
}

ttuint TTLibraryVersion(void) {
    return TT_PROTOCOL_VERSION;
}

TT_DECL_MAGIC(TTMagicData);

ttbyte TTCheckMagic(TT_CONST ttbyte id[])  {
    if (TTCmpMem(id+1, TTMagicData+1, (id[0] < TTMagicData[0] ? id[0] : TTMagicData[0]) - 2 - sizeof(ttuint))) {
	CommonErrno = TT_EXLIB_SIZES;
	return FALSE;
    }
    return TRUE;
}

static ttuint PrintInitError(tthw HW, TT_CONST ttbyte *target, ttbyte module) {
    s_tt_errno *L = GetErrnoLocation();
    /* set TTD.FN.HW to allow error reporting */
    if (HW)
	TTD.FN.HW = *HW;
    fprintf(stderr, "TT-ERROR: %s display target%s `%s' failed: %s%s\n",
	    module ? "loading" : "compiled in",
	    module ? " module" : "",
	    target ? target : (TT_CONST ttbyte *)"(NULL)",
	    TTStrError(L->E), TTStrErrorDetail(L->E, L->S));
    return 0;
}

#ifdef CONF__MODULES
# ifdef LIBDIR
static TT_CONST ttbyte *conf_libdir = LIBDIR "/TT/HW" ;
# else
#  define conf_libdir "HW"
# endif

static ttfns module_InitHW(tthw *HW) {
    /* put all known driver names here (for autoprobing) */
    /* do not autoprobe for `null' or `xml' */
    TT_CONST ttbyte * name [] = {
# ifndef CONF_TT_HW_TWIN
	    "twin",
# endif
# ifndef CONF_TT_HW_GTK
	    "gtk",
# endif
	    NULL, NULL,
    };
    
    
    /* len == strlen(LIBDIR "/TT/HW/hw_twin.so.x.y.z") */
    ttuint i, len = 12 + strlen(TT_PROTOCOL_VERSION_STR) + strlen(conf_libdir);
    ttbyte *buf;
    ttfns FNs;
    ttfns (*init_dl)(tthw *);
    
    if (TTD.HWTarget) {
	/* forbid exiting from LIBDIR */
	if (strchr(TTD.HWTarget, '/')) {
	    CommonErrno = TT_EBAD_TARGET;
	    return (ttfns)0;
	}
	len += strlen( name[0] = TTD.HWTarget );
	/* if Target is specified, try *only* it */
	name[1] = NULL;
    }

    if ((buf = TTAllocMem(len))) {
	for (i = 0; i < sizeof(name)/sizeof(name[0]) && name[i]; i++) {
	    sprintf(buf, "%s/hw_%s.so." TT_PROTOCOL_VERSION_STR, conf_libdir, name[i]);
	    if ((TTD.DlHandle = dlopen(buf, RTLD_NOW|RTLD_GLOBAL))) {
		init_dl = (ttfns (*)(tthw *))dlsym(TTD.DlHandle, "InitModule");
		if (init_dl) {
		    if ((FNs = init_dl(HW))) {
			TTFreeMem(buf);
			return FNs;
		    } else {
			/* CommonErrno will be set by init_dl(HW, FNs) */
		    }
		} else {
		    CommonErrno = TT_EDLERROR_TARGET;
		    TTD.str_dlerror = dlerror();
		}
		/* do not dlclose() now, module may hold error messages */
		/* dlclose(TTD.DlHandle); */
	    } else {
		CommonErrno = TT_EDLERROR_TARGET;
		TTD.str_dlerror = dlerror();
	    }
	    PrintInitError(*HW, name[i], TRUE);
	}
	TTFreeMem(buf);
    } else
	CommonErrno = TT_ENO_MEM;

    return (ttfns)0;
}
#else /* !CONF__MODULES */

static ttfns module_InitHW(tthw *HW) {
    if (TTD.HWTarget)
	fprintf(stderr, "TT-ERROR: modules not enabled, cannot load display target `%s'\n",
		TTD.HWTarget);
    else
	fprintf(stderr, "TT-ERROR: modules not enabled, cannot load any display target\n");
    
    if (!CommonErrno)
	CommonErrno = TT_EFAILED_TARGET;
    return (ttfns)0;
}

#endif /* CONF__MODULES */



static ttuint ttfn_sizes[] = {
    0,
#define el(fntype) sizeof(TT_CAT(s_ttfn_,fntype)),
    TT_LIST(el)
#undef el
};

/*
 * initialize method structures, using values in TTD.FN as default
 * and overloading them with ones set in FNs
 */
#define DeltaFN(i) ((ttuint)((ttbyte *)TFNs[i] - (ttbyte *)TFNs[order_ttobj]))

static void InitFNs(ttfns FNs) {
    ttuint i, j;
    void **src, **dst;
    void *c;
    
    if (FNs != TTD.null_FNs) {
	for (i = order_ttobj; i < order_n; i++) {
	    j = DeltaFN(i) + TT_OFFSETOF(ttfn_ttobj,New);
	    src = (void **)((ttbyte *)FNs     + j);
	    dst = (void **)((ttbyte *)&TTD.FN + j);
	    for (j = TT_OFFSETOF(ttfn_ttobj,New); j < ttfn_sizes[i]; j += sizeof(void *)) {
		if ((c = *src))
		    *dst = c;
		src++; dst++;
	    }
	}
	TTD.FN.HW = FNs->HW;
    }
}

static void CloseTarget(void) {
    if (THW.Close)
	THW.Close();
    
    /* reinitialize at each TTClose() */
    TTD.FN = *TTD.null_FNs;

#ifdef CONF__MODULES
    /* unload module if needed */
    if (TTD.DlHandle) {
	dlclose(TTD.DlHandle);
	TTD.DlHandle = NULL;
    }
#endif
}


static void Close(ttbyte quick) {
    if (TTD.OpenFlag || TTD.PanicFlag) {
	/* save Errno in CommonErrno */
	s_tt_errno *E = GetErrnoLocation();
    
	CommonErrno = E->E;
	CommonErrnoDetail = E->S;
	
	/* close display target. also deletes all native objects */
	if (TTD.OpenFlag)
	    CloseTarget();

	if (TTD.HWTarget)
	    TTFreeMem(TTD.HWTarget);
	TTD.HWOptions = TTD.HWTarget = NULL;

	DelAll_ttobj(quick);
	
	TTD.OpenFlag = TTD.ExitMainLoopFlag = TTD.PanicFlag = FALSE;
	FreeErrnoLocation();
    }
}

void TTClose(void) {
    LOCK;
    Close(0);
    UNLK;
}

void TTCloseQuickNDirty(void) {
    LOCK;
    Close(1);
    UNLK;
}

static ttfns OpenTarget(void) {
    ttfns FNs;
    tthw HW;
	
    /* reinitialize at each TTOpen() */
    TTD.FN = *(TTD.null_FNs = _TT_null_InitHW(&HW));
    
#define AUTOCHECK4(t) (!TTD.HWTarget || !strcmp(t, TTD.HWTarget))
#define AUTOTRY4(t) (AUTOCHECK4(TT_STR(t)) && ((FNs = TT_CAT(TT_CAT(_TT_,t),_InitHW)(&HW)) || PrintInitError(HW, TT_STR(t), FALSE)))

#define CHECK4(t) (TTD.HWTarget && !strcmp(t, TTD.HWTarget))
#define TRY4(t) (CHECK4(TT_STR(t)) && ((FNs = TT_CAT(TT_CAT(_TT_,t),_InitHW)(&HW)) || PrintInitError(HW, TT_STR(t), FALSE)))
    
    /* use `null' or `xml' targets ONLY if explicitly requested */

    if (
	(TTD.HWTarget && !strcmp("null", TTD.HWTarget) && (FNs = TTD.null_FNs)) ||
#ifdef CONF_TT_HW_TWIN
	AUTOTRY4(twin) ||
#endif
#ifdef CONF_TT_HW_GTK
	AUTOTRY4(gtk) ||
#endif
#ifdef CONF_TT_HW_XML
	TRY4(xml) ||
#endif
	(FNs = module_InitHW(&HW))
#undef AUTOCHECK4
#undef AUTOTRY4
#undef CHECK4
#undef TRY4
	)
	;
    else if (!CommonErrno || CommonErrno == TT_EDLERROR_TARGET || CommonErrno >= TT_MAX_ERROR)
	CommonErrno = TT_EFAILED_TARGET;
    
    return FNs;
}

static ttbyte Open(TT_CONST byte *arg) {
    TT_CONST ttbyte * t;
    ttbyte * d, * s;
    ttfns FNs;
    
    if (!TTAssertAlways(sizeof(s_ttfn_ttobj) == TT_OFFSETOF(ttfn_ttobj,New) + 7 * sizeof(void *)))
	return FALSE;
    
    if (!TTD.UTIL.IdList)
	IdListInit();

    if (!TTD.OpenFlag && TTD.PanicFlag) {
	/* cleanup after a failed TTOpen() */
	Close(1);
    }
    if (TTD.OpenFlag) {
	CommonErrno = TT_EALREADY_CONN;
	UNLK;
	return FALSE;
    }
    
    /* if client did not ask for a specific target, pick from environment */
    t = arg ? arg : (ttbyte *)getenv("TTDISPLAY");

    if (t && *t) {
	/* split target and options */
	if ((s = TTD.HWTarget = TTCloneStr(t))) {
	    if ((d = strchr(s, '@')) || (d = strchr(s, ','))) {
		*d++ = '\0';
		TTD.HWOptions = d;
	    } else
		TTD.HWOptions = NULL;
	} else {
	    CommonErrno = TT_ENO_MEM;
	    return FALSE;
	}
    }

    if ((FNs = OpenTarget())) {
	CommonErrno = 0;
	
	TTD.Theme = &s_DummyTheme;
	TTD.Menubar = NULL;
	TTD.Application = NULL;
#ifdef CONF_SOCKET_PTHREADS
	TTD.rErrno_.max = TTD.rErrno_.last = 0;
#else
	TTD.rErrno_.E = TTD.rErrno_.S = 0;
#endif
	TTD.OpenFlag = TRUE;
	TTD.PanicFlag = FALSE;

	InitFNs(FNs);
    } else {
	if (TTD.HWTarget)
	    TTFreeMem(TTD.HWTarget);
	TTD.HWOptions = TTD.HWTarget = NULL;

	TTD.PanicFlag = TRUE;
    }
    return !!FNs;
}

ttbyte TTOpen(TT_CONST ttbyte *arg) {
    ttbyte ret;
    LOCK;
    ret = Open(arg);
    UNLK;
    return ret;
}


tt_errno *TTErrnoLocation(void) {
    s_tt_errno *t;
    LOCK;
    t = GetErrnoLocation();
    UNLK;
    return (tt_errno *)t;
}

TT_FN_ATTR_CONST TT_CONST ttbyte *TTStrError(ttuint E) {
    switch (E) {
      case 0:
	return "success";
      case TT_EXLIB_SIZES:
	return "compiled data sizes are incompatible with libTT now in use!";
      case TT_EALREADY_CONN:
	return "already connected";
	
      case TT_EDLERROR_TARGET:
	/*return "unable to load display target module: ";*/
	return "";
	
      case TT_EFAILED_TARGET:
#if defined(CONF_TT_HW_TWIN) || defined(CONF_TT_HW_GTK)
	return "all compiled-in display targets ("
# ifdef CONF_TT_HW_TWIN
	    "twin"
# endif
# if defined(CONF_TT_HW_TWIN) || defined(CONF_TT_HW_GTK)
	    ", "
# endif
# ifdef CONF_TT_HW_GTK
	    "gtk"
# endif
# ifdef CONF__MODULES
	    ") and all probed modules failed"
# else
	    ") failed and modules not enabled"
# endif
	    "\n\t(all known targets: twin, gtk, (xml), (null))";
#else /* !(defined(CONF_TT_HW_TWIN) || defined(CONF_TT_HW_GTK)) */
# ifdef CONF__MODULES
	return "no display target compiled in, and all probed modules failed"
	    
	    "\n\t(all known targets: twin, gtk, (xml), (null))";
# else
	return "no display target compiled in, and modules not enabled. Please recompile libTT !!";
# endif /* CONF__MODULES */
#endif/* defined(CONF_TT_HW_TWIN) || defined(CONF_TT_HW_GTK) */
	
      case TT_EBAD_TARGET:
	return "malformed TTDISPLAY environment variable";
	
      case TT_EVERSION_TARGET:
	return "display target found, but has wrong version";
	
      case TT_EBAD_ARG:
	return "invalid argument in function call: argument ";

      case TT_EEXIST:
	return "object already exists";

      default:
	if (E > TT_MAX_ERROR && THW.StrError)
	    return THW.StrError(E - TT_MAX_ERROR);
	return "unknown error";
    }
}

TT_FN_ATTR_CONST TT_CONST ttbyte *TTStrErrorDetail(ttuint E, ttuint S) {
    switch (E) {
      case TT_EDLERROR_TARGET:
	return TTD.str_dlerror ? TTD.str_dlerror : (ttbyte *)"";
      case TT_EBAD_ARG:
	switch (S) {
	  case 1: return "1";
	  case 2: return "2";
	  case 3: return "3";
	  case 4: return "4";
	  case 5: return "5";
	  case 6: return "6";
	  case 7: return "7";
	  case 8: return "8";
	  case 9: return "9";
	}
	return "?";
      default:
	if (E > TT_MAX_ERROR && THW.StrErrorDetail)
	    return THW.StrErrorDetail(E - TT_MAX_ERROR, S);
    }
    return "";
}

int TTConnectionFd(void) {
    int fd;
    LOCK;
    fd = THW.ConnectionFd();
    UNLK;
    return fd;
}

ttbyte TTFlush(void) {
    ttbyte ret = TRUE;
    LOCK;
    ret = !(TTD.PanicFlag || (TTD.PanicFlag = !THW.Flush()));
    UNLK;
    return ret;
}

ttbyte TTTimidFlush(void) {
    ttbyte ret = TRUE;
    LOCK;
    ret = !(TTD.PanicFlag || (TTD.PanicFlag = !THW.TimidFlush()));
    UNLK;
    return ret;
}

ttbyte TTSync(void) {
    ttbyte ret = TRUE;
    LOCK;
    ret = !(TTD.PanicFlag || (TTD.PanicFlag = !THW.Sync()));
    UNLK;
    return ret;
}

void TTExitMainLoop(void) {
    /* no LOCK here */
    TTD.ExitMainLoopFlag = TRUE;
    THW.ExitMainLoop();
}

static ttbyte MainLoop(ttbyte once, ttbyte wait) {
    s_tt_errno *E;
    if (!(once ? THW.MainLoopOnce(wait) : THW.MainLoop())) {
	LOCK;
	E = GetErrnoLocation();
	E->E = THW.GetErrno() + TT_MAX_ERROR;
	E->S = THW.GetErrnoDetail();
	UNLK;
	return FALSE;
    }
    return TRUE;
}

ttbyte TTMainLoop(void) {
    return MainLoop(FALSE, 0);
}

ttbyte TTMainLoopOnce(ttbyte wait) {
    return MainLoop(TRUE, wait);
}








/* utility function */
# if TW_SIZEOFHWFONT != 1
static ttfont *CloneStr2TTFont(TT_CONST ttbyte * s, size_t len) {
    ttfont *temp, *save;
    
    if (s) {
	if ((temp = save = (ttfont *)TTAllocMem((len+1) * sizeof(ttfont)))) {
	    while (len--) {
# ifdef CONF__UNICODE
		*temp++ = Tutf_CP437_to_UTF_16[*s++];
# else
		*temp++ = *s++;
# endif
	    }
	    *temp = '\0';
	}
	return save;
    }
    return NULL;
}
#endif



/* functions to manage Ids */

static ttobj *IdList[order_n];
static opaque IdSize[order_n], IdTop[order_n], IdBottom[order_n];

static void IdListInit(void) {
    TTD.UTIL.IdList = IdList;
    TTD.UTIL.IdSize = IdSize;
    TTD.UTIL.IdTop = IdTop;
    TTD.UTIL.IdBottom = IdBottom;
}


TT_INLINE opaque IdListGrow(ttbyte i) {
    opaque oldsize, size;
    ttobj *newIdList;
    
    oldsize = IdSize[i];
    if (oldsize >= MAXID)
	return NOSLOT;

    size = oldsize < SMALLBUFF/3 ? SMALLBUFF/2 : oldsize + (oldsize>>1);
    if (size > MAXID)
	size = MAXID;
    
    if (!(newIdList = (ttobj *)TTReAllocMem(IdList[i], size*sizeof(ttobj))))
	return NOSLOT;
    
    TTWriteMem(newIdList+oldsize, 0, (size-oldsize)*sizeof(ttobj));
    
    IdList[i] = newIdList;
    IdSize[i] = size;
    
    return oldsize;
}

TT_INLINE void IdListShrink(ttbyte i) {
    ttobj *newIdList;
    opaque size = TT_MAX2(BIGBUFF, IdTop[i] << 1);
    
    if (size < IdSize[i] && (newIdList = (ttobj *)TTReAllocMem(IdList[i], size*sizeof(ttobj)))) {
	IdList[i] = newIdList;
	IdSize[i] = size;
    }
}

TT_INLINE opaque IdListGet(ttbyte i) {
    if (IdBottom[i] == IdSize[i])
	return IdListGrow(i);
    
    return IdBottom[i];
}

TT_INLINE ttbyte _AssignId(ttbyte i, ttobj Obj) {
    opaque Id, j;
    if ((Id = IdListGet(i)) != NOSLOT) {
	Obj->id = Id | ((opaque)i << magic_shift);
	IdList[i][Id] = Obj;
	if (IdTop[i] <= Id)
	    IdTop[i] = Id + 1;
	for (j = IdBottom[i] + 1; j < IdTop[i]; j++)
	    if (!IdList[i][j])
		break;
	IdBottom[i] = j;
	return TRUE;
    }
    Errno = TT_ENO_MEM;
    return FALSE;
}

TT_INLINE void _DropId(ttbyte i, ttobj Obj) {
    opaque Id = Obj->id & MAXID, j;

    /* paranoia: we might end up calling DropId() twice on the same object */
    if (Id < IdTop[i] && IdList[i][Id] == Obj) {
	Obj->id = NOID;
	IdList[i][Id] = (ttobj)0;
	if (IdBottom[i] > Id)
	    IdBottom[i] = Id;
	for (j = IdTop[i] - 1; j > IdBottom[i]; j--)
	    if (IdList[i][j])
		break;
	IdTop[i] = (j == IdBottom[i]) ? j : j + 1;
	
	if (IdSize[i] > BIGBUFF && IdSize[i] > (IdTop[i] << 4))
	    IdListShrink(i);
    }
}
    
static ttbyte AssignId(TT_CONST ttfn_ttobj FN, ttobj Obj) {
    if (FN && Obj)
	return _AssignId(FN->order, Obj);
    return FALSE;
}

static void DropId(ttobj Obj) {
    if (Obj)
	_DropId(Obj->id >> magic_shift, Obj);
}


/* ttobj <-> tobj conversion */



TT_INLINE ttbyte InstanceOf(ttfn_ttobj FN, ttobj o) {
    return FN && o && (o->FN->magic & FN->magicmask) == FN->magic;
}


static ttobj Id2Obj(ttbyte i, opaque Id) {
    ttbyte j = Id >> magic_shift;
    ttobj o;
    if (i < order_n && j < order_n) {
	Id &= MAXID;
	if (Id < IdTop[j] && (o = IdList[j][Id]))
	    if (InstanceOf(Id2Fn(i), o))
		return o;
    }
    return (ttobj)0;
}

static ttobj FindNative(ttany id) {
    opaque i, j;
    ttobj o;
    /* search only ttnative:s (?) */
#if 1
    i = order_ttnative;
#else
    for (i = order_ttnative; i <= order_ttnative; i++)
#endif
	for (j = 0; j < IdTop[i]; j++) {
	    if ((o = IdList[i][j]) && o->native == id)
		return o;
	}

    return (ttobj)0;
}


/*
 * wrappers for public and exported methods
 */

#include "seterrno.h"

#include "create.h"

#include "call_m4.h"


/* type querying functions */
ttbyte TTInstanceOf(tt_fn FN, tt_obj o) {
    ttbyte ret;
    if ((opaque)FN == order_n)
	/* handle the case FN is TTFN_ttfn and o is one of TTFN_* */
	ret = (opaque)o > 0 && (opaque)o <= order_n;
    else if ((opaque)FN == type_last)
	/* handle the case FN is TTFN_tttype and o is one of TTFN_ttbyte...TTFN_ttfunction */
	ret = (opaque)o > type_first && (opaque)o <= type_last;
    else {
	LOCK;
	ret = InstanceOf(ID2FN(FN), ID2OBJ(o));
	UNLK;
    }
    return ret;
}

#if 0 /* not currently used */
TT_INLINE ttfn_ttobj ClassOf(ttobj o) {
    ttfn_ttobj ret;
    LOCK;
    ret = o ? o->FN : (void *)o;
    UNLK;
    return ret;
}
#endif

tt_fn TTClassOf(tt_obj o) {
    tt_fn ret;

    if ((opaque)o == 0)
	ret = (tt_fn)0;
    else if ((opaque)o <= order_n)
	/* handle the case o is one of TTFN_* */
	ret = (tt_fn)order_n;
    else if ((opaque)o <= type_last)
	/* handle the case o is one of TTFN_ttint,... */
	ret = (tt_fn)type_last;
    else {
	LOCK;
	/* shortcut for FN2ID(ClassOf(ID2OBJ(o))) */
	ret = (tt_fn) ( ID2OBJ(o) ? ((opaque)o) >> magic_shift : NOID );
	UNLK;
    }
    return ret;
}

ttuint TTGetSize_ttfn(tt_fn FN) {
    ttopaque ret = 0;
    
    if ((opaque)FN == 0)
	;
    else if ((opaque)FN <= order_n)
	/* handle the case FN is one of TTFN_* */
	;
    else if ((opaque)FN <= type_last)
	/* handle the case FN is one of TTFN_ttint,... */
	;
    else {
	ttfn fn;
	/* LOCK; */
	if ((fn = ID2FN(FN)))
	    ret = fn->size;
	/* UNLK; */
    }
    return ret;
}

ttuint TTSizeOf(tt_obj o) {
    return TTGetSize_ttfn(TTClassOf(o));
}

/*
 * return the superclass of the class fn
 */
tt_fn TTGetSuper_ttfn(tt_fn fn) {
    ttfn_ttobj FN, FN_super;
    opaque i;
    
    /*
     * this could be *MUCH* smarter than a dumb linear search,
     * but hardcoding the class tree here would make it impossible
     * to create more classes at runtime.
     */
    if ((FN = ID2FN((opaque)fn))) {
	for (i = (opaque)fn - 1; i >= order_ttobj; i--) {
	    if ((FN_super = ID2FN(i)) && (FN->magic & FN_super->magicmask) == FN_super->magic)
		return FN_super;
	}
    }
    return (tt_fn)0;
}

static TT_CONST ttbyte *ttfn_names[] = {
    NULL,
#define el(t) TT_STR(t),
    TT_LIST(el)
    TT_STR(ttfn),
    TT_TYPELIST(el)
#undef el
    TT_STR(tttype)
};

/*
 * return the name of the class fn
 */
TT_CONST ttbyte *TTGetName_ttfn(tt_fn fn) {
    if ((opaque)fn > 0 && (opaque)fn <= type_last)
	return ttfn_names[(opaque)fn];
    return NULL;
}

/*
 * return the name of the class of o
 */
TT_CONST ttbyte *TTClassNameOf(tt_obj o) {
    return TTGetName_ttfn(TTClassOf(o));
}

/* constructors */
tt_obj TTNew(tt_fn FN) {
    tt_obj ret;
    LOCK;
    ret = (tt_obj) OBJ2ID(New(ID2FN(FN), NULL));
    UNLK;
    return ret;
}

/* destructors */
void TTDel(tt_obj o) {
    LOCK;
    Del(ID2OBJ(o));
    UNLK;
}





/* ttlistener management */

TT_INLINE void DelAllListeners_ttcomponent(ttcomponent o) {
    ttlistener l, next;
    if (o) {
	next = o->listeners;
	while ((l = next)) {
	    next = l->next;
	    TDEL(l);
	}
	/* paranoia */
	o->listeners = NULL;
    }
}

/* ttcallback AVL management */

#define ttavl2ttcallback(avl) ( (avl) ? (ttcallback) ((ttbyte *)(avl) - TT_OFFSETOF(ttcallback,AVL)) : (ttcallback)0 )
#define ttcallback2ttavl(clk) ( (clk) ? &(clk)->AVL : (ttavl)0 )


TT_INLINE ttuint GenAVLkey(ttevent ev) {
    return OBJ2ID(ev->component) ^ (ev->evtype << 10) ^ (ev->evcode << 20);
}

TT_INLINE int CompareEvents(TT_CONST ttevent e1, TT_CONST ttevent e2) {
    if (e1->component == e2->component) {
	if (e1->evtype == e2->evtype) {
	    if (e1->evcode == e2->evcode) {
		/* common part matches. check details. */
		if (e1->evtype == ttevent_evtype_key || e1->evtype == ttevent_evtype_mouse)
		    return e1->evflags - e2->evflags;
		/* else no extra checks needed */
		return 0;
	    }
	    return e1->evcode - e2->evcode;
	}
	return e1->evtype - e2->evtype;
    }
    return OBJ2ID(e1->component) - OBJ2ID(e2->component);
}


static int CompareAVLs(ttavl_c a1, ttavl_c a2) {
    
    return CompareEvents( ttavl2ttcallback(a1)->event, ttavl2ttcallback(a2)->event );
}

static ttcallback FindCallback4Event(ttcallback base, ttevent ev) {
    s_ttcallback evc;
    ttavl key;
    
    /*
     * no need to fully initialize evc here...
     * just evc.AVL and evc.ev are used
     * 
     * if (!TNEW2(ttcallback, &evc))
     *     return (ttcallback)0;
     */
    evc.AVL.AVLkey = GenAVLkey(ev);
    evc.event = ev;
    
    key = AVLFind(ttcallback2ttavl(&evc), ttcallback2ttavl(base), CompareAVLs);
    
    /*
     * so no need to cleanup evc too
     * 
     * TDEL(&evc);
     */
    return key ? ttavl2ttcallback(key) : (ttcallback)0;
}

static void CallFunctionPlain(void *function, ttuint nargs, ttany *args) {
    switch (nargs) {
      case 0:
	((void (*)(void)) function ) ();
	break;
      case 1:
	((void (*)(ttany)) function ) (args[0]);
	break;
      case 2:
	((void (*)(ttany,ttany)) function ) 
	    (args[0],args[1]);
	break;
      case 3:
	((void (*)(ttany,ttany,ttany)) function )
	    (args[0],args[1],args[2]);
	break;
      case 4:
	((void (*)(ttany,ttany,ttany,ttany)) function )
	    (args[0],args[1],args[2],args[3]);
      case 5:
	((void (*)(ttany,ttany,ttany,ttany,ttany)) function )
	    (args[0],args[1],args[2],args[3],args[4]);
	break;
      case 6:
	((void (*)(ttany,ttany,ttany,ttany,ttany,ttany)) function )
	    (args[0],args[1],args[2],args[3],args[4],args[5]);
	break;
      case 7:
	((void (*)(ttany,ttany,ttany,ttany,ttany,ttany,ttany)) function )
	    (args[0],args[1],args[2],args[3],args[4],args[5],args[6]);
	break;
      case 8:
	((void (*)(ttany,ttany,ttany,ttany,ttany,ttany,ttany,ttany)) function )
	    (args[0],args[1],args[2],args[3],args[4],args[5],args[6],args[7]);
	break;
      case 9:
	((void (*)(ttany,ttany,ttany,ttany,ttany,ttany,ttany,ttany,ttany)) function )
	    (args[0],args[1],args[2],args[3],args[4],args[5],args[6],args[7],args[8]);
	break;
      case 10:
	((void (*)(ttany,ttany,ttany,ttany,ttany,ttany,ttany,ttany,ttany,ttany)) function )
	    (args[0],args[1],args[2],args[3],args[4],args[5],args[6],args[7],args[8],args[9]);
	break;
    }
}


/*
 * this must come *after*
 * #include "wrap_m4.h"
 * as it uses a lot of stuff from it
 */
static void DispatchEvent(ttcomponent o, ttevent ev, ttbyte dflags) {
    /*
     * this is CRITICAL code. we must protect o and c from being deleted
     * inside the callbacks, yet realize whether we must delete them after callback returned.
     */
    ttcallback c = (ttcallback)0, cnext;
    tobj o_id, ev_id;
    
    if (dflags & DispatchEvent_dflags_before) {
	/* protect o and ev from being deleted */
	if (ev)
	    ev->refcount++;
	if (o)
    	    o->refcount++;
    }
    
    if (ev && (ev->component = o)) {
	
	if (o->callbacks &&
	    (c = FindCallback4Event(o->callbacks, ev))) {

	    o_id = OBJ2ID(o);
	    ev_id = OBJ2ID(ev);
	    
	    if (!(dflags & DispatchEvent_dflags_before)) {
		/* jump straight to *_after callbacks */
		while (c && !(c->lflags & ttcallback_lflags_after))
		    c = c->next;
	    }
	    
	    for (; c && !(ev->evflags & ttevent_evflags_consumed); c = cnext) {
		
		if (!(dflags & DispatchEvent_dflags_after) &&
		    c->lflags & ttcallback_lflags_after)
		    break;
		
		if (!c->function) {
		    cnext = c->next;
		    continue;
		}
		
		c->refcount++; /* protect c from being deleted */
		
		if (c->args) {
		    /* if c->narg_component is valid, set corresponding arg to ttcomponent o */
		    if (c->narg_component < c->nargs)
			c->args[c->narg_component] = o_id;
		    /* if c->narg_event is valid, set corresponding arg to ttevent ev */
		    if (c->narg_event < c->nargs)
			c->args[c->narg_event] = ev_id;
		}
		
		if (c->lflags & ttcallback_lflags_builtin)
		    
		    CallMethod((opaque)c->function, c->nargs, c->args);
		
		else if (c->lflags & ttcallback_lflags_function_plain)
		    
		    CallFunctionPlain((void *)c->function, c->nargs, c->args);
		
		else
		    
		    c->function(c->nargs, c->args);
		
		/* get next callback NOW, as call above may add/remove callbacks */
		cnext = c->next;
		
		if (!--c->refcount)
		    TDEL(c);
	    }
	}
	
    }
    if (dflags & DispatchEvent_dflags_after) {
	if (ev) {
	    /* delete (ev) in the *_after pass */
	    ev->refcount--;
	    TDEL(ev);
	}
	if (o && !--o->refcount)
	    TDEL(o);
    }
}


static void DispatchSimpleEvent(ttcomponent o, ttuint evtype) {
    s_ttevent evt;
    
    /* correctly initialize all evt fields */
    if (TNEW2(ttevent, &evt)) {
	evt.evtype = evtype;
	evt.evcode = evt.evflags = 0;
        DispatchEvent(o, &evt, DispatchEvent_dflags_both);
	/*
	 * DispatchEvent() also deletes evt. Since it was passed
	 * as argument to TNEW2(), TTFreeMem() will not be called on it.
	 */
    }
}

static TT_CONST struct s_tavl empty_AVL;

static ttcallback AddTo_ttcallback(ttcallback c, ttcomponent o) {
    ttavl root;
    ttcallback base;
    
    if (c && TTAssert(IS(ttcallback,c)) && o && TTAssert(IS(ttcomponent,o)) &&
	!c->component && c->event && !c->prev && !c->next &&
	!TTCmpMem(&c->AVL, &empty_AVL, sizeof(s_ttavl))) {

	c->AVL.AVLkey = GenAVLkey(c->event);

	if ((base = FindCallback4Event(o->callbacks, c->event))) {
	    if (base && (c->lflags & ttcallback_lflags_after)) {
		/* append: linear search through existing list (ugly!) */
		while (base->next)
		    base = base->next;
		c->prev = base;
		base->next = c;
	    } else {
		/* prepend */
		c->AVL = base->AVL;
		c->next = base;
		base->prev = o->callbacks = c;
	    }
		
	} else {
	    root = ttcallback2ttavl(o->callbacks);
	    AVLInsert(ttcallback2ttavl(c), root, CompareAVLs, &root);
	    o->callbacks = ttavl2ttcallback(root);
	}
	return c;
    } else
	SetErrno(TT_EBAD_ARG, c && c->event ? 2 : 1);
    
    return (ttcallback)0;
}

static void Remove_ttcallback(ttcallback c) {
    ttcomponent o;
    ttavl node, nc, ncnext;
    ttcallback first;
    
    if (c && TTAssert(IS(ttcallback,c)) && c->event && (o = c->component)) {
	
	/* we have an AVL tree, where each node is the head of a linked list */
	if ((first = FindCallback4Event(o->callbacks, c->event))) {
	    
	    if (c->prev)
		c->prev->next = c->next;
	    else if (!TTAssert(c == first))
		return;

	    if (c->next)
		c->next->prev = c->prev;
	    
	    if (!c->prev) {
		if (c->next) {
		    nc  = ttcallback2ttavl(c);
		    ncnext = ttcallback2ttavl(c->next);
		    
		    *ncnext = c->AVL;

		    /* we removed the head of the linked list... update the AVL tree */
		    if ((node = c->AVL.AVLParent)) {
			if (node->AVLLeft == nc)
			    node->AVLLeft = ncnext;
			else if (TTAssert(node->AVLRight == nc))
			    node->AVLRight = ncnext;
		    } else
			o->callbacks = c->next;
		    
		    if ((node = c->AVL.AVLLeft))
			node->AVLParent = ncnext;
		    if ((node = c->AVL.AVLRight))
			node->AVLParent = ncnext;
		    
		    
		} else {
		    /* we removed the last node of the linked list... rebalance the AVL tree */
		    node = ttcallback2ttavl(o->callbacks);
		    AVLRemove(ttcallback2ttavl(c), CompareAVLs, &node);
		    o->callbacks = ttavl2ttcallback(node);
		}
	    }
	}
    }
}

/* delete using a finite stack */
TT_INLINE void DelAll_ttcallback(ttcallback base) {
    ttcallback next;
    ttavl node;
    
    while (base) {
	if ((node = base->AVL.AVLLeft))
	    base = ttavl2ttcallback(node);
	else if ((node = base->AVL.AVLRight))
	    base = ttavl2ttcallback(node);
	else {
	    /* destroy the tree while we back up */
	    if ((node = base->AVL.AVLParent)) {
		if (node->AVLLeft == &base->AVL)
		    node->AVLLeft = (ttavl)0;
		else if (node->AVLRight == &base->AVL)
		    node->AVLRight = (ttavl)0;
	    }
	    
	    while (base) {
		next = base->next;
		/* avoid calling AVL tree rebalancing in TDEL(base) */
		base->AVL = empty_AVL;
		base->prev = base->next = (ttcallback)0;
		base->component = (ttcomponent)0;
		TDEL(base);
		
		base = next;
	    }
	    base = ttavl2ttcallback(node);
	}
    }
}

TT_INLINE void DelAllCallbacks_ttcomponent(ttcomponent o) {
    if (o) {
	if (o->callbacks)
	    DelAll_ttcallback(o->callbacks);
	o->callbacks = (ttcallback)0;
    }
}

static void DelAll_ttobj(ttbyte quick) {
    ttuint n;
    ttopaque i;
    ttobj o;
    
    for (n = order_n - 1; n != (ttuint)-1; n--) {
	for (;;) {
	    /* IdTop[n] decreases with us. */
	    if ((i = IdTop[n] - 1) != (ttuint)-1) {
		if (TTAssertAlways(o = IdList[n][i]))
		    if (quick && IS(ttcomponent,o)) {
			/* don't generate delete events */
			DelAllListeners_ttcomponent((ttcomponent)o);
			DelAllCallbacks_ttcomponent((ttcomponent)o);
		    }
		    /*
		     * this is "the right thing" but may
		     * generate *LOTS* of delete events :(
		     */
		    TDEL(o);
		if (n == 0 && i == 0)
		    return;
	    } else
		break;
	}
    }
}

/* ttvisible Repaint helper */
static void Expose_ttvisible(ttvisible o, ttshort x, ttshort y, ttshort w, ttshort h) {
    tteventbig ev;
    
    if (o && TTAssert(IS(ttvisible,o))) {
	if ((ev = Create_tteventbig
	     (ttevent_evtype_expose, 0, 0, x, y, w, h, (ttuint)0, (TT_CONST ttbyte *)0)))
	
	    DispatchEvent((ttcomponent)o, (ttevent)ev, DispatchEvent_dflags_before);
	
	if (o->repaint) {
	    o->refcount++; /* protect o from being deleted */
	    o->repaint((tt_obj)OBJ2ID(o), x, y, w, h);
	    if (!--o->refcount)
		TDEL(o);
	} else
	    o->FN->BuiltinRepaint(o, x, y, w, h);
	
	if (ev)
	    /* also deletes ev */
	    DispatchEvent((ttcomponent)o, (ttevent)ev, DispatchEvent_dflags_after);
    }
}

/* TTD global */


s_tt_d TTD = {
#ifdef CONF_SOCKET_PTHREADS
    TH_R_MUTEX_INITIALIZER,
#endif
    (tttheme)0, (tttheme)0,
    (ttmenubar)0, (ttapplication)0,
	
    (byte *)0, (byte *)0,
    (void *)0,
    (ttbyte)0, (ttbyte)0, (ttbyte)0,
	
    { 0, }, { 0, }, NULL,
	
    {
#include "create_init_m4.h"
    },
    {
	    NULL, NULL, NULL, NULL,
	    
#ifdef CONF_SOCKET_PTHREADS
	    GetErrnoLocation,
#endif
	    CloneStr2TTFont,
	    AssignId,
	    DropId,
	    Id2Obj,
	    FindNative,
	    DispatchEvent,
	    DispatchSimpleEvent,
	    DelAllListeners_ttcomponent,
	    DelAllCallbacks_ttcomponent,
	    AddTo_ttcallback,
	    Remove_ttcallback,
	    Expose_ttvisible,
    },
    {
	(ttfn_ttobj)0, /* order_() == 0 */
#define el(t) (ttfn_ttobj)TT_CAT(TFN_,t),
TT_LIST(el)
#undef el
    },
    
    /* NULL, */
	
    /* { { 0, }, }, */
};


/* TTFN_* globals */
#define el(t) tt_fn TT_CAT(TTFN_,t) = (tt_fn)order_(t);
TT_LIST(el)
#undef el
tt_fn TTFN_ttfn = (tt_fn)order_n;

#define el(t) tt_fn TT_CAT(TTFN_,t) = (tt_fn)type_(t);
TT_TYPELIST(el)
#undef el

tt_fn TTFN_tttype = (tt_fn)type_last;

