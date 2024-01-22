#include "chip8ram.h"
#include <assert.h>

static void chip8_ram_bounds(int index){
    assert(index >= 0 && index < CHIP8_MEMORY_SIZE);
}

void chip8_ram_init(struct chip8_ram *ram,int index , unsigned char val)
{
    chip8_ram_bounds(index);
    ram->memory[index] = val;
}
unsigned char chip8_ram_get(struct chip8_ram *ram, int index)
{
    chip8_ram_bounds(index);
    return ram->memory[index];
}
unsigned short chip8_ram_get_short(struct chip8_ram *ram, int index){
    unsigned char byte1 = chip8_ram_get(ram, index);
    unsigned char byte2 = chip8_ram_get(ram, index + 1);
    return (byte1 << 8) | byte2;
}