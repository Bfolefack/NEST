#include "decode.h"
#include "system_vars.h"
#include "execute.h"

int decode_and_execute(uint32_t ins)
{
    uint8_t opcode = ins >> 24;
    uint8_t hi = opcode >> 4;
    uint8_t lo = opcode & 0x0F;
    uint16_t imm = 0;

    mode = IMPLIED;

    switch (opcode)
    {
    // Unique unary operations
    case 0x00: // BRK
        BRK();
        break;
    case 0x40: // RTI
        RTI();
        break;
    case 0x60: // RTS
        RTS();
        break;
    case 0x08: // PHP
        PHP();
        break;
    case 0x18: // CLC
        CLC();
        break;
    case 0x28: // PLP
        PLP();
        break;
    case 0x38: // SEC
        SEC();
        break;
    case 0x48: // PHA
        PHA();
        break;
    case 0x58: // CLI
        CLI();
        break;
    case 0x68: // PLA
        PLA();
        break;
    case 0x78: // SEI
        SEI();
        break;
    case 0x88: // DEY
        DEY();
        break;
    case 0x98: // TYA
        TYA();
        break;
    case 0xA8: // TAY
        TAY();
        break;
    case 0xB8: // CLV
        CLV();
        break;
    case 0xC8: // INY
        INY();
        break;
    case 0xD8: // CLD
        CLD();
        break;
    case 0xE8: // INX
        INX();
        break;
    case 0xF8: // SED
        SED();
        break;
    case 0x8A: // TXA
        TXA();
        break;
    case 0x9A: // TXS
        TXS();
        break;
    case 0xAA: // TAX
        TAX();
        break;
    case 0xBA: // TSX
        TSX();
        break;
    case 0xCA: // DEX
        DEX();
        break;
    case 0xEA: // NOP
        break;

    //********************************************************
    // Unconditional branches
    case 0x4C: // JMP
        mode = ABSOLUTE;
        imm = (ins >> 8) & 0xFFFF;
        imm = address_to_immediate(imm, mode);
        JMP(imm);
        break;
    case 0x6C: // JMP
        mode = INDIRECT;
        imm = (ins >> 8) & 0xFFFF;
        imm = address_to_immediate(imm, mode);
        JMP(imm);
        break;

    //********************************************************
    // Regular operations
    default:
        switch (lo)
        {
        // BPL, JSR, BMI, BVC, BVS, BCC, BCS, LDY, BCS, CPY, BNE, CPX, BEQ
        case 0x0:
            if (hi % 2 == 1)
            {
                mode = RELATIVE;
                imm = (ins >> 16) & 0xFF;
            }
            else
            {
                if (hi == 0x2)
                {
                    mode = ABSOLUTE;
                    imm = (ins >> 8) & 0xFFFF;
                }
                else
                {
                    mode = IMMEDIATE;
                    imm = (ins >> 16) & 0xFF;
                }
            }
            imm = address_to_immediate(imm, mode);
            switch (hi)
            {
            case 0x1:
                BPL(imm);
                break;
            case 0x2:
                JSR(imm);
                break;
            case 0x3:
                BMI(imm);
                break;
            case 0x5:
                BVC(imm);
                break;
            case 0x7:
                BVS(imm);
                break;
            case 0x9:
                BCC(imm);
                break;
            case 0xA:
                LDY(imm);
                break;
            case 0xB:
                BCS(imm);
                break;
            case 0xC:
                CPY(imm);
                break;
            case 0xD:
                BNE(imm);
                break;
            case 0xE:
                CPX(imm);
                break;
            case 0xF:
                BEQ(imm);
                break;
            }
            break;

        //********************************************************
        // BIT, STY, LDY, CPY, CPX
        case 0x4:
        case 0xC:
            switch (lo)
            {
            case 0x4:
                if (hi % 2 == 0)
                {
                    mode = ZERO_PAGE;
                    imm = (ins >> 16) & 0xFF;
                }
                else
                {
                    mode = ZERO_PAGE_X;
                    imm = (ins >> 16) & 0xFF;
                }
                break;
            case 0xC:
                if (hi % 2 == 0)
                {
                    mode = ABSOLUTE;
                    imm = (ins >> 8) & 0xFFFF;
                }
                else
                {
                    mode = ABSOLUTE_X;
                    imm = (ins >> 8) & 0xFFFF;
                }
                break;
            }
            imm = address_to_immediate(imm, mode);
            switch (hi / 2)
            {
            case 0x1:
                BIT(imm);
                break;
            case 0x4:
                STY(imm);
                break;
            case 0x5:
                LDY(imm);
                break;
            case 0x6:
                CPY(imm);
                break;
            case 0x7:
                CPX(imm);
                break;
            }
            break;

        //********************************************************
        //  ORA, AND, EOR, ADC, STA, LDA, CMP, SBC
        case 0x1:
        case 0x5:
        case 0x9:
        case 0xD:
            switch (lo)
            {
            case 0x1:
                if (hi % 2 == 0)
                {
                    mode = INDIRECT_X;
                    imm = (ins >> 16) & 0xFF;
                }
                else
                {
                    mode = INDIRECT_Y;
                    imm = (ins >> 16) & 0xFF;
                }
                break;
            case 0x5:
                if (hi % 2 == 0)
                {
                    mode = ZERO_PAGE;
                    imm = (ins >> 16) & 0xFF;
                }
                else
                {
                    mode = ZERO_PAGE_X;
                    imm = (ins >> 16) & 0xFF;
                }
                break;
            case 0x9:
                if (hi % 2 == 0)
                {
                    mode = IMMEDIATE;
                    imm = (ins >> 16) & 0xFF;
                }
                else
                {
                    mode = ABSOLUTE_Y;
                    imm = (ins >> 8) & 0xFFFF;
                }
                break;
            case 0xD:
                if (hi % 2 == 0)
                {
                    mode = ABSOLUTE;
                    imm = (ins >> 8) & 0xFFFF;
                }
                else
                {
                    mode = ABSOLUTE_X;
                    imm = (ins >> 8) & 0xFFFF;
                }
                break;
            }
            imm = address_to_immediate(imm, mode);
            switch (hi / 2)
            {
            case 0x0:
                ORA(imm);
                break;
            case 0x1:
                AND(imm);
                break;
            case 0x2:
                EOR(imm);
                break;
            case 0x3:
                ADC(imm);
                break;
            case 0x4:
                STA(imm);
                break;
            case 0x5:
                LDA(imm);
                break;
            case 0x6:
                CMP(imm);
                break;
            case 0x7:
                SBC(imm);
                break;
            }
            break;

        //********************************************************
        // ASL, ROL, LSR, ROR, STX, LDX, DEC, INC
        case 0x2:
        case 0x6:
        case 0xA:
        case 0xE:
            switch (lo)
            {
            case 0x2:
                mode = IMMEDIATE;
                imm = (ins >> 16) & 0xFF;
                break;
            case 0x6:
                if (hi % 2 == 0)
                {
                    mode = ZERO_PAGE;
                    imm = (ins >> 16) & 0xFF;
                }
                else
                {
                    if (hi == 0x9 || hi == 0xB)
                    {
                        mode = ZERO_PAGE_Y;
                    }
                    else
                    {
                        mode = ZERO_PAGE_X;
                    }
                    imm = (ins >> 16) & 0xFF;
                }
                break;
            case 0xA:
                mode = ACCUMULATOR;
                break;
            case 0xE:
                if (hi % 2 == 0)
                {
                    mode = ABSOLUTE;
                    imm = (ins >> 8) & 0xFFFF;
                }
                else
                {
                    if (hi == 0xB)
                    {
                        mode = ABSOLUTE_Y;
                    }
                    else
                    {
                        mode = ABSOLUTE_X;
                    }
                    imm = (ins >> 8) & 0xFFFF;
                }
                break;
            }
            imm = address_to_immediate(imm, mode);
            switch (hi / 2)
            {
            case 0x0:
                ASL(imm);
                break;
            case 0x1:
                ROL(imm);
                break;
            case 0x2:
                LSR(imm);
                break;
            case 0x3:
                ROR(imm);
                break;
            case 0x4:
                STX(imm);
                break;
            case 0x5:
                LDX(imm);
                break;
            case 0x6:
                DEC(imm);
                break;
            case 0x7:
                INC(imm);
                break;
            }
            break;
        }
    }

    // how much to increment the program counter
    switch (mode)
    {
    case IMPLIED:
    case ACCUMULATOR:
        return 1;
    case IMMEDIATE:
    case ZERO_PAGE:
    case ZERO_PAGE_X:
    case ZERO_PAGE_Y:
    case INDIRECT_X:
    case INDIRECT_Y:
    case RELATIVE:
        return 2;
    case ABSOLUTE:
    case ABSOLUTE_X:
    case ABSOLUTE_Y:
    case INDIRECT:
        return 3;
    }
}

