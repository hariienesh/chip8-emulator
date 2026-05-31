#include <string.h>
#include "chip8.h"

void fetch(Chip8 *cpu);

int main(void) {
    Chip8 cpu;
    memset(&cpu, 0, sizeof(cpu));  // zero all memory/regs

    cpu.PC = 0x200;  // CHIP-8 programs start here

    // hand-load two opcodes into RAM
    cpu.mem[0x200] = 0x00; cpu.mem[0x201] = 0xE0;  // 00E0 clear
    cpu.mem[0x202] = 0x12; cpu.mem[0x203] = 0x00;  // 1200 jump→0x200

    // run 3 cycles
    for (int i = 0; i < 3; i++) fetch(&cpu);

    return 0;
}
