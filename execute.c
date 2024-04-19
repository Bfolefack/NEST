#include "system_vars.h"
#include "execute.h"

AddressingMode mode;
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

bool get_flag(Flags flag) {
    if (flag == B) {
        return B;
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
    return (regs.P >> bit_number) & 1;
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

void interrupt() {
    // TO DO 
    RTI();
}

void BRK() {
    assign_flag(I, 1);
    assign_flag(B, 1);
    push((regs.PC + 2) >> 8);
    push(regs.PC + 2);
    push(regs.P);
    interrupt();
}

void RTI() {
    regs.P = pull();
    regs.PC = pull();
    regs.PC |= pull() << 8;
}

void RTS() {
    regs.PC = pull();
    regs.PC |= pull() << 8;
    regs.PC++;
}

void PHP() {
    push(regs.P);
    regs.PC++;
}

void CLC() {
    assign_flag(C, 0);
    regs.PC++;
}

void PLP() {
    regs.P = pull();
    regs.PC++;
}

void SEC() {
    assign_flag(C, 1);
    regs.PC++;
}

void CLI() {
    assign_flag(I, 0);
    regs.PC++;
}

void PLA() {
    regs.A = pull();
    assign_flag(N, regs.A < 0);
    assign_flag(Z, regs.A == 0);
    regs.PC++;
}

void SEI() {
    assign_flag(I, 1);
    regs.PC++;
}

void DEY() {
    regs.Y--;
    assign_flag(N, regs.Y < 0);
    assign_flag(Z, regs.Y == 0);
    regs.PC++;
}

void TYA() {
    regs.A = regs.Y;
    assign_flag(N, regs.A < 0);
    assign_flag(Z, regs.A == 0);
    regs.PC++;
}

void TAY() {
    regs.Y = regs.A;
    assign_flag(N, regs.Y < 0);
    assign_flag(Z, regs.Y == 0);
    regs.PC++;
}

void CLV() {
    assign_flag(V, 0);
    regs.PC++;
}

void INX() {
    regs.X++;
    assign_flag(N, regs.X < 0);
    assign_flag(Z, regs.X == 0);
    regs.PC++;
}

void SED() {
    assign_flag(D, 1);
    regs.PC++;
}

void TXA() {
    regs.A = regs.X;
    assign_flag(N, regs.A < 0);
    assign_flag(Z, regs.A == 0);
    regs.PC++;
}

void TXS() {
    regs.S = regs.X;
    regs.PC++;
}

void TAX() {
    regs.X = regs.A;
    assign_flag(N, regs.X < 0);
    assign_flag(Z, regs.X == 0);
    regs.PC++;
}

void TSX() {
    regs.X = regs.S;
    assign_flag(N, regs.X < 0);
    assign_flag(Z, regs.X == 0);
    regs.PC++;
}

void DEX() {
    regs.X--;
    assign_flag(N, regs.X < 0);
    assign_flag(Z, regs.X == 0);
    regs.PC++;
}

void JMP(uint16_t imm) {
    if (mode == ABSOLUTE) {
        regs.PC = imm;
    }
    else {
        regs.PC = memory[imm];
        regs.PC |= (memory[imm + 1] << 8);
    }
}

void BPL(int8_t imm) {
    if (get_flag(N) == 0) {
        regs.PC = regs.PC + imm;
    }
    else {
        regs.PC += 2;
    }
}

void JSR(uint16_t imm) {
    push((regs.PC + 2) >> 8);
    push(regs.PC + 2);
    regs.PC = imm;
}

void BMI(int8_t imm) {
    if (get_flag(N) == 1) {
        regs.PC = regs.PC + imm;
    }
    else {
        regs.PC = regs.PC + 2;
    }
}

void BVC(int8_t imm) {
    if (get_flag(V) == 0) {
        regs.PC = regs.PC + imm;
    }
    else {
        regs.PC += 2;
    }
}

void BVS(int8_t imm) {
    if (get_flag(V) == 1) {
        regs.PC = regs.PC + imm;
    }
    else {
        regs.PC += 2;
    }
}

void BCC(int8_t imm) {
    if (get_flag(C) == 0) {
        regs.PC = regs.PC + imm;
    }
    else {
        regs.PC += 2;
    }
}

void LDY(uint16_t imm) {
    if (mode == IMMEDIATE) {
        regs.Y = imm;
        regs.PC += 2;
    }
    else if (mode == ZERO_PAGE) {
        regs.Y = memory[imm];
        regs.PC += 2;
    }
    else if (mode == ZERO_PAGE_X) {
        regs.Y =  memory[(imm + regs.X) % 256];
        regs.PC += 2;
    }
    else if (mode == ABSOLUTE) {
        regs.Y = memory[imm];
        regs.PC += 3;
    }
    else if (mode == ABSOLUTE_X) {
        regs.Y = memory[imm + regs.X];
        regs.PC += 3;
    }
    assign_flag(N, regs.Y < 0);
    assign_flag(Z, regs.Y == 0);
}

void BCS(int8_t imm) {
    if (get_flag(C) == 1) {
        regs.PC = regs.PC + imm;
    }
    else {
        regs.PC += 2;
    }
}

void CPY(uint16_t imm) {
    if (mode == IMMEDIATE) {
        
    }
}

void BNE(int8_t imm) {
    if (get_flag(Z) == 0) {
        regs.PC = regs.PC + imm;
    }
    else {
        regs.PC += 2;
    }
}

void CPX(uint16_t imm) {

}

void BEQ(int8_t imm) {
    if (get_flag(Z) == 1) {
        regs.PC = regs.PC + imm;
    }
    else {
        regs.PC += 2;
    }
}
