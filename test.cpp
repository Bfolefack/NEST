#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "system_vars.h"
#include "mem.h"

void test() {
    if (read(0x6001) == 0xDE && read(0x6002) == 0xB0 && read(0x6003) == 0x61) {
        uint8_t status = read(0x6000);
        if (status < 0x80) {
            uint16_t address = 0x6004;
            while ((char) memory[address] != '\0') {
                printf("%c", (char) memory[address]);
                address++;
            }
            printf("\n");
        } else if (status == 0x80) {
            printf("Test still in progress.\n");
        } else if (status == 0x81) {
            printf("Reset required.\n");
        } else {
            printf("Unknown data at 0x6000. Exiting.\n");
            exit(4);
        }
    } else {
        printf("Test output memory in use by another program.\n");
    }
    
}

void dump_chr_rom() {
    
}

void dump_prg_rom(char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {printf("q"); exit(3);}
    for (uint16_t i = 0x8000; i != 0x0; i++) {
        fprintf(file, "%hx: %02hhx\n", i, read(i));
    }
}