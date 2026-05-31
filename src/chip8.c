#include "chip8.h"
#include <stdio.h>

void fetch(Chip8 *cpu) {
  uint8_t high = cpu->mem[cpu->PC];
  uint8_t low = cpu->mem[cpu->PC + 1];
  uint16_t op = (high << 8) | low;

  cpu->PC += 2;

  printf("opcode: 0x%04X\n", op);
}
