#include "twin.h"
#include "data.h"
#include "methods.h"
#include "printk.h"
#include "ext_query.h"

#ifdef CONF__MODULES
# include "dl.h"
#endif
#ifdef CONF_EXT_TT
# include "ext_TT.h"
#endif


static void warn_NoExtension(topaque len, CONST byte *name, uldat tried) {
#ifdef CONF__MODULES
    if (!tried)
	printk("twin: no extension compiled into twin, and all extension modules failed\n"
	       "      for extension `%.*s'\n", Min2((int)len,SMALLBUFF), name);
    else
#endif
    {
	printk("twin: all extensions failed");
	if (name)
	    printk("for `%.*s'\n", Min2((int)len,SMALLBUFF), name);
	else
	    printk(".\n");
    }
}

#if defined(CONF_EXT_TT) /* || defined(...) */
static byte check4(byte *s, uldat len, CONST byte *name) {
    if (len && strncmp(s, name, len))
	return FALSE;
#if 0
    printk("twin: trying given `%.*s' server extension.\n", Min2((int)len,SMALLBUFF), s);
#endif
    return TRUE;
}
#endif


static extension LoadExtension(topaque namelen, CONST byte *name) {
    extension E = (extension)0;
    uldat tried = 0;
    topaque fullnamelen;
    byte *fullname, success;
    
    if (!namelen || memchr(name, '/', namelen) || !(fullname = AllocMem(namelen + 16)))
	return E;
	
    sprintf(fullname, "extensions/ext_%.*s", (int)namelen, name);
    fullnamelen = 15 + namelen;
    
    if ((E = FnExtension->Create(FnExtension, fullnamelen, fullname))) {

	FnExtension->Fn_Module->Used++;
	
	E->CallB = NULL;
	E->Quit = NULL;
	
#define TRY4(e) (check4(STR(e), namelen, name) && (tried++, E->Quit = CAT3(ext_,e,_Quit), CAT3(ext_,e,_Init)(E)))
	success =
#ifdef CONF_EXT_TT
	    TRY4(TT) ||
#endif
#ifdef CONF__MODULES
	    (E->Quit = NULL, Act(DlOpen,E)(E)) ||
#endif
	    (warn_NoExtension(namelen, name, tried), FALSE);

#undef TRY4

	if (!success) {
	    Delete(E);
	    E = (extension)0;
	}
    }
    return E;
}


extension QueryExtension(byte len, CONST byte *name) {
    module M;
    topaque namelen = len;
    
    for (M = All->FirstModule; M; M = M->Next) {
	if (IS_EXTENSION(M) && M->NameLen == namelen + 15 && /* "extensions/ext_" */
	    !CmpMem(M->Name, "extensions/ext_", 15) &&
	    !CmpMem(M->Name+15, name, namelen)) {
	    
	    return (extension)M;
	}
    }
    return LoadExtension(namelen, name);
}

