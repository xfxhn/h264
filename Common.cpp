
#include "Common.h"





//rbsp_trailing_bits( )用于SODB 之后
void rbsp_trailing_bits(BitStream& bs)
{
	if (bs.currentPtr >= bs.endPtr)
	{
		return;
	}

	int rbsp_stop_one_bit = bs.readBit(); // /* equal to 1 */ All f(1)
	while (!byte_aligned(bs))
	{
		int rbsp_alignment_zero_bit = bs.readBit(); // /* equal to 0 */ All f(1)
	}
}

string printHex(unsigned char* data, int len)
{

	for (size_t i = 0; i < len; i++)
	{
		cout << hex << (int)data[i] << "-";
	}
	cout << endl;

	return "";
}

int h264_log2(int32_t value)
{
	assert(value > 0);
	int log2 = 0;
	while (value)
	{
		value >>= 1;
		log2++;
	}
	return log2;
}

void scaling_list(BitStream& bs, int32_t* scalingList, int sizeOfScalingList, bool& useDefaultScalingMatrixFlag)
{

	int lastScale = 8;
	int nextScale = 8;

	for (size_t j = 0; j < sizeOfScalingList; j++)
	{
		if (nextScale != 0)
		{
			//delta_scale 是用于计算缩放比例列表中的第 j 个元素
			int delta_scale = bs.readSE();

			nextScale = (lastScale + delta_scale + 256) % 256;
			useDefaultScalingMatrixFlag = (j == 0 && nextScale == 0);

		}
		scalingList[j] = (nextScale == 0) ? lastScale : nextScale;

		lastScale = scalingList[j];

	}
}

bool byte_aligned(BitStream& bs)
{
	if (bs.bitsLeft % 8 == 0)
	{
		return true;
	}
	return false;
}




