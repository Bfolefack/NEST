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
    return (flags.C << 0) | (flags.Z << 1) | (flags.I << 2) | (flags.D << 3) | (flags.B << 4) | (flags.J << 5) | (flags.V << 6) | (flags.N << 7);
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

// void regs.flags.Flags flag, bool set) {
//     if (flag == B) {
//         B = set;
//         return;
//     }
//     int bit_number;
//     switch (flag) {
//         case N:
//             bit_number = 7;
//             break;
//         case V:
//             bit_number = 6;
//             break;
//         case D:
//             bit_number = 3;
//             break;
//         case I:
//             bit_number = 2;
//             break;
//         case Z:
//             bit_number = 1;
//             break;
//         case C:
//             bit_number = 0;
//             break;
//     }
//     if (set) {
//         regs.P |= 1 << bit_number;
//     }
//     else {
//         regs.P &= ~(1 << bit_number);
//     }
// }

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
}

void RTI() {
    regs.flags = byte_to_flags(pull());
    regs.PC = pull();
    regs.PC |= pull() << 8;
}

void RTS() {
    regs.PC = pull();
    regs.PC |= pull() << 8;
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

void LDY(uint16_t imm) {
    regs.Y = imm;
    regs.flags.N = regs.Y < 0;
    regs.flags.Z = regs.Y == 0;
}

void BCS(int8_t imm) {
    if (regs.flags.C == 1) {
        regs.PC = regs.PC + imm;
        regs.flags.J = 1;
    }
}

void CPY(uint16_t imm) {
    
}

void BNE(int8_t imm) {
    if (regs.flags.Z == 0) {
        regs.PC = regs.PC + imm;
        regs.flags.J = 1;
    }
}

void CPX(uint16_t imm) {

}

void BEQ(int8_t imm) {
    if (regs.flags.Z == 1) {
        regs.PC = regs.PC + imm;
        regs.flags.J = 1;
    }
}
