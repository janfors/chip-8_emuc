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

static bool push(Emulator *emu, u16 val) {
  if (emu->sp + 1 > 10) {
    fprintf(stderr, "STACK OVERFLOW\n");
    return false;
  }
  emu->stack[++emu->sp] = val;
  return true;
}

static u16 pop(Emulator *emu) {
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
  emu->ri = 0x50;
  emu->delayTimer = 0xFF;
  emu->soundTimer = 0xFF;

  // emu->V0 = 0;
  // emu->V1 = 0;
  // emu->V2 = 0;
  // emu->V3 = 0;
  // emu->V4 = 0;
  // emu->V5 = 0;
  // emu->V6 = 0;
  // emu->V7 = 0;
  // emu->V8 = 0;
  // emu->V9 = 0;
  // emu->VA = 0;
  // emu->VB = 0;
  // emu->VC = 0;
  // emu->VD = 0;
  // emu->VE = 0;
  // emu->VF = 0;

  // emu->shouldRedraw = false;

  emu->sdlTimer = SDL_AddTimer(1000 / 60, updateTimer, (void *)emu);

  loadWAV("beep.wav", &beep);

  return 0;
}

void destroyEmulator(Emulator *emu) {
  free(emu->ram);
  SDL_RemoveTimer(emu->sdlTimer);

  unloadSound(&beep);
}

void loadROM(Emulator *emu, const char *romPath) {
  FILE *romd = fopen(romPath, "r");
  if (!romd) {
    fprintf(stderr, "Could not read ROM with path: %s\n", romPath);
    return;
  }

  emu->pc = 0x200; // start of program data

  fseek(romd, 0, SEEK_END);
  u64 romSize = ftell(romd);
  fseek(romd, 0, SEEK_SET);

  if (fread(emu->ram + emu->pc, sizeof(u8), romSize, romd) != romSize) {
    fprintf(stderr, "Failure when reading ROM: %s\n", romPath);
    return;
  }

  fclose(romd);
}

static void drawSprite(Emulator *emu, u8 x, u8 y, u8 height) {
  u8 *spriteData = &emu->ram[emu->ri];

  x = x % 64;
  y = y % 32;

  for (u8 row = 0; row < height; row++) {
    u8 screenY = (y + row) % 32;

    u64 mask = ((u64)*spriteData) << (64 - 8 - x);

    if (emu->display[y] & mask)
      emu->registers[0xF] = 1;

    emu->display[screenY] ^= mask;

    spriteData++;
  }
}

static void runProgram(Emulator *emu) {
  if (emu->pc == ram_size)
    return;
  const u8 byte1 = emu->ram[emu->pc];
  const u8 byte2 = emu->ram[emu->pc + 1];

  const u8 nib1 = (byte1 & 0xF0) >> 4;
  const u8 nib2 = byte1 & 0xF;
  const u8 nib3 = (byte2 & 0xF0) >> 4;
  const u8 nib4 = byte2 & 0xF;

  printf("%X%X %X%X\n", nib1, nib2, nib3, nib4);

  const u16 opPcAddr = (nib2 << 8) | (nib3 << 4) | nib4;
  const u8 value = (nib3 << 4) | nib4;

  switch (nib1) {
  case 0x0:
    switch (nib2) {
    case 0x0:
      switch (nib3) {
      case 0xE:
        switch (nib4) {
        case 0xE: // 0x00EE return from subroutine
          emu->pc = pop(emu);
          break;
        case 0x0: // 0x00E0 clear screen
          memset(emu->display, 0, sizeof(u64) * 32);
          break;
        }
        break;
      }
      break;
    }
    break;

  case 0x1: // jmp
    emu->pc = opPcAddr;
    printf("Jumping to 0x%X\n", emu->pc);
    return;
  case 0x2: // subroutine call
    push(emu, emu->pc);
    emu->pc = opPcAddr;
    return;
  case 0x6: // set register
    emu->registers[nib2] = value;
    break;
  case 0x7: // add to register
    emu->registers[nib2] += value;
    break;
  case 0xA: // set I register
    emu->ri = opPcAddr;
    break;
  case 0xD:
    u8 x = emu->registers[nib2];
    u8 y = emu->registers[nib3];
    u8 n = nib4;

    drawSprite(emu, x, y, n);
    break;
  }

  emu->pc += 2;
}

void runEmulator(Emulator *emu) { runProgram(emu); }
