/*
- add the concept of app update kind, implement game loop sort of thing
- stack of rectangles with coordinates of canvas partial updates

*/

#include <stdbool.h>
#include <stdint.h>

#ifndef mui_panic
#define mui_panic(...)
#endif

#ifndef mui_assert_message
#define mui_assert_message(x, ...)
#endif

#ifndef mui_malloc
#define mui_malloc malloc
#endif

#ifndef mui_free
#define mui_free free
#endif

typedef struct mui_color {
  float r, g, b, a;
} mui_color;

typedef enum mui_kind {
  mui_kind_overlay,
} mui_kind;

typedef enum mui_render_kind {
  mui_render_kind_canvas,
} mui_render_kind;

typedef enum mui_update_kind {
  mui_update_kind_constant_updates,
  mui_update_kind_event_based,
} mui_update_kind;

typedef struct mui_desc {
  mui_kind kind;
  mui_render_kind render_kind;
  mui_update_kind update_kind;
  int x, y, w, h;
} mui_desc;

typedef struct mui_context {
  mui_desc desc;
  mui_color *canvas;
  int x, y, w, h;
  bool should_quit;

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
  uint32_t *canvas;
} mui_x11_context;

static void mui__update_canvas(mui_context *mui) {
  if (mui->desc.render_kind != mui_render_kind_canvas) {
    return;
  }

  int x, y;
  unsigned w, h, border, depth;
  XGetGeometry(mui->x11->display, mui->x11->window, &mui->x11->root, &x, &y, &w, &h, &border, &depth);

  if (mui->x11->image != NULL && mui->w == w && mui->h == h) {
    int y_dst = mui->h - 1;
    for (int y_src = 0; y_src < mui->h; y_src += 1) {
      for (int x_src = 0; x_src < mui->w; x_src += 1) {
        mui_color src = mui->canvas[x_src + y_src * mui->w];
        uint32_t *dst = mui->x11->canvas + (x_src + y_dst * mui->w);

        if (mui->desc.kind == mui_kind_overlay) {
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

    XPutImage(mui->x11->display, mui->x11->window, mui->x11->gc, mui->x11->image, 0, 0, 0, 0, mui->w, mui->h);
  }

  if (mui->w != w || mui->h != h) {
    if (mui->x11->image) {
      XDestroyImage(mui->x11->image);
      mui_free(mui->canvas);
      mui_free(mui->x11->canvas);
    }
  }

  if (mui->x11->image == NULL) {
    mui->canvas = mui_malloc(mui->w * mui->h * sizeof(mui_color));
    mui->x11->canvas = mui_malloc(mui->w * mui->h * 4);
    mui->x11->image = XCreateImage(mui->x11->display, mui->x11->visual, mui->x11->depth, ZPixmap, 0, (char *)mui->x11->canvas, mui->w, mui->h, 32, mui->w * 4);
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

  mui__update_canvas(&mui);
  return mui;
}

bool mui_update(mui_context *mui) {
  mui__update_canvas(mui);

  int should_block = mui->desc.update_kind;
  for (;;) {
    int pending = XPending(mui->x11->display);
    if (pending == 0) {
      if (!should_block)
        break;
      else
        should_block = false;
    }

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
  }

  if (mui->should_quit) {
    XCloseDisplay(mui->x11->display);
  }

  return !mui->should_quit;
}
