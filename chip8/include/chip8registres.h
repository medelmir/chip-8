#ifndef CHIP8CL_CHIP8REGISTRES_H
#define CHIP8CL_CHIP8REGISTRES_H

#include "config.h"
#include <stdint.h>
struct chip8_registres {
    unsigned char V[CHIP8_NB_REGISTRES];  //8bits -> 1 byte so we can use unsigned char will be easier
    unsigned short I;
    unsigned short pc; //program counter
    unsigned char delay_timer;
    unsigned char sound_timer;
    unsigned char  sp;   //stack pointer
};

#endif //CHIP8CL_CHIP8REGISTRES_H
