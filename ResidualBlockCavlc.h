#pragma once
#include "Common.h"



class ParseSlice;
class ResidualBlockCavlc
{


public:
	ResidualBlockCavlc(ParseSlice& slice);

	bool residual_block_cavlc(BitStream& bs, int32_t coeffLevel[16], int32_t startIdx, int32_t endIdx, uint32_t maxNumCoeff, size_t BlkIdx);

	int getNumberCurrent(int32_t startIdx);
public:
	ParseSlice& sliceBase;
};

