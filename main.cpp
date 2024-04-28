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
std::string output_filename;
bool use_tas;
bool create_tas;

int main (int argc, char** argv, char** envp) {
    if (argc < 2) {
        perror("Please provide a filename");
        exit(1);
    }

    init_SDL();
    load_nes(argv[1]);
    
    if (argc > 2) {
        std::string flag(argv[2]);
        if (flag == "-i") {
            use_tas = true;
            create_tas = false;
        }
        else if (flag == "-o") {
            use_tas = false;
            create_tas = true;
        }
        else {
            perror("usage: [rom].nes -i/-o [input].tas/[output].tas");
            exit(1);
        }
        if (use_tas) {
            load_tas(argv[3]);
        }
        else if (create_tas) {
            output_filename = argv[3];
        }
    }
    else {
        use_tas = false;
        create_tas = false;
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
        if (cycle_number % 0x10000 /* is_frame */) {
            if (use_tas) {
                P1_joypad.input.button_register = tas_inputs[frame_number];
            }
            if (create_tas) {
                tas_inputs.push_back(P1_joypad.input.button_register);
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

__attribute__((destructor))
static void write() {
    if (create_tas) {
        write_tas(output_filename);
    }
}