#include "mem.h"
#include "stdint.h"
#include "ppu.h"
#include "system_vars.h"

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
                perror("Cannot read from write-only register");
                exit(2);
            case 2:
                return ppu_regs.ppu_status;
                // TODO - clear write latch (PPU internal register)
            case 4:
                return ppu_regs.oam_data;
            case 7:
                increment_ppu_addr();
                return ppu_regs.ppu_data;
        }
    } else if (address < 0x401F) {
        if (address == 0x4014) {
            return ppu_regs.oam_dma;
        } else {
            return 0; // TODO - APU and I/O registers
        }
    } else {
        return 0; // some mappers may use this space
    }
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
                exit(2);
            case 0: 
                ppu_regs.ppu_ctrl = data;
                return; // TODO : emulate PPU power-up state?
            case 1: 
                ppu_regs.ppu_mask = data;
                return;
            case 3:
                ppu_regs.oam_addr = data;
                return;
            case 4:
                ppu_regs.oam_data = data;
                return; // TODO - increment oam_addr (by how much?)
            case 5:
                ppu_regs.ppu_scroll = data;
                return; // TODO - make scroll behave like addr (toggle w; 16 bits)
            case 6:
                write_to_ppu_addr(data);
                return;
            case 7:
                increment_ppu_addr();
                ppu_regs.ppu_data = data;
                return;
        }
    } else if (address < 0x401F) {
        if (address == 0x4014) {
            return; // TODO - map 256 bytes from CPU to internal PPU OAM
        } else {
            return; // TODO - APU and I/O registers
        }
    } else {
        return; // some mappers may use this space
    }
}