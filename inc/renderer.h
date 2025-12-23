#ifndef __RENDERER_H
#define __RENDERER_H

#include <SDL2/SDL.h>
#include <util.h>

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;

  u32 *pixels;
} Renderer;

Renderer *rendererInit(Renderer *r, const char *name, int width, int height);
void rendererDeinit(Renderer *r);

void render(Renderer *r);

#endif // !__RENDERER_H
