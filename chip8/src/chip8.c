//
// Created by admin on 03/01/2024.
//
#include "chip8.h"
#include <memory.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>

const char chip8_default_character_set[] = {
    0xf0, 0x90, 0x90, 0x90, 0xf0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xf0, 0x10, 0xf0, 0x80, 0xf0, // 2
    0xf0, 0x10, 0xf0, 0x10, 0xf0, // 3
    0x90, 0x90, 0xf0, 0x10, 0x10, // 4
    0xf0, 0x80, 0xf0, 0x10, 0xf0, // 5
    0xf0, 0x80, 0xf0, 0x90, 0xf0, // 6
    0xf0, 0x10, 0x20, 0x40, 0x40, // 7
    0xf0, 0x90, 0xf0, 0x90, 0xf0, // 8
    0xf0, 0x90, 0xf0, 0x10, 0xf0, // 9
    0xf0, 0x90, 0xf0, 0x90, 0x90, // A
    0xe0, 0x90, 0xe0, 0x90, 0xe0, // B
    0xf0, 0x80, 0x80, 0x80, 0xf0, // C
    0xe0, 0x90, 0x90, 0x90, 0xe0, // D
    0xf0, 0x80, 0xf0, 0x80, 0xf0, // E
    0xf0, 0x80, 0xf0, 0x80, 0x80  // F
};
void chip8_init(struct chip8* chip8){
    memset(chip8, 0, sizeof(struct chip8));
    memcpy(&chip8->ram.memory, chip8_default_character_set, sizeof(chip8_default_character_set));
    memset(&chip8->screen, false, sizeof(struct chip8_screen));
}

void chip8_load(struct chip8* chip8, const char* buf, size_t size)
{
    assert (size + CHIP8_PROGRAM_LOAD_ADDRESS < CHIP8_MEMORY_SIZE);
    memcpy(&chip8->ram.memory[CHIP8_PROGRAM_LOAD_ADDRESS], buf, size);
    chip8->registres.pc = CHIP8_PROGRAM_LOAD_ADDRESS;
}

static char chip8_wait_for_key_pressed(struct chip8* chip8)
{
    SDL_Event event;
    while(SDL_WaitEvent(&event)){
        if(event.type != SDL_KEYDOWN){
            continue;
    }
    char c = event.key.keysym.sym;
    char chip8_key = chip8_keyboard_map(&chip8->keyboard, c);
    if (chip8_key != -1)
    {
        return chip8_key;
    }
}
return -1;
}


