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

#include "inlines.h"
#include "fire.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>


#ifdef CONF__MODULES
# include <dlfcn.h>
#endif

static void RealClose(void);

TT_MUTEX_HELPER_DEFS(static);

ttfns _TT_null_InitHW(tthw *HW);
static ttfns module_InitHW(tthw *HW);
ttfns _TT_twin_InitHW(tthw *HW);
ttfns _TT_gtk_InitHW(tthw *HW);
ttfns _TT_X11_InitHW(tthw *HW);
ttfns _TT_xml_InitHW(tthw *HW);

static void GetNow(void);

static ttobj FindNative(ttany id);

static void DropId(ttobj Obj);

static void FireSimpleEvent(ttcomponent o, ttuint evtype);
static void FireEvent(TT_ARG_DIE ttevent ev, ttcomponent o);
static void FireEventLoop(void);

static opaque Method2Order(void *method);

static void DelAll_ttobj(ttbyte quick);

static void Expose_ttvisible(ttvisible o, ttshort x, ttshort y, ttshort w, ttshort h);

static ttdata FindByKey_ttdata(ttdata base, TT_CONST ttbyte *key, ttopaque len);

static ttdata protected_Create_ttdata(ttcomponent c, TT_ARG_READ ttbyte *key, ttopaque key_len, ttany data);


#ifdef CONF_SOCKET_PTHREADS

TT_INLINE ttbyte GrowErrnoLocation(void) {
    s_tt_errno *vec;
    ttuint newmax = rErrno.max <= 8 ? 16 : (rErrno.max<<1);
    
    if ((vec = (s_tt_errno *)TTReallocMem(rErrno.vec, newmax * sizeof(s_tt_errno)))) {

	/* assume (th_self)-1 is _NOT_ a valid thread identifier */
	TTWriteMem(vec + rErrno.max, '\xFF', (newmax-rErrno.max) * sizeof(s_tt_errno));
	rErrno.vec = vec;
	rErrno.max = newmax;
	
	return TT_TRUE;
    }
    /* out of memory! */
    return TT_FALSE;
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
    fprintf(stderr, "TT-CRITICAL: %s:%u: %s(): assertion `%s' failed\n",
	    file, line, function, assertion);
}


ttbyte TTIsPanic(void) {
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
	return TT_FALSE;
    }
    return TT_TRUE;
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
# ifndef CONF_TT_HW_X11
	    "X11",
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
 * initialize method structures, using values in TTD.FNdefault as default
 * and overloading them with ones set in FNs
 */
#define DeltaFN(i) ((ttuint)((ttbyte *)TFNs[i] - (ttbyte *)TFNs[order_ttobj]))

static void InitFNs(ttfns FNs) {
    ttuint i, isuper = 0, j, d;
    void **super1, **super2, **src1, **src2, **dst1, **dst2;
    void *c1, *c2;
    
    for (i = order_ttobj; i < order_n; i++) {

	j = TT_OFFSETOF(ttfn_ttobj,New);
	if (i > order_ttobj) {
	    isuper = (opaque)TTGetSuper_ttfn((tt_fn)i);
	    d = DeltaFN(isuper);
	    super1 = (void **)((ttbyte *)&TTD.FNdefault + d + j);
	    super2 = (void **)((ttbyte *)&TTD.FN        + d + j);
	}
	d = DeltaFN(i);
	src1 =   (void **)((ttbyte *)TTD.FN_hw_null + d + j);
	src2 =   (void **)((ttbyte *)FNs            + d + j);
	dst1 =   (void **)((ttbyte *)&TTD.FNdefault + d + j);
	dst2 =   (void **)((ttbyte *)&TTD.FN        + d + j);
	
	/*
	 * this is TRICKY. we have two sources of methods (src1 and src2)
	 * and we must combine them, writing into dst1 and dst2:
	 * we order non-null methods as below, then we use the last non-null
	 * and put it into corresponding dst.
	 * 
	 * drawing src* on the left and dst* on the right as below,
	 * it means that each dst* must be set with the last src* that is placed
	 * at equal row or in an earlier row.
	 * so src2 cannot go in dst1, but SUPER(src2) can.
	 *                   
	 *             ...   
	 * SUPER(SUPER(src1))
	 * SUPER(SUPER(src2))
	 *       SUPER(src1) 
	 *       SUPER(src2) 
	 *             src1     ->    dst1
	 *             src2     ->    dst2
	 */
	for (j = TT_OFFSETOF(ttfn_ttobj,New); j < ttfn_sizes[i]; j += sizeof(void *)) {
	    if (TTAssertAlways
		((c1 = *src1) || ((j < ttfn_sizes[isuper]) &&
				  ((c1 = *super2) || (c1 = *super1)))))
		*dst1 = c1;
	    
	    if ((c2 = *src2) || (c2 = c1))
		*dst2 = c2;

	    super1++, super2++;
	    src1++; src2++;
	    dst1++; dst2++;
	}
	
    }
    /* set HW methods */
    TTD.FN.HW = FNs->HW;
    TTD.FNdefault.HW = TTD.FN_hw_null->HW;
}

static void CloseTarget(void) {
    if (THW.Close)
	THW.Close();
    
    /* reinitialize at each TTClose() */
    InitFNs(TTD.FN_hw_null);

#ifdef CONF__MODULES
    /* unload module if needed */
    if (TTD.DlHandle) {
	dlclose(TTD.DlHandle);
	TTD.DlHandle = NULL;
    }
#endif
    
    TTD.PanicFlag = TT_FALSE;
}

