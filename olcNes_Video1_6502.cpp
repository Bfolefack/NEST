/*
	olc6502 - An emulation of the 6502/2A03 processor
	"Thanks Dad for believing computers were gonna be a big deal..." - javidx9

	License (OLC-3)
	~~~~~~~~~~~~~~~

	Copyright 2018-2019 OneLoneCoder.com

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:

	1. Redistributions or derivations of source code must retain the above
	copyright notice, this list of conditions and the following disclaimer.

	2. Redistributions or derivative works in binary form must reproduce
	the above copyright notice. This list of conditions and the following
	disclaimer must be reproduced in the documentation and/or other
	materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its
	contributors may be used to endorse or promote products derived
	from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Background
	~~~~~~~~~~
	I love this microprocessor. It was at the heart of two of my favourite
	machines, the BBC Micro, and the Nintendo Entertainment System, as well
	as countless others in that era. I learnt to program on the Model B, and
	I learnt to love games on the NES, so in many ways, this processor is
	why I am the way I am today.

	In February 2019, I decided to undertake a selfish personal project and
	build a NES emulator. Ive always wanted to, and as such I've avoided
	looking at source code for such things. This made making this a real
	personal challenge. I know its been done countless times, and very likely
	in far more clever and accurate ways than mine, but I'm proud of this.

	Datasheet: http://archive.6502.org/datasheets/rockwell_r650x_r651x.pdf

	Files: olc6502.h, olc6502.cpp

	Relevant Video: https://youtu.be/8XmxKPJDGU0

	Links
	~~~~~
	YouTube:	https://www.youtube.com/javidx9
				https://www.youtube.com/javidx9extra
	Discord:	https://discord.gg/WhwHUMV
	Twitter:	https://www.twitter.com/javidx9
	Twitch:		https://www.twitch.tv/javidx9
	GitHub:		https://www.github.com/onelonecoder
	Patreon:	https://www.patreon.com/javidx9
	Homepage:	https://www.onelonecoder.com

	Author
	~~~~~~
	David Barr, aka javidx9, ©OneLoneCoder 2019
*/

#include <iostream>
#include <sstream>
#include <fstream>

#include "cpu.h"
#include "mem.h"
#include "system_vars.h"
#include "loadnes.h"

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"


CPU cpu;
std::ifstream logfile;

class Demo_olc6502 : public olc::PixelGameEngine
{

    
public:
	Demo_olc6502() { sAppName = "olc6502 Demonstration"; }

	std::map<uint16_t, std::string> mapAsm;

	std::string hex(uint32_t n, uint8_t d)
	{
		std::string s(d, '0');
		for (int i = d - 1; i >= 0; i--, n >>= 4)
			s[i] = "0123456789ABCDEF"[n & 0xF];
		return s;
	};

	void DrawRam(int x, int y, uint16_t nAddr, int nRows, int nColumns)
	{
		int nRamX = x, nRamY = y;
		for (int row = 0; row < nRows; row++)
		{
			std::string sOffset = "$" + hex(nAddr, 4) + ":";
			for (int col = 0; col < nColumns; col++)
			{
				sOffset += " " + hex(read(nAddr), 2);
				nAddr += 1;
			}
			DrawString(nRamX, nRamY, sOffset);
			nRamY += 10;
		}
	}

	void DrawCpu(int x, int y)
	{
		std::string status = "STATUS: ";
		DrawString(x , y , "STATUS:", olc::WHITE);
		DrawString(x  + 64, y, "N", cpu.regs.flags.N ? olc::GREEN : olc::RED);
		DrawString(x  + 80, y , "V", cpu.regs.flags.V ? olc::GREEN : olc::RED);
		DrawString(x  + 96, y , "-", cpu.regs.flags.X ? olc::GREEN : olc::RED);
		DrawString(x  + 112, y , "B", cpu.regs.flags.B ? olc::GREEN : olc::RED);
		DrawString(x  + 128, y , "D", cpu.regs.flags.D ? olc::GREEN : olc::RED);
		DrawString(x  + 144, y , "I", cpu.regs.flags.I ? olc::GREEN : olc::RED);
		DrawString(x  + 160, y , "Z", cpu.regs.flags.Z ? olc::GREEN : olc::RED);
		DrawString(x  + 178, y , "C", cpu.regs.flags.C ? olc::GREEN : olc::RED);
		uint8_t flags = (cpu.regs.flags.N << 7) | (cpu.regs.flags.V << 6) | (cpu.regs.flags.X << 5) | (cpu.regs.flags.B << 4) |
						(cpu.regs.flags.D << 3) | (cpu.regs.flags.I << 2) | (cpu.regs.flags.Z << 1) | (cpu.regs.flags.C << 0);
		DrawString(x  + 196, y, hex(flags, 2));
		DrawString(x , y + 10, "PC: $" + hex(cpu.regs.PC, 4));
		DrawString(x , y + 20, "A: $" +  hex(cpu.regs.A, 2) + "  [" + std::to_string(cpu.regs.A) + "]");
		DrawString(x , y + 30, "X: $" +  hex(cpu.regs.X, 2) + "  [" + std::to_string(cpu.regs.X) + "]");
		DrawString(x , y + 40, "Y: $" +  hex(cpu.regs.Y, 2) + "  [" + std::to_string(cpu.regs.Y) + "]");
		DrawString(x , y + 50, "Stack P: $" + hex(cpu.regs.SP, 4));
	}

