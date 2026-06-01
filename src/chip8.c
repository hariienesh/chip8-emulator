#include "chip8.h"
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdint.h>
#include <stdio.h>

void chip8_init_sdl(Chip8 *cpu) {
  SDL_Init(SDL_INIT_VIDEO);

  cpu->window =
      SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       640, 320, SDL_WINDOW_SHOWN);

  cpu->renderer = SDL_CreateRenderer(cpu->window, -1, 0);

  cpu->texture = SDL_CreateTexture(cpu->renderer, SDL_PIXELFORMAT_RGBA8888,
                                   SDL_TEXTUREACCESS_STREAMING, 64, 32);
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
    if (op == 0x00E0)
      printf("CLEAR\n");
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

  case 0xF:
    // Modern CHIP-8 behavior: I remains unchanged after FX55/FX65
    switch (NN) {
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

    default:
      break;
    }

    break;

  case 0x1:
    cpu->PC = NNN;
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
