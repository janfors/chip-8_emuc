#include <SDL2/SDL.h>
#include <emulator.h>
#include <input.h>
#include <renderer.h>
#include <stdio.h>
#include <stdlib.h>
#include <util.h>

const int pixelScale = 16;

void printHelp() {
  printf("Usage: chip-8 <ROM> <options>\nOptions:\n\t-scx to make shift copy "
         "VX\n\t-ojx to enable offset jumping using VX\n");
}

int main(int argc, char **argv) {
  bool scx = false;
  bool ojx = false;

  if (argc < 2 || argc > 4) {
    printHelp();
    return -1;
  }

  for (int i = 2; i < argc; i++) {
    if (strcmp(argv[i], "-scx") == 0)
      scx = true;
    else if (strcmp(argv[i], "-ojx") == 0)
      ojx = true;
    else {
      printHelp();
      return -1;
    }
  }

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
    rendererDeinit(renderer);
    return -1;
  }

  emu.display[0] = (u64)1 << 63;

  if (!loadROM(&emu, argv[1])) {
    rendererDeinit(renderer);
    return -1;
  }

  emu.shiftCopiesVX = scx;
  emu.offsetjmpUsesVX = ojx;

  bool running = true;
  while (running) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      updateKeys(e);

      if (e.type == SDL_QUIT) {
        running = false;
      }
    }

    running = runEmulator(&emu);

    drawFromDisplay(renderer, emu.display);
    render(renderer);
  }

  destroyEmulator(&emu);
  rendererDeinit(renderer);

  return 0;
}
