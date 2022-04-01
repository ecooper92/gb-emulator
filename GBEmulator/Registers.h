#pragma once

#include <cstdint>

class Registers
{
public:
	Registers() { Reset(); }

	void Reset()
	{
		_A = 0x01;
		_BC = 0x0013;
		_DE = 0x00D8;
		_HL = 0x014D;
		_SP = 0xFFFE;

		_zero = false;
		_negative = false;
		_carry = false;
		_half_carry = false;
	}

	bool Zero() { return _zero; }
	bool Negative() { return _negative; }
	bool Carry() { return _carry; }
	bool Half_Carry() { return _half_carry; }

	void Zero(bool v) { _zero = v; }
	void Negative(bool v) { _negative = v; }
	void Carry(bool v) { _carry = v; }
	void Half_Carry(bool v) { _half_carry = v; }

	uint8_t A() { return _A; }
	uint8_t B() { return _BC >> 8; }
	uint8_t C() { return _BC & 0x00FF; }
	uint8_t D() { return _DE >> 8; }
	uint8_t E() { return _DE & 0x00FF; }
	uint8_t H() { return _HL >> 8; }
	uint8_t L() { return _HL & 0x00FF; }

	void A(uint8_t v) { _A = v; }
	void B(uint8_t v) { _BC = (_BC & 0x00FF) | (v << 8); }
	void C(uint8_t v) { _BC = (_BC & 0xFF00) | v; }
	void D(uint8_t v) { _DE = (_DE & 0x00FF) | (v << 8); }
	void E(uint8_t v) { _DE = (_DE & 0xFF00) | v; }
	void H(uint8_t v) { _HL = (_HL & 0x00FF) | (v << 8); }
	void L(uint8_t v) { _HL = (_HL & 0xFF00) | v; }

	uint16_t BC() { return _BC; }
	uint16_t DE() { return _DE; }
	uint16_t HL() { return _HL; }
	uint16_t SP() { return _SP; }
	uint16_t PC() { return _PC; }

	void BC(uint16_t v) { _BC = v; }
	void DE(uint16_t v) { _DE = v; }
	void HL(uint16_t v) { _HL = v; }
	void SP(uint16_t v) { _SP = v; }
	void PC(uint16_t v) { _PC = v; }
private:
	uint8_t _A;
	uint16_t _BC;
	uint16_t _DE;
	uint16_t _HL;
	uint16_t _SP;
	uint16_t _PC;

	bool _zero;
	bool _negative;
	bool _carry;
	bool _half_carry;
};