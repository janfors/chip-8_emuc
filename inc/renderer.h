#ifndef __RENDERER_H
#define __RENDERER_H

#include <SDL2/SDL.h>
#include <util.h>

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;

  u32 pixelScale;
  u32 *pixels;

  int width;
  int height;
} Renderer;

Renderer *rendererInit(Renderer *r, const char *name, int width, int height,
                       u32 pixelScale);
void rendererDeinit(Renderer *r);

void drawFromDisplay(Renderer *r, u64 display[32]);

void render(Renderer *r);

#endif // !__RENDERER_H