static void RealClose(void) {
    
    /* save Errno in CommonErrno */
    s_tt_errno *E = GetErrnoLocation();
    
    CommonErrno = E->E;
    CommonErrnoDetail = E->S;
    
    /* increase TTD.refcount so that we are not called recursively */
    TTD.refcount++;
    
    /* close display target. also deletes all native objects */
    if (TTD.OpenFlag)
	CloseTarget();
    
    if (TTD.HWTarget)
	TTFreeMem(TTD.HWTarget);
    TTD.HWOptions = TTD.HWTarget = NULL;
    
    TTD.DummyTheme = (tttheme)0;
    TTWriteMem((ttbyte *)&s_DummyTheme + TT_OFFSETOF(tttheme, name), '\0',
	       sizeof(s_tttheme) - TT_OFFSETOF(tttheme, name));
    
    DelAll_ttobj(TTD.QuitArg);
    
    FreeErrnoLocation();
    
    TTD.OpenFlag = TTD.ExitMainLoopFlag = TTD.PanicFlag = TTD.QuitArg = TT_FALSE;
    TTD.refcount = 0;
}

static void Close(ttbyte quick) {
    /* do not mess with TTD.refcount */
    th_r_mutex_lock(mutex);

    if (TTD.OpenFlag || TTD.PanicFlag) {
	/* remember arg */
	TTD.QuitArg = quick;
	
	if (!(TTD.refcount &= ~ttobj_refcount_alive))
	    RealClose();
	else {
	    /*
	     * do not call RealClose() explicitly:
	     * UNLK will do it at the right moment,
	     * since we cannot quit from inside a TT* function !
	     */
	}
    }
    th_r_mutex_unlock(mutex);
}

void TTClose(void) {
    Close(TT_FALSE);
}

void TTCloseQuickNDirty(void) {
    Close(TT_TRUE);
}

static ttfns OpenTarget(void) {
    ttfns FNs;
    tthw HW;
	
    /* reinitialize at each TTOpen(). this also sets ->order, ->magicmask, ->magic, ->size */
    TTD.FNdefault = TTD.FN = *(TTD.FN_hw_null = _TT_null_InitHW(&HW));
    
#define AUTOCHECK4(t) (!TTD.HWTarget || !strcmp(t, TTD.HWTarget))
#define AUTOTRY4(t) (AUTOCHECK4(TT_STR(t)) && ((FNs = TT_CAT(TT_CAT(_TT_,t),_InitHW)(&HW)) || PrintInitError(HW, TT_STR(t), TT_FALSE)))

#define CHECK4(t) (TTD.HWTarget && !strcmp(t, TTD.HWTarget))
#define TRY4(t) (CHECK4(TT_STR(t)) && ((FNs = TT_CAT(TT_CAT(_TT_,t),_InitHW)(&HW)) || PrintInitError(HW, TT_STR(t), TT_FALSE)))
    
    /* use `null' or `xml' targets ONLY if explicitly requested */

    if (
	(TTD.HWTarget && !strcmp("null", TTD.HWTarget) && (FNs = TTD.FN_hw_null)) ||
#ifdef CONF_TT_HW_TWIN
	AUTOTRY4(twin) ||
#endif
#ifdef CONF_TT_HW_GTK
	AUTOTRY4(gtk) ||
#endif
#ifdef CONF_TT_HW_X11
	AUTOTRY4(X11) ||
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

static ttbyte BeforeOpenTarget(TT_CONST byte *arg) {
    TT_CONST byte *t;
    ttbyte * d, * s;
    
    if (!TTAssertAlways(sizeof(s_ttfn_ttobj) == TT_OFFSETOF(ttfn_ttobj,New) + 4 * sizeof(void (*)())))
	return TT_FALSE;
    
    if (!TTD.OpenFlag && TTD.PanicFlag) {
	/* cleanup after a failed TTOpen() */
	TTD.QuitArg = TT_TRUE;
	RealClose();
    }
    
    /* if client did not ask for a specific target, pick from environment */
    t = arg ? arg : (TT_CONST ttbyte *)getenv("TTDISPLAY");

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
	    return TT_FALSE;
	}
    }
    return TT_TRUE;
}

static void AfterOpenTarget(ttfns FNs) {
    if (FNs) {
#ifdef CONF_SOCKET_PTHREADS
	TTD.rErrno_.max = TTD.rErrno_.last = 0;
#else
	TTD.rErrno_.E = TTD.rErrno_.S = 0;
#endif
	CommonErrno = 0;
	TTD.OpenFlag = TT_TRUE;
	TTD.PanicFlag = TTD.QuitArg = TT_FALSE;
	    
	TTD.refcount = ttobj_refcount_alive;
    
	InitFNs(FNs);
	
    } else {
	if (TTD.HWTarget)
	    TTFreeMem(TTD.HWTarget);
	TTD.HWOptions = TTD.HWTarget = NULL;

	TTD.PanicFlag = TRUE;
    }
}

static ttbyte Open(TT_CONST byte *arg) {
    ttfns FNs;

    if (TTD.OpenFlag) {
	CommonErrno = TT_EALREADY_CONN;
	return TT_FALSE;
    }
    
    if (!BeforeOpenTarget(arg))
	return TT_FALSE;
    
    if ((FNs = OpenTarget())) {

	AfterOpenTarget(FNs);
	
	TTD.Menubar = NULL;
	TTD.Application = NULL;
	
	
	/* correctly initialize TTD.DummyTheme */
	TNEW2(tttheme, &s_DummyTheme);
	TTCopyMem((ttbyte *)&s_RealDummyTheme + TT_OFFSETOF(tttheme, name),
		  (ttbyte *)&s_DummyTheme + TT_OFFSETOF(tttheme, name),
		  sizeof(s_tttheme) - TT_OFFSETOF(tttheme, name));
	TTD.DummyTheme = &s_DummyTheme;

    } else {
	AfterOpenTarget(FNs);
    }
    return !!FNs;
}

ttbyte Reopen(TT_CONST ttbyte *arg) {
    ttfns FNs;
    ttbyte ret = TT_FALSE;
    
    if (TTD.OpenFlag) {
	CloseTarget();
	if (BeforeOpenTarget(arg)) {
	    FNs = OpenTarget();
	    AfterOpenTarget(FNs);
	    /* FIXME: should also rebuild native objects */
	    ret = !!FNs;
	}
    }
    return ret;
}

