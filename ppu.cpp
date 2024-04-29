#include "system_vars.h"
#include "ppu.h"
#include "vidya.h"
#include <tuple>
#include <array>

PPU_EXTERNAL_REGS ppu_regs;
PPU_INTERNAL_REGS ppu_internals;

uint8_t palette_table[32];
uint8_t vram[2048];
uint8_t oam_data[256];
uint8_t oam_secondary[32];
uint8_t sprite_xes[8];
uint8_t sprite_attributes[8];
uint8_t sprite_tile_data[8][8];
bool odd_frame = false;

using Color = std::tuple<uint8_t, uint8_t, uint8_t>;
uint16_t ppuCycles = 0;
int16_t scanline = 0;

bool sprite_0_used;
bool sprite_0_in_next_scanline;
bool sprite_0_in_scanline;
uint8_t sprites;
uint8_t oam_read;
uint8_t sprite_index_in_oam_data;
uint8_t byte_of_sprite;

uint8_t name_table;
uint8_t attribute;
uint8_t tile_low;
uint8_t tile_high;
Color image_buffer[240][256];

uint16_t shift_pattern_low;
uint16_t shift_pattern_high;
uint16_t shift_attribute_low;
uint16_t shift_attribute_high;

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

bool render_background() {
    uint8_t background_render = (ppu_regs.ppu_mask & 0b00001000) >> 3;
    return background_render;
}

bool render_sprites() {
    uint8_t sprite_render = (ppu_regs.ppu_mask & 0b00010000) >> 4;
    return sprite_render;
}

