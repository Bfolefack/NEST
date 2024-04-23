#include <stdio.h>
#include <stdlib.h>
#include "system_vars.h"
#include "loadnes.h"
#include "cpu.h"
#include "ppu.h"
// #include "decode.h"
// #include "execute.h"

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
    }
}