	void DrawCode(int x, int y, int nLines)
	{
		auto it_a = mapAsm.find(cpu.regs.PC);
		int nLineY = (nLines >> 1) * 10 + y;
		if (it_a != mapAsm.end())
		{
			DrawString(x, nLineY, (*it_a).second, olc::CYAN);
			while (nLineY < (nLines * 10) + y)
			{
				nLineY += 10;
				if (++it_a != mapAsm.end())
				{
					DrawString(x, nLineY, (*it_a).second);
				}
			}
		}

		it_a = mapAsm.find(cpu.regs.PC);
		nLineY = (nLines >> 1) * 10 + y;
		if (it_a != mapAsm.end())
		{
			while (nLineY > y)
			{
				nLineY -= 10;
				if (--it_a != mapAsm.end())
				{
					DrawString(x, nLineY, (*it_a).second);
				}
			}
		}
	}

	bool OnUserCreate()
	{
		// Load Program (assembled at https://www.masswerk.at/6502/assembler.html)
		/*
			*=$8000
			LDX #10
			STX $0000
			LDX #3
			STX $0001
			LDY $0000
			LDA #0
			CLC
			loop
			ADC $0001
			DEY
			BNE loop
			STA $0002
			NOP
			NOP
			NOP
		*/
		
		// Convert hex string into bytes for RAM
		// std::stringstream ss;
        // ss << "";
		// uint16_t nOffset = 0x8000;
		// while (!ss.eof())
		// {
		// 	std::string b;
		// 	ss >> b;
		// 	write(nOffset++, (uint8_t)std::stoul(b, nullptr, 16));
		// }

        char nes[] = "tests/nestest.nes";
        load_nes(nes);

		logfile.open("tests/nestest.log");

		// Set Reset Vector
		// write(0xFFFC, 0x00);
	    // write(0xFFFD, 0xC0);

		// Dont forget to set IRQ and NMI vectors if you want to play with those
				
		// Extract dissassembly
		mapAsm = cpu.disassemble(0x0000, 0xFFFF);

		// Reset
		cpu.reset();
		cpu.regs.PC = 0xC000;
		return true;
	}

	std::string line;
	std::string fail_string;
	bool success = true;
	bool automatic = true;

	void check_fail() {
		uint16_t pc = stoi(line.substr(0, 4), 0, 16);
		if (pc != cpu.regs.PC) {
			success = false;
			fail_string = "PC expected " + line.substr(0, 4);
		}
		for (int i = 0; i < line.length(); i++) {
			if (line[i] == ':') {
				if (line[i - 1] == 'A') {
					uint8_t a = stoi(line.substr(i + 1, 2), 0, 16);
					if (a != cpu.regs.A) {
						success = false;
						fail_string = "A expected " + line.substr(i + 1, 2);
					}
				} 
				else if (line[i - 1] == 'X') {
					uint8_t x = stoi(line.substr(i + 1, 2), 0, 16);
					if (x != cpu.regs.X) {
						success = false;
						fail_string = "X expected " + line.substr(i + 1, 2);
					}
				}
				else if (line[i - 1] == 'Y') {
					uint8_t y = stoi(line.substr(i + 1, 2), 0, 16);
					if (y != cpu.regs.Y) {
						success = false;
						fail_string = "Y expected " + line.substr(i + 1, 2);
					}
				}
				else if (line[i - 2] == 'S' && line[i - 1] == 'P') {
					uint8_t sp = stoi(line.substr(i + 1, 2), 0, 16);
					if (sp != cpu.regs.SP) {
						success = false;
						fail_string = "SP expected " + line.substr(i + 1, 2);
					}
				}
				else if (line[i - 1] == 'P') {
					uint8_t p = stoi(line.substr(i + 1, 2), 0, 16);
					uint8_t flags = (cpu.regs.flags.N << 7) | (cpu.regs.flags.V << 6) | (cpu.regs.flags.X << 5) | (cpu.regs.flags.B << 4) |
									(cpu.regs.flags.D << 3) | (cpu.regs.flags.I << 2) | (cpu.regs.flags.Z << 1) | (cpu.regs.flags.C << 0);
					if (p != flags) {
						success = false;
						fail_string = "P expected " + line.substr(i + 1, 2);
					}
				}
			}
		}
	}

	bool OnUserUpdate(float fElapsedTime)
	{
		Clear(olc::DARK_BLUE);

		if (GetKey(olc::Key::P).bPressed) {
			automatic = !automatic;
		}

		if (GetKey(olc::Key::SPACE).bPressed)
		{
			do
			{
				cpu.clock();
			} 
			while (!cpu.complete());
			std::getline(logfile, line);
			check_fail();
		}
		

		if (automatic && success)
		{
			do
			{
				cpu.clock();
			} 
			while (!cpu.complete());
			std::getline(logfile, line);
			check_fail();
		}

		if (GetKey(olc::Key::R).bPressed) {
			cpu.reset();
			cpu.regs.PC = 0xC000;
		}

		if (GetKey(olc::Key::I).bPressed)
			cpu.interruptRequest();

		if (GetKey(olc::Key::N).bPressed)
			cpu.nonmaskableInterrupt();

		// Draw Ram Page 0x00		
		DrawRam(2, 2, 0x0000, 16, 16);
		DrawRam(2, 182, 0x8000, 16, 16);
		DrawCpu(448, 2);
		DrawCode(448, 72, 26);


		DrawString(10, 370, "SPACE = Step Instruction    R = RESET    I = IRQ    N = NMI");
		std::string result;
		if (success) {
			result = "Pass";
		} 
		else {
			result = "Fail: " + fail_string;
		}
		DrawString(10, 390, result);

		return true;
	}
};





int main()
{
	Demo_olc6502 demo;
	demo.Construct(680, 480, 2, 2);
	demo.Start();
	return 0;
}