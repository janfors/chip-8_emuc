#include <input.h>

bool InputKeys[0x10] = {false};

// works for one key at a time not sure about the correctness
void updateKeys(SDL_Event e) {
  SDL_KeyCode keyCode = e.key.keysym.sym;
  bool toggle = e.key.type == SDL_KEYDOWN ? true : false;

  switch (keyCode) {
  case SDLK_1:
    InputKeys[0x1] = toggle;
    break;
  case SDLK_2:
    InputKeys[0x2] = toggle;
    break;
  case SDLK_3:
    InputKeys[0x3] = toggle;
    break;
  case SDLK_4:
    InputKeys[0xC] = toggle;
    break;
  case SDLK_q:
    InputKeys[0x4] = toggle;
    break;
  case SDLK_w:
    InputKeys[0x5] = toggle;
    break;
  case SDLK_e:
    InputKeys[0x6] = toggle;
    break;
  case SDLK_r:
    InputKeys[0xD] = toggle;
    break;
  case SDLK_a:
    InputKeys[0x7] = toggle;
    break;
  case SDLK_s:
    InputKeys[0x8] = toggle;
    break;
  case SDLK_d:
    InputKeys[0x9] = toggle;
    break;
  case SDLK_f:
    InputKeys[0xE] = toggle;
    break;
  case SDLK_z:
    InputKeys[0xA] = toggle;
    break;
  case SDLK_x:
    InputKeys[0x0] = toggle;
    break;
  case SDLK_c:
    InputKeys[0xB] = toggle;
    break;
  case SDLK_v:
    InputKeys[0xF] = toggle;
    break;
  default:
    break;
  }
}

int getAnyKey() {
  for (int i = 0; i < 0x10; i++) {
    if (InputKeys[i])
      return i;
  }

  return -1;
}
