#include "SDL2/SDL.h"
#include <assert.h>
#include <stdbool.h>
#include <X11/Xlib.h>

int main(void) {

  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  SDL_EnableScreenSaver();

  SDL_DisplayMode dm;
  SDL_GetCurrentDisplayMode(0, &dm);
  SDL_Window *sdl_window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, (int)(dm.w * 0.8), (int)(dm.h * 0.8), SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_HIDDEN | SDL_WINDOW_BORDERLESS);
  assert(sdl_window);
  SDL_ShowWindow(sdl_window);

  SDL_Surface *sdl_surface = SDL_GetWindowSurface(sdl_window);
  assert(sdl_surface);
  printf("Window surface pixel format = %s\n", SDL_GetPixelFormatName(sdl_surface->format->format));
  if (sdl_surface->format->format != SDL_PIXELFORMAT_RGB888) printf("Unsupported surface format! Visuals might be incorrect!\n");

  bool app_running = true;
  while(app_running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT: {
          app_running = false;
        } break;
      }
    }

    // SDL_SetWindowOpacity(sdl_window, 0.0f);

    sdl_surface = SDL_GetWindowSurface(sdl_window);
    assert(sdl_surface);

    uint32_t *pixels = sdl_surface->pixels;
    for (int y = 0; y < sdl_surface->h; y += 1) {
      for (int x = 0; x < sdl_surface->w; x += 1) {
        pixels[x + y * sdl_surface->w] = 0x00FF0033;
      }
    }


    SDL_UpdateWindowSurface(sdl_window);
  }

  return 0;
}