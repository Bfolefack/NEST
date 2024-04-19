#include "system_vars.h"
#include "execute.h"

AddressingMode mode;
const uint16_t STACK_BASE = 0x0100; 

void init() {
    regs.A = 0;
    regs.X = 0;
    regs.Y = 0;
    regs.PC = 0xFFFC;
    regs.S = 0xFF; // Relative address
    regs.flags.C = 0;
    regs.flags.Z = 0;
    regs.flags.I = 1;
    regs.flags.D = 0;
    regs.flags.B = 0;
    regs.flags.J = 0;
    regs.flags.V = 0;
    regs.flags.N = 0;
}

void push(uint8_t value) {
    memory[STACK_BASE + regs.S] = value;
    regs.S--;
}

uint8_t pull() {
    regs.S++;
    return memory[STACK_BASE + regs.S];
}

uint8_t flags_to_byte(Flags flags) {
    return (flags.C << 0) | (flags.Z << 1) | (flags.I << 2) | (flags.D << 3) | 
           (flags.B << 4) | (flags.J << 5) | (flags.V << 6) | (flags.N << 7);
}

Flags byte_to_flags(uint8_t byte) {
    Flags flags;
    flags.C = (byte >> 0) & 1;
    flags.Z = (byte >> 1) & 1;
    flags.I = (byte >> 2) & 1;
    flags.D = (byte >> 3) & 1;
    flags.B = (byte >> 4) & 1;
    flags.J = (byte >> 5) & 1;
    flags.V = (byte >> 6) & 1;
    flags.N = (byte >> 7) & 1;
    return flags;
}

void interrupt() {
    // TO DO 
    RTI();
}

void BRK() {
    regs.flags.I = 1;
    regs.flags.B = 1;
    push((regs.PC + 2) >> 8);
    push(regs.PC + 2);
    push(flags_to_byte(regs.flags));
    interrupt();
    regs.flags.J = 1;
}

void RTI() {
    regs.flags = byte_to_flags(pull());
    regs.PC = pull();
    regs.PC |= pull() << 8;
    regs.flags.J = 1;
}

void RTS() {
    regs.PC = pull();
    regs.PC |= pull() << 8;
    regs.flags.J = 1;
}

void PHP() {
    push(flags_to_byte(regs.flags));
}

void CLC() {
    regs.flags.C = 0;
}

void PLP() {
    regs.flags = byte_to_flags(pull());
}

void SEC() {
    regs.flags.C = 1;
}

void CLI() {
    regs.flags.I = 0;
}

void PLA() {
    regs.A = pull();
    regs.flags.N = regs.A < 0;
    regs.flags.Z = regs.A == 0;
}

void SEI() {
    regs.flags.I = 1;
}

void DEY() {
    regs.Y--;
    regs.flags.N = regs.Y < 0;
    regs.flags.Z = regs.Y == 0;
}

void TYA() {
    regs.A = regs.Y;
    regs.flags.N = regs.A < 0;
    regs.flags.Z = regs.A == 0;
}

void TAY() {
    regs.Y = regs.A;
    regs.flags.N = regs.Y < 0;
    regs.flags.Z = regs.Y == 0;
}

void CLV() {
    regs.flags.V = 0;
}

void INX() {
    regs.X++;
    regs.flags.N = regs.X < 0;
    regs.flags.Z = regs.X == 0;
}

void SED() {
    regs.flags.D = 1;
}

void TXA() {
    regs.A = regs.X;
    regs.flags.N = regs.A < 0;
    regs.flags.Z = regs.A == 0;
}

void TXS() {
    regs.S = regs.X;
}

void TAX() {
    regs.X = regs.A;
    regs.flags.N = regs.X < 0;
    regs.flags.Z = regs.X == 0;
}

void TSX() {
    regs.X = regs.S;
    regs.flags.N = regs.X < 0;
    regs.flags.Z = regs.X == 0;
}

void DEX() {
    regs.X--;
    regs.flags.N = regs.X < 0;
    regs.flags.Z = regs.X == 0;
}

void JMP(uint16_t imm) {
    regs.PC = imm;
    regs.flags.J = 1;
}

void BPL(int8_t imm) {
    if (regs.flags.N == 0) {
        regs.PC = regs.PC + imm;
        regs.flags.J = 1;
    }
}

void JSR(uint16_t imm) {
    push((regs.PC + 2) >> 8);
    push(regs.PC + 2);
    regs.PC = imm;
    regs.flags.J = 1;
}

void BMI(int8_t imm) {
    if (regs.flags.N == 1) {
        regs.PC = regs.PC + imm;
        regs.flags.J = 1;
    }
}

void BVC(int8_t imm) {
    if (regs.flags.V == 0) {
        regs.PC = regs.PC + imm;
        regs.flags.J = 1;
    }
}

void BVS(int8_t imm) {
    if (regs.flags.V == 1) {
        regs.PC = regs.PC + imm;
        regs.flags.J = 1;
    }
}

void BCC(int8_t imm) {
    if (regs.flags.C == 0) {
        regs.PC = regs.PC + imm;
        regs.flags.J = 1;
    }
}

void LDY(uint8_t imm) {
    regs.Y = imm;
    regs.flags.N = regs.Y >> 7;
    regs.flags.Z = regs.Y == 0;
}

void BCS(int8_t imm) {
    if (regs.flags.C == 1) {
        regs.PC = regs.PC + imm;
        regs.flags.J = 1;
    }
}

void CPY(uint8_t imm) {
    regs.flags.N = regs.Y < imm;
    regs.flags.Z = regs.Y == imm;
    regs.flags.C = regs.Y >= imm;
}

void BNE(int8_t imm) {
    if (regs.flags.Z == 0) {
        regs.PC = regs.PC + imm;
        regs.flags.J = 1;
    }
}

void CPX(uint8_t imm) {
    regs.flags.N = regs.X < imm;
    regs.flags.Z = regs.X == imm;
    regs.flags.C = regs.X < imm;
}

void BEQ(int8_t imm) {
    if (regs.flags.Z == 1) {
        regs.PC = regs.PC + imm;
        regs.flags.J = 1;
    }
}

void BIT(int8_t imm) {
    regs.flags.N = imm >> 7;
    regs.flags.V = (imm >> 6) & 1;
    regs.flags.Z = (regs.A & imm) == 0;
}

void STY(uint16_t imm) {
    memory[imm] = regs.Y;
}

void ORA(int8_t imm) {
    regs.A = (regs.A & imm);
    regs.flags.N = regs.A >> 7 == 1;
    regs.flags.Z = regs.A == 0;
}

void AND(int8_t imm) {
    regs.A = (regs.A & imm);
    regs.flags.N = regs.A >> 7 == 1;
    regs.flags.Z = regs.A == 0;
}