#include "cpu.h"

CPU::CPU() {
    // Opcode table
    // Source: https://github.com/OneLoneCoder/olcNES)
    // Format: name, function, addressing mode, cycle count
	op_table = {
		{ "BRK", &CPU::BRK, &CPU::IMM, 7 },{ "ORA", &CPU::ORA, &CPU::INX, 6 },{ "???", &CPU::XXX, &CPU::IMP, 2 },{ "???", &CPU::XXX, &CPU::IMP, 8 },{ "???", &CPU::NOP, &CPU::IMP, 3 },{ "ORA", &CPU::ORA, &CPU::ZPG, 3 },{ "ASL", &CPU::ASL, &CPU::ZPG, 5 },{ "???", &CPU::XXX, &CPU::IMP, 5 },{ "PHP", &CPU::PHP, &CPU::IMP, 3 },{ "ORA", &CPU::ORA, &CPU::IMM, 2 },{ "ASL", &CPU::ASL, &CPU::IMP, 2 },{ "???", &CPU::XXX, &CPU::IMP, 2 },{ "???", &CPU::NOP, &CPU::IMP, 4 },{ "ORA", &CPU::ORA, &CPU::ABS, 4 },{ "ASL", &CPU::ASL, &CPU::ABS, 6 },{ "???", &CPU::XXX, &CPU::IMP, 6 },
		{ "BPL", &CPU::BPL, &CPU::REL, 2 },{ "ORA", &CPU::ORA, &CPU::INY, 5 },{ "???", &CPU::XXX, &CPU::IMP, 2 },{ "???", &CPU::XXX, &CPU::IMP, 8 },{ "???", &CPU::NOP, &CPU::IMP, 4 },{ "ORA", &CPU::ORA, &CPU::ZPX, 4 },{ "ASL", &CPU::ASL, &CPU::ZPX, 6 },{ "???", &CPU::XXX, &CPU::IMP, 6 },{ "CLC", &CPU::CLC, &CPU::IMP, 2 },{ "ORA", &CPU::ORA, &CPU::ABY, 4 },{ "???", &CPU::NOP, &CPU::IMP, 2 },{ "???", &CPU::XXX, &CPU::IMP, 7 },{ "???", &CPU::NOP, &CPU::IMP, 4 },{ "ORA", &CPU::ORA, &CPU::ABX, 4 },{ "ASL", &CPU::ASL, &CPU::ABX, 7 },{ "???", &CPU::XXX, &CPU::IMP, 7 },
		{ "JSR", &CPU::JSR, &CPU::ABS, 6 },{ "AND", &CPU::AND, &CPU::INX, 6 },{ "???", &CPU::XXX, &CPU::IMP, 2 },{ "???", &CPU::XXX, &CPU::IMP, 8 },{ "BIT", &CPU::BIT, &CPU::ZPG, 3 },{ "AND", &CPU::AND, &CPU::ZPG, 3 },{ "ROL", &CPU::ROL, &CPU::ZPG, 5 },{ "???", &CPU::XXX, &CPU::IMP, 5 },{ "PLP", &CPU::PLP, &CPU::IMP, 4 },{ "AND", &CPU::AND, &CPU::IMM, 2 },{ "ROL", &CPU::ROL, &CPU::IMP, 2 },{ "???", &CPU::XXX, &CPU::IMP, 2 },{ "BIT", &CPU::BIT, &CPU::ABS, 4 },{ "AND", &CPU::AND, &CPU::ABS, 4 },{ "ROL", &CPU::ROL, &CPU::ABS, 6 },{ "???", &CPU::XXX, &CPU::IMP, 6 },
		{ "BMI", &CPU::BMI, &CPU::REL, 2 },{ "AND", &CPU::AND, &CPU::INY, 5 },{ "???", &CPU::XXX, &CPU::IMP, 2 },{ "???", &CPU::XXX, &CPU::IMP, 8 },{ "???", &CPU::NOP, &CPU::IMP, 4 },{ "AND", &CPU::AND, &CPU::ZPX, 4 },{ "ROL", &CPU::ROL, &CPU::ZPX, 6 },{ "???", &CPU::XXX, &CPU::IMP, 6 },{ "SEC", &CPU::SEC, &CPU::IMP, 2 },{ "AND", &CPU::AND, &CPU::ABY, 4 },{ "???", &CPU::NOP, &CPU::IMP, 2 },{ "???", &CPU::XXX, &CPU::IMP, 7 },{ "???", &CPU::NOP, &CPU::IMP, 4 },{ "AND", &CPU::AND, &CPU::ABX, 4 },{ "ROL", &CPU::ROL, &CPU::ABX, 7 },{ "???", &CPU::XXX, &CPU::IMP, 7 },
		{ "RTI", &CPU::RTI, &CPU::IMP, 6 },{ "EOR", &CPU::EOR, &CPU::INX, 6 },{ "???", &CPU::XXX, &CPU::IMP, 2 },{ "???", &CPU::XXX, &CPU::IMP, 8 },{ "???", &CPU::NOP, &CPU::IMP, 3 },{ "EOR", &CPU::EOR, &CPU::ZPG, 3 },{ "LSR", &CPU::LSR, &CPU::ZPG, 5 },{ "???", &CPU::XXX, &CPU::IMP, 5 },{ "PHA", &CPU::PHA, &CPU::IMP, 3 },{ "EOR", &CPU::EOR, &CPU::IMM, 2 },{ "LSR", &CPU::LSR, &CPU::IMP, 2 },{ "???", &CPU::XXX, &CPU::IMP, 2 },{ "JMP", &CPU::JMP, &CPU::ABS, 3 },{ "EOR", &CPU::EOR, &CPU::ABS, 4 },{ "LSR", &CPU::LSR, &CPU::ABS, 6 },{ "???", &CPU::XXX, &CPU::IMP, 6 },
		{ "BVC", &CPU::BVC, &CPU::REL, 2 },{ "EOR", &CPU::EOR, &CPU::INY, 5 },{ "???", &CPU::XXX, &CPU::IMP, 2 },{ "???", &CPU::XXX, &CPU::IMP, 8 },{ "???", &CPU::NOP, &CPU::IMP, 4 },{ "EOR", &CPU::EOR, &CPU::ZPX, 4 },{ "LSR", &CPU::LSR, &CPU::ZPX, 6 },{ "???", &CPU::XXX, &CPU::IMP, 6 },{ "CLI", &CPU::CLI, &CPU::IMP, 2 },{ "EOR", &CPU::EOR, &CPU::ABY, 4 },{ "???", &CPU::NOP, &CPU::IMP, 2 },{ "???", &CPU::XXX, &CPU::IMP, 7 },{ "???", &CPU::NOP, &CPU::IMP, 4 },{ "EOR", &CPU::EOR, &CPU::ABX, 4 },{ "LSR", &CPU::LSR, &CPU::ABX, 7 },{ "???", &CPU::XXX, &CPU::IMP, 7 },
		{ "RTS", &CPU::RTS, &CPU::IMP, 6 },{ "ADC", &CPU::ADC, &CPU::INX, 6 },{ "???", &CPU::XXX, &CPU::IMP, 2 },{ "???", &CPU::XXX, &CPU::IMP, 8 },{ "???", &CPU::NOP, &CPU::IMP, 3 },{ "ADC", &CPU::ADC, &CPU::ZPG, 3 },{ "ROR", &CPU::ROR, &CPU::ZPG, 5 },{ "???", &CPU::XXX, &CPU::IMP, 5 },{ "PLA", &CPU::PLA, &CPU::IMP, 4 },{ "ADC", &CPU::ADC, &CPU::IMM, 2 },{ "ROR", &CPU::ROR, &CPU::IMP, 2 },{ "???", &CPU::XXX, &CPU::IMP, 2 },{ "JMP", &CPU::JMP, &CPU::IND, 5 },{ "ADC", &CPU::ADC, &CPU::ABS, 4 },{ "ROR", &CPU::ROR, &CPU::ABS, 6 },{ "???", &CPU::XXX, &CPU::IMP, 6 },
		{ "BVS", &CPU::BVS, &CPU::REL, 2 },{ "ADC", &CPU::ADC, &CPU::INY, 5 },{ "???", &CPU::XXX, &CPU::IMP, 2 },{ "???", &CPU::XXX, &CPU::IMP, 8 },{ "???", &CPU::NOP, &CPU::IMP, 4 },{ "ADC", &CPU::ADC, &CPU::ZPX, 4 },{ "ROR", &CPU::ROR, &CPU::ZPX, 6 },{ "???", &CPU::XXX, &CPU::IMP, 6 },{ "SEI", &CPU::SEI, &CPU::IMP, 2 },{ "ADC", &CPU::ADC, &CPU::ABY, 4 },{ "???", &CPU::NOP, &CPU::IMP, 2 },{ "???", &CPU::XXX, &CPU::IMP, 7 },{ "???", &CPU::NOP, &CPU::IMP, 4 },{ "ADC", &CPU::ADC, &CPU::ABX, 4 },{ "ROR", &CPU::ROR, &CPU::ABX, 7 },{ "???", &CPU::XXX, &CPU::IMP, 7 },
		{ "???", &CPU::NOP, &CPU::IMP, 2 },{ "STA", &CPU::STA, &CPU::INX, 6 },{ "???", &CPU::NOP, &CPU::IMP, 2 },{ "???", &CPU::XXX, &CPU::IMP, 6 },{ "STY", &CPU::STY, &CPU::ZPG, 3 },{ "STA", &CPU::STA, &CPU::ZPG, 3 },{ "STX", &CPU::STX, &CPU::ZPG, 3 },{ "???", &CPU::XXX, &CPU::IMP, 3 },{ "DEY", &CPU::DEY, &CPU::IMP, 2 },{ "???", &CPU::NOP, &CPU::IMP, 2 },{ "TXA", &CPU::TXA, &CPU::IMP, 2 },{ "???", &CPU::XXX, &CPU::IMP, 2 },{ "STY", &CPU::STY, &CPU::ABS, 4 },{ "STA", &CPU::STA, &CPU::ABS, 4 },{ "STX", &CPU::STX, &CPU::ABS, 4 },{ "???", &CPU::XXX, &CPU::IMP, 4 },
		{ "BCC", &CPU::BCC, &CPU::REL, 2 },{ "STA", &CPU::STA, &CPU::INY, 6 },{ "???", &CPU::XXX, &CPU::IMP, 2 },{ "???", &CPU::XXX, &CPU::IMP, 6 },{ "STY", &CPU::STY, &CPU::ZPX, 4 },{ "STA", &CPU::STA, &CPU::ZPX, 4 },{ "STX", &CPU::STX, &CPU::ZPY, 4 },{ "???", &CPU::XXX, &CPU::IMP, 4 },{ "TYA", &CPU::TYA, &CPU::IMP, 2 },{ "STA", &CPU::STA, &CPU::ABY, 5 },{ "TXS", &CPU::TXS, &CPU::IMP, 2 },{ "???", &CPU::XXX, &CPU::IMP, 5 },{ "???", &CPU::NOP, &CPU::IMP, 5 },{ "STA", &CPU::STA, &CPU::ABX, 5 },{ "???", &CPU::XXX, &CPU::IMP, 5 },{ "???", &CPU::XXX, &CPU::IMP, 5 },
		{ "LDY", &CPU::LDY, &CPU::IMM, 2 },{ "LDA", &CPU::LDA, &CPU::INX, 6 },{ "LDX", &CPU::LDX, &CPU::IMM, 2 },{ "???", &CPU::XXX, &CPU::IMP, 6 },{ "LDY", &CPU::LDY, &CPU::ZPG, 3 },{ "LDA", &CPU::LDA, &CPU::ZPG, 3 },{ "LDX", &CPU::LDX, &CPU::ZPG, 3 },{ "???", &CPU::XXX, &CPU::IMP, 3 },{ "TAY", &CPU::TAY, &CPU::IMP, 2 },{ "LDA", &CPU::LDA, &CPU::IMM, 2 },{ "TAX", &CPU::TAX, &CPU::IMP, 2 },{ "???", &CPU::XXX, &CPU::IMP, 2 },{ "LDY", &CPU::LDY, &CPU::ABS, 4 },{ "LDA", &CPU::LDA, &CPU::ABS, 4 },{ "LDX", &CPU::LDX, &CPU::ABS, 4 },{ "???", &CPU::XXX, &CPU::IMP, 4 },
		{ "BCS", &CPU::BCS, &CPU::REL, 2 },{ "LDA", &CPU::LDA, &CPU::INY, 5 },{ "???", &CPU::XXX, &CPU::IMP, 2 },{ "???", &CPU::XXX, &CPU::IMP, 5 },{ "LDY", &CPU::LDY, &CPU::ZPX, 4 },{ "LDA", &CPU::LDA, &CPU::ZPX, 4 },{ "LDX", &CPU::LDX, &CPU::ZPY, 4 },{ "???", &CPU::XXX, &CPU::IMP, 4 },{ "CLV", &CPU::CLV, &CPU::IMP, 2 },{ "LDA", &CPU::LDA, &CPU::ABY, 4 },{ "TSX", &CPU::TSX, &CPU::IMP, 2 },{ "???", &CPU::XXX, &CPU::IMP, 4 },{ "LDY", &CPU::LDY, &CPU::ABX, 4 },{ "LDA", &CPU::LDA, &CPU::ABX, 4 },{ "LDX", &CPU::LDX, &CPU::ABY, 4 },{ "???", &CPU::XXX, &CPU::IMP, 4 },
		{ "CPY", &CPU::CPY, &CPU::IMM, 2 },{ "CMP", &CPU::CMP, &CPU::INX, 6 },{ "???", &CPU::NOP, &CPU::IMP, 2 },{ "???", &CPU::XXX, &CPU::IMP, 8 },{ "CPY", &CPU::CPY, &CPU::ZPG, 3 },{ "CMP", &CPU::CMP, &CPU::ZPG, 3 },{ "DEC", &CPU::DEC, &CPU::ZPG, 5 },{ "???", &CPU::XXX, &CPU::IMP, 5 },{ "INY", &CPU::INY, &CPU::IMP, 2 },{ "CMP", &CPU::CMP, &CPU::IMM, 2 },{ "DEX", &CPU::DEX, &CPU::IMP, 2 },{ "???", &CPU::XXX, &CPU::IMP, 2 },{ "CPY", &CPU::CPY, &CPU::ABS, 4 },{ "CMP", &CPU::CMP, &CPU::ABS, 4 },{ "DEC", &CPU::DEC, &CPU::ABS, 6 },{ "???", &CPU::XXX, &CPU::IMP, 6 },
		{ "BNE", &CPU::BNE, &CPU::REL, 2 },{ "CMP", &CPU::CMP, &CPU::INY, 5 },{ "???", &CPU::XXX, &CPU::IMP, 2 },{ "???", &CPU::XXX, &CPU::IMP, 8 },{ "???", &CPU::NOP, &CPU::IMP, 4 },{ "CMP", &CPU::CMP, &CPU::ZPX, 4 },{ "DEC", &CPU::DEC, &CPU::ZPX, 6 },{ "???", &CPU::XXX, &CPU::IMP, 6 },{ "CLD", &CPU::CLD, &CPU::IMP, 2 },{ "CMP", &CPU::CMP, &CPU::ABY, 4 },{ "NOP", &CPU::NOP, &CPU::IMP, 2 },{ "???", &CPU::XXX, &CPU::IMP, 7 },{ "???", &CPU::NOP, &CPU::IMP, 4 },{ "CMP", &CPU::CMP, &CPU::ABX, 4 },{ "DEC", &CPU::DEC, &CPU::ABX, 7 },{ "???", &CPU::XXX, &CPU::IMP, 7 },
		{ "CPX", &CPU::CPX, &CPU::IMM, 2 },{ "SBC", &CPU::SBC, &CPU::INX, 6 },{ "???", &CPU::NOP, &CPU::IMP, 2 },{ "???", &CPU::XXX, &CPU::IMP, 8 },{ "CPX", &CPU::CPX, &CPU::ZPG, 3 },{ "SBC", &CPU::SBC, &CPU::ZPG, 3 },{ "INC", &CPU::INC, &CPU::ZPG, 5 },{ "???", &CPU::XXX, &CPU::IMP, 5 },{ "INX", &CPU::INX, &CPU::IMP, 2 },{ "SBC", &CPU::SBC, &CPU::IMM, 2 },{ "NOP", &CPU::NOP, &CPU::IMP, 2 },{ "???", &CPU::SBC, &CPU::IMP, 2 },{ "CPX", &CPU::CPX, &CPU::ABS, 4 },{ "SBC", &CPU::SBC, &CPU::ABS, 4 },{ "INC", &CPU::INC, &CPU::ABS, 6 },{ "???", &CPU::XXX, &CPU::IMP, 6 },
		{ "BEQ", &CPU::BEQ, &CPU::REL, 2 },{ "SBC", &CPU::SBC, &CPU::INY, 5 },{ "???", &CPU::XXX, &CPU::IMP, 2 },{ "???", &CPU::XXX, &CPU::IMP, 8 },{ "???", &CPU::NOP, &CPU::IMP, 4 },{ "SBC", &CPU::SBC, &CPU::ZPX, 4 },{ "INC", &CPU::INC, &CPU::ZPX, 6 },{ "???", &CPU::XXX, &CPU::IMP, 6 },{ "SED", &CPU::SED, &CPU::IMP, 2 },{ "SBC", &CPU::SBC, &CPU::ABY, 4 },{ "NOP", &CPU::NOP, &CPU::IMP, 2 },{ "???", &CPU::XXX, &CPU::IMP, 7 },{ "???", &CPU::NOP, &CPU::IMP, 4 },{ "SBC", &CPU::SBC, &CPU::ABX, 4 },{ "INC", &CPU::INC, &CPU::ABX, 7 },{ "???", &CPU::XXX, &CPU::IMP, 7 },
	};
}

