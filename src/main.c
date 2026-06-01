#include "chip8.h"
#include <SDL2/SDL.h>

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage: chip8 <rom path>\n");
    return 1;
  }

  const char *rom_path = argv[1];

  Chip8 cpu;
  memset(&cpu, 0, sizeof(cpu));
  chip8_init_sdl(&cpu);
  cpu.PC = 0x200;

  // load ROM into mem starting at 0x200
  FILE *f = fopen(rom_path, "rb");
  if (!f) {
    printf("Error: could not open ROM at '%s'\n", rom_path);
    return 1;
  }

  fread(&cpu.mem[0x200], 1, MEM_SIZE - 0x200, f);
  fclose(f);

  SDL_Event e;
  int running = 1;

  uint32_t last_timer = SDL_GetTicks();

  while (running) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT)
        running = 0;
      if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
        int key = sdl_key_to_chip8(e.key.keysym.scancode);
        if (key != -1)
          cpu.keys[key] = (e.type == SDL_KEYDOWN) ? 1 : 0;
      }
    }

    fetch(&cpu); // ~500–700Hz in real impl
    chip8_draw(&cpu);
    SDL_Delay(2); // ~500Hz approx

    uint32_t now = SDL_GetTicks();
    if (now - last_timer >= 16) { // ~60Hz
      chip8_tick_timers(&cpu);
      last_timer = now;
    }
  }
  return 0;
}
