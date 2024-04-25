#include "loadnes.h"
#include "system_vars.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

char* name;
uint8_t flags_6;
uint8_t flags_7;
uint8_t flags_8;
uint8_t flags_9;
uint8_t flags_10;
char* padding;

bool uses_chr_ram;

uint32_t prg_rom_size;
uint32_t chr_rom_size;

uint8_t trainer[0x200];
uint8_t prg_rom[0x8000];
uint8_t chr_rom[0x2000];
uint8_t inst_rom[0x2000];
uint8_t prom[0x20];

Mirror_Type mirroring;

void load_nes(char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Cannot read from file");
        exit(-1);
    }

    name = (char*) malloc(4 * sizeof(uint8_t));
    fread(name, sizeof(uint8_t), 4, file);

    prg_rom_size = 16384 * (uint8_t) fgetc(file);

    chr_rom_size = 8192 * (uint8_t) fgetc(file);

    flags_6 = fgetc(file);
    flags_7 = fgetc(file);
    flags_8 = fgetc(file);
    flags_9 = fgetc(file);
    flags_10 = fgetc(file);

    padding = (char*) malloc(5 * sizeof(uint8_t));
    fread(padding, sizeof(uint8_t), 5, file);

    if (flags_6 & 0b10) { // file contains trainer
        fread(trainer, sizeof(uint8_t), 0x200, file);
    }

    fread(prg_rom, sizeof(uint8_t), prg_rom_size, file);

    if (chr_rom_size > 0) {
        fread(chr_rom, sizeof(uint8_t), chr_rom_size, file);
    }

    if (flags_7 & 0b10) { 
        // file contains INST-ROM
        fread(inst_rom, sizeof(uint8_t), 0x2000, file);
        fread(prom, sizeof(uint8_t), 0x20, file); // often omitted
    }

    if (fgetc(file) != EOF) {
        perror("Unknown extra content in file");
        exit(-2);
    }

    if (flags_6 & 0b1) {
        mirroring = HORIZONTAL;
    } else {
        mirroring = VERTICAL;
    }
}