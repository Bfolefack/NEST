#pragma once

extern void init();
extern void BRK();
extern void RTI();
extern void RTS();
extern void PHP();
extern void CLC();
extern void PLP();
extern void SEC();
extern void CLI();
extern void PLA();
extern void SEI();
extern void DEY();
extern void TYA();
extern void TAY();
extern void CLV();
extern void INY();
extern void CLD();
extern void INX();
extern void SED();
extern void TXA();
extern void TXS();
extern void TAX();
extern void TSX();
extern void DEX();
extern void JMP(uint16_t imm);
extern void BPL(uint16_t imm);
extern void JSR(uint16_t imm);
extern void BMI(uint16_t imm);
extern void BVC(uint16_t imm);
extern void BVS(uint16_t imm);
extern void BCC(uint16_t imm);
extern void LDY(uint16_t imm);
extern void BCS(uint16_t imm);
extern void CPY(uint16_t imm);
extern void BNE(uint16_t imm);
extern void CPX(uint16_t imm);
extern void BEQ(uint16_t imm);
extern void BIT(uint16_t imm);
extern void STY(uint16_t imm);
extern void ORA(uint16_t imm);
extern void AND(uint16_t imm);
extern void EOR(uint16_t imm);
extern void ADC(uint16_t imm);
extern void STA(uint16_t imm);
extern void LDA(uint16_t imm);
extern void CMP(uint16_t imm);
extern void SBC(uint16_t imm);
extern void ASL(uint16_t imm, bool accumulator);
extern void ROL(uint16_t imm, bool accumulator);
extern void LSR(uint16_t imm, bool accumulator);
extern void ROR(uint16_t imm, bool accumulator);
extern void STX(uint16_t imm);
extern void LDX(uint16_t imm);
extern void DEC(uint16_t imm);
extern void INC(uint16_t imm);
