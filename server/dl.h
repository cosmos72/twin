#ifndef _TWIN_DL_H
#define _TWIN_DL_H

byte DlOpen(module Module);
void DlClose(module Module);

#define MainSo		((uldat)0)
#define WMSo		((uldat)1)
#define TermSo		((uldat)2)
#define SocketSo	((uldat)3)
#define RCParseSo	((uldat)4)
#define MAX_So		((uldat)5)

module DlLoad(uldat code);
void   DlUnLoad(uldat code);
module DlIsLoaded(uldat code);
udat   DlName2Code(byte *name);

module DlLoadAny(uldat len, byte *name);

void *DlSym(module Module, CONST byte *name);

#endif /* _TWIN_DL_H */