uint16_t mirror_vram_addr(uint16_t addr) {
    if (mirroring == HORIZONTAL) {
        if (addr >= 0x2400 && addr < 0x2C00) {
            return addr - 0x2400;
        }
        else if (addr >= 0x2C00) {
            return addr - 0x2800;
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
        return addr - 0x2000;
    }
}

void ppu_write(uint16_t addr, uint8_t data) {
    if (0 <= addr && addr <= 0x1FFF) {
        // can't happen
    }
    else if (0x2000 <= addr && addr <= 0x2FFF) {
        vram[mirror_vram_addr(addr)] = data;
        // write VRAM
    }
    else if (0x3F00 <= addr && addr <= 0x3FFF) {
        addr &= 0x1F;
        if ((addr & 0b11) == 0) { // mirroring
            addr &= 0xF;
        }
        palette_table[addr] = data;
        // read from palette table
    }
}

uint8_t ppu_read(uint16_t addr) {
    if (0 <= addr && addr <= 0x1FFF) {
        return chr_rom[addr];
        // read from CHR ROM
    } else if (0x2000 <= addr && addr <= 0x2FFF) {
        return vram[mirror_vram_addr(addr)];
        // read from VRAM
    } else if (0x3F00 <= addr && addr <= 0x3FFF) {
        addr = addr & 0x1F;
        if ((addr & 0b11) == 0) {
            addr = addr & 0xF;
        }
        return palette_table[addr];
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

uint8_t tall_sprites() {
    return ppu_regs.ppu_ctrl & 0b100000;
}

void sprite_evaluation() {
    // TODO OAM secondary not updated correctly
    if (0 <= scanline && scanline < 240) {
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
                    if (scanline >= oam_read && (scanline < oam_read + 8 || (tall_sprites() && scanline < oam_read + 16))) {
                        sprite_index_in_oam_data = 64; // stop sprite evaluation this scanline
                        ppu_regs.ppu_status |= 0b00100000;
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
                        if (sprite_index_in_oam_data == 0) {
                            sprite_0_in_next_scanline = true;
                        }
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
            if ((ppuCycles & 0b111) == 0b000) {
                // fetch sprite tile data
                uint8_t secondary_index = (ppuCycles - 264) % 8;
                if (sprites > secondary_index) {
                    uint8_t attributes = oam_secondary[4 * secondary_index + 2];
                    uint16_t tile_index = oam_secondary[4 * secondary_index + 1];
                    uint16_t y_index = (scanline - oam_secondary[4 * secondary_index]);
                    if (tall_sprites()) { 
                        tile_index = (((tile_index & 0b1) << 7) | (tile_index >> 1)) << 5;        
                        if (attributes & 0b10000000) { // vertical flip
                            y_index = 16 - y_index;
                        }
                    } else {
                        tile_index = (((uint16_t) (ppu_regs.ppu_ctrl & 0b1000)) << 9) | (tile_index << 4);
                        if (attributes & 0b10000000) { // vertical flip
                            y_index = 8 - y_index;
                        }
                    }
                    uint8_t plane_0 = ppu_read(tile_index | (y_index << 1) | 0b0);
                    uint8_t plane_1 = ppu_read(tile_index | (y_index << 1) | 0b1);
                    if (attributes & 0b1000000) { // horizontal flip
                        for (uint8_t i = 0; i < 8; i++) {
                            uint16_t index = ((plane_0 & (1 << (7 - i))) >> (6 - i)) | ((plane_1 & (7 - i)) >> (7 - i));
                            sprite_tile_data[secondary_index][i] = ppu_read(0x3F10 | ((attributes & 0b11) << 2) | index);
                        }
                    } else {
                        for (uint8_t i = 0; i < 8; i++) {
                            uint16_t index = ((plane_0 & (1 << i)) >> (i - 1)) | ((plane_1 & (1 << i)) >> i);
                            sprite_tile_data[secondary_index][i] = ppu_read(0x3F10 | ((attributes & 0b11) << 2) | index);
                        }
                    }
                } else {
                    for (uint8_t i = 0; i < 8; i++) {
                        sprite_tile_data[secondary_index][i] = 0;
                    }
                }
            }
        }
    }
}

uint8_t sprite_pixel() {
    uint16_t current_x = ppuCycles - 1;
    uint8_t used_sprite = 8;
    uint8_t diff;
    for (uint8_t i = 0; i < 8; i++) {
        diff = current_x - sprite_xes[i];
        if (diff < 8) {
            used_sprite = i;
            break;
        }
    }
    sprite_0_used = (used_sprite == 0) && sprite_0_in_scanline;
    if (used_sprite < 8) {
        return sprite_tile_data[used_sprite][diff] | (sprite_attributes[used_sprite] & 0b100000); // priority flag added for muxing
    } else {
        return 0; // no sprites hit
    }
}

uint8_t choose_pixel(uint8_t sprite_pixel, uint8_t bg_pixel) {
    if (sprite_0_used && render_background() && render_sprites() && ppuCycles != 256) {
        if (ppuCycles >= 8 || (ppu_regs.ppu_mask & 0b110) == 0b110) {
            if ((sprite_pixel & 0b11) == 0b00 && (bg_pixel & 0b11) == 0b00) {
                ppu_regs.ppu_status |= 0b01000000; // sprite 0 hit
            }
        }
    }

    // if (!render_sprites()) {
    //     return bg_pixel;
    // } else if (!render_background()) {
    //     return (1 << 4) | sprite_pixel;
    // } else if ((sprite_pixel & 0b11) == 0b00) {
    //     return bg_pixel;
    // } else if ((bg_pixel & 0b11) == 0b00) {
    //     return (1 << 4) | sprite_pixel;
    // } else if (sprite_pixel & 0b100000) {
    //     return (1 << 4) | sprite_pixel;
    // } else {
    //     return bg_pixel;
    // }
    return bg_pixel;
}

void update_shift() {
    shift_pattern_low = (shift_pattern_low & 0xFF00) | tile_low;
    shift_pattern_high = (shift_pattern_high & 0xFF00) | tile_high;
    if (attribute & 0b01) {
        shift_attribute_low = (shift_attribute_low & 0xFF00) | 0xFF;
    }
    else {
        shift_attribute_low = shift_attribute_low & 0xFF00;
    }

    if (attribute & 0b10) {
        shift_attribute_high = (shift_attribute_high & 0xFF00) | 0xFF;
    }
    else {
        shift_attribute_high = shift_attribute_high & 0xFF00;
    }
}

void shift_left() {
    shift_pattern_low = shift_pattern_low << 1;
    shift_pattern_high = shift_pattern_high << 1;
    shift_attribute_low = shift_attribute_low << 1;
    shift_attribute_high = shift_attribute_high << 1;
}

void ppu_cycle() {

    sprite_evaluation();

    if (scanline >= -1 && scanline < 240) {
        if (scanline == 0 && ppuCycles == 0) {
            ppuCycles = 1;
        }

        if (scanline == -1 && ppuCycles == 1) {
            ppu_regs.ppu_status &= 0b00011111;
        }

        if ((ppuCycles >= 2 && ppuCycles < 258) || (ppuCycles >= 321 && ppuCycles < 338)) {
            shift_left();
            uint16_t background;
            switch ((ppuCycles - 1) % 8) {
                case 0:
                    update_shift();
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
                    tile_low = ppu_read((background << 12) | (((uint16_t) name_table) << 4) | (fine_y()));
                    break;
                case 6:
                    background = (ppu_regs.ppu_ctrl & 0b10000) >> 4;
                    tile_high = ppu_read((background << 12) | (((uint16_t) name_table) << 4) | (fine_y() + 8));
                    break;
                case 7:
                    if (render_background() || render_sprites()) {
                        if (coarse_x() == 31) {
                            ppu_internals.v = ppu_internals.v & 0xFFE0; // reset coarse x to 0
                        }
                        else {
                            ppu_internals.v++; // increment coarse x
                        }
                        break;
                    }
            }
        }
        // scroll down 1 line
        if (ppuCycles == 256) {
            if (render_background() || render_sprites()) {
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
                        inverse_name_table_y = inverse_name_table_y << 11;
                        ppu_internals.v = (ppu_internals.v & 0b111011111111111) | inverse_name_table_y; 
                    }
                    coarseY = coarseY << 5;
                    ppu_internals.v = (ppu_internals.v & 0b111110000011111) | coarseY;
                }        
            }
        }

        // reset x value
        if (ppuCycles == 257) {
            update_shift();
            if (render_background() || render_sprites()) {
                uint8_t coarseX_t = ppu_internals.t & 0x1F;
                ppu_internals.v = (ppu_internals.v & 0xFFE0) | coarseX_t;
                uint16_t nametableX_t = ppu_internals.t & 0x400;
                ppu_internals.v = (ppu_internals.v & 0xFBFF) | nametableX_t;
            }
        }

        // redundant nametable fetch
        if (ppuCycles == 338 || ppuCycles == 340) {
            name_table = ppu_read(0x2000 | (ppu_internals.v & 0x0FFF));
        }
        if (scanline == -1 && ppuCycles >= 280 && ppuCycles < 305) {
            if (render_background() || render_sprites()) {
                uint16_t fineY_T = ppu_internals.t & 0b111000000000000; 
                uint16_t nametableY_t = ppu_internals.t & 0b100000000000;
                uint16_t coarseY_t = ppu_internals.t & 0b1111100000;
                ppu_internals.v = (ppu_internals.v & 0b0000010000011111) |  fineY_T
                | nametableY_t | coarseY_t;
            } 
        }
    }

    if (scanline == 241 && ppuCycles == 1) {
        ppu_regs.ppu_status = ppu_regs.ppu_status | 0b10000000;
        if (vblank_nmi()) {
            cpu.nonmaskableInterrupt();
        }
    }

    // draw pixels
    uint8_t pixel;
    uint8_t palette;
    
    if (render_background()) {
        uint16_t rendered_bit = 0x8000 >> ppu_internals.x;
        uint8_t pixel_low = (shift_pattern_low & rendered_bit) >> (15 - ppu_internals.x);
        uint8_t pixel_high = (shift_pattern_high & rendered_bit) >> (15 - ppu_internals.x);
        pixel = (pixel_high << 1) | pixel_low;

        uint8_t palette_low = (shift_attribute_low & rendered_bit) >> (15 - ppu_internals.x);
        uint8_t palette_high = (shift_attribute_high & rendered_bit) >> (15 - ppu_internals.x);

        palette = (palette_high << 1) | palette_low;
    }

    uint8_t paletteChoice = choose_pixel(sprite_pixel(), (palette << 2) + pixel);
    Color pixelColor = SYSTEM_PALETTE[ppu_read(0x3F00 | paletteChoice)];
    if (ppuCycles < 256 && scanline < 240 && scanline >= 0) {
        image_buffer[scanline][ppuCycles] = pixelColor;
    }

    ppuCycles++;

    if (ppuCycles == 341 || (odd_frame && scanline == -1 && ppuCycles == 340 && (render_background() || render_sprites()))) {
        ppuCycles = 0;
        sprite_0_in_scanline = sprite_0_in_next_scanline;
        sprite_0_in_next_scanline = false;
        scanline++;
        if (scanline == 261) {
            scanline = -1;
            odd_frame = !odd_frame;
            if (ppuCycles == 0) {
                draw_window();
            }
        }
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