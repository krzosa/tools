#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define base_panic(...) base__panic(__LINE__, __FILE__, __VA_ARGS__)
#define base_assert_message(x, ...)                          \
  do {                                                       \
    if (!(x))                                                \
      base_panic("assertion failed = " #x "\n" __VA_ARGS__); \
  } while (0)
#define base_assert(x) base_assert_message(x, "")
void base__panic(int line, char *file, char *msg, ...) {
  fprintf(stderr, "%s %s:%d\n", msg, file, line);

  char cmd[1024];
  snprintf(cmd, sizeof(cmd), "zenity --error --no-wrap --text=\"%s \n%s:%d\"", msg, file, line);
  system(cmd);

  exit(1);
}

#define mui_assert_message base_assert_message
#define mui_panic base_panic
#include "mui.c"
#include "font.c"

int main() {
  mui_context mui = mui_start(&(mui_desc){
      .x = 32,
      .y = 16,
      .w = 400,
      .h = 16 * 10,
      .update_kind = mui_update_kind_event_based,
  });

  mui_font font = mui_get_builtin_font(2);
  mui_draw_string(&mui, &font, (mui_vec2i){0, mui.h - 16 * 2}, "Testing", true, (mui_color){1, 1, 1, 1});

  while (mui_update(&mui)) {
    mui_font font = mui_get_builtin_font(2);
    mui_draw_string(&mui, &font, (mui_vec2i){0, mui.h - 16 * 2}, "Testing A", true, (mui_color){1, 1, 1, 1});
  }

  return 0;
}
