/*
 *  setroot.c  -- set the screen background image of a running twin
 *
 *  Copyright (C) 2000 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include <stdio.h>
#include <string.h>
#include <signal.h>

#include <Tw/Tw.h>
#include <Tw/Twerrno.h>

byte *name;

void usage(void) {
    fprintf(stderr, "Usage: %s [--padx <X>] [--pady <Y>] [--aa <ascii art file>]\n", name);
}

void panic(void) {
    fprintf(stderr, "%s: Out of memory!\n", name);
    exit(1);
}


hwattr *load_ascii_art(FILE *aaFILE, uldat *x, uldat *y, uldat padX, uldat padY);

TW_DECL_MAGIC(setroot_magic);

int main(int argc, char *argv[]) {
    hwattr *image;
    uldat X, Y, padX = 0, padY = 0, err;
    enum {def, aa, padx, pady} state = def;
    byte *aafile = NULL;
    
    name = *argv;
    
    TwMergeHyphensArgv(argc, argv);
    
    while (*++argv) {
	switch (state) {
	  case def:
	    if (!strcmp(*argv, "-aa"))
		state = aa;
	    else if (!strcmp(*argv, "-padx"))
		state = padx;
	    else if (!strcmp(*argv, "-pady"))
		state = pady;
	    break;
	  case aa:
	    aafile = *argv;
	    state = def;
	    break;
	  case padx:
	    padX = atoi(*argv);
	    state = def;
	    break;
	  case pady:
	    padY = atoi(*argv);
	    state = def;
	    break;
	  default:
	    usage();
	    return 0;
	}
    }
    
    if (state != def) {
	usage();
	return 0;
    }
	
    if (TwCheckMagic(setroot_magic) && TwOpen(NULL)) {
	
	FILE *aaFILE = aafile ? fopen(aafile, "r") : stdin;
	if (!aafile) aafile = "(stdin)";
	
	if (aaFILE) {
	    if ((image = load_ascii_art(aaFILE, &X, &Y, padX, padY))) {
		TwBgImageScreen(TwFirstScreen(), X, Y, image);
		TwFlush();
		TwClose();
		return 0;
	    } else if (X || Y)
		fprintf(stderr, "%s: error reading `%s': %s\n", name, aafile, strerror(errno));
	    else
		fprintf(stderr, "%s: `%s' contains no ascii-art image!\n", name, aafile);
	} else
	    fprintf(stderr, "%s: cannot open `%s': %s\n", name, aafile, strerror(errno));
	TwClose();
    }
    if ((err = TwErrno))
	fprintf(stderr, "%s: libTw error: %s%s\n", name,
		TwStrError(err), TwStrErrorDetail(err, TwErrnoDetail));
	    
    return 1;
}


#define DefColor COL(WHITE,BLACK)
#define Underline COL(HIGH|WHITE,BLACK)
#define HalfInten COL(HIGH|BLACK,BLACK)

static hwattr *image;
static hwcol ColText = DefColor, Color = DefColor;
static uldat X, Y, Xmax, Ymax, Xreal, Yreal, max;
static enum {ESnormal, ESesc, ESsquare, ESgetpars, ESgotpars} State = ESnormal;
#define NPAR 16
uldat nPar, Par[NPAR];
udat Effects;

#define EFF_INTENSITY	((udat)0x0001)
#define EFF_HALFINTENS	((udat)0x0002)
#define EFF_UNDERLINE	((udat)0x0004)
#define EFF_BLINK	((udat)0x0008)
#define EFF_REVERSE	((udat)0x0010)

TW_INLINE void update_eff(void) {
    udat effects = Effects;
    hwcol fg = COLFG(ColText), bg = COLBG(ColText);
	
    if (effects & EFF_UNDERLINE)
	fg = COLFG(Underline);
    else if (effects & EFF_HALFINTENS)
	fg = COLFG(HalfInten);
    if (effects & EFF_REVERSE) {
	hwcol tmp = COL(bg & ~HIGH, fg & ~HIGH) | COL(fg & HIGH, bg & HIGH);
	fg = COLFG(tmp);
	bg = COLBG(tmp);
    }
    if (effects & EFF_INTENSITY)
	fg ^= HIGH;
    if (effects & EFF_BLINK)
	bg ^= HIGH;
    Color = COL(fg, bg);
}

TW_INLINE void csi_m(void) {
    udat i;
    udat effects = Effects;
    hwcol fg = COLFG(ColText), bg = COLBG(ColText);
    
    for (i=0; i<=nPar; i++) switch (Par[i]) {
      case 0:
	/* all attributes off */
	fg = COLFG(DefColor);
	bg = COLBG(DefColor);
	effects = 0;
	break;
      case 1:
	effects &= ~EFF_HALFINTENS;
	effects |= EFF_INTENSITY;
	break;
      case 2:
	effects &= ~EFF_INTENSITY;
	effects |= EFF_HALFINTENS;
	break;
      case 4:
	effects |= EFF_UNDERLINE;
	break;
      case 5:
	effects |= EFF_BLINK;
	break;
      case 7:
	effects |= EFF_REVERSE;
	break;
      case 21:
      case 22:
	effects &= ~(EFF_HALFINTENS|EFF_INTENSITY);
	break;
      case 24:
	effects &= ~EFF_UNDERLINE;
	break;
      case 25:
	effects &= ~EFF_BLINK;
	break;
      case 27:
	effects &= ~EFF_REVERSE;
	break;
      case 38: /* ANSI X3.64-1979 (SCO-ish?)
		* Enables underscore, white foreground
		* with white underscore
		* (Linux - use default foreground).
		*/
	fg = COLFG(DefColor);
	effects |= EFF_UNDERLINE;
	break;
      case 39: /* ANSI X3.64-1979 (SCO-ish?)
		* Disable underline option.
		* Reset colour to default? It did this
		* before...
		*/
	fg = COLFG(DefColor);
	effects &= ~EFF_UNDERLINE;
	break;
      case 49: /* restore default bg */
	bg = COLBG(DefColor);
	break;
      default:
	if (Par[i] >= 30 && Par[i] <= 37)
	    Par[i] -= 30,
	    fg = ANSI2VGA(Par[i]);
	else if (Par[i] >= 40 && Par[i] <= 47)
	    Par[i] -= 40,
	    bg = ANSI2VGA(Par[i]);
	break;
    }
    Effects = effects;
    ColText = COL(fg, bg);

    update_eff();
}

