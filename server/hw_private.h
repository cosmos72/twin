
extern dat (*ChangedVideo)[2][2];
extern byte ChangedVideoFlag;

extern udat ScreenWidth, ScreenHeight;
extern udat TryScreenWidth, TryScreenHeight;

extern FILE *errFILE;

extern struct termios ttysave;

void VideoFlipMouse(void);
void KeyboardEventCommon(udat Code, udat Len, byte *Seq);
void MouseEventCommon(dat x, dat y, dat dx, dat dy, udat IdButtons);
void FillOldVideo(dat Xstart, dat Ystart, dat Xend, dat Yend, hwattr Attr);
void saveDisplaySize(void);


extern display_hw *HW;
	    
#define SaveHW	display_hw *s_hw = HW

#define SetHW(hw)	(HW = (hw))

#define RestoreHW	(HW = s_hw)
	
#define WithHW(hw, call) do { SaveHW; SetHW(hw); (call); RestoreHW; } while (0)

#define XY ((udat  *)HW->gotoxybuf)
#define TT ((uldat *)HW->cursorbuf)

