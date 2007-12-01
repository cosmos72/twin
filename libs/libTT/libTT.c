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


#include <Tutf/Tutf_defs.h>
#include <Tw/Tw_defs.h>

/* include our internal copy of TT.h instead of the public one */
#include "TT.h"
#include <TT/TTerrno.h>

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

static void GetNow(void);

static ttobject FindNative(ttany id);

static void DropId(ttobj Obj);
static ttbyte FixedAssignIdArray(ttopaque ClassId, ttopaque o_n, TT_CONST ttobj *o_array);

static void FireSimpleEvent(ttcomponent o, ttuint evtype);
static void FireEvent(TT_ARG_DIE ttevent ev, ttcomponent o);
static void FireEventLoop(void);

ttuint TTGetMethodIdV(void *method);

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



void TTAssertFail (const char *assertion, const char *file, unsigned int line, const char *function) {
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
	CommonErrno = TT_EBAD_SIZES;
	return TT_FALSE;
    }
    return TT_TRUE;
}

static ttuint PrintInitError(tthw HW, TT_CONST ttbyte *target, ttbyte module) {
    s_tt_errno *L = GetErrnoLocation();
    /* set TTD.Class.HW to allow error reporting */
    if (HW)
	TTD.Class.HW = *HW;
    fprintf(stderr, "TT-ERROR: %s display target%s `%s' failed:\n\t%s%s\n",
	    module ? "loading" : "compiled in",
	    module ? " module" : "",
	    target ? target : (TT_CONST ttbyte *)"(NULL)",
	    TTStrError(L->E), TTStrErrorDetail(L->E, L->S));
    return 0;
}


static ttopaque ttclass_sizes[] = {
#define el(type) sizeof(TT_CAT(s_ttclass_,type)),
    TT_LIST(el)
#undef el
};

/*
 * initialize method structures, using values in TTD.FNdefault as default
 * and overloading them with ones set in FNs
 */
#define DeltaClass(i) ((ttopaque)((ttbyte *)TClasses[i] - (ttbyte *)TClasses[order_first]))

static void InitClasses(ttclasses Classes) {
    ttopaque i, isuper = 0, j, d;
    void **super1, **super2, **src1, **src2, **dst1, **dst2;
    void *c1, *c2;
    
    for (i = order_first; i < order_last; i++) {

	j = ttclass_sizes[0];
	
	if (i > order_first) {
	    isuper = TT_MAGIC_MASK & (ttopaque)TTGetSuper_ttclass((tt_obj)ORDER2ID_CLASS(i));
	    d = DeltaClass(isuper);
	    super1 = (void **)((ttbyte *)&TTD.Class_default + d + j);
	    super2 = (void **)((ttbyte *)&TTD.Class         + d + j);
	}
	d = DeltaClass(i);
	src1 =   (void **)((ttbyte *)TTD.Class_hw_null  + d + j);
	src2 =   (void **)((ttbyte *)Classes            + d + j);
	dst1 =   (void **)((ttbyte *)&TTD.Class_default + d + j);
	dst2 =   (void **)((ttbyte *)&TTD.Class         + d + j);
	
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
	for (j = ttclass_sizes[0]; j < ttclass_sizes[i - order_first]; j += sizeof(void *)) {
	    if (TTAssertAlways
		((c1 = *src1) || ((j < ttclass_sizes[isuper]) &&
				  ((c1 = *super2) || (c1 = *super1)))))
		*dst1 = c1;
	    
	    if ((c2 = *src2) || (c2 = c1))
		*dst2 = c2;

	    super1++, super2++;
	    src1++; src2++;
	    dst1++; dst2++;
	}
	
    }
    
    FixedAssignIdArray(id_ttclass_ttclass, order_last - order_first, (ttobj *)TClasses);
    
    /* set HW methods */
    TTD.Class.HW = Classes->HW;
    TTD.Class_default.HW = TTD.Class_hw_null->HW;
}