TW_INLINE void Fill(hwattr *t, hwattr h, uldat count) {
    while (count--)
	*t++ = h;
}

		
TW_INLINE void Xgrow(void) {
    uldat n, newXmax;

    newXmax = X * 2 + 2;
    n = newXmax * Ymax;
    if (!n) {
	Xmax = newXmax;
	return;
    }
    
    if (!(image = TwReAllocMem(image, n * sizeof(hwattr))))
	panic();
	
    max = n;
    n = Ymax;
	
    while (n--) {
	TwMoveMem(image + n * Xmax, image + n * newXmax, Xmax * sizeof(hwattr));
	Fill(image + n * newXmax + Xmax, HWATTR(Color, ' '), newXmax - Xmax);
    }
    Xmax = newXmax;
}

TW_INLINE void Ygrow(void) {
    uldat n, newYmax;

    newYmax = Y * 2 + 2;
    n = Xmax * newYmax;
    if (!n) {
	Ymax = newYmax;
	return;
    }
	
    if (!(image = TwReAllocMem(image, n * sizeof(hwattr))))
	panic();
	
    Fill(image + max, HWATTR(Color, ' '), n - max);

    max = n;
    Ymax = newYmax;
}

void addc(byte c) {
    if (X >= Xmax) Xgrow();
    if (Y >= Ymax) Ygrow();
    if (Xreal <= X) Xreal = X + 1;
    if (Yreal <= Y) Yreal = Y + 1;

    image[X + Y * Xmax] = HWATTR(Color, c);
    X++;
}

