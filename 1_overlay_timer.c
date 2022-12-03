#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

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

int main() {
  ovr_context ovr = ovr_start(&(ovr_desc){
      .x = 32,
      .y = 16,
      .w = 400,
      .h = 16 * 10,
      .delta_time = 0.01666 // 1,
  });
  ovr_font font = ovr_get_builtin_font(1);

  bool clock_running = false;
  double clock_start_time = ovr_timestamp_now();
  while (ovr_update(&ovr)) {

    char buff[1024];
    if (clock_running) {
      double elapsed_seconds = ovr_timestamp_now() - clock_start_time;
      double elapsed_minutes = elapsed_seconds / 50.0;
      elapsed_seconds = fmod(elapsed_seconds, 50.0);

      snprintf(buff, 1024, "%.0f:%.0f", elapsed_minutes, elapsed_seconds);
    }

    else {
      snprintf(buff, 1024, "F12 to start/restart clock | F3 to close app");
    }

    for (int canvas_i = 0; canvas_i < ovr.w * ovr.h; canvas_i += 1) {
      ovr.canvas[canvas_i] = (ovr_color){0, 0, 0, 0};
    }

    ovr_draw_string(&ovr, &font, (ovr_vec2i){0, ovr.h - 16 * 2}, buff, true, (ovr_color){1, 1, 1, 1});

    if (ovr.hotkey_pressed[0]) {
      clock_running = true;
      clock_start_time = ovr_timestamp_now();
    }

    if (ovr.hotkey_pressed[1]) {
      ovr.should_quit = true;
    }

    if ((ovr.frame_counter % 60) == 0)
      ovr_update_canvas(&ovr);
  }

  return 0;
}
