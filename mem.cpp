#include "mem.h"
#include "ppu.h"
#include "system_vars.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

uint8_t memory [0x800];
uint8_t prg_ram[0x2000];

uint8_t read(uint16_t address) {
    if (address < 0x2000) {
        address = address % 0x0800;
        if (address < 0x00FF) {
            return 0;
        } else {
            return memory[address];
        }
    } else if (address < 0x4000) {
        address = address % 0x0008;
        switch (address) {
            case 0: 
            case 1:
            case 3:
            case 5:
            case 6:
                return 0;
            case 2:
                ppu_internals.w = false;
                uint8_t temp = ppu_regs.ppu_status;
                ppu_regs.ppu_status = ppu_regs.ppu_status & 0b01111111;
                return temp;
            case 4:
                return ppu_regs.oam_data;
            case 7:
                increment_ppu_addr();
                return ppu_regs.ppu_data;
        }
    } else if (address < 0x401F) {
        if (address == 0x4014) {
            return 0;
        } else {
            return 0; // TODO - APU and I/O registers
        }
    } else if (0x6000 <= address && address < 0x8000) {
        return prg_ram[address - 0x6000];
    } else if (address >= 0x8000) {
        if (prg_rom_size < 0x8000) {
            address = address % 0x4000;
        } 
        return prg_rom[address % 0x8000];
    } else {
        return 0; // some mappers may use this space
    }
    return -1; // Unreachable Code
}

void write(uint16_t address, uint8_t data) {
    if (address < 0x2000) {
        address = address % 0x0800;
        if (address < 0x00FF) {
            return; // writing to zero page is meaningless
        } else {
            memory[address] = data;
        }
    } else if (address < 0x4000) {
        address = address % 0x0008;
        switch (address) {
            case 2:
                perror("Cannot write to read-only register");
                exit(3);
            case 0: 
                uint8_t before_status = ppu_regs.ppu_ctrl >> 7;
                ppu_regs.ppu_ctrl = data;
                if (!before_status && ppu_regs.ppu_ctrl >> 7 && ppu_regs.ppu_status >> 7) {
                    cpu.nonmaskableInterrupt();
                }
                return; // TODO : emulate PPU power-up state?
            case 1: 
                ppu_regs.ppu_mask = data;
                return;
            case 3:
                ppu_regs.oam_addr = data;
                return;
            case 4:
                ppu_regs.oam_data = data;
                ppu_regs.oam_addr++;
                return;
            case 5:
                ppu_regs.ppu_scroll = ppu_internals.w ? 
                        (ppu_regs.ppu_addr & 0xFF00) | data :
                        (((uint16_t) data) << 8) | (ppu_regs.ppu_addr & 0xFF);
                ppu_internals.w = !ppu_internals.w;
                return;
            case 6:
                ppu_regs.ppu_data = ppu_internals.w ? 
                        (ppu_regs.ppu_data & 0xFF00) | data :
                        (((uint16_t) data) << 8) | (ppu_regs.ppu_data & 0xFF);
                ppu_internals.w = !ppu_internals.w;
                return;
            case 7:
                ppu_regs.ppu_data = data;
                increment_ppu_addr();
                return;
        }
    } else if (address < 0x401F) {
        if (address == 0x4014) {
            for (uint16_t i = 0; i < 0x100; i++) {
                oam_data[i] = memory[0x100 * data + i];
            }
            return;
        } else {
            return; // TODO - APU and I/O registers
        }
    } else if (0x6000 <= address && address < 0x8000) {
        prg_ram[address - 0x6000] = data;
    } else {
        return; // some mappers may use this space
    }
}