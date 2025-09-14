

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>

typedef unsigned char byte;

struct color_info {
  byte width, shift;

  explicit color_info(unsigned long mask) : width(0), shift(0) {
    while (mask != 0 && ((mask & 1) == 0)) {
      mask >>= 1;
      shift++;
    }
    while (mask & 1) {
      mask >>= 1;
      width++;
    }
  }

  void print(FILE *out) const {
    fprintf(out, "width %u, shift %u\n", (unsigned)width, (unsigned)shift);
  }
};

struct rgb_info {
  color_info red, green, blue;

  explicit rgb_info(const XVisualInfo *xvisinfo)
      : red(xvisinfo->red_mask),     /**/
        green(xvisinfo->green_mask), /**/
        blue(xvisinfo->blue_mask) {
  }

  bool is_truecolor(const XVisualInfo *xvisinfo) const {
    unsigned bits_per_component = (unsigned)xvisinfo->bits_per_rgb;

    return (xvisinfo->c_class == TrueColor || xvisinfo->c_class == DirectColor) /**/
           && red.width != 0 && red.width <= bits_per_component                 /**/
           && green.width != 0 && green.width <= bits_per_component             /**/
           && blue.width != 0 && blue.width <= bits_per_component               /**/
           && (xvisinfo->red_mask & xvisinfo->green_mask) == 0                  /**/
           && (xvisinfo->green_mask & xvisinfo->blue_mask) == 0                 /**/
           && (xvisinfo->blue_mask & xvisinfo->red_mask) == 0;                  /**/
  }

  void print(FILE *out) const {
    fputs("red   ", out);
    red.print(out);
    fputs("green ", out);
    green.print(out);
    fputs("blue  ", out);
    blue.print(out);
  }
};

int main(void) {
  Display *xdisplay = XOpenDisplay(NULL);
  XVisualInfo xvistemplate;
  XVisualInfo *xvisinfo = NULL;
  int xscreen = DefaultScreen(xdisplay);
  // int depth = DefaultDepth(xdisplay, xscreen);
  int xvisinfo_n = 0;

  xvistemplate.visualid = XVisualIDFromVisual(DefaultVisual(xdisplay, xscreen));

  xvisinfo = XGetVisualInfo(xdisplay, VisualIDMask, &xvistemplate, &xvisinfo_n);

  if (xvisinfo != NULL && xvisinfo_n > 0) {
    rgb_info info(xvisinfo);
    info.print(stdout);
    fputs(info.is_truecolor(xvisinfo) ? "truecolor\n" : "palette\n", stdout);
  }
}
