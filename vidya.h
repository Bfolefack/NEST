#pragma once
#include <cstdint>


const int PIXELS_PER_PIXEL = 3;
const int SCREEN_WIDTH = 256 * PIXELS_PER_PIXEL;
const int SCREEN_HEIGHT = 240 * PIXELS_PER_PIXEL;

typedef struct Buttons {
    uint8_t A : 1;
    uint8_t B : 1;
    uint8_t SELECT : 1;
    uint8_t START : 1;
    uint8_t UP : 1;
    uint8_t DOWN : 1;
    uint8_t LEFT : 1;
    uint8_t RIGHT : 1;
} Buttons;

typedef struct Joypad {
    uint8_t strobe: 1;
    uint8_t btn_pointer : 4;
    uint8_t _ : 3;
    union button_union {
        Buttons buttons;
        uint8_t button_register;
    } input;
} Joypad; 
extern Joypad P1_joypad;
extern Joypad P2_joypad;

typedef struct Envelope {
    uint8_t decay : 4;
    uint8_t loop : 1;
    uint8_t constant : 1;
    uint8_t volume : 4;
    uint8_t start : 1;
    uint8_t divider : 3;
    uint8_t decay_counter : 4;
} Envelope;

typedef struct Sweep {
    uint8_t enable : 1;
    uint8_t period : 3;
    uint8_t negate : 1;
    uint8_t shift : 3;
    uint8_t reload : 1;
    uint8_t divider : 3;
} Sweep;

typedef struct LengthCounter {
    uint8_t halt : 1;
    uint8_t load : 5;
    uint8_t counter : 8;
} length_counter;

typedef struct Pulse {
    uint8_t duty: 2;
    uint16_t timer : 11;
    uint16_t timer_reload : 11;
    Sweep sweep;
    LengthCounter length_counter;
    Envelope envelope;
    uint8_t enable : 1;
} Pulse;

typedef struct Triangle {
    uint8_t linear_counter_reload : 1;
    uint8_t linear_counter_load : 7;
    uint8_t enable : 1;
    uint8_t linear_counter : 7;
    uint16_t timer : 11;
    uint16_t timer_reload : 11;
    LengthCounter length_counter;
} Triangle;

typedef struct Noise {
    uint8_t loop_noise : 1;
    uint8_t noise_bit : 1;
    uint8_t timer_reload : 4;
    uint16_t timer : 11;
    LengthCounter length_counter;
    Envelope envelope;
    uint8_t enable : 1;
} Noise;

typedef struct FrameCounter {
    uint8_t mode : 1;
    uint8_t irq_disable : 1;
    uint8_t step : 3;

} FrameCounter;



extern Pulse pulse1;
extern Pulse pulse2;
extern Triangle triangle;
extern Noise noise;
extern FrameCounter frame_counter;


void init_SDL();
void refresh_window();
void draw_window();
void close_SDL();

void APU_write(uint16_t address, uint8_t data);
uint8_t APU_read();
void apu_cycle();
void frame_clock();
void play_sound();