/*
 *  lsobj.c  --  print characteristic of an existing twin object
 *
 *  This program is placed in the public domain.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Tw/Tw.h"
#include "Tw/Twstat.h"
#include "Tw/Twerrno.h"
#include "version.h"

byte *argv0;

udat field_list[] = {
	TWS_obj_Id, TWS_obj_Prev, TWS_obj_Next, TWS_obj_Parent,

	TWS_widget_FirstW, TWS_widget_LastW, TWS_widget_SelectW, TWS_widget_Left, TWS_widget_Up,
	TWS_widget_Width, TWS_widget_Height, TWS_widget_Attrib, TWS_widget_Flags, TWS_widget_XLogic, TWS_widget_YLogic,
	TWS_widget_O_Prev, TWS_widget_O_Next, TWS_widget_Owner, TWS_widget_USE_Fill,
	
	TWS_gadget_ColText, TWS_gadget_ColSelect, TWS_gadget_ColDisabled, TWS_gadget_ColSelectDisabled,
	TWS_gadget_Code, TWS_gadget_G_Prev, TWS_gadget_G_Next, TWS_gadget_Group,
	TWS_gadget_USE_TextV0, TWS_gadget_USE_TextV1, TWS_gadget_USE_TextV2, TWS_gadget_USE_TextV3,
	TWS_gadget_USE_ColorV0, TWS_gadget_USE_ColorV1, TWS_gadget_USE_ColorV2, TWS_gadget_USE_ColorV3,

	TWS_window_Menu, TWS_window_NameLen, TWS_window_Name, TWS_window_ColName,
	TWS_window_BorderPatternV0, TWS_window_BorderPatternV1,
	TWS_window_CurX, TWS_window_CurY, TWS_window_XstSel, TWS_window_YstSel, TWS_window_XendSel, TWS_window_YendSel,
	TWS_window_ColGadgets, TWS_window_ColArrows, TWS_window_ColBars, TWS_window_ColTabs, TWS_window_ColBorder,
	TWS_window_ColText, TWS_window_ColSelect, TWS_window_ColDisabled, TWS_window_ColSelectDisabled,
	TWS_window_State, TWS_window_CursorType,
        TWS_window_MinXWidth, TWS_window_MinYWidth, TWS_window_MaxXWidth, TWS_window_MaxYWidth,
        TWS_window_WLogic, TWS_window_HLogic, TWS_window_USE_Contents, TWS_window_USE_HSplit,

	TWS_screen_NameLen, TWS_screen_Name, TWS_screen_State,
	TWS_screen_USE_BgWidth, TWS_screen_USE_BgHeight, TWS_screen_USE_Bg,

	TWS_group_FirstG, TWS_group_LastG, TWS_group_SelectG,
	
	TWS_menuitem_Window, TWS_menuitem_Left, TWS_menuitem_ShortCut,
	
	TWS_menu_ColItem, TWS_menu_ColSelect, TWS_menu_ColDisabled, TWS_menu_ColSelectDisabled,
	TWS_menu_ColShtCut, TWS_menu_ColSelShtCut, TWS_menu_CommonItems, TWS_menu_FlagDefColInfo,
	TWS_menu_FirstI, TWS_menu_LastI, TWS_menu_SelectI,

	TWS_msgport_WakeUp, TWS_msgport_NameLen, TWS_msgport_Name,
	TWS_msgport_FirstMenu, TWS_msgport_LastMenu, TWS_msgport_FirstW, TWS_msgport_LastW,
	TWS_msgport_FirstGroup, TWS_msgport_LastGroup, TWS_msgport_FirstMutex, TWS_msgport_LastMutex,

	TWS_mutex_O_Prev, TWS_mutex_O_Next, TWS_mutex_Owner, TWS_mutex_Perm, TWS_mutex_NameLen, TWS_mutex_Name,
	
	TWS_row_Code, TWS_row_Flags, TWS_row_Len, TWS_row_Text, TWS_row_ColText,
};

uldat max_list = sizeof(field_list)/sizeof(field_list[0]);

/* these are hardcoded in libTw ABI, so we can hardcode them here too. */
byte *type_list[TWS_highest+2] = {
    "void", "byte", "dat", "ldat", "hwcol", "time_t", "frac_t", "hwfont", "hwattr", "obj_id", "unknown"
};


