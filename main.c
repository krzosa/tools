
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#define APP_Panic(...) APP__Panic(__LINE__, __FILE__, __VA_ARGS__)
#define APP_AssertMessage(x, ...)                           \
  do {                                                      \
    if (!(x))                                               \
      APP_Panic("assertion failed = " #x "\n" __VA_ARGS__); \
  } while (0)
#define APP_Assert(x) APP_AssertMessage(x, "")
void APP__Panic(int line, char *file, char *msg, ...) {
  fprintf(stderr, "%s %s:%d\n", msg, file, line);

  char cmd[1024];
  snprintf(cmd, sizeof(cmd), "zenity --error --no-wrap --text=\"%s \n%s:%d\"", msg, file, line);
  system(cmd);

  exit(1);
}

#include "win.c"

int main(void) {
  WIN_State *win_state = WIN_Init();
  WIN_Window *overlay = WIN_CreateOverlay(win_state, 0, 0, 2880, 50);
  WIN_Window *bottom_overlay = WIN_CreateOverlay(win_state, 100, 1000, 200, 200);

  bool app_is_running = true;
  while (app_is_running) {
    {
      WIN_Canvas *canvas = WIN_GetCanvas(win_state, overlay);
      for (int i = 0; i < canvas->x * canvas->y; i += 1) {
        canvas->pixels[i] = 0x44440000;
      }
      WIN_UpdateCanvas(win_state, overlay);
    }

    {
      WIN_Canvas *canvas = WIN_GetCanvas(win_state, bottom_overlay);
      for (int i = 0; i < canvas->x * canvas->y; i += 1) {
        canvas->pixels[i] = 0x44440000;
      }
      WIN_UpdateCanvas(win_state, bottom_overlay);
    }

    while (XPending(win_state->x11.display)) {
      XEvent ev;
      XNextEvent(win_state->x11.display, &ev);
      switch (ev.type) {
      case ClientMessage:
        if (ev.xclient.data.l[0] == overlay->x11.wm_delete_window) {
          app_is_running = true;
        }
        break;
      }
    }
  }

}
