/*
 *  cat.c  --  twin-aware version of classical `cat'
 *
 *  This program is placed in the public domain.
 *
 */

#include <Tw/Tw.h>
#include <Tw/Twerrno.h>

#include <stdio.h>

#ifdef TW_HAVE_STRING_H
#include <string.h>
#endif
#ifdef TW_HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef TW_HAVE_FCNTL_H
#include <fcntl.h>
#endif

#define COD_QUIT (udat)1

static tmsgport Cat_MsgPort;
static tmenu Cat_Menu;
static twindow Cat_Win;

TW_DECL_MAGIC(cat_magic);

static byte InitCat(void) {
  const tcolor b = TCOL(tblack, twhite), r = TCOL(tred, twhite);
  const tcolor color_array[10] = {b, r, b, b, b, b, r, b, b, b};

  twindow Window;

  return TwCheckMagic(cat_magic) && TwOpen(NULL) && (Cat_MsgPort = TwCreateMsgPort(5, "twcat")) &&
         (Cat_Menu = TwCreateMenu(TCOL(tblack, twhite), TCOL(tblack, tgreen), TCOL(tBLACK, twhite),
                                  TCOL(tBLACK, tblack), TCOL(tred, twhite), TCOL(tred, tgreen),
                                  (byte)0)) &&
         (TwInfo4Menu(Cat_Menu, TW_ROW_ACTIVE, 10, " Twin Cat ", color_array),
          (Cat_Win =
               TwCreateWindow(8, "Twin Cat", NULL, Cat_Menu, TCOL(tblack, tBLACK), TW_LINECURSOR,
                              TW_WINDOW_DRAG | TW_WINDOW_RESIZE | TW_WINDOW_X_BAR |
                                  TW_WINDOW_Y_BAR | TW_WINDOW_CLOSE,
                              TW_WINDOWFL_USEROWS | TW_WINDOWFL_ROWS_DEFCOL, 80, 25, 0))) &&
         (Window = TwWin4Menu(Cat_Menu)) && TwItem4Menu(Cat_Menu, Window, ttrue, 6, " File ") &&
         (TwSetColorsWindow(Cat_Win, 0x1FF, TCOL(tGREEN, twhite), TCOL(tcyan, tblue),
                            TCOL(tBLUE, tblack), TCOL(tWHITE, tBLUE), TCOL(tWHITE, tBLUE),
                            TCOL(tWHITE, tBLACK), TCOL(tBLACK, twhite), TCOL(tblack, tBLACK),
                            TCOL(tblack, twhite)),
          TwConfigureWindow(Cat_Win, 0xF << 2, 0, 0, 7, 3, TW_MAXDAT, TW_MAXDAT),
          TwRow4Menu(Window, COD_QUIT, TW_ROW_INACTIVE, 17, " Quit      Alt-X ")) &&
         TwItem4MenuCommon(Cat_Menu) && (TwMapWindow(Cat_Win, TwFirstScreen()), TwFlush());
}

int main(int argc, char *argv[]) {
  int fd = -1, got;
  char buf[4096];
  tmsg msg;
  tevent_gadget EventG;
  uldat err;

  TwMergeHyphensArgv(argc, argv);

  if (argc > 2) {
    fprintf(stderr, "usage %s filename\n", argv[0]);
    return 0;
  }

  if (!argv[1] || !*argv[1]) {
    fd = 0;
  } else {
    if (!strcmp("-", argv[1]))
      fd = 0;
    else {
      fd = open(argv[1], O_RDONLY);
      if (fd < 0) {
        fprintf(stderr, "%s: failed to open `%s': %s\n", argv[0], argv[1], strerror(errno));
        return 0;
      }
    }
  }

  if (!InitCat()) {
    err = TwErrno;
    fprintf(stderr, "%s: libtw error: %s%s\n", argv[0], TwStrError(err),
            TwStrErrorDetail(err, TwErrnoDetail));
    return 0;
  }

  while ((got = read(fd, buf, 4096)) > 0) {
    TwWriteCharsetWindow(Cat_Win, got, buf);
    TwFlush();
  }
  while ((msg = TwReadMsg(ttrue))) {
    if (msg->Type == TW_MSG_WIDGET_GADGET) {
      EventG = &msg->Event.EventGadget;
      if (EventG->Code == 0 && EventG->W == Cat_Win)
        break;
    }
  }
  if ((err = TwErrno))
    fprintf(stderr, "%s: libtw error: %s%s\n", argv[0], TwStrError(err),
            TwStrErrorDetail(err, TwErrnoDetail));

  if (!TwInPanic()) {
    /* these are not strictly necessary, as the server would cleanup by itself... */
    TwUnMapWindow(Cat_Win);
    TwDeleteWindow(Cat_Win);
    TwDeleteMenu(Cat_Menu);
    TwDeleteMsgPort(Cat_MsgPort);
    TwClose();
    return 0;
  }
  return 1;
}
