/* 16-bit character representation for basic X11 */

typedef XChar2b XChar16;

static void XChar16FromRaw(hwfont raw, XChar16 *p) {
    p->byte1 = raw >> 8;
    p->byte2 = raw & 0xFF;
}

static hwfont XChar16ToRaw(XChar16 *p) {
    return (hwfont)(p->byte1 << 8) | (hwfont)(p->byte2);
}
