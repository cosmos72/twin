#ifndef _TWIN_SCROLLER_H
#define _TWIN_SCROLLER_H

byte InitScroller(void);

extern msgport Scroller_MsgPort;
extern msg Do_Scroll, Dont_Scroll;

#endif /* _TWIN_SCROLLER_H */
