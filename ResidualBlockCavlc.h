#pragma once
#include "Common.h"



class ParseSlice;
class ResidualBlockCavlc
{


public:
	ResidualBlockCavlc(ParseSlice& slice);

	bool residual_block_cavlc(BitStream& bs, int32_t coeffLevel[16], int32_t startIdx, int32_t endIdx, uint32_t maxNumCoeff, size_t i4x4, size_t i8x8, int& TotalCoeff);

	int getNumberCurrent(size_t i4x4, size_t i8x8);

private:
	int getNumCoeffAndTrailingOnes(int nC, uint16_t coeff_token, int& coeff_token_length, int& TrailingOnes, int& TotalCoeff);
public:
	ParseSlice& sliceBase;
};

