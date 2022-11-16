#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define app_panic(...) app__panic(__LINE__, __FILE__, __VA_ARGS__)
#define app_assert_message(x, ...)                          \
  do {                                                      \
    if (!(x))                                               \
      app_panic("assertion failed = " #x "\n" __VA_ARGS__); \
  } while (0)
#define app_assert(x) app_assert_message(x, "")
void app__panic(int line, char *file, char *msg, ...) {
  fprintf(stderr, "%s %s:%d\n", msg, file, line);

  char cmd[1024];
  snprintf(cmd, sizeof(cmd), "zenity --error --no-wrap --text=\"%s \n%s:%d\"", msg, file, line);
  system(cmd);

  exit(1);
}

#define ovr_assert_message app_assert_message
#define ovr_panic app_panic
#include "overlay.c"
#include "font.c"

typedef struct app_time {
  int seconds;
  int minutes;
  int hours;
} app_time;

bool app_time_decrement(app_time *T) {
  if (T->seconds == 0 && T->minutes == 0 && T->hours == 0) {
    return true;
  }

  T->seconds -= 1;
  if (T->seconds < 0) {
    T->seconds = 59;
    T->minutes -= 1;
    if (T->minutes < 0) {
      T->minutes = 59;
      T->hours -= 1;
    }
  }

  return false;
}

int main() {
  ovr_context ovr = ovr_start(&(ovr_desc){
      .x = 32,
      .y = 16,
      .w = 400,
      .h = 16 * 10,
      .delta_time = 1 // 1,
  });
  app_time time = {
      .minutes = 50,
  };
  ovr_font font = ovr_get_builtin_font(2);

  while (ovr_update(&ovr)) {
    char buff[1024];
    snprintf(buff, 1024, "%02d:%02d:%02d", time.hours, time.minutes, time.seconds);

    for (int canvas_i = 0; canvas_i < ovr.w * ovr.h; canvas_i += 1) {
      ovr.canvas[canvas_i] = (ovr_color){0, 0, 0, 0};
    }

    ovr_draw_string(&ovr, &font, (ovr_vec2i){0, ovr.h - 16 * 2}, buff, true, (ovr_color){1, 1, 1, 1});
    app_time_decrement(&time);

    if (ovr.global1) {
      time.seconds = 0;
      time.hours = 0;
      time.minutes = 50;
    }

    if (ovr.global4) {
      ovr.should_quit = true;
    }
  }

  return 0;
}
