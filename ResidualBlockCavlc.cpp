#include "ResidualBlockCavlc.h"

ResidualBlockCavlc::ResidualBlockCavlc()
{

}

bool ResidualBlockCavlc::residual_block_cavlc(
	BitStream& bs, const SliceHeader& sHeader, int32_t coeffLevel[16], int32_t startIdx, int32_t endIdx, uint32_t maxNumCoeff, size_t BlkIdx
)
{

	memset(coeffLevel, 0, sizeof(int32_t) * maxNumCoeff);


	int nC = getNumberCurrent(endIdx);
	return false;
}

int ResidualBlockCavlc::getNumberCurrent(int32_t endIdx)
{

	int nC = 0;
	bool available_top = false;
	bool available_left = false;

	return nC;
}
