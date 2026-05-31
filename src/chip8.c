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
    if (N == 0x0) {
      cpu->V[X] = cpu->V[Y];
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
