#include "system_vars.h"
#include "ppu.h"
#include <tuple>
#include <array>

PPU_EXTERNAL_REGS ppu_regs;
PPU_INTERNAL_REGS ppu_internals;

uint8_t palette_table[32];
uint8_t vram[2048];
uint8_t oam_data[256];
uint8_t oam_secondary[32];
bool odd_frame = false;

using Color = std::tuple<uint8_t, uint8_t, uint8_t>;
uint16_t ppuCycles = 0;
int16_t scanline = 0;

uint8_t sprites;
uint8_t oam_read;
uint8_t sprite_index_in_oam_data;
uint8_t byte_of_sprite;

uint8_t name_table;
uint8_t attribute;
uint8_t tile_low;
uint8_t tile_high;
Color image_buffer[240][256];

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

void ppu_write(uint16_t addr, uint8_t data) {
    if (0 <= addr && addr <= 0x1FFF) {
        // can't happen
    }
    else if(0x2000 <= addr && addr <= 0x2FFF) {
        vram[mirror_vram_addr(addr)] = data;
        // write VRAM
    }
    else if (0x3F00 <= addr && addr <= 0x3FFF) {
        palette_table[addr % 0x20] = data;

        // read from palette table
    }
}

uint8_t ppu_read(uint16_t addr) {
    if (0 <= addr && addr <= 0x1FFF) {
        return chr_rom[addr];
        // read from CHR ROM
    } else if(0x2000 <= addr && addr <= 0x2FFF) {
        return vram[mirror_vram_addr(addr)];
        // read from VRAM
    } else if (0x3F00 <= addr && addr <= 0x3FFF) {
        return palette_table[addr % 0x20];

        // read from palette table
    } else {
        return 0; // unreachable
    }
}

uint8_t vblank_nmi() {
    return ppu_regs.ppu_ctrl >> 7;
}

uint8_t vblank() {
    return ppu_regs.ppu_status >> 7;
}

uint16_t tall_sprites() {
    return ppu_regs.ppu_ctrl & 0b100000;
}


uint16_t fine_y() {
    return (ppu_internals.v & 0b111000000000000) >> 12;
}

uint16_t nametable_y() {
    return (ppu_internals.v & 0b100000000000) >> 11;
}

uint16_t nametable_x() {
    return (ppu_internals.v & 0b10000000000) >> 10;
}

uint16_t coarse_y() {
    return (ppu_internals.v & 0b1111100000) >> 5;
}

uint16_t coarse_x() {
    return ppu_internals.v & 0b11111;
}

void sprite_evaluation() {
    if (0 <= scanline && scanline < 240) {
        // sprite evaluation
        if (1 <= ppuCycles && ppuCycles <= 64) {
            if (ppuCycles & 0b1) {
                oam_read = 0xFF;
            } else {
                oam_secondary[ppuCycles >> 1] = oam_read;
            }
        } else if (65 <= ppuCycles && ppuCycles <= 256) {
            if (ppuCycles & 0b1) {
                oam_read = oam_data[sprite_index_in_oam_data * 4 + byte_of_sprite];
            } else if (sprite_index_in_oam_data < 64) {
                if (sprites >= 8) {
                    if (scanline >= oam_read && (scanline < oam_read + 8 || tall_sprites() && scanline < oam_read + 16)) {
                        sprite_index_in_oam_data = 64; // stop sprite evaluation this scanline
                        //TODO handle overflow in registers
                    } else {
                        sprite_index_in_oam_data++;
                        // bug in original hardware that some games rely on
                        if (byte_of_sprite == 3) {
                            byte_of_sprite = 0;
                        } else {
                            byte_of_sprite++;
                        }
                    }
                } else if (byte_of_sprite == 0) {
                    if (scanline >= oam_read && (scanline < oam_read + 8 || tall_sprites() && scanline < oam_read + 16)) {
                        oam_secondary[sprites * 4 + byte_of_sprite] = oam_read;
                        byte_of_sprite++;
                    } else {
                        sprite_index_in_oam_data++;
                    }
                } else {
                    oam_secondary[sprites * 4 + byte_of_sprite] = oam_read;
                    if (byte_of_sprite == 3) {
                        byte_of_sprite = 0;
                        sprite_index_in_oam_data++;
                        sprites++;
                    } else {
                        byte_of_sprite++;
                    }
                }
            }
        } else if (257 <= ppuCycles && ppuCycles <= 320) {
            
        }
    }
}