ttbyte TTOpen(TT_CONST ttbyte *arg) {
    ttbyte ret;
    th_r_mutex_lock(mutex); /* Lock but do not increase TTD.refcount */
    ret = Open(arg);
    th_r_mutex_unlock(mutex);
    return ret;
}

ttbyte TTReopen(TT_CONST ttbyte *arg) {
    ttbyte ret;
    th_r_mutex_lock(mutex); /* Lock but do not increase TTD.refcount */
    ret = Reopen(arg);
    th_r_mutex_unlock(mutex);
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
#if defined(CONF_TT_HW_TWIN) || defined(CONF_TT_HW_GTK) || defined(CONF_TT_HW_X11)
	return "all compiled-in display targets ("
# ifdef CONF_TT_HW_TWIN
	    "twin"
# endif
# ifdef CONF_TT_HW_GTK
#  ifdef CONF_TT_HW_TWIN
	    ", "
#  endif
	    "gtk"
# endif
# ifdef CONF_TT_HW_X11
#  if defined(CONF_TT_HW_TWIN) || defined(CONF_TT_HW_GTK)
	    ", "
#  endif
	    "X11"
# endif
# ifdef CONF__MODULES
	    ") and all probed modules failed"
# else
	    ") failed and modules not enabled"
# endif
	    "\n\t(all known targets: twin, gtk, X11, (xml), (null))";
#else /* !(defined(CONF_TT_HW_TWIN) || defined(CONF_TT_HW_GTK) || defined(CONF_TT_HW_X11)) */
# ifdef CONF__MODULES
	return "no display target compiled in, and all probed modules failed"
	    
	    "\n\t(all known targets: twin, gtk, X11, (xml), (null))";
# else
	return "no display target compiled in, and modules not enabled. Please recompile libTT !!";
# endif /* CONF__MODULES */
#endif /* defined(CONF_TT_HW_TWIN) || defined(CONF_TT_HW_GTK) || defined(CONF_TT_HW_X11) */
	
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





/* functions to manage Ids */

static ttobj *IdList[order_n];
static opaque IdSize[order_n], IdTop[order_n], IdBottom[order_n];


TT_INLINE opaque IdListGrow(ttbyte i) {
    opaque oldsize, size;
    ttobj *newIdList;
    
    oldsize = IdSize[i];
    if (oldsize >= MAXID)
	return NOSLOT;

    size = oldsize < SMALLBUFF/3 ? SMALLBUFF/2 : oldsize + (oldsize>>1);
    if (size > MAXID)
	size = MAXID;
    
    if (!(newIdList = (ttobj *)TTReallocMem(IdList[i], size*sizeof(ttobj))))
	return NOSLOT;
    
    TTWriteMem(newIdList+oldsize, 0, (size-oldsize)*sizeof(ttobj));
    
    IdList[i] = newIdList;
    IdSize[i] = size;
    
    return oldsize;
}

TT_INLINE void IdListShrink(ttbyte i) {
    ttobj *newIdList;
    opaque size = TT_MAX2(BIGBUFF, IdTop[i] << 1);
    
    if (size < IdSize[i] && (newIdList = (ttobj *)TTReallocMem(IdList[i], size*sizeof(ttobj)))) {
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
    return TT_FALSE;
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
    return TT_FALSE;
}

static void DropId(ttobj Obj) {
    if (Obj)
	_DropId(Obj->id >> magic_shift, Obj);
}



/* type query helpers */


TT_INLINE ttbyte IsInstance_ttfn(ttfn_ttobj FN, ttfn_ttobj t) {
    return FN && t && (t->magic & FN->magicmask) == FN->magic;
}

TT_INLINE ttbyte InstanceOf(ttfn_ttobj FN, ttobj o) {
    return o && IsInstance_ttfn(FN, o->FN);
}


/* ttfn_ttobj <-> tt_fn conversion */

/* unused *//*
TT_INLINE opaque Fn2Id(ttfn_ttobj FN) {
    return FN ? FN->order : NOID;
}
*/

TT_INLINE ttfn_ttobj Id2Fn(opaque FN) {
    if (FN >= order_ttobj && FN < order_n)
	return TFNs[FN];
    return NULL;
}
#define ID2FN(FN) Id2Fn((opaque)(FN))


/* ttobj <-> tobj conversion */


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

ttbyte TTIsInstance_ttfn(tt_fn FN, tt_fn t) {
    ttbyte ret;
    if ((opaque)FN < order_n)
	ret = IsInstance_ttfn(ID2FN(FN), ID2FN(t));
    else if ((opaque)FN == order_n)
	/* handle the case FN is TTFN_ttfn */
	ret = (opaque)FN == (opaque)t;
    else if ((opaque)FN > type_first && (opaque)FN <= type_last)
	/* handle the case FN and t are one of TTFN_ttbyte...TTFN_tttype */
	ret = (opaque)FN == (opaque)t;

    return ret;
}
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
static ttfn GetSuper_ttfn(ttfn FN) {
    ttfn FN_super;
    opaque i;
    
    /*
     * this could be *MUCH* smarter than a dumb linear search,
     * but hardcoding the class tree here would make it impossible
     * to create more classes at runtime.
     */
    if (FN) {
	for (i = (opaque)FN->order - 1; i >= order_ttobj; i--) {
	    if ((FN_super = ID2FN(i)) && (FN->magic & FN_super->magicmask) == FN_super->magic)
		return FN_super;
	}
    }
    return (ttfn)0;
}

tt_fn TTGetSuper_ttfn(tt_fn fn) {
    ttfn fn_super;
    
    if ((fn_super = GetSuper_ttfn(ID2FN(fn))))
	return (tt_fn)fn_super->order;

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


/* tteventmask management */
tt_obj TTGetInstalled_tteventmask(void) {
    tt_obj ret;
    LOCK;
    ret = (tt_obj) OBJ2ID(TTD.InstalledEM);
    UNLK;
    return ret;
}
tt_obj TTGetDefault_tteventmask(void) {
    tt_obj ret;
    LOCK;
    ret = (tt_obj) OBJ2ID(TTD.DefaultEM);
    UNLK;
    return ret;
}
void TTSetInstalled_tteventmask(tt_obj em) {
    tteventmask a0, a1;
    LOCK;
    a0 = ID2(tteventmask,em);
    a1 = TTD.InstalledEM;
    if (a0 != a1) {
	if (a1) TDEL(a1);
	TTD.InstalledEM = a0;
	
	FireEventLoop();
    }
    UNLK;
}
void TTSetDefault_tteventmask(tt_obj em) {
    tteventmask a0, a1;
    LOCK;
    a0 = ID2(tteventmask,em);
    a1 = TTD.DefaultEM;
    if (a0 != a1) {
	if (a1) TDEL(a1);
	TTD.DefaultEM = a0;
	
	FireEventLoop();
    }
    UNLK;
}


/* ttcallback management */

TT_INLINE void DelAllCallbacks_ttcomponent(ttcomponent o) {
    ttcallback l, next;
    if (o) {
	next = o->callbacks;
	while ((l = next)) {
	    next = l->next;
	    TDEL(l);
	}
	/* paranoia */
	o->callbacks = NULL;
    }
}

/* ttlistener AVL management */

#define ttavl2ttlistener(avl) ( (avl) ? (ttlistener) ((ttbyte *)(avl) - TT_OFFSETOF(ttlistener,AVL)) : (ttlistener)0 )
#define ttlistener2ttavl(clk) ( (clk) ? &(clk)->AVL : (ttavl)0 )


TT_INLINE ttuint AVLEventKey(ttevent ev) {
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


static int CompareListenerAVLs(ttavl_c a1, ttavl_c a2) {
    
    return CompareEvents( ttavl2ttlistener(a1)->event, ttavl2ttlistener(a2)->event );
}

static ttlistener FindListener4Event(ttlistener base, ttevent ev) {
    s_ttlistener evc;
    ttavl key;
    
    /*
     * no need to fully initialize evc here...
     * just evc.AVL and evc.ev are used
     * 
     * if (!TNEW2(ttlistener, &evc))
     *     return (ttlistener)0;
     */
    evc.AVL.AVLkey = AVLEventKey(ev);
    evc.event = ev;
    
    key = AVLFind(ttlistener2ttavl(&evc), ttlistener2ttavl(base), CompareListenerAVLs);
    
    /*
     * so no need to cleanup evc too
     * 
     * TDEL(&evc);
     */
    return key ? ttavl2ttlistener(key) : (ttlistener)0;
}

static void CallFunctionPlain(ttlistener_fn function, ttuint nargs, ttany *args) {
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

static void CallListener(ttlistener c, tobj o_id, tobj ev_id) {

    if (!c->function)
	return;

    if (c->args) {
	/* if c->narg_component is valid, set corresponding arg to ttcomponent o */
	if (c->narg_component < c->nargs)
	    c->args[c->narg_component] = o_id;
	/* if c->narg_event is valid, set corresponding arg to ttevent ev */
	if (c->narg_event < c->nargs)
	    c->args[c->narg_event] = ev_id;
    }
    
    if (c->lflags & ttlistener_lflags_builtin)
	
	CallMethod((opaque)c->function, c->nargs, c->args);
    
    else if (c->lflags & ttlistener_lflags_function_array)
	
	((ttlistener_fn_array)c->function) (c->nargs, c->args);
    
    else
	
	CallFunctionPlain(c->function, c->nargs, c->args);
}

static ttbyte IsBlockedEventBy(ttevent ev, tteventmask em) {
    ttuint truth_table = em->truth_table, evtype = ev->evtype, evcode = ev->evcode;
    tt_obj c = (tt_obj)OBJ2ID(ev->component);
    ttbyte e1true, e2true, ctrue;
    
    /* default value: do not block any evtype */
    e1true = TT_FALSE;
    if (em->evtype_mask)
	e1true = TTIsMember_ttbitmask((tt_obj)em->evtype_mask->id, (ttany)evtype);
    
    /* default value: block all evcodes */
    e2true = TT_FALSE;
    if (em->evcode_mask)
	e2true = TTIsMember_ttbitmask((tt_obj)em->evcode_mask->id, (ttany)evcode);
    
    /* default value: block all components */
    ctrue = TT_TRUE;
    if (em->component_mask)
	ctrue = TTIsMember_ttbitmask((tt_obj)em->component_mask->id, (ttany)c);
    
    if (e1true)
	truth_table >>= 1;
    if (e2true)
	truth_table >>= 2;
    if (ctrue)
	truth_table >>= 4;
    
    return (truth_table & 0x1);
}

static ttbyte IsBlockedEvent(ttevent ev) {
    ttlistener l;
    tteventmask em;

    if ((em = TTD.InstalledEM)) {
	if (IsBlockedEventBy(ev, em))
	    return TT_TRUE;
    }
    
    for (l = TTD.FirstL; l; l = l->inprogress_next) {
	
	if ((em = l->event_mask) || (em = TTD.DefaultEM)) {
	    if (IsBlockedEventBy(ev, em))
		return TT_TRUE;
	}
    }
    return TT_FALSE;
}

TT_INLINE void PrependInprogressListener(ttlistener o) {
    if ((o->inprogress_next = TTD.FirstL))
	TTD.FirstL->inprogress_prev = o;
    else
	TTD.LastL = o;
    TTD.FirstL = o;
    o->inprogress_prev = NULL;
}

TT_INLINE void RemoveInprogressListener(ttlistener o) {
    if (o->inprogress_prev)
	o->inprogress_prev->inprogress_next = o->inprogress_next;
    else
	TTD.FirstL = o->inprogress_next;
    if (o->inprogress_next)
	o->inprogress_next->inprogress_prev = o->inprogress_prev;
    else
	TTD.LastL = o->inprogress_prev;
    o->inprogress_prev = o->inprogress_next = NULL;
}

TT_INLINE void AppendQueuedEvent(ttevent o) {
    if ((o->prev = TTD.LastE))
	TTD.LastE->next = o;
    else
	TTD.FirstE = o;
    TTD.LastE = o;
    o->next = NULL;
}

TT_INLINE void RemoveQueuedEvent(ttevent o) {
    if (o->prev)
	o->prev->next = o->next;
    else
	TTD.FirstE = o->next;
    if (o->next)
	o->next->prev = o->prev;
    else
	TTD.LastE = o->prev;
    o->prev = o->next = NULL;
}

/*
 * this must come *after*
 * #include "wrap_m4.h"
 * as it uses a lot of stuff from it
 */
TT_INLINE ttevent FireOneEvent(TT_ARG_DIE ttevent ev) {
    /*
     * this is DANGEROUS code. we must protect c and from being deleted
     * inside the listener function (o and ev are already protected)
     */
    ttlistener c = (ttlistener)0, cnext;
    ttcomponent o = (ttcomponent)0;
    tobj o_id, ev_id;
    ttevent evnext;
    
    if (ev && (o = ev->component) && !(ev->evflags & ttevent_evflags_consumed)) {
	
	if (o->listeners &&
	    (c = FindListener4Event(o->listeners, ev))) {

	    o_id = OBJ2ID(o);
	    ev_id = OBJ2ID(ev);

	    /* set flag saying ev is being fired: this ensures ev is fired only once! */
	    ev->evflags |= ttevent_evflags_firing;
	    
	    for (; c && !(ev->evflags & ttevent_evflags_consumed); c = cnext) {

		/* protect c from being deleted */
		c->refcount++;

		/* push the mask of blocked events onto the stack */
		PrependInprogressListener(c);

		/* actually call the listener function */
		CallListener(c, o_id, ev_id);
		
		/* get next listener NOW, as CallListener() above
		 * may have added/removed listeners */
		cnext = c->next;
		
		/* pop the mask of blocked signals from the stack */
		RemoveInprogressListener(c);
		
		/* delete c if needed */
		if (!--c->refcount)
		    TDEL(c);
	    }
	}
    }

    /* unprotect (o) and delete if needed (GENERATES EVENTS!!!) */
    if (o && !--o->refcount)
	TDEL(o);
	
    /* get next event and dequeue ev */
    evnext = ev->next;
    RemoveQueuedEvent(ev);
	
    /* delete ev */
    ev->refcount--;
    TDEL(ev);
    
    return evnext;
}

static void FireEventLoop(void) {
    ttevent ev, evnext;
    
    for (ev = TTD.FirstE; ev; ev = evnext) {
	
	if ((ev->evflags & ttevent_evflags_firing) || IsBlockedEvent(ev))
	    /*
	     * this event is currently being fired or blocked.
	     * skip it (but keep it queued)
	     */
	    evnext = ev->next;
	else
	    /* fire ev */
	    evnext = FireOneEvent(ev);
    }
}

/* this is the main entry point to fire an event */
static void FireEvent(TT_ARG_DIE ttevent ev, ttcomponent o) {
    if (!ev)
	return;
    
    if (!o) {
	TDEL(ev);
	return;
    }
    
    /* QueueEvent() */
    /* protect o and ev from being deleted */
    ev->refcount++;
    o->refcount++;
    ev->component = o;

    AppendQueuedEvent(ev);

    FireEventLoop();
}

static void FireSimpleEvent(ttcomponent o, ttuint evtype) {
    ttevent ev;
    
    if ((ev = Create_ttevent(evtype, 0, 0)))
        FireEvent(ev, o);
    /*
     * FireEvent() also deletes ev.
     */
}

static void FireChangeEvent(ttcomponent o, ttuint which, ttany value, ttany old_value, ttopaque len) {
    tteventbig ev;
    
    if ((ev = Create4s4_tteventbig(ttevent_evtype_change, which, 0, value, old_value, len, NULL)))
	FireEvent((ttevent)ev, o);
    /*
     * FireEvent() also deletes ev.
     */
}


static TT_CONST struct s_tavl empty_AVL;

static void Sync_ttlistener(ttcomponent o, ttlistener c, ttlistener c_old) {
    ttavl node, nc = ttlistener2ttavl(c), nc_old = ttlistener2ttavl(c_old);
    
    *nc = *nc_old;
    
    if ((node = nc->AVLParent)) {
	if (node->AVLLeft == nc_old)
	    node->AVLLeft = nc;
	else if (TTAssert(node->AVLRight == nc_old))
	    node->AVLRight = nc;
    } else if (TTAssert(o->listeners == c_old))
	o->listeners = c;
    
    if ((node = nc->AVLLeft))
	node->AVLParent = nc;
    if ((node = nc->AVLRight))
	node->AVLParent = nc;
    
    *nc_old = empty_AVL;
}

static void AddTo_ttlistener(ttlistener c, ttcomponent o) {
    ttavl node;
    ttlistener base;
    
    if (c && TTAssert(IS(ttlistener,c)) && o && TTAssert(IS(ttcomponent,o)) &&
	!c->component && c->event && !c->prev && !c->next &&
	!TTCmpMem(&c->AVL, &empty_AVL, sizeof(s_ttavl))) {

	c->AVL.AVLkey = AVLEventKey(c->event);

	if ((base = FindListener4Event(o->listeners, c->event))) {
	    if (base && !(c->lflags & ttlistener_lflags_before)) {
		/* append: linear search through existing list (ugly!) */
		while (base->next)
		    base = base->next;
		c->prev = base;
		base->next = c;
	    } else {
		/* prepend */

		Sync_ttlistener(o, c, base);
		
		c->next = base;
		base->prev = c;
	    }
		
	} else {
	    node = ttlistener2ttavl(o->listeners);
	    AVLInsert(ttlistener2ttavl(c), node, CompareListenerAVLs, &node);
	    o->listeners = ttavl2ttlistener(node);
	}
	return /* c */ ;
    } else
	SetErrno(TT_EBAD_ARG, c && c->event ? 2 : 1);
    
    return /* (ttlistener)0 */ ;
}

static void Remove_ttlistener(ttlistener c) {
    ttcomponent o;
    ttavl node;
    ttlistener base;
    
    if (c && TTAssert(IS(ttlistener,c)) && c->event && (o = c->component)) {
	
	/* we have an AVL tree, where each node is the head of a linked list */
	if ((base = FindListener4Event(o->listeners, c->event))) {
	    
	    if (c->prev)
		c->prev->next = c->next;
	    else if (!TTAssert(c == base))
		return;

	    if (c->next)
		c->next->prev = c->prev;
	    
	    if (!c->prev) {
		if ((base = c->next)) {
		    /* we removed the head of the linked list, sync the AVL tree */
		    
		    Sync_ttlistener(o, base, c);
		    
		    base->prev = c->next = (ttlistener)0;
		    
		} else {
		    /* we removed the last node of the linked list... rebalance the AVL tree */
		    node = ttlistener2ttavl(o->listeners);
		    AVLRemove(ttlistener2ttavl(c), CompareListenerAVLs, &node);
		    o->listeners = ttavl2ttlistener(node);
		}
	    }
	}
    }
}

/* delete using a finite stack */
TT_INLINE void DelAll_ttlistener(ttlistener base) {
    ttlistener next;
    ttavl node;
    
    while (base) {
	if ((node = base->AVL.AVLLeft))
	    base = ttavl2ttlistener(node);
	else if ((node = base->AVL.AVLRight))
	    base = ttavl2ttlistener(node);
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
		base->prev = base->next = (ttlistener)0;
		base->component = (ttcomponent)0;
		TDEL(base);
		
		base = next;
	    }
	    base = ttavl2ttlistener(node);
	}
    }
}

TT_INLINE void DelAllListeners_ttcomponent(ttcomponent o) {
    if (o) {
	if (o->listeners)
	    DelAll_ttlistener(o->listeners);
	o->listeners = (ttlistener)0;
    }
}


TT_INLINE int CompareTF(time_t t1, frac_t f1, time_t t2, frac_t f2) {
    return t1 < t2 ? -1 :
    (
     t1 > t2 ? 1 :
     (
      f1 < f2 ? -1 : f1 > f2 ? 1 : 0
     )
    );
}

static int CompareTimers(TT_CONST tttimer t1, TT_CONST tttimer t2) {
    return CompareTF(t1->t, t1->f, t2->t, t2->f);
}

static void GetNow(void) {
    struct timeval now;

    gettimeofday(&now, NULL);
    TTD.TNow = now.tv_sec;
    TTD.FNow = (frac_t)now.tv_usec MicroSECs;
}

static void Activate_tttimer(tttimer o, ttbyte active) {
    tttimer t = TTD.FirstT;

    if (active && CompareTF(o->t, o->f, TTD.TNow, TTD.FNow) > 0) {
	/* insert */
	if (o->timer_prev || TTD.FirstT == o ||
	    o->timer_next || TTD.LastT == o)
	    return;
	
	while (t && CompareTimers(t, o) < 0)
	    t = t->timer_next;
	if (t) {
	    /* insert in middle */
	    if ((o->timer_prev = t->timer_prev))
		t->timer_prev->timer_next = o;
	    else
		TTD.FirstT = o;
	    
	    if ((o->timer_next = t->timer_next))
		t->timer_next->timer_prev = o;
	    else
		TTD.LastT = o;
	} else {
	    /* append at end */
	    if ((o->timer_prev = TTD.LastT))
		TTD.LastT->timer_next = o;
	    else
		TTD.FirstT = o;
	    TTD.LastT = o;
	}
    } else {
	/* remove */
	if (!(o->timer_prev || TTD.FirstT == o ||
	      o->timer_next || TTD.LastT == o))
	    return;
	    
	if (o->timer_prev)
	    o->timer_prev->timer_next = o->timer_next;
	else
	    TTD.FirstT = o->timer_next;
	
	if (o->timer_next)
	    o->timer_next->timer_prev = o->timer_prev;
	else
	    TTD.LastT = o->timer_prev;
	
	o->timer_prev = o->timer_next = NULL;
    }
}

TT_INLINE void DelAllTimers_ttcomponent(ttcomponent o) {
    if (o) {
	while (o->timers)
	    TDEL(o->timers);
    }
}


/* ttdata AVL management */

#define ttavl2ttdata(avl) ( (avl) ? (ttdata) ((ttbyte *)(avl) - TT_OFFSETOF(ttdata,AVL)) : (ttdata)0 )
#define ttdata2ttavl(data) ( (data) ? &(data)->AVL : (ttavl)0 )


/* compute the hash value of a string */
#define LOG_MAX_HASH	(8*sizeof(ttuint))

static ttuint AVLStringKey(TT_CONST ttbyte *key, ttopaque len) {
    ttuint h = len << (LOG_MAX_HASH - 8), i = LOG_MAX_HASH / 2;
    while (len) {
	h += ((*key++) ^ i) << i;
	if ((i += 4) >= LOG_MAX_HASH)
	    i = 0;
	len--;
    }
    return h;
}

TT_INLINE int CompareDatas(TT_CONST ttdata d1, TT_CONST ttdata d2) {
    if (d1->AVL.AVLkey == d2->AVL.AVLkey) {
	if (d1->key_len == d2->key_len) {
	    return TTCmpMem(d1->key, d2->key, d1->key_len);
	}
	return d1->key_len - d2->key_len;
    }
    return d1->AVL.AVLkey - d2->AVL.AVLkey;
}


static int CompareDataAVLs(ttavl_c a1, ttavl_c a2) {
    
    return CompareDatas( ttavl2ttdata(a1), ttavl2ttdata(a2) );
}

TT_INLINE ttdata Find_ttdata(ttdata key, TT_CONST ttdata base) {

    ttavl avl = AVLFind(ttdata2ttavl(key), ttdata2ttavl(base), CompareDataAVLs);

    return ttavl2ttdata(avl);
}
static ttdata FindByKey_ttdata(ttdata base, TT_CONST ttbyte *key, ttopaque len) {
    s_ttdata d;
    /*
     * no need to fully initialize d here...
     * just d.AVL, d.key and d.key_len are used
     * 
     * if (!TNEW2(ttdata, &d))
     *     return (ttdata)0;
     */
    d.AVL.AVLkey = AVLStringKey(key, len);
    d.key = (ttbyte *)key;
    d.key_len = len;
    return Find_ttdata(&d, base);
    /*
     * so no need to cleanup d too
     * 
     * TDEL(&d);
     */
}

static void AddTo_ttdata(ttdata d, ttcomponent o, ttbyte quickndirty) {
    ttavl root;
    ttdata base;
    
    if (d && TTAssert(IS(ttdata,d)) && o && TTAssert(IS(ttcomponent,o)) &&
	!d->component && !TTCmpMem(&d->AVL, &empty_AVL, sizeof(s_ttavl))) {

	d->AVL.AVLkey = AVLStringKey(d->key, d->key_len);

	if (!quickndirty && (base = Find_ttdata(o->datas, d))) {
	    /* already exists, replace: */
	    base->data = d->data;
	    TDEL(d);
	} else {
	    d->component = o;
	    
	    root = ttdata2ttavl(o->datas);
	    AVLInsert(ttdata2ttavl(d), root, CompareDataAVLs, &root);
	    o->datas = ttavl2ttdata(root);
	}
	return /* d */ ;
    } else
	SetErrno(TT_EBAD_ARG, d ? 2 : 1);
    
    return /* (ttdata)0 */ ;
}

static void Remove_ttdata(ttdata d) {
    ttcomponent o;
    ttavl root;
    
    if (d && TTAssert(IS(ttdata,d)) && (o = d->component) &&
	TTCmpMem(&d->AVL, &empty_AVL, sizeof(s_ttavl))) {

	d->component = NULL;

	/* rebalance the AVL tree */
	root = ttdata2ttavl(o->datas);
	AVLRemove(ttdata2ttavl(d), CompareDataAVLs, &root);
	o->datas = ttavl2ttdata(root);
    }
}

static tt_fn SetData_ttdata(ttdata o, ttany data) {
    ttcomponent c;
    if (TTAssert(o && IS(ttdata,o)) && o->data != data) {
	if ((c = o->component) && o->key_len == 9 && !TTCmpMem(o->key, "user_data", 9))
	    c->user_data = data;
	    
	FIRE_EVENT(o->data = data, o, ttdata_data, data, o->data);
	return TTFN_ttany;
    }
    return (tt_fn)0;
}

static ttdata protected_Create_ttdata(ttcomponent c, TT_ARG_READ ttbyte *key, ttopaque key_len, ttany data) {
    ttdata d;
    ttbyte *dkey;
    
    if (c && TTAssert(c && IS(ttcomponent,c)) && key &&
	(dkey = TTCloneStrL(key, key_len+1))) {
	
	if ((d = TNEW_NOBUILD(ttdata))) {
	    d->key = dkey;
	    d->key_len = key_len;
	    /* no need to check again if key already exists */
	    AddTo_ttdata(d, c, TT_TRUE);
	    /* not o->data = data; we want to keep component->user_data in sync: */
	    SetData_ttdata(d, data);
	    return (ttdata)Build((ttobj)d);
	}
	TTFreeMem(dkey);
    }
    return NULL;
}


/* delete using a finite stack */
TT_INLINE void DelAll_ttdata(ttdata base) {
    ttavl node;
    
    while (base) {
	if ((node = base->AVL.AVLLeft))
	    base = ttavl2ttdata(node);
	else if ((node = base->AVL.AVLRight))
	    base = ttavl2ttdata(node);
	else {
	    /* destroy the tree while we back up */
	    if ((node = base->AVL.AVLParent)) {
		if (node->AVLLeft == &base->AVL)
		    node->AVLLeft = (ttavl)0;
		else if (node->AVLRight == &base->AVL)
		    node->AVLRight = (ttavl)0;
	    }
	    
	    TDEL(base);
	    base = ttavl2ttdata(node);
	}
    }
}

TT_INLINE void DelAllDatas_ttcomponent(ttcomponent o) {
    if (o) {
	if (o->datas)
	    DelAll_ttdata(o->datas);
	o->datas = (ttdata)0;
    }
}




static void DelAllExtras_ttcomponent(ttcomponent o) {
    DelAllCallbacks_ttcomponent(o);
    DelAllListeners_ttcomponent(o);
    DelAllTimers_ttcomponent(o);
    DelAllDatas_ttcomponent(o);
}

static void DelAll_ttobj(ttbyte quick) {
    ttuint n, qn, quick_list[] = { order_ttcallback, order_ttlistener, order_tttimer };
    ttopaque i;
    ttobj o;

    if (quick) {
	/* first, delete all callbacks, listeners and timers */
	for (qn = 2; qn != (ttuint)-1; qn--) {
	    n = quick_list[qn];
	    for (;;) {
		/* IdTop[n] decreases with us. */
		if ((i = IdTop[n] - 1) != (ttuint)-1) {
		    if (TTAssertAlways(o = IdList[n][i])) {
			/* ignore refcount... */
			o->refcount = 0;
			TDEL(o);
		    }
		} else
		    break;
	    }
	}
    }
    
    for (n = order_n - 1; n != (ttuint)-1; n--) {
	for (;;) {
	    /* IdTop[n] decreases with us. */
	    if ((i = IdTop[n] - 1) != (ttuint)-1) {
		if (TTAssertAlways(o = IdList[n][i])) {
		    /* ignore refcount... */
		    o->refcount = 0;
		    /*
		     * this is "the right thing" but may
		     * generate *LOTS* of delete events :(
		     */
		    TDEL(o);
		}
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
	ev = Create4_tteventbig(ttevent_evtype_expose, 0, 0, x, y, w, h);
	
	if (o->repaint) {
	    o->refcount++; /* protect o from being deleted */
	    o->repaint((tt_obj)OBJ2ID(o), x, y, w, h);
	    if (!--o->refcount)
		TDEL(o);
	} else
	    o->FN->BuiltinRepaint(o, x, y, w, h);
	
	if (ev)
	    /* also deletes ev */
	    FireEvent((ttevent)ev, (ttcomponent)o);
    }
}



/* MainLoop stuff */


void TTExitMainLoop(void) {
    /* no LOCK here */
    TTD.ExitMainLoopFlag = TRUE;
}

static void NormalizeTimeval(struct timeval *t) {
    while (t->tv_usec < 0) {
	t->tv_usec += 1000000;
	t->tv_sec--;
    }
    while (t->tv_usec >= 1000000) {
	t->tv_usec -= 1000000;
	t->tv_sec++;
    }
    if (t->tv_sec < 0 || (t->tv_sec == 0 && t->tv_usec < 0))
	t->tv_sec = t->tv_usec = 0;
}

static void ComputeDelay(struct timeval *t) {
    if (TTAssert(TTD.FirstT)) {
	t->tv_sec = TTD.FirstT->t - TTD.TNow;
	t->tv_usec = (TTD.FirstT->f - TTD.FNow) / MicroSEC;
	NormalizeTimeval(t);
    }
}

static ttbyte MainLoop(ttbyte wait) {
    fd_set fset;
    struct timeval t;
    tttimer c;
    int fd = -1;
    s_tt_errno *E;
    ttbyte ret = TT_TRUE, eff_wait;
    
    if (TTD.FirstT) {
	eff_wait = TT_FALSE;
	fd = THW.ConnectionFd();
	FD_ZERO(&fset);
    } else
	eff_wait = wait;
    
    for (;;) {

#if 0
	printf("THW.MainLoopOnce(eff_wait = %d", (int)eff_wait);
	fflush(stdout);
#endif
	ret = THW.MainLoopOnce(eff_wait);
#if 0
	printf(")\n");
#endif

	if (!ret || TTD.ExitMainLoopFlag)
	    break;
	
	if (!TTD.FirstT) {
	    
	    eff_wait = wait;
	    
	} else {
	    eff_wait = TT_FALSE;
	    
	    THW.Flush();
	    
	    if (fd >= 0)
		FD_CLR(fd, &fset);
	    fd = THW.ConnectionFd();
	    if (fd >= 0)
		FD_SET(fd, &fset);

	    GetNow();
	    ComputeDelay(&t);
		
#if 0
	    printf("select(%d, [%d], NULL, NULL, {%ld, %ld}",
		   fd+1, fd, (long)t.tv_sec, (long)t.tv_usec);
	    fflush(stdout);
#endif
	    UNLK;
	    select(fd+1, &fset, NULL, NULL, &t);
	    LOCK;
#if 0
	    printf(")\n");
#endif

	    while ((c = TTD.FirstT)) {
		GetNow();
		ComputeDelay(&t);
#if 0
		printf("timer 0x%X\t{%ld, %ld}\n",
		       (unsigned)c->id, (long)t.tv_sec, (long)t.tv_usec);
#endif
		if (t.tv_sec == 0 && t.tv_usec == 0) {
		    c->refcount++; /* protect c from being deleted */

		    /*
		     * timers are one-shot. to obtain periodic timers,
		     * they should reactivate themselves with c->FN->SetDelay()
		     */
		    Activate_tttimer(c, TT_FALSE);
		    
		    CallListener((ttlistener)c, c->component ? c->component->id : TT_NOID, c->id);
		    
		    if (!--c->refcount)
			TDEL(c);
		} else
		    break;
	    }
	}
    }

    if (!ret) {
	LOCK;
	E = GetErrnoLocation();
	E->E = THW.GetErrno() + TT_MAX_ERROR;
	E->S = THW.GetErrnoDetail();
	UNLK;
    }
    
    TTD.ExitMainLoopFlag = TT_FALSE;
    return ret;
}

ttbyte TTMainLoop(void) {
    return MainLoop(TT_TRUE);
}

ttbyte TTMainLoopOnce(ttbyte wait) {
    TTD.ExitMainLoopFlag = TT_TRUE;
    return MainLoop(wait);
}



/* TTD global */


s_tt_d TTD = {
#ifdef CONF_SOCKET_PTHREADS
    TH_R_MUTEX_INITIALIZER,
#endif
    (ttuint)0,
	
    (tttheme)0, (tttheme)0,
    (ttmenubar)0, (ttapplication)0,
	
    (tteventmask)0, (tteventmask)0,
	
    (ttevent)0, (ttevent)0,
	
    (ttlistener)0, (ttlistener)0,

    (tttimer)0, (tttimer)0,
    (time_t)0,
    (frac_t)0,

    (byte *)0, (byte *)0,
    (void *)0,
	
    (ttbyte)0, (ttbyte)0, (ttbyte)0, (ttbyte)0,
	
    { 0, }, { 0, }, NULL,
	
    {
#include "create_init_m4.h"
    },
    {
	IdList, IdSize, IdTop, IdBottom,
	    
#ifdef CONF_SOCKET_PTHREADS
	    GetErrnoLocation,
#endif
	    AssignId,
	    DropId,
	    Id2Obj,
	    FindNative,
	    GetNow,
	    
	    FireEvent,
	    FireSimpleEvent,
	    FireChangeEvent,
	    
	    AddTo_ttlistener,
	    Remove_ttlistener,
	    Activate_tttimer,
	    
	    AddTo_ttdata,
	    Remove_ttdata,
	    SetData_ttdata,
	    FindByKey_ttdata,
	    protected_Create_ttdata,
	    
	    DelAllExtras_ttcomponent,
	    
	    Expose_ttvisible,
	    
	    RealClose,
    },
    

    /* FN_hw_null */
    NULL,

    /* FNs */
    {
	(ttfn_ttobj)0, /* order_() == 0 */
#define el(t) (ttfn_ttobj)TFN(t),
	 TT_LIST(el)
#undef el
    },
    
	
    /* FN */
    /* { { 0, }, }, */
    
    /* FNdefault */
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

