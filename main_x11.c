#include <stdlib.h>
#include <X11/Xlib.h>
#include "vfbwin.h"

#define WIDTH 1024
#define HEIGHT 768


int main(int argc, char *argv[]) {
  Display *display = XOpenDisplay(NULL);
  int screen = XDefaultScreen(display);
  Window root_window = XRootWindow(display, screen);
  Visual *visual = XDefaultVisual(display, screen);
  int depth = XDefaultDepth(display, screen);

	unsigned char *pix = (unsigned char*) malloc(WIDTH * HEIGHT * 4);
	XImage *image = XCreateImage(
      display, visual, depth, ZPixmap, 0,
			(char *) pix,
			WIDTH, HEIGHT, 32, WIDTH * 4);
  volatile int dirty = 0;
  volatile int vsync = 0;
  vfbwin_start(pix, WIDTH, HEIGHT, &dirty, &vsync);

  unsigned long black = XBlackPixel(display, screen);
  unsigned long white = XWhitePixel(display, screen);
  Window window = XCreateSimpleWindow(display, root_window,
                                      100, 100, WIDTH, HEIGHT,
                                      4, black, white);
  XStoreName(display, window, "vfbwin");
  XSelectInput(display, window,
               ExposureMask | StructureNotifyMask |
               KeyPressMask | ButtonPressMask);
  GC gc = XCreateGC(display, window, 0, 0);
  XMapWindow(display, window);

  int done = 0;
  int width = 1;
  int height = 1;
  while (!done) {
    XEvent event;
    XNextEvent(display, &event);
    switch (event.type) {
      case KeyPress:
      case ButtonPress:
        done = 1;
        break;

      case ConfigureNotify:
        width = event.xconfigure.width;
        height = event.xconfigure.height;
        break;

      case Expose:
        if (event.xexpose.count) break;
        XPutImage(display, window, gc, image, 0, 0, 0, 0, width, height);
        break;
    }
  }

	XDestroyImage(image);
  XFreeGC(display, gc);
  XCloseDisplay(display);
}

