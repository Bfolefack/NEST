#include "loadnes.h"
#include "stdio.h"
#include "stdbool.h"

#define byte char

byte* name;
byte flags_6;
byte flags_7;
byte flags_8;
byte flags_9;
byte flags_10;
byte* padding;

bool uses_chr_ram;

uint32_t prg_rom_size;
uint32_t chr_rom_size;

char* trainer;
char* prg_rom;
char* chr_rom;
char* inst_rom;
char* prom;

void load_nes(char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Cannot read from file");
        exit(-1);
    }

    name = (byte*) malloc(4 * sizeof(byte));
    fread(name, sizeof(byte), 4, file);

    prg_rom_size = 16384 * (uint8_t) fgetc(file);

    chr_rom_size = 8192 * (uint8_t) fgetc(file);

    flags_6 = fgetc(file);
    flags_7 = fgetc(file);
    flags_8 = fgetc(file);
    flags_9 = fgetc(file);
    flags_10 = fgetc(file);

    padding = (byte*) malloc(5 * sizeof(byte));
    fread(padding, sizeof(byte), 5, file);

    if (flags_6 & 0b10) { 
        // file contains trainer
        trainer = malloc(512 * sizeof(byte));
        fread(trainer, sizeof(byte), 512, file);
    } else {
        trainer = NULL;
    }

    prg_rom = malloc(prg_rom_size * sizeof(byte));
    fread(prg_rom, sizeof(byte), prg_rom_size, file);

    if (chr_rom_size > 0) {
        chr_rom = malloc(chr_rom_size * sizeof(byte));
        fread(chr_rom, sizeof(byte), chr_rom_size, file);
    } else {
        chr_rom = NULL;
    }

    if (flags_7 & 0b100) { 
        // file contains INST-ROM
        inst_rom = malloc(8192 * sizeof(byte));
        fread(inst_rom, sizeof(byte), 8192, file);
        prom = malloc(32 * sizeof(byte));
        if (fread(prom, sizeof(byte), 32, file) < 32) {
            // PROM section is often omitted
            free(prom);
            prom = NULL;
        }
    } else {
        inst_rom = NULL;
    }

    if (fgetc != EOF) {
        perror("Unknown extra content");
        exit(-2);
    }
}