static void CloseTarget(void) {
    if (THW.Close)
	THW.Close();
    
    /* reinitialize at each TTClose() */
    InitClasses(TTD.Class_hw_null);

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
	
	if (!(TTD.refcount &= ~ttobject_refcount_alive))
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



#ifdef CONF__MODULES
# ifdef LIBDIR
static TT_CONST ttbyte *conf_libdir = LIBDIR "/TT/modules/HW" ;
# else
#  define conf_libdir "HW"
# endif

static ttclasses module_InitHW(tthw *HW) {
    /* put all known driver names here (for autoprobing) */
    /* do not autoprobe for `twin_detunnel', `null' or `xml' */
    TT_CONST ttbyte * name [] = {
# ifndef CONF_TT_HW_TWIN_TUNNEL
	    "twin_tunnel",
# endif
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
    
    
    /* len == strlen(LIBDIR "/TT/HW/twin_detunnel.so.x.y.z") */
    ttuint i, len = 20 + strlen(TT_PROTOCOL_VERSION_STR) + strlen(conf_libdir);
    ttbyte *buf;
    ttclasses Classes;
    ttclasses (*init_dl)(tthw *);
    
    if (TTD.HWTarget) {
	/* forbid exiting from LIBDIR */
	if (strchr(TTD.HWTarget, '/')) {
	    CommonErrno = TT_ETARGET_BAD;
	    return (ttclasses)0;
	}
	len += strlen( name[0] = TTD.HWTarget );
	/* if Target is specified, try *only* it */
	name[1] = NULL;
    }

    if ((buf = TTAllocMem(len))) {
	for (i = 0; i < sizeof(name)/sizeof(name[0]) && name[i]; i++) {
	    sprintf(buf, "%s/%s.so." TT_PROTOCOL_VERSION_STR, conf_libdir, name[i]);
	    if ((TTD.DlHandle = dlopen(buf, RTLD_NOW|RTLD_GLOBAL))) {
		init_dl = (ttclasses (*)(tthw *))dlsym(TTD.DlHandle, "InitModule");
		if (init_dl) {
		    if ((Classes = init_dl(HW))) {
			TTFreeMem(buf);
			return Classes;
		    } else {
			/* CommonErrno will be set by init_dl(HW, Classes) */
		    }
		} else {
		    CommonErrno = TT_ETARGET_DLERROR;
		    TTD.str_dlerror = dlerror();
		}
		/* do not dlclose() now, module may hold error messages */
		/* dlclose(TTD.DlHandle); */
	    } else {
		CommonErrno = TT_ETARGET_DLERROR;
		TTD.str_dlerror = dlerror();
	    }
	    PrintInitError(*HW, name[i], TRUE);
	}
	TTFreeMem(buf);
    } else
	CommonErrno = TT_ENO_MEM;

    return (ttclasses)0;
}
#else /* !CONF__MODULES */

static ttclasses module_InitHW(tthw *HW) {
    if (TTD.HWTarget)
	fprintf(stderr, "TT-ERROR: modules not enabled, cannot load display target `%s'\n",
		TTD.HWTarget);
    else
	fprintf(stderr, "TT-ERROR: modules not enabled, cannot load any display target\n");
    
    if (!CommonErrno)
	CommonErrno = TT_ETARGET_FAILED;
    return (ttclasses)0;
}

#endif /* CONF__MODULES */




static ttclasses module_InitHW(tthw *HW);
ttclasses _TT_null_InitHW(tthw *HW);
ttclasses _TT_twin_InitHW(tthw *HW);
ttclasses _TT_twin_tunnel_InitHW(tthw *HW);
ttclasses _TT_twin_detunnel_InitHW(tthw *HW);
ttclasses _TT_gtk_InitHW(tthw *HW);
ttclasses _TT_X11_InitHW(tthw *HW);
ttclasses _TT_xml_InitHW(tthw *HW);

typedef struct s_target_hw * target_hw;
struct s_target_hw {
    TT_CONST ttbyte *name;
    ttclasses (*InitHW)(tthw *HW);
    ttbyte autoprobe;
};

#define TARGET(t)	{ TT_STR(t), TT_CAT(TT_CAT(_TT_,t),_InitHW), TT_FALSE }
#define AUTOTARGET(t)	{ TT_STR(t), TT_CAT(TT_CAT(_TT_,t),_InitHW), TT_TRUE }

/* array of built-in targets */
static struct s_target_hw target_array [] = {
    /* placeholder for client-specified target */
    /* WARNING: this currently allows only for ONE client-specified target */
    { NULL, NULL, TT_TRUE },

#ifdef CONF_TT_HW_TWIN_TUNNEL
    AUTOTARGET(twin_tunnel),
#endif
    /*
     * cannot link twin_detunnel, it references twin server symbols
     * and would break *ALL* other libTT clients
     */
#if 0
# ifdef CONF_TT_HW_TWIN_DETUNNEL
    TARGET(twin_detunnel),
# endif
#endif
#ifdef CONF_TT_HW_TWIN
    AUTOTARGET(twin),
#endif
#ifdef CONF_TT_HW_GTK
    AUTOTARGET(gtk),
#endif
#ifdef CONF_TT_HW_X11
    AUTOTARGET(X11),
#endif
    
    /* use `null' or `xml' targets ONLY if explicitly requested */

#ifdef CONF_TT_HW_XML
    TARGET(xml),
#endif
    TARGET(null),
};
    
#define target_array_n (sizeof(target_array)/sizeof(target_array[0]))


/* WARNING: this currently allows only for ONE client-specified target */
ttbyte TTRegisterTarget(TT_CONST byte *name, ttclasses (*InitHW)(tthw *HW)) {
    target_array[0].name = name;
    target_array[0].InitHW = InitHW;
    return TT_TRUE;
}

static ttclasses OpenTarget(void) {
    ttclasses Classes = NULL;
    tthw HW;
    target_hw t;
    ttuint i;
    
    /* reinitialize at each TTOpen(). this also sets ->order, ->magicmask, ->magic, ->size */
    TTD.Class_default = TTD.Class = *(TTD.Class_hw_null = _TT_null_InitHW(&HW));
    
#define CHK4(name)	(TTD.HWTarget && !strcmp(name, TTD.HWTarget))
#define AUTOCHK4(name)	(!TTD.HWTarget || !strcmp(name, TTD.HWTarget))
#define ERROR4(name)	PrintInitError(HW, name, TT_FALSE)
    
    for (i = 0; i < target_array_n; i++) {
	t = &target_array[i];
	if (t->name && t->InitHW && (t->autoprobe ? AUTOCHK4(t->name) : CHK4(t->name)) &&
	    ((Classes = t->InitHW(&HW)) || ERROR4(t->name))) {
	    
	    break;
	}
    }
#undef CHK4
#undef AUTOCHK4
#undef ERROR4

    if (!Classes)
	Classes = module_InitHW(&HW);
	
    if (!Classes) {
	if (!CommonErrno || CommonErrno == TT_ETARGET_DLERROR || CommonErrno >= TT_MAX_ERROR)
	    CommonErrno = TT_ETARGET_FAILED;
    }
    
    return Classes;
}

static ttbyte Subsystems_Init(void) {
    ttbyte _TT_field_Init(void);
    ttbyte _TT_method_Init(void);

    return
	_TT_field_Init() &&
	_TT_method_Init();
}

static ttbyte BeforeOpenTarget(TT_CONST byte *arg) {
    
    TT_CONST byte *t;
    ttbyte * d, * s;
    
    if (!TTAssertAlways(sizeof(s_ttclass_ttobj) ==
			TT_OFFSETOF(ttclass_ttobject,New)) ||
	!TTAssertAlways(sizeof(s_ttclass_ttobject) ==
			sizeof(s_ttclass_ttobj) + 4 * sizeof(void (*)(void))))
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
    
    return Subsystems_Init();
}

static void AfterOpenTarget(ttclasses Classes) {
    if (Classes) {
#ifdef CONF_SOCKET_PTHREADS
	TTD.rErrno_.max = TTD.rErrno_.last = 0;
#else
	TTD.rErrno_.E = TTD.rErrno_.S = 0;
#endif
	CommonErrno = 0;
	TTD.OpenFlag = TT_TRUE;
	TTD.PanicFlag = TTD.QuitArg = TT_FALSE;
	    
	TTD.refcount = ttobject_refcount_alive;
    
	InitClasses(Classes);

    } else {
	if (TTD.HWTarget)
	    TTFreeMem(TTD.HWTarget);
	TTD.HWOptions = TTD.HWTarget = NULL;

	TTD.PanicFlag = TRUE;
    }
}

static ttbyte Open(TT_CONST byte *arg) {
    ttclasses Classes;

    if (TTD.OpenFlag) {
	CommonErrno = TT_ETARGET_ALREADY_OPEN;
	return TT_FALSE;
    }
    
    if (!BeforeOpenTarget(arg))
	return TT_FALSE;
    
    if ((Classes = OpenTarget())) {
	AfterOpenTarget(Classes);
	
	TTD.Menubar = NULL;
	TTD.Application = NULL;
	
	
	/* correctly initialize TTD.DummyTheme */
	TNEW2(tttheme, &s_DummyTheme);
	TTCopyMem((ttbyte *)&s_RealDummyTheme + TT_OFFSETOF(tttheme, name),
		  (ttbyte *)&s_DummyTheme + TT_OFFSETOF(tttheme, name),
		  sizeof(s_tttheme) - TT_OFFSETOF(tttheme, name));
	TTD.DummyTheme = &s_DummyTheme;

    } else {
	AfterOpenTarget(Classes);
    }
    return !!Classes;
}

ttbyte Reopen(TT_CONST ttbyte *arg) {
    ttclasses Classes;
    ttbyte ret = TT_FALSE;
    
    if (TTD.OpenFlag) {
	CloseTarget();
	if (BeforeOpenTarget(arg)) {
	    Classes = OpenTarget();
	    AfterOpenTarget(Classes);
	    /* FIXME: should also rebuild native objects */
	    ret = !!Classes;
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
      case TT_EBAD_SIZES:
	return "compiled data sizes are incompatible with libTT now in use!";
      case TT_ENO_MEM:
	return "Out of memory!";
      case TT_ETARGET_ALREADY_OPEN:
	return "already opened";
	
      case TT_ETARGET_DLERROR:
	/*return "unable to load display target module: ";*/
	return "";
	
      case TT_ETARGET_FAILED:
#if defined(CONF_TT_HW_TWIN_TUNNEL) || defined(CONF_TT_HW_TWIN) || defined(CONF_TT_HW_GTK) || defined(CONF_TT_HW_X11)
	return "all compiled-in display targets failed ("
# ifdef CONF_TT_HW_TWIN_TUNNEL
	    "twin_tunnel"
# endif
# ifdef CONF_TT_HW_TWIN
#  if defined(CONF_TT_HW_TWIN_TUNNEL)
	    ", "
#  endif
	    "twin"
# endif
# ifdef CONF_TT_HW_GTK
#  if defined(CONF_TT_HW_TWIN_TUNNEL) || defined(CONF_TT_HW_TWIN)
	    ", "
#  endif
	    "gtk"
# endif
# ifdef CONF_TT_HW_X11
#  if defined(CONF_TT_HW_TWIN_TUNNEL) || defined(CONF_TT_HW_TWIN) || defined(CONF_TT_HW_GTK)
	    ", "
#  endif
	    "X11"
# endif
# ifdef CONF__MODULES
	    ")\n\tand all probed modules failed"
# else
	    ")\n\tand modules not enabled"
# endif
	    "\n\t(all known targets: twin_tunnel, twin, gtk, X11, [twin_detunnel], [xml], [null])";
#else /* !(defined(CONF_TT_HW_TWIN_TUNNEL) || defined(CONF_TT_HW_TWIN) || defined(CONF_TT_HW_GTK) || defined(CONF_TT_HW_X11)) */
# ifdef CONF__MODULES
	return "no display target compiled in, and all probed modules failed"
	    
	    "\n\t(all known targets: twin_tunnel, twin, gtk, X11, [twin_detunnel], [xml], [null])";
# else
	return "no display target compiled in, and modules not enabled. Please recompile libTT !!";
# endif /* CONF__MODULES */
#endif /* defined(CONF_TT_HW_TWIN) || defined(CONF_TT_HW_GTK) || defined(CONF_TT_HW_X11) */
	
      case TT_ETARGET_BAD:
	return "malformed TTDISPLAY environment variable";
	
      case TT_ETARGET_NO_EXTENSION:
	return "missing or failed `libTT' server extension, needed by ";

      case TT_ETARGET_BAD_EXTENSION_VERSION:
	return "need `libTT' server extension version "TT_PROTOCOL_VERSION_STR", found ";
	
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
    static ttbyte buf[] = "255.255.255";
    
    switch (E) {
      case TT_ETARGET_DLERROR:
	return TTD.str_dlerror ? TTD.str_dlerror : (ttbyte *)"";
      case TT_ETARGET_NO_EXTENSION:
	switch (S) {
	  case TT_EDETAIL_TARGET_TWIN_TUNNEL:
	    return "display target `twin_tunnel'";
	  case TT_EDETAIL_TARGET_TWIN_DETUNNEL:
	    return "display target `twin_detunnel'";
	  case TT_EDETAIL_TARGET_TWIN:
	    return "display target `twin'";
	  case TT_EDETAIL_TARGET_GTK:
	    return "display target `gtk'";
	  case TT_EDETAIL_TARGET_X11:
	    return "display target `X11'";
	  case TT_EDETAIL_TARGET_XML:
	    return "display target `xml'";
	  case TT_EDETAIL_TARGET_NULL:
	    return "display target `null' failed";
	}
	return "unknown display target";
      case TT_ETARGET_BAD_EXTENSION_VERSION:
	sprintf(buf, "%d.%d.%d", (S>>16)&0xFF, (S>>8)&0xFF, S&0xFF);
	return buf;
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

static ttobj *IdList[order_last];
static ttopaque IdSize[order_last], IdTop[order_last], IdBottom[order_last];


static ttopaque IdListGrow(ttopaque i, ttopaque size) {
    ttopaque oldsize;
    ttobj *newIdList;
    
    oldsize = IdSize[i];
    if (oldsize >= TT_MAXID)
	return TT_BADID;

    /* use size as a minimum hint for the new size */
    if (size && size < TT_MAXID)
	oldsize = size;
    
    size = oldsize < TT_SMALLBUFF/3 ? TT_SMALLBUFF/2 : oldsize + (oldsize>>1);
    if (size > TT_MAXID)
	size = TT_MAXID;
    
    if (!(newIdList = (ttobj *)TTReallocMem(IdList[i], size*sizeof(ttobj))))
	return TT_BADID;
    
    TTWriteMem(newIdList+oldsize, 0, (size-oldsize)*sizeof(ttobj));
    
    IdList[i] = newIdList;
    IdSize[i] = size;
    
    return oldsize;
}

TT_INLINE ttopaque FixedIdListGrow(ttbyte i, ttopaque size) {
    ttopaque oldsize;
    ttobj *newIdList;
    
    oldsize = IdSize[i];
    if (oldsize >= TT_MAXID)
	return TT_BADID;

    /* use size as exact new size */
    if (size > TT_MAXID)
	size = TT_MAXID;
    
    if (!(newIdList = (ttobj *)TTReallocMem(IdList[i], size*sizeof(ttobj))))
	return TT_BADID;
    
    TTWriteMem(newIdList+oldsize, 0, (size-oldsize)*sizeof(ttobj));
    
    IdList[i] = newIdList;
    IdSize[i] = size;
    
    return oldsize;
}

TT_INLINE void IdListShrink(ttopaque i) {
    ttobj *newIdList;
    ttopaque size = TT_MAX2(TT_BIGBUFF, IdTop[i] << 1);
    
    if (size < IdSize[i] && (newIdList = (ttobj *)TTReallocMem(IdList[i], size*sizeof(ttobj)))) {
	IdList[i] = newIdList;
	IdSize[i] = size;
    }
}

TT_INLINE ttopaque IdListGet(ttopaque i) {
    if (IdBottom[i] == IdSize[i])
	return IdListGrow(i, 0);
    
    return IdBottom[i];
}

TT_INLINE ttbyte _AssignId(ttopaque i, ttobj Obj) {
    ttopaque Id, j;
    
    if (i < order_last && (Id = IdListGet(i)) != TT_BADID) {
	Obj->id = Id | i << TT_MAGIC_SHIFT;

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

TT_INLINE ttbyte _FixedAssignId(ttopaque i, ttobj Obj) {
    ttopaque Id = Obj->id & TT_MAGIC_MASK, j;
    
    if (i < order_last && i == (Obj->id >> TT_MAGIC_SHIFT) &&
	((Id < IdTop[i] && !IdList[i][Id]) || IdListGrow(i, Id))) {

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

static ttbyte AssignId(TT_CONST ttclass Class, ttobj o) {
    if (Class && o)
	return _AssignId(Class->id & TT_MAGIC_MASK, o);
    return TT_FALSE;
}

static ttbyte FixedAssignId(TT_CONST ttclass Class, ttobj o) {
    if (Class && o)
	return _FixedAssignId(Class->id & TT_MAGIC_MASK, o);
    return TT_FALSE;
}

static ttbyte FixedAssignIdArray(ttopaque ClassId, ttopaque o_n, TT_CONST ttobj *o_array) {
    ttobj o;
    ttopaque used_n = 0, i, j, used_j;
    
    if (ClassId >= id_ttclass_first && ClassId < id_ttclass_last &&
	!IdTop[ i = ClassId & TT_MAGIC_MASK ] &&
	o_n && (o_n <= IdSize[i] || FixedIdListGrow(i, o_n)) != TT_BADID) {
	
	for (j = 0; j < o_n; j++) {
	    o = (ttobj)o_array[j];
	    if (TTAssertAlways(o && (o->id >> TT_MAGIC_SHIFT) == i &&
			       (used_j = o->id & TT_MAGIC_MASK) <= o_n)) {
		if (used_n < used_j)
		    used_n = used_j;
		
		IdList[i][used_j] = o;
	    } else
		return TT_FALSE;
	}
	IdBottom[i] = IdTop[i] = used_n;
	return TT_TRUE;
    }
    return TT_FALSE;
}


TT_INLINE void _DropId(ttopaque i, ttobj o) {
    ttopaque Id = o->id & TT_MAXID, j;

    /* paranoia: we might end up calling DropId() twice on the same object */
    if (Id < IdTop[i] && IdList[i][Id] == o) {
	if (IS(ttobject,o) && !(((ttobject)o)->oflags & ttobject_oflags_const))
	    /* cannot write into read-only objects... */
	    o->id = TT_NOID;
	IdList[i][Id] = (ttobj)0;
	if (IdBottom[i] > Id)
	    IdBottom[i] = Id;
	for (j = IdTop[i] - 1; j > IdBottom[i]; j--)
	    if (IdList[i][j])
		break;
	IdTop[i] = (j == IdBottom[i]) ? j : j + 1;
	
	if (IdSize[i] > TT_BIGBUFF && IdSize[i] > (IdTop[i] << 4))
	    IdListShrink(i);
    }
}

static void DropId(ttobj o) {
    if (o)
	_DropId(o->id >> TT_MAGIC_SHIFT, o);
}



static ttobject FindNative(ttany id) {
    ttopaque i, j;
    ttobject o;
    /* search only ttnative:s (?) */
#if 1
    i = order_ttnative;
#else
    for (i = order_ttobject; i < order_last; i++)
#endif
	for (j = 0; j < IdTop[i]; j++) {
	    if ((o = (ttobject)IdList[i][j]) && IS(ttobject,o) && o->native == id)
		return o;
	}

    return (ttobject)0;
}




/*
 * ttclass-related functions
 */

#include "class.h"







/*
 * wrappers for public and exported methods
 */

#include "seterrno.h"

#include "create.h"



/* constructors */
tt_obj TTNew(tt_obj Class) {
    tt_obj ret;
    LOCK;
    ret = (tt_obj) OBJ2ID(New(ID2CLASS(Class), NULL));
    UNLK;
    return ret;
}

/* destructors */
void TTDel(tt_obj o) {
    LOCK;
    Del(ID2(ttobject, o));
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

static void CallFunctionPlain(ttlistener_fn function, ttopaque args_n, ttany *args) {
    switch (args_n) {
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
      default:
	/* argh. this is bad */
	TTAssertAlways(args_n <= 10);
	break;
    }
}

static void CallListener(ttlistener c, tobj o_id, tobj ev_id) {
    static ttarg unused_return_value;
    ttany *args;
    ttopaque args_n;
    
    if (c->function && c->args) {

	args_n = c->args->array_n;
	args = c->args->array;
	
	/* if c->arg_component_n is valid, set corresponding arg to ttcomponent o */
	if (c->arg_component_n < args_n)
	    args[c->arg_component_n] = o_id;
	/* if c->arg_event_n is valid, set corresponding arg to ttevent ev */
	if (c->arg_event_n < args_n)
	    args[c->arg_event_n] = ev_id;
    
	if (c->lflags & ttlistener_lflags_ttmethod)
	
	    (void)TTCallY_ttmethod((tt_obj)(ttopaque)c->function, &unused_return_value, args_n, args);
    
	else if (c->lflags & ttlistener_lflags_function_array)
	    
	    ((ttlistener_array_fn)c->function) (args_n, args);
    
	else

	    CallFunctionPlain(c->function, args_n, args);
    }
}

static ttbyte IsBlockedEventBy(ttevent ev, tteventmask em) {
    ttuint truth_table = em->truth_table, evtype = ev->evtype, evcode = ev->evcode;
    tt_obj c = (tt_obj)OBJ2ID(ev->component);
    ttbyte e1true, e2true, ctrue;
    
    /* default value: do not block any evtype */
    e1true = TT_FALSE;
    if (em->evtype_mask)
	e1true = TTContains_ttbitmask((tt_obj)em->evtype_mask->id, (ttany)evtype);
    
    /* default value: block all evcodes */
    e2true = TT_FALSE;
    if (em->evcode_mask)
	e2true = TTContains_ttbitmask((tt_obj)em->evcode_mask->id, (ttany)evcode);
    
    /* default value: block all components */
    ctrue = TT_TRUE;
    if (em->component_mask)
	ctrue = TTContains_ttbitmask((tt_obj)em->component_mask->id, (ttany)(ttopaque)c);
    
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
    
    if ((ev = Create4s4_tteventbig(ttevent_evtype_change,
				   which | (order_ttfield<<TT_MAGIC_SHIFT),
				   0, value, old_value, len, NULL)))
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


TT_INLINE int CompareTF(ttany t1, ttany f1, ttany t2, ttany f2) {
    return t1 < t2 ? -1 :
    (
     t1 > t2 ? 1 :
     (
      f1 < f2 ? -1 : f1 > f2 ? 1 : 0
     )
    );
}

static int CompareTimers(TT_CONST tttimer t1, TT_CONST tttimer t2) {
    return CompareTF(t1->delay_t, t1->delay_f, t2->delay_t, t2->delay_f);
}

static void GetNow(void) {
    struct timeval now;

    gettimeofday(&now, NULL);
    TTD.TNow = (ttany)now.tv_sec;
    TTD.FNow = (ttany)now.tv_usec MicroSECs;
}

static void Activate_tttimer(tttimer o, ttbyte active) {
    tttimer t = TTD.FirstT;

    if (active && CompareTF(o->delay_t, o->delay_f, TTD.TNow, TTD.FNow) > 0) {
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

static ttbyte SetData_ttdata(ttdata o, ttany data) {
    ttcomponent c;
    if (TTAssert(o && IS(ttdata,o)) && !(o->oflags & ttobject_oflags_const)) {
	if (data != o->data) {
	    if ((c = o->component) && o->key_len == 9 && !TTCmpMem(o->key, "user_data", 9))
		c->user_data = data;
	    
	    FIRE_EVENT(o->data = data, o, ttdata_data, data, o->data);
	}
	return TT_TRUE;
    }
    return TT_FALSE;
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
	    return (ttdata)Build((ttobject)d);
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

static void HardDel(ttobj o) {
    if (IS(ttobject,o) && !(((ttobject)o)->oflags & ttobject_oflags_const)) {
	/* ignore refcount... */
	((ttobject)o)->refcount = 0;
	
	/*
	 * this is "the right thing" but may
	 * generate *LOTS* of delete events :(
	 */
	TDEL(o);
    } else {
	DropId(o);
    }
}

static void DelAll_ttobj(ttbyte quick) {
    ttopaque i, n, qn, quick_list[] = { order_ttcallback, order_ttlistener, order_tttimer };
    ttobj o;

    if (quick) {
	/* first, delete all callbacks, listeners and timers */
	for (qn = 2; qn != (ttopaque)-1; qn--) {
	    n = quick_list[qn];
	    for (;;) {
		/* IdTop[n] decreases with us. */
		if ((i = IdTop[n] - 1) != (ttopaque)-1) {
		    if (TTAssertAlways(o = IdList[n][i]))
			HardDel(o);
		} else
		    break;
	    }
	}
    }
    
    for (n = order_last - 1; n != (ttopaque)-1; n--) {
	for (;;) {
	    /* IdTop[n] decreases with us. */
	    if ((i = IdTop[n] - 1) != (ttopaque)-1) {
		if (TTAssertAlways(o = IdList[n][i]))
		    HardDel(o);
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
	    o->Class->BuiltinRepaint(o, x, y, w, h);
	
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
	t->tv_sec = (long)TTD.FirstT->delay_t - (long)TTD.TNow;
	/*
	 * doing
	 * ((long)TTD.FirstT->f - (long)TTD.FNow) / MicroSEC
	 * overflows since ttany is usually same size and signedness as 'unsigned long'
	 */
	t->tv_usec = (long)TTD.FirstT->delay_f / MicroSEC - (long)TTD.FNow / MicroSEC;
	NormalizeTimeval(t);
#ifdef DEBUG_TT_TIMER
	printf("now     {%ld, %ld}\n", (long)TTD.TNow, (long)TTD.FNow);
	printf("timer   {%ld, %ld}\n", (long)TTD.FirstT->delay_t, (long)TTD.FirstT->delay_f);
	printf("compute timer 0x%X\t{%ld, %ld}\n",
	       (unsigned)TTD.FirstT->id, (long)t->tv_sec, (long)t->tv_usec);
#endif
    }
}

static ttbyte MainLoop(ttbyte wait) {
    fd_set fset;
    struct timeval t;
    tttimer c;
    int fd = -1, sel_ret;
    s_tt_errno *E;
    ttbyte ret = TT_TRUE, eff_wait;
    
    if (TTD.FirstT)
	eff_wait = TT_FALSE;
    else
	eff_wait = wait;
    
    fd = THW.ConnectionFd();
    FD_ZERO(&fset);
    
    for (;;) {

#ifdef DEBUG_TT_TIMER
	printf("THW.MainLoopOnce(eff_wait = %d", (int)eff_wait);
	fflush(stdout);
#endif
	ret = THW.MainLoopOnce(eff_wait);
#ifdef DEBUG_TT_TIMER
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
		
#ifdef DEBUG_TT_TIMER
	    printf("select(%d, [%d], NULL, NULL, {%ld, %ld}",
		   fd+1, fd, (long)t.tv_sec, (long)t.tv_usec);
	    fflush(stdout);
#endif
	    UNLK;
	    sel_ret = select(fd+1, &fset, NULL, NULL, &t);
	    LOCK;
#ifdef DEBUG_TT_TIMER
	    printf(") = %d\n", sel_ret);
#endif

	    while ((c = TTD.FirstT)) {
		GetNow();
		ComputeDelay(&t);
		
		if (t.tv_sec == 0 && t.tv_usec < 1000) {
#ifdef DEBUG_TT_TIMER
		    printf("run     timer 0x%X\t{%ld, %ld}\n",
			   (unsigned)c->id, (long)t.tv_sec, (long)t.tv_usec);
#endif
		    c->refcount++; /* protect c from being deleted */

		    /*
		     * timers are one-shot. to obtain periodic timers,
		     * they should reactivate themselves with c->Class->SetDelay()
		     */
		    Activate_tttimer(c, TT_FALSE);
		    
		    CallListener((ttlistener)c, c->component ? c->component->id : TT_NOID, c->id);
		    
		    if (!--c->refcount)
			TDEL(c);
		} else {
#ifdef DEBUG_TT_TIMER
		    printf("wait    timer 0x%X\t{%ld, %ld}\n",
			   (unsigned)c->id, (long)t.tv_sec, (long)t.tv_usec);
#endif
		    break;
		}
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
    (ttany)0, (ttany)0,

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
	    FixedAssignId,
	    FixedAssignIdArray,
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
    

    /* Class_hw_null */
    NULL,

    /* Classes */
    {
	/* order_ttobj == 0 */
#define el(t) (ttclass)TClass(t),
	 TT_LIST(el)
#undef el
    },
    
	
    /* Class */
    /* { { 0, }, }, */
    
    /* Class_default */
    /* { { 0, }, }, */
};


/* TTClass_* globals */
#define el(t) tt_obj TT_CAT(TTClass_,t) = (tt_obj)id_ttclass_(t);
TT_LIST(el)
TT_TYPELIST(el)
#undef el

