/*
 *  lsobj.c  --  print characteristic of an existing twin object
 *
 *  This program is placed in the public domain.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <Tw/Tw.h>
#include <Tw/Twerrno.h>
#include <Tw/Twstat.h>
#include <Tw/Twstat_defs.h>

#include "version.h"

byte *argv0;

enum {
#define EL(field) TWS_CAT(lsobj_,field),
    TWS_field_list_EL(EL)
#undef EL
    lsobj_max_list0,
    dummy = lsobj_max_list0 - 1,
#define EL(field) TWS_CAT(lsobj_,field),
    TWS_field_list_List_EL(EL)
#undef EL
    lsobj_max_list1
};

udat field_list[] = {
#define EL(field) TWS_CAT(TWS_,field),
    TWS_field_list_EL(EL)
    TWS_field_list_List_EL(EL)
#undef EL
};


/* these are hardcoded in libTw ABI, so we can hardcode them here too. */
byte *type_list[TWS_highest+2] = {
    "void", "byte", "dat", "ldat", "hwcol", "time_t", "frac_t", "hwfont", "hwattr", "tobj", "unknown"
};


byte *name_list[] = {
#define EL(field) TWS_STR(field),
    TWS_field_list_EL(EL)
    TWS_field_list_List_EL(EL)
#undef EL
};

    
void Usage(void) {
    fprintf(stderr, "Usage: %s [option] [object_id]\n"
	    "If no arguments are given, the first screen is used as default object_id\n"
	    "Currently known options:\n"
	    " -h, --help              display this help and exit\n"
	    " -V, --version           output version information and exit\n"
	    " -r, --recursive         also show lists of parents, children, ...\n"
	    " -v, --verbose           always show all data, even huge arrays\n",
	    argv0);
}

void ShowVersion(void) {
    fputs("twlsobj " TWIN_VERSION_STR "\n", stdout);
}

static void human_print(TW_CONST byte *data, uldat len, byte may_trim) {
    putchar(' '); putchar('"');
    if (may_trim && len > 100)
	len = 100;
    else
	may_trim = FALSE;
    while (len--) {
	switch (*data) {
	  case '\0':
	    putchar('\\');
	    putchar('0');
	    break;
	  case '\"': case '\'': case '\\':
	    putchar('\\');
	    putchar(*data);
	    break;
	  default:
	    if (*data < ' ')
		printf("\\x%02x", (int)*data);
	    else
		putchar(*data);
	    break;
	}
	data++;
    }
    printf("\"%s\n", may_trim ? "..." : "");
}

static void human_print_tobjs(TW_CONST byte *data, uldat len, byte may_trim) {
    putchar(' '); putchar('{');
    if (may_trim && len > 100)
	len = 100;
    else
	may_trim = FALSE;
    while (len >= sizeof(tobj)) {
	len -= sizeof(tobj);
	printf("0x%lx%s", (long)*(TW_CONST tobj *)data, len >= sizeof(tobj) ? ", " : "");
	data += sizeof(tobj);
    }
    printf("}%s\n", may_trim ? "..." : "");
}

TW_DECL_MAGIC(lsobj_magic);

int main(int argc, char *argv[]) {
    uldat err = 1, id, index_list, len;
    tslist TSL;
    tsfield TSF;
    uldat n, v_id;
    udat type_id, max_list;
    byte verbose = FALSE, recursive = FALSE, isvec;
    
    TwMergeHyphensArgv(argc, argv);
    
    argv0 = argv[0];
 
    argc--;
    argv++;
    
    while (argc--) {
	if (!strcmp(*argv, "-h") || !strcmp(*argv, "-help")) {
	    Usage();
	    return 0;
	} else if (!strcmp(*argv, "-V") || !strcmp(*argv, "-version")) {
	    ShowVersion();
	    return 0;
	} else if (!strcmp(*argv, "-r") || !strcmp(*argv, "-recursive")) {
	    recursive = TRUE;
	} else if (!strcmp(*argv, "-v") || !strcmp(*argv, "-verbose")) {
	    verbose = TRUE;
	} else if ((id = strtoul(*argv, NULL, 0)) != TW_NOID) {
	    err = 0;
	}
	argv++;
    }

    max_list = recursive ? lsobj_max_list1 : lsobj_max_list0;
    
    if (TwCheckMagic(lsobj_magic) && TwOpen(NULL)) do {

	if (err == 1) {
	    id = TwGetAll();
	    if (TwErrno == TW_ESERVER_NO_FUNCTION) {
		TwErrno = 0;
		id = TwFirstScreen();
	    }
	}
	TSL = TwStatA(id, max_list, field_list);
	
	if (TSL) {
	    for (id = 0, TSF = TSL->TSF; id < TSL->N; id++, TSF++) {
		for (index_list = 0; index_list < max_list; index_list++) {
		    if (field_list[index_list] == TSF->hash)
			break;
		}
		if (index_list < max_list) {
		    printf("%s", name_list[index_list]);
		    len = strlen(name_list[index_list]);
		} else {
		    len = 13;
		    printf("unknown_field");
		}
		while (len < 32)
		    len++, putchar(' ');
		    
		isvec = TSF->type >= TWS_vec && (TSF->type & ~TWS_vec) <= TWS_last;
		if (isvec)
		    type_id = TSF->type & ~TWS_vec;
		else
		    type_id = TSF->type;
		
		if (type_id == TWS_tobj)
		    type_id = TWS_highest;
		else if (type_id > TWS_tobj)
		    /* unknown */
		    type_id = TWS_highest + 1;
		
		if (isvec) {
		    len += 4 + strlen(type_list[type_id]);
		    v_id = TSF->type & ~TWS_vec;
		    if (v_id == TWS_tobj)
			v_id = TWS_uldat;
		    if (v_id < TWS_highest) {
			n = v_id = TSF->TWS_field_vecL / lsobj_magic[v_id];
			do
			    len++;
			while (v_id /= 10);
			printf("= (%s[%d])", type_list[type_id], (int)n);
		    } else
			printf("= (%s[])", type_list[type_id]);
		} else {
		    len += 2 + strlen(type_list[type_id]);
		    printf("= (%s)", type_list[type_id]);
		}
		while (len < 44)
		    len++, putchar(' ');
		
		if (isvec) {
		    if (TSF->TWS_field_vecL && TSF->TWS_field_vecV) {
			if (type_id == TWS_highest)
			    human_print_tobjs(TSF->TWS_field_vecV, TSF->TWS_field_vecL, !verbose);
			else
			    human_print(TSF->TWS_field_vecV, TSF->TWS_field_vecL, !verbose);
		    } else
			printf(" (NULL)\n");
		} else {
		    err = TSF->TWS_field_scalar;
		    printf("% -11ld # 0x%lx\n", (long)err, (long)err);
		}
	    }
	    TwDeleteStat(TSL);
	}
    } while (0);
    
    if ((err = TwErrno)) {
	fprintf(stderr, "%s: libTw error: %s%s\n", argv0,
		TwStrError(err), TwStrErrorDetail(err, TwErrnoDetail));
	return 1;
    }
    return 0;
}