void ppu_cycle() {

    sprite_evaluation();

    if (scanline >= -1 && scanline < 240) {
        

        if (scanline == -1 && ppuCycles == 1) {
            ppu_regs.ppu_status = ppu_regs.ppu_status & 0b01111111;
        }

        if ((ppuCycles >= 1 && ppuCycles < 257) || (ppuCycles >= 321 && ppuCycles < 337)) {
            uint16_t background;
            switch ((ppuCycles - 1) % 8) {
                case 0:
                    name_table = ppu_read(0x2000 | (ppu_internals.v & 0x0FFF));
                    break;
                case 2: 
                    attribute = ppu_read(0x23C0 | (nametable_y() << 11)
                    | (nametable_x() << 10))
                    | ((coarse_y() >> 2) << 3)
                    | (coarse_x() >> 2);

                    if (coarse_y() & 0x02) {
                        attribute = attribute >> 4;
                    }
                    if (coarse_x() & 0x02) {
                        attribute = attribute >> 2;
                    }
                    attribute = attribute & 0x03;
                    break;
                case 4: 
                    background = (ppu_regs.ppu_ctrl & 0b10000) >> 4;
                    tile_low = ppu_read((background << 12) + ((uint16_t)name_table << 4) + (fine_y()));
                    break;
                case 6:
                    background = (ppu_regs.ppu_ctrl & 0b10000) >> 4;
                    tile_low = ppu_read((background << 12) + ((uint16_t)name_table << 4) + (fine_y() + 8));
                    break;
                case 7:
                    if (coarse_x() == 31) {
                        ppu_internals.v = ppu_internals.v & 0xFFE0; // reset coarse x to 0
                        uint16_t inverse_name_table_x = ~nametable_x();
                        inverse_name_table_x =  inverse_name_table_x << 10;
                        ppu_internals.v = (ppu_internals.v & 0b111101111111111) | inverse_name_table_x; 
                    }
                    else {
                        ppu_internals.v++;
                    }
                    break;
            }
        }

        // scroll down 1 line
        if (ppuCycles == 256) {
            uint16_t fineY = fine_y();
            if (fineY < 7) {
                fineY++;
                fineY = fineY << 12;
                ppu_internals.v = (ppu_internals.v & 0xFFF) | fineY;
            }
            else {
                ppu_internals.v = (ppu_internals.v & 0xFFF);
                uint16_t coarseY = coarse_y();
                if (coarseY < 29) {
                    coarseY++;
                }
                else {
                    coarseY = 0;
                    uint16_t inverse_name_table_y = ~nametable_y();
                    inverse_name_table_y =  inverse_name_table_y << 11;
                    ppu_internals.v = (ppu_internals.v & 0b111011111111111) | inverse_name_table_y; 
                }
                coarseY = coarseY << 5;
                ppu_internals.v = (ppu_internals.v & 0b111110000011111) | coarseY;
            }
        }

        if (ppuCycles == 257) {
            
        }
    }

    if (scanline == 241 && ppuCycles == 1) {
        if (vblank_nmi()) {
            ppu_regs.ppu_status = ppu_regs.ppu_status | 0b10000000;
            cpu.nonmaskableInterrupt();
        }
    }

    if (ppuCycles == 340 || (odd_frame && ppuCycles == 339 && scanline == -1)) {
        ppuCycles = 0;
        sprites = 0;
        sprite_index_in_oam_data = 0;
        byte_of_sprite = 0;
        if (scanline == 260) {
            scanline = -1;
            odd_frame = !odd_frame;
        } else {
            scanline++;
        }
    } else {
        ppuCycles++;
    }
}

Color mask_pixel(Color color) {
    if (ppu_regs.ppu_mask & 0b1) {
        // render in grayscale
        uint8_t average = (uint8_t) (((uint16_t) std::get<0>(color) + (uint16_t) std::get<1>(color) + (uint16_t) std::get<2>(color)) / 3);
        return Color(average, average, average);
    } else {
        if (ppu_regs.ppu_mask & 0b100000) {
            // emphasize red
            color = Color(std::get<0>(color) + (std::get<0>(color) < 0xE0 ? 0x20 : 0x0), std::get<1>(color), std::get<2>(color));
        }
        if (ppu_regs.ppu_mask & 0b1000000) {
            // emphasize green
            color = Color(std::get<0>(color), std::get<1>(color) + (std::get<1>(color) < 0xE0 ? 0x20 : 0x0), std::get<2>(color));
        }
        if (ppu_regs.ppu_mask & 0b10000000) {
            // emphasize blue
            color = Color(std::get<0>(color), std::get<1>(color), std::get<2>(color) + (std::get<2>(color) < 0xE0 ? 0x20 : 0x0));
        }
        return color;
    }   
}