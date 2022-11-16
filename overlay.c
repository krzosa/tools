/*
- add the concept of app update kind, implement game loop sort of thing
- stack of rectangles with coordinates of canvas partial updates

*/

#include <stdbool.h>
#include <stdint.h>

#ifndef ovr_panic
#define ovr_panic(...)
#endif

#ifndef ovr_assert_message
#define ovr_assert_message(x, ...)
#endif

#ifndef ovr_malloc
#define ovr_malloc malloc
#endif

#ifndef ovr_free
#define ovr_free free
#endif

typedef struct ovr_color {
  float r, g, b, a;
} ovr_color;

typedef enum ovr_kind {
  ovr_kind_overlay,
} ovr_kind;

typedef struct ovr_desc {
  ovr_kind kind;
  int x, y, w, h;
  double delta_time;
} ovr_desc;

typedef struct ovr_rect {
  int x, y, w, h;
} ovr_rect;

typedef struct ovr_context {
  ovr_desc desc;
  ovr_color *canvas;
  ovr_rect update_stack[32];
  int update_stack_len;

  int x, y, w, h;
  bool should_quit;

  double app_start_time;
  double frame_start_time;
  double delta_time;

  bool global1;
  bool global2;
  bool global3;
  bool global4;

  struct ovr_x11_context *x11;
  char buffer[4096];
} ovr_context;

//
// Implementation
//

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>

typedef struct ovr_x11_context {
  XImage *image;
  Display *display;
  Window root;
  int screen;
  Visual *visual;
  int depth;
  Window window;
  GC gc;
  Atom wm_delete_window;
  uint32_t *canvas;
} ovr_x11_context;

static double timestamp_now(void) {
  struct timespec tspec;
  clock_gettime(CLOCK_MONOTONIC, &tspec);
  const uint64_t now = ((uint64_t)tspec.tv_sec * 1000000000 + (uint64_t)tspec.tv_nsec);
  return (double)now / 1000000000.0;
}

static void ovr__update_canvas(ovr_context *ovr) {
  int x, y;
  unsigned w, h, border, depth;
  XGetGeometry(ovr->x11->display, ovr->x11->window, &ovr->x11->root, &x, &y, &w, &h, &border, &depth);

  if (ovr->x11->image != NULL && ovr->w == w && ovr->h == h) {
    int y_dst = ovr->h - 1;
    for (int y_src = 0; y_src < ovr->h; y_src += 1) {
      for (int x_src = 0; x_src < ovr->w; x_src += 1) {
        ovr_color src = ovr->canvas[x_src + y_src * ovr->w];
        uint32_t *dst = ovr->x11->canvas + (x_src + y_dst * ovr->w);

        if (ovr->desc.kind == ovr_kind_overlay) {
          // X11 expects premultiplied alpha for transparent windows
          src.r *= src.a;
          src.g *= src.a;
          src.b *= src.a;
        }

        uint32_t a = (uint32_t)(src.a * 255.f) << 24;
        uint32_t r = (uint32_t)(src.r * 255.f) << 16;
        uint32_t g = (uint32_t)(src.g * 255.f) << 8;
        uint32_t b = (uint32_t)(src.g * 255.f);
        *dst = a | r | g | b;
      }
      y_dst -= 1;
    }

    XPutImage(ovr->x11->display, ovr->x11->window, ovr->x11->gc, ovr->x11->image, 0, 0, 0, 0, ovr->w, ovr->h);
  }

  if (ovr->w != w || ovr->h != h) {
    if (ovr->x11->image) {
      XDestroyImage(ovr->x11->image);
      ovr_free(ovr->canvas);
      ovr_free(ovr->x11->canvas);
    }
  }

  if (ovr->x11->image == NULL) {
    ovr->canvas = ovr_malloc(ovr->w * ovr->h * sizeof(ovr_color));
    ovr->x11->canvas = ovr_malloc(ovr->w * ovr->h * 4);
    ovr->x11->image = XCreateImage(ovr->x11->display, ovr->x11->visual, ovr->x11->depth, ZPixmap, 0, (char *)ovr->x11->canvas, ovr->w, ovr->h, 32, ovr->w * 4);
  }

  ovr->w = w;
  ovr->h = h;
  ovr->x = x;
  ovr->y = y;
}