uint8_t CPU::pull() {
    regs.SP++;
    return read(STACK_BASE + regs.SP);
}

void CPU::push(uint8_t value) {
    write(STACK_BASE + regs.SP, value);
    regs.SP--;
}

void CPU::pullFlags() {
    uint8_t flags = pull();
    regs.flags.C = flags >> 7;
    regs.flags.Z = flags >> 6;
    regs.flags.I = flags >> 5;
    regs.flags.D = flags >> 4;
    regs.flags.B = flags >> 3;
    regs.flags.X = flags >> 2;
    regs.flags.V = flags >> 1;
    regs.flags.N = flags >> 0;
}

void CPU::pushFlags() {
    uint8_t flags = (regs.flags.C << 7) | (regs.flags.Z << 6) | (regs.flags.I << 5) | (regs.flags.D << 4) |
                    (regs.flags.B << 3) | (regs.flags.X << 2) | (regs.flags.V << 1) | (regs.flags.N << 0);
    push(flags);
}

void CPU::clock() {
    if (cycles == 0) {
        // Fetch
        opcode = read(regs.PC); // First byte of instruction
        regs.PC++;

        // Decode
        bool add_cycle1 = (this->*op_table[opcode].mode)();

        // Execute
        bool add_cycle2 = (this->*op_table[opcode].function)();
        cycles = op_table[opcode].cycles + (add_cycle1 && add_cycle2);
    }

    cycles--;
}


