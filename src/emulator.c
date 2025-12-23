#include <emulator.h>
#include <stdio.h>
#include <stdlib.h>

const u32 ram_size = 4096; // 4kB

// because of some convention font data is 0x50-0x9F
static void insertFontChar(u8 *ram_start, u32 offset, u8 fontBytes[5]) {
  for (int i = 0; i < 5; i++) {
    *(ram_start + 0x50 + offset + i) = fontBytes[i];
  }
}

int initEmulator(Emulator *emu) {
  emu->ram = malloc(ram_size);
  if (!emu->ram) {
    fprintf(stderr, "Failed allocation for the CHIP-8 RAM\n");
    return -1;
  }

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
  emu->ri = (u16 *)emu->ram + 0x50;
  // Not sure about the stack implementation yet
  emu->delay_timer = 0xFF;
  emu->sound_timer = 0xFF;

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

  return 0;
}
