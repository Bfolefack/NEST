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
        uint32_t ins = (((uint32_t) prg_rom[regs.PC]) << 24)
                     | (((uint32_t) prg_rom[regs.PC + 1]) << 16)
                     | (((uint32_t) prg_rom[regs.PC + 2]) << 8)
                     | (((uint32_t) prg_rom[regs.PC + 3]));

        uint32_t pc_increment = decode_and_execute(ins);
        if (!regs.flags.J) {
            regs.PC += pc_increment;
        }
        regs.flags.J = 0;
    }
}