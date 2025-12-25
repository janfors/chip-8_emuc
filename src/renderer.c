#include <SDL2/SDL.h>
#include <renderer.h>
#include <stdio.h>

Renderer *rendererInit(Renderer *r, const char *name, int width, int height,
                       u32 pixelScale) {
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
  r->width = width;
  r->height = height;

  u32 *pixels = calloc(width * height, sizeof(u32));
  if (!pixels) {
    fprintf(stderr, "Could not allocate space for the pixel array\n");
    SDL_Quit();
    return NULL;
  }

  r->pixelScale = pixelScale;
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
  SDL_UpdateTexture(r->texture, NULL, r->pixels, r->width * sizeof(u32));
  SDL_RenderCopy(r->renderer, r->texture, NULL, NULL);
  SDL_RenderPresent(r->renderer);
}

const u8 COLOR_WHITE = 255;
const u8 COLOR_BLACK = 0;

static void drawPixel(Renderer *r, u32 x0, u32 y0) {
  u32 *base = r->pixels + y0 * r->pixelScale * r->width + x0 * r->pixelScale;

  for (u32 y = 0; y < r->pixelScale; y++) {
    memset(base + y * r->width, COLOR_WHITE, sizeof(u32) * r->pixelScale);
  }
}

void drawFromDisplay(Renderer *r, u64 display[32]) {
  memset(r->pixels, COLOR_BLACK, sizeof(u32) * r->width * r->height);

  for (u32 line = 0; line < 32; line++) {
    for (u32 i = 0; i < 64; i++) {
      if ((display[line] & ((u64)1 << (63 - i))) != 0) {
        drawPixel(r, i, line);
      }
    }
  }
}
