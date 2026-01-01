#include <emulator.h>
#include <sound.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// not even gonna comment on how clean this codebase is...
#include <SDL2/SDL.h>

const u32 ram_size = 4096; // 4kB

extern bool InputKeys[16];

static DebugState debugState;

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

  emu->sdlTimer = SDL_AddTimer(1000 / 60, updateTimer, (void *)emu);

  loadWAV("beep.wav", &beep);

  srand(time(NULL));

#ifdef DEBUG
  debugState.stepping = true;
  debugState.breakAddr = 0x0;
#endif

  return 0;
}

void destroyEmulator(Emulator *emu) {
  free(emu->ram);
  SDL_RemoveTimer(emu->sdlTimer);

  unloadSound(&beep);
}

bool loadROM(Emulator *emu, const char *romPath) {
  FILE *romd = fopen(romPath, "r");
  if (!romd) {
    fprintf(stderr, "Could not read ROM with path: %s\n", romPath);
    return false;
  }

  emu->pc = 0x200; // start of program data

  fseek(romd, 0, SEEK_END);
  u64 romSize = ftell(romd);

  if (romSize > ram_size) {
    fprintf(stderr, "WARNING: ROM size exceeds ram size of %d", ram_size);
    fclose(romd);
    return false;
  }

  fseek(romd, 0, SEEK_SET);

  if (fread(emu->ram + emu->pc, sizeof(u8), romSize, romd) != romSize) {
    fprintf(stderr, "Failure when reading ROM: %s\n", romPath);
    // fclose(romd);
    // return false;
  }

  fclose(romd);
  return true;
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

static void step(Emulator *emu) {
  if (emu->pc == ram_size)
    return;
  const u8 byte1 = emu->ram[emu->pc];
  const u8 byte2 = emu->ram[emu->pc + 1];

  const u8 nib1 = (byte1 & 0xF0) >> 4;
  const u8 nib2 = byte1 & 0xF;
  const u8 nib3 = (byte2 & 0xF0) >> 4;
  const u8 nib4 = byte2 & 0xF;

#ifdef DEBUG
  printf("0x%X: %X%X %X%X\n", emu->pc, nib1, nib2, nib3, nib4);
#endif

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
#ifdef DEBUG
    printf(" jumping to 0x%X\n", emu->pc);
#endif
    return;
  case 0x2: // subroutine call
    push(emu, emu->pc);
    emu->pc = opPcAddr;
    return;
  case 0x3: // skip if VX == NN
    if (emu->registers[nib2] == value)
      emu->pc += 2;
    break;
  case 0x4: // skip if VX != NN
    if (emu->registers[nib2] != value)
      emu->pc += 2;
    break;
  case 0x5: // skip if VX == VY
    if (emu->registers[nib2] == emu->registers[nib3])
      emu->pc += 2;
    break;
  case 0x6: // set register
    emu->registers[nib2] = value;
    break;
  case 0x7: // add to register
    emu->registers[nib2] += value;
    break;
  case 0x8: // arithmetic operations
    switch (nib4) {
    case 0x0: // VX = VY
      emu->registers[nib2] = emu->registers[nib3];
      break;
    case 0x1: // binary OR of VX and VY -> VX
      emu->registers[nib2] |= emu->registers[nib3];
      break;
    case 0x2: // binary AND of VX and VY -> VX
      emu->registers[nib2] &= emu->registers[nib3];
      break;
    case 0x3: // logical XOR of VX and VY -> VX
      emu->registers[nib2] ^= emu->registers[nib3];
      break;
    case 0x4: // VY += VX
      emu->registers[nib2] += emu->registers[nib3];
      break;
    case 0x5: // VX -= VY
      emu->registers[0xF] = 0;
      if (emu->registers[nib2] > emu->registers[nib3])
        emu->registers[0xF] = 1;

      emu->registers[nib2] -= emu->registers[nib3];
      break;
    case 0x6: // VX ?= VY -> VX >> 1
      if (emu->shiftCopiesVX)
        emu->registers[nib2] = emu->registers[nib3];

      emu->registers[0xF] = emu->registers[nib2] & 0xFE;
      emu->registers[nib2] = emu->registers[nib2] >> 1;
      break;
    case 0x8: // VX ?= VY -> VX << 1
      if (emu->shiftCopiesVX)
        emu->registers[nib2] = emu->registers[nib3];

      emu->registers[0xF] = emu->registers[nib2] & 0xFE;
      emu->registers[nib2] = emu->registers[nib2] >> 1;
      break;
    case 0x7: // VX = VY - VX
      emu->registers[0xF] = 0;
      if (emu->registers[nib3] > emu->registers[nib2])
        emu->registers[0xF] = 1;

      emu->registers[nib2] = emu->registers[nib3] - emu->registers[nib2];
      break;
    }
    break;
  case 0x9: // skip if VX != VY
    if (emu->registers[nib2] != emu->registers[nib3])
      emu->pc += 2;
    break;
  case 0xA: // set I register
    emu->ri = opPcAddr;
    break;
  case 0xB: // jmp with offset
    if (emu->offsetjmpUsesVX) {
      emu->pc = emu->registers[nib2] + value;
    } else {
      emu->pc = emu->registers[0x0] + opPcAddr;
    }
    return;
  case 0xC:        // VX = rand & NN
    u8 r = rand(); // should be fine ig
    emu->registers[nib2] = r & value;
    break;
  case 0xD: // draw at X and Y with height N
    u8 x = emu->registers[nib2];
    u8 y = emu->registers[nib3];
    u8 n = nib4;

    drawSprite(emu, x, y, n);
    break;
  case 0xE:
    switch (nib3) {
    case 0x9: // skip if key of VX is pressed
      if (InputKeys[emu->registers[nib2]])
        emu->pc += 2;
      break;
    case 0xA: // skip if key of VX is not pressed
      if (!InputKeys[emu->registers[nib2]])
        emu->pc += 2;
      break;
    }
    break;
  case 0xF:
    switch (nib3) {
    case 0x0:
      switch (nib4) {
      case 0x7: // VX = delayTimer
        emu->registers[nib2] = emu->delayTimer;
        break;
      case 0xA: // waits for key if presesd put it in VX
        int key = getAnyKey();
        if (key == -1)
          return;

        emu->registers[nib2] = (u8)key;
        break;
      }
      break;
    case 0x1:
      switch (nib4) {
      case 0x5: // delayTimer = VX
        emu->delayTimer = emu->registers[nib2];
        break;
      case 0x8: // soundTimer = VX
        emu->soundTimer = emu->registers[nib2];
        break;
      case 0xE: // add to index reg
        emu->ri += emu->registers[nib2];
        break;
      }
      break;
    case 0x2:
      switch (nib4) {
      case 0x9: // ri = char corresponding to VX
        emu->ri = 0x50 + emu->registers[nib2] * 5;
        break;
      }
      break;
    case 0x3:
      switch (nib4) {
      case 0x3: // store VX as decimal at ri
        emu->ram[emu->ri] = emu->registers[nib2] / 100;
        emu->ram[emu->ri + 1] = (emu->registers[nib2] / 10) % 10;
        emu->ram[emu->ri + 2] = emu->registers[nib2] % 10;
        break;
      }
      break;
    case 0x5:
      switch (nib4) {
      case 0x5: // store all registers at ri
        for (u16 i = 0; i < 0x10; i++) {
          emu->ram[emu->ri + i] = emu->registers[i];
        }
        break;
      }
      break;
    case 0x6:
      switch (nib4) {
      case 0x5: // load registers from memory at ri
        for (u16 i = 0; i < 0x10; i++) {
          emu->registers[i] = emu->ram[emu->ri + i];
        }
        break;
      }
      break;
    }
  }

  emu->pc += 2;
}

