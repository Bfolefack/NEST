#include "system_vars.h"
#include "execute.h"

Registers regs;
uint8_t memory[4000];
bool B = 0; // Break flag

const uint16_t STACK_BASE = 0x0100; 

void init() {
    regs.A = 0;
    regs.X = 0;
    regs.Y = 0;
    regs.PC = 0xFFFC;
    regs.S = 0xFF; // Relative address
    regs.P = 0b00000100; // I = 1
}

void push(uint8_t value) {
    memory[STACK_BASE + regs.S] = value;
    regs.S--;
}

uint8_t pull() {
    regs.S++;
    return memory[STACK_BASE + regs.S];
}

void assign_flag(Flags flag, bool set) {
    if (flag == B) {
        B = set;
        return;
    }
    int bit_number;
    switch (flag) {
        case N:
            bit_number = 7;
            break;
        case V:
            bit_number = 6;
            break;
        case D:
            bit_number = 3;
            break;
        case I:
            bit_number = 2;
            break;
        case Z:
            bit_number = 1;
            break;
        case C:
            bit_number = 0;
            break;
    }
    if (set) {
        regs.P |= 1 << bit_number;
    }
    else {
        regs.P &= ~(1 << bit_number);
    }
}

void BRK() {
    assign_flag(I, 1);
    assign_flag(B, 1);
    push(regs.PC + 2);
    push(regs.P);
}

void RTI() {
    regs.P = pull();
    regs.PC = pull();
}

void RTS() {
    regs.PC = pull();
    regs.PC++;
}

void PHP() {
    push(regs.P);
}

void CLC() {
    assign_flag(C, 0);
}

void PLP() {
    regs.P = pull();
}

void SEC() {
    assign_flag(C, 1);
}

void CLI() {
    assign_flag(I, 0);
}

void PLA() {
    regs.A = pull();
    assign_flag(N, regs.A < 0);
    assign_flag(Z, regs.A == 0);
}

void SEI() {
    assign_flag(I, 1);
}

void DEY() {
    regs.Y--;
    assign_flag(N, regs.Y < 0);
    assign_flag(Z, regs.Y == 0);
}