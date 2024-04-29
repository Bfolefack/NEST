#include "system_vars.h"

using Color = std::tuple<uint8_t, uint8_t, uint8_t>;
extern Color image_buffer[240][256];

typedef struct PPU_EXTERNAL_REGS {
    uint8_t ppu_ctrl;
    uint8_t ppu_mask;
    uint8_t ppu_status;
    uint8_t oam_addr;
    uint8_t oam_data;
    uint16_t ppu_scroll;
    uint16_t ppu_addr;
    uint8_t ppu_data;
    uint8_t oam_dma;
} PPU_EXTERNAL_REGS;

typedef struct PPU_INTERNAL_REGS {
    // bits 14-12: fine y scroll
    // bits 11-10: nametable select
    // bits 9-5: coarse Y scroll
    // bits 4-0: coarse X scroll
    uint16_t v; 
    uint16_t t;
    uint8_t x; // fine x scroll: 3 bits
    bool w; // 1 bit
} PPU_INTERNAL_REGS;

extern PPU_EXTERNAL_REGS ppu_regs;
extern PPU_INTERNAL_REGS ppu_internals;
extern uint8_t data_buffer;
extern uint8_t palette_table[32];
extern uint8_t vram[2048];
extern uint8_t oam_data[256];

extern void ppu_cycle();
extern uint8_t ppu_read(uint16_t addr);
void ppu_write(uint16_t addr, uint8_t data);
extern uint8_t vblank();

