#include <stdint.h>

typedef struct WIN_Canvas {
  int x, y;
  uint32_t *pixels;
} WIN_Canvas;

// @reference: stolen from sokol_app.h 
typedef enum WIN_KeyCode {
    WIN_KeyCode_INVALID          = 0,
    WIN_KeyCode_SPACE            = 32,
    WIN_KeyCode_APOSTROPHE       = 39,  /* ' */
    WIN_KeyCode_COMMA            = 44,  /* , */
    WIN_KeyCode_MINUS            = 45,  /* - */
    WIN_KeyCode_PERIOD           = 46,  /* . */
    WIN_KeyCode_SLASH            = 47,  /* / */
    WIN_KeyCode_0                = 48,
    WIN_KeyCode_1                = 49,
    WIN_KeyCode_2                = 50,
    WIN_KeyCode_3                = 51,
    WIN_KeyCode_4                = 52,
    WIN_KeyCode_5                = 53,
    WIN_KeyCode_6                = 54,
    WIN_KeyCode_7                = 55,
    WIN_KeyCode_8                = 56,
    WIN_KeyCode_9                = 57,
    WIN_KeyCode_SEMICOLON        = 59,  /* ; */
    WIN_KeyCode_EQUAL            = 61,  /* = */
    WIN_KeyCode_A                = 65,
    WIN_KeyCode_B                = 66,
    WIN_KeyCode_C                = 67,
    WIN_KeyCode_D                = 68,
    WIN_KeyCode_E                = 69,
    WIN_KeyCode_F                = 70,
    WIN_KeyCode_G                = 71,
    WIN_KeyCode_H                = 72,
    WIN_KeyCode_I                = 73,
    WIN_KeyCode_J                = 74,
    WIN_KeyCode_K                = 75,
    WIN_KeyCode_L                = 76,
    WIN_KeyCode_M                = 77,
    WIN_KeyCode_N                = 78,
    WIN_KeyCode_O                = 79,
    WIN_KeyCode_P                = 80,
    WIN_KeyCode_Q                = 81,
    WIN_KeyCode_R                = 82,
    WIN_KeyCode_S                = 83,
    WIN_KeyCode_T                = 84,
    WIN_KeyCode_U                = 85,
    WIN_KeyCode_V                = 86,
    WIN_KeyCode_W                = 87,
    WIN_KeyCode_X                = 88,
    WIN_KeyCode_Y                = 89,
    WIN_KeyCode_Z                = 90,
    WIN_KeyCode_LEFT_BRACKET     = 91,  /* [ */
    WIN_KeyCode_BACKSLASH        = 92,  /* \ */
    WIN_KeyCode_RIGHT_BRACKET    = 93,  /* ] */
    WIN_KeyCode_GRAVE_ACCENT     = 96,  /* ` */
    WIN_KeyCode_WORLD_1          = 161, /* non-US #1 */
    WIN_KeyCode_WORLD_2          = 162, /* non-US #2 */
    WIN_KeyCode_ESCAPE           = 256,
    WIN_KeyCode_ENTER            = 257,
    WIN_KeyCode_TAB              = 258,
    WIN_KeyCode_BACKSPACE        = 259,
    WIN_KeyCode_INSERT           = 260,
    WIN_KeyCode_DELETE           = 261,
    WIN_KeyCode_RIGHT            = 262,
    WIN_KeyCode_LEFT             = 263,
    WIN_KeyCode_DOWN             = 264,
    WIN_KeyCode_UP               = 265,
    WIN_KeyCode_PAGE_UP          = 266,
    WIN_KeyCode_PAGE_DOWN        = 267,
    WIN_KeyCode_HOME             = 268,
    WIN_KeyCode_END              = 269,
    WIN_KeyCode_CAPS_LOCK        = 280,
    WIN_KeyCode_SCROLL_LOCK      = 281,
    WIN_KeyCode_NUM_LOCK         = 282,
    WIN_KeyCode_PRINT_SCREEN     = 283,
    WIN_KeyCode_PAUSE            = 284,
    WIN_KeyCode_F1               = 290,
    WIN_KeyCode_F2               = 291,
    WIN_KeyCode_F3               = 292,
    WIN_KeyCode_F4               = 293,
    WIN_KeyCode_F5               = 294,
    WIN_KeyCode_F6               = 295,
    WIN_KeyCode_F7               = 296,
    WIN_KeyCode_F8               = 297,
    WIN_KeyCode_F9               = 298,
    WIN_KeyCode_F10              = 299,
    WIN_KeyCode_F11              = 300,
    WIN_KeyCode_F12              = 301,
    WIN_KeyCode_F13              = 302,
    WIN_KeyCode_F14              = 303,
    WIN_KeyCode_F15              = 304,
    WIN_KeyCode_F16              = 305,
    WIN_KeyCode_F17              = 306,
    WIN_KeyCode_F18              = 307,
    WIN_KeyCode_F19              = 308,
    WIN_KeyCode_F20              = 309,
    WIN_KeyCode_F21              = 310,
    WIN_KeyCode_F22              = 311,
    WIN_KeyCode_F23              = 312,
    WIN_KeyCode_F24              = 313,
    WIN_KeyCode_F25              = 314,
    WIN_KeyCode_KP_0             = 320,
    WIN_KeyCode_KP_1             = 321,
    WIN_KeyCode_KP_2             = 322,
    WIN_KeyCode_KP_3             = 323,
    WIN_KeyCode_KP_4             = 324,
    WIN_KeyCode_KP_5             = 325,
    WIN_KeyCode_KP_6             = 326,
    WIN_KeyCode_KP_7             = 327,
    WIN_KeyCode_KP_8             = 328,
    WIN_KeyCode_KP_9             = 329,
    WIN_KeyCode_KP_DECIMAL       = 330,
    WIN_KeyCode_KP_DIVIDE        = 331,
    WIN_KeyCode_KP_MULTIPLY      = 332,
    WIN_KeyCode_KP_SUBTRACT      = 333,
    WIN_KeyCode_KP_ADD           = 334,
    WIN_KeyCode_KP_ENTER         = 335,
    WIN_KeyCode_KP_EQUAL         = 336,
    WIN_KeyCode_LEFT_SHIFT       = 340,
    WIN_KeyCode_LEFT_CONTROL     = 341,
    WIN_KeyCode_LEFT_ALT         = 342,
    WIN_KeyCode_LEFT_SUPER       = 343,
    WIN_KeyCode_RIGHT_SHIFT      = 344,
    WIN_KeyCode_RIGHT_CONTROL    = 345,
    WIN_KeyCode_RIGHT_ALT        = 346,
    WIN_KeyCode_RIGHT_SUPER      = 347,
    WIN_KeyCode_MENU             = 348,
} WIN_KeyCode;

