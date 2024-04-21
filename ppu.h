#include "system_vars.h"
typedef struct PPUCTRL {
    uint8_t V : 1; // NMI Enable
    uint8_t P : 1; // PPU Master/Slave - don't use
    uint8_t H : 1; // Sprite Size
    uint8_t B : 1; // Background Pattern Table Address
    uint8_t S : 1; // Sprite Pattern Table Address
    uint8_t I : 1; // VRAM address increment
    uint8_t N2 : 1;
    uint8_t N1 : 1;
} PPUCTRL;

typedef struct PPUMASK {
    uint8_t B : 1;
    uint8_t G : 1;
    uint8_t R : 1;
    uint8_t s : 1;
    uint8_t b : 1;
    uint8_t M : 1;
    uint8_t m : 1;
    uint8_t G : 1;
} PPUMASK;


typedef struct PPUSTATUS {
    uint8_t V : 1;
    uint8_t S : 1;
    uint8_t O : 1;

} PPUSTATUS;

typedef struct PPUADDR {
    uint8_t lower8;
    uint8_t higher8;
    uint16_t fullAddr;
    bool higherByte;
} PPUADDR;

typedef struct PPUREGS {
    PPUCTRL ppu_ctrl;
    PPUMASK ppu_mask;
    PPUSTATUS ppu_status;
    uint8_t oam_addr;
    uint8_t oam_data;
    uint8_t ppu_scroll;
    uint8_t ppu_addr;
    uint8_t ppu_data;
    uint8_t oam_dma;
} PPUREGS;

