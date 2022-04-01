#pragma once

#include <cstdint>
#include <exception>
#include <iostream>

#include "Memory.h"
#include "Registers.h"

class CPU
{
public:
	CPU(Memory& memory, Registers& registers) : memory(memory), registers(registers) {}

	uint8_t And(uint8_t a, uint8_t b)
	{
		uint8_t r = a & b;
		registers.Zero(r == 0);
		registers.Negative(false);
		registers.Carry(false);
		registers.Half_Carry(true);
		return a & b;
	}

	uint8_t Or(uint8_t a, uint8_t b)
	{
		uint8_t r = a | b;
		registers.Zero(registers.A() == 0);
		registers.Negative(false);
		registers.Half_Carry(false);
		registers.Carry(false);
		return r;
	}

	uint8_t Xor(uint8_t a, uint8_t b)
	{
		uint8_t r = a ^ b;
		registers.Zero(registers.A() == 0);
		registers.Half_Carry(false);
		registers.Carry(false);
		registers.Negative(false);
		return r;
	}

	void Compare(uint8_t a, uint8_t b)
	{
		registers.Zero(a == b);
		registers.Negative(true);
		registers.Carry(a < b);
		registers.Half_Carry((a & 0xF) < (b & 0xF));
	}

	uint8_t Adc(uint8_t a, uint8_t b)
	{
		b += registers.Carry() ? 1 : 0;
		return Add(a, b);
	}

	uint8_t Add(uint8_t a, uint8_t b)
	{
		uint8_t r = a + b;
		uint16_t c = (static_cast<uint16_t>(a) + static_cast<uint16_t>(b)) & 0xFF00;
		uint8_t hc = ((a & 0xF) + (b & 0xF)) & 0xF0;
		registers.Zero(r == 0);
		registers.Negative(false);
		registers.Carry(c > 0);
		registers.Half_Carry(hc > 0);
		return r;
	}

	uint16_t Add(uint16_t a, uint8_t b)
	{
		uint16_t c = ((a & 0xFF) + b) & 0xFF00;
		uint16_t hc = ((a & 0xF) + (b & 0xF)) & 0xF0;
		registers.Zero(false);
		registers.Negative(false);
		registers.Carry(c > 0);
		registers.Half_Carry(hc > 0);
		return a + b;
	}

	uint16_t Add(uint16_t a, uint16_t b)
	{
		uint32_t c = (static_cast<uint32_t>(a) + static_cast<uint32_t>(b)) & 0xFFFF0000;
		uint16_t hc = ((a & 0xFFF) + (b & 0xFFF)) & 0xF000;
		// Zero not modified.
		registers.Negative(false);
		registers.Carry(c > 0);
		registers.Half_Carry(hc > 0);
		return a + b;
	}

	uint8_t Sub(uint8_t a, uint8_t b)
	{
		uint8_t r = a - b;
		registers.Zero(r == 0);
		registers.Negative(true);
		registers.Carry(a > b);
		registers.Half_Carry((a & 0xF) > (b & 0xF));
		return r;
	}

	uint8_t Decrement(uint8_t r)
	{
		r--;
		registers.Zero(r == 0);
		registers.Negative(true);
		registers.Half_Carry((r & 0xF) == 0xF);
		return r;
	}

	uint16_t Decrement(uint16_t r)
	{
		r--;
		// No flags set on 16 bit op
		return r;
	}

	uint8_t Increment(uint8_t r)
	{
		r++;
		registers.Zero(r == 0);
		registers.Negative(false);
		registers.Half_Carry((r & 0xF) == 0x0);
		return r;
	}

	uint16_t Increment(uint16_t r)
	{
		r++;
		// No flags set on 16 bit op
		return r;
	}