#define WIN_AssertMessage APP_AssertMessage
#define WIN_Assert APP_Assert
#define WIN_Panic APP_Panic
#define WIN_Free free
#define WIN_Malloc malloc

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>

typedef struct WIN_State_X11 {
  Display *display;
  Window root;
  int screen;
} WIN_State_X11;

typedef struct WIN_State {
  WIN_State_X11 x11;
} WIN_State;

typedef struct WIN_Window_X11 {
  Window window;
  XVisualInfo vinfo;
  Atom wm_delete_window;
  XImage *canvas_image;
  GC gc;
} WIN_Window_X11;

typedef struct WIN_Window {
  int x, y;
  WIN_Canvas canvas;
  WIN_Window_X11 x11;
} WIN_Window;

#define WIN_AllocStruct(T) (T *)WIN_Alloc(sizeof(T))
void *WIN_Alloc(size_t size) {
  void *p = WIN_Malloc(size);
  if (!p)
    WIN_Panic("Failed to allocate memory on the heap ...");
  memset(p, 0, size);
  return p;
}

WIN_State *WIN_Init(void) {
  WIN_State *s = WIN_AllocStruct(WIN_State);

  s->x11.display = XOpenDisplay(NULL); //@Cleanup close display
  s->x11.root = DefaultRootWindow(s->x11.display);
  s->x11.screen = DefaultScreen(s->x11.display);

  return s;
}

