#include "system_vars.h"

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
    uint8_t v;
    uint8_t t;
    uint8_t x;
    bool w;
} PPU_INTERNAL_REGS;

extern PPU_EXTERNAL_REGS ppu_regs;
extern PPU_INTERNAL_REGS ppu_internals;
extern uint8_t data_buffer;
extern uint8_t palette_table[32];
extern uint8_t vram[2048];
extern uint8_t oam_data[256];

extern void increment_ppu_addr();


