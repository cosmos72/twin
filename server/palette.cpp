/*
 *  palette.cpp
 *
 *  Copyright (C) 1993-2025 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "palette.h"

#define L 85
#define M 170
#define H 255

#define A 73
#define B 126
#define C 174
#define D 217
#define E 255

#define TGRAY(n) TRGB((n), (n), (n))

#ifdef TWIN_PALETTE_SOLARIZED
#define TDARK TRGB(4, 8, 16)
#define TLIGHT TRGB(187, 187, 187)
#else
#define TDARK TRGB(0, 0, 0)
#define TLIGHT TRGB(M, M, M)
#endif

/* the 256-color palette, compatible with xterm-256color palette */
const trgb Palette[tpalette_n] = {
    TDARK,         TRGB(M, 0, 0), TRGB(0, M, 0), TRGB(M, M, 0), TRGB(0, 0, M), TRGB(M, 0, M),
    TRGB(0, M, M), TLIGHT, /**/
    TRGB(L, L, L), TRGB(H, L, L), TRGB(L, H, L), TRGB(H, H, L), TRGB(L, L, H), TRGB(H, L, H),
    TRGB(L, H, H), TRGB(H, H, H), /**/

    TRGB(0, 0, 0), TRGB(0, 0, A), TRGB(0, 0, B), TRGB(0, 0, C), TRGB(0, 0, D), TRGB(0, 0, E),
    TRGB(0, A, 0), TRGB(0, A, A), TRGB(0, A, B), TRGB(0, A, C), TRGB(0, A, D), TRGB(0, A, E),
    TRGB(0, B, 0), TRGB(0, B, A), TRGB(0, B, B), TRGB(0, B, C), TRGB(0, B, D), TRGB(0, B, E),
    TRGB(0, C, 0), TRGB(0, C, A), TRGB(0, C, B), TRGB(0, C, C), TRGB(0, C, D), TRGB(0, C, E),
    TRGB(0, D, 0), TRGB(0, D, A), TRGB(0, D, B), TRGB(0, D, C), TRGB(0, D, D), TRGB(0, D, E),
    TRGB(0, E, 0), TRGB(0, E, A), TRGB(0, E, B), TRGB(0, E, C), TRGB(0, E, D), TRGB(0, E, E),

    TRGB(A, 0, 0), TRGB(A, 0, A), TRGB(A, 0, B), TRGB(A, 0, C), TRGB(A, 0, D), TRGB(A, 0, E),
    TRGB(A, A, 0), TRGB(A, A, A), TRGB(A, A, B), TRGB(A, A, C), TRGB(A, A, D), TRGB(A, A, E),
    TRGB(A, B, 0), TRGB(A, B, A), TRGB(A, B, B), TRGB(A, B, C), TRGB(A, B, D), TRGB(A, B, E),
    TRGB(A, C, 0), TRGB(A, C, A), TRGB(A, C, B), TRGB(A, C, C), TRGB(A, C, D), TRGB(A, C, E),
    TRGB(A, D, 0), TRGB(A, D, A), TRGB(A, D, B), TRGB(A, D, C), TRGB(A, D, D), TRGB(A, D, E),
    TRGB(A, E, 0), TRGB(A, E, A), TRGB(A, E, B), TRGB(A, E, C), TRGB(A, E, D), TRGB(A, E, E),

    TRGB(B, 0, 0), TRGB(B, 0, A), TRGB(B, 0, B), TRGB(B, 0, C), TRGB(B, 0, D), TRGB(B, 0, E),
    TRGB(B, A, 0), TRGB(B, A, A), TRGB(B, A, B), TRGB(B, A, C), TRGB(B, A, D), TRGB(B, A, E),
    TRGB(B, B, 0), TRGB(B, B, A), TRGB(B, B, B), TRGB(B, B, C), TRGB(B, B, D), TRGB(B, B, E),
    TRGB(B, C, 0), TRGB(B, C, A), TRGB(B, C, B), TRGB(B, C, C), TRGB(B, C, D), TRGB(B, C, E),
    TRGB(B, D, 0), TRGB(B, D, A), TRGB(B, D, B), TRGB(B, D, C), TRGB(B, D, D), TRGB(B, D, E),
    TRGB(B, E, 0), TRGB(B, E, A), TRGB(B, E, B), TRGB(B, E, C), TRGB(B, E, D), TRGB(B, E, E),

    TRGB(C, 0, 0), TRGB(C, 0, A), TRGB(C, 0, B), TRGB(C, 0, C), TRGB(C, 0, D), TRGB(C, 0, E),
    TRGB(C, A, 0), TRGB(C, A, A), TRGB(C, A, B), TRGB(C, A, C), TRGB(C, A, D), TRGB(C, A, E),
    TRGB(C, B, 0), TRGB(C, B, A), TRGB(C, B, B), TRGB(C, B, C), TRGB(C, B, D), TRGB(C, B, E),
    TRGB(C, C, 0), TRGB(C, C, A), TRGB(C, C, B), TRGB(C, C, C), TRGB(C, C, D), TRGB(C, C, E),
    TRGB(C, D, 0), TRGB(C, D, A), TRGB(C, D, B), TRGB(C, D, C), TRGB(C, D, D), TRGB(C, D, E),
    TRGB(C, E, 0), TRGB(C, E, A), TRGB(C, E, B), TRGB(C, E, C), TRGB(C, E, D), TRGB(C, E, E),

    TRGB(D, 0, 0), TRGB(D, 0, A), TRGB(D, 0, B), TRGB(D, 0, C), TRGB(D, 0, D), TRGB(D, 0, E),
    TRGB(D, A, 0), TRGB(D, A, A), TRGB(D, A, B), TRGB(D, A, C), TRGB(D, A, D), TRGB(D, A, E),
    TRGB(D, B, 0), TRGB(D, B, A), TRGB(D, B, B), TRGB(D, B, C), TRGB(D, B, D), TRGB(D, B, E),
    TRGB(D, C, 0), TRGB(D, C, A), TRGB(D, C, B), TRGB(D, C, C), TRGB(D, C, D), TRGB(D, C, E),
    TRGB(D, D, 0), TRGB(D, D, A), TRGB(D, D, B), TRGB(D, D, C), TRGB(D, D, D), TRGB(D, D, E),
    TRGB(D, E, 0), TRGB(D, E, A), TRGB(D, E, B), TRGB(D, E, C), TRGB(D, E, D), TRGB(D, E, E),

    TRGB(E, 0, 0), TRGB(E, 0, A), TRGB(E, 0, B), TRGB(E, 0, C), TRGB(E, 0, D), TRGB(E, 0, E),
    TRGB(E, A, 0), TRGB(E, A, A), TRGB(E, A, B), TRGB(E, A, C), TRGB(E, A, D), TRGB(E, A, E),
    TRGB(E, B, 0), TRGB(E, B, A), TRGB(E, B, B), TRGB(E, B, C), TRGB(E, B, D), TRGB(E, B, E),
    TRGB(E, C, 0), TRGB(E, C, A), TRGB(E, C, B), TRGB(E, C, C), TRGB(E, C, D), TRGB(E, C, E),
    TRGB(E, D, 0), TRGB(E, D, A), TRGB(E, D, B), TRGB(E, D, C), TRGB(E, D, D), TRGB(E, D, E),
    TRGB(E, E, 0), TRGB(E, E, A), TRGB(E, E, B), TRGB(E, E, C), TRGB(E, E, D), TRGB(E, E, E),

    TGRAY(15),     TGRAY(25),     TGRAY(35),     TGRAY(45),     TGRAY(55),     TGRAY(65),
    TGRAY(75),     TGRAY(85),     TGRAY(95),     TGRAY(105),    TGRAY(115),    TGRAY(125),
    TGRAY(135),    TGRAY(145),    TGRAY(155),    TGRAY(165),    TGRAY(175),    TGRAY(185),
    TGRAY(195),    TGRAY(205),    TGRAY(215),    TGRAY(225),    TGRAY(235),    TGRAY(245),
};