WIN_Canvas *WIN_GetCanvas(WIN_State *s, WIN_Window *window) {
  int x, y;
  unsigned w, h, border, depth;
  XGetGeometry(s->x11.display, window->x11.window, &s->x11.root, &x, &y, &w, &h, &border, &depth);

  if (window->canvas.x != w || window->canvas.y != h) {
    if (window->canvas.pixels) {
      XDestroyImage(window->x11.canvas_image);
      WIN_Free(window->canvas.pixels);
    }
  }

  if (window->x11.canvas_image == NULL) {
    window->canvas.pixels = WIN_Alloc(w * h * 4);
    window->x11.canvas_image = XCreateImage(s->x11.display, window->x11.vinfo.visual, window->x11.vinfo.depth, ZPixmap, 0, (char *)window->canvas.pixels, w, h, 32, w * 4);
  }

  window->canvas.x = w;
  window->canvas.y = h;

  return &window->canvas;
}

// @reference: stolen from sokol_app.h
WIN_KeyCode WIN_X11_TranslateToKeyCode(WIN_State *s, int scancode) {
    int dummy;
    KeySym* keysyms = XGetKeyboardMapping(s->x11.display, scancode, 1, &dummy);
    WIN_Assert(keysyms);
    KeySym keysym = keysyms[0];
    XFree(keysyms);
    switch (keysym) {
        case XK_Escape:         return WIN_KeyCode_ESCAPE;
        case XK_Tab:            return WIN_KeyCode_TAB;
        case XK_Shift_L:        return WIN_KeyCode_LEFT_SHIFT;
        case XK_Shift_R:        return WIN_KeyCode_RIGHT_SHIFT;
        case XK_Control_L:      return WIN_KeyCode_LEFT_CONTROL;
        case XK_Control_R:      return WIN_KeyCode_RIGHT_CONTROL;
        case XK_Meta_L:
        case XK_Alt_L:          return WIN_KeyCode_LEFT_ALT;
        case XK_Mode_switch:    /* Mapped to Alt_R on many keyboards */
        case XK_ISO_Level3_Shift: /* AltGr on at least some machines */
        case XK_Meta_R:
        case XK_Alt_R:          return WIN_KeyCode_RIGHT_ALT;
        case XK_Super_L:        return WIN_KeyCode_LEFT_SUPER;
        case XK_Super_R:        return WIN_KeyCode_RIGHT_SUPER;
        case XK_Menu:           return WIN_KeyCode_MENU;
        case XK_Num_Lock:       return WIN_KeyCode_NUM_LOCK;
        case XK_Caps_Lock:      return WIN_KeyCode_CAPS_LOCK;
        case XK_Print:          return WIN_KeyCode_PRINT_SCREEN;
        case XK_Scroll_Lock:    return WIN_KeyCode_SCROLL_LOCK;
        case XK_Pause:          return WIN_KeyCode_PAUSE;
        case XK_Delete:         return WIN_KeyCode_DELETE;
        case XK_BackSpace:      return WIN_KeyCode_BACKSPACE;
        case XK_Return:         return WIN_KeyCode_ENTER;
        case XK_Home:           return WIN_KeyCode_HOME;
        case XK_End:            return WIN_KeyCode_END;
        case XK_Page_Up:        return WIN_KeyCode_PAGE_UP;
        case XK_Page_Down:      return WIN_KeyCode_PAGE_DOWN;
        case XK_Insert:         return WIN_KeyCode_INSERT;
        case XK_Left:           return WIN_KeyCode_LEFT;
        case XK_Right:          return WIN_KeyCode_RIGHT;
        case XK_Down:           return WIN_KeyCode_DOWN;
        case XK_Up:             return WIN_KeyCode_UP;
        case XK_F1:             return WIN_KeyCode_F1;
        case XK_F2:             return WIN_KeyCode_F2;
        case XK_F3:             return WIN_KeyCode_F3;
        case XK_F4:             return WIN_KeyCode_F4;
        case XK_F5:             return WIN_KeyCode_F5;
        case XK_F6:             return WIN_KeyCode_F6;
        case XK_F7:             return WIN_KeyCode_F7;
        case XK_F8:             return WIN_KeyCode_F8;
        case XK_F9:             return WIN_KeyCode_F9;
        case XK_F10:            return WIN_KeyCode_F10;
        case XK_F11:            return WIN_KeyCode_F11;
        case XK_F12:            return WIN_KeyCode_F12;
        case XK_F13:            return WIN_KeyCode_F13;
        case XK_F14:            return WIN_KeyCode_F14;
        case XK_F15:            return WIN_KeyCode_F15;
        case XK_F16:            return WIN_KeyCode_F16;
        case XK_F17:            return WIN_KeyCode_F17;
        case XK_F18:            return WIN_KeyCode_F18;
        case XK_F19:            return WIN_KeyCode_F19;
        case XK_F20:            return WIN_KeyCode_F20;
        case XK_F21:            return WIN_KeyCode_F21;
        case XK_F22:            return WIN_KeyCode_F22;
        case XK_F23:            return WIN_KeyCode_F23;
        case XK_F24:            return WIN_KeyCode_F24;
        case XK_F25:            return WIN_KeyCode_F25;

        case XK_KP_Divide:      return WIN_KeyCode_KP_DIVIDE;
        case XK_KP_Multiply:    return WIN_KeyCode_KP_MULTIPLY;
        case XK_KP_Subtract:    return WIN_KeyCode_KP_SUBTRACT;
        case XK_KP_Add:         return WIN_KeyCode_KP_ADD;

        case XK_KP_Insert:      return WIN_KeyCode_KP_0;
        case XK_KP_End:         return WIN_KeyCode_KP_1;
        case XK_KP_Down:        return WIN_KeyCode_KP_2;
        case XK_KP_Page_Down:   return WIN_KeyCode_KP_3;
        case XK_KP_Left:        return WIN_KeyCode_KP_4;
        case XK_KP_Begin:       return WIN_KeyCode_KP_5;
        case XK_KP_Right:       return WIN_KeyCode_KP_6;
        case XK_KP_Home:        return WIN_KeyCode_KP_7;
        case XK_KP_Up:          return WIN_KeyCode_KP_8;
        case XK_KP_Page_Up:     return WIN_KeyCode_KP_9;
        case XK_KP_Delete:      return WIN_KeyCode_KP_DECIMAL;
        case XK_KP_Equal:       return WIN_KeyCode_KP_EQUAL;
        case XK_KP_Enter:       return WIN_KeyCode_KP_ENTER;

        case XK_a:              return WIN_KeyCode_A;
        case XK_b:              return WIN_KeyCode_B;
        case XK_c:              return WIN_KeyCode_C;
        case XK_d:              return WIN_KeyCode_D;
        case XK_e:              return WIN_KeyCode_E;
        case XK_f:              return WIN_KeyCode_F;
        case XK_g:              return WIN_KeyCode_G;
        case XK_h:              return WIN_KeyCode_H;
        case XK_i:              return WIN_KeyCode_I;
        case XK_j:              return WIN_KeyCode_J;
        case XK_k:              return WIN_KeyCode_K;
        case XK_l:              return WIN_KeyCode_L;
        case XK_m:              return WIN_KeyCode_M;
        case XK_n:              return WIN_KeyCode_N;
        case XK_o:              return WIN_KeyCode_O;
        case XK_p:              return WIN_KeyCode_P;
        case XK_q:              return WIN_KeyCode_Q;
        case XK_r:              return WIN_KeyCode_R;
        case XK_s:              return WIN_KeyCode_S;
        case XK_t:              return WIN_KeyCode_T;
        case XK_u:              return WIN_KeyCode_U;
        case XK_v:              return WIN_KeyCode_V;
        case XK_w:              return WIN_KeyCode_W;
        case XK_x:              return WIN_KeyCode_X;
        case XK_y:              return WIN_KeyCode_Y;
        case XK_z:              return WIN_KeyCode_Z;
        case XK_1:              return WIN_KeyCode_1;
        case XK_2:              return WIN_KeyCode_2;
        case XK_3:              return WIN_KeyCode_3;
        case XK_4:              return WIN_KeyCode_4;
        case XK_5:              return WIN_KeyCode_5;
        case XK_6:              return WIN_KeyCode_6;
        case XK_7:              return WIN_KeyCode_7;
        case XK_8:              return WIN_KeyCode_8;
        case XK_9:              return WIN_KeyCode_9;
        case XK_0:              return WIN_KeyCode_0;
        case XK_space:          return WIN_KeyCode_SPACE;
        case XK_minus:          return WIN_KeyCode_MINUS;
        case XK_equal:          return WIN_KeyCode_EQUAL;
        case XK_bracketleft:    return WIN_KeyCode_LEFT_BRACKET;
        case XK_bracketright:   return WIN_KeyCode_RIGHT_BRACKET;
        case XK_backslash:      return WIN_KeyCode_BACKSLASH;
        case XK_semicolon:      return WIN_KeyCode_SEMICOLON;
        case XK_apostrophe:     return WIN_KeyCode_APOSTROPHE;
        case XK_grave:          return WIN_KeyCode_GRAVE_ACCENT;
        case XK_comma:          return WIN_KeyCode_COMMA;
        case XK_period:         return WIN_KeyCode_PERIOD;
        case XK_slash:          return WIN_KeyCode_SLASH;
        case XK_less:           return WIN_KeyCode_WORLD_1; /* At least in some layouts... */
        default:                return WIN_KeyCode_INVALID;
    }
}

