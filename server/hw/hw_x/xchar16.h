/* 16-bit character representation for basic X11 */

typedef XChar2b XChar16;

INLINE XChar16 RawToXChar16(hwfont raw) {
  XChar16 p = {(unsigned char)(raw >> 8), (unsigned char)(raw & 0xFF)};
  return p;
}

INLINE hwfont XChar16ToRaw(XChar16 p) { return ((hwfont)p.byte1 << 8) | ((hwfont)p.byte2); }
