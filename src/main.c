#include "chip8.h"

void fetch(Chip8 *cpu);

int main() {
    Chip8 cpu = {0};  

    cpu.PC = 0x200;

    cpu.mem[0x200] = 0x8A;
    cpu.mem[0x201] = 0xB4;

    cpu.mem[0x202] = 0x12;
    cpu.mem[0x203] = 0x34;

    fetch(&cpu);
    fetch(&cpu);

    return 0;
}
