/* 16-bit character representation for xft */

typedef FcChar16 XChar16;

INLINE XChar16 RawToXChar16(hwfont raw) {
    return (XChar16) raw;
}

INLINE hwfont XChar16ToRaw(XChar16 p) {
    return (hwfont)p;
}
