#ifndef __EMULATOR_H
#define __EMULATOR_H

#include <util.h>

typedef struct {
  u8 *ram;
  u16 pc;
  u16 *ri;
  u16 *sp;
  u8 delay_timer;
  u8 sound_timer;

  // 64x32 monochrome (each bit is a pixel)
  u64 display[32];

  u8 V0;
  u8 V1;
  u8 V2;
  u8 V3;
  u8 V4;
  u8 V5;
  u8 V6;
  u8 V7;
  u8 V8;
  u8 V9;
  u8 VA;
  u8 VB;
  u8 VC;
  u8 VD;
  u8 VE;
  u8 VF; // flag register
} Emulator;

int initEmulator(Emulator *emu);

#endif // !__EMULATOR_H
