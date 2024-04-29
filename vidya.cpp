#include "vidya.h"
#include "ppu.h"
#include "cpu.h"
#include "system_vars.h"
#include "src/include/SDL2/SDL.h"
#include "src/include/SDL2/SDL_audio.h"
#include "stdint.h"
#include <time.h>
#include <chrono>

#define SAMPLE_COUNT 16384
#define TARGET_FREQUENCY 357954
#define WAVEFORM_SIZE 1230

SDL_Window *window = NULL;
SDL_Surface *screenSurface = NULL;
SDL_Window *wave_window = NULL;
SDL_Surface *wave_screenSurface = NULL;

Joypad P1_joypad = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
Pulse pulse1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
Pulse pulse2 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
Triangle triangle = {0, 0, 0, 0, 0, 0, 0, 0};
Noise noise = {0, 0, 0, 0, 0, 0, 0, 0, 0};
FrameCounter frame_counter = {0, 0, 0};
const uint8_t length_counter_lookup[32] = {10, 254, 20, 2, 40, 4, 80, 6, 160, 8, 60, 10, 14, 12, 26, 14, 12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30};
const uint16_t noise_lookup[16] = {2, 4, 8, 16, 32, 48, 64, 80, 101, 127, 190, 254, 381, 508, 1016, 2034};
SDL_AudioSpec want, have;
SDL_AudioDeviceID dev;

uint8_t *pulse1_waveform;
uint8_t *pulse2_waveform;
uint8_t *triangle_waveform;
uint8_t *noise_waveform;

void init_SDL()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    }
    else
    {
        window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        wave_window = SDL_CreateWindow("Waveform", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WAVEFORM_SIZE, 500, SDL_WINDOW_SHOWN);
        if (window == NULL)
        {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        }
        else
        {
            screenSurface = SDL_GetWindowSurface(window);
            wave_screenSurface = SDL_GetWindowSurface(wave_window);
        }
    }

    SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));
    SDL_FillRect(wave_screenSurface, NULL, SDL_MapRGB(wave_screenSurface->format, 0x00, 0x00, 0x00));
    SDL_UpdateWindowSurface(window);
    SDL_UpdateWindowSurface(wave_window);
    srand(time(NULL));

    pulse1_waveform = (uint8_t *)calloc(WAVEFORM_SIZE, sizeof(uint8_t));
    pulse2_waveform = (uint8_t *)calloc(WAVEFORM_SIZE, sizeof(uint8_t));
    triangle_waveform = (uint8_t *)calloc(WAVEFORM_SIZE, sizeof(uint8_t));
    noise_waveform = (uint8_t *)calloc(WAVEFORM_SIZE, sizeof(uint8_t));

    // Set up the audio stream
    SDL_zero(want);
    want.freq = TARGET_FREQUENCY; // or whatever your sample rate is
    want.format = AUDIO_S16SYS;
    want.channels = 1;           // 1 for mono, 2 for stereo
    want.samples = SAMPLE_COUNT; // a good value for games, adjust as necessary
    want.callback = NULL;        // we're using SDL_QueueAudio, not a callback
    dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);

    printf("Opened audio device with %d channels\n", have.channels);
    printf("Opened audio device with frequency %d\n", have.freq);
    printf("Opened audio device with samples %d\n", have.samples);
    printf("Opened audio device with format %d\n", have.format);
    if (dev == 0)
    {
        printf("Failed to open audio: %s\n", SDL_GetError());
    }
}

