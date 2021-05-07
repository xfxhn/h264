#include "ResidualBlockCavlc.h"
#include "ParseSlice.h"



ResidualBlockCavlc::ResidualBlockCavlc(ParseSlice& slice) :sliceBase(slice)
{

}

bool ResidualBlockCavlc::residual_block_cavlc(
	BitStream& bs, int32_t coeffLevel[16], int32_t startIdx, int32_t endIdx, uint32_t maxNumCoeff, size_t i4x4, size_t i8x8, int& TotalCoeff
)
{

	memset(coeffLevel, 0, sizeof(int32_t) * maxNumCoeff);



	int coeff_token_length = 0;
	int TrailingOnes = 0;

	int numberCurrent = getNumberCurrent(i4x4, i8x8);


	//获取16bit因为token最长只有16bit
	uint16_t coeff_token = bs.getMultiBit(16);

	getNumCoeffAndTrailingOnes(numberCurrent, coeff_token, coeff_token_length, TrailingOnes, TotalCoeff);
	//读取真正的token长度
	bs.readMultiBit(coeff_token_length);

	return false;
}

int ResidualBlockCavlc::getNumberCurrent(size_t i4x4, size_t i8x8)
{

	int nC = 0;
	bool availableTop = false;
	bool availableLeft = false;


	int nA = 0; //左侧块
	int nB = 0;	//上测块


	const int BlkIdx = i8x8 * 4 + i4x4;

	if (BlkIdx == 0 || BlkIdx == 2 || BlkIdx == 8 || BlkIdx == 10)
	{
		if (sliceBase.mbX > 0) {
			const int leftMbIdx = sliceBase.CurrMbAddr - 1;
			const int left4x4Idx = BlkIdx + 5;

			nA = sliceBase.macroblock[leftMbIdx]->mb_luma_4x4_non_zero_count_coeff[left4x4Idx];
			availableLeft = true;
		}
	}
	else
	{
		if (BlkIdx % 2 == 1) {

			nA = sliceBase.macroblock[sliceBase.CurrMbAddr]->mb_luma_4x4_non_zero_count_coeff[BlkIdx - 1];
			availableLeft = true;
		}
		else {
			nA = sliceBase.macroblock[sliceBase.CurrMbAddr]->mb_luma_4x4_non_zero_count_coeff[BlkIdx - 3];
			availableLeft = true;
		}
	}

	if (i8x8 < 2)
	{
		if (i4x4 < 2) {
			if (sliceBase.mbY > 0)
			{
				const int topMbIdx = sliceBase.CurrMbAddr - sliceBase.sHeader->sps.PicWidthInMbs;
				const int top4x4Idx = BlkIdx + 10;
				nB = sliceBase.macroblock[topMbIdx]->mb_luma_4x4_non_zero_count_coeff[top4x4Idx];
				availableTop = true;
			}
		}
		else {
			nB = sliceBase.macroblock[sliceBase.CurrMbAddr]->mb_luma_4x4_non_zero_count_coeff[BlkIdx - 1];
			availableTop = true;
		}
	}
	else
	{
		if (i4x4 < 2) {
			nB = sliceBase.macroblock[sliceBase.CurrMbAddr]->mb_luma_4x4_non_zero_count_coeff[BlkIdx - 6];
			availableTop = true;
		}
		else {
			nB = sliceBase.macroblock[sliceBase.CurrMbAddr]->mb_luma_4x4_non_zero_count_coeff[BlkIdx - 2];
			availableTop = true;
		}
	}

	if (availableTop && availableLeft)
	{
		nC = (nA + nB + 1) >> 1;
	}
	else if (availableLeft && !availableTop)
	{
		nC = nA;
	}
	else if (availableTop && !availableLeft)
	{
		nC = nB;
	}
	else
	{
		nC = 0;
	}


	return nC;
}

