#ifndef _TWIN_PRINTK_H
#define _TWIN_PRINTK_H

int printk(CONST byte *format, ...);
int flushk(void);

byte RegisterPrintk(int fd);
void UnRegisterPrintk(void);


#endif /* _TWIN_PRINTK_H */
