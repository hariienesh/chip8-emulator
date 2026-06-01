#include <SDL2/SDL.h>
#include "chip8.h"

int main(void) {
    Chip8 cpu;
    memset(&cpu, 0, sizeof(cpu));
    chip8_init_sdl(&cpu);
    cpu.PC = 0x200;

    // load ROM into mem starting at 0x200
    FILE *f = fopen("rom.ch8", "rb");
    if (!f) {
        printf("Failed to open ROM\n");
        return 1;
    }

    fread(&cpu.mem[0x200], 1, MEM_SIZE - 0x200, f);
    fclose(f);

    SDL_Event e;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&e))
            if (e.type == SDL_QUIT) running = 0;

        fetch(&cpu);      // ~500–700Hz in real impl
        chip8_draw(&cpu);
        SDL_Delay(2);     // ~500Hz approx
    }
    return 0;
}
