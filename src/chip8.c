#include "chip8.h"
#include <stdint.h>
#include <stdio.h>

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
