/*
 *  libTT.c  --  'The Text Toolkit' library implementation
 *
 *  Copyright (C) 2001 by Massimiliano Ghilardi
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

#include <stdarg.h>

#ifdef CONF_SOCKET_PTHREADS
# include <pthread.h>
#endif

#include "Tw/Tw.h"
#include "Tw/Twavl.h"
#include "Tw/Twerrno.h"

#include "TT/TT.h"
#include "TT/TTerrno.h"

#include "Tutf/Tutf.h"
#include "Tutf/Tutf_defs.h"




#include "TTtypes.h"
#include "TTm4.h"
#include "TTassert.h"

#include "mutex.h"
#include "version.h"


/* typedef s_tt_errno */

#ifdef CONF_SOCKET_PTHREADS

typedef struct {
    uldat E;
    uldat S;
    tw_self T;
} s_tt_errno;

typedef struct {
    s_tt_errno *vec;
    uldat max, last;
} s_tt_errno_vec;

#else

typedef tw_errno s_tt_errno;
typedef tw_errno s_tt_errno_vec;

#endif


typedef struct s_tt_d {
#ifdef CONF_SOCKET_PTHREADS
    tw_mutex mutex;
#endif
    tttheme DefaultTheme;
    ttmenu DefaultMenubar;
    ttapplication Application;
    tavl AVLttobjs;
    
    s_tt_errno_vec rErrno_;
    
    byte OpenFlag;
    byte PanicFlag;
} tt_d;


#include "dummytheme.h"

static s_tt_errno rCommonErrno;
#define CommonErrno (rCommonErrno.E)
#define CommonErrnoDetail (rCommonErrno.S)

static tt_d TTD
#ifdef CONF_SOCKET_PTHREADS
 = { TW_MUTEX_INITIALIZER, }
#endif
;

#define rErrno	(TTD.rErrno_)
#define mutex	(TTD.mutex)

#define LOCK tw_mutex_lock(mutex)
#define UNLK tw_mutex_unlock(mutex)


#ifdef CONF_SOCKET_PTHREADS

