#pragma once
#include "Common.h"
#include "ParsePPS.h"


class Macroblock
{
public:
	uint8_t* data;
	size_t bitOffset;
	ParsePPS& pps;
public:
	Macroblock(uint8_t* data, size_t bitOffset, ParsePPS& pps);
	~Macroblock();
};

