#include "vidya.h"

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
}

uint8_t** create_image(){
    uint8_t** image = (uint8_t**) calloc(SCREEN_HEIGHT, sizeof(uint8_t*));
    for(int y = 0; y < SCREEN_HEIGHT; y++){
        image[y] = (uint8_t*) calloc(SCREEN_WIDTH, sizeof(uint8_t));
    }
    return image;
}

void draw_SDL(uint8_t** image){
    SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));
    for(int y = 0; y < SCREEN_HEIGHT; y++){
        for(int x = 0; x < SCREEN_WIDTH; x++){
            uint8_t pixel = image[y][x];
            SDL_Rect rect = {x, y, 1, 1};
            SDL_FillRect(screenSurface, &rect, SDL_MapRGB(screenSurface->format, pixel, pixel, pixel));
        }
    }
    SDL_UpdateWindowSurface(window);

}

void close_SDL(){
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main (int argc, char* args[]){
    printf("Waluigi");
    init_SDL();
    uint8_t** image = create_image();
    for(int y = 0; y < SCREEN_HEIGHT; y++){
        for(int x = 0; x < SCREEN_WIDTH; x++){
            image[y][x] = x;
        }
    }
    draw_SDL(image);
    SDL_Delay(2000);
    close_SDL();
    return 0;
}