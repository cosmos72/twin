#ifndef _TW_PRINTK_H
#define _TW_PRINTK_H

int printk(CONST byte *format, ...);
int flushk(void);

byte RegisterPrintk(int fd);
void UnRegisterPrintk(void);


#endif /* _TW_PRINTK_H */
