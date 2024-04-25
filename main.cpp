#include <stdio.h>
#include <stdlib.h>
#include "system_vars.h"
#include "loadnes.h"
#include "cpu.h"
#include "ppu.h"
#include "test.h"
#include "vidya.h"

uint64_t cycle_number;
CPU cpu= CPU();

int main (int argc, char** argv, char** envp) {
    if (argc < 2) {
        perror("Please provide a filename");
        exit(1);
    }

    init_SDL();
    load_nes(argv[1]);

    cycle_number = 0;

    while (1) {
        refresh_window();
        ppu_cycle();
        ppu_cycle();
        ppu_cycle();
        cpu.clock();
        cycle_number++;
        if (cycle_number % 0x100 == 0x0) {
            test();
        }
    }
}