	void Update(double delta_ms)
	{
		int cycles_to_complete = (delta_ms / 1000) * CYCLES_PER_SECOND;

		while (cycles_to_complete > 0)
		{
			if (cycles_to_complete > CYCLES_PER_SECOND * 10)
			{

			}
			bool jump = false;
			uint8_t time = 0;
			uint16_t pc = registers.PC();
			uint8_t opcode = memory.Read8(pc);
			switch (opcode)
			{
			case 0x00: // NOOP
			case 0x7F: // LD A, A
			case 0x40: // LD B, B
			case 0x49: // LD C, C
			case 0x52: // LD D, D
			case 0x5B: // LD E, E
			case 0x64: // LD H, H
			case 0x6D: // LD L, L
				time = 4;
				break;
			case 0x01: // LD u16->BC
				time = 12;
				registers.BC(memory.Read16(pc + 1));
				break;
			case 0x02: // LD (BC)->A
				time = 8;
				registers.A(memory.Read8(registers.BC(), false));
				break;
			case 0x03: // INC BC
				time = 8;
				registers.BC(Increment(registers.BC()));
				break;
			case 0x05: // DEC B
				time = 4;
				registers.B(Decrement(registers.B()));
				break;
			case 0x06: // LD B, u8
				time = 8;
				registers.B(memory.Read8(pc + 1));
				break;
			case 0x08: // LD SP->(u16)
				time = 20;
				memory.Write16(memory.Read16(pc + 1), registers.SP());
				break;
			case 0x0B: // DEC BC
				time = 8;
				registers.BC(Decrement(registers.BC()));
				break;
			case 0x0C: // INC C
				time = 4;
				registers.C(Increment(registers.C()));
				break;
			case 0x0D: // DEC C
				time = 4;
				registers.C(Decrement(registers.C()));
				break;
			case 0x0E:
				time = 8;
				registers.C(memory.Read8(pc + 1));
				break;
			case 0x11: // LD DE
				time = 12;
				registers.DE(memory.Read16(pc + 1));
				break;
			case 0x1A: // LD (DE)->A
				time = 8;
				registers.A(memory.Read8(registers.DE(), false));
				break;
			case 0x18: // JP n
				time = 12;
				jump = true;
				registers.PC(registers.PC() + memory.Read8(pc + 1) + 2);
				break;
			case 0x19: // ADD DE -> HL
				time = 8;
				registers.HL(Add(registers.HL(), registers.DE()));
				break;
			case 0x20: // JP NZ n
				if (!registers.Zero())
				{
					time = 12;
					jump = true;
					registers.PC(registers.PC() + memory.Readi8(pc + 1) + 2);
				}
				else
				{
					time = 8;
					memory.Readi8(pc + 1); // throw away read.
				}
				break;
			case 0x21: // LD HL
				time = 12;
				registers.HL(memory.Read16(pc + 1));
				break;
			case 0x28: // JP Z,n
				if (registers.Zero())
				{
					time = 12;
					jump = true;
					registers.PC(registers.PC() + memory.Read8(pc + 1));
				}
				else
				{
					time = 8;
					memory.Read8(pc + 1); // throw away read.
				}
				break;
			case 0x2A: // (HL)->A, HL++
				time = 8;
				registers.A(memory.Read8(registers.HL(), false));
				registers.HL(registers.HL() + 1);
				break;
			case 0x31: // LD SP
				time = 12;
				registers.SP(memory.Read16(pc + 1));
				break;
			case 0x36: // LD u8->(HL)
				time = 12;
				memory.Write8(registers.HL(), memory.Read8(pc + 1));
				break;
			case 0x3E: // LD A
				time = 8;
				registers.A(memory.Read8(pc + 1));
				break;
			case 0x32: // Store A->(HL); HL--
				time = 8;
				memory.Write8(registers.HL(), registers.A());
				registers.HL(registers.HL() - 1);
				break;
			case 0x42: // LD D->B
				time = 4;
				registers.B(registers.D());
				break;
			case 0x46: // LD (HL)->B
				time = 8;
				registers.B(memory.Read8(registers.HL(), false));
				break;
			case 0x70: // LD B->(HL)
				time = 8;
				memory.Write8(registers.HL(), registers.B());
				break;
			case 0x71: // LD C->(HL)
				time = 8;
				memory.Write8(registers.HL(), registers.C());
				break;
			case 0x72: // LD D->(HL)
				time = 8;
				memory.Write8(registers.HL(), registers.D());
				break;
			case 0x73: // LD E->(HL)
				time = 8;
				memory.Write8(registers.HL(), registers.E());
				break;
			case 0x74: // LD H->(HL)
				time = 8;
				memory.Write8(registers.HL(), registers.H());
				break;
			case 0x75: // LD L->(HL)
				time = 8;
				memory.Write8(registers.HL(), registers.L());
				break;
			case 0x77: // LD A->(HL)
				time = 8;
				memory.Write8(registers.HL(), registers.A());
				break;
			case 0x78: // LD B->A
				time = 4;
				registers.A(registers.B());
				break;
			case 0x79: // LD C->A
				time = 4;
				registers.A(registers.C());
				break;
			case 0x7A: // LD D->A
				time = 4;
				registers.A(registers.D());
				break;
			case 0x7B: // LD E->A
				time = 4;
				registers.A(registers.E());
				break;
			case 0x7C: // LD H->A
				time = 4;
				registers.A(registers.H());
				break;
			case 0x7D: // LD L->A
				time = 4;
				registers.A(registers.L());
				break;
			case 0x7E: // LD (HL)->A
				time = 8;
				registers.A(memory.Read8(registers.HL(), false));
				break;
			case 0x80: // ADD B->A
				time = 4;
				registers.A(Add(registers.A(), registers.B()));
				break;
			case 0x81: // ADD C->A
				time = 4;
				registers.A(Add(registers.A(), registers.C()));
				break;
			case 0x82: // ADD D->A
				time = 4;
				registers.A(Add(registers.A(), registers.D()));
				break;
			case 0x83: // ADD E->A
				time = 4;
				registers.A(Add(registers.A(), registers.E()));
				break;
			case 0x84: // ADD H->A
				time = 4;
				registers.A(Add(registers.A(), registers.H()));
				break;
			case 0x85: // ADD L->A
				time = 4;
				registers.A(Add(registers.A(), registers.L()));
				break;
			case 0x86: // ADD (HL)->A
				time = 8;
				registers.A(Add(registers.A(), memory.Read8(registers.HL(), false)));
				break;
			case 0x87: // ADD A->A
				time = 4;
				registers.A(Add(registers.A(), registers.A()));
				break;
			case 0x88: // ADC B->A
				time = 4;
				registers.A(Adc(registers.A(), registers.B()));
				break;
			case 0x89: // ADC C->A
				time = 4;
				registers.A(Adc(registers.A(), registers.C()));
				break;
			case 0x8A: // ADC D->A
				time = 4;
				registers.A(Adc(registers.A(), registers.D()));
				break;
			case 0x8B: // ADC E->A
				time = 4;
				registers.A(Adc(registers.A(), registers.E()));
				break;
			case 0x8C: // ADC H->A
				time = 4;
				registers.A(Adc(registers.A(), registers.H()));
				break;
			case 0x8D: // ADC L->A
				time = 4;
				registers.A(Adc(registers.A(), registers.L()));
				break;
			case 0x8E: // ADC (HL)->A
				time = 8;
				registers.A(Adc(registers.A(), memory.Read8(registers.HL(), false)));
				break;
			case 0x8F: // ADC A->A
				time = 4;
				registers.A(Adc(registers.A(), registers.A()));
				break;
			case 0x95: // SUB A, L
				time = 4;
				registers.A(Sub(registers.A(), registers.L()));
				break;
			case 0xA7: // AND A,A
				time = 4;
				registers.A(And(registers.A(), registers.A()));
				break;
			case 0xAF: // XOR A
				time = 4;
				registers.A(Xor(registers.A(), registers.A()));
				break;
			case 0xB1: // OR A, C
				time = 4;
				registers.A(Or(registers.A(), registers.C()));
				break;
			case 0xC0: // RET NZ
				if (!registers.Zero())
				{
					time = 20;
					registers.PC(memory.Read16(registers.SP(), false) + 1);
					registers.SP(registers.SP() + 2);
					jump = true;
				}
				else
				{
					time = 8;
				}
				break;
			case 0xC1: // POP BC
				time = 12;
				registers.BC(memory.Read16(registers.SP(), false));
				registers.SP(registers.SP() + 1);
				break;
			case 0xC3: // JP
				time = 16;
				jump = true;
				registers.PC(memory.Read16(pc + 1));
				break;
			case 0xC5: // PUSH BC
				time = 16;
				registers.SP(registers.SP() - 2);
				memory.Write16(registers.SP(), registers.BC());
				break;
			case 0xC9: // RET
				time = 16;
				jump = true;
				registers.PC(memory.Read16(registers.SP(), false) + 1);
				registers.SP(registers.SP() + 2);
				break;
			case 0xCC: // CALL Z
				if (registers.Zero())
				{
					time = 24;
					jump = true;
					registers.SP(registers.SP() - 2);
					memory.Write16(registers.SP(), registers.PC());
					registers.PC(memory.Read16(pc + 1));
				}
				else
				{
					time = 12;
					memory.Read16(pc + 1); // Throw away read.
				}
				break;
			case 0xCD: // Call
				time = 24;
				jump = true;
				registers.SP(registers.SP() - 2);
				memory.Write16(registers.SP(), registers.PC());
				registers.PC(memory.Read16(pc + 1));
				break;
			case 0xD0: // RET NC
				if (!registers.Carry())
				{
					time = 20;
					registers.PC(memory.Read16(registers.SP(), false) + 1);
					registers.SP(registers.SP() + 2);
					jump = true;
				}
				else
				{
					time = 8;
				}
				break;
			case 0xE0: // LD A->(FF00 + u8)
				time = 12;
				memory.Write8(memory.Read8(pc + 1) + 0xFF00, registers.A());
				break;
			case 0xE2: // LD A->(FF00 + C)
				time = 8;
				memory.Write8(registers.C() + 0xFF00, registers.A());
				break;
			case 0xE8: // ADD SP,u8
				time = 16;
				registers.SP(Add(registers.SP(), memory.Read8(pc + 1)));
				break;
			case 0xEA: // LD A
				time = 16;
				memory.Write8(memory.Read16(pc + 1), registers.A());
				break;
			case 0xF0: // LD (0xFF00 + u8)->A
				time = 12;
				registers.A(memory.Read8(0xFF00 + memory.Read8(pc + 1), false));
				break;
			case 0xFA: // LD (u16)->A
				time = 16;
				registers.A(memory.Read8(memory.Read16(pc + 1), false));
				break;
			case 0xF3:
				time = 4;
				// TODO: Disable interupts
				break;
			case 0xFE: // CP A,n
				time = 8;
				Compare(registers.A(), memory.Read8(pc + 1));
				break;
			default:
				std::cout << "Missing Instruction: " << std::hex << opcode << std::endl;
				std::terminate();
				break;
			}

			// If we didn't jump somewhere then move the program counter.
			if (!jump)
			{
				registers.PC(registers.PC() + memory.Read_Count());
			}

			memory.Reset_Read_Count();
			cycles_to_complete -= time;
		}
	}

private:
	const static uint32_t CYCLES_PER_SECOND = 4194304;
	Memory& memory;
	Registers &registers;
};