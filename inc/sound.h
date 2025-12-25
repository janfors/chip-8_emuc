#ifndef __SOUND_H
#define __SOUND_H

#include <SDL2/SDL.h>
#include <util.h>

typedef struct {
  SDL_AudioSpec wavSpec;
  Uint32 wavLength;
  Uint8 *wavBuffer;

  SDL_AudioDeviceID deviceID;
} Sound;

void loadWAV(const char *path, Sound *sound);
void unloadSound(Sound *sound);

int playSound(Sound *sound);

#endif // !__SOUND_H