/********** Addressing modes **********/

bool CPU::IMP() {
    data = regs.A;
    return 0;
}

bool CPU::IMM() {
    absolute_addr = regs.PC;
    regs.PC++;
    data = read(absolute_addr);
    return 0;
}

bool CPU::ZPG() {
    absolute_addr = read(regs.PC);
    regs.PC++;
    data = read(absolute_addr);
    return 0;
}

bool CPU::ZPX() {
    absolute_addr = read(regs.PC) + regs.X;
    regs.PC++;
    data = read(absolute_addr);
    return 0;
}

bool CPU::ZPY() {
    absolute_addr = read(regs.PC) + regs.Y;
    regs.PC++;
    data = read(absolute_addr);
    return 0;
}

bool CPU::REL() {
    relative_addr = read(regs.PC);
    regs.PC++;

    if ((relative_addr >> 7) == 1) {
        // Sign extend to 16 bits
        relative_addr |= 0xFF00;
    }
    data = read(absolute_addr);
    return 0;
}

bool CPU::ABS() {
    uint8_t lo = read(regs.PC);
    regs.PC++;
    uint8_t hi = read(regs.PC);
    regs.PC++;
    absolute_addr = (hi << 8) | lo;
    data = read(absolute_addr);
    return 0;
}

bool CPU::ABX() {
    uint8_t lo = read(regs.PC);
    regs.PC++;
    uint8_t hi = read(regs.PC);
    regs.PC++;
    absolute_addr = ((hi << 8) | lo) + regs.X;
    data = read(absolute_addr);

    if ((absolute_addr >> 8) != hi) {
        // New page, additional cycle may be needed
        return 1;
    }
    else {
        return 0;
    }
}

