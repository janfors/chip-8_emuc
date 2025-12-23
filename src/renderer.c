#include <SDL2/SDL.h>
#include <renderer.h>
#include <stdio.h>

Renderer *rendererInit(Renderer *r, const char *name, int width, int height) {
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    fprintf(stderr, "Could not init SDL2: %s\n", SDL_GetError());
    return NULL;
  }

  SDL_Window *window =
      SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                       width, height, SDL_WINDOW_SHOWN);
  if (!window) {
    fprintf(stderr, "Could not create an SDL2 Window: %s\n", SDL_GetError());
    SDL_Quit();
    return NULL;
  }

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    fprintf(stderr, "Could not create an SDL2 Renderer: %s\n", SDL_GetError());
    SDL_Quit();
    return NULL;
  }

  SDL_Texture *texture =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                        SDL_TEXTUREACCESS_STREAMING, width, height);
  if (!texture) {
    fprintf(stderr, "Could not create an SDL2 Texture: %s\n", SDL_GetError());
    SDL_Quit();
    return NULL;
  }

  r->window = window;
  r->renderer = renderer;
  r->texture = texture;

  u32 *pixels = malloc(sizeof(width * height));
  if (!pixels) {
    fprintf(stderr, "Could not allocate space for the pixel array\n");
    SDL_Quit();
    return NULL;
  }

  r->pixels = pixels;

  return r;
}

void rendererDeinit(Renderer *r) {
  SDL_DestroyTexture(r->texture);
  SDL_DestroyRenderer(r->renderer);
  SDL_DestroyWindow(r->window);

  free(r->pixels);

  SDL_Quit();
}

void render(Renderer *r) {
  SDL_RenderClear(r->renderer);
  // TODO: update texture and copy to renderer
  SDL_RenderPresent(r->renderer);
}
