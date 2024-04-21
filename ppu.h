#include "system_vars.h"
typedef struct PPUCTRL {
    uint8_t V; // NMI Enable
    uint8_t P; // PPU Master/Slave - don't use
    uint8_t H; // Sprite Size
    uint8_t B; // Background Pattern Table Address
    uint8_t S; // Sprite Pattern Table Address
    uint8_t I;
    uint8_t N1;
    uint8_t N0;
} PPUCTRL;

typedef struct PPUMASK {
    uint8_t B;
    uint8_t G;
    uint8_t R;
    uint8_t s;
    uint8_t b;
    uint8_t M;
    uint8_t m;
    uint8_t G;
} PPUMASK;


typedef struct PPUSTATUS {
    uint8_t V;
    uint8_t S;
    uint8_t O;
} PPUSTATUS;