static byte RgbToPaletteColorLow8(const byte red, const byte green, const byte blue) {
  return (red >= L) | (green >= L) << 1 | (blue >= L) << 2;
}

static byte RgbToPaletteColorHigh8(const byte red, const byte green, const byte blue) {
  return (red >= M) | (green >= M) << 1 | (blue >= M) << 2 | 8;
}

static byte RgbToPaletteGray24(const byte red, const byte green, const byte blue) {
  const byte avg = ((udat)green * 2 + (udat)red + (udat)blue) / 4;
  const byte index_0_25 = ((udat)avg * 205) / 2048; // faster than avg / 10;
  switch (index_0_25) {
  case 0:
    return 232;
  case 25:
    return 255;
  default:
    return (index_0_25 - 1) + 232;
  }
}

static byte Intensity256To6(const byte intensity) {
  if (intensity <= (B + C) / 2) {
    if (intensity <= A / 2) {
      return 0;
    } else if (intensity <= (A + B) / 2) {
      return 1;
    } else
      return 2;
  } else if (intensity <= (C + D) / 2) {
    return 3;
  } else if (intensity <= (D + E) / 2) {
    return 4;
  } else {
    return 5;
  }
}

static byte RgbToPaletteColor216(const byte red, const byte green, const byte blue) {
  return Intensity256To6(red) * 36 + Intensity256To6(green) * 6 + Intensity256To6(blue) + 16;
}

