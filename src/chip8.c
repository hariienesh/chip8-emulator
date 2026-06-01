#include "chip8.h"
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void chip8_init_sdl(Chip8 *cpu) {
  SDL_Init(SDL_INIT_VIDEO);

  cpu->window =
      SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       640, 320, SDL_WINDOW_SHOWN);

  cpu->renderer = SDL_CreateRenderer(cpu->window, -1, 0);

  cpu->texture = SDL_CreateTexture(cpu->renderer, SDL_PIXELFORMAT_RGBA8888,
                                   SDL_TEXTUREACCESS_STREAMING, 64, 32);

  static const uint8_t font[80] = {
      0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
      0x20, 0x60, 0x20, 0x20, 0x70, // 1
      0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
      0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
      0x90, 0x90, 0xF0, 0x10, 0x10, // 4
      0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
      0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
      0xF0, 0x10, 0x20, 0x40, 0x40, // 7
      0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
      0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
      0xF0, 0x90, 0xF0, 0x90, 0x90, // A
      0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
      0xF0, 0x80, 0x80, 0x80, 0xF0, // C
      0xE0, 0x90, 0x90, 0x90, 0xE0, // D
      0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
      0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  };

  memcpy(&cpu->mem[0x000], font, 80);
}

void chip8_draw(Chip8 *cpu) {
  uint32_t pixels[32 * 64]; // RGBA, one per pixel

  for (int y = 0; y < 32; y++) {
    for (int x = 0; x < 64; x++) {
      uint8_t mask = 0x80 >> (x % 8);

      pixels[y * 64 + x] = (cpu->display[y][x / 8] & (0x80 >> (x % 8)))
                               ? 0xFFFFFFFF
                               : 0x000000FF;
    }
  }

  SDL_UpdateTexture(cpu->texture, NULL, pixels, 64 * sizeof(uint32_t));
  SDL_RenderClear(cpu->renderer);
  SDL_RenderCopy(cpu->renderer, cpu->texture, NULL, NULL);
  SDL_RenderPresent(cpu->renderer);
}

void chip8_tick_timers(Chip8 *cpu) {
  if (cpu->delay_timer > 0)
    cpu->delay_timer--;
  if (cpu->sound_timer > 0) {
    // beep here later
    cpu->sound_timer--;
  }
}

int sdl_key_to_chip8(SDL_Scancode sc) {
  switch (sc) {
  case SDL_SCANCODE_1:
    return 0x1;
  case SDL_SCANCODE_2:
    return 0x2;
  case SDL_SCANCODE_3:
    return 0x3;
  case SDL_SCANCODE_4:
    return 0xC;

  case SDL_SCANCODE_Q:
    return 0x4;
  case SDL_SCANCODE_W:
    return 0x5;
  case SDL_SCANCODE_E:
    return 0x6;
  case SDL_SCANCODE_R:
    return 0xD;

  case SDL_SCANCODE_A:
    return 0x7;
  case SDL_SCANCODE_S:
    return 0x8;
  case SDL_SCANCODE_D:
    return 0x9;
  case SDL_SCANCODE_F:
    return 0xE;

  case SDL_SCANCODE_Z:
    return 0xA;
  case SDL_SCANCODE_X:
    return 0x0;
  case SDL_SCANCODE_C:
    return 0xB;
  case SDL_SCANCODE_V:
    return 0xF;

  default:
    return -1;
  }
}

