#ifndef _TW_SCROLLER_H
#define _TW_SCROLLER_H

byte InitScroller(void);

extern msgport Scroller_MsgPort;
extern msg Do_Scroll, Dont_Scroll;

#endif /* _TW_SCROLLER_H */
