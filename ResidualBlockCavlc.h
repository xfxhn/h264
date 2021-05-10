#pragma once
#include "Common.h"
//#include "CalvcTable.h"



class ParseSlice;
class ResidualBlockCavlc
{


public:
	ResidualBlockCavlc(ParseSlice& slice);

	bool residual_block_cavlc(BitStream& bs, int32_t coeffLevel[16], int32_t startIdx, int32_t endIdx, uint32_t maxNumCoeff, size_t i4x4, size_t i8x8, int& TotalCoeff);

	int getNumberCurrent(size_t i4x4, size_t i8x8);

private:
	int getNumCoeffAndTrailingOnes(int nC, uint16_t coeff_token, int& coeff_token_length, int& TrailingOnes, int& TotalCoeff);
	int getTotalZeros(BitStream& bs, uint32_t TotalCoeff,uint32_t maxNumCoeff);
	int getRunbefore(BitStream& bs, uint32_t zerosLeft);
public:
	ParseSlice& sliceBase;
	bool	trailing_ones_sign_flag;
	int     levelVal[16]; //非零系数,带符号
	int		level_suffix;
	int		level_prefix;
	int		levelCode;
	int     runVal[16];
};

