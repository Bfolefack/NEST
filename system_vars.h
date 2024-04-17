#include <stdbool.h>
#include <stdint.h>

typedef struct SystemFlags {
    bool N; // Negative
    bool Z; // Zero
    bool C; // Carry
    bool I; // Interrupt
    bool D; // Decimal
    bool B; // Break
    bool V; // Overflow
} SystemFlags;

typedef struct Registers {
    uint8_t A; // Accumulator
    uint8_t X; // X register
    uint8_t Y; // Y register
    uint8_t S; // Stack pointer
    uint16_t PC; // Program counter
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

uint8_t memory[4000];