void refresh_window()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            close_SDL();
            exit(0);
        }
        else if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                close_SDL();
                exit(0);
                break;
            case SDLK_UP:
            case SDLK_w: // UP
                P1_joypad.input.buttons.UP = 1;
                break;
            case SDLK_LEFT:
            case SDLK_a: // LEFT
                P1_joypad.input.buttons.LEFT = 1;
                break;
            case SDLK_DOWN:
            case SDLK_s: // DOWN
                P1_joypad.input.buttons.DOWN = 1;
                break;
            case SDLK_RIGHT:
            case SDLK_d: // RIGHT
                P1_joypad.input.buttons.RIGHT = 1;
                break;
            case SDLK_z:
            case SDLK_LSHIFT: // B
                P1_joypad.input.buttons.B = 1;
                break;
            case SDLK_x:
            case SDLK_SPACE: // A
                P1_joypad.input.buttons.A = 1;
                break;
            case SDLK_RETURN: // START
                P1_joypad.input.buttons.START = 1;
                break;
            case SDLK_RSHIFT: // SELECT
                P1_joypad.input.buttons.SELECT = 1;
                break;
            case SDLK_DELETE: // RESET
                cpu.reset();
                break;
            }
        }
        else if (event.type == SDL_KEYUP)
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_UP:
            case SDLK_w: // UP
                P1_joypad.input.buttons.UP = 0;
                break;
            case SDLK_LEFT:
            case SDLK_a: // LEFT
                P1_joypad.input.buttons.LEFT = 0;
                break;
            case SDLK_DOWN:
            case SDLK_s: // DOWN
                P1_joypad.input.buttons.DOWN = 0;
                break;
            case SDLK_RIGHT:
            case SDLK_d: // RIGHT
                P1_joypad.input.buttons.RIGHT = 0;
                break;
            case SDLK_z:
            case SDLK_LSHIFT: // B
                P1_joypad.input.buttons.B = 0;
                break;
            case SDLK_x:
            case SDLK_SPACE: // A
                P1_joypad.input.buttons.A = 0;
                break;
            case SDLK_RETURN: // START
                P1_joypad.input.buttons.START = 0;
                break;
            case SDLK_RSHIFT: // SELECT
                P1_joypad.input.buttons.SELECT = 0;
                break;
            }
        }
    }
}

void draw_window()
{
    static uint64_t time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));
    for (int y = 0; y < SCREEN_HEIGHT; y++)
    {
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            SDL_Rect rect = {x * PIXELS_PER_PIXEL, y * PIXELS_PER_PIXEL, PIXELS_PER_PIXEL, PIXELS_PER_PIXEL};
            SDL_FillRect(screenSurface, &rect, SDL_MapRGB(screenSurface->format, std::get<0>(image_buffer[y][x]), std::get<1>(image_buffer[y][x]), std::get<2>(image_buffer[y][x])));
        }
    }

    uint64_t new_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    uint64_t delta = new_time - time;
    time = new_time;
    if (delta < 16666)
    {
        SDL_Delay((16666 - delta) / 1000);
    }
    SDL_UpdateWindowSurface(window);
}

void close_SDL()
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void update_length_counter(LengthCounter &length_counter)
{ // Clocked by frame counter
    if (!length_counter.halt && length_counter.counter > 0)
    {
        length_counter.counter--;
    }
}

void update_sweep(Sweep &sweep)
{ // Updates on half-frame clock
    if (sweep.divider == 0 | sweep.reload)
    {
        sweep.reload = 0;
        sweep.divider = sweep.period;
    }
    else
    {
        sweep.divider--;
    }
}

void update_envelope(Envelope &envelope)
{ // Clocked by frame counter
    if (envelope.start)
    {
        envelope.decay_counter = 15;
        envelope.divider = envelope.volume;
        envelope.start = 0;
    }
    else
    {
        if (envelope.divider == 0)
        {
            envelope.divider = envelope.volume;
            if (envelope.decay_counter == 0)
            {
                if (envelope.loop)
                {
                    envelope.decay_counter = 15;
                }
            }
            else
            {
                envelope.decay_counter--;
            }
        }
        else
        {
            envelope.divider--;
        }
    }
}

void update_pulse(Pulse &pulse)
{
    if (pulse.sweep.enable)
    {
        uint16_t target_period = pulse.timer >> pulse.sweep.shift;
        if (pulse.sweep.negate)
        {
            if (pulse.timer > target_period)
            {
                pulse.timer -= ((pulse.timer >> pulse.sweep.shift) + (&pulse == &pulse1));
            }
            else
            {
                pulse.timer = 0;
            }
        }
        else
        {
            pulse.timer += (pulse.timer >> pulse.sweep.shift);
        }
    }
    if (pulse.timer > 0)
    {
        pulse.timer--;
    }
    else
    {
        pulse.timer = pulse.timer_reload;
    }
}

uint8_t process_pulse(Pulse pulse)
{
    static uint8_t duty_lookup[4] = {
        0b01000000,
        0b01100000,
        0b01111000,
        0b10011111};
    uint8_t volume = pulse.envelope.constant ? pulse.envelope.volume : pulse.envelope.decay_counter;
    // TODO: Allow sweep to mute pulse
    uint8_t wave = (duty_lookup[pulse.duty] >> (pulse.timer & 0b111)) & 1;
    wave = pulse.length_counter.counter > 0 && wave && pulse.timer > 7;
    return wave * volume;
}

