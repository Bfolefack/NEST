#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <unistd.h>
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
    int tas_input_index = 0;

    // Game loop
    while (1) {
        //apu_cycle();
        ppu_cycle();
        //play_sound();
        ppu_cycle();
        //play_sound();
        ppu_cycle();
        //play_sound();
        cpu.clock();
        if (cycle_number % 29780 == 0) {
            if (use_tas && tas_input_index < tas_inputs.size() && frame_number == tas_inputs[tas_input_index].first) {
                P1_joypad.input.button_register = tas_inputs[tas_input_index].second;
                tas_input_index++;
            }
            if (create_tas && P1_joypad.input.button_register != 0) {
                tas_inputs.push_back({frame_number, P1_joypad.input.button_register});
            }
            frame_number++;
        }
        if (cycle_number % 7445 == 0){
            refresh_window();
            frame_clock();
        }
        cycle_number++;
    }
}

__attribute__((destructor))
static void write() {
    if (create_tas) {
        write_tas(output_filename);
    }
}