typedef struct WIN_Event {

} WIN_Event;

bool NextEvent(WIN_State *s, WIN_Event *event) {
  if(XPending(s->x11.display)) {
    XEvent ev;
    XNextEvent(s->x11.display, &ev);
    return true;
  }
  return false;
}

bool NextEventBlocking(WIN_State *s, WIN_Event *event) {
  XEvent ev;
  XNextEvent(s->x11.display, &ev);
  return true;
}

void WIN_UpdateCanvas(WIN_State *s, WIN_Window *window) {
  XPutImage(s->x11.display, window->x11.window, window->x11.gc, window->x11.canvas_image, 0, 0, 0, 0, window->canvas.x, window->canvas.y);
}

void WIN_UpdateCanvasRect(WIN_State *s, WIN_Window *window, int x, int y, int w, int h) {
  XPutImage(s->x11.display, window->x11.window, window->x11.gc, window->x11.canvas_image, x, y, x, y, w, h);
}

WIN_Window *WIN_CreateOverlay(WIN_State *s, int x, int y, int w, int h) {
  WIN_Window *window = WIN_AllocStruct(WIN_Window);
  if (!XMatchVisualInfo(s->x11.display, s->x11.screen, 32, TrueColor, &window->x11.vinfo))
    WIN_Panic("Failed to get 32bit visual, cant do transparency on this system");

  XSetWindowAttributes attributes = {
      .colormap = XCreateColormap(s->x11.display, s->x11.root, window->x11.vinfo.visual, AllocNone),
      .override_redirect = true,
  };

  // @cleanup XUnmapWindow
  window->x11.window = XCreateWindow(s->x11.display, s->x11.root, x, y, w, h, 0,
                                     window->x11.vinfo.depth,
                                     InputOutput, window->x11.vinfo.visual,
                                     CWColormap | CWBorderPixel | CWBackPixel | CWOverrideRedirect, &attributes);
  if (window->x11.window == None)
    WIN_Panic("Failed to create X window");

  XserverRegion region = XFixesCreateRegion(s->x11.display, NULL, 0);
  XFixesSetWindowShapeRegion(s->x11.display, window->x11.window, ShapeInput, 0, 0, region);
  XFixesDestroyRegion(s->x11.display, region);

  XMapWindow(s->x11.display, window->x11.window); // @Cleanup unmap

  window->x11.wm_delete_window = XInternAtom(s->x11.display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(s->x11.display, window->x11.window, &window->x11.wm_delete_window, 1);

  window->x11.gc = XCreateGC(s->x11.display, window->x11.window, 0, 0);

  return window;
}