static void chip8_exec_extended_f(struct chip8* chip8, unsigned short opcode)
{
    unsigned char x = (opcode >> 8) & 0x000f;
    switch (opcode & 0x00ff)
    {
        //LD Vx, DT : set Vx = delay timer value
        case 0x07:
            chip8->registres.V[x] = chip8->registres.delay_timer;
        break;
        //LD Vx, K : wait for a key press, store the value of the key in Vx
        case 0x0A:
        {
            char pressed_key = chip8_wait_for_key_pressed(chip8);
            chip8->registres.V[x] = pressed_key;
        }
        break;
        //LD DT, Vx : set delay timer = Vx
        case 0x15:
            chip8->registres.delay_timer = chip8->registres.V[x];
        break;
        //LD ST, Vx : set sound timer = Vx
        case 0x18:
            chip8->registres.sound_timer = chip8->registres.V[x];
        break;
        //ADD I, Vx : set I = I + Vx
        case 0x1E:
            chip8->registres.I += chip8->registres.V[x];
        break;
        //LD F, Vx : set I = location of sprite for digit Vx
        case 0x29:
            chip8->registres.I = chip8->registres.V[x] * CHIP8_DEFAULT_SPRITE_HEIGHT;
        break;
        //LD B, Vx : store BCD representation of Vx in memory locations I, I+1, and I+2
        case 0x33:
        {
            unsigned char hundreds = chip8->registres.V[x] / 100;
            unsigned char tens = (chip8->registres.V[x] / 10) % 10;
            unsigned char units = chip8->registres.V[x] % 10;
            chip8_ram_init(&chip8->ram, chip8->registres.I, hundreds);
            chip8_ram_init(&chip8->ram, chip8->registres.I + 1, tens);
            chip8_ram_init(&chip8->ram, chip8->registres.I + 2, units);
        }
        break;
        //LD [I], Vx : store registers V0 through Vx in memory starting at location I
        case 0x55:
        {
            for (int i = 0 ; i <= x ; i++)
            {
                chip8_ram_init(&chip8->ram, chip8->registres.I + i, chip8->registres.V[i]);
            }
        }
        break;
        //LD Vx, [I] : read registers V0 through Vx from memory starting at location I
        case 0x65:
        {
            for (int i = 0 ; i <= x ; i++)
            {
                chip8->registres.V[i] = chip8_ram_get(&chip8->ram, chip8->registres.I + i);
            }
        }
        break;
    }

}
static void chip8_exec_extended_8(struct chip8* chip8, unsigned short opcode){
    unsigned char x = (opcode >> 8) & 0x000f;
    unsigned char y = (opcode >> 4) & 0x000f;
    unsigned char fbits = opcode & 0x000f;
    unsigned short tmp=0;
    switch (fbits)
    {
    //LD Vx, Vy : set Vx = Vy
    case 0x00:
        chip8->registres.V[x] = chip8->registres.V[y];
    break;
    //OR Vx, Vy : set Vx = Vx OR Vy
    case 0x01:
        chip8->registres.V[x] |= chip8->registres.V[y];
    break;
    //AND Vx, Vy : set Vx = Vx AND Vy
    case 0x02:
        chip8->registres.V[x] &= chip8->registres.V[y];
    break;
    //XOR Vx, Vy : set Vx = Vx XOR Vy
    case 0x03:
        chip8->registres.V[x] ^= chip8->registres.V[y];
    break;
    //ADD Vx, Vy : set Vx = Vx + Vy, set VF = carry
    case 0x04:
        tmp = chip8->registres.V[x] + chip8->registres.V[y];
        chip8->registres.V[0x0f] = false;
        if (tmp > 0xff){
            chip8->registres.V[0x0f] = true;
        }
        chip8->registres.V[x] = tmp;
        break;
    //SUB Vx, Vy : set Vx = Vx - Vy, set VF = NOT borrow
    case 0x05:
        chip8->registres.V[0x0f] = false;
        if (chip8->registres.V[x] > chip8->registres.V[y]){
            chip8->registres.V[0x0f] = true;
        }
        chip8->registres.V[x] -= chip8->registres.V[y];
    break;
    //SHR Vx {, Vy} : set Vx = Vx SHR 1
    case 0x06:

        chip8->registres.V[0x0f] = chip8->registres.V[x] & 0x01;
        chip8->registres.V[x] /= 2;
    break;
    //SUBN Vx, Vy : set Vx = Vy - Vx, set VF = NOT borrow
    case 0x07:
        chip8->registres.V[0x0f] = false;
        if (chip8->registres.V[y] > chip8->registres.V[x]){
            chip8->registres.V[0x0f] = true;
        }
        chip8->registres.V[x] = chip8->registres.V[y] - chip8->registres.V[x];
    break;
    //SHL Vx {, Vy} : set Vx = Vx SHL 1
    case 0x0E:
        chip8->registres.V[0x0f] = chip8->registres.V[x] & 0x80;
        chip8->registres.V[x] *= 2;
    break;
    }
}
static void chip8_exec_extended(struct chip8* chip8, unsigned short opcode){
    unsigned short nnn = opcode & 0x0fff;
    unsigned char x = (opcode >> 8) & 0x000f;
    unsigned char kk = opcode  & 0x00ff;
    unsigned char y = (opcode >> 4) & 0x000f;
    unsigned char n = opcode & 0x000f;
    const char* sprite;
    switch (opcode & 0xf000)
    {
    //JP addr : jump to location
    case 0x1000:
        chip8->registres.pc = nnn;
    break;
    //CALL addr : call subroutine
    case 0x2000:
        chip8_stack_push(chip8, chip8->registres.pc);
        chip8->registres.pc = nnn;
    break;
    //SE Vx, byte : skip next instruction if Vx = byte
    case 0x3000:
        if(chip8->registres.V[x] == kk){
            chip8->registres.pc += 2;
        }
    break;
    //SNE Vx, byte : skip next instruction if Vx != byte
    case 0x4000:
        if(chip8->registres.V[x] != kk){
            chip8->registres.pc += 2;
        }
    break;
    //SE Vx, Vy : skip next instruction if Vx = Vy
    case 0x5000:
        if(chip8->registres.V[x] == chip8->registres.V[y]){
            chip8->registres.pc += 2;
        }
    break;
    //LD Vx, byte : set Vx = byte
    case 0x6000:
        chip8->registres.V[x] = kk;
    break;
    //ADD Vx, byte : set Vx = Vx + byte
    case 0x7000:
        chip8->registres.V[x] += kk;
    break;
    case 0x8000:
        chip8_exec_extended_8(chip8, opcode);
    break;
    //SNE Vx, Vy : skip next instruction if Vx != Vy
    case 0x9000:
        if(chip8->registres.V[x] != chip8->registres.V[y]){
            chip8->registres.pc += 2;
        }
    break;
    //LD I, addr : set I = addr
    case 0xA000:
        chip8->registres.I = nnn;
    break;
    //JP V0, addr : jump to location V0 + addr
    case 0xB000:
        chip8->registres.pc = chip8->registres.V[0] + nnn;
    break;
    //RND Vx, byte : set Vx = random byte AND byte
    case 0xC000:
        srand(clock());
        chip8->registres.V[x] = (rand() % 255) & kk;
    break;
    //DRW Vx, Vy, nibble : display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
    case 0xD000:
        sprite  = (const char*)&chip8->ram.memory[chip8->registres.I];
        chip8->registres.V[0x0f]=chip8_screen_draw_sprite(&chip8->screen, chip8->registres.V[x], chip8->registres.V[y], sprite, n);
    break;
    //SKP Vx : skip next instruction if key with the value of Vx is pressed
    case 0xE000:
        switch (opcode & 0x00ff)
        {
        case 0x009E:
            if(chip8_keyboard_is_down(&chip8->keyboard, chip8->registres.V[x])){
                chip8->registres.pc += 2;
            }
        break;
        //SKNP Vx : skip next instruction if key with the value of Vx is not pressed
        case 0x00A1:
            if(!chip8_keyboard_is_down(&chip8->keyboard, chip8->registres.V[x])){
                chip8->registres.pc += 2;
            }
        break;
        }
    break;
    //LD Vx, DT : set Vx = delay timer value
    case 0xF000:
        chip8_exec_extended_f(chip8, opcode);
    break;
    }
}
void chip8_exec(struct chip8* chip8, unsigned short opcode){
    switch(opcode){
        //CLS : clear screen
        case 0x00E0:
            chip8_screen_clear(&chip8->screen);
        break;
        //RET : return from subroutine
        case 0x00EE:
            chip8->registres.pc= chip8_stack_pop(chip8);
        break;
        default:
            chip8_exec_extended(chip8, opcode);
    };
}