TT_INLINE byte GrowErrnoLocation(void) {
    s_tt_errno *vec;
    uldat newmax = rErrno.max <= 8 ? 16 : (rErrno.max<<1);
    
    if ((vec = (s_tt_errno *)TTReAllocMem(rErrno.vec, newmax * sizeof(s_tt_errno)))) {

	/* assume (tw_self)-1 is _NOT_ a valid thread identifier */
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
    tw_self self;
    uldat i;

    do if (TTD.OpenFlag) {
	self = tw_self_get();
	
	/* we cache last thread that called GetErrnoLocation() */
	i = rErrno.last;
	if (i < rErrno.max && rErrno.vec[i].T == self) {
	    r = &rErrno.vec[i];
	    break;
	}
	
	for (i=0; i<rErrno.max; i++) {
	    if (rErrno.vec[i].T == self)
		break;
	    if (rErrno.vec[i].T == (tw_self)-1) {
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

# define GetErrnoLocation()	(&TTD.rErrno_)
# define FreeErrnoLocation()	do { } while (0)

#endif /* CONF_SOCKET_PTHREADS */


#define Errno (GetErrnoLocation()->E)
# define ErrnoDetail (GetErrnoLocation()->S)



void TTAssertFail (char *assertion, char *file, unsigned int line, char *function) {
    fprintf(stderr, "TT-WARNING: %s:%u: %s: assertion `%s' failed\n",
	    file, line, function, assertion);
}


byte TTInPanic(void) {
    return TTD.PanicFlag;
}

uldat TTLibraryVersion(void) {
    return TT_PROTOCOL_VERSION;
}

TT_DECL_MAGIC(TTMagicData);

byte TTCheckMagic(TT_CONST byte id[])  {
    if (TTCmpMem(id+1, TTMagicData+1, (id[0] < TTMagicData[0] ? id[0] : TTMagicData[0]) - 2 - sizeof(uldat))) {
	CommonErrno = TT_EXLIB_SIZES;
	return FALSE;
    }
    return TRUE;
}

TW_DECL_MAGIC(TT_Tw_magic);

byte TTOpen(byte TT_CONST * target, ...) {
    byte TT_CONST * TTDisplay;

    LOCK;
    if (TTD.OpenFlag) {
	CommonErrno = TT_EALREADY_CONN;
	UNLK;
	return FALSE;
    }
    
    if (target != NULL) {
	va_list ap;
	va_start(ap, target);
	TTDisplay = va_arg(ap, byte TT_CONST *);
	va_end(ap);
    } else
	TTDisplay = target;

    if (target) {
	if (!TTCmpStr(target, "tw") || !TTCmpStr(target, "Tw") || !TTCmpStr(target, "twin") || !TTCmpStr(target, "Twin"))
	    /* ok, target is libTw */
	    ;
	else {
	    CommonErrno = TT_EUNKNOWN_TARGET;
	    UNLK;
	    return FALSE;
	}
    }
    
    if (TwCheckMagic(TT_Tw_magic) && TwOpen(TTDisplay)) {
	CommonErrno = 0;

	TTD.DefaultTheme = &s_DummyTheme;
	TTD.DefaultMenubar = NULL;
	TTD.Application = NULL;
	TTD.AVLttobjs = NULL;
# ifdef CONF_SOCKET_PTHREADS
	TTD.rErrno_.max = TTD.rErrno_.last = 0;
# else
	TTD.rErrno_.E = TTD.rErrno_.S = 0;
# endif
	TTD.OpenFlag = TRUE;
	TTD.PanicFlag = FALSE;
	
	UNLK;
	return TRUE;
    }
	
    CommonErrno = TT_MAX_ERROR + TwErrno;
    CommonErrnoDetail = TwErrnoDetail;
    UNLK;
    return FALSE;
}

static void DelAll_ttobj(tavl base);

void TTClose(void) {
    LOCK;
    if (TTD.OpenFlag) {
	/* save Errno in CommonErrno */
	s_tt_errno *E = GetErrnoLocation();
    
	rCommonErrno.E = E->E;
	rCommonErrno.S = E->S;
	
	/* close libTw connection. also deletes all libTw objects */
	TwClose();
	
	/* delete all libTT objects */
	DelAll_ttobj(TTD.AVLttobjs);
	
	TTD.PanicFlag = TTD.OpenFlag = FALSE;
	FreeErrnoLocation();
    }
    UNLK;
}


tt_errno *TTErrnoLocation(void) {
    s_tt_errno *t;
    LOCK; t = GetErrnoLocation(); UNLK;
    return (tt_errno *)t;
}

TT_FNATTR_CONST TT_CONST byte *TTStrError(uldat E) {
    switch (E) {
      case 0:
	return "success";
      case TT_EXLIB_SIZES:
	return "compiled data sizes are incompatible with libTT now in use!";
      case TT_EALREADY_CONN:
	return "already connected";
      case TT_EUNKNOWN_TARGET:
	return "unknown display target. Known targets are: \"Tw\".";
      default:
	if (E > TT_MAX_ERROR)
	    return TwStrError(E - TT_MAX_ERROR);
	return "unknown error";
    }
}

TT_FNATTR_CONST TT_CONST byte *TTStrErrorDetail(uldat E, uldat S) {
    switch (E) {
      default:
	if (E > TT_MAX_ERROR)
	    return TwStrErrorDetail(E - TT_MAX_ERROR, S);
    }
    return "";
}

int TTConnectionFd(void) {
    int fd;
    LOCK; fd = TTD.OpenFlag ? TwConnectionFd() : TW_NOFD; UNLK;
    return fd;
}

byte TTFlush(void) {
    if (!TwFlush())
	TTD.PanicFlag = TRUE;
    return !TTD.PanicFlag;
}

byte TTTimidFlush(void) {
    byte ret = TwTimidFlush();
    if (!ret)
	TTD.PanicFlag = TRUE;
    return ret;
}

byte TTSync(void) {
    if (!TwSync())
	TTD.PanicFlag = TRUE;
    return !TTD.PanicFlag;
}

byte TTMainLoop(void) {
    s_tt_errno *E;
    if (!TwMainLoop()) {
	LOCK;
	E = GetErrnoLocation();
	E->E = TwErrno + TT_MAX_ERROR;
	E->S = TwErrnoDetail;
	UNLK;
	return FALSE;
    }
    return TRUE;
}





/*
 * wrappers for public and exported methods
 */

byte TTInstanceOf(ttfn_ttobj FN, ttobj o) {
    return FN && o && (FN->magicbit & o->FN->magic) != 0;
}

ttfn_ttobj TTClassOf(ttobj o) {
    return o ? o->FN : (void *)o;
}





# if TW_SIZEOFHWFONT == 1
# define CloneStr2HWFont TTCloneMem
#else
hwfont *CloneStr2HWFont(byte TT_CONST * s, size_t len) {
    hwfont *temp, *save;
    
    if (s) {
	if ((temp = save = (hwfont *)TTAllocMem((len+1) * sizeof(hwfont)))) {
	    while (len--) {
# ifdef CONF__UNICODE
		*temp++ = Tutf_IBM437_to_UTF_16[*s++];
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


#define DELTA_tavl ((size_t)&((ttobj)0)->AVLttobjs)
static int TTobjCompare(tavl_c t1, tavl_c t2) {
    ttobj_c o1 = (ttobj_c)((byte TT_CONST *)t1 - DELTA_tavl), o2 = (ttobj_c)((byte TT_CONST *)t1 - DELTA_tavl);
    return o1->id - o2->id ? o1->id - o2->id : (int)(o1 - o2);
}

#define TTAVLInsert_ttobj(o) do { \
    (o)->AVLttobjs.AVLkey = (o)->id; \
    AVLInsert(&(o)->AVLttobjs, TTD.AVLttobjs, TTobjCompare, &TTD.AVLttobjs); \
} while (0)

#define TTAVLRemove_ttobj(o) \
	AVLRemove(&(o)->AVLttobjs, TTobjCompare, &TTD.AVLttobjs)

#define TTAVLFind_ttobj(o) \
	AVLFind(&(o)->AVLttobjs, TTD.AVLttobjs, TTobjCompare)


#define tavl_2_ttobj(a) ((ttobj)((byte *)a - DELTA_tavl))
#define ttobj_2_tavl(o) (&(o)->AVLttobjs)




/* ttlistener */
TT_INLINE ttlistener AddListener_ttobj(ttobj o) {
    ttlistener l;
    if ((l = (ttlistener)TTAllocMem(sizeof (struct s_ttlistener)))) {
	if ((l->next = o->listeners))
	    l->next->prev = l;
	l->prev = NULL;
	l->t = NULL;
	o->listeners = l;
    }
    return l;
}

static void DelListener_ttobj(ttobj o, ttlistener l) {
    if (l->t)
	TwDeleteListener(l->t);
    if (l->next)
	l->next->prev = l->prev;
    if (l->prev)
	l->prev->next = l->next;
    else
	o->listeners = l->next;
    TTFreeMem(l);
}

TT_INLINE void DelAllListeners_ttobj(ttobj o) {
    ttlistener l, next = o->listeners;
    while ((l = next)) {
	next = l->next;
	if (l->t)
	    TwDeleteListener(l->t);
	TTFreeMem(l);
    }
    o->listeners = NULL;
}

TT_INLINE void DelAllListenersQuickNDirty_ttobj(ttobj o) {
    ttlistener l, next = o->listeners;
    while ((l = next)) {
	next = l->next;
	/* don't TwDeleteListener(l->t), we already did TwClose() */
	TTFreeMem(l);
    }
    o->listeners = NULL;
}


#define o tavl_2_ttobj(base)

static void DelAllIds_ttobj(tavl base) {
    if (base) {
	o->id = TW_NOID;
	DelAllIds_ttobj(base->AVLLeft);
	DelAllIds_ttobj(base->AVLRight);
    }
}

static void real_DelAll_ttobj(tavl base) {
    if (base) {
	DelAllListenersQuickNDirty_ttobj(o);
	
	real_DelAll_ttobj(base->AVLLeft);
	real_DelAll_ttobj(base->AVLRight);
	
	o->FN->Del(o);
    }
}

#undef o

/*
 * delete all libTT objects. no need to keep the AVLttobjs tree balanced,
 * and no need to TwDeleteObj() or TwDeleteListener() as we already did TwClose()
 */
static void DelAll_ttobj(tavl base) {
    DelAllIds_ttobj(base);
    real_DelAll_ttobj(base);
}





/* ttobj */
static ttobj Realize(ttobj o) {
    if (o->FN->Realize(o)) {
	TTAVLInsert_ttobj(o);
	return o;
    }
    if (o)
	o->FN->Del(o);
    return (ttobj)0;
}


ttobj TTNew(ttfn_ttobj FN) {
    ttobj o;
    if ((o = FN->New(FN)))
	return Realize(o);
    return o;
}
void TTDel(ttobj o) {
    if (o && TTAssert(IS(ttobj,o))) {
	TTAVLRemove_ttobj(o);
	o->FN->Del(o);
    }
}
void TTSetUserData(ttobj o, void *data) {
    if (o && TTAssert(IS(ttobj,o)))
	o->userdata = data;
}
void *TTGetUserData(ttobj o) {
    if (o && TTAssert(IS(ttobj,o)))
	return o->userdata;
    return NULL;
}



/* ttvisible */
void TTAdd_ttvisible(ttvisible o, ttvisible child) {
    if (o && child && TTAssert(IS(ttvisible,o) && IS(ttvisible,child)))
	o->FN->Add(o, child);
}
void TTAddTo_ttvisible(ttvisible o, ttvisible parent) {
    if (o && parent && TTAssert(IS(ttvisible,o) && IS(ttvisible,parent)))
	o->FN->AddTo(o, parent);
}
void TTRemove_ttvisible(ttvisible o) {
    if (o && TTAssert(IS(ttvisible,o)))
	o->FN->Remove(o);
}
void TTSetVisible_ttvisible(ttvisible o, byte on_off) {
    if (o && TTAssert(IS(ttvisible,o)))
	o->FN->SetVisible(o, on_off);
}   
void TTSetTheme_ttvisible(ttvisible o, tttheme theme) {
    if (o && TTAssert(IS(ttvisible,o)) && (!theme || TTAssert(IS(tttheme,theme))))
	o->FN->SetTheme(o, theme);
}
void TTRepaint_ttvisible(ttvisible o, dat width, dat height, dat x, dat y) {
    if (o && TTAssert(IS(ttvisible,o)))
	o->FN->Repaint(o, width, height, x, y);
}
ttvisible TTGetPrev_ttvisible(ttvisible o) {
    if (o && TTAssert(IS(ttvisible,o)))
	return o->prev;
    return NULL;
}   
ttvisible TTGetNext_ttvisible(ttvisible o) {
    if (o && TTAssert(IS(ttvisible,o)))
	return o->next;
    return NULL;
}
ttvisible TTGetParent_ttvisible(ttvisible o) {
    if (o && TTAssert(IS(ttvisible,o)))
	return o->parent;
    return NULL;
}
ttvisible TTGetFirstChild_ttvisible(ttvisible o) {
    if (o && TTAssert(IS(ttvisible,o)))
	return o->firstchild;
    return NULL;
}
ttvisible TTGetLastChild_ttvisible(ttvisible o) {
    if (o && TTAssert(IS(ttvisible,o)))
	return o->lastchild;
    return NULL;
}
tttheme TTGetTheme_ttvisible(ttvisible o) {
    if (o && TTAssert(IS(ttvisible,o)))
	return o->theme;
    return NULL;
}


/* ttwidget */



/* ttnative */
ttnative TTCreate_ttnative(uldat id) {
    struct s_ttobj s; tavl a; ttnative o;

    s.id = s.AVLttobjs.AVLkey = id;
    
    if ((a = TTAVLFind_ttobj(&s)))
	return (ttnative)tavl_2_ttobj(a);
    
    if ((o = TTFN_ttnative->New(TTFN_ttnative))) {
	o->id = id;
	return (ttnative)Realize((ttobj)o);
    }
    return NULL;
}
ttnative TTGetFirstScreen_ttnative(void) {
    return (ttnative)TTCreate_ttnative(TwFirstScreen());
}


/* ttlabel */
byte TTSetText_ttlabel(ttlabel o, byte TT_CONST * text) {
    if (o && TTAssert(IS(ttlabel,o)))
	return o->FN->SetText(o, text);
    return FALSE;
}
hwfont TT_CONST *TTGetText_ttlabel(ttlabel o) {
    if (o && TTAssert(IS(ttlabel,o)))
	return o->text;
    return NULL;
}
ttlabel TTCreate_ttlabel(byte TT_CONST * text) {
    ttlabel o;
    if ((o = TTFN_ttlabel->New(TTFN_ttlabel))) {
	o->textlen = text ? strlen(text) : 0;
	if (!text || (o->text = CloneStr2HWFont(text, o->textlen)))
	    return (ttlabel)Realize((ttobj)o);
	TT_DEL(o);
    }
    return NULL;
}

/* ttanybutton */
byte TTSetText_ttanybutton(ttanybutton o, hwfont TT_CONST * text, dat width, dat height, dat pitch) {
    if (o && TTAssert(IS(ttanybutton,o)))
	return o->FN->SetText(o, text, width, height, pitch);
    return FALSE;
}
dat TTGetTextWidth_ttanybutton(ttanybutton o) {
    if (o && TTAssert(IS(ttanybutton,o)))
	return o->textwidth;
    return 0;
}
dat TTGetTextHeight_ttanybutton(ttanybutton o) {
    if (o && TTAssert(IS(ttanybutton,o)))
	return o->textheight;
    return 0;
}
hwfont TT_CONST *TTGetText_ttanybutton(ttanybutton o) {
    if (o && TTAssert(IS(ttanybutton,o)))
	return o->text;
    return NULL;
}
ttanybutton TTCreate_ttanybutton(hwfont TT_CONST * text, dat width, dat height, dat pitch) {
    ttanybutton o;
    hwfont *_text;
    if ((o = TTFN_ttanybutton->New(TTFN_ttanybutton))) {
	if (!text || (o->text = _text = TTAllocMem((ldat)width * height * sizeof(hwfont)))) {
	    o->textwidth = width;
	    o->textheight = height;
	    while (height--) {
		TTCopyMem(text, _text, width * sizeof(hwfont));
		text += pitch;
		_text += width;
	    }
	    return (ttanybutton)Realize((ttobj)o);
	}
	TT_DEL(o);
    }
    return NULL;
}

/* ttbutton */
ttbutton TTCreate_ttbutton(hwfont TT_CONST * text, dat width, dat height, dat pitch) {
    ttbutton o;
    hwfont *_text;
    if ((o = TTFN_ttbutton->New(TTFN_ttbutton))) {
	if (!text || (o->text = _text = TTAllocMem((ldat)width * height * sizeof(hwfont)))) {
	    o->textwidth = width;
	    o->textheight = height;
	    while (height--) {
		TTCopyMem(text, _text, width * sizeof(hwfont));
		text += pitch;
		_text += width;
	    }
	    return (ttbutton)Realize((ttobj)o);
	}
	TT_DEL(o);
    }
    return NULL;
}


/* ttcheckbutton */


/* ttbuttongroup */


/* ttwindow */


/* ttframe */


/* ttscrollbar */


/* ttslider */


/* ttscroller */


/* ttmenuitem */


/* ttcheckmenuitem */


/* ttradiomenuitem */


/* ttmenuwindow */


/* ttmenu */
ttmenuwindow TTGetMenuWindow_ttmenu(ttmenu o) {
    if (o && TTAssert(IS(ttmenu,o)))
	return o->menuwindow;
    return NULL;
}


/* ttmenubar */


/* ttanytext */


/* tttextfield */


/* tttextarea */


/* tttheme */


/* ttapplication */
ttapplication TTCreate_ttapplication(byte TT_CONST * name) {
    if (!TTD.Application && (TTD.Application = TTFN_ttapplication->New(TTFN_ttapplication))) {
	if ((TTD.Application->name = TTCloneStr(name)))
	    return (ttapplication)Realize((ttobj)TTD.Application);
	TT_DEL(TTD.Application);
	TTD.Application = NULL;
    }
    return NULL;
}


#include "methodsm4.h"


