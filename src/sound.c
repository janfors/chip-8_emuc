#include <sound.h>

void loadWAV(const char *path, Sound *sound) {
  SDL_LoadWAV(path, &sound->wavSpec, &sound->wavBuffer, &sound->wavLength);
  sound->deviceID = SDL_OpenAudioDevice(NULL, 0, &sound->wavSpec, NULL, 0);
}

void unloadSound(Sound *sound) {
  SDL_CloseAudioDevice(sound->deviceID);
  SDL_FreeWAV(sound->wavBuffer);
}

int playSound(Sound *sound) {
  int failed =
      SDL_QueueAudio(sound->deviceID, sound->wavBuffer, sound->wavLength);

  if (failed) {
    fprintf(stderr, "Failure to play a sound\n");
    return -1;
  }

  SDL_PauseAudioDevice(sound->deviceID, 0);
  return 0;
}
