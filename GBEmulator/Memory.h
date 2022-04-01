#pragma once

#include <istream>
#include <iterator>
#include <cstdint>
#include <string>
#include <array>
#include <vector>

#include "Memory_Segment_Type.h"

class Memory
{
public:
	enum class MBC_Type { None, MBC3, MBC5 };
	enum class IO_Type { SCY = 0xFF42, SCX = 0xFF43, LY = 0xFF44 };

	Memory(std::istream& s)
		: _read_count(0), _rom(std::istreambuf_iterator<char>(s), {})
	{
		_internal_ram.resize(Segment_Size(Memory_Segment_Type::RAM_INTERNAL));
		_internal_switched_ram.resize(Segment_Size(Memory_Segment_Type::RAM_INTERNAL_SWITCHED));
		_vram.resize(Segment_Size(Memory_Segment_Type::VRAM));
		_oam.resize(Segment_Size(Memory_Segment_Type::OAM));
		_invalid.resize(Segment_Size(Memory_Segment_Type::INVALID));
		_io.resize(Segment_Size(Memory_Segment_Type::IO));
		_high_ram.resize(Segment_Size(Memory_Segment_Type::RAM_HIGH));
		_interupts.resize(Segment_Size(Memory_Segment_Type::INTERUPT_ENABLE));

		_io[Segment_Offset(static_cast<std::size_t>(IO_Type::LY), Memory_Segment_Type::IO)] = 148;
	}

	uint8_t MBC()
	{
		return _rom[0x147];
	}

	std::string Get_Title()
	{
		auto title_begin_it = _rom.begin();
		std::advance(title_begin_it, 0x134);

		auto title_end_it = _rom.begin();
		std::advance(title_end_it, 0x142);

		return { title_begin_it, title_end_it };
	}

	uint8_t Read8(uint16_t address, bool advance_count = true)
	{
		_read_count += advance_count ? 1 : 0;

		auto type = Get_Memory_Segment(address);
		auto offset = Segment_Offset(address, type);
		const auto &memory = Get_Read_Memory(type);

		return memory[offset];
	}
	
	int8_t Readi8(uint16_t address, bool advance_count = true)
	{
		_read_count += advance_count ? 1 : 0;

		auto type = Get_Memory_Segment(address);
		auto offset = Segment_Offset(address, type);
		const auto& memory = Get_Read_Memory(type);

		return memory[offset];
	}

	uint16_t Read16(uint16_t address, bool advance_count = true)
	{
		_read_count += advance_count ? 2 : 0;

		auto type = Get_Memory_Segment(address);
		auto offset = Segment_Offset(address, type);
		const auto& memory = Get_Read_Memory(type);

		return memory[offset] | (memory[offset + 1] << 8);
	}

	void Write8(uint16_t address, uint8_t value)
	{
		auto type = Get_Memory_Segment(address);
		auto offset = Segment_Offset(address, type);
		auto& memory = Get_Write_Memory(type);

		memory[offset] = value;
	}

	void Write16(uint16_t address, uint16_t value)
	{
		auto type = Get_Memory_Segment(address);
		auto offset = Segment_Offset(address, type);
		auto& memory = Get_Write_Memory(type);

		memory[offset] = value & 0xFF;
		memory[offset + 1] = value >> 8;
	}

	uint16_t Read_Count()
	{
		return _read_count;
	}

	void Reset_Read_Count()
	{
		_read_count = 0;
	}

private:
	std::vector<uint8_t>& Get_Read_Memory(Memory_Segment_Type type)
	{
		switch (type)
		{
		case Memory_Segment_Type::ROM_FIXED:
			return _rom;
		case Memory_Segment_Type::RAM_INTERNAL:
			return _internal_ram;
		case Memory_Segment_Type::RAM_INTERNAL_SWITCHED:
			return _internal_switched_ram;
		case Memory_Segment_Type::VRAM:
			return _vram;
		case Memory_Segment_Type::OAM:
			return _oam;
		case Memory_Segment_Type::INVALID:
			return _invalid;
		case Memory_Segment_Type::IO:
			return _io;
		case Memory_Segment_Type::RAM_HIGH:
			return _high_ram;
		case Memory_Segment_Type::INTERUPT_ENABLE:
			return _interupts;
		default:
			std::terminate();
		}
	}

	std::vector<uint8_t>& Get_Write_Memory(Memory_Segment_Type type)
	{
		switch (type)
		{
		case Memory_Segment_Type::ROM_FIXED:
			return _rom;
		case Memory_Segment_Type::RAM_INTERNAL:
			return _internal_ram;
		case Memory_Segment_Type::RAM_INTERNAL_SWITCHED:
			return _internal_switched_ram;
		case Memory_Segment_Type::VRAM:
			return _vram;
		case Memory_Segment_Type::OAM:
			return _oam;
		case Memory_Segment_Type::INVALID:
			return _invalid;
		case Memory_Segment_Type::IO:
			return _io;
		case Memory_Segment_Type::RAM_HIGH:
			return _high_ram;
		case Memory_Segment_Type::INTERUPT_ENABLE:
			return _interupts;
		default:
			std::terminate();
		}
	}

	uint8_t _read_count;
	std::vector<uint8_t> _rom;
	std::vector<uint8_t> _internal_ram;
	std::vector<uint8_t> _internal_switched_ram;
	std::vector<uint8_t> _vram;
	std::vector<uint8_t> _oam;
	std::vector<uint8_t> _invalid;
	std::vector<uint8_t> _io;
	std::vector<uint8_t> _high_ram;
	std::vector<uint8_t> _interupts;
};