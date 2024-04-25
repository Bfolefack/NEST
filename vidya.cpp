#include "vidya.h"
#include "ppu.h"
#include "cpu.h"
#include "src/include/SDL2/SDL.h"

SDL_Window* window = NULL;
SDL_Surface* screenSurface = NULL;

Joypad P1_joypad = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


void init_SDL(){
    if( SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    } else {
        window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if( window == NULL){
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        } else {
            screenSurface = SDL_GetWindowSurface(window);
        }
    }
    SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));
    SDL_UpdateWindowSurface(window);
}

void refresh_window(){
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        if(event.type == SDL_QUIT){
            close_SDL();
            exit(0);
        } else if (event.type == SDL_KEYDOWN) {
            switch(event.key.keysym.sym){
                case SDLK_ESCAPE:
                    close_SDL();
                    exit(0);
                    break;
                case SDLK_w: // UP
                    P1_joypad.UP = 1;
                    break;
                case SDLK_a: // LEFT
                    P1_joypad.LEFT = 1;
                    break;
                case SDLK_s: // DOWN
                    P1_joypad.DOWN = 1;
                    break;
                case SDLK_d: // RIGHT
                    P1_joypad.RIGHT = 1;
                    break;
                case SDLK_LSHIFT: // B
                    P1_joypad.B = 1;
                    break;
                case SDLK_SPACE: // A
                    P1_joypad.A = 1;
                    break;
                case SDLK_RETURN: // START
                    P1_joypad.START = 1;
                    break;
                case SDLK_RSHIFT: // SELECT
                    P1_joypad.SELECT = 1;
                    break;
                case SDLK_DELETE: // RESET
                    cpu.reset();
                    break;
            }
        
        }
    }
}

void draw_window(){
    SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));
    for(int y = 0; y < SCREEN_HEIGHT; y++){
        for(int x = 0; x < SCREEN_WIDTH; x++){
            SDL_Rect rect = {x, y, 1, 1};
            SDL_FillRect(screenSurface, &rect, SDL_MapRGB(screenSurface->format, std::get<0>(image_buffer[y][x]), std::get<1>(image_buffer[y][x]), std::get<2>(image_buffer[y][x])));
        }
    }
    SDL_UpdateWindowSurface(window);
}

void close_SDL(){
    SDL_DestroyWindow(window);
    SDL_Quit();
}