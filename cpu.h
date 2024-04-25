#pragma once

#include <cstdint>
#include <string>
#include <vector>

class CPU {
public:
    CPU();
    // ~CPU();

    struct Flags {
        bool C; // Carry
        bool Z; // Zero
        bool I; // Interrupt
        bool D; // Decimal
        bool B; // Break
        bool X; // Unused
        bool V; // Overflow
        bool N; // Negative
    };

    struct Registers {
        uint8_t A; // Accumulator
        uint8_t X; // X register
        uint8_t Y; // Y register
        uint16_t PC; // Program counter
        uint8_t SP; // Stack pointer
        Flags flags; // Status register
    } regs;

    // Addressing modes
    bool IMP(); // Implied
    bool IMM(); // Immediate
    bool ZPG(); // Zero-page
    bool ZPX(); // Zero-page X
    bool ZPY(); // Zero-page Y
    bool REL(); // Relative
    bool ABS(); // Absolute
    bool ABX(); // Absolute X
    bool ABY(); // Absolute Y
    bool IND(); // Indirect
    bool IDX(); // Indirect X
    bool IDY(); // Indirect Y

    // Instructions
    bool ADC();	bool AND();	
    bool ASL();	bool BCC();
	bool BCS();	bool BEQ();	
    bool BIT();	bool BMI();
	bool BNE();	bool BPL();	
    bool BRK();	bool BVC();
	bool BVS();	bool CLC();	
    bool CLD();	bool CLI();
	bool CLV();	bool CMP();	
    bool CPX();	bool CPY();
	bool DEC();	bool DEX();	
    bool DEY();	bool EOR();
	bool INC();	bool INX();	
    bool INY();	bool JMP();
	bool JSR();	bool LDA();	
    bool LDX();	bool LDY();
	bool LSR();	bool NOP();	
    bool ORA();	bool PHA();
	bool PHP();	bool PLA();	
    bool PLP();	bool ROL();
	bool ROR();	bool RTI();	
    bool RTS();	bool SBC();
	bool SEC();	bool SED();	
    bool SEI();	bool STA();
	bool STX();	bool STY();	
    bool TAX();	bool TAY();
	bool TSX();	bool TXA();	
    bool TXS();	bool TYA();
    bool XXX(); // Illegal

    void clock();

    // Interrupts
    void reset();
    void interruptRequest();
    void nonmaskableInterrupt();

    // Instruction information
    uint8_t opcode = 0;
    uint16_t absolute_addr = 0;
    uint16_t relative_addr = 0;
    uint8_t data = 0;
    uint8_t cycles = 0;

private:
    const uint16_t STACK_BASE = 0x0100;
    const uint16_t PC_START = 0xFFFC;
    const uint16_t SP_START = 0xFF;
    const uint16_t IRQ_ADDR = 0xFFFE;
    const uint16_t NMI_ADDR = 0xFFFA;
    const int RESET_CYCLES = 8;
    const int IRQ_CYCLES = 7;
    const int NMI_CYCLES = 8;

    // Stack
    uint8_t pull();
    void push(uint8_t value);
    void pullFlags();
    void pushFlags();

    void print_regs();

    struct Instruction {
        std::string name;
        bool(CPU::*function)(void) = nullptr; // Instruction function
        bool(CPU::*mode)(void) = nullptr; // Addressing mode
        uint8_t cycles = 0;
    };
    
    std::vector<Instruction> op_table; // Opcode table
};