bool CPU::ABY() {
    uint8_t lo = read(regs.PC);
    regs.PC++;
    uint8_t hi = read(regs.PC);
    regs.PC++;
    absolute_addr = ((hi << 8) | lo) + regs.Y;
    data = read(absolute_addr);

    if ((absolute_addr >> 8) != hi) {
        // New page, additional cycle may be needed
        return 1;
    }
    else {
        return 0;
    }
}

bool CPU::IND() {
    uint8_t lo = read(regs.PC);
    regs.PC++;
    uint8_t hi = read(regs.PC);
    regs.PC++;
    uint16_t addr = ((hi << 8) | lo);

    if (lo == 0x00FF) {
        // NES page boundary bug
        absolute_addr = (read(addr & 0xFF00) << 8) | read(addr);
    }
    else {
        absolute_addr = (read(addr + 1) << 8) | read(addr);
    }
    data = read(absolute_addr);
    return 0;
}

bool CPU::INX() {
    uint8_t addr = read(regs.PC);
    regs.PC++;
    uint8_t lo = read(addr + regs.X);
    uint8_t hi = read(addr + regs.X + 1);
    absolute_addr = (hi << 8) | lo;
    data = read(absolute_addr);
    return 0;
}

bool CPU::INY() {
    uint8_t addr = read(regs.PC);
    regs.PC++;
    uint8_t lo = read(addr);
    uint8_t hi = read(addr + 1);
    absolute_addr = ((hi << 8) | lo) + regs.Y;
    data = read(absolute_addr);

    if ((absolute_addr >> 8) != hi) {
        // New page, additional cycle may be needed
        return 1;
    }
    else {
        return 0;
    }
}


