#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct Registers {
    uint8_t A; // Accumulator
    uint8_t X; // X register
    uint8_t Y; // Y register
    uint16_t PC; // Program counter
    uint8_t S; // Stack pointer
    uint8_t P; // Status register (NVxxDIZC)
} Registers;

typedef enum Flags {
    N, // Negative
    Z, // Zero
    C, // Carry
    I, // Interrupt
    D, // Decimal
    V // Overflow
} Flags;

typedef enum AddressingMode {
    ACCUMULATOR,
    IMMEDIATE,
    ZERO_PAGE,
    ZERO_PAGE_X,
    ZERO_PAGE_Y,
    ABSOLUTE,
    ABSOLUTE_X,
    ABSOLUTE_Y,
    INDIRECT,
    INDIRECT_X,
    INDIRECT_Y,
    IMPLIED,
    RELATIVE
} AddressingMode;

extern AddressingMode mode;
extern uint8_t memory[4000];
extern Registers regs;