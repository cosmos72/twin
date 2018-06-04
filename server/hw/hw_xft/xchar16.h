/* 16-bit character representation for xft */

typedef FcChar16 XChar16;

static void XChar16FromRaw(hwfont raw, XChar16 *p) {
    *p = (XChar16) raw;
}

static hwfont XChar16ToRaw(XChar16 *p) {
    return (hwfont)(*p);
}
