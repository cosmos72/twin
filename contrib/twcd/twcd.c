/* 
   This program is (c)2000 Joe Wreschnig under the GNU GPL v2
   Email piman@pfnet.org with comments, it's my first TWIN program

   TWCD - a CD player for TWIN. Uses libcdaudio.

   Based off the TWEdit, TWCat, and TWSysMon code.
*/

#include <stdio.h>
#include <errno.h>
#include <cdaudio.h>
#include <Tw/Tw.h>
#include <Tw/Twerrno.h>

#define COD_QUIT (udat)1
#define CD_PLAY (udat)2
#define CD_STOP (udat)3
#define CD_NEXT (udat)4
#define CD_PREV (udat)5
#define CD_EJECT (udat)6

static tmsgport TWCD_MsgPort;
static tmenu TWCD_Menu;
static twindow TWCD_Win;
int track = 1, cdid;
struct disc_info cdinfo;

TW_DECL_MAGIC(twcd_magic);

byte InitTWCD(void) {
    twindow FileWindow,PlayWindow;

    if (!TwCheckMagic(twcd_magic) || !TwOpen(NULL))
	return FALSE;

    TWCD_MsgPort=TwCreateMsgPort(4,"twcd");

    TWCD_Menu=TwCreateMenu(COL(BLACK,WHITE),COL(BLACK,GREEN),
        COL(HIGH|BLACK,WHITE),COL(HIGH|BLACK,BLACK),COL(RED,WHITE),
        COL(RED,GREEN),(byte)0);

    TwInfo4Menu(TWCD_Menu,TW_ROW_ACTIVE,(uldat)8," Twin CD ","ppppppttp");

    TWCD_Win=TwCreateWindow(9, " Twin CD ", NULL, TWCD_Menu,
	COL(HIGH|WHITE,BLUE), TW_NOCURSOR,
        TW_WINDOW_DRAG|TW_WINDOW_CLOSE|TW_WINDOW_WANT_MOUSE|TW_WINDOW_WANT_KEYS,
        TW_WINDOWFL_USEROWS|TW_WINDOWFL_ROWS_DEFCOL, (udat)18, (udat)3, (udat)0);

    FileWindow=TwWin4Menu(TWCD_Menu);
    TwItem4Menu(TWCD_Menu, FileWindow, TRUE, 6, " File ");
    TwRow4Menu(FileWindow, COD_QUIT, TW_ROW_ACTIVE, 6, " Quit ");
    PlayWindow=TwWin4Menu(TWCD_Menu);
    TwItem4Menu(TWCD_Menu, PlayWindow, TRUE, 6, " Play ");

    if (!TwItem4MenuCommon(TWCD_Menu))
    	return FALSE;

    TwGotoXYWindow(TWCD_Win, 0, 2);
    TwWriteAsciiWindow(TWCD_Win, 17, "<< [] || -> >> /\\");
    TwMapWindow(TWCD_Win, TwFirstScreen());
    TwFlush();

    return TRUE;
}

void writeTrack (char *title) {
    TwGotoXYWindow(TWCD_Win, 0, 0);
    TwWriteAsciiWindow(TWCD_Win, 20, "                    ");
    TwGotoXYWindow(TWCD_Win, 0, 0);
    TwWriteAsciiWindow(TWCD_Win, strlen(title), title);
}

void changeTrack(void) {
    char foo[20];
    if (track != -1) {
        sprintf(foo, "Track %d", track);
        writeTrack(foo);
        cd_play(cdid, track);
    } else writeTrack("no disc");
}                                                      

int main(int argc, char **argv) {
    tmsg Msg;
    tevent_any EventA;
    tevent_mouse EventM;
    udat Code, x, y;
    cdid = cd_init_device("/dev/cdrom");
    cd_stat(cdid, &cdinfo);
    printf("TWCD 0.2, (c)2000 Joe Wreschnig under the terms of the GNU GPL v2 or\nlater. See the COPYING file included with the distribution for the\nfull license, or visit http://www.gnu.org\n\n");
    if (!InitTWCD()) {
        fprintf(stderr, "%s: libTw error: %s%s\n",
            argv[0], TwStrError(TwErrno), TwStrErrorDetail(TwErrno, TwErrnoDetail));
        return 1;
    }
    writeTrack("   Twin CD 0.1");
    while ((Msg=TwReadMsg(TRUE))) {
        static byte eject = 0;
        EventA=&Msg->Event;
        if (Msg->Type==TW_MSG_WIDGET_MOUSE) {
            EventM=&Msg->Event.EventMouse;
            Code = EventA->EventMouse.Code;
            y = EventA->EventMouse.Y;
            x = EventA->EventMouse.X;
/*            printf("Code: %d\nX: %d\nY: %d\n", Code, x, y); */
            if (isPRESS(Code) && y == 2) switch (x) {
                case 0:
                case 1:
                { if (track > 1 && !eject) track--; changeTrack(); break; }
                case 3:
                case 4:
                { cd_stop(cdid); break; }
                case 6:
                case 7:
                {
                cd_stat(cdid, &cdinfo);
                if (cdinfo.disc_mode == CDAUDIO_PLAYING) {
                    cd_pause(cdid);
                  } else {
                    cd_resume(cdid);
                  } break;
                }
                case 9:
                case 10:
                { cd_play(cdid, track); cd_stat(cdid, &cdinfo); changeTrack(); break; }
                case 12:
                case 13:
                { if (track < cdinfo.disc_total_tracks && !eject) track++;
                    changeTrack();
                    cd_stat(cdid, &cdinfo);
                    break;
                }
                case 15:
                case 16:
                { if (eject == 0) {
                    cd_eject(cdid); eject = 1; track = -1;
                  } else {
                    cd_close(cdid); eject = 0; track = 1; cd_stat(cdid, &cdinfo);
                  } changeTrack(); break;
                }
                default:
                 break;
            }
        } else if (Msg->Type==TW_MSG_MENU_ROW) {
            if (EventA->EventMenu.Code == COD_QUIT && EventA->EventMenu.W == TWCD_Win)
                break;
        } else if (Msg->Type==TW_MSG_WIDGET_GADGET) {
            if (EventA->EventGadget.Code == 0 && EventA->EventGadget.W == TWCD_Win)
                break;
        }
    }

    if (TwErrno)
        printf("%s: libTw error: %s%s\n", argv[0],
	       TwStrError(TwErrno), TwStrErrorDetail(TwErrno, TwErrnoDetail));

    if (!TwInPanic()) {
        TwUnMapWindow(TWCD_Win);
        TwDeleteWindow(TWCD_Win);
        TwDeleteMenu(TWCD_Menu);
        TwDeleteMsgPort(TWCD_MsgPort);
        TwClose();
        return 0;
    }
    return 1;
}