/********** Instructions **********/

bool CPU::ADC() {
    uint16_t result = (uint16_t) regs.A + (uint16_t) data + (uint16_t) regs.flags.C;
    regs.flags.N = ((result >> 7) & 1) == 1;
    regs.flags.Z = (result & 0x00FF) == 0;
    regs.flags.C = (result >> 8) == 1;
    regs.flags.V = ((regs.A >> 7) == (data >> 7)) && ((regs.A >> 7) != ((result >> 7) & 1));
    regs.A = result;
    return 1;
}

bool CPU::AND() {
    regs.A = regs.A & data;
    regs.flags.N = (regs.A >> 7) == 1;
    regs.flags.Z = regs.A == 0;
    return 1;
}

bool CPU::ASL() {
    regs.flags.C = (data >> 7) == 1;
    uint8_t result = data << 1;
    regs.flags.N = (result >> 7) == 1;
    regs.flags.Z = result == 0;

    if (op_table[opcode].mode == &CPU::IMP) {
        regs.A = result;
    }
    else {
        write(absolute_addr, result);
    }
    return 0;
}

bool CPU::BCC() {
    if (regs.flags.C == 0) {
        cycles++;
        absolute_addr = regs.PC + relative_addr;

        if ((absolute_addr & 0xFF00) != (regs.PC & 0xFF00)) {
            cycles++;
        }

        regs.PC = absolute_addr;
    }
    return 0;
}

