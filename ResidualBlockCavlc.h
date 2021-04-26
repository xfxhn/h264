#pragma once
#include "Common.h"
#include "SliceHeader.h"





class ResidualBlockCavlc
{

public:
	ResidualBlockCavlc();

	bool residual_block_cavlc(BitStream& bs, const SliceHeader& sHeader, int32_t coeffLevel[16], int32_t startIdx, int32_t endIdx, uint32_t maxNumCoeff, size_t BlkIdx);

	int getNumberCurrent(int32_t startIdx);
};