static udat TrueColorDistance(const trgb rgb0, const trgb rgb1) {
  const dat red_delta = (dat)TRED(rgb0) - (dat)TRED(rgb1);
  const dat green_delta = (dat)TGREEN(rgb0) - (dat)TGREEN(rgb1);
  const dat blue_delta = (dat)TBLUE(rgb0) - (dat)TBLUE(rgb1);
  return (udat)(red_delta < 0 ? -red_delta : red_delta) +
         (udat)(green_delta < 0 ? -green_delta : green_delta) +
         (udat)(blue_delta < 0 ? -blue_delta : blue_delta);
}

byte TrueColorToPalette8(const trgb rgb) {
  const byte red = TRED(rgb);
  const byte green = TGREEN(rgb);
  const byte blue = TBLUE(rgb);
  return RgbToPaletteColorLow8(red, green, blue);
}

byte TrueColorToPalette16(trgb rgb) {
  const byte red = TRED(rgb);
  const byte green = TGREEN(rgb);
  const byte blue = TBLUE(rgb);
  const byte index0 = RgbToPaletteColorLow8(red, green, blue);
  const byte index1 = RgbToPaletteColorHigh8(red, green, blue);
  const trgb rgb0 = Palette[index0];
  const trgb rgb1 = Palette[index1];
  const udat distance0 = TrueColorDistance(rgb, rgb0);
  const udat distance1 = TrueColorDistance(rgb, rgb1);
  if (distance0 < distance1) {
    return index0;
  } else {
    return index1;
  }
}

byte TrueColorToPalette256(trgb rgb) {
  const byte red = TRED(rgb);
  const byte green = TGREEN(rgb);
  const byte blue = TBLUE(rgb);
  const byte index0 = RgbToPaletteColorLow8(red, green, blue);
  const byte index1 = RgbToPaletteColorHigh8(red, green, blue);
  const byte index2 = RgbToPaletteGray24(red, green, blue);
  const byte index3 = RgbToPaletteColor216(red, green, blue);
  const trgb rgb0 = Palette[index0];
  const trgb rgb1 = Palette[index1];
  const trgb rgb2 = Palette[index2];
  const trgb rgb3 = Palette[index3];
  const udat distance0 = TrueColorDistance(rgb, rgb0);
  const udat distance1 = TrueColorDistance(rgb, rgb1);
  const udat distance2 = TrueColorDistance(rgb, rgb2);
  const udat distance3 = TrueColorDistance(rgb, rgb3);
  if (distance0 <= distance1) {
    if (distance2 <= distance3) {
      if (distance0 <= distance2) {
        return index0;
      } else {
        return index2;
      }
    } else {
      if (distance0 <= distance3) {
        return index0;
      } else {
        return index3;
      }
    }
  } else {
    if (distance2 <= distance3) {
      if (distance1 <= distance2) {
        return index1;
      } else {
        return index2;
      }
    } else {
      if (distance1 <= distance3) {
        return index1;
      } else {
        return index3;
      }
    }
  }
}

#undef H
#undef M
#undef L

#undef A
#undef B
#undef C
#undef D
#undef E

#undef TGRAY
#undef TDARK
#undef TLIGHT
