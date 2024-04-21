#include "system_vars.h"

typedef struct PPUREGS {
    uint8_t ppu_ctrl;
    uint8_t ppu_mask;
    uint8_t ppu_status;
    uint8_t oam_addr;
    uint8_t oam_data;
    uint8_t ppu_scroll;
    uint16_t ppu_addr;
    uint8_t ppu_data;
    uint8_t oam_dma;
} PPUREGS;

extern PPUREGS ppu_regs;