void update_triangle(Triangle &triangle)
{
    if (triangle.linear_counter_reload)
    {
        triangle.linear_counter = triangle.linear_counter_load;
    }
    else if (triangle.linear_counter > 0)
    {
        triangle.linear_counter--;
    }
    if (!triangle.length_counter.halt)
    {
        triangle.linear_counter_reload = 0;
    }
    if (triangle.length_counter.counter > 0 && triangle.linear_counter > 0)
    {
        if (triangle.timer == 0)
        {
            triangle.timer = triangle.timer_reload;
        }
        else
        {
            triangle.timer--;
        }
    }
}

uint8_t process_triangle(Triangle triangle)
{
    static uint8_t sequencer[32] = {
        15, 14, 13, 12, 11, 10, 9, 8,
        7, 6, 5, 4, 3, 2, 1, 0,
        0, 1, 2, 3, 4, 5, 6, 7,
        8, 9, 10, 11, 12, 13, 14, 15};
    uint8_t volume = triangle.length_counter.counter > 0 && triangle.linear_counter > 0 && triangle.enable ? sequencer[triangle.timer & 0b11111] : 0;
    return volume;
}

void update_noise(Noise &noise)
{
    if (noise.length_counter.counter > 0)
    {
        if (noise.timer == 0)
        {
            noise.timer = noise_lookup[noise.timer_reload];
            noise.noise_bit = rand() & 1;
        }
        else
        {
            noise.timer--;
        }
    }
}

uint8_t process_noise(Noise noise)
{
    uint8_t volume = noise.envelope.constant ? noise.envelope.volume : noise.envelope.decay_counter;
    uint8_t wave = noise.noise_bit;
    wave = noise.length_counter.counter > 0 && wave;
    return wave * volume;
}

float process_sound()
{
    uint8_t pulse1_wave = process_pulse(pulse1);
    uint8_t pulse2_wave = process_pulse(pulse2);
    uint8_t triangle_wave = process_triangle(triangle);
    uint8_t noise_wave = process_noise(noise);
    float pulse_out = pulse1_wave == 0 && pulse2_wave == 0 ? 0 : 95.88f / ((8128.0f / (pulse1_wave + pulse2_wave)) + 100.f);
    float tnd_out = triangle_wave == 0 && noise_wave == 0 ? 0 : 159.79f / ((1.0f / (triangle_wave / 8227.0f + noise_wave / 12241.0f)) + 100.f);

    static uint16_t waveform_index = 0;
    pulse1_waveform[waveform_index] = pulse1_wave;
    pulse2_waveform[waveform_index] = pulse2_wave;
    triangle_waveform[waveform_index] = triangle_wave;
    noise_waveform[waveform_index] = noise_wave;
    waveform_index++;
    if (waveform_index == WAVEFORM_SIZE)
    {
        waveform_index = 0;
        SDL_FillRect(wave_screenSurface, NULL, SDL_MapRGB(wave_screenSurface->format, 0x00, 0x00, 0x00));
        int factor = 2;
        for (int i = 0; i < WAVEFORM_SIZE; i++)
        {
            SDL_Rect rect = {i * factor, 100 - (pulse1_waveform[i] * (100 / 16)) + 1, factor, 1};
            SDL_FillRect(wave_screenSurface, &rect, SDL_MapRGB(wave_screenSurface->format, pulse1.enable ? 0x00 : 0xFF, pulse1.enable ? 0xFF : 0x00, 0x00));
            rect = {i * factor, 200 - (pulse2_waveform[i] * (100 / 16)) + 1, factor, 1};
            SDL_FillRect(wave_screenSurface, &rect, SDL_MapRGB(wave_screenSurface->format, pulse2.enable ? 0x00 : 0xFF, pulse2.enable ? 0xFF : 0x00, 0x00));
            rect = {i * factor, 300 - (triangle_waveform[i] * (100 / 16)) + 1, factor, 1};
            SDL_FillRect(wave_screenSurface, &rect, SDL_MapRGB(wave_screenSurface->format, triangle.enable ? 0x00 : 0xFF, triangle.enable ? 0xFF : 0x00, 0x00));
            rect = {i * factor, 400 - (noise_waveform[i] * (100 / 16)) + 1, factor, 1};
            SDL_FillRect(wave_screenSurface, &rect, SDL_MapRGB(wave_screenSurface->format, noise.enable ? 0x00 : 0xFF, noise.enable ? 0xFF : 0x00, 0x00));
        }
        SDL_UpdateWindowSurface(wave_window);
    }

    return pulse_out + tnd_out;
    // return 0;
}


