
extern hwattr *OldVideo;

extern byte ValidOldVideo;
/*
 * set ValidOldVideo to TRUE if the contents of OldVideo[] is valid,
 * set it to FALSE if it's not valid
 * (for example after a window resize or some other operation that
 * corrupts the display).
 */

extern dat (*ChangedVideo)[2][2];
extern byte ChangedVideoFlag;
extern byte ChangedMouseFlag;

extern udat ScreenWidth, ScreenHeight;

extern FILE *errFILE;

extern dat Last_x, Last_y;

extern struct termios ttysave;

void VideoFlipMouse(void);
void KeyboardEventCommon(udat Code, udat Len, byte *Seq);
void MouseEventCommon(dat x, dat y, dat dx, dat dy, udat IdButtons);
void FillOldVideo(dat Xstart, dat Ystart, dat Xend, dat Yend, hwattr Attr);
void InitTtysave(void);
void saveDisplaySize(void);


