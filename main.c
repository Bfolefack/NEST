#include <stdio.h>
#include <stdlib.h>
#include "system_vars.h"
#include "loadnes.h"
#include "decode.h"
#include "execute.h"

int main (int argc, char** argv, char** envp) {
    if (argc < 2) {
        perror("Please provide a filename");
        exit(1);
    }

    load_nes(argv[1]);
    init();

    while (1) {
        int pc_increment = decode_and_execute(prg_rom[regs.PC]);
        if (!regs.flags.J) {
            regs.PC += pc_increment;
            regs.flags.J = 0;
        }
    }
}