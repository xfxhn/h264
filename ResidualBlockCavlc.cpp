#include "ResidualBlockCavlc.h"
#include "ParseSlice.h"



ResidualBlockCavlc::ResidualBlockCavlc(ParseSlice& slice) :sliceBase(slice)
{

}

bool ResidualBlockCavlc::residual_block_cavlc(
	BitStream& bs, int32_t coeffLevel[16], int32_t startIdx, int32_t endIdx, uint32_t maxNumCoeff, size_t i4x4, size_t i8x8
)
{

	memset(coeffLevel, 0, sizeof(int32_t) * maxNumCoeff);


	int nC = getNumberCurrent(i4x4, i8x8);
	return false;
}

int ResidualBlockCavlc::getNumberCurrent(size_t i4x4, size_t i8x8)
{

	int nC = 0;
	bool availableTop = false;
	bool availableLeft = false;


	int nA = 0; //×ó²à¿é
	int nB = 0;	//ÉÏ²â¿é


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
			//console.log(arr[BlkIdx - 6])
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
