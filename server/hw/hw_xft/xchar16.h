/* 16-bit character representation for xft */

typedef FcChar16 XChar16;

inline XChar16 RawToXChar16(trune raw) {
  return (XChar16)raw;
}

inline trune XChar16ToRaw(XChar16 p) {
  return (trune)p;
}
