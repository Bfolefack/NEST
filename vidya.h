#pragma once


// #include "src/include/SDL2/SDL_image.h"
#include <stdio.h>
#include "src/include/SDL2/SDL.h"

const int SCREEN_WIDTH = 256;
const int SCREEN_HEIGHT = 240;

void init_SDL();
uint8_t** create_image();
void draw_SDL(uint8_t** image);
void close_SDL();
