#include "system_vars.h"
#include "ppu.h"
#include "execute.h"

PPUREGS ppu_regs;
bool higher_byte = true;

void write_to_ppu_addr(uint8_t data) {
    ppu_regs.ppu_addr = higher_byte ? 
                        (((uint16_t) data) << 8) | (ppu_regs.ppu_addr & 0xFF) : 
                        (ppu_regs.ppu_addr & 0xFF00) | data;
    higher_byte = !higher_byte;
}

void increment_ppu_addr() {
    ppu_regs.ppu_addr += (ppu_regs.ppu_ctrl & 0b100) ? 32 : 1;
}