#ifndef SYSTEM_VARS_H
#define SYSTEM_VARS_H

#include <stdbool.h>
#include <stdint.h>


typedef struct Flags {
    uint8_t C : 1; // Carry
    uint8_t Z : 1; // Zero
    uint8_t I : 1; // Interrupt
    uint8_t D : 1; // Decimal
    uint8_t B : 1; // Break
    uint8_t J : 1; // Jumped
    uint8_t V : 1; // Overflow
    uint8_t N : 1; // Negative
} Flags;



typedef struct Registers {
    uint8_t A; // Accumulator
    uint8_t X; // X register
    uint8_t Y; // Y register
    uint16_t PC; // Program counter
    uint8_t S; // Stack pointer
    Flags flags;
} Registers;

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

extern uint8_t memory[4000];
extern Registers regs;

#endif