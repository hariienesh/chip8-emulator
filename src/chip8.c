#include "chip8.h"
#include <stdint.h>
#include <stdio.h>

void execute(Chip8 *cpu, uint16_t op) {
  uint8_t type = (op >> 12) & 0xF;
  uint16_t NNN = op & 0x0FFF;

  switch (type) {
  case 0x0:
    if (op == 0x00E0)
      printf("CLEAR\n");
    break;
  case 0x1:
    cpu->PC = NNN;
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
