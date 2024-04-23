#include <stdio.h>
#include <stdlib.h>
#include "system_vars.h"
#include "loadnes.h"
#include "cpu.h"
#include "ppu.h"
#include "test.h"

uint64_t cycle_number;

int main (int argc, char** argv, char** envp) {
    if (argc < 2) {
        perror("Please provide a filename");
        exit(1);
    }

    load_nes(argv[1]);

    CPU cpu = CPU();

    while (1) {
        ppu_cycle(1);
        ppu_cycle(1);
        ppu_cycle(1);
        cpu.clock();
        cycle_number++;
        if (cycle_number % 0x1000 == 0x0) {
            test();
        }
    }
}