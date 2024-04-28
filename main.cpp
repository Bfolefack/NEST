#include <stdio.h>
#include <stdlib.h>
#include "system_vars.h"
#include "loadnes.h"
#include "loadtas.h"
#include "cpu.h"
#include "ppu.h"
#include "test.h"
#include "vidya.h"

uint64_t cycle_number = 0;
uint64_t frame_number = 0;
CPU cpu;
bool use_tas_input;

int main (int argc, char** argv, char** envp) {
    if (argc < 2) {
        perror("Please provide a filename");
        exit(1);
    }

    init_SDL();
    load_nes(argv[1]);
    
    if (argc > 2) {
        use_tas_input = true;
        load_tas(argv[2]);
    }
    else {
        use_tas_input = false;
    }

    if (mapper != 0) {
        printf("Unsupported mapper %hhu", mapper);
        exit(2);
    }

    cpu.reset();
    cycle_number = 0;

    while (1) {
        if(cycle_number % 0x100)
            refresh_window();
        if (0 /* is_frame */) {
            if (use_tas_input) {
                P1_joypad.input.button_register = tas_inputs[frame_number];
            }
            frame_number++;
        }
        ppu_cycle();
        ppu_cycle();
        ppu_cycle();
        cpu.clock();
        cycle_number++; 
        // if (cycle_number % 0x100 == 0x0) {
        //     test();
        // }
    }
}