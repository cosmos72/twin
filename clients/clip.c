/*
 *  clip.c  --  a wannabe editor for twin. Currently just echoes keyboard.
 *
 *  This program is placed in the public domain.
 *
 */

#include <stdio.h>

#include <Tw/Tw.h>
#include <Tw/Twerrno.h>

#define COD_QUIT (udat)1

static tmsgport Clip_MsgPort;
static tmenu Clip_Menu;
static twindow Clip_Win;

TW_DECL_MAGIC(clip_magic);

static byte InitClip(void) {
  twindow Window;

  return TwCheckMagic(clip_magic) && TwOpen(NULL) &&
         (Clip_MsgPort = TwCreateMsgPort(11, "twclipboard")) &&
         (Clip_Menu = TwCreateMenu(TCOL(tblack, twhite), TCOL(tblack, tgreen),
                                   TCOL(thigh | tblack, twhite), TCOL(thigh | tblack, tblack),
                                   TCOL(tred, twhite), TCOL(tred, tgreen), (byte)0)) &&
         (TwInfo4Menu(Clip_Menu, TW_ROW_ACTIVE, 16, " Twin Clipboard ",
                      (const tcolor *)"ptpppptppppppppp"),
          (Clip_Win =
               TwCreateWindow(14, "Twin Clipboard", NULL, Clip_Menu,
                              TCOL(thigh | twhite, thigh | tblack), TW_LINECURSOR,
                              TW_WINDOW_WANT_KEYS | TW_WINDOW_DRAG | TW_WINDOW_RESIZE |
                                  TW_WINDOW_X_BAR | TW_WINDOW_Y_BAR | TW_WINDOW_CLOSE,
                              TW_WINDOWFL_CURSOR_ON | TW_WINDOWFL_ROWS_DEFCOL, 38, 18, 0))) &&
         (Window = TwWin4Menu(Clip_Menu)) &&
         TwRow4Menu(Window, COD_QUIT, TW_ROW_INACTIVE, 17, " Quit      Alt-X ") &&
         TwItem4Menu(Clip_Menu, Window, ttrue, 6, " File ") &&
         (TwSetColorsWindow(Clip_Win, 0x1FF, TCOL(thigh | tgreen, twhite), TCOL(tcyan, tblue),
                            TCOL(thigh | tblue, tblack), TCOL(thigh | twhite, thigh | tblue),
                            TCOL(thigh | twhite, thigh | tblue),
                            TCOL(thigh | twhite, thigh | tblack), TCOL(thigh | tblack, twhite),
                            TCOL(tblack, thigh | tblack), TCOL(tblack, twhite)),
          TwConfigureWindow(Clip_Win, 0xF << 2, 0, 0, 7, 3, TW_MAXDAT, TW_MAXDAT),
          (Window = TwWin4Menu(Clip_Menu))) &&
         TwItem4Menu(Clip_Menu, Window, tfalse, 6, " Clip ") &&
         TwRow4Menu(Window, (udat)0, TW_ROW_INACTIVE, 16, " Undo           ") &&
         TwRow4Menu(Window, (udat)0, TW_ROW_INACTIVE, 16, " Redo           ") &&
         TwRow4Menu(Window, (udat)0, TW_ROW_IGNORE, 16,
                    "\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4") &&
         TwRow4Menu(Window, (udat)0, TW_ROW_ACTIVE, 16, " Cut            ") &&
         TwRow4Menu(Window, (udat)0, TW_ROW_ACTIVE, 16, " Copy           ") &&
         TwRow4Menu(Window, (udat)0, TW_ROW_ACTIVE, 16, " Paste          ") &&
         TwRow4Menu(Window, (udat)0, TW_ROW_ACTIVE, 16, " Clear          ") &&
         TwRow4Menu(Window, (udat)0, TW_ROW_IGNORE, 16,
                    "\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4") &&
         TwRow4Menu(Window, (udat)0, TW_ROW_INACTIVE, 16, " Show Clipboard ") &&
         (Window = TwWin4Menu(Clip_Menu)) && TwItem4Menu(Clip_Menu, Window, ttrue, 8, " Search ") &&
         TwRow4Menu(Window, (udat)0, TW_ROW_ACTIVE, 9, " Find    ") &&
         TwRow4Menu(Window, (udat)0, TW_ROW_ACTIVE, 9, " Replace ") &&
         TwItem4MenuCommon(Clip_Menu) && (TwMapWindow(Clip_Win, TwFirstScreen()), TwFlush());
}

int main(int argc, char *argv[]) {
  tmsg msg;
  uldat err, WinN = 1;

  if (InitClip())
    while ((msg = TwReadMsg(ttrue))) {
      if (msg->Type == TW_MSG_WIDGET_KEY) {

        tevent_keyboard EventK = &msg->Event.EventKeyboard;
        (void)TwWriteCharsetWindow(EventK->W, EventK->SeqLen, EventK->AsciiSeq);

      } else if (msg->Type == TW_MSG_SELECTION) {
        /*
         * send msg->Event.EventSelection.Window as ReqPrivate field,
         * so that we will get it back in TW_MSG_SELECTIONNOTIFY message
         * without having to store it manually
         */
        TwRequestSelection(TwGetOwnerSelection(), msg->Event.EventSelection.W);

      } else if (msg->Type == TW_MSG_SELECTIONNOTIFY) {

        tevent_selectionnotify EventN = &msg->Event.EventSelectionNotify;
        if (EventN->Magic == TW_SEL_UTF8MAGIC)
          (void)TwWriteUtf8Window(EventN->ReqPrivate, EventN->Len, EventN->Data);

      } else if (msg->Type == TW_MSG_SELECTIONCLEAR) {
        ;
      } else if (msg->Type == TW_MSG_WIDGET_GADGET) {

        tevent_gadget EventG = &msg->Event.EventGadget;
        if (EventG->Code == 0 && EventG->W == Clip_Win) {
          TwUnMapWindow(Clip_Win);
          TwDeleteWindow(Clip_Win);
          if (!--WinN)
            break;
        }
      }
    }
  if ((err = TwErrno))
    fprintf(stderr, "%s: libtw error: %s%s\n", argv[0], TwStrError(err),
            TwStrErrorDetail(err, TwErrnoDetail));

  if (!TwInPanic()) {
    /* these are not strictly necessary, as the server would cleanup by itself... */
    TwDeleteMenu(Clip_Menu);
    TwDeleteMsgPort(Clip_MsgPort);
    TwClose();
    return 0;
  }
  return 1;
}
