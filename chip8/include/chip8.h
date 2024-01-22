//
// Created by admin on 13/12/2023.
//

#ifndef CHIP8CL_CHIP8_H
#define CHIP8CL_CHIP8_H

#include "config.h"
#include <stdint.h>
#include <stddef.h>
#include "chip8ram.h"
#include "chip8registres.h"
#include "chip8stack.h"
#include "chip8keyboard.h"
#include "chip8screen.h"
struct chip8 {
    struct chip8_ram ram;
    struct chip8_stack stack;
    struct chip8_registres registres;
    struct chip8_keyboard keyboard;
    struct chip8_screen screen;
};

void chip8_init(struct chip8* chip8);
void chip8_load(struct chip8* chip8, const char* buf, size_t size);
void chip8_exec(struct chip8* chip8, unsigned short opcode);


#endif //CHIP8CL_CHIP8_H
