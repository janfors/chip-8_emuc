#include <emulator.h>
#include <sound.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// not even gonna comment on how clean this codebase is...
#include <SDL2/SDL.h>

const u32 ram_size = 4096; // 4kB

// because of some convention font data is 0x50-0x9F
static void insertFontChar(u8 *ram_start, u32 offset, u8 fontBytes[5]) {
  for (int i = 0; i < 5; i++) {
    *(ram_start + 0x50 + offset + i) = fontBytes[i];
  }
}

static Sound beep;

static bool push(Emulator *emu, u16 *val) {
  if (emu->sp + 1 > 10) {
    fprintf(stderr, "STACK OVERFLOW\n");
    return false;
  }
  emu->stack[++emu->sp] = val;
  return true;
}

static u16 *pop(Emulator *emu) {
  if (emu->sp == 0)
    return 0; // not sure about the handling of this but eh
  return emu->stack[emu->sp--];
}

// TODO:
// this maybe should be a callback within the renderer
static Uint32 updateTimer(Uint32 interval, void *emuPtr) {
  Emulator *emu = (Emulator *)emuPtr;
  emu->delayTimer--;
  emu->soundTimer--;

  if (emu->soundTimer > 0) {
    playSound(&beep);
  }

  return interval;
}

int initEmulator(Emulator *emu) {
  emu->ram = calloc(ram_size, sizeof(u32));
  if (!emu->ram) {
    fprintf(stderr, "Failed allocation for the CHIP-8 RAM\n");
    return -1;
  }

  memset(emu->display, 0, sizeof(u64) * 32);

  insertFontChar(emu->ram, 0, (u8[5]){0xF0, 0x90, 0x90, 0x90, 0xF0});  // 0
  insertFontChar(emu->ram, 5, (u8[5]){0x20, 0x60, 0x20, 0x20, 0x70});  // 1
  insertFontChar(emu->ram, 10, (u8[5]){0xF0, 0x10, 0xF0, 0x80, 0xF0}); // 2
  insertFontChar(emu->ram, 15, (u8[5]){0xF0, 0x10, 0xF0, 0x10, 0xF0}); // 3
  insertFontChar(emu->ram, 20, (u8[5]){0x90, 0x90, 0xF0, 0x10, 0x10}); // 4
  insertFontChar(emu->ram, 25, (u8[5]){0xF0, 0x80, 0xF0, 0x10, 0xF0}); // 5
  insertFontChar(emu->ram, 30, (u8[5]){0xF0, 0x80, 0xF0, 0x90, 0xF0}); // 6
  insertFontChar(emu->ram, 35, (u8[5]){0xF0, 0x10, 0x20, 0x40, 0x40}); // 7
  insertFontChar(emu->ram, 40, (u8[5]){0xF0, 0x90, 0xF0, 0x90, 0xF0}); // 8
  insertFontChar(emu->ram, 45, (u8[5]){0xF0, 0x90, 0xF0, 0x10, 0xF0}); // 9
  insertFontChar(emu->ram, 50, (u8[5]){0xF0, 0x90, 0xF0, 0x90, 0x90}); // A
  insertFontChar(emu->ram, 55, (u8[5]){0xE0, 0x90, 0xE0, 0x90, 0xE0}); // B
  insertFontChar(emu->ram, 60, (u8[5]){0xF0, 0x80, 0x80, 0x80, 0xF0}); // C
  insertFontChar(emu->ram, 65, (u8[5]){0xE0, 0x90, 0x90, 0x90, 0xE0}); // D
  insertFontChar(emu->ram, 70, (u8[5]){0xF0, 0x80, 0xF0, 0x80, 0xF0}); // E
  insertFontChar(emu->ram, 75, (u8[5]){0xF0, 0x80, 0xF0, 0x80, 0x80}); // F

  emu->pc = 0;
  emu->sp = 0;
  emu->ri = (u16 *)emu->ram + 0x50;
  emu->delayTimer = 0xFF;
  emu->soundTimer = 0xFF;

  emu->V0 = 0;
  emu->V1 = 0;
  emu->V2 = 0;
  emu->V3 = 0;
  emu->V4 = 0;
  emu->V5 = 0;
  emu->V6 = 0;
  emu->V7 = 0;
  emu->V8 = 0;
  emu->V9 = 0;
  emu->VA = 0;
  emu->VB = 0;
  emu->VC = 0;
  emu->VD = 0;
  emu->VE = 0;
  emu->VF = 0;

  emu->shouldRedraw = false;

  emu->sdlTimer = SDL_AddTimer(1000 / 60, updateTimer, (void *)emu);

  loadWAV("beep.wav", &beep);

  return 0;
}

void destroyEmulator(Emulator *emu) {
  free(emu->ram);
  SDL_RemoveTimer(emu->sdlTimer);

  unloadSound(&beep);
}

void runEmulator(Emulator *emu) {
  emu->shouldRedraw = false;
  // only set the shouldRedraw to true when an operation that changes the
  // display occurs?

  emu->shouldRedraw = true;
}