int ResidualBlockCavlc::getNumCoeffAndTrailingOnes(int nC, uint16_t coeff_token, int& coeff_token_length, int& TrailingOnes, int& TotalCoeff)
{

	if (0 <= nC && nC < 2)
	{
		if ((coeff_token >> 15) == 1) //(1)b
		{
			coeff_token_length = 1;
			TrailingOnes = 0;
			TotalCoeff = 0;
		}
		else if ((coeff_token >> 10) == 5) //(0001 01)b ==> (00 0101)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 1;
		}
		else if ((coeff_token >> 14) == 1) //(01)b
		{
			coeff_token_length = 2;
			TrailingOnes = 1;
			TotalCoeff = 1;
		}
		else if ((coeff_token >> 8) == 7) //(0000 0111)b
		{
			coeff_token_length = 8;
			TrailingOnes = 0;
			TotalCoeff = 2;
		}
		else if ((coeff_token >> 10) == 4) //(0001 00)b ==> (00 0100)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 2;
		}
		else if ((coeff_token >> 13) == 1) //(001)b
		{
			coeff_token_length = 3;
			TrailingOnes = 2;
			TotalCoeff = 2;
		}
		else if ((coeff_token >> 7) == 7) //(0000 0011 1)b ==> (0 0000 0111)b
		{
			coeff_token_length = 9;
			TrailingOnes = 0;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 8) == 6) //(0000 0110)b
		{
			coeff_token_length = 8;
			TrailingOnes = 1;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 9) == 5) //(0000 101)b ==> (000 0101)b
		{
			coeff_token_length = 7;
			TrailingOnes = 2;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 11) == 3) //(0001 1)b ==> (0 0011)b
		{
			coeff_token_length = 5;
			TrailingOnes = 3;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 6) == 7) //(0000 0001 11)b ==> (00 0000 0111)b
		{
			coeff_token_length = 10;
			TrailingOnes = 0;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 7) == 6) //(0000 0011 0)b ==> (0 0000 0110)b
		{
			coeff_token_length = 9;
			TrailingOnes = 1;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 8) == 5) //(0000 0101)b ==> (0000 0101)b
		{
			coeff_token_length = 8;
			TrailingOnes = 2;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 10) == 3) //(0000 11)b ==> (00 0011)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 5) == 7) //(0000 0000 111)b ==> (000 0000 0111)b
		{
			coeff_token_length = 11;
			TrailingOnes = 0;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 6) == 6) //(0000 0001 10)b ==> (00 0000 0110)b
		{
			coeff_token_length = 10;
			TrailingOnes = 1;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 7) == 5) //(0000 0010 1)b ==> (0 0000 0101)b
		{
			coeff_token_length = 9;
			TrailingOnes = 2;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 9) == 4) //(0000 100)b ==> (000 0100)b
		{
			coeff_token_length = 7;
			TrailingOnes = 3;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 3) == 15) //(0000 0000 0111 1)b ==> (0 0000 0000 1111)b
		{
			coeff_token_length = 13;
			TrailingOnes = 0;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 5) == 6) //(0000 0000 110)b ==> (000 0000 0110)b
		{
			coeff_token_length = 11;
			TrailingOnes = 1;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 6) == 5) //(0000 0001 01)b ==> (00 0000 0101)b
		{
			coeff_token_length = 10;
			TrailingOnes = 2;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 8) == 4) //(0000 0100)b ==> (0000 0100)b
		{
			coeff_token_length = 8;
			TrailingOnes = 3;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 3) == 11) //(0000 0000 0101 1)b ==> (0 0000 0000 1011)b
		{
			coeff_token_length = 13;
			TrailingOnes = 0;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 3) == 14) //(0000 0000 0111 0)b ==> (0 0000 0000 1110)b
		{
			coeff_token_length = 13;
			TrailingOnes = 1;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 5) == 5) //(0000 0000 101)b ==> (000 0000 0101)b
		{
			coeff_token_length = 11;
			TrailingOnes = 2;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 7) == 4) //(0000 0010 0)b ==> (0 0000 0100)b
		{
			coeff_token_length = 9;
			TrailingOnes = 3;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 3) == 8) //(0000 0000 0100 0)b ==> (0 0000 0000 1000)b
		{
			coeff_token_length = 13;
			TrailingOnes = 0;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 3) == 10) //(0000 0000 0101 0)b ==> (0 0000 0000 1010)b
		{
			coeff_token_length = 13;
			TrailingOnes = 1;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 3) == 13) //(0000 0000 0110 1)b ==> (0 0000 0000 1101)b
		{
			coeff_token_length = 13;
			TrailingOnes = 2;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 6) == 4) //(0000 0001 00)b ==> (00 0000 0100)b
		{
			coeff_token_length = 10;
			TrailingOnes = 3;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 2) == 15) //(0000 0000 0011 11)b ==> (00 0000 0000 1111)b
		{
			coeff_token_length = 14;
			TrailingOnes = 0;
			TotalCoeff = 9;
		}
		else if ((coeff_token >> 2) == 14) //(0000 0000 0011 10)b ==> (00 0000 0000 1110)b
		{
			coeff_token_length = 14;
			TrailingOnes = 1;
			TotalCoeff = 9;
		}
		else if ((coeff_token >> 3) == 9) //(0000 0000 0100 1)b ==> (0 0000 0000 1001)b
		{
			coeff_token_length = 13;
			TrailingOnes = 2;
			TotalCoeff = 9;
		}
		else if ((coeff_token >> 5) == 4) //(0000 0000 100)b ==> (000 0000 0100)b
		{
			coeff_token_length = 11;
			TrailingOnes = 3;
			TotalCoeff = 9;
		}
		else if ((coeff_token >> 2) == 11) //(0000 0000 0010 11)b ==> (00 0000 0000 1011)b
		{
			coeff_token_length = 14;
			TrailingOnes = 0;
			TotalCoeff = 10;
		}
		else if ((coeff_token >> 2) == 10) //(0000 0000 0010 10)b ==> (00 0000 0000 1010)b
		{
			coeff_token_length = 14;
			TrailingOnes = 1;
			TotalCoeff = 10;
		}
		else if ((coeff_token >> 2) == 13) //(0000 0000 0011 01)b ==> (00 0000 0000 1101)b
		{
			coeff_token_length = 14;
			TrailingOnes = 2;
			TotalCoeff = 10;
		}
		else if ((coeff_token >> 3) == 12) //(0000 0000 0110 0)b ==> (0 0000 0000 1100)b
		{
			coeff_token_length = 13;
			TrailingOnes = 3;
			TotalCoeff = 10;
		}
		else if ((coeff_token >> 1) == 15) //(0000 0000 0001 111)b ==> (000 0000 0000 1111)b
		{
			coeff_token_length = 15;
			TrailingOnes = 0;
			TotalCoeff = 11;
		}
		else if ((coeff_token >> 1) == 14) //(0000 0000 0001 110)b ==> (000 0000 0000 1110)b
		{
			coeff_token_length = 15;
			TrailingOnes = 1;
			TotalCoeff = 11;
		}
		else if ((coeff_token >> 2) == 9) //(0000 0000 0010 01)b ==> (00 0000 0000 1001)b
		{
			coeff_token_length = 14;
			TrailingOnes = 2;
			TotalCoeff = 11;
		}
		else if ((coeff_token >> 2) == 12) //(0000 0000 0011 00)b ==> (00 0000 0000 1100)b
		{
			coeff_token_length = 14;
			TrailingOnes = 3;
			TotalCoeff = 11;
		}
		else if ((coeff_token >> 1) == 11) //(0000 0000 0001 011)b ==> (000 0000 0000 1011)b
		{
			coeff_token_length = 15;
			TrailingOnes = 0;
			TotalCoeff = 12;
		}
		else if ((coeff_token >> 1) == 10) //(0000 0000 0001 010)b ==> (000 0000 0000 1010)b
		{
			coeff_token_length = 15;
			TrailingOnes = 1;
			TotalCoeff = 12;
		}
		else if ((coeff_token >> 1) == 13) //(0000 0000 0001 101)b ==> (000 0000 0000 1101)b
		{
			coeff_token_length = 15;
			TrailingOnes = 2;
			TotalCoeff = 12;
		}
		else if ((coeff_token >> 2) == 8) //(0000 0000 0010 00)b ==> (00 0000 0000 1000)b
		{
			coeff_token_length = 14;
			TrailingOnes = 3;
			TotalCoeff = 12;
		}
		else if ((coeff_token >> 0) == 15) //(0000 0000 0000 1111)b ==> (0000 0000 0000 1111)b
		{
			coeff_token_length = 16;
			TrailingOnes = 0;
			TotalCoeff = 13;
		}
		else if ((coeff_token >> 1) == 1) //(0000 0000 0000 001)b ==> (000 0000 0000 0001)b
		{
			coeff_token_length = 15;
			TrailingOnes = 1;
			TotalCoeff = 13;
		}
		else if ((coeff_token >> 1) == 9) //(0000 0000 0001 001)b ==> (000 0000 0000 1001)b
		{
			coeff_token_length = 15;
			TrailingOnes = 2;
			TotalCoeff = 13;
		}
		else if ((coeff_token >> 1) == 12) //(0000 0000 0001 100)b ==> (000 0000 0000 1100)b
		{
			coeff_token_length = 15;
			TrailingOnes = 3;
			TotalCoeff = 13;
		}
		else if ((coeff_token >> 0) == 11) //(0000 0000 0000 1011)b ==> (0000 0000 0000 1011)b
		{
			coeff_token_length = 16;
			TrailingOnes = 0;
			TotalCoeff = 14;
		}
		else if ((coeff_token >> 0) == 14) //(0000 0000 0000 1110)b ==> (0000 0000 0000 1110)b
		{
			coeff_token_length = 16;
			TrailingOnes = 1;
			TotalCoeff = 14;
		}
		else if ((coeff_token >> 0) == 13) //(0000 0000 0000 1101)b ==> (0000 0000 0000 1101)b
		{
			coeff_token_length = 16;
			TrailingOnes = 2;
			TotalCoeff = 14;
		}
		else if ((coeff_token >> 1) == 8) //(0000 0000 0001 000)b ==> (000 0000 0000 1000)b
		{
			coeff_token_length = 15;
			TrailingOnes = 3;
			TotalCoeff = 14;
		}
		else if ((coeff_token >> 0) == 7) //(0000 0000 0000 0111)b ==> (0000 0000 0000 0111)b
		{
			coeff_token_length = 16;
			TrailingOnes = 0;
			TotalCoeff = 15;
		}
		else if ((coeff_token >> 0) == 10) //(0000 0000 0000 1010)b ==> (0000 0000 0000 1010)b
		{
			coeff_token_length = 16;
			TrailingOnes = 1;
			TotalCoeff = 15;
		}
		else if ((coeff_token >> 0) == 9) //(0000 0000 0000 1001)b ==> (0000 0000 0000 1001)b
		{
			coeff_token_length = 16;
			TrailingOnes = 2;
			TotalCoeff = 15;
		}
		else if ((coeff_token >> 0) == 12) //(0000 0000 0000 1100)b ==> (0000 0000 0000 1100)b
		{
			coeff_token_length = 16;
			TrailingOnes = 3;
			TotalCoeff = 15;
		}
		else if ((coeff_token >> 0) == 4) //(0000 0000 0000 0100)b ==> (0000 0000 0000 0100)b
		{
			coeff_token_length = 16;
			TrailingOnes = 0;
			TotalCoeff = 16;
		}
		else if ((coeff_token >> 0) == 6) //(0000 0000 0000 0110)b ==> (0000 0000 0000 0110)b
		{
			coeff_token_length = 16;
			TrailingOnes = 1;
			TotalCoeff = 16;
		}
		else if ((coeff_token >> 0) == 5) //(0000 0000 0000 0101)b ==> (0000 0000 0000 0101)b
		{
			coeff_token_length = 16;
			TrailingOnes = 2;
			TotalCoeff = 16;
		}
		else if ((coeff_token >> 0) == 8) //(0000 0000 0000 1000)b ==> (0000 0000 0000 1000)b
		{
			coeff_token_length = 16;
			TrailingOnes = 3;
			TotalCoeff = 16;
		}
	}
	else if (2 <= nC && nC < 4)
	{
		if ((coeff_token >> 14) == 0x0003) // (11)b
		{
			coeff_token_length = 2;
			TrailingOnes = 0;
			TotalCoeff = 0;
		}
		else if ((coeff_token >> 10) == 0x000B) // (0010 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 1;
		}
		else if ((coeff_token >> 14) == 0x0002) // (10)b
		{
			coeff_token_length = 2;
			TrailingOnes = 1;
			TotalCoeff = 1;
		}
		else if ((coeff_token >> 10) == 0x0007) // (0001 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 2;
		}
		else if ((coeff_token >> 11) == 0x0007) // (0011 1)b
		{
			coeff_token_length = 5;
			TrailingOnes = 1;
			TotalCoeff = 2;
		}
		else if ((coeff_token >> 13) == 0x0003) // (011)b
		{
			coeff_token_length = 3;
			TrailingOnes = 2;
			TotalCoeff = 2;
		}
		else if ((coeff_token >> 9) == 0x0007) // (0000 111)b
		{
			coeff_token_length = 7;
			TrailingOnes = 0;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 10) == 0x000A) // (0010 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 10) == 0x0009) // (0010 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 12) == 0x0005) // (0101)b
		{
			coeff_token_length = 4;
			TrailingOnes = 3;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 8) == 0x0007) // (0000 0111)b
		{
			coeff_token_length = 8;
			TrailingOnes = 0;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 10) == 0x0006) // (0001 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 10) == 0x0005) // (0001 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 12) == 0x0004) // (0100)b
		{
			coeff_token_length = 4;
			TrailingOnes = 3;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 8) == 0x0004) // (0000 0100)b
		{
			coeff_token_length = 8;
			TrailingOnes = 0;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 9) == 0x0006) // (0000 110)b
		{
			coeff_token_length = 7;
			TrailingOnes = 1;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 9) == 0x0005) // (0000 101)b
		{
			coeff_token_length = 7;
			TrailingOnes = 2;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 11) == 0x0006) // (0011 0)b
		{
			coeff_token_length = 5;
			TrailingOnes = 3;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 7) == 0x0007) // (0000 0011 1)b
		{
			coeff_token_length = 9;
			TrailingOnes = 0;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 8) == 0x0006) // (0000 0110)b
		{
			coeff_token_length = 8;
			TrailingOnes = 1;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 8) == 0x0005) // (0000 0101)b
		{
			coeff_token_length = 8;
			TrailingOnes = 2;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 10) == 0x0008) // (0010 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 5) == 0x000F) // (0000 0001 111)b
		{
			coeff_token_length = 11;
			TrailingOnes = 0;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 7) == 0x0006) // (0000 0011 0)b
		{
			coeff_token_length = 9;
			TrailingOnes = 1;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 7) == 0x0005) // (0000 0010 1)b
		{
			coeff_token_length = 9;
			TrailingOnes = 2;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 10) == 0x0004) // (0001 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 5) == 0x000B) // (0000 0001 011)b
		{
			coeff_token_length = 11;
			TrailingOnes = 0;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 5) == 0x000E) // (0000 0001 110)b
		{
			coeff_token_length = 11;
			TrailingOnes = 1;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 5) == 0x000D) // (0000 0001 101)b
		{
			coeff_token_length = 11;
			TrailingOnes = 2;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 9) == 0x0004) // (0000 100)b
		{
			coeff_token_length = 7;
			TrailingOnes = 3;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 4) == 0x000F) // (0000 0000 1111)b
		{
			coeff_token_length = 12;
			TrailingOnes = 0;
			TotalCoeff = 9;
		}
		else if ((coeff_token >> 5) == 0x000A) // (0000 0001 010)b
		{
			coeff_token_length = 11;
			TrailingOnes = 1;
			TotalCoeff = 9;
		}
		else if ((coeff_token >> 5) == 0x0009) // (0000 0001 001)b
		{
			coeff_token_length = 11;
			TrailingOnes = 2;
			TotalCoeff = 9;
		}
		else if ((coeff_token >> 7) == 0x0004) // (0000 0010 0)b
		{
			coeff_token_length = 9;
			TrailingOnes = 3;
			TotalCoeff = 9;
		}
		else if ((coeff_token >> 4) == 0x000B) // (0000 0000 1011)b
		{
			coeff_token_length = 12;
			TrailingOnes = 0;
			TotalCoeff = 10;
		}
		else if ((coeff_token >> 4) == 0x000E) // (0000 0000 1110)b
		{
			coeff_token_length = 12;
			TrailingOnes = 1;
			TotalCoeff = 10;
		}
		else if ((coeff_token >> 4) == 0x000D) // (0000 0000 1101)b
		{
			coeff_token_length = 12;
			TrailingOnes = 2;
			TotalCoeff = 10;
		}
		else if ((coeff_token >> 5) == 0x000C) // (0000 0001 100)b
		{
			coeff_token_length = 11;
			TrailingOnes = 3;
			TotalCoeff = 10;
		}
		else if ((coeff_token >> 4) == 0x0008) // (0000 0000 1000)b
		{
			coeff_token_length = 12;
			TrailingOnes = 0;
			TotalCoeff = 11;
		}
		else if ((coeff_token >> 4) == 0x000A) // (0000 0000 1010)b
		{
			coeff_token_length = 12;
			TrailingOnes = 1;
			TotalCoeff = 11;
		}
		else if ((coeff_token >> 4) == 0x0009) // (0000 0000 1001)b
		{
			coeff_token_length = 12;
			TrailingOnes = 2;
			TotalCoeff = 11;
		}
		else if ((coeff_token >> 5) == 0x0008) // (0000 0001 000)b
		{
			coeff_token_length = 11;
			TrailingOnes = 3;
			TotalCoeff = 11;
		}
		else if ((coeff_token >> 3) == 0x000F) // (0000 0000 0111 1)b
		{
			coeff_token_length = 13;
			TrailingOnes = 0;
			TotalCoeff = 12;
		}
		else if ((coeff_token >> 3) == 0x000E) // (0000 0000 0111 0)b
		{
			coeff_token_length = 13;
			TrailingOnes = 1;
			TotalCoeff = 12;
		}
		else if ((coeff_token >> 3) == 0x000D) // (0000 0000 0110 1)b
		{
			coeff_token_length = 13;
			TrailingOnes = 2;
			TotalCoeff = 12;
		}
		else if ((coeff_token >> 4) == 0x000C) // (0000 0000 1100)b
		{
			coeff_token_length = 12;
			TrailingOnes = 3;
			TotalCoeff = 12;
		}
		else if ((coeff_token >> 3) == 0x000B) // (0000 0000 0101 1)b
		{
			coeff_token_length = 13;
			TrailingOnes = 0;
			TotalCoeff = 13;
		}
		else if ((coeff_token >> 3) == 0x000A) // (0000 0000 0101 0)b
		{
			coeff_token_length = 13;
			TrailingOnes = 1;
			TotalCoeff = 13;
		}
		else if ((coeff_token >> 3) == 0x0009) // (0000 0000 0100 1)b
		{
			coeff_token_length = 13;
			TrailingOnes = 2;
			TotalCoeff = 13;
		}
		else if ((coeff_token >> 3) == 0x000C) // (0000 0000 0110 0)b
		{
			coeff_token_length = 13;
			TrailingOnes = 3;
			TotalCoeff = 13;
		}
		else if ((coeff_token >> 3) == 0x0007) // (0000 0000 0011 1)b
		{
			coeff_token_length = 13;
			TrailingOnes = 0;
			TotalCoeff = 14;
		}
		else if ((coeff_token >> 2) == 0x000B) // (0000 0000 0010 11)b
		{
			coeff_token_length = 14;
			TrailingOnes = 1;
			TotalCoeff = 14;
		}
		else if ((coeff_token >> 3) == 0x0006) // (0000 0000 0011 0)b
		{
			coeff_token_length = 13;
			TrailingOnes = 2;
			TotalCoeff = 14;
		}
		else if ((coeff_token >> 3) == 0x0008) // (0000 0000 0100 0)b
		{
			coeff_token_length = 13;
			TrailingOnes = 3;
			TotalCoeff = 14;
		}
		else if ((coeff_token >> 2) == 0x0009) // (0000 0000 0010 01)b
		{
			coeff_token_length = 14;
			TrailingOnes = 0;
			TotalCoeff = 15;
		}
		else if ((coeff_token >> 2) == 0x0008) // (0000 0000 0010 00)b
		{
			coeff_token_length = 14;
			TrailingOnes = 1;
			TotalCoeff = 15;
		}
		else if ((coeff_token >> 2) == 0x000A) // (0000 0000 0010 10)b
		{
			coeff_token_length = 14;
			TrailingOnes = 2;
			TotalCoeff = 15;
		}
		else if ((coeff_token >> 3) == 0x0001) // (0000 0000 0000 1)b
		{
			coeff_token_length = 13;
			TrailingOnes = 3;
			TotalCoeff = 15;
		}
		else if ((coeff_token >> 2) == 0x0007) // (0000 0000 0001 11)b
		{
			coeff_token_length = 14;
			TrailingOnes = 0;
			TotalCoeff = 16;
		}
		else if ((coeff_token >> 2) == 0x0006) // (0000 0000 0001 10)b
		{
			coeff_token_length = 14;
			TrailingOnes = 1;
			TotalCoeff = 16;
		}
		else if ((coeff_token >> 2) == 0x0005) // (0000 0000 0001 01)b
		{
			coeff_token_length = 14;
			TrailingOnes = 2;
			TotalCoeff = 16;
		}
		else if ((coeff_token >> 2) == 0x0004) // (0000 0000 0001 00)b
		{
			coeff_token_length = 14;
			TrailingOnes = 3;
			TotalCoeff = 16;
		}
	}
	else if (4 <= nC && nC < 8)
	{
		if ((coeff_token >> 12) == 0x000F) // (1111)b
		{
			coeff_token_length = 4;
			TrailingOnes = 0;
			TotalCoeff = 0;
		}
		else if ((coeff_token >> 10) == 0x000F) // (0011 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 1;
		}
		else if ((coeff_token >> 12) == 0x000E) // (1110)b
		{
			coeff_token_length = 4;
			TrailingOnes = 1;
			TotalCoeff = 1;
		}
		else if ((coeff_token >> 10) == 0x000B) // (0010 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 2;
		}
		else if ((coeff_token >> 11) == 0x000F) // (0111 1)b
		{
			coeff_token_length = 5;
			TrailingOnes = 1;
			TotalCoeff = 2;
		}
		else if ((coeff_token >> 12) == 0x000D) // (1101)b
		{
			coeff_token_length = 4;
			TrailingOnes = 2;
			TotalCoeff = 2;
		}
		else if ((coeff_token >> 10) == 0x0008) // (0010 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 11) == 0x000C) // (0110 0)b
		{
			coeff_token_length = 5;
			TrailingOnes = 1;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 11) == 0x000E) // (0111 0)b
		{
			coeff_token_length = 5;
			TrailingOnes = 2;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 12) == 0x000C) // (1100)b
		{
			coeff_token_length = 4;
			TrailingOnes = 3;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 9) == 0x000F) // (0001 111)b
		{
			coeff_token_length = 7;
			TrailingOnes = 0;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 11) == 0x000A) // (0101 0)b
		{
			coeff_token_length = 5;
			TrailingOnes = 1;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 11) == 0x000B) // (0101 1)b
		{
			coeff_token_length = 5;
			TrailingOnes = 2;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 12) == 0x000B) // (1011)b
		{
			coeff_token_length = 4;
			TrailingOnes = 3;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 9) == 0x000B) // (0001 011)b
		{
			coeff_token_length = 7;
			TrailingOnes = 0;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 11) == 0x0008) // (0100 0)b
		{
			coeff_token_length = 5;
			TrailingOnes = 1;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 11) == 0x0009) // (0100 1)b
		{
			coeff_token_length = 5;
			TrailingOnes = 2;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 12) == 0x000A) // (1010)b
		{
			coeff_token_length = 4;
			TrailingOnes = 3;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 9) == 0x0009) // (0001 001)b
		{
			coeff_token_length = 7;
			TrailingOnes = 0;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 10) == 0x000E) // (0011 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 10) == 0x000D) // (0011 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 12) == 0x0009) // (1001)b
		{
			coeff_token_length = 4;
			TrailingOnes = 3;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 9) == 0x0008) // (0001 000)b
		{
			coeff_token_length = 7;
			TrailingOnes = 0;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 10) == 0x000A) // (0010 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 10) == 0x0009) // (0010 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 12) == 0x0008) // (1000)b
		{
			coeff_token_length = 4;
			TrailingOnes = 3;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 8) == 0x000F) // (0000 1111)b
		{
			coeff_token_length = 8;
			TrailingOnes = 0;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 9) == 0x000E) // (0001 110)b
		{
			coeff_token_length = 7;
			TrailingOnes = 1;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 9) == 0x000D) // (0001 101)b
		{
			coeff_token_length = 7;
			TrailingOnes = 2;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 11) == 0x000D) // (0110 1)b
		{
			coeff_token_length = 5;
			TrailingOnes = 3;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 8) == 0x000B) // (0000 1011)b
		{
			coeff_token_length = 8;
			TrailingOnes = 0;
			TotalCoeff = 9;
		}
		else if ((coeff_token >> 8) == 0x000E) // (0000 1110)b
		{
			coeff_token_length = 8;
			TrailingOnes = 1;
			TotalCoeff = 9;
		}
		else if ((coeff_token >> 9) == 0x000A) // (0001 010)b
		{
			coeff_token_length = 7;
			TrailingOnes = 2;
			TotalCoeff = 9;
		}
		else if ((coeff_token >> 10) == 0x000C) // (0011 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 9;
		}
		else if ((coeff_token >> 7) == 0x000F) // (0000 0111 1)b
		{
			coeff_token_length = 9;
			TrailingOnes = 0;
			TotalCoeff = 10;
		}
		else if ((coeff_token >> 8) == 0x000A) // (0000 1010)b
		{
			coeff_token_length = 8;
			TrailingOnes = 1;
			TotalCoeff = 10;
		}
		else if ((coeff_token >> 8) == 0x000D) // (0000 1101)b
		{
			coeff_token_length = 8;
			TrailingOnes = 2;
			TotalCoeff = 10;
		}
		else if ((coeff_token >> 9) == 0x000C) // (0001 100)b
		{
			coeff_token_length = 7;
			TrailingOnes = 3;
			TotalCoeff = 10;
		}
		else if ((coeff_token >> 7) == 0x000B) // (0000 0101 1)b
		{
			coeff_token_length = 9;
			TrailingOnes = 0;
			TotalCoeff = 11;
		}
		else if ((coeff_token >> 7) == 0x000E) // (0000 0111 0)b
		{
			coeff_token_length = 9;
			TrailingOnes = 1;
			TotalCoeff = 11;
		}
		else if ((coeff_token >> 8) == 0x0009) // (0000 1001)b
		{
			coeff_token_length = 8;
			TrailingOnes = 2;
			TotalCoeff = 11;
		}
		else if ((coeff_token >> 8) == 0x000C) // (0000 1100)b
		{
			coeff_token_length = 8;
			TrailingOnes = 3;
			TotalCoeff = 11;
		}
		else if ((coeff_token >> 7) == 0x0008) // (0000 0100 0)b
		{
			coeff_token_length = 9;
			TrailingOnes = 0;
			TotalCoeff = 12;
		}
		else if ((coeff_token >> 7) == 0x000A) // (0000 0101 0)b
		{
			coeff_token_length = 9;
			TrailingOnes = 1;
			TotalCoeff = 12;
		}
		else if ((coeff_token >> 7) == 0x000D) // (0000 0110 1)b
		{
			coeff_token_length = 9;
			TrailingOnes = 2;
			TotalCoeff = 12;
		}
		else if ((coeff_token >> 8) == 0x0008) // (0000 1000)b
		{
			coeff_token_length = 8;
			TrailingOnes = 3;
			TotalCoeff = 12;
		}
		else if ((coeff_token >> 6) == 0x000D) // (0000 0011 01)b
		{
			coeff_token_length = 10;
			TrailingOnes = 0;
			TotalCoeff = 13;
		}
		else if ((coeff_token >> 7) == 0x0007) // (0000 0011 1)b
		{
			coeff_token_length = 9;
			TrailingOnes = 1;
			TotalCoeff = 13;
		}
		else if ((coeff_token >> 7) == 0x0009) // (0000 0100 1)b
		{
			coeff_token_length = 9;
			TrailingOnes = 2;
			TotalCoeff = 13;
		}
		else if ((coeff_token >> 7) == 0x000C) // (0000 0110 0)b
		{
			coeff_token_length = 9;
			TrailingOnes = 3;
			TotalCoeff = 13;
		}
		else if ((coeff_token >> 6) == 0x0009) // (0000 0010 01)b
		{
			coeff_token_length = 10;
			TrailingOnes = 0;
			TotalCoeff = 14;
		}
		else if ((coeff_token >> 6) == 0x000C) // (0000 0011 00)b
		{
			coeff_token_length = 10;
			TrailingOnes = 1;
			TotalCoeff = 14;
		}
		else if ((coeff_token >> 6) == 0x000B) // (0000 0010 11)b
		{
			coeff_token_length = 10;
			TrailingOnes = 2;
			TotalCoeff = 14;
		}
		else if ((coeff_token >> 6) == 0x000A) // (0000 0010 10)b
		{
			coeff_token_length = 10;
			TrailingOnes = 3;
			TotalCoeff = 14;
		}
		else if ((coeff_token >> 6) == 0x0005) // (0000 0001 01)b
		{
			coeff_token_length = 10;
			TrailingOnes = 0;
			TotalCoeff = 15;
		}
		else if ((coeff_token >> 6) == 0x0008) // (0000 0010 00)b
		{
			coeff_token_length = 10;
			TrailingOnes = 1;
			TotalCoeff = 15;
		}
		else if ((coeff_token >> 6) == 0x0007) // (0000 0001 11)b
		{
			coeff_token_length = 10;
			TrailingOnes = 2;
			TotalCoeff = 15;
		}
		else if ((coeff_token >> 6) == 0x0006) // (0000 0001 10)b
		{
			coeff_token_length = 10;
			TrailingOnes = 3;
			TotalCoeff = 15;
		}
		else if ((coeff_token >> 6) == 0x0001) // (0000 0000 01)b
		{
			coeff_token_length = 10;
			TrailingOnes = 0;
			TotalCoeff = 16;
		}
		else if ((coeff_token >> 6) == 0x0004) // (0000 0001 00)b
		{
			coeff_token_length = 10;
			TrailingOnes = 1;
			TotalCoeff = 16;
		}
		else if ((coeff_token >> 6) == 0x0003) // (0000 0000 11)b
		{
			coeff_token_length = 10;
			TrailingOnes = 2;
			TotalCoeff = 16;
		}
		else if ((coeff_token >> 6) == 0x0002) // (0000 0000 10)b
		{
			coeff_token_length = 10;
			TrailingOnes = 3;
			TotalCoeff = 16;
		}
	}
	else if (8 <= nC)
	{
		if ((coeff_token >> 10) == 0x0003) // (0000 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 0;
		}
		else if ((coeff_token >> 10) == 0x0000) // (0000 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 1;
		}
		else if ((coeff_token >> 10) == 0x0001) // (0000 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 1;
		}
		else if ((coeff_token >> 10) == 0x0004) // (0001 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 2;
		}
		else if ((coeff_token >> 10) == 0x0005) // (0001 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 2;
		}
		else if ((coeff_token >> 10) == 0x0006) // (0001 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 2;
		}
		else if ((coeff_token >> 10) == 0x0008) // (0010 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 10) == 0x0009) // (0010 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 10) == 0x000A) // (0010 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 10) == 0x000B) // (0010 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 10) == 0x000C) // (0011 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 10) == 0x000D) // (0011 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 10) == 0x000E) // (0011 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 10) == 0x000F) // (0011 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 10) == 0x0010) // (0100 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 10) == 0x0011) // (0100 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 10) == 0x0012) // (0100 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 10) == 0x0013) // (0100 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 10) == 0x0014) // (0101 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 10) == 0x0015) // (0101 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 10) == 0x0016) // (0101 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 10) == 0x0017) // (0101 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 10) == 0x0018) // (0110 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 10) == 0x0019) // (0110 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 10) == 0x001A) // (0110 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 10) == 0x001B) // (0110 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 10) == 0x001C) // (0111 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 10) == 0x001D) // (0111 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 10) == 0x001E) // (0111 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 10) == 0x001F) // (0111 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 10) == 0x0020) // (1000 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 9;
		}
		else if ((coeff_token >> 10) == 0x0021) // (1000 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 9;
		}
		else if ((coeff_token >> 10) == 0x0022) // (1000 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 9;
		}
		else if ((coeff_token >> 10) == 0x0023) // (1000 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 9;
		}
		else if ((coeff_token >> 10) == 0x0024) // (1001 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 10;
		}
		else if ((coeff_token >> 10) == 0x0025) // (1001 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 10;
		}
		else if ((coeff_token >> 10) == 0x0026) // (1001 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 10;
		}
		else if ((coeff_token >> 10) == 0x0027) // (1001 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 10;
		}
		else if ((coeff_token >> 10) == 0x0028) // (1010 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 11;
		}
		else if ((coeff_token >> 10) == 0x0029) // (1010 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 11;
		}
		else if ((coeff_token >> 10) == 0x002A) // (1010 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 11;
		}
		else if ((coeff_token >> 10) == 0x002B) // (1010 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 11;
		}
		else if ((coeff_token >> 10) == 0x002C) // (1011 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 12;
		}
		else if ((coeff_token >> 10) == 0x002D) // (1011 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 12;
		}
		else if ((coeff_token >> 10) == 0x002E) // (1011 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 12;
		}
		else if ((coeff_token >> 10) == 0x002F) // (1011 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 12;
		}
		else if ((coeff_token >> 10) == 0x0030) // (1100 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 13;
		}
		else if ((coeff_token >> 10) == 0x0031) // (1100 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 13;
		}
		else if ((coeff_token >> 10) == 0x0032) // (1100 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 13;
		}
		else if ((coeff_token >> 10) == 0x0033) // (1100 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 13;
		}
		else if ((coeff_token >> 10) == 0x0034) // (1101 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 14;
		}
		else if ((coeff_token >> 10) == 0x0035) // (1101 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 14;
		}
		else if ((coeff_token >> 10) == 0x0036) // (1101 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 14;
		}
		else if ((coeff_token >> 10) == 0x0037) // (1101 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 14;
		}
		else if ((coeff_token >> 10) == 0x0038) // (1110 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 15;
		}
		else if ((coeff_token >> 10) == 0x0039) // (1110 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 15;
		}
		else if ((coeff_token >> 10) == 0x003A) // (1110 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 15;
		}
		else if ((coeff_token >> 10) == 0x003B) // (1110 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 15;
		}
		else if ((coeff_token >> 10) == 0x003C) // (1111 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 16;
		}
		else if ((coeff_token >> 10) == 0x003D) // (1111 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 16;
		}
		else if ((coeff_token >> 10) == 0x003E) // (1111 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 16;
		}
		else if ((coeff_token >> 10) == 0x003F) // (1111 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 16;
		}
	}
	else if (-1 == nC)
	{
		if ((coeff_token >> 14) == 0x0001) // (01)b
		{
			coeff_token_length = 2;
			TrailingOnes = 0;
			TotalCoeff = 0;
		}
		else if ((coeff_token >> 10) == 0x0007) // (0001 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 1;
		}
		else if ((coeff_token >> 15) == 0x0001) // (1)b
		{
			coeff_token_length = 1;
			TrailingOnes = 1;
			TotalCoeff = 1;
		}
		else if ((coeff_token >> 10) == 0x0004) // (0001 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 2;
		}
		else if ((coeff_token >> 10) == 0x0006) // (0001 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 2;
		}
		else if ((coeff_token >> 13) == 0x0001) // (001)b
		{
			coeff_token_length = 3;
			TrailingOnes = 2;
			TotalCoeff = 2;
		}
		else if ((coeff_token >> 10) == 0x0003) // (0000 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 9) == 0x0003) // (0000 011)b
		{
			coeff_token_length = 7;
			TrailingOnes = 1;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 9) == 0x0002) // (0000 010)b
		{
			coeff_token_length = 7;
			TrailingOnes = 2;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 10) == 0x0005) // (0001 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 10) == 0x0002) // (0000 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 8) == 0x0003) // (0000 0011)b
		{
			coeff_token_length = 8;
			TrailingOnes = 1;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 8) == 0x0002) // (0000 0010)b
		{
			coeff_token_length = 8;
			TrailingOnes = 2;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 9) == 0x0000) // (0000 000)b
		{
			coeff_token_length = 7;
			TrailingOnes = 3;
			TotalCoeff = 4;
		}
	}
	else if (-2 == nC)
	{
		if ((coeff_token >> 15) == 0x0001) // (1)b
		{
			coeff_token_length = 1;
			TrailingOnes = 0;
			TotalCoeff = 0;
		}
		else if ((coeff_token >> 9) == 0x000F) // (0001 111)b
		{
			coeff_token_length = 7;
			TrailingOnes = 0;
			TotalCoeff = 1;
		}
		else if ((coeff_token >> 14) == 0x0001) // (01)b
		{
			coeff_token_length = 2;
			TrailingOnes = 1;
			TotalCoeff = 1;
		}
		else if ((coeff_token >> 9) == 0x000E) // (0001 110)b
		{
			coeff_token_length = 7;
			TrailingOnes = 0;
			TotalCoeff = 2;
		}
		else if ((coeff_token >> 9) == 0x000D) // (0001 101)b
		{
			coeff_token_length = 7;
			TrailingOnes = 1;
			TotalCoeff = 2;
		}
		else if ((coeff_token >> 13) == 0x0001) // (001)b
		{
			coeff_token_length = 3;
			TrailingOnes = 2;
			TotalCoeff = 2;
		}
		else if ((coeff_token >> 7) == 0x0007) // (0000 0011 1)b
		{
			coeff_token_length = 9;
			TrailingOnes = 0;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 9) == 0x000C) // (0001 100)b
		{
			coeff_token_length = 7;
			TrailingOnes = 1;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 9) == 0x000B) // (0001 011)b
		{
			coeff_token_length = 7;
			TrailingOnes = 2;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 11) == 0x0001) // (0000 1)b
		{
			coeff_token_length = 5;
			TrailingOnes = 3;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 7) == 0x0006) // (0000 0011 0)b
		{
			coeff_token_length = 9;
			TrailingOnes = 0;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 7) == 0x0005) // (0000 0010 1)b
		{
			coeff_token_length = 9;
			TrailingOnes = 1;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 9) == 0x000A) // (0001 010)b
		{
			coeff_token_length = 7;
			TrailingOnes = 2;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 10) == 0x0001) // (0000 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 6) == 0x0007) // (0000 0001 11)b
		{
			coeff_token_length = 10;
			TrailingOnes = 0;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 6) == 0x0006) // (0000 0001 10)b
		{
			coeff_token_length = 10;
			TrailingOnes = 1;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 7) == 0x0004) // (0000 0010 0)b
		{
			coeff_token_length = 9;
			TrailingOnes = 2;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 9) == 0x0009) // (0001 001)b
		{
			coeff_token_length = 7;
			TrailingOnes = 3;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 5) == 0x0007) // (0000 0000 111)b
		{
			coeff_token_length = 11;
			TrailingOnes = 0;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 5) == 0x0006) // (0000 0000 110)b
		{
			coeff_token_length = 11;
			TrailingOnes = 1;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 6) == 0x0005) // (0000 0001 01)b
		{
			coeff_token_length = 10;
			TrailingOnes = 2;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 9) == 0x0008) // (0001 000)b
		{
			coeff_token_length = 7;
			TrailingOnes = 3;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 4) == 0x0007) // (0000 0000 0111)b
		{
			coeff_token_length = 12;
			TrailingOnes = 0;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 4) == 0x0006) // (0000 0000 0110)b
		{
			coeff_token_length = 12;
			TrailingOnes = 1;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 5) == 0x0005) // (0000 0000 101)b
		{
			coeff_token_length = 11;
			TrailingOnes = 2;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 6) == 0x0004) // (0000 0001 00)b
		{
			coeff_token_length = 10;
			TrailingOnes = 3;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 3) == 0x0007) // (0000 0000 0011 1)b
		{
			coeff_token_length = 13;
			TrailingOnes = 0;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 4) == 0x0005) // (0000 0000 0101)b
		{
			coeff_token_length = 12;
			TrailingOnes = 1;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 4) == 0x0004) // (0000 0000 0100)b
		{
			coeff_token_length = 12;
			TrailingOnes = 2;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 5) == 0x0004) // (0000 0000 100)b
		{
			coeff_token_length = 11;
			TrailingOnes = 3;
			TotalCoeff = 8;
		}
	}
	return 0;
}
