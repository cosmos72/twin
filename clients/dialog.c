/*
 *  dialog.c  --  twin-aware version of classical `dialog'
 *
 *  Copyright (C) 2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include <Tw/Tw.h>
#include <Tw/Twerrno.h>

#define COD_QUIT (udat)1

#define Max2(a, b) ((a) > (b) ? (a) : (b))

static tmsgport Dialog_MsgPort;
static tmenu Dialog_Menu;
static twindow Dialog_Win;

static byte separate_output;
static const char *title = "twdialog", *backtitle, *text, *input;

static dat width = 80, height = 25, percent;

typedef struct {
  char *tag;
  char *item;
  byte status;
} t_list;

static t_list *list;
static int listN;

static byte (*mode)(void);

static void AllocList(void) {
  if (!(list = (t_list *)malloc(listN * sizeof(t_list)))) {
    fprintf(stderr, "twdialog: out of memory!\n");
    exit(1);
  }
}

static byte OnOff(char *arg) {
  return strlen(arg) == 2 && (arg[0] == 'O' || arg[0] == 'o') && (arg[1] == 'N' || arg[1] == 'n');
}

static void Usage(void) {
  fprintf(stderr,
          "twdialog version 0.1.\n"
          "Display dialog boxes from shell scripts\n"
          "\n"
          "Usage: twdialog [--title <title>] [--separate-output] [--backtitle <backtitle>] <Box "
          "options>\n"
          "\n"
          "Box options:\n"
          "\n"
          "  --yesno     <text> <height> <width>\n"
          "  --msgbox    <text> <height> <width>\n"
          "  --infobox   <text> <height> <width>\n"
          "  --inputbox  <text> <height> <width> [<init>]\n"
          "  --textbox   <file> <height> <width>\n"
          "  --menu      <text> <height> <width> <menu height> <tag1> <item1>...\n"
          "  --checklist <text> <height> <width> <list height> <tag1> <item1> <status1>...\n"
          "  --radiolist <text> <height> <width> <list height> <tag1> <item1> <status1>...\n"
          "  --gauge     <text> <height> <width> <percent>\n");
}

static byte InitButtons(const char *bt1, const char *bt2) {
  return TwCreateButtonGadget(Dialog_Win, strlen(bt1), 1, bt1, 1, 0, TCOL(tblack, twhite),
                              TCOL(tblack, tgreen), TCOL(tBLACK, tgreen),
                              width / 2 - (bt2 ? strlen(bt1) + 2 : strlen(bt1) / 2), height - 2) &&
         (!bt2 || TwCreateButtonGadget(Dialog_Win, strlen(bt2), 1, bt2, 2, 0, TCOL(tblack, twhite),
                                       TCOL(tblack, tgreen), TCOL(tBLACK, tgreen),
                                       width / 2 + strlen(bt2) / 2, height - 2));
}

static byte InitGadgets(byte radio) {
  (void)radio;
  return tfalse;
}

static int CountNewLines(const char *s) {
  int ret = 0;
  while (s) {
    if ((s = strchr(s, '\n'))) {
      ret++;
      s++;
    }
  }
  return ret;
}

static byte ShowText(void) {
  if (text) {
    int len = strlen(text);
    if (!memchr(text, '\n', len))
      TwGotoXYWindow(Dialog_Win, (width - len) / 2, 0);
    TwWriteCharsetWindow(Dialog_Win, len, text);
  }
  return ttrue;
}

static byte ShowFile(void) {
  return tfalse;
}

static byte InitYesNoBox(void) {
  return ShowText() && InitButtons(" Yes ", " No ");
}

static byte InitMsgBox(void) {
  return ShowText() && InitButtons(" OK ", NULL);
}

static byte InitInfoBox(void) {
  return ShowText();
}

static byte InitInputBox(void) {
  return tfalse;
}

static byte InitTextBox(void) {
  return ShowFile() && InitButtons(" EXIT ", NULL);
}

static byte InitMenuBox(void) {
  int i, y;
  if (ShowText()) {
    y = CountNewLines(text) + 2;
    for (i = 0; i < listN; i++) {
      TwGotoXYWindow(Dialog_Win, 3, i + y);
      TwWriteCharsetWindow(Dialog_Win, strlen(list[i].tag), list[i].tag);
      TwGotoXYWindow(Dialog_Win, 10, i + y);
      TwWriteCharsetWindow(Dialog_Win, strlen(list[i].item), list[i].item);
    }
    return InitButtons("  OK  ", "Cancel");
  }
  return tfalse;
}

static byte InitCheckBox(void) {
  return ShowText() && InitGadgets(ttrue);
}

static byte InitRadioBox(void) {
  return ShowText() && InitGadgets(tfalse);
}

static byte InitGaugeBox(void) {
  return ShowText();
}

static byte ParseArgs(int argc, char *argv[]) {
  int eaten;

  if (++argv, !--argc) {
    Usage();
    return tfalse;
  }
  while (argc) {
    eaten = 1;

    if (!strcmp(*argv, "--separate-output"))
      separate_output = ttrue;
    else if (argc >= 2 && !strcmp(*argv, "--title"))
      title = argv[1], eaten = 2;
    else if (argc >= 2 && !strcmp(*argv, "--backtitle"))
      backtitle = argv[1], eaten = 2;
    else if (argc == 4 && !strcmp(*argv, "--yesno")) {
      eaten = argc;
      mode = InitYesNoBox;
      text = argv[1];
      height = atoi(argv[2]);
      width = atoi(argv[3]);
    } else if (argc == 4 && !strcmp(*argv, "--msgbox")) {
      eaten = argc;
      mode = InitMsgBox;
      text = argv[1];
      height = atoi(argv[2]);
      width = atoi(argv[3]);
    } else if (argc == 4 && !strcmp(*argv, "--infobox")) {
      eaten = argc;
      mode = InitInfoBox;
      text = argv[1];
      height = atoi(argv[2]);
      width = atoi(argv[3]);
    } else if ((argc == 4 || argc == 5) && !strcmp(*argv, "--inputbox")) {
      eaten = argc;
      mode = InitInputBox;
      text = argv[1];
      height = atoi(argv[2]);
      width = atoi(argv[3]);
      if (argc == 5)
        input = argv[4];
    } else if (argc == 4 && !strcmp(*argv, "--textbox")) {
      eaten = argc;
      mode = InitTextBox;
      text = argv[1];
      height = atoi(argv[2]);
      width = atoi(argv[3]);
    } else if (argc >= 7 && (argc % 2) == 1 && !strcmp(*argv, "--menu") &&
               (listN = atoi(argv[4])) * 2 + 5 == argc) {
      mode = InitMenuBox;
      text = argv[1];
      height = atoi(argv[2]);
      width = atoi(argv[3]);
      AllocList();
      for (eaten = 0; eaten < listN; eaten++) {
        list[eaten].tag = argv[5 + eaten * 2];
        list[eaten].item = argv[6 + eaten * 2];
      }
      eaten = argc;
    } else if (argc >= 8 && (argc % 3) == 2 && !strcmp(*argv, "--checklist") &&
               (listN = atoi(argv[4])) * 3 + 5 == argc) {
      mode = InitCheckBox;
      text = argv[1];
      height = atoi(argv[2]);
      width = atoi(argv[3]);
      AllocList();
      for (eaten = 0; eaten < listN; eaten++) {
        list[eaten].tag = argv[5 + eaten * 3];
        list[eaten].item = argv[6 + eaten * 3];
        list[eaten].status = OnOff(argv[7 + eaten * 3]);
      }
      eaten = argc;
    } else if (argc >= 8 && (argc % 3) == 2 && !strcmp(*argv, "--radiolist") &&
               (listN = atoi(argv[4])) * 3 + 5 == argc) {
      mode = InitRadioBox;
      text = argv[1];
      height = atoi(argv[2]);
      width = atoi(argv[3]);
      AllocList();
      for (eaten = 0; eaten < listN; eaten++) {
        list[eaten].tag = argv[5 + eaten * 3];
        list[eaten].item = argv[6 + eaten * 3];
        list[eaten].status = OnOff(argv[7 + eaten * 3]);
      }
      eaten = argc;
    } else if (argc == 5 && !strcmp(*argv, "--gauge")) {
      eaten = argc;
      mode = InitGaugeBox;
      text = argv[1];
      height = atoi(argv[2]);
      width = atoi(argv[3]);
      percent = atoi(argv[4]);
    } else {
      Usage();
      return tfalse;
    }
    argc -= eaten;
    argv += eaten;
  }
  width -= 2;
  height -= 2;
  return ttrue;
}

TW_DECL_MAGIC(dialog_magic);

static byte InitDialog(void) {
  const tcolor b = TCOL(tblack, twhite), r = TCOL(tred, twhite);
  const tcolor color_array[13] = {b, r, b, b, b, b, r, b, b, b, b, b, b};
  twindow Window;

  return TwCheckMagic(dialog_magic) && TwOpen(NULL) &&
         (Dialog_MsgPort = TwCreateMsgPort(8, "twdialog")) &&
         (Dialog_Menu = TwCreateMenu(TCOL(tblack, twhite), TCOL(tblack, tgreen),
                                     TCOL(tBLACK, twhite), TCOL(tBLACK, tblack), TCOL(tred, twhite),
                                     TCOL(tred, tgreen), (byte)0)) &&
         (TwInfo4Menu(Dialog_Menu, TW_ROW_ACTIVE, 13, " Twin Dialog ", color_array), ttrue) &&
         (Window = TwWin4Menu(Dialog_Menu)) &&
         TwRow4Menu(Window, COD_QUIT, TW_ROW_INACTIVE, 6, " Quit ") &&
         TwItem4Menu(Dialog_Menu, Window, ttrue, 6, " File ") && TwItem4MenuCommon(Dialog_Menu) &&
         (Dialog_Win =
              TwCreateWindow(strlen(title), title, NULL, Dialog_Menu, TCOL(tblack, twhite),
                             TW_LINECURSOR, TW_WINDOW_DRAG | TW_WINDOW_RESIZE | TW_WINDOW_CLOSE,
                             TW_WINDOWFL_ROWS_DEFCOL, width, height, 0)) &&
         (TwSetColorsWindow(Dialog_Win, 1 << 6, 0, 0, 0, 0, 0, 0, TCOL(tWHITE, tblue), 0, 0),
          ttrue) &&
         (*mode)() && (TwMapWindow(Dialog_Win, TwFirstScreen()), ttrue) && TwFlush();
}

int main(int argc, char *argv[]) {
  tmsg msg;
  tevent_gadget EventG;
  uldat err;

  if (!ParseArgs(argc, argv))
    return 255;

  if (InitDialog())
    while ((msg = TwReadMsg(ttrue))) {
      if (msg->Type == TW_MSG_WIDGET_GADGET) {
        EventG = &msg->Event.EventGadget;
        if (EventG->W == Dialog_Win)
          switch (EventG->Code) {
          case 0:
            return 1;
          default:
            return EventG->Code - 1;
          }
      }
    }
  if ((err = TwErrno))
    fprintf(stderr, "%s: libtw error: %s%s\n", argv[0], TwStrError(err),
            TwStrErrorDetail(err, TwErrnoDetail));

  if (!TwInPanic())
    TwClose();
  return 255;
}