bool CPU::BCS() {
    if (regs.flags.C == 1) {
        cycles++;
        absolute_addr = regs.PC + relative_addr;

        if ((absolute_addr & 0xFF00) != (regs.PC & 0xFF00)) {
            cycles++;
        }

        regs.PC = absolute_addr;
    }
    return 0;
}

bool CPU::BEQ() {
    if (regs.flags.Z == 1) {
        cycles++;
        absolute_addr = regs.PC + relative_addr;

        if ((absolute_addr & 0xFF00) != (regs.PC & 0xFF00)) {
            cycles++;
        }

        regs.PC = absolute_addr;
    }
    return 0;
}

bool CPU::BIT() {
    regs.flags.N = data >> 7;
    regs.flags.Z = (regs.A & data) == 0;
    regs.flags.V = (data >> 6) & 1;
    return 0;
}

bool CPU::BMI() {
    if (regs.flags.N == 1) {
        cycles++;
        absolute_addr = regs.PC + relative_addr;

        if ((absolute_addr & 0xFF00) != (regs.PC & 0xFF00)) {
            cycles++;
        }

        regs.PC = absolute_addr;
    }
    return 0;
}

bool CPU::BNE() {
    if (regs.flags.Z == 0) {
        cycles++;
        absolute_addr = regs.PC + relative_addr;

        if ((absolute_addr & 0xFF00) != (regs.PC & 0xFF00)) {
            cycles++;
        }

        regs.PC = absolute_addr;
    }
    return 0;
}