// uint64_t generate_rotator()
// {
//     uint64_t rotator = 0;
//     float accumulator = 0;
//     for (int i = 0; i < 64; i++)
//     {
//         accumulator += (1789773.f / TARGET_FREQUENCY) - (int)((1789773.f / TARGET_FREQUENCY));
//         if (accumulator < 1)
//         {
//             rotator |= 0 << i;
//         }
//         else
//         {
//             rotator |= 1ull << i;
//             accumulator -= 1;
//         }
//     }
//     return rotator;
// }

void play_sound()
{

    static int16_t *sample_buffer = (int16_t *)calloc(SAMPLE_COUNT, sizeof(int16_t));
    // static uint64_t rotator = generate_rotator();
    static int8_t sample_countdown = 0;
    static int16_t *single_sample_buffer = (int16_t *)calloc(sample_countdown, sizeof(int16_t));
    static int16_t sample_count = 0;
    static int16_t sound = 0;

    if (0 == sample_countdown)
    {
        sound = (int16_t)((process_sound()) * (0x7FFF));
        sample_buffer[sample_count] = sound;
        sample_count++;
        sample_countdown = (int)((1789773.f /2/ TARGET_FREQUENCY));
        // uint64_t bit = (rotator & 1);
        // rotator = (rotator >> 1) | (bit << 63ull);

        if (sample_count == SAMPLE_COUNT)
        {
            SDL_QueueAudio(dev, sample_buffer, SAMPLE_COUNT * sizeof(int16_t));
            SDL_PauseAudioDevice(dev, 0);
            sample_count = 0;
        }
    }
    else
    {
        sample_countdown--;
    }

    // int16_t sound = (int16_t) ((process_sound()) * 0x7FFF);
    // sample_buffer[sample_count] = sound;
    // sample_count++;
    // if(sample_count == SAMPLE_COUNT){
    //     SDL_QueueAudio(dev, sample_buffer, SAMPLE_COUNT * sizeof(int16_t));
    //     SDL_PauseAudioDevice(dev, 0);
    //     sample_count = 0;
    // }
}

void apu_cycle()
{
    static uint8_t alternator = 1;
    if (alternator)
    {
        if (pulse1.enable)
            update_pulse(pulse1);
        if (pulse2.enable)
            update_pulse(pulse2);
        if (triangle.enable)
            update_noise(noise);
        alternator = 0;
    }
    else
    {
        alternator = 1;
    }
    if (triangle.enable)
        update_triangle(triangle);
    play_sound();
}

void frame_clock()
{
    if (frame_counter.mode == 0)
    {
        if (frame_counter.step % 2 == 1)
        {
            update_length_counter(pulse1.length_counter);
            update_length_counter(pulse2.length_counter);
            update_length_counter(triangle.length_counter);
            update_length_counter(noise.length_counter);
            update_sweep(pulse1.sweep);
            update_sweep(pulse2.sweep);
        }
        update_envelope(pulse1.envelope);
        update_envelope(pulse2.envelope);
        update_envelope(noise.envelope);
        frame_counter.step++;
        if (frame_counter.step == 4)
        {
            if (!frame_counter.irq_disable)
            {
                cpu.interruptRequest();
            }
            frame_counter.step = 0;
        }
    }
    else
    {
        if (frame_counter.step == 1 || frame_counter.step == 4)
        {
            update_length_counter(pulse1.length_counter);
            update_length_counter(pulse2.length_counter);
            update_length_counter(triangle.length_counter);
            update_length_counter(noise.length_counter);
            update_sweep(pulse1.sweep);
            update_sweep(pulse2.sweep);
        }
        if (frame_counter.step != 3)
        {
            update_envelope(pulse1.envelope);
            update_envelope(pulse2.envelope);
            update_envelope(noise.envelope);
        }
        frame_counter.step++;
        if (frame_counter.step == 6)
        {
            frame_counter.step = 0;
        }
    }
}