void execute(Chip8 *cpu, uint16_t op) {
  uint8_t type = (op >> 12) & 0xF;
  uint8_t X = (op >> 8) & 0xF;
  uint8_t Y = (op >> 4) & 0xF;
  uint8_t N = op & 0xF;
  uint8_t NN = (op) & 0xFF;
  uint16_t NNN = op & 0xFFF;

  switch (type) {
  case 0x0:
    switch (op) {
    case 0x00EE:
      cpu->PC = cpu->stack[--cpu->SP];
      break;
    }
    break;

  case 0x1:
    cpu->PC = NNN;
    break;

  case 0x2:
    cpu->stack[cpu->SP++] = cpu->PC;
    cpu->PC = NNN;
    break;

  case 0x3:
    if (cpu->V[X] == NN)
      cpu->PC += 2;
    break;

  case 0x4:
    if (cpu->V[X] != NN)
      cpu->PC += 2;
    break;

  case 0x5:
    if (cpu->V[X] == cpu->V[Y])
      cpu->PC += 2;
    break;

  case 0x9:
    if (cpu->V[X] != cpu->V[Y])
      cpu->PC += 2;
    break;

  case 0x6:
    cpu->V[X] = NN;
    break;

  case 0x7:
    cpu->V[X] += NN;
    break;

  case 0x8:
    switch (N) {

    case 0x0:
      cpu->V[X] = cpu->V[Y];
      break;

    case 0x1:
      cpu->V[X] = cpu->V[X] | cpu->V[Y];
      break;

    case 0x2:
      cpu->V[X] = cpu->V[X] & cpu->V[Y];
      break;

    case 0x3:
      cpu->V[X] = cpu->V[X] ^ cpu->V[Y];
      break;

    case 0x4: {
      uint16_t sum = cpu->V[X] + cpu->V[Y];
      cpu->V[0xF] = (sum > 0xFF);
      cpu->V[X] = sum & 0xFF;
    } break;

    case 0x5: {
      uint8_t vx = cpu->V[X];
      uint8_t vy = cpu->V[Y];
      uint8_t vf = (vx >= vy);

      cpu->V[X] = vx - vy;
      cpu->V[0xF] = vf;
    } break;

    case 0x6: {
      uint8_t vx = cpu->V[X];
      uint8_t vf = vx & 0x1;

      cpu->V[X] = vx >> 1;
      cpu->V[0xF] = vf;
    } break;

    case 0x7: {
      uint8_t vx = cpu->V[X];
      uint8_t vy = cpu->V[Y];
      uint8_t vf = (vy >= vx);

      cpu->V[X] = vy - vx;
      cpu->V[0xF] = vf;
    } break;

    case 0xE: {
      uint8_t vx = cpu->V[X];
      uint8_t vf = (vx >> 7) & 1;

      cpu->V[X] = vx << 1;
      cpu->V[0xF] = vf;
    } break;
    }
    break;

  case 0xA:
    cpu->I = NNN;
    break;

  case 0xB:
    cpu->PC = cpu->V[0] + NNN;
    break;

  case 0xD: {
    uint8_t x = cpu->V[X];
    uint8_t y = cpu->V[Y];

    cpu->V[0xF] = 0;

    for (int row = 0; row < N; row++) {
      uint8_t sprite_byte = cpu->mem[cpu->I + row];

      for (int col = 0; col < 8; col++) {
        uint8_t sprite_bit = (sprite_byte >> (7 - col)) & 1;

        int px = (x + col) % 64;
        int py = (y + row) % 32;

        int byte_index = px / 8;
        int bit_index = px % 8;

        uint8_t mask = 0x80 >> bit_index;

        if (sprite_bit) {
          if (cpu->display[py][byte_index] & mask) {
            cpu->V[0xF] = 1;
          }

          cpu->display[py][byte_index] ^= mask;
        }
      }
    }

    break;
  }

  case 0xE:
    switch (NN) {
    case 0x9E:
      if (cpu->keys[cpu->V[X]]) {
        cpu->PC += 2;
      }

      break;
    case 0xA1:
      if (!cpu->keys[cpu->V[X]]) {
        cpu->PC += 2;
      }

      break;
    }

    break;

  case 0xF:
    // Modern CHIP-8 behavior: I remains unchanged after FX55/FX65
    switch (NN) {
    case 0x07:
      cpu->V[X] = cpu->delay_timer;
      break;

    case 0x15:
      cpu->delay_timer = cpu->V[X];
      break;

    case 0x18:
      cpu->sound_timer = cpu->V[X];
      break;

    case 0x33: {
      uint8_t val = cpu->V[X];

      cpu->mem[cpu->I + 0] = val / 100;
      cpu->mem[cpu->I + 1] = (val / 10) % 10;
      cpu->mem[cpu->I + 2] = val % 10;
    } break;

    case 0x55:
      for (int i = 0; i <= X; i++) {
        cpu->mem[cpu->I + i] = cpu->V[i];
      }
      break;
    case 0x65:
      for (int i = 0; i <= X; i++) {
        cpu->V[i] = cpu->mem[cpu->I + i];
      }
      break;

    case 0x0A: {
      int found = 0;

      for (int i = 0; i <= 15; i++) {
        if (cpu->keys[i] == 1) {
          found = 1;
          cpu->V[X] = i;
          break;
        }
      }

      if (found != 1)
        cpu->PC -= 2;

      break;
    }

    case 0x1E:
      cpu->I += cpu->V[X];
      break;

    case 0x29:
      cpu->I = cpu->V[X] * 5;
      break;

    default:
      break;
    }

    break;

  default:
    break;
  }
}

void fetch(Chip8 *cpu) {
  uint8_t high = cpu->mem[cpu->PC];
  uint8_t low = cpu->mem[cpu->PC + 1];

  uint16_t op = ((uint16_t)high << 8) | low;

  cpu->PC += 2;

  printf("opcode: 0x%04X\n", op);

  execute(cpu, op);
}
