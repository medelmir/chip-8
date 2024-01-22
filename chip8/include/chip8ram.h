#ifndef CHIP8RAM_H
#define CHIP8RAM_H

#include "config.h"
struct chip8_ram {
    char memory[CHIP8_MEMORY_SIZE];
};

void chip8_ram_init(struct chip8_ram *ram,int index , unsigned char val);
unsigned char chip8_ram_get(struct chip8_ram *ram, int index);
void chip8_ram_destroy(struct chip8_ram *ram);
unsigned short chip8_ram_get_short(struct chip8_ram *ram, int index);
#endif