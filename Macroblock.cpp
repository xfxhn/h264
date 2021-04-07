#include "Macroblock.h"

Macroblock::Macroblock(uint8_t* data, size_t bitOffset, ParsePPS& pps):pps(pps),data(data), bitOffset(bitOffset)
{


}

Macroblock::~Macroblock()
{
}
