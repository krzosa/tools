#include <stdbool.h>
#include <stdint.h>

#ifndef mui_panic
#define mui_panic(...)
#endif

#ifndef mui_assert_message
#define mui_assert_message(x, ...)
#endif

typedef enum mui_kind {
  mui_kind_overlay,
} mui_kind;

typedef enum mui_render_kind {
  mui_render_kind_canvas,
} mui_render_kind;

typedef struct mui_desc {
  mui_kind kind;
  mui_render_kind render_kind;
  int x, y, w, h;
} mui_desc;

typedef struct mui_context {
  mui_desc desc;
  uint32_t *canvas;
  int x, y, w, h;
  int should_quit;

  struct mui_x11_context *x11;
  char buffer[4096];
} mui_context;

//
// Implementation
//

#include <stdlib.h>
#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>

typedef struct mui_x11_context {
  XImage *image;

  Display *display;
  Window root;
  int screen;
  Visual *visual;
  int depth;
  Window window;
  GC gc;
  Atom wm_delete_window;
} mui_x11_context;

void mui__update_canvas(mui_context *mui) {
  if (mui->desc.render_kind != mui_render_kind_canvas) {
    return;
  }

  int x, y;
  unsigned w, h, border, depth;
  XGetGeometry(mui->x11->display, mui->x11->window, &mui->x11->root, &x, &y, &w, &h, &border, &depth);

  if (mui->x11->image != NULL && mui->w == w && mui->h == h) {
    XPutImage(mui->x11->display, mui->x11->window, mui->x11->gc, mui->x11->image, 0, 0, 0, 0, mui->w, mui->h);
  }

  if (mui->w != w || mui->h != h) {
    XDestroyImage(mui->x11->image);
  }

  if (mui->x11->image == NULL) {
    mui->canvas = malloc(mui->w * mui->h * 4);
    mui->x11->image = XCreateImage(mui->x11->display, mui->x11->visual, mui->x11->depth, ZPixmap, 0, (char *)mui->canvas, mui->w, mui->h, 32, mui->w * 4);
  }

  mui->w = w;
  mui->h = h;
  mui->x = x;
  mui->y = y;
}

mui_context mui_start(const mui_desc *desc) {

  mui_context mui = {.desc = *desc, .x = desc->x, .y = desc->y, .w = desc->w, .h = desc->h};
  mui_assert_message(sizeof(mui.buffer) > sizeof(mui_x11_context), "Sizeof platform buffer is too small to fit platform data");
  mui_assert_message(desc->kind == mui_kind_overlay, "invalid kind of mui application selected");

  mui.x11 = (mui_x11_context *)mui.buffer;
  mui.x11->display = XOpenDisplay(NULL); // @cleanup XCloseDisplay
  if (!mui.x11->display)
    mui_panic("Failed to open X display");

  mui.x11->root = DefaultRootWindow(mui.x11->display);
  if (mui.x11->root == None)
    mui_panic("Failed to retrieve the X Root window");

  mui.x11->screen = DefaultScreen(mui.x11->display);

  XVisualInfo vinfo;
  if (!XMatchVisualInfo(mui.x11->display, mui.x11->screen, 32, TrueColor, &vinfo))
    mui_panic("Failed to get 32bit visual, cant do transparency on this system");

  mui.x11->visual = vinfo.visual;
  mui.x11->depth = vinfo.depth;

  XSetWindowAttributes attributes = {
      .colormap = XCreateColormap(mui.x11->display, mui.x11->root, mui.x11->visual, AllocNone),
      .override_redirect = true,
  };

  // @cleanup XUnmapWindow
  mui.x11->window = XCreateWindow(mui.x11->display, mui.x11->root, mui.x, mui.y, mui.w, mui.h, 0, mui.x11->depth,
                                  InputOutput, mui.x11->visual,
                                  CWColormap | CWBorderPixel | CWBackPixel | CWOverrideRedirect, &attributes);
  if (mui.x11->window == None)
    mui_panic("Failed to create X window");

  XserverRegion region = XFixesCreateRegion(mui.x11->display, NULL, 0);
  XFixesSetWindowShapeRegion(mui.x11->display, mui.x11->window, ShapeInput, 0, 0, region);
  XFixesDestroyRegion(mui.x11->display, region);

  XMapWindow(mui.x11->display, mui.x11->window);
  mui.x11->wm_delete_window = XInternAtom(mui.x11->display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(mui.x11->display, mui.x11->window, &mui.x11->wm_delete_window, 1);
  mui.x11->gc = XCreateGC(mui.x11->display, mui.x11->window, 0, 0);

  XGrabKey(mui.x11->display, XKeysymToKeycode(mui.x11->display, XK_Y), ControlMask | ShiftMask, mui.x11->root, False, GrabModeAsync, GrabModeAsync);
  XSelectInput(mui.x11->display, mui.x11->root, KeyPressMask);

  return mui;
}

int mui_update(mui_context *mui) {
  mui__update_canvas(mui);

  XEvent ev;
  XNextEvent(mui->x11->display, &ev);
  switch (ev.type) {
  case ClientMessage:
    if (ev.xclient.data.l[0] == mui->x11->wm_delete_window) {
      mui->should_quit = true;
    }
    break;
  case KeyPress:
    if ((ev.xkey.state & (ShiftMask | ControlMask | Mod1Mask | Mod4Mask)) == (ShiftMask | ControlMask)) {
      printf("Hot key pressed!");

      fflush(stdout);
      // XUngrabKey(display, keycode, modifiers, grab_window);
      // should_quit = true;
    }
  default:
    break;
  }

  if (mui->should_quit) {
    XCloseDisplay(mui->x11->display);
  }

  return !mui->should_quit;
}