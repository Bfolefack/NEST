#ifndef SYSTEM_VARS_H
#define SYSTEM_VARS_H

#include <stdbool.h>
#include <stdint.h>
typedef enum Mirror_Type {
    HORIZONTAL, VERTICAL
} Mirror_Type;

extern uint8_t memory[4096];

extern prg_rom_size;

extern uint8_t trainer[0x200];
extern uint8_t prg_rom[0x8000];
extern uint8_t chr_rom[0x2000];
extern uint8_t inst_rom[0x2000];
extern uint8_t prom[0x20];

extern Mirror_Type mirroring;

#endif