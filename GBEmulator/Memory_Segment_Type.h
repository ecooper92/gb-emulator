#pragma once

#include <cstdint>
#include <array>

enum class Memory_Segment_Type
{
	ROM_FIXED,
	ROM_SWITCHED,
	VRAM,
	RAM_EXTERNAL,
	RAM_INTERNAL,
	RAM_INTERNAL_SWITCHED,
	RAM_ECHO,
	OAM,
	INVALID,
	IO,
	RAM_HIGH,
	INTERUPT_ENABLE
};

struct Memory_Segment_Data
{
	Memory_Segment_Type type;
	uint16_t start;
	uint16_t end;

	constexpr Memory_Segment_Data(Memory_Segment_Type type, uint16_t start, uint16_t end) : type(type), start(start), end(end) {}

	constexpr uint16_t Size() const { return end - start + 1; }
};

constexpr std::array<Memory_Segment_Data, 12> MEMORY_SEGMENTS =
{
	Memory_Segment_Data{Memory_Segment_Type::ROM_FIXED, 0, 0x3FFF},
	Memory_Segment_Data{Memory_Segment_Type::ROM_SWITCHED, 0x4000, 0x7FFF},
	Memory_Segment_Data{Memory_Segment_Type::VRAM, 0x8000, 0x9FFF},
	Memory_Segment_Data{Memory_Segment_Type::RAM_EXTERNAL, 0xA000, 0xBFFF},
	Memory_Segment_Data{Memory_Segment_Type::RAM_INTERNAL, 0xC000, 0xCFFF},
	Memory_Segment_Data{Memory_Segment_Type::RAM_INTERNAL_SWITCHED, 0xD000, 0xDFFF},
	Memory_Segment_Data{Memory_Segment_Type::RAM_ECHO, 0xE000, 0xFDFF},
	Memory_Segment_Data{Memory_Segment_Type::OAM, 0xFE00, 0xFE9F},
	Memory_Segment_Data{Memory_Segment_Type::INVALID, 0xFEA0, 0xFEFF},
	Memory_Segment_Data{Memory_Segment_Type::IO, 0xFF00, 0xFF7F},
	Memory_Segment_Data{Memory_Segment_Type::RAM_HIGH, 0xFF80, 0xFFFE},
	Memory_Segment_Data{Memory_Segment_Type::INTERUPT_ENABLE, 0xFFFF, 0xFFFF}
};

constexpr uint16_t Segment_Size(Memory_Segment_Type type)
{
	return MEMORY_SEGMENTS[static_cast<std::size_t>(type)].Size();
}

constexpr uint16_t Segment_Offset(uint16_t address, Memory_Segment_Type type)
{
	return address - MEMORY_SEGMENTS[static_cast<std::size_t>(type)].start;
}

constexpr Memory_Segment_Type Get_Memory_Segment(uint16_t address)
{
	for (const auto& segment : MEMORY_SEGMENTS)
	{
		if (address >= segment.start && address <= segment.end)
		{
			return segment.type;
		}
	}

	// If we got here, the memory ranges above are wrong. 16 bit range should be completely mapped.
	std::terminate();
}