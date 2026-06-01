#ifndef CHIP8_H
#define CHIP8_H

#include <SDL2/SDL.h>
#include <stdint.h>

#define MEM_SIZE 4096 // 4 KB RAM
#define STACK_SIZE 16

typedef struct {
  uint8_t V[16]; // VO-VF
  uint16_t I;    // address reg
  uint16_t PC;   // program counter
  uint8_t SP;    // stack pointer
  uint16_t stack[STACK_SIZE];
  uint8_t mem[MEM_SIZE];
  uint8_t display[32][8]; // 32 rows, 8 bytes wide (64 bits)
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture; // 64x32 pixel buffer
  uint8_t delay_timer;
  uint8_t sound_timer;
  uint8_t keys[16];
} Chip8;

void chip8_init_sdl(Chip8 *cpu);
void fetch(Chip8 *cpu);
void chip8_draw(Chip8 *cpu);
void chip8_tick_timers(Chip8 *cpu);

#endif