static u16 getStdinAddr() {
  u32 val;

  printf("Enter address -> ");
  if (scanf("0x%x", &val) != 1) {
    printf("Invalid input format should be 0x...\n");
    return ram_size + 1;
  }

  if (val > ram_size) {
    printf("Memory address exceeds ram space of: 0x%X", ram_size);
    return ram_size + 1;
  }

  return (u16)val;
}

bool runEmulator(Emulator *emu) {
#ifdef DEBUG
  if (debugState.stepping) {
    printf("(h)elp -> ");
    char in = getchar();
    getc(stdin);

    switch (in) {
    case 'h':
      printf("(s)tep (r)egisters (m)emory (b)reak (q)uit\n");
      return true;
    case 's':
      break;
    case 'r':
      for (u8 i = 0; i < 0x10; i++) {
        printf("V%d: 0x%X\n", i, emu->registers[i]);
      }
      return true;
    case 'm':
      u16 startAddr = getStdinAddr();
      if (startAddr == ram_size + 1)
        return true;
      for (int i = 0; i < 40; i++) {
        if (i % 10 == 0) {
          printf("\n0x%04X|\t", i + startAddr);
        }

        printf("%02X ", emu->ram[i + startAddr]);
      }
      printf("\n");
      return true;
    case 'b':
      u16 b = getStdinAddr();
      if (b == ram_size + 1)
        return true;
      debugState.breakAddr = b;
      debugState.stepping = false;
      break;
    case 'q':
      return false;
    default:
      return true;
    }
  } else if (emu->pc >= debugState.breakAddr) {
    debugState.stepping = true;
  }
#endif
  step(emu);
  return true;
}