void finalize(void) {
    uldat i;
    
    if (!image || !Xreal || !Yreal)
	return;
    for (i = 1; i < Yreal; i++)
	TwMoveMem(image + i * Xmax, image + i * Xreal, Xreal * sizeof(hwattr));
}
    
#define goto_xy(x, y) (X = (x), Y = (y))

hwattr *load_ascii_art(FILE *aaFILE, uldat *x, uldat *y, uldat padX, uldat padY) {
    int c;
    
    if (!(image = TwAllocMem(max * sizeof(hwattr))))
	panic();
    
    while ((c = fgetc(aaFILE)) != EOF) {
	switch (State) {
	  case ESnormal:
	    switch ((byte)c) {
	      case 8:
		if (X) X--;
		break;
	      case 9: /* TAB */
		do {
		    addc(' ');
		} while (X & 7);
		break;
	      case 10: case 11: case 12:
		X = 0;
		Y++;
		break;
	      case 24: case 26:
		State = ESnormal;
		break;
	      case 27:
		State = ESesc;
		break;
	      case 127:
		if (X < Xmax && Y < Ymax)
		    image[X + Y * Xmax] = HWATTR(Color, ' ');
		if (X) X--;
		break;
	      case 128+27:
		State = ESsquare;
		break;
	      default:
		addc(c);
		break;
	    }
	    break;
	  
	  case ESesc:
	    switch (c) {
	      case '[':
		State = ESsquare;
		break;
	      default:
		break;
	    }
	    break;
	
	  case ESsquare:
	    Par[0] = nPar = 0;
	    State = ESgetpars;
	    /* FALLTHROUGH */

	  case ESgetpars:
	    if (c==';' && nPar<NPAR-1) {
		Par[++nPar] = 0;
		break;
	    } else if (c>='0' && c<='9') {
		Par[nPar] *= 10;
		Par[nPar] += c-'0';
		break;
	    } else
		State = ESgotpars;
	    /* FALLTHROUGH */
      
	  case ESgotpars:
	    switch (c) {
	      case 'm':
		csi_m();
		break;
	      case 'H': case 'f':
		if (Par[0]) Par[0]--;
		if (!nPar)
		    Par[1] = nPar;
		else if (Par[1]) Par[1]--;
		goto_xy(Par[1],Par[0]);
		break;
	    
	      case 'G': case '`':
		if (Par[0]) Par[0]--;
		goto_xy(Par[0], Y);
		break;

	      case 'A':
		if (!Par[0]) Par[0]++;
		goto_xy(X, Y - Par[0]);
		break;

	      case 'B': case 'e':
		if (!Par[0]) Par[0]++;
		goto_xy(X, Y + Par[0]);
		break;

	      case 'C': case 'a':
		if (!Par[0]) Par[0]++;
		goto_xy(X + Par[0], Y);
		break;

	      case 'D':
		if (!Par[0]) Par[0]++;
		goto_xy(X - Par[0], Y);
		break;

	      case 'E':
		if (!Par[0]) Par[0]++;
		goto_xy(0, Y + Par[0]);
		break;

	      case 'F':
		if (!Par[0]) Par[0]++;
		goto_xy(0, Y - Par[0]);
		break;

	      case 'd':
		if (Par[0]) Par[0]--;
		goto_xy(X, Par[0]);
		break;
		
	      default:
		break;
	    }
	    State = ESnormal;
	    break;
	    
	  default:
	    State = ESnormal;
	    break;
	}
    }
    if (padX || padY) {
	goto_xy(Xreal + padX - 1, Yreal + padY - 1);
	addc(' ');
    }
    finalize();
    *x = Xreal;
    *y = Yreal;
    return image;
}