void APU_write(uint16_t address, uint8_t data)
{
    switch (address)
    {
    case 0x4015:
        pulse1.enable = data & 0b00000001;
        pulse2.enable = (data & 0b00000010) >> 1;
        triangle.enable = (data & 0b00000100) >> 2;
        noise.enable = (data & 0b00001000) >> 3;
        if (!pulse1.enable)
        {
            pulse1.length_counter.counter = 0;
        }
        if (!pulse2.enable)
        {
            pulse2.length_counter.counter = 0;
        }
        if (!triangle.enable)
        {
            triangle.length_counter.counter = 0;
        }
        if (!noise.enable)
        {
            noise.length_counter.counter = 0;
        }
        break;
    case 0x4000:
        pulse1.duty = data >> 6;
        pulse1.length_counter.halt = (data >> 5) & 1;
        pulse1.envelope.loop = (data >> 5) & 1;
        pulse1.envelope.constant = (data >> 4) & 1;
        pulse1.envelope.volume = data & 0b00001111;
        break;
    case 0x4001:
        pulse1.sweep.enable = data >> 7;
        pulse1.sweep.period = (data >> 4) & 0b00000111;
        pulse1.sweep.negate = (data >> 3) & 1;
        pulse1.sweep.shift = data & 0b00000111;
        break;
    case 0x4002:
        pulse1.timer_reload = (pulse1.timer_reload & 0b11100000000) | data;
        break;
    case 0x4003:
        pulse1.timer_reload = (pulse1.timer_reload & 0b00011111111) | (((uint16_t)(data & 0b00000111)) << 8);
        pulse1.length_counter.load = data >> 3;
        pulse1.length_counter.counter = length_counter_lookup[pulse1.length_counter.load];
        pulse1.envelope.start = 1;
        break;
    case 0x4004:
        pulse2.duty = data >> 6;
        pulse2.length_counter.halt = (data >> 5) & 1;
        pulse2.envelope.loop = (data >> 5) & 1;
        pulse2.envelope.constant = (data >> 4) & 1;
        pulse2.envelope.volume = data & 0b00001111;
        break;
    case 0x4005:
        pulse2.sweep.enable = data >> 7;
        pulse2.sweep.period = (data >> 4) & 0b00000111;
        pulse2.sweep.negate = (data >> 3) & 1;
        pulse2.sweep.shift = data & 0b00000111;
        break;
    case 0x4006:
        pulse2.timer_reload = (pulse2.timer_reload & 0b11100000000) | data;
        break;
    case 0x4007:
        pulse2.timer_reload = (pulse2.timer_reload & 0b00011111111) | (((uint16_t)(data & 0b00000111)) << 8);
        pulse2.length_counter.load = data >> 3;
        pulse2.length_counter.counter = length_counter_lookup[pulse2.length_counter.load];
        pulse2.envelope.start = 1;
        break;
    case 0x4008:
        triangle.length_counter.halt = (data >> 7) & 1;
        triangle.linear_counter_load = data & 0b01111111;
        break;
    case 0x400A:
        triangle.timer_reload = (triangle.timer_reload & 0b11100000000) | data;
        break;
    case 0x400B:
        triangle.timer_reload = (triangle.timer_reload & 0b00011111111) | (((uint16_t)(data & 0b00000111)) << 8);
        triangle.length_counter.load = data >> 3;
        triangle.length_counter.counter = length_counter_lookup[triangle.length_counter.load];
        triangle.linear_counter_reload = 1;
        break;
    case 0x400C:
        noise.length_counter.halt = (data >> 5) & 1;
        noise.envelope.loop = (data >> 5) & 1;
        noise.envelope.constant = (data >> 4) & 1;
        noise.envelope.volume = data & 0b00001111;
        break;
    case 0x400E:
        noise.loop_noise = (data >> 7) & 1;
        noise.timer_reload = data & 0b00001111;
        break;
    case 0x400F:
        noise.length_counter.load = data >> 3;
        noise.length_counter.counter = length_counter_lookup[noise.length_counter.load];
        noise.envelope.start = 1;
        break;
    }
}

uint8_t APU_read()
{
    // reading from $4015
    uint8_t status = 0;
    status |= pulse1.length_counter.counter > 0;
    status |= pulse2.length_counter.counter > 0 << 1;
    status |= triangle.length_counter.counter > 0 << 2;
    status |= noise.length_counter.counter > 0 << 3;
    status |= 0 << 4;
    status |= 0 << 5;
    status |= frame_counter.irq_disable << 6;
    status |= 0 << 7;
    frame_counter.irq_disable = 0;
    return status;
}
