#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define Panic(...) __Panic(__LINE__, __FILE__, __VA_ARGS__)
#define AssertMessage(x, ...)                     \
  do {                                \
    if (!(x))                         \
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
#include "os.c"

int main() {
  mui_context mui = mui_start(&(mui_desc){
    .w = 800,
    .h = 600,
  });
  while (mui_update(&mui)) {
    for (int i = 0; i < mui.w * mui.h; i += 1) {
      mui.canvas[i] = 0x80808080;
    }    
  }

  return 0;
}