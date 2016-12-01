#include "twin.h"

#ifdef CONF_EXT

#include "data.h"
#include "dl.h"
#include "methods.h"
#include "printk.h"
#include "ext_query.h"


static void warn_NoExtension(topaque len, CONST byte *name, uldat tried) {
    if (!tried)
	printk("twin: no extension compiled into twin, and all extension modules failed\n"
	       "      for extension `%.*s'\n", Min2((int)len,TW_SMALLBUFF), name);
    else
    {
	printk("twin: all extensions failed");
	if (name)
	    printk("for `%.*s'\n", Min2((int)len,TW_SMALLBUFF), name);
	else
	    printk(".\n");
    }
}

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
	    (E->Quit = NULL, Act(DlOpen,E)(E)) ||
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

#endif /* CONF_EXT */
