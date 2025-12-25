#include <SDL2/SDL.h>
#include <emulator.h>
#include <renderer.h>
#include <stdio.h>
#include <stdlib.h>
#include <util.h>

const int pixelScale = 16;

int main() {
  Renderer *renderer = malloc(sizeof(Renderer));
  renderer = rendererInit(renderer, "CHIP-8", 64 * pixelScale, 32 * pixelScale,
                          pixelScale);
  if (!renderer) {
    fprintf(stderr, "Failed initializing SDL2");
    return -1;
  }

  Emulator emu;
  if (initEmulator(&emu) != 0) {
    fprintf(stderr, "Emulator initialization failed\n");
    return -1;
  }

  emu.display[0] = 1UL << 63;

  bool running = true;
  while (running) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        running = false;
      }
    }

    drawFromDisplay(renderer, emu.display);
    render(renderer);
  }

  rendererDeinit(renderer);

  return 0;
}
