#ifndef __INPUT_H
#define __INPUT_H

#include <SDL2/SDL.h>
#include <util.h>

typedef struct {
  bool k1, k2, k3, kC;
  bool k4, k5, k6, kD;
  bool k7, k8, k9, kE;
  bool kA, k0, kB, kF;
} InputKeys;

void updateKeys(InputKeys *keys, SDL_Event e);

#endif // !__INPUT_h