uint16_t load_word(uint16_t imm){
    return (((uint16_t) memory[imm+1]) << 8) | memory[imm];
}

int address_to_immediate(uint16_t imm, AddressingMode mode)
{
    switch (mode)
    {
    case IMPLIED:
        imm = 0;
        break;
    case ACCUMULATOR:
        imm = regs.A;
        break;
    case IMMEDIATE:
        imm = imm;
        break;
    case ABSOLUTE:
        imm = (imm & 0xFF) << 8 | (imm >> 8);
        imm = memory[imm];
        break;
    case ABSOLUTE_X:
        imm = (imm & 0xFF) << 8 | (imm >> 8);
        imm = memory[imm + regs.X];
        break;
    case ABSOLUTE_Y:
        imm = (imm & 0xFF) << 8 | (imm >> 8);
        imm = memory[imm + regs.Y];
        break;
    case ZERO_PAGE:
        imm = memory[imm];
        break;
    case ZERO_PAGE_X:
        imm = memory[(uint8_t)(imm + regs.X)];
        break;
    case ZERO_PAGE_Y:
        imm = memory[(uint8_t)(imm + regs.Y)];
        break;
    case INDIRECT:
        imm = (imm & 0xFF) << 8 | (imm >> 8);
        imm = load_word(imm);
        imm = load_word(imm);
        break;
    case INDIRECT_X:
        imm = (uint8_t)(imm + regs.X);
        imm = load_word(imm);
        imm = memory[imm];
        break;
    case INDIRECT_Y:
        imm = load_word(imm) + regs.Y;
        imm = memory[imm];
        break;
    case RELATIVE:
        imm = regs.PC + imm;
        break;
    }
    return imm;
}

int main()
{
    init();
    decode_and_execute(0);
    printf("hello");
    return 0;
}