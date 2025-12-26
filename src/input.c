#include <input.h>

// works for one key at a time not sure about the correctness
void updateKeys(InputKeys *keys, SDL_Event e) {
  SDL_KeyCode keyCode = e.key.keysym.sym;
  bool toggle = e.key.type == SDL_KEYDOWN ? true : false;

  switch (keyCode) {
  case SDLK_1:
    keys->k1 = toggle;
    break;
  case SDLK_2:
    keys->k2 = toggle;
    break;
  case SDLK_3:
    keys->k3 = toggle;
    break;
  case SDLK_4:
    keys->kC = toggle;
    break;
  case SDLK_q:
    keys->k4 = toggle;
    break;
  case SDLK_w:
    keys->k5 = toggle;
    break;
  case SDLK_e:
    keys->k6 = toggle;
    break;
  case SDLK_r:
    keys->kD = toggle;
    break;
  case SDLK_a:
    keys->k7 = toggle;
    break;
  case SDLK_s:
    keys->k8 = toggle;
    break;
  case SDLK_d:
    keys->k9 = toggle;
    break;
  case SDLK_f:
    keys->kE = toggle;
    break;
  case SDLK_z:
    keys->kA = toggle;
    break;
  case SDLK_x:
    keys->k0 = toggle;
    break;
  case SDLK_c:
    keys->kB = toggle;
    break;
  case SDLK_v:
    keys->kF = toggle;
    break;
  default:
    break;
  }
}
