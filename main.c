#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define Panic(...) __Panic(__LINE__, __FILE__, __VA_ARGS__)
#define AssertMessage(x, ...)                           \
  do {                                                  \
    if (!(x))                                           \
      Panic("Assertion failed = " #x "\n" __VA_ARGS__); \
  } while (0)
#define Assert(x) AssertMessage(x, "")
void __Panic(int line, char *file, char *msg, ...) {
  fprintf(stderr, "%s %s:%d\n", msg, file, line);

  char cmd[1024];
  snprintf(cmd, sizeof(cmd), "zenity --error --no-wrap --text=\"%s \n%s:%d\"", msg, file, line);
  system(cmd);

  exit(1);
}

#define mui_assert_message AssertMessage
#define mui_panic Panic
#include "mui.c"
#include "font.c"

int main() {
  mui_context mui = mui_start(&(mui_desc){
      .x = 32,
      .y = 16,
      .w = 400,
      .h = 16 * 10,
  });

  for (int y = 0; y < 50; y += 1) {
    for (int x = 0; x < 50; x += 1) {
      mui.canvas[x + y * mui.w] = (mui_color){1, 1, 1, 1};
    }
  }

  mui_font font = mui_get_builtin_font(2);
  mui_draw_string(&mui, &font, (mui_vec2i){0, 0}, "Testing", true, (mui_color){1, 1, 1, 1});

  while (mui_update(&mui)) {
  }

  return 0;
}
