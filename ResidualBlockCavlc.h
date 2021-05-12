#pragma once
#include "Common.h"
//#include "CalvcTable.h"



class ParseSlice;
class ResidualBlockCavlc
{


public:
	ResidualBlockCavlc(ParseSlice& slice);

	bool residual_block_cavlc(BitStream& bs, int coeffLevel[16], int startIdx, int endIdx, uint32_t maxNumCoeff, int& TotalCoeff, RESIDUAL_LEVEL residualLevel, size_t i4x4 = 0, size_t i8x8 = 0);

	int getNumberCurrent(RESIDUAL_LEVEL residualLevel, size_t i4x4, size_t i8x8);

private:
	int getNumCoeffAndTrailingOnes(int nC, uint16_t coeff_token, int& coeff_token_length, int& TrailingOnes, int& TotalCoeff);
	int getTotalZeros(BitStream& bs, uint32_t TotalCoeff, uint32_t maxNumCoeff);
	int getRunbefore(BitStream& bs, uint32_t zerosLeft);
public:
	ParseSlice& sliceBase;
	bool	trailing_ones_sign_flag;
	int     levelVal[16]; //非零系数,带符号
	int		level_suffix;
	int		level_prefix;

	int     runVal[16];
};

