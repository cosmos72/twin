/*
 *  clutter.c  --  Useless client that spams twin with windows. Used for benchmarks ;-)
 *
 *  This program is placed in the public domain.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Tw/Tw.h>
#include <Tw/Twerrno.h>

static tmsgport Clutter_MsgPort;
static tmenu Clutter_Menu;
static tscreen Clutter_Screen;

dat X, Y;

#define MAXLRAND48 0x7FFFFFFFl

static byte NewClutterWindow(void) {
  twindow Window;
  if ((Window = TwCreateWindow(
           7, "Clutter", NULL, Clutter_Menu, TCOL(tblack, thigh | tblack), TW_LINECURSOR,
           TW_WINDOW_DRAG | TW_WINDOW_RESIZE | TW_WINDOW_X_BAR | TW_WINDOW_Y_BAR | TW_WINDOW_CLOSE,
           TW_WINDOWFL_ROWS_DEFCOL, 11, 3, 0))) {

    TwConfigureWindow(Window, 0x3, lrand48() / (MAXLRAND48 / X), lrand48() / (MAXLRAND48 / Y), 0, 0,
                      0, 0);
    TwMapWindow(Window, Clutter_Screen);
    return ttrue;
  }
  return tfalse;
}

TW_DECL_MAGIC(clutter_magic);

static byte InitClutter(void) {

  if (!TwCheckMagic(clutter_magic) || !TwOpen(NULL))
    return tfalse;

  srand48(time(NULL));
  Clutter_Screen = TwFirstScreen();
  X = TwGetDisplayWidth();
  Y = TwGetDisplayHeight();

  if ((Clutter_MsgPort = TwCreateMsgPort(9, "twclutter")) &&
      (Clutter_Menu = TwCreateMenu(TCOL(tblack, twhite), TCOL(tblack, tgreen),
                                   TCOL(thigh | tblack, twhite), TCOL(thigh | tblack, tblack),
                                   TCOL(tred, twhite), TCOL(tred, tgreen), (byte)0)) &&
      TwItem4MenuCommon(Clutter_Menu)) {

    TwInfo4Menu(Clutter_Menu, TW_ROW_ACTIVE, 17, " Useless Clutter ", NULL);

    return ttrue;
  }
  return tfalse;
}

int main(int argc, char *argv[]) {
  tmsg msg;
  uldat err;

  if (!InitClutter()) {
    err = TwErrno;
    fprintf(stderr, "%s: libtw error: %s%s\n", argv[0], TwStrError(err),
            TwStrErrorDetail(err, TwErrnoDetail));
    return 1;
  }

  while (NewClutterWindow()) {
    while ((msg = TwReadMsg(tfalse))) {
      if (msg->Type == TW_MSG_WIDGET_GADGET) {
        if (msg->Event.EventGadget.Code == 0) {
          return 0;
        }
      }
    }
  }
  if ((err = TwErrno)) {
    fprintf(stderr, "%s: libtw error: %s%s\n", argv[0], TwStrError(err),
            TwStrErrorDetail(err, TwErrnoDetail));
    return 1;
  }
  return 0;
}
