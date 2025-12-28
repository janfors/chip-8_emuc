#ifndef __EMULATOR_H
#define __EMULATOR_H

#include <SDL2/SDL.h>
#include <input.h>
#include <util.h>

typedef struct {
  u8 *ram;
  u16 pc; // this is not clean really should be a pointer I think
  u16 ri;
  u16 sp;        // not a real stack pointer but hell
  u16 stack[16]; // should be plenty pc addresses
  u8 delayTimer;
  u8 soundTimer;

  // 64x32 monochrome (each bit is a pixel)
  u64 display[32];

  u8 registers[16];

  // bool shouldRedraw;
  SDL_TimerID sdlTimer;

  bool shiftCopiesVX;
  bool offsetjmpUsesVX;
} Emulator;

int initEmulator(Emulator *emu);
void destroyEmulator(Emulator *emu);

void loadROM(Emulator *emu, const char *romPath);

void runEmulator(Emulator *emu);

#endif // !__EMULATOR_H
