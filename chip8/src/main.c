#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <windows.h>
#include <time.h>
#include "chip8.h"
#include "chip8keyboard.h"
#include "SDL2/SDL.h"
const char keyboard_map[CHIP8_TOTAL_KEYS] = {
    SDLK_0,
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_4,
    SDLK_5,
    SDLK_6,
    SDLK_7,
    SDLK_8,
    SDLK_9,
    SDLK_a,
    SDLK_b,
    SDLK_c,
    SDLK_d,
    SDLK_e,
    SDLK_f
};
int main(int argc, char *argv[])
{
    if (argc < 2){
        printf("Error: Missing ROM file\n");
        return -1;
    }
    const char* filename = argv[1];
    printf("%s\n", filename);
    FILE* f = fopen(filename, "rb");
    if (f == NULL){
        printf("Error: Can't open file\n");
        return -1;
    }
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    char buffer[fsize];
    int res = fread(buffer, fsize, 1, f);
    if (res != 1){
        printf("Error: Can't read file\n");
        return -1;
    }
    struct chip8 chip8;
    chip8_init(&chip8);
    chip8_load(&chip8, buffer, fsize);
     chip8_keyboard_set_map(&chip8.keyboard, keyboard_map);
    ///////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////
    /*chip8.registres.V[0] = 0x06;
    chip8.registres.V[1] = 0x10;
    chip8_exec(&chip8, 0x8017);
    printf("%x\n", chip8.registres.pc);
    printf("%x\n", chip8.registres.V[0]);*/
    /*chip8_screen_draw_sprite(&chip8.screen, 1, 1, &chip8.ram.memory[0x00], 5);*/

    ///////////////////////////////////////////////////////////////:
    printf ("Essai compilation avec SDL2\n");
    /* Create a window */
      SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow(
        EMULATOR_WINDOW_TITLE,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        CHIP8_WIDTH * CHIP8_WINDOW_MULTIPLIER,
        CHIP8_HEIGHT * CHIP8_WINDOW_MULTIPLIER,
        SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_TEXTUREACCESS_TARGET);
while(1)
{
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT){
                goto end;
            }
        switch(event.type)
        {
            case SDL_KEYDOWN:
            {
                char vkey = event.key.keysym.sym;
                int key =  chip8_keyboard_map(&chip8.keyboard, vkey);
                printf("%x\n", key);
                break;
            }
            case SDL_KEYUP:
            {
                printf("key down\n");
                break;
            }
        }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (int x = 0 ; x < CHIP8_WIDTH ; x++)
        {
            for (int y = 0 ; y < CHIP8_HEIGHT ; y++)
            {
                if (chip8_screen_is_set(&chip8.screen, x, y))
                {
                    SDL_Rect r;
                    r.x = x * CHIP8_WINDOW_MULTIPLIER;
                    r.y = y * CHIP8_WINDOW_MULTIPLIER;
                    r.w = CHIP8_WINDOW_MULTIPLIER;
                    r.h = CHIP8_WINDOW_MULTIPLIER;
                    SDL_RenderFillRect(renderer, &r);
                }
            }
        if (chip8.registres.delay_timer > 0)
        {
            Sleep(100);
            chip8.registres.delay_timer--;
        }
        if (chip8.registres.sound_timer > 0)
        {
            Beep(1300, 100 * chip8.registres.sound_timer);
            chip8.registres.sound_timer = 0;
        }
            }
        SDL_RenderPresent(renderer);
        
    unsigned short opcode = chip8_ram_get_short(&chip8.ram, chip8.registres.pc);
    chip8.registres.pc += 2;  // 2 bytes  opcode
    chip8_exec(&chip8, opcode);
}
end:
    SDL_DestroyWindow(window);
    return 0;
}

