#include <SDL2/SDL.h>
#include <renderer.h>
#include <stdio.h>
#include <stdlib.h>
#include <util.h>

const int pixelScale = 16;

int main() {
  Renderer *renderer = malloc(sizeof(Renderer));
  renderer = rendererInit(renderer, "CHIP-8", 64 * pixelScale, 32 * pixelScale);
  if (!renderer) {
    fprintf(stderr, "Failed initializing SDL2");
    return -1;
  }

  render(renderer);
  getc(stdin);
  rendererDeinit(renderer);
}
