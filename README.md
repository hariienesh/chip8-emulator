# chip8-emulator
 
A CHIP-8 emulator written in C.
 
CHIP-8 is an interpreted language from the 1970s, designed for early microcomputers. Emulating it means building the guts of a small computer from scratch — memory, registers, a fetch-decode-execute loop, a stack, timers, display, and input.
 
## Build
 
```bash
gcc main.c chip8.c -o chip8 -lSDL2
```
 
## Run
 
```bash
./chip8 rom.ch8
```
 
## Keyboard
 
```
CHIP-8    →   Keyboard
1 2 3 C       1 2 3 4
4 5 6 D       Q W E R
7 8 9 E       A S D F
A 0 B F       Z X C V
```
 
## Stack
 
- C
- SDL2
## ROMs
 
Any public-domain CHIP-8 ROM works. Test suite: [corax89/chip8-test-rom](https://github.com/corax89/chip8-test-rom)