bool CPU::BPL() {
    if (regs.flags.N == 0) {
        cycles++;
        absolute_addr = regs.PC + relative_addr;

        if ((absolute_addr & 0xFF00) != (regs.PC & 0xFF00)) {
            cycles++;
        }

        regs.PC = absolute_addr;
    }
    return 0;
}

bool CPU::BRK() {

}

bool CPU::BVC() {
    if (regs.flags.V == 0) {
        cycles++;
        absolute_addr = regs.PC + relative_addr;

        if ((absolute_addr & 0xFF00) != (regs.PC & 0xFF00)) {
            cycles++;
        }

        regs.PC = absolute_addr;
    }
    return 0;
}

bool CPU::BVS() {
    if (regs.flags.V == 1) {
        cycles++;
        absolute_addr = regs.PC + relative_addr;

        if ((absolute_addr & 0xFF00) != (regs.PC & 0xFF00)) {
            cycles++;
        }

        regs.PC = absolute_addr;
    }
    return 0;
}

bool CPU::CLC() {
    regs.flags.C = 0;
    return 0;
}

bool CPU::CLD() {
    regs.flags.D = 0;
    return 0;
}

bool CPU::CLI() {
    regs.flags.I = 0;
    return 0;
}

bool CPU::CLV() {
    regs.flags.V = 0;
    return 0;
}

bool CPU::CMP() {
    regs.flags.N = ((regs.A - data) >> 7) == 1;
    regs.flags.Z = regs.A == data;
    regs.flags.C = regs.A >= data;
    return 1;
}

bool CPU::CPX() {
    regs.flags.N = ((regs.X - data) >> 7) == 1;
    regs.flags.Z = regs.X == data;
    regs.flags.C = regs.X >= data;
    return 0;
}

bool CPU::CPY() {
    regs.flags.N = ((regs.Y - data) >> 7) == 1;
    regs.flags.Z = regs.Y == data;
    regs.flags.C = regs.Y >= data;
    return 0;
}

bool CPU::DEC() {
    uint8_t result = data - 1;
    write(absolute_addr, result);
    regs.flags.N = (result >> 7) == 1;
    regs.flags.Z = result == 0;
    return 0;
}

bool CPU::DEX() {
    regs.X--;
    regs.flags.N = (regs.X >> 7) == 1;
    regs.flags.Z = regs.X == 0;
    return 0;
}

bool CPU::DEY() {
    regs.Y--;
    regs.flags.N = (regs.Y >> 7) == 1;
    regs.flags.Z = regs.Y == 0;
    return 0;
}

bool CPU::EOR() {
    regs.A ^= data;
    regs.flags.N = (regs.A >> 7) == 1;
    regs.flags.Z = regs.A == 0;
    return 1;
}

bool CPU::INC() {
    uint8_t result = data + 1;
    write(absolute_addr, result);
    regs.flags.N = (result >> 7) == 1;
    regs.flags.Z = result == 0;
    return 0;
}

bool CPU::INX() {
    regs.X++;
    regs.flags.N = (regs.X >> 7) == 1;
    regs.flags.Z = regs.X == 0;
    return 0;
}

bool CPU::INY() {
    regs.Y++;
    regs.flags.N = (regs.Y >> 7) == 1;
    regs.flags.Z = regs.Y == 0;
    return 0;
}

bool CPU::JMP() {
    regs.PC = absolute_addr;
    return 0;   
}

bool CPU::JSR() {
    regs.PC--;
    push(regs.PC >> 8);
    push(regs.PC);
    regs.PC = absolute_addr;
    return 0;
}

bool CPU::LDA() {
    regs.A = data;
    regs.flags.N = (regs.A >> 7) == 1;
    regs.flags.Z = regs.A == 0;
    return 1;
}

bool CPU::LDX() {
    regs.X = data;
    regs.flags.N = (regs.X >> 7) == 1;
    regs.flags.Z = regs.X == 0;
    return 1;
}