ovr_context ovr_start(const ovr_desc *desc) {
  ovr_context ovr = {
      .desc = *desc,
      .x = desc->x,
      .y = desc->y,
      .w = desc->w,
      .h = desc->h,
      .delta_time = desc->delta_time,
      .app_start_time = timestamp_now(),
  };

  if (ovr.delta_time == 0.0)
    ovr.delta_time = 1.0 / 60.0;

  if (ovr.w == 0)
    ovr.w = 800;

  if (ovr.h == 0)
    ovr.h = 600;

  ovr_assert_message(sizeof(ovr.buffer) > sizeof(ovr_x11_context), "Sizeof platform buffer is too small to fit platform data");
  ovr_assert_message(desc->kind == ovr_kind_overlay, "invalid kind of ovr application selected");

  ovr.x11 = (ovr_x11_context *)ovr.buffer;
  ovr.x11->display = XOpenDisplay(NULL); // @cleanup XCloseDisplay
  if (!ovr.x11->display)
    ovr_panic("Failed to open X display");

  ovr.x11->root = DefaultRootWindow(ovr.x11->display);
  if (ovr.x11->root == None)
    ovr_panic("Failed to retrieve the X Root window");

  ovr.x11->screen = DefaultScreen(ovr.x11->display);

  XVisualInfo vinfo;
  if (!XMatchVisualInfo(ovr.x11->display, ovr.x11->screen, 32, TrueColor, &vinfo))
    ovr_panic("Failed to get 32bit visual, cant do transparency on this system");

  ovr.x11->visual = vinfo.visual;
  ovr.x11->depth = vinfo.depth;

  XSetWindowAttributes attributes = {
      .colormap = XCreateColormap(ovr.x11->display, ovr.x11->root, ovr.x11->visual, AllocNone),
      .override_redirect = true,
  };

  // @cleanup XUnmapWindow
  ovr.x11->window = XCreateWindow(ovr.x11->display, ovr.x11->root, ovr.x, ovr.y, ovr.w, ovr.h, 0, ovr.x11->depth,
                                  InputOutput, ovr.x11->visual,
                                  CWColormap | CWBorderPixel | CWBackPixel | CWOverrideRedirect, &attributes);
  if (ovr.x11->window == None)
    ovr_panic("Failed to create X window");

  XserverRegion region = XFixesCreateRegion(ovr.x11->display, NULL, 0);
  XFixesSetWindowShapeRegion(ovr.x11->display, ovr.x11->window, ShapeInput, 0, 0, region);
  XFixesDestroyRegion(ovr.x11->display, region);

  XMapWindow(ovr.x11->display, ovr.x11->window);

  ovr.x11->wm_delete_window = XInternAtom(ovr.x11->display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(ovr.x11->display, ovr.x11->window, &ovr.x11->wm_delete_window, 1);

  XGrabKey(ovr.x11->display, XKeysymToKeycode(ovr.x11->display, XK_F1), ControlMask | ShiftMask, ovr.x11->root, False, GrabModeAsync, GrabModeAsync);
  XGrabKey(ovr.x11->display, XKeysymToKeycode(ovr.x11->display, XK_F2), ControlMask | ShiftMask, ovr.x11->root, False, GrabModeAsync, GrabModeAsync);
  XGrabKey(ovr.x11->display, XKeysymToKeycode(ovr.x11->display, XK_F3), ControlMask | ShiftMask, ovr.x11->root, False, GrabModeAsync, GrabModeAsync);
  XGrabKey(ovr.x11->display, XKeysymToKeycode(ovr.x11->display, XK_F4), ControlMask | ShiftMask, ovr.x11->root, False, GrabModeAsync, GrabModeAsync);
  XSelectInput(ovr.x11->display, ovr.x11->root, KeyPressMask);

  ovr.x11->gc = XCreateGC(ovr.x11->display, ovr.x11->window, 0, 0);
  ovr__update_canvas(&ovr);
  return ovr;
}

bool ovr_update(ovr_context *ovr) {
  ovr__update_canvas(ovr);

  // First frame frame_start is not initialized
  // update time then is a big number so there is so
  // it doesn't sleep and all is good
  double time_now = timestamp_now();
  double update_time = time_now - ovr->frame_start_time;
  if (update_time < ovr->delta_time) {
    double time_to_sleep = ovr->delta_time - update_time;
    uint64_t time_to_sleep_u = (uint64_t)(time_to_sleep * 1000000);
    if (time_to_sleep_u)
      usleep(time_to_sleep_u);
  }

  do {
    double time_now = timestamp_now();
    update_time = time_now - ovr->frame_start_time;
  } while (update_time < ovr->delta_time);

  ovr->frame_start_time = timestamp_now();

  ovr->global1 = false;
  ovr->global2 = false;
  ovr->global3 = false;
  ovr->global4 = false;

  while (XPending(ovr->x11->display)) {
    XEvent ev;
    XNextEvent(ovr->x11->display, &ev);
    switch (ev.type) {
    case ClientMessage:
      if (ev.xclient.data.l[0] == ovr->x11->wm_delete_window) {
        ovr->should_quit = true;
      }
      break;
    case KeyPress:
      KeySym sym = XLookupKeysym(&ev.xkey, 0);
      if ((ev.xkey.state & (ShiftMask | ControlMask | Mod1Mask | Mod4Mask)) == (ShiftMask | ControlMask)) {
        if (sym == XK_F1) {
          ovr->global1 = true;
        }
        if (sym == XK_F2) {
          ovr->global2 = true;
        }
        if (sym == XK_F3) {
          ovr->global3 = true;
        }
        if (sym == XK_F4) {
          ovr->global4 = true;
        }
      }
    default:
      break;
    }
  }

  if (ovr->should_quit) {
    XCloseDisplay(ovr->x11->display);
  }

  return !ovr->should_quit;
}
