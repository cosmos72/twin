#ifndef _TW_DL_H
#define _TW_DL_H

byte DlOpen(module *Module);
void DlClose(module *Module);

#define MainSo   ((uldat)0)
#define TermSo   ((uldat)1)
#define SocketSo ((uldat)2)
#define MAX_So   (SocketSo+1)

module *DlLoad(uldat code);
void  DlUnLoad(uldat code);

module *DlLoadAny(uldat len, byte *name);

#endif /* _TW_DL_H */
