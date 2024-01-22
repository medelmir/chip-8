#include "chip8stack.h"
#include "chip8.h"
#include <assert.h>

static void chip8_stack_bounds(struct chip8 *chip8)
{
    assert(chip8->registres.sp < sizeof(chip8->stack.stack));
}

void chip8_stack_push(struct chip8 *chip8, unsigned short val)
{
    chip8->registres.sp += 1;  //on increment le stack pointer
    chip8_stack_bounds(chip8); //verifier si on est pas en dehors du stack
    chip8->stack.stack[chip8->registres.sp] = val;
}
unsigned short chip8_stack_pop(struct chip8 *chip8)
{
    chip8_stack_bounds(chip8); //verifier si on est pas en dehors du stack
    unsigned short val = chip8->stack.stack[chip8->registres.sp]; //on recupere la valeur
    chip8->registres.sp -= 1; //on decremente le stack pointer
    return val; //on retourne la valeur
}