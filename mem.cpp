#include "mem.h"
#include "ppu.h"
#include "system_vars.h"
#include "vidya.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

uint8_t memory [0x800];
uint8_t prg_ram[0x2000];
uint8_t data_buffer;

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
        uint8_t temp;
        switch (address) {
            case 0: 
            case 1:
            case 3:
            case 5:
            case 6:
                return 0;
            case 2:
                ppu_internals.w = false;
                temp = ppu_regs.ppu_status;
                ppu_regs.ppu_status = ppu_regs.ppu_status & 0b01111111;
                return temp;
            case 4:
                return ppu_regs.oam_data;
            case 7:
                uint8_t data = data_buffer;
                data_buffer = ppu_read(ppu_internals.v);
                if (ppu_internals.v >= 0x3F00) {
                    data = data_buffer;
                }
            
                ppu_internals.v += (ppu_regs.ppu_ctrl & 0b100) ? 32 : 1;
                return data;
        }
    } else if (address < 0x401F) {
        if (address == 0x4014) {
            return 0;
        }  else if (address == 0x4016) {
            if(P1_joypad.btn_pointer > 7) {
                return 1;
            }
            uint8_t response = 0;
            switch ((uint8_t) P1_joypad.btn_pointer)
            {
            case 0:
                response = P1_joypad.A;
            case 1:
                response = P1_joypad.B;
            case 2:
                response = P1_joypad.SELECT;
            case 3:
                response = P1_joypad.START;
            case 4:
                response = P1_joypad.UP;
            case 5:
                response = P1_joypad.DOWN;
            case 6:
                response = P1_joypad.LEFT;
            case 7:
                response = P1_joypad.RIGHT;
            }
            if (!P1_joypad.strobe && P1_joypad.btn_pointer < 8) {
                P1_joypad.btn_pointer++;
            }
            return response;
        } else if (address == 0x4017) {
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
        uint8_t temp;
        switch (address) {
            case 2:
                perror("Cannot write to read-only register");
                exit(3);
            case 0: 
                temp = ppu_regs.ppu_ctrl;
                ppu_regs.ppu_ctrl = data;
                ppu_internals.t = (ppu_internals.t & 0b1111001111111111) | ((((uint16_t) ppu_regs.ppu_ctrl) & 0b00000011) << 10);
                if (!(temp >> 7) && ppu_regs.ppu_ctrl >> 7 && ppu_regs.ppu_status >> 7) {
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
                if (ppu_internals.w) {
                    uint16_t fine_y = data & 0x07;
                    ppu_internals.t = (ppu_internals.t & 0b1000111111111111) | (fine_y << 12);
                    uint16_t coarse_y = data >> 3;
                    ppu_internals.t = (ppu_internals.t & 0b1111110000011111) | (coarse_y << 5);
                }
                else {
                    ppu_internals.x = data & 0x07;
                    uint16_t coarse_x = data >> 3;
                    ppu_internals.t = (ppu_internals.t & 0b1111111111100000) | (coarse_x);
                }
                ppu_internals.w = !ppu_internals.w;
                return;
            case 6:
                if (ppu_internals.w) {
                    ppu_internals.t = (ppu_internals.t & 0xFF00) | data;
                    ppu_internals.v = ppu_internals.t;
                }
                else {
                    ppu_internals.t = (ppu_internals.t & 0xFF) | (data << 8);
                }
                ppu_internals.w = !ppu_internals.w;
                return;
            case 7:
                ppu_regs.ppu_data = data;
                ppu_internals.v += (ppu_regs.ppu_ctrl & 0b100) ? 32 : 1;
                return;
        }
    } else if (address < 0x401F) {
        if (address == 0x4014) {
            for (uint16_t i = 0; i < 0x100; i++) {
                oam_data[i] = memory[0x100 * data + i];
            }
            return;
        }   else if (address == 0x4016) {
            if (data & 0x01) {
                P1_joypad.strobe = 1;
                P1_joypad.btn_pointer = 0;
            } else {
                P1_joypad.strobe = 0;
            }
            return;
        } else if (address == 0x4017) {
        }  else {
            return; // TODO - APU and I/O registers
        }
    } else if (0x6000 <= address && address < 0x8000) {
        prg_ram[address - 0x6000] = data;
    } else {
        return; // some mappers may use this space
    }
}