bool CPU::LDY() {
    regs.Y = data;
    regs.flags.N = (regs.Y >> 7) == 1;
    regs.flags.Z = regs.Y == 0;
    return 1;
}

bool CPU::LSR() {
    regs.flags.C = data & 1;
    uint8_t result = data >> 1;
    regs.flags.N = (result >> 7) == 1;
    regs.flags.Z = result == 0;

    if (op_table[opcode].mode == &CPU::IMP) {
        regs.A = result;
    }
    else {
        write(absolute_addr, result);
    }
    return 0;
}

bool CPU::NOP() {
    return 0;
}

bool CPU::ORA() {
    regs.A |= data;
    regs.flags.N = (regs.A >> 7) == 1;
    regs.flags.Z = regs.A == 0;
    return 1;
}

bool CPU::PHA() {
    push(data);
    return 0;
}

bool CPU::PHP() {
    regs.flags.B = 1;
    pushFlags();
    regs.flags.B = 0;
    return 0;
}

bool CPU::PLA() {
    regs.A = pull();
    regs.flags.N = (regs.A >> 7) == 1;
    regs.flags.Z = regs.A == 0;
    return 0;
}

bool CPU::PLP() {
    pullFlags();
    return 0;
}

bool CPU::ROL() {
    uint8_t result = (data << 1) | regs.flags.C;
    regs.flags.C = (data >> 7) == 1;
    regs.flags.N = (result >> 7) == 1;
    regs.flags.Z = result == 0;

    if (op_table[opcode].mode == &CPU::IMP) {
        regs.A = result;
    }
    else {
        write(absolute_addr, result);
    }
    return 0;
}

bool CPU::ROR() {
    uint8_t result = (regs.flags.C << 7) | (data >> 1);
    regs.flags.C = data & 1;
    regs.flags.N = (result >> 7) == 1;
    regs.flags.Z = result == 0;

    if (op_table[opcode].mode == &CPU::IMP) {
        regs.A = result;
    }
    else {
        write(absolute_addr, result);
    }
    return 0;
}

bool CPU::RTI() {
    
}

bool CPU::RTS() {
    regs.PC = pull() | (pull() << 8);
    regs.PC++;
    return 0;
}

bool CPU::SBC() {
    uint16_t result = (uint16_t) regs.A + (uint16_t) (~data) + (uint16_t) regs.flags.C;
    regs.flags.N = ((result >> 7) & 1) == 1;
    regs.flags.Z = (result & 0x00FF) == 0;
    regs.flags.C = (result >> 8) == 1;
    regs.flags.V = ((regs.A >> 7) == (data >> 7)) && ((regs.A >> 7) != ((result >> 7) & 1));
    regs.A = result;
    return 1;
}

bool CPU::SEC() {
    regs.flags.C = 1;
    return 0;
}

bool CPU::SED() {
    regs.flags.D = 1;
    return 0;
}

bool CPU::SEI() {
    regs.flags.I = 1;
    return 0;
}

bool CPU::STA() {
    write(absolute_addr, regs.A);
    return 0;
}

bool CPU::STX() {
    write(absolute_addr, regs.X);
    return 0;
}

bool CPU::STY() {
    write(absolute_addr, regs.Y);
    return 0;
}

bool CPU::TAX() {
    regs.X = regs.A;
    regs.flags.N = (regs.X >> 7) == 1;
    regs.flags.Z = regs.X == 0;
    return 0;
}

bool CPU::TAY() {
    regs.Y = regs.A;
    regs.flags.N = (regs.Y >> 7) == 1;
    regs.flags.Z = regs.Y == 0;
    return 0;
}

bool CPU::TSX() {
    regs.X = regs.SP;
    regs.flags.N = (regs.X >> 7) == 1;
    regs.flags.Z = regs.X == 0;
    return 0;
}

bool CPU::TXA() {
    regs.A = regs.X;
    regs.flags.N = (regs.A >> 7) == 1;
    regs.flags.Z = regs.A == 0;
    return 0;
}

bool CPU::TXS() {
    regs.SP = regs.X;
    return 0;
}

bool CPU::TYA() {
    regs.A = regs.Y;
    regs.flags.N = (regs.A >> 7) == 1;
    regs.flags.Z = regs.A == 0;
    return 0;
}

bool CPU::XXX() {
    return 0;
}
