#pragma once
#include "Common.h"
//#include "CalvcTable.h"



class ParseSlice;
class ResidualBlockCavlc
{


public:
	ResidualBlockCavlc(ParseSlice& slice);

	bool residual_block_cavlc(BitStream& bs, int* coeffLevel, int startIdx, int endIdx, uint32_t maxNumCoeff, int& TotalCoeff, RESIDUAL_LEVEL residualLevel, size_t BlkIdx = 0);

	int getNumberCurrent(RESIDUAL_LEVEL residualLevel, size_t BlkIdx);

private:
	int getNumCoeffAndTrailingOnes(int nC, uint16_t coeff_token, int& coeff_token_length, int& TrailingOnes, int& TotalCoeff);
	int getTotalZeros(BitStream& bs, uint32_t TotalCoeff, uint32_t maxNumCoeff);
	int getRunbefore(BitStream& bs, uint32_t zerosLeft);
	int findTable(BitStream& bs, int maxNumber, int* lengthTable, int* codeTable);

	int findChromaAC_nA_nB(const int CbCr, const int BlkIdx, int& nA, int& nB, bool& availableLeft, bool& availableTop);
public:
	ParseSlice& sliceBase;
	bool	trailing_ones_sign_flag;
	int     levelVal[16]; //非零系数,带符号
	int		level_suffix;
	int		level_prefix;

	int     runVal[16];
};

