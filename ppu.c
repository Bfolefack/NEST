#include "system_vars.h"
#include "ppu.h"
#include "execute.h"

PPUREGS ppu_regs;

uint16_t ppuAddr;
bool higherByte = true;
uint8_t ppuCtrl = 0;


void writeToPPUADDR(uint8_t data) {
    if (higherByte) {
        ppuAddr = (data << 8) + (ppuAddr & 0xFF);
    }
    else {
        ppuAddr = (ppuAddr & 0xFF00) + data;
    }
    higherByte = !higherByte;
}

void incrementPPUADDR() {
    uint8_t VRAMIncrement = (ppuCtrl >> 2) & 1;

    if (VRAMIncrement) {
        ppuAddr += 32;
    }
    else {
        ppuAddr += 1;
    }
}

void writeToPPUCTRL(uint8_t data) {
    ppuCtrl = data;
}