//
// Created by admin on 01/01/2024.
//

#ifndef CHIP8CL_CHIP8KEYBOARD_H
#define CHIP8CL_CHIP8KEYBOARD_H

#include <stdbool.h>
#include <stdint.h>
#include "config.h"

struct chip8_keyboard  {
    bool keyboard[CHIP8_TOTAL_KEYS];
    const char* keyboard_map;
};
void chip8_keyboard_set_map(struct chip8_keyboard* keyboard, const char* map);
int chip8_keyboard_map(struct chip8_keyboard* keyboard, char key);
void chip8_keyboard_down(struct chip8_keyboard* keyboard, int key); //on met le key a true
void chip8_keyboard_up(struct chip8_keyboard* keyboard, int key); //on met le key a false
bool chip8_keyboard_is_down(struct chip8_keyboard* keyboard, int key); //on verifie si la key est true ou false




#endif //CHIP8CL_CHIP8KEYBOARD_H
