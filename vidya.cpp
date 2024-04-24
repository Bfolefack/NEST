#include "vidya.h"
#include "ppu.h"
#include "src/include/SDL2/SDL.h"

SDL_Window* window = NULL;
SDL_Surface* screenSurface = NULL;



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