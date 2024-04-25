#pragma once
#include <cstdint>

const int SCREEN_WIDTH = 256;
const int SCREEN_HEIGHT = 240;


typedef struct Joypad {
    uint8_t strobe: 1;
    uint8_t btn_pointer : 4;
    uint8_t _ : 3;
    uint8_t A : 1;
    uint8_t B : 1;
    uint8_t SELECT : 1;
    uint8_t START : 1;
    uint8_t UP : 1;
    uint8_t DOWN : 1;
    uint8_t LEFT : 1;
    uint8_t RIGHT : 1;
} Joypad; 
extern Joypad P1_joypad;
extern Joypad P2_joypad;

void init_SDL();
void refresh_window();
void draw_window();
void close_SDL();
