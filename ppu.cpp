#include "system_vars.h"
#include "ppu.h"
#include <tuple>
#include <array>

PPU_EXTERNAL_REGS ppu_regs;
PPU_INTERNAL_REGS ppu_internals;
uint8_t palette_table[32];
uint8_t vram[2048];
uint8_t oam_data[256];
uint8_t data_buffer = 0;
using Color = std::tuple<uint8_t, uint8_t, uint8_t>;
uint16_t ppuCycles = 0;
uint16_t scanline = 0;
bool vblank = false;

// source: https://bugzmanov.github.io/nes_ebook/chapter_6_3.html
const std::array<Color, 64> SYSTEM_PALETTE = {
    Color(0x80, 0x80, 0x80), Color(0x00, 0x3D, 0xA6), Color(0x00, 0x12, 0xB0), Color(0x44, 0x00, 0x96),
    Color(0xA1, 0x00, 0x5E), Color(0xC7, 0x00, 0x28), Color(0xBA, 0x06, 0x00), Color(0x8C, 0x17, 0x00),
    Color(0x5C, 0x2F, 0x00), Color(0x10, 0x45, 0x00), Color(0x05, 0x4A, 0x00), Color(0x00, 0x47, 0x2E),
    Color(0x00, 0x41, 0x66), Color(0x00, 0x00, 0x00), Color(0x05, 0x05, 0x05), Color(0x05, 0x05, 0x05),
    Color(0xC7, 0xC7, 0xC7), Color(0x00, 0x77, 0xFF), Color(0x21, 0x55, 0xFF), Color(0x82, 0x37, 0xFA),
    Color(0xEB, 0x2F, 0xB5), Color(0xFF, 0x29, 0x50), Color(0xFF, 0x22, 0x00), Color(0xD6, 0x32, 0x00),
    Color(0xC4, 0x62, 0x00), Color(0x35, 0x80, 0x00), Color(0x05, 0x8F, 0x00), Color(0x00, 0x8A, 0x55),
    Color(0x00, 0x99, 0xCC), Color(0x21, 0x21, 0x21), Color(0x09, 0x09, 0x09), Color(0x09, 0x09, 0x09),
    Color(0xFF, 0xFF, 0xFF), Color(0x0F, 0xD7, 0xFF), Color(0x69, 0xA2, 0xFF), Color(0xD4, 0x80, 0xFF),
    Color(0xFF, 0x45, 0xF3), Color(0xFF, 0x61, 0x8B), Color(0xFF, 0x88, 0x33), Color(0xFF, 0x9C, 0x12),
    Color(0xFA, 0xBC, 0x20), Color(0x9F, 0xE3, 0x0E), Color(0x2B, 0xF0, 0x35), Color(0x0C, 0xF0, 0xA4),
    Color(0x05, 0xFB, 0xFF), Color(0x5E, 0x5E, 0x5E), Color(0x0D, 0x0D, 0x0D), Color(0x0D, 0x0D, 0x0D),
    Color(0xFF, 0xFF, 0xFF), Color(0xA6, 0xFC, 0xFF), Color(0xB3, 0xEC, 0xFF), Color(0xDA, 0xAB, 0xEB),
    Color(0xFF, 0xA8, 0xF9), Color(0xFF, 0xAB, 0xB3), Color(0xFF, 0xD2, 0xB0), Color(0xFF, 0xEF, 0xA6),
    Color(0xFF, 0xF7, 0x9C), Color(0xD7, 0xE8, 0x95), Color(0xA6, 0xED, 0xAF), Color(0xA2, 0xF2, 0xDA),
    Color(0x99, 0xFF, 0xFC), Color(0xDD, 0xDD, 0xDD), Color(0x11, 0x11, 0x11), Color(0x11, 0x11, 0x11)
};

void increment_ppu_addr() {
    ppu_regs.ppu_addr += (ppu_regs.ppu_ctrl & 0b100) ? 32 : 1;
}

uint16_t mirror_vram_addr(uint16_t addr) {
    if (mirroring == HORIZONTAL) {
        if (addr >= 0x2400 && addr < 0x2800) {
            return addr - 0x2400;
        }
        else if (addr >= 0x2C00) {
            return addr - 0x2400;
        }
        return addr - 0x2000;
    }
    else {
        if (addr >= 0x2800 && addr < 0x2C00) {
            return addr - 0x2800;
        }
        else if (addr >= 0x2C00) {
            return addr - 0x2800;
        }
        return addr - 0x2800;
    }
}

uint8_t read_ppu() {
    uint16_t addr = ppu_regs.ppu_addr;
    increment_ppu_addr();
    uint8_t result = 0;

    if (0 <= addr && addr <= 0x1FFF) {
        result = data_buffer;
        data_buffer = chr_rom[addr];
        // read from CHR ROM
    }
    else if(0x2000 <= addr && addr <= 0x2FFF) {
        result = data_buffer;
        data_buffer = vram[mirror_vram_addr(addr)];
        // read from VRAM
    }
    else if (0x3F00 <= addr && addr <= 0x3FFF) {
        result = palette_table[addr % 0x20];

        // read from palette table
    }
    return result;
}

uint8_t vblank_nmi() {
    return ppu_regs.ppu_ctrl >> 7;
}

uint8_t vblank() {
    return ppu_regs.ppu_status >> 7;
}

void write_to_ctrl(uint8_t value) {
    uint8_t before_status = vblank_nmi();
    ppu_regs.ppu_ctrl = value;
    if (!before_status && vblank_nmi() && vblank()) {
        // generate NMI
    }
}

bool ppu_cycle(uint8_t cycles) {
    ppuCycles += cycles;
    if (ppuCycles >= 341) {
        ppuCycles -= 341;
        scanline += 1;
    }

    if (scanline == 241) {
        if (vblank_nmi()) {
            ppu_regs.ppu_status = ppu_regs.ppu_status | 0b10000000;
            cpu.nonmaskableInterrupt();
        }
    }

    if (scanline >= 262) {
        scanline = 0;
        ppu_regs.ppu_status = ppu_regs.ppu_status & 0b01111111;
        return true;
    }

    return false;
}

void render() {
    
}