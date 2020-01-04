/* 16-bit character representation for xft */

typedef FcChar16 XChar16;

INLINE XChar16 RawToXChar16(trune raw) {
  return (XChar16)raw;
}

INLINE trune XChar16ToRaw(XChar16 p) {
  return (trune)p;
}