byte *name_list[] = {
	"obj_Id", "obj_Prev", "obj_Next", "obj_Parent",

	"widget_FirstW", "widget_LastW", "widget_SelectW", "widget_Left", "widget_Up",
	"widget_Width", "widget_Height", "widget_AttribW", "widget_FlagsW", "widget_XLogic", "widget_YLogic",
	"widget_O_Prev", "widget_O_Next", "widget_Owner", "widget_USE_Fill",
	
	"gadget_ColText", "gadget_ColSelect", "gadget_ColDisabled", "gadget_ColSelectDisabled",
	"gadget_Code", "gadget_G_Prev", "gadget_G_Next", "gadget_Group",
	"gadget_USE_TextV0", "gadget_USE_TextV1", "gadget_USE_TextV2", "gadget_USE_TextV3",
	"gadget_USE_ColorV0", "gadget_USE_ColorV1", "gadget_USE_ColorV2", "gadget_USE_ColorV3",

	"window_Menu", "window_NameLen", "window_Name", "window_ColName",
	"window_BorderPatternV0", "window_BorderPatternV1",
	"window_CurX", "window_CurY", "window_XstSel", "window_YstSel", "window_XendSel", "window_YendSel",
	"window_ColGadgets", "window_ColArrows", "window_ColBars", "window_ColTabs", "window_ColBorder",
	"window_ColText", "window_ColSelect", "window_ColDisabled", "window_ColSelectDisabled",
	"window_State", "window_CursorType",
        "window_MinXWidth", "window_MinYWidth", "window_MaxXWidth", "window_MaxYWidth",
        "window_WLogic", "window_HLogic", "window_USE_Contents", "window_USE_HSplit",

	"screen_NameLen", "screen_Name", "screen_State",
	"screen_USE_BgWidth", "screen_USE_BgHeight", "screen_USE_Bg",
	
	"group_FirstG", "group_LastG", "group_SelectG",
	
	"menuitem_Window", "menuitem_Left", "menuitem_ShortCut",
	
	"menu_ColItem", "menu_ColSelect", "menu_ColDisabled", "menu_ColSelectDisabled",
	"menu_ColShtCut", "menu_ColSelShtCut", "menu_CommonItems", "menu_FlagDefColInfo",
	"menu_FirstI", "menu_LastI", "menu_SelectI",

	"msgport_WakeUp", "msgport_NameLen", "msgport_Name",
	"msgport_FirstMenu", "msgport_LastMenu", "msgport_FirstW", "msgport_LastW",
	"msgport_FirstGroup", "msgport_LastGroup", "msgport_FirstMutex", "msgport_LastMutex",

	"mutex_O_Prev", "mutex_O_Next", "mutex_Owner", "mutex_Perm", "mutex_NameLen", "mutex_Name",

	"row_Code", "row_Flags", "row_Len", "row_Text", "row_ColText",
};

    
void Usage(void) {
    fprintf(stderr, "Usage: %s [option] [object_id]\n"
	    "If no arguments are given, the first screen is used as default object_id\n"
	    "Currently known options:\n"
	    " -h, -help               display this help and exit\n"
	    " -V, -version            output version information and exit\n"
	    " -v, -verbose            always show all data, even huge arrays\n",
	    argv0);
}

void ShowVersion(void) {
    fputs("twlsobj " TWIN_VERSION_STR "\n", stdout);
}

static void human_print(byte *data, uldat len, byte may_trim) {
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

TW_DECL_MAGIC(lsobj_magic);

int main(int argc, char *argv[]) {
    uldat err = 1, id, index_list, len;
    tslist TSL;
    tsfield TSF;
    udat type_id;
    byte verbose = FALSE, isvec;
    
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
	} else if (!strcmp(*argv, "-v") || !strcmp(*argv, "-verbose")) {
	    verbose = TRUE;
	} else {
	    err = 0;
	    id = strtoul(*argv, NULL, 0);
	}
	argv++;
    }
    
    if (TwCheckMagic(lsobj_magic) && TwOpen(NULL)) do {

	if (err == 1)
	    id = TwFirstScreen();
	
	TSL = TwAStat(id, max_list, field_list);
	
	if (TSL) {
	    for (id = 0, TSF = TSL->TSF; id < TSL->N; id++, TSF++) {
		for (index_list = 0; index_list < max_list; index_list++) {
		    if (field_list[index_list] == TSF->hash)
			break;
		}
		if (index_list < max_list) {
		    len = strlen(name_list[index_list]);
		    printf("%s", name_list[index_list]);
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
			type_id = TWS_highest + 1;
		    
		    len += 2 + (isvec ? 2 : 0) + strlen(type_list[type_id]);

		    printf("= (%s%s)", type_list[type_id], isvec ? "[]" : "");
		    while (len < 44)
			len++, putchar(' ');

		    if (TSF->type >= TWS_vec && (TSF->type & ~TWS_vec) <= TWS_last) {
			if (TSF->TWS_field_vecL && TSF->TWS_field_vecV)
			    human_print(TSF->TWS_field_vecV, TSF->TWS_field_vecL, !verbose);
			else
			    printf(" (NULL)\n");
		    } else {
			err = TSF->TWS_field_scalar;
			printf("% -11ld # 0x%lx\n", (long)err, (long)err);
		    }
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

